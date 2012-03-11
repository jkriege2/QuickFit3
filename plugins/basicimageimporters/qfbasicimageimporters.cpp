#include "qfbasicimageimporters.h"
#include "qfimagereaderradhard.h"

QStringList QFBasicImageImporters::getIDs() const {
    QStringList res;
    res<<"imageimporter_libtiff";
    res<<"imageimporter_radhard";
    return res;
}

QFImporter* QFBasicImageImporters::createImporter(QString id) const  {
    if (id=="imageimporter_libtiff") {
        return new QFImageReaderLIBTIFF();
    } 
    if (id=="imageimporter_radhard") {
        return new QFImageReaderRadhard();
    }
    return NULL;
}

Q_EXPORT_PLUGIN2(importers_basicimages, QFBasicImageImporters)
