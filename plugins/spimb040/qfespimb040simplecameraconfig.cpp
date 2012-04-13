#include "qfespimb040simplecameraconfig.h"
#include <QtGui>
#include <iostream>

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


    // initialize raw image memory ...
    viewData.reset();

    // create widgets and actions

    createWidgets();
    createActions();
    displayStates(QFESPIMB040SimpleCameraConfig::Disconnected);
    if (cmbAcquisitionDevice->count()<=0) displayStates(QFESPIMB040SimpleCameraConfig::Inactive);
    init(0, NULL, "");
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

void QFESPIMB040SimpleCameraConfig::init(int camViewID, QFPluginServices* pluginServices, QString configDirectory) {
    m_camViewID=camViewID;
    m_pluginServices=pluginServices;
    m_extManager=NULL;
    if (pluginServices) m_extManager=pluginServices->getExtensionManager();
    cmbAcquisitionDevice->init(m_extManager);
    cmbPreviewConfiguration->init(configDirectory);
    if (m_pluginServices) {
        cmbPreviewConfiguration->init(m_pluginServices->getConfigFileDirectory());
    }
    camView->init(camViewID, this);
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
    if (actDisConnect->isChecked()) {
        actDisConnect->setChecked(false);
        disconnectDevice();
    }
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


void QFESPIMB040SimpleCameraConfig::loadSettings(QSettings& settings, QString prefix) {
    if (camView) camView->loadSettings(settings, prefix+"cam_view/");

    setChecked(settings.value(prefix+"enabled", 0).toBool());
    //cmbAcquisitionDevice->setCurrentIndex(settings.value(prefix+"last_device", 0).toInt());
    cmbAcquisitionDevice->loadSettings(settings, prefix+"device/");
    spinAcquisitionDelay->setValue(settings.value(prefix+"acquisition_delay", 0).toDouble());
    cmbPreviewConfiguration->setCurrentConfig(settings.value(prefix+"preview_config", "default").toString());
}

void QFESPIMB040SimpleCameraConfig::storeSettings(QSettings& settings, QString prefix) {
    if (camView) camView->storeSettings(settings, prefix+"cam_view/");

    settings.setValue(prefix+"enabled", isChecked());
    //settings.setValue(prefix+"last_device", cmbAcquisitionDevice->currentIndex());
    settings.setValue(prefix+"acquisition_delay", spinAcquisitionDelay->value());
    settings.setValue(prefix+"preview_config", cmbPreviewConfiguration->currentConfigName());//currentConfigFilename());
    cmbAcquisitionDevice->storeSettings(settings, prefix+"device/");

}



void QFESPIMB040SimpleCameraConfig::createWidgets() {
    previewTimer=new QTimer(this);
    previewTimer->setSingleShot(true);
    previewTimer->setInterval(5);


    ////////////////////////////////////////////////////////////////////////////////////////////////
    // create main layout
    ////////////////////////////////////////////////////////////////////////////////////////////////
    QFormLayout* camlayout=new QFormLayout(this);
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

    QHBoxLayout* hbl=new QHBoxLayout(this);
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

    camlayout->addRow(tr("<b>Preview:</b>"), cmbPreviewConfiguration);
    connect(cmbPreviewConfiguration, SIGNAL(currentIndexChanged(int)), this, SLOT(previewCurrentIndexChanged(int)));
    cmbPreviewConfiguration->setStopResume(this);

    hbl=new QHBoxLayout(this);
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


bool QFESPIMB040SimpleCameraConfig::connectDevice(QFExtension* extension, QFExtensionCamera* cam, int camera) {

    viewData.reset();

    locked=false;


    if (cam && extension) {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        viewData.extension=extension;
        viewData.camera=cam;
        viewData.usedCamera=camera;
        cam->setLogging(m_log);
        bool s=cam->connectDevice(camera);
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
        if (viewData.camera->isConnected(viewData.usedCamera)) {
            viewData.camera->disconnectDevice(viewData.usedCamera);
        }
    }
}


bool QFESPIMB040SimpleCameraConfig::acquireSingle() {
    if (viewData.camera) {
        int usedcam=viewData.usedCamera;
        if (viewData.camera->isConnected(usedcam)) {
            int dw=viewData.camera->getImageWidth(usedcam);
            int dh=viewData.camera->getImageHeight(usedcam);
            uint64_t timestamp=0;
            viewData.rawImage.resize(dw, dh);
            bool ok=viewData.camera->acquire(usedcam, viewData.rawImage.data(), &timestamp);
            viewData.lastAcquisitionTime=QTime::currentTime();
            double exposure=viewData.camera->getExposureTime(usedcam);
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
        if (actDisConnect->isChecked()) {
            // connect to a device

            connectDevice(cmbAcquisitionDevice->currentExtension(), cmbAcquisitionDevice->currentExtensionCamera(), cmbAcquisitionDevice->currentCameraID());
            if (!cam->isConnected(camIdx)) {
                displayStates(QFESPIMB040SimpleCameraConfig::Disconnected);
                if (m_log) m_log->log_error(tr("error connecting to device %1, camera %2!\n").arg(extension->getName()).arg(camIdx));
            } else {
                displayStates(QFESPIMB040SimpleCameraConfig::Connected);
                if (m_log) m_log->log_text(tr("connected to device %1, camera %2!\n").arg(extension->getName()).arg(camIdx));

            }
        } else {
            //disconnect from the current device and delete the settings widget
            displayStates(QFESPIMB040SimpleCameraConfig::Disconnected);
            if (cam->isConnected(camIdx))  {
                cam->disconnectDevice(camIdx);

            }
            if (m_log) m_log->log_text(tr("disconnected from device %1, camera %2!\n").arg(extension->getName()).arg(camIdx));
        }
    }
}




void QFESPIMB040SimpleCameraConfig::previewSingle() {
    if (viewData.camera) {
        int camIdx=viewData.usedCamera;
        QFExtensionCamera* cam=viewData.camera;
        if (viewData.camera->isConnected(camIdx)) {
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
            QString filename="";
            filename=cmbPreviewConfiguration->currentConfigFilename();
            QSettings* settings=new QSettings(filename, QSettings::IniFormat);
            viewData.camera->useCameraSettings(camIdx, *settings);
            if (acquireSingle()) {
                camView->show();
                camView->setPixelSize(cam->getPixelWidth(camIdx)/m_magnification, cam->getPixelHeight(camIdx)/m_magnification);
                camView->displayImageComplete(viewData.rawImage, viewData.timestamp, viewData.exposureTime);
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
        if (viewData.camera->isConnected(camIdx)) {
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
        if (viewData.camera->isConnected(camIdx)) {
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

        if (cam->isConnected(camIdx)) {
            if (viewData.continuous_is_first) {
                viewData.continuous_is_first=false;
            }
            if (acquireSingle()) {
                camView->setPixelSize(cam->getPixelWidth(camIdx)/m_magnification, cam->getPixelHeight(camIdx)/m_magnification);
                camView->displayImage(viewData.rawImage, viewData.timestamp, viewData.exposureTime);
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
}

void QFESPIMB040SimpleCameraConfig::resume() {
    //qDebug()<<"resume";
    if (restartPreview) {
        actStartStopPreview->setChecked(true);
        startStopPreview();
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
    actDisConnect->setChecked(true);
}

void QFESPIMB040SimpleCameraConfig::disconnectCamera() {
    if (!isChecked()) return;
    actDisConnect->setChecked(false);
}

void QFESPIMB040SimpleCameraConfig::setReadOnly(bool readonly) {
    cmbAcquisitionDevice->setReadOnly(readonly);
}
