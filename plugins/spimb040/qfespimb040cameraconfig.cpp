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


QFESPIMB040CameraConfig::QFESPIMB040CameraConfig(QFESPIMB040MainWindow* parent, int camViewID, QFExtensionServices* pluginServices):
    QGroupBox(parent)
{
    m_camViewID=camViewID;
    m_parent=parent;
    m_pluginServices=pluginServices;
    m_extManager=pluginServices->getExtensionManager();
    camView=NULL;
    locked=false;

    // search for available camera plugins
    findCameras(m_pluginServices->getExtensionManager());

    // initialize raw image memory ...
    viewData.reset();

    // create widgets and actions
    setTitle(tr(" Camera %1: ").arg(camViewID+1));
    createWidgets(m_pluginServices->getExtensionManager());
    createActions();
    rereadConfigCombos();
    displayStates(QFESPIMB040CameraConfig::Disconnected);
    if (cameras.size()<=0) displayStates(QFESPIMB040CameraConfig::Inactive);

}

QFESPIMB040CameraConfig::~QFESPIMB040CameraConfig()
{
    if (camView) {
        camView->close();
        delete camView;
        camView=NULL;
    }
}

bool QFESPIMB040CameraConfig::lockCamera(QFExtension** extension, QFExtensionCamera** ecamera, int* camera) {
    if (locked || (!actDisConnect->isChecked()) || (!viewData.camera) || (!viewData.extension)) {
        *camera=-1;
        *extension=NULL;
        *ecamera=NULL;
        return false;
    }
    actStartStopPreview->setChecked(false);
    displayStates(QFESPIMB040CameraConfig::Locked);

    QString filename="";

    if (cmbAcquisitionConfiguration->currentIndex()>=0) filename=cmbAcquisitionConfiguration->itemData(cmbAcquisitionConfiguration->currentIndex()).toString();
    QSettings* settings=new QSettings(filename, QSettings::IniFormat);
    viewData.camera->useCameraSettings(viewData.usedCamera, *settings);
    delete settings;
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

void QFESPIMB040CameraConfig::rereadConfigCombos() {
    // store current setting of comboboxes
    QString ac=cmbAcquisitionConfiguration->currentText();
    QString pc=cmbPreviewConfiguration->currentText();
    cmbAcquisitionConfiguration->clear();
    cmbPreviewConfiguration->clear();
    if (cameras.size()<=0) return;

    // find applicable .ccf files
    QPoint p = cmbAcquisitionDevice->itemData(cmbAcquisitionDevice->currentIndex()).toPoint();
    QFExtension* extension=NULL;
    QFExtensionCamera* cam=NULL;
    int camIdx=p.y();
    if ((p.x()>=0)&&(p.x()<cameras.size())) {
        extension=qobject_cast<QFExtension*>(cameras[p.x()]);
        cam=qobject_cast<QFExtensionCamera*>(cameras[p.x()]);
    }
    if ((!cam)||(!extension)) return;
    QString directory=m_pluginServices->getConfigFileDirectory()+"/plugins/extensions/"+extension->getID();
    mk_all_dir(directory.toStdString());
    //std::cout<<"should have created all dirs: '"<<directory.toStdString()<<"'"<<std::endl;
    directory+="/";
    QStringList filenames;
    QDir dir = QDir(directory, "*.ccf");
    foreach (QString fileName, dir.entryList(QDir::Files)) {
        QString absfn=dir.absoluteFilePath(fileName);
        filenames.append(absfn);
    }
    // if there are no configurations, make sure there is at least default.ccf
    if (filenames.isEmpty()) {
        QFile file(directory+"default.ccf");
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        file.close();
        filenames.append(directory+"default.ccf");
    }
    // fill comboboxes
    for (int i=0; i<filenames.size(); i++) {
        QString fn=QFileInfo(filenames[i]).baseName();
        cmbAcquisitionConfiguration->addItem(fn, filenames[i]);
        cmbPreviewConfiguration->addItem(fn, filenames[i]);
    }


    // FINALLY: make sure the combo boxes have their values set.
    int configidx=cmbAcquisitionConfiguration->findText(ac);
    if (configidx<0) configidx=0;
    cmbAcquisitionConfiguration->setCurrentIndex(configidx);

    configidx=cmbPreviewConfiguration->findText(pc);
    if (configidx<0) configidx=0;
    cmbPreviewConfiguration->setCurrentIndex(configidx);

}

void QFESPIMB040CameraConfig::loadSettings(ProgramOptions* settings, QString prefix) {
    if (camView) camView->loadSettings(settings, prefix+"cam_view/");

    cmbAcquisitionDevice->setCurrentIndex(settings->getQSettings()->value(prefix+"last_device", 0).toInt());
    spinAcquisitionDelay->setValue(settings->getQSettings()->value(prefix+"acquisition_delay", 0).toDouble());

    QString configfile=settings->getQSettings()->value(prefix+"preview_config", "default").toString();
    int configidx=cmbPreviewConfiguration->findText(configfile);
    if (configidx<0) configidx=0;
    cmbPreviewConfiguration->setCurrentIndex(configidx);

    configfile=settings->getQSettings()->value(prefix+"acquisition_config", "default").toString();
    configidx=cmbAcquisitionConfiguration->findText(configfile);
    if (configidx<0) configidx=0;
    cmbAcquisitionConfiguration->setCurrentIndex(configidx);
}

void QFESPIMB040CameraConfig::storeSettings(ProgramOptions* settings, QString prefix) {
    if (camView) camView->storeSettings(settings, prefix+"cam_view/");

    settings->getQSettings()->setValue(prefix+"last_device", cmbAcquisitionDevice->currentIndex());
    settings->getQSettings()->setValue(prefix+"acquisition_delay", spinAcquisitionDelay->value());
    settings->getQSettings()->setValue(prefix+"acquisition_config", cmbAcquisitionConfiguration->currentText());
    settings->getQSettings()->setValue(prefix+"preview_config", cmbPreviewConfiguration->currentText());

}

void QFESPIMB040CameraConfig::closeEvent ( QCloseEvent * event ) {
    // disconnect devices and close camera view:
    if (actDisConnect->isChecked()) actDisConnect->setChecked(false);
    if (camView) {
        camView->close();
    }
}

void QFESPIMB040CameraConfig::findCameras(QFExtensionManager* extManager) {
    // load available acquisition devices from extManager
    QStringList ids=extManager->getIDList();
    for (int i=0; i<ids.size(); i++) {
        QObject* extobj=extManager->getQObjectInstance(ids[i]);
        QFExtension* extension=extManager->getInstance(ids[i]);
        QFExtensionCamera* cam = qobject_cast<QFExtensionCamera*>(extobj);
        if (cam) cameras.append(extobj);
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
    cmbAcquisitionDevice=new QComboBox(this);
    for (int i=0; i<cameras.size(); i++) {
        QFExtension* extension=qobject_cast<QFExtension*>(cameras[i]);
        QFExtensionCamera* cam = qobject_cast<QFExtensionCamera*>(cameras[i]);
        for (unsigned int j=0; j<cam->getCameraCount(); j++) {
            QPoint sl(i, j);
            QString name=extension->getName();
            if (cam->getCameraCount()>0) name=name+QString(" #%1").arg(j);
            cmbAcquisitionDevice->addItem(QIcon(extension->getIconFilename()), name, sl);
        }
    }
    QHBoxLayout* hbl=new QHBoxLayout(this);
    hbl->setContentsMargins(0,0,0,0);
    hbl->addWidget(cmbAcquisitionDevice);
    btnConnect=new QToolButton(this);
    hbl->addWidget(btnConnect);
    camlayout->addRow(tr("<b>Device:</b>"), hbl);
    cmbAcquisitionDevice->setEnabled(false);
    connect(cmbAcquisitionDevice, SIGNAL(currentIndexChanged(int)), this, SLOT(rereadConfigCombos()));



    hbl=new QHBoxLayout(this);
    hbl->setContentsMargins(0,0,0,0);
    hbl->addWidget(new QLabel(tr("configuration: "), this));
    cmbPreviewConfiguration=new QComboBox(this);
    hbl->addWidget(cmbPreviewConfiguration);
    btnPreviewConfig=new QToolButton(this);
    hbl->addWidget(btnPreviewConfig);
    hbl->addSpacing(5);
    btnDeletePreviewConfig=new QToolButton(this);
    btnDeletePreviewConfig->setIcon(QIcon(":/spimb040/config_delete.png"));
    btnDeletePreviewConfig->setToolTip(tr("delete current configuration"));
    hbl->addWidget(btnDeletePreviewConfig);
    btnSaveAsPreviewConfig=new QToolButton(this);
    btnSaveAsPreviewConfig->setIcon(QIcon(":/spimb040/config_saveas.png"));
    btnSaveAsPreviewConfig->setToolTip(tr("save current configuration with a new name"));
    hbl->addWidget(btnSaveAsPreviewConfig);
    camlayout->addRow(tr("<b>Preview:</b>"), hbl);

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



    hbl=new QHBoxLayout(this);
    hbl->setContentsMargins(0,0,0,0);
    hbl->addWidget(new QLabel(tr("configuration: "), this));
    cmbAcquisitionConfiguration=new QComboBox(this);
    hbl->addWidget(cmbAcquisitionConfiguration);
    btnAcquisitionConfig=new QToolButton(this);
    hbl->addWidget(btnAcquisitionConfig);
    hbl->addSpacing(5);
    btnDeleteAcquisitionConfig=new QToolButton(this);
    btnDeleteAcquisitionConfig->setIcon(QIcon(":/spimb040/config_delete.png"));
    btnDeleteAcquisitionConfig->setToolTip(tr("delete current configuration"));
    hbl->addWidget(btnDeleteAcquisitionConfig);
    btnSaveAsAcquisitionConfig=new QToolButton(this);
    btnSaveAsAcquisitionConfig->setIcon(QIcon(":/spimb040/config_saveas.png"));
    btnSaveAsAcquisitionConfig->setToolTip(tr("save current configuration with a new name"));
    hbl->addWidget(btnSaveAsAcquisitionConfig);
    camlayout->addRow(tr("<b>Acquisition:</b>"), hbl);
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

    actPreviewConfig = new QAction(QIcon(":/spimb040/acquisitionsettings.png"), tr("&Configure Camera"), this);
    connect(actPreviewConfig, SIGNAL(triggered()), this, SLOT(configPreviewCamera()));

    actDeletePreviewConfig = new QAction(QIcon(":/spimb040/config_delete.png"), tr("&Delete Configuration"), this);
    connect(actDeletePreviewConfig, SIGNAL(triggered()), this, SLOT(deletePreviewConfig()));

    actSaveAsPreviewConfig = new QAction(QIcon(":/spimb040/config_saveas.png"), tr("&Save Configuration As ..."), this);
    connect(actSaveAsPreviewConfig, SIGNAL(triggered()), this, SLOT(saveasPreviewConfig()));


    actAcquisitionConfig = new QAction(QIcon(":/spimb040/acquisitionsettings.png"), tr("&Configure Camera"), this);
    connect(actAcquisitionConfig, SIGNAL(triggered()), this, SLOT(configAcquisitionCamera()));

    actDeleteAcquisitionConfig = new QAction(QIcon(":/spimb040/config_delete.png"), tr("&Delete Configuration"), this);
    connect(actDeleteAcquisitionConfig, SIGNAL(triggered()), this, SLOT(deleteAcquisitionConfig()));

    actSaveAsAcquisitionConfig = new QAction(QIcon(":/spimb040/config_saveas.png"), tr("&Save Configuration As ..."), this);
    connect(actSaveAsAcquisitionConfig, SIGNAL(triggered()), this, SLOT(saveasAcquisitionConfig()));



    camView->addUserAction(actPreviewConfig);
    camView->addUserAction(actStartStopPreview);
    camView->addUserAction(actPreviewSingle);
    btnConnect->setDefaultAction(actDisConnect);
    btnPreviewConfig->setDefaultAction(actPreviewConfig);
    btnPreviewSingle->setDefaultAction(actPreviewSingle);
    btnPreviewContinuous->setDefaultAction(actStartStopPreview);
    btnDeletePreviewConfig->setDefaultAction(actDeletePreviewConfig);
    btnSaveAsPreviewConfig->setDefaultAction(actSaveAsPreviewConfig);
    btnAcquisitionConfig->setDefaultAction(actAcquisitionConfig);
    btnDeleteAcquisitionConfig->setDefaultAction(actDeleteAcquisitionConfig);
    btnSaveAsAcquisitionConfig->setDefaultAction(actSaveAsAcquisitionConfig);



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
        actPreviewConfig->setEnabled(true);
        actDeletePreviewConfig->setEnabled(true);
        actSaveAsPreviewConfig->setEnabled(true);
        actAcquisitionConfig->setEnabled(true);
        actDeleteAcquisitionConfig->setEnabled(true);
        actSaveAsAcquisitionConfig->setEnabled(true);

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
        actPreviewConfig->setEnabled(false);
        actDeletePreviewConfig->setEnabled(false);
        actSaveAsPreviewConfig->setEnabled(false);
        actAcquisitionConfig->setEnabled(false);
        actDeleteAcquisitionConfig->setEnabled(true);
        actSaveAsAcquisitionConfig->setEnabled(true);

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
        actPreviewConfig->setEnabled(false);
        actDeletePreviewConfig->setEnabled(false);
        actSaveAsPreviewConfig->setEnabled(false);
        actAcquisitionConfig->setEnabled(false);
        actDeleteAcquisitionConfig->setEnabled(false);
        actSaveAsAcquisitionConfig->setEnabled(false);

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
        actPreviewConfig->setEnabled(false);
        actDeletePreviewConfig->setEnabled(false);
        actSaveAsPreviewConfig->setEnabled(false);
        actAcquisitionConfig->setEnabled(false);
        actDeleteAcquisitionConfig->setEnabled(false);
        actSaveAsAcquisitionConfig->setEnabled(false);

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
        actPreviewConfig->setEnabled(false);
        actDeletePreviewConfig->setEnabled(false);
        actSaveAsPreviewConfig->setEnabled(false);
        actAcquisitionConfig->setEnabled(false);
        actDeleteAcquisitionConfig->setEnabled(false);
        actSaveAsAcquisitionConfig->setEnabled(false);

        cmbAcquisitionDevice->setEnabled(true);
        cmbPreviewConfiguration->setEnabled(false);
        cmbAcquisitionConfiguration->setEnabled(false);
        spinAcquisitionDelay->setEnabled(false);
    }
}


bool QFESPIMB040CameraConfig::connectDevice(int device, int camera) {
    QFExtension* extension=NULL;
    QFExtensionCamera* cam=NULL;


    viewData.reset();

    locked=false;

    if ((device>=0)&&(device<cameras.size())) {
        extension=qobject_cast<QFExtension*>(cameras[device]);
        cam=qobject_cast<QFExtensionCamera*>(cameras[device]);
    }

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

    QPoint p = cmbAcquisitionDevice->itemData(cmbAcquisitionDevice->currentIndex()).toPoint();
    QFExtension* extension=NULL;
    QFExtensionCamera* cam=NULL;
    int camIdx=p.y();
    if ((p.x()>=0)&&(p.x()<cameras.size())) {
        extension=qobject_cast<QFExtension*>(cameras[p.x()]);
        cam=qobject_cast<QFExtensionCamera*>(cameras[p.x()]);
    }
    //std::cout<<"disConnectAcquisitionDevice()  dev="<<p.x()<<" cam="<<p.y()<<"  cam*="<<cam<<" extension*="<<extension<<std::endl;

    if (cam && extension) {
        if (actDisConnect->isChecked()) {
            // connect to a device

            connectDevice(p.x(), p.y());
            if (!cam->isConnected(camIdx)) {
                displayStates(QFESPIMB040CameraConfig::Disconnected);
                m_parent->log_error(tr("error connecting to device %1, camera %2!\n").arg(extension->getName()).arg(camIdx));
            } else {
                displayStates(QFESPIMB040CameraConfig::Connected);
                m_parent->log_text(tr("connected to device %1, camera %2!\n").arg(extension->getName()).arg(camIdx));
                camView->show();
                previewSingle();
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



void QFESPIMB040CameraConfig::configPreviewCamera() {
    int camIdx=viewData.usedCamera;
    /*if (viewData.camera) {
        if (viewData.camera->isConnected(camIdx)) {*/
            QString filename="";
            if (cmbPreviewConfiguration->currentIndex()>=0) filename=cmbPreviewConfiguration->itemData(cmbPreviewConfiguration->currentIndex()).toString();
            if (filename.size()>0) {
                QSettings* settings=new QSettings(filename, QSettings::IniFormat);
                viewData.camera->showCameraSettingsDialog(camIdx, *settings, this);
                settings->sync();
                delete settings;
            }
        /*}
    }*/
}


void QFESPIMB040CameraConfig::configAcquisitionCamera() {
    int camIdx=viewData.usedCamera;
    /*if (viewData.camera) {
        if (viewData.camera->isConnected(camIdx)) {*/
            QString filename="";
            if (cmbAcquisitionConfiguration->currentIndex()>=0) filename=cmbAcquisitionConfiguration->itemData(cmbAcquisitionConfiguration->currentIndex()).toString();
            if (filename.size()>0) {
                QSettings* settings=new QSettings(filename, QSettings::IniFormat);
                viewData.camera->showCameraSettingsDialog(camIdx, *settings, this);
                settings->sync();
                delete settings;
            }
        /*}
    }*/
}


void QFESPIMB040CameraConfig::deletePreviewConfig() {
    QString filename="";
    if (cmbPreviewConfiguration->currentIndex()>=0) filename=cmbPreviewConfiguration->itemData(cmbPreviewConfiguration->currentIndex()).toString();
    if (filename.size()>0) {
        int ret = QMessageBox::question(this, tr("Delete Configurtion ..."),
                        tr("Do you really want to delete the configuration '%1'?").arg(QFileInfo(filename).baseName()),
                        QMessageBox::Yes | QMessageBox::No,  QMessageBox::No);
        if (ret==QMessageBox::Yes) {
            QFile::remove(filename);
            rereadConfigCombos();
            emit configFilesChanged();
        }
    }
}


void QFESPIMB040CameraConfig::saveasPreviewConfig() {
    QString filename="";
    if (cmbPreviewConfiguration->currentIndex()>=0) filename=cmbPreviewConfiguration->itemData(cmbPreviewConfiguration->currentIndex()).toString();
    if (filename.size()>0) {
        bool ok;
        QString newname = QInputDialog::getText(this, tr("Save Configurtion As ..."),
                                          tr("New Name:"), QLineEdit::Normal,
                                          QFileInfo(filename).baseName(), &ok);
        if (ok && !newname.isEmpty()) {
            QString newfilename=QFileInfo(filename).absolutePath()+"/"+newname+".ccf";
            if (QFile::exists(newfilename)) {
                int ret = QMessageBox::question(this, tr("Save Configurtion As ..."),
                                tr("A Configuration with the name '%1' already exists.\n"
                                   "Do you want to overwrite?").arg(newname),
                                QMessageBox::Yes | QMessageBox::No,  QMessageBox::No);
                if (ret==QMessageBox::No) ok=false;
            }
            if (ok) {
                copy_file(filename.toStdString(), newfilename.toStdString());
                rereadConfigCombos();
                emit configFilesChanged();
            }
        }
    }
}


void QFESPIMB040CameraConfig::deleteAcquisitionConfig() {
    QString filename="";
    if (cmbAcquisitionConfiguration->currentIndex()>=0) filename=cmbAcquisitionConfiguration->itemData(cmbAcquisitionConfiguration->currentIndex()).toString();
    if (filename.size()>0) {
        int ret = QMessageBox::question(this, tr("Delete Configurtion ..."),
                        tr("Do you really want to delete the configuration '%1'?").arg(QFileInfo(filename).baseName()),
                        QMessageBox::Yes | QMessageBox::No,  QMessageBox::No);
        if (ret==QMessageBox::Yes) {
            QFile::remove(filename);
            rereadConfigCombos();
            emit configFilesChanged();
        }
    }}


void QFESPIMB040CameraConfig::saveasAcquisitionConfig() {
    QString filename="";
    if (cmbAcquisitionConfiguration->currentIndex()>=0) filename=cmbAcquisitionConfiguration->itemData(cmbAcquisitionConfiguration->currentIndex()).toString();
    if (filename.size()>0) {
        bool ok;
        QString newname = QInputDialog::getText(this, tr("Save Configurtion As ..."),
                                          tr("New Name:"), QLineEdit::Normal,
                                          QFileInfo(filename).baseName(), &ok);
        if (ok && !newname.isEmpty()) {
            QString newfilename=QFileInfo(filename).absolutePath()+"/"+newname+".ccf";
            if (QFile::exists(newfilename)) {
                int ret = QMessageBox::question(this, tr("Save Configurtion As ..."),
                                tr("A Configuration with the name '%1' already exists.\n"
                                   "Do you want to overwrite?").arg(newname),
                                QMessageBox::Yes | QMessageBox::No,  QMessageBox::No);
                if (ret==QMessageBox::No) ok=false;
            }
            if (ok) {
                copy_file(filename.toStdString(), newfilename.toStdString());
                rereadConfigCombos();
                emit configFilesChanged();
            }
        }
    }
}



void QFESPIMB040CameraConfig::previewSingle() {
    if (viewData.camera) {
        int camIdx=viewData.usedCamera;
        QFExtension* extension=viewData.extension;
        QFExtensionCamera* cam=viewData.camera;
        if (viewData.camera->isConnected(camIdx)) {
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
            QString filename="";
            if (cmbPreviewConfiguration->currentIndex()>=0) filename=cmbPreviewConfiguration->itemData(cmbPreviewConfiguration->currentIndex()).toString();
            QSettings* settings=new QSettings(filename, QSettings::IniFormat);
            viewData.camera->useCameraSettings(camIdx, *settings);
            if (acquireSingle()) {
                camView->show();
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
        QFExtension* extension=viewData.extension;
        QFExtensionCamera* cam=viewData.camera;
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
                if (cmbPreviewConfiguration->currentIndex()>=0) filename=cmbPreviewConfiguration->itemData(cmbPreviewConfiguration->currentIndex()).toString();
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



