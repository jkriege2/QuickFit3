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


#ifndef QFIMAGEWRITERRAWDOUBLE_H
#define QFIMAGEWRITERRAWDOUBLE_H
#include "qfpluginexporters.h"
#include "qfexporter.h"
#include "qfexporterimageseries.h"
#include <QFile>

/*! \brief QFExporter class for fit function
    \ingroup qf3exporterplugins_exporters_basicimage

*/
class QFImageWriterRAWDouble: public QFExporterImageSeries {
    public:
        QFImageWriterRAWDouble();
        virtual ~QFImageWriterRAWDouble();
        /*! \copydoc QFFitFunction::filter()   */
        virtual QString filter() const ;
        /*! \copydoc QFFitFunction::formatName()   */
        virtual QString formatName() const ;

        virtual bool open(const QString &filename);
        /** \brief close the currently opened image file */
        virtual void close();

    protected:
        /** \brief read a new frame into the given array of floating point numbers */
        virtual bool intWriteFrameFloat(const float* data);
        /** \brief read a new frame into the given array of floating point numbers */
        virtual bool intWriteFrameDouble(const double* data);
        /** \brief read a new frame into the given array of integers */
        virtual bool intWriteFrameUINT16(const uint16_t* data);
        /** \brief read a new frame into the given array of integers */
        virtual bool intWriteFrameUINT8(const uint8_t* data);
    private:
        QFile file;

};

#endif // QFIMAGEWRITERRAWDOUBLE_H
