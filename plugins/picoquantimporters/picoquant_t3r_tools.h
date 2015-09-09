
#ifndef PICOQUANT_TOOLS_H
#define PICOQUANT_TOOLS_H

#include <QString>
#include <stdio.h>
#include <stdlib.h>

/*
The following structures are used to hold the TimeHarp file data
They reflect the file structure.
The data types used here to match the file structure are correct
for MSVC++. They may have to be changed for other compilers.
*/

typedef struct {
    long int Start;
    long int Step;
    long int End;
} tParamStruct;

typedef struct{ float Start;
                float Step;
                float End;  } tParamStruct6;

typedef struct {
    long int MapTo;
    long int Show;
} tCurveMapping;

/* The following represents the readable ASCII file header portion} */

typedef struct {
    char Ident[16];
    char SoftwareVersion[6];
    char HardwareVersion[6];
    char FileTime[18];
    char CRLF[2];
    char CommentField[256];
} TTTRTxtHdr;

typedef struct {
    char Ident[16];
    char FormatVersion[6];
    char CreatorName[18];
    char CreatorVersion[12];
    char FileTime[18];
    char CRLF[2];
    char CommentField[256]; } TTTRTxtHdr6;

/* The following is binary header information */

typedef struct {
    long int Channels;
    long int Curves;
    long int BitsPerChannel;
    long int RoutingChannels;
    long int NumberOfBoards;
    long int ActiveCurve;
    long int MeasMode;
    long int SubMode;
    long int RangeNo;
    long int Offset;			/* in ns */
    long int Tacq;				/* in ms */
    long int StopAt;
    long int StopOnOvfl;
    long int Restart;
    long int DispLinLog;
    long int DispTimeFrom;
    long int DispTimeTo;
    long int DispCountsFrom;
    long int DispCountsTo;
    tCurveMapping DispCurves[8];
    tParamStruct Params[3];
    long int RepeatMode;
    long int RepeatsPerCurve;
    long int RepeatTime;
    long int RepeatWaitTime;
    char ScriptName[20];
} TTTRBinHdr;

typedef struct {
    long int Channels;
    long int Curves;
    long int BitsPerChannel;
    long int RoutingChannels;
    long int NumberOfBoards;
    long int ActiveCurve;
    long int MeasMode;
    long int SubMode;
    long int RangeNo;
    long int Offset;			/* in ns */
    long int Tacq;				/* in ms */
    long int StopAt;
    long int StopOnOvfl;
    long int Restart;
    long int DispLinLog;
    long int DispTimeFrom;
    long int DispTimeTo;
    long int DispCountsFrom;
    long int DispCountsTo;
    tCurveMapping DispCurves[8];
    tParamStruct6 Params[3];
    long int RepeatMode;
    long int RepeatsPerCurve;
    long int RepeatTime;
    long int RepeatWaitTime;
    char ScriptName[20];} TTTRBinHdr6;

typedef struct {
    long int BoardSerial;
    long int CFDZeroCross;
    long int CFDDiscrMin;
    long int SyncLevel;
    long int CurveOffset;
    float Resolution;
} TTTRBoardHdr;

typedef struct {
    char HardwareIdent[16];
    char HardwareVersion[8];
    long int BoardSerial;
    long int CFDZeroCross;
    long int CFDDiscrMin;
    long int SyncLevel;
    long int CurveOffset;
    float Resolution;} TTTRBoardHdr6;

typedef struct {
    long int Globclock;
    long int Reserved1;
    long int Reserved2;
    long int Reserved3;
    long int Reserved4;
    long int Reserved5;
    long int Reserved6;
    long int SyncRate;
    long int TTTRCFDRate;
    long int TTTRStopAfter;
    long int TTTRStopReason;
    long int NoOfRecords;
    long int SpecialHeaderSize;
} TTTRHdr;

typedef struct {
    long int Globclock;
    long int ExtDevices;
    long int Reserved1;
    long int Reserved2;
    long int Reserved3;
    long int Reserved4;
    long int Reserved5;
    long int SyncRate;
    long int TTTRCFDRate;
    long int TTTRStopAfter;
    long int TTTRStopReason;
    long int NoOfRecords;
    long int SpecialHeaderSize;} TTTRHdr6;

/*The following data records will appear in the file NoOfRecords times*/

typedef struct {
    unsigned TimeTag	:16;
    unsigned Channel	:12;
    unsigned Route		:2;
    unsigned Valid		:1;
    unsigned Reserved	:1;
}  TTTRrecordType;


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
