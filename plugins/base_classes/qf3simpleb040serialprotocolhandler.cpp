#include "qf3simpleb040serialprotocolhandler.h"
#include <QObject>

QF3SimpleB040SerialProtocolHandler::QF3SimpleB040SerialProtocolHandler(JKSerialConnection* com, QString name) {
    this->com=com;
    this->log=NULL;
    this->LOG_PREFIX="";
    this->name=name;
}

void QF3SimpleB040SerialProtocolHandler::setLogging(QFPluginLogService* log, QString LOG_PREFIX) {
    this->log=log;
    this->LOG_PREFIX=LOG_PREFIX;
}

void QF3SimpleB040SerialProtocolHandler::sendCommand(QString command) {
    if (!com) return;
    com->write(QString(command+"\n").toStdString());
}

QString QF3SimpleB040SerialProtocolHandler::queryCommand(QString command) {
    if (!com) return "";
    std::string res="";
    //std::cout<<"\n\ncommand: '"<<command<<"'\n";
    com->clearBuffer();
    if (com->write(QString(command+"\n").toStdString())) {
        res=com->readUntil("\n\n");
        //std::cout<<" ... reading ... ";
    }
    //std::cout<<"   direct_result: '"<<toprintablestr(res)<<"' ";
    checkComError();
    //if (res.size()>=2) res=res.erase(res.size()-2, 2);
    //std::cout<<"   returned_result: '"<<toprintablestr(res)<<"'\n\n";
    QString data="";
    for (int i=0; i<res.size(); i++) {
        data+=QLatin1Char(res[i]);
    }
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

bool QF3SimpleB040SerialProtocolHandler::checkComConnected() {
    if (!com) return false;
    bool c=com->isConnectionOpen();
    if (!c) {
        if (log) log->log_error(QObject::tr("%1 Could not connect to %2!!!\n").arg(LOG_PREFIX).arg(name));
        if (log) log->log_error(QObject::tr("%1 reason: %2\n").arg(LOG_PREFIX).arg(com->getLastError().c_str()));
    }
    return c;
}

