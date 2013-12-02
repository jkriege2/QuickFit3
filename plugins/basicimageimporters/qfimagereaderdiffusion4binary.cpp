#include "qfimagereaderdiffusion4binary.h"
#include <QObject>
#include <QObject>
#include <QtGlobal>
#include <QDebug>
#include "programoptions.h"
#include "binarydatatools.h"

QFImageReaderDiffusion4Binary::QFImageReaderDiffusion4Binary() {
    width=0;
    height=0;
    filename="";
    input.close();
    frame=0;
    frames=0;
    timestep=0;
    framedata=NULL;
}

QFImageReaderDiffusion4Binary::~QFImageReaderDiffusion4Binary()
{
    input.close();
    if (framedata) free(framedata);
    framedata=NULL;
}

QString QFImageReaderDiffusion4Binary::filter() const {
    return QObject::tr("Diffusion4 BTS binary (*.bin)");
}

QString QFImageReaderDiffusion4Binary::formatName() const {
    return QObject::tr("Diffusion4 BTS binary");
}

bool QFImageReaderDiffusion4Binary::open(QString filename) {
    close();
    width=0;
    height=0;
    frame=0;
    frames=0;
    this->filename="";
    if (framedata) free(framedata);
    framedata=NULL;

    input.setFileName(filename);
    if (input.open(QIODevice::ReadOnly)) {
        this->filename=filename;

        width=binfileReadUint64(input);
        height=1;
        frames=binfileReadUint64(input);
        timestep=binfileReadDouble(input);
        frame=-1;

        framedata=(uint64_t*)calloc(width*height, sizeof(uint64_t));

        return nextFrame();
    }
    return false;
}

uint32_t QFImageReaderDiffusion4Binary::countFrames() {
    if ((input.isOpen() && input.isReadable())) return frames;
    return 0;
}

void QFImageReaderDiffusion4Binary::close() {
    input.close();
    frame=0;
    frames=0;
    timestep=0;
    filename="";
    width=0;
    height=0;
    if (framedata) free(framedata);
    framedata=NULL;

}

void QFImageReaderDiffusion4Binary::reset() {
    QString fn=filename;
    close();
    if (!fn.isEmpty()) open(fn);
}


bool QFImageReaderDiffusion4Binary::nextFrame() {
    if (!(input.isOpen() && input.isReadable())) return false;
    if (input.atEnd()) return false;
    frame++;
    const int rsize=width*height*sizeof(uint64_t);
    return (input.read((char*)framedata, rsize)==rsize);
}

uint16_t QFImageReaderDiffusion4Binary::intFrameWidth() {
    if ((input.isOpen() && input.isReadable())) return width;
    return 0;
}

uint16_t QFImageReaderDiffusion4Binary::intFrameHeight() {
    if ((input.isOpen() && input.isReadable())) return height;
    return 0;
}



bool QFImageReaderDiffusion4Binary::intReadFrameFloat(float *data) {
    if (!(input.isOpen() && input.isReadable())) return false;
    if (!framedata) return false;

    for (int i=0; i<width*height; i++) data[i]=framedata[i];

    return true;
}

bool QFImageReaderDiffusion4Binary::intReadFrameUINT16(uint16_t *data) {
    if (!(input.isOpen() && input.isReadable())) return false;
    if (!framedata) return false;

    for (int i=0; i<width*height; i++) data[i]=framedata[i];

    return true;
}

bool QFImageReaderDiffusion4Binary::intReadFrameDouble(double *data) {
    if (!(input.isOpen() && input.isReadable())) return false;
    if (!framedata) return false;

    for (int i=0; i<width*height; i++) data[i]=framedata[i];

    return true;
}
