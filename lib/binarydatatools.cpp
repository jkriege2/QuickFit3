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










QVector<qlonglong> stringToQlonglongArray_base64(const QString& data) {
    QVector<qlonglong> result;

    QByteArray a=QByteArray::fromBase64(data.toAscii());
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

    QByteArray a=QByteArray::fromHex(data.toAscii());
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
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
    QVector<qlonglong> datai=data;
    for (int i=0; i<datai.size(); i++) {
        datai[i]=qToLittleEndian(data[i]);
    }
    QByteArray result=QByteArray::fromRawData((char*)datai.constData(), datai.size()*sizeof(qlonglong));
    return result.toBase64();
#else
    QByteArray result=QByteArray::fromRawData((char*)data.constData(), data.size()*sizeof(qlonglong));
    return result.toBase64();
#endif
}

QByteArray qlonglongArrayToString_hex(const QVector<qlonglong>& data) {
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
    QVector<qlonglong> datai=data;
    for (int i=0; i<datai.size(); i++) {
        datai[i]=qToLittleEndian(data[i]);
    }
    QByteArray result=QByteArray::fromRawData((char*)datai.constData(), datai.size()*sizeof(qlonglong));
    return result.toHex();
#else
    QByteArray result=QByteArray::fromRawData((char*)data.constData(), data.size()*sizeof(qlonglong));
    return result.toHex();
#endif
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
