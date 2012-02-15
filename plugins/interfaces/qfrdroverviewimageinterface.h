#ifndef QFRDROVERVIEWIMAGEINTERFACE_H
#define QFRDROVERVIEWIMAGEINTERFACE_H


#include <QtPlugin>
#include <stdint.h>
#include <QString>
#include <QList>

/*! \brief represents a common data interface for preview images
    \ingroup qf3rdrdp_imaging_fcs

    The functions in this interface allow to expose access to a set of overview images where each
    image may have a different size and name. All images should have the format \c double, so they
    can represent any type of data.

    Also additional geometrical elements may be defined and returned as a list of

*/
class QFRDROverviewImageInterface {
    public:
        enum OverviewImageGeoElementType {
            PIGErectangle,
            PIGEtext,
            PIGEcircle,
            PIGEellipse,
            PIGEline,
            PIGEcross
        };

        /*! \brief represents a geometric element, overlayed to the preview image

            the different shapes use the data memebers as follows:
              - \c PIGErectangle draws a rectangle from (x,y) to (x+width, y+height)
              - \c PIGEtext draws the given text starting at (x,y)
              - \c PIGEcircle draws a circle with radius width around (x,y)
              - \c PIGEellipse draws an ellipse around (x,y) with the half axes width and height
              - \c PIGEline draws a line from (x,y) to (x+width, y+height)
              - \c PIGEcross draws a cross marker at (x,y)
            .
         */
        struct OverviewImageGeoElement {
            OverviewImageGeoElementType type;
            QString title;
            double x;
            double y;
            double width;
            double height;
        };

        virtual ~QFRDROverviewImageInterface() {}

        /** \brief returns the number of available preview images */
        virtual int getPreviewImageCount() const =0;
        /** \brief returns the width of the given image */
        virtual int getPreviewImageWidth(int image) const=0;
        /** \brief returns the height of the given image */
        virtual int getPreviewImageHeight(int image) const=0;
        /** \brief returns the name of the given image */
        virtual QString getPreviewImageName(int image) const=0;

        /*! \brief return a pointer to a uint16_t image of size getDataImageWidth() * getDataImageHeight()

            This image can be used to allow the user to select points in the image, or just as an overview. E.g. for
            imFCS-data this could simply be the averaged intensity of every pixel.

            \return  a uint16_t image of size getDataImageWidth() * getDataImageHeight(), or NULL if no image is available
         */
        virtual double* getPreviewImage(int image) const=0;
        /*! \brief returns a list of geometrix elements overlayn to the image */
        virtual QList<OverviewImageGeoElement> getPreviewImageGeoElements(int image) const=0;

};

Q_DECLARE_INTERFACE( QFRDROverviewImageInterface,
                     "www.dkfz.de.b040.quickfit3.fcsplugin.QFRDROverviewImageInterface/1.0")


#endif // QFRDROVERVIEWIMAGEINTERFACE_H
