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

#include "qfextensionpccsled.h"
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtGlobal>
#include <QtWidgets>
#else
#include <QtGui>
#endif


#include <QtPlugin>
#include <iostream>

#define LIGHTSOURCE_ACTION_DURATION 1
#define LOG_PREFIX "[LightSourcePCCPLED]: "
#define GLOBAL_CONFIGFILE "lights_pccsled.ini"
#define CONNECTION_DELAY_MS 200

QFExtensionPCCSLED::QFExtensionPCCSLED(QObject* parent):
    QObject(parent)
{
	logService=NULL;
}

QFExtensionPCCSLED::~QFExtensionPCCSLED() {

}



void QFExtensionPCCSLED::projectChanged(QFProject* oldProject, QFProject* project) {
	/* usually cameras do not have to react to a change of the project in QuickFit .. so you don't need to do anything here
	   But: possibly you could read config information from the project here
	 */
}

void QFExtensionPCCSLED::initExtension() {
    /* do initializations here but do not yet connect to the camera! */
    QString ini=services->getGlobalConfigFileDirectory()+QString("/%1").arg(GLOBAL_CONFIGFILE);
    if (!QFile::exists(ini)) ini=services->getConfigFileDirectory()+QString("/%1").arg(GLOBAL_CONFIGFILE);
    if (!QFile::exists(ini)) ini=services->getAssetsDirectory()+QString("/plugins/")+getID()+QString("/%1").arg(GLOBAL_CONFIGFILE);
    QSettings inifile(ini, QSettings::IniFormat);
    int count=inifile.value("lightsource_count", 0).toUInt();
    for (int i=0; i<count; i++) {
        LIGHTSOURCE s;
        s.port=ports.addCOMPort(inifile, "lightsource"+QString::number(i+1)+"/");
        s.infoMessage="";
        s.lastAction=QTime::currentTime();
        s.serial=new QF3SimpleB040SerialProtocolHandler(ports.getCOMPort(s.port), getName());
        sources.append(s);
    }
    //qDebug()<<sources.size()<<" light sources";
}


void QFExtensionPCCSLED::loadSettings(ProgramOptions* settingspo) {
	/* here you could read config information from the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini
	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFExtensionPCCSLED::storeSettings(ProgramOptions* settingspo) {
	/* here you could write config information to the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini

	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFExtensionPCCSLED::deinit() {
    /* add code for cleanup here */
    QSettings inifile(services->getGlobalConfigFileDirectory()+QString("/%1").arg(GLOBAL_CONFIGFILE), QSettings::IniFormat);
    if (inifile.isWritable()) {
        inifile.setValue("lightsource_count", getLightSourceCount());
        for (int i=0; i<sources.size(); i++) {
            int p=sources[i].port;
            if (ports.getCOMPort(p)) {
                ports.storeCOMPort(p, inifile, "lightsource"+QString::number(i+1)+"/");
            }
        }
    }
}

unsigned int QFExtensionPCCSLED::getLightSourceCount() {
    return sources.size();
}

void QFExtensionPCCSLED::lightSourceConnect(unsigned int lightSource) {
    if (lightSource<0 || lightSource>=getLightSourceCount()) return;
    JKSerialConnection* com=ports.getCOMPort(sources[lightSource].port);
    if (!com) return;
    QMutex* mutex=ports.getMutex(sources[lightSource].port);
    QMutexLocker locker(mutex);
    com->open();
    if (com->isConnectionOpen()) {
        QTime t;
        t.start();
        // wait CONNECTION_DELAY_MS ms for connection!
        while (t.elapsed()<CONNECTION_DELAY_MS)  {
            QApplication::processEvents();
        }
        sources[lightSource].infoMessage=sources[lightSource].serial->queryCommand("?");
        //qDebug()<<"infoMessage '"<<infoMessage<<"'";
        if (!(sources[lightSource].infoMessage.toLower().contains("pccs") && sources[lightSource].infoMessage.toLower().contains("jan krieger"))) {
            com->close();
            log_error(tr("%1 Could not connect to Light Source Driver [port=%1  baud=%2]!!!\n").arg(com->get_port().c_str()).arg(com->get_baudrate()));
            log_error(tr("%1 reason: received wrong ID string from light source driver: string was '%2'\n").arg(LOG_PREFIX).arg(sources[lightSource].infoMessage));
        }
    } else {
        log_error(tr("%1 Could not connect to Light Source Driver [port=%1  baud=%2]!!!\n").arg(com->get_port().c_str()).arg(com->get_baudrate()));
        log_error(tr("%1 reason: %2\n").arg(LOG_PREFIX).arg(sources[lightSource].serial->getLastError()));
    }
}

