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


#ifndef QFQTIMAGEIMPORTERPNG_H
#define QFQTIMAGEIMPORTERPNG_H
#include "qfpluginimporters.h"
#include "qfimporterimageseries.h"
#include <QPixmap>
#include <QImage>




/*! \brief QFImporterImageSeries class for PNG Images, using the Qt image readers
    \ingroup qf3importerplugins_importers_qtimages

*/
class QFQtImageImporterPNG: public QFImporterImageSeries {
    public:
        QFQtImageImporterPNG();
        virtual ~QFQtImageImporterPNG();
        /*! \copydoc QFFitFunction::filter()   */
        virtual QString filter() const ;
        /*! \copydoc QFFitFunction::formatName()   */
        virtual QString formatName() const ;

        /** \brief open the given image sequence file
         *  \param filename name of the image sequence file
         *  \return \c true on success
         */
        virtual bool open(QString filename);
        /** \brief close the currently opened image file */
        virtual void close();
        /** \brief return the number of frames in the files.
         *
         *  This does not change the state of the class. This method may have a long runtime, if it has to really count the frames!
         */
        virtual uint32_t countFrames();
        /** \brief move the reading pointer back to the first frame */
        virtual void reset();
        /** \brief move on to the next frame in the file. return \c false if no further image exists */
        virtual bool nextFrame();

    protected:
        /** \brief return the width of the frames (valid after open() returned \c true */
        virtual uint32_t intFrameWidth();
        /** \brief return the height of the frames (valid after open() returned \c true */
        virtual uint32_t intFrameHeight();
        /** \brief return the number of channels in a frame (valid after open() returned \c true
         *
         *  \note The dfeault implementation of this function returns 1!
         */
        virtual uint32_t intFrameChannels();
        /** \brief read a new frame into the given array of floating point numbers */
        virtual bool intReadFrameFloat(float* data, int channel=0);
        /** \brief read a new frame into the given array of floating point numbers */
        virtual bool intReadFrameDouble(double* data, int channel=0);
        /** \brief read a new frame into the given array of integers */
        virtual bool intReadFrameUINT16(uint16_t* data, int channel=0);

        /** \brief this QImage is used to represent the image-file */
        QImage img;
        /** \brief \c true, if this is a greyscale image */
        bool allGrey;


};

class QFQtImageImporterJPEG: public QFQtImageImporterPNG {
    public:
        QFQtImageImporterJPEG();
        /*! \copydoc QFFitFunction::filter()   */
        virtual QString filter() const ;
        /*! \copydoc QFFitFunction::formatName()   */
        virtual QString formatName() const ;

        /** \brief open the given image sequence file
         *  \param filename name of the image sequence file
         *  \return \c true on success
         */
        virtual bool open(QString filename);
};

class QFQtImageImporterBMP: public QFQtImageImporterPNG {
    public:
        QFQtImageImporterBMP();
        /*! \copydoc QFFitFunction::filter()   */
        virtual QString filter() const ;
        /*! \copydoc QFFitFunction::formatName()   */
        virtual QString formatName() const ;

        /** \brief open the given image sequence file
         *  \param filename name of the image sequence file
         *  \return \c true on success
         */
        virtual bool open(QString filename);
};

class QFQtImageImporterGIF: public QFQtImageImporterPNG {
    public:
        QFQtImageImporterGIF();
        /*! \copydoc QFFitFunction::filter()   */
        virtual QString filter() const ;
        /*! \copydoc QFFitFunction::formatName()   */
        virtual QString formatName() const ;

        /** \brief open the given image sequence file
         *  \param filename name of the image sequence file
         *  \return \c true on success
         */
        virtual bool open(QString filename);
};

class QFQtImageImporterPBM: public QFQtImageImporterPNG {
    public:
        QFQtImageImporterPBM();
        /*! \copydoc QFFitFunction::filter()   */
        virtual QString filter() const ;
        /*! \copydoc QFFitFunction::formatName()   */
        virtual QString formatName() const ;

        /** \brief open the given image sequence file
         *  \param filename name of the image sequence file
         *  \return \c true on success
         */
        virtual bool open(QString filename);
};

class QFQtImageImporterPGM: public QFQtImageImporterPNG {
    public:
        QFQtImageImporterPGM();
        /*! \copydoc QFFitFunction::filter()   */
        virtual QString filter() const ;
        /*! \copydoc QFFitFunction::formatName()   */
        virtual QString formatName() const ;

        /** \brief open the given image sequence file
         *  \param filename name of the image sequence file
         *  \return \c true on success
         */
        virtual bool open(QString filename);
};

class QFQtImageImporterPPM: public QFQtImageImporterPNG {
    public:
        QFQtImageImporterPPM();
        /*! \copydoc QFFitFunction::filter()   */
        virtual QString filter() const ;
        /*! \copydoc QFFitFunction::formatName()   */
        virtual QString formatName() const ;

        /** \brief open the given image sequence file
         *  \param filename name of the image sequence file
         *  \return \c true on success
         */
        virtual bool open(QString filename);
};

#endif // QFQTIMAGEIMPORTERPNG_H
