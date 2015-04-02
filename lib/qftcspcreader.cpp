/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
