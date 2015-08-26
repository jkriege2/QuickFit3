/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    

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

#include "qftcspcreaderpicoquantpt3.h"
#include <QObject>

QFTCSPCReaderPicoquantPT3::QFTCSPCReaderPicoquantPT3() {
    tttrfile=NULL;
    currentTTTRRecordNum=0;
    ofltime=0;
    overflows=0;
    duration=0;
}

QFTCSPCReaderPicoquantPT3::~QFTCSPCReaderPicoquantPT3() {
    if (tttrfile) fclose(tttrfile);
}

QString QFTCSPCReaderPicoquantPT3::filter() const {
    return QObject::tr("PicoQuant PicoHarp 300 File (*.pt3)");
}

QString QFTCSPCReaderPicoquantPT3::formatName() const {
    return QObject::tr("PicoQuant PicoHarp 300 PT3 File");
}

bool QFTCSPCReaderPicoquantPT3::open(const QString &filename, const QString &parameters) {
    Q_UNUSED(parameters)

    close();
    fileinfo.init(filename);
    tttrfile=fopen(filename.toLatin1().data(), "rb");
    if (tttrfile) {
        QString error="";
        bool ok=PT3ReadConfiguration(tttrfile, &txtHeader, &binHeader, &boardHeader, &TTTRHeader, error);
        syncperiod = (double)1E9 / TTTRHeader.CntRate0; //in nanoseconds

        if (!ok) {
            setLastError(error);
            return false;
        }


        fileinfo.properties["CommentField"]=txtHeader.CommentField;
        fileinfo.properties["FileTime"]=txtHeader.FileTime;
        fileinfo.properties["CreatorName"]=txtHeader.CreatorName;
        fileinfo.properties["CreatorVersion"]=txtHeader.CreatorVersion;

        fileinfo.properties["Tacq"]=(qlonglong)binHeader.Tacq;
        fileinfo.properties["NumberOfBoards"]=(qlonglong)binHeader.NumberOfBoards;
        fileinfo.properties["RoutingChannels"]=(qlonglong)binHeader.RoutingChannels;
        fileinfo.properties["MeasMode"]=(qlonglong)binHeader.MeasMode;
        fileinfo.properties["SubMode"]=(qlonglong)binHeader.SubMode;
        fileinfo.properties["StopAt"]=(qlonglong)binHeader.StopAt;
        fileinfo.properties["StopOnOvfl"]=(qlonglong)binHeader.StopOnOvfl;

        fileinfo.properties["HardwareIdent"]=boardHeader.HardwareIdent;
        fileinfo.properties["HardwareVersion"]=boardHeader.HardwareVersion;
        fileinfo.properties["HardwareSerial"]=boardHeader.HardwareSerial;
        fileinfo.properties["SyncDivider"]=(qlonglong)boardHeader.SyncDivider;
        fileinfo.properties["CFDZeroCross0"]=(qlonglong)boardHeader.CFDZeroCross0;
        fileinfo.properties["CFDLevel0"]=(qlonglong)boardHeader.CFDLevel0;
        fileinfo.properties["CFDZeroCross1"]=(qlonglong)boardHeader.CFDZeroCross1;
        fileinfo.properties["CFDLevel1"]=(qlonglong)boardHeader.CFDLevel1;
        fileinfo.properties["Resolution"]=boardHeader.Resolution;


        fileinfo.properties["CntRate0"]=(qlonglong)TTTRHeader.CntRate0;
        fileinfo.properties["CntRate1"]=(qlonglong)TTTRHeader.CntRate1;
        fileinfo.properties["StopAfter"]=(qlonglong)TTTRHeader.StopAfter;
        fileinfo.properties["StopReason"]=(qlonglong)TTTRHeader.StopReason;
        fileinfo.properties["NoOfRecords"]=(qlonglong)TTTRHeader.Records;

        fgetpos(tttrfile, &fileResetPos);
        currentTTTRRecordNum=0;
        current.microtime_offset=0;
        current.microtime_deltaT=boardHeader.Resolution;
        current.isPhoton=true;
        current.marker_type=0;
        nextRecord();

        // read some photons to estimate a countrate
        cr.clear();
        double t=0;
        uint16_t cnt=0;
        do {
            QFTCSPCRecord r=getCurrentRecord();
            t=r.absoluteTime();
            cr[r.input_channel]=cr.value(r.input_channel, 0)+1;
            cnt++;
        } while (nextRecord() && (t<0.01) && (cnt<10000));
        for (int i=0; i<inputChannels(); i++) {
            cr[i]=cr.value(i, 0)/t/1000.0;
        }

        fsetpos(tttrfile, &fileResetPos);
        currentTTTRRecordNum=0;
        current.microtime_offset=0;
        current.microtime_deltaT=boardHeader.Resolution;

        duration=double(binHeader.Tacq)/1000.0;

        return true;
    }
    setLastError(QObject::tr("could not open TTTR file '%1'").arg(filename));
    duration=0;
    return false;
}

