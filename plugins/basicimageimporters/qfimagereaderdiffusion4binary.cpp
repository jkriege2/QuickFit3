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
    framedata.clear();
}

QFImageReaderDiffusion4Binary::~QFImageReaderDiffusion4Binary()
{
    input.close();
    framedata.clear();
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
    framedata.clear();

    input.setFileName(filename);
    if (input.open(QIODevice::ReadOnly)) {
        this->filename=filename;

        width=binfileReadUint64(input);
        height=1;
        frames=binfileReadUint64(input);
        timestep=binfileReadDouble(input);
        frame=-1;

        //framedata=(double*)calloc(width*height, sizeof(double));
        framedata.clear();
        for (int i=0; i<width*height; i++) framedata<<0.0;

        /*qDebug()<<width<<height<<frames<<timestep;
        qint64 p=input.pos();
        int cnt=0;
        while (cnt<100) {
            nextFrame();
            float f[width*height];
            double d[width*height];
            uint16_t ui16[width*height];
            readFrameFloat(f);
            readFrameDouble(d);
            readFrameUINT16(ui16);
            qDebug()<<cnt<<f[0]<<d[0]<<ui16[0];
            cnt++;
        }
        input.seek(p);*/
        frame=-1;
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
    framedata.clear();

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
    /*for (int i=0; i<width*height; i++) {
        framedata[i]=binfileReadUint64(input);
        qDebug()<<"   "<<framedata[i];
    }*/
    binfileReadDoubleArray(input, framedata, width*height);
    return true;
}

QVariant QFImageReaderDiffusion4Binary::getFileProperty(const QString &name, const QVariant &defaultValue) const
{
    QString n=name.toUpper().trimmed();
    if (n=="FRAMERATE") {
        return 1.0/timestep;
    }
    return QFImporterImageSeries::getFileProperty(name, defaultValue);
}

uint32_t QFImageReaderDiffusion4Binary::intFrameWidth() {
    if ((input.isOpen() && input.isReadable())) return width;
    return 0;
}

uint32_t QFImageReaderDiffusion4Binary::intFrameHeight() {
    if ((input.isOpen() && input.isReadable())) return height;
    return 0;
}



bool QFImageReaderDiffusion4Binary::intReadFrameFloat(float *data) {
    if (!(input.isOpen() && input.isReadable())) return false;
    if (framedata.isEmpty()) return false;

    for (int i=0; i<width*height; i++) data[i]=framedata[i];

    return true;
}

bool QFImageReaderDiffusion4Binary::intReadFrameUINT16(uint16_t *data) {
    if (!(input.isOpen() && input.isReadable())) return false;
    if (framedata.isEmpty()) return false;

    for (int i=0; i<width*height; i++) data[i]=framedata[i];

    return true;
}

bool QFImageReaderDiffusion4Binary::intReadFrameDouble(double *data) {
    if (!(input.isOpen() && input.isReadable())) return false;
    if (framedata.isEmpty()) return false;

    for (int i=0; i<width*height; i++) data[i]=framedata[i];

    return true;
}
