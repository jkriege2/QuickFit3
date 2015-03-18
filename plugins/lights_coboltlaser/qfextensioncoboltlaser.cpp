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

#include "qfextensioncoboltlaser.h"
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtGlobal>
#include <QtWidgets>
#else
#include <QtGui>
#endif


#include <QtPlugin>
#include <iostream>

#define LIGHTSOURCE_ACTION_DURATION 1
#define LOG_PREFIX "[LightSourceCobolt]: "
#define GLOBAL_CONFIGFILE "lights_coboltlaser.ini"
#define CONNECTION_DELAY_MS 200

QFExtensionCoboltLaser::QFExtensionCoboltLaser(QObject* parent):
    QObject(parent)
{
    logService=NULL;
}

QFExtensionCoboltLaser::~QFExtensionCoboltLaser() {

}



void QFExtensionCoboltLaser::projectChanged(QFProject* oldProject, QFProject* project) {
    /* usually cameras do not have to react to a change of the project in QuickFit .. so you don't need to do anything here
       But: possibly you could read config information from the project here
     */

    Q_UNUSED(project);
    Q_UNUSED(oldProject);

}

void QFExtensionCoboltLaser::initExtension() {
    /* do initializations here but do not yet connect to the camera! */
    QString ini=services->getGlobalConfigFileDirectory()+QString("/%1").arg(GLOBAL_CONFIGFILE);
    if (!QFile::exists(ini)) ini=services->getConfigFileDirectory()+QString("/%1").arg(GLOBAL_CONFIGFILE);
    if (!QFile::exists(ini)) ini=services->getAssetsDirectory()+QString("/plugins/")+getID()+QString("/%1").arg(GLOBAL_CONFIGFILE);
    QFPluginServices::getInstance()->log_global_text(tr("%1loading INI-file %2\n").arg(QString(LOG_PREFIX)).arg(ini));
    QSettings inifile(ini, QSettings::IniFormat);
    int count=inifile.value("lightsource_count", 0).toUInt();
    for (int i=0; i<count; i++) {
        LIGHTSOURCE s;
        s.port=ports.addCOMPort(inifile, "lightsource"+QString::number(i+1)+"/", 115200, JKSC8databits, JKSCnoParity, JKSCnoHandshaking, JKSConeStopbit, 500);
        s.serialNumber="";
        s.setPower=0;
        s.lastAction=QTime::currentTime();
        s.serial=new QFCoboltLaserProtocolHandler(ports.getCOMPort(s.port), getName());
        s.type=string2CoboltLaserType(inifile.value("lightsource"+QString::number(i+1)+"/type", "DPSS4").toString());
        s.powerFactor_ParamToSend=0.001;
        if (inifile.contains("lightsource"+QString::number(i+1)+"/power_unit")) {
            QString pu=inifile.value("lightsource"+QString::number(i+1)+"/power_unit", "W").toString();
            if (pu.toLower()=="mw") s.powerFactor_ParamToSend=1;
            if (pu.toLower()=="uw") s.powerFactor_ParamToSend=1000;
            if (pu.toLower()=="kw") s.powerFactor_ParamToSend=0.000001;
        }
        if (inifile.contains("lightsource"+QString::number(i+1)+"/power_send_factor")) {
            s.powerFactor_ParamToSend=inifile.value("lightsource"+QString::number(i+1)+"/power_send_factor", 0.001).toDouble();
        }
        s.powerFactor_ParamRead=1;
        if (s.type==cltMLD6) s.powerFactor_ParamRead=1.0/s.powerFactor_ParamToSend;
        if (inifile.contains("lightsource"+QString::number(i+1)+"/power_read_factor")) {
            s.powerFactor_ParamRead=inifile.value("lightsource"+QString::number(i+1)+"/power_read_factor", s.powerFactor_ParamRead).toDouble();
        }
        s.max_power=inifile.value("lightsource"+QString::number(i+1)+"/max_power", 25).toDouble();
        s.wavelength=inifile.value("lightsource"+QString::number(i+1)+"/wavelength", 0).toDouble();
        s.label=inifile.value("lightsource"+QString::number(i+1)+"/label", tr("Cobolt %3: %1 %2nm").arg(coboltLaserType2HumanReadableString(s.type)).arg(s.wavelength).arg(i+1)).toString();
        s.line_enabled=true;
        s.setPower=s.max_power;
        if (s.wavelength<=0) {
            s.wavelength=decodeWavelengthAndLasername(s.label);
        }
        s.serialNumber="";
        sources.append(s);
    }
    //qDebug()<<sources.size()<<" light sources";
}


