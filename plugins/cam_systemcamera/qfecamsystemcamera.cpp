#include "qfecamsystemcamera.h"
#include <QtGui>
#include <QtPlugin>
#include <iostream>
#include "videocapture.h"


#define LOG_PREFIX QString("cam_systemcam >>> ").toUpper()


static void RGBToGray(uint8_t* rgb, uint32_t* out, int width, int height, QFECamSystemcamera::toGrayMethods method) {
    switch(method) {
        case QFECamSystemcamera::tgmRed:
            for (int y=0; y<height; y++) {
                for (int x=0; x<width; x++) {
                    out[y*width+x]=rgb[y*width*3+x*3];
                }
            }
            break;
        case QFECamSystemcamera::tgmGreen:
            for (int y=0; y<height; y++) {
                for (int x=0; x<width; x++) {
                    out[y*width+x]=rgb[y*width*3+x*3+1];
                }
            }
            break;
        case QFECamSystemcamera::tgmBlue:
            for (int y=0; y<height; y++) {
                for (int x=0; x<width; x++) {
                    out[y*width+x]=rgb[y*width*3+x*3+2];
                }
            }
            break;
        case QFECamSystemcamera::tgmAverage:
            for (int y=0; y<height; y++) {
                for (int x=0; x<width; x++) {
                    out[y*width+x]=round(double(rgb[y*width*3+x*3]+rgb[y*width*3+x*3+1]+rgb[y*width*3+x*3+2])/3.0);
                }
            }
            break;
    }

}


QFECamSystemcamera::QFECamSystemcamera(QObject* parent):
    QObject(parent)
{
	logService=NULL;
    toGrayMethod=QFECamSystemcamera::tgmAverage;
}

QFECamSystemcamera::~QFECamSystemcamera() {

}


void QFECamSystemcamera::deinit() {
	/* add code for cleanup here */
    for (int i=0; i<vids.size(); i++) {
        delete vids[i];
    }
    vids.clear();
}

void QFECamSystemcamera::projectChanged(QFProject* oldProject, QFProject* project) {
	/* usually cameras do not have to react to a change of the project in QuickFit .. so you don't need to do anything here
	   But: possibly you could read config information from the project here
	 */
}

void QFECamSystemcamera::initExtension() {
    /* do initializations here but do not yet connect to the camera! */
    VideoCapture cap;
    cameras=cap.getDeviceList();
    for (int i=0; i<cameras.size(); i++) {
        vids.append(new VideoCapture());
    }
}

void QFECamSystemcamera::loadSettings(ProgramOptions* settingspo) {
	/* here you could read config information from the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini
	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFECamSystemcamera::storeSettings(ProgramOptions* settingspo) {
	/* here you could write config information to the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini

	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

	}

unsigned int QFECamSystemcamera::getCameraCount() {
    return cameras.size();
}

void QFECamSystemcamera::useCameraSettings(unsigned int camera, const QSettings& settings) {
    /* set the camera settings to the values specified in settings parameter, called before acquire() */
}

bool QFECamSystemcamera::prepareAcquisition(unsigned int camera, const QSettings& settings, QString filenamePrefix) {
    /* set the camera settings to the values specified in settings parameter, called before startAcquisition() */

    // uncomment this if the code is the same as in useCameraSettings()
    //useCameraSettings(camera, settings);

    return true;
}


void QFECamSystemcamera::showCameraSettingsDialog(unsigned int camera, QSettings& settings, QWidget* parent) {
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

int QFECamSystemcamera::getImageWidth(unsigned int camera) {
    VideoCapture* vid=vids.value(camera, NULL);
    if (vid) return vid->getVideoWidth();
    return 0;
}

int QFECamSystemcamera::getImageHeight(unsigned int camera) {
    VideoCapture* vid=vids.value(camera, NULL);
    if (vid) return vid->getVideoHeight();
    return 0;
}

double QFECamSystemcamera::getPixelWidth(unsigned int camera) {
    return 1;
}

double QFECamSystemcamera::getPixelHeight(unsigned int camera) {
    return 1;
}


QString QFECamSystemcamera::getCameraName(unsigned int camera) {
    if (camera>0 && camera<cameras.size()) return cameras[camera].c_str();
    return QString("");
}

QString QFECamSystemcamera::getCameraSensorName(unsigned int camera) {
    return "";
}

bool QFECamSystemcamera::isConnected(unsigned int camera) {
    VideoCapture* vid=vids.value(camera, NULL);
    if (vid) return vid->isConnected();
    return false;
}

bool QFECamSystemcamera::acquire(unsigned int camera, uint32_t* data, uint64_t* timestamp) {
    if (camera>=cameras.size()) return false;
    if (timestamp!=NULL) {
        *timestamp=(uint64_t)round(timer[camera].elapsed());;
    }

    uint8_t* img=(uint8_t*)calloc(getImageWidth(camera)*getImageHeight(camera)*3, sizeof(uint8_t));

    vids[camera]->captureFrame(img);

    RGBToGray(img, data, getImageWidth(camera), getImageHeight(camera), toGrayMethod);

    free(img);
    return true;
}

bool QFECamSystemcamera::connectDevice(unsigned int camera) {
    if (camera>=cameras.size()) return false;
    timer[camera]=QElapsedTimer();
    timer[camera].start();
    return vids[camera]->connect(camera);
}

void QFECamSystemcamera::disconnectDevice(unsigned int camera) {
    /* disconnect from the given camera */
     if (camera>=cameras.size()) return ;
     vids[camera]->disconnect();
}

double QFECamSystemcamera::getExposureTime(unsigned int camera) {
    VideoCapture* vid=vids.value(camera, NULL);
    if (vid) return vid->getLastExposureTimet();
    return 0;
}

bool QFECamSystemcamera::startAcquisition(unsigned int camera) {
    return false;
}

void QFECamSystemcamera::cancelAcquisition(unsigned int camera) {

}

bool QFECamSystemcamera::isAcquisitionRunning(unsigned int camera, double* percentageDone) {
    return false;
}

void QFECamSystemcamera::getAcquisitionDescription(unsigned int camera, QList<AcquititonFileDescription> *files, QMap<QString, QVariant>* parameters) {
}

bool QFECamSystemcamera::getAcquisitionPreview(unsigned int camera, uint32_t* data) {
    return false;
}

int QFECamSystemcamera::getAcquisitionProgress(unsigned int camera) {
    return 0; // return a number between 0 and 100 which indicates the progress of a currently running acquisition
}

bool QFECamSystemcamera::isCameraSettingChangable(QFExtensionCamera::CameraSetting which) const  { 
	return false; 
}

void QFECamSystemcamera::changeCameraSetting(QSettings& settings, QFExtensionCamera::CameraSetting which, QVariant value)  {  

}

QVariant QFECamSystemcamera::getCameraSetting(QSettings& settings, QFExtensionCamera::CameraSetting which) const  { 
	return QVariant(); 
} 

void QFECamSystemcamera::log_text(QString message) {
	if (logService) logService->log_text(LOG_PREFIX+message);
	else if (services) services->log_text(LOG_PREFIX+message);
}

void QFECamSystemcamera::log_warning(QString message) {
	if (logService) logService->log_warning(LOG_PREFIX+message);
	else if (services) services->log_warning(LOG_PREFIX+message);
}

void QFECamSystemcamera::log_error(QString message) {
	if (logService) logService->log_error(LOG_PREFIX+message);
	else if (services) services->log_error(LOG_PREFIX+message);
}


Q_EXPORT_PLUGIN2(cam_systemcam, QFECamSystemcamera)
