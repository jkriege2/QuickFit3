/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "binarydatatools.h"
#include <QElapsedTimer>
#include <QByteArray>
#include <QtEndian>
#include <QtGlobal>
#include <QDebug>
#include "qftools.h"
QVector<double> stringToDoubleArray_base64(const QString& data) {
    QVector<double> result;

    QByteArray a=QByteArray::fromBase64(data.toLatin1());
    long long count=a.size()/sizeof(double);
    result.fill(0.0, count);
    const double* d=(double*)a.constData();
    for (long long i=0; i<count; i++) {
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
        result[i]=qToBigEndian(d[i]);
#else
        result[i]=d[i];
#endif
    }
    return result;
}

QVector<double> stringToDoubleArray_hex(const QString& data) {
    QVector<double> result;

    QByteArray a=QByteArray::fromHex(data.toLatin1());
    long long count=a.size()/sizeof(double);
    result.fill(0.0, count);
    const double* d=(double*)a.constData();
    for (long long i=0; i<count; i++) {
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
        result[i]=qToBigEndian(d[i]);
#else
        result[i]=d[i];
#endif
    }
    return result;
}

QByteArray doubleArrayToString_base64(const QVector<double>& data) {
    //QElapsedTimer t;
    //t.start();
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
    QVector<double> datai=data;
    for (int i=0; i<datai.size(); i++) {
        datai[i]=qToLittleEndian(data[i]);
    }
    QByteArray result=QByteArray::fromRawData((char*)datai.constData(), datai.size()*sizeof(double));
    return  result.toBase64();
#else
    QByteArray result=QByteArray::fromRawData((char*)data.constData(), data.size()*sizeof(double));
    return result.toBase64();
#endif
    //qDebug()<<"doubleArrayToString_base64(N="<<data.size()<<"):   "<<double(t.nsecsElapsed())/1.0e6<<" ms";
    //return s;
}

QByteArray doubleArrayToString_hex(const QVector<double>& data) {
    //QElapsedTimer t;
    //t.start();

#if Q_BYTE_ORDER == Q_BIG_ENDIAN
    QVector<double> datai=data;
    for (int i=0; i<datai.size(); i++) {
        datai[i]=qToLittleEndian(data[i]);
    }
    QByteArray result=QByteArray::fromRawData((char*)datai.constData(), datai.size()*sizeof(double));
    return  result.toHex();
#else
    QByteArray result=QByteArray::fromRawData((char*)data.constData(), data.size()*sizeof(double));
    return  result.toHex();
#endif
    //qDebug()<<"doubleArrayToString_hex(N="<<data.size()<<"):   "<<double(t.nsecsElapsed())/1.0e6<<" ms";
    //return s;
}










QVector<qlonglong> stringToQlonglongArray_base64(const QString& data) {
    QVector<qlonglong> result;

    QByteArray a=QByteArray::fromBase64(data.toLatin1());
    long long count=a.size()/sizeof(qlonglong);
    result.fill(0.0, count);
    const qlonglong* d=(qlonglong*)a.constData();
    for (long long i=0; i<count; i++) {
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
        result[i]=qToBigEndian(d[i]);
#else
        result[i]=d[i];
#endif
    }
    return result;
}

QVector<qlonglong> stringToQlonglongArray_hex(const QString& data) {
    QVector<qlonglong> result;

    QByteArray a=QByteArray::fromHex(data.toLatin1());
    long long count=a.size()/sizeof(qlonglong);
    result.fill(0.0, count);
    const qlonglong* d=(qlonglong*)a.constData();
    for (long long i=0; i<count; i++) {
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
        result[i]=qToBigEndian(d[i]);
#else
        result[i]=d[i];
#endif
    }
    return result;
}

QByteArray qlonglongArrayToString_base64(const QVector<qlonglong>& data) {
//    QElapsedTimer t;
//    t.start();
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
    QVector<qlonglong> datai=data;
    for (int i=0; i<datai.size(); i++) {
        datai[i]=qToLittleEndian(data[i]);
    }
    QByteArray result=QByteArray::fromRawData((char*)datai.constData(), datai.size()*sizeof(qlonglong));
    return  result.toBase64();
#else
    QByteArray result=QByteArray::fromRawData((char*)data.constData(), data.size()*sizeof(qlonglong));
    return  result.toBase64();
#endif
    //qDebug()<<"qlonglongArrayToString_base64(N="<<data.size()<<"):   "<<double(t.nsecsElapsed())/1.0e6<<" ms";
    //return s;

}

QByteArray qlonglongArrayToString_hex(const QVector<qlonglong>& data) {
//    QElapsedTimer t;
//    t.start();
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
    QVector<qlonglong> datai=data;
    for (int i=0; i<datai.size(); i++) {
        datai[i]=qToLittleEndian(data[i]);
    }
    QByteArray result=QByteArray::fromRawData((char*)datai.constData(), datai.size()*sizeof(qlonglong));
    QString s= result.toHex();
#else
    QByteArray result=QByteArray::fromRawData((char*)data.constData(), data.size()*sizeof(qlonglong));
    return  result.toHex();
#endif
    //qDebug()<<"qlonglongArrayToString_hex(N="<<data.size()<<"):   "<<double(t.nsecsElapsed())/1.0e6<<" ms";
    //return s;
}










