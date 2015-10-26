/*
Copyright (c) 2012-2015 by Sebastian Isbaner

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

#include "readArrivalTimes.h"
#include<QDebug>

/* ensuring these bit-lengths of datatypes
 *
 *    - sizeof(void*): 64 bits
 *    - sizeof(char): 8 bits
 *    - sizeof(int): 32 bits
 *    - sizeof(short): 16 bits
 *    - sizeof(long int): 32 bits
 *    - sizeof(long long int): 64 bits
 *    - sizeof(float): 32 bits
 *    - sizeof(double): 64 bits
 *    - sizeof(bool): 8 bits
 *
 * */

int readTTTRfile( Photons& photons, QVector<double> &markerTimePin4, QVector<double> &markerTimePin5, const QString filename, const Channel route0, const Marker Pin4, bool forceAlternation, bool isV6)
{

#pragma pack(push)  /* push current alignment to stack */
#pragma pack(4)     /* set alignment to 1 byte boundary */

    /* The following represents the readable ASCII file header portion} */

    typedef struct{ int32_t Start;
                    int32_t Step;
                    int32_t End;  } tParamStruct;

    typedef struct{ int32_t MapTo;
                    int32_t Show; } tCurveMapping;

    struct {
        int8_t Ident[16];
        int8_t SoftwareVersion[6];
        int8_t HardwareVersion[6];
        int8_t FileTime[18];
        int8_t CRLF[2];
        int8_t CommentField[256]; } TxtHdr;

    /* The following is binary header information */

    struct {
        int32_t Channels;
        int32_t Curves;
        int32_t BitsPerChannel;
        int32_t RoutingChannels;
        int32_t NumberOfBoards;
        int32_t ActiveCurve;
        int32_t MeasMode;
        int32_t SubMode;
        int32_t RangeNo;
        int32_t Offset;			/* in ns */
        int32_t Tacq;				/* in ms */
        int32_t StopAt;
        int32_t StopOnOvfl;
        int32_t Restart;
        int32_t DispLinLog;
        int32_t DispTimeFrom;
        int32_t DispTimeTo;
        int32_t DispCountsFrom;
        int32_t DispCountsTo;
        tCurveMapping DispCurves[8];
        tParamStruct Params[3];
        int32_t RepeatMode;
        int32_t RepeatsPerCurve;
        int32_t RepeatTime;
        int32_t RepeatWaitTime;
        int8_t ScriptName[20];} BinHdr;

    struct {
        int32_t BoardSerial;
        int32_t CFDZeroCross;
        int32_t CFDDiscrMin;
        int32_t SyncLevel;
        int32_t CurveOffset;
        float Resolution;} BoardHdr;


    struct {
        int32_t Globclock;
        int32_t Reserved1;
        int32_t Reserved2;
        int32_t Reserved3;
        int32_t Reserved4;
        int32_t Reserved5;
        int32_t Reserved6;
        int32_t SyncRate;
        int32_t TTTRCFDRate;
        int32_t TTTRStopAfter;
        int32_t TTTRStopReason;
        int32_t NoOfRecords;
        int32_t SpecialHeaderSize;} TTTRHdr;


    typedef struct{ float Start;
                    float Step;
                    float End;  } tParamStruct6;

     struct {
        int32_t Globclock;
        int32_t ExtDevices;
        int32_t Reserved1;
        int32_t Reserved2;
        int32_t Reserved3;
        int32_t Reserved4;
        int32_t Reserved5;
        int32_t SyncRate;
        int32_t TTTRCFDRate;
        int32_t TTTRStopAfter;
        int32_t TTTRStopReason;
        int32_t NoOfRecords;
        int32_t SpecialHeaderSize;} TTTRHdr6;

     struct {
        int8_t HardwareIdent[16];
        int8_t HardwareVersion[8];
        int32_t BoardSerial;
        int32_t CFDZeroCross;
        int32_t CFDDiscrMin;
        int32_t SyncLevel;
        int32_t CurveOffset;
        float Resolution;} TTTRBoardHdr6;

     struct {
        int32_t Channels;
        int32_t Curves;
        int32_t BitsPerChannel;
        int32_t RoutingChannels;
        int32_t NumberOfBoards;
        int32_t ActiveCurve;
        int32_t MeasMode;
        int32_t SubMode;
        int32_t RangeNo;
        int32_t Offset;			/* in ns */
        int32_t Tacq;				/* in ms */
        int32_t StopAt;
        int32_t StopOnOvfl;
        int32_t Restart;
        int32_t DispLinLog;
        int32_t DispTimeFrom;
        int32_t DispTimeTo;
        int32_t DispCountsFrom;
        int32_t DispCountsTo;
        tCurveMapping DispCurves[8];
        tParamStruct6 Params[3];
        int32_t RepeatMode;
        int32_t RepeatsPerCurve;
        int32_t RepeatTime;
        int32_t RepeatWaitTime;
        int8_t ScriptName[20];} TTTRBinHdr6;

     struct {
        int8_t Ident[16];
        int8_t FormatVersion[6];
        int8_t CreatorName[18];
        int8_t CreatorVersion[12];
        int8_t FileTime[18];
        int8_t CRLF[2];
        int8_t CommentField[256]; } TTTRTxtHdr6;



    // The following data records will appear in the file NoOfRecords times

    struct {
        unsigned TimeTag	:16;
        unsigned Channel	:12;
        unsigned Route		:2;
        unsigned Valid		:1;
        unsigned Reserved	:1; }  TTTRrecord;

#pragma pack(pop)   /* restore original alignment from stack */

    if (!isV6) {

        AlexEvalLog::text("\nreading TTTR file...");
        if((photons.size()>0)||(markerTimePin4.size()>0)||(markerTimePin5.size()>0)) {
            AlexEvalLog::warning("Warning: overwriting data");
            photons.clear();
            markerTimePin4.clear();
            markerTimePin5.clear();
        }


        //--------------------------------------------------------
        // Open TTTR file
        //--------------------------------------------------------

        FILE *fpin;
        int32_t result,ii;
        long markerPin4=0, markerPin5=0, markerPin9=0; // count the markers in the data stream
        unsigned long overflows=0;
        double ofltime=0, truetime=0;
        //    int marker=0; // ensures that the markers alternate --> Pin 4 is always the first marker
        int markerErrorCount=0;
        Photon photon;
        qDebug()<<"DonorEm="<<QString::number(DonorEm,2).toStdString().c_str();
        Channel route1;
        Marker Pin5;
        if(route0==DonorEm) route1=AcceptorEm;
        else if(route0==AcceptorEm) route1=DonorEm;
        else qWarning()<<"error channel assignment";
        if(Pin4==DonorEx) Pin5=AcceptorEx;
        else if(Pin4==AcceptorEx) Pin5=DonorEx;
        else qWarning()<<"error marker assignment";
        Marker currentMarker=Pin5;


        if((fpin=fopen(filename.toStdString().c_str(),"rb"))==NULL)
        {AlexEvalLog::warning("\ncannot open input file\n"); return EXIT_FAILURE;}

        //--------------------------------------------------------
        // Read header
        //--------------------------------------------------------
        result = fread( &TxtHdr, 1, sizeof(TxtHdr) ,fpin);
        if (result!= sizeof(TxtHdr))
        { AlexEvalLog::warning("\nerror reading input file, aborted. 1");return EXIT_FAILURE;}

        if(strncmp((char*)TxtHdr.SoftwareVersion,"5.",2)!=0)
        {
            if(strncmp((char*)TxtHdr.SoftwareVersion,"6.",2)==0) {
                return readTTTRfile(photons, markerTimePin4, markerTimePin5,filename,route0,Pin4,forceAlternation, true);
            } else {
                AlexEvalLog::warning(QString("\nInput file version is %1. This program is for v5.x/6.x only. Aborted.").arg(
                                         (char*)TxtHdr.SoftwareVersion));return EXIT_FAILURE;
            }
        }
        result = fread( &BinHdr, 1, sizeof(BinHdr) ,fpin);
        if (result!= sizeof(BinHdr))
        {
            AlexEvalLog::warning("\nerror reading input file, aborted. 2"); return EXIT_FAILURE;
        }

        if (BinHdr.NumberOfBoards != 1)
        {
            AlexEvalLog::warning("\nerror: number of boards must be 1, aborted."); return EXIT_FAILURE;
        }

        result = fread( &BoardHdr, 1, sizeof(BoardHdr) ,fpin);
        if (result!= sizeof(BoardHdr))
        {
            AlexEvalLog::warning("\nerror reading input file, aborted. 3"); return EXIT_FAILURE;
        }

        result = fread( &TTTRHdr, 1, sizeof(TTTRHdr) ,fpin);
        if (result!= sizeof(TTTRHdr))
        {
            AlexEvalLog::warning("\nerror reading input file, aborted. 4"); return EXIT_FAILURE;;
        }

        AlexEvalLog::text("No of Records: "+QString::number(TTTRHdr.NoOfRecords));

        /* v5.0: skip the special header (you may need to read it if you
       want to interpret an imaging file */

        fseek(fpin,TTTRHdr.SpecialHeaderSize*4,SEEK_CUR);

        // reserve memory for vector containing all photons
        if (TTTRHdr.NoOfRecords > std::numeric_limits<int>::max()) qFatal("Too much data entries in file to use with integer data type");
        markerTimePin4.reserve((int)(TTTRHdr.NoOfRecords/4));
        markerTimePin5.reserve((int)(TTTRHdr.NoOfRecords/4));
        photons.reserve((int)(TTTRHdr.NoOfRecords/4));

        //--------------------------------------------------------

        for(ii=0; ii<TTTRHdr.NoOfRecords; ii++) // Reads the *.t3r file and combines the photons of one alternation period (blue or yellow)
        {
            result = fread( &TTTRrecord, 1, sizeof(TTTRrecord) ,fpin);
            if (result!= sizeof(TTTRrecord)) {
                AlexEvalLog::warning("\nerror reading input file, aborted. 6"); return EXIT_FAILURE;
            }

            truetime = (ofltime + TTTRrecord.TimeTag) * TTTRHdr.Globclock * 1e-9; // convert to seconds

            if(TTTRrecord.Valid)
            {
                photon.time=truetime;
                if (TTTRrecord.Route==0) {
                    photon.flags=currentMarker|route0;
                } else if (TTTRrecord.Route==1) {
                    photon.flags=currentMarker|route1;
                } else if (TTTRrecord.Route==2) {
                    AlexEvalLog::warning("unknown route (valid record) 2");
                    photon.flags=currentMarker|unknownChannel3;
                } else if (TTTRrecord.Route==3) {
                    AlexEvalLog::warning("unknown route (valid record) 3");
                    photon.flags=currentMarker|unknownChannel4;
                } else {
                    AlexEvalLog::warning("unknown route (valid record)"); return EXIT_FAILURE;
                }
                photons.push_back(photon);
            } // end of valid (photon) record
            else // this means we have a 'special record'
            {
                if(TTTRrecord.Channel & 0x800)  // Overflow,  0x800 = 0b100000000000
                {
                    ofltime += 65536; // unwrap the time tag overflow
                    overflows++;
                } else if (TTTRrecord.Channel & 0b100) { //0b100=0x004, Start marker at Pin 5
                    markerPin5++;
                    if(currentMarker==Pin4) {
                        markerTimePin5.push_back(truetime);
                    } else {
                        markerErrorCount++;
                        if(forceAlternation)
                            AlexEvalLog::warning("at record #"+QString::number(ii)+": found non-alternating marker at pin 5 ");
                        else markerTimePin5.push_back(truetime);
                    }
                    currentMarker=Pin5;
                } else if (TTTRrecord.Channel & 0b10) { //0b10=0x002, Start marker at Pin 4
                    markerPin4++;
                    if (currentMarker==Pin5) { // check alternation
                        markerTimePin4.push_back(truetime);
                    } else {
                        markerErrorCount++;
                        if(forceAlternation)
                            AlexEvalLog::warning("at record #"+QString::number(ii)+": found non-alternating marker at pin 4 ");
                        else markerTimePin4.push_back(truetime);
                    }
                    currentMarker=Pin4;
                } else if (TTTRrecord.Channel & 0b1) { // marker at Pin 9
                    markerPin9++;
                    currentMarker=Marker3;
                    AlexEvalLog::warning("at record #"+QString::number(ii)+": found marker at pin 9");
                } else {
                    AlexEvalLog::warning("Unknown special record #"+QString::number(ii) + " in channel " + QString::number(TTTRrecord.Channel));
                }
            } // end of special record

        } // end of reading binary
        fclose(fpin);

        if(photons.isEmpty()) {
            AlexEvalLog::warning("no photons found!");
            return EXIT_FAILURE;
        }
        QString resultInfo;
        QTextStream out(&resultInfo);
        out<<"\nSuccessfully read TTTR data with "<<TTTRHdr.NoOfRecords<<" records\n";
        out<<"found "<<markerTimePin4.size()<<" alternation periods at Pin4 and "<<markerTimePin5.size()<<" at Pin 5\n";
        out<<"number of markers at pin 4: "<<markerPin4<<" of which "<<markerPin4-markerTimePin4.size()<<" were ignored\n";
        out<<"number of markers at pin 5: "<<markerPin5<<" of which "<<markerPin5-markerTimePin5.size()<<" were ignored\n";
        out<<"number of markers at pin 9: "<<markerPin9 << " \n";
        out<<"in total "<<markerPin4+markerPin5+markerPin9<<" marker records of which "<<markerErrorCount<<" were not alternating\n";
        out<<"average countrate is "<<1e-3*(photons.size())/(photons.back().time-photons.front().time)<<"kHz";
        AlexEvalLog::text(resultInfo);

        return EXIT_SUCCESS;
    } else {
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// read V6
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        AlexEvalLog::text("\nreading TTTR file...");
        if((photons.size()>0)||(markerTimePin4.size()>0)||(markerTimePin5.size()>0)) {
            AlexEvalLog::warning("Warning: overwriting data");
            photons.clear();
            markerTimePin4.clear();
            markerTimePin5.clear();
        }


        //--------------------------------------------------------
        // Open TTTR file
        //--------------------------------------------------------

        FILE *fpin;
        int32_t result,ii;
        long markerPin4=0, markerPin5=0, markerPin9=0; // count the markers in the data stream
        unsigned long overflows=0;
        double ofltime=0, truetime=0;
        //    int marker=0; // ensures that the markers alternate --> Pin 4 is always the first marker
        int markerErrorCount=0;
        Photon photon;
        qDebug()<<"DonorEm="<<QString::number(DonorEm,2).toStdString().c_str();
        Channel route1;
        Marker Pin5;
        if(route0==DonorEm) route1=AcceptorEm;
        else if(route0==AcceptorEm) route1=DonorEm;
        else qWarning()<<"error channel assignment";
        if(Pin4==DonorEx) Pin5=AcceptorEx;
        else if(Pin4==AcceptorEx) Pin5=DonorEx;
        else qWarning()<<"error marker assignment";
        Marker currentMarker=Pin5;


        if((fpin=fopen(filename.toStdString().c_str(),"rb"))==NULL)
        {AlexEvalLog::warning("\ncannot open input file\n"); return EXIT_FAILURE;}

        //--------------------------------------------------------
        // Read header
        //--------------------------------------------------------
        result = fread( &TTTRTxtHdr6, 1, sizeof(TTTRTxtHdr6) ,fpin);
        if (result!= sizeof(TTTRTxtHdr6))
        { AlexEvalLog::warning("\nerror reading input file, aborted. 1");return EXIT_FAILURE;}

        if(strncmp((char*)TTTRTxtHdr6.FormatVersion,"6.",2)!=0)
        {
            AlexEvalLog::warning(QString("\nInput file version is %1. This program is for v5.x/6.x only. Aborted.").arg(
                                     (char*)TTTRTxtHdr6.FormatVersion));return EXIT_FAILURE;
        }
        result = fread( &TTTRBinHdr6, 1, sizeof(TTTRBinHdr6) ,fpin);
        if (result!= sizeof(TTTRBinHdr6))
        {
            AlexEvalLog::warning("\nerror reading input file, aborted. 2"); return EXIT_FAILURE;
        }

        if (TTTRBinHdr6.NumberOfBoards != 1)
        {
            AlexEvalLog::warning("\nerror: number of boards must be 1, aborted."); return EXIT_FAILURE;
        }

        result = fread( &TTTRBoardHdr6, 1, sizeof(TTTRBoardHdr6) ,fpin);
        if (result!= sizeof(TTTRBoardHdr6))
        {
            AlexEvalLog::warning("\nerror reading input file, aborted. 3"); return EXIT_FAILURE;
        }

        result = fread( &TTTRHdr6, 1, sizeof(TTTRHdr6) ,fpin);
        if (result!= sizeof(TTTRHdr6))
        {
            AlexEvalLog::warning("\nerror reading input file, aborted. 4"); return EXIT_FAILURE;;
        }

        AlexEvalLog::text("No of Records: "+QString::number(TTTRHdr6.NoOfRecords));

        /* v5.0: skip the special header (you may need to read it if you
       want to interpret an imaging file */

        fseek(fpin,TTTRHdr6.SpecialHeaderSize*4,SEEK_CUR);

        // reserve memory for vector containing all photons
        if (TTTRHdr6.NoOfRecords > std::numeric_limits<int>::max()) qFatal("Too much data entries in file to use with integer data type");
        markerTimePin4.reserve((int)(TTTRHdr6.NoOfRecords/4));
        markerTimePin5.reserve((int)(TTTRHdr6.NoOfRecords/4));
        photons.reserve((int)(TTTRHdr6.NoOfRecords/4));

        //--------------------------------------------------------

        for(ii=0; ii<TTTRHdr6.NoOfRecords; ii++) // Reads the *.t3r file and combines the photons of one alternation period (blue or yellow)
        {
            result = fread( &TTTRrecord, 1, sizeof(TTTRrecord) ,fpin);
            if (result!= sizeof(TTTRrecord)) {
                AlexEvalLog::warning("\nerror reading input file, aborted. 6"); return EXIT_FAILURE;
            }

            truetime = (ofltime + TTTRrecord.TimeTag) * TTTRHdr6.Globclock * 1e-9; // convert to seconds

            if(TTTRrecord.Valid)
            {
                photon.time=truetime;
                if (TTTRrecord.Route==0) {
                    photon.flags=currentMarker|route0;
                } else if (TTTRrecord.Route==1) {
                    photon.flags=currentMarker|route1;
                } else if (TTTRrecord.Route==2) {
                    AlexEvalLog::warning("unknown route (valid record) 2");
                    photon.flags=currentMarker|unknownChannel3;
                } else if (TTTRrecord.Route==3) {
                    AlexEvalLog::warning("unknown route (valid record) 3");
                    photon.flags=currentMarker|unknownChannel4;
                } else {
                    AlexEvalLog::warning("unknown route (valid record)"); return EXIT_FAILURE;
                }
                photons.push_back(photon);
            } // end of valid (photon) record
            else // this means we have a 'special record'
            {
                if(TTTRrecord.Channel & 0x800)  // Overflow,  0x800 = 0b100000000000
                {
                    ofltime += 65536; // unwrap the time tag overflow
                    overflows++;
                } else if (TTTRrecord.Channel & 0b100) { //0b100=0x004, Start marker at Pin 5
                    markerPin5++;
                    if(currentMarker==Pin4) {
                        markerTimePin5.push_back(truetime);
                    } else {
                        markerErrorCount++;
                        if(forceAlternation)
                            AlexEvalLog::warning("at record #"+QString::number(ii)+": found non-alternating marker at pin 5 ");
                        else markerTimePin5.push_back(truetime);
                    }
                    currentMarker=Pin5;
                } else if (TTTRrecord.Channel & 0b10) { //0b10=0x002, Start marker at Pin 4
                    markerPin4++;
                    if (currentMarker==Pin5) { // check alternation
                        markerTimePin4.push_back(truetime);
                    } else {
                        markerErrorCount++;
                        if(forceAlternation)
                            AlexEvalLog::warning("at record #"+QString::number(ii)+": found non-alternating marker at pin 4 ");
                        else markerTimePin4.push_back(truetime);
                    }
                    currentMarker=Pin4;
                } else if (TTTRrecord.Channel & 0b1) { // marker at Pin 9
                    markerPin9++;
                    currentMarker=Marker3;
                    AlexEvalLog::warning("at record #"+QString::number(ii)+": found marker at pin 9");
                } else {
                    AlexEvalLog::warning("Unknown special record #"+QString::number(ii) + " in channel " + QString::number(TTTRrecord.Channel));
                }
            } // end of special record

        } // end of reading binary
        fclose(fpin);

        if(photons.isEmpty()) {
            AlexEvalLog::warning("no photons found!");
            return EXIT_FAILURE;
        }
        QString resultInfo;
        QTextStream out(&resultInfo);
        out<<"\nSuccessfully read TTTR data with "<<TTTRHdr6.NoOfRecords<<" records\n";
        out<<"found "<<markerTimePin4.size()<<" alternation periods at Pin4 and "<<markerTimePin5.size()<<" at Pin 5\n";
        out<<"number of markers at pin 4: "<<markerPin4<<" of which "<<markerPin4-markerTimePin4.size()<<" were ignored\n";
        out<<"number of markers at pin 5: "<<markerPin5<<" of which "<<markerPin5-markerTimePin5.size()<<" were ignored\n";
        out<<"number of markers at pin 9: "<<markerPin9 << " \n";
        out<<"in total "<<markerPin4+markerPin5+markerPin9<<" marker records of which "<<markerErrorCount<<" were not alternating\n";
        out<<"average countrate is "<<1e-3*(photons.size())/(photons.back().time-photons.front().time)<<"kHz";
        AlexEvalLog::text(resultInfo);

        return EXIT_SUCCESS;
    }
}



