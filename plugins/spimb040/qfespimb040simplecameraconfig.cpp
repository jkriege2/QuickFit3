/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

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

#include "qfespimb040simplecameraconfig.h"
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtGlobal>
#include <QtWidgets>
#else
#include <QtGui>
#endif


#include <iostream>
#include "qfespimb040opticssetup.h"

QFESPIMB040SimpleCameraConfig::viewDataStruct::viewDataStruct() {
    reset();
}

void QFESPIMB040SimpleCameraConfig::viewDataStruct::reset() {
    rawImage.assign(100, 100, 0);
    acqFrames=acqFramesQR=0;
    acqStartedQR=acqStarted=lastAcquisitionTime=QTime::currentTime();
    continuous_is_first=false;
    abort_continuous_acquisition=false;
    extension=NULL;
    camera=NULL;
    usedCamera=0;
    timestamp=0;
    exposureTime=0;
    measurementDevice=NULL;
}





QFESPIMB040SimpleCameraConfig::QFESPIMB040SimpleCameraConfig(QWidget* parent):
    QGroupBox(parent)
{
    m_camViewID=0;
    m_extManager=NULL;
    camView=NULL;
    locked=false;
    restartPreview=false;
    m_magnification=1;
    m_lastUserPreview="";


    // initialize raw image memory ...
    viewData.reset();

    // create widgets and actions

    createWidgets();
    createActions();
    displayStates(QFESPIMB040SimpleCameraConfig::Disconnected);
    if (cmbAcquisitionDevice->count()<=0) displayStates(QFESPIMB040SimpleCameraConfig::Inactive);
    init(0, NULL, "", NULL);
    setCheckable(true);
    setChecked(true);
}


QFESPIMB040SimpleCameraConfig::~QFESPIMB040SimpleCameraConfig() {
    //qDebug()<<"destroy";
    if (camView) {
        camView->close();
        delete camView;
        camView=NULL;
    }
    //qDebug()<<"destroy done";
}

void QFESPIMB040SimpleCameraConfig::setMagnification(double mag) {
    m_magnification=mag;
}

void QFESPIMB040SimpleCameraConfig::init(int camViewID, QFPluginServices* pluginServices, QString configDirectory, QFESPIMB040OpticsSetupBase* opticsSetup) {
    m_camViewID=camViewID;
    m_pluginServices=pluginServices;
    m_extManager=NULL;
    if (pluginServices) m_extManager=pluginServices->getExtensionManager();
    cmbAcquisitionDevice->init(m_extManager);
    cmbPreviewConfiguration->init(configDirectory);
    if (m_pluginServices) {
        cmbPreviewConfiguration->init(m_pluginServices->getConfigFileDirectory());
    }
    camView->init(camViewID, this, opticsSetup);
    setTitle(tr(" Camera %1: ").arg(camViewID+1));
    displayStates(QFESPIMB040SimpleCameraConfig::Disconnected);
    if (cmbAcquisitionDevice->count()<=0) displayStates(QFESPIMB040SimpleCameraConfig::Inactive);
}
void QFESPIMB040SimpleCameraConfig::showEvent ( QShowEvent * event ) {
    stopTimer();
    startTimerSingleShot(true);
}

void QFESPIMB040SimpleCameraConfig::startTimerSingleShot(bool connect) {
    this->disconnect(previewTimer, SIGNAL(timeout()), this, SLOT(previewContinuous()));
    if (connect) this->connect(previewTimer, SIGNAL(timeout()), this, SLOT(previewContinuous()));
    previewTimer->setSingleShot(true);
    previewTimer->setInterval(spinAcquisitionDelay->value()+5);
    previewTimer->start();
}

void QFESPIMB040SimpleCameraConfig::stopTimer() {
    previewTimer->stop();
    previewTimer->disconnect();
}

void QFESPIMB040SimpleCameraConfig::closeEvent ( QCloseEvent * event ) {
    stopPreview();
    stopTimer();
    //QApplication::processEvents();
    if (camView) {
        camView->hide();
    }
    // disconnect devices and close camera view:
    disconnectDevice();
    //qDebug()<<"closed";
}

