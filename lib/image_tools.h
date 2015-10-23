/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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

QFLIB_EXPORT void yuv420_rgb (unsigned char Y1, unsigned char Cb, unsigned char Cr, int *ER, int *EG, int *EB);

QFLIB_EXPORT void process_image_rgb32 (const uint8_t * videoFrame,  uint8_t* output, int width, int height, int pos_r=1, int pos_g=2, int pos_b=3);

QFLIB_EXPORT void process_image_rgb24 (const uint8_t * videoFrame,  uint8_t* output, int width, int height, int pos_r=0, int pos_g=1, int pos_b=2);

QFLIB_EXPORT void process_image_yuyv (const uint8_t * videoFrame,  uint8_t* output, int width, int height, int pos_Y1=0, int pos_U=1, int pos_Y2=2, int pos_V=3);

QFLIB_EXPORT void process_image_grey (const uint8_t * videoFrame,  uint8_t* output, int width, int height);


#include "qftools.h"
#include <stdlib.h>
#include "statistics_tools.h"
#include "lib_imexport.h"
#include <QPixmap>

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
                if (nx<nw && ny<nh) data[f*nw*nh+ny*nw+nx]=data[f*nw*nh+ny*nw+nx]+temp[f*width*height+y*width+x];
            }
        }
    }
    qfFree(temp);
}

enum QFImageSymmetry {
    qfisUnknown,
    qfisHorizonal,
    qfisVertical
};

/*! \brief find image symmetry
    \ingroup qf3lib_tools

    This function calculates the left-right and up-down cross-correlation coefficient and returns a symmetry according to which one is larger
 */
template <class T>
inline QFImageSymmetry qfGetImageSymetry(const T* data, int width, int height, double* crossLR=NULL, double* crossTB=NULL) {
    QFImageSymmetry sym=qfisUnknown;
    if (data && width>0 && height>0) {
        int lmin=0;
        int lmax=width/2;
        int delta=lmax;
        if (lmax>=lmin && lmin<width && lmax<width ) {
            double sumX=0, sum2X=0, cnt=0;
            double sumY=0, sum2Y=0;
            double prod=0;
            for (int y=0; y<height; y++) {
                for (int x=lmin; x<=lmax; x++) {
                    const int xx=x+delta;
                    if (x<width && xx<width) {
                        const int idx1=y*width+x;
                        const int idx2=y*width+xx;
                        sumX+=data[idx1];
                        sumY+=data[idx2];
                        sum2X+=data[idx1]*data[idx1];
                        sum2Y+=data[idx2]*data[idx2];
                        prod+=data[idx1]*data[idx2];
                        cnt++;
                    }
                }
            }
            const double cclr=(cnt*prod-sumX*sumY)/sqrt(cnt*sum2X-sumX*sumX)/sqrt(cnt*sum2Y-sumY*sumY);
            if (crossLR) *crossLR=cclr;
            int lmin=0;
            int lmax=height/2;
            int delta=lmax;

            sumX=0;
            sum2X=0;
            cnt=0;
            sumY=0;
            sum2Y=0;
            prod=0;
            for (int y=lmin; y<=lmax; y++) {
                for (int x=0; x<width; x++) {
                    const int yy=y+delta;
                    if (y<height && yy<height) {
                        const int idx1=y*width+x;
                        const int idx2=yy*width+x;
                        sumX+=data[idx1];
                        sumY+=data[idx2];
                        sum2X+=data[idx1]*data[idx1];
                        sum2Y+=data[idx2]*data[idx2];
                        prod+=data[idx1]*data[idx2];
                        cnt++;
                    }
                }
            }
            const double cctb=(cnt*prod-sumX*sumY)/sqrt(cnt*sum2X-sumX*sumX)/sqrt(cnt*sum2Y-sumY*sumY);
            if (crossTB) *crossTB=cctb;
            if (cctb-cclr>0.1) return qfisVertical;
            if (cclr-cctb>0.1) return qfisHorizonal;
        }
    }

    return sym;
}


enum QFImageHalf {
    qfihNone=-1,
    qfihLeft=0,
    qfihRight=1,
    qfihTop=2,
    qfihBottom=3,

    qfihAny=qfihNone
};