void QFExtensionCoboltLaser::loadSettings(ProgramOptions* settingspo) {
	/* here you could read config information from the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini
	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFExtensionCoboltLaser::storeSettings(ProgramOptions* settingspo) {
	/* here you could write config information to the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini

	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFExtensionCoboltLaser::deinit() {
    /* add code for cleanup here */
    QSettings inifile(services->getGlobalConfigFileDirectory()+QString("/%1").arg(GLOBAL_CONFIGFILE), QSettings::IniFormat);
    if (inifile.isWritable()) {
        inifile.setValue("lightsource_count", getLightSourceCount());
        for (int i=0; i<sources.size(); i++) {
            int p=sources[i].port;
            if (ports.getCOMPort(p)) {
                ports.storeCOMPort(p, inifile, "lightsource"+QString::number(i+1)+"/");
                inifile.setValue("lightsource"+QString::number(i+1)+"/type", coboltLaserType2String(sources[i].type));
                inifile.setValue("lightsource"+QString::number(i+1)+"/wavelength", sources[i].wavelength);
                inifile.setValue("lightsource"+QString::number(i+1)+"/max_power", sources[i].max_power);
                inifile.setValue("lightsource"+QString::number(i+1)+"/power_send_factor", sources[i].powerFactor_ParamToSend);
                inifile.setValue("lightsource"+QString::number(i+1)+"/label", sources[i].label);
            }
        }
    }
}

unsigned int QFExtensionCoboltLaser::getLightSourceCount() {
    return sources.size();
}

void QFExtensionCoboltLaser::lightSourceConnect(unsigned int lightSource) {
    if (lightSource<0 || lightSource>=getLightSourceCount()) return;
    JKSerialConnection* com=ports.getCOMPort(sources[lightSource].port);
    QFCoboltLaserProtocolHandler* serial=sources[lightSource].serial;
    if (!com || !serial) return;

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
        sources[lightSource].serialNumber=sources[lightSource].serial->queryCommand("sn?");
        //qDebug()<<"serialNumber '"<<sources[lightSource].serialNumber<<"'";
        if (sources[lightSource].serial->hasErrorOccured()) {
            com->close();
            log_error(tr("%1 Could not connect to Cobolt Laser [port=%2  baud=%3]!!!\n").arg(LOG_PREFIX).arg(com->get_port().c_str()).arg(com->get_baudrate()));
            log_error(tr("%1 reason: Could not read serial number! Error message was: %2\n").arg(LOG_PREFIX).arg(sources[lightSource].serial->getLastError()));
        } else {
            log_text(tr("%1 connected to Cobolt Laser [port=%2  baud=%3]!!!\n").arg(LOG_PREFIX).arg(com->get_port().c_str()).arg(com->get_baudrate()));
            log_text(tr("%1 label: %2\n").arg(LOG_PREFIX).arg(sources[lightSource].label));
            log_text(tr("%1 serial number: %2\n").arg(LOG_PREFIX).arg(sources[lightSource].serialNumber));
            log_text(tr("%1 laser type: %2\n").arg(LOG_PREFIX).arg(coboltLaserType2HumanReadableString(sources[lightSource].type)));
            log_text(tr("%1 wavelength: %2 nm\n").arg(LOG_PREFIX).arg(sources[lightSource].wavelength));
            log_text(tr("%1 max. power: %2 mW\n").arg(LOG_PREFIX).arg(sources[lightSource].max_power));

            sources[lightSource].setPower=CQStringToDouble(serial->queryCommand("p?"))*sources[lightSource].powerFactor_ParamRead;
            sources[lightSource].line_enabled=CQStringToDouble(serial->queryCommand("pa?"))>0.0;

            sources[lightSource].hours=0;
            if (sources[lightSource].type==cltMLD6) {
                sources[lightSource].hours=CQStringToDouble(sources[lightSource].serial->queryCommand("hrs?"));
                log_text(tr("%1 laser head operating hours: %2\n").arg(LOG_PREFIX).arg(sources[lightSource].hours));
                log_text(tr("%1 entering constant power mode\n").arg(LOG_PREFIX));
                serial->sendCommand(QString("cp"));

            }
            sources[lightSource].line_enabled=true;
           if (sources[lightSource].line_enabled) serial->sendCommand(QString("p %1").arg(sources[lightSource].setPower*sources[lightSource].powerFactor_ParamToSend,0,'f'));
           else  serial->sendCommand(QString("p 0"));

        }
    } else {
        log_error(tr("%1 Could not connect to Cobolt Laser [port=%1  baud=%2]!!!\n").arg(com->get_port().c_str()).arg(com->get_baudrate()));
        log_error(tr("%1 reason: %2\n").arg(LOG_PREFIX).arg(sources[lightSource].serial->getLastError()));
    }
}

