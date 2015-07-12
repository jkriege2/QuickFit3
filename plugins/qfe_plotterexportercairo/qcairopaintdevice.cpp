/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "qcairopaintdevice.h"
#include "qcairopaintengine.h"
#include <QDebug>

QCairoPaintDevice::QCairoPaintDevice(const QSizeF &size, const QString &fileName, CairoFileType filetype, bool exportText):
    QPaintDevice()
{
    //qDebug()<<"QCairoPaintDevice::QCairoPaintDevice"<<engine;
    m_fileName=fileName;
    m_size=size;
    m_filetype=filetype;
    engine = new QCairoPaintEngine();
    m_exportText=exportText;
}

QCairoPaintDevice::~QCairoPaintDevice()
{
    delete engine;
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

bool QCairoPaintDevice::getExportText() const
{
    return m_exportText;
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