bool QFESPIMB040SimpleCameraConfig::lockCamera(QFExtension** extension, QFExtensionCamera** ecamera, int* camera, QString* previewSettingsFilename) {
    if (locked || (!actDisConnect->isChecked()) || (!viewData.camera) || (!viewData.extension)) {
        *camera=-1;
        *extension=NULL;
        *ecamera=NULL;
        return false;
    }
    restartPreview=actStartStopPreview->isChecked();
    actStartStopPreview->setChecked(false);
    stopPreview();
    //QApplication::processEvents();
    //QApplication::processEvents();
    //QApplication::processEvents();
    displayStates(QFESPIMB040SimpleCameraConfig::Locked);

    QString filename="";
    *previewSettingsFilename="";
    //if (cmbPreviewConfiguration->currentIndex()>=0) *previewSettingsFilename=cmbPreviewConfiguration->itemData(cmbPreviewConfiguration->currentIndex()).toString();
    *previewSettingsFilename=cmbPreviewConfiguration->currentConfigFilename();
    *extension=viewData.extension;
    *ecamera=viewData.camera;
    *camera=viewData.usedCamera;
    locked=true;
    return true;
}

void QFESPIMB040SimpleCameraConfig::releaseCamera() {
    if (locked) {
        displayStates(QFESPIMB040SimpleCameraConfig::Connected);
        locked=false;
        if (restartPreview) {
            actStartStopPreview->setChecked(true);
            startStopPreview();
        }
    }
}


void QFESPIMB040SimpleCameraConfig::loadSettings(QSettings& settings, QString prefix, bool dontLoadDevices) {
    if (camView) camView->loadSettings(settings, prefix+"cam_view/");

    setChecked(settings.value(prefix+"enabled", 0).toBool());
    //cmbAcquisitionDevice->setCurrentIndex(settings.value(prefix+"last_device", 0).toInt());
    if (!dontLoadDevices) cmbAcquisitionDevice->loadSettings(settings, prefix+"device/");
    spinAcquisitionDelay->setValue(settings.value(prefix+"acquisition_delay", 0).toDouble());
    QString def=settings.value(prefix+"preview_config_default", "").toString();
    cmbPreviewConfiguration->setDefaultConfig(def);
    if (def.isEmpty()) {
        cmbPreviewConfiguration->setCurrentConfig(settings.value(prefix+"preview_config", "default").toString());
    }
}

void QFESPIMB040SimpleCameraConfig::loadSettings(QFManyFilesSettings &settings, QString prefix, bool dontLoadDevices)
{
    if (camView) camView->loadSettings(settings, prefix+"cam_view/");

    setChecked(settings.value(prefix+"enabled", 0).toBool());
    //cmbAcquisitionDevice->setCurrentIndex(settings.value(prefix+"last_device", 0).toInt());
    if (!dontLoadDevices) cmbAcquisitionDevice->loadSettings(settings, prefix+"device/");
    spinAcquisitionDelay->setValue(settings.value(prefix+"acquisition_delay", 0).toDouble());
    QString def=settings.value(prefix+"preview_config_default", "").toString();
    cmbPreviewConfiguration->setDefaultConfig(def);
    if (def.isEmpty()) {
        cmbPreviewConfiguration->setCurrentConfig(settings.value(prefix+"preview_config", "default").toString());
    }
}

void QFESPIMB040SimpleCameraConfig::storeSettings(QSettings& settings, QString prefix) {
    if (camView) camView->storeSettings(settings, prefix+"cam_view/");

    settings.setValue(prefix+"enabled", isChecked());
    //settings.setValue(prefix+"last_device", cmbAcquisitionDevice->currentIndex());
    settings.setValue(prefix+"acquisition_delay", spinAcquisitionDelay->value());
    settings.setValue(prefix+"preview_config", cmbPreviewConfiguration->currentConfigName());
    settings.setValue(prefix+"preview_config_default", cmbPreviewConfiguration->getDefaultConfig());
    cmbAcquisitionDevice->storeSettings(settings, prefix+"device/");

}

