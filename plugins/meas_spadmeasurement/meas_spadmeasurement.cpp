#include "meas_spadmeasurement.h"
#include <QtGui>
#include <QtPlugin>
#include <iostream>

#define LOG_PREFIX "[SPADMeasurement]: "
#define CONNECTION_DELAY_MS 200

QFExtensionB040SPADMeasurement::QFExtensionB040SPADMeasurement(QObject* parent):
    QObject(parent)
{
    //serial=new QF3SimpleB040SerialProtocolHandler(&com, getName());
	logService=NULL;

}

QFExtensionB040SPADMeasurement::~QFExtensionB040SPADMeasurement() {
    devices.clear();
}


void QFExtensionB040SPADMeasurement::deinit() {
	/* add code for cleanup here */
    QSettings inifile(services->getGlobalConfigFileDirectory()+"/meas_spadmeasurement.ini", QSettings::IniFormat);
    if (inifile.isWritable()) {
        inifile.setValue("device_count", getMeasurementDeviceCount());
        for (unsigned int i=0; i<getMeasurementDeviceCount(); i++) {
            int p=devices[i].port;
            if (ports.getCOMPort(p)) {
                ports.storeCOMPort(p, inifile, "device"+QString::number(i+1)+"/");
            }
        }
    }
}

void QFExtensionB040SPADMeasurement::projectChanged(QFProject* oldProject, QFProject* project) {

}

void QFExtensionB040SPADMeasurement::initExtension() {
    /* do initializations here but do not yet connect to the camera! */
    QString ini=services->getGlobalConfigFileDirectory()+QString("/meas_spadmeasurement.ini");
    if (!QFile::exists(ini)) ini=services->getConfigFileDirectory()+QString("/meas_spadmeasurement.ini");
    if (!QFile::exists(ini)) ini=services->getAssetsDirectory()+QString("/plugins/")+getID()+QString("/meas_spadmeasurement.ini");
    QSettings inifile(ini, QSettings::IniFormat);
    int device_count=inifile.value("device_count", 0).toUInt();
    for (int i=0; i<device_count; i++) {
        DEVICEINFO s;
        s.port=ports.addCOMPort(inifile, "device"+QString::number(i+1)+"/", 115200);
        s.infoMessage="";
        s.lastAction=QTime::currentTime();
        s.serial=new QF3SimpleB040SerialProtocolHandler(ports.getCOMPort(s.port), getName());
        //s.serial->setAddToCommand("");
        s.label=inifile.value("device"+QString::number(i+1)+"/label", tr("B040 SPAD surveilance #%1").arg(i+1)).toString();
        s.autoconnect=inifile.value("device"+QString::number(i+1)+"/autoconnect", false).toBool();
        for (int j=0; j<3; j++) {
            s.voltage_factor[j]=inifile.value("device"+QString::number(i+1)+"/voltage_factor", 1.0).toDouble();
            s.voltage_offset[j]=inifile.value("device"+QString::number(i+1)+"/voltage_offset", 0.0).toDouble();
            s.current_factor[j]=inifile.value("device"+QString::number(i+1)+"/current_factor", 1.0).toDouble();
            s.current_offset[j]=inifile.value("device"+QString::number(i+1)+"/current_offset", 0.0).toDouble();

            s.valuelabels[j]=inifile.value("device"+QString::number(i+1)+"/voltage"+QString::number(j)+"_label", tr("voltage %1 [V]").arg(j)).toString();
            s.valueids[j]=inifile.value("device"+QString::number(i+1)+"/voltage"+QString::number(j)+"_id", QString("VOLTAGE%1").arg(j)).toString();
            s.valuelabels[3+j]=inifile.value("device"+QString::number(i+1)+"/current"+QString::number(j)+"_label", tr("current %1 [mA]").arg(j)).toString();
            s.valueids[3+j]=inifile.value("device"+QString::number(i+1)+"/current"+QString::number(j)+"_id", QString("CURRENT%1").arg(j)).toString();
        }
        s.valuelabels[6]=inifile.value("device"+QString::number(i+1)+"/temperature0_label", tr("temperature 0")).toString();
        s.valuelabels[7]=inifile.value("device"+QString::number(i+1)+"/temperature1_label", tr("temperature 1")).toString();
        s.valueids[6]=inifile.value("device"+QString::number(i+1)+"/temperature0_id", tr("TEMP0")).toString();
        s.valueids[7]=inifile.value("device"+QString::number(i+1)+"/temperature1_id", tr("TEMP1")).toString();
        devices.append(s);
        if (s.autoconnect) connectMeasurementDevice(devices.size()-1);
    }
}

