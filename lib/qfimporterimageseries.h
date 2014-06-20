/*
    Copyright (c) 2008-2014 Jan W. Krieger, German Cancer Research Center

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

#ifndef QFImporterImageSeries_H
#define QFImporterImageSeries_H

#include <stdint.h>
#include <QString>
#include <QVariant>
#include "qfimporter.h"

/*! \brief interface for image reader classes
    \ingroup qf3rdrdp_imaging_fcs

    This class is used to read an image series frame by frame. It is used in this manner:

\code
    create reader;
    reader.open(filename);
    width=reader.frameWidth();
    height=reader.frameHeight();
    data=array of size width*height
    do {
        reader.readFrame(&data);
    } while (reader.nextFrame());
    reader.close();
\endcode

    The class also allows to set a binning and a cropping which will be applied transparently for the user.
*/
class QFLIB_EXPORT QFImporterImageSeries: public QFImporter {
    public:
        QFImporterImageSeries();
        virtual ~QFImporterImageSeries() {}
        /** \brief open the given image sequence file
         *  \param filename name of the image sequence file
         *  \return \c true on success
         */
        virtual bool open(QString filename)=0;
        /** \brief close the currently opened image file */
        virtual void close()=0;
        /** \brief return the number of frames in the files.
         *
         *  This does not change the state of the class. This method may have a long runtime, if it has to really count the frames!
         */
        virtual uint32_t countFrames()=0;
        /** \brief move the reading pointer back to the first frame */
        virtual void reset()=0;
        /** \brief move on to the next frame in the file. return \c false if no further image exists */
        virtual bool nextFrame()=0;
        /** \brief return the width of the frames (valid after open() returned \c true */
        uint32_t frameWidth();
        /** \brief return the height of the frames (valid after open() returned \c true */
        uint32_t frameHeight();
        /** \brief read a new frame into the given array of floating point numbers */
        bool readFrameFloat(float* data);
        /** \brief read a new frame into the given array of double-precision floating point numbers */
        bool readFrameDouble(double* data);
        /** \brief read a new frame into the given array of integers */
        bool readFrameUINT16(uint16_t* data);
        /** \brief get named properties stored in the file
         *
         *  standardized property names are:
         *    - \c FRAMERATE framerate given in fps
         *    - \c PIXEL_SIZE pixel size given in micrometer
         *  .
         * \returns This function returns the required value, or a default value supplied to the function
         */
        virtual QVariant getFileProperty(const QString& name, const QVariant& defaultValue=QVariant()) const;

        /** \brief set binning */
        void setBinning(int bin) { binning=bin; if (binning<1) binning=1; }
        int getBinning() const { return binning; }
        void setInterleavedBinning(bool bin) { interleavedBinning=bin; }
        bool getInterleavedBinning() const { return interleavedBinning; }
        void setAverageBinning(bool bin) { averageBinning=bin; }
        bool getAverageBinning() const { return averageBinning; }
        /** \brief set cropping */
        void setCropping(int x0, int x1, int y0, int y1);
        void unsetCropping() { crop=false; }

        bool getUseCropping() const { return crop; }
        int getCropX0() const { return x0; }
        int getCropX1() const { return x1; }
        int getCropY0() const { return y0; }
        int getCropY1() const { return y1; }
    protected:
        /** \brief return the width of the frames (valid after open() returned \c true */
        virtual uint32_t intFrameWidth()=0;
        /** \brief return the height of the frames (valid after open() returned \c true */
        virtual uint32_t intFrameHeight()=0;
        /** \brief read a new frame into the given array of floating point numbers */
        virtual bool intReadFrameFloat(float* data)=0;
        /** \brief read a new frame into the given array of floating point numbers */
        virtual bool intReadFrameDouble(double* data)=0;
        /** \brief read a new frame into the given array of integers */
        virtual bool intReadFrameUINT16(uint16_t* data)=0;


    private:

        int binning;
        int x0;
        int x1;
        int y0;
        int y1;
        bool crop;
        bool interleavedBinning;
        bool averageBinning;
};



#endif // QFImporterImageSeries_H
