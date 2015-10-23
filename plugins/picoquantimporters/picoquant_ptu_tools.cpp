#include <QtCore>
#include "picoquant_ptu_tools.h"
#include <wchar.h>
#include "qftools.h"
#include <stdint.h>
const int32_t EpochDiff = 25569; // days between 30/12/1899 and 01/01/1970
const int32_t SecsInDay = 86400; // number of seconds in a day

union TgPTUTagHeadValue {
    int64_t integ;
    double dbl;
};

// A Tag entry
struct TgPTUTagHead{
  int8_t Ident[32];     // Identifier of the tag
  int32_t Idx;            // Index for multiple tags or -1
  uint32_t Typ;  // Type of tag ty..... see const section
  TgPTUTagHeadValue TagValue; // Value of tag.
} PTUTagHead;

time_t TDateTime_TimeT(double Convertee)
{
  time_t Result((long)(((Convertee) - EpochDiff) * SecsInDay));
  return Result;
}


#ifndef WCHAR
#define WCHAR wchar_t
#endif

bool PTUReadConfiguration(FILE* fpin, PTUInfo &info, QString &error) {

    info.channels=4;
    info.duration=0;
    error="";
    int8_t Magic[8];
    int8_t Version[8];
    //char Buffer[40];
    int8_t* AnsiBuffer;
    WCHAR* WideBuffer;
    int Result;

    long long NumRecords = -1;
    long long RecordType = 0;
    double GlobRes = 0.0;
    double Resolution = 0.0;


    Result = fread( &Magic, 1, sizeof(Magic) ,fpin);
    if (Result!= sizeof(Magic))
    {
      error=QObject::tr("error reading header, aborted.");
      return false;
    }
    Result = fread(&Version, 1, sizeof(Version) ,fpin);
    if (Result!= sizeof(Version))
      {
      error=QObject::tr("error reading header, aborted.");
      return false;
    }
    if (strncmp((char*)Magic, "PQTTTR", 6))
    {
      error=QObject::tr("Wrong Magic, this is not a PTU file.");
      return false;
    }

    info.version=(char*)Version;
    //fprintf(fpout, "Tag Version: %s \n", Version);

    // read tagged header
    do
    {
      // This loop is very generic. It reads all header items and displays the identifier and the
      // associated value, quite independent of what they mean in detail.
      // Only some selected items are explicitly retrieved and kept in memory because they are
      // needed to subsequently interpret the TTTR record data.

      Result = fread( &PTUTagHead, 1, sizeof(PTUTagHead) ,fpin);
      if (Result!= sizeof(PTUTagHead))
      {
          error=QObject::tr("Incomplete File.");
          return false;
      }

      QString tagID=(char*)PTUTagHead.Ident;
      QVariant tagData;

      //strcpy(Buffer, PTUTagHead.Ident);
      /*if (PTUTagHead.Idx > -1)
      {
        sprintf(Buffer, "%s(%d)", PTUTagHead.Ident,PTUTagHead.Idx);
      }
      fprintf(fpout, "\n%-40s", Buffer);*/
      switch (PTUTagHead.Typ) {
          case tyEmpty8:
              //fprintf(fpout, "<empty Tag>");
              break;
          case tyBool8:
              tagData=bool(PTUTagHead.TagValue.integ);
              break;
          case tyInt8:
              tagData=int(PTUTagHead.TagValue.integ);
              // get some Values we need to analyse records
              if (strcmp((char*)PTUTagHead.Ident, TTTRTagNumRecords)==0) // Number of records
                          NumRecords = PTUTagHead.TagValue.integ;
              if (strcmp((char*)PTUTagHead.Ident, TTTRTagTTTRRecType)==0) // TTTR RecordType
                          RecordType = PTUTagHead.TagValue.integ;
              break;
          case tyBitSet64:
              tagData=QVariant((qint64)PTUTagHead.TagValue.integ);
              break;
          case tyColor8:
              tagData=QVariant((qint64)PTUTagHead.TagValue.integ);
              break;
          case tyFloat8:
              tagData=PTUTagHead.TagValue.dbl;
              if (strcmp((char*)PTUTagHead.Ident, TTTRTagRes)==0) // Resolution for TCSPC-Decay
                      Resolution = PTUTagHead.TagValue.dbl;
              if (strcmp((char*)PTUTagHead.Ident, TTTRTagGlobRes)==0) // Global resolution for timetag
                      GlobRes = PTUTagHead.TagValue.dbl; // in ns
              break;
          case tyFloat8Array: {
              double* buf=(double*)qfMalloc(PTUTagHead.TagValue.integ);
              fread(buf, PTUTagHead.TagValue.integ, 1, fpin);
              QVariantList vl;
              for ( long long i=0; i<PTUTagHead.TagValue.integ/(int)sizeof(double); i++) {
                  vl<<buf[i];
              }
              tagData=vl;
              //fprintf(fpout, "<Float Array with %d Entries>", PTUTagHead.TagValue / sizeof(double));
              // only seek the Data, if one needs the data, it can be loaded here
              //fseek(fpin, (long)PTUTagHead.TagValue, SEEK_CUR);
          } break;
          case tyTDateTime:
              time_t CreateTime;
              CreateTime = TDateTime_TimeT(PTUTagHead.TagValue.dbl);
              tagData=QDateTime::fromTime_t(CreateTime);
              //fprintf(fpout, "%s", asctime(gmtime(&CreateTime)), "\0");
              break;
          case tyAnsiString:
              AnsiBuffer = (int8_t*)qfCalloc((size_t)PTUTagHead.TagValue.integ,1);
              Result = fread(AnsiBuffer, 1, (size_t)PTUTagHead.TagValue.integ, fpin);
              if (Result!= PTUTagHead.TagValue.integ) {
                  error=QObject::tr("Incomplete File.");
                  qfFree(AnsiBuffer);
                  return false;
              }
              tagData=QString((char*)AnsiBuffer);
              qfFree(AnsiBuffer);
              break;
          case tyWideString:
              WideBuffer = (WCHAR*)qfCalloc((size_t)PTUTagHead.TagValue.integ,1);
              Result = fread(WideBuffer, 1, (size_t)PTUTagHead.TagValue.integ, fpin);
              if (Result!= PTUTagHead.TagValue.integ) {
                  error=QObject::tr("Incomplete File.");
                  qfFree(WideBuffer);
                  return false;
              }
              //fwprintf(fpout, L"%s", WideBuffer);
              tagData=QString::fromUtf16((ushort*)WideBuffer);
              qfFree(WideBuffer);
              break;
          case tyBinaryBlob:{
                  //fprintf(fpout, "<Binary Blob contains %d Bytes>", PTUTagHead.TagValue);
                  // only seek the Data, if one needs the data, it can be loaded here
                  QByteArray buf(PTUTagHead.TagValue.integ, '\0');
                  fread(buf.data(), buf.size(), 1, fpin);
                  tagData=buf;
                  //        fseek(fpin, (long)PTUTagHead.TagValue, SEEK_CUR);
              }
              break;
          default:
              printf("Illegal Type identifier found! Broken file?");
              return false;
      }
      if (tagID=="HWRouter_Channels" && tagData.toInt()>0) info.channels=tagData.toInt();
      else if (tagID=="MeasDesc_AcquisitionTime" && tagData.toDouble()>0) info.duration=tagData.toDouble()/1000.0;
      else if (tagID=="File_Comment" ) info.comment=tagData.toString();
      info.header_data.append(qMakePair(tagID, tagData));
    }
    while((strncmp((char*)PTUTagHead.Ident, FileTagEnd, sizeof(FileTagEnd))));

    info.NumRecords=NumRecords;
    info.RecordType=RecordType;
    info.GlobRes=GlobRes;
    info.Resolution=Resolution;

  // End Header loading

    // TTTR Record type
    switch (RecordType)
    {
      case rtPicoHarpT2:
        info.device=QString("PicoHarp T2");
        info.trueTimeToSecond=info.GlobRes;//*1e-12;
        //fprintf(fpout,"\nrecord# chan   nsync truetime/ps\n");
        break;
      case rtPicoHarpT3:
        info.device=QString("PicoHarp T3");
        info.trueTimeToSecond=info.GlobRes;//*1e-9;
        //fprintf(fpout,"\nrecord# chan   nsync truetime/ns dtime");
        break;
      case rtHydraHarpT2:
        info.device=QString("HydraHarp V1 T2");
        info.trueTimeToSecond=info.GlobRes;//*1e-12;
        //fprintf(fpout,"\nrecord# chan   nsync truetime/ps\n");
        break;
      case rtHydraHarpT3:
        info.device=QString("HydraHarp V1 T3");
        info.trueTimeToSecond=info.GlobRes*1e9;
        //fprintf(fpout,"\nrecord# chan   nsync truetime/ns dtime\n");
        break;
      case rtHydraHarp2T2:
        info.device=QString("HydraHarp V2 T2");
        info.trueTimeToSecond=info.GlobRes;//*1e-12;
        //fprintf(fpout,"\nrecord# chan   nsync truetime/ps\n");
        break;
      case rtHydraHarp2T3:
        info.device=QString("HydraHarp V2 T3");
        info.trueTimeToSecond=info.GlobRes;//*1e-9;
        //fprintf(fpout,"\nrecord# chan   nsync truetime/ns dtime\n");
        break;
     case rtTimeHarp260NT3:
        info.device=QString("TimeHarp260N T3");
        info.trueTimeToSecond=info.GlobRes;//*1e-9;
        //fprintf(fpout,"\nrecord# chan   nsync truetime/ns dtime\n");
        break;
     case rtTimeHarp260NT2:
        info.device=QString("TimeHarp260N T2");
        info.trueTimeToSecond=info.GlobRes;//*1e-12;
        //fprintf(fpout,"\nrecord# chan   nsync truetime/ps\n");
        break;
      case rtTimeHarp260PT3:
        info.device=QString("TimeHarp260P T3");
        info.trueTimeToSecond=info.GlobRes;//*1e-9;
        //fprintf(fpout,"\nrecord# chan   nsync truetime/ns dtime\n");
        break;
     case rtTimeHarp260PT2:
        info.device=QString("TimeHarp260P T2");
        info.trueTimeToSecond=info.GlobRes;//*1e-12;
        //fprintf(fpout,"\nrecord# chan   nsync truetime/ps\n");
        break;
      default:
        error=QObject::tr("Unknown record type: 0x%1").arg(RecordType,0,16,QChar('0'));
        return false;
    }

    return true;
}

