/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    last modification: $LastChangedDate: 2014-09-02 17:40:55 +0200 (Di, 02 Sep 2014) $  (revision $Rev: 3430 $)

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

#include "qftcspcreaderpicoquantptu.h"
#include <QObject>
#include <QDebug>

QFTCSPCReaderPicoquantPTU::QFTCSPCReaderPicoquantPTU() {
    tttrfile=NULL;
    currentTTTRRecordNum=0;
    ofltime=0;
    overflows=0;
    ptuinfo.duration=0;
}

QFTCSPCReaderPicoquantPTU::~QFTCSPCReaderPicoquantPTU() {
    if (tttrfile) fclose(tttrfile);
}

QString QFTCSPCReaderPicoquantPTU::filter() const {
    return QObject::tr("PicoQuant Unified TTTR File (*.ptu)");
}

QString QFTCSPCReaderPicoquantPTU::formatName() const {
    return QObject::tr("PicoQuant Unified TTTR PTU File");
}

bool QFTCSPCReaderPicoquantPTU::open(const QString &filename, const QString &parameters) {
    Q_UNUSED(parameters)

    close();
    fileinfo.init(filename);
    tttrfile=fopen(filename.toLatin1().data(), "rb");
    if (tttrfile) {
        QString error="";
        bool ok=PTUReadConfiguration(tttrfile, ptuinfo, error);

        fileinfo.properties["PTU_DEVICE"]=ptuinfo.device;
        fileinfo.properties["PTU_VERSION"]=ptuinfo.version;
        fileinfo.properties["PTU_DEVICE"]=ptuinfo.device;
        fileinfo.comment=ptuinfo.comment;
        /*qDebug()<<"-------------------------------------------------------";
        qDebug()<<"device = "<<ptuinfo.device;
        qDebug()<<"version = "<<ptuinfo.version;
        qDebug()<<"channels = "<<ptuinfo.channels;
        qDebug()<<"duration = "<<ptuinfo.duration;
        qDebug()<<"NumRecords = "<<ptuinfo.NumRecords;
        qDebug()<<"RecordType = "<<ptuinfo.RecordType;
        qDebug()<<"GlobRes = "<<ptuinfo.GlobRes;
        qDebug()<<"Resolution = "<<ptuinfo.Resolution;
        qDebug()<<"trueTimeToSecond = "<<ptuinfo.trueTimeToSecond;
        qDebug()<<"-------------------------------------------------------";*/
        for (int i=0; i<ptuinfo.header_data.size(); i++) {
            //qDebug()<<ptuinfo.header_data[i].first<<" = "<<ptuinfo.header_data[i].second;
            QString id=ptuinfo.header_data[i].first;
            QString idd=id;
            int cnt=1;
            while (fileinfo.properties.contains(idd)) {
                idd=id+QString::number(cnt);
                cnt++;
            }
            fileinfo.properties[idd]=ptuinfo.header_data[i].second;
        }
        //qDebug()<<"-------------------------------------------------------";


        if (!ok) {
            setLastError(error);
            return false;
        }

        fgetpos(tttrfile, &fileResetPos);
        currentTTTRRecordNum=0;
        current.microtime_offset=0;
        current.microtime_deltaT=ptuinfo.Resolution;
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
            if (r.isPhoton) cr[r.input_channel]=cr.value(r.input_channel, 0)+1;
            cnt++;
        } while (nextRecord() && (t<0.01) && (cnt<10000));
        for (int i=0; i<inputChannels(); i++) {
            cr[i]=cr.value(i, 0)/t/1000.0;
        }

        fsetpos(tttrfile, &fileResetPos);
        currentTTTRRecordNum=0;
        current.microtime_offset=0;
        current.microtime_deltaT=ptuinfo.Resolution;

        //duration=double(binHeader.Tacq)/1000.0;

        return true;
    }
    setLastError(QObject::tr("could not open TTTR file '%1'").arg(filename));
    ptuinfo.duration=0;
    return false;
}

bool QFTCSPCReaderPicoquantPTU::isOpenParametersUsed(QString *optionsDescription) const
{
    Q_UNUSED(optionsDescription)

    return false;
}

void QFTCSPCReaderPicoquantPTU::close() {
    if (tttrfile) fclose(tttrfile);
    tttrfile=NULL;
    //fileResetPos=0;
    currentTTTRRecordNum=0;
    ofltime=0;
    overflows=0;

}

void QFTCSPCReaderPicoquantPTU::reset() {
    fgetpos(tttrfile, &fileResetPos);
    currentTTTRRecordNum=0;
    ofltime=0;
    overflows=0;
    nextRecord();

}

bool QFTCSPCReaderPicoquantPTU::nextRecord() {
    uint32_t TTTRRecord;
    bool ok=false;
    do {
        // READ ONE RECORD and check for errors
        int result = fread( &TTTRRecord, 1, sizeof(TTTRRecord) ,tttrfile);
        if (result!= sizeof(TTTRRecord)) {
            setLastError(QObject::tr("error reading input file."));
            return false;
        }
        currentTTTRRecordNum++;
        switch (ptuinfo.RecordType) {
            case rtPicoHarpT2:
                ptuinfo.IsT2 = true;
                ok=ProcessPHT2(ptuinfo, TTTRRecord, current, ofltime, overflows);
                break;
            case rtPicoHarpT3:
                ptuinfo.IsT2 = false;
                ok=ProcessPHT3(ptuinfo, TTTRRecord, current, ofltime, overflows);
                break;
            case rtHydraHarpT2:
                ptuinfo.IsT2 = true;
                ok=ProcessHHT2(ptuinfo, TTTRRecord, current, ofltime, overflows, 1);
                break;
            case rtHydraHarpT3:
                ptuinfo.IsT2 = false;
                ok=ProcessHHT3(ptuinfo, TTTRRecord, current, ofltime, overflows, 1);
                break;
            case rtHydraHarp2T2:
            case rtTimeHarp260NT2:
            case rtTimeHarp260PT2:
                ptuinfo.IsT2 = true;
                ok=ProcessHHT2(ptuinfo, TTTRRecord, current, ofltime, overflows, 2);
                break;
            case rtHydraHarp2T3:
            case rtTimeHarp260NT3:
            case rtTimeHarp260PT3:
                ptuinfo.IsT2 = false;
                ok=ProcessHHT3(ptuinfo, TTTRRecord, current, ofltime, overflows, 2);
                break;
            default:
                return false;
        }

    } while (!ok);


    return (int64_t(currentTTTRRecordNum)<ptuinfo.NumRecords && !feof(tttrfile));
}






double QFTCSPCReaderPicoquantPTU::measurementDuration() const {
    //return double(binHeader.Tacq)/1000.0;
    return ptuinfo.duration;
}

uint16_t QFTCSPCReaderPicoquantPTU::inputChannels() const {

    return ptuinfo.channels;
}

double QFTCSPCReaderPicoquantPTU::avgCountRate(uint16_t channel) const {
    return cr.value(channel, 0);
}

QFTCSPCRecord QFTCSPCReaderPicoquantPTU::getCurrentRecord() const {
    return current;
}

double QFTCSPCReaderPicoquantPTU::percentCompleted() const {
    return double(currentTTTRRecordNum)/double(ptuinfo.NumRecords)*100.0;
}

