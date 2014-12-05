/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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



#ifndef QFRDROVERVIEWIMAGEINTERFACE_H
#define QFRDROVERVIEWIMAGEINTERFACE_H


#include <QtPlugin>
#include <stdint.h>
#include <QString>
#include <QList>



/*! \brief represents a common data interface for additional images
    \ingroup qf3rdrdp_imaging_fcs

    The functions in this interface allow to expose access to a set of overview images where each
    image may have a different size and name. All images should have the format \c double, so they
    can represent any type of data.

    Also additional geometrical elements may be defined and returned as a list of

*/
class QFRDRAdditionalImagesInterface {
    public:
        enum AdditionalImagesGeoElementType {
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
        struct AdditionalImagesGeoElement {
            AdditionalImagesGeoElementType type;
            QString title;
            double x;
            double y;
            double width;
            double height;
        };

        virtual ~QFRDRAdditionalImagesInterface() {}

        /** \brief returns the number of available preview images */
        virtual int getAdditionalImagesCount() const =0;
        /** \brief returns the width of the given image */
        virtual int getAdditionalImagesWidth(int image) const=0;
        /** \brief returns the height of the given image */
        virtual int getAdditionalImagesHeight(int image) const=0;
        /** \brief returns the name of the given image */
        virtual QString getAdditionalImagesName(int image) const=0;

        /*! \brief return a pointer to a double image of size getDataImageWidth() * getDataImageHeight()

            This image can be used to allow the user to select points in the image, or just as an overview. E.g. for
            imFCS-data this could simply be the averaged intensity of every pixel.

            \return  a double image of size getDataImageWidth() * getDataImageHeight(), or NULL if no image is available
         */
        virtual double* getAdditionalImage(int image) const=0;
        /*! \brief returns a list of geometrix elements overlayn to the image */
        virtual QList<AdditionalImagesGeoElement> getAdditionalImagesAnnotations(int image) const=0;

};

Q_DECLARE_INTERFACE( QFRDRAdditionalImagesInterface,
                     "www.dkfz.de.b040.quickfit3.fcsplugin.QFRDRAdditionalImagesInterface/1.0")




















/*! \brief represents a common data interface for overview images
    \ingroup qf3rdrdp_imaging_fcs

    All overview images have to have the same size.

*/
class QFRDROverviewImagesInterface {
    public:

        virtual ~QFRDROverviewImagesInterface() {}

        /** \brief returns the number of available preview images */
        virtual int getOverviewImageCount() const =0;
        /** \brief returns the width of the given image */
        virtual int getOverviewImageWidth() const=0;
        /** \brief returns the height of the given image */
        virtual int getOverviewImageHeight() const=0;
        /** \brief returns the name of the given image */
        virtual QString getOverviewImageName(int image) const=0;

        /*! \brief return a pointer to a double image of size getDataImageWidth() * getDataImageHeight()

            This image can be used to allow the user to select points in the image, or just as an overview. E.g. for
            imFCS-data this could simply be the averaged intensity of every pixel.

            \return  a double image of size getDataImageWidth() * getDataImageHeight(), or NULL if no image is available
         */
        virtual double* getOverviewImage(int image) const=0;


};

Q_DECLARE_INTERFACE( QFRDROverviewImagesInterface,
                     "www.dkfz.de.b040.quickfit3.fcsplugin.QFRDROverviewImagesInterface/1.0")


#endif // QFRDROVERVIEWIMAGEINTERFACE_H
