/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/



/**
 * \defgroup image_tools tool functions for images/arrays
 * \ingroup tools
 *
 * Functions in this group allow to analyze images and provid ebasic capabilitiesto work with images and arrays
 */

/** \file image_tools.h
 *  \ingroup image_tools
 */

#include <stdint.h>
#include <cmath>
#include "lib_imexport.h"

#ifndef IMAGE_TOOLS_H_INCLUDED
#define IMAGE_TOOLS_H_INCLUDED

/*! \brief calculate the histogram of an image/array
    \ingroup image_tools

    \param[in] image the image/array to use as input
    \param[in] size size of \a image
    \param[out] histogram the memory location where the histogram should be written to. Note that this has to point to an array with sufficiently many
                     item, i.e. 256 for an 8-Bit array, 65536 for an 16-bit array ...
    \param[out] hist_min may be used to return the first histogramchannel != 0
    \param[out] hist_max may be used to return the last histogramchannel != 0
 */
template <typename T>
inline void calc_histogram(const T* image, const uint32_t size, uint32_t* histogram, uint32_t* hist_min=0, uint32_t* hist_max=0) {
    register uint32_t idx=0;
    register uint32_t histsize=256;
    for (unsigned int i=1; i<sizeof(T); i++) histsize*=256;
    for (uint32_t i=0; i<histsize; i++) histogram[i]=0;
    for (idx=0; idx<size; idx++) {
        histogram[image[idx] ]++;
    }

    if (hist_min || hist_max) {
        uint32_t hmin=histsize-1;
        uint32_t hmax=0;
        bool minf=false;
        for (uint32_t i=0; i<histsize; i++) {
            if (histogram[i]>0) {
                hmax=i;
                if (!minf) {
                    hmin=i;
                    minf=true;
                }
            }
        }
        if (hist_max) *hist_max=hmax;
        if (hist_min) *hist_min=hmin;
    }
}

/*! \brief calculate the statistics of an image/array
    \ingroup image_tools

    \param[in] image the image/array to use as input
    \param[in] size size of \a image
    \param[out] mean average value
    \param[out] stddev standard deviation
    \param[out] min smallest value
    \param[out] max largest value
    \param[out] sum sum over all elements
    \param[out] sum2 sum over all squared elements
 */
template <typename T, typename Tsums>
inline void calc_statistics(const T* image, const uint32_t size, double* mean, double* stddev=0, T* min=0, T*max=0, Tsums* sum=0, Tsums* sum2=0) {
    T mmin=image[0];
    T mmax=image[0];
    Tsums s=0, s2=0;
    for (register uint32_t i=0; i<size; i++) {
        const register T v=image[i];
        s+=v;
        s2+=v*v;
        if (v>mmax) mmax=v;
        if (v<mmin) mmin=v;
    }
    const double N=(double)size;
    *mean=(double)s/N;
    if (stddev) *stddev=sqrt(1.0/(N-1.0)*(s2-s/N));
    if (min) *min=mmin;
    if (max) *max=mmax;
    if (sum) *sum=s;
    if (sum2) *sum2=s2;

}


#include <stdint.h>

LIB_EXPORT void yuv420_rgb (unsigned char Y1, unsigned char Cb, unsigned char Cr, int *ER, int *EG, int *EB);

LIB_EXPORT void process_image_rgb32 (const uint8_t * videoFrame,  uint8_t* output, int width, int height, int pos_r=1, int pos_g=2, int pos_b=3);

LIB_EXPORT void process_image_rgb24 (const uint8_t * videoFrame,  uint8_t* output, int width, int height, int pos_r=0, int pos_g=1, int pos_b=2);

LIB_EXPORT void process_image_yuyv (const uint8_t * videoFrame,  uint8_t* output, int width, int height, int pos_Y1=0, int pos_U=1, int pos_Y2=2, int pos_V=3);

LIB_EXPORT void process_image_grey (const uint8_t * videoFrame,  uint8_t* output, int width, int height);


#endif // IMAGE_TOOLS_H_INCLUDED
