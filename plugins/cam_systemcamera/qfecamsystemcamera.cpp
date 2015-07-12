/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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

#include "qfecamsystemcamera.h"
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtGlobal>
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "qfplugin.h"
#include <iostream>
#include "videocapture.h"
#include "syscamconfigdialog.h"
#include "qfmathtools.h"

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
        case QFECamSystemcamera::tgmSaturation:
            for (int y=0; y<height; y++) {
                for (int x=0; x<width; x++) {
                    QColor c(rgb[y*width*3+x*3], rgb[y*width*3+x*3+1], rgb[y*width*3+x*3+2]);
                    out[y*width+x]=c.saturation();
                }
            }
            break;
        case QFECamSystemcamera::tgmBrightness:
            for (int y=0; y<height; y++) {
                for (int x=0; x<width; x++) {
                    QColor c(rgb[y*width*3+x*3], rgb[y*width*3+x*3+1], rgb[y*width*3+x*3+2]);
                    out[y*width+x]=c.value();
                }
            }
            break;
    }

}


QFECamSystemcamera::QFECamSystemcamera(QObject* parent):
    QObject(parent)
{
	logService=NULL;
    toGrayMethod=QFECamSystemcamera::tgmBrightness;
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

    Q_UNUSED(project);
    Q_UNUSED(oldProject);

}

