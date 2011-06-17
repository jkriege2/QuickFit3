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
    detectorsIniPath="/usr/local/etc/andor";
}

QFExtensionCameraAndor::~QFExtensionCameraAndor() {

}


void QFExtensionCameraAndor::deinit() {

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
    
    detectorsIniPath=settings->value(getID()+"/detectorsIniPath", detectorsIniPath).toString();
}

void QFExtensionCameraAndor::storeSettings(ProgramOptions* settingspo) {
    if (!settingspo) return;
    if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings());
    
    settings->setValue(getID()+"/detectorsIniPath", detectorsIniPath);
}

unsigned int QFExtensionCameraAndor::getCameraCount() {
    at_32 lNumCameras;
    GetAvailableCameras(&lNumCameras);
    return lNumCameras;
}

void QFExtensionCameraAndor::useCameraSettings(unsigned int camera, const QSettings& settings) {
}

bool QFExtensionCameraAndor::prepareAcquisition(unsigned int camera, const QSettings& settings, QString filenamePrefix) {
    useCameraSettings(camera, settings);
    return true;
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

    
   at_32 *data = NULL;

   //Initialise and setup defaults
   unsigned int error;
   error=Initialize(detectorsIniPath.toStdString().c_str());

   // TODO: Go on here ...
   cout << "Initialising..." << endl;
   sleep(2);
   if(error==DRV_SUCCESS) error=GetDetector(&width, &height);
   if(error==DRV_SUCCESS) error=SetShutter(1,0,50,50);
   if(error==DRV_SUCCESS) error=SetTriggerMode(trigMode);
   if(error==DRV_SUCCESS) error=SetAcquisitionMode(AcqMode);
   if(error==DRV_SUCCESS) error=SetReadMode(ReadMode);
   if(error==DRV_SUCCESS) error=SetExposureTime(expoTime);
   if(error==DRV_SUCCESS) error=SetAccumulationCycleTime(accTime);
   if(error==DRV_SUCCESS) error=SetNumberAccumulations(numAccs);
   if(error==DRV_SUCCESS) error=SetKineticCycleTime(kinTime);
   if(error==DRV_SUCCESS) error=SetNumberKinetics(numKins);
   if(error==DRV_SUCCESS) error=SetMultiTrack(numTracks,trackHeight,trackOffset, &trackBottom, &trackGap);
   randomTracks = new int[height*2];
   randomTracks[0]=1; randomTracks[1]=1;
   if(error==DRV_SUCCESS) SetReadMode(2);
   if(error==DRV_SUCCESS) error=SetRandomTracks(numTracks, randomTracks);
   if(error==DRV_SUCCESS) SetReadMode(ReadMode);

   subImage.left = 1; subImage.right = width; subImage.top = 1; subImage.bottom = height;
   if(error==DRV_SUCCESS) error=SetImage(hbin,vbin,subImage.left, subImage.right, subImage.top, subImage.bottom);
   if(error==DRV_SUCCESS) error=SetHSSpeed(0, 0);

   if(error!=DRV_SUCCESS){
    cout << "!!Error initialising system!!:: " << error << endl;
      return 1;
   }
    
    
    return conn;
}

void QFExtensionCameraAndor::disconnectDevice(unsigned int camera) {
    int i=camera;
    if (getCameraCount()>0) {
        /* wait for camera(s) temperature to rise up to 5°C */
        services->log_global_text(tr("%1 heating cameras to 5°C ...\n").arg(LOG_PREFIX));

        int temp=getTemperature(i);
        CoolerOFF();
        services->log_global_text(tr("%2 cameras #%3: cooler off, temperature = %1 °C ...\n").arg(temp).arg(LOG_PREFIX).arg(i+1));
        std::cout<<tr("%2 cameras #%3: cooler off, temperature = %1 °C ...\n").arg(temp).arg(LOG_PREFIX).arg(i+1).toStdString()<<std::endl;
        bool tempOK=false;

        QProgressDialog progress(tr("Cooling Andor Camera #%1").arg(camera+1), "", 0, 100, NULL);
        progress.setWindowModality(Qt::WindowModal);
        while (!tempOK) {
            tempOK=true;
            int temp1=getTemperature(i);
            progress.setValue(33);
            progress.setLabelText(tr("cameras #%2: temperature = %1 °C ...\n").arg(temp1).arg(i+1));
            std::cout<<tr("%2 cameras #%3: cooler off, temperature = %1 °C ...\n").arg(temp1).arg(LOG_PREFIX).arg(i+1).toStdString()<<std::endl;
            if ((temp1<5)&&(temp1!=-999)) tempOK=false;
            QApplication::processEvents();
        }
        progress.setValue(66);
        progress.setLabelText(tr("cameras #%2: shutdown ...\n").arg(i+1));
        QApplication::processEvents();
        ShutDown();
        progress.setValue(100);

        services->log_global_text(tr("%1 heating cameras to 5°C ... DONE\n").arg(LOG_PREFIX));    
        std::cout<<tr("%1 heating cameras to 5°C ... DONE\n").arg(LOG_PREFIX).toStdString()<<std::endl;;    
    }
    
    
    conn = false;
}

double QFExtensionCameraAndor::getExposureTime(unsigned int camera) {
	return 0;
}



bool QFExtensionCameraAndor::startAcquisition(unsigned int camera) {
    return false;
}

void QFExtensionCameraAndor::cancelAcquisition(unsigned int camera) {
}

bool QFExtensionCameraAndor::isAcquisitionRunning(unsigned int camera, double* percentageDone) {
    return false;
}

void QFExtensionCameraAndor::getAcquisitionDescription(unsigned int camera, QStringList* files, QMap<QString, QVariant>* parameters) {
}

bool QFExtensionCameraAndor::getAcquisitionPreview(unsigned int camera, uint32_t* data) {
    return false;
}

int QFExtensionCameraAndor::getAcquisitionProgress(unsigned int camera) {
    return 0;
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


bool QFExtensionCameraAndor::selectCamera (int iSelectedCamera) {
    std::cout<<"select ANDOR camera "<<iSelectedCamera<<std::endl;
    int SelectedCamera=0;
    GetCurrentCamera(&SelectedCamera);
    if (iSelectedCamera==SelectedCamera) return true;
    if ((iSelectedCamera < getCameraCount()) && (iSelectedCamera >= 0) ) {
        at_32 lCameraHandle;
        GetCameraHandle(iSelectedCamera, &lCameraHandle);
        SetCurrentCamera(lCameraHandle);
        return true;
    } else  {
        return false;
    }
}

int QFExtensionCameraAndor::getTemperature(int cam) {
    int temp1=-999;
    if (!selectCamera(cam)) {
        //services->log_global_error(tr("%2 cameras #%1: could not connect ...\n").arg(i+1).arg(LOG_PREFIX));
        std::cout<<tr("%2 cameras #%1: could not connect ...\n").arg(cam+1).arg(LOG_PREFIX).toStdString()<<std::endl;
        return -999;
    } else {
        GetTemperature(&temp1);
        std::cout<<tr("%2 cameras #%3: temperature = %1°C ...\n").arg(temp1).arg(LOG_PREFIX).arg(cam+1).toStdString()<<std::endl;
        //if ((temp1<5)&&(temp1!=-999)) tempOK=false;
        return temp1;
    }
}

//Q_EXPORT_PLUGIN2(cam_radhard2, QFExtensionCameraAndor)
Q_EXPORT_PLUGIN2(TARGETNAME, QFExtensionCameraAndor)

// global variables for