QVector<bool> stringToBoolArray(const QString& data) {
    QVector<bool> result;
    result.fill(false, data.size());
    for(int i=0; i<data.size(); i++) {
        if (data[i]!='0') result[i]=true;
    }
    return result;
}


QByteArray boolArrayToString(const QVector<bool>& data) {
    QByteArray result;
    for (int i=0; i<data.size(); i++) {
        if (data[i]) result.append('1');
        else result.append('0');
    }
    return result;
}







void binfileWriteUint64(QFile& file, uint64_t data) {
    uint64_t w=qToLittleEndian(data);
    file.write((char*)(&w), sizeof(w));
}

uint64_t binfileReadUint64(QFile& file) {
    uint64_t d;
    file.read((char*)&d, sizeof(d));
    return qFromLittleEndian(d);
}

uint64_t binfileReadUint64(FILE* file) {
    uint64_t d;
    fread((char*)&d, sizeof(d),1,file);
    return qFromLittleEndian(d);
}

void binfileWriteUint32(QFile& file, uint32_t data) {
    uint32_t w=qToLittleEndian(data);
    file.write((char*)(&w), sizeof(w));
}

uint32_t binfileReadUint32(QFile& file) {
    uint32_t d=0;
    file.read((char*)&d, sizeof(d));
    return qFromLittleEndian(d);
}

int32_t binfileReadInt32(QFile& file) {
    int32_t d=0;
    file.read((char*)&d, sizeof(d));
    return qFromLittleEndian(d);
}

int16_t binfileReadInt16(QFile& file) {
    int16_t d=0;
    file.read((char*)&d, sizeof(d));
    return qFromLittleEndian(d);
}

int8_t binfileReadInt8(QFile& file) {
    uint32_t d=0;
    file.read((char*)&d, sizeof(d));
    return qFromLittleEndian(d);
}

int64_t binfileReadInt64(QFile& file) {
    uint64_t d=0;
    file.read((char*)&d, sizeof(d));
    return qFromLittleEndian(d);
}

void binfileWriteUint16(QFile& file, uint16_t data) {
    uint16_t w=qToLittleEndian(data);
    file.write((char*)(&w), sizeof(w));
}

uint16_t binfileReadUint16(QFile& file) {
    uint16_t d;
    file.read((char*)&d, sizeof(d));
    return qFromLittleEndian(d);
}

void binfileWriteUint8(QFile& file, uint8_t data) {
    uint8_t w=qToLittleEndian(data);
    file.write((char*)(&w), sizeof(w));
}

uint8_t binfileReadUint8(QFile& file) {
    uint8_t d;
    file.read((char*)&d, sizeof(d));
    return qFromLittleEndian(d);
}

void binfileWriteDouble(QFile& file, double data) {
    double w=qToLittleEndian(data);
    file.write((char*)(&w), sizeof(w));
}

double binfileReadDouble(QFile& file) {
    double d;
    file.read((char*)&d, sizeof(d));
    return qFromLittleEndian(d);
}

void binfileWriteDoubleArray(QFile& file, const double* data, uint32_t dataN) {
    for (uint32_t i=0; i<dataN; i++)  {
        const double w=qToLittleEndian(data[i]);
        file.write((char*)(&w), sizeof(w));
    }
}

void binfileWriteDoubleArrayMinus1(QFile& file, const double* data, uint32_t dataN) {
    for (uint32_t i=0; i<dataN; i++)  {
        const double w=qToLittleEndian(data[i]-1.0);
        file.write((char*)(&w), sizeof(w));
    }
}

void binfileReadDoubleArray(QFile& file, double* data, uint32_t dataN) {
    file.read((char*)data, dataN*sizeof(double));
    for (uint32_t i=0; i<dataN; i++)  {
        double d=data[i];
        data[i]=qFromLittleEndian(d);
    }
}

double* binfileReadDoubleArray(QFile& file, uint32_t dataN) {
    if (dataN<=0) return NULL;
    double* res=(double*)qfMalloc(dataN*sizeof(double));
    binfileReadDoubleArray(file, res, dataN);
    return res;
}

QVector<double> binfileReadDoubleArrayV(QFile& file, uint32_t dataN) {
    QVector<double> res;
    if (dataN>0) binfileReadDoubleArray(file, res, dataN);
    return res;
}

void binfileReadDoubleArray(QFile& file, QVector<double>& dataOut, uint32_t dataN) {
    dataOut.resize(dataN);
    if (dataN>0) {
        binfileReadDoubleArray(file, dataOut.data(), dataN);
    }
}

void binfileReadUInt16Array(QFile& file, uint16_t* data, uint32_t dataN) {
    file.read((char*)data, dataN*sizeof(uint16_t));
    for (uint32_t i=0; i<dataN; i++)  {
        uint16_t d=data[i];
        data[i]=qFromLittleEndian(d);
    }
}