void QFESPIMB040SimpleCameraConfig::storeSettings(QFManyFilesSettings &settings, QString prefix)
{
    if (camView) camView->storeSettings(settings, prefix+"cam_view/");

    settings.setValue(prefix+"enabled", isChecked());
    //settings.setValue(prefix+"last_device", cmbAcquisitionDevice->currentIndex());
    settings.setValue(prefix+"acquisition_delay", spinAcquisitionDelay->value());
    settings.setValue(prefix+"preview_config", cmbPreviewConfiguration->currentConfigName());
    settings.setValue(prefix+"preview_config_default", cmbPreviewConfiguration->getDefaultConfig());
    cmbAcquisitionDevice->storeSettings(settings, prefix+"device/");
}

void QFESPIMB040SimpleCameraConfig::setCurrentPreview(int id)
{
    QStringList sl=getPreviewConfigs();
    if (id>=0 && id<sl.size()) {
        cmbPreviewConfiguration->setCurrentConfig(sl[id]);
        QApplication::processEvents();
        QApplication::processEvents();
    }
}

void QFESPIMB040SimpleCameraConfig::setCurrentPreview(const QString &id)
{
    cmbPreviewConfiguration->setCurrentConfig(id);
    QApplication::processEvents();
    QApplication::processEvents();
}

QStringList QFESPIMB040SimpleCameraConfig::getPreviewConfigs() const
{
    return cmbPreviewConfiguration->getConfigs();
}



void QFESPIMB040SimpleCameraConfig::createWidgets() {
    previewTimer=new QTimer(this);
    previewTimer->setSingleShot(true);
    previewTimer->setInterval(5);


    ////////////////////////////////////////////////////////////////////////////////////////////////
    // create main layout
    ////////////////////////////////////////////////////////////////////////////////////////////////
    QFormLayout* camlayout=new QFormLayout();
    setLayout(camlayout);


    ////////////////////////////////////////////////////////////////////////////////////////////////
    // create camera view widgets ... and hide them
    ////////////////////////////////////////////////////////////////////////////////////////////////
    camView=new QFESPIMB040CameraView(NULL, m_camViewID, this);
    camView->close();



    ////////////////////////////////////////////////////////////////////////////////////////////////
    // create input widgets for camera devices
    ////////////////////////////////////////////////////////////////////////////////////////////////
    cmbAcquisitionDevice=new QFCameraComboBox(this);
    cmbAcquisitionDevice->setSizeAdjustPolicy(QComboBox::AdjustToContents);

    QHBoxLayout* hbl=new QHBoxLayout();
    hbl->setContentsMargins(0,0,0,0);
    hbl->addWidget(cmbAcquisitionDevice);
    btnConnect=new QToolButton(this);
    hbl->addWidget(btnConnect);
    hbl->addStretch();
    camlayout->addRow(tr("<b>Device:</b>"), hbl);
    cmbAcquisitionDevice->setEnabled(false);
    cmbAcquisitionDevice->setSizeAdjustPolicy(QComboBox::AdjustToContents);



    cmbPreviewConfiguration=new QFCameraConfigEditorWidget(this);
    cmbPreviewConfiguration->connectTo(cmbAcquisitionDevice);
    cmbPreviewConfiguration->setSetCurrentDefaultEnabled(true);

    camlayout->addRow(tr("<b>Preview:</b>"), cmbPreviewConfiguration);
    connect(cmbPreviewConfiguration, SIGNAL(currentIndexChanged(int)), this, SLOT(previewCurrentIndexChanged(int)));
    cmbPreviewConfiguration->setStopResume(this);

    hbl=new QHBoxLayout();
    hbl->setContentsMargins(0,0,0,0);
    spinAcquisitionDelay=new QDoubleSpinBox(this);
    spinAcquisitionDelay->setMinimum(2);
    spinAcquisitionDelay->setSuffix(tr("ms"));
    spinAcquisitionDelay->setMaximum(10000);
    spinAcquisitionDelay->setSingleStep(1);
    spinAcquisitionDelay->setDecimals(0);
    spinAcquisitionDelay->setEnabled(false);
    btnPreviewSingle=new QToolButton(this);
    btnPreviewSingle->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    QFrame* line=new QFrame(this);
    line->setFrameStyle(QFrame::VLine | QFrame::Raised);
    line->setLineWidth(0);
    line->setMidLineWidth(1);
    line->setMinimumWidth(8);
    line->setMaximumWidth(8);
    hbl->addWidget(btnPreviewSingle);
    hbl->addWidget(line);
    hbl->addWidget(new QLabel(tr("delay: "), this));
    hbl->addWidget(spinAcquisitionDelay);
    btnPreviewContinuous=new QToolButton(this);
    btnPreviewContinuous->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    hbl->addWidget(btnPreviewContinuous);
    hbl->addStretch();
    camlayout->addRow("", hbl);
}





