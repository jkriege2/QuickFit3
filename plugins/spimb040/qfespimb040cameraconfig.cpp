#include "qfespimb040cameraconfig.h"
#include "qfespimb040mainwindow.h"
#include <QtGui>
#include <iostream>

QFESPIMB040CameraConfig::viewDataStruct::viewDataStruct() {
    reset();
}

void QFESPIMB040CameraConfig::viewDataStruct::reset() {
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


QFCameraConfigNotifier* QFESPIMB040CameraConfig::m_notifier=NULL;



QFESPIMB040CameraConfig::QFESPIMB040CameraConfig(QFESPIMB040MainWindow* parent, int camViewID, QFExtensionServices* pluginServices):
    QGroupBox(parent)
{
    if (m_notifier==NULL) m_notifier=new QFCameraConfigNotifier(this);
    m_camViewID=camViewID;
    m_parent=parent;
    m_pluginServices=pluginServices;
    m_extManager=pluginServices->getExtensionManager();
    camView=NULL;
    locked=false;


    // initialize raw image memory ...
    viewData.reset();

    // create widgets and actions
    setTitle(tr(" Camera %1: ").arg(camViewID+1));
    createWidgets(m_pluginServices->getExtensionManager());
    createActions();
    displayStates(QFESPIMB040CameraConfig::Disconnected);
    if (cmbAcquisitionDevice->count()<=0) displayStates(QFESPIMB040CameraConfig::Inactive);
    connect(m_notifier, SIGNAL(doUpdate()), this, SLOT(loadObjectives()));
}

QFESPIMB040CameraConfig::~QFESPIMB040CameraConfig()
{
    if (camView) {
        camView->close();
        delete camView;
        camView=NULL;
    }
}

bool QFESPIMB040CameraConfig::lockCamera(QFExtension** extension, QFExtensionCamera** ecamera, int* camera, QString* acquisitionSettingsFilename, QString* previewSettingsFilename) {
    if (locked || (!actDisConnect->isChecked()) || (!viewData.camera) || (!viewData.extension)) {
        *camera=-1;
        *extension=NULL;
        *ecamera=NULL;
        return false;
    }
    actStartStopPreview->setChecked(false);
    displayStates(QFESPIMB040CameraConfig::Locked);

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

void QFESPIMB040CameraConfig::releaseCamera() {
    if (locked) {
        displayStates(QFESPIMB040CameraConfig::Connected);
        locked=false;
    }
}


void QFESPIMB040CameraConfig::loadSettings(ProgramOptions* settings, QString prefix) {
    if (camView) camView->loadSettings(settings, prefix+"cam_view/");

    loadObjectives();

    cmbAcquisitionDevice->setCurrentIndex(settings->getQSettings()->value(prefix+"last_device", 0).toInt());
    spinAcquisitionDelay->setValue(settings->getQSettings()->value(prefix+"acquisition_delay", 0).toDouble());
    cmbObjecive->setCurrentIndex(cmbObjecive->findText(settings->getQSettings()->value(prefix+"objective", "").toString()));
    cmbObjeciveProjection->setCurrentIndex(cmbObjeciveProjection->findText(settings->getQSettings()->value(prefix+"objective_projection", "").toString()));
    cmbPreviewConfiguration->setCurrentConfig(settings->getQSettings()->value(prefix+"preview_config", "default").toString());
    cmbAcquisitionConfiguration->setCurrentConfig(settings->getQSettings()->value(prefix+"acquisition_config", "default").toString());
}

void QFESPIMB040CameraConfig::storeSettings(ProgramOptions* settings, QString prefix) {
    if (camView) camView->storeSettings(settings, prefix+"cam_view/");

    settings->getQSettings()->setValue(prefix+"last_device", cmbAcquisitionDevice->currentIndex());
    settings->getQSettings()->setValue(prefix+"acquisition_delay", spinAcquisitionDelay->value());
    settings->getQSettings()->setValue(prefix+"objective", cmbObjecive->currentText());
    settings->getQSettings()->setValue(prefix+"objective_projection", cmbObjeciveProjection->currentText());
    settings->getQSettings()->setValue(prefix+"acquisition_config", cmbAcquisitionConfiguration->currentConfigName());//currentConfigFilename());
    settings->getQSettings()->setValue(prefix+"preview_config", cmbPreviewConfiguration->currentConfigName());//currentConfigFilename());

}

void QFESPIMB040CameraConfig::closeEvent ( QCloseEvent * event ) {
    if (camView) {
        camView->close();
    }
    // disconnect devices and close camera view:
    if (actDisConnect->isChecked()) {
        actDisConnect->setChecked(false);
        disconnectDevice();
    }
}


void QFESPIMB040CameraConfig::createWidgets(QFExtensionManager* extManager) {
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // create main layout
    ////////////////////////////////////////////////////////////////////////////////////////////////
    QFormLayout* camlayout=new QFormLayout(this);
    setLayout(camlayout);


    ////////////////////////////////////////////////////////////////////////////////////////////////
    // create camera view widgets ... and hide them
    ////////////////////////////////////////////////////////////////////////////////////////////////
    camView=new QFESPIMB040CameraView(m_camViewID, QString(m_pluginServices->getConfigFileDirectory()+"/spimb040_cam1.log"), m_pluginServices, NULL);
    camView->close();



    ////////////////////////////////////////////////////////////////////////////////////////////////
    // create input widgets for camera devices
    ////////////////////////////////////////////////////////////////////////////////////////////////
    cmbAcquisitionDevice=new QFCameraComboBox(extManager, this);
    QHBoxLayout* hbl=new QHBoxLayout(this);
    hbl->setContentsMargins(0,0,0,0);
    hbl->addWidget(cmbAcquisitionDevice);
    btnConnect=new QToolButton(this);
    hbl->addWidget(btnConnect);
    camlayout->addRow(tr("<b>Device:</b>"), hbl);
    cmbAcquisitionDevice->setEnabled(false);


    QGridLayout* ogl=new QGridLayout(this);
    ogl->addWidget(new QLabel("detection:"), 0,0);
    ogl->addWidget(new QLabel("lightsheet:"), 1,0);
    cmbObjecive=new QComboBox(this);
    connect(cmbObjecive, SIGNAL(currentIndexChanged(int)), this, SLOT(currentObjectiveChanged(int)));
    ogl->addWidget(cmbObjecive, 0,1);
    cmbObjeciveProjection=new QComboBox(this);
    connect(cmbObjeciveProjection, SIGNAL(currentIndexChanged(int)), this, SLOT(currentObjectiveChanged(int)));
    ogl->addWidget(cmbObjeciveProjection, 1,1);
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
    ogl->setColumnStretch(5, 1);

    camlayout->addRow(tr("<b>Objectives:</b>"), ogl);

    cmbPreviewConfiguration=new QFCameraConfigEditorWidget(m_pluginServices->getConfigFileDirectory(), this);
    cmbPreviewConfiguration->connectTo(cmbAcquisitionDevice);
    camlayout->addRow(tr("<b>Preview:</b>"), cmbPreviewConfiguration);

    hbl=new QHBoxLayout(this);
    hbl->setContentsMargins(0,0,0,0);
    spinAcquisitionDelay=new QDoubleSpinBox(this);
    spinAcquisitionDelay->setMinimum(0);
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




    cmbAcquisitionConfiguration=new QFCameraConfigEditorWidget(m_pluginServices->getConfigFileDirectory(), this);
    cmbAcquisitionConfiguration->connectTo(cmbAcquisitionDevice);
    camlayout->addRow(tr("<b>Acquisition:</b>"), cmbAcquisitionConfiguration);
}





void QFESPIMB040CameraConfig::createActions() {
    actDisConnect = new QAction(QIcon(":/spimb040/acquisitionconnect.png"), tr("&Connect"), this);
    actDisConnect->setCheckable(true);
    connect(actDisConnect, SIGNAL(triggered()), this, SLOT(disConnectAcquisitionDevice()));

    actStartStopPreview = new QAction(QIcon(":/spimb040/acquisitionstart.png"), tr("&Start acquisition"), this);
    actStartStopPreview->setCheckable(true);
    connect(actStartStopPreview, SIGNAL(triggered()), this, SLOT(startStopPreview()));

    actPreviewSingle = new QAction(QIcon(":/spimb040/acquisitionsingle.png"), tr("&Acquire single image"), this);
    connect(actPreviewSingle, SIGNAL(triggered()), this, SLOT(previewSingle()));


    camView->addUserAction(cmbPreviewConfiguration->configAction());
    camView->addUserAction(actStartStopPreview);
    camView->addUserAction(actPreviewSingle);
    btnConnect->setDefaultAction(actDisConnect);
    btnPreviewSingle->setDefaultAction(actPreviewSingle);
    btnPreviewContinuous->setDefaultAction(actStartStopPreview);



}

void QFESPIMB040CameraConfig::displayStates(QFESPIMB040CameraConfig::States state) {
    if (state==QFESPIMB040CameraConfig::Connected) {
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


    } else if (state==QFESPIMB040CameraConfig::Previewing) {
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
        cmbPreviewConfiguration->setEnabled(false);
        cmbAcquisitionConfiguration->setEnabled(true);
        spinAcquisitionDelay->setEnabled(true);


    } else if (state==QFESPIMB040CameraConfig::Inactive) {
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


    } else if (state==QFESPIMB040CameraConfig::Locked) {
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


bool QFESPIMB040CameraConfig::connectDevice(QFExtension* extension, QFExtensionCamera* cam, int camera) {

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
            m_parent->log_error(tr("error connecting to device %1, camera %2 ...\n").arg(extension->getName()).arg(camera));
        }
        QApplication::restoreOverrideCursor();
        return s;
    } else {
        return false;
    }
}


void QFESPIMB040CameraConfig::disconnectDevice() {
    locked=false;
    if (viewData.camera) {
        if (viewData.camera->isConnected(viewData.usedCamera)) {
            viewData.camera->disconnectDevice(viewData.usedCamera);
        }
    }
}


bool QFESPIMB040CameraConfig::acquireSingle() {
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





void QFESPIMB040CameraConfig::disConnectAcquisitionDevice() {
    if (cmbAcquisitionDevice->currentIndex()<0) {
        // if there are no devices registered: deactivate everything!
        displayStates(QFESPIMB040CameraConfig::Disconnected);
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
                displayStates(QFESPIMB040CameraConfig::Disconnected);
                m_parent->log_error(tr("error connecting to device %1, camera %2!\n").arg(extension->getName()).arg(camIdx));
            } else {
                displayStates(QFESPIMB040CameraConfig::Connected);
                m_parent->log_text(tr("connected to device %1, camera %2!\n").arg(extension->getName()).arg(camIdx));
                //camView->show();
                //previewSingle();
            }
        } else {
            //disconnect from the current device and delete the settings widget
            displayStates(QFESPIMB040CameraConfig::Disconnected);
            if (cam->isConnected(camIdx))  {
                cam->disconnectDevice(camIdx);
                //cam->setLogging(NULL);
            }
            m_parent->log_text(tr("disconnected from device %1, camera %2!\n").arg(extension->getName()).arg(camIdx));
        }
    }
}




void QFESPIMB040CameraConfig::previewSingle() {
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
                camView->displayImage(viewData.rawImage, viewData.timestamp, viewData.exposureTime);
            } else {
                m_parent->log_error(tr("could not acquire frame, as device is not connected ...!\n"));
            }
            delete settings;
            QApplication::restoreOverrideCursor();
        }
    }
}



