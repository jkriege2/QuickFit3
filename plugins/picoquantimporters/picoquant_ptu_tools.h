#ifndef PICOQUANT_PTU_TOOLS_H
#define PICOQUANT_PTU_TOOLS_H

//************************************************************************
//
// PicoQuant Unified TTTR (PTU)    File Access Demo in C/C++
//
// This is demo code. Use at your own risk. No warranties.
//
// Tested with MS Visual Studio 2010 and Mingw 4.5
//
// Marcus Sackrow, PicoQuant GmbH, December 2013
//
//
//************************************************************************
#include <QDebug>
#include  <stdio.h>
#include  <stddef.h>
#include  <stdlib.h>
#include  <time.h>
#include <QString>
#include <QMap>
#include <QVariant>
#include <QObject>
#include "qftcspcreader.h"

// some important Tag Idents (TTagHead.Ident) that we will need to read the most common content of a PTU file
// check the output of this program and consult the tag dictionary if you need more
#define TTTRTagTTTRRecType "TTResultFormat_TTTRRecType"
#define TTTRTagNumRecords  "TTResult_NumberOfRecords"  // Number of TTTR Records in the File;
#define TTTRTagRes         "MeasDesc_Resolution"       // Resolution for the Dtime (T3 Only)
#define TTTRTagGlobRes     "MeasDesc_GlobalResolution" // Global Resolution of TimeTag(T2) /NSync (T3)
#define FileTagEnd         "Header_End"                // Always appended as last tag (BLOCKEND)

// TagTypes  (TTagHead.Typ)
#define tyEmpty8      0xFFFF0008
#define tyBool8       0x00000008
#define tyInt8        0x10000008
#define tyBitSet64    0x11000008
#define tyColor8      0x12000008
#define tyFloat8      0x20000008
#define tyTDateTime   0x21000008
#define tyFloat8Array 0x2001FFFF
#define tyAnsiString  0x4001FFFF
#define tyWideString  0x4002FFFF
#define tyBinaryBlob  0xFFFFFFFF

// RecordTypes
#define rtPicoHarpT3     0x00010303    // (SubID = $00 ,RecFmt: $01) (V1), T-Mode: $03 (T3), HW: $03 (PicoHarp)
#define rtPicoHarpT2     0x00010203    // (SubID = $00 ,RecFmt: $01) (V1), T-Mode: $02 (T2), HW: $03 (PicoHarp)
#define rtHydraHarpT3    0x00010304    // (SubID = $00 ,RecFmt: $01) (V1), T-Mode: $03 (T3), HW: $04 (HydraHarp)
#define rtHydraHarpT2    0x00010204    // (SubID = $00 ,RecFmt: $01) (V1), T-Mode: $02 (T2), HW: $04 (HydraHarp)
#define rtHydraHarp2T3   0x01010304    // (SubID = $01 ,RecFmt: $01) (V2), T-Mode: $03 (T3), HW: $04 (HydraHarp)
#define rtHydraHarp2T2   0x01010204    // (SubID = $01 ,RecFmt: $01) (V2), T-Mode: $02 (T2), HW: $04 (HydraHarp)
#define rtTimeHarp260NT3 0x00010305    // (SubID = $00 ,RecFmt: $01) (V2), T-Mode: $03 (T3), HW: $05 (TimeHarp260N)
#define rtTimeHarp260NT2 0x00010205    // (SubID = $00 ,RecFmt: $01) (V2), T-Mode: $02 (T2), HW: $05 (TimeHarp260N)
#define rtTimeHarp260PT3 0x00010306    // (SubID = $00 ,RecFmt: $01) (V1), T-Mode: $02 (T3), HW: $06 (TimeHarp260P)
#define rtTimeHarp260PT2 0x00010206    // (SubID = $00 ,RecFmt: $01) (V1), T-Mode: $02 (T2), HW: $06 (TimeHarp260P)

#pragma pack(8) //structure alignment to 8 byte boundaries




time_t TDateTime_TimeT(double Convertee);



struct PTUInfo {
   QString version;
   QList< QPair<QString, QVariant> > header_data ;
   long long NumRecords;
   long long RecordType;
   double GlobRes;
   double Resolution;
   QString device;
   QString comment;
   double trueTimeToSecond;
   int channels;
   double duration;
   bool IsT2;
};


// procedures for Photon, overflow, marker

