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

#include "qftcspcreaderpicoquant.h"
#include <QObject>

QFTCSPCReaderPicoquant::QFTCSPCReaderPicoquant() {
    tttrfile=NULL;
    currentTTTRRecordNum=0;
//fileResetPos=0;
    ofltime=0;
    overflows=0;
    isV6=false;
}

QFTCSPCReaderPicoquant::~QFTCSPCReaderPicoquant() {
    if (tttrfile) fclose(tttrfile);
}

QString QFTCSPCReaderPicoquant::filter() const {
    return QObject::tr("PicoQuant TTTR (*.t3r)");
}

QString QFTCSPCReaderPicoquant::formatName() const {
    return QObject::tr("PicoQuant TTTR File");
}

bool QFTCSPCReaderPicoquant::open(const QString &filename, const QString &parameters) {
    Q_UNUSED(parameters)
    close();
    fileinfo.init(filename);
    tttrfile=fopen(filename.toLatin1().data(), "rb");
    if (tttrfile) {
        QString error="";
        isV6=false;
        bool ok=TTTRReadConfiguration(tttrfile, &txtHeader, &binHeader, &boardHeader, &TTTRHeader, error, isV6);
        if (!ok && isV6) {
            fseek(tttrfile,0,SEEK_SET);
            ok=TTTRReadConfiguration6(tttrfile, &txtHeader6, &binHeader6, &boardHeader6, &TTTRHeader6, error);
        }

        if (!ok) {
            setLastError(error);
            return false;
        }

        if (isV6) {
            fileinfo.properties["CommentField"]=txtHeader6.CommentField;
            fileinfo.properties["FileTime"]=txtHeader6.FileTime;
            fileinfo.properties["CreatorName"]=txtHeader6.CreatorName;
            fileinfo.properties["CreatorVersion"]=txtHeader6.CreatorVersion;

            fileinfo.properties["NumberOfChannels"]=(qlonglong)binHeader6.Channels;
            fileinfo.properties["NumberOfBoards"]=(qlonglong)binHeader6.NumberOfBoards;
            fileinfo.properties["RoutingChannels"]=(qlonglong)binHeader6.RoutingChannels;
            fileinfo.properties["MeasMode"]=(qlonglong)binHeader6.MeasMode;
            fileinfo.properties["SubMode"]=(qlonglong)binHeader6.SubMode;
            fileinfo.properties["AcquisitionTime"]=(qlonglong)binHeader6.Tacq;
            fileinfo.properties["StopAt"]=(qlonglong)binHeader6.StopAt;
            fileinfo.properties["StopOnOvfl"]=(qlonglong)binHeader6.StopOnOvfl;

            fileinfo.properties["BoardSerial"]=(qlonglong)boardHeader6.BoardSerial;
            fileinfo.properties["CFDZeroCross"]=(qlonglong)boardHeader6.CFDZeroCross;
            fileinfo.properties["CFDDiscrMin"]=(qlonglong)boardHeader6.CFDDiscrMin;
            fileinfo.properties["SyncLevel"]=(qlonglong)boardHeader6.SyncLevel;
            fileinfo.properties["CFDDiscrMin"]=(qlonglong)boardHeader6.CFDDiscrMin;

            fileinfo.properties["Globclock"]=(qlonglong)TTTRHeader6.Globclock;
            fileinfo.properties["SyncRate"]=(qlonglong)TTTRHeader6.SyncRate;
            fileinfo.properties["TTTRCFDRate"]=(qlonglong)TTTRHeader6.TTTRCFDRate;
            fileinfo.properties["TTTRStopAfter"]=(qlonglong)TTTRHeader6.TTTRStopAfter;
            fileinfo.properties["TTTRStopReason"]=(qlonglong)TTTRHeader6.TTTRStopReason;
            fileinfo.properties["NoOfRecords"]=(qlonglong)TTTRHeader6.NoOfRecords;
        } else {
            fileinfo.properties["CommentField"]=txtHeader.CommentField;
            fileinfo.properties["FileTime"]=txtHeader.FileTime;
            fileinfo.properties["HardwareVersion"]=txtHeader.HardwareVersion;
            fileinfo.properties["SoftwareVersion"]=txtHeader.SoftwareVersion;

            fileinfo.properties["NumberOfChannels"]=(qlonglong)binHeader.Channels;
            fileinfo.properties["NumberOfBoards"]=(qlonglong)binHeader.NumberOfBoards;
            fileinfo.properties["RoutingChannels"]=(qlonglong)binHeader.RoutingChannels;
            fileinfo.properties["MeasMode"]=(qlonglong)binHeader.MeasMode;
            fileinfo.properties["SubMode"]=(qlonglong)binHeader.SubMode;
            fileinfo.properties["AcquisitionTime"]=(qlonglong)binHeader.Tacq;
            fileinfo.properties["StopAt"]=(qlonglong)binHeader.StopAt;
            fileinfo.properties["StopOnOvfl"]=(qlonglong)binHeader.StopOnOvfl;

            fileinfo.properties["BoardSerial"]=(qlonglong)boardHeader.BoardSerial;
            fileinfo.properties["CFDZeroCross"]=(qlonglong)boardHeader.CFDZeroCross;
            fileinfo.properties["CFDDiscrMin"]=(qlonglong)boardHeader.CFDDiscrMin;
            fileinfo.properties["SyncLevel"]=(qlonglong)boardHeader.SyncLevel;
            fileinfo.properties["CFDDiscrMin"]=(qlonglong)boardHeader.CFDDiscrMin;

            fileinfo.properties["Globclock"]=(qlonglong)TTTRHeader.Globclock;
            fileinfo.properties["SyncRate"]=(qlonglong)TTTRHeader.SyncRate;
            fileinfo.properties["TTTRCFDRate"]=(qlonglong)TTTRHeader.TTTRCFDRate;
            fileinfo.properties["TTTRStopAfter"]=(qlonglong)TTTRHeader.TTTRStopAfter;
            fileinfo.properties["TTTRStopReason"]=(qlonglong)TTTRHeader.TTTRStopReason;
            fileinfo.properties["NoOfRecords"]=(qlonglong)TTTRHeader.NoOfRecords;
        }

        fgetpos(tttrfile, &fileResetPos);
        currentTTTRRecordNum=0;
        current.microtime_offset=0;
        if (isV6) current.microtime_deltaT=boardHeader6.Resolution;
        else current.microtime_deltaT=boardHeader.Resolution;
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
        if (isV6) current.microtime_deltaT=boardHeader6.Resolution;
        else current.microtime_deltaT=boardHeader.Resolution;

        return true;
    }
    setLastError(QObject::tr("could not open TTTR file '%1'").arg(filename));
    return false;
}

