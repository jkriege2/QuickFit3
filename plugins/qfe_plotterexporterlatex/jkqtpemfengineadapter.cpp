#include "jkqtpemfengineadapter.h"


JKQTPEMFEngineAdapter::JKQTPEMFEngineAdapter()
{
    id="EMF";
}

QString JKQTPEMFEngineAdapter::getFilter() const
{

    return QObject::tr("Enhanced Metafile (*.emf)");
}

QString JKQTPEMFEngineAdapter::getFormatName() const
{
    return id;
}

QString JKQTPEMFEngineAdapter::getFormatID() const
{
    return id;
}

QStringList JKQTPEMFEngineAdapter::getFileExtension() const
{
    return QStringList("emf");
}

bool JKQTPEMFEngineAdapter::getSetAbsolutePaperSize() const
{
    return false;
}

double JKQTPEMFEngineAdapter::getPrintSizeXInMM() const
{
    return 2000;
}

double JKQTPEMFEngineAdapter::getPrintSizeYInMM() const
{
    return 1000;
}

bool JKQTPEMFEngineAdapter::isPrinter() const
{
    return false;
}

QPaintDevice *JKQTPEMFEngineAdapter::createPaintdevice(const QString &filename, int widthPix, int heightPix) const
{
    //qDebug()<<"create EMF-device for "<<filename<<QSize(widthPix, heightPix);
    EmfPaintDevice * emf=new EmfPaintDevice(QSize(widthPix, heightPix), filename);
    return emf;
}
