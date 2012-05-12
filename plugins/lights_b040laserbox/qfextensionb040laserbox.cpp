#include "qfextensionb040laserbox.h"
#include <QtGui>
#include <QtPlugin>
#include <iostream>

#define LIGHTSOURCE_ACTION_DURATION 1
#define LOG_PREFIX "[LightSourceB040LaserBox]: "
#define GLOBAL_CONFIGFILE "lights_b040laserbox.ini"
#define CONNECTION_DELAY_MS 200

QFExtensionB040LaserBox::QFExtensionB040LaserBox(QObject* parent):
    QObject(parent)
{
	logService=NULL;
}

QFExtensionB040LaserBox::~QFExtensionB040LaserBox() {

}



void QFExtensionB040LaserBox::projectChanged(QFProject* oldProject, QFProject* project) {
	/* usually cameras do not have to react to a change of the project in QuickFit .. so you don't need to do anything here
	   But: possibly you could read config information from the project here
	 */
}

void QFExtensionB040LaserBox::initExtension() {
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
        s.setPower=0;
        s.lastAction=QTime::currentTime();
        s.serial=new QF3SimpleB040SerialProtocolHandler(ports.getCOMPort(s.port), getName());
        sources.append(s);
    }
    //qDebug()<<sources.size()<<" light sources";
}


void QFExtensionB040LaserBox::loadSettings(ProgramOptions* settingspo) {
	/* here you could read config information from the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini
	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFExtensionB040LaserBox::storeSettings(ProgramOptions* settingspo) {
	/* here you could write config information to the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini

	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFExtensionB040LaserBox::deinit() {
    /* add code for cleanup here */
    QSettings inifile(services->getGlobalConfigFileDirectory()+QString("/%1").arg(GLOBAL_CONFIGFILE), QSettings::IniFormat);
    if (inifile.isWritable()) {
        inifile.setValue("lightsource_count", getLightSourceCount());
        for (unsigned int i=0; i<sources.size(); i++) {
            int p=sources[i].port;
            if (ports.getCOMPort(p)) {
                ports.storeCOMPort(p, inifile, "lightsource"+QString::number(i+1)+"/");
            }
        }
    }
}

unsigned int QFExtensionB040LaserBox::getLightSourceCount() {
    return sources.size();
}

void QFExtensionB040LaserBox::lightSourceConnect(unsigned int lightSource) {
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
        qDebug()<<"infoMessage '"<<sources[lightSource].infoMessage<<"'";
        if (!(sources[lightSource].infoMessage.toLower().contains("laser controller") && sources[lightSource].infoMessage.toLower().contains("jan krieger"))) {
            com->close();
            log_error(tr("%1 Could not connect to Light Source Driver [port=%1  baud=%2]!!!\n").arg(com->get_port().c_str()).arg(com->get_baudrate()));
            log_error(tr("%1 reason: received wrong ID string from light source driver: string was '%2'\n").arg(LOG_PREFIX).arg(sources[lightSource].infoMessage));
        }
    } else {
        log_error(tr("%1 Could not connect to Light Source Driver [port=%1  baud=%2]!!!\n").arg(com->get_port().c_str()).arg(com->get_baudrate()));
        log_error(tr("%1 reason: %2\n").arg(LOG_PREFIX).arg(sources[lightSource].serial->getLastError()));
    }
}

void QFExtensionB040LaserBox::lightSourceDisonnect(unsigned int lightSource) {
    if (lightSource<0 || lightSource>=getLightSourceCount()) return;
    JKSerialConnection* com=ports.getCOMPort(sources[lightSource].port);
    if (!com) return;

    QMutex* mutex=ports.getMutex(sources[lightSource].port);
    QMutexLocker locker(mutex);
    com->close();
}

void QFExtensionB040LaserBox::setLightSourceLogging(QFPluginLogService *logService) {
    this->logService=logService;
}

bool QFExtensionB040LaserBox::isLightSourceConnected(unsigned int lightSource) {
    if (lightSource<0 || lightSource>=getLightSourceCount()) return false;
    JKSerialConnection* com=ports.getCOMPort(sources[lightSource].port);
    if (!com) return false;
    QMutex* mutex=ports.getMutex(sources[lightSource].port);
    QMutexLocker locker(mutex);
    return com->isConnectionOpen();
}

