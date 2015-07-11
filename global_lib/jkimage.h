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



#ifndef JKIMAGE_H_INCLUDED
#define JKIMAGE_H_INCLUDED

#include <stdint.h>
#include <stdlib.h>
#ifndef NO_LIBTIFF
#  include <tiffio.h>
#endif
#ifdef JKIMAGE_USES_TINYTIFF
#  include "tinytiffwriter.h"
#endif
#include <vector>
#include <iostream>
#include <fstream>
#include <string.h>
#include <locale>
#include "tools.h"
#include "histogram.h"

/**
 * \defgroup jkimage Templated Image Class JKImage
 * \ingroup image_tools
 */

/*! \brief this class provides basic capabilities to manage 2D images.
    \ingroup jkimage

    This class stores data as a 2D array of a given type (templated class!). It implements
    a small set of basic operations for the image which mainly cover memory management,
    initialization and resizing.

 */
template <class T>
class JKImage {
    protected:
        /** \brief array of the data */
        T* d;


        /** \brief indicates whether this data array is managed externally */
        bool is_external;


        /** \brief image width */
        uint32_t iwidth;


        /** \brief image height */
        uint32_t iheight;


        /** \brief just some dummy data */
        T dummy;


    public:
        /** \brief class constructor, constructs an empty (NULL) image */
        inline JKImage() {
            iwidth=0;
            iheight=0;
            d=NULL;
            dummy=0;
            is_external=false;
        };
        /** \brief class constructor, constructs an empty (NULL) image */
        inline JKImage(const JKImage& other) {
            dummy=0;
            is_external=false;
            d=NULL;
            iwidth=0;
            iheight=0;
            resize(other.width(), other.height());
            assign(other);
        };

        /** \brief class constructor, constructs an externally managed image */
        inline JKImage(T* data, uint32_t iwidth, uint32_t iheight) {
            this->iwidth=iwidth;
            this->iheight=iheight;
            this->d=data;
            dummy=0;
            is_external=true;
        };

        /** \brief class constructor, constructs an image of the given size */
        inline JKImage(uint32_t iwidth, uint32_t iheight) {
            this->iwidth=0;
            this->iheight=0;
            this->d=NULL;
            dummy=0;
            is_external=false;
            resize(iwidth, iheight);
        };

        /** \brief class destructor */
        ~JKImage() {
            if (d) {
                if (!is_external) free(d);
                d=NULL;
                iwidth=0;
                iheight=0;
                is_external=false;
            }
        }


        /** \brief returns pointer to the data */
        inline T* data() const { return d; };

        /** \brief returns pointer to a copy of the data */
        inline T* dataCopy() const {
            T* dd=(T*)malloc(iwidth*iheight*sizeof(T));
            memcpy(dd, d, iwidth*iheight*sizeof(T));
            return dd;
        };

        /** \brief returns pointer to a copy of the data */
        template<class Tout>
        inline Tout* dataCopyNewType() const {
            Tout* dd=(Tout*)malloc(iwidth*iheight*sizeof(Tout));
            //std::cout<<"dataCopyNewType w="<<iwidth<<"  h="<<iheight<<"\n";
            for (uint32_t i=0; i<iwidth*iheight; i++) {
                const T dv=d[i];
                dd[i]=dv;
            }
            return dd;
        };

        /** \brief returns pointer to a copy of the data */
        template<class Tout>
        inline Tout* dataCopyNewType(T scale) const {
            Tout* dd=(Tout*)malloc(iwidth*iheight*sizeof(Tout));
            for (uint32_t i=0; i<iwidth*iheight; i++) {
                dd[i]=d[i]*scale;
            }
            return dd;
        };


        /** \brief return image width */
        inline uint32_t width() const { return iwidth; }


        /** \brief return image height */
        inline uint32_t height() const { return iheight; }

        /** \brief return whether this image is from externally managed memory */
        inline bool isExternal() const { return is_external; }


        /** \brief access image pixel */
        inline T& operator()(uint32_t x, uint32_t y) {
            if ((x<iwidth) && (y<iheight)) {
                return d[y*iwidth+x];
            }
            return dummy;
        };

        /** \brief access image pixel */
        inline const T& operator()(uint32_t x, uint32_t y) const {
            return const_cast<JKImage<T>*>(this)->operator()(x,y);
        };


        /** \brief access image pixel */
        inline T get(uint32_t x, uint32_t y) const {
            if ((x<iwidth) && (y<iheight)) {
                return d[y*iwidth+x];
            }
            return dummy;
        };

        inline void setAt(uint32_t x, uint32_t y, T v) {
            if ((x<iwidth) && (y<iheight)) {
                d[y*iwidth+x]=v;
            }
        }


        /** \brief access image pixel */
        inline T& operator()(uint32_t i) {
            if ((i<iwidth*iheight)&&(i>=0)) {
                return d[i];
            }
            return dummy;
        };

        /** \brief access to a specified line */
        inline T* line(uint32_t y) const {
            if ((y<iheight)) {
                return &(d[y*iwidth]);
            }
            return dummy;
        }


        /** \brief resize the image to a given size */
        inline void resize(uint32_t newwidth, uint32_t newheight) {
            if ((newwidth==iwidth) && (newheight==iheight)) return;
            if (d && !is_external) free(d);
            d=NULL;
            iwidth=newwidth;
            iheight=newheight;
            d=(T*)calloc(newwidth*newheight,sizeof(T));
            is_external=false;
        }


        /** \brief resize the image to a given size and set all pixels to the given value */
        inline void assign(uint32_t newwidth, uint32_t newheight, T value=0) {
            resize(newwidth, newheight);
            for (register uint32_t i=0; i< iwidth*iheight; i++) {
                d[i]=value;
            }
        };


        /*! \brief copy data from the given array into the image
            \param newimage points to the new image, this has to be of size \c newwidth*newheight*sizeof(T)
            \param newwidth width of \a newimage
            \param newheight height of \a newimage

            \note The data is converted to the datatype of this class when copied!!! This uses the implicit C++ type casting.
         */
        template <class t>
        inline void assign(t* newimage, uint32_t newwidth, uint32_t newheight) {
            resize(newwidth, newheight);
            for (uint32_t i=0; i< iwidth*iheight; i++) {
                d[i]=newimage[i];
            }
        }


        /** \brief set all pixels to the given value */
        inline void setAll(T value=0) {
            for (uint32_t i=0; i< iwidth*iheight; i++) {
                d[i]=value;
            }
        }
        /** \brief scale image by a factor */
        inline void scale(T factor) {
            for (uint32_t i=0; i< iwidth*iheight; i++) {
                d[i]=factor*d[i];
            }
        }
        /** \brief add a constant to the image */
        inline void add(T adder) {
            for (uint32_t i=0; i< iwidth*iheight; i++) {
                d[i]=adder+d[i];
            }
        }
        /** \brief subtract a constant from the image */
        inline void subtract(T adder) {
            for (uint32_t i=0; i< iwidth*iheight; i++) {
                d[i]=d[i]-adder;
            }
        }

