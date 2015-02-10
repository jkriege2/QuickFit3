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


#include "qfimagewriterlibtiff.h"

FILE* QFImageWriterLibTIFF::fLibTIFFLog=NULL;
QString QFImageWriterLibTIFF::fLibTIFFLogFilename=QString("");
QMutex* QFImageWriterLibTIFF::mutex=NULL;


QFImageWriterLibTIFF::QFImageWriterLibTIFF(uint32_t compression, const QString& compression_name, bool noDouble, bool onefileperchannel):
    QFExporterImageSeries()
{
    tif.clear();
    frames=0;
    this->compression=compression;
    this->compression_name=compression_name;
    this->noDouble=noDouble;
    this->onefileperchannel=onefileperchannel;

    QString lf=ProgramOptions::getConfigValue("exporters_basicimages/libtiff/logfile", ProgramOptions::getInstance()->getConfigFileDirectory()+"/exporters_basicimages_libtiff.log").toString();
    if (ProgramOptions::getConfigValue("exporters_basicimages/libtiff/log", false).toBool()) {
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
        TIFFSetWarningHandler(libTIFFWarningHandler);
        TIFFSetErrorHandler(libTIFFErrorHandler);
    } else {
        QMutexLocker lock(mutex);
        if (fLibTIFFLog) {
            FILE* f=fLibTIFFLog;
            fLibTIFFLog=NULL;
            fclose(f);
        }
        fLibTIFFLogFilename="";
        TIFFSetWarningHandler(0);
        TIFFSetErrorHandler(0);
    }
}

QFImageWriterLibTIFF::~QFImageWriterLibTIFF()
{
    if (tif.size()>0) close();
}

QString QFImageWriterLibTIFF::filter() const {
    return QString("%1 (*.tif *.tiff)").arg(formatName());
}

QString QFImageWriterLibTIFF::formatName() const {
    if (onefileperchannel) {
        if (noDouble) return QObject::tr("TIFF File%1, single-channel, 8-32bit").arg(compression_name);
        else return QObject::tr("TIFF Image Series%1, single-channel, 8-64bit").arg(compression_name);

    } else {
        if (noDouble) return QObject::tr("TIFF File%1, multi-channel, 8-32bit").arg(compression_name);
        else return QObject::tr("TIFF File%1, multi-channel, 8-64bit").arg(compression_name);
    }
}

bool QFImageWriterLibTIFF::open(const QString& filename)
{
    close();
    QStringList fns;
    if (onefileperchannel) {
        QFileInfo fi(filename);
        const QString base=fi.absolutePath()+"/"+fi.baseName();
        const QString ext=fi.completeSuffix();
        int fw=1;
        if (channels>=10) fw=2;
        if (channels>=100) fw=3;
        for (uint32_t i=0; i<channels; i++) {
            QString fn=base+QString("_%1.").arg(i,fw,10,QLatin1Char('0'))+ext;
            fns<<fn;
            tif << TIFFOpen(fn.toLatin1().data(),"w");
        }
    } else {
        tif << TIFFOpen(filename.toLatin1().data(),"w");
        fns<<filename;
    }
    frames=0;
    bool ok=true;
    for (int i=0; i<tif.size(); i++) {
        ok=ok&&tif[i];
        if (!tif[i]) {
            setLastError(QObject::tr("could not open file '%1' for writing!").arg(fns.value(i)));
        }
    }
    if (!ok) {
        close();
    }
    return ok;
}

void QFImageWriterLibTIFF::close()
{
    for (int i=0; i<tif.size(); i++) {
        if (tif[i]) TIFFClose(tif[i]);
    }
    tif.clear();
    frames=0;
}

bool QFImageWriterLibTIFF::intWriteFrameFloat(const float *image)
{
    if (onefileperchannel) {
        bool ok=true;
        for (int i=0; i<tif.size(); i++) {
            if (tif[i]) {
                ok=ok&&TIFFWriteSingleFrame(tif[i], &(image[i*width*height]), 1, SAMPLEFORMAT_IEEEFP);
            }
        }
        return ok;
    } else {
        if (tif.value(0, NULL)) {
            return TIFFWriteSingleFrame(tif.value(0, NULL), image, channels, SAMPLEFORMAT_IEEEFP);
        }
    }
    return false;
}

bool QFImageWriterLibTIFF::intWriteFrameDouble(const double *image)
{
    if (noDouble) {
        QVector<float> d(width*height*channels);
        for (int i=0; i<d.size(); i++) d[i]=image[i];
        return intWriteFrameFloat(d.data());
    } else {
        if (onefileperchannel) {
            bool ok=true;
            for (int i=0; i<tif.size(); i++) {
                if (tif[i]) {
                    ok=ok&&TIFFWriteSingleFrame(tif[i], &(image[i*width*height]), 1, SAMPLEFORMAT_IEEEFP);
                }
            }
            return ok;
        } else {
            if (tif.value(0, NULL)) {
                return TIFFWriteSingleFrame(tif.value(0, NULL), image, channels, SAMPLEFORMAT_IEEEFP);
            }
        }
    }
    return false;
}

bool QFImageWriterLibTIFF::intWriteFrameUINT32(const uint32_t *image)
{
    if (onefileperchannel) {
        bool ok=true;
        for (int i=0; i<tif.size(); i++) {
            if (tif[i]) {
                ok=ok&&TIFFWriteSingleFrame(tif[i], &(image[i*width*height]), 1, SAMPLEFORMAT_UINT);
            }
        }
        return ok;
    } else {
        if (tif.value(0, NULL)) {
            return TIFFWriteSingleFrame(tif.value(0, NULL), image, channels, SAMPLEFORMAT_UINT);
        }
    }
    return false;
}

bool QFImageWriterLibTIFF::intWriteFrameUINT16(const uint16_t *image)
{
    if (onefileperchannel) {
        bool ok=true;
        for (int i=0; i<tif.size(); i++) {
            if (tif[i]) {
                ok=ok&&TIFFWriteSingleFrame(tif[i], &(image[i*width*height]), 1, SAMPLEFORMAT_UINT);
            }
        }
        return ok;
    } else {
        if (tif.value(0, NULL)) {
            return TIFFWriteSingleFrame(tif.value(0, NULL), image, channels, SAMPLEFORMAT_UINT);
        }
    }
    return false;

}

bool QFImageWriterLibTIFF::intWriteFrameUINT8(const uint8_t *image)
{
    if (onefileperchannel) {
        bool ok=true;
        for (int i=0; i<tif.size(); i++) {
            if (tif[i]) {
                ok=ok&&TIFFWriteSingleFrame(tif[i], &(image[i*width*height]), 1, SAMPLEFORMAT_UINT);
            }
        }
        return ok;
    } else {
        if (tif.value(0, NULL)) {
            return TIFFWriteSingleFrame(tif.value(0, NULL), image, channels, SAMPLEFORMAT_UINT);
        }
    }
    return false;

}