int readBurstFile(QVector<Burst> &burstVector, const QString filename)
{
    QList<QVector<double> > data;
    Burst burst;
    readCSVtoList(data, filename);

    int n=data.size();

    if(n!=4) AlexEvalLog::warning(QString::number(n) + "columns instead of 4 in data file");
    n=data[0].size();
    for(int i=0;i<n;i++) {
        burst.startIdx=data[0].value(i,0);
        burst.startTime=data[0].value(i,0);
        burst.numberOfPhotons=data[0].value(i,0);
        burst.duration=data[0].value(i,0)*1e-3; // convert from ms to s
        burstVector.append(burst);
    }
    return EXIT_SUCCESS;

}



int readCSVtoList(QList<QVector<double> >& data, const QString filename, const QString delimiter, const QString comment)
{
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {AlexEvalLog::warning(file.errorString()); return EXIT_FAILURE;}
    AlexEvalLog::text("read file"+filename);
    QTextStream in(&file);

    bool conv_ok=true;
    QString line, header;
    int lc=-1;

    do {// skip header lines
        line=in.readLine();
        header+=line;
        ++lc;
    } while( line.startsWith("#")||line.startsWith("\\")||line.startsWith("/")||line.startsWith(comment) );

    int lc_header=lc;
    qDebug()<<"cut "<<lc_header<<"lines of header";
    QStringList lines=line.split(delimiter);
    int ncol=lines.size();
    if (data.size()==ncol) qDebug("reading %i columns",ncol);
    else AlexEvalLog::warning("too less vectors specified");
    data.reserve(ncol);
    while(!in.atEnd()&&conv_ok) {
        conv_ok=true;
        for(int i=0;i<ncol;i++) {
            double d=lines[i].toDouble(&conv_ok);
            if(!conv_ok) AlexEvalLog::warning("error: conversion to double failed in line" + QString::number(lc));
            data[i].append(d);
        }
        line=in.readLine();
        lines=line.split(delimiter);
        lc++;
    }
    qDebug()<<"read"<<lc<<"lines with"<<ncol<<" columns and"<<lc-lc_header-1<<"rows";

    file.close();

    return EXIT_SUCCESS;
}



