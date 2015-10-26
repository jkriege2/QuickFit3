/*
Copyright (c) 2014
	
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


#include "qfqtimageimporterpng.h"
#include <QObject>
#include "qftools.h"


QFQtImageImporterPNG::QFQtImageImporterPNG() {
    allGrey=true;
}

QFQtImageImporterPNG::~QFQtImageImporterPNG()
{

}

QString QFQtImageImporterPNG::filter() const {
    return QObject::tr("PNG: Portable Newtork Graphics [Qt] (*.png)");
}

QString QFQtImageImporterPNG::formatName() const {
    return QObject::tr("PNG: Portable Newtork Graphics [Qt]");
}

bool QFQtImageImporterPNG::open(QString filename)
{
    // load the frame from the file into the internal datastore
    if (img.load(filename, "PNG")) {
        allGrey=img.allGray(); // check for greyscale
        return !img.isNull();
    } else {
        return false;
    }
}

void QFQtImageImporterPNG::close()
{
    // reset internal image to a NULL-image
    img=QImage();
}

uint32_t QFQtImageImporterPNG::countFrames()
{
    // there is only one frame per PNG-file
    return 1;
}

void QFQtImageImporterPNG::reset()
{
    // there is only one frame, so we have to do nothing
}

bool QFQtImageImporterPNG::nextFrame()
{
    // only one frame, so there is no next frame!
    return false;
}

uint32_t QFQtImageImporterPNG::intFrameWidth()
{
    return img.width();
}

uint32_t QFQtImageImporterPNG::intFrameHeight()
{
    return img.height();
}

uint32_t QFQtImageImporterPNG::intFrameChannels()
{
    if (allGrey) return 1;
    else return 3;
}

bool QFQtImageImporterPNG::intReadFrameFloat(float *data, int channel)
{
    // all images will be 8-bit images. therefore we only implement intReadFrameUINT16() and use it here
    uint16_t* d=(uint16_t*)qfMalloc(img.width()*img.height()*sizeof(uint16_t));
    bool ok=intReadFrameUINT16(d, channel);
    if (ok) for (int i=0; i<img.width()*img.height(); i++) data[i]=d[i];
    qfFree(d);
    return ok;
}

bool QFQtImageImporterPNG::intReadFrameDouble(double *data, int channel)
{
    // all images will be 8-bit images. therefore we only implement intReadFrameUINT16() and use it here
    uint16_t* d=(uint16_t*)qfMalloc(img.width()*img.height()*sizeof(uint16_t));
    bool ok=intReadFrameUINT16(d, channel);
    if (ok) for (int i=0; i<img.width()*img.height(); i++) data[i]=d[i];
    qfFree(d);
    return ok;
}

bool QFQtImageImporterPNG::intReadFrameUINT16(uint16_t *data, int channel)
{
    if (allGrey) {
        for (int y=0; y<img.height(); y++) {
            for (int x=0; x<img.width(); x++) {
                data[y*img.width()+x]= qGray(img.pixel(x,y));
            }
        }
        return true;
    } else if (channel==0){
        for (int y=0; y<img.height(); y++) {
            for (int x=0; x<img.width(); x++) {
                data[y*img.width()+x]= qRed(img.pixel(x,y));
            }
        }
        return true;
    } else if (channel==1){
        for (int y=0; y<img.height(); y++) {
            for (int x=0; x<img.width(); x++) {
                data[y*img.width()+x]= qGreen(img.pixel(x,y));
            }
        }
        return true;
    } else if (channel==2){
        for (int y=0; y<img.height(); y++) {
            for (int x=0; x<img.width(); x++) {
                data[y*img.width()+x]= qBlue(img.pixel(x,y));
            }
        }
        return true;
    }
    return false;
}



QFQtImageImporterJPEG::QFQtImageImporterJPEG()
{

}

QString QFQtImageImporterJPEG::filter() const {
    return QObject::tr("JPEG-files [Qt] (*.jpg *.jpeg)");
}

QString QFQtImageImporterJPEG::formatName() const {
    return QObject::tr("JPEG-files [Qt]");
}

bool QFQtImageImporterJPEG::open(QString filename)
{
    // load the frame from the file into the internal datastore
    if (img.load(filename, "JPEG")) {
        allGrey=img.allGray(); // check for greyscale
        return !img.isNull();
    } else {
        return false;
    }
}

QFQtImageImporterGIF::QFQtImageImporterGIF()
{

}

QString QFQtImageImporterGIF::filter() const {
    return QObject::tr("GIF: Graphic Interchange Format [Qt] (*.gif)");
}

QString QFQtImageImporterGIF::formatName() const {
    return QObject::tr("GIF: Graphic Interchange Format [Qt]");
}

bool QFQtImageImporterGIF::open(QString filename)
{
    // load the frame from the file into the internal datastore
    if (img.load(filename, "GIF")) {
        allGrey=img.allGray(); // check for greyscale
        return !img.isNull();
    } else {
        return false;
    }
}


QFQtImageImporterBMP::QFQtImageImporterBMP()
{

}

QString QFQtImageImporterBMP::filter() const {
    return formatName()+QString(" (*.bmp)");
}

QString QFQtImageImporterBMP::formatName() const {
    return QObject::tr("BMP: Windows Bitmaps [Qt]");
}

bool QFQtImageImporterBMP::open(QString filename)
{
    // load the frame from the file into the internal datastore
    if (img.load(filename, "BMP")) {
        allGrey=img.allGray(); // check for greyscale
        return !img.isNull();
    } else {
        return false;
    }
}



QFQtImageImporterPBM::QFQtImageImporterPBM()
{

}

QString QFQtImageImporterPBM::filter() const {
    return formatName()+QString(" (*.pbm)");

}

QString QFQtImageImporterPBM::formatName() const {
    return QObject::tr("PBM-files: Portable Bitmap [Qt]");
}

bool QFQtImageImporterPBM::open(QString filename)
{
    // load the frame from the file into the internal datastore
    if (img.load(filename, "PBM")) {
        allGrey=img.allGray(); // check for greyscale
        return !img.isNull();
    } else {
        return false;
    }
}


QFQtImageImporterPGM::QFQtImageImporterPGM()
{

}

QString QFQtImageImporterPGM::filter() const {
    return formatName()+QString(" (*.pgm)");
}

QString QFQtImageImporterPGM::formatName() const {
    return QObject::tr("PGM-files: Portable Graymap [Qt]");
}

bool QFQtImageImporterPGM::open(QString filename)
{
    // load the frame from the file into the internal datastore
    if (img.load(filename, "PGM")) {
        allGrey=img.allGray(); // check for greyscale
        return !img.isNull();
    } else {
        return false;
    }
}


QFQtImageImporterPPM::QFQtImageImporterPPM()
{

}

QString QFQtImageImporterPPM::filter() const {
    return formatName()+QString(" (*.ppm)");

}

QString QFQtImageImporterPPM::formatName() const {
    return QObject::tr("PPM-files: Portable Pixmap [Qt]");
}

bool QFQtImageImporterPPM::open(QString filename)
{
    // load the frame from the file into the internal datastore
    if (img.load(filename, "PPM")) {
        allGrey=img.allGray(); // check for greyscale
        return !img.isNull();
    } else {
        return false;
    }
}