unsigned int QFExtensionB040LaserBox::getLightSourceLineCount(unsigned int lightSource) {
    if (lightSource<0 || lightSource>=getLightSourceCount()) return 0;
    JKSerialConnection* com=ports.getCOMPort(sources[lightSource].port);
    if (!com) return 0;
    return 1;
}

QString QFExtensionB040LaserBox::getLightSourceLineDescription(unsigned int lightSource, unsigned int wavelengthLine) {
    if (lightSource<0 || lightSource>=getLightSourceCount()) return "";
    JKSerialConnection* com=ports.getCOMPort(sources[lightSource].port);
    QF3SimpleB040SerialProtocolHandler* serial=sources[lightSource].serial;
    if (!com || !serial) return QString("");
    QMutex* mutex=ports.getMutex(sources[lightSource].port);
    QMutexLocker locker(mutex);
    return tr("line #%2: %1 nm").arg(serial->queryCommand("w").toDouble()).arg(wavelengthLine+1);
    //return tr("line #%1").arg(wavelengthLine+1);
}

void QFExtensionB040LaserBox::getLightSourceLinePowerRange(unsigned int lightSource, unsigned int wavelengthLine, double &minimum, double &maximum) {
    minimum=maximum=0;
    if (lightSource<0 || lightSource>=getLightSourceCount()) return;
    JKSerialConnection* com=ports.getCOMPort(sources[lightSource].port);
    QF3SimpleB040SerialProtocolHandler* serial=sources[lightSource].serial;
    if (!com || !serial) return ;
    QMutex* mutex=ports.getMutex(sources[lightSource].port);
    QMutexLocker locker(mutex);
    minimum=0;
    //qDebug()<<"POWER_MAX = "<<serial->queryCommand("PMAX_POWER");
    maximum=serial->queryCommand("PMAX_POWER").toDouble();
}

QString QFExtensionB040LaserBox::getLightSourceLinePowerUnit(unsigned int lightSource, unsigned int wavelengthLine) {
    return tr("mW");
}

void QFExtensionB040LaserBox::setLightSourcePower(unsigned int lightSource, unsigned int wavelengthLine, double power) {
    if (lightSource<0 || lightSource>=getLightSourceCount()) return;
    JKSerialConnection* com=ports.getCOMPort(sources[lightSource].port);
    QF3SimpleB040SerialProtocolHandler* serial=sources[lightSource].serial;
    if (!com || !serial) return ;
    QMutex* mutex=ports.getMutex(sources[lightSource].port);
    QMutexLocker locker(mutex);
    serial->sendCommand(QString("p%1").arg(power,0,'f'));
    sources[lightSource].lastAction=QTime::currentTime();
}

double QFExtensionB040LaserBox::getLightSourceCurrentSetPower(unsigned int lightSource, unsigned int wavelengthLine) {
    if (lightSource<0 || lightSource>=getLightSourceCount()) return 0;
    JKSerialConnection* com=ports.getCOMPort(sources[lightSource].port);
    QF3SimpleB040SerialProtocolHandler* serial=sources[lightSource].serial;
    QMutex* mutex=ports.getMutex(sources[lightSource].port);
    QMutexLocker locker(mutex);
    if (!com || !serial) return 0;
    return serial->queryCommand("g").toDouble();
}

double QFExtensionB040LaserBox::getLightSourceCurrentMeasuredPower(unsigned int lightSource, unsigned int wavelengthLine) {
    if (lightSource<0 || lightSource>=getLightSourceCount()) return 0;
    JKSerialConnection* com=ports.getCOMPort(sources[lightSource].port);
    QF3SimpleB040SerialProtocolHandler* serial=sources[lightSource].serial;
    if (!com || !serial) return 0;
    QMutex* mutex=ports.getMutex(sources[lightSource].port);
    QMutexLocker locker(mutex);
    return serial->queryCommand("r").toDouble();

}