        /** \brief set all pixels to 0 */
        inline void clear() {
            setAll(0);
        }


        /** \brief resize the image to a given size and set all pixels to the given value */
        template <class t>
        inline void assign(const JKImage<t>& img) {
            resize(img.width(), img.height());
            const t* d1=img.data();
            for (register uint32_t i=0; i< iwidth*iheight; i++) {
                d[i]=d1[i];
            }
        }



        /** \brief subtract the given image from the current */
        template <class t>
        inline void subtract(JKImage<t>& img) {
            if ((iwidth==img.width()) && (iheight==img.height()) && d) {
                const t* d1=img.data();
                for (register uint32_t i=0; i< iwidth*iheight; i++) {
                    d[i] = d[i]-d1[i];
                }
            }
        }


        /** \brief copy the given line to a new array and possibly load an increasing array of values in lineX */
        template <class t>
        inline void copyLine(uint32_t y, t* line, t* lineX=NULL) {
            if (lineX) {
                for (register uint32_t i=0; i< iwidth; i++) {
                    line[i] = d[y*iwidth+i];
                    lineX[i] = i;
                }
            } else {
                for (register uint32_t i=0; i< iwidth; i++) {
                    line[i] = d[y*iwidth+i];
                }
            }

        }

        /** \brief copy the given column to a new array and possibly load an increasing array of values in lineX */
        template <class t>
        inline void copyColumn(uint32_t x, t* line, t* lineX=NULL) const {
            if (lineX) {
                for (register uint32_t i=0; i< iheight; i++) {
                    line[i] = d[i*iwidth+x];
                    lineX[i] = i;
                }
            } else {
                for (register uint32_t i=0; i< iheight; i++) {
                    line[i] = d[i*iwidth+x];
                }
            }

        }


        /** \brief copy the given line to a new array and possibly load an increasing array of values in lineX, reverses the pixel order */
        template <class t>
        inline void copyLineReverse(uint32_t y, t* line, t* lineX=NULL) const {
            if (lineX) {
                for (register uint32_t i=0; i< iwidth; i++) {
                    line[i] = d[y*iwidth+iwidth-1-i];
                    lineX[i] = i;
                }
            } else {
                for (register uint32_t i=0; i< iwidth; i++) {
                    line[i] = d[y*iwidth+iwidth-1-i];
                }
            }

        }

        /** \brief copy the given column to a new array and possibly load an increasing array of values in lineX, reverses the pixel order */
        template <class t>
        inline void copyColumnReverse(uint32_t x, t* line, t* lineX=NULL) const {
            if (lineX) {
                for (register uint32_t i=0; i< iheight; i++) {
                    line[i] = d[(iheight-1-i)*iwidth+x];
                    lineX[i] = i;
                }
            } else {
                for (register uint32_t i=0; i< iheight; i++) {
                    line[i] = d[(iheight-1-i)*iwidth+x];
                }
            }

        }




        /** \brief copy a sum over all linse to a new array and possibly load an increasing array of values in lineX */
        template <class t>
        inline void copyLineSum(t* line, t* lineX=NULL) const {
            if (lineX) {
                for (register uint32_t i=0; i< iwidth; i++) {
                    line[i] = 0;
                    lineX[i] = i;
                }
                for (register uint32_t y=0; y< iheight; y++)
                    for (register uint32_t i=0; i< iwidth; i++) {
                        line[i] += d[y*iwidth+i];
                    }
            } else {
                for (register uint32_t i=0; i< iwidth; i++) {
                    line[i] = 0;
                }
                for (register uint32_t y=0; y< iheight; y++)
                    for (register uint32_t i=0; i< iwidth; i++) {
                        line[i] += d[y*iwidth+i];
                    }
            }

        }

        /** \brief copy a sum over all columns to a new array and possibly load an increasing array of values in lineX */
        template <class t>
        inline void copyColumnSum(t* line, t* lineX=NULL) const {
            if (lineX) {
                for (register uint32_t i=0; i< iheight; i++) {
                    line[i] = 0;
                    lineX[i] = i;
                }
                for (register uint32_t x=0; x< iwidth; x++)
                    for (register uint32_t i=0; i< iheight; i++) {
                        line[i] += d[i*iwidth+x];
                    }
            } else {
                for (register uint32_t i=0; i< iheight; i++) {
                    line[i] = 0;
                }
                for (register uint32_t x=0; x< iwidth; x++)
                    for (register uint32_t i=0; i< iheight; i++) {
                        line[i] += d[i*iwidth+x];
                    }
            }

        }


        /** \brief copy a sum over all lines to a new array and possibly load an increasing array of values in lineX, reverses the pixel order */
        template <class t>
        inline void copyLineSumReverse(t* line, t* lineX=NULL) {
            if (lineX) {
                for (register uint32_t i=0; i< iheight; i++) {
                    line[i] = 0;
                    lineX[i] = i;
                }
                for (register uint32_t y=0; y< iheight; y++)
                    for (register uint32_t i=0; i< iwidth; i++) {
                        line[i] += d[y*iwidth+iwidth-1-i];
                    }
            } else {
                for (register uint32_t i=0; i< iheight; i++) {
                    line[i] = 0;
                }
                for (register uint32_t y=0; y< iheight; y++)
                    for (register uint32_t i=0; i< iwidth; i++) {
                        line[i] += d[y*iwidth+iwidth-1-i];
                    }
            }

        }

        /** \brief copy a sum over all columns to a new array and possibly load an increasing array of values in lineX, reverses the pixel order */
        template <class t>
        inline void copyColumnSumReverse(t* line, t* lineX=NULL) const {
            if (lineX) {
                for (register uint32_t i=0; i< iwidth; i++) {
                    line[i] = 0;
                    lineX[i] = i;
                }
                for (register uint32_t x=0; x< iwidth; x++)
                    for (register uint32_t i=0; i< iheight; i++) {
                        line[i] += d[(iheight-1-i)*iwidth+x];
                    }
            } else {
                for (register uint32_t i=0; i< iwidth; i++) {
                    line[i] = 0;
                }
                for (register uint32_t x=0; x< iwidth; x++)
                    for (register uint32_t i=0; i< iheight; i++) {
                        line[i] += d[(iheight-1-i)*iwidth+x];
                    }
            }

        }












