/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

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

#include "qftcspcreaderpicoquant.h"
#include <QObject>

QFTCSPCReaderPicoquant::QFTCSPCReaderPicoquant() {
    tttrfile=NULL;
    currentTTTRRecordNum=0;
//fileResetPos=0;
    ofltime=0;
    overflows=0;
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
    close();
    fileinfo.init(filename);
    tttrfile=fopen(filename.toAscii().data(), "rb");
    if (tttrfile) {
        QString error="";
        bool ok=TTTRReadConfiguration(tttrfile, &txtHeader, &binHeader, &boardHeader, &TTTRHeader, error);
        /*fprintf(fpout,"# Identifier       : %.*s\n",sizeof(txtHeader.Ident),txtHeader.Ident);
        fprintf(fpout,"# Software Version : %.*s\n",sizeof(txtHeader.SoftwareVersion),txtHeader.SoftwareVersion);
        fprintf(fpout,"# Hardware Version : %.*s\n",sizeof(txtHeader.HardwareVersion),txtHeader.HardwareVersion);
        fprintf(fpout,"# Time of Creation : %.*s\n",sizeof(txtHeader.FileTime),txtHeader.FileTime);
        fprintf(fpout,"# File Comment     : %.*s\n",sizeof(txtHeader.CommentField),txtHeader.CommentField);

        fprintf(fpout,"# No of Channels   : %ld\n",binHeader.Channels);
        fprintf(fpout,"# No of Curves     : %ld\n",binHeader.Curves);
        fprintf(fpout,"# Bits per Channel : %ld\n",binHeader.BitsPerChannel);
        fprintf(fpout,"# Routing Channels : %ld\n",binHeader.RoutingChannels);
        fprintf(fpout,"# No of Boards     : %ld\n",binHeader.NumberOfBoards);
        fprintf(fpout,"# Active Curve     : %ld\n",binHeader.ActiveCurve);
        fprintf(fpout,"# Measurement Mode : %ld\n",binHeader.MeasMode);
        fprintf(fpout,"# Measurem.SubMode : %ld\n",binHeader.SubMode);
        fprintf(fpout,"# Range No         : %ld\n",binHeader.RangeNo);
        fprintf(fpout,"# Offset           : %ld\n",binHeader.Offset);
        fprintf(fpout,"# AcquisitionTime  : %ld\n",binHeader.Tacq);
        fprintf(fpout,"# Stop at          : %ld\n",binHeader.StopAt);
        fprintf(fpout,"# Stop on Ovfl.    : %ld\n",binHeader.StopOnOvfl);
        fprintf(fpout,"# Restart          : %ld\n",binHeader.Restart);
        fprintf(fpout,"# DispLinLog       : %ld\n",binHeader.DispLinLog);
        fprintf(fpout,"# DispTimeAxisFrom : %ld\n",binHeader.DispTimeFrom);
        fprintf(fpout,"# DispTimeAxisTo   : %ld\n",binHeader.DispTimeTo);
        fprintf(fpout,"# DispCountAxisFrom: %ld\n",binHeader.DispCountsFrom);
        fprintf(fpout,"# DispCountAxisTo  : %ld\n",binHeader.DispCountsTo);

        fprintf(fpout,"# Board serial     : %ld\n",boardHeader.BoardSerial);
        fprintf(fpout,"# CFDZeroCross     : %ld\n",boardHeader.CFDZeroCross);
        fprintf(fpout,"# CFDDiscriminMin  : %ld\n",boardHeader.CFDDiscrMin);
        fprintf(fpout,"# SYNCLevel        : %ld\n",boardHeader.SyncLevel);
        fprintf(fpout,"# Curve Offset     : %ld\n",boardHeader.CurveOffset);
        fprintf(fpout,"# Resolution       : %f\n", boardHeader.Resolution);

        fprintf(fpout,"# Glob Clock       : %ld\n",TTTRHeader.Globclock);
        fprintf(fpout,"# Sync Rate        : %ld\n",TTTRHeader.SyncRate);
        fprintf(fpout,"# Average CFD Rate : %ld\n",TTTRHeader.TTTRCFDRate);
        fprintf(fpout,"# Stop After       : %ld\n",TTTRHeader.TTTRStopAfter);
        fprintf(fpout,"# Stop Reason      : %ld\n",TTTRHeader.TTTRStopReason);
        fprintf(fpout,"# No of Records    : %ld\n",TTTRHeader.NoOfRecords);
        fprintf(fpout,"# Special Hdr Size : %ld\n\n",TTTRHeader.SpecialHeaderSize);
        printf("\nNo. of Records	 : %ld\n\n",TTTRHeader.NoOfRecords);*/

        if (!ok) {
            setLastError(error);
            return false;
        }

        fgetpos(tttrfile, &fileResetPos);
        currentTTTRRecordNum=0;
        current.microtime_offset=0;
        current.microtime_deltaT=boardHeader.Resolution;
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

        return true;
    }
    setLastError(QObject::tr("could not open TTTR file '%1'").arg(filename));
    return false;
}

bool QFTCSPCReaderPicoquant::isOpenParametersUsed(QString *optionsDescription) const
{
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
        double truetime = double(ofltime + TTTRrecord.TimeTag) * TTTRHeader.Globclock/1000000000.0 ; /* convert to seconds */

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

    return (int64_t(currentTTTRRecordNum)<TTTRHeader.NoOfRecords && !feof(tttrfile));
}

double QFTCSPCReaderPicoquant::measurementDuration() const {
    return double(binHeader.Tacq)/1000.0;
}

uint16_t QFTCSPCReaderPicoquant::inputChannels() const {

    return binHeader.RoutingChannels;
}

double QFTCSPCReaderPicoquant::avgCountRate(uint16_t channel) const {
    return cr.value(channel, 0);
}

QFTCSPCRecord QFTCSPCReaderPicoquant::getCurrentRecord() const {
    return current;
}

double QFTCSPCReaderPicoquant::percentCompleted() const {
    return double(currentTTTRRecordNum)/double(TTTRHeader.NoOfRecords)*100.0;
}