void QFESPIMB040CameraConfig::startStopPreview() {
    if (viewData.camera) {
        int camIdx=viewData.usedCamera;
        //QFExtension* extension=viewData.extension;
        //QFExtensionCamera* cam=viewData.camera;
        if (viewData.camera->isConnected(camIdx)) {
            if (actStartStopPreview->isChecked()) {
                displayStates(QFESPIMB040CameraConfig::Previewing);
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
                previewContinuous();
                delete settings;
            } else {
                displayStates(QFESPIMB040CameraConfig::Connected);
            }
        }
    }
}



void QFESPIMB040CameraConfig::previewContinuous() {
    if (viewData.camera) {
        int camIdx=viewData.usedCamera;
        QFExtension* extension=viewData.extension;
        QFExtensionCamera* cam=viewData.camera;
        if (viewData.abort_continuous_acquisition) {
            displayStates(QFESPIMB040CameraConfig::Connected);
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
            m_parent->log_error(tr("could not acquire frame, as device is not connected ...!\n"));
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
            QTimer::singleShot(spinAcquisitionDelay->value(), this, SLOT(previewContinuous()));
        }
        /*} else {
            actStartStopAcquisition->setChecked(false);
            actStartStopAcquisition->setIcon(QIcon(":/spimb040/acquisitionstart.png"));
            actStartStopAcquisition->setText(tr("&Start Acquisition"));
            actStartStopAcquisition->setEnabled(false);
            actAcquireSingle->setEnabled(false);
            actCameraConfig->setEnabled(false);
        }*/
    }
}