        /** \brief copy a sum over all linse to a new array and possibly load an increasing array of values in lineX */
        template <class t>
        inline void copyLineAverage(t* line, t* lineX=NULL) const {
            if (!line) return;
            if (lineX) {
                for (register uint32_t i=0; i< iwidth; i++) {
                    line[i] = 0;
                    lineX[i] = i;
                }
                for (register uint32_t y=0; y< iheight; y++)
                    for (register uint32_t i=0; i< iwidth; i++) {
                        line[i] += d[y*iwidth+i];
                    }
            } else {
                for (register uint32_t i=0; i< iwidth; i++) {
                    line[i] = 0;
                }
                for (register uint32_t y=0; y< iheight; y++)
                    for (register uint32_t i=0; i< iwidth; i++) {
                        line[i] += d[y*iwidth+i];
                    }
            }
            for (register uint32_t i=0; i< iwidth; i++) {
                line[i] /= iheight;
            }

        }

        /** \brief copy a sum over all columns to a new array and possibly load an increasing array of values in lineX */
        template <class t>
        inline void copyColumnAverage(t* line, t* lineX=NULL) const {
            if (!line) return;
            if (lineX) {
                for (register uint32_t i=0; i< iheight; i++) {
                    line[i] = 0;
                    lineX[i] = i;
                }
                for (register uint32_t x=0; x< iwidth; x++)
                    for (register uint32_t i=0; i< iheight; i++) {
                        line[i] += d[i*iwidth+x];
                    }
            } else {
                for (register uint32_t i=0; i< iheight; i++) {
                    line[i] = 0;
                }
                for (register uint32_t x=0; x< iwidth; x++)
                    for (register uint32_t i=0; i< iheight; i++) {
                        line[i] += d[i*iwidth+x];
                    }
            }
            for (register uint32_t i=0; i< iheight; i++) {
                line[i] /= iwidth;
            }

        }


        /** \brief copy a sum over all lines to a new array and possibly load an increasing array of values in lineX, reverses the pixel order */
        template <class t>
        inline void copyLineAverageReverse(t* line, t* lineX=NULL) const {
            if (!line) return;
            if (lineX) {
                for (register uint32_t i=0; i< iwidth; i++) {
                    line[i] = 0;
                    lineX[i] = i;
                }
                for (register uint32_t y=0; y< iheight; y++)
                    for (register uint32_t i=0; i< iwidth; i++) {
                        line[i] += d[y*iwidth+iwidth-1-i];
                    }
            } else {
                for (register uint32_t i=0; i< iwidth; i++) {
                    line[i] = 0;
                }
                for (register uint32_t y=0; y< iheight; y++)
                    for (register uint32_t i=0; i< iwidth; i++) {
                        line[i] += d[y*iwidth+iwidth-1-i];
                    }
            }
            for (register uint32_t i=0; i< iwidth; i++) {
                line[i] /= iheight;
            }

        }

        /** \brief copy a sum over all columns to a new array and possibly load an increasing array of values in lineX, reverses the pixel order */
        template <class t>
        inline void copyColumnAverageReverse(t* line, t* lineX=NULL) const {
            if (!line) return;
            if (lineX) {
                for (register uint32_t i=0; i< iheight; i++) {
                    line[i] = 0;
                    lineX[i] = i;
                }
                for (register uint32_t x=0; x< iwidth; x++)
                    for (register uint32_t i=0; i< iheight; i++) {
                        line[i] += d[(iheight-1-i)*iwidth+x];
                    }
            } else {
                for (register uint32_t i=0; i< iwidth; i++) {
                    line[i] = 0;
                }
                for (register uint32_t x=0; x< iwidth; x++)
                    for (register uint32_t i=0; i< iheight; i++) {
                        line[i] += d[(iheight-1-i)*iwidth+x];
                    }
            }
            for (register uint32_t i=0; i< iheight; i++) {
                line[i] /= iwidth;
            }

        }







        /** \brief return min and max of image values
         *
         * Call it with \code T min, max; min=img.min_max(max); \endcode to obtain
         * the minimum and maximum pixel value in the variables min and max.
         */
        inline T min_max(T& maxx) const {
            register T minn=0;
            maxx=0;
            if (d) {
                minn=maxx=d[0];
                for (register uint32_t i=1; i< iwidth*iheight; i++) {
                    register T t=d[i];
                    if (t>maxx) maxx=t;
                    if (t<minn) minn=t;
                }
            }
            return minn;
        }

        /** \brief return min and max of image values and ignore masked pixels
         *
         * Call it with \code T min, max; min=img.min_max(max); \endcode to obtain
         * the minimum and maximum pixel value in the variables min and max.
         */
        inline T min_max(T& maxx, bool* mask) const {
            register T minn=0;
            maxx=0;
            if (d) {
                bool first=true;
                for (register uint32_t i=1; i< iwidth*iheight; i++) {
                    if (!mask[i]) {
                        if (first) {
                            minn=maxx=d[i];
                            first=false;
                        } else {
                            register T t=d[i];
                            if (t>maxx) maxx=t;
                            if (t<minn) minn=t;
                        }
                    }
                }
            }
            return minn;
        }

        /*! \brief add a border to the image on each side, with the given value
         */
        inline void add_border(T value, int borderwidth=1) {
            T* temp=dataCopy();
            uint32_t w=iwidth;
            uint32_t h=iheight;

            resize(iwidth+2*borderwidth, iheight+2*borderwidth);
            setAll(value);
            for (int i=0; i<h; i++) {
                for (int j=0; j<w; j++) {
                    d[borderwidth+i+(borderwidth+i)*iwidth]=temp[i+j*w];
                }
            }
            free(temp);
        }

        /*! \brief average all border pixels
         */
        template<class Tint>
        inline Tint get_border_average(int borderwidth=1) {
            Tint avg=0;
            Tint count=0;
            for (int i=borderwidth; i<iheight-borderwidth; i++) {
                for (int j=0; j<borderwidth; j++) {
                    avg=avg+d[i*iwidth+j]+d[i*iwidth+(iwidth-j-1)];
                    count=count+2;
                }
            }
            for (int j=0; j<borderwidth; j++) {
                for (int i=0; i<iwidth; i++) {
                    avg=avg+d[j*iwidth+i]+d[j*iwidth+(iheight-i-1)];
                    count=count+2;
                }
            }
            return avg/count;
        }


        /*! \brief return the value of the pixel nearest to the specified coordinates

            if the given coordinate is outside the image range, the value of the nearest rim-pixel is returned
         */
        inline T get_nearest(double x, double y) const {
            if (!d) return 0;
            long ix1=floor(x);
            long iy1=floor(y);
            if (ix1<0) ix1=0;
            if (ix1>=iwidth) ix1=iwidth-1;
            if (iy1<0) iy1=0;
            if (iy1>=iheight) iy1=iheight-1;
            return d[iy1*iwidth+ix1];
        }