int readTTTRfileOld( QVector<double> &photonArrivalTimeRoute0, QVector<double> &photonArrivalTimeRoute1, QVector<double> &markerTimePin4, QVector<double> &markerTimePin5, const QString filename, bool forceAlternation)
{
    /* The following represents the readable ASCII file header portion} */
#pragma pack(push)  /* push current alignment to stack */
#pragma pack(4)     /* set alignment to 1 byte boundary */

    typedef struct{ int32_t Start;
                    int32_t Step;
                    int32_t End;  } tParamStruct;

    typedef struct{ int32_t MapTo;
                    int32_t Show; } tCurveMapping;

    struct {
        int8_t Ident[16];
        int8_t SoftwareVersion[6];
        int8_t HardwareVersion[6];
        int8_t FileTime[18];
        int8_t CRLF[2];
        int8_t CommentField[256]; } TxtHdr;

    /* The following is binary header information */

    struct {
        int32_t Channels;
        int32_t Curves;
        int32_t BitsPerChannel;
        int32_t RoutingChannels;
        int32_t NumberOfBoards;
        int32_t ActiveCurve;
        int32_t MeasMode;
        int32_t SubMode;
        int32_t RangeNo;
        int32_t Offset;			/* in ns */
        int32_t Tacq;				/* in ms */
        int32_t StopAt;
        int32_t StopOnOvfl;
        int32_t Restart;
        int32_t DispLinLog;
        int32_t DispTimeFrom;
        int32_t DispTimeTo;
        int32_t DispCountsFrom;
        int32_t DispCountsTo;
        tCurveMapping DispCurves[8];
        tParamStruct Params[3];
        int32_t RepeatMode;
        int32_t RepeatsPerCurve;
        int32_t RepeatTime;
        int32_t RepeatWaitTime;
        int8_t ScriptName[20];} BinHdr;

    struct {
        int32_t BoardSerial;
        int32_t CFDZeroCross;
        int32_t CFDDiscrMin;
        int32_t SyncLevel;
        int32_t CurveOffset;
        float Resolution;} BoardHdr;


    struct {
        int32_t Globclock;
        int32_t Reserved1;
        int32_t Reserved2;
        int32_t Reserved3;
        int32_t Reserved4;
        int32_t Reserved5;
        int32_t Reserved6;
        int32_t SyncRate;
        int32_t TTTRCFDRate;
        int32_t TTTRStopAfter;
        int32_t TTTRStopReason;
        int32_t NoOfRecords;
        int32_t SpecialHeaderSize;} TTTRHdr;

    // The following data records will appear in the file NoOfRecords times

    struct {
        unsigned TimeTag	:16;
        unsigned Channel	:12;
        unsigned Route		:2;
        unsigned Valid		:1;
        unsigned Reserved	:1; }  TTTRrecord;


#pragma pack(pop)   /* restore original alignment from stack */
    AlexEvalLog::text("reading TTTR file...");
    if((photonArrivalTimeRoute0.size()>0)||(photonArrivalTimeRoute1.size()>0)||(markerTimePin4.size()>0)||(markerTimePin5.size()>0)) {
        AlexEvalLog::warning("Warning: overwriting data");
        photonArrivalTimeRoute0.clear();
        photonArrivalTimeRoute1.clear();
        markerTimePin4.clear();
        markerTimePin5.clear();
    }


    //--------------------------------------------------------
    // Open TTTR file
    //--------------------------------------------------------

    FILE *fpin;
    int32_t result,ii;
    long markerPin4=0, markerPin5=0, markerPin9=0; // count the markers in the data stream
    unsigned long overflows=0;
    double ofltime=0, truetime=0;
    int marker=0; // ensures that the markers alternate --> Pin 4 is always the first marker
    float kanal0 = 0.0;
    float kanal1 = 0.0;
    int markerErrorCount=0;


    if((fpin=fopen(filename.toStdString().c_str(),"rb"))==NULL)
    {AlexEvalLog::warning("\ncannot open input file\n"); return EXIT_FAILURE;}

    //--------------------------------------------------------
    // Read header
    //--------------------------------------------------------
    result = fread( &TxtHdr, 1, sizeof(TxtHdr) ,fpin);
    if (result!= sizeof(TxtHdr))
    { AlexEvalLog::warning("\nerror reading input file, aborted. 1");return EXIT_FAILURE;}

    if(strncmp((char*)TxtHdr.SoftwareVersion,"5.",2))
    {
        AlexEvalLog::warning(QString("\nInput file version is %1. This program is for v5.0 only. Aborted.").arg(
                   (char*)TxtHdr.SoftwareVersion));return EXIT_FAILURE;
    }
    result = fread( &BinHdr, 1, sizeof(BinHdr) ,fpin);
    if (result!= sizeof(BinHdr))
    {
      AlexEvalLog::warning("\nerror reading input file, aborted. 2"); return EXIT_FAILURE;
     }

     if (BinHdr.NumberOfBoards != 1)
     {
       AlexEvalLog::warning("\nerror: number of boards must be 1, aborted."); return EXIT_FAILURE;
     }

    result = fread( &BoardHdr, 1, sizeof(BoardHdr) ,fpin);
    if (result!= sizeof(BoardHdr))
    {
      AlexEvalLog::warning("\nerror reading input file, aborted. 3"); return EXIT_FAILURE;
    }

    result = fread( &TTTRHdr, 1, sizeof(TTTRHdr) ,fpin);
    if (result!= sizeof(TTTRHdr))
     {
       AlexEvalLog::warning("\nerror reading input file, aborted. 4"); return EXIT_FAILURE;;
     }

    AlexEvalLog::text("\nNo of Records: "+QString::number(TTTRHdr.NoOfRecords));

    /* v5.0: skip the special header (you may need to read it if you
       want to interpret an imaging file */

    fseek(fpin,TTTRHdr.SpecialHeaderSize*4,SEEK_CUR);

    // reserve memory for vector containing all photons
    if (TTTRHdr.NoOfRecords > std::numeric_limits<int>::max()) qFatal("Too much data entries in file to use with integer data type");
    photonArrivalTimeRoute0.reserve((int)(TTTRHdr.NoOfRecords/4));
    photonArrivalTimeRoute1.reserve((int)(TTTRHdr.NoOfRecords/4));
    markerTimePin4.reserve((int)(TTTRHdr.NoOfRecords/2));
    markerTimePin5.reserve((int)(TTTRHdr.NoOfRecords/2));

    //--------------------------------------------------------

    for(ii=0; ii<TTTRHdr.NoOfRecords; ii++) // Reads the *.t3r file and combines the photons of one alternation period (blue or yellow)
     {
        result = fread( &TTTRrecord, 1, sizeof(TTTRrecord) ,fpin);
        if (result!= sizeof(TTTRrecord)) {
            AlexEvalLog::warning("\nerror reading input file, aborted. 6"); return EXIT_FAILURE;
        }

        truetime = (ofltime + TTTRrecord.TimeTag) * TTTRHdr.Globclock * 1e-9; // convert to seconds

        if(TTTRrecord.Valid)
        {

            if (TTTRrecord.Route==0) {kanal0 += 1.0;
            photonArrivalTimeRoute0.push_back(truetime);
            } else if (TTTRrecord.Route==1) {kanal1 += 1.0;
                photonArrivalTimeRoute1.push_back(truetime);
            } else {
                AlexEvalLog::warning("unknown route signal (valid record)"); return EXIT_FAILURE;
            }
        }
        else // this means we have a 'special record'
        {
            if(TTTRrecord.Channel & 0x800)  // Overflow
            {
                ofltime += 65536; // unwrap the time tag overflow
                overflows++;
            } else if (TTTRrecord.Channel & 0x004) { // Start marker at Pin 5 (488er Laser = Donor)
                markerPin5++;
                if(marker==1) {
                    markerTimePin5.push_back(truetime);
                    marker=0;
                } else {
                    markerErrorCount++;
                    if(forceAlternation)
                        AlexEvalLog::warning("at record #"+QString::number(ii)+": non-alternating marker at pin 5 found");
                    else markerTimePin5.push_back(truetime);
                }
            } else if (TTTRrecord.Channel & 0x002) { // Start marker at Pin 4 (568er Laser)
                markerPin4++;
                if (marker==0) {
                    markerTimePin4.push_back(truetime);
                    marker=1;
                } else {
                    markerErrorCount++;
                    if(forceAlternation)
                        AlexEvalLog::warning("at record #"+QString::number(ii)+": non-alternating marker at pin 4 found");
                    else markerTimePin4.push_back(truetime);
                }
            } else {
                AlexEvalLog::warning("Unknown special record #"+QString::number(ii) + " in channel " + QString::number(TTTRrecord.Channel));
            }
        }

     } // end of reading binary
    fclose(fpin);

    if(photonArrivalTimeRoute0.isEmpty()||photonArrivalTimeRoute1.isEmpty()) {
        AlexEvalLog::warning("no photons found!");
        return EXIT_FAILURE;
    }
    QString resultInfo;
    QTextStream out(&resultInfo);
    out<<"\nSuccessfully read TTTR data with "<<TTTRHdr.NoOfRecords<<" records\n";
    out<<"found "<<markerTimePin4.size()<<" alternation periods at Pin4 and "<<markerTimePin5.size()<<" at Pin 5\n";
    out<<"number of markers at pin 4: "<<markerPin4<<" of which "<<markerPin4-markerTimePin4.size()<<" were ignored\n";
    out<<"number of markers at pin 5: "<<markerPin5<<" of which "<<markerPin5-markerTimePin5.size()<<" were ignored\n";
    out<<"number of markers at pin 9: "<<markerPin9 << " \n";
    out<<"in total "<<markerPin4+markerPin5+markerPin9<<" marker records of which "<<markerErrorCount<<" were not alternating\n";
    out<<"average countrate is "<<1e-3*(photonArrivalTimeRoute0.size()+photonArrivalTimeRoute1.size())/(photonArrivalTimeRoute0.back()-photonArrivalTimeRoute0.front())<<"kHz";
    AlexEvalLog::text(resultInfo);

    return EXIT_SUCCESS;
}


