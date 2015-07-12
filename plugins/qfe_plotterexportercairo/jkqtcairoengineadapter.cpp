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
    else txt=QObject::tr("[text as polygons] ");
    switch(format) {
        case formatEPS2: return QObject::tr("Cairo encapsuled PostScript, level 2 %1(*.eps)").arg(txt);
        case formatEPS3: return QObject::tr("Cairo encapsuled PostScript, level 3 %1(*.eps)").arg(txt);
        case formatPS2: return QObject::tr("Cairo PostScript, level 2 %1(*.ps)").arg(txt);
        case formatPS3: return QObject::tr("Cairo PostScript, level 3 %1(*.ps)").arg(txt);
        case formatSVG11: return QObject::tr("Cairo SVG 1.1 %1(*.svg)").arg(txt);
        case formatSVG12: return QObject::tr("Cairo SVG 1.2 %1(*.svg)").arg(txt);
        case formatPDF14: return QObject::tr("Cairo PDF 1.4 %1(*.pdf)").arg(txt);
        default:
        case formatPDF15: return QObject::tr("Cairo PDF 1.5 %1(*.pdf)").arg(txt);
    }
}

QString JKQTPCairoEngineAdapter::getFormatName() const
{
    QString txt="";
    if (exportText) txt=QObject::tr("[editable text] ");
    else txt=QObject::tr("[text as polygons] ");
    switch(format) {
        case formatEPS2: return QObject::tr("Cairo encapsuled PostScript, level 2 %1").arg(txt);
        case formatEPS3: return QObject::tr("Cairo encapsuled PostScript, level 3 %1").arg(txt);
        case formatPS2: return QObject::tr("Cairo PostScript, level 2 %1").arg(txt);
        case formatPS3: return QObject::tr("Cairo PostScript, level 2 %1").arg(txt);
        case formatSVG11: return QObject::tr("Cairo 1.1 SVG %1").arg(txt);
        case formatSVG12: return QObject::tr("Cairo 1.2 SVG %1").arg(txt);
        case formatPDF14: return QObject::tr("Cairo PDF 1.4 %1").arg(txt);
        default:
        case formatPDF15: return QObject::tr("Cairo PDF 1.5 %1").arg(txt);
    }
}

QString JKQTPCairoEngineAdapter::getFormatID() const
{
    QString txt="";
    if (exportText) txt=QString("_EditableText");
    switch(format) {
        case formatEPS2:  return QString("CairoEPS2")+txt;
        case formatEPS3: return QString("CairoEPS3")+txt;
        case formatPS2: return QString("CairoPS2")+txt;
        case formatPS3: return QString("CairoPS3")+txt;
        case formatSVG11: return QString("CairoSVG11")+txt;
        case formatSVG12: return QString("CairoSVG12")+txt;
        case formatPDF14: return QString("CairoPDF14")+txt;
        default:
        case formatPDF15: return QString("CairoPDF15")+txt;
    }
}

QStringList JKQTPCairoEngineAdapter::getFileExtension() const
{
    switch(format) {
        case formatEPS2: 
		case formatEPS3: return QStringList("eps");
        case formatPS2:
        case formatPS3: return QStringList("ps");
        case formatSVG11: 
        case formatSVG12: return QStringList("svg");
        case formatPDF14: 
        default:
        case formatPDF15: return QStringList("pdf");
    }
}

bool JKQTPCairoEngineAdapter::getSetAbsolutePaperSize() const
{
    return true;
}

double JKQTPCairoEngineAdapter::getPrintSizeXInMM() const
{
    return -1;
}