        /*! \brief do a linear interpolation of the given data
\verbatim
                                             * (x2,v2)

                  * (x1, v1)


            ------+--------------------------+------------>  X
                  x1                         x2
\enbverbatim
         */
        template <class Tint>
        static inline Tint linear_interpolation(double x1, double v1, double x2, double v2, double x)  {
            return v1+(v2-v1)*(x-x1)/(x2-x1);
        }

        /*! \brief do a linear interpolation of a given coordinate

            if the given coordinate is outside the image range, the value of the nearest rim-pixel is returned
         */
        template <class Tint>
        inline Tint bilinear_interpolate(double x, double y) const {
            long ix1=floor(x);
            long ix2=ceil(x);
            if (ix1==ix2) ix2=ix1+1;
            long iy1=floor(y);
            long iy2=ceil(y);
            if (iy1==iy2) iy2=iy1+1;
            if (ix1>=0 && ix2>=0 && iy1>=0 && iy2>=0
                && ix1<int64_t(iwidth) && ix2<int64_t(iwidth) && iy1<int64_t(iheight) && iy2<int64_t(iheight) ) {

                /* +--> x
                   |
                 y V   v11 --------- P1 ----------------- v21
                        |            |                     |
                        |            |                     |
                        |            |                     |
                        |            |                     |
                        |          (x,y)                   |
                        |            |                     |
                        |            |                     |
                       v12 --------- P2 ----------------- y22

                */
                const T v11=d[ix1+iy1*iwidth];
                const T v12=d[ix1+iy2*iwidth];
                const T v21=d[ix2+iy1*iwidth];
                const T v22=d[ix2+iy2*iwidth];
                const Tint P1=linear_interpolation<Tint>(ix1, v11, ix2, v21, x);
                const Tint P2=linear_interpolation<Tint>(ix1, v12, ix2, v22, x);
                return linear_interpolation<Tint>(iy1, P1, iy2, P2, y);
            } else {
                return get_nearest(x, y);
            }
        }


        /*! \brief calculate the statistics (& optionally a histogram) of the given image

            This function takes the given image and a given mask which both have to be of size \c image_width*image_height
            and calculates the image statistics (sum of pixel values, average pixel value, stadard deviation, minimum-
            and maximum pixel value). If the histogram_... pointers are provided the function also calculates a histogram
            of the pixel values.

            \param[in] image points to the input image
            \param[in] mask points to the input image mask (for broken pixels)
            \param[in] image_width width of the image in pixels
            \param[in] image_height height of the image in pixels
            \param[out] brokenPixels returns the number of broken pixels, i.e. the number of \c true pixels in mask)
            \param[out] sum sum of all pixel values \f$ \sum\limits_{<x,y>}g(x,y) \f$
            \param[out] mean sum of all pixel values \f$ \frac{1}{\mbox{imagewidth}\cdot\mbox{imageheight}}\cdot\sum\limits_{<x,y>}g(x,y) \f$
            \param[out] stddev standard deviation of all pixel values \f$ sqrt{\frac{1}{\mbox{imagewidth}\cdot\mbox{imageheight}-1}\cdot\left[\left(\sum\limits_{<x,y>}g(x,y)^2\right)-\mbox{imagewidth}\cdot\mbox{imageheight}\cdot\mbox{mean}\right]} \f$
            \param[out] imin returns the smallest pixel value
            \param[out] imax returns the biggest pixel value
            \param[in,out] histogram_x points to an array of the histogram x-values. This could point to a new array afterwards, as the function internally calls realloc() !)
            \param[in,out] histogram_y points to an array of the histogram y-values. This could point to a new array afterwards, as the function internally calls realloc() !)
            \param[in,out] histogram_n points to the number of histogram bins. If histogram_autoscale is \c true this could be changed to a new values and histogram_x/histogram_y will be realloced
            \param[out] histogram_min returns the smallest histogram x-value
            \param[out] histogram_max returns the biggest histogram x-value
            \param[out] histogram_fmax returns the biggest histogram y-value
            \param[in] histogram_autoscale if \c true the number of histogram bins is given by the range of pixel values

        */
        inline void calcImageStatistics(bool* mask, uint32_t* brokenPixels, double* sum, double* mean, double* stddev, T* imin, T* imax, double** histogram_x=NULL, double** histogram_y=NULL, uint32_t* histogram_n=NULL, double* histogram_min=NULL, double* histogram_max=NULL, double* histogram_fmax=NULL, bool histogram_autoscale=false) const {
            T* image=d;
            uint32_t image_width=iwidth;
            uint32_t image_height=iheight;
            if (!image || !mask || !brokenPixels || !sum || !mean || !stddev || !imin || !imax || image_width<=0 || image_height<=0) return;
            register T iimin=image[0];
            register T iimax=image[0];
            //*imin=image[0];
            //*imax=image[0];
            //register bool first=true;
            register double sum2=0;
            register double ssum=0;
            //*brokenPixels=0;
            uint32_t bp=0;
            uint32_t loopsize=image_width*image_height;
            if (loopsize<=0) return;
            // calculate sum of grayvalues, sum of squared grayvalues (for std dev.), as well as min and max of image
            // care for the masked pisels!
            iimin=image[0];
            iimax=image[0];
            for (register uint32_t i=0; i<loopsize; i++) {
                if (!mask[i]) {
                    register T im=image[i];
                    if (im<iimin) iimin=im;
                    if (im>iimax) iimax=im;
                    ssum+=im;
                    sum2+=im*im;
                } else bp++;
            }
            *sum=ssum;
            *mean=ssum/(double)(image_width*image_height);
            *stddev=sqrt((sum2-ssum*ssum/(double)loopsize)/(double)(loopsize-1));
            *brokenPixels=bp;
            T hrange=iimax-iimin;

            if (histogram_n && histogram_min && histogram_max && histogram_fmax) {
                uint32_t hn=*histogram_n;
                if (histogram_autoscale && hrange>0) {
                    hn=(uint32_t)std::abs((int32_t)hrange);
                    if (hn<1) { hn=1; }
                    if (hn>100000) { hn=100000; }
                }
                *histogram_n=hn;

                *histogram_x=(double*)realloc(*histogram_x, hn*sizeof(double));
                *histogram_y=(double*)realloc(*histogram_y, hn*sizeof(double));

                double* hx = (*histogram_x);
                double* hy = (*histogram_y);
                /*for (int i=0; i<hn; i++) {
                    hx[i]=i;
                    hy[i]=0;
                }*/
                // set histogram to all 0



                if (hrange==0) {
                    /*for (int i=0; i<hn; i++) {
                        hy[i]=hx[i]=0;
                    }*/
                    *histogram_min=0;
                    *histogram_max=1;
                    *histogram_fmax=1;
                    memset(hx, hn*sizeof(double),0);
                    memset(hy, hn*sizeof(double),0);
                } else {
                    double dh=(double)hrange/(double)hn;
                    double hfmax=0;
                    //memset(hy, hn*sizeof(double),0);
                    for (uint32_t i=0; i<hn; i++) {
                        hy[i]=0;
                        hx[i]=(double)(iimin)+((double)i)*dh;
                    }
                    *histogram_min=(double)(iimin);
                    *histogram_max=(double)(iimin)+(double)(hn-1)*dh;
                    for (register uint32_t i=0; i<loopsize; i++) {
                        if (!mask[i]) {
                            int bin=floor((double)(image[i]-(iimin))/dh);
                            if (bin>=(int64_t)hn) bin=hn-1;
                            if (bin<0) bin=0;
                            const double h=hy[bin]+1;
                            if (h>hfmax) hfmax=h;
                            hy[bin]=h;
                        }
                    }
                    *histogram_fmax=hfmax;

                }
            }
            *imin=iimin;
            *imax=iimax;
        }


