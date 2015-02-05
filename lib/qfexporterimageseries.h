/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2014-09-26 12:42:55 +0200 (Fr, 26 Sep 2014) $  (revision $Rev: 3509 $)

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

#ifndef QFExporterImageSeries_H
#define QFExporterImageSeries_H

#include <stdint.h>
#include <QString>
#include <QVariant>
#include "qfexporter.h"

/*! \brief interface for image reader classes
    \ingroup qf3importerplugins

    This class is used to write an image series frame by frame. It is used in this manner:

\code
    create reader;
    writer.setFrameSize(width, height);
    if (writer.open(filename)) {
        for (int i=0; i<frames; i++) {
            data=getFrame();
            writer.writeFrameXXX(&data);
        } while (writer.nextFrame());
        writer.close();
    }
\endcode

    So all frames have to have the same size, which has to be set, using SetFrameSize(), before opening the file!!!
*/
class QFLIB_EXPORT QFExporterImageSeries: public QFExporter {
    public:
        QFExporterImageSeries();
        virtual ~QFExporterImageSeries() {}

        void setFrameSize(uint32_t width, uint32_t height, uint32_t channels);
        /** \brief open the given image sequence file
         *  \param filename name of the image sequence file
         *  \return \c true on success
         */
        virtual bool open(const QString & filename)=0;
        /** \brief close the currently opened image file */
        virtual void close()=0;

        /** \brief read a new frame into the given array of floating point numbers */
        bool writeFrameFloat(const float* data);
        /** \brief read a new frame into the given array of double-precision floating point numbers */
        bool writeFrameDouble(const double *data);
        /** \brief read a new frame into the given array of integers */
        bool writeFrameUINT16(const uint16_t* data);
        /** \brief read a new frame into the given array of integers */
        bool writeFrameUINT8(const uint8_t* data);

        inline uint32_t getFrameWidth()  const {
            return width;
        }
        inline uint32_t getFrameHeight()  const {
            return height;
        }

        inline uint32_t getFrameChannels()  const {
            return channels;
        }

    protected:
        /** \brief read a new frame into the given array of floating point numbers.
         *
         *  \note multi-channel data has to be given in non-interleaved mode (i.e. the frame in ch0, the frame in ch1, ... as one big array!
         */
        virtual bool intWriteFrameFloat(const float* data)=0;
        /** \brief read a new frame into the given array of floating point numbers
         *
         *  \note multi-channel data has to be given in non-interleaved mode (i.e. the frame in ch0, the frame in ch1, ... as one big array!
         */
        virtual bool intWriteFrameDouble(const double* data)=0;
        /** \brief read a new frame into the given array of integers
         *
         *  \note multi-channel data has to be given in non-interleaved mode (i.e. the frame in ch0, the frame in ch1, ... as one big array!
         */
        virtual bool intWriteFrameUINT16(const uint16_t* data)=0;
        /** \brief read a new frame into the given array of integers
         *
         *  \note multi-channel data has to be given in non-interleaved mode (i.e. the frame in ch0, the frame in ch1, ... as one big array!
         */
        virtual bool intWriteFrameUINT8(const uint8_t* data)=0;


    protected:

        uint32_t width;
        uint32_t height;
        uint32_t channels;
};



#endif // QFExporterImageSeries_H