int checkAlternationPeriods(Photons &photons, QVector<double> &markerTimeDexc, QVector<double> &markerTimeAexc)
{
    qDebug("check alternation periods NEW");

    double periodDonorMean=0;
    double periodAcceptorMean=0;
    if(markerTimeDexc.isEmpty()&&markerTimeAexc.isEmpty()) {
        AlexEvalLog::warning("no markers found.");
        return EXIT_FAILURE;
    } else if(markerTimeDexc.isEmpty()) {
        checkMarker(markerTimeAexc);
        periodAcceptorMean=(markerTimeAexc.last()- markerTimeAexc.first())/(markerTimeAexc.size()-1);
        AlexEvalLog::warning(QString("no donor markers found. Assume the same period as for acceptor markers: %1us").arg(periodAcceptorMean*1e6));
        for(int i=0; i<markerTimeAexc.size();++i) {
            markerTimeDexc.push_back(markerTimeAexc.at(i) + periodAcceptorMean/2);
        }
    } else if ( markerTimeAexc.isEmpty()) {
        checkMarker(markerTimeDexc);
        periodDonorMean=(markerTimeDexc.last()- markerTimeDexc.first())/(markerTimeDexc.size()-1);
        AlexEvalLog::warning(QString("no acceptor markers found. Assume the same period as for donor markers: %1us").arg(periodDonorMean*1e6));
        for(int i=0; i<markerTimeDexc.size();i++) {
            markerTimeAexc.push_back(markerTimeDexc.at(i) + periodDonorMean/2);
        }
    } else { // should be the regular case
        periodDonorMean=(markerTimeDexc.last()- markerTimeDexc.first())/(markerTimeDexc.size()-1);
        periodAcceptorMean=(markerTimeAexc.last()- markerTimeAexc.first())/(markerTimeAexc.size()-1);
    }

    if(checkMarker(markerTimeDexc,markerTimeAexc)==-1){
        AlexEvalLog::warning("Error");
    }

    // remove unassignable photons
    setRange(photons,markerTimeDexc.first(),markerTimeAexc.last());

    if(markerTimeDexc.size()!=markerTimeAexc.size()) {
        AlexEvalLog::warning("Dmarker size=" + QString::number(markerTimeDexc.size())+" Amarker size="+ QString::number(markerTimeAexc.size()));
        return EXIT_FAILURE;
    }

    // assign correct excitation channels
    int markeridx=0,photonIdx=0;
    bool setperiod;
    while(photonIdx<photons.size()) {
        setperiod=false;
        while ((!setperiod) && (markeridx<markerTimeDexc.size())) {
            if ((markerTimeDexc[markeridx]<photons[photonIdx].time )&& (photons[photonIdx].time<=markerTimeAexc[markeridx])) { // note that the makro time of two events can indeed be the same. thus the "smaller or equal" operator is vital!
                // then it is in the donor excitation period
                if(photons[photonIdx].flags&DonorEx); // if true, the photon is already correct assigned
                else {
#ifdef MYDEBUG
                    AlexEvalLog::text("at index "+QString::number(last)+": setting Dex"+" (time="+QString::number(photons[last].time)+", markerTimeDexc="+QString::number(markerTimeDexc.at(markeridx))+" markerTimeAexc=" + QString::number(markerTimeAexc.at(markeridx))+")" );
#endif
#ifdef PHOTONMACRO
                    SetDex(photons[photonIdx]);
#else
                    photons[photonIdx].setDex();
#endif

                }
                setperiod=true;
            } else if ((markerTimeAexc[markeridx]<photons[photonIdx].time) && (photons[photonIdx].time<=markerTimeDexc[markeridx+1])) {
                if(photons[photonIdx].flags&AcceptorEx); // already correct assigned
                else {
#ifdef MYDEBUG
                    AlexEvalLog::text("at index "+QString::number(last)+": setting Aex"+" (time="+QString::number(photons[last].time)+", markerTimeAexc="+QString::number(markerTimeAexc.at(markeridx))+" markerTimeDexc=" + QString::number(markerTimeDexc.at(markeridx+1))+")" );
#endif
#ifdef PHOTONMACRO
                    SetAex(photons[photonIdx]);
#else
                    photons[photonIdx].setAex();
#endif
                }
                setperiod=true;
            } else ++markeridx; // try next alternation period
        }
        ++photonIdx;
    }



    return EXIT_SUCCESS;
}

