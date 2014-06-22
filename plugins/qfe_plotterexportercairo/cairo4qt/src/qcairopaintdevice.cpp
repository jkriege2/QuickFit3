#include "qcairopaintdevice.h"
#include "qpaintengine_cairo_p.h"
#include <QDebug>

QCairoPaintDevice::QCairoPaintDevice(const QSizeF &size, const QString &fileName, CairoFileType filetype):
    QPaintDevice()
{
    qDebug()<<"QCairoPaintDevice::QCairoPaintDevice"<<engine;
    m_fileName=fileName;
    m_size=size;
    m_filetype=filetype;
    engine = new QCairoPaintEngine();
}

QCairoPaintDevice::~QCairoPaintDevice()
{
    //delete engine;
}

QPaintEngine *QCairoPaintDevice::paintEngine() const
{
    return engine;
}

QCairoPaintDevice::CairoFileType QCairoPaintDevice::getFileType() const
{
    return m_filetype;
}

QString QCairoPaintDevice::getFileName() const
{
    return m_fileName;
}

QSizeF QCairoPaintDevice::getFileSize() const
{
    return QSizeF(metric(QPaintDevice::PdmWidth), metric(QPaintDevice::PdmHeight));
}

QSizeF QCairoPaintDevice::getFileSizeMM() const
{
    return QSizeF(metric(QPaintDevice::PdmWidthMM), metric(QPaintDevice::PdmHeightMM));
}

int QCairoPaintDevice::metric(QPaintDevice::PaintDeviceMetric metric) const
{
    int xdpi = 72;
    int ydpi = 72;

    switch (metric){
        case QPaintDevice::PdmWidth:
            return m_size.width()/25.4*xdpi;
        case QPaintDevice::PdmHeight:
            return m_size.height()/25.4*ydpi;
        case QPaintDevice::PdmWidthMM:
            return m_size.width();
        case QPaintDevice::PdmHeightMM:
            return m_size.height();
        case QPaintDevice::PdmNumColors:
            return 65536;//should it be millions?
        case QPaintDevice::PdmDepth:
            return 32;
        case QPaintDevice::PdmDpiX:
        case QPaintDevice::PdmPhysicalDpiX:
            return xdpi;
        case QPaintDevice::PdmDpiY:
        case QPaintDevice::PdmPhysicalDpiY:
            return ydpi;
        default:
            qWarning ("QCairoPaintDevice::Strange metric asked");
            return 0;
    }
}