/*
 * //Got Photon
    //  TimeTag: Raw TimeTag from Record * Globalresolution = Real Time arrival of Photon
    //  DTime: Arrival time of Photon after last Sync event (T3 only) DTime * Resolution = Real time arrival of Photon after last Sync event
    //  Channel: Channel the Photon arrived (0 = Sync channel for T2 measurements)
    void GotPhoton(long long TimeTag, int Channel, int DTime)
    {
      if(IsT2)
      {
          fprintf(fpout,"%I64u CHN %1x %I64u %8.0lf\n", RecNum, Channel, TimeTag, (TimeTag * GlobRes * 1e12));
      }
      else
      {
        fprintf(fpout,"%I64u CHN %1x %I64u %8.0lf %10u\n", RecNum, Channel, TimeTag, (TimeTag * GlobRes * 1e9), DTime);
      }
    }

    //Got Marker
    //  TimeTag: Raw TimeTag from Record * Globalresolution = Real Time arrival of Photon
    //  Markers: Bitfield of arrived Markers, different markers can arrive at same time (same record)
    void GotMarker(long long TimeTag, int Markers)
    {
      fprintf(fpout,"%I64u MAR %2x %I64u\n", RecNum, Markers, TimeTag);
    }

    //Got Overflow
    //  Count: Some TCSPC provide Overflow compression = if no Photons between overflow you get one record for multiple Overflows
    void GotOverflow(int Count)
    {
      fprintf(fpout,"%I64u OFL * %2x\n", RecNum, Count);
    }
 * */

// PicoHarp T3 input
/**/

inline bool ProcessPHT3(PTUInfo& ptu, const uint32_t& TTTRRecord, QFTCSPCRecord& rec, uint64_t& oflcorrection, uint64_t& overflows) {
    const int T3WRAPAROUND = 65536;
    union {
        uint32_t allbits;
        struct {
            unsigned numsync  :16;
            unsigned dtime    :12;
            unsigned channel  :4;
        } bits;
        struct {
            unsigned numsync  :16;
            unsigned markers  :12;
            unsigned channel  :4;
        } special;
    } Record;

    rec.isPhoton=false;
    rec.input_channel=0;

    Record.allbits = TTTRRecord;
    if(Record.bits.channel==0xF) { //this means we have a special record
      if(Record.special.markers==0) { //not a marker means overflow
          //GotOverflow(1);
          oflcorrection += T3WRAPAROUND; // unwrap the time tag overflow
          overflows++;
          return false;
      } else {
          rec.macrotime = double(oflcorrection + Record.bits.numsync)*ptu.trueTimeToSecond;
          rec.marker_type=Record.special.markers;
          rec.isPhoton=false;
          return true;
        //GotMarker(truensync, m);
      }
    } else {
      if(
          (Record.bits.channel==0) //Should never occur in T3 Mode
        ||(Record.bits.channel>4) //Should not occur with current routers
        )
      {
        return false;
        //error=QObject::tr("Illegal Channel: #%1d %1u",dlen,Record.bits.channel);
        //fprintf(fpout,"\nillegal channel ");
      }

      rec.macrotime = double(oflcorrection + Record.bits.numsync)*ptu.trueTimeToSecond;
      rec.microtime_channel = Record.bits.dtime;
      rec.input_channel = Record.bits.channel;
      rec.isPhoton=true;
      //qDebug()<<"got photon "<<rec.input_channel<<rec.macrotime;
      return true;
      //GotPhoton(truensync, c, m);
      //dlen++;
    }
    return false;
}

inline bool ProcessPHT2(PTUInfo& ptu, const uint32_t& TTTRRecord, QFTCSPCRecord& rec, uint64_t& oflcorrection, uint64_t& overflows) {
    //return false;


    const int T2WRAPAROUND = 210698240;
    union {
        unsigned int allbits;
        struct {
            unsigned time   :28;
            unsigned channel  :4;
        } bits;
    } Record;
    unsigned int markers;
    Record.allbits = TTTRRecord;

    rec.isPhoton=false;
    rec.input_channel=0;

    if(Record.bits.channel==0xF) //this means we have a special record
    {
        //in a special record the lower 4 bits of time are marker bits
        markers=Record.bits.time&0xF;
        if(markers==0) //this means we have an overflow record
        {
            oflcorrection += T2WRAPAROUND; // unwrap the time tag overflow
            overflows++;
            return false;
        }
        else //a marker
        {
            //Strictly, in case of a marker, the lower 4 bits of time are invalid
            //because they carry the marker bits. So one could zero them out.
            //However, the marker resolution is only a few tens of nanoseconds anyway,
            //so we can just ignore the few picoseconds of error.
            rec.macrotime = double(oflcorrection + Record.bits.time)*ptu.trueTimeToSecond;
            rec.marker_type=Record.bits.time&0xF;
            rec.isPhoton=false;
            return true;
                  //GotMarker(truetime, markers);
        }
    } else {
        if((int)Record.bits.channel > 4) { //Should not occur
            //printf(" Illegal Chan: #%I64u %1u\n",RecNum,Record.bits.channel);
            //fprintf(fpout," illegal chan.\n");
            return false;
        } else {
            /*if(Record.bits.channel==0) cnt_0++;
            if(Record.bits.channel>=1) cnt_1++;*/

            rec.macrotime = double(oflcorrection + Record.bits.time)*ptu.trueTimeToSecond;
            rec.microtime_channel = 0;
            rec.input_channel = Record.bits.channel;
            rec.isPhoton=true;
            return true;
            //qDebug()<<"got photon "<<rec.input_channel<<rec.macrotime;
            // GotPhoton(truetime, c, m);
        }
    }
    return false;
}