void QFExtensionPCCSLED::lightSourceDisonnect(unsigned int lightSource) {
    if (lightSource<0 || lightSource>=getLightSourceCount()) return;
    JKSerialConnection* com=ports.getCOMPort(sources[lightSource].port);
    if (!com) return;
    QMutex* mutex=ports.getMutex(sources[lightSource].port);
    QMutexLocker locker(mutex);
    com->close();
}

void QFExtensionPCCSLED::setLightSourceLogging(QFPluginLogService *logService) {
    this->logService=logService;
}

bool QFExtensionPCCSLED::isLightSourceConnected(unsigned int lightSource) {
    if (lightSource<0 || lightSource>=getLightSourceCount()) return false;
    JKSerialConnection* com=ports.getCOMPort(sources[lightSource].port);
    if (!com) return false;
    QMutex* mutex=ports.getMutex(sources[lightSource].port);
    QMutexLocker locker(mutex);
    return com->isConnectionOpen();
}

unsigned int QFExtensionPCCSLED::getLightSourceLineCount(unsigned int lightSource) {
    if (lightSource<0 || lightSource>=getLightSourceCount()) return 0;
    JKSerialConnection* com=ports.getCOMPort(sources[lightSource].port);
    if (!com) return 0;
    return 1;
}

QString QFExtensionPCCSLED::getLightSourceLineDescription(unsigned int lightSource, unsigned int wavelengthLine) {
    if (lightSource<0 || lightSource>=getLightSourceCount()) return "";
    JKSerialConnection* com=ports.getCOMPort(sources[lightSource].port);
    QF3SimpleB040SerialProtocolHandler* serial=sources[lightSource].serial;
    if (!com || !serial) return QString("");
    QMutex* mutex=ports.getMutex(sources[lightSource].port);
    QMutexLocker locker(mutex);
    QString led=serial->queryCommand("Y").trimmed();
    QString manufacturer=serial->queryCommand("q").trimmed();
    QString wavelength=serial->queryCommand("j").trimmed();
    QString power=serial->queryCommand("f").trimmed();
    QString name=manufacturer;
    if (!name.isEmpty()) name+=" ";
    name+=led;
    if (wavelength.size()>0 || power.size()>0) {
        name+="(";
        name+=wavelength;
        if (wavelength.size()>0 && power.size()>0) name+=", ";
        name+=power;
        name+=")";
    }
    return name;
    //return tr("line #%1").arg(wavelengthLine+1);
}

void QFExtensionPCCSLED::getLightSourceLinePowerRange(unsigned int lightSource, unsigned int wavelengthLine, double &minimum, double &maximum) {
    minimum=maximum=0;
    if (lightSource<0 || lightSource>=getLightSourceCount()) return;
    JKSerialConnection* com=ports.getCOMPort(sources[lightSource].port);
    QF3SimpleB040SerialProtocolHandler* serial=sources[lightSource].serial;
    if (!com || !serial) return ;
    QMutex* mutex=ports.getMutex(sources[lightSource].port);
    QMutexLocker locker(mutex);
    minimum=0;
    maximum=serial->queryCommand("X").toDouble();
}

