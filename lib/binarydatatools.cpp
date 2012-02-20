#include "binarydatatools.h"
#include <QByteArray>
#include <QtEndian>
#include <QtGlobal>

QVector<double> stringToDoubleArray_base64(const QString& data) {
    QVector<double> result;

    QByteArray a=QByteArray::fromBase64(data.toAscii());
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

    QByteArray a=QByteArray::fromHex(data.toAscii());
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
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
    QVector<double> datai=data;
    for (int i=0; i<datai.size(); i++) {
        datai[i]=qToLittleEndian(data[i]);
    }
    QByteArray result=QByteArray::fromRawData((char*)datai.constData(), datai.size()*sizeof(double));
    return result.toBase64();
#else
    QByteArray result=QByteArray::fromRawData((char*)data.constData(), data.size()*sizeof(double));
    return result.toBase64();
#endif
}

QByteArray doubleArrayToString_hex(const QVector<double>& data) {
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
    QVector<double> datai=data;
    for (int i=0; i<datai.size(); i++) {
        datai[i]=qToLittleEndian(data[i]);
    }
    QByteArray result=QByteArray::fromRawData((char*)datai.constData(), datai.size()*sizeof(double));
    return result.toHex();
#else
    QByteArray result=QByteArray::fromRawData((char*)data.constData(), data.size()*sizeof(double));
    return result.toHex();
#endif
}

