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
#include <QObject>
#include "libtiff_tools.h"
#include <QObject>
#include <QtGlobal>
#include <QDebug>
#include "programoptions.h"

FILE* QFImageWriterLibTIFF::fLibTIFFLog=NULL;
QString QFImageWriterLibTIFF::fLibTIFFLogFilename=QString("");
QMutex* QFImageWriterLibTIFF::mutex=NULL;


QFImageWriterLibTIFF::QFImageWriterLibTIFF(uint32_t compression, const QString& compression_name, bool noDouble) {
    tif=NULL;
    frames=0;
    this->compression=compression;
    this->compression_name=compression_name;
    this->noDouble=noDouble;

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
    if (tif) close();
}

QString QFImageWriterLibTIFF::filter() const {
    if (noDouble) return QObject::tr("TIFF Image Series, no 64-bit%1 [libTIFF] (*.tif *.tiff)").arg(compression_name);
    else return QObject::tr("TIFF Image Series%1 [libTIFF] (*.tif *.tiff)").arg(compression_name);
}

QString QFImageWriterLibTIFF::formatName() const {
    if (noDouble) return QObject::tr("TIFF Image Series, no 64-bit%1").arg(compression_name);
    else return QObject::tr("TIFF Image Series%1").arg(compression_name);
}

bool QFImageWriterLibTIFF::open(const QString& filename)
{
    close();
    tif = TIFFOpen(filename.toLatin1().data(),"w");
    frames=0;
    if (tif) {
        return true;
    }
    return false;
}

void QFImageWriterLibTIFF::close()
{
    if (tif) TIFFClose(tif);
    tif=NULL;
    frames=0;
}

bool QFImageWriterLibTIFF::intWriteFrameFloat(const float *image)
{
    /*if (!tif) return false;
    if (frames>0) TIFFWriteDirectory(tif);
    frames++;
    TIFFTWriteFloat(tif, data, width, height, 0,0,0,0,compression);
    if (frames==1 && comment.size()>0) TIFFSetField(tif,TIFFTAG_IMAGEDESCRIPTION,comment.toLocal8Bit().data());
    return true;*/


    if (!tif) return false;
    if (frames>0) TIFFWriteDirectory(tif);
    frames++;

    uint16 frame_width=width;
    uint16 frame_height=height;
    uint16 frame_channels=channels;
    uint32 rowsperstrip = (uint32)-1;

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, frame_width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, frame_height);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);

    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, sizeof(float)*8);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, frame_channels);
    rowsperstrip = TIFFDefaultStripSize(tif, rowsperstrip);
    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
    TIFFSetField(tif, TIFFTAG_FILLORDER, FILLORDER_MSB2LSB);
    TIFFSetField(tif,TIFFTAG_SAMPLEFORMAT,SAMPLEFORMAT_IEEEFP);
    TIFFSetField(tif,TIFFTAG_ORIENTATION,ORIENTATION_TOPLEFT);

    if (frames==1 && comment.size()>0) TIFFSetField(tif,TIFFTAG_IMAGEDESCRIPTION,comment.toLatin1().data());

    // write frame data
    // data is broken up into strips where each strip contains rowsperstrip complete rows of data
    // each stript then has a size of rowsperstrip*frame_width pixels. the last strip is possibly
    // smaller, so it is NOT padded with dummy data.
    float* const buf = (float*)_TIFFmalloc(TIFFStripSize(tif)); // data buffer for a strip of the image
    for (unsigned int row = 0; (row<frame_height); row+=rowsperstrip) {
        // compute rows in this strip:
        uint32 nrow = rowsperstrip;
        if ((row + rowsperstrip)>frame_height) {
            nrow=frame_height-row; // this is the last strip ... and it is a bit smaller! ... it only contains the last rows of the image
        }
        tstrip_t strip = TIFFComputeStrip(tif,row,0);
        tsize_t bi = 0;
        // go through the fraem row-wise
        for (unsigned int rr = 0; rr<nrow; ++rr) {
            for (unsigned int cc = 0; cc<frame_width; ++cc) { // go through all pixels in the current row
                for (unsigned int co=0; co<frame_channels; co++ ) { // go through all channels, so we have order RGBRGBRGB ...
                    buf[bi++] = (float)image[co*frame_width*frame_height+(row + rr)*frame_width+ cc];
                }
            }
        }
        if (TIFFWriteEncodedStrip(tif,strip,buf,bi*sizeof(float))<0) {
            return false;
        }
    }
    _TIFFfree(buf);
    return true;
}

