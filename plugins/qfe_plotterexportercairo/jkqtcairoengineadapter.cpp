#include "jkqtcairoengineadapter.h"
#include "cairo4qt/src/qpaintengine_cairo_p.h"
#include "cairo4qt/src/qcairopaintdevice.h"

JKQTPCairoEngineAdapter::JKQTPCairoEngineAdapter()
{
    id="CAIRO_PDF";
}

QString JKQTPCairoEngineAdapter::getFilter() const
{
    return QObject::tr("Cairo PDF (*.pdf)");
}

QString JKQTPCairoEngineAdapter::getFormatName() const
{
    return id;
}

QString JKQTPCairoEngineAdapter::getFormatID() const
{
    return id;
}

QStringList JKQTPCairoEngineAdapter::getFileExtension() const
{
    return QStringList("pdf");
}

bool JKQTPCairoEngineAdapter::getSetAbsolutePaperSize() const
{
    return true;
}

double JKQTPCairoEngineAdapter::getPrintSizeXInMM() const
{
    return 210;
}

double JKQTPCairoEngineAdapter::getPrintSizeYInMM() const
{
    return 297;
}

bool JKQTPCairoEngineAdapter::isPrinter() const
{
    return false;
}

QPaintDevice *JKQTPCairoEngineAdapter::createPaintdevice(const QString &filename, int widthPix, int heightPix) const
{
    qDebug()<<"create Cairo-device for "<<filename<<QSize(widthPix, heightPix);
    QCairoPaintDevice * cairo=new QCairoPaintDevice(QSize(ceil(double(widthPix)/96.0*25.4), ceil(double(heightPix)/96.0*25.4)), filename, QCairoPaintDevice::cftPDF);
    return cairo;
    //return NULL;
}
