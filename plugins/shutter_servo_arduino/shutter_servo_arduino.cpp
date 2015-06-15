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

#include "shutter_servo_arduino.h"
#include<QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif


#include "qfplugin.h"
#include <iostream>

#define LOG_PREFIX "[ShutServoArd]: "
#define CONNECTION_DELAY_MS 50

QFExtensionShutterServoArduino::QFExtensionShutterServoArduino(QObject* parent):
    QObject(parent)
{
    //serial=new QF3SimpleB040SerialProtocolHandler(&com, getName());
	logService=NULL;

}

QFExtensionShutterServoArduino::~QFExtensionShutterServoArduino() {
    shutters.clear();
}


void QFExtensionShutterServoArduino::deinit() {
	/* add code for cleanup here */
    QSettings inifile(services->getGlobalConfigFileDirectory()+"/shutter_servo_arduino.ini", QSettings::IniFormat);
    if (inifile.isWritable()) {
        inifile.setValue("shutter_count", getShutterCount());
        for (unsigned int i=0; i<getShutterCount(); i++) {
            int p=shutters[i].port;
            if (ports.getCOMPort(p)) {
                ports.storeCOMPort(p, inifile, "shutter"+QString::number(i+1)+"/");
                inifile.setValue("shutter"+QString::number(i+1)+"/shutter_operation_duration", shutters[i].shutter_operation_duration);
                inifile.setValue("shutter"+QString::number(i+1)+"/id", shutters[i].id);
            }
        }
    }
}

void QFExtensionShutterServoArduino::projectChanged(QFProject* /*oldProject*/, QFProject* /*project*/) {

}

void QFExtensionShutterServoArduino::initExtension() {
    /* do initializations here but do not yet connect to the camera! */
    QString ini=services->getGlobalConfigFileDirectory()+QString("/shutter_servo_arduino.ini");
    if (!QFile::exists(ini)) ini=services->getConfigFileDirectory()+QString("/shutter_servo_arduino.ini");
    if (!QFile::exists(ini)) ini=services->getAssetsDirectory()+QString("/plugins/")+getID()+QString("/shutter_servo_arduino.ini");
    QFPluginServices::getInstance()->log_global_text(tr("%1loading INI-file %2\n").arg(QString(LOG_PREFIX)).arg(ini));
    QSettings inifile(ini, QSettings::IniFormat);
    int shutter_count=inifile.value("shutter_count", 0).toUInt();
    for (int i=0; i<shutter_count; i++) {
        SHUTTER s;
        s.port=ports.addCOMPort(inifile, "shutter"+QString::number(i+1)+"/");
        s.id=inifile.value("shutter"+QString::number(i+1)+"/id", i+1).toInt();
        s.infoMessage="";
        s.lastAction=QTime::currentTime();
        s.serial=new QF3SimpleB040SerialProtocolHandler(ports.getCOMPort(s.port), getName());
        s.shutter_operation_duration=inifile.value("shutter"+QString::number(i+1)+"/shutter_operation_duration", 500).toInt();
        s.label=inifile.value("shutter"+QString::number(i+1)+"/label", tr("%1 #%2").arg(getName()).arg(i+1)).toString();
        s.description=inifile.value("shutter"+QString::number(i+1)+"/description", tr("%1 #%2").arg(getDescription())).toString();
        shutters.append(s);
    }
}

