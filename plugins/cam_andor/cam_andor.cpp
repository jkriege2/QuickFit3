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
        qDebug()<<QString(msg)+tr("\n%3    error code was: %1 [%2]\n%3    instruction was: %4").arg(error).arg(andorErrorToString(error)).arg(LOG_PREFIX).arg(#s); \
        log_error(QString(msg)+tr("\n%3    error code was: %1 [%2]\n%3    instruction was: %4").arg(error).arg(andorErrorToString(error)).arg(LOG_PREFIX).arg(#s)); \
        return false; \
    } \
}

#define CHECK_NO_RETURN(s, msg) \
{ \
    unsigned int error=s; \
    if (error!=DRV_SUCCESS) { \
        qDebug()<<QString(msg)+tr("\n%3    error code was: %1 [%2]\n%3    instruction was: %4").arg(error).arg(andorErrorToString(error)).arg(LOG_PREFIX).arg(#s); \
        log_warning(QString(msg)+tr("\n%3    error code was: %1 [%2]\n%3    instruction was: %4").arg(error).arg(andorErrorToString(error)).arg(LOG_PREFIX).arg(#s)); \
    } \
}

#define CHECK_ON_ERROR(s, msg, onError) \
{ \
    unsigned int error=s; \
    if (error!=DRV_SUCCESS) { \
        qDebug()<<QString(msg)+tr("\n%3    error code was: %1 [%2]\n%3    instruction was: %4").arg(error).arg(andorErrorToString(error)).arg(LOG_PREFIX).arg(#s); \
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
    width=128;
    height=128;


    AcqMode=1;
    ReadMode=4;
    expoTime=0.1;
    trigMode=0;
    trigInvert=false;
    numKins=2;
    numAccs=1;
    kinTime=0;
    accTime=0;

    subImage_hstart=1;
    subImage_hend=128;
    subImage_vstart=1;
    subImage_vend=128;

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
    horizontalSpeed=0;
    readoutTime=0;
    fileformat=0;

    acquisitionFilenamePrefix="";

    spooling=false;

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

    storeGlobalSettings();
}

unsigned int QFExtensionCameraAndor::getCameraCount() {
    at_32 lNumCameras;
    GetAvailableCameras(&lNumCameras);
    return lNumCameras;
}

void QFExtensionCameraAndor::showCameraSettingsDialog(unsigned int camera, QSettings& settings, QWidget* parent) {
	/* open a dialog that configures the camera.

	   usually you should display a modal QDialog descendent which writes back config when the user clicks OK

	   alternatively you may also display a window which stays open and allows the suer to set settings also
	   during the measurement.
	*/
    if (selectCamera(camera)) {
        AndorSettingsDialog* dlg=new AndorSettingsDialog(camera, parent);
        QString headModel="";
        CameraInfo info;
        if (camInfos.contains(camera)) {
            headModel=" ["+camInfos[camera].headModel+"]";
            info=camInfos[camera];
        }
        dlg->setWindowTitle(tr("Andor Camera #%1%2 Settings").arg(camera).arg(headModel));
        dlg->setupWidgets();
        dlg->readSettings(settings);
        dlg->setInfo(getCameraInfo(camera, false, false, true, false));

        uint32_t* data=(uint32_t*)calloc(info.width*info.height, sizeof(uint32_t));
        useCameraSettings(camera, settings);
        acquireFullFrame(camera, data);
        dlg->setImage(data);
        int oldShutterMode=camGlobalSettings[camera].shutterMode;
        camGlobalSettings[camera].shutterMode=2; // close shutter
        setGlobalSettings(camera);
        if ( dlg->exec()==QDialog::Accepted ) {
             dlg->writeSettings(settings);
        }
        camGlobalSettings[camera].shutterMode=oldShutterMode; // reset shutter mode
        setGlobalSettings(camera);

        free(data);
        delete dlg;
    }
}


void QFExtensionCameraAndor::setSettingsFromQSettings(QFExtensionCameraAndor::CameraInfo& info, const QSettings& settings) {
    QString prefix="cam_andor/";
    int readMode=settings.value(prefix+"read_mode", 0).toInt();
    info.ReadMode=4;
    if (readMode==1) info.ReadMode=0;

    info.trigMode=settings.value(prefix+"trigger_mode", info.trigMode).toInt();
    info.AcqMode=settings.value(prefix+"acquisition_mode", info.AcqMode).toInt();
    info.fileformat=settings.value(prefix+"fileformat", info.fileformat).toInt();
    info.expoTime=settings.value(prefix+"exposure_time", info.expoTime*1e6).toDouble()/1e6;
    info.kinTime=settings.value(prefix+"kinetic_cycle_time", info.kinTime*1e6).toDouble()/1e6;
    info.numKins=settings.value(prefix+"kinetic_cycles", info.numKins).toInt();
    info.numAccs=settings.value(prefix+"accumulate_cycles", info.numAccs).toInt();
    info.accTime=settings.value(prefix+"accumulate_cycles_time", info.accTime*1e6).toDouble()/1e6;
    info.frameTransfer=settings.value(prefix+"frame_transfer", info.frameTransfer).toBool();
    info.baselineClamp=settings.value(prefix+"baseline_clamp", info.baselineClamp).toBool();
    info.baselineOffset=settings.value(prefix+"baseline_offset", info.baselineOffset).toInt();
    info.emgain=settings.value(prefix+"emgain", info.emgain).toInt();
    info.preamp_gain=settings.value(prefix+"preamp_gain", info.preamp_gain).toInt();
    info.vsSpeed=settings.value(prefix+"vertical_shift_speed", info.vsSpeed).toInt();
    info.vsAmplitude=settings.value(prefix+"vertical_shift_amplitude", info.vsAmplitude).toInt();
    info.hsSpeed=settings.value(prefix+"horizontal_shift_speed", info.hsSpeed).toInt();
    info.subImage_hstart=settings.value(prefix+"subimage_hstart", 1).toInt();
    info.subImage_hend=settings.value(prefix+"subimage_hend", info.width).toInt();
    info.subImage_vstart=settings.value(prefix+"subimage_vstart", 1).toInt();
    info.subImage_vend=settings.value(prefix+"subimage_vend", info.height).toInt();
    if (info.subImage_vstart>info.subImage_vend) qSwap(info.subImage_vstart, info.subImage_vend);
    if (info.subImage_hstart>info.subImage_hend) qSwap(info.subImage_hstart, info.subImage_hend);

    info.hbin=settings.value(prefix+"horizontal_binning", info.hbin).toInt();
    info.vbin=settings.value(prefix+"vertical_binning", info.vbin).toInt();
    info.outputAmplifier=settings.value(prefix+"amplifier", info.outputAmplifier).toInt();
    info.ADchannel=settings.value(prefix+"ad_channel", info.ADchannel).toInt();
    info.advancedEMGain=settings.value(prefix+"advanced_emgain", info.advancedEMGain).toBool();

    info.trigMode=settings.value(prefix+"trigger_mode", info.trigMode).toInt();
    info.trigInvert=settings.value(prefix+"trigger_invert", info.trigInvert).toInt();
}


void QFExtensionCameraAndor::useCameraSettings(unsigned int camera, const QSettings& settings) {
    CameraInfo info;//=camInfos[camera];
    readCameraProperties(camera, info);

    setSettingsFromQSettings(info, settings);

    // simplifications for preview mode
    info.AcqMode=3; // single scan mode for preview
    info.numKins=1;

    if (info.subImage_vend>info.height) info.subImage_vend=info.height;
    if (info.subImage_hend>info.width) info.subImage_hend=info.width;
    if (info.subImage_vstart<1) info.subImage_vstart=1;
    if (info.subImage_hstart<1) info.subImage_hstart=1;

    setCameraSettings(camera, info);
    readCameraProperties(camera, info);
    camInfos[camera]=info;

}




int QFExtensionCameraAndor::getImageWidth(unsigned int camera) {
    if (!isConnected(camera)) return 0;
    return (abs(camInfos[camera].subImage_hend-camInfos[camera].subImage_hstart)+1)/camInfos[camera].hbin;
}

int QFExtensionCameraAndor::getImageHeight(unsigned int camera) {
    if (!isConnected(camera)) return 0;
    return (abs(camInfos[camera].subImage_vend-camInfos[camera].subImage_vstart)+1)/camInfos[camera].vbin;
}

double QFExtensionCameraAndor::getPixelWidth(unsigned int camera) {
    if (!isConnected(camera)) return 1;
    return camInfos[camera].pixelWidth*(double)(camInfos[camera].hbin);
}

double QFExtensionCameraAndor::getPixelHeight(unsigned int camera) {
    if (!isConnected(camera)) return 1;
    return camInfos[camera].pixelHeight*(double)(camInfos[camera].vbin);
}


bool QFExtensionCameraAndor::isConnected(unsigned int camera) {
    return camConnected.contains(camera);
}

bool QFExtensionCameraAndor::acquire(unsigned int camera, uint32_t* data, uint64_t* timestamp) {
    if (!isConnected(camera)) return false;

    if (!selectCamera(camera)) return false;

    CHECK(SetAcquisitionMode(1), tr("error while setting \"single scan\" acquisition mode"));
    CHECK(StartAcquisition(), tr("error starting acquisition"));

    CameraInfo info;
    if (camInfos.contains(camera)) info=camInfos[camera];

    int status;

	//Loop until acquisition finished
	CHECK(GetStatus(&status), tr("error while waiting for frame"));
	QTime time;
	time.start();
	double timeout=(info.expoTime*10.0+0.1)*1000.0;
	while ((status==DRV_ACQUIRING)&&(time.elapsed()<=timeout)) {
		CHECK(GetStatus(&status), tr("error while waiting for frame"));
	}
	if (time.elapsed()<=timeout)  {


        int imagesize=getImageWidth(camera)*getImageHeight(camera);
        at_32* imageData = (at_32*)malloc(imagesize*sizeof(at_32));
        CHECK_ON_ERROR(GetAcquiredData(imageData,imagesize), tr("error while acquiring frame"), free(imageData));

        for(int i=0;i<imagesize;i++) data[i]=imageData[i];

        free(imageData);

		return true;
	} else {
		log_error(tr("\n%1    error: acquiring image (exposure time %2 ms) timed out after %3 seconds ").arg(LOG_PREFIX).arg(info.expoTime*1000.0).arg(timeout/1000.0));
		return false;
	}


}



bool QFExtensionCameraAndor::acquireFullFrame(unsigned int camera, uint32_t* data, uint64_t* timestamp) {
    if (!isConnected(camera)) return false;

    if (!selectCamera(camera)) return false;


    CameraInfo info;
    if (camInfos.contains(camera)) info=camInfos[camera];

    CHECK(SetAcquisitionMode(1), tr("error while setting \"single scan\" acquisition mode"));
    CHECK(SetFullImage(1, 1), tr("error while settings full sensor size (no binnig) as image size"));
    CHECK(StartAcquisition(), tr("error while starting acquisition"));

    int status;
    CHECK(GetStatus(&status), tr("error while waiting for frame"));
    QTime time;

	time.start();
	double timeout=(info.expoTime*10.0+0.1)*1000.0;
	while ((status==DRV_ACQUIRING)&&(time.elapsed()<=timeout)) {
		CHECK(GetStatus(&status), tr("error while waiting for frame"));
	}
	if (time.elapsed()<=timeout)  {


        at_32* imageData = (at_32*)malloc(info.width*info.height*sizeof(at_32));
        CHECK_ON_ERROR(GetAcquiredData(imageData, info.width*info.height), tr("error while acquiring frame"), free(imageData));

        for(int i=0;i<info.width*info.height;i++) data[i]=imageData[i];
        free(imageData);

		return true;
	} else {
		log_error(tr("\n%1    error: acquiring full-frame image (exposure time %2 ms) timed out after %3 seconds ").arg(LOG_PREFIX).arg(info.expoTime*1000.0).arg(timeout/1000.0));
		return false;
	}
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
        //info.subImage=QRect(1,1,info.width, info.height);
        info.subImage_hstart=1;
        info.subImage_vstart=1;
        info.subImage_hend=info.width;
        info.subImage_vend=info.height;

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



bool QFExtensionCameraAndor::prepareAcquisition(unsigned int camera, const QSettings& settings, QString filenamePrefix) {
    useCameraSettings(camera, settings);

    CameraInfo info;//=camInfos[camera];
    readCameraProperties(camera, info);

    setSettingsFromQSettings(info, settings);

    info.acquisitionFilenamePrefix=filenamePrefix;

    if (info.subImage_vend>info.height) info.subImage_vend=info.height;
    if (info.subImage_hend>info.width) info.subImage_hend=info.width;
    if (info.subImage_vstart<1) info.subImage_vstart=1;
    if (info.subImage_hstart<1) info.subImage_hstart=1;

    setCameraSettings(camera, info);
    readCameraProperties(camera, info);
    camInfos[camera]=info;

    // check whether an acquisition thread is still alive ... if so, kill it, as it has to be erronemous
    if (camThreads.contains(camera)) {
        CamAndorAcquisitionThread* oldthread=camThreads[camera];
        if (oldthread) {
            oldthread->terminate();
            oldthread->wait();
            oldthread->deleteLater();
        }
    }


    // now create a new acquisition thread
    CamAndorAcquisitionThread* thread=new CamAndorAcquisitionThread(this);
    connect(thread, SIGNAL(log_error(QString)), this, SLOT(tlog_error(QString)));
    connect(thread, SIGNAL(log_warning(QString)), this, SLOT(tlog_warning(QString)));
    connect(thread, SIGNAL(log_text(QString)), this, SLOT(tlog_text(QString)));
    thread->init(camera, filenamePrefix, info.fileformat, info.numKins, getImageWidth(camera), getImageHeight(camera), info.expoTime, LOG_PREFIX);
    camThreads[camera]=thread;

    return true;
}


bool QFExtensionCameraAndor::startAcquisition(unsigned int camera) {
    //if (selectCamera(camera)) {
        CamAndorAcquisitionThread* thread=camThreads.value(camera, NULL);
        if (!thread) return false;
        thread->start();
        /*bool spooling=false;
        CameraInfo info=camInfos[camera];
        if (info.fileformat==1) { // TIFF (spooling)
            CHECK(SetSpool(1, 7, camInfos[camera].acquisitionFilenamePrefix.toAscii().data(), 100), tr("error while enabling spooling mode"));
            spooling = true;
        } else if (info.fileformat==2) { // Andor SIF (spooling)
            CHECK(SetSpool(1, 6, camInfos[camera].acquisitionFilenamePrefix.toAscii().data(), 100), tr("error while enabling spooling mode"));
            spooling = true;
        } else if (info.fileformat==3) { // FITS (spooling)
            CHECK(SetSpool(1, 5, camInfos[camera].acquisitionFilenamePrefix.toAscii().data(), 100), tr("error while enabling spooling mode"));
            spooling = true;
        } else if (info.fileformat==4) { // 16-bit raw
            //CHECK(SetSpool(1, 2, camInfos[camera].acquisitionFilenamePrefix.toAscii().data(), 100), tr("error while enabling spooling mode"));
        }
        if (!spooling) {

            thread->start(QThread::HighestPriority);
        }
        CHECK(StartAcquisition(), tr("error while starting acquisition"));*/

        return true;
    //} else {
    //    return false;
    //}
}

void QFExtensionCameraAndor::cancelAcquisition(unsigned int camera) {
    /*if (selectCamera(camera)) {
        CHECK_NO_RETURN(AbortAcquisition(), tr("error while aborting acquisition"));
    }*/
    CamAndorAcquisitionThread* thread=camThreads.value(camera, NULL);
    if (thread) {
        thread->cancelAcquisition();
    }
}

bool QFExtensionCameraAndor::isAcquisitionRunning(unsigned int camera, double* percentageDone) {
    /*if (selectCamera(camera)) {
        if (percentageDone) *percentageDone=getAcquisitionProgress(camera);
        int status;
        CHECK(GetStatus(&status), tr("error while waiting for frame"));
        return (status==DRV_ACQUIRING);
    } else {
        return false;
    }*/
    CamAndorAcquisitionThread* thread=camThreads.value(camera, NULL);
    if (thread) {
        return thread->isRunning();
    } else {
        return false;
    }
}

void QFExtensionCameraAndor::getAcquisitionDescription(unsigned int camera, QStringList* files, QMap<QString, QVariant>* parameters) {
}

bool QFExtensionCameraAndor::getAcquisitionPreview(unsigned int camera, uint32_t* data) {
    return false;
}

int QFExtensionCameraAndor::getAcquisitionProgress(unsigned int camera) {
    /*if (selectCamera(camera)) {
        int p=0;
        CHECK(GetSpoolProgress(&p), tr("error acquiring spool progress"));
        return 100*p/camInfos[camera].numKins;
    }  else {
        return 0;
    }*/
    CamAndorAcquisitionThread* thread=camThreads.value(camera, NULL);
    if (thread) {
        return (int)round(thread->getProgress());
    } else {
        return 0;
    }
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



QString QFExtensionCameraAndor::getCameraInfo(int camera, bool showHeadModel, bool showSensorSize, bool extendedInfo, bool currentSettings) {
    int i=camera;
    if (camInfos.contains(i)) {
        QString s="";
        if (showHeadModel) s+=tr("<i>head:</i> %1<br>").arg(camInfos[i].headModel);
        s+=tr("<i>serial no:</i> %1<br>").arg(camInfos[i].serialNumber);
        if (showSensorSize) s+=tr("<i>size:</i> %1&times;%2<br>").arg(camInfos[i].width).arg(camInfos[i].height);
        if (currentSettings) {
            s+=tr("<i>readout:</i> %1&times;, %2 MHz, %3 bit<br>").arg(camInfos[i].preampGainF).arg(camInfos[i].horizontalSpeed).arg(camInfos[i].bitDepth);
            s+=tr("<i>readout time:</i> %1 &mu;s<br>").arg(camInfos[i].readoutTime*1.0e6);
            s+=tr("<i>vertical speed:</i> %1 &mu;s/pixel<br>").arg(camInfos[i].verticalSpeed);
            //s+=tr("<i>horizontal speed:</i> <br>").arg(camInfos[i].horizontalSpeed);
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
  #ifdef MAX_PATH
    char text[MAX_PATH];
  #else
    char text[1024];
  #endif
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
        switch (info.trigMode) {
            case 1: { CHECK(SetTriggerMode(1), tr("error while setting trigger mode: external")); } break;
            case 2: { CHECK(SetTriggerMode(6), tr("error while setting trigger mode: external start")); } break;
            case 3: { CHECK(SetTriggerMode(7), tr("error while setting trigger mode: external exposure (bulb)")); } break;

            default:
            case 0: { CHECK(SetTriggerMode(0), tr("error while setting trigger mode: internal")); } break;
        }
        if (info.trigMode>0) {
            if (info.trigInvert) {
                CHECK(SetTriggerInvert(1), tr("error while setting trigger mode: falling edge"));
            } else {
                CHECK(SetTriggerInvert(0), tr("error while setting trigger mode: rising edge"));
            }
        }


        CHECK(SetAcquisitionMode(info.AcqMode), tr("error while setting acquisition mode"));
        CHECK(SetReadMode(info.ReadMode), tr("error while setting read mode"));
        CHECK(SetFrameTransferMode((info.frameTransfer)?1:0), tr("error while setting frame transfer mode"));
        CHECK(SetADChannel(info.ADchannel), tr("while setting AD converter channel"));
        CHECK(SetOutputAmplifier(info.outputAmplifier), tr("error while setting output amplifier"));
        CHECK(SetHSSpeed(info.outputAmplifier, info.hsSpeed), tr("error while setting horicontal shift speed"));
        CHECK(SetPreAmpGain(info.preamp_gain), tr("while setting pre-amplifier gain"));
        CHECK(SetVSAmplitude(info.vsAmplitude), tr("while setting pre-amplifier gain"));
        CHECK(SetVSSpeed(info.vsSpeed), tr("while setting pre-amplifier gain"));
        CHECK(SetExposureTime(info.expoTime), tr("error while setting exposure time"));
        CHECK(SetNumberAccumulations(info.numAccs), tr("error while setting number of accumulations"));
        CHECK(SetAccumulationCycleTime(info.accTime), tr("error while settings accumulation cycle time"));
        CHECK(SetNumberKinetics(info.numKins), tr("error while setting number of kinetic cycles"));
        CHECK(SetKineticCycleTime(info.kinTime), tr("error while setting kinetic cycle time"));
        if (info.cropMode) {
            CHECK(SetIsolatedCropMode(1, abs(info.subImage_vend-info.subImage_vstart)+1, abs(info.subImage_hend-info.subImage_hstart)+1, info.vbin, info.hbin), tr("error while settings isolated crop mode"));
        } else {
            CHECK(SetIsolatedCropMode(0, abs(info.subImage_vend-info.subImage_vstart)+1, abs(info.subImage_hend-info.subImage_hstart)+1, info.vbin, info.hbin), tr("error while settings isolated crop mode"));
            //qDebug()<<"setImage("<<info.hbin<<info.vbin<<info.subImage_hstart<< info.subImage_hend<< info.subImage_vstart<< info.subImage_vend;
            CHECK(SetImage(info.hbin,info.vbin,info.subImage_hstart, info.subImage_hend, info.subImage_vstart, info.subImage_vend), tr("error while settings image size"));
        }

        CHECK(SetImageRotate(0), tr("error while switching image rotation off"));
        CHECK(SetImageFlip(0,0), tr("error while switching image flipping off"));
        CHECK(SetEMGainMode(2), tr("error while setting linear EM gain mode"));
        CHECK(SetEMAdvanced((info.advancedEMGain)?1:0), tr("error while setting advanced EM gain mode"));
        CHECK(SetEMCCDGain(info.emgain), tr("error while setting EM gain"));
        CHECK(SetBaselineOffset(info.baselineOffset), tr("error while setting baseline offset"));
        CHECK(SetBaselineClamp((info.baselineClamp)?1:0), tr("error while setting baseline clamp mode"));

        // read back actual settings
        float exposure=0, accumulate=0, kinetic=0;
        CHECK(GetAcquisitionTimings(&exposure, &accumulate, &kinetic), tr("error reading acquisition timings"));
        info.expoTime=exposure;
        info.accTime=accumulate;
        info.kinTime=kinetic;

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



