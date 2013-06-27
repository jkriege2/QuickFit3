#include "qfsimpletcspcimporterplugin.h"
#include "qfsimpletcspcimporter.h"


QStringList QFSimpleTCSPCImporterPlugin::getIDs() const {
    QStringList res;
    res<<"tcspcimporter_simpletcspc";
    return res;
}

QFImporter* QFSimpleTCSPCImporterPlugin::createImporter(QString id) const  {
    if (id=="tcspcimporter_simpletcspc") {
        return new QFSimpleTCSPCImporter();
    } 
    return NULL;
}

Q_EXPORT_PLUGIN2(importers_simpletcspcimporter, QFSimpleTCSPCImporterPlugin)
