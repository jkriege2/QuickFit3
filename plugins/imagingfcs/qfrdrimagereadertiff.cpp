#include "qfrdrimagereadertiff.h"

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


template<typename t, typename tout>
bool QFRDRImageReaderTIFF_load_nontiled(tout* data_out, TIFF *const tif, const uint32 width, const uint32 height) {
    t *const buf = (t*)_TIFFmalloc(TIFFStripSize(tif));
    if (buf) {
        uint32 row, rowsperstrip = (uint32)-1;
        TIFFGetField(tif,TIFFTAG_ROWSPERSTRIP,&rowsperstrip);
        for (row = 0; row<height; row+= rowsperstrip) {
            uint32 nrow = (row+rowsperstrip>height?height-row:rowsperstrip);
            tstrip_t strip = TIFFComputeStrip(tif, row, 0);
            if ((TIFFReadEncodedStrip(tif,strip,buf,-1))<0) {
                _TIFFfree(buf);
                return false;
            }
            const t *ptr = buf;
            for (unsigned int rr = 0; rr<nrow; ++rr) {
                for (unsigned int cc = 0; cc<width; ++cc) {
                    data_out[cc+(row+rr)*width] = (tout)*(ptr++);
                }
            }
        }
        _TIFFfree(buf);
    } else return false;
    return true;
}



template<typename t, typename tout>
bool QFRDRImageReaderTIFF_load_tiled(tout* data_out, TIFF *const tif, const uint32 width, const uint32 height, const uint32 tile_width, const uint32 tile_height) {
    t *const buf = (t*)_TIFFmalloc(TIFFTileSize(tif));
    if (buf) {
        unsigned int sample=0;
        for (unsigned int row = 0; row<height; row+=tile_height) {
            for (unsigned int col = 0; col<width; col+=tile_width) {
                if (TIFFReadTile(tif,buf,col,row,0,sample)<0) {
                    _TIFFfree(buf);
                    return false;
                }
                const t *ptr = buf;
                for (unsigned int rr = row; rr<qMin((unsigned int)(row+tile_height),(unsigned int)height); ++rr) {
                    for (unsigned int cc = col; cc<qMin((unsigned int)(col+tile_width),(unsigned int)width); ++cc) {
                        data_out[cc+rr*width] = (tout)*(ptr++);
                    }
                }
            }
        }
        _TIFFfree(buf);
    } else return false;
    return true;
}

bool QFRDRImageReaderTIFF::readFrameUINT16(uint16_t* data) {
    if (!tif) return false;

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
                    if (sampleformat==SAMPLEFORMAT_UINT) QFRDRImageReaderTIFF_load_tiled<uint8_t,  uint16_t>(data,  tif, nx, ny, tw, th);
                    else {
                        setLastError(QObject::tr("can not load 8-bit tiled image in format different from UINT   (from file '%1')").arg(filename));
                        return false;
                    }
                    break;
                case 16 :
                    if (sampleformat==SAMPLEFORMAT_UINT) QFRDRImageReaderTIFF_load_tiled<uint16_t,  uint16_t>(data,  tif, nx, ny, tw, th);
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
                    if (sampleformat==SAMPLEFORMAT_UINT) QFRDRImageReaderTIFF_load_nontiled<uint8_t,  uint16_t>(data,  tif, nx, ny);
                    else {
                        setLastError(QObject::tr("can not load 8-bit image in format different from UINT   (from file '%1')").arg(filename));
                        return false;
                    }
                    break;
                case 16 :
                    if (sampleformat==SAMPLEFORMAT_UINT) QFRDRImageReaderTIFF_load_nontiled<uint16_t,  uint16_t>(data,  tif, nx, ny);
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
    return true;
}