double QFESPIMB040CameraConfig::magnification() {
    return objective().magnification;
}

bool QFESPIMB040CameraConfig::objectivesWritable() const {
    return QSettings(m_pluginServices->getGlobalConfigFileDirectory()+"/spimb040_objectives.ini", QSettings::IniFormat).isWritable();
}

void QFESPIMB040CameraConfig::loadObjectives() {
    QSettings inifile(m_pluginServices->getGlobalConfigFileDirectory()+"/spimb040_objectives.ini", QSettings::IniFormat);
    QStringList groups=inifile.childGroups();
    objectives.clear();
    QString currentO=cmbObjecive->currentText();
    QString currentP=cmbObjeciveProjection->currentText();
    cmbObjecive->clear();
    cmbObjeciveProjection->clear();
    for (int i=0; i<groups.size(); i++) {
        QString g=groups[i];
        ObjectiveDescription o;
        o.name=inifile.value(g+"/name", "").toString();
        o.manufacturer=inifile.value(g+"/manufacturer", "").toString();
        o.magnification=inifile.value(g+"/magnification", 1).toDouble();
        o.NA=inifile.value(g+"/na", 1).toDouble();
        objectives.append(o);
        cmbObjecive->addItem(o.name);
        cmbObjeciveProjection->addItem(o.name);
    }
    int i=cmbObjecive->findText(currentO);
    if (i<0) i=0;
    cmbObjecive->setCurrentIndex(i);
    i=cmbObjeciveProjection->findText(currentP);
    if (i<0) i=0;
    cmbObjeciveProjection->setCurrentIndex(i);
}