void QFExtensionCoboltLaser::lightSourceDisonnect(unsigned int lightSource) {
    if (lightSource<0 || lightSource>=getLightSourceCount()) return;
    JKSerialConnection* com=ports.getCOMPort(sources[lightSource].port);
    if (!com) return;

    QMutex* mutex=ports.getMutex(sources[lightSource].port);
    QMutexLocker locker(mutex);
    com->close();
}

void QFExtensionCoboltLaser::setLightSourceLogging(QFPluginLogService *logService) {
    this->logService=logService;
}

bool QFExtensionCoboltLaser::isLightSourceConnected(unsigned int lightSource) {
    if (lightSource<0 || lightSource>=getLightSourceCount()) return false;
    JKSerialConnection* com=ports.getCOMPort(sources[lightSource].port);
    if (!com) return false;
    QMutex* mutex=ports.getMutex(sources[lightSource].port);
    QMutexLocker locker(mutex);
    return com->isConnectionOpen();
}

unsigned int QFExtensionCoboltLaser::getLightSourceLineCount(unsigned int lightSource) {
    if (lightSource<0 || lightSource>=getLightSourceCount()) return 0;
    JKSerialConnection* com=ports.getCOMPort(sources[lightSource].port);
    if (!com) return 0;
    return 1;
}

QString QFExtensionCoboltLaser::getLightSourceLineDescription(unsigned int lightSource, unsigned int wavelengthLine) {
    if (lightSource<0 || lightSource>=getLightSourceCount()) return "";
    return tr("%1nm").arg(sources[lightSource].wavelength);//.arg(wavelengthLine+1);
}

void QFExtensionCoboltLaser::getLightSourceLinePowerRange(unsigned int lightSource, unsigned int wavelengthLine, double &minimum, double &maximum) {
    minimum=maximum=0;
    if (lightSource<0 || lightSource>=getLightSourceCount()) return;
    minimum=0;
    //qDebug()<<"POWER_MAX = "<<serial->queryCommand("PMAX_POWER");
    maximum=sources[lightSource].max_power;
}

QString QFExtensionCoboltLaser::getLightSourceLinePowerUnit(unsigned int lightSource, unsigned int wavelengthLine) {
    return tr("mW");
}

void QFExtensionCoboltLaser::setLightSourcePower(unsigned int lightSource, unsigned int wavelengthLine, double power) {
    if (lightSource<0 || lightSource>=getLightSourceCount()) return;
    JKSerialConnection* com=ports.getCOMPort(sources[lightSource].port);
    QFCoboltLaserProtocolHandler* serial=sources[lightSource].serial;
    if (!com || !serial) return ;
    QMutex* mutex=ports.getMutex(sources[lightSource].port);
    QMutexLocker locker(mutex);
    if (sources[lightSource].type==cltMLD6) {
        serial->sendCommand(QString("cp "));
    }

    if (sources[lightSource].line_enabled) {
        serial->sendCommand(QString("p %1").arg(power*sources[lightSource].powerFactor_ParamToSend,0,'f'));
        //qDebug()<<"sending :"<<QString("p %1").arg(power*sources[lightSource].powerFactor_ParamToSend,0,'f')<<"\n";
    } else serial->sendCommand(QString("p 0"));
    sources[lightSource].lastAction=QTime::currentTime();
    sources[lightSource].setPower=power;
}

