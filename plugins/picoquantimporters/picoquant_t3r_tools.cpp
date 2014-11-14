
#include "picoquant_t3r_tools.h"
#include <QObject>
#include "qftools.h"

bool TTTRReadConfiguration(FILE* fpin, TTTRTxtHdr* txtHeader, TTTRBinHdr* binHeader, TTTRBoardHdr* boardHeader, TTTRHdr* TTTRHeader, QString &error) {
    long int result;

    error="";

    result = fread( txtHeader, 1, sizeof(TTTRTxtHdr) ,fpin);
    if (result!= sizeof(TTTRTxtHdr)) {
        error=QObject::tr("error reading input file.");
        return false;
    }

    if(strncmp(txtHeader->SoftwareVersion,"5.",2)) {
        error=QObject::tr("Input file version is %1. This program is for v5.x only").arg(txtHeader->SoftwareVersion);
        return false;
    }

    result = fread( binHeader, 1, sizeof(TTTRBinHdr) ,fpin);
    if (result!= sizeof(TTTRBinHdr)) {
        error="error reading input file.";
        return false;
    }


    if (binHeader->NumberOfBoards != 1) {
        //error=QObject::tr("number of boards must be 1, but is %1.").arg(binHeader->NumberOfBoards);
        //return false;
    }

    result = fread( boardHeader, 1, sizeof(TTTRBoardHdr) ,fpin);
    if (result!= sizeof(TTTRBoardHdr)) {
        error=QObject::tr("error reading input file");
        return false;
    }

    result = fread( TTTRHeader, 1, sizeof(TTTRHdr) ,fpin);
    if (result!= sizeof(TTTRHdr)) {
        error=QObject::tr("error reading input file");
        return false;
    }

    /* v5.0: skip the special header (you may need to read it if you
       want to interpret an imaging file */
    fseek(fpin,TTTRHeader->SpecialHeaderSize*4,SEEK_CUR);

    return true;
};
