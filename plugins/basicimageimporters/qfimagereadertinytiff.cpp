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

#include "qfimagereadertinytiff.h"
#include <QObject>
#include <QObject>
#include <QtGlobal>
#include <QDebug>
#include "programoptions.h"

FILE* QFImageReaderTinyTIFF::fLibTIFFLog=NULL;
QString QFImageReaderTinyTIFF::fLibTIFFLogFilename=QString("");
QMutex* QFImageReaderTinyTIFF::mutex=NULL;

QFImageReaderTinyTIFF::QFImageReaderTinyTIFF() {
    if (!mutex) mutex=new QMutex(QMutex::Recursive);
    width=0;
    height=0;
    filename="";
    tif=NULL;
    frame=0;
    QString lf=ProgramOptions::getConfigValue("importers_basicimages/tinytiff/logfile", ProgramOptions::getInstance()->getConfigFileDirectory()+"/importers_basicimages_tinytiff.log").toString();
    if (ProgramOptions::getConfigValue("importers_basicimages/tinytiff/log", false).toBool()) {
        QMutexLocker lock(mutex);

        if (fLibTIFFLogFilename!=lf) {
            if (fLibTIFFLog) {
                FILE* f=fLibTIFFLog;
                fLibTIFFLog=NULL;
                fclose(f);
            }
            fLibTIFFLogFilename=lf;
            fLibTIFFLog=fopen(fLibTIFFLogFilename.toLocal8Bit().data(), "a");
        }
    } else {
        QMutexLocker lock(mutex);
        if (fLibTIFFLog) {
            FILE* f=fLibTIFFLog;
            fLibTIFFLog=NULL;
            fclose(f);
        }
        fLibTIFFLogFilename="";
    }
}

QFImageReaderTinyTIFF::~QFImageReaderTinyTIFF()
{
    if (tif!=NULL) TinyTIFFReader_close(tif);
    tif=NULL;
}

QString QFImageReaderTinyTIFF::filter() const {
    return QObject::tr("TIFF File [tinyTIFF] (*.tiff *.tif)");
}

QString QFImageReaderTinyTIFF::formatName() const {
    return QObject::tr("TIFF [tinyTIFF]");
}

bool QFImageReaderTinyTIFF::open(QString filename) {
    close();
    width=0;
    height=0;
    frame=0;
    imageDescription="";
    fileinfo.init(filename);
    tif=TinyTIFFReader_open(filename.toLocal8Bit().data());
    if (TinyTIFFReader_success(tif)) {
        this->filename=filename;
        width=TinyTIFFReader_getWidth(tif);
        height=TinyTIFFReader_getHeight(tif);
        imageDescription=TinyTIFFReader_getImageDescription(tif).c_str();
        tinyTIFFMessageHandler("QFImageReaderTinyTIFF", QObject::tr("opened file '%1', size: %2x%3\n").arg(filename).arg(width).arg(height));

        qfimdtGetQuickFitMetaData(fileinfo.properties, filename);

        bool metaOK=false;
        int cnt=0;
        while (!metaOK && cnt<5) {
            QByteArray imgD=TinyTIFFReader_getImageDescription(tif).c_str();

            if (imgD.size()>0) {
                metaOK=qfimdtGetOMEMetaData(fileinfo.properties, imgD);
                if (!metaOK) {
                    metaOK=qfimdtGetImageJMetaData(fileinfo.properties, imgD);
                }
                if (!metaOK) {
                    metaOK=qfimdtGetTinyTIFFMetaData(fileinfo.properties, imgD);
                }
                imageDescription+=QString("\n\n%1").arg(QString::fromLatin1(imgD.data()));
            }
            metaOK=metaOK||(!nextFrame());
            cnt++;
        }
        //reset();
        TinyTIFFReader_close(tif);
        tif=TinyTIFFReader_open(filename.toLocal8Bit().data());
        frame=0;

        return tif;
    } else {
        tinyTIFFErrorHandler("QFImageReaderTinyTIFF", QObject::tr("error opening file '%1': %2\n").arg(filename).arg(TinyTIFFReader_getLastError(tif)));
        close();
        width=0;
        height=0;
        tif=NULL;
        return false;
    }
    return false;
}

uint32_t QFImageReaderTinyTIFF::countFrames() {
    if (tif) {
        if (fileinfo.properties.contains("FRAMES")) {
            bool ok=false;
            uint32_t f=fileinfo.properties["FRAMES"].toULongLong(&ok);
            if (ok && f>0) {
                return f;
            }
        }

        return TinyTIFFReader_countFrames(tif);
    }
    return 0;
}

void QFImageReaderTinyTIFF::close() {
    if (tif) {
        TinyTIFFReader_close(tif);
        tinyTIFFMessageHandler("QFImageReaderTinyTIFF", QObject::tr("closed file '%1'\n").arg(filename));
    }
    tif=NULL;
    width=0;
    height=0;
    frame=0;
    filename="";
}

void QFImageReaderTinyTIFF::reset() {
    QString fn=filename;
    close();
    if (!fn.isEmpty()) open(fn);
}


