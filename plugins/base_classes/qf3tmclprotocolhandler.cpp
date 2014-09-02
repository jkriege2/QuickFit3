/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center
	
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

#include "qf3tmclprotocolhandler.h"
#include <QDebug>
#include <QTime>


#include <QObject>

QF3TMCLProtocolHandler::QF3TMCLProtocolHandler(JKSerialConnection* com, QString name) {
    this->com=com;
    this->log=NULL;
    this->LOG_PREFIX="";
    this->name=name;
    this->retries=10;
    this->command_delay_ms=30;
}

void QF3TMCLProtocolHandler::setLogging(QFPluginLogService* log, QString LOG_PREFIX) {
    this->log=log;
    this->LOG_PREFIX=LOG_PREFIX;
}

bool QF3TMCLProtocolHandler::sendAndCheckCommand(uint8_t Address, uint8_t Command, uint8_t Type, uint8_t Motor, int32_t Value) {
    bool ok=sendCommand(Address, Command, Type, Motor, Value);
    if (ok) {
        uint8_t Address;
        uint8_t Status;
        int32_t Value;
        ok=readResult(&Address, &Status, &Value);
    }
    return ok;
}

bool QF3TMCLProtocolHandler::queryCommand(uint8_t Address, uint8_t Command, uint8_t Type, uint8_t Motor, int32_t Value, uint8_t *rAddress, uint8_t *rStatus, int32_t *rValue) {
    int retry=0;
    bool done=false;
    bool ok=false;
    while (retry<retries && !done) {
        //qDebug()<<"queryCommand "<<Command<<"   retry="<<retry;
        ok=sendCommand(Address, Command, Type, Motor, Value);
        QTime t;
        t.start();
        while (t.elapsed()<command_delay_ms);
        ok=ok&&readResult(rAddress, rStatus, rValue)!=0;
        if (ok) done=true;
        retry++;
    }
    return ok;
}

bool QF3TMCLProtocolHandler::queryStatus(uint8_t Address, uint8_t Command, uint8_t Type, uint8_t Motor, int32_t Value, uint8_t *rStatus) {
    int32_t rValue;
    uint8_t rAddress;
    return queryCommand( Address,  Command,  Type,  Motor,  Value, &rAddress, rStatus, &rValue);
}

bool QF3TMCLProtocolHandler::setAxisParameter(uint8_t Address, uint8_t parameter, int32_t value, uint8_t motor) {
    uint8_t status=0;
    bool ok=queryStatus(Address, TMCL_SAP, parameter, motor, value, &status);
    return ok&&(status==100);
}

bool QF3TMCLProtocolHandler::getAxisParameter(uint8_t Address, uint8_t parameter, int32_t &value, uint8_t motor) {
    uint8_t status=0;
    uint8_t rAddress=0;
    bool ok=queryCommand(Address, TMCL_GAP, parameter, motor, value, &rAddress, &status, &value);
    return ok&&(status==100);

}

bool QF3TMCLProtocolHandler::setGlobalParameter(uint8_t Address, uint8_t parameter, int32_t value, uint8_t bank) {
    uint8_t status=0;
    bool ok=queryStatus(Address, TMCL_SGP, parameter, bank, value, &status);
    return ok&&(status==100);
}

bool QF3TMCLProtocolHandler::getGlobalParameter(uint8_t Address, uint8_t parameter, int32_t &value, uint8_t bank) {
    uint8_t status=0;
    uint8_t rAddress=0;
    bool ok=queryCommand(Address, TMCL_GGP, parameter, bank, value, &rAddress, &status, &value);
    return ok&&(status==100);
}

QString QF3TMCLProtocolHandler::getFirmwareVersion(uint8_t Address, uint8_t motor) {
    int retry=0;
    bool done=false;
    bool ok=false;
    QString ver="";
    while (retry<retries && !done) {
        ok=sendCommand(Address, TMCL_GETFIRMWARE, 0, motor, 0);
        QTime t;
        t.start();
        while (t.elapsed()<command_delay_ms);
        ok=ok&&readVersionResult(ver);
        if (ok) done=true;
        retry++;
    }
    return ver;
}

bool QF3TMCLProtocolHandler::sendCommand(uint8_t Address, uint8_t Command, uint8_t Type, uint8_t Motor, int32_t Value) {
    if (!com) return false;

    unsigned char nIndex, nChecksum;
    uint8_t buffer[10];

    buffer[0]=Address;
    buffer[1]=Command;
    buffer[2]=Type;
    buffer[3]=Motor;
    buffer[4]=Value >> 24;
    buffer[5]=Value >> 16;
    buffer[6]=Value >> 8;
    buffer[7]=Value & 0xff;
    buffer[8]=0;


    // initialize array nCommand[9] before calculating checksum below
    nChecksum = buffer[0];
    for (nIndex=1; nIndex < 8; nIndex++) {
        nChecksum += buffer[nIndex];
    }
    buffer[8] = nChecksum;

    return com->write(buffer, 9);
}

bool QF3TMCLProtocolHandler::readVersionResult(QString& version) {
    if (!com) return false;

    uint8_t RxBuffer[9], Checksum;


    bool ok=false;
    ok=com->read(RxBuffer, 9);

    version="";

    if (ok) {

        Checksum=0;
        for(int i=1; i<9; i++) {
            version=version+RxBuffer[i];
        }
    }

    return ok;
}

int QF3TMCLProtocolHandler::readResult(uint8_t *Address, uint8_t *Status, int32_t *Value) {
    if (!com) return false;

    uint8_t RxBuffer[9], Checksum;


    bool ok=false;
    ok=com->read(RxBuffer, 9);
    //for (int i=0; i<9;i++) qDebug()<<hex<< RxBuffer[i]<<dec;
    //qDebug()<<"\n";

    //Check if enough bytes can be read
    if (ok) {

        Checksum=0;
        for(int i=0; i<8; i++) {
            Checksum+=RxBuffer[i];
        }

        if(Checksum!=RxBuffer[8]) {
            //qDebug()<<"checksum error "<<Checksum<<RxBuffer[8];
            return -1;
        }

        *Address=RxBuffer[0];
        *Status=RxBuffer[2];
        *Value=(RxBuffer[4] << 24) | (RxBuffer[5] << 16) | (RxBuffer[6] << 8) | RxBuffer[7];
    }

    return (ok)?1:0;
}


void QF3TMCLProtocolHandler::checkComError() {
    if (!com) return;
    if (com->hasErrorOccured()) {
        if (log) log->log_error(QObject::tr("%1 serial port error: %2\n").arg(LOG_PREFIX).arg(com->getLastError().c_str()));
    }
}

bool QF3TMCLProtocolHandler::hasErrorOccured() {
    if (!com) return true;
    return com->hasErrorOccured();
}

QString QF3TMCLProtocolHandler::getLastError() {
    if (!com) return QObject::tr("no COM port given to QF3TMCLProtocolHandler");
    return com->getLastError().c_str();
}

bool QF3TMCLProtocolHandler::checkComConnected() {
    if (!com) return false;
    bool c=com->isConnectionOpen();
    if (!c) {
        if (log) log->log_error(QObject::tr("%1 Could not connect to %2!!!\n").arg(LOG_PREFIX).arg(name));
        if (log) log->log_error(QObject::tr("%1 reason: %2\n").arg(LOG_PREFIX).arg(com->getLastError().c_str()));
    }
    return c;
}
