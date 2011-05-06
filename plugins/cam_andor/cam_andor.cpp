#include "cam_andor.h"
#include <QtGui>
#include <QtPlugin>
#include <iostream>
#include <unistd.h>
#include <time.h>

#define LOG_PREFIX "[Andor]:  "

QFExtensionCameraAndor::QFExtensionCameraAndor(QObject* parent):
    QObject(parent)
{
    logService=NULL;
    conn=false;
}

QFExtensionCameraAndor::~QFExtensionCameraAndor() {

}


void QFExtensionCameraAndor::deinit() {
	/* add code for cleanup here */
}

void QFExtensionCameraAndor::projectChanged(QFProject* oldProject, QFProject* project) {
	/* usually cameras do not have to react to a change of the project in QuickFit .. so you don't need to do anything here
	   But: possibly you could read config information from the project here
	 */
}

void QFExtensionCameraAndor::initExtension() {
    services->log_global_text(tr("%2initializing extension '%1' ...\n").arg(getName()).arg(LOG_PREFIX));

    loadSettings(NULL);

    services->log_global_text(tr("%2initializing extension '%1' ... DONE\n").arg(getName()).arg(LOG_PREFIX));
}




void QFExtensionCameraAndor::loadSettings(ProgramOptions* settingspo) {
    if (!settingspo) return;
    if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings());
}

void QFExtensionCameraAndor::storeSettings(ProgramOptions* settingspo) {
    if (!settingspo) return;
    if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings());
}

unsigned int QFExtensionCameraAndor::getCameraCount() {
    return 1;
}

void QFExtensionCameraAndor::useCameraSettings(unsigned int camera, const QSettings& settings) {
}

void QFExtensionCameraAndor::prepareAcquisition(unsigned int camera, const QSettings& settings) {
    useCameraSettings(camera, settings);
}


void QFExtensionCameraAndor::showCameraSettingsDialog(unsigned int camera, QSettings& settings, QWidget* parent) {
	/* open a dialog that configures the camera.

	   usually you should display a modal QDialog descendent which writes back config when the user clicks OK

	   alternatively you may also display a window which stays open and allows the suer to set settings also
	   during the measurement.
	*/
    QDialog* dlg=new QDialog(parent);

    QVBoxLayout* lay=new QVBoxLayout(dlg);
    dlg->setLayout(lay);

    QFormLayout* formlayout=new QFormLayout(dlg);



    /*spIterations=new QSpinBox(dlg);
    spIterations->setMinimum(0);
    spIterations->setMaximum(100000);
    spIterations->setValue(settings.value("radhard2/iterations", 1000).toUInt());
    formlayout->addRow(tr("iterations:"), spIterations);
    connect(spIterations, SIGNAL(valueChanged(int)), this, SLOT(updateAcquisitionTime()));*/


    lay->addLayout(formlayout);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, dlg);
    lay->addWidget(buttonBox);

    connect(buttonBox, SIGNAL(accepted()), dlg, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), dlg, SLOT(reject()));

    if ( dlg->exec()==QDialog::Accepted ) {
         //  read back values entered into the widgets
         //settings.setValue("radhard2/iterations", spIterations->value());
    }
    delete dlg;
}

int QFExtensionCameraAndor::getImageWidth(unsigned int camera) {
    return 32;
}

int QFExtensionCameraAndor::getImageHeight(unsigned int camera) {
    return 32;
}

bool QFExtensionCameraAndor::isConnected(unsigned int camera) {
    return conn;
}

bool QFExtensionCameraAndor::acquire(unsigned int camera, uint32_t* data, uint64_t* timestamp) {

    return true;
}

bool QFExtensionCameraAndor::connectDevice(unsigned int camera) {
    conn=false;
    timer.start();
    conn=true;
    return conn;
}

void QFExtensionCameraAndor::disconnectDevice(unsigned int camera) {
    log_text(tr("disconnected\n"));
    conn = false;
}

double QFExtensionCameraAndor::getExposureTime(unsigned int camera) {
	return 0;
}



bool QFExtensionCameraAndor::startAcquisition(unsigned int camera, QString filenamePrefix) {
    return false;
}

bool QFExtensionCameraAndor::cancelAcquisition(unsigned int camera) {
    return true;
}

bool QFExtensionCameraAndor::isAcquisitionRunning(unsigned int camera, double* percentageDone) {
    return false;
}

void QFExtensionCameraAndor::getAcquisitionDescription(unsigned int camera, QStringList* files, QMap<QString, QVariant>* parameters) {
}

bool QFExtensionCameraAndor::getAcquisitionPreview(unsigned int camera, uint32_t* data) {
    return false;
}

void QFExtensionCameraAndor::log_text(QString message) {
	if (logService) logService->log_text(LOG_PREFIX+message);
	else if (services) services->log_text(LOG_PREFIX+message);
}

void QFExtensionCameraAndor::log_warning(QString message) {
	if (logService) logService->log_warning(LOG_PREFIX+message);
	else if (services) services->log_warning(LOG_PREFIX+message);
}

void QFExtensionCameraAndor::log_error(QString message) {
	if (logService) logService->log_error(LOG_PREFIX+message);
	else if (services) services->log_error(LOG_PREFIX+message);
}


//Q_EXPORT_PLUGIN2(cam_radhard2, QFExtensionCameraAndor)
Q_EXPORT_PLUGIN2(TARGETNAME, QFExtensionCameraAndor)

// global variables for
