#ifndef QFIMAGEREADERDIFFUSION4BINARY_H
#define QFIMAGEREADERDIFFUSION4BINARY_H

#include "qfpluginimporters.h"
#include "qfimporter.h"
#include "qfimporterimageseries.h"
#include <stdint.h>
#include <QFile>

/*! \brief QFImporter class for TIFF files using tinyTIFF reader which supports only a simple baseset of TIFF files, but with more frames than LibTIFF (\see QFImageReaderLIBTIFF)
    \ingroup qf3importerplugins_importers_basicimages

*/
class QFImageReaderDiffusion4Binary: public QFImporterImageSeries
{
    public:
        QFImageReaderDiffusion4Binary();
        virtual ~QFImageReaderDiffusion4Binary();



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
        virtual QVariant getFileProperty(const QString& name, const QVariant& defaultValue=QVariant()) const;


    protected:
        /** \brief return the width of the frames (valid after open() returned \c true */
        virtual uint16_t intFrameWidth();
        /** \brief return the height of the frames (valid after open() returned \c true */
        virtual uint16_t intFrameHeight();
        /** \brief read a new frame into the given array of floating point numbers */
        virtual bool intReadFrameFloat(float* data);
        /** \brief read a new frame into the given array of integers */
        virtual bool intReadFrameUINT16(uint16_t* data);
        virtual bool intReadFrameDouble(double* data);
        QFile input;
        QString filename;
        uint16_t width;
        uint16_t height;
        uint64_t frame;
        uint64_t frames;
        double timestep;
        QVector<double> framedata;

    public:
};

#endif // QFIMAGEREADERDIFFUSION4BINARY_H