bool QFImageReaderTinyTIFF::nextFrame() {
    if (!tif) return false;
    frame++;
    bool ok=TinyTIFFReader_readNext(tif)!=0;
    if (ok) {
        ok=(TinyTIFFReader_getWidth(tif)==width)&&(TinyTIFFReader_getHeight(tif)==height);
        if (!ok) tinyTIFFErrorHandler("QFImageReaderTinyTIFF", QObject::tr("error in file '%1': frame %2 does not have the correct size: size if %3x%4, expected %5x%6\n").arg(filename).arg(frame).arg(TinyTIFFReader_getWidth(tif)).arg(TinyTIFFReader_getHeight(tif)).arg(width).arg(height));
    }
    return ok;
}

uint32_t QFImageReaderTinyTIFF::intFrameWidth() {
    if (tif) return width;
    return 0;
}

uint32_t QFImageReaderTinyTIFF::intFrameHeight() {
    if (tif) return height;
    return 0;
}


template <class Tin, class Tout>
void readTinyTIFFFrame(TinyTIFFReaderFile* tif, Tout* buffer) {
    uint32_t wwidth=TinyTIFFReader_getWidth(tif);
    uint32_t hheight=TinyTIFFReader_getHeight(tif);
    Tin* tmp=(Tin*)qfCalloc(wwidth*hheight, sizeof(Tin));
    TinyTIFFReader_getSampleData(tif, tmp, 0);
    for (uint32_t i=0; i<wwidth*hheight; i++) {
        buffer[i]=tmp[i];
    }
    qfFree(tmp);
}


bool QFImageReaderTinyTIFF::intReadFrameFloat(float *data, int channel) {
    if (!tif) return false;

    uint32_t wwidth=TinyTIFFReader_getWidth(tif);
    uint32_t hheight=TinyTIFFReader_getHeight(tif);
    if (!(wwidth>0 && hheight>0)) tinyTIFFErrorHandler("QFImageReaderTinyTIFF", QObject::tr("error in file '%1': frame %2 is too small\n").arg(filename).arg(frame));
    else {
        uint16_t sformat=TinyTIFFReader_getSampleFormat(tif);
        uint16_t bits=TinyTIFFReader_getBitsPerSample(tif);

        if (sformat==TINYTIFFREADER_SAMPLEFORMAT_UINT) {
            if (bits==8) readTinyTIFFFrame<uint8_t, float>(tif, data);
            else if (bits==16) readTinyTIFFFrame<uint16_t, float>(tif, data);
            else if (bits==32) readTinyTIFFFrame<uint32_t, float>(tif, data);
            else {
                tinyTIFFErrorHandler("QFImageReaderTinyTIFF", QObject::tr("frame %1 has a datatype not convertible to float (type=%2, bitspersample=%3)\n").arg(frame).arg(sformat).arg(bits));
                return false;
            }
        } else if (sformat==TINYTIFFREADER_SAMPLEFORMAT_INT) {
            if (bits==8) readTinyTIFFFrame<int8_t, float>(tif, data);
            else if (bits==16) readTinyTIFFFrame<int16_t, float>(tif, data);
            else if (bits==32) readTinyTIFFFrame<int32_t, float>(tif, data);
            else {
                tinyTIFFErrorHandler("QFImageReaderTinyTIFF", QObject::tr("frame %1 has a datatype not convertible to float (type=%2, bitspersample=%3)\n").arg(frame).arg(sformat).arg(bits));
                return false;
            }
        } else if (sformat==TINYTIFFREADER_SAMPLEFORMAT_FLOAT) {
            if (bits==32) readTinyTIFFFrame<float, float>(tif, data);
            else {
                tinyTIFFErrorHandler("QFImageReaderTinyTIFF", QObject::tr("frame %1 has a datatype not convertible to float (type=%2, bitspersample=%3)\n").arg(frame).arg(sformat).arg(bits));
                return false;
            }
        } else {
            tinyTIFFErrorHandler("QFImageReaderTinyTIFF", QObject::tr("frame %1 has a datatype not convertible to float (type=%2, bitspersample=%3)\n").arg(frame).arg(sformat).arg(bits));
            return false;
        }
    }
    if (TinyTIFFReader_wasError(tif)) {
        tinyTIFFErrorHandler("QFImageReaderTinyTIFF", QObject::tr("error reading frame %1: %2\n").arg(frame).arg(TinyTIFFReader_getLastError(tif)));
        return false;
    }

    return true;
}

