#include "qfcameraconfigwidget.h"
#include <QtGui>
#include <iostream>
#include "qfespimb040mainwindow.h"

QFCameraConfigWidget::viewDataStruct::viewDataStruct() {
    reset();
}

void QFCameraConfigWidget::viewDataStruct::reset() {
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


QFCameraConfigNotifier* QFCameraConfigWidget::m_notifier=NULL;



QFCameraConfigWidget::QFCameraConfigWidget(QFESPIMB040MainWindow* parent, int camViewID, QFPluginServices* pluginServices):
    QGroupBox(parent)
{
    if (m_notifier==NULL) m_notifier=new QFCameraConfigNotifier(this);
    m_camViewID=camViewID;
    m_parent=parent;
    m_pluginServices=pluginServices;
    m_extManager=pluginServices->getExtensionManager();
    camView=NULL;
    locked=false;
    restartPreview=false;


    // initialize raw image memory ...
    viewData.reset();

    // create widgets and actions
    setTitle(tr(" Camera %1: ").arg(camViewID+1));
    createWidgets(m_pluginServices->getExtensionManager());
    createActions();
    displayStates(QFCameraConfigWidget::Disconnected);
    if (cmbAcquisitionDevice->count()<=0) displayStates(QFCameraConfigWidget::Inactive);
    connect(m_notifier, SIGNAL(doUpdate()), this, SLOT(loadObjectives()));
}


QFCameraConfigWidget::~QFCameraConfigWidget() {
    //qDebug()<<"destroy";
    if (camView) {
        camView->close();
        delete camView;
        camView=NULL;
    }
    //qDebug()<<"destroy done";
}


void QFCameraConfigWidget::closeEvent ( QCloseEvent * event ) {
    stopPreview();
    previewTimer->stop();
    previewTimer->disconnect();
    //QApplication::processEvents();
    if (camView) {
        camView->close();
    }
    // disconnect devices and close camera view:
    if (actDisConnect->isChecked()) {
        actDisConnect->setChecked(false);
        disconnectDevice();
    }
    //qDebug()<<"closed";
}

bool QFCameraConfigWidget::lockCamera(QFExtension** extension, QFExtensionCamera** ecamera, int* camera, QString* acquisitionSettingsFilename, QString* previewSettingsFilename) {
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
    displayStates(QFCameraConfigWidget::Locked);

    QString filename="";
    *previewSettingsFilename="";
    *acquisitionSettingsFilename="";
    *acquisitionSettingsFilename=cmbAcquisitionConfiguration->currentConfigFilename();
    //if (cmbPreviewConfiguration->currentIndex()>=0) *previewSettingsFilename=cmbPreviewConfiguration->itemData(cmbPreviewConfiguration->currentIndex()).toString();
    *previewSettingsFilename=cmbPreviewConfiguration->currentConfigFilename();
    *extension=viewData.extension;
    *ecamera=viewData.camera;
    *camera=viewData.usedCamera;
    locked=true;
    return true;
}

void QFCameraConfigWidget::releaseCamera() {
    if (locked) {
        displayStates(QFCameraConfigWidget::Connected);
        locked=false;
        if (restartPreview) {
            actStartStopPreview->setChecked(true);
            startStopPreview();
        }
    }
}


void QFCameraConfigWidget::loadSettings(ProgramOptions* settings, QString prefix) {
    if (camView) camView->loadSettings(*(settings->getQSettings()), prefix+"cam_view/");

    loadObjectives();

    cmbAcquisitionDevice->setCurrentIndex(settings->getQSettings()->value(prefix+"last_device", 0).toInt());
    spinAcquisitionDelay->setValue(settings->getQSettings()->value(prefix+"acquisition_delay", 0).toDouble());
    cmbObjective->setCurrentIndex(cmbObjective->findText(settings->getQSettings()->value(prefix+"objective", "").toString()));
    cmbObjectiveProjection->setCurrentIndex(cmbObjectiveProjection->findText(settings->getQSettings()->value(prefix+"objective_projection", "").toString()));
    cmbTubelens->setCurrentIndex(cmbTubelens->findText(settings->getQSettings()->value(prefix+"tubelens", "").toString()));
    cmbPreviewConfiguration->setCurrentConfig(settings->getQSettings()->value(prefix+"preview_config", "default").toString());
    cmbAcquisitionConfiguration->setCurrentConfig(settings->getQSettings()->value(prefix+"acquisition_config", "default").toString());
}

void QFCameraConfigWidget::storeSettings(ProgramOptions* settings, QString prefix) {
    if (camView) camView->storeSettings(*(settings->getQSettings()), prefix+"cam_view/");

    settings->getQSettings()->setValue(prefix+"last_device", cmbAcquisitionDevice->currentIndex());
    settings->getQSettings()->setValue(prefix+"acquisition_delay", spinAcquisitionDelay->value());
    settings->getQSettings()->setValue(prefix+"objective", cmbObjective->currentText());
    settings->getQSettings()->setValue(prefix+"objective_projection", cmbObjectiveProjection->currentText());
    settings->getQSettings()->setValue(prefix+"tubelens", cmbTubelens->currentText());
    settings->getQSettings()->setValue(prefix+"acquisition_config", cmbAcquisitionConfiguration->currentConfigName());//currentConfigFilename());
    settings->getQSettings()->setValue(prefix+"preview_config", cmbPreviewConfiguration->currentConfigName());//currentConfigFilename());

}



void QFCameraConfigWidget::createWidgets(QFExtensionManager* extManager) {
    previewTimer=new QTimer(this);
    previewTimer->setSingleShot(true);
    previewTimer->setInterval(5);
    previewTimer->stop();
    connect(previewTimer, SIGNAL(timeout()), this, SLOT(previewContinuous()));

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
    cmbAcquisitionDevice=new QFCameraComboBox(extManager, this);
    cmbAcquisitionDevice->setSizeAdjustPolicy(QComboBox::AdjustToContents);

    QHBoxLayout* hbl=new QHBoxLayout(this);
    hbl->setContentsMargins(0,0,0,0);
    hbl->addWidget(cmbAcquisitionDevice);
    btnConnect=new QToolButton(this);
    hbl->addWidget(btnConnect);
    camlayout->addRow(tr("<b>Device:</b>"), hbl);
    cmbAcquisitionDevice->setEnabled(false);


    QGridLayout* ogl=new QGridLayout(this);
    ogl->setContentsMargins(0,0,0,0);
    ogl->setHorizontalSpacing(1);
    ogl->setVerticalSpacing(1);
    ogl->addWidget(new QLabel("detection:"), 0,0);
    ogl->addWidget(new QLabel("lightsheet:"), 1,0);
    ogl->addWidget(new QLabel("tube lens:"), 2,0);
    cmbObjective=new QComboBox(this);
    cmbObjective->setSizeAdjustPolicy(QComboBox::AdjustToContents);

    connect(cmbObjective, SIGNAL(currentIndexChanged(int)), this, SLOT(currentObjectiveChanged(int)));
    ogl->addWidget(cmbObjective, 0,1);
    cmbObjectiveProjection=new QComboBox(this);
    cmbObjectiveProjection->setSizeAdjustPolicy(QComboBox::AdjustToContents);

    connect(cmbObjectiveProjection, SIGNAL(currentIndexChanged(int)), this, SLOT(currentObjectiveChanged(int)));
    ogl->addWidget(cmbObjectiveProjection, 1,1);
    cmbTubelens=new QComboBox(this);
    connect(cmbTubelens, SIGNAL(currentIndexChanged(int)), this, SLOT(currentObjectiveChanged(int)));
    ogl->addWidget(cmbTubelens, 2,1);
    btnAddObjective=new QToolButton(this);
    btnAddObjective->setToolTip(tr("add a new objective"));
    btnAddObjective->setIcon(QIcon(":/spimb040/objective_add.png"));
    btnAddObjective->setEnabled(objectivesWritable());
    connect(btnAddObjective, SIGNAL(clicked()), this, SLOT(addObjective()));
    ogl->addWidget(btnAddObjective, 0,2);
    btnDeleteObjective=new QToolButton(this);
    btnDeleteObjective->setToolTip(tr("delete an objective"));
    btnDeleteObjective->setIcon(QIcon(":/spimb040/objective_delete.png"));
    btnDeleteObjective->setEnabled(objectivesWritable());
    connect(btnDeleteObjective, SIGNAL(clicked()), this, SLOT(deleteObjective()));
    ogl->addWidget(btnDeleteObjective, 0,3);
    btnEditObjective=new QToolButton(this);
    btnEditObjective->setToolTip(tr("edit an objective"));
    btnEditObjective->setIcon(QIcon(":/spimb040/objective_rename.png"));
    btnEditObjective->setEnabled(objectivesWritable());
    connect(btnEditObjective, SIGNAL(clicked()), this, SLOT(editObjective()));
    ogl->addWidget(btnEditObjective, 0,4);
    labDetectObjectiveDescription=new QLabel(this);
    ogl->addWidget(labDetectObjectiveDescription, 0,5);
    labProjectObjectiveDescription=new QLabel(this);
    ogl->addWidget(labProjectObjectiveDescription, 1,5);
    labTubelensDescription=new QLabel(this);
    ogl->addWidget(labTubelensDescription, 2,5);
    ogl->setColumnStretch(5, 1);

    camlayout->addRow(tr("<b>Objectives:</b>"), ogl);

    cmbPreviewConfiguration=new QFCameraConfigEditorWidget(this, m_pluginServices->getConfigFileDirectory());
    cmbPreviewConfiguration->connectTo(cmbAcquisitionDevice);
    camlayout->addRow(tr("<b>Preview:</b>"), cmbPreviewConfiguration);
    connect(cmbPreviewConfiguration, SIGNAL(currentIndexChanged(int)), this, SLOT(previewCurrentIndexChanged(int)));
    cmbPreviewConfiguration->setStopResume(this);

    hbl=new QHBoxLayout(this);
    hbl->setContentsMargins(0,0,0,0);
    spinAcquisitionDelay=new QDoubleSpinBox(this);
    spinAcquisitionDelay->setMinimum(2);
    spinAcquisitionDelay->setSuffix(tr(" ms"));
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




    cmbAcquisitionConfiguration=new QFCameraConfigEditorWidget(this, m_pluginServices->getConfigFileDirectory());
    cmbAcquisitionConfiguration->connectTo(cmbAcquisitionDevice);
    camlayout->addRow(tr("<b>Acquisition:</b>"), cmbAcquisitionConfiguration);
}





void QFCameraConfigWidget::createActions() {
    actDisConnect = new QAction(QIcon(":/spimb040/acquisitionconnect.png"), tr("&Connect"), this);
    actDisConnect->setCheckable(true);
    connect(actDisConnect, SIGNAL(triggered()), this, SLOT(disConnectAcquisitionDevice()));

    actStartStopPreview = new QAction(QIcon(":/spimb040/acquisitionstart.png"), tr("&start acq."), this);
    actStartStopPreview->setToolTip(tr("start a continuous preview acquisition with the given frame delay"));
    actStartStopPreview->setCheckable(true);
    connect(actStartStopPreview, SIGNAL(triggered()), this, SLOT(startStopPreview()));

    actPreviewSingle = new QAction(QIcon(":/spimb040/acquisitionsingle.png"), tr("&Acq. single"), this);
    actPreviewSingle->setToolTip(tr("acquire a single frame and show it in the preview window"));
    connect(actPreviewSingle, SIGNAL(triggered()), this, SLOT(previewSingle()));

    actPreviewConfig=cmbPreviewConfiguration->configAction();
    //actPreviewConfig= new QAction(QIcon(":/spimb040/stagesettings.png"), tr("&Configure preview settings"), this);
    //connect(actPreviewConfig, SIGNAL(triggered()), this, SLOT(configPreview()));


    camView->addUserAction(actPreviewConfig);//cmbPreviewConfiguration->configAction());
    camView->addUserAction(actStartStopPreview);
    camView->addUserAction(actPreviewSingle);
    btnConnect->setDefaultAction(actDisConnect);
    btnPreviewSingle->setDefaultAction(actPreviewSingle);
    btnPreviewContinuous->setDefaultAction(actStartStopPreview);



}

void QFCameraConfigWidget::displayStates(QFCameraConfigWidget::States state) {
    if (state==QFCameraConfigWidget::Connected) {
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
        cmbAcquisitionConfiguration->setEnabled(true);
        spinAcquisitionDelay->setEnabled(true);


    } else if (state==QFCameraConfigWidget::Previewing) {
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
        cmbAcquisitionConfiguration->setEnabled(true);
        spinAcquisitionDelay->setEnabled(true);


    } else if (state==QFCameraConfigWidget::Inactive) {
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
        cmbAcquisitionConfiguration->setEnabled(false);
        spinAcquisitionDelay->setEnabled(false);


    } else if (state==QFCameraConfigWidget::Locked) {
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
        cmbAcquisitionConfiguration->setEnabled(false);
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
        cmbAcquisitionConfiguration->setEnabled(false);
        spinAcquisitionDelay->setEnabled(false);
    }
}


bool QFCameraConfigWidget::connectDevice(QFExtension* extension, QFExtensionCamera* cam, int camera) {

    viewData.reset();

    locked=false;


    if (cam && extension) {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        viewData.extension=extension;
        viewData.camera=cam;
        viewData.usedCamera=camera;
        cam->setLogging(m_parent);
        bool s=cam->connectDevice(camera);
        if (!s) {
            //cam->setLogging(NULL);
            viewData.reset();
            if (m_parent) m_parent->log_error(tr("error connecting to device %1, camera %2 ...\n").arg(extension->getName()).arg(camera));
        }
        QApplication::restoreOverrideCursor();
        return s;
    } else {
        return false;
    }
}


void QFCameraConfigWidget::disconnectDevice() {
    locked=false;
    if (viewData.camera) {
        if (viewData.camera->isConnected(viewData.usedCamera)) {
            viewData.camera->disconnectDevice(viewData.usedCamera);
        }
    }
}


bool QFCameraConfigWidget::acquireSingle() {
    if (viewData.camera) {
        int usedcam=viewData.usedCamera;
        if (viewData.camera->isConnected(usedcam)) {
            //viewData.camera->acquire(usedcam);
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





void QFCameraConfigWidget::disConnectAcquisitionDevice() {
    if (cmbAcquisitionDevice->currentIndex()<0) {
        // if there are no devices registered: deactivate everything!
        displayStates(QFCameraConfigWidget::Disconnected);
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
                displayStates(QFCameraConfigWidget::Disconnected);
                if (m_parent) m_parent->log_error(tr("error connecting to device %1, camera %2!\n").arg(extension->getName()).arg(camIdx));
            } else {
                displayStates(QFCameraConfigWidget::Connected);
                if (m_parent) m_parent->log_text(tr("connected to device %1, camera %2!\n").arg(extension->getName()).arg(camIdx));
                //camView->show();
                //previewSingle();
            }
        } else {
            //disconnect from the current device and delete the settings widget
            displayStates(QFCameraConfigWidget::Disconnected);
            if (cam->isConnected(camIdx))  {
                cam->disconnectDevice(camIdx);
                //cam->setLogging(NULL);
            }
            if (m_parent) m_parent->log_text(tr("disconnected from device %1, camera %2!\n").arg(extension->getName()).arg(camIdx));
        }
    }
}




void QFCameraConfigWidget::previewSingle() {
    if (viewData.camera) {
        int camIdx=viewData.usedCamera;
        //QFExtension* extension=viewData.extension;
        QFExtensionCamera* cam=viewData.camera;
        if (viewData.camera->isConnected(camIdx)) {
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
            QString filename="";
            //if (cmbPreviewConfiguration->currentIndex()>=0) filename=cmbPreviewConfiguration->itemData(cmbPreviewConfiguration->currentIndex()).toString();
            filename=cmbPreviewConfiguration->currentConfigFilename();
            QSettings* settings=new QSettings(filename, QSettings::IniFormat);
            viewData.camera->useCameraSettings(camIdx, *settings);
            if (acquireSingle()) {
                camView->show();
                camView->setPixelSize(cam->getPixelWidth(camIdx)/magnification(), cam->getPixelHeight(camIdx)/magnification());
                camView->displayImageComplete(viewData.rawImage, viewData.timestamp, viewData.exposureTime);
            } else {
                if (m_parent) m_parent->log_error(tr("could not acquire frame, as device is not connected ...!\n"));
            }
            delete settings;
            QApplication::restoreOverrideCursor();
        }
    }
}



void QFCameraConfigWidget::stopPreview() {
    //qDebug()<<"stopPreview()";
    if (viewData.camera) {
        int camIdx=viewData.usedCamera;
        if (viewData.camera->isConnected(camIdx)) {
            //qDebug()<<"   cam connected";
            viewData.abort_continuous_acquisition=true;
            actStartStopPreview->setChecked(false);
            displayStates(QFCameraConfigWidget::Connected);
            //QApplication::processEvents();
            viewData.abort_continuous_acquisition=true;
            previewTimer->stop();
        }
    }
}


void QFCameraConfigWidget::startStopPreview() {
    //qDebug()<<"startStopPreview() actStartSTop="<<actStartStopPreview->isChecked();
    if (viewData.camera) {
        int camIdx=viewData.usedCamera;
        //QFExtension* extension=viewData.extension;
        //QFExtensionCamera* cam=viewData.camera;
        if (viewData.camera->isConnected(camIdx)) {
            if (actStartStopPreview->isChecked()) {
                displayStates(QFCameraConfigWidget::Previewing);
                viewData.acqFrames=0;
                viewData.acqFramesQR=0;
                viewData.acqStarted.start();
                viewData.acqStartedQR.start();
                viewData.abort_continuous_acquisition=false;
                viewData.continuous_is_first=true;
                QString filename="";
                //if (cmbPreviewConfiguration->currentIndex()>=0) filename=cmbPreviewConfiguration->itemData(cmbPreviewConfiguration->currentIndex()).toString();
                filename=cmbPreviewConfiguration->currentConfigFilename();
                QSettings* settings=new QSettings(filename, QSettings::IniFormat);
                viewData.camera->useCameraSettings(camIdx, *settings);
                camView->show();
                //previewContinuous();
                previewTimer->setSingleShot(true);
                previewTimer->setInterval(spinAcquisitionDelay->value()+2);
                disconnect(previewTimer, SIGNAL(timeout()), this, SLOT(previewContinuous()));
                connect(previewTimer, SIGNAL(timeout()), this, SLOT(previewContinuous()));
                previewTimer->start();
                delete settings;
            } else {
                displayStates(QFCameraConfigWidget::Connected);
            }
        }
    }
}



void QFCameraConfigWidget::previewContinuous() {
    if (!camView) return;
    static int cnt=0;
    cnt++;
    if (viewData.camera) {
        int camIdx=viewData.usedCamera;
        QFExtension* extension=viewData.extension;
        QFExtensionCamera* cam=viewData.camera;
        //qDebug()<<"preview "<<cnt<<":  abort="<<viewData.abort_continuous_acquisition;
        if (viewData.abort_continuous_acquisition) {
            displayStates(QFCameraConfigWidget::Connected);
            //qDebug()<<"preview_abort "<<cnt;
            cnt--;
            return;
        }

        if (cam->isConnected(camIdx)) {
            if (viewData.continuous_is_first) {
                viewData.continuous_is_first=false;
            }
            if (acquireSingle()) {
                camView->setPixelSize(cam->getPixelWidth(camIdx)/magnification(), cam->getPixelHeight(camIdx)/magnification());
                camView->displayImage(viewData.rawImage, viewData.timestamp, viewData.exposureTime);
            }
        } else {
            if (m_parent) m_parent->log_error(tr("could not acquire frame, as device is not connected ...!\n"));
        }


        if (viewData.acqFramesQR%5==0) {
            double framerate=(double)viewData.acqFramesQR/(double)(viewData.acqStartedQR.elapsed()/1000.0);
            //labVideoSettings->setText(tr("%1 x %2 : %3 fps     ").arg(image.width()).arg(image.height()).arg(framerate));
            camView->displayCameraConfig(extension->getName()+" #"+QString::number(camIdx), framerate);
            if (viewData.acqStartedQR.elapsed()>10000) {
                viewData.acqStartedQR.start();
                viewData.acqFramesQR=0;
            }
        }
        viewData.abort_continuous_acquisition=false;


        // start timer till next acquisition
        if (actStartStopPreview->isChecked()) {
            //QApplication::processEvents();
            //QTimer::singleShot(spinAcquisitionDelay->value()+3, this, SLOT(previewContinuous()));
            previewTimer->setSingleShot(true);
            previewTimer->setInterval(spinAcquisitionDelay->value()+2);
            previewTimer->start();
        }
        /*} else {
            actStartStopAcquisition->setChecked(false);
            actStartStopAcquisition->setIcon(QIcon(":/spimb040/acquisitionstart.png"));
            actStartStopAcquisition->setText(tr("&Start Acquisition"));
            actStartStopAcquisition->setEnabled(false);
            actAcquireSingle->setEnabled(false);
            actCameraConfig->setEnabled(false);
        }*/
        //qDebug()<<"preview_done "<<cnt;
        cnt--;
    }
}

double QFCameraConfigWidget::magnification() {
    return objective().magnification*tubelens().magnification;
}

bool QFCameraConfigWidget::objectivesWritable() const {
    return QSettings(m_pluginServices->getGlobalConfigFileDirectory()+"/spimb040_objectives.ini", QSettings::IniFormat).isWritable();
}

void QFCameraConfigWidget::loadObjectives() {
    QSettings inifile(m_pluginServices->getGlobalConfigFileDirectory()+"/spimb040_objectives.ini", QSettings::IniFormat);
    QStringList groups=inifile.childGroups();
    objectives.clear();
    QString currentO=cmbObjective->currentText();
    QString currentP=cmbObjectiveProjection->currentText();
    QString currentT=cmbTubelens->currentText();
    cmbObjective->clear();
    cmbObjectiveProjection->clear();
    for (int i=0; i<groups.size(); i++) {
        QString g=groups[i];
        ObjectiveDescription o;
        o.name=inifile.value(g+"/name", "").toString();
        o.manufacturer=inifile.value(g+"/manufacturer", "").toString();
        o.magnification=inifile.value(g+"/magnification", 1).toDouble();
        o.NA=inifile.value(g+"/na", 1).toDouble();
        objectives.append(o);
        cmbObjective->addItem(o.name);
        cmbTubelens->addItem(o.name);
        cmbObjectiveProjection->addItem(o.name);
    }
    int i=cmbObjective->findText(currentO);
    if (i<0) i=0;
    cmbObjective->setCurrentIndex(i);
    i=cmbObjectiveProjection->findText(currentP);
    if (i<0) i=0;
    cmbObjectiveProjection->setCurrentIndex(i);
    i=cmbTubelens->findText(currentP);
    if (i<0) i=0;
    cmbTubelens->setCurrentIndex(i);
}

void QFCameraConfigWidget::storeObjectives() {
    QSettings inifile(m_pluginServices->getGlobalConfigFileDirectory()+"/spimb040_objectives.ini", QSettings::IniFormat);
    if (inifile.isWritable()) {
        inifile.clear();
        for (int i=0; i<objectives.size(); i++) {
            ObjectiveDescription o=objectives[i];
            QString g="objective"+QString::number(i);
            inifile.setValue(g+"/name", o.name);
            inifile.setValue(g+"/manufacturer", o.manufacturer);
            inifile.setValue(g+"/magnification", o.magnification);
            inifile.setValue(g+"/na", o.NA);
        }
        emit m_notifier->emitUpdate();
    }
    loadObjectives();
}

void QFCameraConfigWidget::deleteObjective() {
    int i=cmbObjective->currentIndex();
    if (i>=0 && i<objectives.size()) {
        objectives.removeAt(i);
    }
    storeObjectives();
    loadObjectives();
}


void QFCameraConfigWidget::editObjective() {
    int i=cmbObjective->currentIndex();
    if (i>=0 && i<objectives.size()) {
        ObjectiveDescription d=objectives[i];

        QF3ObjectiveEditor dlg(d, NULL);
        if (dlg.exec()==QDialog::Accepted) {
            objectives[i]=dlg.getData();
        }

    }
    storeObjectives();
    loadObjectives();
}

void QFCameraConfigWidget::addObjective() {
    ObjectiveDescription d;
    d.name=tr("new objective");
    QF3ObjectiveEditor dlg(d, NULL);
    if (dlg.exec()==QDialog::Accepted) {
        d=dlg.getData();
        while (objectiveExists(d.name)) {
            QMessageBox::critical(this, "Objective Editor", "An objective with the same name already exists. Please rename!", QMessageBox::Ok, QMessageBox::Ok);
            if (dlg.exec()!=QDialog::Accepted) return;
            d=dlg.getData();
        }
        //qDebug()<<d.name;
        objectives.append(d);
    }
    storeObjectives();
    loadObjectives();
}

ObjectiveDescription QFCameraConfigWidget::getObjectiveDescription(int i) {
    return objectives[i];
}

ObjectiveDescription QFCameraConfigWidget::objective() {
    ObjectiveDescription d;
    int i=cmbObjective->currentIndex();
    if (i>=0 && i<objectives.size()) return objectives[i];
    return d;
}

ObjectiveDescription QFCameraConfigWidget::tubelens() {
    ObjectiveDescription d;
    int i=cmbTubelens->currentIndex();
    if (i>=0 && i<objectives.size()) return objectives[i];
    return d;
}
ObjectiveDescription QFCameraConfigWidget::objectiveProjection() {
    ObjectiveDescription d;
    int i=cmbObjectiveProjection->currentIndex();
    if (i>=0 && i<objectives.size()) return objectives[i];
    return d;
}

bool QFCameraConfigWidget::objectiveExists(QString name) {
    for (int i=0; i<objectives.size(); i++) {
        if (name==objectives[i].name) return true;
    }
    return false;

}

void QFCameraConfigWidget::currentObjectiveChanged(int idx) {
    int iD=cmbObjective->currentIndex();
    if (iD>=0 && iD<objectives.size()) labDetectObjectiveDescription->setText(tr("magn.: %1x  NA: %2").arg(objectives[iD].magnification).arg(objectives[iD].NA));
    else labDetectObjectiveDescription->setText("");
    int iP=cmbObjectiveProjection->currentIndex();
    if (iP>=0 && iP<objectives.size()) labProjectObjectiveDescription->setText(tr("magn.: %1x  NA: %2").arg(objectives[iP].magnification).arg(objectives[iP].NA));
    else labProjectObjectiveDescription->setText("");
    int iT=cmbTubelens->currentIndex();
    if (iT>=0 && iT<objectives.size()) labTubelensDescription->setText(tr("magn.: %1x  NA: %2").arg(objectives[iT].magnification).arg(objectives[iT].NA));
    else labTubelensDescription->setText("");
}

void QFCameraConfigWidget::stop() {
    //qDebug()<<"stop";
    restartPreview=actStartStopPreview->isChecked();
    stopPreview();
}

void QFCameraConfigWidget::resume() {
    //qDebug()<<"resume";
    if (restartPreview) {
        actStartStopPreview->setChecked(true);
        startStopPreview();
    }
}

void QFCameraConfigWidget::previewCurrentIndexChanged(int index) {
    emit previewConfigChanged();
    if (actStartStopPreview->isChecked()) { // just stop & resume, as this will load the new settings in the resume operation!
        stopPreview();
        actStartStopPreview->setChecked(true);
        startStopPreview();
    }
}