bool QFTCSPCReaderPicoquant::isOpenParametersUsed(QString *optionsDescription) const
{
    Q_UNUSED(optionsDescription)
    return false;
}

void QFTCSPCReaderPicoquant::close() {
    if (tttrfile) fclose(tttrfile);
    tttrfile=NULL;
    //fileResetPos=0;
    currentTTTRRecordNum=0;
    ofltime=0;
    overflows=0;

}

void QFTCSPCReaderPicoquant::reset() {
    fgetpos(tttrfile, &fileResetPos);
    currentTTTRRecordNum=0;
    ofltime=0;
    overflows=0;
    nextRecord();

}

bool QFTCSPCReaderPicoquant::nextRecord() {
    TTTRrecordType TTTRrecord;
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

        double truetime;
        if (isV6) truetime = double(ofltime + TTTRrecord.TimeTag) * TTTRHeader6.Globclock/1000000000.0 ; /* convert to seconds */
        else truetime = double(ofltime + TTTRrecord.TimeTag) * TTTRHeader.Globclock/1000000000.0 ; /* convert to seconds */

        current.isPhoton=false;
        current.marker_type=0;
        current.macrotime=truetime;
        current.input_channel=0;
        current.microtime_channel=0;
        if(TTTRrecord.Valid) {
            current.isPhoton=true;
            current.input_channel=TTTRrecord.Route;
            current.microtime_channel=TTTRrecord.Channel;
            ok=true;
        } else { /* this means we have a 'special record' */
            if(TTTRrecord.Channel & 0x800) { /* NEW v.5.0: Instead of te overflow bit we now evaluate this */
                /*	         AFTER! evaluating the valid record. */
                ofltime += 65536; /* unwrap the time tag overflow */
                overflows++;
                ok=false;
            }
            if(TTTRrecord.Channel & 0x007)	{
                current.isPhoton=false;
                current.marker_type=TTTRrecord.Channel & 0x007;
                ok=true;
            }
        }
    } while (!ok);

    if (isV6) return (int64_t(currentTTTRRecordNum)<TTTRHeader6.NoOfRecords && !feof(tttrfile));
    else return (int64_t(currentTTTRRecordNum)<TTTRHeader.NoOfRecords && !feof(tttrfile));
}

double QFTCSPCReaderPicoquant::measurementDuration() const {
    if (isV6) return double(binHeader6.Tacq)/1000.0;
    else return double(binHeader.Tacq)/1000.0;
}

uint16_t QFTCSPCReaderPicoquant::inputChannels() const {

    if (isV6) return binHeader6.RoutingChannels;
    else return binHeader.RoutingChannels;
}

uint32_t QFTCSPCReaderPicoquant::microtimeChannels() const
{
    if (isV6) return binHeader6.Channels;
    else return binHeader.Channels;

}

double QFTCSPCReaderPicoquant::microtimeChannelsResolutionPicoSeconds() const
{
    if (isV6) return boardHeader6.Resolution*1e3;
    else return boardHeader.Resolution*1e3;

}

double QFTCSPCReaderPicoquant::avgCountRate(uint16_t channel) const {
    return cr.value(channel, 0);
}

QFTCSPCRecord QFTCSPCReaderPicoquant::getCurrentRecord() const {
    return current;
}

double QFTCSPCReaderPicoquant::percentCompleted() const {
    if (isV6) return double(currentTTTRRecordNum)/double(TTTRHeader6.NoOfRecords)*100.0;
    else return double(currentTTTRRecordNum)/double(TTTRHeader.NoOfRecords)*100.0;

}

