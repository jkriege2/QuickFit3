/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

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

#include "cam_andor.h"
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif


#include "qfplugin.h"
#include <iostream>
#include <unistd.h>
#include <time.h>
#include <QMapIterator>
#include "qmodernprogresswidget.h"
#include "flowlayout.h"
#include <cmath>
#include "andorsettingsdialog.h"
#include <QElapsedTimer>
// if defined: enables some timing measurements
//#define DEBUG_TIMING

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

#ifndef MAX_PATH
#  define MAX_PATH 1024
#endif

#define UPDATE_TEMP_INTERVAL 1131

#define LOG_PREFIX "[Andor]:  "
#define GLOBAL_INI services->getConfigFileDirectory()+QString("/cam_andor.ini")


//QElapsedTimer __timer;
//__timer.start();
//qDebug()<<"ANDOR_INSTRUCTION  \""<<#s<<"\":  duration: "<<__timer.nsecsElapsed()/1e6<<" msecs";

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
    cooling_wait=true;
    lastShutterAction=QTime::currentTime();
}


void QFExtensionCameraAndor::CameraGlobalSettings::readSettings(QSettings& settings, int camera) {
    settings.sync();
    coolerOn=settings.value(QString("camera%1/cooler_on").arg(camera), coolerOn).toBool();
    cooling_wait=settings.value(QString("camera%1/cooling_wait").arg(camera), cooling_wait).toBool();
    setTemperature=settings.value(QString("camera%1/set_temperature").arg(camera), setTemperature).toInt();
    fanMode=settings.value(QString("camera%1/fan_mode").arg(camera), fanMode).toInt();
}

void QFExtensionCameraAndor::CameraGlobalSettings::writeSettings(QSettings& settings, int camera) const {
    settings.setValue(QString("camera%1/cooler_on").arg(camera), coolerOn);
    settings.setValue(QString("camera%1/cooling_wait").arg(camera), cooling_wait);
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
    sensitivity=0;
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

    Q_UNUSED(project);
    Q_UNUSED(oldProject);
}

