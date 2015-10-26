
#ifndef PICOQUANT_PT3_TOOLS_H
#define PICOQUANT_PT3_TOOLS_H


#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <QString>
#include <stdlib.h>
#include <stdint.h>
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

#pragma pack(4) //structure alignment to 4 byte boundaries

/* These are substructures used below */

struct PT3tParamStruct{ float Start;
                float Step;
                float End;  } ;

struct PT3tCurveMapping{ int32_t MapTo;
                int32_t Show; } ;

/* The following represents the readable ASCII file header portion */

struct PT3TxtHdr{		int8_t Ident[16];				//"PicoHarp 300"
                int8_t FormatVersion[6];		//file format version
                int8_t CreatorName[18];		//name of creating software
                int8_t CreatorVersion[12];	//version of creating software
                int8_t FileTime[18];
                int8_t CRLF[2];
                int8_t CommentField[256]; } ;

/* The following is binary file header information */

struct PT3BinHdr{		int32_t Curves;
                int32_t BitsPerRecord;
                int32_t RoutingChannels;
                int32_t NumberOfBoards;
                int32_t ActiveCurve;
                int32_t MeasMode;
                int32_t SubMode;
                int32_t RangeNo;
                int32_t Offset;
                int32_t Tacq;				// in ms
                int32_t StopAt;
                int32_t StopOnOvfl;
                int32_t Restart;
                int32_t DispLinLog;
                int32_t DispTimeFrom;		// 1ns steps
                int32_t DispTimeTo;
                int32_t DispCountsFrom;
                int32_t DispCountsTo;
                PT3tCurveMapping DispCurves[PT3DISPCURVES];
                PT3tParamStruct Params[3];
                int32_t RepeatMode;
                int32_t RepeatsPerCurve;
                int32_t RepeatTime;
                int32_t RepeatWaitTime;
                int8_t ScriptName[20];	} ;

/* The next is a board specific header */

struct PT3BoardHdr{
                int8_t HardwareIdent[16];
                int8_t HardwareVersion[8];
                int32_t HardwareSerial;
                int32_t SyncDivider;
                int32_t CFDZeroCross0;
                int32_t CFDLevel0;
                int32_t CFDZeroCross1;
                int32_t CFDLevel1;
                float Resolution;
                //below is new in format version 2.0
                int32_t RouterModelCode;
                int32_t RouterEnabled;
                int32_t RtChan1_InputType;
                int32_t RtChan1_InputLevel;
                int32_t RtChan1_InputEdge;
                int32_t RtChan1_CFDPresent;
                int32_t RtChan1_CFDLevel;
                int32_t RtChan1_CFDZeroCross;
                int32_t RtChan2_InputType;
                int32_t RtChan2_InputLevel;
                int32_t RtChan2_InputEdge;
                int32_t RtChan2_CFDPresent;
                int32_t RtChan2_CFDLevel;
                int32_t RtChan2_CFDZeroCross;
                int32_t RtChan3_InputType;
                int32_t RtChan3_InputLevel;
                int32_t RtChan3_InputEdge;
                int32_t RtChan3_CFDPresent;
                int32_t RtChan3_CFDLevel;
                int32_t RtChan3_CFDZeroCross;
                int32_t RtChan4_InputType;
                int32_t RtChan4_InputLevel;
                int32_t RtChan4_InputEdge;
                int32_t RtChan4_CFDPresent;
                int32_t RtChan4_CFDLevel;
                int32_t RtChan4_CFDZeroCross;		} ;


/* The next is a TTTR mode specific header */

struct PT3TTTRHdr{
                int32_t ExtDevices;
                int32_t Reserved1;
                int32_t Reserved2;
                int32_t CntRate0;
                int32_t CntRate1;
                int32_t StopAfter;
                int32_t StopReason;
                int32_t Records;
                int32_t ImgHdrSize;		} ;


/*The following data records appear for each T3 mode event*/

union	PT3Record {
                   uint32_t allbits;
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