        inline bool findDefectivePixels(bool* mask) const {
          bool result=false;
          histogram<T> hist(d,iwidth*iheight);
          double median    = hist.median();
          /*double mean      =*/ hist.get_mean();
          double deviation = hist.get_deviation();


            for (uint32_t i=0; i<iwidth*iheight; i++) {
            //mask[i]=false;
              if((d[i] > (median + 2 * deviation)) || (d[i] < (median - 2 * deviation))){
                result=true;
                mask[i]=true;
              }
            }
            return result;
        }


        /*! \brief calculate the statistics (& optionally a histogram) of the given image

            This function takes the given image and a given mask which both have to be of size \c image_width*image_height
            and calculates the image statistics (sum of pixel values, average pixel value, stadard deviation, minimum-
            and maximum pixel value). If the histogram_... pointers are provided the function also calculates a histogram
            of the pixel values.

            \param[in] image points to the input image
            \param[in] image_width width of the image in pixels
            \param[in] image_height height of the image in pixels
            \param[out] sum sum of all pixel values \f$ \sum\limits_{<x,y>}g(x,y) \f$
            \param[out] mean sum of all pixel values \f$ \frac{1}{\mbox{imagewidth}\cdot\mbox{imageheight}}\cdot\sum\limits_{<x,y>}g(x,y) \f$
            \param[out] stddev standard deviation of all pixel values \f$ sqrt{\frac{1}{\mbox{imagewidth}\cdot\mbox{imageheight}-1}\cdot\left[\left(\sum\limits_{<x,y>}g(x,y)^2\right)-\mbox{imagewidth}\cdot\mbox{imageheight}\cdot\mbox{mean}\right]} \f$
            \param[out] imin returns the smallest pixel value
            \param[out] imax returns the biggest pixel value
            \param[in,out] histogram_x points to an array of the histogram x-values. This could point to a new array afterwards, as the function internally calls realloc() !)
            \param[in,out] histogram_y points to an array of the histogram y-values. This could point to a new array afterwards, as the function internally calls realloc() !)
            \param[in,out] histogram_n points to the number of histogram bins. If histogram_autoscale is \c true this could be changed to a new values and histogram_x/histogram_y will be realloced
            \param[out] histogram_min returns the smallest histogram x-value
            \param[out] histogram_max returns the biggest histogram x-value
            \param[out] histogram_fmax returns the biggest histogram y-value
            \param[in] histogram_autoscale if \c true the number of histogram bins is given by the range of pixel values

        */
        inline void calcImageStatistics(double* sum, double* mean, double* stddev, T* imin, T* imax, double** histogram_x=NULL, double** histogram_y=NULL, uint32_t* histogram_n=NULL, double* histogram_min=NULL, double* histogram_max=NULL, double* histogram_fmax=NULL, bool histogram_autoscale=false) const {
            T* image=d;
            uint32_t image_width=iwidth;
            uint32_t image_height=iheight;
            if (!image  || !sum || !mean || !stddev || !imin || !imax || image_width<=0 || image_height<=0) return;
            register T iimin=image[0];
            register T iimax=image[0];
            //*imin=image[0];
            //*imax=image[0];
            //register bool first=true;
            register double sum2=0;
            register double ssum=0;
            uint32_t loopsize=image_width*image_height;
            if (loopsize<=0) return;
            // calculate sum of grayvalues, sum of squared grayvalues (for std dev.), as well as min and max of image
            // care for the masked pisels!
            iimin=image[0];
            iimax=image[0];
            for (register uint32_t i=0; i<loopsize; i++) {
                register T im=image[i];
                if (im<iimin) iimin=im;
                if (im>iimax) iimax=im;
                ssum+=im;
                sum2+=im*im;
            }
            *sum=ssum;
            *mean=ssum/(double)(image_width*image_height);
            *stddev=sqrt((sum2-ssum*ssum/(double)loopsize)/(double)(loopsize-1));
            T hrange=iimax-iimin;

            if (histogram_n && histogram_min && histogram_max && histogram_fmax && histogram_x && histogram_y) {
                uint32_t hn=*histogram_n;
                if (histogram_autoscale && hrange>0) {
                    hn=(uint32_t)std::abs((int32_t)hrange);
                    if (hn<1) { hn=1; }
                    if (hn>100000) { hn=100000; }
                }
                *histogram_n=hn;

                *histogram_x=(double*)realloc(*histogram_x, hn*sizeof(double));
                *histogram_y=(double*)realloc(*histogram_y, hn*sizeof(double));


                double* hx = (*histogram_x);
                double* hy = (*histogram_y);
                /*for (int i=0; i<hn; i++) {
                    hx[i]=i;
                    hy[i]=0;
                }*/
                // set histogram to all 0



                if (hrange==0) {
                    /*for (int i=0; i<hn; i++) {
                        hy[i]=hx[i]=0;
                    }*/
                    *histogram_min=0;
                    *histogram_max=1;
                    *histogram_fmax=1;
                    memset(hx, hn*sizeof(double),0);
                    memset(hy, hn*sizeof(double),0);
                } else {
                    double dh=(double)hrange/(double)hn;
                    double hfmax=0;
                    //memset(hy, hn*sizeof(double),0);
                    for (uint32_t i=0; i<hn; i++) {
                        hy[i]=0;
                        hx[i]=(double)(iimin)+((double)i)*dh;
                    }
                    *histogram_min=(double)(iimin);
                    *histogram_max=(double)(iimin)+(double)(hn-1)*dh;
                    for (register uint32_t i=0; i<loopsize; i++) {
                        int bin=floor((double)(image[i]-(iimin))/dh);
                        if (bin>=(int64_t)hn) bin=hn-1;
                        if (bin<0) bin=0;
                        double h=hy[bin]+1;
                        if (h>hfmax) hfmax=h;
                        hy[bin]=h;
                    }
                    *histogram_fmax=hfmax;

                }
            }
            *imin=iimin;
            *imax=iimax;
        }

