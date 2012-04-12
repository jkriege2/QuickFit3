#include "qf3tmclprotocolhandler.h"
#include <QDebug>
#include <QTime>


#include <QObject>

QF3TMCLProtocolHandler::QF3TMCLProtocolHandler(JKSerialConnection* com, QString name) {
    this->com=com;
    this->log=NULL;
    this->LOG_PREFIX="";
    this->name=name;
}

void QF3TMCLProtocolHandler::setLogging(QFPluginLogService* log, QString LOG_PREFIX) {
    this->log=log;
    this->LOG_PREFIX=LOG_PREFIX;
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
    while (retry<10 && !done) {
        //qDebug()<<"queryCommand "<<Command<<"   retry="<<retry;
        ok=sendCommand(Address, Command, Type, Motor, Value);
        QTime t;
        t.start();
        while (t.elapsed()<30);
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

int QF3TMCLProtocolHandler::readResult(uint8_t *Address, uint8_t *Status, int32_t *Value) {
    if (!com) return false;

    UCHAR RxBuffer[9], Checksum;


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