double QFExtensionCoboltLaser::getLightSourceCurrentSetPower(unsigned int lightSource, unsigned int wavelengthLine) {
    if (lightSource<0 || lightSource>=getLightSourceCount()) return 0;
    return sources[lightSource].setPower;
    /*JKSerialConnection* com=ports.getCOMPort(sources[lightSource].port);
    QFCoboltLaserProtocolHandler* serial=sources[lightSource].serial;
    QMutex* mutex=ports.getMutex(sources[lightSource].port);
    QMutexLocker locker(mutex);
    if (!com || !serial) return 0;
    return serial->queryCommand("p?").toDouble();*/
}

double QFExtensionCoboltLaser::getLightSourceCurrentMeasuredPower(unsigned int lightSource, unsigned int wavelengthLine) {
    if (lightSource<0 || lightSource>=getLightSourceCount()) return 0;
    JKSerialConnection* com=ports.getCOMPort(sources[lightSource].port);
    QFCoboltLaserProtocolHandler* serial=sources[lightSource].serial;
    if (!com || !serial) return 0;
    QMutex* mutex=ports.getMutex(sources[lightSource].port);
    QMutexLocker locker(mutex);
    return serial->queryCommand("pa?").toDouble()*sources[lightSource].powerFactor_ParamRead;

}

void QFExtensionCoboltLaser::setLightSourceLineEnabled(unsigned int lightSource, unsigned int wavelengthLine, bool enabled) {
    if (lightSource<0 || lightSource>=getLightSourceCount()) return ;
    JKSerialConnection* com=ports.getCOMPort(sources[lightSource].port);
    QFCoboltLaserProtocolHandler* serial=sources[lightSource].serial;
    if (!com || !serial) return ;
    QMutex* mutex=ports.getMutex(sources[lightSource].port);
    QMutexLocker locker(mutex);
    if (!enabled) serial->sendCommand("p 0");
    else serial->sendCommand(QString("p %1").arg(sources[lightSource].setPower*sources[lightSource].powerFactor_ParamToSend,0,'f'));
    sources[lightSource].lastAction=QTime::currentTime();
    sources[lightSource].line_enabled=enabled;
}

bool QFExtensionCoboltLaser::getLightSourceLineEnabled(unsigned int lightSource, unsigned int wavelengthLine) {
    if (lightSource<0 || lightSource>=getLightSourceCount()) return false;
    return sources[lightSource].line_enabled;
}

bool QFExtensionCoboltLaser::isLastLightSourceActionFinished(unsigned int lightSource) {
    if (lightSource<0 || lightSource>=getLightSourceCount()) return false;
    JKSerialConnection* com=ports.getCOMPort(sources[lightSource].port);
    QFCoboltLaserProtocolHandler* serial=sources[lightSource].serial;
    if (!com || !serial) return false;
    QMutex* mutex=ports.getMutex(sources[lightSource].port);
    QMutexLocker locker(mutex);
    return sources[lightSource].lastAction.elapsed()>LIGHTSOURCE_ACTION_DURATION;
}

QString QFExtensionCoboltLaser::getLightSourceDescription(unsigned int lightSource) {
    if (lightSource<0 || lightSource>=getLightSourceCount()) return "";

    QString d=tr("Cobolt Laser %1 sn.%2 (maxPower=%3mW, wavelength=%4nm, opHours=%5)")
              .arg(coboltLaserType2HumanReadableString(sources[lightSource].type))
              .arg(sources[lightSource].serialNumber)
              .arg(sources[lightSource].max_power)
              .arg(sources[lightSource].wavelength)
              .arg(sources[lightSource].hours);

    return d;
}

QString QFExtensionCoboltLaser::getLightSourceShortName(unsigned int lightSource) {
    if (lightSource<0 || lightSource>=getLightSourceCount()) return "";
    return sources[lightSource].label;
}

