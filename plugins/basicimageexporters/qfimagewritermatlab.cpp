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


#include "qfimagewritermatlab.h"
#include <QObject>
#include "binarydatatools.h"
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QTextStream>

QFImageWriterMatlab::QFImageWriterMatlab():
    QFExporterImageSeries()
{
    file=NULL;
    data.clear();
    frames=0;
}

QFImageWriterMatlab::~QFImageWriterMatlab()
{
    if (TinyMATWriter_fOK(file)) {
        TinyMATWriter_close(file);
    }
    file=NULL;
    data.clear();
    frames=0;
}

QString QFImageWriterMatlab::filter() const {
    return QObject::tr("Matlab MAT file [image series as matrix] (*.mat)");
}

QString QFImageWriterMatlab::formatName() const {
    return QObject::tr("Matlab MAT file [image series as matrix]");
}

bool QFImageWriterMatlab::open(const QString& filename)
{
    close();
    file=TinyMATWriter_open(filename.toLocal8Bit().data());
    if (TinyMATWriter_fOK(file)) {
        return true;
    }
    if (file) TinyMATWriter_close(file);
    file=NULL;
    return false;
}

void QFImageWriterMatlab::close()
{
    if (TinyMATWriter_fOK(file)) {
        QVector<double> d;
        for (int i=0; i<data.size(); i++) {
            d.operator <<(data[i]);
        }
        int32_t sizes[4]={(int32_t)width,(int32_t)height,(int32_t)frames,(int32_t)channels};
        uint32_t ns=4;
        if (channels==1) ns=3;
        if (frames==1 && channels==1) ns=2;
        if (frames==1 && channels>1) {
            ns=3;
            sizes[2]=channels;
        }
        TinyMATWriter_writeMatrixND_rowmajor(file, "image", d.data(), sizes, ns);
        TinyMATWriter_close(file);
    }
    file=NULL;
    data.clear();
    frames=0;
}

bool QFImageWriterMatlab::intWriteFrameFloat(const float *data)
{
    QVector<double> d(width*height*channels);
    for (int i=0; i<d.size(); i++) d[i]=data[i];
    return intWriteFrameDouble(d.data());
}

bool QFImageWriterMatlab::intWriteFrameDouble(const double *data)
{
    if (!TinyMATWriter_fOK(file)) return false;

    while (this->data.size()<(int)channels) {
        this->data.append(QVector<double>());
    }
    for (uint32_t c=0; c<channels; c++) {
        QVector<double> d(width*height,0);
        for (uint32_t i=0; i<width*height; i++) {
            d[i]=data[c*width*height+i];
        }
        this->data[c].operator<<(d);
    }
    frames++;
    return true;
}

bool QFImageWriterMatlab::intWriteFrameUINT32(const uint32_t *data)
{
    QVector<double> d(width*height*channels);
    for (int i=0; i<d.size(); i++) d[i]=data[i];
    return intWriteFrameDouble(d.data());
}

bool QFImageWriterMatlab::intWriteFrameUINT16(const uint16_t *data)
{
    QVector<double> d(width*height*channels);
    for (int i=0; i<d.size(); i++) d[i]=data[i];
    return intWriteFrameDouble(d.data());
}

bool QFImageWriterMatlab::intWriteFrameUINT8(const uint8_t *data)
{
    QVector<double> d(width*height*channels);
    for (int i=0; i<d.size(); i++) d[i]=data[i];
    return intWriteFrameDouble(d.data());
}

