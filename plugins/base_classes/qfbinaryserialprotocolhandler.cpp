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

#include "qfbinaryserialprotocolhandler.h"


#include <QObject>
#include <QTime>
#include <QDebug>

QFBinarySerialProtocolHandler::QFBinarySerialProtocolHandler(QFSerialConnection *com, QString name)
{
    this->com=com;
    this->log=NULL;
    this->LOG_PREFIX="";
    this->name=name;
}

void QFBinarySerialProtocolHandler::setLogging(QFPluginLogService* log, QString LOG_PREFIX) {
    this->log=log;
    this->LOG_PREFIX=LOG_PREFIX;
}

void QFBinarySerialProtocolHandler::sendCommand(const QByteArray &command) {
    if (!com) return;
    //qDebug()<<"sending: "<<command.toHex();
    com->write((uint8_t*)command.data(), command.size());
}

QByteArray QFBinarySerialProtocolHandler::queryCommand(const QByteArray &command, int resultSize) {
    QByteArray result;
    if (!com) return result;
    std::string res="";
    com->clearBuffer();
    com->clearBuffer();
    result=QByteArray(resultSize, 0);
    //qDebug()<<"querying: "<<command.toHex()<<"   expecting "<<resultSize<<" bytes back!";
    //qDebug()<<"querying: "<<QString(toprintablestr(QString(command+sendTerminationString).toStdString()).c_str())<<"   timeout="<<com->get_timeout_ms()<<"ms";
    if (com->write((uint8_t*)command.data(), command.size())) {
        QTime t;
        t.start();
        com->read((uint8_t*)result.data(), resultSize);
    }
    //qDebug()<<"received: "<<result.toHex();
    //qDebug()<<"  received: "<<QString(toprintablestr(res).c_str());
    checkComError();
    QString data="";
    return result;
}

QByteArray QFBinarySerialProtocolHandler::queryCommand(const QByteArray &command, const QByteArray &endMessage, int resultMaxSize)
{
    QByteArray result;
    if (!com) return result;
    com->clearBuffer();
    com->clearBuffer();
    result.resize(resultMaxSize);
    //qDebug()<<"querying: "<<QString(toprintablestr(QString(command+sendTerminationString).toStdString()).c_str())<<"   timeout="<<com->get_timeout_ms()<<"ms";
    if (com->write((uint8_t*)command.data(), command.size())) {
        QTime t;
        t.start();
        int newSize=com->readUntil(result.data(), resultMaxSize, endMessage.data(), endMessage.size());
        result.resize(newSize);
    }
    //qDebug()<<"  received: "<<QString(toprintablestr(res).c_str());
    checkComError();

    return result.left(result.size()-endMessage.size());
}

void QFBinarySerialProtocolHandler::checkComError() {
    if (!com) return;
    if (com->hasErrorOccured()) {
        if (log) log->log_error(QObject::tr("%1 serial port error: %2\n").arg(LOG_PREFIX).arg(com->getLastError().c_str()));
    }
}

bool QFBinarySerialProtocolHandler::hasErrorOccured() {
    if (!com) return true;
    return com->hasErrorOccured();
}

QString QFBinarySerialProtocolHandler::getLastError() {
    if (!com) return QObject::tr("no COM port given to %1").arg(name);
    return com->getLastError().c_str();
}

bool QFBinarySerialProtocolHandler::checkComConnected() {
    if (!com) return false;
    bool c=com->isConnectionOpen();
    if (!c) {
        if (log) log->log_error(QObject::tr("%1 Could not connect to %2!!!\n").arg(LOG_PREFIX).arg(name));
        if (log) log->log_error(QObject::tr("%1 reason: %2\n").arg(LOG_PREFIX).arg(com->getLastError().c_str()));
    }
    return c;
}

