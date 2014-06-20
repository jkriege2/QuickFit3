/*
    Copyright (c) 2008-2014 Jan W. Krieger, German Cancer Research Center

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

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

#ifndef IMAGETOOLS_H
#define IMAGETOOLS_H
#include "qftools.h"
#include <stdlib.h>

/*! \brief bin each frame in an image series (video)
    \ingroup qf3lib_tools
 */
template <class T>
inline void qfVideoBinInFrame(T* data, int width, int height, int frames, int binning) {
    if (width<=0 || height<=0 || frames<=0 || !data) return;
    if (binning<=1) return;
    T* temp=duplicateArray(data, width*height*frames);
    const int64_t nw=width/binning;
    const int64_t nh=height/binning;
    for (int64_t i=0; i<width*height*frames; i++) {
        data[i]=0;
    }
    for (int64_t f=0; f<frames; f++) {
        for (int64_t y=0; y<height; y++) {
            const int64_t ny=y/binning;
            for (int64_t x=0; x<width; x++) {
                const int64_t nx=x/binning;
                data[f*nw*nh+ny*nh+nx]=data[f*nw*nh+ny*nh+nx]+temp[f*width*height+y*width+x];
            }
        }
    }
    free(temp);
}


#endif // IMAGETOOLS_H
