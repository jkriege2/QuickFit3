#include "qfextensionlinearstagepi2protocolhandler.h"

QFExtensionLinearStagePI2ProtocolHandler::QFExtensionLinearStagePI2ProtocolHandler(JKSerialConnection *com, QMutex* mutexSerial, QString name)
{
    this->com=com;
    this->log=NULL;
    this->LOG_PREFIX="";
    this->name=name;
    this->mutexSerial=mutexSerial;
}

void QFExtensionLinearStagePI2ProtocolHandler::setLogging(QFPluginLogService* log, QString LOG_PREFIX) {
    this->log=log;
    this->LOG_PREFIX=LOG_PREFIX;
}

bool QFExtensionLinearStagePI2ProtocolHandler::checkComConnected()
{
    QMutexLocker lock(mutexSerial);
    bool c=com.isConnectionOpen();
    if (!c) {
        if (log) log->log->log_error(tr(LOG_PREFIX+ " Could not connect to Mercury C-863 Motor Controller!!!\n"));
        if (log) log->log_error(tr(LOG_PREFIX+ " reason: %1\n").arg(com.getLastError().c_str()));
        for (int i=0; i<axes.size(); i++) {
            axes[i].state=QFExtensionLinearStage::Error;
        }
    }
    return c;
}

void QFExtensionLinearStagePI2ProtocolHandler::selectAxis(int i)
{
    QMutexLocker lock(mutexSerial);
    if ((i>=0)&&(i<axes.size())) {
        QChar IDx=axes[i].ID;
        if (currentID!=IDx) {
            //std::cout<<"selecting motor "<<IDx<<"\n";
            std::string s(2, IDx.toLatin1());
            s[0]=1;
            com.write(s);
            //com.write(s);
            checkComError();
            queryCommand("TB");
            currentID=IDx;
        }
    }
}

void QFExtensionLinearStagePI2ProtocolHandler::sendCommand(std::string command)
{
    com.write(command+"\x0D");
}

std::string QFExtensionLinearStagePI2ProtocolHandler::queryCommand(std::string command)
{
    std::string res="";
    //std::cout<<"\n\ncommand (stage "<<currentID<<"): '"<<command<<"'";
    com.clearBuffer();
    if (com.write(command+"\x0D")) {
        res=com.readUntil(3);
        //std::cout<<" ... reading ... ";
    }
    //std::cout<<"   direct_result: '"<<toprintablestr(res)<<"' ";
    checkComError();
    if (res.size()>=3) res=res.erase(res.size()-3, 3);
    //std::cout<<"   returned_result: '"<<toprintablestr(res)<<"'\n\n";
    return res;
}

void QFExtensionLinearStagePI2ProtocolHandler::checkComError()
{
    QMutexLocker lock(mutexSerial);
    if (com.hasErrorOccured()) {
        for (int i=0; i<axes.size(); i++) {
            axes[i].state=QFExtensionLinearStage::Error;
            if (log) log->log_error(tr(LOG_PREFIX+ " serial port error: %1\n").arg(com.getLastError().c_str()));
        }
    }
}