void QFESPIMB040CameraConfig::storeObjectives() {
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

void QFESPIMB040CameraConfig::deleteObjective() {
    int i=cmbObjecive->currentIndex();
    if (i>=0 && i<objectives.size()) {
        objectives.removeAt(i);
    }
    storeObjectives();
    loadObjectives();
}


void QFESPIMB040CameraConfig::editObjective() {
    int i=cmbObjecive->currentIndex();
    if (i>=0 && i<objectives.size()) {
        ObjectiveDescription d=objectives[i];

        QFESPIMB040ObjectiveEditor dlg(d, NULL);
        if (dlg.exec()==QDialog::Accepted) {
            objectives[i]=dlg.getData();
        }

    }
    storeObjectives();
    loadObjectives();
}

void QFESPIMB040CameraConfig::addObjective() {
    ObjectiveDescription d;
    d.name=tr("new objective");
    QFESPIMB040ObjectiveEditor dlg(d, NULL);
    if (dlg.exec()==QDialog::Accepted) {
        d=dlg.getData();
        while (objectiveExists(d.name)) {
            QMessageBox::critical(this, "Objective Editor", "An objective with the same name already exists. Please rename!", QMessageBox::Ok, QMessageBox::Ok);
            if (dlg.exec()!=QDialog::Accepted) return;
            d=dlg.getData();
        }
        qDebug()<<d.name;
        objectives.append(d);
    }
    storeObjectives();
    loadObjectives();
}

ObjectiveDescription QFESPIMB040CameraConfig::getObjectiveDescription(int i) {
    return objectives[i];
}

ObjectiveDescription QFESPIMB040CameraConfig::objective() {
    ObjectiveDescription d;
    int i=cmbObjecive->currentIndex();
    if (i>=0 && i<objectives.size()) return objectives[i];
    return d;
}

ObjectiveDescription QFESPIMB040CameraConfig::objectiveProjection() {
    ObjectiveDescription d;
    int i=cmbObjeciveProjection->currentIndex();
    if (i>=0 && i<objectives.size()) return objectives[i];
    return d;
}

bool QFESPIMB040CameraConfig::objectiveExists(QString name) {
    for (int i=0; i<objectives.size(); i++) {
        if (name==objectives[i].name) return true;
    }
    return false;

}

void QFESPIMB040CameraConfig::currentObjectiveChanged(int idx) {
    int iD=cmbObjecive->currentIndex();
    if (iD>=0 && iD<objectives.size()) labDetectObjectiveDescription->setText(tr("magn.: %1x  NA: %2").arg(objectives[iD].magnification).arg(objectives[iD].NA));
    else labDetectObjectiveDescription->setText("");
    int iP=cmbObjeciveProjection->currentIndex();
    if (iP>=0 && iP<objectives.size()) labProjectObjectiveDescription->setText(tr("magn.: %1x  NA: %2").arg(objectives[iP].magnification).arg(objectives[iP].NA));
    else labProjectObjectiveDescription->setText("");
}