void QFESPIMB040SimpleCameraConfig::createActions() {
    actDisConnect = new QAction(QIcon(":/spimb040/acquisitionconnect.png"), tr("&Connect"), this);
    actDisConnect->setCheckable(true);
    connect(actDisConnect, SIGNAL(toggled(bool)), this, SLOT(disConnectAcquisitionDevice()));

    actStartStopPreview = new QAction(QIcon(":/spimb040/acquisitionstart.png"), tr("&Start acquisition"), this);
    actStartStopPreview->setCheckable(true);
    connect(actStartStopPreview, SIGNAL(toggled(bool)), this, SLOT(startStopPreview()));

    actPreviewSingle = new QAction(QIcon(":/spimb040/acquisitionsingle.png"), tr("&Acquire single image"), this);
    connect(actPreviewSingle, SIGNAL(triggered()), this, SLOT(previewSingle()));

    actPreviewConfig=cmbPreviewConfiguration->configAction();

    camView->addUserAction(actPreviewConfig);//cmbPreviewConfiguration->configAction());
    camView->addUserAction(actStartStopPreview);
    camView->addUserAction(actPreviewSingle);
    btnConnect->setDefaultAction(actDisConnect);
    btnPreviewSingle->setDefaultAction(actPreviewSingle);
    btnPreviewContinuous->setDefaultAction(actStartStopPreview);
}