bool QFTCSPCReaderPicoquantPT3::isOpenParametersUsed(QString *optionsDescription) const
{
    Q_UNUSED(optionsDescription)

    return false;
}

void QFTCSPCReaderPicoquantPT3::close() {
    if (tttrfile) fclose(tttrfile);
    tttrfile=NULL;
    //fileResetPos=0;
    currentTTTRRecordNum=0;
    ofltime=0;
    overflows=0;

}

void QFTCSPCReaderPicoquantPT3::reset() {
    fgetpos(tttrfile, &fileResetPos);
    currentTTTRRecordNum=0;
    ofltime=0;
    overflows=0;
    nextRecord();

}

bool QFTCSPCReaderPicoquantPT3::nextRecord() {
    PT3Record TTTRrecord;
    bool ok=false;
    do {
        // READ ONE RECORD and check for errors
        int result = fread( &TTTRrecord, 1, sizeof(TTTRrecord) ,tttrfile);
        if (result!= sizeof(TTTRrecord)) {
            setLastError(QObject::tr("error reading input file."));
            return false;
        }
        currentTTTRRecordNum++;

        // calculate the true time of the photon
        double truensync = ((double)ofltime+(double)TTTRrecord.bits.numsync);
        double truetime = (truensync*syncperiod + (double)TTTRrecord.bits.dtime*(double)boardHeader.Resolution)/1000000000.0;

        current.macrotime=truetime;
        current.isPhoton=false;
        if(TTTRrecord.bits.channel!=0xF) {
            current.input_channel=TTTRrecord.bits.channel;
            current.microtime_channel=TTTRrecord.bits.dtime;
            current.isPhoton=true;
            ok=true;
        } else { /* this means we have a 'special record' */
            if(TTTRrecord.special.markers==0) { // not a marker means overflow
                ofltime += PT3T3WRAPAROUND; /* unwrap the time tag overflow */
                overflows++;
                ok=false;
            } else {
                // we have a marker
                truetime = truensync*syncperiod/1000000000.0;
                current.macrotime=truetime;
                current.marker_type=TTTRrecord.special.markers;
                ok=true;
            }
        }
    } while (!ok);


    return (int64_t(currentTTTRRecordNum)<TTTRHeader.Records && !feof(tttrfile));
}

double QFTCSPCReaderPicoquantPT3::measurementDuration() const {
    //return double(binHeader.Tacq)/1000.0;
    return duration;
}

uint16_t QFTCSPCReaderPicoquantPT3::inputChannels() const {

    return binHeader.RoutingChannels;
}

double QFTCSPCReaderPicoquantPT3::avgCountRate(uint16_t channel) const {
    return cr.value(channel, 0);
}

QFTCSPCRecord QFTCSPCReaderPicoquantPT3::getCurrentRecord() const {
    return current;
}

double QFTCSPCReaderPicoquantPT3::percentCompleted() const {
    return double(currentTTTRRecordNum)/double(TTTRHeader.Records)*100.0;
}

uint32_t QFTCSPCReaderPicoquantPT3::microtimeChannels() const
{
    return 65536;
}

double QFTCSPCReaderPicoquantPT3::microtimeChannelsResolutionPicoSeconds() const
{
    return boardHeader.Resolution*1e3;
}

