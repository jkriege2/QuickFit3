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

#include "servo_pololu_maestro.h"
#include <QtGui>
#include <QtPlugin>
#include <iostream>
#include "dlgpololumaestrotester.h"

#define LOG_PREFIX "[PololuMaestro]: "
#define CONNECTION_DELAY_MS 50

QFExtensionServoPololuMaestro::QFExtensionServoPololuMaestro(QObject* parent):
    QObject(parent)
{
    //serial=new QF3SimpleB040SerialProtocolHandler(&com, getName());
	logService=NULL;

}

QFExtensionServoPololuMaestro::~QFExtensionServoPololuMaestro() {
    shutters.clear();
}


void QFExtensionServoPololuMaestro::deinit() {
	/* add code for cleanup here */
    QSettings inifile(services->getGlobalConfigFileDirectory()+"/servo_pololu_maestro.ini", QSettings::IniFormat);
    if (inifile.isWritable()) {
        inifile.setValue("device_count", getShutterCount());
        for (int i=0; i<deviceIndex.size(); i++) {
            if (deviceIndex[i].first==0) {
                int p=shutters[deviceIndex[i].second].port;
                if (ports.getCOMPort(p)) {
                    ports.storeCOMPort(p, inifile, "device"+QString::number(i+1)+"/");
                }
                inifile.setValue("device"+QString::number(i+1)+"/type", "shutter");
                inifile.setValue("device"+QString::number(i+1)+"/operation_duration", shutters[deviceIndex[i].second].operation_duration);
                inifile.setValue("device"+QString::number(i+1)+"/id", shutters[deviceIndex[i].second].id);
                inifile.setValue("device"+QString::number(i+1)+"/shutter_on", shutters[deviceIndex[i].second].on_target);
                inifile.setValue("device"+QString::number(i+1)+"/shutter_off", shutters[deviceIndex[i].second].off_target);
                inifile.setValue("device"+QString::number(i+1)+"/label", shutters[deviceIndex[i].second].label);
                inifile.setValue("device"+QString::number(i+1)+"/description", shutters[deviceIndex[i].second].description);
            }
        }
    }
}

void QFExtensionServoPololuMaestro::projectChanged(QFProject* oldProject, QFProject* project) {

}

void QFExtensionServoPololuMaestro::initExtension() {
    /* do initializations here but do not yet connect to the camera! */
    QString ini=services->getGlobalConfigFileDirectory()+QString("/servo_pololu_maestro.ini");
    if (!QFile::exists(ini)) ini=services->getConfigFileDirectory()+QString("/servo_pololu_maestro.ini");
    if (!QFile::exists(ini)) ini=services->getAssetsDirectory()+QString("/plugins/")+getID()+QString("/servo_pololu_maestro.ini");
    QSettings inifile(ini, QSettings::IniFormat);
    int shutter_count=inifile.value("device_count", 0).toUInt();
    deviceIndex.clear();
    for (int i=0; i<shutter_count; i++) {
        QString type=inifile.value("device"+QString::number(i+1)+"/type", i+1).toString().trimmed().toLower();
        if (type=="shutter") {
            deviceIndex[i]=qMakePair(0, shutters.size());
            SHUTTER s;
            s.port=ports.addCOMPort(inifile, "device"+QString::number(i+1)+"/");
            s.id=inifile.value("device"+QString::number(i+1)+"/id", i+1).toInt();
            s.infoMessage="";
            s.lastAction=QTime::currentTime();
            s.serial=new QFBinarySerialProtocolHandler(ports.getCOMPort(s.port), getName());
            s.operation_duration=inifile.value("device"+QString::number(i+1)+"/operation_duration", 500).toInt();
            s.label=inifile.value("device"+QString::number(i+1)+"/label", tr("%1 #%2").arg(getName()).arg(i+1)).toString();
            s.description=inifile.value("device"+QString::number(i+1)+"/description", tr("%1 #%2").arg(getDescription())).toString();
            s.currentState=false;
            s.on_target=inifile.value("device"+QString::number(i+1)+"/shutter_on", 1500).toInt();
            s.off_target=inifile.value("device"+QString::number(i+1)+"/shutter_off", 1000).toInt();
            shutters.append(s);
        } else if (type=="wheel" || type=="changer" || type=="filterwheel" || type=="filterchanger") {
            deviceIndex[i]=qMakePair(1, wheels.size());
            FILTERWHEEL s;
            s.port=ports.addCOMPort(inifile, "device"+QString::number(i+1)+"/");
            s.id=inifile.value("device"+QString::number(i+1)+"/id", i+1).toInt();
            s.infoMessage="";
            s.lastAction=QTime::currentTime();
            s.serial=new QFBinarySerialProtocolHandler(ports.getCOMPort(s.port), getName());
            s.operation_duration=inifile.value("device"+QString::number(i+1)+"/operation_duration", 500).toInt();
            s.label=inifile.value("device"+QString::number(i+1)+"/label", tr("%1 #%2").arg(getName()).arg(i+1)).toString();
            s.description=inifile.value("device"+QString::number(i+1)+"/description", tr("%1 #%2").arg(getDescription())).toString();
            s.currentFilter=0;
            int pos=inifile.value("device"+QString::number(i+1)+"/filterchanger_positions", 2).toInt();
            for (int j=0; j<pos; j++) {
                s.targets.append(inifile.value("device"+QString::number(i+1)+"/filterchanger_position"+QString::number(j+1), 1000+j*(2000-1000)/(pos-1)).toInt());
            }
            wheels.append(s);
        }
    }

    QAction* action=new QAction(QIcon(getIconFilename()), tr("Test Pololu Maestro"), this);
    connect(action, SIGNAL(triggered()), this, SLOT(showTester()));
    if (services) {
        QMenu* m=services->getMenu("extensions");
        if (m) {
            m->addAction(action);
        }
    }
}