void QFESPIMB040SimpleCameraConfig::displayStates(QFESPIMB040SimpleCameraConfig::States state) {
    if (state==QFESPIMB040SimpleCameraConfig::Connected) {
        setEnabled(true);
        actDisConnect->setIcon(QIcon(":/spimb040/acquisitiondisconnect.png"));
        actDisConnect->setText(tr("&Disconnect"));
        actDisConnect->setEnabled(true);
        actDisConnect->setChecked(true);
        actStartStopPreview->setEnabled(true);
        actStartStopPreview->setChecked(false);
        actStartStopPreview->setIcon(QIcon(":/spimb040/acquisitionstart.png"));
        actStartStopPreview->setText(tr("&Start Preview"));
        actPreviewSingle->setEnabled(true);

        cmbAcquisitionDevice->setEnabled(false);
        cmbPreviewConfiguration->setEnabled(true);
        spinAcquisitionDelay->setEnabled(true);


    } else if (state==QFESPIMB040SimpleCameraConfig::Previewing) {
        setEnabled(true);
        actDisConnect->setIcon(QIcon(":/spimb040/acquisitiondisconnect.png"));
        actDisConnect->setText(tr("&Disconnect"));
        actDisConnect->setEnabled(true);
        actDisConnect->setChecked(true);
        actStartStopPreview->setEnabled(true);
        actStartStopPreview->setChecked(true);
        actStartStopPreview->setIcon(QIcon(":/spimb040/acquisitionstop.png"));
        actStartStopPreview->setText(tr("&Stop Preview"));
        actPreviewSingle->setEnabled(false);


        cmbAcquisitionDevice->setEnabled(false);
        cmbPreviewConfiguration->setEnabled(true);
        spinAcquisitionDelay->setEnabled(true);


    } else if (state==QFESPIMB040SimpleCameraConfig::Inactive) {
        setEnabled(false);
        actDisConnect->setIcon(QIcon(":/spimb040/acquisitionconnect.png"));
        actDisConnect->setText(tr("&Connect"));
        actDisConnect->setEnabled(false);
        actDisConnect->setChecked(false);
        actStartStopPreview->setEnabled(false);
        actStartStopPreview->setChecked(false);
        actStartStopPreview->setIcon(QIcon(":/spimb040/acquisitionstart.png"));
        actStartStopPreview->setText(tr("&Start Preview"));
        actPreviewSingle->setEnabled(false);


        cmbAcquisitionDevice->setEnabled(false);
        cmbPreviewConfiguration->setEnabled(false);
        spinAcquisitionDelay->setEnabled(false);


    } else if (state==QFESPIMB040SimpleCameraConfig::Locked) {
        setEnabled(true);
        actDisConnect->setIcon(QIcon(":/spimb040/acquisitiondisconnect.png"));
        actDisConnect->setText(tr("&Disconnect"));
        actDisConnect->setEnabled(false);
        actDisConnect->setChecked(true);
        actStartStopPreview->setEnabled(false);
        actStartStopPreview->setChecked(false);
        actStartStopPreview->setIcon(QIcon(":/spimb040/acquisitionstart.png"));
        actStartStopPreview->setText(tr("&Start Preview"));
        actPreviewSingle->setEnabled(false);


        cmbAcquisitionDevice->setEnabled(false);
        cmbPreviewConfiguration->setEnabled(false);
        spinAcquisitionDelay->setEnabled(false);
    } else { // Disconnected
        setEnabled(true);
        actDisConnect->setIcon(QIcon(":/spimb040/acquisitionconnect.png"));
        actDisConnect->setText(tr("&Connect"));
        actDisConnect->setEnabled(true);
        actDisConnect->setChecked(false);
        actStartStopPreview->setEnabled(false);
        actStartStopPreview->setChecked(false);
        actStartStopPreview->setIcon(QIcon(":/spimb040/acquisitionstart.png"));
        actStartStopPreview->setText(tr("&Start Preview"));
        actPreviewSingle->setEnabled(false);


        cmbAcquisitionDevice->setEnabled(true);
        cmbPreviewConfiguration->setEnabled(false);
        spinAcquisitionDelay->setEnabled(false);
    }
}


bool QFESPIMB040SimpleCameraConfig::connectDevice(QFExtension* extension, QFExtensionCamera* cam, int camera, QObject* object) {

    viewData.reset();

    locked=false;


    if (cam && extension) {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        viewData.extension=extension;
        viewData.camera=cam;
        viewData.measurementDevice=qobject_cast<QFExtensionMeasurementAndControlDevice*>(object);
        viewData.usedCamera=camera;
        cam->setCameraLogging(m_log);
        bool s=cam->connectCameraDevice(camera);
        if (!s) {
            //cam->setLogging(NULL);
            viewData.reset();
            if (m_log) m_log->log_error(tr("error connecting to device %1, camera %2 ...\n").arg(extension->getName()).arg(camera));
        }
        QApplication::restoreOverrideCursor();
        return s;
    } else {
        return false;
    }
}

void QFESPIMB040SimpleCameraConfig::setLog(QFPluginLogService* logger) {
    m_log=logger;
}


void QFESPIMB040SimpleCameraConfig::disconnectDevice() {
    locked=false;
    if (viewData.camera) {
        if (viewData.camera->isCameraConnected(viewData.usedCamera)) {
            viewData.camera->disconnectCameraDevice(viewData.usedCamera);
        }
    }
}


