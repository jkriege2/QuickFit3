#include "qftcspcreader.h"

double QFTCSPCRecord::absoluteTime() const {
    return macrotime+(microtime_offset+double(microtime_channel)*microtime_deltaT)*1e-9;
}

QFTCSPCRecord::QFTCSPCRecord(uint16_t input_channel, double macrotime, int16_t microtime_channel, double microtime_offset, double microtime_deltaT) {
    this->input_channel=input_channel;
    this->macrotime=macrotime;
    this->microtime_channel=microtime_channel;
    this->microtime_offset=microtime_offset;
    this->microtime_deltaT=microtime_deltaT;
}

QFTCSPCReader::QFTCSPCReader() {

}

QFTCSPCReader::~QFTCSPCReader() {

}