QString QFExtensionPCCSLED::getLightSourceLinePowerUnit(unsigned int lightSource, unsigned int wavelengthLine) {
    return tr("mA");
}

void QFExtensionPCCSLED::setLightSourcePower(unsigned int lightSource, unsigned int wavelengthLine, double power) {
    if (lightSource<0 || lightSource>=getLightSourceCount()) return;
    JKSerialConnection* com=ports.getCOMPort(sources[lightSource].port);
    QF3SimpleB040SerialProtocolHandler* serial=sources[lightSource].serial;
    if (!com || !serial) return ;
    QMutex* mutex=ports.getMutex(sources[lightSource].port);
    QMutexLocker locker(mutex);
    serial->sendCommand(QString("I%1").arg((int)round(power)));
    sources[lightSource].lastAction=QTime::currentTime();
}

double QFExtensionPCCSLED::getLightSourceCurrentSetPower(unsigned int lightSource, unsigned int wavelengthLine) {
    if (lightSource<0 || lightSource>=getLightSourceCount()) return 0;
    JKSerialConnection* com=ports.getCOMPort(sources[lightSource].port);
    QF3SimpleB040SerialProtocolHandler* serial=sources[lightSource].serial;
    if (!com || !serial) return 0;
    QMutex* mutex=ports.getMutex(sources[lightSource].port);
    QMutexLocker locker(mutex);
    return serial->queryCommand("G").toDouble();
}

double QFExtensionPCCSLED::getLightSourceCurrentMeasuredPower(unsigned int lightSource, unsigned int wavelengthLine) {
    if (lightSource<0 || lightSource>=getLightSourceCount()) return 0;
    JKSerialConnection* com=ports.getCOMPort(sources[lightSource].port);
    QF3SimpleB040SerialProtocolHandler* serial=sources[lightSource].serial;
    if (!com || !serial) return 0;
    QMutex* mutex=ports.getMutex(sources[lightSource].port);
    QMutexLocker locker(mutex);
    return serial->queryCommand("C").toDouble();

}

void QFExtensionPCCSLED::setLightSourceLineEnabled(unsigned int lightSource, unsigned int wavelengthLine, bool enabled) {
    if (lightSource<0 || lightSource>=getLightSourceCount()) return ;
    JKSerialConnection* com=ports.getCOMPort(sources[lightSource].port);
    QF3SimpleB040SerialProtocolHandler* serial=sources[lightSource].serial;
    if (!com || !serial) return ;
    QMutex* mutex=ports.getMutex(sources[lightSource].port);
    QMutexLocker locker(mutex);
    if (enabled) serial->sendCommand("L1");
    else serial->sendCommand("L0");
    sources[lightSource].lastAction=QTime::currentTime();
}

bool QFExtensionPCCSLED::getLightSourceLineEnabled(unsigned int lightSource, unsigned int wavelengthLine) {
    if (lightSource<0 || lightSource>=getLightSourceCount()) return false;
    JKSerialConnection* com=ports.getCOMPort(sources[lightSource].port);
    QF3SimpleB040SerialProtocolHandler* serial=sources[lightSource].serial;
    if (!com || !serial) return false;
    QMutex* mutex=ports.getMutex(sources[lightSource].port);
    QMutexLocker locker(mutex);
    return !serial->queryCommand("N").startsWith("0");
}

bool QFExtensionPCCSLED::isLastLightSourceActionFinished(unsigned int lightSource) {
    if (lightSource<0 || lightSource>=getLightSourceCount()) return false;
    JKSerialConnection* com=ports.getCOMPort(sources[lightSource].port);
    QF3SimpleB040SerialProtocolHandler* serial=sources[lightSource].serial;
    if (!com || !serial) return false;
    QMutex* mutex=ports.getMutex(sources[lightSource].port);
    QMutexLocker locker(mutex);
    return sources[lightSource].lastAction.elapsed()>LIGHTSOURCE_ACTION_DURATION;
}