bool QFESPIMB040SimpleCameraConfig::acquireSingle() {
    if (viewData.camera) {
        int usedcam=viewData.usedCamera;
        if (viewData.camera->isCameraConnected(usedcam)) {
            int dw=viewData.camera->getCameraImageWidth(usedcam);
            int dh=viewData.camera->getCameraImageHeight(usedcam);
            uint64_t timestamp=0;
            viewData.rawImage.resize(dw, dh);
            QMap<QString, QVariant> parameters;
            bool ok=viewData.camera->acquireOnCamera(usedcam, viewData.rawImage.data(), &timestamp, &parameters);
            viewData.parameters=parameters;
            viewData.lastAcquisitionTime=QTime::currentTime();
            double exposure=viewData.camera->getCameraExposureTime(usedcam);
            viewData.timestamp=timestamp;
            viewData.exposureTime=exposure;
            viewData.acqFrames++;
            viewData.acqFramesQR++;
            return ok;
        }
    }
    return false;
}





void QFESPIMB040SimpleCameraConfig::disConnectAcquisitionDevice() {
    if (cmbAcquisitionDevice->currentIndex()<0) {
        // if there are no devices registered: deactivate everything!
        displayStates(QFESPIMB040SimpleCameraConfig::Disconnected);
        return;
    }

    QFExtension* extension=cmbAcquisitionDevice->currentExtension();
    QFExtensionCamera* cam=cmbAcquisitionDevice->currentExtensionCamera();
    int camIdx=cmbAcquisitionDevice->currentCameraID();

    //std::cout<<"disConnectAcquisitionDevice()  dev="<<p.x()<<" cam="<<p.y()<<"  cam*="<<cam<<" extension*="<<extension<<std::endl;

    if (cam && extension) {
        if (!cam->isCameraConnected(camIdx)) {
            // connect to a device

            connectDevice(cmbAcquisitionDevice->currentExtension(), cmbAcquisitionDevice->currentExtensionCamera(), cmbAcquisitionDevice->currentCameraID(), cmbAcquisitionDevice->currentCameraQObject());
            if (!cam->isCameraConnected(camIdx)) {
                displayStates(QFESPIMB040SimpleCameraConfig::Disconnected);
                if (m_log) m_log->log_error(tr("error connecting to device %1, camera %2!\n").arg(extension->getName()).arg(camIdx));
            } else {
                displayStates(QFESPIMB040SimpleCameraConfig::Connected);
                if (m_log) m_log->log_text(tr("connected to device %1, camera %2!\n").arg(extension->getName()).arg(camIdx));
                cmbPreviewConfiguration->setDefaultAsCurrentConfig();
            }
        } else {
            //disconnect from the current device and delete the settings widget
            displayStates(QFESPIMB040SimpleCameraConfig::Disconnected);
            //if (cam->isCameraConnected(camIdx))  {
                cam->disconnectCameraDevice(camIdx);

            //}
            if (m_log) m_log->log_text(tr("disconnected from device %1, camera %2!\n").arg(extension->getName()).arg(camIdx));
        }
    }
}




void QFESPIMB040SimpleCameraConfig::previewSingle() {
    if (viewData.camera) {
        int camIdx=viewData.usedCamera;
        QFExtensionCamera* cam=viewData.camera;
        if (viewData.camera->isCameraConnected(camIdx)) {
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
            QString filename="";
            filename=cmbPreviewConfiguration->currentConfigFilename();
            QSettings* settings=new QSettings(filename, QSettings::IniFormat);
            viewData.camera->useCameraSettings(camIdx, *settings);
            if (acquireSingle()) {
                camView->show();
                camView->setPixelSize(cam->getCameraPixelWidth(camIdx)/m_magnification, cam->getCameraPixelHeight(camIdx)/m_magnification);
                camView->displayImageComplete(viewData.rawImage, viewData.timestamp, viewData.exposureTime, viewData.parameters);
            } else {
                if (m_log) m_log->log_error(tr("could not acquire frame, as device is not connected ...!\n"));
            }
            delete settings;
            QApplication::restoreOverrideCursor();
        }
    }
}