void QFExtensionCoboltLaser::log_text(QString message) {
    if (logService) logService->log_text(LOG_PREFIX+message);
    else if (services) services->log_text(LOG_PREFIX+message);
}

void QFExtensionCoboltLaser::log_warning(QString message) {
    if (logService) logService->log_warning(LOG_PREFIX+message);
    else if (services) services->log_warning(LOG_PREFIX+message);
}

void QFExtensionCoboltLaser::log_error(QString message) {
    if (logService) logService->log_error(LOG_PREFIX+message);
    else if (services) services->log_error(LOG_PREFIX+message);
}

QString QFExtensionCoboltLaser::coboltLaserType2HumanReadableString(QFExtensionCoboltLaser::CoboltLaserTypes type)
{
    switch(type) {
        case cltDPSS4: return tr("DPSS gen.4");
        case cltDPSS5: return tr("DPSS gen.5");
        case cltDPSS6: return tr("DPSS gen.6");
        case cltMLD6: return tr("MLD gen.6");
        default: return tr("generic");
    }
}

QString QFExtensionCoboltLaser::coboltLaserType2String(QFExtensionCoboltLaser::CoboltLaserTypes type)
{
    switch(type) {
        case cltDPSS4: return tr("DPSS4");
        case cltDPSS5: return tr("DPSS5");
        case cltDPSS6: return tr("DPSS6");
        case cltMLD6: return tr("MLD6");
        default: return tr("GENERIC");
    }
}

QFExtensionCoboltLaser::CoboltLaserTypes QFExtensionCoboltLaser::string2CoboltLaserType(const QString &type)
{
    QString t=type.trimmed().toUpper();
    if (t=="DPSS4" || t=="4") {
        return cltDPSS4;
    } else if (t=="DPSS5" || t=="5") {
        return cltDPSS5;
    } else if (t=="DPSS6" || t=="6") {
        return cltDPSS6;
    } else if (t=="MLD6" || t=="MLD" || t=="DIODE6") {
        return cltMLD6;
    }
    return cltGeneric;
}

double QFExtensionCoboltLaser::decodeWavelengthAndLasername(const QString &wl)
{
    QString w=wl.trimmed().toLower();
    bool ok=false;
    double wd=w.toDouble(&ok);
    if (ok) return wd;
    if (w.contains("zouk")) return 355;
    if (w.contains("twist")) return 457;
    if (w.contains("blues")) return 479;
    if (w.contains("calypso")) return 491;
    if (w.contains("fandango")) return 515;
    if (w.contains("samba")) return 532;
    if (w.contains("jive")) return 561;
    if (w.contains("mambo")) return 594;
    if (w.contains("flamenco")) return 660;
    if (w.contains("rumba")) return 1064;
    if (w.contains("355")) return 355;
    if (w.contains("457")) return 457;
    if (w.contains("479")) return 479;
    if (w.contains("491")) return 491;
    if (w.contains("515")) return 515;
    if (w.contains("532")) return 532;
    if (w.contains("561")) return 561;
    if (w.contains("594")) return 594;
    if (w.contains("660")) return 660;
    if (w.contains("1064")) return 1064;
    if (w.contains("405")) return 405;
    if (w.contains("445")) return 445;
    if (w.contains("473")) return 473;
    if (w.contains("488")) return 488;
    if (w.contains("640")) return 640;

    return 0;
}

void QFExtensionCoboltLaser::showLightSourceSettingsDialog(unsigned int lightSource, QWidget *parent) {
    QString ini1=services->getGlobalConfigFileDirectory()+QString("/")+GLOBAL_CONFIGFILE;
    QString ini2=services->getConfigFileDirectory()+QString("/")+GLOBAL_CONFIGFILE;
    QString ini3=services->getAssetsDirectory()+QString("/plugins/")+getID()+QString("/")+GLOBAL_CONFIGFILE;
    QMessageBox::information(parent, getName(), tr("There is no configuration dialog for this device. Set all config in the appropriate ini file:\n  %1\n  or: %2\n  or: %3").arg(ini1).arg(ini2).arg(ini3));
}

unsigned int QFExtensionCoboltLaser::getMeasurementDeviceCount()
{
    return getLightSourceCount();
}