bool QFImageReaderTinyTIFF::intReadFrameUINT16(uint16_t *data, int channel) {
    if (!tif) return false;

    uint32_t wwidth=TinyTIFFReader_getWidth(tif);
    uint32_t hheight=TinyTIFFReader_getHeight(tif);
    if (!(wwidth>0 && hheight>0)) tinyTIFFErrorHandler("QFImageReaderTinyTIFF", QObject::tr("error in file '%1': frame %2 is too small\n").arg(filename).arg(frame));
    else {
        uint16_t sformat=TinyTIFFReader_getSampleFormat(tif);
        uint16_t bits=TinyTIFFReader_getBitsPerSample(tif);

        if (sformat==TINYTIFFREADER_SAMPLEFORMAT_UINT) {
            if (bits==8) readTinyTIFFFrame<uint8_t, uint16_t>(tif, data);
            else if (bits==16) readTinyTIFFFrame<uint16_t, uint16_t>(tif, data);
            else {
                tinyTIFFErrorHandler("QFImageReaderTinyTIFF", QObject::tr("frame %1 has a datatype not convertible to uint16 (type=%2, bitspersample=%3)\n").arg(frame).arg(sformat).arg(bits));
                return false;
            }
        } else if (sformat==TINYTIFFREADER_SAMPLEFORMAT_INT) {
            if (bits==8) readTinyTIFFFrame<int8_t, uint16_t>(tif, data);
            else if (bits==16) readTinyTIFFFrame<int16_t, uint16_t>(tif, data);
            else {
                tinyTIFFErrorHandler("QFImageReaderTinyTIFF", QObject::tr("frame %1 has a datatype not convertible to uint16 (type=%2, bitspersample=%3)\n").arg(frame).arg(sformat).arg(bits));
                return false;
            }
        } else if (sformat==TINYTIFFREADER_SAMPLEFORMAT_FLOAT) {
            tinyTIFFErrorHandler("QFImageReaderTinyTIFF", QObject::tr("frame %1 has a datatype not convertible to uint16 (type=%2, bitspersample=%3)\n").arg(frame).arg(sformat).arg(bits));
            return false;
        } else {
            tinyTIFFErrorHandler("QFImageReaderTinyTIFF", QObject::tr("frame %1 has a datatype not convertible to uint16 (type=%2, bitspersample=%3)\n").arg(frame).arg(sformat).arg(bits));
            return false;
        }
    }
    if (TinyTIFFReader_wasError(tif)) {
        tinyTIFFErrorHandler("QFImageReaderTinyTIFF", QObject::tr("error reading frame %d: %s\n").arg(frame).arg(TinyTIFFReader_getLastError(tif)));
        return false;
    }

    return true;
}

bool QFImageReaderTinyTIFF::intReadFrameDouble(double *data, int channel) {
    if (!tif) return false;

    uint32_t wwidth=TinyTIFFReader_getWidth(tif);
    uint32_t hheight=TinyTIFFReader_getHeight(tif);
    if (!(wwidth>0 && hheight>0)) tinyTIFFErrorHandler("QFImageReaderTinyTIFF", QObject::tr("error in file '%1': frame %2 is too small\n").arg(filename).arg(frame));
    else {
        uint16_t sformat=TinyTIFFReader_getSampleFormat(tif);
        uint16_t bits=TinyTIFFReader_getBitsPerSample(tif);

        if (sformat==TINYTIFFREADER_SAMPLEFORMAT_UINT) {
            if (bits==8) readTinyTIFFFrame<uint8_t, double>(tif, data);
            else if (bits==16) readTinyTIFFFrame<uint16_t, double>(tif, data);
            else if (bits==32) readTinyTIFFFrame<uint32_t, double>(tif, data);
            else {
                tinyTIFFErrorHandler("QFImageReaderTinyTIFF", QObject::tr("frame %1 has a datatype not convertible to double (type=%2, bitspersample=%3)\n").arg(frame).arg(sformat).arg(bits));
                return false;
            }
        } else if (sformat==TINYTIFFREADER_SAMPLEFORMAT_INT) {
            if (bits==8) readTinyTIFFFrame<int8_t, double>(tif, data);
            else if (bits==16) readTinyTIFFFrame<int16_t, double>(tif, data);
            else if (bits==32) readTinyTIFFFrame<int32_t, double>(tif, data);
            else {
                tinyTIFFErrorHandler("QFImageReaderTinyTIFF", QObject::tr("frame %1 has a datatype not convertible to double (type=%2, bitspersample=%3)\n").arg(frame).arg(sformat).arg(bits));
                return false;
            }
        } else if (sformat==TINYTIFFREADER_SAMPLEFORMAT_FLOAT) {
            if (bits==32) readTinyTIFFFrame<float, double>(tif, data);
            else {
                tinyTIFFErrorHandler("QFImageReaderTinyTIFF", QObject::tr("frame %1 has a datatype not convertible to double (type=%2, bitspersample=%3)\n").arg(frame).arg(sformat).arg(bits));
                return false;
            }
        } else {
            tinyTIFFErrorHandler("QFImageReaderTinyTIFF", QObject::tr("frame %1 has a datatype not convertible to double (type=%2, bitspersample=%3)\n").arg(frame).arg(sformat).arg(bits));
            return false;
        }
    }

    if (TinyTIFFReader_wasError(tif)) {
        tinyTIFFErrorHandler("QFImageReaderTinyTIFF", QObject::tr("error reading frame %1: %2\n").arg(frame).arg(TinyTIFFReader_getLastError(tif)));
        return false;
    }
    return true;
}
