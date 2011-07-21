#include "cam_andor.h"
#include <QtGui>
#include <QtPlugin>
#include <iostream>
#include <unistd.h>
#include <time.h>
#include <QMapIterator>
#include "qmodernprogresswidget.h"
#include "flowlayout.h"
#include <cmath>
#include "andorsettingsdialog.h"



#ifndef __WINDOWS__
# if defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32)
#  define __WINDOWS__
# endif
#endif

#ifndef __LINUX__
# if defined(linux)
#  define __LINUX__
# endif
#endif

#ifndef __WINDOWS__
# ifndef __LINUX__
#  warning("these methods are ment to be used under windows or linux ... no other system were tested")
# endif
#endif


#ifdef __WINDOWS__
#  include "ATMCD32D.H"
#else
#  include "atmcdLXd.h"
#endif




#define LOG_PREFIX "[Andor]:  "
#define GLOBAL_INI services->getConfigFileDirectory()+QString("/cam_andor.ini")

#define CHECK(s, msg) \
{ \
    unsigned int error=s; \
    if (error!=DRV_SUCCESS) { \
        log_error(QString(msg)+tr("\n%3    error code was: %1 [%2]\n%3    instruction was: %4").arg(error).arg(andorErrorToString(error)).arg(LOG_PREFIX).arg(#s)); \
        return false; \
    } \
}

#define CHECK_NO_RETURN(s, msg) \
{ \
    unsigned int error=s; \
    if (error!=DRV_SUCCESS) { \
        log_warning(QString(msg)+tr("\n%3    error code was: %1 [%2]\n%3    instruction was: %4").arg(error).arg(andorErrorToString(error)).arg(LOG_PREFIX).arg(#s)); \
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

QFExtensionCameraAndor::CameraGlobalSettings::CameraGlobalSettings() {
    coolerOn=true;
    setTemperature=20;
    fanMode=0;
    shutterMode=1;
    shutterOpeningTime=50;
    shutterClosingTime=50;
}


void QFExtensionCameraAndor::CameraGlobalSettings::readSettings(QSettings& settings, int camera) {
    settings.sync();
    coolerOn=settings.value(QString("camera%1/cooler_on").arg(camera), coolerOn).toBool();
    setTemperature=settings.value(QString("camera%1/set_temperature").arg(camera), setTemperature).toInt();
    fanMode=settings.value(QString("camera%1/fan_mode").arg(camera), fanMode).toInt();
}

void QFExtensionCameraAndor::CameraGlobalSettings::writeSettings(QSettings& settings, int camera) const {
    settings.setValue(QString("camera%1/cooler_on").arg(camera), coolerOn);
    settings.setValue(QString("camera%1/set_temperature").arg(camera), setTemperature);
    settings.setValue(QString("camera%1/fan_mode").arg(camera), fanMode);
    settings.sync();
}

QFExtensionCameraAndor::CameraInfo::CameraInfo() {
    width=10;
    height=10;


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
    emgain=1;
    outputAmplifier=0;
    frameTransfer=true;


    preamp_gain=0;
    vsAmplitude=0;
    vsSpeed=0;
    hsSpeed=0;
    advancedEMGain=false;

    cropMode=false;

    baselineOffset=0;
    baselineClamp=true;

    frameTransfer=true;

    headModel="";
    serialNumber=0;

    ADchannel=0;
    bitDepth=1;
    preampGainF=1;
    pixelWidth=0;
    pixelHeight=0;
    verticalSpeed=0;
    horizontalSpeed=0;readoutTime=0;

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
    storeGlobalSettings();
}

void QFExtensionCameraAndor::projectChanged(QFProject* oldProject, QFProject* project) {
	/* usually cameras do not have to react to a change of the project in QuickFit .. so you don't need to do anything here
	   But: possibly you could read config information from the project here
	 */
}

void QFExtensionCameraAndor::initExtension() {
    services->log_global_text(tr("%2initializing extension '%1' ...\n").arg(getName()).arg(LOG_PREFIX));

    // read and output version infos:
    char version[1024];
    GetVersionInfo(AT_SDKVersion, version, 1023);
    SDKVersion=version;
    services->log_global_text(tr("%2    SDK version: %1\n").arg(QString(version)).arg(LOG_PREFIX));
    GetVersionInfo(AT_DeviceDriverVersion, version, 1023);
    deviceDriverVersion=version;
    services->log_global_text(tr("%2    device driver version: %1\n").arg(QString(version)).arg(LOG_PREFIX));


    // try to load global settings for all available cameras (more may be added by connectDevice() function
    // writeback of these occurs in deinit()
    // these settings will be modified by a special dialog available through a menu entry!
    QSettings inifile(GLOBAL_INI, QSettings::IniFormat);
    camGlobalSettings.clear();
    for (unsigned int i=0; i<getCameraCount(); i++) {
        CameraGlobalSettings global;
        global.readSettings(inifile, i);
        camGlobalSettings[i]=global;
    }


    // create a (still invisible) window that shows the current global camera settings
    // + a menu item to display it.
    dlgGlobalSettings=new QWidget(NULL, Qt::Tool|Qt::WindowTitleHint|Qt::WindowCloseButtonHint|Qt::WindowStaysOnTopHint);
    dlgGlobalSettings->hide();
    dlgGlobalSettings_layout=new FlowLayout(dlgGlobalSettings);
    dlgGlobalSettings->setLayout(dlgGlobalSettings_layout);
    dlgGlobalSettings->setWindowTitle(tr("Andor: Global Settings"));

    QAction* actShowGlobal=new QAction(tr("show global settings ..."), this);
    connect(actShowGlobal, SIGNAL(triggered()), dlgGlobalSettings, SLOT(show()));
    QMenu* extm=services->getMenu("extensions");
    if (extm) {
        QMenu* andorMenu=extm->addMenu(QIcon(getIconFilename()), tr("Andor Cameras"));
        andorMenu->addAction(actShowGlobal);
    }

/*
    #ifdef __LINUX__
    { services->log_global_text(tr("%2  loading Andor driver module into Linux kernel (running andordrvlx_load) ...").arg(getName()).arg(LOG_PREFIX));

      QProcess *myProcess = new QProcess(this);
      myProcess->start(QString("andordrvlx_load"));
      bool ok=myProcess->waitForFinished(10000);
      if (!ok) {
          services->log_global_error(tr("\n%2  ERROR loading Andor driver module into Linux kernel ... process timed out after 10 seconds\n").arg(getName()).arg(LOG_PREFIX));
          myProcess->kill();
      } else {
          QProcess::ExitStatus status=myProcess->exitStatus();
          int code=myProcess->exitCode();
          if (status!=QProcess::NormalExit) {
              services->log_global_error(tr("\n%2  ERROR loading Andor driver module into Linux kernel ... process crashed\n").arg(getName()).arg(LOG_PREFIX));
          } else if (code!=0)  {
              services->log_global_error(tr("\n%2  ERROR loading Andor driver module into Linux kernel ... errorcode non-zero, was %1\n").arg(code).arg(LOG_PREFIX));
          } else {
              services->log_global_text(tr("OK\n"));
          }
      }

      delete myProcess;
    }

    #endif
*/

    // load settings
    loadSettings(NULL);

    // start continuous display of global settings
    updateTemperatures();

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
    AndorSettingsDialog* dlg=new AndorSettingsDialog(camera, parent);
    QString headModel="";
    if (camInfos.contains(camera)) {
        headModel=" ["+camInfos[camera].headModel+"]";
    }
    dlg->setWindowTitle(tr("Andor Camera #%1%2 Settings").arg(camera).arg(headModel));
    dlg->setupWidgets();
    dlg->readSettings(settings, "cam_andor/");
    dlg->setInfo(getCameraInfo(camera, false, false, true, false));
    if ( dlg->exec()==QDialog::Accepted ) {
         dlg->writeSettings(settings, "cam_andor/");
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


	//log_text(tr("acquiring image w=%1, h=%2\n").arg(info.width).arg(info.height));
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
    QApplication::processEvents();

    //at_32 *data = NULL;

    //Initialise and setup defaults
    char path[2048];

    if (selectCamera(camera)) {
        // ensure that global camera settings are loaded (usually done in initExtension), if they are not present,
        // try to load them from global INI file.
        if (!camGlobalSettings.contains(camera)) {
            QSettings inifile(GLOBAL_INI, QSettings::IniFormat);
            CameraGlobalSettings global;
            global.readSettings(inifile, camera);
            camGlobalSettings[camera]=global;
        }

        // initialize camera, if init fails, call ShutDown and the Initialize again ... if this also fails -> exit with an error!
        strcpy(path, detectorsIniPath.toStdString().c_str());
        progress.setLabelText(tr("initializing camera %1 ...<br>&nbsp;&nbsp;&nbsp;&nbsp;detectorsIniPath='%2'").arg(camera).arg(detectorsIniPath));
        QApplication::processEvents();
        { unsigned int error1=Initialize(path);
            if (error1!=DRV_SUCCESS) {
                log_warning(tr("Error during Initialize\n%3    error code was: %1 [%2]\n%3    instruction was: %4\n").arg(error1).arg(andorErrorToString(error1)).arg(LOG_PREFIX).arg("Initialize(path)"));
                log_warning(tr(" trying shutdown() + initialize cycle").arg(LOG_PREFIX));
                CHECK(ShutDown(), tr("Error during Shutdown"));
                CHECK(Initialize(path), tr("Error during Initialize"));
            }
        }



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
            camConnected.insert(camera);
            readCameraProperties(camera, info);
            log_text(tr("connected to Andor Camera #%1, width=%2, height=%3\n").arg(camera).arg(info.width).arg(info.height));
            log_text(tr("    width=%1, height=%2\n").arg(info.width).arg(info.height));
            log_text(tr("    head model = %1\n").arg(info.headModel));
            log_text(tr("    serial number = %1\n").arg(info.serialNumber));
            log_text(tr("    controller card = %1\n").arg(info.controllerCard));

            camInfos[camera]=info;
            if (!setGlobalSettings(camera)) {
                log_error(tr("error setting global settings")); \
                return false;
            }
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

        QModernProgressDialog progress(tr("\"Heating\" Andor Camera #%1").arg(camera+1), "", NULL);
        progress.setWindowModality(Qt::ApplicationModal);
        progress.setHasCancel(false);
        progress.open();
        while (!tempOK) {
            tempOK=true;
            int temp1=getTemperature(i);
            //progress.setValue(33);
            progress.setLabelText(tr("cameras #%2: temperature = %1 °C ...\n").arg(temp1).arg(i+1));
            //std::cout<<tr("%2 cameras #%3: cooler off, temperature = %1 °C ...\n").arg(temp1).arg(LOG_PREFIX).arg(i+1).toStdString()<<std::endl;
            if ((temp1<5)&&(temp1!=-999)) tempOK=false;
            QApplication::processEvents();
            //progress.repaint();
            //usleep(100000);

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
    if ((iSelectedCamera < (long)getCameraCount()) && (iSelectedCamera >= 0) ) {
        at_32 lCameraHandle;
        GetCameraHandle(iSelectedCamera, &lCameraHandle);
        SetCurrentCamera(lCameraHandle);
        return true;
    } else  {
        return false;
    }
}


QString QFExtensionCameraAndor::getCameraInfo(int camera, bool showHeadModel, bool showSensorSize, bool extendedInfo, bool currentSettings) {
    int i=camera;
    if (camInfos.contains(i)) {
        QString s="";
        if (showHeadModel) s+=tr("<i>head:</i> %1<br>").arg(camInfos[i].headModel);
        s+=tr("<i>serial no:</i> %1<br>").arg(camInfos[i].serialNumber);
        if (showSensorSize) s+=tr("<i>size:</i> %1&times;%2<br>").arg(camInfos[i].width).arg(camInfos[i].height);
        if (currentSettings) {
            s+=tr("<i>preamp gain:</i> %1&times;<br>").arg(camInfos[i].preampGainF);
            s+=tr("<i>bit depth:</i> %1<br>").arg(camInfos[i].bitDepth);
            s+=tr("<i>readout time:</i> %1 &mu;s<br>").arg(camInfos[i].readoutTime*1.0e6);
            s+=tr("<i>vertical speed:</i> %1 &mu;s/pixel<br>").arg(camInfos[i].verticalSpeed);
            s+=tr("<i>horizontal speed:</i> %1 MHz<br>").arg(camInfos[i].horizontalSpeed);
        }
        if (extendedInfo) {
            s+=tr("<i>pixel size:</i> %1 &times; %2 &mu;m<sup>2</sup><br>").arg(camInfos[i].pixelWidth).arg(camInfos[i].pixelHeight);
            s+=tr("<i>controller card:</i> %1<br>").arg(camInfos[i].controllerCard);
            s+=tr("<i>Andor SDK ver.:</i> %1<br>").arg(SDKVersion);
            s+=tr("<i>Andor driver ver.:</i> %1<br>").arg(deviceDriverVersion);
        }

        if (s.endsWith("<br>")) s=s.left(s.size()-QString("<br>").size());
        return s;
    } else {
        return "";
    }
}

void QFExtensionCameraAndor::readCameraProperties(int camera, QFExtensionCameraAndor::CameraInfo& info) {
    CHECK_NO_RETURN(GetDetector(&(info.width), &(info.height)), tr("error while getting detector info"));

    char text[MAX_PATH];
    CHECK_NO_RETURN(GetHeadModel(text), tr("error while retrieving head model"));
    info.headModel=text;
    CHECK_NO_RETURN(GetCameraSerialNumber(&(info.serialNumber)), tr("error while reading camera serial number"));
    CHECK_NO_RETURN(GetControllerCardModel(text), tr("error while reading controller card"));
    info.controllerCard=text;
    CHECK_NO_RETURN(GetBitDepth(info.ADchannel, &(info.bitDepth)), tr("error while reading bit depth"));
    CHECK_NO_RETURN(GetPixelSize(&(info.pixelWidth), &(info.pixelHeight)), tr("error while reading pixel size"));
    CHECK_NO_RETURN(GetReadOutTime(&(info.readoutTime)), tr("error while reading readout time"));
    CHECK_NO_RETURN(GetPreAmpGain(info.preamp_gain, &(info.preampGainF)), tr("error while reading preamplifier gain"));
    CHECK_NO_RETURN(GetVerticalSpeed(info.vsSpeed, &(info.verticalSpeed)), tr("error while reading preamplifier gain"));
    CHECK_NO_RETURN(GetHSSpeed(info.ADchannel, info.outputAmplifier, info.hsSpeed, &(info.horizontalSpeed)), tr("error while reading horicontal shift speed"));

}

bool QFExtensionCameraAndor::setCameraSettings(int camera, QFExtensionCameraAndor::CameraInfo& info) {

    if (selectCamera(camera)) {


        CHECK(SetTriggerMode(info.trigMode), tr("error while setting trigger mode"));
        CHECK(SetAcquisitionMode(info.AcqMode), tr("error while setting acquisition mode"));
        CHECK(SetReadMode(info.ReadMode), tr("error while setting read mode"));
        CHECK(SetImageRotate(0), tr("error while switching image rotation off"));
        CHECK(SetImageFlip(0,0), tr("error while switching image flipping off"));
        CHECK(SetEMAdvanced((info.advancedEMGain)?1:0), tr("error while setting advanced EM gain mode"));
        CHECK(SetEMCCDGain(info.emgain), tr("error while setting EM gain"));
        CHECK(SetOutputAmplifier(info.outputAmplifier), tr("error while setting output amplifier"));
        CHECK(SetFrameTransferMode((info.frameTransfer)?1:0), tr("error while setting frame transfer mode"));
        CHECK(SetADChannel(info.ADchannel), tr("while setting AD converter channel"));
        CHECK(SetPreAmpGain(info.preamp_gain), tr("while setting pre-amplifier gain"));
        CHECK(SetHSSpeed(info.outputAmplifier, info.hsSpeed), tr("error while setting horicontal shift speed"));
        CHECK(SetImage(info.hbin,info.vbin,info.subImage.left(), info.subImage.right(), info.subImage.top(), info.subImage.bottom()), tr("error while settings image size"));
        CHECK(SetIsolatedCropMode((info.cropMode)?1:0, info.subImage.height(), info.subImage.width(), info.vbin, info.hbin), tr("error while settings isolated crop mode"));
        CHECK(SetExposureTime(info.expoTime), tr("error while setting exposure time"));
        CHECK(SetAccumulationCycleTime(info.accTime), tr("error while settings accumulation cycle time"));
        CHECK(SetNumberAccumulations(info.numAccs), tr("error while setting number of accumulations"));
        CHECK(SetKineticCycleTime(info.kinTime), tr("error while setting kinetic cycle time"));
        CHECK(SetNumberKinetics(info.numKins), tr("error while setting number of kinetic cycles"));
        CHECK(SetBaselineOffset(info.baselineOffset), tr("error while setting baseline offset"));
        CHECK(SetBaselineClamp((info.baselineClamp)?1:0), tr("error while setting baseline clamp mode"));


        return true;
    }

    return false;
}

bool QFExtensionCameraAndor::setShutter(int camera, int mode, int closingtime, int openingtime) {
    if (selectCamera(camera)) {
        /* SetShutter(
            typ = 0 (TTL low opens shutter)
            mode = 0 (Auto)
            closingtime = 50
            openingtime = 50
        */
        CHECK(SetShutter(1,mode,closingtime, openingtime), tr("error while setting shutter"));
        return true;
    } else {
        return false;
    }
}

bool QFExtensionCameraAndor::setTemperature(int camera, bool coolerOn, int temperature, int fanMode) {
    if (selectCamera(camera)) {
        CHECK(SetFanMode(fanMode), tr("error setting fan mode"));
        CHECK(SetTemperature(temperature), tr("error setting temperature"));
        if (coolerOn) {
            CHECK(CoolerON(), tr("error while switching cooler on"));
        } else {
            CHECK(CoolerOFF(), tr("error while switching cooler off"));
        }
        return true;
    } else {
        return false;
    }
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

void QFExtensionCameraAndor::storeGlobalSettings() {
    QSettings inifile(GLOBAL_INI, QSettings::IniFormat);
    QMapIterator<int, CameraGlobalSettings> it(camGlobalSettings);
    while (it.hasNext()) {
        it.next();
        it.value().writeSettings(inifile, it.key());
    }
}


bool QFExtensionCameraAndor::setGlobalSettings(int cam) {
    int camera=cam;
    bool ok=true;
    if (cam<0) {
        QMapIterator<int, CameraGlobalSettings> it(camGlobalSettings);
        while (it.hasNext()) {
            it.next();
            camera=it.key();
            if (isConnected(camera)) {
                ok=ok&&setTemperature(camera, it.value().coolerOn, it.value().setTemperature, it.value().fanMode);
                ok=ok&&setShutter(camera, it.value().shutterMode, it.value().shutterClosingTime, it.value().shutterOpeningTime);
            }
        }
    } else {
        if (camGlobalSettings.contains(camera) && isConnected(camera)) {
            ok=setTemperature(camera, camGlobalSettings[camera].coolerOn, camGlobalSettings[camera].setTemperature, camGlobalSettings[camera].fanMode);
            ok=ok&&setShutter(camera, camGlobalSettings[camera].shutterMode, camGlobalSettings[camera].shutterClosingTime, camGlobalSettings[camera].shutterOpeningTime);
        }
    }
    return ok;
}

QString QFExtensionCameraAndor::andorErrorToString(unsigned int error) {
    switch (error) {
        case DRV_SUCCESS: return tr("success"); break;
        case DRV_ERROR_CODES : return tr("DRV_ERROR_CODES"); break;
        case DRV_VXDNOTINSTALLED : return tr("VxD not installed"); break;
        case DRV_ERROR_SCAN : return tr("DRV_ERROR_SCAN"); break;
        case DRV_ERROR_CHECK_SUM : return tr("DRV_ERROR_CHECK_SUM"); break;
        case DRV_ERROR_FILELOAD : return tr("DRV_ERROR_FILELOAD"); break;
        case DRV_UNKNOWN_FUNCTION : return tr("unknown function"); break;
        case DRV_ERROR_VXD_INIT : return tr("DRV_ERROR_VXD_INIT"); break;
        case DRV_ERROR_ADDRESS : return tr("DRV_ERROR_ADDRESS"); break;
        case DRV_ERROR_PAGELOCK : return tr("DRV_ERROR_PAGELOCK"); break;
        case DRV_ERROR_PAGEUNLOCK : return tr("DRV_ERROR_PAGEUNLOCK"); break;
        case DRV_ERROR_BOARDTEST : return tr("DRV_ERROR_BOARDTEST"); break;
        case DRV_ERROR_ACK : return tr("DRV_ERROR_ACK"); break;
        case DRV_ERROR_UP_FIFO : return tr("DRV_ERROR_UP_FIFO"); break;
        case DRV_ERROR_PATTERN : return tr("DRV_ERROR_PATTERN"); break;

        case DRV_ACQUISITION_ERRORS : return tr("RV_ACQUISITION_ERRORS"); break;
        case DRV_ACQ_BUFFER : return tr("DRV_ACQ_BUFFER"); break;
        case DRV_ACQ_DOWNFIFO_FULL : return tr("DRV_ACQ_DOWNFIFO_FULL"); break;
        case DRV_PROC_UNKONWN_INSTRUCTION : return tr("DRV_PROC_UNKONWN_INSTRUCTION"); break;
        case DRV_ILLEGAL_OP_CODE : return tr("illegal opcode"); break;
        case DRV_KINETIC_TIME_NOT_MET : return tr("kinetic time not met"); break;
        case DRV_ACCUM_TIME_NOT_MET : return tr("accumulation time not met"); break;
        case DRV_NO_NEW_DATA : return tr("no new data"); break;
        case DRV_SPOOLERROR : return tr("DRV_SPOOLERROR"); break;
        case DRV_SPOOLSETUPERROR : return tr("DRV_SPOOLSETUPERROR"); break;
        case DRV_FILESIZELIMITERROR : return tr("DRV_FILESIZELIMITERROR"); break;
        case DRV_ERROR_FILESAVE : return tr("DRV_ERROR_FILESAVE"); break;

        case DRV_TEMPERATURE_CODES : return tr("DRV_TEMPERATURE_CODES"); break;
        case DRV_TEMPERATURE_OFF : return tr("DRV_TEMPERATURE_OFF"); break;
        case DRV_TEMPERATURE_NOT_STABILIZED : return tr("temperature not stabilized"); break;
        case DRV_TEMPERATURE_STABILIZED : return tr("temperature stabilized"); break;
        case DRV_TEMPERATURE_NOT_REACHED : return tr("temperature not reached"); break;
        case DRV_TEMPERATURE_OUT_RANGE : return tr("temperature out of range"); break;
        case DRV_TEMPERATURE_NOT_SUPPORTED : return tr("temperature not supported"); break;
        case DRV_TEMPERATURE_DRIFT : return tr("DRV_TEMPERATURE_DRIFT"); break;


        case DRV_GENERAL_ERRORS : return tr("general error"); break;
        case DRV_INVALID_AUX : return tr("DRV_INVALID_AUX"); break;
        case DRV_COF_NOTLOADED : return tr("DRV_COF_NOTLOADED"); break;
        case DRV_FPGAPROG : return tr("DRV_FPGAPROG"); break;
        case DRV_FLEXERROR : return tr("DRV_FLEXERROR"); break;
        case DRV_GPIBERROR : return tr("GPIB error"); break;
        case DRV_EEPROMVERSIONERROR : return tr("DRV_EEPROMVERSIONERROR"); break;

        case DRV_DATATYPE : return tr("DRV_DATATYPE"); break;
        case DRV_DRIVER_ERRORS : return tr("DRV_DRIVER_ERRORS"); break;
        case DRV_P1INVALID : return tr("parameter 1 invalid"); break;
        case DRV_P2INVALID : return tr("parameter 2 invalid"); break;
        case DRV_P3INVALID : return tr("parameter 3 invalid"); break;
        case DRV_P4INVALID : return tr("parameter 4 invalid"); break;
        case DRV_INIERROR : return tr("DRV_INIERROR"); break;
        case DRV_COFERROR : return tr("DRV_COFERROR"); break;
        case DRV_ACQUIRING : return tr("DRV_ACQUIRING"); break;
        case DRV_IDLE : return tr("DRV_IDLE"); break;
        case DRV_TEMPCYCLE : return tr("DRV_TEMPCYCLE"); break;
        case DRV_NOT_INITIALIZED : return tr("system not initialized"); break;
        case DRV_P5INVALID : return tr("parameter 5 invalid"); break;
        case DRV_P6INVALID : return tr("parameter 6 invalid"); break;
        case DRV_INVALID_MODE : return tr("invalid mode"); break;
        case DRV_INVALID_FILTER : return tr("invalid filter"); break;

        case DRV_I2CERRORS : return tr("DRV_I2CERRORS"); break;
        case DRV_I2CDEVNOTFOUND : return tr("DRV_I2CDEVNOTFOUND"); break;
        case DRV_I2CTIMEOUT : return tr("DRV_I2CTIMEOUT"); break;
        case DRV_P7INVALID : return tr("DRV_P7INVALID"); break;
        case DRV_P8INVALID : return tr("DRV_P8INVALID"); break;
        case DRV_P9INVALID : return tr("DRV_P9INVALID"); break;
        case DRV_P10INVALID : return tr("DRV_P10INVALID"); break;


        case DRV_USBERROR : return tr("DRV_USBERROR"); break;
        case DRV_IOCERROR : return tr("DRV_IOCERROR"); break;
        case DRV_VRMVERSIONERROR : return tr("DRV_VRMVERSIONERROR"); break;
        case DRV_USB_INTERRUPT_ENDPOINT_ERROR : return tr("DRV_USB_INTERRUPT_ENDPOINT_ERROR"); break;
        case DRV_RANDOM_TRACK_ERROR : return tr("DRV_RANDOM_TRACK_ERROR"); break;
        case DRV_INVALID_TRIGGER_MODE : return tr("DRV_INVALID_TRIGGER_MODE"); break;
        case DRV_LOAD_FIRMWARE_ERROR : return tr("DRV_LOAD_FIRMWARE_ERROR"); break;
        case DRV_DIVIDE_BY_ZERO_ERROR : return tr("DRV_DIVIDE_BY_ZERO_ERROR"); break;
        case DRV_INVALID_RINGEXPOSURES : return tr("DRV_INVALID_RINGEXPOSURES"); break;
        case DRV_BINNING_ERROR : return tr("DRV_BINNING_ERROR"); break;
        case DRV_INVALID_AMPLIFIER : return tr("DRV_INVALID_AMPLIFIER"); break;

        case DRV_ERROR_NOCAMERA: return tr("no camera"); break;
        case DRV_NOT_SUPPORTED: return tr("not supported"); break;
        case DRV_NOT_AVAILABLE: return tr("feature not available"); break;

        case DRV_ERROR_MAP: return tr("DRV_ERROR_MAP"); break;
        case DRV_ERROR_UNMAP: return tr("DRV_ERROR_UNMAP"); break;
        case DRV_ERROR_MDL: return tr("DRV_ERROR_MDL"); break;
        case DRV_ERROR_UNMDL: return tr("DRV_ERROR_UNMDL"); break;
        case DRV_ERROR_BUFFSIZE: return tr("DRV_ERROR_BUFFSIZE"); break;
        case DRV_ERROR_NOHANDLE: return tr("DRV_ERROR_NOHANDLE"); break;

        case DRV_GATING_NOT_AVAILABLE: return tr("gating not available"); break;
        case DRV_FPGA_VOLTAGE_ERROR: return tr("FPGA voltage error"); break;

        case DRV_OW_CMD_FAIL: return tr("DRV_OW_CMD_FAIL"); break;
        case DRV_OWMEMORY_BAD_ADDR: return tr("DRV_OWMEMORY_BAD_ADDR"); break;
        case DRV_OWCMD_NOT_AVAILABLE: return tr("DRV_OWCMD_NOT_AVAILABLE"); break;
        case DRV_OW_NO_SLAVES: return tr("DRV_OW_NO_SLAVES"); break;
        case DRV_OW_NOT_INITIALIZED: return tr("DRV_OW_NOT_INITIALIZED"); break;
        case DRV_OW_ERROR_SLAVE_NUM: return tr("DRV_OW_ERROR_SLAVE_NUM"); break;
        case DRV_MSTIMINGS_ERROR: return tr("DRV_MSTIMINGS_ERROR"); break;

        case DRV_OA_NULL_ERROR: return tr("DRV_OA_NULL_ERROR"); break;
        case DRV_OA_PARSE_DTD_ERROR: return tr("DRV_OA_PARSE_DTD_ERROR"); break;
        case DRV_OA_DTD_VALIDATE_ERROR: return tr("DRV_OA_DTD_VALIDATE_ERROR"); break;
        case DRV_OA_FILE_ACCESS_ERROR: return tr("DRV_OA_FILE_ACCESS_ERROR"); break;
        case DRV_OA_FILE_DOES_NOT_EXIST: return tr("DRV_OA_FILE_DOES_NOT_EXIST"); break;
        case DRV_OA_XML_INVALID_OR_NOT_FOUND_ERROR: return tr("DRV_OA_XML_INVALID_OR_NOT_FOUND_ERROR"); break;
        case DRV_OA_PRESET_FILE_NOT_LOADED: return tr("DRV_OA_PRESET_FILE_NOT_LOADED"); break;
        case DRV_OA_USER_FILE_NOT_LOADED: return tr("DRV_OA_USER_FILE_NOT_LOADED"); break;
        case DRV_OA_PRESET_AND_USER_FILE_NOT_LOADED: return tr("DRV_OA_PRESET_AND_USER_FILE_NOT_LOADED"); break;
        case DRV_OA_INVALID_FILE: return tr("DRV_OA_INVALID_FILE"); break;
        case DRV_OA_FILE_HAS_BEEN_MODIFIED: return tr("DRV_OA_FILE_HAS_BEEN_MODIFIED"); break;
        case DRV_OA_BUFFER_FULL: return tr("DRV_OA_BUFFER_FULL"); break;
        case DRV_OA_INVALID_STRING_LENGTH: return tr("DRV_OA_INVALID_STRING_LENGTH"); break;
        case DRV_OA_INVALID_CHARS_IN_NAME: return tr("DRV_OA_INVALID_CHARS_IN_NAME"); break;
        case DRV_OA_INVALID_NAMING: return tr("DRV_OA_INVALID_NAMING"); break;
        case DRV_OA_MODE_ALREADY_EXISTS: return tr("DRV_OA_MODE_ALREADY_EXISTS"); break;
        case DRV_OA_STRINGS_NOT_EQUAL: return tr("DRV_OA_STRINGS_NOT_EQUAL"); break;
        case DRV_OA_NO_USER_DATA: return tr("DRV_OA_NO_USER_DATA"); break;
        case DRV_OA_VALUE_NOT_SUPPORTED: return tr("DRV_OA_VALUE_NOT_SUPPORTED"); break;
        case DRV_OA_MODE_DOES_NOT_EXIST: return tr("DRV_OA_MODE_DOES_NOT_EXIST"); break;

    #ifdef __LINUX__
        case DRV_PROCESSING_FAILED: return tr("processing failed"); break;
        case DRV_OA_FAILED_TO_GET_MODE: return tr("DRV_OA_FAILED_TO_GET_MODE"); break;
        case DRV_OA_CAMERA_NOT_SUPPORTED: return tr("DRV_OA_CAMERA_NOT_SUPPORTED"); break;
        case DRV_OA_GET_CAMERA_ERROR: return tr("DRV_OA_GET_CAMERA_ERROR"); break;
        case KERN_MEM_ERROR : return tr("KERN_MEM_ERROR"); break;
        case DRV_P11INVALID : return tr("DRV_P11INVALID"); break;
        case DRV_INVALID_COUNTCONVERT_MODE: return tr("DRV_INVALID_COUNTCONVERT_MODE"); break;
    #endif

        default: return tr(""); break;
    }
    return "";
}


void QFExtensionCameraAndor::updateTemperatures() {
    for (unsigned int i=0; i<getCameraCount(); i++) {
        AndorGlobalCameraSettingsWidget* widget=camGlobalSettingsWidgets.value(i, NULL);
        if (!widget) {
            widget=new AndorGlobalCameraSettingsWidget(i, dlgGlobalSettings);
            camGlobalSettingsWidgets[i]=widget;
            dlgGlobalSettings_layout->addWidget(widget);
            connect(widget, SIGNAL(settingsChanged(int,int,bool,int,int)), this, SLOT(globalSettingsChanged(int,int,bool,int,int)));
        }

        if (isConnected(i)) {
            if (widget) {
                widget->setSettings(camGlobalSettings[i].fanMode, camGlobalSettings[i].coolerOn, camGlobalSettings[i].setTemperature, camGlobalSettings[i].shutterMode);
                widget->setVisible(true);
                widget->setInfo(getCameraInfo(i, true, true, true, true));
                if (selectCamera(i)) {
                    int min, max;
                    if (GetTemperatureRange(&min, &max)==DRV_SUCCESS) {
                        widget->setRange(min, max);
                    }

                    float temp;
                    unsigned int status=GetTemperatureF(&temp);
                    int progress=0;
                    bool updateTemp=false;
                    switch(status) {
                        case DRV_NOT_INITIALIZED: progress=-1; updateTemp=true; break;
                        case DRV_ERROR_ACK: progress=-1; updateTemp=true; break;
                        case DRV_TEMP_OFF: progress=-2; updateTemp=true; break;
                        case DRV_TEMP_STABILIZED: progress=0; updateTemp=true; break;
                        case DRV_TEMP_NOT_REACHED: progress=1; updateTemp=true; break;
                        case DRV_TEMP_DRIFT: progress=3; updateTemp=true; break;
                        case DRV_TEMP_NOT_STABILIZED: progress=2; updateTemp=true; break;

                        default: break;
                    }

                    if (updateTemp) widget->showCurrentTemperature(progress, temp);
                } else {
                    widget->showCurrentTemperature(-1, 0);
                }
            }
        } else {
            if (widget) widget->setVisible(false);
        }
    }

    QTimer::singleShot(250, this, SLOT(updateTemperatures()));
}

void QFExtensionCameraAndor::globalSettingsChanged(int camera, int fan_mode, bool cooling_on, int temperature, int shutterMode) {
    if (!camGlobalSettings.contains(camera)) {
        QSettings inifile(GLOBAL_INI, QSettings::IniFormat);
        CameraGlobalSettings global;
        global.readSettings(inifile, camera);
        camGlobalSettings[camera]=global;
    }

    camGlobalSettings[camera].coolerOn=cooling_on;
    camGlobalSettings[camera].fanMode=fan_mode;
    camGlobalSettings[camera].setTemperature=temperature;
    camGlobalSettings[camera].shutterMode=shutterMode;
    setGlobalSettings(camera);
    storeGlobalSettings();
}


Q_EXPORT_PLUGIN2(cam_andor, QFExtensionCameraAndor)