//int setAlternationPeriods(Photons &photons, QVector<double> &markerTimeDexc, QVector<double> &markerTimeAexc)
//{
//}

int checkMarker(QVector<double> &markerTimeChannel1, QVector<double> &markerTimeChannel2)
{
    qDebug("check marker");
    int inserted=-1; // counter for the total inserted marker events

    if(markerTimeChannel1.isEmpty()||markerTimeChannel2.isEmpty()) {
        AlexEvalLog::warning("no markers found");
        return inserted;
    }

    // When marker events at the beginning or at the end are missing, their corresponding markers in the other channel are deleted

    while (markerTimeChannel1.first()>markerTimeChannel2.first()) markerTimeChannel2.pop_front(); // assure that channel 1 has the first marker event
    while(markerTimeChannel1.last()>markerTimeChannel2.last()) markerTimeChannel1.pop_back(); // assure that channel 2 has the last marker event

    double start = markerTimeChannel1.first();
    double end = markerTimeChannel2.last();
    setRange(markerTimeChannel1,start,end);
    setRange(markerTimeChannel2,start,end); // faster than "while() pop_front()"

    qDebug("Channel 1");
    inserted=checkMarker(markerTimeChannel1);

    qDebug("Channel 2");
    inserted+=checkMarker(markerTimeChannel2);

    QString markerInfo;
    QTextStream out(&markerInfo);
    out<<"marker  ch1 ... ch2\n";
    out<<"size "<<markerTimeChannel1.size()<<"..."<<markerTimeChannel2.size()<<"\n";
    out<<"first "<<markerTimeChannel1.first()<<"..."<<markerTimeChannel2.first()<<"\n";
    out<<"last "<<markerTimeChannel1.last()<<"..."<<markerTimeChannel2.last()<<"";


    int eventCount=0;
    for(int i=0; (i<markerTimeChannel1.size())&&(i<markerTimeChannel2.size());i++) { // consistecy check: not necessary
        if(markerTimeChannel1[i]>markerTimeChannel2[i]) {
            AlexEvalLog::warning(QString("markerCh1>markerCh2 at %1. removing event in Ch2").arg(i));
            markerTimeChannel2.remove(i);
            eventCount++;
            int event=i;
            int offset=5;
            for(int j=qMax(event-offset,0);j<qMin(event+offset,markerTimeChannel1.size());++j) {
                out<<j<<":"<<markerTimeChannel1.at(j)*1e6<<"..."<<markerTimeChannel2.at(j)*1e6<<"\n";
            }
            if((eventCount/(markerTimeChannel1.size()+markerTimeChannel2.size())) > 0.2) {
                AlexEvalLog::warning("too many (>20%) irregular marker events. aborting...");
                return -1;
            }
        }
    }
    AlexEvalLog::text(markerInfo);

    return inserted;
}



