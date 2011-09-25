#ifndef QFRDRIMAGEREADER_H
#define QFRDRIMAGEREADER_H

#include <stdint.h>
#include <QString>

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
    .
*/
class QFRDRImageReader {
    public:
        virtual ~QFRDRImageReader()  {}
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
        virtual uint16_t frameWidth()=0;
        /** \brief return the height of the frames (valid after open() returned \c true */
        virtual uint16_t frameHeight()=0;
        /** \brief read a new frame into the given array of floating point numbers */
        virtual bool readFrameFloat(float* data)=0;
        /** \brief read a new frame into the given array of integers */
        virtual bool readFrameUINT16(uint16_t* data)=0;
        /** \brief return a description of the last error that occured */
        QString lastError() const {
            return err;
        }
        /** \brief return a filter string for the file format */
        virtual QString filter() const =0;
        /** \brief return a name string for the file format */
        virtual QString formatName() const =0;
    protected:
        void setLastError(QString err) {
            this->err=err;
        }

    private:
        QString err;
};



#endif // QFRDRIMAGEREADER_H
