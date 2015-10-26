
#ifndef PICOQUANT_TOOLS_H
#define PICOQUANT_TOOLS_H

#include <QString>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#pragma pack(push)  /* push current alignment to stack */
#pragma pack(4)     /* set alignment to 1 byte boundary */

/*
The following structures are used to hold the TimeHarp file data
They reflect the file structure.
The data types used here to match the file structure are correct
for MSVC++. They may have to be changed for other compilers.
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
typedef struct {
    int32_t Start;
    int32_t Step;
    int32_t End;
} tParamStruct;

typedef struct{ float Start;
                float Step;
                float End;  } tParamStruct6;

typedef struct {
    int32_t MapTo;
    int32_t Show;
} tCurveMapping;

/* The following represents the readable ASCII file header portion} */

typedef struct {
    int8_t Ident[16];
    int8_t SoftwareVersion[6];
    int8_t HardwareVersion[6];
    int8_t FileTime[18];
    int8_t CRLF[2];
    int8_t CommentField[256];
} TTTRTxtHdr;

typedef struct {
    int8_t Ident[16];
    int8_t FormatVersion[6];
    int8_t CreatorName[18];
    int8_t CreatorVersion[12];
    int8_t FileTime[18];
    int8_t CRLF[2];
    int8_t CommentField[256]; } TTTRTxtHdr6;

/* The following is binary header information */

typedef struct {
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
    int8_t ScriptName[20];
} TTTRBinHdr;

typedef struct {
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

typedef struct {
    int32_t BoardSerial;
    int32_t CFDZeroCross;
    int32_t CFDDiscrMin;
    int32_t SyncLevel;
    int32_t CurveOffset;
    float Resolution;
} TTTRBoardHdr;

typedef struct {
    int8_t HardwareIdent[16];
    int8_t HardwareVersion[8];
    int32_t BoardSerial;
    int32_t CFDZeroCross;
    int32_t CFDDiscrMin;
    int32_t SyncLevel;
    int32_t CurveOffset;
    float Resolution;} TTTRBoardHdr6;

typedef struct {
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
    int32_t SpecialHeaderSize;
} TTTRHdr;

typedef struct {
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

/*The following data records will appear in the file NoOfRecords times*/

typedef struct {
    unsigned TimeTag	:16;
    unsigned Channel	:12;
    unsigned Route		:2;
    unsigned Valid		:1;
    unsigned Reserved	:1;
}  TTTRrecordType;

#pragma pack(pop)   /* restore original alignment from stack */

////////////////////////////////////////////////////////////////////////////////////
// read the headers from the TTTR file and return them. This will move the read
// pointer to the location of the actual data
//
// This function returns true on success and false else (with an error description
// in the error parameter error).
////////////////////////////////////////////////////////////////////////////////////
bool TTTRReadConfiguration(FILE* fpin, TTTRTxtHdr* txtHeader, TTTRBinHdr* binHeader, TTTRBoardHdr* boardHeader, TTTRHdr* TTTRHeader, QString& error, bool& isV6);
bool TTTRReadConfiguration6(FILE* fpin, TTTRTxtHdr6* txtHeader, TTTRBinHdr6* binHeader, TTTRBoardHdr6* boardHeader, TTTRHdr6* TTTRHeader, QString& error);

#endif // PICOQUANT_TOOLS_H