void QFExtensionServoPololuMaestro::loadSettings(ProgramOptions* settingspo) {
	/* here you could read config information from the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    //QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini

	// ALTERNATIVE: read/write Information to/from plugins/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFExtensionServoPololuMaestro::storeSettings(ProgramOptions* settingspo) {
	/* here you could write config information to the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    //QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini

	// ALTERNATIVE: read/write Information to/from plugins/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

unsigned int QFExtensionServoPololuMaestro::getShutterCount() {
    return shutters.size();
}

void QFExtensionServoPololuMaestro::shutterConnect(unsigned int shutter) {
    if (shutter<0 || shutter>=getShutterCount()) return;
    JKSerialConnection* com=ports.getCOMPort(shutters[shutter].port);
    if (!com) return;
    QMutex* mutex=ports.getMutex(shutters[shutter].port);
    QMutexLocker locker(mutex);
    com->open();
    if (com->isConnectionOpen()) {
        QTime t;
        t.start();
        // wait CONNECTION_DELAY_MS ms for connection!
        while (t.elapsed()<CONNECTION_DELAY_MS)  {
            QApplication::processEvents();
        }
    } else {
        log_error(tr("%1 Could not connect to Servo Shutter Driver [port=%1  baud=%2]!!!\n").arg(com->get_port().c_str()).arg(com->get_baudrate()));
        log_error(tr("%1 reason: %2\n").arg(LOG_PREFIX).arg(shutters[shutter].serial->getLastError()));
    }
}

void QFExtensionServoPololuMaestro::shutterDisonnect(unsigned int shutter) {
    if (shutter<0 || shutter>=getShutterCount()) return;
    JKSerialConnection* com=ports.getCOMPort(shutters[shutter].port);
    if (!com) return;
    QMutex* mutex=ports.getMutex(shutters[shutter].port);
    QMutexLocker locker(mutex);
    com->close();
}

bool QFExtensionServoPololuMaestro::isShutterConnected(unsigned int shutter) {
    if (shutter<0 || shutter>=getShutterCount()) return false;
    JKSerialConnection* com=ports.getCOMPort(shutters[shutter].port);
    if (!com) return false;
    QMutex* mutex=ports.getMutex(shutters[shutter].port);
    QMutexLocker locker(mutex);
    return com->isConnectionOpen();
}

bool QFExtensionServoPololuMaestro::isShutterOpen(unsigned int shutter)  {
    if (shutter<0 || shutter>=getShutterCount()) return false;
    JKSerialConnection* com=ports.getCOMPort(shutters[shutter].port);
    if (!com) return false;
    QMutex* mutex=ports.getMutex(shutters[shutter].port);
    QMutexLocker locker(mutex);
    if (!com->isConnectionOpen()) return false;

    QByteArray result=shutters[shutter].serial->queryCommand(pololuMaestroBuildGetPosCommand(shutters[shutter].id), 2);
    int pos=result[0]+0xFF*result[1];

    return pos>=shutters[shutter].on_target*4;
}

void QFExtensionServoPololuMaestro::setShutterState(unsigned int shutter, bool opened) {
    if (shutter<0 || shutter>=getShutterCount()) return;
    JKSerialConnection* com=ports.getCOMPort(shutters[shutter].port);
    if (!com) return ;
    QMutex* mutex=ports.getMutex(shutters[shutter].port);
    QMutexLocker locker(mutex);
    if (!com->isConnectionOpen()) return ;

    int target=shutters[shutter].off_target;
    if (opened) target=shutters[shutter].on_target;

    shutters[shutter].serial->sendCommand(pololuMaestroBuildSetPosCommand(shutters[shutter].id, target));
    shutters[shutter].lastAction=QTime::currentTime();

    /*QTime t=QTime::currentTime();
    QTime t2;
    t.start();
    while (t.elapsed()<20) {
        t2.start();
    }*/
}