void QFExtensionB040SPADMeasurement::loadSettings(ProgramOptions* settingspo) {
	/* here you could read config information from the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    //QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini

	// ALTERNATIVE: read/write Information to/from plugins/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFExtensionB040SPADMeasurement::storeSettings(ProgramOptions* settingspo) {
	/* here you could write config information to the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    //QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini

	// ALTERNATIVE: read/write Information to/from plugins/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}









void QFExtensionB040SPADMeasurement::setLogging(QFPluginLogService* logService) {
    this->logService=logService;
}

unsigned int QFExtensionB040SPADMeasurement::getMeasurementDeviceCount()
{
    return devices.size();

}

QString QFExtensionB040SPADMeasurement::getMeasurementDeviceName(unsigned int measuremenDevice)
{
    if (measuremenDevice>=0 && measuremenDevice<devices.size()) {
        return devices[measuremenDevice].label;
    }
    return QString();
}

void QFExtensionB040SPADMeasurement::showMeasurementDeviceSettingsDialog(unsigned int measuremenDevice, QWidget *parent)
{
    QString ini1=services->getGlobalConfigFileDirectory()+QString("/meas_spadmeasurement.ini");
    QString ini2=services->getConfigFileDirectory()+QString("/meas_spadmeasurement.ini");
    QString ini3=services->getAssetsDirectory()+QString("/plugins/")+getID()+QString("/meas_spadmeasurement.ini");
    QMessageBox::information(parent, getName(), tr("There is no configuration dialog for this device. Set all config in the appropriate ini file:\n  %1\n  or: %2\n  or: %3").arg(ini1).arg(ini2).arg(ini3));

}

bool QFExtensionB040SPADMeasurement::isMeasurementDeviceConnected(unsigned int measuremenDevice)
{
    if (measuremenDevice<0 || measuremenDevice>=getMeasurementDeviceCount()) return false;
    JKSerialConnection* com=ports.getCOMPort(devices[measuremenDevice].port);
    if (!com) return false;
    QMutex* mutex=ports.getMutex(devices[measuremenDevice].port);
    QMutexLocker locker(mutex);
    return com->isConnectionOpen();
}

void QFExtensionB040SPADMeasurement::connectMeasurementDevice(unsigned int measuremenDevice)
{
    if (measuremenDevice<0 || measuremenDevice>=getMeasurementDeviceCount()) return;
    JKSerialConnection* com=ports.getCOMPort(devices[measuremenDevice].port);
    if (!com) return;
    QMutex* mutex=ports.getMutex(devices[measuremenDevice].port);
    QMutexLocker locker(mutex);
    com->open();
    if (com->isConnectionOpen()) {
        QTime t;
        t.start();
        // wait CONNECTION_DELAY_MS ms for connection!
        while (t.elapsed()<CONNECTION_DELAY_MS)  {
            QApplication::processEvents();
        }
        devices[measuremenDevice].infoMessage=devices[measuremenDevice].serial->queryCommand("?");
        //qDebug()<<"infoMessage '"<<infoMessage<<"'";
        if (!(devices[measuremenDevice].infoMessage.toLower().contains("spadarray controller") && devices[measuremenDevice].infoMessage.toLower().contains("jan krieger"))) {
            com->close();
            log_error(tr("%1 Could not connect to SPADArray Controller [port=%1  baud=%2]!!!\n").arg(com->get_port().c_str()).arg(com->get_baudrate()));
            log_error(tr("%1 reason: received wrong ID string from SPADArray Controller: string was '%2'\n").arg(LOG_PREFIX).arg(devices[measuremenDevice].infoMessage));
        } else {
            log_text(tr("%1 Connected to SPADArray Controller [port=%2  baud=%3] \n%1 welcome message:'%4'\n").arg(LOG_PREFIX).arg(com->get_port().c_str()).arg(com->get_baudrate()).arg(devices[measuremenDevice].infoMessage));
        }
    } else {
        log_error(tr("%1 Could not connect to SPADArray Controller [port=%1  baud=%2]!!!\n").arg(com->get_port().c_str()).arg(com->get_baudrate()));
        log_error(tr("%1 reason: %2\n").arg(LOG_PREFIX).arg(devices[measuremenDevice].serial->getLastError()));
    }
}

void QFExtensionB040SPADMeasurement::disconnectMeasurementDevice(unsigned int measuremenDevice)
{
    if (measuremenDevice<0 || measuremenDevice>=getMeasurementDeviceCount()) return;
    JKSerialConnection* com=ports.getCOMPort(devices[measuremenDevice].port);
    if (!com) return;
    QMutex* mutex=ports.getMutex(devices[measuremenDevice].port);
    QMutexLocker locker(mutex);
    com->close();

}








void QFExtensionB040SPADMeasurement::setMeasurementDeviceLogging(QFPluginLogService *logService)
{
    this->logService=logService;
}


QVariant QFExtensionB040SPADMeasurement::getMeasurementDeviceValue(unsigned int measuremenDevice, unsigned int value)
{
    if (measuremenDevice<0 || measuremenDevice>=getMeasurementDeviceCount()) return QVariant();
    JKSerialConnection* com=ports.getCOMPort(devices[measuremenDevice].port);
    QF3SimpleB040SerialProtocolHandler* serial=devices[measuremenDevice].serial;
    if (!com) return QVariant();
    QMutex* mutex=ports.getMutex(devices[measuremenDevice].port);
    QMutexLocker locker(mutex);
    if (!com->isConnectionOpen()) return QVariant();


    bool ok=false;
    if (value>=0 && value<3) {
        QString rs=serial->queryCommand(QString("U%1").arg(value));
        double r=(rs.toDouble(&ok)/100.0-devices[measuremenDevice].voltage_offset[value])*devices[measuremenDevice].voltage_factor[value];
        if (ok&&r<1000) return r;
    } else if (value>=3 && value<6) {
        QString rs=serial->queryCommand(QString("I%1").arg(value-3));
        double r=(rs.toDouble(&ok)-devices[measuremenDevice].current_offset[value-3])*devices[measuremenDevice].current_factor[value-3];
        if (ok&&r<1000) return r;
    } else if (value==6 || value==7) {
        QString rs=serial->queryCommand(QString("T%1").arg(value-6));
        double r=rs.toDouble(&ok)/10.0;
        if (ok&&r<1000) return r;
    } else if (value>=8 && value<12) {
        QString rs=serial->queryCommand(QString("Q%1").arg(value-8));
        bool r=(rs.toInt(&ok)!=0);
        return r;
    } else if (value==12) {
        QString rs=serial->queryCommand(QString("B"));
        bool r=(rs.toInt(&ok)!=0);
        return r;
    }

    return QVariant();
}

void QFExtensionB040SPADMeasurement::setMeasurementDeviceValue(unsigned int measuremenDevice, unsigned int value, const QVariant &data)
{
    if (measuremenDevice<0 || measuremenDevice>=getMeasurementDeviceCount()) return ;
    JKSerialConnection* com=ports.getCOMPort(devices[measuremenDevice].port);
    QF3SimpleB040SerialProtocolHandler* serial=devices[measuremenDevice].serial;
    if (!com) return ;
    QMutex* mutex=ports.getMutex(devices[measuremenDevice].port);
    QMutexLocker locker(mutex);
    if (!com->isConnectionOpen()) return ;

    if (value==12) {
        if (data.toBool()) serial->sendCommand(QString("A1"));
        else serial->sendCommand(QString("A0"));
    }
    return ;
}

unsigned int QFExtensionB040SPADMeasurement::getMeasurementDeviceValueCount(unsigned int measuremenDevice)
{
    return 13;
}

QString QFExtensionB040SPADMeasurement::getMeasurementDeviceValueName(unsigned int measuremenDevice, unsigned int value)
{
    if (measuremenDevice>=0 && measuremenDevice<devices.size()) {
        if (value>=0 && value<8) return devices[measuremenDevice].valuelabels.value(value, QString("PARAMETER%1").arg(value));
        switch (value) {
            case 8: return tr("relay 1");
            case 9: return tr("relay 2");
            case 10: return tr("relay 3");
            case 11: return tr("relay 4");
            case 12: return tr("all relais");
            default: return "";
        }
    }
    return "";
}

QString QFExtensionB040SPADMeasurement::getMeasurementDeviceValueShortName(unsigned int measuremenDevice, unsigned int value)
{
    if (measuremenDevice>=0 && measuremenDevice<devices.size()) {
        if (value>=0 && value<8) return devices[measuremenDevice].valueids.value(value, QString("PARAMETER%1").arg(value));
        switch (value) {
            case 8: return tr("RELAY1");
            case 9: return tr("RELAY2");
            case 10: return tr("RELAY3");
            case 11: return tr("RELAY4");
            case 12: return tr("ALL_RELAIS");
            default: return "";
        }
    }
    return "";
}

bool QFExtensionB040SPADMeasurement::isMeasurementDeviceValueEditable(unsigned int measuremenDevice, unsigned int value)
{
    return value==12;
}

QVariant::Type QFExtensionB040SPADMeasurement::getMeasurementDeviceEditableValueType(unsigned int measuremenDevice, unsigned int value)
{
    if (value>=0 && value<8) return QVariant::Double;
    if (value>=8 && value<13) return QVariant::Bool;

    return QVariant::Invalid;

}

QFExtensionMeasurementAndControlDevice::WidgetTypes QFExtensionB040SPADMeasurement::getMeasurementDeviceValueWidget(unsigned int measuremenDevice, unsigned int value, QStringList *comboboxEntries)
{
    if (value>=0 && value<8) return QFExtensionMeasurementAndControlDevice::mdLabel;
    if (value>=8 && value<13) return QFExtensionMeasurementAndControlDevice::mdCheckBox;

    return QFExtensionMeasurementAndControlDevice::mdDefault;

}

void QFExtensionB040SPADMeasurement::getMeasurementDeviceEditableValueRange(unsigned int measuremenDevice, unsigned int value, double &minimum, double &maximum)
{

}

void QFExtensionB040SPADMeasurement::log_text(QString message) {
	if (logService) logService->log_text(message);
	else if (services) services->log_text(message);
}

void QFExtensionB040SPADMeasurement::log_warning(QString message) {
	if (logService) logService->log_warning(message);
	else if (services) services->log_warning(message);
}

void QFExtensionB040SPADMeasurement::log_error(QString message) {
	if (logService) logService->log_error(message);
	else if (services) services->log_error(message);
}



Q_EXPORT_PLUGIN2(meas_spadmeasurement, QFExtensionB040SPADMeasurement)
