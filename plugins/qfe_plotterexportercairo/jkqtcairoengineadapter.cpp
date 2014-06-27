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

JKQTPCairoEngineAdapter::JKQTPCairoEngineAdapter(Format format, bool exportText)
{
    this->format=format;
    this->exportText=exportText;
}

QString JKQTPCairoEngineAdapter::getFilter() const
{
    QString txt="";
    if (exportText) txt=QObject::tr("[editable text] ");
    switch(format) {
        case formatEPS: return QObject::tr("Cairo encapsuled PostScript %1(*.eps)").arg(txt);
        case formatPS: return QObject::tr("Cairo PostScript %1(*.ps)").arg(txt);
        case formatSVG: return QObject::tr("Cairo SVG %1(*.svg)").arg(txt);
        default:
        case formatPDF: return QObject::tr("Cairo PDF %1(*.pdf)").arg(txt);
    }
}

QString JKQTPCairoEngineAdapter::getFormatName() const
{
    QString txt="";
    if (exportText) txt=QObject::tr("[editable text] ");
    switch(format) {
        case formatEPS: return QObject::tr("Cairo encapsuled PostScript%1").arg(txt);
        case formatPS: return QObject::tr("Cairo PostScript%1").arg(txt);
        case formatSVG: return QObject::tr("Cairo SVG%1").arg(txt);
        default:
        case formatPDF: return QObject::tr("Cairo PDF%1").arg(txt);
    }
}

QString JKQTPCairoEngineAdapter::getFormatID() const
{
    QString txt="";
    if (exportText) txt=QString("_EditableText");
    switch(format) {
        case formatEPS: return QString("CairoEPS")+txt;
        case formatPS: return QString("CairoPS")+txt;
        case formatSVG: return QString("CairoSVG")+txt;
        default:
        case formatPDF: return QString("CairoPDF")+txt;
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
            cairo=new QCairoPaintDevice(QSize(ceil(double(widthPix)/96.0*25.4), ceil(double(heightPix)/96.0*25.4)), filename, QCairoPaintDevice::cftEPS, exportText);
            break;
        case formatPS:
            cairo=new QCairoPaintDevice(QSize(ceil(double(widthPix)/96.0*25.4), ceil(double(heightPix)/96.0*25.4)), filename, QCairoPaintDevice::cftPS, exportText);
            break;
        case formatSVG:
            cairo=new QCairoPaintDevice(QSize(ceil(double(widthPix)/96.0*25.4), ceil(double(heightPix)/96.0*25.4)), filename, QCairoPaintDevice::cftSVG, exportText);
            break;
        default:
        case formatPDF:
            cairo=new QCairoPaintDevice(QSize(ceil(double(widthPix)/96.0*25.4), ceil(double(heightPix)/96.0*25.4)), filename, QCairoPaintDevice::cftPDF, exportText);
            break;
    }

    return cairo;
    //return NULL;
}