void QFESPIMB040SimpleCameraConfig::stopPreview() {
    //qDebug()<<"stopPreview()";
    if (viewData.camera) {
        int camIdx=viewData.usedCamera;
        if (viewData.camera->isCameraConnected(camIdx)) {
            //qDebug()<<"   cam connected";
            viewData.abort_continuous_acquisition=true;
            actStartStopPreview->setChecked(false);
            displayStates(QFESPIMB040SimpleCameraConfig::Connected);
            viewData.abort_continuous_acquisition=true;
            stopTimer();
        }
    }
}


void QFESPIMB040SimpleCameraConfig::startStopPreview() {
    //qDebug()<<"startStopPreview() actStartSTop="<<actStartStopPreview->isChecked();
    if (viewData.camera) {
        int camIdx=viewData.usedCamera;
        if (viewData.camera->isCameraConnected(camIdx)) {
            if (actStartStopPreview->isChecked()) {
                displayStates(QFESPIMB040SimpleCameraConfig::Previewing);
                viewData.acqFrames=0;
                viewData.acqFramesQR=0;
                viewData.acqStarted.start();
                viewData.acqStartedQR.start();
                viewData.abort_continuous_acquisition=false;
                viewData.continuous_is_first=true;
                QString filename="";
                filename=cmbPreviewConfiguration->currentConfigFilename();
                QSettings* settings=new QSettings(filename, QSettings::IniFormat);
                viewData.camera->useCameraSettings(camIdx, *settings);
                camView->show();
                startTimerSingleShot(true);
                delete settings;
            } else {
                displayStates(QFESPIMB040SimpleCameraConfig::Connected);
            }
        }
    }
}



void QFESPIMB040SimpleCameraConfig::previewContinuous() {

    if (!camView) return;
    static int cnt=0;
    cnt++;
    if (viewData.camera) {
        int camIdx=viewData.usedCamera;
        QFExtension* extension=viewData.extension;
        QFExtensionCamera* cam=viewData.camera;
        //qDebug()<<"preview "<<cnt<<":  abort="<<viewData.abort_continuous_acquisition;
        if (viewData.abort_continuous_acquisition) {
            displayStates(QFESPIMB040SimpleCameraConfig::Connected);
            //qDebug()<<"preview_abort "<<cnt;
            cnt--;
            return;
        }

        if (cam->isCameraConnected(camIdx)) {
            if (viewData.continuous_is_first) {
                viewData.continuous_is_first=false;
            }
            if (acquireSingle()) {
                camView->setPixelSize(cam->getCameraPixelWidth(camIdx)/m_magnification, cam->getCameraPixelHeight(camIdx)/m_magnification);
                camView->displayImage(viewData.rawImage, viewData.timestamp, viewData.exposureTime, viewData.parameters);
            }
        } else {
            if (m_log) m_log->log_error(tr("could not acquire frame, as device is not connected ...!\n"));
        }


        if (viewData.acqFramesQR%5==0) {
            double framerate=(double)viewData.acqFramesQR/(double)(viewData.acqStartedQR.elapsed()/1000.0);
            camView->displayCameraConfig(extension->getName()+" #"+QString::number(camIdx), framerate);
            if (viewData.acqStartedQR.elapsed()>10000) {
                viewData.acqStartedQR.start();
                viewData.acqFramesQR=0;
            }
        }
        viewData.abort_continuous_acquisition=false;


        // start timer till next acquisition
        if (actStartStopPreview->isChecked()) {
            startTimerSingleShot(true);
        }
        cnt--;
    }
}


void QFESPIMB040SimpleCameraConfig::stop() {
    //qDebug()<<"stop";
    restartPreview=actStartStopPreview->isChecked();
    stopPreview();
    QElapsedTimer timer;
    timer.start();
    while (timer.elapsed()<150) {
        QApplication::processEvents();
    }
}

void QFESPIMB040SimpleCameraConfig::resume() {
    //qDebug()<<"resume";
    if (restartPreview) {
        actStartStopPreview->setChecked(true);
        startStopPreview();
        QElapsedTimer timer;
        timer.start();
        while (timer.elapsed()<150) {
            QApplication::processEvents();
        }
    }
}

