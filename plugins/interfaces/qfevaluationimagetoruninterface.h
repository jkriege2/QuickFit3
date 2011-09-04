#ifndef QFEVALUATIONIMAGETORUNINTERFACE_H
#define QFEVALUATIONIMAGETORUNINTERFACE_H

#include <QtPlugin>
#include <stdint.h>

/*! \brief represents a common data interface that allows to operate on image data (x,y) that is represented by an indexed list ("runs")
    \ingroup qf3rdrdp_imaging_fcs

*/
class QFEvaluationImageToRunInterface {
    public:
        virtual ~QFEvaluationImageToRunInterface() {}

        /** \brief return width of the image */
        virtual int getDataImageWidth() const=0;
        /** \brief return height of the image */
        virtual int getDataImageHeight() const=0;

        /*! \brief return a pointer to a uint16_t image of size getDataImageWidth() * getDataImageHeight()

            This image can be used to allow the user to select points in the image, or just as an overview. E.g. for
            imFCS-data this could simply be the averaged intensity of every pixel.

            \return  a uint16_t image of size getDataImageWidth() * getDataImageHeight(), or NULL if no image is available
         */
        virtual uint16_t* getDataImagePreview() const=0;
        /** \brief convert a pixel coordinate to a rund index */
        virtual int xyToRun(int x, int y) const=0;
        /** \brief convert a run to a pixel x-coordinate */
        virtual int runToX(int run) const=0;
        /** \brief convert a run to a pixel y-coordinate */
        virtual int runToY(int run) const=0;
        /** \brief convert a pixel coordinate to an array index (in correlations and sigmas) */
        virtual int xyToIndex(int x, int y) const=0;

};

Q_DECLARE_INTERFACE( QFEvaluationImageToRunInterface,
                     "www.dkfz.de.b040.quickfit3.fcsplugin.QFEvaluationImageToRunInterface/1.0")

#endif // QFEVALUATIONIMAGETORUNINTERFACE_H
