#include "shutter_servo_arduino.h"
#include <QtGui>
#include <QtPlugin>
#include <iostream>

#define LOG_PREFIX "[ShutArd]: "

QFExtensionShutterServoArduino::QFExtensionShutterServoArduino(QObject* parent):
    QObject(parent)
{
    serial=new QF3SimpleB040SerialProtocolHandler(&com, getName());
	logService=NULL;
    shutter_count=4;
    COMPort="COM1";
    COMPortSpeed=115200;
    shutter_operation_duration=250;
    lastAction=QTime::currentTime();

}

QFExtensionShutterServoArduino::~QFExtensionShutterServoArduino() {

}


void QFExtensionShutterServoArduino::deinit() {
	/* add code for cleanup here */
    QSettings inifile(services->getGlobalConfigFileDirectory()+"/shutter_servo_arduino.ini", QSettings::IniFormat);
    if (inifile.isWritable()) {
        inifile.setValue("driver/port", COMPort);
        inifile.setValue("driver/port_speed", COMPortSpeed);
        inifile.setValue("driver/shutter_count", shutter_count);
        inifile.setValue("driver/shutter_operation_duration", shutter_operation_duration);
    }
}

void QFExtensionShutterServoArduino::projectChanged(QFProject* oldProject, QFProject* project) {

}

void QFExtensionShutterServoArduino::initExtension() {
    /* do initializations here but do not yet connect to the camera! */
    QString ini=services->getGlobalConfigFileDirectory()+QString("/shutter_servo_arduino.ini");
    if (!QFile::exists(ini)) ini=services->getConfigFileDirectory()+QString("/shutter_servo_arduino.ini");
    if (!QFile::exists(ini)) ini=services->getAssetsDirectory()+QString("/plugins/")+getID()+QString("/shutter_servo_arduino.ini");
    QSettings inifile(ini, QSettings::IniFormat);
    COMPort=inifile.value("driver/port", COMPort).toString();
    COMPortSpeed=inifile.value("driver/port_speed", COMPortSpeed).toUInt();
    shutter_count=inifile.value("driver/shutter_count", shutter_count).toUInt();
    shutter_operation_duration=inifile.value("driver/shutter_operation_duration", shutter_operation_duration).toUInt();
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
    return shutter_count;
}

void QFExtensionShutterServoArduino::shutterConnect(unsigned int shutter) {
    com.set_port(COMPort.toStdString());
    com.set_baudrate(COMPortSpeed);
    com.set_stopbits(JKSConeStopbit);
    com.set_parity(JKSCnoParity);
    com.set_databits(JKSC8databits);
    com.open();
    if (com.isConnectionOpen()) {
        std::string info=serial->queryCommand("?");
        infoMessage=info.c_str();
        if (!(infoMessage.toLower().contains("servo controller") && infoMessage.toLower().contains("jan krieger"))) com.close();
    } else {
        log_error(tr(LOG_PREFIX " Could not connect to Servo Shutter Driver (B040)!!!\n"));
        log_error(tr(LOG_PREFIX " reason: %1\n").arg(com.getLastError().c_str()));
    }
}

void QFExtensionShutterServoArduino::shutterDisonnect(unsigned int shutter) {
    com.close();
}

bool QFExtensionShutterServoArduino::isShutterConnected(unsigned int shutter) {
    return com.isConnectionOpen();
}

bool QFExtensionShutterServoArduino::isShutterOpen(unsigned int shutter)  {
    return serial->queryCommand(format("Q%d", shutter)).substr(0,1)!="0";
}

void QFExtensionShutterServoArduino::setShutterState(unsigned int shutter, bool opened) {
    if (opened) serial->sendCommand(format("S%d1", shutter));
    else serial->sendCommand(format("S%d0", shutter));
    lastAction=QTime::currentTime();
}

bool QFExtensionShutterServoArduino::isLastShutterActionFinished(unsigned int shutter) {
    return lastAction.elapsed()>shutter_operation_duration;
}

QString QFExtensionShutterServoArduino::getShutterDescription(unsigned int shutter)  {
    return getDescription();
}

QString QFExtensionShutterServoArduino::getShutterShortName(unsigned int shutter)  {
    return getName();
}


void QFExtensionShutterServoArduino::showShutterSettingsDialog(unsigned int shutter, QWidget* parent) {

    bool globalIniWritable=QSettings(services->getGlobalConfigFileDirectory()+"/shutter_servo_arduino.ini", QSettings::IniFormat).isWritable();


    if (globalIniWritable) {
        /////////////////////////////////////////////////////////////////////////////////
        // if you want the settings dialog to be modal, you may uncomment the next lines
        // and add implementations
        /////////////////////////////////////////////////////////////////////////////////

        QDialog* dlg=new QDialog(parent);

        QVBoxLayout* lay=new QVBoxLayout(dlg);
        dlg->setLayout(lay);

        QFormLayout* formlayout=new QFormLayout(dlg);


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
    }
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


Q_EXPORT_PLUGIN2(shutter_servo_arduino, QFExtensionShutterServoArduino)
