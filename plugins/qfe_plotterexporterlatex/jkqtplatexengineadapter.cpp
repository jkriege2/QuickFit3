/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

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

#include "jkqtplatexengineadapter.h"


JKQTPLatexEngineAdapter::JKQTPLatexEngineAdapter(bool docMode, bool colorMode, QTeXPaintDevice::OutputMode outputMode, bool m_useLatexParser)
{
    this->docMode=docMode;
    this->colorMode=colorMode;
    this->outputMode=outputMode;
    this->m_useLatexParser=m_useLatexParser;
    id="TIKZ";
    if (outputMode==QTeXPaintDevice::Pgf) id="PGF";
    if (!colorMode) id+="_BW";
    if (docMode) id+="_DOC";
    if (!m_useLatexParser) id+="_NOTEXPARSER";
}

QString JKQTPLatexEngineAdapter::getFilter() const
{
    QString m="";
    if (outputMode==QTeXPaintDevice::Tikz) {
        m=QObject::tr("Tikz");
    } else if (outputMode==QTeXPaintDevice::Pgf) {
        m=QObject::tr("Pgf");
    }
    if (!colorMode) m+=QObject::tr(" b&w");
    else m+=QObject::tr(" color");
    if (!docMode) m+=QObject::tr(", include file");
    else  m+=QObject::tr(", full document");
    if (!m_useLatexParser) m+=QObject::tr(", no TeX parser");
    return QObject::tr("LaTeX [%1] (*.tex)").arg(m);
}

QString JKQTPLatexEngineAdapter::getFormatName() const
{
    return id;
}

QString JKQTPLatexEngineAdapter::getFormatID() const
{
    return id;
}

QStringList JKQTPLatexEngineAdapter::getFileExtension() const
{
    return QStringList("tex");
}

bool JKQTPLatexEngineAdapter::getSetAbsolutePaperSize() const
{
    return true;
}

double JKQTPLatexEngineAdapter::getPrintSizeXInMM() const
{
    return 210;
}

double JKQTPLatexEngineAdapter::getPrintSizeYInMM() const
{
    return 297;
}

bool JKQTPLatexEngineAdapter::isPrinter() const
{
    return false;
}

bool JKQTPLatexEngineAdapter::useLatexParser() const
{
    return m_useLatexParser;
}

QPaintDevice *JKQTPLatexEngineAdapter::createPaintdevice(const QString &filename, int widthPix, int heightPix) const
{
    //qDebug()<<"create EMF-device for "<<filename<<QSize(widthPix, heightPix);
    QTeXPaintDevice * emf=new QTeXPaintDevice(filename, QSize(ceil(double(widthPix)/96.0*25.4*72.0), ceil(double(heightPix)/96.0*25.4*72.0)), QTeXPaintDevice::pt);
    emf->setColorMode((colorMode)?(QPrinter::Color):(QPrinter::GrayScale));
    emf->setDocumentMode(docMode);
    emf->setEscapeTextMode(m_useLatexParser);
    return emf;
}
