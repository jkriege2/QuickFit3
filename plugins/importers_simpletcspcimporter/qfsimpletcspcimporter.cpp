/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "qfsimpletcspcimporter.h"
#include <QObject>
#include <QDebug>
#include "binarydatatools.h"

#define MASK_GREEN_ON    uint64_t(0x8000000000000000ULL)
#define MASK_GREEN_OFF   uint64_t(0x4000000000000000ULL)
#define MASK_RED_ON      uint64_t(0x2000000000000000ULL)
#define MASK_RED_OFF     uint64_t(0x1000000000000000ULL)
#define MASK_CH0         uint64_t(0x0800000000000000ULL)
#define MASK_CH1         uint64_t(0x0200000000000000ULL)
#define MASK_CH2         uint64_t(0x0400000000000000ULL)

#define MASK_GETTIME     uint64_t(0x01FFFFFFFFFFFFFFULL)

QFSimpleTCSPCImporter::QFSimpleTCSPCImporter() {
    tttrfile=NULL;
    currentTTTRRecordNum=0;
    temporalResolutionSeconds=6e-9;
    lastTTTRMacroTime=0;
    averageInterphotonTime=0;
}

QFSimpleTCSPCImporter::~QFSimpleTCSPCImporter() {
    if (tttrfile) fclose(tttrfile);
}

QString QFSimpleTCSPCImporter::filter() const {
    return QObject::tr("Simple TCSPC file [Hungarian 64-bit] (*.dat)");
}

QString QFSimpleTCSPCImporter::formatName() const {
    return QObject::tr("Simple TCSPC file [Hungarian 64-bit]");
}

bool QFSimpleTCSPCImporter::open(const QString &filename, const QString &parameters) {
    close();
    fileinfo.init(filename);
    tttrfile=fopen(filename.toAscii().data(), "rb");
    bool dok=false;
    double pd=parameters.toDouble(&dok);
    if (parameters.isEmpty() || !dok) {
        temporalResolutionSeconds=6e-9;
    } else {
        temporalResolutionSeconds=pd*1.0e-9;
    }
    if (tttrfile) {
        QString error="";

        // first we search for 4 64-bit words that are all 0xFFFFFFFFFFFFFFFF as  these
        // mark the end of the header. This is the same as 32 8-bit words!
        uint8_t din=0, dinlast=0;
        long FFcounter=0;
        while (!feof(tttrfile) && FFcounter<32) {
            dinlast=din;
            fread(&din, 1, 1, tttrfile);
            if (din!=0xFF) {
                if (FFcounter<32) FFcounter=0;
            } else {
                FFcounter++;
            }
        }
        if (FFcounter==32) {
            // we are now just before the first photon record in the file!
            // we read one record to get the initial arrival time:
            lastTTTRMacroTime=binfileReadUint64(tttrfile);
            //fread(&lastTTTRMacroTime, 1, sizeof(lastTTTRMacroTime), tttrfile);
            lastTTTRMacroTime=lastTTTRMacroTime&MASK_GETTIME;
            initialTTTRMacroTime=lastTTTRMacroTime;
            fgetpos(tttrfile, &fileResetPos);

            // we'll determine the number of records in the file now, by calculating
            // floor((filesize-fileResetPos)/8), as every photon record is 64-bit, i.e. 8-bytes long
            long int startbyte=ftell(tttrfile);
            fseek(tttrfile, 0, SEEK_END);
            long int endbyte=ftell(tttrfile);
            //qDebug()<<endbyte<<startbyte<<(endbyte-startbyte)/8;
            if ((endbyte-startbyte)/8>0) {
                recordsInTTTRFile=(endbyte-startbyte)/8;
                fsetpos(tttrfile, &fileResetPos);

                lastTTTRMacroTime=initialTTTRMacroTime;
                currentTTTRRecordNum=0;
                current.microtime_offset=0;
                current.microtime_deltaT=0;
                averageInterphotonTime=0;
                nextRecord();

                // read some photons to estimate a countrate
                cr.clear();
                double t=0;
                long cnt=0;
                do {
                    QFTCSPCRecord r=getCurrentRecord();
                    t=r.absoluteTime();
                    if (r.isPhoton) {
                        cr[r.input_channel]=cr.value(r.input_channel, 0)+1;
                        //qDebug()<<r.input_channel<<": "<<t;
                        cnt++;
                    }
                } while (nextRecord() && (t<1) && (cnt<10000));
                for (int i=0; i<inputChannels(); i++) {
                    cr[i]=cr.value(i, 0)/t/1000.0;
                }

                averageInterphotonTime=getCurrentRecord().absoluteTime()/double(cnt);


                fsetpos(tttrfile, &fileResetPos);
                currentTTTRRecordNum=0;
                lastTTTRMacroTime=initialTTTRMacroTime;
                current.microtime_offset=0;
                current.microtime_deltaT=0;

                return true;
            } else {
                setLastError(QObject::tr("no records found in TTTR file '%1'").arg(filename));
                close();
                return false;

            }
        } else {
            setLastError(QObject::tr("could not find header-end marker in TTTR file '%1'").arg(filename));
            close();
            return false;

        }
    }
    setLastError(QObject::tr("could not open TTTR file '%1'").arg(filename));
    return false;
}