void QFExtensionCoboltLaser::showMeasurementDeviceSettingsDialog(unsigned int measuremenDevice, QWidget *parent)
{
    showLightSourceSettingsDialog(measuremenDevice, parent);
}

bool QFExtensionCoboltLaser::isMeasurementDeviceConnected(unsigned int measuremenDevice)
{
    return isLightSourceConnected(measuremenDevice);
}

void QFExtensionCoboltLaser::connectMeasurementDevice(unsigned int measuremenDevice)
{
    lightSourceConnect(measuremenDevice);
}

void QFExtensionCoboltLaser::disconnectMeasurementDevice(unsigned int measuremenDevice)
{
    lightSourceDisonnect(measuremenDevice);
}

void QFExtensionCoboltLaser::setMeasurementDeviceLogging(QFPluginLogService *logService)
{
    setLightSourceLogging(logService);
}

QString QFExtensionCoboltLaser::getMeasurementDeviceName(unsigned int measuremenDevice)
{
    return getLightSourceShortName(measuremenDevice);
}





unsigned int QFExtensionCoboltLaser::getMeasurementDeviceValueCount(unsigned int measuremenDevice)
{
    if (measuremenDevice<0 || measuremenDevice>=getMeasurementDeviceCount()) return false;
    int cnt=2;
    if (sources[measuremenDevice].type==cltMLD6) {
        cnt += 0;
    }
    return cnt;
}

QString QFExtensionCoboltLaser::getMeasurementDeviceValueName(unsigned int measuremenDevice, unsigned int value)
{
    if (measuremenDevice<0 || measuremenDevice>=getMeasurementDeviceCount()) return false;
    if (value==0) return tr("measured power [mW]");
    if (value==1) return tr("drive current [mA]");
    if (sources[measuremenDevice].type==cltMLD6) {

    }
    return QString();
}

QString QFExtensionCoboltLaser::getMeasurementDeviceValueShortName(unsigned int measuremenDevice, unsigned int value)
{
    if (measuremenDevice<0 || measuremenDevice>=getMeasurementDeviceCount()) return false;
    if (value==0) return tr("measured_power_mW");
    if (value==1) return tr("drive_current_mA");
    if (sources[measuremenDevice].type==cltMLD6) {

    }
    return QString();

}
QVariant QFExtensionCoboltLaser::getMeasurementDeviceValue(unsigned int measuremenDevice, unsigned int value)
{
    if (measuremenDevice<0 || measuremenDevice>=getMeasurementDeviceCount()) return false;
    if (value==0) return getLightSourceCurrentMeasuredPower(measuremenDevice, 0);

    JKSerialConnection* com=ports.getCOMPort(sources[measuremenDevice].port);
    QFCoboltLaserProtocolHandler* serial=sources[measuremenDevice].serial;
    if (!com || !serial) return QVariant();
    QMutex* mutex=ports.getMutex(sources[measuremenDevice].port);
    QMutexLocker locker(mutex);
    if (value==1) return serial->queryCommand("i?").toDouble();
    if (sources[measuremenDevice].type==cltMLD6) {

    }
    return QString();

}


bool QFExtensionCoboltLaser::isMeasurementDeviceValueEditable(unsigned int measuremenDevice, unsigned int value)
{
    return false;
}

void QFExtensionCoboltLaser::setMeasurementDeviceValue(unsigned int measuremenDevice, unsigned int value, const QVariant &data)
{
}

QVariant::Type QFExtensionCoboltLaser::getMeasurementDeviceEditableValueType(unsigned int measuremenDevice, unsigned int value)
{
    return QVariant::String;
}

QFExtensionMeasurementAndControlDevice::WidgetTypes QFExtensionCoboltLaser::getMeasurementDeviceValueWidget(unsigned int measuremenDevice, unsigned int value, QStringList *comboboxEntries)
{
    return QFExtensionMeasurementAndControlDevice::mdDefault;
}

void QFExtensionCoboltLaser::getMeasurementDeviceEditableValueRange(unsigned int measuremenDevice, unsigned int value, double &minimum, double &maximum)
{
}


Q_EXPORT_PLUGIN2(TARGETNAME, QFExtensionCoboltLaser)
