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

/*! \brief QFExporter class for fit function
    \ingroup qf3exporterplugins_exporters_basicimage

*/
class QFImageWriterLibTIFF: public QFExporterImageSeries {
    public:
        QFImageWriterLibTIFF(uint32_t compression, const QString& compression_name, bool noDouble);
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
        virtual bool intWriteFrameUINT16(const uint16_t* image);
        /** \brief read a new frame into the given array of integers */
        virtual bool intWriteFrameUINT8(const uint8_t* data);
    private:
        TIFF* tif;
        uint64_t frames;
        uint32_t compression;
        QString compression_name;
        bool noDouble;

};

#endif // QFIMAGEWRITERLIBTIFF_H
