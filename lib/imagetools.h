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