/*! \brief copy one half of an image
    \ingroup qf3lib_tools

    input has to be of size inWidth*inHeight, ahereas output is expected to be of size inWidth*inHeight/2!
*/
template <typename T1, typename T2>
inline void qfCopyImageHalf(T1* output, const T2* input, int inWidth, int inHeight, QFImageHalf imageHalf) {
    int width=inWidth;
    int height=inHeight;
    int shiftx=0;
    int shifty=0;
    bool ok=false;
    if (imageHalf==qfihLeft) {
        width=inWidth/2;
        height=inHeight;
        shiftx=0;
        shifty=0;
        ok=true;
    } else if (imageHalf==qfihRight) {
        width=inWidth/2;
        height=inHeight;
        shiftx=width;
        shifty=0;
        ok=true;
    } else if (imageHalf==qfihTop) {
        width=inWidth;
        height=inHeight/2;
        shiftx=0;
        shifty=0;
        ok=true;
    } else if (imageHalf==qfihBottom) {
        width=inWidth;
        height=inHeight/2;
        shiftx=0;
        shifty=height;
        ok=true;
    }
    for (int y=0; y<height; y++) {
        for (int x=0; x<width; x++) {
            const int idxIn=(y+shifty)*inWidth+x+shiftx;
            const int idxOut=y*width+x;
            output[idxOut]=input[idxIn];
        }
    }
}

/*! \brief copy one half of an image into  new allocated memory
    \ingroup qf3lib_tools

    input has to be of size inWidth*inHeight, ahereas output is expected to be of size inWidth*inHeight/2!
*/
template <typename T1, typename T2>
inline void qfAllocCopyImageHalf(T1*& output, const T2* input, int inWidth, int inHeight, QFImageHalf imageHalf) {
    int width=inWidth;
    int height=inHeight;
    int shiftx=0;
    int shifty=0;
    bool ok=false;
    if (imageHalf==qfihLeft) {
        width=inWidth/2;
        height=inHeight;
        shiftx=0;
        shifty=0;
        ok=true;
    } else if (imageHalf==qfihRight) {
        width=inWidth/2;
        height=inHeight;
        shiftx=width;
        shifty=0;
        ok=true;
    } else if (imageHalf==qfihTop) {
        width=inWidth;
        height=inHeight/2;
        shiftx=0;
        shifty=0;
        ok=true;
    } else if (imageHalf==qfihBottom) {
        width=inWidth;
        height=inHeight/2;
        shiftx=0;
        shifty=height;
        ok=true;
    }
    output=(T1*)qfMalloc(width*height*sizeof(T1));
    for (int y=0; y<height; y++) {
        for (int x=0; x<width; x++) {
            const int idxIn=(y+shifty)*inWidth+x+shiftx;
            const int idxOut=y*width+x;
            output[idxOut]=input[idxIn];
        }
    }
}

/*! \brief reorder the data in the given image, so it represents the given half
    \ingroup qf3lib_tools

    the data is reordered inside the image and the new width and height are returned.

 */
template <class T>
inline void qfReduceToImageHalf(QFImageHalf imageHalf, const T* data, int width, int height, int& newwidth, int&newheight) {
    newwidth=width;
    newheight=height;
    int shiftx=0;
    int shifty=0;
    bool ok=false;
    if (imageHalf==qfihLeft) {
        newwidth=width/2;
        newheight=height;
        for (int y=0; y<newheight; y++) {
            for (int x=0; x<newwidth; x++) {
                int idx=y*newwidth+x;
                int oldidx=y*width+x;
                data[idx]=data[oldidx];
            }
        }
        shiftx=0;
        shifty=0;
        ok=true;
    } else if (imageHalf==qfihRight) {
        newwidth=width/2;
        newheight=height;
        shiftx=newwidth;
        shifty=0;
        for (int y=0; y<newheight; y++) {
            for (int x=0; x<newwidth; x++) {
                int idx=y*newwidth+x;
                int oldidx=y*width+x+newwidth;
                data[idx]=data[oldidx];
            }
        }
        ok=true;
    } else if (imageHalf==qfihTop) {
        newwidth=width;
        newheight=height/2;
        for (int y=0; y<newheight; y++) {
            for (int x=0; x<newwidth; x++) {
                int idx=(newheight+y)*newwidth+x;
                int oldidx=y*width+x;
                data[idx]=data[oldidx];
            }
        }
        for (int y=0; y<newheight; y++) {
            for (int x=0; x<newwidth; x++) {
                int idx=y*newwidth+x;
                int oldidx=(y+newheight)*width+x;
                data[idx]=data[oldidx];
            }
        }
        shiftx=0;
        shifty=0;
        ok=true;
    } else if (imageHalf==qfihBottom) {
        newwidth=width;
        newheight=height/2;
        shiftx=0;
        shifty=newheight;
        for (int y=0; y<newheight; y++) {
            for (int x=0; x<newwidth; x++) {
                int idx=y*newwidth+x;
                int oldidx=(y+newheight)*width+x;
                data[idx]=data[oldidx];
            }
        }
        ok=true;
    }
}


