#ifndef QFIMAGEREADERLIBTIFF_H
#define QFIMAGEREADERLIBTIFF_H
#include "qfpluginimporters.h"
#include "qfimporter.h"
#include "qfimporterimageseries.h"
#include <tiffio.h>
#include <QMutex>



/*! \brief QFImporter class for TIFF files using libTIFF
    \ingroup qf3importerplugins_importers_basicimages

*/
class QFImageReaderLIBTIFF: public QFImporterImageSeries {
    public:
        QFImageReaderLIBTIFF();
        virtual ~QFImageReaderLIBTIFF();
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
        /** \brief move the reading pointer back to the first frame */
        virtual void reset();
        /** \brief return the number of frames in the files.
         *
         *  This does not change the state of the class. This method may have a long runtime, if it has to really count the frames!
         */
        virtual uint32_t countFrames();
        /** \brief move on to the next frame in the file. return \c false if no further image exists */
        virtual bool nextFrame();


    protected:
        /** \brief return the width of the frames (valid after open() returned \c true */
        virtual uint32_t intFrameWidth();
        /** \brief return the height of the frames (valid after open() returned \c true */
        virtual uint32_t intFrameHeight();
        /** \brief read a new frame into the given array of floating point numbers */
        virtual bool intReadFrameFloat(float* data);
        /** \brief read a new frame into the given array of integers */
        virtual bool intReadFrameUINT16(uint16_t* data);
        virtual bool intReadFrameDouble(double* data);
        uint32_t width;
        uint32_t height;
        TIFF* tif;
        QString filename;

        static void logTIFFMessage(const char* module, const char* fmt, ...)  {
            va_list vl;
            va_start(vl,fmt);
            libTIFFMessageHandler(module, fmt, vl);
            va_end(vl);

        }

        static void logTIFFError(const char* module, const char* fmt, ...)  {
            va_list vl;
            va_start(vl,fmt);
            libTIFFErrorHandler(module, fmt, vl);
            va_end(vl);

        }

        static void logTIFFWarning(const char* module, const char* fmt, ...)  {
            va_list vl;
            va_start(vl,fmt);
            libTIFFWarningHandler(module, fmt, vl);
            va_end(vl);

        }

    public:
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
};

#endif // QFIMAGEREADERLIBTIFF_H