bool QFImageWriterLibTIFF::intWriteFrameDouble(const double *image)
{
    if (noDouble) {
        QVector<float> d(width*height*channels);
        for (int i=0; i<d.size(); i++) d[i]=image[i];
        return intWriteFrameFloat(d.data());
    } else {
       /* if (!tif) return false;
        if (frames>0) TIFFWriteDirectory(tif);
        frames++;
        TIFFTWriteDouble(tif, data, width, height, 0,0,0,0,compression);
        if (frames==1 && comment.size()>0) TIFFSetField(tif,TIFFTAG_IMAGEDESCRIPTION,comment.toLocal8Bit().data());
        return true;*/

        if (!tif) return false;
        if (frames>0) TIFFWriteDirectory(tif);
        frames++;

        uint16 frame_width=width;
        uint16 frame_height=height;
        uint16 frame_channels=channels;
        uint32 rowsperstrip = (uint32)-1;

        TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, frame_width);
        TIFFSetField(tif, TIFFTAG_IMAGELENGTH, frame_height);
        TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);

        TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
        TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
        TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, sizeof(double)*8);
        TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, frame_channels);
        rowsperstrip = TIFFDefaultStripSize(tif, rowsperstrip);
        TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
        TIFFSetField(tif, TIFFTAG_FILLORDER, FILLORDER_MSB2LSB);
        TIFFSetField(tif,TIFFTAG_SAMPLEFORMAT,SAMPLEFORMAT_IEEEFP);
        TIFFSetField(tif,TIFFTAG_ORIENTATION,ORIENTATION_TOPLEFT);

        if (frames==1 && comment.size()>0) TIFFSetField(tif,TIFFTAG_IMAGEDESCRIPTION,comment.toLatin1().data());

        // write frame data
        // data is broken up into strips where each strip contains rowsperstrip complete rows of data
        // each stript then has a size of rowsperstrip*frame_width pixels. the last strip is possibly
        // smaller, so it is NOT padded with dummy data.
        double* const buf = (double*)_TIFFmalloc(TIFFStripSize(tif)); // data buffer for a strip of the image
        for (unsigned int row = 0; (row<frame_height); row+=rowsperstrip) {
            // compute rows in this strip:
            uint32 nrow = rowsperstrip;
            if ((row + rowsperstrip)>frame_height) {
                nrow=frame_height-row; // this is the last strip ... and it is a bit smaller! ... it only contains the last rows of the image
            }
            tstrip_t strip = TIFFComputeStrip(tif,row,0);
            tsize_t bi = 0;
            // go through the fraem row-wise
            for (unsigned int rr = 0; rr<nrow; ++rr) {
                for (unsigned int cc = 0; cc<frame_width; ++cc) { // go through all pixels in the current row
                    for (unsigned int co=0; co<frame_channels; co++ ) { // go through all channels, so we have order RGBRGBRGB ...
                        buf[bi++] = (double)image[co*frame_width*frame_height+(row + rr)*frame_width+ cc];
                    }
                }
            }
            if (TIFFWriteEncodedStrip(tif,strip,buf,bi*sizeof(double))<0) {
                return false;
            }
        }
        _TIFFfree(buf);
        return true;
    }
}

