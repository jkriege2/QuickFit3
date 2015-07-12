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

#include "qfsimpleserialprotocolhandler.h"


#include <QObject>
#include <QTime>
#include <QDebug>

QFSimpleSerialProtocolHandler::QFSimpleSerialProtocolHandler(QFSerialConnection *com, QString name, const QString &sendTerminationString, const QString &receiveReadUntilString)
{
    this->com=com;
    this->log=NULL;
    this->LOG_PREFIX="";
    this->name=name;
    this->sendTerminationString=sendTerminationString;
    this->receiveReadUntilString=receiveReadUntilString;
}

void QFSimpleSerialProtocolHandler::setLogging(QFPluginLogService* log, QString LOG_PREFIX) {
    this->log=log;
    this->LOG_PREFIX=LOG_PREFIX;
}

void QFSimpleSerialProtocolHandler::sendCommand(QString command) {
    if (!com) return;
    com->write(QString(command+sendTerminationString).toStdString());
}

QString QFSimpleSerialProtocolHandler::queryCommand(QString command) {
    if (!com) return "";
    std::string res="";
    com->clearBuffer();
    com->clearBuffer();
    //qDebug()<<"querying: "<<QString(toprintablestr(QString(command+sendTerminationString).toStdString()).c_str())<<"   timeout="<<com->get_timeout_ms()<<"ms";
    if (com->write(QString(command+sendTerminationString).toStdString())) {
        QTime t;
        t.start();
        res=com->readUntil(receiveReadUntilString.toStdString());
    }
    //qDebug()<<"  received: "<<QString(toprintablestr(res).c_str());
    checkComError();
    QString data="";
    for (unsigned int i=0; i<res.size(); i++) {
        if (res[i]>0) data+=QLatin1Char(res[i]);
    }
    return data.left(data.size()-receiveReadUntilString.size());
}

void QFSimpleSerialProtocolHandler::checkComError() {
    if (!com) return;
    if (com->hasErrorOccured()) {
        if (log) log->log_error(QObject::tr("%1 serial port error: %2\n").arg(LOG_PREFIX).arg(com->getLastError().c_str()));
    }
}

bool QFSimpleSerialProtocolHandler::hasErrorOccured() {
    if (!com) return true;
    return com->hasErrorOccured();
}

QString QFSimpleSerialProtocolHandler::getLastError() {
    if (!com) return QObject::tr("no COM port given to %1").arg(name);
    return com->getLastError().c_str();
}

void QFSimpleSerialProtocolHandler::setSendTerminationString(const QString &str)
{
    sendTerminationString=str;
}

void QFSimpleSerialProtocolHandler::setReceiveUntilString(const QString &str)
{
    receiveReadUntilString=str;
}

bool QFSimpleSerialProtocolHandler::checkComConnected() {
    if (!com) return false;
    bool c=com->isConnectionOpen();
    if (!c) {
        if (log) log->log_error(QObject::tr("%1 Could not connect to %2!!!\n").arg(LOG_PREFIX).arg(name));
        if (log) log->log_error(QObject::tr("%1 reason: %2\n").arg(LOG_PREFIX).arg(com->getLastError().c_str()));
    }
    return c;
}

