#include "qfacquisitionconfig.h"
#include <QtGui>
#include <iostream>





QFAcquisitionConfig::QFAcquisitionConfig(QWidget* parent):
    QGroupBox(parent)
{
    m_extManager=NULL;
    locked=false;



    // create widgets and actions

    createWidgets();
    createActions();
    displayStates(QFAcquisitionConfig::Disconnected);
    if (cmbAcquisitionDevice->count()<=0) displayStates(QFAcquisitionConfig::Inactive);
    init( NULL, "");
    setCheckable(true);
    setChecked(true);
}


QFAcquisitionConfig::~QFAcquisitionConfig() {
}


void QFAcquisitionConfig::init(QFPluginServices* pluginServices, QString configDirectory) {
    m_pluginServices=pluginServices;
    m_extManager=NULL;
    if (pluginServices) m_extManager=pluginServices->getExtensionManager();
    cmbAcquisitionDevice->init(m_extManager);
    setTitle(tr(" Acquisition: "));
    displayStates(QFAcquisitionConfig::Disconnected);
    if (cmbAcquisitionDevice->count()<=0) displayStates(QFAcquisitionConfig::Inactive);
}
void QFAcquisitionConfig::showEvent ( QShowEvent * event ) {
}


void QFAcquisitionConfig::closeEvent ( QCloseEvent * event ) {
    //QApplication::processEvents();
    // disconnect devices and close camera view:
    if (actDisConnect->isChecked()) {
        actDisConnect->setChecked(false);
        disconnectDevice();
    }
    //qDebug()<<"closed";
}

bool QFAcquisitionConfig::lockDevice(QFExtension** extension, QFExtensionAcquisition** ecamera, int* camera) {
    if (locked || (!actDisConnect->isChecked())) {
        *camera=-1;
        *extension=NULL;
        *ecamera=NULL;
        return false;
    }
    //QApplication::processEvents();
    //QApplication::processEvents();
    //QApplication::processEvents();
    displayStates(QFAcquisitionConfig::Locked);

    QString filename="";
    *extension=cmbAcquisitionDevice->currentExtension();
    *ecamera=cmbAcquisitionDevice->currentExtensionAcquisition();
    *camera=cmbAcquisitionDevice->currentAcquisitionID();
    locked=true;
    return true;
}

void QFAcquisitionConfig::releaseDevice() {
    if (locked) {
        displayStates(QFAcquisitionConfig::Connected);
        locked=false;
    }
}


void QFAcquisitionConfig::loadSettings(QSettings& settings, QString prefix) {

    setChecked(settings.value(prefix+"enabled", 0).toBool());
    //cmbAcquisitionDevice->setCurrentIndex(settings.value(prefix+"last_device", 0).toInt());
    cmbAcquisitionDevice->loadSettings(settings, prefix+"device/");
}

void QFAcquisitionConfig::storeSettings(QSettings& settings, QString prefix) {

    settings.setValue(prefix+"enabled", isChecked());
    //settings.setValue(prefix+"last_device", cmbAcquisitionDevice->currentIndex());
    cmbAcquisitionDevice->storeSettings(settings, prefix+"device/");

}



void QFAcquisitionConfig::createWidgets() {


    ////////////////////////////////////////////////////////////////////////////////////////////////
    // create main layout
    ////////////////////////////////////////////////////////////////////////////////////////////////
    QFormLayout* camlayout=new QFormLayout(this);
    setLayout(camlayout);





    ////////////////////////////////////////////////////////////////////////////////////////////////
    // create input widgets for camera devices
    ////////////////////////////////////////////////////////////////////////////////////////////////
    cmbAcquisitionDevice=new QFAcquisitionComboBox(this);
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




}





void QFAcquisitionConfig::createActions() {
    actDisConnect = new QAction(QIcon(":/spimb040/acquisitionconnect.png"), tr("&Connect"), this);
    actDisConnect->setCheckable(true);
    connect(actDisConnect, SIGNAL(toggled(bool)), this, SLOT(disConnectAcquisitionDevice()));

    btnConnect->setDefaultAction(actDisConnect);
}

void QFAcquisitionConfig::displayStates(QFAcquisitionConfig::States state) {
    if (state==QFAcquisitionConfig::Connected) {
        setEnabled(true);
        actDisConnect->setIcon(QIcon(":/spimb040/acquisitiondisconnect.png"));
        actDisConnect->setText(tr("&Disconnect"));
        actDisConnect->setEnabled(true);
        actDisConnect->setChecked(true);

        cmbAcquisitionDevice->setEnabled(false);




    } else if (state==QFAcquisitionConfig::Inactive) {
        setEnabled(false);
        actDisConnect->setIcon(QIcon(":/spimb040/acquisitionconnect.png"));
        actDisConnect->setText(tr("&Connect"));
        actDisConnect->setEnabled(false);
        actDisConnect->setChecked(false);


        cmbAcquisitionDevice->setEnabled(false);


    } else if (state==QFAcquisitionConfig::Locked) {
        setEnabled(true);
        actDisConnect->setIcon(QIcon(":/spimb040/acquisitiondisconnect.png"));
        actDisConnect->setText(tr("&Disconnect"));
        actDisConnect->setEnabled(false);
        actDisConnect->setChecked(true);


        cmbAcquisitionDevice->setEnabled(false);
    } else { // Disconnected
        setEnabled(true);
        actDisConnect->setIcon(QIcon(":/spimb040/acquisitionconnect.png"));
        actDisConnect->setText(tr("&Connect"));
        actDisConnect->setEnabled(true);
        actDisConnect->setChecked(false);


        cmbAcquisitionDevice->setEnabled(true);
    }
}


