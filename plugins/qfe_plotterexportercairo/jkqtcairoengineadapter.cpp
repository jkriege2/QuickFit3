/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

    last modification: $LastChangedDate$  (revision $Rev$)

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


#include "jkqtcairoengineadapter.h"
#include "qcairopaintdevice.h"

JKQTPCairoEngineAdapter::JKQTPCairoEngineAdapter(Format format)
{
    this->format=format;
}

QString JKQTPCairoEngineAdapter::getFilter() const
{
    switch(format) {
        case formatEPS: return QObject::tr("Cairo encapsuled PostScript (*.eps)");
        case formatPS: return QObject::tr("Cairo PostScript (*.ps)");
        case formatSVG: return QObject::tr("Cairo SVG (*.svg)");
        default:
        case formatPDF: return QObject::tr("Cairo PDF (*.pdf)");
    }
}

QString JKQTPCairoEngineAdapter::getFormatName() const
{
    switch(format) {
        case formatEPS: return QObject::tr("Cairo encapsuled PostScript");
        case formatPS: return QObject::tr("Cairo PostScript");
        case formatSVG: return QObject::tr("Cairo SVG");
        default:
        case formatPDF: return QObject::tr("Cairo PDF");
    }
}

QString JKQTPCairoEngineAdapter::getFormatID() const
{
    switch(format) {
        case formatEPS: return QString("CairoEPS");
        case formatPS: return QString("CairoPS");
        case formatSVG: return QString("CairoSVG");
        default:
        case formatPDF: return QString("CairoPDF");
    }
}

QStringList JKQTPCairoEngineAdapter::getFileExtension() const
{
    switch(format) {
        case formatEPS: return QStringList("eps");
        case formatPS: return QStringList("ps");
        case formatSVG: return QStringList("svg");
        default:
        case formatPDF: return QStringList("pdf");
    }
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
    //qDebug()<<"create Cairo-device for "<<filename<<QSize(widthPix, heightPix);
    QCairoPaintDevice * cairo=NULL;
    switch(format) {
        case formatEPS:
            cairo=new QCairoPaintDevice(QSize(ceil(double(widthPix)/96.0*25.4), ceil(double(heightPix)/96.0*25.4)), filename, QCairoPaintDevice::cftEPS);
            break;
        case formatPS:
            cairo=new QCairoPaintDevice(QSize(ceil(double(widthPix)/96.0*25.4), ceil(double(heightPix)/96.0*25.4)), filename, QCairoPaintDevice::cftPS);
            break;
        case formatSVG:
            cairo=new QCairoPaintDevice(QSize(ceil(double(widthPix)/96.0*25.4), ceil(double(heightPix)/96.0*25.4)), filename, QCairoPaintDevice::cftSVG);
            break;
        default:
        case formatPDF:
            cairo=new QCairoPaintDevice(QSize(ceil(double(widthPix)/96.0*25.4), ceil(double(heightPix)/96.0*25.4)), filename, QCairoPaintDevice::cftPDF);
            break;
    }

    return cairo;
    //return NULL;
}
