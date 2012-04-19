#include "qfpicoquantimporters.h"
#include "qftcspcreaderpicoquant.h"
#include "qftcspcreaderpicoquantpt3.h"

QStringList QFPicoQuantImporters::getIDs() const {
    QStringList res;
    res<<"tcspcimporter_picoquant";
    res<<"tcspcimporter_picoquantpt3";
    return res;
}

QFImporter* QFPicoQuantImporters::createImporter(QString id) const  {
    if (id=="tcspcimporter_picoquant") {
        return new QFTCSPCReaderPicoquant();
    } 
    if (id=="tcspcimporter_picoquantpt3") {
        return new QFTCSPCReaderPicoquantPT3();
    }
    return NULL;
}

Q_EXPORT_PLUGIN2(importers_picoquant, QFPicoQuantImporters)