QString QFExtensionPCCSLED::getLightSourceDescription(unsigned int lightSource) {
    if (lightSource<0 || lightSource>=getLightSourceCount()) return "";
    JKSerialConnection* com=ports.getCOMPort(sources[lightSource].port);
    QF3SimpleB040SerialProtocolHandler* serial=sources[lightSource].serial;
    if (!com || !serial) return QString("");
    return tr("Programmable Constant Current Source for LEDs");
}

QString QFExtensionPCCSLED::getLightSourceShortName(unsigned int lightSource) {
    if (lightSource<0 || lightSource>=getLightSourceCount()) return "";
    JKSerialConnection* com=ports.getCOMPort(sources[lightSource].port);
    QF3SimpleB040SerialProtocolHandler* serial=sources[lightSource].serial;
    if (!com || !serial) return QString("");
    return tr("PCCS");
}

void QFExtensionPCCSLED::log_text(QString message) {
    if (logService) logService->log_text(LOG_PREFIX+message);
    else if (services) services->log_text(LOG_PREFIX+message);
}

void QFExtensionPCCSLED::log_warning(QString message) {
    if (logService) logService->log_warning(LOG_PREFIX+message);
    else if (services) services->log_warning(LOG_PREFIX+message);
}

void QFExtensionPCCSLED::log_error(QString message) {
    if (logService) logService->log_error(LOG_PREFIX+message);
    else if (services) services->log_error(LOG_PREFIX+message);
}

void QFExtensionPCCSLED::showLightSourceSettingsDialog(unsigned int lightSource, QWidget *parent) {
    QString ini1=services->getGlobalConfigFileDirectory()+QString("/")+GLOBAL_CONFIGFILE;
    QString ini2=services->getConfigFileDirectory()+QString("/")+GLOBAL_CONFIGFILE;
    QString ini3=services->getAssetsDirectory()+QString("/plugins/")+getID()+QString("/")+GLOBAL_CONFIGFILE;
    QMessageBox::information(parent, getName(), tr("There is no configuration dialog for this device. Set all config in the appropriate ini file:\n  %1\n  or: %2\n  or: %3").arg(ini1).arg(ini2).arg(ini3));
}


unsigned int QFExtensionPCCSLED::getShutterCount()
{
    return getLightSourceCount();
}

void QFExtensionPCCSLED::shutterConnect(unsigned int shutter) {
    lightSourceConnect(shutter);
}

void QFExtensionPCCSLED::shutterDisonnect(unsigned int shutter) {
    lightSourceDisonnect(shutter);
}

void QFExtensionPCCSLED::setShutterLogging(QFPluginLogService *logService) {
    setLightSourceLogging(logService);
}

bool QFExtensionPCCSLED::isShutterConnected(unsigned int shutter) {
    return isLightSourceConnected(shutter);
}

bool QFExtensionPCCSLED::isShutterOpen(unsigned int shutter) {
    return getLightSourceLineEnabled(shutter, 0);
}

void QFExtensionPCCSLED::setShutterState(unsigned int shutter, bool opened) {
    setLightSourceLineEnabled(shutter, 0, opened);
}

bool QFExtensionPCCSLED::isLastShutterActionFinished(unsigned int shutter) {
    return isLastLightSourceActionFinished(shutter);
}

QString QFExtensionPCCSLED::getShutterDescription(unsigned int shutter) {
    return tr("shutter for")+getLightSourceDescription(shutter);
}

QString QFExtensionPCCSLED::getShutterShortName(unsigned int shutter) {
    return tr("shutter: ")+getLightSourceShortName(shutter);
}

void QFExtensionPCCSLED::showShutterSettingsDialog(unsigned int shutter, QWidget *parent) {
    showLightSourceSettingsDialog(shutter, parent);
}










Q_EXPORT_PLUGIN2(lights_pccsled, QFExtensionPCCSLED)