void QFExtensionCameraAndor::initExtension() {
    services->log_global_text(tr("%2initializing extension '%1' ...\n").arg(getName()).arg(LOG_PREFIX));


    QString pd=QFPluginServices::getInstance()->getPluginConfigDirectory(getID());
    QDir(QFPluginServices::getInstance()->getConfigFileDirectory()).mkpath(pd);
    QDir d(pd);
    QDir a(QFPluginServices::getInstance()->getPluginAssetsDirectory(getID()));
    QStringList sl=a.entryList(QStringList("*.ccf"));
    for (int i=0; i<sl.size(); i++) {
        //qDebug()<<a.absoluteFilePath(sl[i])<<d.absoluteFilePath(sl[i]);
        if (!QFile::exists(d.absoluteFilePath(sl[i]))) QFile::copy(a.absoluteFilePath(sl[i]), d.absoluteFilePath(sl[i]));
    }

    // read and output version infos:
    char version[1024];
    GetVersionInfo(AT_SDKVersion, version, 1023);
    SDKVersion=version;
    services->log_global_text(tr("%2    SDK version: %1\n").arg(QString(version)).arg(LOG_PREFIX));
    GetVersionInfo(AT_DeviceDriverVersion, version, 1023);
    deviceDriverVersion=version;
    services->log_global_text(tr("%2    device driver version: %1\n").arg(QString(version)).arg(LOG_PREFIX));
    at_32 lNumCameras;
    GetAvailableCameras(&lNumCameras);
    services->log_global_text(tr("%2    available cameras: %1\n").arg(lNumCameras).arg(LOG_PREFIX));


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
    dlgGlobalSettings_layout=new FlowLayout();
    dlgGlobalSettings->setLayout(dlgGlobalSettings_layout);
    dlgGlobalSettings->setWindowTitle(tr("Andor: Global Settings"));

    QAction* actShowGlobal=new QAction(tr("Andor Cameras: Show global settings ..."), this);
    actShowGlobal->setIcon(QIcon(":/cam_andor/show_global.png"));
    connect(actShowGlobal, SIGNAL(triggered()), dlgGlobalSettings, SLOT(show()));
    QMenu* extm=services->getMenu("extensions");
    if (extm) {
        //QMenu* andorMenu=extm->addMenu(QIcon(getIconFilename()), tr("Andor Cameras"));
        //andorMenu->addAction(actShowGlobal);
        extm->addAction(actShowGlobal);
    }


    // load settings
    loadSettings(NULL);

    // start continuous display of global settings
    updateGlobalSettingsWidgets();

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

unsigned int QFExtensionCameraAndor::getCameraCount() const {
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

        uint32_t* data=(uint32_t*)qfCalloc(info.width*info.height, sizeof(uint32_t));
        uint32_t* data1=(uint32_t*)qfCalloc(info.width*info.height, sizeof(uint32_t));
        useCameraSettings(camera, settings);
        acquireFullFrame(camera, data);
        useCameraSettings(camera, settings);
        acquireOnCamera(camera, data1);
        dlg->setImage(data);
        int oldShutterMode=camGlobalSettings[camera].shutterMode;
        camGlobalSettings[camera].shutterMode=2; // close shutter
        setGlobalSettings(camera);
        if ( dlg->exec()==QDialog::Accepted ) {
             dlg->writeSettings(settings);
        }
        camGlobalSettings[camera].shutterMode=oldShutterMode; // reset shutter mode
        setGlobalSettings(camera);

        qfFree(data);
        qfFree(data1);
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
    info.emgain_enabled=settings.value(prefix+"emgain_enabled", info.emgain_enabled).toBool();
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
    CHECK_NO_RETURN(SetAcquisitionMode(1), tr("error while setting \"single scan\" acquisition mode"));
    camInfos[camera]=info;

}




int QFExtensionCameraAndor::getCameraImageWidth(unsigned int camera) {
    if (!isCameraConnected(camera)) return 0;
    return (abs(camInfos[camera].subImage_hend-camInfos[camera].subImage_hstart)+1)/camInfos[camera].hbin;
}

int QFExtensionCameraAndor::getCameraImageHeight(unsigned int camera) {
    if (!isCameraConnected(camera)) return 0;
    return (abs(camInfos[camera].subImage_vend-camInfos[camera].subImage_vstart)+1)/camInfos[camera].vbin;
}

double QFExtensionCameraAndor::getCameraPixelWidth(unsigned int camera) {
    if (!isCameraConnected(camera)) return 1;
    return camInfos[camera].pixelWidth*(double)(camInfos[camera].hbin);
}

QString QFExtensionCameraAndor::getCameraName(unsigned int camera) {
    if (!camInfos.contains(camera)) return "";
    return camInfos[camera].headModel;
}

QString QFExtensionCameraAndor::getCameraSensorName(unsigned int camera) {
    if (!camInfos.contains(camera)) return "";
    return camInfos[camera].headModel;
}

double QFExtensionCameraAndor::getCameraPixelHeight(unsigned int camera) {
    if (!isCameraConnected(camera)) return 1;
    return camInfos[camera].pixelHeight*(double)(camInfos[camera].vbin);
}


bool QFExtensionCameraAndor::isCameraConnected(unsigned int camera) {
    return camConnected.contains(camera);
}

bool QFExtensionCameraAndor::acquireOnCamera(unsigned int camera, uint32_t* data, uint64_t* /*timestamp*/, QMap<QString, QVariant>* parameters) {
    if (!isCameraConnected(camera)) return false;

    if (!selectCamera(camera)) return false;

#ifdef DEBUG_TIMING
    HighResTimer timer;
    timer.start();
#endif

    CHECK(StartAcquisition(), tr("error starting acquisition"));

#ifdef DEBUG_TIMING
    //qDebug()<<"init "<<timer.get_time()<<" us";
    timer.start();
#endif

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
#ifdef DEBUG_TIMING
    //qDebug()<<"acquisition "<<timer.get_time()<<" us";
    timer.start();
#endif
    if (time.elapsed()<=timeout)  {


        int imagesize=getCameraImageWidth(camera)*getCameraImageHeight(camera);
        at_32* imageData = (at_32*)qfMalloc(imagesize*sizeof(at_32));
        CHECK_ON_ERROR(GetAcquiredData(imageData,imagesize), tr("error while acquiring frame"), qfFree(imageData));

        for(int i=0;i<imagesize;i++) data[i]=imageData[i];

        qfFree(imageData);
#ifdef DEBUG_TIMING
        //qDebug()<<"data read "<<timer.get_time()<<" us";
        timer.start();
#endif

        if (parameters) internalGetAcquisitionDescription(camera, parameters);
        return true;
    } else {
        log_error(tr("\n%1    error: acquiring image (exposure time %2 ms) timed out after %3 seconds ").arg(LOG_PREFIX).arg(info.expoTime*1000.0).arg(timeout/1000.0));
        return false;
    }


}



bool QFExtensionCameraAndor::acquireFullFrame(unsigned int camera, uint32_t* data, uint64_t* /*timestamp*/) {
    if (!isCameraConnected(camera)) return false;

    if (!selectCamera(camera)) return false;


    CameraInfo info;
    if (camInfos.contains(camera)) info=camInfos[camera];

    CHECK(SetIsolatedCropMode(0, info.width, info.height, 1, 1), tr("error while switching off isolated crop mode"));
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


        at_32* imageData = (at_32*)qfMalloc(info.width*info.height*sizeof(at_32));
        CHECK_ON_ERROR(GetAcquiredData(imageData, info.width*info.height), tr("error while acquiring frame"), qfFree(imageData));

        for(int i=0;i<info.width*info.height;i++) data[i]=imageData[i];
        qfFree(imageData);

		return true;
	} else {
		log_error(tr("\n%1    error: acquiring full-frame image (exposure time %2 ms) timed out after %3 seconds ").arg(LOG_PREFIX).arg(info.expoTime*1000.0).arg(timeout/1000.0));
		return false;
	}
}