void QFECamSystemcamera::initExtension() {

    QString pd=QFPluginServices::getInstance()->getPluginConfigDirectory(getID());
    QDir(QFPluginServices::getInstance()->getConfigFileDirectory()).mkpath(pd);
    QDir d(pd);
    QDir a(QFPluginServices::getInstance()->getPluginAssetsDirectory(getID()));
    QStringList sl=a.entryList(QStringList("*.ccf"));
    for (int i=0; i<sl.size(); i++) {
        //qDebug()<<a.absoluteFilePath(sl[i])<<d.absoluteFilePath(sl[i]);
        if (!QFile::exists(d.absoluteFilePath(sl[i]))) QFile::copy(a.absoluteFilePath(sl[i]), d.absoluteFilePath(sl[i]));
    }

    /* do initializations here but do not yet connect to the camera! */
    VideoCapture cap;
    cameras=cap.getDeviceList();
    QMenu* scm=new QMenu(tr("&System Camera Controls"));
    scm->setIcon(QIcon(":/cam_systemcam/qfecamsystemcamera.png"));
    for (size_t i=0; i<cameras.size(); i++) {
        vids.append(new VideoCapture());
        if (vids[i]->getConfigDialogCount()>0) {
            QMenu* m=scm->addMenu(cameras[i].c_str());
            for (int d=0; d<vids[i]->getConfigDialogCount(); d++) {
                QAction* a=new QAction(vids[i]->getConfigDialogName(d).c_str(), this);
                dlgActions[a]=qMakePair(vids[i], d);
                m->addAction(a);
                connect(a, SIGNAL(triggered()), this, SLOT(dlgActionTriggered()));
            }
        }
    }
    QMenu* extm=services->getMenu("extensions");
    if (extm && scm->children().size()>0) {
        extm->addMenu(scm);
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

unsigned int QFECamSystemcamera::getCameraCount() const {
    return cameras.size();
}

#define VIDSETPROP(prop, name) if (settings.contains(name) && vid->supportsUserProperty(prop)) vid->setUserProperty(prop, settings.value(name).toInt());

void QFECamSystemcamera::useCameraSettings(unsigned int camera, const QSettings& settings) {
    VideoCapture* vid=vids.value(camera, NULL);
    if (vid) {
        VIDSETPROP(VideoCapture::upAutogain, "syscam/autogain");
        VIDSETPROP(VideoCapture::upAutoWhitebalance, "syscam/autowhitebalance");
        VIDSETPROP(VideoCapture::upPowerlineFilter, "syscam/powerlinefilter");
        VIDSETPROP(VideoCapture::upBacklightCompensation, "syscam/backlightcompensation");
        VIDSETPROP(VideoCapture::upBlueBalance, "syscam/bluebalance");
        VIDSETPROP(VideoCapture::upBrightness, "syscam/brightness");
        VIDSETPROP(VideoCapture::upContrast, "syscam/contrast");
        VIDSETPROP(VideoCapture::upExposure, "syscam/exposure");
        VIDSETPROP(VideoCapture::upGain, "syscam/gain");
        VIDSETPROP(VideoCapture::upGamma, "syscam/gamma");
        VIDSETPROP(VideoCapture::upHue, "syscam/hue");
        VIDSETPROP(VideoCapture::upRedBalance, "syscam/redbalance");
        VIDSETPROP(VideoCapture::upSaturation, "syscam/saturation");
        VIDSETPROP(VideoCapture::upSharpness, "syscam/sharpness");
        if (settings.contains("syscam/togray")) toGrayMethod=(QFECamSystemcamera::toGrayMethods)settings.value("syscam/togray", 5).toInt();

        //settings.beginGroup("moresyscam");
        QStringList key=settings.allKeys();
        for (int i=0; i<key.size(); i++) {
            if (key[i].startsWith("moresyscam/")) {
                QRegExp rx("moresyscam/control(\\d+)",Qt::CaseInsensitive);
                int pos = rx.indexIn(key[i]);
                if (pos > -1) {
                    int id = rx.cap(1).toInt();
                    VIDSETPROP(id, key[i]);
                 }
            }
        }
        //settings.endGroup();

    }
}

bool QFECamSystemcamera::prepareCameraAcquisition(unsigned int camera, const QSettings& settings, QString /*filenamePrefix*/) {
    /* set the camera settings to the values specified in settings parameter, called before startAcquisition() */

    // uncomment this if the code is the same as in useCameraSettings()
    useCameraSettings(camera, settings);

    return true;
}


void QFECamSystemcamera::showCameraSettingsDialog(unsigned int camera, QSettings& settings, QWidget* parent) {
	/* open a dialog that configures the camera.

	   usually you should display a modal QDialog descendent which writes back config when the user clicks OK

	   alternatively you may also display a window which stays open and allows the suer to set settings also
	   during the measurement.
	*/
    VideoCapture* vid=vids.value(camera, NULL);
    if (!vid) return;

	/////////////////////////////////////////////////////////////////////////////////
	// if you want the settings dialog to be modal, you may uncomment the next lines
	// and add implementations
	/////////////////////////////////////////////////////////////////////////////////
    SyscamConfigDialog* dlg=new SyscamConfigDialog(vid, camera, parent);
    dlg->loadFromQSettings(settings);

    if ( dlg->exec()==QDialog::Accepted ) {
        dlg->saveToQSettings(settings);
    }
    delete dlg;

}

int QFECamSystemcamera::getCameraImageWidth(unsigned int camera) {
    VideoCapture* vid=vids.value(camera, NULL);
    if (vid) return vid->getVideoWidth();
    return 0;
}

int QFECamSystemcamera::getCameraImageHeight(unsigned int camera) {
    VideoCapture* vid=vids.value(camera, NULL);
    if (vid) return vid->getVideoHeight();
    return 0;
}

double QFECamSystemcamera::getCameraPixelWidth(unsigned int /*camera*/) {
    return 1;
}

double QFECamSystemcamera::getCameraPixelHeight(unsigned int /*camera*/) {
    return 1;
}


QString QFECamSystemcamera::getCameraName(unsigned int camera) {
    if (camera>0 && camera<cameras.size()) return cameras[camera].c_str();
    return QString("");
}

QString QFECamSystemcamera::getCameraSensorName(unsigned int camera) {
    return getCameraName(camera);
}

bool QFECamSystemcamera::isCameraConnected(unsigned int camera) {
    VideoCapture* vid=vids.value(camera, NULL);
    if (vid) return vid->isConnected();
    return false;
}

bool QFECamSystemcamera::acquireOnCamera(unsigned int camera, uint32_t* data, uint64_t* timestamp, QMap<QString, QVariant>* /*parameters*/) {
    if (camera>=cameras.size()) return false;
    if (timestamp!=NULL) {
        *timestamp=(uint64_t)round(timer[camera].elapsed());;
    }

    uint8_t* img=(uint8_t*)qfCalloc(getCameraImageWidth(camera)*getCameraImageHeight(camera)*3, sizeof(uint8_t));
    int retry=0;
    bool ok=false;
    while (retry<40 && !ok) {
        vids[camera]->captureFrame(img);
        int w=getCameraImageWidth(camera);
        int h=getCameraImageHeight(camera);
        RGBToGray(img, data, w, h, toGrayMethod);
        ok=true;
        bool allZero=true;
        for (int i=0; i<w*h; i++)  {
            if (!QFFloatIsOK(data[i])) {
                ok=false;
                break;
            }
            allZero=allZero&&(data[i]==0);
        }
        if (allZero) ok=false;
        retry++;
    }

    if (retry>5) log_warning(tr("retries for frame: %1\n").arg(retry));
    qfFree(img);
    return true;
}

bool QFECamSystemcamera::connectCameraDevice(unsigned int camera) {
    if (camera>=cameras.size()) return false;
    timer[camera]=QElapsedTimer();
    timer[camera].start();
    return vids[camera]->connect(camera);
}

void QFECamSystemcamera::disconnectCameraDevice(unsigned int camera) {
    /* disconnect from the given camera */
     if (camera>=cameras.size()) return ;
     vids[camera]->disconnect();
}

double QFECamSystemcamera::getCameraExposureTime(unsigned int camera) {
    VideoCapture* vid=vids.value(camera, NULL);
    if (vid) return vid->getLastExposureTimet();
    return 0;
}

bool QFECamSystemcamera::startCameraAcquisition(unsigned int /*camera*/) {
    return false;
}

void QFECamSystemcamera::cancelCameraAcquisition(unsigned int /*camera*/) {

}

bool QFECamSystemcamera::isCameraAcquisitionRunning(unsigned int /*camera*/) {
    return false;
}

void QFECamSystemcamera::getCameraAcquisitionDescription(unsigned int /*camera*/, QList<CameraAcquititonFileDescription> */*files*/, QMap<QString, QVariant>* /*parameters*/) {
}

bool QFECamSystemcamera::getCameraAcquisitionPreview(unsigned int /*camera*/, uint32_t* /*data*/) {
    return false;
}

int QFECamSystemcamera::getCameraAcquisitionProgress(unsigned int /*camera*/) {
    return 0; // return a number between 0 and 100 which indicates the progress of a currently running acquisition
}

bool QFECamSystemcamera::isCameraSettingChangable(QFExtensionCamera::CameraSetting /*which*/)   {
	return false; 
}

void QFECamSystemcamera::changeCameraSetting(QSettings& /*settings*/, QFExtensionCamera::CameraSetting /*which*/, QVariant /*value*/)  {

}

QVariant QFECamSystemcamera::getCameraSetting(QSettings& /*settings*/, QFExtensionCamera::CameraSetting /*which*/)   {
    return QVariant();
}

QVariant QFECamSystemcamera::getCameraCurrentSetting(int /*camera*/, QFExtensionCamera::CameraSetting /*which*/)
{
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

void QFECamSystemcamera::dlgActionTriggered() {
    if (dlgActions.contains(sender())) {
        QPair<VideoCapture*, int> p=dlgActions[sender()];
        if (p.first) {
            p.first->showConfigDialog(p.second);
        }
    }
}


Q_EXPORT_PLUGIN2(cam_systemcam, QFECamSystemcamera)
