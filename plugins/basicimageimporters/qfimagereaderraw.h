#ifndef QFIMAGEREADERRAW_H
#define QFIMAGEREADERRAW_H


#include "qfpluginimporters.h"
#include "qfimporter.h"
#include "qfimporterimageseries.h"
#include <tiffio.h>
#include <QMutex>
#include <stdint.h>
#include "qftools.h"
#include "qfimagemetadatatool.h"

/*! \brief QFImporter class for TIFF files using tinyTIFF reader which supports only a simple baseset of TIFF files, but with more frames than LibTIFF (\see QFImageReaderLIBTIFF)
    \ingroup qf3importerplugins_importers_basicimages

*/
template<typename T>
class QFImageReaderRAW: public QFImporterImageSeries
{
    public:
        inline QFImageReaderRAW(const QString& type_name) {
            this->type_name=type_name;
            width=0;
            channels=0;
            height=0;
            rawFile=NULL;
            frame=-1;
            currentFrame=NULL;
        }

        inline virtual ~QFImageReaderRAW() {
            close();
        }



        /*! \copydoc QFFitFunction::filter()   */
        inline virtual QString filter() const  {
            return formatName()+QObject::tr(" (*.raw *.dat *.bin)");
        }
        /*! \copydoc QFFitFunction::formatName()   */
        inline virtual QString formatName() const  {
            return QObject::tr("RAW files [%1]").arg(type_name);
        }

        /** \brief open the given image sequence file
         *  \param filename name of the image sequence file
         *  \return \c true on success
         */
        inline virtual bool open(QString filename) {
            close();
            bool ok=false;
            if (QFile::exists(filename) && QFile::exists(filename+".description.txt")) {
                QSettings set(filename+".description.txt", QSettings::IniFormat);
                width=set.value("width", 0).toInt();
                height=set.value("height", 0).toInt();
                channels=set.value("channels", 1).toInt();
                int bps=set.value("bitspersample", int(sizeof(T)*8)).toInt();
                ok=((bps==sizeof(T)*8)&&(width>0)&&(height>0)&&(channels>0));
            } else if (QFile::exists(filename)) {
                ok=true;
            }
            if (ok) {
                rawFile=new QFile(filename);
                if (rawFile) ok=rawFile->open(QFile::ReadOnly);
                else ok=false;
                if (!ok && rawFile) {
                    width=0;
                    height=0;
                    channels=0;
                    delete rawFile;
                    rawFile=NULL;
                    frame=-1;
                }
            }

            if (ok) {
                this->filename=filename;
                currentFrame=qfMallocT<T>(width*height);
                nextFrame();
            }
            return ok;
        }

        /** \brief close the currently opened image file */
        inline virtual void close() {
            if (currentFrame) qfFree(currentFrame);
            if (rawFile) {
               delete rawFile;
            }
            currentFrame=NULL;
            rawFile=NULL;
            frame=-1;
            width=0;
            height=0;
            channels=0;
        }

        /** \brief move the reading pointer back to the first frame */
        inline virtual void reset() {
            if (rawFile) {
                rawFile->seek(0);
            }
        }

        /** \brief return the number of frames in the files.
         *
         *  This does not change the state of the class. This method may have a long runtime, if it has to really count the frames!
         */
        inline virtual uint32_t countFrames() {
            if (rawFile) {
                return rawFile->size()/(width*height*channels*sizeof(T));
            }
            return 0;
        }

        /** \brief move on to the next frame in the file. return \c false if no further image exists */
        inline virtual bool nextFrame() {
            if (rawFile) {
                if ((uint64_t)rawFile->read((char*)currentFrame, width*height*channels*uint64_t(sizeof(T)))==width*height*channels*uint64_t(sizeof(T))) {
                    frame++;
                    return true;
                }
            }
            return false;
        }


    protected:
        /** \brief return the width of the frames (valid after open() returned \c true */
        inline virtual uint32_t intFrameWidth() {
            return width;
        }

        /** \brief return the height of the frames (valid after open() returned \c true */
        inline virtual uint32_t intFrameHeight() {
            return height;
        }

        inline virtual uint32_t intFrameChannels() {
            return channels;
        }

        /** \brief read a new frame into the given array of floating point numbers */
        inline virtual bool intReadFrameFloat(float* data, int channel=0) {
            if (!currentFrame) return false;
            for (uint64_t i=0; i<width*height; i++) {
                data[i]=currentFrame[channel*width*height+i];
            }
            return true;
        }

        /** \brief read a new frame into the given array of integers */
        inline virtual bool intReadFrameUINT16(uint16_t* data, int channel=0) {
            if (!currentFrame) return false;
            for (uint64_t i=0; i<width*height; i++) {
                data[i]=currentFrame[channel*width*height+i];
            }
            return true;
        }
        inline virtual bool intReadFrameDouble(double* data, int channel=0) {
            if (!currentFrame) return false;
            for (uint64_t i=0; i<width*height; i++) {
                data[i]=currentFrame[channel*width*height+i];
            }
            return true;
        }

        QFile* rawFile;
        QString filename;
        uint32_t width;
        uint32_t height;
        uint32_t channels;
        uint32_t frame;
        QString type_name;
        T* currentFrame;
};

#endif // QFIMAGEREADERRAW_H
