/*
Copyright (c) 2008-2014 Jan Buchholz & Jan W. Krieger, German Cancer Research Center

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

#include "qfimagereaderradhard.h"

#include <QObject>
#include <QtGlobal>
#include "qftools.h"

QFImageReaderRadhard::QFImageReaderRadhard():
    QFImporterImageSeries()
{
    width=0;
    height=0;
    filename="";
    frameSize=0;
    file=NULL;
    buffer=NULL;
}

QFImageReaderRadhard::~QFImageReaderRadhard() {
    if (file) close();
}

QString QFImageReaderRadhard::filter() const {
    return QObject::tr("raw Radhard file (*.rrh *.raw.dat)");
}

QString QFImageReaderRadhard::formatName() const {
    return QObject::tr("RadHard Raw Data");
}

unsigned int QFImageReaderRadhard::calculateFrameSize() {
  // lets assume we have correct shapes frames here... with correct CRCs
  //only accepts 32 bit packed data format

  unsigned char *buffer = new unsigned char [MAX_FRAME_SIZE];
  QDataStream in(file);
  unsigned int len = in.readRawData((char*)buffer, MAX_FRAME_SIZE);

  unsigned char *bufferPos=buffer;
  unsigned int pos=0;
  unsigned frameSize=0;
  file->seek(0);
  uint16_t crc=0xFFFFU;
  do {
    crc=crc_ccitt(crc,bufferPos,1);
    bufferPos+=1;
    pos+=1;
    uint16_t crc0=*((uint16_t*)bufferPos);
    uint16_t crc1=crc16::reverse<uint16_t>(crc);
    if(crc0==crc1)frameSize=pos+2;
  }while((pos<len)&&(frameSize==0));
  file->seek(0);
  delete[] buffer;
  return frameSize;
}


bool QFImageReaderRadhard::open(QString filename) {
  close();
  bool result=true;
  file = new QFile(filename);
  fileinfo.init(filename);
  if (!file->open(QIODevice::ReadOnly)) {
      setLastError(file->errorString());
      return false;
  }
  this->filename=filename;
  frameSize=calculateFrameSize();
  fprintf(stderr,"FRAMESIZE: %i",frameSize);
  if (frameSize<=134) {
      height=(frameSize-6)/4;
      width=32;
  } else {
      switch(frameSize) {
        case   134: width=   32; height= 32; break; //RH2
        case  2058: width=  128; height=128; break; //MS
        case 32778: width= 1024; height= 32; break; //RH2 CORR
        default: {
          frameSize=0;
        } break;
      }
  }
  if(width==0||height==0) {
      setLastError(QObject::tr("could not determine frame size: framesize=%1  with=%2  height=%3").arg(frameSize).arg(width).arg(height));
      close();
      result=false;
  }
  if (result) {
      buffer = (unsigned char*)qfMalloc(frameSize*sizeof(unsigned char)); //new unsigned char [frameSize];
  }
  nextFrame();
  return result;
}

void QFImageReaderRadhard::close() {
  if (!file) return ;
  if (buffer) qfFree(buffer);
  buffer=NULL;
  file->close();
  delete file;
  file=NULL;
  filename="";
}

uint32_t QFImageReaderRadhard::countFrames() {
  uint32_t result=0;
  if (!file) return 0;
  unsigned int size=file->size();
  if(frameSize!=0) {
    if((size % frameSize)==0)
      result=size/frameSize;
  }
  return result-1;
}

bool QFImageReaderRadhard::nextFrame() {
    if (!file) return false;
    QDataStream in(file);
    in.readRawData((char*)buffer, frameSize);
    return !file->atEnd();
}

void QFImageReaderRadhard::reset() {
    QString fn=filename;
    close();
    open(fn);
}

uint32_t QFImageReaderRadhard::intFrameWidth() {
    return width;
}

uint32_t QFImageReaderRadhard::intFrameHeight() {
    return height;
}

bool QFImageReaderRadhard::intReadFrameUINT16(uint16_t* data) {
  if (!file) return false;
  return readFrame_<uint16_t>(data);
}

bool QFImageReaderRadhard::intReadFrameDouble(double *data) {
    if (!file) return false;
    return readFrame_<double>(data);
}

bool QFImageReaderRadhard::intReadFrameFloat(float* data) {
    if (!file) return false;
  return readFrame_<float>(data);
}