inline bool ProcessHHT2(PTUInfo& ptu, const uint32_t& TTTRRecord, QFTCSPCRecord& rec, uint64_t& oflcorrection, uint64_t& overflows, int HHVersion) {
    const int T2WRAPAROUND_V1 = 33552000;
    const int T2WRAPAROUND_V2 = 33554432;
    union{
        uint32_t   allbits;
        struct{
                unsigned timetag  :25;
            unsigned channel  :6;
            unsigned special  :1; // or sync, if channel==0
        } bits;
    } T2Rec;
    T2Rec.allbits = TTTRRecord;

    rec.isPhoton=false;
    rec.input_channel=0;

    if(T2Rec.bits.special==1) {
        if(T2Rec.bits.channel==0x3F) //an overflow record
        {
            if(HHVersion == 1)
            {
                oflcorrection += (uint64_t)T2WRAPAROUND_V1;
                overflows++;
                return false;
            } else {
                //number of overflows is stored in timetag
                if(T2Rec.bits.timetag==0) //if it is zero it is an old style single overflow
                {
                    oflcorrection += (uint64_t)T2WRAPAROUND_V2;  //should never happen with new Firmware!
                    overflows++;
                    return false;
                }
                else
                {
                    oflcorrection += (uint64_t)T2WRAPAROUND_V2 * T2Rec.bits.timetag;
                    overflows++;
                    return false;
                }
            }
        }

        if((T2Rec.bits.channel>=1)&&(T2Rec.bits.channel<=15)) //markers
        {
            //Note that actual marker tagging accuracy is only some ns.
            rec.macrotime = double(oflcorrection + T2Rec.bits.timetag)*ptu.trueTimeToSecond;
            rec.marker_type=T2Rec.bits.channel;
            rec.isPhoton=false;
            return true;
            //GotMarker(truetime, m);
        }

        if(T2Rec.bits.channel==0) //sync
        {
            //truetime = oflcorrection + T2Rec.bits.timetag;
            //GotPhoton(truetime, 0, 0);
            rec.macrotime = double(oflcorrection + T2Rec.bits.timetag)*ptu.trueTimeToSecond;
            rec.microtime_channel = 0;
            rec.input_channel = 0;
            rec.isPhoton=true;
            return true;
        }
    }
    else //regular input channel
    {
        //GotPhoton(truetime, c, 0);
        rec.macrotime = double(oflcorrection + T2Rec.bits.timetag)*ptu.trueTimeToSecond;
        rec.microtime_channel = 0;
        rec.input_channel = T2Rec.bits.channel + 1;
        rec.isPhoton=true;
        return true;
    }
    return false;

}

inline bool ProcessHHT3(PTUInfo& ptu, const uint32_t& TTTRRecord, QFTCSPCRecord& rec, uint64_t& oflcorrection, uint64_t& overflows, int HHVersion) {
    const int T3WRAPAROUND = 1024;
    union {
        uint32_t allbits;
        struct  {
            unsigned nsync    :10;  // numer of sync period
            unsigned dtime    :15;    // delay from last sync in units of chosen resolution
            unsigned channel  :6;
            unsigned special  :1;
        } bits;
    } T3Rec;
    T3Rec.allbits = TTTRRecord;

    rec.isPhoton=false;
    rec.input_channel=0;

    if(T3Rec.bits.special==1)
    {
        if(T3Rec.bits.channel==0x3F) //overflow
        {
            //number of overflows is stored in nsync
            if((T3Rec.bits.nsync==0) || (HHVersion==1)) //if it is zero or old version it is an old style single oferflow
            {
                oflcorrection += (uint64_t)T3WRAPAROUND;
                overflows++;
                return false;
            }
            else
            {
                oflcorrection += (uint64_t)T3WRAPAROUND * T3Rec.bits.nsync;
                overflows++;
                return false;
            }
        }
        if((T3Rec.bits.channel>=1)&&(T3Rec.bits.channel<=15)) //markers
        {
            //the time unit depends on sync period which can be obtained from the file header
            rec.macrotime = double(oflcorrection + T3Rec.bits.nsync)*ptu.trueTimeToSecond;
            rec.marker_type=T3Rec.bits.channel;
            rec.isPhoton=false;
            return true;
        }
    }
    else //regular input channel
    {
        //the nsync time unit depends on sync period which can be obtained from the file header
        //the dtime unit depends on the resolution and can also be obtained from the file header
        rec.macrotime = double(oflcorrection + T3Rec.bits.nsync)*ptu.trueTimeToSecond;
        rec.microtime_channel = T3Rec.bits.dtime;
        rec.input_channel = T3Rec.bits.channel;
        rec.isPhoton=true;
        return true;
    }
    return false;
}




bool PTUReadConfiguration(FILE* fpin, PTUInfo &info, QString &error);


#endif // PICOQUANT_PTU_TOOLS_H