bool QFExtensionServoPololuMaestro::isLastShutterActionFinished(unsigned int shutter) {
    if (shutter<0 || shutter>=getShutterCount()) return true;
    return shutters[shutter].lastAction.elapsed()>shutters[shutter].operation_duration;
}

QString QFExtensionServoPololuMaestro::getShutterDescription(unsigned int shutter)  {
    if (shutter<0 || shutter>=getShutterCount()) return getDescription();
    return shutters[shutter].description;
}

QString QFExtensionServoPololuMaestro::getShutterShortName(unsigned int shutter)  {
    if (shutter<0 || shutter>=getShutterCount()) return getName();
    return shutters[shutter].label;
}


void QFExtensionServoPololuMaestro::showShutterSettingsDialog(unsigned int shutter, QWidget* parent) {
/*
    bool globalIniWritable=QSettings(services->getGlobalConfigFileDirectory()+"/servo_pololu_maestro.ini", QSettings::IniFormat).isWritable();


    if (globalIniWritable) {
        /////////////////////////////////////////////////////////////////////////////////
        // if you want the settings dialog to be modal, you may uncomment the next lines
        // and add implementations
        /////////////////////////////////////////////////////////////////////////////////

        QDialog* dlg=new QDialog(parent);

        QVBoxLayout* lay=new QVBoxLayout();
        dlg->setLayout(lay);

        QFormLayout* formlayout=new QFormLayout();


        formlayout->addRow("", new QLabel(tr("<b>All settings marked with * will be<br>used when connecting the next time!</b>"), dlg));

        QComboBox* cmbPort=new QComboBox(dlg);
        std::vector<std::string> ports=JKSerialConnection::listPorts();
        for (unsigned int i=0; i<ports.size(); i++) {
            cmbPort->addItem(ports[i].c_str());
        }
        cmbPort->setEditable(false);
        cmbPort->setCurrentIndex(cmbPort->findText(COMPort));
        formlayout->addRow(tr("serial &port*:"), cmbPort);

        lay->addLayout(formlayout);

        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, dlg);
        lay->addWidget(buttonBox);

        connect(buttonBox, SIGNAL(accepted()), dlg, SLOT(accept()));
        connect(buttonBox, SIGNAL(rejected()), dlg, SLOT(reject()));

        if ( dlg->exec()==QDialog::Accepted ) {
             //  read back values entered into the widgets and store in settings
             COMPort=cmbPort->currentText();
        }
        delete dlg;

    } else {
        QMessageBox::information(parent, getName(), tr("Your user account is not allowd to write to the global config directory!"));
    }*/
    QString ini1=services->getGlobalConfigFileDirectory()+QString("/servo_pololu_maestro.ini");
    QString ini2=services->getConfigFileDirectory()+QString("/servo_pololu_maestro.ini");
    QString ini3=services->getAssetsDirectory()+QString("/plugins/")+getID()+QString("/servo_pololu_maestro.ini");
    QMessageBox::information(parent, getName(), tr("There is no configuration dialog for this device. Set all config in the appropriate ini file:\n  %1\n  or: %2\n  or: %3").arg(ini1).arg(ini2).arg(ini3));
}


void QFExtensionServoPololuMaestro::setLogging(QFPluginLogService* logService) {
	this->logService=logService;
}

void QFExtensionServoPololuMaestro::log_text(QString message) {
	if (logService) logService->log_text(message);
	else if (services) services->log_text(message);
}

void QFExtensionServoPololuMaestro::log_warning(QString message) {
	if (logService) logService->log_warning(message);
	else if (services) services->log_warning(message);
}

void QFExtensionServoPololuMaestro::log_error(QString message) {
	if (logService) logService->log_error(message);
	else if (services) services->log_error(message);
}

void QFExtensionServoPololuMaestro::setShutterLogging(QFPluginLogService* logService) {
    this->logService=logService;
}

unsigned int QFExtensionServoPololuMaestro::getFilterChangerCount()
{
    return wheels.size();
}

