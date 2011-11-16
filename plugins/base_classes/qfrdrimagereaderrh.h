#ifndef QFRDRIMAGEREADERRH_H
#define QFRDRIMAGEREADERRH_H

#include "qfrdrimagereader.h"
#include "../../../LIB/trunk/frame.h"
#include "../../../LIB/trunk/crc_ccitt.h"
#include <QFile>
#include <QDataStream>
#include <stdint.h>

#define MAX_FRAME_SIZE (64*1024)

/*! \brief image reader class for Radhard raw images (implements QFRDRImageReader)
    \ingroup qf3rdrdp_imaging_fcs

    \note <<<THIS CODE IS NOT PLATFORM INDEPENDENT>>>
    This class is used to read an image series frame by frame. It is used in this manner:
*/
class QFRDRImageReaderRH : public QFRDRImageReader {
    public:
        QFRDRImageReaderRH();
        ~QFRDRImageReaderRH();
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
        QString filename;
        unsigned int frameSize;
        QFile *file;
        unsigned char* buffer;

        unsigned int calculateFrameSize();

        template < typename T >
        bool readFrame_(T* data) {
          if (!file) return false;
          bool result=false;
          frame<T> *f = new frame<T>(data,width,height);
          if(buffer[0]==0xFFU) {
            f->template load_packed<uint32_t>((uint32_t*)&(buffer[4]));
            result=true;
          } else {
            setLastError(QObject::tr("did not find 0xFF at start of frame, found %1").arg(buffer[0],0,16));
          }
          delete f;
          return result;
        }
};

#endif // QFRDRIMAGEREADERRH_H
