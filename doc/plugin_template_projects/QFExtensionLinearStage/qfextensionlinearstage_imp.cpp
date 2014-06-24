/*
Copyright (c) 2014
	
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


#include "qfextensionlinearstage_imp.h"
#include <QtGui>
#include <QtPlugin>
#include <iostream>


#define LOG_PREFIX QString("target_id >>> ").toUpper()

QFExtensionLinearStageImplementation::QFExtensionLinearStageImplementation(QObject* parent):
    QObject(parent)
{
	logService=NULL;
}

QFExtensionLinearStageImplementation::~QFExtensionLinearStageImplementation() {

}


void QFExtensionLinearStageImplementation::deinit() {
	/* add code for cleanup here */
}

void QFExtensionLinearStageImplementation::projectChanged(QFProject* oldProject, QFProject* project) {
	/* usually cameras do not have to react to a change of the project in QuickFit .. so you don't need to do anything here
	   But: possibly you could read config information from the project here
	 */
}

void QFExtensionLinearStageImplementation::initExtension() {
    /* do initializations here but do not yet connect to the camera! */
}

void QFExtensionLinearStageImplementation::loadSettings(ProgramOptions* settingspo) {
	/* here you could read config information from the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini

	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFExtensionLinearStageImplementation::storeSettings(ProgramOptions* settingspo) {
	/* here you could write config information to the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini

	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

unsigned int QFExtensionLinearStageImplementation::getAxisCount() {
	return 1;
}

void QFExtensionLinearStageImplementation::showSettingsDialog(QWidget* parent) {
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

bool QFExtensionLinearStageImplementation::isConnected(unsigned int axis) {
	return false;
}

void QFExtensionLinearStageImplementation::connectDevice(unsigned int axis) {
}

void QFExtensionLinearStageImplementation::disconnectDevice(unsigned int axis) {
}

void QFExtensionLinearStageImplementation::setLogging(QFPluginLogService* logService) {
	this->logService=logService;
}

void QFExtensionLinearStageImplementation::setJoystickActive(unsigned int axis, bool enabled) {
}

bool QFExtensionLinearStageImplementation::isJoystickActive(unsigned int axis) {
	return false;
}

QFExtensionLinearStage::AxisState QFExtensionLinearStageImplementation::getAxisState(unsigned int axis)  {
	return QFExtensionLinearStage::Disconnected;
}

void QFExtensionLinearStageImplementation::stop(unsigned int axis) {
}

double QFExtensionLinearStageImplementation::getPosition(unsigned int axis) {
	return 0.0;
}

void QFExtensionLinearStageImplementation::move(double newPosition) {
}


void QFExtensionLinearStageImplementation::log_text(QString message) {
	if (logService) logService->log_text(LOG_PREFIX+message);
	else if (services) services->log_text(LOG_PREFIX+message);
}

void QFExtensionLinearStageImplementation::log_warning(QString message) {
	if (logService) logService->log_warning(LOG_PREFIX+message);
	else if (services) services->log_warning(LOG_PREFIX+message);
}

void QFExtensionLinearStageImplementation::log_error(QString message) {
	if (logService) logService->log_error(LOG_PREFIX+message);
	else if (services) services->log_error(LOG_PREFIX+message);
}


Q_EXPORT_PLUGIN2(target_id, QFExtensionLinearStageImplementation)
