#include "qfextensionlinearstagepi2protocolhandler.h"

QFExtensionLinearStagePI2ProtocolHandler::QFExtensionLinearStagePI2ProtocolHandler(JKSerialConnection *com, QMutex* mutexSerial, QString name)
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

JKSerialConnection *QFExtensionLinearStagePI2ProtocolHandler::getCOM() const
{
    return com;
}


