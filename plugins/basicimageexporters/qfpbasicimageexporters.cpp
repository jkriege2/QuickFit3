/*
Copyright (c) 2014
	
	last modification: $LastChangedDate: 2014-06-24 16:05:58 +0200 (Di, 24 Jun 2014) $  (revision $Rev: 3289 $)

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


#include "qfpbasicimageexporters.h"
#include <tiffio.h>
#include "qfimagewriterlibtiff.h"
#include "qfimagewriterrawdouble.h"
#include "qfimagewriterrawfloat.h"
#include "qfbasicimageexportersoptionswidget.h"
#include "qfpluginservices.h"


QFPBasicImageExporters::QFPBasicImageExporters(QObject *parent)
{
    QFPluginServices::getInstance()->registerSettingsPane(this);
}

QStringList QFPBasicImageExporters::getIDs() const {
    QStringList res;
    res<<"exporter_image_libtiff";
    res<<"exporter_image_libtiff_64";
#ifdef ZIP_SUPPORT
    res<<"exporter_image_libtiff_deflate";
#endif
#ifdef LZW_SUPPORT
    res<<"exporter_image_libtiff_lzw";
#endif
#ifdef JPEG_SUPPORT
    res<<"exporter_image_libtiff_jpeg";
#endif
#ifdef LZMA_SUPPORT
    res<<"exporter_image_libtiff_lzma";
#endif
    res<<"exporter_image_libtiff_64";
#ifdef ZIP_SUPPORT
    res<<"exporter_image_libtiff_deflate_64";
#endif
#ifdef LZW_SUPPORT
    res<<"exporter_image_libtiff_lzw_64";
#endif
#ifdef JPEG_SUPPORT
    res<<"exporter_image_libtiff_jpeg_64";
#endif
#ifdef LZMA_SUPPORT
    res<<"exporter_image_libtiff_lzma_64";
#endif
    res<<"exporter_image_raw_double";
    res<<"exporter_image_raw_float";

    return res;
}

QFExporter* QFPBasicImageExporters::createExporter(QString id) const  {
    if (id=="exporter_image_libtiff") {
        return new QFImageWriterLibTIFF(COMPRESSION_NONE, tr(", no compression"), true);
    } else if (id=="exporter_image_libtiff_lzw") {
        return new QFImageWriterLibTIFF(COMPRESSION_LZW, tr(", LZW compression"), true);
    } else if (id=="exporter_image_libtiff_jpeg") {
        return new QFImageWriterLibTIFF(COMPRESSION_JPEG, tr(", JPEG compression"), true);
    } else if (id=="exporter_image_libtiff_lzma") {
        return new QFImageWriterLibTIFF(COMPRESSION_LZMA, tr(", LZMA compression"), true);
    } else if (id=="exporter_image_libtiff_deflate") {
        return new QFImageWriterLibTIFF(COMPRESSION_DEFLATE, tr(", ZIP/Deflate compression"), true);
    } else if (id=="exporter_image_libtiff_64") {
        return new QFImageWriterLibTIFF(COMPRESSION_NONE, tr(", no compression"), false);
    } else if (id=="exporter_image_libtiff_lzw_64") {
        return new QFImageWriterLibTIFF(COMPRESSION_LZW, tr(", LZW compression"), false);
    } else if (id=="exporter_image_libtiff_jpeg_64") {
        return new QFImageWriterLibTIFF(COMPRESSION_JPEG, tr(", JPEG compression"), false);
    } else if (id=="exporter_image_libtiff_lzma_64") {
        return new QFImageWriterLibTIFF(COMPRESSION_LZMA, tr(", LZMA compression"), false);
    } else if (id=="exporter_image_libtiff_deflate_64") {
        return new QFImageWriterLibTIFF(COMPRESSION_DEFLATE, tr(", ZIP/Deflate compression"), false);
    } else if (id=="exporter_image_raw_double") {
        return new QFImageWriterRAWDouble();
    } else if (id=="exporter_image_raw_float") {
        return new QFImageWriterRAWFloat();
    }
    return NULL;
}

QString QFPBasicImageExporters::pluginOptionsName() const
{
    return getName();
}

QIcon QFPBasicImageExporters::pluginOptionsIcon() const
{
    return QIcon(QIcon(":/importers_basicimages/icon_large.png"));
}

QFPluginOptionsWidget *QFPBasicImageExporters::createOptionsWidget(QWidget *parent)
{
    return new QFBasicImageExportersOptionsWidget(this, parent);
}


Q_EXPORT_PLUGIN2(exporters_basicimage, QFPBasicImageExporters)