bool QFRDRImageReaderTIFF::readFrame(float* data) {
    if (!tif) return false;

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
                    if (sampleformat==SAMPLEFORMAT_UINT) QFRDRImageReaderTIFF_load_tiled<uint8_t,  float>(data,  tif, nx, ny, tw, th);
                    else if (sampleformat==SAMPLEFORMAT_INT) QFRDRImageReaderTIFF_load_tiled<int8_t,  float>(data,  tif, nx, ny, tw, th);
                    else {
                        setLastError(QObject::tr("can not load 8-bit tiled image in format different from INT or UINT   (from file '%1')").arg(filename));
                        return false;
                    }
                    break;
                case 16 :
                    if (sampleformat==SAMPLEFORMAT_UINT) QFRDRImageReaderTIFF_load_tiled<uint16_t,  float>(data,  tif, nx, ny, tw, th);
                    else if (sampleformat==SAMPLEFORMAT_INT) QFRDRImageReaderTIFF_load_tiled<int16_t,  float>(data,  tif, nx, ny, tw, th);
                    else {
                        setLastError(QObject::tr("can not load 16-bit tiled image in format different from INT or UINT   (from file '%1')").arg(filename));
                        return false;
                    }
                    break;
                case 32 :
                    if (sampleformat==SAMPLEFORMAT_UINT) QFRDRImageReaderTIFF_load_tiled<uint32_t,  float>(data,  tif, nx, ny, tw, th);
                    else if (sampleformat==SAMPLEFORMAT_INT) QFRDRImageReaderTIFF_load_tiled<int32_t,  float>(data,  tif, nx, ny, tw, th);
                    else if (sampleformat==SAMPLEFORMAT_IEEEFP) QFRDRImageReaderTIFF_load_tiled<float,  float>(data,  tif, nx, ny, tw, th);
                    else {
                        setLastError(QObject::tr("can not load 32-bit tiled image in format different from INT, UINT, IEEE   (from file '%1')").arg(filename));
                        return false;
                    }
                    break;
                case 64 :
                    if (sampleformat==SAMPLEFORMAT_UINT) QFRDRImageReaderTIFF_load_tiled<uint64_t,  float>(data,  tif, nx, ny, tw, th);
                    else if (sampleformat==SAMPLEFORMAT_INT) QFRDRImageReaderTIFF_load_tiled<int64_t,  float>(data,  tif, nx, ny, tw, th);
                    else if (sampleformat==SAMPLEFORMAT_IEEEFP) QFRDRImageReaderTIFF_load_tiled<double,  float>(data,  tif, nx, ny, tw, th);
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
                    if (sampleformat==SAMPLEFORMAT_UINT) QFRDRImageReaderTIFF_load_nontiled<uint8_t,  float>(data,  tif, nx, ny);
                    else if (sampleformat==SAMPLEFORMAT_INT) QFRDRImageReaderTIFF_load_nontiled<int8_t,  float>(data,  tif, nx, ny);
                    else {
                        setLastError(QObject::tr("can not load 8-bit image in format different from INT or UINT   (from file '%1')").arg(filename));
                        return false;
                    }
                    break;
                case 16 :
                    if (sampleformat==SAMPLEFORMAT_UINT) QFRDRImageReaderTIFF_load_nontiled<uint16_t,  float>(data,  tif, nx, ny);
                    else if (sampleformat==SAMPLEFORMAT_INT) QFRDRImageReaderTIFF_load_nontiled<int16_t,  float>(data,  tif, nx, ny);
                    else {
                        setLastError(QObject::tr("can not load 16-bit image in format different from INT or UINT   (from file '%1')").arg(filename));
                        return false;
                    }
                    break;
                case 32 :
                    if (sampleformat==SAMPLEFORMAT_UINT) QFRDRImageReaderTIFF_load_nontiled<uint32_t,  float>(data,  tif, nx, ny);
                    else if (sampleformat==SAMPLEFORMAT_INT) QFRDRImageReaderTIFF_load_nontiled<int32_t,  float>(data,  tif, nx, ny);
                    else if (sampleformat==SAMPLEFORMAT_IEEEFP) QFRDRImageReaderTIFF_load_nontiled<float,  float>(data,  tif, nx, ny);
                    else {
                        setLastError(QObject::tr("can not load 32-bit image in format different from INT, UINT, IEEE   (from file '%1')").arg(filename));
                        return false;
                    }
                    break;
                case 64 :
                    if (sampleformat==SAMPLEFORMAT_UINT) QFRDRImageReaderTIFF_load_nontiled<uint64_t,  float>(data,  tif, nx, ny);
                    else if (sampleformat==SAMPLEFORMAT_INT) QFRDRImageReaderTIFF_load_nontiled<int64_t,  float>(data,  tif, nx, ny);
                    else if (sampleformat==SAMPLEFORMAT_IEEEFP) QFRDRImageReaderTIFF_load_nontiled<double,  float>(data,  tif, nx, ny);
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
    return true;
}