int checkMarker(QVector<double> &markerTime,double alternationPeriod)
{
    qDebug("check %i markers",markerTime.size());
    int inserted=0; // counter for the total inserted marker events

    if(markerTime.isEmpty()) {
        AlexEvalLog::warning("no markers found.");
        return inserted;
    }

    // calculate the average time between marker events
    double timeBetweenMarkers=(markerTime.last()- markerTime.first())/(markerTime.size()-1);
    if(!alternationPeriod) alternationPeriod=timeBetweenMarkers; // approximate alternation period by average

    AlexEvalLog::text("time between markers is "+QString::number(timeBetweenMarkers*1e6)+"us. Using alternation period of "+QString::number(alternationPeriod*1e6)+"us");

    // FIXME: add correction at the beginning of the vector.
    for(int ii=0;ii<markerTime.size()-1;++ii){
        if(((markerTime[ii+1]-markerTime[ii])>alternationPeriod*1.8)) { // if the deviation from the approximated alternation period is more than 20%, insert a new marker
            AlexEvalLog::text(QString("time between marker %1 and %2 is %3 us. inserting...").arg(ii).arg(ii+1).arg((markerTime[ii+1]-markerTime[ii])*1e6,0,'f',4));
            markerTime.insert(ii+1,markerTime[ii]+alternationPeriod);
            inserted++;
            if(inserted>200) {
                AlexEvalLog::warning("Inserted 200 additional marker events. Aborting now...");
                return inserted;
            }
        }
    }
    qDebug("inserted %i markers",inserted);

    return inserted;
}



