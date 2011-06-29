#include "cam_andor.h"
#include <QtGui>
#include <QtPlugin>
#include <iostream>
#include <unistd.h>
#include <time.h>
#include "qmodernprogresswidget.h"

#define LOG_PREFIX "[Andor]:  "


#define CHECK(s, msg) \
{ \
    unsigned int error=s; \
    if (error!=DRV_SUCCESS) { \
        log_error(QString(msg)+tr("\n%3    error code was: %1 [%2]\n%3    instruction was: %4").arg(error).arg(andorErrorToString(error)).arg(LOG_PREFIX).arg(#s)); \
        return false; \
    } \
}

#define CHECK_ON_ERROR(s, msg, onError) \
{ \
    unsigned int error=s; \
    if (error!=DRV_SUCCESS) { \
        log_error(QString(msg)+tr("\n%3    error code was: %1 [%2]\n%3    instruction was: %4").arg(error).arg(andorErrorToString(error)).arg(LOG_PREFIX).arg(#s)); \
        { onError; }; \
        return false; \
    } \
}

QFExtensionCameraAndor::CameraInfo::CameraInfo() {
    width=10;
    height=10;

    shutterMode=1;

    AcqMode=1;
    ReadMode=4;
    expoTime=0.1;
    trigMode=0;
    numKins=2;
    numAccs=1;
    kinTime=0;
    accTime=0;
    subImage=QRect(1,1,512,512);
    hbin=1;
    vbin=1;
    spool=0;

}


QFExtensionCameraAndor::QFExtensionCameraAndor(QObject* parent):
    QObject(parent)
{
    logService=NULL;
    conn=false;
    #ifdef __LINUX__
    detectorsIniPath=detectorsIniPath_init="/usr/local/etc/andor";
    #else
    detectorsIniPath=detectorsIniPath_init="./";
    #endif
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

    detectorsIniPath=settings.value(getID()+"/detectorsIniPath", detectorsIniPath).toString();
}

void QFExtensionCameraAndor::storeSettings(ProgramOptions* settingspo) {
    if (!settingspo) return;
    if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings());

    if (detectorsIniPath!=detectorsIniPath_init) settings.setValue(getID()+"/detectorsIniPath", detectorsIniPath);
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
    if (!isConnected(camera)) return 0;
    return camInfos[camera].width;
}

int QFExtensionCameraAndor::getImageHeight(unsigned int camera) {
    if (!isConnected(camera)) return 0;
    return camInfos[camera].height;
}

bool QFExtensionCameraAndor::isConnected(unsigned int camera) {
    return camConnected.contains(camera);
}

bool QFExtensionCameraAndor::acquire(unsigned int camera, uint32_t* data, uint64_t* timestamp) {
    if (!isConnected(camera)) return false;

    if (!selectCamera(camera)) return false;

    CHECK(StartAcquisition(), tr(""));

    CameraInfo info;
    if (camInfos.contains(camera)) info=camInfos[camera];

    int status;

	//Loop until acquisition finished
	CHECK(GetStatus(&status), tr("error while waiting for frame"));
	while(status==DRV_ACQUIRING) {
		CHECK(GetStatus(&status), tr("error while waiting for frame"));
	}


	log_text(tr("acquiring image w=%1, h=%2\n").arg(info.width).arg(info.height));
	at_32* imageData = (at_32*)malloc(info.width*info.height*sizeof(at_32));
	CHECK_ON_ERROR(GetAcquiredData(imageData, info.width*info.height), tr("error while acquiring frame"), free(imageData));

	for(int i=0;i<info.width*info.height;i++) data[i]=imageData[i];
	free(imageData);

    return true;
}



bool QFExtensionCameraAndor::connectDevice(unsigned int camera) {

    timer.start();

    QModernProgressDialog progress(tr("Initializing Andor Camera #%1").arg(camera+1), "", NULL);
    progress.setWindowModality(Qt::WindowModal);
    progress.setHasCancel(false);
    progress.open();


    at_32 *data = NULL;

    //Initialise and setup defaults
    char path[2048];

    if (selectCamera(camera)) {


        strcpy(path, detectorsIniPath.toStdString().c_str());
        progress.setLabelText(tr("initializing camera %1 ...<br>&nbsp;&nbsp;&nbsp;&nbsp;detectorsIniPath='%2'").arg(camera).arg(detectorsIniPath));
        CHECK(Initialize(path), tr("Error during Initialize"));

        // wait for >2 seconds and call QApplication::processEvents(); here and there,
        // so the widgets are updated
        for (int i=0; i<200; i++)  {
            usleep(10000);
            QApplication::processEvents();
            if (progress.wasCanceled()) {
                progress.accept();
                return false;
            }
        }


        // init CameraInfo object
        CameraInfo info;
        //if (camInfos.contains(camera)) info=camInfos[camera];

        CHECK(GetDetector(&(info.width), &(info.height)), tr("error while getting detector info"));
        info.subImage=QRect(1,1,info.width, info.height);

        if (setCameraSettings(camera, info)) {
            camInfos[camera]=info;
            camConnected.insert(camera);
            log_text(tr("connected to Andor Camera #%1, width=%2, height=%3\n").arg(camera).arg(info.width).arg(info.height));
            return true;
        } else {
            log_error("error while setting camera settings!");
            return false;
        }

    } else {
        progress.accept();
        log_error(tr("could not select camera %1.").arg(camera));
        return false;
    }
    progress.accept();
    return false;
}

void QFExtensionCameraAndor::disconnectDevice(unsigned int camera) {
    int i=camera;
    if (getCameraCount()>0) {
        selectCamera(camera);
        SetShutter(1,2,50,50);
        /* wait for camera(s) temperature to rise up to 5°C */
        services->log_global_text(tr("%1 heating cameras to 5°C ...\n").arg(LOG_PREFIX));

        int temp=getTemperature(i);
        CoolerOFF();
        services->log_global_text(tr("%2 cameras #%3: cooler off, temperature = %1 °C ...\n").arg(temp).arg(LOG_PREFIX).arg(i+1));
        //std::cout<<tr("%2 cameras #%3: cooler off, temperature = %1 °C ...\n").arg(temp).arg(LOG_PREFIX).arg(i+1).toStdString()<<std::endl;
        bool tempOK=false;

        QModernProgressDialog progress(tr("Cooling Andor Camera #%1").arg(camera+1), "", NULL);
        progress.setWindowModality(Qt::WindowModal);
        progress.setHasCancel(false);
        progress.openDelayed(500);
        while (!tempOK) {
            tempOK=true;
            int temp1=getTemperature(i);
            //progress.setValue(33);
            progress.setLabelText(tr("cameras #%2: temperature = %1 °C ...\n").arg(temp1).arg(i+1));
            std::cout<<tr("%2 cameras #%3: cooler off, temperature = %1 °C ...\n").arg(temp1).arg(LOG_PREFIX).arg(i+1).toStdString()<<std::endl;
            if ((temp1<5)&&(temp1!=-999)) tempOK=false;
            QApplication::processEvents();
        }
        //progress.setValue(66);
        progress.setLabelText(tr("cameras #%2: shutdown ...\n").arg(i+1));
        QApplication::processEvents();
        ShutDown();
        //progress.setValue(100);
        progress.accept();

        services->log_global_text(tr("%1 heating cameras to 5°C ... DONE\n").arg(LOG_PREFIX));
        //std::cout<<tr("%1 heating cameras to 5°C ... DONE\n").arg(LOG_PREFIX).toStdString()<<std::endl;;
    }


    camConnected.remove(camera);
}

double QFExtensionCameraAndor::getExposureTime(unsigned int camera) {
    if (!isConnected(camera)) return 0;
    return camInfos[camera].expoTime;
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
    //std::cout<<"select ANDOR camera "<<iSelectedCamera<<std::endl;
    #ifdef __WINDOWS__
    long int SelectedCamera=0;
    #else
    int SelectedCamera=0;
    #endif
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



bool QFExtensionCameraAndor::setCameraSettings(int camera, QFExtensionCameraAndor::CameraInfo& info) {

    if (selectCamera(camera)) {
        CHECK(GetDetector(&(info.width), &(info.height)), tr("error while getting detector info"));
        /* SetShutter(
            typ = 0 (TTL low opens shutter)
            mode = 0 (Auto)
            closingtime = 50
            openingtime = 50
        */
        CHECK(SetShutter(1,info.shutterMode,50,50), tr("error while setting shutter"));
        CHECK(SetTriggerMode(info.trigMode), tr(""));
        CHECK(SetAcquisitionMode(info.AcqMode), tr(""));
        CHECK(SetReadMode(info.ReadMode), tr(""));
        CHECK(SetExposureTime(info.expoTime), tr(""));
        CHECK(SetAccumulationCycleTime(info.accTime), tr(""));
        CHECK(SetNumberAccumulations(info.numAccs), tr(""));
        CHECK(SetKineticCycleTime(info.kinTime), tr(""));
        CHECK(SetNumberKinetics(info.numKins), tr(""));
        CHECK(SetReadMode(info.ReadMode), tr(""));
        CHECK(SetImage(info.hbin,info.vbin,info.subImage.left(), info.subImage.right(), info.subImage.top(), info.subImage.bottom()), tr(""));
        CHECK(SetHSSpeed(0, 0), tr(""));

        return true;
    }

    return false;
}

int QFExtensionCameraAndor::getTemperature(int cam) {
    int temp1=-999;
    if (!selectCamera(cam)) {
        //services->log_global_error(tr("%2 cameras #%1: could not connect ...\n").arg(i+1).arg(LOG_PREFIX));
        //std::cout<<tr("%2 cameras #%1: could not connect ...\n").arg(cam+1).arg(LOG_PREFIX).toStdString()<<std::endl;
        return -999;
    } else {
        GetTemperature(&temp1);
        //std::cout<<tr("%2 cameras #%3: temperature = %1°C ...\n").arg(temp1).arg(LOG_PREFIX).arg(cam+1).toStdString()<<std::endl;
        //if ((temp1<5)&&(temp1!=-999)) tempOK=false;
        return temp1;
    }
}


QString QFExtensionCameraAndor::andorErrorToString(unsigned int error) {
    switch (error) {
        case DRV_SUCCESS: return tr("success"); break;
        case DRV_ERROR_CODES : return tr("DRV_ERROR_CODES"); break;
        case DRV_VXDNOTINSTALLED : return tr("VxD not installed"); break;
        case DRV_ERROR_SCAN : return tr(""); break;
        case DRV_ERROR_CHECK_SUM : return tr(""); break;
        case DRV_ERROR_FILELOAD : return tr(""); break;
        case DRV_UNKNOWN_FUNCTION : return tr("unknown function"); break;
        case DRV_ERROR_VXD_INIT : return tr(""); break;
        case DRV_ERROR_ADDRESS : return tr(""); break;
        case DRV_ERROR_PAGELOCK : return tr(""); break;
        case DRV_ERROR_PAGEUNLOCK : return tr(""); break;
        case DRV_ERROR_BOARDTEST : return tr(""); break;
        case DRV_ERROR_ACK : return tr(""); break;
        case DRV_ERROR_UP_FIFO : return tr(""); break;
        case DRV_ERROR_PATTERN : return tr(""); break;

        case DRV_ACQUISITION_ERRORS : return tr(""); break;
        case DRV_ACQ_BUFFER : return tr(""); break;
        case DRV_ACQ_DOWNFIFO_FULL : return tr(""); break;
        case DRV_PROC_UNKONWN_INSTRUCTION : return tr(""); break;
        case DRV_ILLEGAL_OP_CODE : return tr("illegal opcode"); break;
        case DRV_KINETIC_TIME_NOT_MET : return tr("kinetic time not met"); break;
        case DRV_ACCUM_TIME_NOT_MET : return tr("accumulation time not met"); break;
        case DRV_NO_NEW_DATA : return tr("no new data"); break;
        case DRV_SPOOLERROR : return tr(""); break;
        case DRV_SPOOLSETUPERROR : return tr(""); break;
        case DRV_FILESIZELIMITERROR : return tr(""); break;
        case DRV_ERROR_FILESAVE : return tr(""); break;

        case DRV_TEMPERATURE_CODES : return tr(""); break;
        case DRV_TEMPERATURE_OFF : return tr(""); break;
        case DRV_TEMPERATURE_NOT_STABILIZED : return tr("temperature not stabilized"); break;
        case DRV_TEMPERATURE_STABILIZED : return tr("temperature stabilized"); break;
        case DRV_TEMPERATURE_NOT_REACHED : return tr("temperature not reached"); break;
        case DRV_TEMPERATURE_OUT_RANGE : return tr(""); break;
        case DRV_TEMPERATURE_NOT_SUPPORTED : return tr(""); break;
        case DRV_TEMPERATURE_DRIFT : return tr(""); break;


        case DRV_GENERAL_ERRORS : return tr(""); break;
        case DRV_INVALID_AUX : return tr(""); break;
        case DRV_COF_NOTLOADED : return tr(""); break;
        case DRV_FPGAPROG : return tr(""); break;
        case DRV_FLEXERROR : return tr(""); break;
        case DRV_GPIBERROR : return tr("GPIB error"); break;
        case DRV_EEPROMVERSIONERROR : return tr(""); break;

        case DRV_DATATYPE : return tr(""); break;
        case DRV_DRIVER_ERRORS : return tr(""); break;
        case DRV_P1INVALID : return tr("parameter 1 invalid"); break;
        case DRV_P2INVALID : return tr("parameter 2 invalid"); break;
        case DRV_P3INVALID : return tr("parameter 3 invalid"); break;
        case DRV_P4INVALID : return tr("parameter 4 invalid"); break;
        case DRV_INIERROR : return tr(""); break;
        case DRV_COFERROR : return tr(""); break;
        case DRV_ACQUIRING : return tr(""); break;
        case DRV_IDLE : return tr(""); break;
        case DRV_TEMPCYCLE : return tr(""); break;
        case DRV_NOT_INITIALIZED : return tr("system not initialized"); break;
        case DRV_P5INVALID : return tr("parameter 5 invalid"); break;
        case DRV_P6INVALID : return tr("parameter 6 invalid"); break;
        case DRV_INVALID_MODE : return tr("invalid mode"); break;
        case DRV_INVALID_FILTER : return tr("invalid filter"); break;

        case DRV_I2CERRORS : return tr(""); break;
        case DRV_I2CDEVNOTFOUND : return tr(""); break;
        case DRV_I2CTIMEOUT : return tr(""); break;
        case DRV_P7INVALID : return tr(""); break;
        case DRV_P8INVALID : return tr(""); break;
        case DRV_P9INVALID : return tr(""); break;
        case DRV_P10INVALID : return tr(""); break;


        case DRV_USBERROR : return tr(""); break;
        case DRV_IOCERROR : return tr(""); break;
        case DRV_VRMVERSIONERROR : return tr(""); break;
        case DRV_USB_INTERRUPT_ENDPOINT_ERROR : return tr(""); break;
        case DRV_RANDOM_TRACK_ERROR : return tr(""); break;
        case DRV_INVALID_TRIGGER_MODE : return tr(""); break;
        case DRV_LOAD_FIRMWARE_ERROR : return tr(""); break;
        case DRV_DIVIDE_BY_ZERO_ERROR : return tr(""); break;
        case DRV_INVALID_RINGEXPOSURES : return tr(""); break;
        case DRV_BINNING_ERROR : return tr(""); break;
        case DRV_INVALID_AMPLIFIER : return tr(""); break;

        case DRV_ERROR_NOCAMERA: return tr("no camera"); break;
        case DRV_NOT_SUPPORTED: return tr(""); break;
        case DRV_NOT_AVAILABLE: return tr("feature not available"); break;

        case DRV_ERROR_MAP: return tr(""); break;
        case DRV_ERROR_UNMAP: return tr(""); break;
        case DRV_ERROR_MDL: return tr(""); break;
        case DRV_ERROR_UNMDL: return tr(""); break;
        case DRV_ERROR_BUFFSIZE: return tr(""); break;
        case DRV_ERROR_NOHANDLE: return tr(""); break;

        case DRV_GATING_NOT_AVAILABLE: return tr("gating not available"); break;
        case DRV_FPGA_VOLTAGE_ERROR: return tr("FPGA voltage error"); break;

        case DRV_OW_CMD_FAIL: return tr(""); break;
        case DRV_OWMEMORY_BAD_ADDR: return tr(""); break;
        case DRV_OWCMD_NOT_AVAILABLE: return tr(""); break;
        case DRV_OW_NO_SLAVES: return tr(""); break;
        case DRV_OW_NOT_INITIALIZED: return tr(""); break;
        case DRV_OW_ERROR_SLAVE_NUM: return tr(""); break;
        case DRV_MSTIMINGS_ERROR: return tr(""); break;

        case DRV_OA_NULL_ERROR: return tr(""); break;
        case DRV_OA_PARSE_DTD_ERROR: return tr(""); break;
        case DRV_OA_DTD_VALIDATE_ERROR: return tr(""); break;
        case DRV_OA_FILE_ACCESS_ERROR: return tr(""); break;
        case DRV_OA_FILE_DOES_NOT_EXIST: return tr(""); break;
        case DRV_OA_XML_INVALID_OR_NOT_FOUND_ERROR: return tr(""); break;
        case DRV_OA_PRESET_FILE_NOT_LOADED: return tr(""); break;
        case DRV_OA_USER_FILE_NOT_LOADED: return tr(""); break;
        case DRV_OA_PRESET_AND_USER_FILE_NOT_LOADED: return tr(""); break;
        case DRV_OA_INVALID_FILE: return tr(""); break;
        case DRV_OA_FILE_HAS_BEEN_MODIFIED: return tr(""); break;
        case DRV_OA_BUFFER_FULL: return tr(""); break;
        case DRV_OA_INVALID_STRING_LENGTH: return tr(""); break;
        case DRV_OA_INVALID_CHARS_IN_NAME: return tr(""); break;
        case DRV_OA_INVALID_NAMING: return tr(""); break;
        case DRV_OA_MODE_ALREADY_EXISTS: return tr(""); break;
        case DRV_OA_STRINGS_NOT_EQUAL: return tr(""); break;
        case DRV_OA_NO_USER_DATA: return tr(""); break;
        case DRV_OA_VALUE_NOT_SUPPORTED: return tr(""); break;
        case DRV_OA_MODE_DOES_NOT_EXIST: return tr(""); break;

    #ifdef __LINUX__
        case DRV_PROCESSING_FAILED: return tr("processing failed"); break;
        case DRV_OA_FAILED_TO_GET_MODE: return tr(""); break;
        case DRV_OA_CAMERA_NOT_SUPPORTED: return tr(""); break;
        case DRV_OA_GET_CAMERA_ERROR: return tr(""); break;
        case KERN_MEM_ERROR : return tr(""); break;
        case DRV_P11INVALID : return tr(""); break;
        case DRV_INVALID_COUNTCONVERT_MODE: return tr(""); break;
    #endif

        default: return tr(""); break;
    }
    return "";
}



Q_EXPORT_PLUGIN2(TARGETNAME, QFExtensionCameraAndor)



