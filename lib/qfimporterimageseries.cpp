#include "qfimporterimageseries.h"
#include <cstdlib>
#include <cmath>
#include<QDebug>

QFImporterImageSeries::QFImporterImageSeries() {
    binning=1;
    x0=0;
    x1=0;
    y0=0;
    y1=0;
    crop=false;
    interleavedBinning=false;
    averageBinning=false;
}

uint16_t QFImporterImageSeries::frameWidth() {
    if (binning<1) binning=1;
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
    if (!interleavedBinning) {
        return w/binning;
    } else {
        if (binning>1) return w-(binning-1);
    }
    return w;
}

uint16_t QFImporterImageSeries::frameHeight() {
    if (binning<1) binning=1;
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
    if (!interleavedBinning) {
        return h/binning;
    } else {
        if (binning>1) return h-(binning-1);
    }
    return h;
}

bool QFImporterImageSeries::readFrameFloat(float* data) {
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
    float binAvg=1.0;
    if (averageBinning) binAvg=binning*binning;
    if (!interleavedBinning) {
        for (register int y=0; y<h*binning; y++) {
            for (register int x=0; x<w*binning; x++) {
                const int idx=(y/binning)*w+(x/binning);
                data[idx]=data[idx]+d1[y*ww+x]/binAvg;
            }
        }
    } else {
        for (register int y=0; y<h; y++) {
            for (register int x=0; x<w; x++) {
                int idx=y*w+x;
                data[idx]=0;
                for (int bx=0; bx<binning; bx++) {
                    for (int by=0; by<binning; by++) {
                        data[idx]=data[idx]+d1[(y+by)*ww+x+bx]/binAvg;
                    }
                }
            }
        }
    }

    free(d);
    free(d1);
    return true;
}

bool QFImporterImageSeries::readFrameDouble(double *data) {
    if (!crop && binning==1) return intReadFrameDouble(data);
    int fw=intFrameWidth();
    int fh=intFrameHeight();
    double* d=(double*)malloc(fw*fh*sizeof(double));
    double* d1=(double*)malloc(fw*fh*sizeof(double));
    bool OK=intReadFrameDouble(d);
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
    double binAvg=1.0;
    if (averageBinning) binAvg=binning*binning;

    if (!interleavedBinning) {
        for (register int y=0; y<h*binning; y++) {
            for (register int x=0; x<w*binning; x++) {
                data[(y/binning)*w+(x/binning)]=data[(y/binning)*w+(x/binning)]+d1[y*ww+x]/binAvg;
            }
        }
    } else {
        for (register int y=0; y<h; y++) {
            for (register int x=0; x<w; x++) {
                int idx=y*w+x;
                data[idx]=0;
                for (int bx=0; bx<binning; bx++) {
                    for (int by=0; by<binning; by++) {
                        data[idx]=data[idx]+d1[(y+by)*ww+x+bx]/binAvg;
                    }
                }
            }
        }
    }

    free(d);
    free(d1);
    return true;
}

bool QFImporterImageSeries::readFrameUINT16(uint16_t* data) {
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
    uint16_t binAvg=1.0;
    if (averageBinning) binAvg=binning*binning;

    if (!interleavedBinning) {
        for (register int y=0; y<h*binning; y++) {
            for (register int x=0; x<w*binning; x++) {
                data[(y/binning)*w+(x/binning)]=data[(y/binning)*w+(x/binning)]+d1[y*ww+x]/binAvg;
            }
        }
    } else {
        for (register int y=0; y<h; y++) {
            for (register int x=0; x<w; x++) {
                int idx=y*w+x;
                data[idx]=0;
                for (int bx=0; bx<binning; bx++) {
                    for (int by=0; by<binning; by++) {
                        data[idx]=data[idx]+d1[(y+by)*ww+x+bx]/binAvg;
                    }
                }
            }
        }
    }

    free(d);
    free(d1);
    return true;
}

QVariant QFImporterImageSeries::getFileProperty(const QString &name, const QVariant &defaultValue) const
{
    return defaultValue;
}

void QFImporterImageSeries::setCropping(int x0, int x1, int y0, int y1) {
    this->x0=x0;
    this->x1=x1;
    this->y0=y0;
    this->y1=y1;
    crop=true;
    //qDebug()<<"enabled cropping   x = "<<x0<<"..."<<x1<<"   y = "<<y0<<"..."<<y1;
}
