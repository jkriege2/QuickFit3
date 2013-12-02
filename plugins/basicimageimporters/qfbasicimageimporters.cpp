#include "qfbasicimageimporters.h"
#include "qfimagereaderradhard.h"
#include "qfbasicimageimportersoptionswidget.h"
#include "qfimagereaderdiffusion4binary.h"


QStringList QFBasicImageImporters::getIDs() const {
    QStringList res;
    res<<"imageimporter_libtiff";
    res<<"imageimporter_tinytiff";
    res<<"imageimporter_radhard";
    res<<"imageimporter_diffusion4btsbinary";
    return res;
}

QFImporter* QFBasicImageImporters::createImporter(QString id) const  {
    if (id=="imageimporter_libtiff") {
        return new QFImageReaderLIBTIFF();
    } 
    if (id=="imageimporter_tinytiff") {
        return new QFImageReaderTinyTIFF();
    }
    if (id=="imageimporter_radhard") {
        return new QFImageReaderRadhard();
    }
    if (id=="imageimporter_diffusion4btsbinary") {
        return new QFImageReaderDiffusion4Binary();
    }
    return NULL;
}

QString QFBasicImageImporters::pluginOptionsName() const
{
    return getName();
}

QIcon QFBasicImageImporters::pluginOptionsIcon() const
{
    return QIcon(QIcon(":/importers_basicimages/icon_large.png"));
}

QFPluginOptionsWidget *QFBasicImageImporters::createOptionsWidget(QWidget *parent)
{
    return new QFBasicImageImportersOptionsWidget(this, parent);
}

QFBasicImageImporters::QFBasicImageImporters(QObject *parent)
{
    QFPluginServices::getInstance()->registerSettingsPane(this);
}

Q_EXPORT_PLUGIN2(importers_basicimages, QFBasicImageImporters)