int background(BurstDual &background, const Photons photons)
{
    int photonIdx=0;

    qDebug("background");
    if(photons.isEmpty()) {
        AlexEvalLog::warning("no photon times. aborting...");
        return EXIT_FAILURE;
    }
    background.reset();
    background.type=Background;
    background.startTime =photons.first().time;
    double tend=photons.last().time;
    background.duration=tend - background.startTime;

    // count the photons
    QString markerInfo;
    QTextStream out(&markerInfo);
    out<<"background  donor ... acceptor \n";
//    out<<"size "<<markerTimeDexc.size()<<" ... "<<markerTimeAexc.size()<<"\n ";
//    out<<"first marker "<<markerTimeDexc.first()<<" ... "<<markerTimeAexc.first()<<"\n ";
//    out<<"last marker "<<markerTimeDexc.last()<<" ... "<<markerTimeAexc.last()<<"\n ";
    out<<"first photon "<<photons.first().time<<"\n ";
    out<<"last photon "<<photons.last().time;
    AlexEvalLog::text(markerInfo);

    background.startIdx=photonIdx;

    while(photonIdx<photons.size()) { // count the number of photons of the new burst
        ++background.numberOfPhotons;
#ifdef PHOTONMACRO
        if     (isDem_Dex(photons.at(photonIdx))) ++background.n_Dem_Dex;
        else if(isAem_Dex(photons.at(photonIdx))) ++background.n_Aem_Dex;
        else if(isAem_Aex(photons.at(photonIdx))) ++background.n_Aem_Aex;
        else if(isDem_Aex(photons.at(photonIdx))) ++background.n_Dem_Aex;
//        else if(isDem(photons.at(photonIdx))) ++background.n_Dem_Dex; // this is for single color spFRET measurements
//        else if(isAem(photons.at(photonIdx))) ++background.n_Aem_Dex; // this is for single color spFRET measurements
#else
        if(photons.at(photonIdx).isDem_Dex()) ++background.n_Dem_Dex;
        else if(photons.at(photonIdx).isAem_Aex()) ++background.n_Aem_Aex;
        else if(photons.at(photonIdx).isAem_Dex()) ++background.n_Aem_Dex;
        else if(photons.at(photonIdx).isDem_Aex()) ++background.n_Dem_Aex;
#endif
        ++photonIdx;
    }

    qDebug("background search found %i background photons",background.numberOfPhotons);
    qDebug(background.toString().c_str());
    return EXIT_SUCCESS;
}


