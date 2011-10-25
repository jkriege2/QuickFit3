#ifndef QFRDRIMAGEREADERTIFF_H
#define QFRDRIMAGEREADERTIFF_H

#include "qfrdrimagereader.h"
#include <tiffio.h>

/*! \brief image reader class for TIFF images (implements QFRDRImageReader)
    \ingroup qf3rdrdp_imaging_fcs

    This class is used to read an image series frame by frame. It is used in this manner:
*/
class QFRDRImageReaderTIFF : public QFRDRImageReader {
    public:
        QFRDRImageReaderTIFF();
        ~QFRDRImageReaderTIFF();
        /** \brief open the given image sequence file
         *  \param filename name of the image sequence file
         *  \return \c true on success
         */
        virtual bool open(QString filename);
        /** \brief close the currently opened image file */
        virtual void close();
        /** \brief move the reading pointer back to the first frame */
        virtual void reset();
        /** \brief return the number of frames in the files.
         *
         *  This does not change the state of the class. This method may have a long runtime, if it has to really count the frames!
         */
        virtual uint32_t countFrames();
        /** \brief move on to the next frame in the file. return \c false if no further image exists */
        virtual bool nextFrame();

        /** \brief return a filter string for the file format */
        virtual QString filter() const;
        /** \brief return a name string for the file format */
        virtual QString formatName() const ;
    protected:
        /** \brief return the width of the frames (valid after open() returned \c true */
        virtual uint16_t intFrameWidth();
        /** \brief return the height of the frames (valid after open() returned \c true */
        virtual uint16_t intFrameHeight();
        /** \brief read a new frame into the given array of floating point numbers */
        virtual bool intReadFrameFloat(float* data);
        /** \brief read a new frame into the given array of integers */
        virtual bool intReadFrameUINT16(uint16_t* data);

        uint16_t width;
        uint16_t height;
        TIFF* tif;
        QString filename;
};

#endif // QFRDRIMAGEREADERTIFF_H