bool QFExtensionCameraAndor::connectCameraDevice(unsigned int camera) {

    timer.start();

    QModernProgressDialog progress(tr("Initializing Andor Camera #%1").arg(camera+1), "", NULL);
    progress.setWindowModality(Qt::WindowModal);
    progress.setHasCancel(false);
    progress.open();
    QApplication::processEvents(QEventLoop::AllEvents, 50);


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
        progress.setLabelText(tr("initializing camera %1 ...<br>&nbsp;&nbsp;&nbsp;&nbsp;detectorsIniPath='%2'<br>&nbsp;&nbsp;&nbsp;&nbsp;<b>Note:</b> The program screen may freeze during this step ... in that case, please be patient, the program didn't crash!").arg(camera).arg(detectorsIniPath));

        QApplication::processEvents();
        QApplication::processEvents();
        { unsigned int error1=Initialize(path);
            if (error1!=DRV_SUCCESS) {
                log_warning(tr("Error during Initialize\n%3    error code was: %1 [%2]\n%3    instruction was: %4\n").arg(error1).arg(andorErrorToString(error1)).arg(LOG_PREFIX).arg("Initialize(path)"));
                log_warning(tr(" trying shutdown() + initialize cycle").arg(LOG_PREFIX));
                CHECK(ShutDown(), tr("Error during Shutdown"));
                CHECK(Initialize(path), tr("Error during Initialize"));
            }
        }


        progress.setLabelText(tr("initializing camera %1 ...<br>&nbsp;&nbsp;&nbsp;&nbsp;detectorsIniPath='%2'<br>&nbsp;&nbsp;&nbsp;&nbsp;acquiring camera configuration ...").arg(camera).arg(detectorsIniPath));


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

void QFExtensionCameraAndor::disconnectCameraDevice(unsigned int camera) {
    int i=camera;
    if (getCameraCount()>0) {
        selectCamera(camera);
        SetShutter(1,2,50,50);
        /* wait for camera(s) temperature to rise up to 5C */
        services->log_global_text(tr("%1 heating cameras to 5C ...\n").arg(LOG_PREFIX));

        int temp=getTemperature(i);
        CoolerOFF();
        services->log_global_text(tr("%2 cameras #%3: cooler off, temperature = %1 C ...\n").arg(temp).arg(LOG_PREFIX).arg(i+1));
        //std::cout<<tr("%2 cameras #%3: cooler off, temperature = %1 C ...\n").arg(temp).arg(LOG_PREFIX).arg(i+1).toStdString()<<std::endl;
        bool tempOK=false;

        QModernProgressDialog progress(tr("\"Heating\" Andor Camera #%1").arg(camera+1), "", NULL);
        progress.setWindowModality(Qt::ApplicationModal);
        progress.setHasCancel(false);
        progress.open();
        while (!tempOK && camGlobalSettings[camera].cooling_wait) {
            tempOK=true;
            int temp1=getTemperature(i);
            //progress.setValue(33);
            progress.setLabelText(tr("cameras #%2: temperature = %1 C ...\n").arg(temp1).arg(i+1));
            //std::cout<<tr("%2 cameras #%3: cooler off, temperature = %1 C ...\n").arg(temp1).arg(LOG_PREFIX).arg(i+1).toStdString()<<std::endl;
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

        services->log_global_text(tr("%1 heating cameras to 5C ... DONE\n").arg(LOG_PREFIX));
        //std::cout<<tr("%1 heating cameras to 5C ... DONE\n").arg(LOG_PREFIX).toStdString()<<std::endl;;
    }


    camConnected.remove(camera);
}

double QFExtensionCameraAndor::getCameraExposureTime(unsigned int camera) {
    if (!isCameraConnected(camera)) return 0;
    return camInfos[camera].expoTime;
}


bool QFExtensionCameraAndor::isCameraSettingChangable(QFExtensionCamera::CameraSetting which)   {
    if (which==QFExtensionCamera::CamSetExposureTime) return true;
    if (which==QFExtensionCamera::CamSetNumberFrames) return true;
    if (which==QFExtensionCamera::CamSetGain) return true;
    if (which==QFExtensionCamera::CamSetEMGAIN) return true;
    if (which==QFExtensionCamera::CamSetFrametime) return true;
    if (which==QFExtensionCamera::CamSetHorizontalBinning) return true;
    if (which==QFExtensionCamera::CamSetVerticalBinning) return true;
    if (which==QFExtensionCamera::CamSetTemporalBinning) return true;
    return false;
}

void QFExtensionCameraAndor::changeCameraSetting(QSettings& settings, QFExtensionCamera::CameraSetting which, QVariant value)  {
    QString prefix="cam_andor/";
    if (which==QFExtensionCamera::CamSetExposureTime) settings.setValue(prefix+"exposure_time", value);
    if (which==QFExtensionCamera::CamSetNumberFrames) settings.setValue(prefix+"kinetic_cycles", value);
    if (which==QFExtensionCamera::CamSetEMGAIN) {
        settings.setValue(prefix+"emgain", value.toDouble());
        settings.setValue(prefix+"emgain_enabled", value.toDouble()>0);
    }
    if (which==QFExtensionCamera::CamSetGain) settings.setValue(prefix+"preamp_gain", value);
    if (which==QFExtensionCamera::CamSetFrametime) settings.setValue(prefix+"kinetic_cycle_time", value);
    if (which==QFExtensionCamera::CamSetHorizontalBinning) settings.setValue(prefix+"horizontal_binning", value);
    if (which==QFExtensionCamera::CamSetVerticalBinning) settings.setValue(prefix+"vertical_binning", value);
    if (which==QFExtensionCamera::CamSetTemporalBinning) settings.setValue(prefix+"accumulate_cycles", value);
}

QVariant QFExtensionCameraAndor::getCameraSetting(QSettings& settings, QFExtensionCamera::CameraSetting which)  {
    QString prefix="cam_andor/";
    if (which==QFExtensionCamera::CamSetExposureTime) return settings.value(prefix+"exposure_time");
    if (which==QFExtensionCamera::CamSetNumberFrames) return settings.value(prefix+"kinetic_cycles");
    if (which==QFExtensionCamera::CamSetGain) return settings.value(prefix+"preamp_gain");
    if (which==QFExtensionCamera::CamSetFrametime) return settings.value(prefix+"kinetic_cycle_time");
    if (which==QFExtensionCamera::CamSetEMGAIN) return settings.value(prefix+"emgain");
    if (which==QFExtensionCamera::CamSetHorizontalBinning) return settings.value(prefix+"horizontal_binning");
    if (which==QFExtensionCamera::CamSetVerticalBinning) return settings.value(prefix+"vertical_binning");
    if (which==QFExtensionCamera::CamSetTemporalBinning) return settings.value(prefix+"accumulate_cycles");
    return QVariant();
}

QVariant QFExtensionCameraAndor::getCameraCurrentSetting(int camera, QFExtensionCamera::CameraSetting which)
{
    if (camera<0 || camera>=(long long)getCameraCount()) return QVariant();

    if (which==QFExtensionCamera::CamSetExposureTime) return camInfos[camera].expoTime;
    if (which==QFExtensionCamera::CamSetNumberFrames) return camInfos[camera].numKins;
    if (which==QFExtensionCamera::CamSetGain) return camInfos[camera].preampGainF;
    if (which==QFExtensionCamera::CamSetFrametime) return camInfos[camera].kinTime;
    if (which==QFExtensionCamera::CamSetEMGAIN) {
        if (camInfos[camera].emgain_enabled)
            return camInfos[camera].emgain;
        else
            return 0;
    }
    if (which==QFExtensionCamera::CamSetHorizontalBinning) return camInfos[camera].hbin;
    if (which==QFExtensionCamera::CamSetVerticalBinning) return camInfos[camera].vbin;
    if (which==QFExtensionCamera::CamSetTemporalBinning) return camInfos[camera].accTime;


    return QVariant();
}

bool QFExtensionCameraAndor::prepareCameraAcquisition(unsigned int camera, const QSettings& settings, QString filenamePrefix) {
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
    bool ok=thread->init(camera, filenamePrefix, info.fileformat, info.numKins, getCameraImageWidth(camera), getCameraImageHeight(camera), info.expoTime, LOG_PREFIX);
    if (!ok) {
        delete thread;
        return false;
    }
    camThreads[camera]=thread;

    return true;
}


bool QFExtensionCameraAndor::startCameraAcquisition(unsigned int camera) {
    CamAndorAcquisitionThread* thread=camThreads.value(camera, NULL);
    if (!thread) return false;
    thread->start();
    return true;
}

void QFExtensionCameraAndor::cancelCameraAcquisition(unsigned int camera) {
    /*if (selectCamera(camera)) {
        CHECK_NO_RETURN(AbortAcquisition(), tr("error while aborting acquisition"));
    }*/
    CamAndorAcquisitionThread* thread=camThreads.value(camera, NULL);
    if (thread) {
        thread->cancelAcquisition();
    }
}

bool QFExtensionCameraAndor::isCameraAcquisitionRunning(unsigned int camera) {
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

void QFExtensionCameraAndor::internalGetAcquisitionDescription(unsigned int camera, QMap<QString, QVariant>* parameters) {
    QFExtensionCameraAndor::CameraInfo info=camInfos[camera];
    char text[512];

    switch(info.ReadMode) {
        case 0: (*parameters)["read_mode"]=QString("full vertical binning (%1)").arg(info.ReadMode); break;
        case 1: (*parameters)["read_mode"]=QString("multi-track (%1)").arg(info.ReadMode); break;
        case 2: (*parameters)["read_mode"]=QString("random-track (%1)").arg(info.ReadMode); break;
        case 3: (*parameters)["read_mode"]=QString("single-track (%1)").arg(info.ReadMode); break;
        case 4: (*parameters)["read_mode"]=QString("image (%1)").arg(info.ReadMode); break;
        default: (*parameters)["read_mode"]=info.ReadMode; break;
    }
    switch(info.AcqMode) {
        case 1: (*parameters)["acquisition_mode"]=QString("single scan (%1)").arg(info.AcqMode); break;
        case 2: (*parameters)["acquisition_mode"]=QString("accumulate (%1)").arg(info.AcqMode); break;
        case 3: (*parameters)["acquisition_mode"]=QString("kinetics (%1)").arg(info.AcqMode); break;
        case 4: (*parameters)["acquisition_mode"]=QString("fast kinetics (%1)").arg(info.AcqMode); break;
        case 5: (*parameters)["acquisition_mode"]=QString("run till abort (%1)").arg(info.AcqMode); break;
        default: (*parameters)["acquisition_mode"]=info.AcqMode; break;
    }

    (*parameters)["sensor_width"]=info.width;
    (*parameters)["sensor_height"]=info.height;
    (*parameters)["exposure_time"]=(double)info.expoTime;
    (*parameters)["roi_xstart"]=info.subImage_hstart;
    (*parameters)["roi_xend"]=info.subImage_hend;
    (*parameters)["roi_ystart"]=info.subImage_vstart;
    (*parameters)["roi_yend"]=info.subImage_vend;
    (*parameters)["image_width"]=getCameraImageWidth(camera);
    (*parameters)["image_height"]=getCameraImageHeight(camera);
    (*parameters)["sensor_temperature"]=getTemperature(camera);

    switch(camGlobalSettings[camera].shutterMode) { //0: auto, 1: open, 2: close
        case 0:  (*parameters)["shutter_state"]=QString("auto"); break;
        case 1:  (*parameters)["shutter_state"]=QString("open"); break;
        case 2:  (*parameters)["shutter_state"]=QString("closed"); break;
    }
    (*parameters)["cooler_enabled"]=camGlobalSettings[camera].coolerOn;
    (*parameters)["cooler_target_temperature"]=camGlobalSettings[camera].setTemperature;
    switch(camGlobalSettings[camera].fanMode) { //0: full, 1: low, 2: off
        case 0:  (*parameters)["fan_mode"]=QString("full"); break;
        case 1:  (*parameters)["fan_mode"]=QString("low"); break;
        case 2:  (*parameters)["fan_mode"]=QString("off"); break;
    }

    (*parameters)["camera_manufacturer"]="Andor";
    (*parameters)["camera_sdk_version"]=SDKVersion;
    (*parameters)["camera_driver_version"]=deviceDriverVersion;
    (*parameters)["pixel_units"]=QString("arbitrary");
    selectCamera(camera);
    GetAmpDesc(info.outputAmplifier, text, 512);
    (*parameters)["output_amplifier"]=QString(text);


    (*parameters)["sequence_length"]=info.numAccs;
    (*parameters)["frame_time"]=(double)info.kinTime;
    (*parameters)["accumulation_time"]=(double)info.accTime;
    (*parameters)["spooling"]=info.spooling;
    (*parameters)["readout_time"]=(double)info.readoutTime;
    (*parameters)["camera_sensitivity"]=(double)info.sensitivity;
    (*parameters)["horizontal_shift_speed"]=(double)info.horizontalSpeed;
    (*parameters)["vertical_shift_speed"]=info.verticalSpeed;
    (*parameters)["pixel_width"]=(double)info.pixelWidth*(double)info.hbin;
    (*parameters)["pixel_height"]=(double)info.pixelHeight*(double)info.vbin;
    (*parameters)["preamplifier_gain"]=double(info.preampGainF);
    (*parameters)["bit_depth"]=info.bitDepth;
    (*parameters)["camera_serial_number"]=info.serialNumber;
    (*parameters)["camera_controller_card"]=info.controllerCard;
    (*parameters)["camera_sensor"]=info.headModel;
    (*parameters)["baseline_clamp"]=info.baselineClamp;
    if (!info.baselineClamp) (*parameters)["baseline_offset"]=info.baselineOffset;
    (*parameters)["frame_transfer"]=info.frameTransfer;
    (*parameters)["crop_mode"]=info.cropMode;
    (*parameters)["vertical_shift_amplitude"]=info.vsAmplitude;
    int gain=0;
    GetEMCCDGain(&gain);
    (*parameters)["emgain"]=gain;
    (*parameters)["emgain_enabled"]=info.emgain_enabled;
    (*parameters)["emgain_advanced"]=info.advancedEMGain;
    (*parameters)["spooling_mode"]=info.spool;
    (*parameters)["binning_vertical"]=info.vbin;
    (*parameters)["binning_horizontal"]=info.hbin;
    (*parameters)["kinetic_time"]=(double)info.kinTime;
    (*parameters)["accumulations"]=info.numAccs;
    (*parameters)["images"]=info.numKins;
    (*parameters)["trigger_invert"]=info.trigInvert;
    switch(info.trigMode) {
        case 0: (*parameters)["trigger_mode"]="internal (0)"; break;
        case 1: (*parameters)["trigger_mode"]="external (1)"; break;
        case 6: (*parameters)["trigger_mode"]="external start (6)"; break;
        case 7: (*parameters)["trigger_mode"]="external exposure, bulb (7)"; break;
        case 9: (*parameters)["trigger_mode"]="external FVB EM (9)"; break;
        case 10: (*parameters)["trigger_mode"]="software (10)"; break;
        default: (*parameters)["trigger_mode"]=info.trigMode; break;
    }
}

void QFExtensionCameraAndor::getCameraAcquisitionDescription(unsigned int camera, QList<QFExtensionCamera::CameraAcquititonFileDescription>* files, QMap<QString, QVariant>* parameters) {
    QStringList fileNames, fileTypes;
    CamAndorAcquisitionThread* thread=camThreads.value(camera, NULL);
    char text[512];
    double duration=0;
    if (thread) {
        fileNames=thread->getOutputFilenames();
        fileTypes=thread->getOutputFilenameTypes();
        duration=thread->getDurationMilliseconds();
    }


    internalGetAcquisitionDescription(camera, parameters);
    selectCamera(camera);
    QString qefilename=thread->getPrefix()+"_cameraqe.dat";
    QFDataExportTool qedata;
    int i=0;
    char headModel[MAX_PATH];
    if (GetHeadModel(headModel)==DRV_SUCCESS){
        qedata.setColTitle(0, tr("wavelength [nm]"));
        qedata.setColTitle(1, tr("QE"));
        for (float l=100; l<2000; l++) {
            float qe=0;
#ifdef __WINDOWS__
            if (GetQE(headModel,l,&qe)==DRV_SUCCESS){
                qedata.set(0, i, (double)l);
                qedata.set(1, i, (double)qe);
            }
#else
            if (GetQE(headModel,l,0,&qe)==DRV_SUCCESS){
                qedata.set(0, i, (double)l);
                qedata.set(1, i, (double)qe);
            }
#endif
            i++;
        }
        if (qedata.getRowCount()>0) {
            qedata.save(qefilename, 0);
            QFExtensionCamera::CameraAcquititonFileDescription d;
            d.name=qefilename;
            d.type="CSV";
            d.description=tr("Andor Camera Quantum Efficiency");
            files->append(d);
        }

    }

    (*parameters)["duration_milliseconds"]=duration;

    for (int i=0; i<fileNames.size(); i++) {
        QFExtensionCamera::CameraAcquititonFileDescription d;
        d.name=fileNames[i];
        d.type=fileTypes[i];
        d.description=tr("Andor camera acquisition image series");
        files->append(d);
    }

}

bool QFExtensionCameraAndor::getCameraAcquisitionPreview(unsigned int /*camera*/, uint32_t* /*data*/) {
    return false;
}


int QFExtensionCameraAndor::getCameraAcquisitionProgress(unsigned int camera) {
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
    if (GetSensitivity(info.ADchannel, info.outputAmplifier, info.hsSpeed, info.preamp_gain, &(info.sensitivity))!=DRV_SUCCESS) {
        info.sensitivity=0;
    }
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
        if (info.emgain_enabled && info.emgain>0) {
            //qDebug()<<"setting EMGain: "<<info.emgain;
            CHECK(SetEMCCDGain(info.emgain), tr("error while setting EM gain %1").arg(info.emgain));
        } else {
            //qDebug()<<"setting EMGain off ("<<info.emgain<<")";
            CHECK(SetEMCCDGain(0), tr("error while setting EM gain 0 (switch off)"));
        }
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

bool QFExtensionCameraAndor::setCamShutter(int camera, int mode, int closingtime, int openingtime) {
    if (selectCamera(camera)) {
        /* SetShutter(
            typ = 0 (TTL low opens shutter)
            mode = 0 (Auto)
            closingtime = 50
            openingtime = 50
        */
        camGlobalSettings[camera].lastShutterAction=QTime::currentTime();
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
        //std::cout<<tr("%2 cameras #%3: temperature = %1C ...\n").arg(temp1).arg(LOG_PREFIX).arg(cam+1).toStdString()<<std::endl;
        //if ((temp1<5)&&(temp1!=-999)) tempOK=false;
        return temp1;
    }
}

float QFExtensionCameraAndor::getTemperatureF(int cam) {
    float temp1=-999;
    if (!selectCamera(cam)) {
        //services->log_global_error(tr("%2 cameras #%1: could not connect ...\n").arg(i+1).arg(LOG_PREFIX));
        //std::cout<<tr("%2 cameras #%1: could not connect ...\n").arg(cam+1).arg(LOG_PREFIX).toStdString()<<std::endl;
        return -999;
    } else {
        GetTemperatureF(&temp1);
        //std::cout<<tr("%2 cameras #%3: temperature = %1C ...\n").arg(temp1).arg(LOG_PREFIX).arg(cam+1).toStdString()<<std::endl;
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
            if (isCameraConnected(camera)) {
                ok=ok&&setTemperature(camera, it.value().coolerOn, it.value().setTemperature, it.value().fanMode);
                ok=ok&&setCamShutter(camera, it.value().shutterMode, it.value().shutterClosingTime, it.value().shutterOpeningTime);
            }
        }
    } else {
        if (camGlobalSettings.contains(camera) && isCameraConnected(camera)) {
            ok=setTemperature(camera, camGlobalSettings[camera].coolerOn, camGlobalSettings[camera].setTemperature, camGlobalSettings[camera].fanMode);
            ok=ok&&setCamShutter(camera, camGlobalSettings[camera].shutterMode, camGlobalSettings[camera].shutterClosingTime, camGlobalSettings[camera].shutterOpeningTime);
        }
    }
    return ok;
}



void QFExtensionCameraAndor::updateGlobalSettingsWidgets(bool startTimer) {
    for (unsigned int i=0; i<getCameraCount(); i++) {
        AndorGlobalCameraSettingsWidget* widget=camGlobalSettingsWidgets.value(i, NULL);
        if (!widget) {
            widget=new AndorGlobalCameraSettingsWidget(i, dlgGlobalSettings);
            camGlobalSettingsWidgets[i]=widget;
            dlgGlobalSettings_layout->addWidget(widget);
            connect(widget, SIGNAL(settingsChanged(int,int,bool,bool,int,int)), this, SLOT(globalSettingsChanged(int,int,bool,bool,int,int)));
        }

        if (isCameraConnected(i)) {
            if (widget) {
                widget->setSettings(camGlobalSettings[i].fanMode, camGlobalSettings[i].coolerOn, camGlobalSettings[i].cooling_wait, camGlobalSettings[i].setTemperature, camGlobalSettings[i].shutterMode);
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

    if (startTimer) QTimer::singleShot(UPDATE_TEMP_INTERVAL, this, SLOT(updateGlobalSettingsWidgets()));
}



void QFExtensionCameraAndor::globalSettingsChanged(int camera, int fan_mode, bool cooling_on, bool cooling_wait, int temperature, int shutterMode) {
    if (!camGlobalSettings.contains(camera)) {
        QSettings inifile(GLOBAL_INI, QSettings::IniFormat);
        CameraGlobalSettings global;
        global.readSettings(inifile, camera);
        camGlobalSettings[camera]=global;
    }

    camGlobalSettings[camera].coolerOn=cooling_on;
    camGlobalSettings[camera].cooling_wait=cooling_wait;
    camGlobalSettings[camera].fanMode=fan_mode;
    camGlobalSettings[camera].setTemperature=temperature;
    camGlobalSettings[camera].shutterMode=shutterMode;
    setGlobalSettings(camera);
    storeGlobalSettings();
}






unsigned int QFExtensionCameraAndor::getShutterCount()  {
    return getCameraCount();
}

void  QFExtensionCameraAndor::shutterConnect(unsigned int /*shutter*/) {

}

void  QFExtensionCameraAndor::shutterDisonnect(unsigned int /*shutter*/) {

}

bool  QFExtensionCameraAndor::isShutterConnected(unsigned int shutter)  {
    return isCameraConnected(shutter);
}

bool  QFExtensionCameraAndor::isShutterOpen(unsigned int shutter)  {
    if (camGlobalSettings.contains(shutter)) {
        if (camGlobalSettings[shutter].shutterMode!=2) return true;
        else return false;
    }
    return false;
}

void  QFExtensionCameraAndor::setShutterState(unsigned int shutter, bool opened) {
    if (shutter>=getShutterCount()) return;
    if (opened) camGlobalSettings[shutter].shutterMode=1;
    else camGlobalSettings[shutter].shutterMode=2;
    setGlobalSettings(shutter);
    storeGlobalSettings();
    updateGlobalSettingsWidgets(false);

}

QString QFExtensionCameraAndor::getShutterDescription(unsigned int shutter) {
    if (shutter<getShutterCount()) return tr("internal camera shutter");
    return QString("");
}


QString QFExtensionCameraAndor::getShutterShortName(unsigned int shutter) {
    if (shutter<getShutterCount()) return tr("camera shutter");
    return QString("");
}

bool QFExtensionCameraAndor::isLastShutterActionFinished(unsigned int shutter) {
    if (shutter>=getShutterCount()) return true;
    return camGlobalSettings[shutter].lastShutterAction.elapsed()>150;
}

void QFExtensionCameraAndor::showShutterSettingsDialog(unsigned int /*axis*/, QWidget* parent) {
    QMessageBox::information(parent, tr("Andor camera driver"), tr("there is no configuration dialog for the internal shutter!"));
}

void QFExtensionCameraAndor::setShutterLogging(QFPluginLogService* logService) {
    setCameraLogging(logService);
}

unsigned int QFExtensionCameraAndor::getMeasurementDeviceCount()
{
    return getCameraCount();
}

void QFExtensionCameraAndor::showMeasurementDeviceSettingsDialog(unsigned int /*measurementDevice*/, QWidget *parent) {
    QMessageBox::information(parent, tr("Andor camera driver"), tr("there is no configuration dialog for the camera as measurement device!"));
}

bool QFExtensionCameraAndor::isMeasurementDeviceConnected(unsigned int measurementDevice)
{
    return isCameraConnected(measurementDevice);
}

void QFExtensionCameraAndor::connectMeasurementDevice(unsigned int /*measurementDevice*/)
{

}

void QFExtensionCameraAndor::disconnectMeasurementDevice(unsigned int /*measurementDevice*/)
{
}

QString QFExtensionCameraAndor::getMeasurementDeviceName(unsigned int measuremenDevice)
{
    return getCameraName(measuremenDevice);
}

void QFExtensionCameraAndor::setMeasurementDeviceLogging(QFPluginLogService *logService)
{
    setCameraLogging(logService);
}

unsigned int QFExtensionCameraAndor::getMeasurementDeviceValueCount(unsigned int measurementDevice)
{
    if (measurementDevice<getMeasurementDeviceCount()) {
        return 1;
    }
    return 0;
}

QVariant QFExtensionCameraAndor::getMeasurementDeviceValue(unsigned int measurementDevice, unsigned int value)
{
    if (measurementDevice<getMeasurementDeviceCount()) {
        // WE HAVE TO CHECK WHETHER AN ACQUISITION IS CURRENTLY RUNNING; SO WE DON'T DISTURB THE THREAD!!!
        CamAndorAcquisitionThread* thread=camThreads.value(measurementDevice, NULL);
        if (thread && thread->isRunning()) {
            if (value==0) return  thread->getTemperature();
        } else {
            if (value==0)  return double(getTemperatureF(measurementDevice));
        }
    }
    return QVariant();
}

QString QFExtensionCameraAndor::getMeasurementDeviceValueName(unsigned int /*measurementDevice*/, unsigned int value)
{
    if (value==0)  return tr("Temperature [C]");
    return QString();
}

QString QFExtensionCameraAndor::getMeasurementDeviceValueShortName(unsigned int /*measurementDevice*/, unsigned int value)
{
    if (value==0) return QString("temperature_degC");
    return QString();
}

bool QFExtensionCameraAndor::isMeasurementDeviceValueEditable(unsigned int /*measuremenDevice*/, unsigned int value)
{
    return false;
}

void QFExtensionCameraAndor::setMeasurementDeviceValue(unsigned int /*measuremenDevice*/, unsigned int /*value*/, const QVariant &/*data*/)
{

}

QVariant::Type QFExtensionCameraAndor::getMeasurementDeviceEditableValueType(unsigned int /*measuremenDevice*/, unsigned int /*value*/)
{
    return QVariant::Invalid;
}

QFExtensionMeasurementAndControlDevice::WidgetTypes QFExtensionCameraAndor::getMeasurementDeviceValueWidget(unsigned int /*measuremenDevice*/, unsigned int /*value*/, QStringList */*comboboxEntries*/)
{
    return QFExtensionMeasurementAndControlDevice::mdDefault;
}

void QFExtensionCameraAndor::getMeasurementDeviceEditableValueRange(unsigned int /*measuremenDevice*/, unsigned int /*value*/, double &/*minimum*/, double &/*maximum*/)
{

}

Q_EXPORT_PLUGIN2(cam_andor, QFExtensionCameraAndor)