int setRange(Photons& photons, const double &Tstart, const double &Tend)
{
    int idxStart=0,idxStop=photons.size()-1,n=photons.size();
    if(Tstart>Tend) {
        AlexEvalLog::warning("start>end");
        photons.clear();
        return n;
    }
    if (Tstart>photons.first().time&&Tend>=photons.last().time) {
        while(photons.at(idxStart).time<Tstart) idxStart++;
        photons.remove(0,idxStart);
        idxStop++;
    } else if (Tstart<=photons.first().time&&Tend<photons.last().time) {
        while(photons.at(idxStop).time>Tend) idxStop--;
        photons.remove(idxStop,photons.size()-idxStop);
    } else if(Tstart>photons.first().time&&Tend<photons.last().time) {
        while(photons.at(idxStart).time<Tstart) idxStart++;
        idxStop=idxStart;
        while(photons.at(idxStop).time<Tend) idxStop++;
        photons.remove(idxStop,photons.size()-idxStop);
        photons.remove(0,idxStart);
    } else {
        qDebug("start: %f first: %f end: %f last: %f",Tstart,photons.first().time,Tend,photons.last().time);
        qWarning("out of range");
        return EXIT_FAILURE;
    }

    qDebug("removed %i entries in front and %i at the end",idxStart,n-idxStop);
    return n-idxStop+idxStart;
}



int setRange(QVector<double> &orderedVector, const double &start, const double &end)
{
    int idxStart=0,idxStop=orderedVector.size()-1,n=orderedVector.size();
    if(start>end) {
        AlexEvalLog::warning("start>end");
        orderedVector.clear();
        return n;
    }
    if (start>orderedVector.first()&&end>=orderedVector.last()) {
        while(orderedVector.at(idxStart)<start) idxStart++;
        orderedVector.remove(0,idxStart);
        idxStop++;
    } else if (start<=orderedVector.first()&&end<orderedVector.last()) {
        while(orderedVector.at(idxStop)>end) idxStop--;
        orderedVector.remove(idxStop,orderedVector.size()-idxStop);
    } else if(start>orderedVector.first()&&end<orderedVector.last()) {
        while(orderedVector.at(idxStart)<start) idxStart++;
        idxStop=idxStart;
        while(orderedVector.at(idxStop)<end) idxStop++;
        orderedVector.remove(idxStop,orderedVector.size()-idxStop);
        orderedVector.remove(0,idxStart);
    } else {
        qDebug("start: %f first: %f end: %f last: %f",start,orderedVector.first(),end,orderedVector.last());
        qWarning("out of range");
        return EXIT_FAILURE;
    }

    qDebug("removed %i entries in front and %i at the end",idxStart,n-idxStop);
    return n-idxStop+idxStart;
}


int estimateMarkerEvents(QVector<double> &markerTimeDelayed, const QVector<double> &markerTime, const double offsetTime)
{
    if (markerTimeDelayed.size()>0) {
        qWarning("estimateMarkerEvents: vector not empty. clearing...");
        markerTimeDelayed.clear();
    }
    markerTimeDelayed.reserve(markerTime.size());
    for(int i=0;i<markerTime.size();++i)
        markerTimeDelayed.append(markerTime.at(i)+offsetTime);

    return EXIT_SUCCESS;
}


int constructMarkerEvents(QVector<double> &markerTime, QVector<double> &markerTimeDelayed, const double &period, const double &offsetTime, const double &tMax)
{
    if (markerTime.size()>0||markerTimeDelayed.size()>0) {
        qWarning("vectors not empty. clearing...");
        markerTime.clear();
        markerTimeDelayed.clear();
    }
    int nEvents = (int) (tMax/period);
    if (nEvents<=0) {
        AlexEvalLog::warning("nEvents<=0. check period and offset times");
        return EXIT_FAILURE;
    }

    for(int i=0;i<nEvents;++i){
        markerTime.append(i*period);
        markerTimeDelayed.append(i*period+offsetTime);
    }

    return EXIT_SUCCESS;
}


int writeListtoCSV(const QList<QVector<double> >& data, const QString &prefix, const QString filename, const QString delimiter, const QString comment)
{
    QFile file;
    if (filename.endsWith(".csv")||filename.endsWith(".txt")||filename.endsWith(".asc"))
        file.setFileName(filename);
    else file.setFileName(filename+".csv");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {AlexEvalLog::warning(file.errorString()); return EXIT_FAILURE;}
    AlexEvalLog::text("write file"+filename);
    QTextStream out(&file);
    out.setRealNumberPrecision(9);
    out.setRealNumberNotation(QTextStream::SmartNotation);
    QString header(prefix);

    if(prefix.startsWith(comment));
    else {
        header=header.replace("\n","\n"+comment);
        header=comment+header;
    }
    if(header.endsWith(comment)) header.chop(comment.length());
    if(header.endsWith("\n"));
    else header +="\n";
    out<<header;

    int ncol=data.size();
    qDebug("writing %i columns",ncol);
    int maxSize=0;
    for(int i=0;i<ncol;i++) {
        if (data.at(i).size()>maxSize) {
            maxSize=data.at(i).size();
        }
    }

    for(int i=0;i<maxSize;++i) {
        for(int j=0;j<ncol-1;++j) {
            out<<data.at(j).value(i,NAN)<<delimiter;
        }
        out<<data.at(ncol-1).value(i,NAN)<<"\n";
    }

    qDebug()<<"wrote"<<maxSize<<"lines with"<<ncol<<"columns";

    file.close();

    return EXIT_SUCCESS;
}


