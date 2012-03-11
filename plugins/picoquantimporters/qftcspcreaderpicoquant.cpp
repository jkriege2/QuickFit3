#include "qftcspcreaderpicoquant.h"
#include <QObject>

QFTCSPCReaderPicoquant::QFTCSPCReaderPicoquant() {

}

QFTCSPCReaderPicoquant::~QFTCSPCReaderPicoquant() {
}

QString QFTCSPCReaderPicoquant::filter() const {
    return QObject::tr("PicoQuant TTTR (*.t3r)");
}

QString QFTCSPCReaderPicoquant::formatName() const {
    return QObject::tr("PicoQuant TTTR File");
}

bool QFTCSPCReaderPicoquant::open(QString filename)
{
    return false;
}

void QFTCSPCReaderPicoquant::close() {
}

void QFTCSPCReaderPicoquant::reset() {
}

bool QFTCSPCReaderPicoquant::nextRecord() {
    return false;
}

double QFTCSPCReaderPicoquant::measurementDuration() const {
    return 0;
}

int32_t QFTCSPCReaderPicoquant::inputChannels() const {
    return 1;
}

QFTCSPCRecord QFTCSPCReaderPicoquant::getCurrentRecord() const {
    return QFTCSPCRecord(0);
}