bool QFSimpleTCSPCImporter::isOpenParametersUsed( QString *optionsDescription) const
{
    if (optionsDescription) *optionsDescription=QObject::tr("temporal resolution [ns]");
    return true;
}

void QFSimpleTCSPCImporter::close() {
    if (tttrfile) fclose(tttrfile);
    tttrfile=NULL;
    //fileResetPos=0;
    currentTTTRRecordNum=0;
    lastTTTRMacroTime=0;


}

void QFSimpleTCSPCImporter::reset() {
    fgetpos(tttrfile, &fileResetPos);
    currentTTTRRecordNum=0;
    lastTTTRMacroTime=initialTTTRMacroTime;
    nextRecord();

}

bool QFSimpleTCSPCImporter::nextRecord() {
    uint64_t TTTRrecord;
    bool ok=false;

    do {
        // READ ONE RECORD and check for errors
        /*int result = fread( &TTTRrecord, 1, sizeof(TTTRrecord) ,tttrfile);
        if (result!= sizeof(TTTRrecord)) {
            setLastError(QObject::tr("error reading input file."));
            return false;
        }*/
        TTTRrecord=binfileReadUint64(tttrfile);
        currentTTTRRecordNum++;

        // calculate the true time of the photon
        double truetime = double( (TTTRrecord & MASK_GETTIME) - initialTTTRMacroTime) * temporalResolutionSeconds ;

        lastTTTRMacroTime=TTTRrecord&MASK_GETTIME;
        current.isPhoton=false;
        current.marker_type=0;
        current.macrotime=truetime;
        current.input_channel=0;
        current.microtime_channel=0;

        if ((TTTRrecord & MASK_GREEN_ON)==MASK_GREEN_ON) {
            current.marker_type=1;
            ok=true;
        } else if ((TTTRrecord & MASK_GREEN_OFF)==MASK_GREEN_OFF) {
            current.marker_type=2;
            ok=true;
        } else if ((TTTRrecord & MASK_RED_ON)==MASK_RED_ON) {
            current.marker_type=3;
            ok=true;
        } else if ((TTTRrecord & MASK_RED_OFF)==MASK_RED_OFF) {
            current.marker_type=4;
            ok=true;
        } else if ((TTTRrecord & MASK_CH0)==MASK_CH0) {
            current.isPhoton=true;
            current.input_channel=0;
            current.marker_type=4;
            ok=true;
        } else if ((TTTRrecord & MASK_CH1)==MASK_CH1) {
            current.isPhoton=true;
            current.input_channel=1;
            current.marker_type=4;
            ok=true;
        } else if ((TTTRrecord & MASK_CH2)==MASK_CH2) {
            current.isPhoton=true;
            current.input_channel=2;
            current.marker_type=4;
            ok=true;
        } else {
            ok=false;
        }
        //if (ok) qDebug()<<"   "<<QString::number(TTTRrecord, 16);


    } while (!ok);

    return ((currentTTTRRecordNum<recordsInTTTRFile) && !feof(tttrfile));
}

double QFSimpleTCSPCImporter::measurementDuration() const {
    return double(recordsInTTTRFile)*averageInterphotonTime;
}

uint16_t QFSimpleTCSPCImporter::inputChannels() const {

    return 3;
}

double QFSimpleTCSPCImporter::avgCountRate(uint16_t channel) const {
    return cr.value(channel, 0);
}

QFTCSPCRecord QFSimpleTCSPCImporter::getCurrentRecord() const {
    return current;
}

double QFSimpleTCSPCImporter::percentCompleted() const {
    return double(currentTTTRRecordNum)/double(recordsInTTTRFile)*100.0;
}