void QFExtensionServoPololuMaestro::filterChangerConnect(unsigned int filterChanger)
{
    if (filterChanger<0 || filterChanger>=getFilterChangerCount()) return;
    JKSerialConnection* com=ports.getCOMPort(wheels[filterChanger].port);
    if (!com) return;
    QMutex* mutex=ports.getMutex(wheels[filterChanger].port);
    QMutexLocker locker(mutex);
    com->open();
    if (com->isConnectionOpen()) {
        QTime t;
        t.start();
        // wait CONNECTION_DELAY_MS ms for connection!
        while (t.elapsed()<CONNECTION_DELAY_MS)  {
            QApplication::processEvents();
        }
    } else {
        log_error(tr("%1 Could not connect to Servo Filterwheel Driver [port=%1  baud=%2]!!!\n").arg(com->get_port().c_str()).arg(com->get_baudrate()));
        log_error(tr("%1 reason: %2\n").arg(LOG_PREFIX).arg(wheels[filterChanger].serial->getLastError()));
    }

}

void QFExtensionServoPololuMaestro::filterChangerDisonnect(unsigned int filterChanger)
{
    if (filterChanger<0 || filterChanger>=getFilterChangerCount()) return;
    JKSerialConnection* com=ports.getCOMPort(wheels[filterChanger].port);
    if (!com) return;
    QMutex* mutex=ports.getMutex(wheels[filterChanger].port);
    QMutexLocker locker(mutex);
    com->close();
}

void QFExtensionServoPololuMaestro::setFilterChangerLogging(QFPluginLogService *logService)
{
    this->logService=logService;
}

bool QFExtensionServoPololuMaestro::isFilterChangerConnected(unsigned int filterChanger)
{
    if (filterChanger<0 || filterChanger>=getFilterChangerCount()) return false;
    JKSerialConnection* com=ports.getCOMPort(wheels[filterChanger].port);
    if (!com) return false;
    QMutex* mutex=ports.getMutex(wheels[filterChanger].port);
    return com->isConnectionOpen();
}

unsigned int QFExtensionServoPololuMaestro::getFilterChangerFilterCount(unsigned int filterChanger)
{
    if (filterChanger<0 || filterChanger>=getFilterChangerCount()) return 0;
    return wheels[filterChanger].targets.size();
}

void QFExtensionServoPololuMaestro::setFilterChangerFilter(unsigned int filterChanger, unsigned int filter)
{
    if (filterChanger<0 || filterChanger>=getFilterChangerCount()) return;
    JKSerialConnection* com=ports.getCOMPort(wheels[filterChanger].port);
    if (!com) return;
    QMutex* mutex=ports.getMutex(wheels[filterChanger].port);
    QMutexLocker locker(mutex);
    if (com->isConnectionOpen()) {
        wheels[filterChanger].serial->sendCommand(pololuMaestroBuildSetPosCommand(wheels[filterChanger].id, wheels[filterChanger].targets.value(filter, 0)));
        wheels[filterChanger].currentFilter=filter;
        wheels[filterChanger].lastAction=QTime::currentTime();
    }
}

unsigned int QFExtensionServoPololuMaestro::getFilterChangerCurrentFilter(unsigned int filterChanger)
{
    if (filterChanger<0 || filterChanger>=getFilterChangerCount()) return 0;
    return wheels[filterChanger].currentFilter;
}

bool QFExtensionServoPololuMaestro::isLastFilterChangerActionFinished(unsigned int filterChanger)
{
    if (filterChanger<0 || filterChanger>=getFilterChangerCount()) return true;
    return wheels[filterChanger].lastAction.elapsed()>wheels[filterChanger].operation_duration;

}

QString QFExtensionServoPololuMaestro::getFilterChangerDescription(unsigned int filterChanger)
{
    if (filterChanger<0 || filterChanger>=getFilterChangerCount()) return "";
    return wheels[filterChanger].description;
}

QString QFExtensionServoPololuMaestro::getFilterChangerShortName(unsigned int filterChanger)
{
    if (filterChanger<0 || filterChanger>=getFilterChangerCount()) return "";
    return wheels[filterChanger].label;
}

void QFExtensionServoPololuMaestro::showFilterChangerSettingsDialog(unsigned int filterChanger, QWidget *parent)
{
    showShutterSettingsDialog(filterChanger, parent);
}

void QFExtensionServoPololuMaestro::showTester()
{
    DlgPololuMaestroTester* dlg=new DlgPololuMaestroTester(NULL);
    dlg->setWindowIcon(QIcon(getIconFilename()));
    dlg->exec();
    delete dlg;
}


Q_EXPORT_PLUGIN2(servo_pololu_maestro, QFExtensionServoPololuMaestro)
