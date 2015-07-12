/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

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

#ifndef IMAGETOOLS_H
#define IMAGETOOLS_H
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
                data[f*nw*nh+ny*nw+nx]=data[f*nw*nh+ny*nw+nx]+temp[f*width*height+y*width+x];
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

#endif // IMAGETOOLS_H