        /*! \brief calculate the statistics (& optionally a histogram) of the given image

            This function takes the given image and a given mask which both have to be of size \c image_width*image_height
            and calculates the image statistics (sum of pixel values, average pixel value, stadard deviation, minimum-
            and maximum pixel value). If the histogram_... pointers are provided the function also calculates a histogram
            of the pixel values.

            \param[in] image points to the input image
            \param[in] mask points to the input image mask (for broken pixels)
            \param[in] image_width width of the image in pixels
            \param[in] image_height height of the image in pixels
            \param[out] brokenPixels returns the number of broken pixels, i.e. the number of \c true pixels in mask)
            \param[out] sum sum of all pixel values \f$ \sum\limits_{<x,y>}g(x,y) \f$
            \param[out] mean sum of all pixel values \f$ \frac{1}{\mbox{imagewidth}\cdot\mbox{imageheight}}\cdot\sum\limits_{<x,y>}g(x,y) \f$
            \param[out] stddev standard deviation of all pixel values \f$ sqrt{\frac{1}{\mbox{imagewidth}\cdot\mbox{imageheight}-1}\cdot\left[\left(\sum\limits_{<x,y>}g(x,y)^2\right)-\mbox{imagewidth}\cdot\mbox{imageheight}\cdot\mbox{mean}\right]} \f$
            \param[out] imin returns the smallest pixel value
            \param[out] imax returns the biggest pixel value
            \param[in,out] histogram_x points to an array of the histogram x-values. This could point to a new array afterwards, as the function internally calls realloc() !)
            \param[in,out] histogram_y points to an array of the histogram y-values. This could point to a new array afterwards, as the function internally calls realloc() !)
            \param[in,out] histogram_n points to the number of histogram bins. If histogram_autoscale is \c true this could be changed to a new values and histogram_x/histogram_y will be realloced
            \param[out] histogram_min returns the smallest histogram x-value
            \param[out] histogram_max returns the biggest histogram x-value
            \param[out] histogram_fmax returns the biggest histogram y-value
            \param[in] histogram_autoscale if \c true the number of histogram bins is given by the range of pixel values

        */
        inline void calcImageStatistics(JKImage<bool> mask, uint32_t* brokenPixels, double* sum, double* mean, double* stddev, T* imin, T* imax, double** histogram_x=NULL, double** histogram_y=NULL, uint32_t* histogram_n=NULL, double* histogram_min=NULL, double* histogram_max=NULL, double* histogram_fmax=NULL, bool histogram_autoscale=false) const {
            calcImageStatistics(mask.data(), brokenPixels, sum, mean, stddev, imin, imax, histogram_x, histogram_y, histogram_n, histogram_min, histogram_max, histogram_fmax, histogram_autoscale);
        }


        /** \brief returns \c true if the size of this image differs from the given \a compareImage */
        template <class t>
        inline bool sizeDiffersFrom(const JKImage<t>& compareImage) const {
            return (width()!=compareImage.width()) || (height()!=compareImage.height());
        }

        /** \brief save image as a list of all pixels that are != 0 */
        inline bool save_nonzero_list(std::string filename) const {
            T* data=d;
            uint32_t width=iwidth;
            uint32_t height=iheight;
            FILE* f=fopen(filename.c_str(), "w");
            if (!f) return false;

            for (uint32_t y=0; y<height; y++) {
                for (uint32_t x=0; x<width; x++) {
                    if (data[y*width+x]!=0) fprintf(f,"%ld, %ld\n", (long)x, (long)y);
                }
            }
            fclose(f);
            return true;
        }

        /** \brief save image as a list of all pixels that are == 0 */
        inline bool save_zero_list(std::string filename) const {
            T* data=d;
            uint32_t width=iwidth;
            uint32_t height=iheight;
            FILE* f=fopen(filename.c_str(), "w");
            if (!f) return false;

            for (uint32_t y=0; y<height; y++) {
                for (uint32_t x=0; x<width; x++) {
                    if (data[y*width+x]==0) fprintf(f,"%ld, %ld\n", (long)x, (long)y);
                }
            }
            fclose(f);
            return true;
        }

        /** \brief load image, as stored by save__nonzero_list */
        inline bool load_nonzero_list(std::string filename) {

            setAll(0);

            FILE* f=fopen(filename.c_str(), "r");
            std::vector<double> r=csv_readline(f);
            while (r.size()>0) {
                if (r.size()>=2) {
                    uint32_t x=r[0];
                    uint32_t y=r[1];
                    if ((x>0) && (x<iwidth) && (y>0) && (y<iwidth)) d[y*iwidth+x]=(T)1;
                }
                r=csv_readline(f);
            }
            fclose(f);

            return true;
        }

        /** \brief load image, as stored by save_zero_list */
        inline bool load_zero_list(std::string filename) {

            setAll(1);

            FILE* f=fopen(filename.c_str(), "r");
            std::vector<double> r=csv_readline(f);
            while (r.size()>0) {
                if (r.size()>=2) {
                    int x=r[0];
                    int y=r[1];
                    if (x>0 && x<iwidth && y>0 && y<iwidth) d[y*iwidth+x]=(T)0;
                }
                r=csv_readline(f);
            }
            fclose(f);

            return true;
        }


        /** \brief save image as a Comma-Separated Values file */
        inline bool save_csv(std::string filename, std::string separator=std::string(", "), char decimalPoint='.') const {
            std::fstream filestr;

            filestr.open (filename.c_str(), std::fstream::out | std::fstream::app);
            if (!filestr.fail()) {
                filestr<<to_csv(separator, decimalPoint).c_str();
            }

            filestr.close();
            return true;
        }



        /** \brief save image as a Comma-Separated Values file */
        inline std::string to_csv(std::string separator=std::string(", "), char decimalPoint='.') const {
            T* data=d;
            uint32_t width=iwidth;
            uint32_t height=iheight;
            std::string output="";
            output+=format("# width = %ld\n", (long)width);
            output+=format("# height = %ld\n", (long)height);
            output+=format("\n\n");

            for (uint32_t y=0; y<height; y++) {
                for (uint32_t x=0; x<width; x++) {
                    if (x>0) output+=separator;
                    double d=data[y*width+x];
                    //fprintf(f,"%lg", d);
                    char dat[128];
                    sprintf(dat, "%lg", d);
                    // change decimal point
                    if (decimalPoint!='.') {
                      if (strlen(dat)>0) for (size_t i=0; i<strlen(dat); i++) {
                        if (dat[i]=='.') dat[i]=decimalPoint;
                      }
                    }
                    output+=dat;
                }
                output+="\n";
            }
            return output;
        }


