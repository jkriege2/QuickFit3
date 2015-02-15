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


#ifndef QFIMAGEWRITERLIBTIFF_H
#define QFIMAGEWRITERLIBTIFF_H
#include "qfpluginexporters.h"
#include "qfexporter.h"
#include "qfexporterimageseries.h"
#include "libtiff_tools.h"
#include <stdio.h>
#include <stdlib.h>
#include <QMutex>
#include <QMutexLocker>
#include <QObject>
#include "libtiff_tools.h"
#include <QObject>
#include <QtGlobal>
#include <QDebug>
#include "programoptions.h"
#include "qfimagemetadatatool.h"

/*! \brief QFExporter class for fit function
    \ingroup qf3exporterplugins_exporters_basicimage

*/
class QFImageWriterLibTIFF: public QFExporterImageSeries {
    public:
        QFImageWriterLibTIFF(uint32_t compression, const QString& compression_name, bool noDouble, bool onefileperchannel);
        virtual ~QFImageWriterLibTIFF();
        /*! \copydoc QFFitFunction::filter()   */
        virtual QString filter() const ;
        /*! \copydoc QFFitFunction::formatName()   */
        virtual QString formatName() const ;

        virtual bool open(const QString &filename);
        /** \brief close the currently opened image file */
        virtual void close();
        static FILE* fLibTIFFLog;
        static QString fLibTIFFLogFilename;
        static QMutex* mutex;



        static void libTIFFErrorHandler(const char* module, const char* fmt, va_list ap) {
            QMutexLocker lock(mutex);
            if (fLibTIFFLog) {
                fprintf(fLibTIFFLog, "error (%s): ", module);
                fprintf(fLibTIFFLog, fmt, ap);
                fflush(fLibTIFFLog);
            }
        }

        static void libTIFFWarningHandler(const char* module, const char* fmt, va_list ap) {
            QMutexLocker lock(mutex);
            if (fLibTIFFLog) {
                fprintf(fLibTIFFLog, "warning (%s): ", module);
                fprintf(fLibTIFFLog, fmt, ap);
                fflush(fLibTIFFLog);
            }
        }

        static void libTIFFMessageHandler(const char* module, const char* fmt, va_list ap) {
            QMutexLocker lock(mutex);
            if (fLibTIFFLog) {
                fprintf(fLibTIFFLog, "message (%s): ", module);
                fprintf(fLibTIFFLog, fmt, ap);
                fflush(fLibTIFFLog);
            }
        }

    protected:
        /** \brief read a new frame into the given array of floating point numbers */
        virtual bool intWriteFrameFloat(const float* image);
        /** \brief read a new frame into the given array of floating point numbers */
        virtual bool intWriteFrameDouble(const double* data);
        /** \brief read a new frame into the given array of integers */
        virtual bool intWriteFrameUINT32(const uint32_t* image);
        /** \brief read a new frame into the given array of integers */
        virtual bool intWriteFrameUINT16(const uint16_t* image);
        /** \brief read a new frame into the given array of integers */
        virtual bool intWriteFrameUINT8(const uint8_t* data);

        template <typename T>
        bool TIFFWriteSingleFrame(TIFF* tiflocal, const T* image, uint16 frame_channels, uint32 sampleformat) {

            if (!tiflocal) return false;
            if (frames>0) TIFFWriteDirectory(tiflocal);
            frames++;

            uint16 frame_width=width;
            uint16 frame_height=height;
            uint32 rowsperstrip = (uint32)-1;

            TIFFSetField(tiflocal, TIFFTAG_IMAGEWIDTH, frame_width);
            TIFFSetField(tiflocal, TIFFTAG_IMAGELENGTH, frame_height);
            TIFFSetField(tiflocal, TIFFTAG_COMPRESSION, compression);

            TIFFSetField(tiflocal, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
            TIFFSetField(tiflocal, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
            TIFFSetField(tiflocal, TIFFTAG_BITSPERSAMPLE, sizeof(T)*8);
            TIFFSetField(tiflocal, TIFFTAG_SAMPLESPERPIXEL, frame_channels);
            rowsperstrip = TIFFDefaultStripSize(tiflocal, rowsperstrip);
            TIFFSetField(tiflocal, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
            TIFFSetField(tiflocal, TIFFTAG_FILLORDER, FILLORDER_MSB2LSB);
            TIFFSetField(tiflocal,TIFFTAG_SAMPLEFORMAT,sampleformat);
            TIFFSetField(tiflocal,TIFFTAG_ORIENTATION,ORIENTATION_TOPLEFT);

            //if (frames==1 && comment.size()>0) TIFFSetField(tif,TIFFTAG_IMAGEDESCRIPTION,comment.toLatin1().data());

            if (frames==1) TIFFSetField(tiflocal,TIFFTAG_IMAGEDESCRIPTION,qfimdtBuildImageJMetaData(0, deltaX, deltaY, deltaZ, unitname,comment).data());
            float res=1.0/deltaX;
            TIFFSetField(tiflocal,TIFFTAG_XRESOLUTION,res);
            res=1.0/deltaY;
            TIFFSetField(tiflocal,TIFFTAG_YRESOLUTION,res);
            TIFFSetField(tiflocal,TIFFTAG_RESOLUTIONUNIT,RESUNIT_NONE);
            TIFFSetField(tiflocal,TIFFTAG_SOFTWARE,"www.dkfz.de.QuickFit3.Plugins.QFPBasicImageExporters");

            // write frame data
            // data is broken up into strips where each strip contains rowsperstrip complete rows of data
            // each stript then has a size of rowsperstrip*frame_width pixels. the last strip is possibly
            // smaller, so it is NOT padded with dummy data.
            T* const buf = (T*)_TIFFmalloc(TIFFStripSize(tiflocal)); // data buffer for a strip of the image
            for (unsigned int row = 0; (row<frame_height); row+=rowsperstrip) {
                // compute rows in this strip:
                uint32 nrow = rowsperstrip;
                if ((row + rowsperstrip)>frame_height) {
                    nrow=frame_height-row; // this is the last strip ... and it is a bit smaller! ... it only contains the last rows of the image
                }
                tstrip_t strip = TIFFComputeStrip(tiflocal,row,0);
                tsize_t bi = 0;
                // go through the fraem row-wise
                for (unsigned int rr = 0; rr<nrow; ++rr) {
                    for (unsigned int cc = 0; cc<frame_width; ++cc) { // go through all pixels in the current row
                        for (unsigned int co=0; co<frame_channels; co++ ) { // go through all channels, so we have order RGBRGBRGB ...
                            buf[bi++] = (T)image[co*frame_width*frame_height+(row + rr)*frame_width+ cc];
                        }
                    }
                }
                if (TIFFWriteEncodedStrip(tiflocal,strip,buf,bi*sizeof(T))<0) {
                    return false;
                }
            }
            _TIFFfree(buf);
            return true;
        }
    private:
        QList<TIFF*> tif;
        uint64_t frames;
        uint32_t compression;
        QString compression_name;
        bool noDouble;
        bool onefileperchannel;

};

#endif // QFIMAGEWRITERLIBTIFF_H
