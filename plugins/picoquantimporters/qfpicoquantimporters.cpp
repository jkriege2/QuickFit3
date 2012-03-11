#include "qfpicoquantimporters.h"

QStringList QFPicoQuantImporters::getIDs() const {
    QStringList res;
    res<<"tcspcimporter_picoquant";
    return res;
}

QFImporter* QFPicoQuantImporters::createImporter(QString id) const  {
    if (id=="tcspcimporter_picoquant") {
        return new QFTCSPCReaderPicoquant();
    } 
    return NULL;
}

Q_EXPORT_PLUGIN2(importers_picoquant, QFPicoQuantImporters)