        /** \brief save image as a column/row (depending on separator) of values */
        inline std::string to_csv_column(std::string separator=std::string("\n"), char decimalPoint='.') const {
            T* data=d;
            uint32_t width=iwidth;
            uint32_t height=iheight;
            std::string output="";

            for (uint32_t y=0; y<height; y++) {
                for (uint32_t x=0; x<width; x++) {
                    if (x>0 || y>0) output+=separator;
                    double d=data[y*width+x];
                    //fprintf(f,"%lg", d);
                    char dat[128];
                    sprintf(dat, "%lg", d);
                    // change decimal point
                    if (decimalPoint!='.') {
                      if (strlen(dat)>0) for (size_t i=0; i<strlen(dat); i++) {
                        if (dat[i]=='.') dat[i]=decimalPoint;
                      }
                    }
                    output+=dat;
                }
            }
            return output;
        }

        /** \brief write image as a Matlab script into the returned string */
        inline std::string to_matlab(std::string varname=std::string("image")) const {
            T* data=d;
            uint32_t width=iwidth;
            uint32_t height=iheight;
            std::locale classicloc("C");
            std::stringstream ss (stringstream::out);
            ss.imbue(classicloc);
            if (varname.size()<=0) ss<<"[ ";
            else ss<<varname.c_str()<<" = [ ";
            for (uint32_t y=0; y<height; y++) {
                for (uint32_t x=0; x<width; x++) {
                    ss<<data[y*width+x]<<" ";
                }
                if (y<height-1) ss<<"; ";
            }
            ss<<" ];\n";
            return ss.str();
        }


        /** \brief save image as a Matlab script */
        inline bool save_matlab(std::string filename, std::string varname=std::string("image")) const {
            std::fstream filestr;

            filestr.open (filename.c_str(), std::fstream::out | std::fstream::app);
            if (!filestr.fail()) {
                filestr<<to_matlab(varname);
            }

            filestr.close();
            return true;
        }

        /** \brief save image as a SYLK file */
        inline bool save_sylk(std::string filename) const {
            T* data=d;
            long int width=iwidth;
            long int height=iheight;

            FILE* f;
            f=fopen(filename.c_str(), "w");
            if (!f) return false;
            fprintf(f, "ID;Pjkimage.h\n");
            fprintf(f, "B;X%ld;Y%ld\n", width, height);

            for (long int y=0; y<height; y++) {
                for (long int x=0; x<width; x++) {
                    double d=data[y*width+x];
                    fprintf(f, "C;X%ld;Y%ld;K%lf\n", x+1,y+1,d);
                }
                fputc('\n', f);
            }
            fprintf(f, "E\n");
            return true;
        }
#ifdef JKIMAGE_USES_TINYTIFF
        /** \brief save the image as TIFF with dataformat float (32 bit) */
        inline bool save_tinytifffloat(std::string filename) const {
            uint32_t width=iwidth;
            uint32_t height=iheight;

            TinyTIFFFile* tif=TinyTIFFWriter_open(filename.c_str(), sizeof(float)*8, width, height);
            if (tif) {
               float* fl= dataCopyNewType<float>();
               if (fl) {
                   TinyTIFFWriter_writeImage(tif, fl);
                   TinyTIFFWriter_close(tif);
               }
               free(fl);
               return true;
            } else return false;
        }
        /** \brief save the image as TIFF with dataformat unsigned int (16 bit) */
        inline bool save_tinytiffuint16(std::string filename) const {
            uint32_t width=iwidth;
            uint32_t height=iheight;

            TinyTIFFFile* tif=TinyTIFFWriter_open(filename.c_str(), sizeof(uint16_t)*8, width, height);
            if (tif) {
               uint16_t* fl= dataCopyNewType<uint16_t>();
               TinyTIFFWriter_writeImage(tif, fl);
               TinyTIFFWriter_close(tif);
               free(fl);
               return true;
            } else return false;
        }
        /** \brief save the image as TIFF with dataformat int (16 bit) */
        inline bool save_tinytiffint16(std::string filename) const {
            uint32_t width=iwidth;
            uint32_t height=iheight;

            TinyTIFFFile* tif=TinyTIFFWriter_open(filename.c_str(), sizeof(int16_t)*8, width, height);
            if (tif) {
               int16_t* fl= dataCopyNewType<int16_t>();
               TinyTIFFWriter_writeImage(tif, fl);
               TinyTIFFWriter_close(tif);
               free(fl);
               return true;
            } else return false;
        }

        /** \brief save the image as TIFF with dataformat unsigned int (16 bit). The data is scaled to the range 0..0xFFFF */
        inline bool save_tinytiffuint16scaled(std::string filename) const {
            uint32_t width=iwidth;
            uint32_t height=iheight;

            T mi, ma;
            mi=min_max(ma);

            TinyTIFFFile* tif=TinyTIFFWriter_open(filename.c_str(), sizeof(uint16_t)*8, width, height);
            if (tif) {
               uint16_t* temp=(uint16_t*)calloc(iwidth*iheight, sizeof(uint16_t));
               for (int i=0; i<iwidth*iheight; i++) {
                    temp[i]=uint16_t(0xFFFF*double(d[i]-mi)/double(ma-mi));
               }
               TinyTIFFWriter_writeImage(tif, temp);
               TinyTIFFWriter_close(tif);
               free(temp);
               return true;
            } else return false;
        }

#endif

#ifdef _TIFFIO_
        /** \brief save the image as TIFF with dataformat float (32 bit) */
        inline bool save_tifffloat(std::string filename) const {
            T* data=d;
            uint32_t width=iwidth;
            uint32_t height=iheight;

            TIFF* tif = TIFFOpen(filename.c_str(), "w");
            if (!tif) return false;


            int sampleperpixel = 1;
            int bitspersample = sizeof(float)*8;
            TIFFSetField(tif,TIFFTAG_IMAGEWIDTH,width);
            TIFFSetField(tif,TIFFTAG_IMAGELENGTH,height);
            TIFFSetField(tif,TIFFTAG_ORIENTATION,ORIENTATION_TOPLEFT);
            TIFFSetField(tif,TIFFTAG_SAMPLESPERPIXEL,sampleperpixel);
            TIFFSetField(tif,TIFFTAG_BITSPERSAMPLE,bitspersample);
            TIFFSetField(tif,TIFFTAG_PLANARCONFIG,PLANARCONFIG_CONTIG);
            TIFFSetField(tif,TIFFTAG_PHOTOMETRIC,PHOTOMETRIC_MINISBLACK);
            TIFFSetField(tif,TIFFTAG_COMPRESSION,COMPRESSION_NONE);
            TIFFSetField(tif,TIFFTAG_FILLORDER,FILLORDER_MSB2LSB);
            TIFFSetField(tif,TIFFTAG_SOFTWARE,"JKImage library");
            TIFFSetField(tif,TIFFTAG_SAMPLEFORMAT,SAMPLEFORMAT_IEEEFP);

            tsize_t linebytes = sampleperpixel * width*bitspersample/8;     // length in memory of one row of pixel in the image.
            unsigned char *buf = NULL;        // buffer used to store the row of pixel information for writing to file
            //    Allocating memory to store the pixels of current row
            if (TIFFScanlineSize(tif)<linebytes)
                buf =(unsigned char *)_TIFFmalloc(linebytes);
            else
                buf = (unsigned char *)_TIFFmalloc(TIFFScanlineSize(tif));
            float* dat=(float*)buf;

            // We set the strip size of the file to be size of one row of pixels
            TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(tif, linebytes));

