#include "qf3comportmanager.h"

QF3ComPortManager::QF3ComPortManager()
{
}

QF3ComPortManager::~QF3ComPortManager()
{
    clear();
}

void QF3ComPortManager::clear()
{
    for (int i=0; i<coms.size(); i++) {
        coms[i].com->close();
        delete coms[i].com;
        delete coms[i].mutex;
    }
    coms.clear();
}


int QF3ComPortManager::addCOMPort(QSettings& settings, QString prefix) {
#ifdef Q_OS_WIN32
    QString port=settings.value(prefix+"port", "COM1").toString();
#else
    QString port=settings.value(prefix+"port", "/dev/ttyS0").toString();
#endif
    int speed=settings.value(prefix+"port_speed", "9600").toInt();
    JKSCdatabits databits=string2JKSCdatabits(settings.value(prefix+"port_databits", "8").toString().toStdString());
    JKSCparity parity=string2JKSCparity(settings.value(prefix+"port_parity", "none").toString().toStdString());
    JKSChandshaking handshaking=string2JKSChandshaking(settings.value(prefix+"port_handshaking", "none").toString().toStdString());
    JKSCstopbits stopbits=string2JKSCstopbits(settings.value(prefix+"port_stopbits", "1").toString().toStdString());
    unsigned int timeout=settings.value(prefix+"port_timeout", 500).toInt();

    int portID=-1;
    for (int i=0; i<coms.size(); i++) {
        if (coms[i].com->get_port()==port.toStdString()) {
            portID=i;
            break;
        }
    }

    if (portID<0) {
        portID=coms.size();
        COMPORTS p;
        p.com=new JKSerialConnection(port.toStdString(), speed, databits, parity, stopbits, handshaking);
        p.mutex=new QMutex(QMutex::Recursive);
        coms.append(p);
    }
    coms[portID].com->set_baudrate(speed);
    coms[portID].com->set_databits(databits);
    coms[portID].com->set_handshaking(handshaking);
    coms[portID].com->set_parity(parity);
    coms[portID].com->set_port(port.toStdString());
    coms[portID].com->set_stopbits(stopbits);
    coms[portID].com->set_timeout_ms(timeout);
    return portID;
}

void QF3ComPortManager::storeCOMPort(int port, QSettings& settings, QString prefix) {
    if (port>=0 && port<coms.size()) {
        settings.setValue(prefix+"port", QString(coms[port].com->get_port().c_str()));
        settings.setValue(prefix+"port_speed", coms[port].com->get_baudrate());
        if (coms[port].com->get_databits()!=JKSC8databits) settings.setValue(prefix+"port_databits", QString(JKSCdatabits2string(coms[port].com->get_databits()).c_str()));
        if (coms[port].com->get_parity()!=JKSCnoParity) settings.setValue(prefix+"port_parity", QString(JKSCparity2string(coms[port].com->get_parity()).c_str()));
        if (coms[port].com->get_handshaking()!=JKSCnoHandshaking) settings.setValue(prefix+"port_handshaking", QString(JKSChandshaking2string(coms[port].com->get_handshaking()).c_str()));
        if (coms[port].com->get_stopbits()!=JKSConeStopbit) settings.setValue(prefix+"port_stopbits", QString(JKSCstopbits2string(coms[port].com->get_stopbits()).c_str()));
        if (coms[port].com->get_timeout_ms()!=500) settings.setValue(prefix+"port_timeout", coms[port].com->get_timeout_ms());
    }
}

JKSerialConnection* QF3ComPortManager::getCOMPort(int port) const {
    if (port>=0 && port<coms.size()) return coms.at(port).com;
    return NULL;
}

QMutex *QF3ComPortManager::getMutex(int port) const {
    if (port>=0 && port<coms.size()) return coms.at(port).mutex;
    return NULL;
}
