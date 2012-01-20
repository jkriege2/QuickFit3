#include "qfrdrimagereader.h"
#include <cstdlib>
#include <cmath>
#include<QDebug>

QFRDRImageReader::QFRDRImageReader() {
    binning=1;
    x0=0;
    x1=0;
    y0=0;
    y1=0;
    crop=false;
}

uint16_t QFRDRImageReader::frameWidth() {
    int xx0=x0;
    int xx1=x1;
    if (!crop) {
        xx0=0;
        xx1=intFrameWidth()-1;
    }
    if (xx0<0) xx0=0;
    if (xx0>intFrameWidth()-1) xx0=intFrameWidth()-1;
    if (xx1<0) xx1=0;
    if (xx1>intFrameWidth()-1) xx1=intFrameWidth()-1;

    int w=abs(xx1-xx0)+1;
    return w/binning;
}

uint16_t QFRDRImageReader::frameHeight() {
    int yy0=y0;
    int yy1=y1;
    if (!crop) {
        yy0=0;
        yy1=intFrameHeight()-1;
    }
    if (yy0<0) yy0=0;
    if (yy0>intFrameHeight()-1) yy0=intFrameHeight()-1;
    if (yy1<0) yy1=0;
    if (yy1>intFrameHeight()-1) yy1=intFrameHeight()-1;

    int h=abs(yy1-yy0)+1;
    return h/binning;
}

bool QFRDRImageReader::readFrameFloat(float* data) {
    if (!crop && binning==1) return intReadFrameFloat(data);
    int fw=intFrameWidth();
    int fh=intFrameHeight();
    float* d=(float*)malloc(fw*fh*sizeof(float));
    float* d1=(float*)malloc(fw*fh*sizeof(float));
    bool OK=intReadFrameFloat(d);
    if (!OK) {
        free(d);
        free(d1);
        return false;
    }

    int xx0=x0;
    int xx1=x1;
    if (!crop) {
        xx0=0;
        xx1=intFrameWidth()-1;
    }
    if (xx0<0) xx0=0;
    if (xx0>intFrameWidth()-1) xx0=intFrameWidth()-1;
    if (xx1<0) xx1=0;
    if (xx1>intFrameWidth()-1) xx1=intFrameWidth()-1;
    if (xx0>xx1) qSwap(xx0, xx1);

    int yy0=y0;
    int yy1=y1;
    if (!crop) {
        yy0=0;
        yy1=intFrameHeight()-1;
    }
    if (yy0<0) yy0=0;
    if (yy0>intFrameHeight()-1) yy0=intFrameHeight()-1;
    if (yy1<0) yy1=0;
    if (yy1>intFrameHeight()-1) yy1=intFrameHeight()-1;
    if (yy0>yy1) qSwap(yy0, yy1);

    uint16_t w=frameWidth();
    uint16_t h=frameHeight();
    int hh=abs(yy1-yy0)+1;
    int ww=abs(xx1-xx0)+1;

    for (register int y=0; y<hh; y++) {
        for (register int x=0; x<ww; x++) {
            d1[y*ww+x]=d[(yy0+y)*fw+xx0+x];
        }
    }

    for (register int i=0; i<w*h; i++) data[i]=0;
    for (register int y=0; y<h*binning; y++) {
        for (register int x=0; x<w*binning; x++) {
            data[(y/binning)*w+(x/binning)]=data[(y/binning)*w+(x/binning)]+d1[y*ww+x];
        }
    }

    free(d);
    free(d1);
    return true;
}

bool QFRDRImageReader::readFrameUINT16(uint16_t* data) {
    if (!crop && binning==1) return intReadFrameUINT16(data);
    int fw=intFrameWidth();
    int fh=intFrameHeight();
    uint16_t* d=(uint16_t*)malloc(fw*fh*sizeof(uint16_t));
    uint16_t* d1=(uint16_t*)malloc(fw*fh*sizeof(uint16_t));
    bool OK=intReadFrameUINT16(d);
    if (!OK) {
        free(d);
        free(d1);
        return false;
    }

    int xx0=x0;
    int xx1=x1;
    if (!crop) {
        xx0=0;
        xx1=intFrameWidth()-1;
    }
    if (xx0<0) xx0=0;
    if (xx0>intFrameWidth()-1) xx0=intFrameWidth()-1;
    if (xx1<0) xx1=0;
    if (xx1>intFrameWidth()-1) xx1=intFrameWidth()-1;
    if (xx0>xx1) qSwap(xx0, xx1);

    int yy0=y0;
    int yy1=y1;
    if (!crop) {
        yy0=0;
        yy1=intFrameHeight()-1;
    }
    if (yy0<0) yy0=0;
    if (yy0>intFrameHeight()-1) yy0=intFrameHeight()-1;
    if (yy1<0) yy1=0;
    if (yy1>intFrameHeight()-1) yy1=intFrameHeight()-1;
    if (yy0>yy1) qSwap(yy0, yy1);

    uint16_t w=frameWidth();
    uint16_t h=frameHeight();
    int hh=abs(yy1-yy0)+1;
    int ww=abs(xx1-xx0)+1;

    for (register int y=0; y<hh; y++) {
        for (register int x=0; x<ww; x++) {
            d1[y*ww+x]=d[(yy0+y)*fw+xx0+x];
        }
    }

    for (register int i=0; i<w*h; i++) data[i]=0;
    for (register int y=0; y<h*binning; y++) {
        for (register int x=0; x<w*binning; x++) {
            data[(y/binning)*w+(x/binning)]=data[(y/binning)*w+(x/binning)]+d1[y*ww+x];
        }
    }

    free(d);
    free(d1);
    return true;
}

void QFRDRImageReader::setCropping(int x0, int x1, int y0, int y1) {
    this->x0=x0;
    this->x1=x1;
    this->y0=y0;
    this->y1=y1;
    crop=true;
    //qDebug()<<"enabled cropping   x = "<<x0<<"..."<<x1<<"   y = "<<y0<<"..."<<y1;
}
