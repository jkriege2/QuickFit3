#include "qfespimb040mainwindow.h"
#include <QtGui>

QFESPIMB040MainWindow::viewDataStruct::viewDataStruct() {
    reset();
}

void QFESPIMB040MainWindow::viewDataStruct::reset() {
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





QFESPIMB040MainWindow::QFESPIMB040MainWindow(QFExtensionManager* extManager, QWidget* parent):
    QWidget(parent)
{
    // search for available camera plugins
    findCameras(extManager);

    // initialize raw image memory ...
    viewData.append(new viewDataStruct());
    viewData.append(new viewDataStruct());

    // create widgets and actions
    createWidgets(extManager);
    createActions();
}

QFESPIMB040MainWindow::~QFESPIMB040MainWindow()
{
    if (camView1) {
        camView1->close();
        delete camView1;
        camView1=NULL;
    }
    if (camView2) {
        camView2->close();
        delete camView2;
        camView2=NULL;
    }
}

void QFESPIMB040MainWindow::loadSettings(ProgramOptions* settings) {
    jkloadWidgetGeometry((*settings->getQSettings()), this, "plugin_spim_b040/");
    if (camView1) camView1->loadSettings(settings, "plugin_spim_b040/cam_view1_");
    if (camView2) camView2->loadSettings(settings, "plugin_spim_b040/cam_view2_");

    cmbAcquisitionDevice1->setCurrentIndex((settings->getQSettings())->value("plugin_spim_b040/cam1/last_device", 0).toInt());
    spinAcquisitionDelay1->setValue((settings->getQSettings())->value("plugin_spim_b040/cam1/acquisition_delay", 0).toDouble());

    cmbAcquisitionDevice2->setCurrentIndex((settings->getQSettings())->value("plugin_spim_b040/cam2/last_device", 0).toInt());
    spinAcquisitionDelay2->setValue((settings->getQSettings())->value("plugin_spim_b040/cam2/acquisition_delay", 0).toDouble());
}

void QFESPIMB040MainWindow::storeSettings(ProgramOptions* settings) {
    jksaveWidgetGeometry((*settings->getQSettings()), this, "plugin_spim_b040/");
    if (camView1) camView1->storeSettings(settings, "plugin_spim_b040/cam_view1_");
    if (camView2) camView2->storeSettings(settings, "plugin_spim_b040/cam_view2_");

    (settings->getQSettings())->setValue("plugin_spim_b040/cam1/last_device", cmbAcquisitionDevice1->currentIndex());
    (settings->getQSettings())->setValue("plugin_spim_b040/cam1/acquisition_delay", spinAcquisitionDelay1->value());

    (settings->getQSettings())->setValue("plugin_spim_b040/cam2/last_device", cmbAcquisitionDevice2->currentIndex());
    (settings->getQSettings())->setValue("plugin_spim_b040/cam2/acquisition_delay", spinAcquisitionDelay2->value());

}

void QFESPIMB040MainWindow::closeEvent ( QCloseEvent * event ) {
    // disconnect devices and close camera view:
    if (actDisConnectAcquisition1->isChecked()) actDisConnectAcquisition1->setChecked(false);
    if (actDisConnectAcquisition2->isChecked()) actDisConnectAcquisition2->setChecked(false);
    if (camView1) {
        camView1->close();
    }
    if (camView2) {
        camView2->close();
    }
}

void QFESPIMB040MainWindow::findCameras(QFExtensionManager* extManager) {
    // load available acquisition devices from extManager
    QStringList ids=extManager->getIDList();
    for (int i=0; i<ids.size(); i++) {
        QObject* extobj=extManager->getQObjectInstance(ids[i]);
        QFExtension* extension=extManager->getInstance(ids[i]);
        QFExtensionCamera* cam = qobject_cast<QFExtensionCamera*>(extobj);
        if (cam) cameras.append(extobj);
    }
}

void QFESPIMB040MainWindow::createWidgets(QFExtensionManager* extManager) {
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // create main layout
    ////////////////////////////////////////////////////////////////////////////////////////////////
    QGridLayout* mainlayout=new QGridLayout(this);
    setLayout(mainlayout);


    ////////////////////////////////////////////////////////////////////////////////////////////////
    // create camera view widgets ... and hide them
    ////////////////////////////////////////////////////////////////////////////////////////////////
    camView1=new QFESPIMB040CameraView(QString(QApplication::applicationDirPath()+"/spimb040_cam1.log"), extManager, NULL);
    camView1->close();
    camView2=new QFESPIMB040CameraView(QString(QApplication::applicationDirPath()+"/spimb040_cam1.log"), extManager, NULL);
    camView2->close();


    ////////////////////////////////////////////////////////////////////////////////////////////////
    // create group boxes for camera widgets
    ////////////////////////////////////////////////////////////////////////////////////////////////
    QGroupBox* camgroup1=new QGroupBox(tr("Camera &1:"), this);
    mainlayout->addWidget(camgroup1, 0, 0);
    QFormLayout* camlayout1=new QFormLayout(this);
    camgroup1->setLayout(camlayout1);
    QGroupBox* camgroup2=new QGroupBox(tr("Camera &2:"), this);
    mainlayout->addWidget(camgroup2, 0, 1);
    QFormLayout* camlayout2=new QFormLayout(this);
    camgroup2->setLayout(camlayout2);


    ////////////////////////////////////////////////////////////////////////////////////////////////
    // create input widgets for camera devices
    ////////////////////////////////////////////////////////////////////////////////////////////////
    cmbAcquisitionDevice1=new QComboBox(this);
    cmbAcquisitionDevice2=new QComboBox(this);
    for (int i=0; i<cameras.size(); i++) {
        QFExtension* extension=qobject_cast<QFExtension*>(cameras[i]);
        QFExtensionCamera* cam = qobject_cast<QFExtensionCamera*>(cameras[i]);
        for (unsigned int j=0; j<cam->getCameraCount(); j++) {
            QPoint sl(i, j);
            QString name=extension->getName();
            if (cam->getCameraCount()>0) name=name+QString(" #%1").arg(j);
            cmbAcquisitionDevice1->addItem(QIcon(extension->getIconFilename()), name, sl);
            cmbAcquisitionDevice2->addItem(QIcon(extension->getIconFilename()), name, sl);
        }
    }
    QHBoxLayout* hbl=new QHBoxLayout(this);
    hbl->setContentsMargins(0,0,0,0);
    hbl->addWidget(cmbAcquisitionDevice1);
    btnConnect1=new QToolButton(this);
    hbl->addWidget(btnConnect1);
    btnConfig1=new QToolButton(this);
    hbl->addWidget(btnConfig1);
    camlayout1->addRow(tr("&Device:"), hbl);


    hbl=new QHBoxLayout(this);
    hbl->setContentsMargins(0,0,0,0);
    hbl->addWidget(cmbAcquisitionDevice2);
    btnConnect2=new QToolButton(this);
    hbl->addWidget(btnConnect2);
    btnConfig2=new QToolButton(this);
    hbl->addWidget(btnConfig2);
    camlayout2->addRow(tr("De&vice:"), hbl);



    hbl=new QHBoxLayout(this);
    hbl->setContentsMargins(0,0,0,0);
    spinAcquisitionDelay1=new QDoubleSpinBox(this);
    spinAcquisitionDelay1->setMinimum(0);
    spinAcquisitionDelay1->setSuffix(tr("ms"));
    spinAcquisitionDelay1->setMaximum(10000);
    spinAcquisitionDelay1->setSingleStep(1);
    spinAcquisitionDelay1->setDecimals(0);
    spinAcquisitionDelay1->setEnabled(false);
    btnAcquireSingle1=new QToolButton(this);
    btnAcquireSingle1->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    QFrame* line=new QFrame(this);
    line->setFrameStyle(QFrame::VLine | QFrame::Raised);
    line->setLineWidth(0);
    line->setMidLineWidth(1);
    line->setMinimumWidth(8);
    line->setMaximumWidth(8);
    hbl->addWidget(btnAcquireSingle1);
    hbl->addWidget(line);
    hbl->addWidget(new QLabel(tr("delay: "), this));
    hbl->addWidget(spinAcquisitionDelay1);
    btnAcquireContinuous1=new QToolButton(this);
    btnAcquireContinuous1->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    hbl->addWidget(btnAcquireContinuous1);
    hbl->addStretch();
    camlayout1->addRow(tr("Control:"), hbl);


    hbl=new QHBoxLayout(this);
    hbl->setContentsMargins(0,0,0,0);
    spinAcquisitionDelay2=new QDoubleSpinBox(this);
    spinAcquisitionDelay2->setMinimum(0);
    spinAcquisitionDelay2->setSuffix(tr("ms"));
    spinAcquisitionDelay2->setMaximum(10000);
    spinAcquisitionDelay2->setSingleStep(1);
    spinAcquisitionDelay2->setDecimals(0);
    spinAcquisitionDelay2->setEnabled(false);
    btnAcquireSingle2=new QToolButton(this);
    btnAcquireSingle2->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    line=new QFrame(this);
    line->setFrameStyle(QFrame::VLine | QFrame::Raised);
    line->setLineWidth(0);
    line->setMidLineWidth(1);
    line->setMinimumWidth(8);
    line->setMaximumWidth(8);
    hbl->addWidget(btnAcquireSingle2);
    hbl->addWidget(line);
    hbl->addWidget(new QLabel(tr("delay: "), this));
    hbl->addWidget(spinAcquisitionDelay2);
    btnAcquireContinuous2=new QToolButton(this);
    btnAcquireContinuous2->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    hbl->addWidget(btnAcquireContinuous2);
    hbl->addStretch();
    camlayout2->addRow(tr("Control:"), hbl);



    ////////////////////////////////////////////////////////////////////////////////////////////////
    // create input widgets for camera devices
    ////////////////////////////////////////////////////////////////////////////////////////////////
    logMain=new QtLogFile(this);
    mainlayout->addWidget(logMain, 1,0,1,2);


}

void QFESPIMB040MainWindow::createActions() {
    actDisConnectAcquisition1 = new QAction(QIcon(":/spimb040/acquisitionconnect.png"), tr("Cam 1: &Connect"), this);
    actDisConnectAcquisition1->setCheckable(true);
    connect(actDisConnectAcquisition1, SIGNAL(triggered()), this, SLOT(disConnectAcquisition1()));

    actStartStopAcquisition1 = new QAction(QIcon(":/spimb040/acquisitionstart.png"), tr("Cam 1: &Start acquisition"), this);
    actStartStopAcquisition1->setCheckable(true);
    connect(actStartStopAcquisition1, SIGNAL(triggered()), this, SLOT(startStopAcquisition1()));

    actAcquireSingle1 = new QAction(QIcon(":/spimb040/acquisitionsingle.png"), tr("Cam 1: &Acquire single image"), this);
    connect(actAcquireSingle1, SIGNAL(triggered()), this, SLOT(acquireSingle1()));

    actCameraConfig1 = new QAction(QIcon(":/spimb040/acquisitionsettings.png"), tr("Cam 1: &Configure Camera"), this);
    connect(actCameraConfig1, SIGNAL(triggered()), this, SLOT(configCamera1()));

    camView1->addUserAction(actCameraConfig1);
    camView1->addUserAction(actStartStopAcquisition1);
    camView1->addUserAction(actAcquireSingle1);
    btnConnect1->setDefaultAction(actDisConnectAcquisition1);
    btnConfig1->setDefaultAction(actCameraConfig1);
    btnAcquireSingle1->setDefaultAction(actAcquireSingle1);
    btnAcquireContinuous1->setDefaultAction(actStartStopAcquisition1);



    actDisConnectAcquisition2 = new QAction(QIcon(":/spimb040/acquisitionconnect.png"), tr("Cam 2: &Connect"), this);
    actDisConnectAcquisition2->setCheckable(true);
    connect(actDisConnectAcquisition2, SIGNAL(triggered()), this, SLOT(disConnectAcquisition2()));

    actStartStopAcquisition2 = new QAction(QIcon(":/spimb040/acquisitionstart.png"), tr("Cam 2: &Start acquisition"), this);
    actStartStopAcquisition2->setCheckable(true);
    connect(actStartStopAcquisition2, SIGNAL(triggered()), this, SLOT(startStopAcquisition2()));

    actAcquireSingle2 = new QAction(QIcon(":/spimb040/acquisitionsingle.png"), tr("Cam 2: &Acquire single image"), this);
    connect(actAcquireSingle2, SIGNAL(triggered()), this, SLOT(acquireSingle2()));

    actCameraConfig2 = new QAction(QIcon(":/spimb040/acquisitionsettings.png"), tr("Cam 2: &Configure Camera"), this);
    connect(actCameraConfig2, SIGNAL(triggered()), this, SLOT(configCamera2()));

    camView2->addUserAction(actCameraConfig2);
    camView2->addUserAction(actStartStopAcquisition2);
    camView2->addUserAction(actAcquireSingle2);
    btnConnect2->setDefaultAction(actDisConnectAcquisition2);
    btnConfig2->setDefaultAction(actCameraConfig2);
    btnAcquireSingle2->setDefaultAction(actAcquireSingle2);
    btnAcquireContinuous2->setDefaultAction(actStartStopAcquisition2);

}



bool QFESPIMB040MainWindow::connectDevice(int camView, int device, int camera) {
    QFExtension* extension=NULL;
    QFExtensionCamera* cam=NULL;


    if ((camView<0) || (camView>=viewData.size())) return false;

    viewData[camView]->reset();

    if ((device>=0)&&(device<cameras.size())) {
        extension=qobject_cast<QFExtension*>(cameras[device]);
        cam=qobject_cast<QFExtensionCamera*>(cameras[device]);
    }

    if (cam && extension) {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        viewData[camView]->extension=extension;
        viewData[camView]->camera=cam;
        cam->setLogging(this);
        bool s=cam->connectDevice(camera);
        if (!s) {
            //cam->setLogging(NULL);
            viewData[camView]->reset();
            logMain->log_error(tr("error connecting to device %1, camera %2 ...\n").arg(extension->getName()).arg(camera));
        }
        QApplication::restoreOverrideCursor();
        return s;
    } else {
        return false;
    }
}


void QFESPIMB040MainWindow::disconnectDevice(int camView) {
    if ((camView<0) || (camView>=viewData.size())) return;

    if (viewData[camView]->camera) {
        if (viewData[camView]->camera->isConnected(viewData[camView]->usedCamera)) {
            viewData[camView]->camera->disconnectDevice(viewData[camView]->usedCamera);
        }
    }
}


bool QFESPIMB040MainWindow::acquireSingle(int camView) {
    if ((camView<0) || (camView>=viewData.size())) return false;

    if (viewData[camView]->camera) {
        int usedcam=viewData[camView]->usedCamera;
        if (viewData[camView]->camera->isConnected(usedcam)) {
            //viewData[camView]->camera->acquire(usedcam);
            int dw=viewData[camView]->camera->getImageWidth(usedcam);
            int dh=viewData[camView]->camera->getImageHeight(usedcam);
            uint64_t timestamp=0;
            viewData[camView]->rawImage.resize(dw, dh);
            bool ok=viewData[camView]->camera->acquire(usedcam, viewData[camView]->rawImage.data(), &timestamp);
            viewData[camView]->lastAcquisitionTime=QTime::currentTime();
            double exposure=viewData[camView]->camera->getExposureTime(usedcam);
            viewData[camView]->timestamp=timestamp;
            viewData[camView]->exposureTime=exposure;
            viewData[camView]->acqFrames++;
            viewData[camView]->acqFramesQR++;
            return ok;
        }
    }
    return false;
}





void QFESPIMB040MainWindow::disConnectAcquisition1() {
    if (cmbAcquisitionDevice1->currentIndex()<0) {
        // if there are no devices registered: deactivate everything!
        actDisConnectAcquisition1->setIcon(QIcon(":/spimb040/acquisitionconnect.png"));
        actDisConnectAcquisition1->setText(tr("&Connect"));
        actStartStopAcquisition1->setEnabled(false);
        actAcquireSingle1->setEnabled(false);
        actDisConnectAcquisition1->setEnabled(false);
        cmbAcquisitionDevice1->setEnabled(false);
        actCameraConfig1->setEnabled(false);
        return;
    }

    QPoint p = cmbAcquisitionDevice1->itemData(cmbAcquisitionDevice1->currentIndex()).toPoint();
    QFExtension* extension=NULL;
    QFExtensionCamera* cam=NULL;
    int camIdx=p.y();
    if ((p.x()>=0)&&(p.x()<cameras.size())) {
        extension=qobject_cast<QFExtension*>(cameras[p.x()]);
        cam=qobject_cast<QFExtensionCamera*>(cameras[p.x()]);
    }
    std::cout<<"disConnectAcquisition()  dev="<<p.x()<<" cam="<<p.y()<<"  cam*="<<cam<<" extension*="<<extension<<std::endl;

    if (cam && extension) {
        if (actDisConnectAcquisition1->isChecked()) {
            // connect to a device

            connectDevice(0, p.x(), p.y());
            if (!cam->isConnected(camIdx)) {
                actDisConnectAcquisition1->setChecked(false);
                logMain->log_error(tr("error connecting to device %1, camera %2!\n").arg(extension->getName()).arg(camIdx));
            } else {
                actDisConnectAcquisition1->setIcon(QIcon(":/spimb040/acquisitiondisconnect.png"));
                actDisConnectAcquisition1->setText(tr("&Disconnect"));
                actStartStopAcquisition1->setEnabled(true);
                actStartStopAcquisition1->setChecked(false);
                actAcquireSingle1->setEnabled(true);
                actCameraConfig1->setEnabled(true);
                cmbAcquisitionDevice1->setEnabled(false);
                spinAcquisitionDelay1->setEnabled(true);
                logMain->log_text(tr("connected to device %1, camer %2!\n").arg(extension->getName()).arg(camIdx));
                camView1->show();
                acquireSingle1();
            }
        } else {
            //disconnect from the current device and delete the settings widget
            actDisConnectAcquisition1->setIcon(QIcon(":/spimb040/acquisitionconnect.png"));
            actDisConnectAcquisition1->setText(tr("&Connect"));
            actStartStopAcquisition1->setEnabled(false);
            actStartStopAcquisition1->setChecked(false);
            actStartStopAcquisition1->setIcon(QIcon(":/spimb040/acquisitionstart.png"));
            actStartStopAcquisition1->setText(tr("&Start Acquisition"));
            actAcquireSingle1->setEnabled(false);
            cmbAcquisitionDevice1->setEnabled(true);
            spinAcquisitionDelay1->setEnabled(false);
            actCameraConfig1->setEnabled(false);
            if (cam->isConnected(camIdx))  {
                cam->disconnectDevice(camIdx);
                //cam->setLogging(NULL);
            }
            logMain->log_text(tr("disconnected from device %1, camera %2!\n").arg(extension->getName()).arg(camIdx));
        }
    }
}

void QFESPIMB040MainWindow::disConnectAcquisition2() {
    if (cmbAcquisitionDevice2->currentIndex()<0) {
        // if there are no devices registered: deactivate everything!
        actDisConnectAcquisition2->setIcon(QIcon(":/spimb040/acquisitionconnect.png"));
        actDisConnectAcquisition2->setText(tr("&Connect"));
        actStartStopAcquisition2->setEnabled(false);
        actAcquireSingle2->setEnabled(false);
        actDisConnectAcquisition2->setEnabled(false);
        cmbAcquisitionDevice2->setEnabled(false);
        actCameraConfig2->setEnabled(false);
        return;
    }

    QPoint p = cmbAcquisitionDevice2->itemData(cmbAcquisitionDevice2->currentIndex()).toPoint();
    QFExtension* extension=NULL;
    QFExtensionCamera* cam=NULL;
    int camIdx=p.y();
    if ((p.x()>=0)&&(p.x()<cameras.size())) {
        extension=qobject_cast<QFExtension*>(cameras[p.x()]);
        cam=qobject_cast<QFExtensionCamera*>(cameras[p.x()]);
    }
    std::cout<<"disConnectAcquisition2()  dev="<<p.x()<<" cam="<<p.y()<<"  cam*="<<cam<<" extension*="<<extension<<std::endl;

    if (cam && extension) {
        if (actDisConnectAcquisition2->isChecked()) {
            // connect to a device

            connectDevice(1, p.x(), p.y());
            if (!cam->isConnected(camIdx)) {
                actDisConnectAcquisition2->setChecked(false);
                logMain->log_error(tr("error connecting to device %1, camera %2!\n").arg(extension->getName()).arg(camIdx));
            } else {
                actDisConnectAcquisition2->setIcon(QIcon(":/spimb040/acquisitiondisconnect.png"));
                actDisConnectAcquisition2->setText(tr("&Disconnect"));
                actStartStopAcquisition2->setEnabled(true);
                actStartStopAcquisition2->setChecked(false);
                actAcquireSingle2->setEnabled(true);
                actCameraConfig2->setEnabled(true);
                cmbAcquisitionDevice2->setEnabled(false);
                spinAcquisitionDelay2->setEnabled(true);
                logMain->log_text(tr("connected to device %1, camer %2!\n").arg(extension->getName()).arg(camIdx));
                camView2->show();
                acquireSingle2();
            }
        } else {
            //disconnect from the current device and delete the settings widget
            actDisConnectAcquisition2->setIcon(QIcon(":/spimb040/acquisitionconnect.png"));
            actDisConnectAcquisition2->setText(tr("&Connect"));
            actStartStopAcquisition2->setEnabled(false);
            actStartStopAcquisition2->setChecked(false);
            actStartStopAcquisition2->setIcon(QIcon(":/spimb040/acquisitionstart.png"));
            actStartStopAcquisition2->setText(tr("&Start Acquisition"));
            actAcquireSingle2->setEnabled(false);
            cmbAcquisitionDevice2->setEnabled(true);
            spinAcquisitionDelay2->setEnabled(false);
            actCameraConfig2->setEnabled(false);
            if (cam->isConnected(camIdx))  {
                cam->disconnectDevice(camIdx);
                //cam->setLogging(NULL);
            }
            logMain->log_text(tr("disconnected from device %1, camera %2!\n").arg(extension->getName()).arg(camIdx));
        }
    }
}


void QFESPIMB040MainWindow::configCamera1() {
    if (viewData.size()<=0) return;
    if (viewData[0]->camera) {
        int camIdx=viewData[0]->usedCamera;
        if (viewData[0]->camera->isConnected(camIdx)) {
            viewData[0]->camera->showSettingsWidget(camIdx, this);
        }
    }
}

void QFESPIMB040MainWindow::configCamera2() {
    if (viewData.size()<=1) return;
    if (viewData[1]->camera) {
        int camIdx=viewData[1]->usedCamera;
        if (viewData[1]->camera->isConnected(camIdx)) {
            viewData[1]->camera->showSettingsWidget(camIdx, this);
        }
    }
}



void QFESPIMB040MainWindow::acquireSingle1() {
    if (viewData.size()<=0) return;
    if (viewData[0]->camera) {
        int camIdx=viewData[0]->usedCamera;
        QFExtension* extension=viewData[0]->extension;
        QFExtensionCamera* cam=viewData[0]->camera;
        if (viewData[0]->camera->isConnected(camIdx)) {
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
            actStartStopAcquisition1->setChecked(false);
            actStartStopAcquisition1->setIcon(QIcon(":/spimb040/acquisitionstart.png"));
            actStartStopAcquisition1->setText(tr("&Start Acquisition"));
            if (acquireSingle(0)) {
                camView1->show();
                camView1->displayImage(viewData[0]->rawImage, viewData[0]->timestamp, viewData[0]->exposureTime);
            } else {
                logMain->log_error(tr("could not acquire frame, as device is not connected ...!\n"));
            }
            QApplication::restoreOverrideCursor();
        }
    }
}

void QFESPIMB040MainWindow::acquireSingle2() {
    if (viewData.size()<=1) return;
    if (viewData[1]->camera) {
        int camIdx=viewData[1]->usedCamera;
        QFExtension* extension=viewData[1]->extension;
        QFExtensionCamera* cam=viewData[1]->camera;
        if (viewData[1]->camera->isConnected(camIdx)) {
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
            actStartStopAcquisition2->setChecked(false);
            actStartStopAcquisition2->setIcon(QIcon(":/spimb040/acquisitionstart.png"));
            actStartStopAcquisition2->setText(tr("&Start Acquisition"));
            if (acquireSingle(1)) {
                camView2->show();
                camView2->displayImage(viewData[1]->rawImage, viewData[1]->timestamp, viewData[1]->exposureTime);
            } else {
                logMain->log_error(tr("could not acquire frame, as device is not connected ...!\n"));
            }
            QApplication::restoreOverrideCursor();
        }
    }
}




void QFESPIMB040MainWindow::startStopAcquisition1() {
    if (viewData.size()<=0) return;
    if (viewData[0]->camera) {
        int camIdx=viewData[0]->usedCamera;
        QFExtension* extension=viewData[0]->extension;
        QFExtensionCamera* cam=viewData[0]->camera;
        if (viewData[0]->camera->isConnected(camIdx)) {
            if (actStartStopAcquisition1->isChecked()) {
                actStartStopAcquisition1->setIcon(QIcon(":/spimb040/acquisitionstop.png"));
                actStartStopAcquisition1->setText(tr("&Stop Acquisition"));
                actAcquireSingle1->setEnabled(false);
                actCameraConfig1->setEnabled(false);
                viewData[0]->acqFrames=0;
                viewData[0]->acqFramesQR=0;
                viewData[0]->acqStarted.start();
                viewData[0]->acqStartedQR.start();
                viewData[0]->abort_continuous_acquisition=false;
                viewData[0]->continuous_is_first=true;
                camView1->show();
                acquireContinuous1();
            } else {
                actStartStopAcquisition1->setIcon(QIcon(":/spimb040/acquisitionstart.png"));
                actStartStopAcquisition1->setText(tr("&Start Acquisition"));
                actAcquireSingle1->setEnabled(true);
                actCameraConfig1->setEnabled(true);
            }
        }
    }
}

void QFESPIMB040MainWindow::startStopAcquisition2() {
    if (viewData.size()<=0) return;
    if (viewData[1]->camera) {
        int camIdx=viewData[1]->usedCamera;
        QFExtension* extension=viewData[1]->extension;
        QFExtensionCamera* cam=viewData[1]->camera;
        if (viewData[1]->camera->isConnected(camIdx)) {
            if (actStartStopAcquisition2->isChecked()) {
                actStartStopAcquisition2->setIcon(QIcon(":/spimb040/acquisitionstop.png"));
                actStartStopAcquisition2->setText(tr("&Stop Acquisition"));
                actAcquireSingle2->setEnabled(false);
                actCameraConfig2->setEnabled(false);
                viewData[1]->acqFrames=0;
                viewData[1]->acqFramesQR=0;
                viewData[1]->acqStarted.start();
                viewData[1]->acqStartedQR.start();
                viewData[1]->abort_continuous_acquisition=false;
                viewData[1]->continuous_is_first=true;
                camView2->show();
                acquireContinuous2();
            } else {
                actStartStopAcquisition2->setIcon(QIcon(":/spimb040/acquisitionstart.png"));
                actStartStopAcquisition2->setText(tr("&Start Acquisition"));
                actAcquireSingle2->setEnabled(true);
                actCameraConfig2->setEnabled(true);
            }
        }
    }
}




void QFESPIMB040MainWindow::acquireContinuous1() {
    if (viewData.size()<=0) return;
    if (viewData[0]->camera) {
        int camIdx=viewData[0]->usedCamera;
        QFExtension* extension=viewData[0]->extension;
        QFExtensionCamera* cam=viewData[0]->camera;
        if (viewData[0]->abort_continuous_acquisition) {
            actStartStopAcquisition1->setChecked(false);
            actStartStopAcquisition1->setIcon(QIcon(":/spimb040/acquisitionstart.png"));
            actStartStopAcquisition1->setText(tr("&Start Acquisition"));
            actAcquireSingle1->setEnabled(true);
            actCameraConfig1->setEnabled(true);
            return;
        }

        if (cam->isConnected(camIdx)) {
            if (viewData[0]->continuous_is_first) {
                viewData[0]->continuous_is_first=false;
            }
            if (acquireSingle(0)) {
                camView1->displayImage(viewData[0]->rawImage, viewData[0]->timestamp, viewData[0]->exposureTime);
            }
        } else {
            logMain->log_error(tr("could not acquire frame, as device is not connected ...!\n"));
        }


        if (viewData[0]->acqFramesQR%5==0) {
            double framerate=(double)viewData[0]->acqFramesQR/(double)(viewData[0]->acqStartedQR.elapsed()/1000.0);
            //labVideoSettings->setText(tr("%1 x %2 : %3 fps     ").arg(image.width()).arg(image.height()).arg(framerate));
            camView1->displayCameraConfig(extension->getName()+" #"+QString::number(camIdx), framerate);
            if (viewData[0]->acqStartedQR.elapsed()>10000) {
                viewData[0]->acqStartedQR.start();
                viewData[0]->acqFramesQR=0;
            }
        }
        viewData[0]->abort_continuous_acquisition=false;


        // start timer till next acquisition
        if (actStartStopAcquisition1->isChecked()) {
            QTimer::singleShot(spinAcquisitionDelay1->value(), this, SLOT(acquireContinuous1()));
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

void QFESPIMB040MainWindow::acquireContinuous2() {
    if (viewData.size()<=1) return;
    if (viewData[1]->camera) {
        int camIdx=viewData[1]->usedCamera;
        QFExtension* extension=viewData[1]->extension;
        QFExtensionCamera* cam=viewData[1]->camera;
        if (viewData[1]->abort_continuous_acquisition) {
            actStartStopAcquisition2->setChecked(false);
            actStartStopAcquisition2->setIcon(QIcon(":/spimb040/acquisitionstart.png"));
            actStartStopAcquisition2->setText(tr("&Start Acquisition"));
            actAcquireSingle2->setEnabled(true);
            actCameraConfig2->setEnabled(true);
            return;
        }

        if (cam->isConnected(camIdx)) {
            if (viewData[1]->continuous_is_first) {
                viewData[1]->continuous_is_first=false;
            }
            if (acquireSingle(1)) {
                camView2->displayImage(viewData[1]->rawImage, viewData[1]->timestamp, viewData[1]->exposureTime);
            }
        } else {
            logMain->log_error(tr("could not acquire frame, as device is not connected ...!\n"));
        }


        if (viewData[1]->acqFramesQR%5==0) {
            double framerate=(double)viewData[1]->acqFramesQR/(double)(viewData[1]->acqStartedQR.elapsed()/1000.0);
            camView2->displayCameraConfig(extension->getName()+" #"+QString::number(camIdx), framerate);
            if (viewData[1]->acqStartedQR.elapsed()>10000) {
                viewData[1]->acqStartedQR.start();
                viewData[1]->acqFramesQR=0;
            }
        }
        viewData[1]->abort_continuous_acquisition=false;


        // start timer till next acquisition
        if (actStartStopAcquisition2->isChecked()) {
            QTimer::singleShot(spinAcquisitionDelay2->value(), this, SLOT(acquireContinuous2()));
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


















void QFESPIMB040MainWindow::log_text(QString message) {
    logMain->log_text(message);
};

void QFESPIMB040MainWindow::log_warning(QString message) {
    logMain->log_warning(message);
};

void QFESPIMB040MainWindow::log_error(QString message) {
    logMain->log_error(message);
};





