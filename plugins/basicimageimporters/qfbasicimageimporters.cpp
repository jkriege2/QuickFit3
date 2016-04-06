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

#include "qfbasicimageimporters.h"
#include "qfimagereaderradhard.h"
#include "qfbasicimageimportersoptionswidget.h"
#include "qfimagereaderdiffusion4binary.h"
#include "qfimagereaderraw.h"
#include "qfimagereadercsv.h"

QStringList QFBasicImageImporters::getIDs() const {
    QStringList res;
    res<<"imageimporter_libtiff";
    res<<"imageimporter_tinytiff";
    res<<"imageimporter_radhard";
    res<<"imageimporter_diffusion4btsbinary";
    res<<"imageimporter_rawunit8";
    res<<"imageimporter_rawunit16";
    res<<"imageimporter_rawunit32";
    res<<"imageimporter_rawunit64";
    res<<"imageimporter_rawfloat";
    res<<"imageimporter_rawdouble";
    res<<"imageimporter_csv";
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
    if (id=="imageimporter_rawunit8") {
        return new QFImageReaderRAW<uint8_t>("uint 8-bit");
    }
    if (id=="imageimporter_rawunit16") {
        return new QFImageReaderRAW<uint16_t>("uint 16-bit");
    }
    if (id=="imageimporter_rawunit32") {
        return new QFImageReaderRAW<uint32_t>("uint 32-bit");
    }
    if (id=="imageimporter_rawunit64") {
        return new QFImageReaderRAW<uint64_t>("uint 64-bit");
    }
    if (id=="imageimporter_rawfloat") {
        return new QFImageReaderRAW<float>("float 32-bit");
    }
    if (id=="imageimporter_rawdouble") {
        return new QFImageReaderRAW<double>("double 64-bit");
    }
    if (id=="imageimporter_csv") {
        return new QFImageReaderCSV();
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

QFBasicImageImporters::QFBasicImageImporters(QObject *parent):
    QObject(parent)
{
    QFPluginServices::getInstance()->registerSettingsPane(this);
}

Q_EXPORT_PLUGIN2(importers_basicimages, QFBasicImageImporters)
