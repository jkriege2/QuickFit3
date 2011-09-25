#include "qfrdrimagereadertiff.h"
#include "libtiff_tools.h"
#include <QObject>
#include <QtGlobal>

QFRDRImageReaderTIFF::QFRDRImageReaderTIFF() {
    width=0;
    height=0;
    filename="";
    tif=NULL;
}

QFRDRImageReaderTIFF::~QFRDRImageReaderTIFF() {
    close();
}

QString QFRDRImageReaderTIFF::filter() const {
    return QObject::tr("TIFF File (*.tiff *.tif)");
}

QString QFRDRImageReaderTIFF::formatName() const {
    return QObject::tr("TIFF");
}


bool QFRDRImageReaderTIFF::open(QString filename) {
    close();
    tif = TIFFOpen(filename.toAscii().data(),"r");
    if (tif) {
        uint32 nx,ny;
        TIFFGetField(tif,TIFFTAG_IMAGEWIDTH,&nx);
        TIFFGetField(tif,TIFFTAG_IMAGELENGTH,&ny);
        width=nx;
        height=ny;
        this->filename=filename;
        return true;
    } else {
        width=0;
        height=0;
        setLastError(QObject::tr("libtiff: error opening file '%1'").arg(filename));
        this->filename="";
        return false;
    }
}

void QFRDRImageReaderTIFF::close() {
    if (tif) TIFFClose(tif);
    tif=NULL;
}

uint32_t QFRDRImageReaderTIFF::countFrames() {
    if (!tif) return 0;

    uint32_t nb_images = 0;
    tdir_t dir=TIFFCurrentDirectory(tif);
    do {
        ++nb_images;
    } while (TIFFReadDirectory(tif));
    TIFFSetDirectory(tif,dir);
    return nb_images;
}

void QFRDRImageReaderTIFF::reset() {
    if (!tif) return ;
    TIFFSetDirectory(tif,0);
}

bool QFRDRImageReaderTIFF::nextFrame() {
    if (!tif) return false;
    return TIFFReadDirectory(tif);
}

uint16_t QFRDRImageReaderTIFF::frameWidth() {
    return width;
}

uint16_t QFRDRImageReaderTIFF::frameHeight() {
    return height;
}


bool QFRDRImageReaderTIFF::readFrameUINT16(uint16_t* data) {
    if (!tif) return false;
    bool ok=true;
    uint16 samplesperpixel, bitspersample;
    uint16 sampleformat = SAMPLEFORMAT_UINT;
    uint32 nx,ny;
    TIFFGetField(tif,TIFFTAG_IMAGEWIDTH,&nx);
    TIFFGetField(tif,TIFFTAG_IMAGELENGTH,&ny);
    TIFFGetField(tif,TIFFTAG_SAMPLESPERPIXEL,&samplesperpixel);
    TIFFGetField(tif, TIFFTAG_SAMPLEFORMAT, &sampleformat);
    TIFFGetFieldDefaulted(tif,TIFFTAG_BITSPERSAMPLE,&bitspersample);
    if (samplesperpixel>1) {
        setLastError(QObject::tr("can not load frames with more than one sample/pixel (RGB, RGBA, ...)   (from file '%1')").arg(filename));
        return false;
    } else {
        uint16 photo, config;
        TIFFGetField(tif,TIFFTAG_PLANARCONFIG,&config);
        TIFFGetField(tif,TIFFTAG_PHOTOMETRIC,&photo);
        // we only read image with one sample per pixel, so we do nothave to care for TIFFTAG_PLANARCONFIG
        if (TIFFIsTiled(tif)) { // load a tiled frame
            uint32 tw, th;
            TIFFGetField(tif,TIFFTAG_TILEWIDTH,&tw);
            TIFFGetField(tif,TIFFTAG_TILELENGTH,&th);
            switch (bitspersample) {
                case 8 :
                    if (sampleformat==SAMPLEFORMAT_UINT) ok=TIFFLoadTiled<uint8_t,  uint16_t>(data,  tif, nx, ny, tw, th);
                    else {
                        setLastError(QObject::tr("can not load 8-bit tiled image in format different from UINT   (from file '%1')").arg(filename));
                        return false;
                    }
                    break;
                case 16 :
                    if (sampleformat==SAMPLEFORMAT_UINT) ok=TIFFLoadTiled<uint16_t,  uint16_t>(data,  tif, nx, ny, tw, th);
                    else {
                        setLastError(QObject::tr("can not load 16-bit tiled image in format different from UINT   (from file '%1')").arg(filename));
                        return false;
                    }
                    break;
                default: {
                        setLastError(QObject::tr("can not load tiled frame sample format in TIFF file   (from file '%1')").arg(filename));
                        return false;
                    }
                    break;
            }
        } else { // load a non-tiled frame
            switch (bitspersample) {
                case 8 :
                    if (sampleformat==SAMPLEFORMAT_UINT) ok=TIFFLoadNontiled<uint8_t,  uint16_t>(data,  tif, nx, ny);
                    else {
                        setLastError(QObject::tr("can not load 8-bit image in format different from UINT   (from file '%1')").arg(filename));
                        return false;
                    }
                    break;
                case 16 :
                    if (sampleformat==SAMPLEFORMAT_UINT) ok=TIFFLoadNontiled<uint16_t,  uint16_t>(data,  tif, nx, ny);
                    else {
                        setLastError(QObject::tr("can not load 16-bit image in format different from UINT   (from file '%1')").arg(filename));
                        return false;
                    }
                    break;
                default: {
                        setLastError(QObject::tr("can not load frame sample format in TIFF file   (from file '%1')").arg(filename));
                        return false;
                    }
                    break;
            }
        }
    }
    if (!ok) {
        setLastError(QObject::tr("unknown error when reading frame from TIFF file   (from file '%1')").arg(filename));
    }
    return ok;
}

