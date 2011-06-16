#include "qfestagetest.h"
#include <QtGui>
#include <QtPlugin>
#include <iostream>

QFEStageTest::QFEStageTest(QObject* parent):
    QObject(parent)
{
	logService=NULL;
}

QFEStageTest::~QFEStageTest() {

}


void QFEStageTest::deinit() {
	/* add code for cleanup here */
}

void QFEStageTest::projectChanged(QFProject* oldProject, QFProject* project) {
	/* usually cameras do not have to react to a change of the project in QuickFit .. so you don't need to do anything here
	   But: possibly you could read config information from the project here
	 */
}

void QFEStageTest::initExtension() {
    /* do initializations here but do not yet connect to the camera! */
}

void QFEStageTest::loadSettings(ProgramOptions* settingspo) {
	/* here you could read config information from the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini

	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFEStageTest::storeSettings(ProgramOptions* settingspo) {
	/* here you could write config information to the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini

	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

unsigned int QFEStageTest::getAxisCount() {
	return 1;
}

void QFEStageTest::showSettingsDialog(QWidget* parent) {
	/* open a dialog that configures the camera.

	   usually you should display a modal QDialog descendent which writes back config when the user clicks OK

	   alternatively you may also display a window which stays open and allows the suer to set settings also
	   during the measurement.
	*/


	/////////////////////////////////////////////////////////////////////////////////
	// if you want the settings dialog to be modal, you may uncomment the next lines
	// and add implementations
	/////////////////////////////////////////////////////////////////////////////////
    /*
	QDialog* dlg=new QDialog(parent);

    QVBoxLayout* lay=new QVBoxLayout(dlg);
    dlg->setLayout(lay);

    QFormLayout* formlayout=new QFormLayout(dlg);


    //  create your widgets here, do not to initialize them with the current settings
    // QWidget* widget=new QWidget(dlg);
    // lay->addRow(tr("Name"), widget);


    lay->addLayout(formlayout);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, dlg);
    lay->addWidget(buttonBox);

    connect(buttonBox, SIGNAL(accepted()), dlg, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), dlg, SLOT(reject()));

    if ( dlg->exec()==QDialog::Accepted ) {
         //  read back values entered into the widgets and store in settings
    }
    delete dlg;
	*/
}

bool QFEStageTest::isConnected(unsigned int axis) {
	return false;
}

void QFEStageTest::connectDevice(unsigned int axis) {
}

void QFEStageTest::disconnectDevice(unsigned int axis) {
}

void QFEStageTest::setLogging(QFPluginLogService* logService) {
	this->logService=logService;
}

void QFEStageTest::setJoystickActive(unsigned int axis, bool enabled) {
}

bool QFEStageTest::isJoystickActive(unsigned int axis) {
	return false;
}

QFExtensionLinearStage::AxisState QFEStageTest::getAxisState(unsigned int axis)  {
	return QFExtensionLinearStage::Disconnected;
}

void QFEStageTest::stop(unsigned int axis) {
}

double QFEStageTest::getPosition(unsigned int axis) {
	return 0.0;
}

void QFEStageTest::move(double newPosition) {
}


void QFEStageTest::log_text(QString message) {
	if (logService) logService->log_text(message);
	else if (services) services->log_text(message);
}

void QFEStageTest::log_warning(QString message) {
	if (logService) logService->log_warning(message);
	else if (services) services->log_warning(message);
}

void QFEStageTest::log_error(QString message) {
	if (logService) logService->log_error(message);
	else if (services) services->log_error(message);
}


Q_EXPORT_PLUGIN2(stage_test, QFEStageTest)
