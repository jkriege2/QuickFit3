#include "picoquant_pt3_tools.h"
#include <QObject>
#include <QDebug>

bool PT3ReadConfiguration(FILE* fpin, PT3TxtHdr* txtHeader, PT3BinHdr* binHeader, PT3BoardHdr* boardHeader, PT3TTTRHdr* TTTRHeader, QString& error) {
    long int result;

    error="";

    result = fread( txtHeader, 1, sizeof(PT3TxtHdr) ,fpin);
    if (result!= sizeof(PT3TxtHdr)) {
        error=QObject::tr("error reading input file.");
        return false;
    }

    //qDebug()<<"Format version: "<<txtHeader->FormatVersion;
    //qDebug()<<"creating software: "<<txtHeader->CreatorName;
    //qDebug()<<"creating software version: "<<txtHeader->CreatorVersion;

    if(strncmp(txtHeader->Ident,"PicoHarp 300",3)) {
        error=QObject::tr("Input file is not a PicoHarp 300 file ");
        return false;
    }

    if(strncmp(txtHeader->FormatVersion,"2.0",3)) {
        error=QObject::tr("Input file version is %1. This program is for v2.0 only").arg(txtHeader->FormatVersion);
        return false;
    }

    result = fread( binHeader, 1, sizeof(PT3BinHdr) ,fpin);
    if (result!= sizeof(PT3BinHdr)) {
        error="error reading input file.";
        return false;
    }


    if (binHeader->MeasMode  != PT3MEASMODE_T3) {
        error=QObject::tr("wrong measurement mode: This plugin may only process T3 data");
        return false;
    }

    result = fread( boardHeader, 1, sizeof(PT3BoardHdr) ,fpin);
    if (result!= sizeof(PT3BoardHdr)) {
        error=QObject::tr("error reading input file");
        return false;
    }

    result = fread( TTTRHeader, 1, sizeof(PT3TTTRHdr) ,fpin);
    if (result!= sizeof(PT3TTTRHdr)) {
        error=QObject::tr("error reading input file");
        return false;
    }

     //double syncperiod = (double)1E9 / PT3TTTRHdr.CntRate0; //in nanoseconds

    /* v5.0: skip the special header (you may need to read it if you
       want to interpret an imaging file */
    fseek(fpin,TTTRHeader->ImgHdrSize*4,SEEK_CUR);

    return true;
}