bool QFRDRImageReaderTIFF::readFrameFloat(float* data) {
    if (!tif) return false;

    bool ok=true;

    uint16 samplesperpixel, bitspersample;
    uint16 sampleformat = SAMPLEFORMAT_UINT;
    uint32 nx,ny;
    TIFFGetField(tif,TIFFTAG_IMAGEWIDTH,&nx);
    TIFFGetField(tif,TIFFTAG_IMAGELENGTH,&ny);
    TIFFGetField(tif,TIFFTAG_SAMPLESPERPIXEL,&samplesperpixel);
    TIFFGetField(tif, TIFFTAG_SAMPLEFORMAT, &sampleformat);
    TIFFGetFieldDefaulted(tif,TIFFTAG_BITSPERSAMPLE,&bitspersample);
    if (samplesperpixel>1) {
        setLastError(QObject::tr("can not load frames with more than one sample/pixel (RGB, RGBA, ...)   (from file '%1')").arg(filename));
        return false;
    } else {
        uint16 photo, config;
        TIFFGetField(tif,TIFFTAG_PLANARCONFIG,&config);
        TIFFGetField(tif,TIFFTAG_PHOTOMETRIC,&photo);
        // we only read image with one sample per pixel, so we do nothave to care for TIFFTAG_PLANARCONFIG
        if (TIFFIsTiled(tif)) { // load a tiled frame
            uint32 tw, th;
            TIFFGetField(tif,TIFFTAG_TILEWIDTH,&tw);
            TIFFGetField(tif,TIFFTAG_TILELENGTH,&th);
            switch (bitspersample) {
                case 8 :
                    if (sampleformat==SAMPLEFORMAT_UINT) ok=TIFFLoadTiled<uint8_t,  float>(data,  tif, nx, ny, tw, th);
                    else if (sampleformat==SAMPLEFORMAT_INT) ok=TIFFLoadTiled<int8_t,  float>(data,  tif, nx, ny, tw, th);
                    else {
                        setLastError(QObject::tr("can not load 8-bit tiled image in format different from INT or UINT   (from file '%1')").arg(filename));
                        return false;
                    }
                    break;
                case 16 :
                    if (sampleformat==SAMPLEFORMAT_UINT) ok=TIFFLoadTiled<uint16_t,  float>(data,  tif, nx, ny, tw, th);
                    else if (sampleformat==SAMPLEFORMAT_INT) ok=TIFFLoadTiled<int16_t,  float>(data,  tif, nx, ny, tw, th);
                    else {
                        setLastError(QObject::tr("can not load 16-bit tiled image in format different from INT or UINT   (from file '%1')").arg(filename));
                        return false;
                    }
                    break;
                case 32 :
                    if (sampleformat==SAMPLEFORMAT_UINT) ok=TIFFLoadTiled<uint32_t,  float>(data,  tif, nx, ny, tw, th);
                    else if (sampleformat==SAMPLEFORMAT_INT) ok=TIFFLoadTiled<int32_t,  float>(data,  tif, nx, ny, tw, th);
                    else if (sampleformat==SAMPLEFORMAT_IEEEFP) ok=TIFFLoadTiled<float,  float>(data,  tif, nx, ny, tw, th);
                    else {
                        setLastError(QObject::tr("can not load 32-bit tiled image in format different from INT, UINT, IEEE   (from file '%1')").arg(filename));
                        return false;
                    }
                    break;
                case 64 :
                    if (sampleformat==SAMPLEFORMAT_UINT) ok=TIFFLoadTiled<uint64_t,  float>(data,  tif, nx, ny, tw, th);
                    else if (sampleformat==SAMPLEFORMAT_INT) ok=TIFFLoadTiled<int64_t,  float>(data,  tif, nx, ny, tw, th);
                    else if (sampleformat==SAMPLEFORMAT_IEEEFP) ok=TIFFLoadTiled<double,  float>(data,  tif, nx, ny, tw, th);
                    else {
                        setLastError(QObject::tr("can not load 64-bit tiled image in format different from INT, UINT, IEEE   (from file '%1')").arg(filename));
                        return false;
                    }
                    break;
                default: {
                        setLastError(QObject::tr("can not load tiled frame sample format in TIFF file   (from file '%1')").arg(filename));
                        return false;
                    }
                    break;
            }
        } else { // load a non-tiled frame
            switch (bitspersample) {
                case 8 :
                    if (sampleformat==SAMPLEFORMAT_UINT) ok=TIFFLoadNontiled<uint8_t,  float>(data,  tif, nx, ny);
                    else if (sampleformat==SAMPLEFORMAT_INT) ok=TIFFLoadNontiled<int8_t,  float>(data,  tif, nx, ny);
                    else {
                        setLastError(QObject::tr("can not load 8-bit image in format different from INT or UINT   (from file '%1')").arg(filename));
                        return false;
                    }
                    break;
                case 16 :
                    if (sampleformat==SAMPLEFORMAT_UINT) ok=TIFFLoadNontiled<uint16_t,  float>(data,  tif, nx, ny);
                    else if (sampleformat==SAMPLEFORMAT_INT) ok=TIFFLoadNontiled<int16_t,  float>(data,  tif, nx, ny);
                    else {
                        setLastError(QObject::tr("can not load 16-bit image in format different from INT or UINT   (from file '%1')").arg(filename));
                        return false;
                    }
                    break;
                case 32 :
                    if (sampleformat==SAMPLEFORMAT_UINT) ok=TIFFLoadNontiled<uint32_t,  float>(data,  tif, nx, ny);
                    else if (sampleformat==SAMPLEFORMAT_INT) ok=TIFFLoadNontiled<int32_t,  float>(data,  tif, nx, ny);
                    else if (sampleformat==SAMPLEFORMAT_IEEEFP) ok=TIFFLoadNontiled<float,  float>(data,  tif, nx, ny);
                    else {
                        setLastError(QObject::tr("can not load 32-bit image in format different from INT, UINT, IEEE   (from file '%1')").arg(filename));
                        return false;
                    }
                    break;
                case 64 :
                    if (sampleformat==SAMPLEFORMAT_UINT) ok=TIFFLoadNontiled<uint64_t,  float>(data,  tif, nx, ny);
                    else if (sampleformat==SAMPLEFORMAT_INT) ok=TIFFLoadNontiled<int64_t,  float>(data,  tif, nx, ny);
                    else if (sampleformat==SAMPLEFORMAT_IEEEFP) ok=TIFFLoadNontiled<double,  float>(data,  tif, nx, ny);
                    else {
                        setLastError(QObject::tr("can not load 64-bit image in format different from INT, UINT, IEEE   (from file '%1')").arg(filename));
                        return false;
                    }
                    break;
                default: {
                        setLastError(QObject::tr("can not load frame sample format in TIFF file   (from file '%1')").arg(filename));
                        return false;
                    }
                    break;
            }
        }
    }
    if (!ok) {
        setLastError(QObject::tr("unknown error when reading frame from TIFF file   (from file '%1')").arg(filename));
    }
    return ok;
}
