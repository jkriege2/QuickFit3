#include "qftcspcreader.h"


QFTCSPCRecord::QFTCSPCRecord(uint16_t input_channel, double macrotime, int16_t microtime_channel, double microtime_offset, double microtime_deltaT, bool isPhoton, uint8_t marker_type) {
    this->input_channel=input_channel;
    this->macrotime=macrotime;
    this->microtime_channel=microtime_channel;
    this->microtime_offset=microtime_offset;
    this->microtime_deltaT=microtime_deltaT;
    this->isPhoton=isPhoton;
    this->marker_type=marker_type;
}

QFTCSPCReader::QFTCSPCReader() {

}

QFTCSPCReader::~QFTCSPCReader() {

}