void QFExtensionShutterServoArduino::loadSettings(ProgramOptions* settingspo) {
	/* here you could read config information from the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    //QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini

	// ALTERNATIVE: read/write Information to/from plugins/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFExtensionShutterServoArduino::storeSettings(ProgramOptions* settingspo) {
	/* here you could write config information to the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    //QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini

	// ALTERNATIVE: read/write Information to/from plugins/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

unsigned int QFExtensionShutterServoArduino::getShutterCount() {
    return shutters.size();
}

void QFExtensionShutterServoArduino::shutterConnect(unsigned int shutter) {
    if (shutter>=getShutterCount()) return;
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
        shutters[shutter].infoMessage=shutters[shutter].serial->queryCommand("?");
        //qDebug()<<"infoMessage '"<<infoMessage<<"'";
        if (!(shutters[shutter].infoMessage.toLower().contains("servo controller") && shutters[shutter].infoMessage.toLower().contains("jan krieger"))) {
            com->close();
            log_error(tr("%1 Could not connect to Servo Shutter Driver [port=%1  baud=%2]!!!\n").arg(com->get_port().c_str()).arg(com->get_baudrate()));
            log_error(tr("%1 reason: received wrong ID string from shutter driver: string was '%2'\n").arg(LOG_PREFIX).arg(shutters[shutter].infoMessage));
        }
    } else {
        log_error(tr("%1 Could not connect to Servo Shutter Driver [port=%1  baud=%2]!!!\n").arg(com->get_port().c_str()).arg(com->get_baudrate()));
        log_error(tr("%1 reason: %2\n").arg(LOG_PREFIX).arg(shutters[shutter].serial->getLastError()));
    }
}

void QFExtensionShutterServoArduino::shutterDisonnect(unsigned int shutter) {
    if (shutter>=getShutterCount()) return;
    JKSerialConnection* com=ports.getCOMPort(shutters[shutter].port);
    if (!com) return;
    QMutex* mutex=ports.getMutex(shutters[shutter].port);
    QMutexLocker locker(mutex);
    com->close();
}

bool QFExtensionShutterServoArduino::isShutterConnected(unsigned int shutter) {
    if (shutter>=getShutterCount()) return false;
    JKSerialConnection* com=ports.getCOMPort(shutters[shutter].port);
    if (!com) return false;
    QMutex* mutex=ports.getMutex(shutters[shutter].port);
    QMutexLocker locker(mutex);
    return com->isConnectionOpen();
}

bool QFExtensionShutterServoArduino::isShutterOpen(unsigned int shutter)  {
    if (shutter>=getShutterCount()) return false;
    JKSerialConnection* com=ports.getCOMPort(shutters[shutter].port);
    if (!com) return false;
    QMutex* mutex=ports.getMutex(shutters[shutter].port);
    QMutexLocker locker(mutex);
    if (!com->isConnectionOpen()) return false;
    QString result=shutters[shutter].serial->queryCommand(QString("Q")+QString::number(shutters[shutter].id));
    //qDebug()<<"shutter result: "<<result;
    if (result.startsWith("0")) return false;
    else return true;
}

void QFExtensionShutterServoArduino::setShutterState(unsigned int shutter, bool opened) {
    if (shutter>=getShutterCount()) return;
    JKSerialConnection* com=ports.getCOMPort(shutters[shutter].port);
    if (!com) return ;
    QMutex* mutex=ports.getMutex(shutters[shutter].port);
    QMutexLocker locker(mutex);
    if (!com->isConnectionOpen()) return ;
    if (opened) shutters[shutter].serial->sendCommand(QString("S")+QString::number(shutters[shutter].id)+"1");
    else shutters[shutter].serial->sendCommand(QString("S")+QString::number(shutters[shutter].id)+"0");
    shutters[shutter].lastAction=QTime::currentTime();

    QTime t=QTime::currentTime();
    QTime t2;
    t.start();
    while (t.elapsed()<20) {
        t2.start();
    }
}

bool QFExtensionShutterServoArduino::isLastShutterActionFinished(unsigned int shutter) {
    if (shutter>=getShutterCount()) return true;
    return shutters[shutter].lastAction.elapsed()>shutters[shutter].shutter_operation_duration;
}

QString QFExtensionShutterServoArduino::getShutterDescription(unsigned int shutter)  {
    if (shutter>=getShutterCount()) return getDescription();
    return shutters[shutter].description;
}

QString QFExtensionShutterServoArduino::getShutterShortName(unsigned int shutter)  {
    if (shutter>=getShutterCount()) return getName();
    return shutters[shutter].label;
}


void QFExtensionShutterServoArduino::showShutterSettingsDialog(unsigned int /*shutter*/, QWidget* parent) {
/*
    bool globalIniWritable=QSettings(services->getGlobalConfigFileDirectory()+"/shutter_servo_arduino.ini", QSettings::IniFormat).isWritable();


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
    QString ini1=services->getGlobalConfigFileDirectory()+QString("/shutter_servo_arduino.ini");
    QString ini2=services->getConfigFileDirectory()+QString("/shutter_servo_arduino.ini");
    QString ini3=services->getAssetsDirectory()+QString("/plugins/")+getID()+QString("/shutter_servo_arduino.ini");
    QMessageBox::information(parent, getName(), tr("There is no configuration dialog for this device. Set all config in the appropriate ini file:\n  %1\n  or: %2\n  or: %3").arg(ini1).arg(ini2).arg(ini3));
}


void QFExtensionShutterServoArduino::setLogging(QFPluginLogService* logService) {
	this->logService=logService;
}

void QFExtensionShutterServoArduino::log_text(QString message) {
	if (logService) logService->log_text(message);
	else if (services) services->log_text(message);
}

void QFExtensionShutterServoArduino::log_warning(QString message) {
	if (logService) logService->log_warning(message);
	else if (services) services->log_warning(message);
}

void QFExtensionShutterServoArduino::log_error(QString message) {
	if (logService) logService->log_error(message);
	else if (services) services->log_error(message);
}

void QFExtensionShutterServoArduino::setShutterLogging(QFPluginLogService* logService) {
    this->logService=logService;
}


Q_EXPORT_PLUGIN2(shutter_servo_arduino, QFExtensionShutterServoArduino)
