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

#include "qf3simpleb040serialprotocolhandler.h"
#include <QObject>
#include <QTime>
#include <QDebug>

QF3SimpleB040SerialProtocolHandler::QF3SimpleB040SerialProtocolHandler(JKSerialConnection* com, QString name) {
    this->com=com;
    this->log=NULL;
    this->LOG_PREFIX="";
    this->name=name;
    addToCommand="\n";
}

void QF3SimpleB040SerialProtocolHandler::setLogging(QFPluginLogService* log, QString LOG_PREFIX) {
    this->log=log;
    this->LOG_PREFIX=LOG_PREFIX;
}

void QF3SimpleB040SerialProtocolHandler::sendCommand(QString command) {
    if (!com) return;
    com->write(QString(command+addToCommand).toStdString());
}

QString QF3SimpleB040SerialProtocolHandler::queryCommand(QString command) {
    if (!com) return "";
    std::string res="";
    //qDebug()<<"\n\ncommand: '"<<command<<"'";
    com->clearBuffer();
    com->clearBuffer();
    if (com->write(QString(command+addToCommand).toStdString())) {
        QTime t;
        t.start();
        //while (t.elapsed()<1);
        res=com->readUntil("\n\n");
        //std::cout<<" ... reading ... ";
    }
    //qDebug()<<"   direct_result: '"<<toprintablestr(res).c_str()<<"' ";
    checkComError();
    //if (res.size()>=2) res=res.erase(res.size()-2, 2);
    //qDebug()<<"   returned_result: '"<<toprintablestr(res).c_str()<<"'";
    QString data="";
    for (unsigned int i=0; i<res.size(); i++) {
        if (res[i]>0) data+=QLatin1Char(res[i]);
    }
    //qDebug()<<"queryCommand("<<command<<")   "<<data.left(data.size()-2);
    return data.left(data.size()-2);
}

void QF3SimpleB040SerialProtocolHandler::checkComError() {
    if (!com) return;
    if (com->hasErrorOccured()) {
        if (log) log->log_error(QObject::tr("%1 serial port error: %2\n").arg(LOG_PREFIX).arg(com->getLastError().c_str()));
    }
}

bool QF3SimpleB040SerialProtocolHandler::hasErrorOccured() {
    if (!com) return true;
    return com->hasErrorOccured();
}

QString QF3SimpleB040SerialProtocolHandler::getLastError() {
    if (!com) return QObject::tr("no COM port given to QF3SimpleB040SerialProtocolHandler");
    return com->getLastError().c_str();
}

void QF3SimpleB040SerialProtocolHandler::setAddToCommand(const QString &add)
{
    addToCommand=add;
}

bool QF3SimpleB040SerialProtocolHandler::checkComConnected() {
    if (!com) return false;
    bool c=com->isConnectionOpen();
    if (!c) {
        if (log) log->log_error(QObject::tr("%1 Could not connect to %2!!!\n").arg(LOG_PREFIX).arg(name));
        if (log) log->log_error(QObject::tr("%1 reason: %2\n").arg(LOG_PREFIX).arg(com->getLastError().c_str()));
    }
    return c;
}

