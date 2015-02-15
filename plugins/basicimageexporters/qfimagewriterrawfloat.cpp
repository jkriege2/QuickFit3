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


#include "qfimagewriterrawfloat.h"
#include <QObject>
#include "binarydatatools.h"

QFImageWriterRAWFloat::QFImageWriterRAWFloat():
    QFExporterImageSeries()
{

}

QFImageWriterRAWFloat::~QFImageWriterRAWFloat()
{
    if (file.isOpen()) close();
}

QString QFImageWriterRAWFloat::filter() const {
    return QObject::tr("RAW [32-bit float] Image Series (*.raw *.dat)");
}

QString QFImageWriterRAWFloat::formatName() const {
    return QObject::tr("RAW [32-bit float] Image Series");
}

bool QFImageWriterRAWFloat::open(const QString& filename)
{
    close();
    file.setFileName(filename);
    if (file.open(QIODevice::WriteOnly)) {
        QFileInfo fi(filename);
        QDir d=fi.absoluteDir();
        QFile description(d.absoluteFilePath(fi.baseName()+".description.txt"));
        if (description.open(QIODevice::WriteOnly|QIODevice::Text)) {
            QTextStream str(&description);
            str<<"data_format=float\n";
            str<<"bitspersample=32\n";
            str<<"width="<<width<<"\n";
            str<<"height="<<height<<"\n";
            str<<"channels="<<channels<<"\n";
            str<<"deltaX="<<deltaX<<"\n";
            str<<"deltaY="<<deltaY<<"\n";
            str<<"deltaZ="<<deltaZ<<"\n";
            str<<"unit="<<unitname<<"\n";
            description.close();
        }
        return true;
    }
    return false;
}

void QFImageWriterRAWFloat::close()
{
    if (file.isOpen()) {
        QFileInfo fi(filename);
        QDir d=fi.absoluteDir();
        QFile description(d.absoluteFilePath(fi.baseName()+".description.txt"));
        if (description.open(QIODevice::Append|QIODevice::Text)) {
            QTextStream str(&description);
            str<<"frames="<<frames<<"\n";
            description.close();
        }
        file.close();
    }
}

bool QFImageWriterRAWFloat::intWriteFrameFloat(const float *data)
{
    if (!file.isOpen()) return false;
    binfileWriteFloatArray(file, data, width*height*channels);
    return true;
}

bool QFImageWriterRAWFloat::intWriteFrameDouble(const double *data)
{
    QVector<float> d(width*height*channels);
    for (int i=0; i<d.size(); i++) d[i]=data[i];
    return intWriteFrameFloat(d.data());
}

bool QFImageWriterRAWFloat::intWriteFrameUINT32(const uint32_t *data)
{
    QVector<float> d(width*height*channels);
    for (int i=0; i<d.size(); i++) d[i]=data[i];
    return intWriteFrameFloat(d.data());
}

bool QFImageWriterRAWFloat::intWriteFrameUINT16(const uint16_t *data)
{
    QVector<float> d(width*height*channels);
    for (int i=0; i<d.size(); i++) d[i]=data[i];
    return intWriteFrameFloat(d.data());
}

bool QFImageWriterRAWFloat::intWriteFrameUINT8(const uint8_t *data)
{
    QVector<float> d(width*height*channels);
    for (int i=0; i<d.size(); i++) d[i]=data[i];
    return intWriteFrameFloat(d.data());
}

