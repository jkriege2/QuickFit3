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


#include "qfextensioncamera_imp.h"
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include <QtPlugin>
#include <iostream>


#define LOG_PREFIX QString("target_id >>> ").toUpper()


QFExtensionCameraImplementation::QFExtensionCameraImplementation(QObject* parent):
    QObject(parent)
{
	logService=NULL;
}

QFExtensionCameraImplementation::~QFExtensionCameraImplementation() {

}


void QFExtensionCameraImplementation::deinit() {
	/* add code for cleanup here */
}

void QFExtensionCameraImplementation::projectChanged(QFProject* oldProject, QFProject* project) {
	/* usually cameras do not have to react to a change of the project in QuickFit .. so you don't need to do anything here
	   But: possibly you could read config information from the project here
	 */
}

void QFExtensionCameraImplementation::initExtension() {
    /* do initializations here but do not yet connect to the camera! */
}

void QFExtensionCameraImplementation::loadSettings(ProgramOptions* settingspo) {
	/* here you could read config information from the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini
	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFExtensionCameraImplementation::storeSettings(ProgramOptions* settingspo) {
	/* here you could write config information to the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini

	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

	}

unsigned int QFExtensionCameraImplementation::getCameraCount() const {
    return 1;
	/* how man cameras may be accessed by your plugin (e.g. if you use one driver to access several cameras */
}
void QFExtensionCameraImplementation::useCameraSettings(unsigned int camera, const QSettings& settings) {
    /* set the camera settings to the values specified in settings parameter, called before acquire() */
}

bool QFExtensionCameraImplementation::prepareCameraAcquisition(unsigned int camera, const QSettings& settings, QString filenamePrefix) {
    /* set the camera settings to the values specified in settings parameter, called before startAcquisition() */

    // uncomment this if the code is the same as in useCameraSettings()
    //useCameraSettings(camera, settings);

    return true;
}


void QFExtensionCameraImplementation::showCameraSettingsDialog(unsigned int camera, QSettings& settings, QWidget* parent) {
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
    // lay->setValue(settings.value(QString("device/name%1").arg(camera), devfaultValue ).toInt());


    lay->addLayout(formlayout);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, dlg);
    lay->addWidget(buttonBox);

    connect(buttonBox, SIGNAL(accepted()), dlg, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), dlg, SLOT(reject()));

    if ( dlg->exec()==QDialog::Accepted ) {
         //  read back values entered into the widgets and store in settings
         // settings.setValue(QString("device/name%1").arg(camera), widget->value() );
    }
    delete dlg;
	*/
}

int QFExtensionCameraImplementation::getCameraImageWidth(unsigned int camera) {
    return /* width of the next image from the given camera */;
}

int QFExtensionCameraImplementation::getCameraImageHeight(unsigned int camera) {
    return /* height of the next image from the given camera */;
}

double QFExtensionCameraImplementation::getCameraPixelWidth(unsigned int camera) {
    return /* width of pixel in microns */;
}

double QFExtensionCameraImplementation::getCameraPixelHeight(unsigned int camera) {
    return /* width of pixel in microns */;
}


virtual QString QFExtensionCameraImplementation::getCameraName(unsigned int camera) {
    return /* name of the camera */;
}

virtual QString QFExtensionCameraImplementation::getCameraSensorName(unsigned int camera) {
    return /* name of the camera's sensor */;
}

bool QFExtensionCameraImplementation::isCameraConnected(unsigned int camera) {
    return /* are we connected to the specified camera */;
}

bool QFExtensionCameraImplementation::acquireOnCamera(unsigned int camera, uint32_t* data, uint64_t* timestamp, QMap<QString, QVariant>* parameters) {
    if (timestamp!=NULL) {
        *timestamp=/* store a timestamp information */;
    }

	/*
	    acquire a new image from the given camera and store it to data
		You may expect that data is at least as large as getImageWidth(camera)*getImageHeight(camera)
	*/
}

bool QFExtensionCameraImplementation::connectCameraDevice(unsigned int camera) {
    /* connect to the given camera */
	return true;
}

void QFExtensionCameraImplementation::disconnectCameraDevice(unsigned int camera) {
    /* disconnect from the given camera */
}

double QFExtensionCameraImplementation::getCameraExposureTime(unsigned int camera) {
    return /* exposure time of the last image */;
}

bool QFExtensionCameraImplementation::startCameraAcquisition(unsigned int camera) {
    return false;
}

void QFExtensionCameraImplementation::cancelCameraAcquisition(unsigned int camera) {

}

bool QFExtensionCameraImplementation::isCameraAcquisitionRunning(unsigned int camera, double* percentageDone) {
    return false;
}

void QFExtensionCameraImplementation::getCameraAcquisitionDescription(unsigned int camera, QList<QFExtensionCamera::CameraAcquititonFileDescription>* files, QMap<QString, QVariant>* parameters) {
}

bool QFExtensionCameraImplementation::getCameraAcquisitionPreview(unsigned int camera, uint32_t* data) {
    return false;
}

int QFExtensionCameraImplementation::getCameraAcquisitionProgress(unsigned int camera) {
    return 0; // return a number between 0 and 100 which indicates the progress of a currently running acquisition
}

bool QFExtensionCameraImplementation::isCameraSettingChangable(QFExtensionCamera::CameraSetting which)   { 
	return false; 
}

void QFExtensionCameraImplementation::changeCameraSetting(QSettings& settings, QFExtensionCamera::CameraSetting which, QVariant value)  {  

}

QVariant QFExtensionCameraImplementation::getCameraSetting(QSettings& settings, QFExtensionCamera::CameraSetting which)   { 
	return QVariant(); 
} 

QVariant QFExtensionCameraImplementation::getCameraCurrentSetting(int camera, QFExtensionCamera::CameraSetting which)   { 
	return QVariant(); 
} 

void QFExtensionCameraImplementation::log_text(QString message) {
	if (logService) logService->log_text(LOG_PREFIX+message);
	else if (services) services->log_text(LOG_PREFIX+message);
}

void QFExtensionCameraImplementation::log_warning(QString message) {
	if (logService) logService->log_warning(LOG_PREFIX+message);
	else if (services) services->log_warning(LOG_PREFIX+message);
}

void QFExtensionCameraImplementation::log_error(QString message) {
	if (logService) logService->log_error(LOG_PREFIX+message);
	else if (services) services->log_error(LOG_PREFIX+message);
}


Q_EXPORT_PLUGIN2(target_id, QFExtensionCameraImplementation)
