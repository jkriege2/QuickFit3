/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

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

#include "qfextensionlinearstagepi2protocolhandler.h"

QMap<QFSerialConnection*, QChar> QFExtensionLinearStagePI2ProtocolHandler::currentID=QMap<QFSerialConnection*, QChar>();

QFExtensionLinearStagePI2ProtocolHandler::QFExtensionLinearStagePI2ProtocolHandler(QFSerialConnection *com, QMutex* mutexSerial, QString name)
{
    this->com=com;
    this->log=NULL;
    this->LOG_PREFIX="";
    this->name=name;
    this->mutexSerial=mutexSerial;
    currentID[com]=QChar(0);
}

void QFExtensionLinearStagePI2ProtocolHandler::setLogging(QFPluginLogService* log, QString LOG_PREFIX) {
    this->log=log;
    this->LOG_PREFIX=LOG_PREFIX;
}

void QFExtensionLinearStagePI2ProtocolHandler::resetCurrentAxis()
{
    currentID[com]=QChar(0);
}

bool QFExtensionLinearStagePI2ProtocolHandler::checkComConnected()
{
    bool c=com->isConnectionOpen();

    return c;
}

void QFExtensionLinearStagePI2ProtocolHandler::selectAxis(QChar ID)
{
    if (!currentID.contains(com) || currentID[com]!=ID) {
        //std::cout<<"selecting motor "<<IDx<<"\n";
        std::string s(2, ID.toLatin1());
        s[0]=1;
        com->write(s);
        //com->write(s);
        queryCommand("TB");
        currentID[com]=ID;
    }
}

void QFExtensionLinearStagePI2ProtocolHandler::sendCommand(std::string command)
{
    com->write(command+"\x0D");
}

std::string QFExtensionLinearStagePI2ProtocolHandler::queryCommand(std::string command)
{
    std::string res="";
    //std::cout<<"\n\ncommand (stage "<<currentID<<"): '"<<command<<"'";
    com->clearBuffer();
    if (com->write(command+"\x0D")) {
        res=com->readUntil(3);
        //std::cout<<" ... reading ... ";
    }
    //std::cout<<"   direct_result: '"<<toprintablestr(res)<<"' ";
    if (res.size()>=3) res=res.erase(res.size()-3, 3);
    //std::cout<<"   returned_result: '"<<toprintablestr(res)<<"'\n\n";
    return res;
}

QMutex *QFExtensionLinearStagePI2ProtocolHandler::getMutex() const
{
    return mutexSerial;
}

QFSerialConnection *QFExtensionLinearStagePI2ProtocolHandler::getCOM() const
{
    return com;
}