bool QFImageWriterLibTIFF::intWriteFrameUINT16(const uint16_t *image)
{
    /*if (!tif) return false;
    if (frames>0) TIFFWriteDirectory(tif);
    frames++;
    TIFFTWriteUint16(tif, data, width, height, 0,0,0,0,compression);
    if (frames==1 && comment.size()>0) TIFFSetField(tif,TIFFTAG_IMAGEDESCRIPTION,comment.toLocal8Bit().data());
    return true;*/

    if (!tif) return false;
    if (frames>0) TIFFWriteDirectory(tif);
    frames++;

    uint16 frame_width=width;
    uint16 frame_height=height;
    uint16 frame_channels=channels;
    uint32 rowsperstrip = (uint32)-1;

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, frame_width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, frame_height);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);

    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, sizeof(uint16_t)*8);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, frame_channels);
    rowsperstrip = TIFFDefaultStripSize(tif, rowsperstrip);
    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
    TIFFSetField(tif, TIFFTAG_FILLORDER, FILLORDER_MSB2LSB);
    TIFFSetField(tif,TIFFTAG_SAMPLEFORMAT,SAMPLEFORMAT_UINT);
    TIFFSetField(tif,TIFFTAG_ORIENTATION,ORIENTATION_TOPLEFT);

    if (frames==1 && comment.size()>0) TIFFSetField(tif,TIFFTAG_IMAGEDESCRIPTION,comment.toLatin1().data());

    // write frame data
    // data is broken up into strips where each strip contains rowsperstrip complete rows of data
    // each stript then has a size of rowsperstrip*frame_width pixels. the last strip is possibly
    // smaller, so it is NOT padded with dummy data.
    uint16_t* const buf = (uint16_t*)_TIFFmalloc(TIFFStripSize(tif)); // data buffer for a strip of the image
    for (unsigned int row = 0; (row<frame_height); row+=rowsperstrip) {
        // compute rows in this strip:
        uint32 nrow = rowsperstrip;
        if ((row + rowsperstrip)>frame_height) {
            nrow=frame_height-row; // this is the last strip ... and it is a bit smaller! ... it only contains the last rows of the image
        }
        tstrip_t strip = TIFFComputeStrip(tif,row,0);
        tsize_t bi = 0;
        // go through the fraem row-wise
        for (unsigned int rr = 0; rr<nrow; ++rr) {
            for (unsigned int cc = 0; cc<frame_width; ++cc) { // go through all pixels in the current row
                for (unsigned int co=0; co<frame_channels; co++ ) { // go through all channels, so we have order RGBRGBRGB ...
                    buf[bi++] = (uint16_t)image[co*frame_width*frame_height+(row + rr)*frame_width+ cc];
                }
            }
        }
        if (TIFFWriteEncodedStrip(tif,strip,buf,bi*sizeof(uint16_t))<0) {
            return false;
        }
    }
    _TIFFfree(buf);
    return true;
}

bool QFImageWriterLibTIFF::intWriteFrameUINT8(const uint8_t *image)
{
    /*if (!tif) return false;
    if (frames>0) TIFFWriteDirectory(tif);
    frames++;
    TIFFTWriteUint8(tif, data, width, height, 0,0,0,0,compression);
    if (frames==1 && comment.size()>0) TIFFSetField(tif,TIFFTAG_IMAGEDESCRIPTION,comment.toLocal8Bit().data());
    return true;*/

    if (!tif) return false;
    if (frames>0) TIFFWriteDirectory(tif);
    frames++;

    uint16 frame_width=width;
    uint16 frame_height=height;
    uint16 frame_channels=channels;
    uint32 rowsperstrip = (uint32)-1;

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, frame_width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, frame_height);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);

    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, sizeof(uint8_t)*8);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, frame_channels);
    rowsperstrip = TIFFDefaultStripSize(tif, rowsperstrip);
    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
    TIFFSetField(tif, TIFFTAG_FILLORDER, FILLORDER_MSB2LSB);
    TIFFSetField(tif,TIFFTAG_SAMPLEFORMAT,SAMPLEFORMAT_UINT);
    TIFFSetField(tif,TIFFTAG_ORIENTATION,ORIENTATION_TOPLEFT);

    if (frames==1 && comment.size()>0) TIFFSetField(tif,TIFFTAG_IMAGEDESCRIPTION,comment.toLatin1().data());

    // write frame data
    // data is broken up into strips where each strip contains rowsperstrip complete rows of data
    // each stript then has a size of rowsperstrip*frame_width pixels. the last strip is possibly
    // smaller, so it is NOT padded with dummy data.
    uint8_t* const buf = (uint8_t*)_TIFFmalloc(TIFFStripSize(tif)); // data buffer for a strip of the image
    for (unsigned int row = 0; (row<frame_height); row+=rowsperstrip) {
        // compute rows in this strip:
        uint32 nrow = rowsperstrip;
        if ((row + rowsperstrip)>frame_height) {
            nrow=frame_height-row; // this is the last strip ... and it is a bit smaller! ... it only contains the last rows of the image
        }
        tstrip_t strip = TIFFComputeStrip(tif,row,0);
        tsize_t bi = 0;
        // go through the fraem row-wise
        for (unsigned int rr = 0; rr<nrow; ++rr) {
            for (unsigned int cc = 0; cc<frame_width; ++cc) { // go through all pixels in the current row
                for (unsigned int co=0; co<frame_channels; co++ ) { // go through all channels, so we have order RGBRGBRGB ...
                    buf[bi++] = (uint8_t)image[co*frame_width*frame_height+(row + rr)*frame_width+ cc];
                }
            }
        }
        if (TIFFWriteEncodedStrip(tif,strip,buf,bi*sizeof(uint8_t))<0) {
            return false;
        }
    }
    _TIFFfree(buf);
    return true;
}

