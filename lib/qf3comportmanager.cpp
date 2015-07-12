/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg
	
	

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

int QF3ComPortManager::addCOMPort(QSettings &settings, QString prefix, int defaultSpeed, QFSCdatabits defaultdatabits, QFSCparity defaultparity, QFSChandshaking defaulthandshaking, QFSCstopbits defaultstopbits, int defaulttimeout) {
#ifdef Q_OS_WIN32
    QString port=settings.value(prefix+"port", "COM1").toString();
#else
    QString port=settings.value(prefix+"port", "/dev/ttyS0").toString();
#endif
    int speed=settings.value(prefix+"port_speed", defaultSpeed).toInt();
    QFSCdatabits databits=string2QFSCdatabits(settings.value(prefix+"port_databits", QFSCdatabits2string(defaultdatabits).c_str()).toString().toStdString());
    QFSCparity parity=string2QFSCparity(settings.value(prefix+"port_parity", QFSChandshaking2string(defaulthandshaking).c_str()).toString().toStdString());
    QFSChandshaking handshaking=string2QFSChandshaking(settings.value(prefix+"port_handshaking", QFSCparity2string(defaultparity).c_str()).toString().toStdString());
    QFSCstopbits stopbits=string2QFSCstopbits(settings.value(prefix+"port_stopbits", QFSCstopbits2string(defaultstopbits).c_str()).toString().toStdString());
    unsigned int timeout=settings.value(prefix+"port_timeout", defaulttimeout).toInt();

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
        p.com=new QFSerialConnection(port.toStdString(), speed, databits, parity, stopbits, handshaking);
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
        if (coms[port].com->get_databits()!=QFSC8databits) settings.setValue(prefix+"port_databits", QString(QFSCdatabits2string(coms[port].com->get_databits()).c_str()));
        if (coms[port].com->get_parity()!=QFSCnoParity) settings.setValue(prefix+"port_parity", QString(QFSCparity2string(coms[port].com->get_parity()).c_str()));
        if (coms[port].com->get_handshaking()!=QFSCnoHandshaking) settings.setValue(prefix+"port_handshaking", QString(QFSChandshaking2string(coms[port].com->get_handshaking()).c_str()));
        if (coms[port].com->get_stopbits()!=QFSConeStopbit) settings.setValue(prefix+"port_stopbits", QString(QFSCstopbits2string(coms[port].com->get_stopbits()).c_str()));
        if (coms[port].com->get_timeout_ms()!=500) settings.setValue(prefix+"port_timeout", coms[port].com->get_timeout_ms());
    }
}

QFSerialConnection* QF3ComPortManager::getCOMPort(int port) const {
    if (port>=0 && port<coms.size()) return coms.at(port).com;
    return NULL;
}

QMutex *QF3ComPortManager::getMutex(int port) const {
    if (port>=0 && port<coms.size()) return coms.at(port).mutex;
    return NULL;
}
