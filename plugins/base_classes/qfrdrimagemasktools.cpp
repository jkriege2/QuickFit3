#include "qfrdrimagemasktools.h"
#include <QFile>
#include <QTextStream>
#include "csvtools.h"
#include <QDebug>

QFRDRImageMaskTools::QFRDRImageMaskTools()
{
    mask=NULL;
    mask_w=0;
    mask_h=0;
}

QFRDRImageMaskTools::~QFRDRImageMaskTools()
{
    if (mask) free(mask);
}

void QFRDRImageMaskTools::maskInit(uint16_t w, uint16_t h)
{
    maskDelete();
    if (w*h>0) {
        mask_w=w;
        mask_h=h;
        mask=(bool*)malloc(w*h*sizeof(bool));
        for (uint32_t i=0; i<w*h; i++)  {
            mask[i]=false;
        }
    }
}

void QFRDRImageMaskTools::maskDelete()
{
    if (mask) free(mask);
    mask=NULL;
    mask_w=0;
    mask_h=0;
}

QString QFRDRImageMaskTools::maskToListString(const QString &coordinate_separator, const QString &pixel_separator) const
{
    QString res="";
    QTextStream str(&res);
    bool first=true;
    for (uint16_t y=0; y<mask_h; y++) {
        for (uint16_t x=0; x<mask_w; x++) {
            if (mask[y*mask_w+x]) {
                if (!first) str<<pixel_separator;
                str<<x<<coordinate_separator<<y;
                first=false;
            }
        }
    }
    return res;
}

void QFRDRImageMaskTools::maskLoadFromListString(const QString& maskstring, QChar coordinate_separator, QChar pixel_separator)
{
    QString m=maskstring;
    QTextStream str(&m);
    while (!str.atEnd())  {
        QVector<double> d=csvReadline(str, coordinate_separator, '#', -1, QString(pixel_separator), QString(" \n\r"));
        //qDebug()<<"maskLoadFromListString: "<<d;
        if (d.size()==2) {
            int idx=d[1]*mask_w+d[0];
            if (idx>=0 && idx<mask_h*mask_w) mask[idx]=true;
        }
    }

}


void QFRDRImageMaskTools::maskLoad(const QString &filename)
{
    QFile f(filename);
    if (f.open(QIODevice::ReadOnly)) {
        maskClear();
        maskLoadFromString(f.readAll());
        f.close();
    }
}

void QFRDRImageMaskTools::maskLoadFromString(const QString& maskstring)
{
    maskLoadFromListString(maskstring, ',', '\n');
}

void QFRDRImageMaskTools::maskSave(const QString &filename) const
{
    QFile f(filename);
    if (f.open(QIODevice::WriteOnly|QIODevice::Text)) {
        QTextStream str(&f);
        str<<maskToString();
        f.close();
    }
}

QString QFRDRImageMaskTools::maskToString() const
{
    return maskToListString(", ", "\n");
}

QString QFRDRImageMaskTools::maskToIndexString(QChar separator) const
{
    if (!mask) return "";
    QString res="";
    bool first=true;
    for (uint16_t i=0; i<mask_w*mask_h; i++) {
        if (mask[i]) {
            if (!first) res+=separator;
            res+=QString::number(i);
            first=false;
        }
    }
    return res;
}

void QFRDRImageMaskTools::maskLoadFromIndexString(const QString &data, QChar separator)
{
    if (!mask) return;
    QStringList sl=data.split(separator);

    for (int i=0; i<sl.size(); i++) {
        bool ok=false;
        uint16_t idx=sl[i].toUInt(&ok);
        if (ok) maskSetIdx(idx, true);
    }
}

void QFRDRImageMaskTools::maskClear()
{
    if (!mask) return;
    for (uint16_t i=0; i<mask_w*mask_h; i++) {
        mask[i]=false;
    }
}

void QFRDRImageMaskTools::maskSetAll()
{
    if (!mask) return;
    for (uint16_t i=0; i<mask_w*mask_h; i++) {
        mask[i]=true;
    }
}

void QFRDRImageMaskTools::maskUnset(uint16_t x, uint16_t y)
{
    if (!mask) return;
    mask[y*mask_w+x]=false;
}

void QFRDRImageMaskTools::maskSet(uint16_t x, uint16_t y, bool value)
{
    if (!mask) return;
    mask[y*mask_w+x]=value;
}

void QFRDRImageMaskTools::maskToggle(uint16_t x, uint16_t y)
{
    mask[y*mask_w+x]=!mask[y*mask_w+x];
}

void QFRDRImageMaskTools::maskInvert()
{
    if (!mask) return;
    for (uint32_t i=0; i<mask_w*mask_h; i++) {
        mask[i]=!mask[i];
    }
    //qDebug()<<"maskInverted: "<<maskToString();
}

bool QFRDRImageMaskTools::maskGet(uint16_t x, uint16_t y) const
{
    if (!mask) return false;
    return mask[y*mask_w+x];

}

bool *QFRDRImageMaskTools::maskGet() const
{
    return mask;
}

uint16_t QFRDRImageMaskTools::maskGetWidth() const
{
    return mask_w;
}

uint16_t QFRDRImageMaskTools::maskGetHeight() const
{
    return mask_h;
}

long QFRDRImageMaskTools::maskGetCount() const
{
    long result=0;
    if (!mask) return 0;
    for (uint32_t i=0; i<mask_w*mask_h; i++) {
        if (mask[i]) result++;
    }
    return result;
}

void QFRDRImageMaskTools::maskSetIdx(uint16_t idx, bool value)
{
    if (idx>=0 && idx<mask_w*mask_h) mask[idx]=value;
}

bool QFRDRImageMaskTools::maskGetIdx(uint16_t idx) const
{
    if (idx>=0 && idx<mask_w*mask_h) return mask[idx];
    return false;
}

QList<int> QFRDRImageMaskTools::maskToIndexList()
{
    QList<int> lst;
    for (uint32_t i=0; i<mask_w*mask_h; i++) {
        if (mask[i]) lst<<i;
    }

    return lst;
}