QVector<bool> QFLIB_EXPORT resizeBoolVecImage(const QVector<bool>& image, int w, int h, int newWidth, int newHeight, int* askUserResizeMasks=NULL, const QString& image_name=QString("image"), const QString &comment=QString()) ;

template<class T>
inline void qfBinImage(T* img_out,const  T*image, int width, int height, int binning, bool avg=false) {
    //qDebug()<<"qfBinImage"<<img_out<<image<<width<<height<<binning<<avg;
    if (!img_out || !image || width<=0 || height<=0) return;
    const int nw=qMax(1,width/binning);
    const int nh=qMax(1,height/binning);
    if (nw*nh<=0) return;
    //qDebug()<<"qfBinImage"<<nw<<nh;
    for (int i=0; i<nw*nh; i++) {
        img_out[i]=0;
    }
    if (binning<1) return;
    if (binning==1) {
        for (int i=0; i<nw*nh; i++) {
            img_out[i]=image[i];
        }
        return;
    }
    for (int y=0; y<height; y++) {
        const int ny=y/binning;
        for (int x=0; x<width; x++) {
            const int nx=x/binning;
            if (nx<nw && ny<nh) img_out[ny*nw+nx]=img_out[ny*nw+nx]+image[y*width+x];
        }
    }
    if (avg) {
        for (int i=0; i<nw*nh; i++) {
            img_out[i]=img_out[i]/(binning*binning);
        }
    }
}

template<class T>
inline T* qfBinImageCreate(const  T*image, int width, int height, int binning, bool avg=false) {
    if (width*height>0) {
        const int nw=qMax(1,width/binning);
        const int nh=qMax(1,height/binning);
        T* res=(T*)qfMalloc(nw*nh*sizeof(T));
        //qDebug()<<"qfBinImageCreate "<<nw<<nh<<res;
        if (res) qfBinImage(res, image, width, height, binning, avg);
        return res;
    } else {
        return NULL;
    }
}



inline void qfBinMaskImage(bool* img_out,const  bool*image, int width, int height, int binning, bool use_or=true) {
    if (!img_out || !image || width<=0 || height<=0) return;
    const int nw=qMax(1,width/binning);
    const int nh=qMax(1,height/binning);
    if (use_or) {
        for (int i=0; i<nw*nh; i++) {
            img_out[i]=false;
        }
    } else {
        for (int i=0; i<nw*nh; i++) {
            img_out[i]=true;
        }
    }
    if (binning<1) return;
    if (binning==1) {
        for (int i=0; i<nw*nh; i++) {
            img_out[i]=image[i];
        }
        return;
    }
    if (use_or) {
        for (int y=0; y<height; y++) {
            const int ny=y/binning;
            for (int x=0; x<width; x++) {
                const int nx=x/binning;
                img_out[ny*nw+nx]=img_out[ny*nw+nx]|image[y*width+x];
            }
        }
    } else {
        for (int y=0; y<height; y++) {
            const int ny=y/binning;
            for (int x=0; x<width; x++) {
                const int nx=x/binning;
                img_out[ny*nw+nx]=img_out[ny*nw+nx]&image[y*width+x];
            }
        }
    }

}


inline bool* qfBinMaskImageCreate(const  bool*image, int width, int height, int binning, bool use_or=true) {
    const int nw=width/binning;
    const int nh=height/binning;
    bool* res=(bool*)qfMalloc(nw*nh*sizeof(bool));
    qfBinMaskImage(res, image, width, height, binning, use_or);
    return res;
}

#endif // IMAGE_TOOLS_H_INCLUDED