double JKQTPCairoEngineAdapter::getPrintSizeYInMM() const
{
    return -1;
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
        case formatEPS2:
            cairo=new QCairoPaintDevice(QSize(ceil(double(widthPix)/96.0*25.4), ceil(double(heightPix)/96.0*25.4)), filename, QCairoPaintDevice::cftEPS2, exportText);
            break;
        case formatEPS3:
            cairo=new QCairoPaintDevice(QSize(ceil(double(widthPix)/96.0*25.4), ceil(double(heightPix)/96.0*25.4)), filename, QCairoPaintDevice::cftEPS3, exportText);
            break;
        case formatPS2:
            cairo=new QCairoPaintDevice(QSize(ceil(double(widthPix)/96.0*25.4), ceil(double(heightPix)/96.0*25.4)), filename, QCairoPaintDevice::cftPS2, exportText);
            break;
        case formatPS3:
            cairo=new QCairoPaintDevice(QSize(ceil(double(widthPix)/96.0*25.4), ceil(double(heightPix)/96.0*25.4)), filename, QCairoPaintDevice::cftPS3, exportText);
            break;
        case formatSVG11:
            cairo=new QCairoPaintDevice(QSize(ceil(double(widthPix)/96.0*25.4), ceil(double(heightPix)/96.0*25.4)), filename, QCairoPaintDevice::cftSVG11, exportText);
            break;
        case formatSVG12:
            cairo=new QCairoPaintDevice(QSize(ceil(double(widthPix)/96.0*25.4), ceil(double(heightPix)/96.0*25.4)), filename, QCairoPaintDevice::cftSVG12, exportText);
            break;
        case formatPDF14:
            cairo=new QCairoPaintDevice(QSize(ceil(double(widthPix)/96.0*25.4), ceil(double(heightPix)/96.0*25.4)), filename, QCairoPaintDevice::cftPDF14, exportText);
            break;
        default:
        case formatPDF15:
            cairo=new QCairoPaintDevice(QSize(ceil(double(widthPix)/96.0*25.4), ceil(double(heightPix)/96.0*25.4)), filename, QCairoPaintDevice::cftPDF15, exportText);
            break;
    }

    return cairo;
    //return NULL;
}

QPaintDevice *JKQTPCairoEngineAdapter::createPaintdeviceMM(const QString &filename, double widthMM, double heightMM) const
{
    QCairoPaintDevice * cairo=NULL;
    switch(format) {
        case formatEPS2:
            cairo=new QCairoPaintDevice(QSizeF(ceil(double(widthMM)*1.05), ceil(double(heightMM)*1.05)), filename, QCairoPaintDevice::cftEPS2, exportText);
            break;
        case formatEPS3:
            cairo=new QCairoPaintDevice(QSizeF(ceil(double(widthMM)*1.05), ceil(double(heightMM)*1.05)), filename, QCairoPaintDevice::cftEPS3, exportText);
            break;
        case formatPS2:
            cairo=new QCairoPaintDevice(QSizeF(ceil(double(widthMM)*1.05), ceil(double(heightMM)*1.05)), filename, QCairoPaintDevice::cftPS2, exportText);
            break;
        case formatPS3:
            cairo=new QCairoPaintDevice(QSizeF(ceil(double(widthMM)*1.05), ceil(double(heightMM)*1.05)), filename, QCairoPaintDevice::cftPS3, exportText);
            break;
        case formatSVG11:
            cairo=new QCairoPaintDevice(QSizeF(ceil(double(widthMM)*1.05), ceil(double(heightMM)*1.05)), filename, QCairoPaintDevice::cftSVG11, exportText);
            break;
       case formatSVG12:
            cairo=new QCairoPaintDevice(QSizeF(ceil(double(widthMM)*1.05), ceil(double(heightMM)*1.05)), filename, QCairoPaintDevice::cftSVG12, exportText);
            break;
        case formatPDF14:
            cairo=new QCairoPaintDevice(QSizeF(ceil(double(widthMM)*1.05), ceil(double(heightMM)*1.05)), filename, QCairoPaintDevice::cftPDF14, exportText);
            break;
        default:
        case formatPDF15:
            cairo=new QCairoPaintDevice(QSizeF(ceil(double(widthMM)*1.05), ceil(double(heightMM)*1.05)), filename, QCairoPaintDevice::cftPDF15, exportText);
            break;
    }

    return cairo;
}
