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

#ifndef PICOQUANT_PT3_TOOLS_H
#define PICOQUANT_PT3_TOOLS_H


#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <QString>
#include <stdlib.h>

#define PT3DISPCURVES 8
#define PT3T3WRAPAROUND 65536
#define PT3MEASMODE_T2 2
#define PT3MEASMODE_T3 3


/*
The following structures are used to hold the file data
They directly reflect the file structure.
The data types used here to match the file structure are correct
for the tested compilers.
They may have to be changed for other compilers.
*/


#pragma pack(4) //structure alignment to 4 byte boundaries

/* These are substructures used below */

struct PT3tParamStruct{ float Start;
                float Step;
                float End;  } ;

struct PT3tCurveMapping{ int MapTo;
                int Show; } ;

/* The following represents the readable ASCII file header portion */

struct PT3TxtHdr{		char Ident[16];				//"PicoHarp 300"
                char FormatVersion[6];		//file format version
                char CreatorName[18];		//name of creating software
                char CreatorVersion[12];	//version of creating software
                char FileTime[18];
                char CRLF[2];
                char CommentField[256]; } ;

/* The following is binary file header information */

struct PT3BinHdr{		int Curves;
                int BitsPerRecord;
                int RoutingChannels;
                int NumberOfBoards;
                int ActiveCurve;
                int MeasMode;
                int SubMode;
                int RangeNo;
                int Offset;
                int Tacq;				// in ms
                int StopAt;
                int StopOnOvfl;
                int Restart;
                int DispLinLog;
                int DispTimeFrom;		// 1ns steps
                int DispTimeTo;
                int DispCountsFrom;
                int DispCountsTo;
                PT3tCurveMapping DispCurves[PT3DISPCURVES];
                PT3tParamStruct Params[3];
                int RepeatMode;
                int RepeatsPerCurve;
                int RepeatTime;
                int RepeatWaitTime;
                char ScriptName[20];	} ;

/* The next is a board specific header */

struct PT3BoardHdr{
                char HardwareIdent[16];
                char HardwareVersion[8];
                int HardwareSerial;
                int SyncDivider;
                int CFDZeroCross0;
                int CFDLevel0;
                int CFDZeroCross1;
                int CFDLevel1;
                float Resolution;
                //below is new in format version 2.0
                int RouterModelCode;
                int RouterEnabled;
                int RtChan1_InputType;
                int RtChan1_InputLevel;
                int RtChan1_InputEdge;
                int RtChan1_CFDPresent;
                int RtChan1_CFDLevel;
                int RtChan1_CFDZeroCross;
                int RtChan2_InputType;
                int RtChan2_InputLevel;
                int RtChan2_InputEdge;
                int RtChan2_CFDPresent;
                int RtChan2_CFDLevel;
                int RtChan2_CFDZeroCross;
                int RtChan3_InputType;
                int RtChan3_InputLevel;
                int RtChan3_InputEdge;
                int RtChan3_CFDPresent;
                int RtChan3_CFDLevel;
                int RtChan3_CFDZeroCross;
                int RtChan4_InputType;
                int RtChan4_InputLevel;
                int RtChan4_InputEdge;
                int RtChan4_CFDPresent;
                int RtChan4_CFDLevel;
                int RtChan4_CFDZeroCross;		} ;


/* The next is a TTTR mode specific header */

struct PT3TTTRHdr{
                int ExtDevices;
                int Reserved1;
                int Reserved2;
                int CntRate0;
                int CntRate1;
                int StopAfter;
                int StopReason;
                int Records;
                int ImgHdrSize;		} ;


/*The following data records appear for each T3 mode event*/

union	PT3Record {
                 unsigned int allbits;
                 struct
                 {
                    unsigned numsync	:16;
                    unsigned dtime		:12;
                    unsigned channel	:4;
                 } bits;
                 struct
                 {
                    unsigned numsync	:16;
                    unsigned markers	:12;
                    unsigned channel	:4;
                 } special;
        } ;



////////////////////////////////////////////////////////////////////////////////////
// read the headers from the TTTR file and return them. This will move the read
// pointer to the location of the actual data
//
// This function returns true on success and false else (with an error description
// in the error parameter error).
////////////////////////////////////////////////////////////////////////////////////
bool PT3ReadConfiguration(FILE* fpin, PT3TxtHdr* txtHeader, PT3BinHdr* binHeader, PT3BoardHdr* boardHeader, PT3TTTRHdr* TTTRHeader, QString& error);

#endif // PICOQUANT_PT3_TOOLS_H