void QFExtensionB040LaserBox::setLightSourceLineEnabled(unsigned int lightSource, unsigned int wavelengthLine, bool enabled) {
    if (lightSource<0 || lightSource>=getLightSourceCount()) return ;
    JKSerialConnection* com=ports.getCOMPort(sources[lightSource].port);
    QF3SimpleB040SerialProtocolHandler* serial=sources[lightSource].serial;
    if (!com || !serial) return ;
    QMutex* mutex=ports.getMutex(sources[lightSource].port);
    QMutexLocker locker(mutex);
    if (enabled) serial->sendCommand("O1");
    else serial->sendCommand("O0");
    sources[lightSource].lastAction=QTime::currentTime();
}

bool QFExtensionB040LaserBox::getLightSourceLineEnabled(unsigned int lightSource, unsigned int wavelengthLine) {
    if (lightSource<0 || lightSource>=getLightSourceCount()) return false;
    JKSerialConnection* com=ports.getCOMPort(sources[lightSource].port);
    QF3SimpleB040SerialProtocolHandler* serial=sources[lightSource].serial;
    if (!com || !serial) return false;
    QMutex* mutex=ports.getMutex(sources[lightSource].port);
    QMutexLocker locker(mutex);
    return !serial->queryCommand("o").startsWith("0");
}

bool QFExtensionB040LaserBox::isLastLightSourceActionFinished(unsigned int lightSource) {
    if (lightSource<0 || lightSource>=getLightSourceCount()) return false;
    JKSerialConnection* com=ports.getCOMPort(sources[lightSource].port);
    QF3SimpleB040SerialProtocolHandler* serial=sources[lightSource].serial;
    if (!com || !serial) return false;
    QMutex* mutex=ports.getMutex(sources[lightSource].port);
    QMutexLocker locker(mutex);
    return sources[lightSource].lastAction.elapsed()>LIGHTSOURCE_ACTION_DURATION;
}

QString QFExtensionB040LaserBox::getLightSourceDescription(unsigned int lightSource) {
    if (lightSource<0 || lightSource>=getLightSourceCount()) return "";
    JKSerialConnection* com=ports.getCOMPort(sources[lightSource].port);
    QF3SimpleB040SerialProtocolHandler* serial=sources[lightSource].serial;
    if (!com || !serial) return QString("");
    return tr("B040 Laser Remote Control");
}

QString QFExtensionB040LaserBox::getLightSourceShortName(unsigned int lightSource) {
    if (lightSource<0 || lightSource>=getLightSourceCount()) return "";
    JKSerialConnection* com=ports.getCOMPort(sources[lightSource].port);
    QF3SimpleB040SerialProtocolHandler* serial=sources[lightSource].serial;
    if (!com || !serial) return QString("");
    return tr("Laser Box");
}

void QFExtensionB040LaserBox::log_text(QString message) {
    if (logService) logService->log_text(LOG_PREFIX+message);
    else if (services) services->log_text(LOG_PREFIX+message);
}

void QFExtensionB040LaserBox::log_warning(QString message) {
    if (logService) logService->log_warning(LOG_PREFIX+message);
    else if (services) services->log_warning(LOG_PREFIX+message);
}

void QFExtensionB040LaserBox::log_error(QString message) {
    if (logService) logService->log_error(LOG_PREFIX+message);
    else if (services) services->log_error(LOG_PREFIX+message);
}

void QFExtensionB040LaserBox::showLightSourceSettingsDialog(unsigned int lightSource, QWidget *parent) {
    QString ini1=services->getGlobalConfigFileDirectory()+QString("/")+GLOBAL_CONFIGFILE;
    QString ini2=services->getConfigFileDirectory()+QString("/")+GLOBAL_CONFIGFILE;
    QString ini3=services->getAssetsDirectory()+QString("/plugins/")+getID()+QString("/")+GLOBAL_CONFIGFILE;
    QMessageBox::information(parent, getName(), tr("There is no configuration dialog for this device. Set all config in the appropriate ini file:\n  %1\n  or: %2\n  or: %3").arg(ini1).arg(ini2).arg(ini3));
}


Q_EXPORT_PLUGIN2(lights_b040laserbox, QFExtensionB040LaserBox)