            //Now writing image to the file one strip at a time
            for (uint32_t y = 0; y < height; y++)
            {
                for (uint32_t i=0; i<width; i++) {
                    dat[i]=data[y*width+i];
                }
                //memcpy(buf, dat, linebytes);    // check the index here, and figure out why not using h*linebytes
                if (TIFFWriteScanline(tif, buf, y, 0) < 0)
                break;
            }

            TIFFClose(tif);
            if (buf) _TIFFfree(buf);
            return true;
        }


        /** \brief save the image as TIFF with dataformat unsiged int 16-bit */
        inline bool save_tiffuint16(std::string filename) const {
            T* data=d;
            uint32_t width=iwidth;
            uint32_t height=iheight;

            TIFF* tif = TIFFOpen(filename.c_str(), "w");
            if (!tif) return false;


            int sampleperpixel = 1;
            int bitspersample = sizeof(uint16_t)*8;
            TIFFSetField(tif,TIFFTAG_IMAGEWIDTH,width);
            TIFFSetField(tif,TIFFTAG_IMAGELENGTH,height);
            TIFFSetField(tif,TIFFTAG_ORIENTATION,ORIENTATION_TOPLEFT);
            TIFFSetField(tif,TIFFTAG_SAMPLESPERPIXEL,sampleperpixel);
            TIFFSetField(tif,TIFFTAG_BITSPERSAMPLE,bitspersample);
            TIFFSetField(tif,TIFFTAG_PLANARCONFIG,PLANARCONFIG_CONTIG);
            TIFFSetField(tif,TIFFTAG_PHOTOMETRIC,PHOTOMETRIC_MINISBLACK);
            TIFFSetField(tif,TIFFTAG_COMPRESSION,COMPRESSION_NONE);
            TIFFSetField(tif,TIFFTAG_FILLORDER,FILLORDER_MSB2LSB);
            TIFFSetField(tif,TIFFTAG_SOFTWARE,"JKImage library");
            TIFFSetField(tif,TIFFTAG_SAMPLEFORMAT,SAMPLEFORMAT_UINT);

            tsize_t linebytes = sampleperpixel * width*bitspersample/8;     // length in memory of one row of pixel in the image.
            unsigned char *buf = NULL;        // buffer used to store the row of pixel information for writing to file
            //    Allocating memory to store the pixels of current row
            if (TIFFScanlineSize(tif)<linebytes)
                buf =(unsigned char *)_TIFFmalloc(linebytes);
            else
                buf = (unsigned char *)_TIFFmalloc(TIFFScanlineSize(tif));
            uint16_t* dat=(uint16_t*)buf;

            // We set the strip size of the file to be size of one row of pixels
            TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(tif, linebytes));

            //Now writing image to the file one strip at a time
            for (uint32_t y = 0; y < height; y++)
            {
                for (uint32_t i=0; i<width; i++) {
                    if (data[y*width+i]<0) dat[i]=0;
                    if (data[y*width+i]>(double)0xFFFF) dat[i]=0xFFFF;
                    else dat[i]=round(data[y*width+i]);
                }
                //memcpy(buf, dat, linebytes);    // check the index here, and figure out why not using h*linebytes
                if (TIFFWriteScanline(tif, buf, y, 0) < 0)
                break;
            }

            if (buf) _TIFFfree(buf);
            TIFFClose(tif);
            return true;
        }


        /** \brief save the image as TIFF with dataformat unsiged int 16-bit */
        inline bool save_tiffuint32(std::string filename) const {
            T* data=d;
            uint32_t width=iwidth;
            uint32_t height=iheight;

            TIFF* tif = TIFFOpen(filename.c_str(), "w");
            if (!tif) return false;


            int sampleperpixel = 1;
            int bitspersample = sizeof(uint32_t)*8;
            TIFFSetField(tif,TIFFTAG_IMAGEWIDTH,width);
            TIFFSetField(tif,TIFFTAG_IMAGELENGTH,height);
            TIFFSetField(tif,TIFFTAG_ORIENTATION,ORIENTATION_TOPLEFT);
            TIFFSetField(tif,TIFFTAG_SAMPLESPERPIXEL,sampleperpixel);
            TIFFSetField(tif,TIFFTAG_BITSPERSAMPLE,bitspersample);
            TIFFSetField(tif,TIFFTAG_PLANARCONFIG,PLANARCONFIG_CONTIG);
            TIFFSetField(tif,TIFFTAG_PHOTOMETRIC,PHOTOMETRIC_MINISBLACK);
            TIFFSetField(tif,TIFFTAG_COMPRESSION,COMPRESSION_NONE);
            TIFFSetField(tif,TIFFTAG_FILLORDER,FILLORDER_MSB2LSB);
            TIFFSetField(tif,TIFFTAG_SOFTWARE,"JKImage library");
            TIFFSetField(tif,TIFFTAG_SAMPLEFORMAT,SAMPLEFORMAT_UINT);

            tsize_t linebytes = sampleperpixel * width*bitspersample/8;     // length in memory of one row of pixel in the image.
            unsigned char *buf = NULL;        // buffer used to store the row of pixel information for writing to file
            //    Allocating memory to store the pixels of current row
            if (TIFFScanlineSize(tif)<linebytes)
                buf =(unsigned char *)_TIFFmalloc(linebytes);
            else
                buf = (unsigned char *)_TIFFmalloc(TIFFScanlineSize(tif));
            uint32_t* dat=(uint32_t*)buf;

            // We set the strip size of the file to be size of one row of pixels
            TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(tif, linebytes));

            //Now writing image to the file one strip at a time
            for (uint32_t y = 0; y < height; y++)
            {
                for (uint32_t i=0; i<width; i++) {
                    if (data[y*width+i]<0) dat[i]=0;
                    if (data[y*width+i]>(double)0xFFFFFFFF) dat[i]=0xFFFFFFFF;
                    else dat[i]=round(data[y*width+i]);
                }
                //memcpy(buf, dat, linebytes);    // check the index here, and figure out why not using h*linebytes
                if (TIFFWriteScanline(tif, buf, y, 0) < 0)
                break;
            }

            if (buf) _TIFFfree(buf);
            TIFFClose(tif);
            return true;
        }
#endif
};


#endif // JKIMAGE_H_INCLUDED
