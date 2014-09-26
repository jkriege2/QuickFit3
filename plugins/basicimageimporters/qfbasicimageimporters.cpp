/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

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

#include "qfbasicimageimporters.h"
#include "qfimagereaderradhard.h"
#include "qfbasicimageimportersoptionswidget.h"
#include "qfimagereaderdiffusion4binary.h"


QStringList QFBasicImageImporters::getIDs() const {
    QStringList res;
    res<<"imageimporter_libtiff";
    res<<"imageimporter_tinytiff";
    res<<"imageimporter_radhard";
    res<<"imageimporter_diffusion4btsbinary";
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

QFBasicImageImporters::QFBasicImageImporters(QObject *parent)
{
    QFPluginServices::getInstance()->registerSettingsPane(this);
}

Q_EXPORT_PLUGIN2(importers_basicimages, QFBasicImageImporters)