void QFESPIMB040SimpleCameraConfig::previewCurrentIndexChanged(int index) {
    emit previewConfigChanged();
    if (actStartStopPreview->isChecked()) { // just stop & resume, as this will load the new settings in the resume operation!
        stopPreview();
        actStartStopPreview->setChecked(true);
        startStopPreview();
    }
}

void QFESPIMB040SimpleCameraConfig::connectCamera() {
    if (!isChecked()) return;

    bool b=actDisConnect->signalsBlocked();
    actDisConnect->blockSignals(true);
    actDisConnect->setChecked(true);
    disConnectAcquisitionDevice();
    actDisConnect->blockSignals(b);

}

void QFESPIMB040SimpleCameraConfig::disconnectCamera() {
    if (!isChecked()) return;

    bool b=actDisConnect->signalsBlocked();
    actDisConnect->blockSignals(true);
    actDisConnect->setChecked(false);
    disConnectAcquisitionDevice();
    actDisConnect->blockSignals(b);
}

void QFESPIMB040SimpleCameraConfig::setReadOnly(bool readonly) {
    cmbAcquisitionDevice->setReadOnly(readonly);
}

void QFESPIMB040SimpleCameraConfig::overridePreview(const QString &previewfilename)
{
    if (!isCameraConnected()) {
        return;
    }
    //qDebug()<<"overridePreview: start: m_lastUserPreview="<<m_lastUserPreview<<"   new="<<cmbPreviewConfiguration->currentConfigFilename();
    if (QFile::exists(previewfilename)) {
        if (!QFile::exists(m_lastUserPreview) || m_lastUserPreview.isEmpty()) {
            m_lastUserPreview=cmbPreviewConfiguration->currentConfigFilename();
        }
        cmbPreviewConfiguration->setCurrentConfigFile(previewfilename);

        //qDebug()<<"overridePreview: end: m_lastUserPreview="<<m_lastUserPreview<<"   new="<<cmbPreviewConfiguration->currentConfigFilename();
    }
}

void QFESPIMB040SimpleCameraConfig::resetPreview()
{
    if (!isCameraConnected()) {
        return;
    }
    //qDebug()<<"resetPreview: start: m_lastUserPreview="<<m_lastUserPreview;
    if (QFile::exists(m_lastUserPreview)) {
        cmbPreviewConfiguration->setCurrentConfigFile(m_lastUserPreview);
        m_lastUserPreview="";
    }
    //qDebug()<<"resetPreview: end: m_lastUserPreview="<<m_lastUserPreview;
}

bool QFESPIMB040SimpleCameraConfig::isCameraConnected() const {
    if (viewData.camera) {
        int camIdx=viewData.usedCamera;
        return viewData.camera->isCameraConnected(camIdx);
    }
    return false;
}

void QFESPIMB040SimpleCameraConfig::storeMeasurements(QMap<QString, QVariant> &data, const QString &prefix) {
    QFExtensionMeasurementAndControlDevice* md=viewData.measurementDevice;
    int camIdx=viewData.usedCamera;
    //qDebug()<<"camera   extension="<<viewData.extension<<"   md="<<viewData.measurementDevice<<"   camIdx="<<camIdx<<"    md="<<md;
    if (md) {
        //qDebug()<<"camera"<<camIdx<<" is measurement device connected: "<<md->isMeasurementDeviceConnected(camIdx);
        for (unsigned int i=0; i<md->getMeasurementDeviceValueCount(camIdx); i++) {
            QString n=md->getMeasurementDeviceValueShortName(camIdx, i);
            data[prefix+n]=md->getMeasurementDeviceValue(camIdx, i);
            //qDebug()<<"   "<<i+1<<"/"<<md->getMeasurementDeviceValueCount(camIdx)<<":   "<<prefix+md->getMeasurementDeviceValueShortName(camIdx, i)<<"="<<md->getMeasurementDeviceValue(camIdx, i);
            //qDebug()<<"   "<<i+1<<"/"<<md->getMeasurementDeviceValueCount(camIdx)<<":   "<<prefix+n<<"="<<data[prefix+n];
        }
    }
}