bool QFAcquisitionConfig::connectDevice(QFExtension* extension, QFExtensionAcquisition* cam, int camera, QObject* object) {


    locked=false;


    if (cam && extension) {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        cam->setLogging(m_log);
        bool s=cam->connectAcquisitionDevice(camera);
        if (!s) {
            //cam->setLogging(NULL);
            if (m_log) m_log->log_error(tr("error connecting to device %1, camera %2 ...\n").arg(extension->getName()).arg(camera));
        }
        QApplication::restoreOverrideCursor();
        return s;
    } else {
        return false;
    }
}

void QFAcquisitionConfig::setLog(QFPluginLogService* logger) {
    m_log=logger;
}


void QFAcquisitionConfig::disconnectDevice() {
    locked=false;
    if (viewData.camera) {
        if (viewData.camera->isConnected(viewData.usedCamera)) {
            viewData.camera->disconnectDevice(viewData.usedCamera);
        }
    }
}







void QFAcquisitionConfig::disConnectAcquisitionDevice() {
    if (cmbAcquisitionDevice->currentIndex()<0) {
        // if there are no devices registered: deactivate everything!
        displayStates(QFAcquisitionConfig::Disconnected);
        return;
    }

    QFExtension* extension=cmbAcquisitionDevice->currentExtension();
    QFExtensionAcquisition* cam=cmbAcquisitionDevice->currentExtensionCamera();
    int camIdx=cmbAcquisitionDevice->currentCameraID();

    //std::cout<<"disConnectAcquisitionDevice()  dev="<<p.x()<<" cam="<<p.y()<<"  cam*="<<cam<<" extension*="<<extension<<std::endl;

    if (cam && extension) {
        if (actDisConnect->isChecked()) {
            // connect to a device

            connectDevice(cmbAcquisitionDevice->currentExtension(), cmbAcquisitionDevice->currentExtensionCamera(), cmbAcquisitionDevice->currentCameraID(), cmbAcquisitionDevice->currentCameraQObject());
            if (!cam->isConnected(camIdx)) {
                displayStates(QFAcquisitionConfig::Disconnected);
                if (m_log) m_log->log_error(tr("error connecting to device %1, camera %2!\n").arg(extension->getName()).arg(camIdx));
            } else {
                displayStates(QFAcquisitionConfig::Connected);
                if (m_log) m_log->log_text(tr("connected to device %1, camera %2!\n").arg(extension->getName()).arg(camIdx));

            }
        } else {
            //disconnect from the current device and delete the settings widget
            displayStates(QFAcquisitionConfig::Disconnected);
            if (cam->isConnected(camIdx))  {
                cam->disconnectDevice(camIdx);

            }
            if (m_log) m_log->log_text(tr("disconnected from device %1, camera %2!\n").arg(extension->getName()).arg(camIdx));
        }
    }
}




void QFAcquisitionConfig::stop() {
}

void QFAcquisitionConfig::resume() {
}


void QFAcquisitionConfig::connectAcquition() {
    if (!isChecked()) return;

    bool b=actDisConnect->signalsBlocked();
    actDisConnect->blockSignals(true);
    actDisConnect->setChecked(true);
    disConnectAcquisitionDevice();
    actDisConnect->blockSignals(b);

}

void QFAcquisitionConfig::disconnectAcquition() {
    if (!isChecked()) return;

    bool b=actDisConnect->signalsBlocked();
    actDisConnect->blockSignals(true);
    actDisConnect->setChecked(false);
    disConnectAcquisitionDevice();
    actDisConnect->blockSignals(b);
}

void QFAcquisitionConfig::setReadOnly(bool readonly) {
    cmbAcquisitionDevice->setReadOnly(readonly);
}

bool QFAcquisitionConfig::isAcquitionConnected() const {
    if (viewData.camera) {
        int camIdx=viewData.usedCamera;
        return viewData.camera->isConnected(camIdx);
    }
    return false;
}

void QFAcquisitionConfig::storeMeasurements(QMap<QString, QVariant> &data, const QString &prefix) {
    QFExtensionMeasurementDevice* md=viewData.measurementDevice;
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
