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

#include "jkqtpemfengineadapter.h"
#include <QApplication>
#include <QDesktopWidget>


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
    return true;
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

QPaintDevice *JKQTPEMFEngineAdapter::createPaintdeviceMM(const QString &filename, double widthMM, double heightMM) const
{
    //qDebug()<<"create EMF-device for "<<filename<<QSize(widthPix, heightPix);
    EmfPaintDevice * emf=new EmfPaintDevice(QSize(widthMM/25.4*QApplication::desktop()->logicalDpiX(), heightMM/25.4*QApplication::desktop()->logicalDpiY()), filename);
    return emf;

}
