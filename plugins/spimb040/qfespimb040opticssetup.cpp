#include "qfespimb040opticssetup.h"
#include "objectives.h"
#include "ui_qfespimb040opticssetup.h"
#include "objectives.h"
#include "filters.h"
#include "qfespimb040mainwindow.h"

QFESPIMB040OpticsSetup::QFESPIMB040OpticsSetup(QWidget* parent,  QFPluginLogService* log, QFPluginServices* pluginServices) :
    QWidget(parent),
    ui(new Ui::QFESPIMB040OpticsSetup)
{
    m_pluginServices=pluginServices;
    m_log=log;
    ui->setupUi(this);
    ui->camConfig1->init(0, m_pluginServices, m_pluginServices->getGlobalConfigFileDirectory());
    ui->camConfig1->setLog(m_log);
    ui->camConfig2->init(1, m_pluginServices, m_pluginServices->getGlobalConfigFileDirectory());
    ui->camConfig2->setLog(m_log);
    ui->stageSetup->init(m_log, m_pluginServices);
    ui->filtTransmission->setFilterINI(m_pluginServices->getGlobalConfigFileDirectory()+"/spimb040_filters.ini", m_pluginServices->getConfigFileDirectory()+"/spimb040_filters.ini");
    ui->filtDetection->setFilterINI(m_pluginServices->getGlobalConfigFileDirectory()+"/spimb040_filters.ini", m_pluginServices->getConfigFileDirectory()+"/spimb040_filters.ini");
    ui->filtSplitter->setFilterINI(m_pluginServices->getGlobalConfigFileDirectory()+"/spimb040_filters.ini", m_pluginServices->getConfigFileDirectory()+"/spimb040_filters.ini");
    ui->filtDetection11->setFilterINI(m_pluginServices->getGlobalConfigFileDirectory()+"/spimb040_filters.ini", m_pluginServices->getConfigFileDirectory()+"/spimb040_filters.ini");
    ui->filtDetection21->setFilterINI(m_pluginServices->getGlobalConfigFileDirectory()+"/spimb040_filters.ini", m_pluginServices->getConfigFileDirectory()+"/spimb040_filters.ini");
    ui->objDetection->setObjectivesINI(m_pluginServices->getGlobalConfigFileDirectory()+"/spimb040_objectives.ini", m_pluginServices->getConfigFileDirectory()+"/spimb040_objectives.ini");
    ui->objProjection->setObjectivesINI(m_pluginServices->getGlobalConfigFileDirectory()+"/spimb040_objectives.ini", m_pluginServices->getConfigFileDirectory()+"/spimb040_objectives.ini");
    ui->objTube1->setObjectivesINI(m_pluginServices->getGlobalConfigFileDirectory()+"/spimb040_objectives.ini", m_pluginServices->getConfigFileDirectory()+"/spimb040_objectives.ini");
    ui->objTube2->setObjectivesINI(m_pluginServices->getGlobalConfigFileDirectory()+"/spimb040_objectives.ini", m_pluginServices->getConfigFileDirectory()+"/spimb040_objectives.ini");
    ui->shutterMainIllumination->init(m_log, m_pluginServices);
    ui->cmbLightpathConfig->setIcon(QIcon(":/spimb040/lightpath.png"));
    ui->btnLockFiltersEtc->setChecked(true);
}

QFESPIMB040OpticsSetup::~QFESPIMB040OpticsSetup()
{
    delete ui;
}

void QFESPIMB040OpticsSetup::lockStages() {
    ui->stageSetup->lockStages();
}

void QFESPIMB040OpticsSetup::unlockStages() {
    ui->stageSetup->unlockStages();
}

void QFESPIMB040OpticsSetup::closeEvent(QCloseEvent * event) {
    ui->camConfig1->close();
    ui->camConfig2->close();
}

void QFESPIMB040OpticsSetup::showEvent( QShowEvent * event ) {
    ui->camConfig1->show();
    ui->camConfig2->show();
}

void QFESPIMB040OpticsSetup::loadSettings(QSettings& settings, QString prefix) {
    ui->camConfig1->loadSettings(settings, prefix+"cam_config1/");
    ui->camConfig2->loadSettings(settings, prefix+"cam_config2/");
    ui->stageSetup->loadSettings(settings, prefix+"stages/");
    ui->filtDetection11->loadSettings(settings, prefix+"filters/detection11");
    ui->filtDetection21->loadSettings(settings, prefix+"filters/detection21");
    ui->filtSplitter->loadSettings(settings, prefix+"filters/detection_splitter");
    ui->filtTransmission->loadSettings(settings, prefix+"filters/illumination_transmission");
    ui->filtDetection->loadSettings(settings, prefix+"filters/detection");
    ui->objDetection->loadSettings(settings, prefix+"objectives/detection");
    ui->objProjection->loadSettings(settings, prefix+"objectives/projection");
    ui->objTube1->loadSettings(settings, prefix+"objectives/tubelens1");
    ui->objTube2->loadSettings(settings, prefix+"objectives/tubelens2");
    ui->shutterMainIllumination->loadSettings(settings, prefix+"main_illumination_shutter");
}

void QFESPIMB040OpticsSetup::storeSettings(QSettings& settings, QString prefix) {
    ui->camConfig1->storeSettings(settings, prefix+"cam_config1/");
    ui->camConfig2->storeSettings(settings, prefix+"cam_config2/");
    ui->stageSetup->storeSettings(settings, prefix+"stages/");
    ui->filtDetection11->saveSettings(settings, prefix+"filters/detection11");
    ui->filtDetection21->saveSettings(settings, prefix+"filters/detection21");
    ui->filtSplitter->saveSettings(settings, prefix+"filters/detection_splitter");
    ui->filtTransmission->saveSettings(settings, prefix+"filters/illumination_transmission");
    ui->filtDetection->saveSettings(settings, prefix+"filters/detection");
    ui->objDetection->saveSettings(settings, prefix+"objectives/detection");
    ui->objProjection->saveSettings(settings, prefix+"objectives/projection");
    ui->objTube1->saveSettings(settings, prefix+"objectives/tubelens1");
    ui->objTube2->saveSettings(settings, prefix+"objectives/tubelens2");
    ui->shutterMainIllumination->saveSettings(settings, prefix+"main_illumination_shutter");

}

double QFESPIMB040OpticsSetup::getCameraMagnification(int setup_cam) const {
    if (setup_cam==0) {
        return ui->objDetection->objective().magnification*ui->objTube1->objective().magnification;
    } else if (setup_cam==1) {
        return ui->objDetection->objective().magnification*ui->objTube2->objective().magnification;
    }
    return 1;
}

QMap<QString, QVariant> QFESPIMB040OpticsSetup::getSetup(int setup_cam ) const {
    QMap<QString, QVariant> setup;

    FilterDescription filter=ui->filtDetection->filter();
    if (filter.isValid) {
        setup["filters/detection/name"]=filter.name;
        setup["filters/detection/type"]=filter.type;
        setup["filters/detection/manufacturer"]=filter.manufacturer;
    }

    if (setup_cam<0 || setup_cam==0) {
        filter=ui->filtDetection11->filter();
        if (filter.isValid) {
            setup["filters/detection_cam1/name"]=filter.name;
            setup["filters/detection_cam1/type"]=filter.type;
            setup["filters/detection_cam1/manufacturer"]=filter.manufacturer;
        }
    }

    if (setup_cam<0 || setup_cam==1) {
        filter=ui->filtDetection21->filter();
        if (filter.isValid) {
            setup["filters/detection_cam2/name"]=filter.name;
            setup["filters/detection_cam2/type"]=filter.type;
            setup["filters/detection_cam2/manufacturer"]=filter.manufacturer;
        }
    }

    filter=ui->filtSplitter->filter();
    if (filter.isValid) {
        setup["filters/detection_splitter/name"]=filter.name;
        setup["filters/detection_splitter/type"]=filter.type;
        setup["filters/detection_splitter/manufacturer"]=filter.manufacturer;
    }

    filter=ui->filtTransmission->filter();
    if (filter.isValid) {
        setup["filters/illumination_transmission/name"]=filter.name;
        setup["filters/illumination_transmission/type"]=filter.type;
        setup["filters/illumination_transmission/manufacturer"]=filter.manufacturer;
    }

    ObjectiveDescription objective=ui->objDetection->objective();
    setup["objectives/detection/name"]=objective.name;
    setup["objectives/detection/manufacturer"]=objective.manufacturer;
    setup["objectives/detection/NA"]=objective.NA;
    setup["objectives/detection/magnification"]=objective.magnification;

    objective=ui->objProjection->objective();
    setup["objectives/projection/name"]=objective.name;
    setup["objectives/projection/manufacturer"]=objective.manufacturer;
    setup["objectives/projection/NA"]=objective.NA;
    setup["objectives/projection/magnification"]=objective.magnification;

    if (setup_cam<0 || setup_cam==0) {
        objective=ui->objTube1->objective();
        setup["objectives/tube_lens1/name"]=objective.name;
        setup["objectives/tube_lens1/manufacturer"]=objective.manufacturer;
        setup["objectives/tube_lens1/NA"]=objective.NA;
        setup["objectives/tube_lens1/magnification"]=objective.magnification;
    }

    if (setup_cam<0 || setup_cam==1) {
        objective=ui->objTube2->objective();
        setup["objectives/tube_lens2/name"]=objective.name;
        setup["objectives/tube_lens2/manufacturer"]=objective.manufacturer;
        setup["objectives/tube_lens2/NA"]=objective.NA;
        setup["objectives/tube_lens2/magnification"]=objective.magnification;
    }


    return setup;
}


void QFESPIMB040OpticsSetup::setLogging(QFPluginLogService* log) {
    m_log=log;
    ui->camConfig1->setLog(m_log);
    ui->camConfig2->setLog(m_log);
    ui->stageSetup->setLog(m_log);
    ui->shutterMainIllumination->setLog(m_log);
}

bool QFESPIMB040OpticsSetup::lockCamera(int setup_cam, QFExtension** extension, QFExtensionCamera** ecamera, int* camera, QString* acquisitionSettingsFilename, QString* previewSettingsFilename) {
    switch(setup_cam) {
        case 0: return ui->camConfig1->lockCamera(extension, ecamera, camera, acquisitionSettingsFilename, previewSettingsFilename);
        case 1: return ui->camConfig2->lockCamera(extension, ecamera, camera, acquisitionSettingsFilename, previewSettingsFilename);
    }
    return false;
}

void QFESPIMB040OpticsSetup::releaseCamera(int setup_cam) {
    switch(setup_cam) {
        case 0: ui->camConfig1->releaseCamera(); break;
        case 1: ui->camConfig2->releaseCamera(); break;
    }
}

void QFESPIMB040OpticsSetup::on_btnConnectDevices_clicked() {
    ui->stageSetup->connectStages();
    ui->shutterMainIllumination->connectShutter();
}

void QFESPIMB040OpticsSetup::on_btnConnectCameras_clicked() {
    ui->camConfig1->connectCamera();
    ui->camConfig2->connectCamera();
}

void QFESPIMB040OpticsSetup::on_btnDisconnectDevices_clicked() {
    ui->stageSetup->disconnectStages();
    ui->shutterMainIllumination->disconnectShutter();
}

void QFESPIMB040OpticsSetup::on_btnDisconnectCameras_clicked() {
    ui->camConfig1->disconnectCamera();
    ui->camConfig2->disconnectCamera();
}


void QFESPIMB040OpticsSetup::updateMagnifications() {
    double m=ui->objDetection->objective().magnification*ui->objTube1->objective().magnification;
    ui->camConfig1->setMagnification(m);
    m=ui->objDetection->objective().magnification*ui->objTube2->objective().magnification;
    ui->camConfig2->setMagnification(m);
}


QFExtensionLinearStage* QFESPIMB040OpticsSetup::getXStage() {
    return ui->stageSetup->getXStage();
}

QFExtensionLinearStage* QFESPIMB040OpticsSetup::getYStage() {
    return ui->stageSetup->getYStage();
}

QFExtensionLinearStage* QFESPIMB040OpticsSetup::getZStage() {
    return ui->stageSetup->getZStage();
}

QFExtension* QFESPIMB040OpticsSetup::getXStageExtension() {
    return ui->stageSetup->getXStageExtension();
}

QFExtension* QFESPIMB040OpticsSetup::getYStageExtension() {
    return ui->stageSetup->getYStageExtension();
}

QFExtension* QFESPIMB040OpticsSetup::getZStageExtension() {
    return ui->stageSetup->getZStageExtension();
}

int QFESPIMB040OpticsSetup::getXStageAxis() {
    return ui->stageSetup->getXStageAxis();
}

int QFESPIMB040OpticsSetup::getYStageAxis() {
    return ui->stageSetup->getYStageAxis();
}

int QFESPIMB040OpticsSetup::getZStageAxis() {
    return ui->stageSetup->getZStageAxis();
}

bool QFESPIMB040OpticsSetup::isXStageConnected() const {
    return ui->stageSetup->isXStageConnected();
}

bool QFESPIMB040OpticsSetup::isYStageConnected() const {
    return ui->stageSetup->isYStageConnected();
}

bool QFESPIMB040OpticsSetup::isZStageConnected() const {
    return ui->stageSetup->isZStageConnected();
}

bool QFESPIMB040OpticsSetup::isStageConnected(QFExtensionLinearStage* stage, int id, bool& found) {
    found=false;
    if (!stage || id<0) return false;
    found=true;
    if (stage==getXStage() && id==getXStageAxis()) return isXStageConnected();
    if (stage==getYStage() && id==getYStageAxis()) return isYStageConnected();
    if (stage==getZStage() && id==getZStageAxis()) return isZStageConnected();
    found=false;
    return false;
}

QFCameraComboBox* QFESPIMB040OpticsSetup::cameraComboBox(int camera) const {
    if (camera==0) return ui->camConfig1->cameraComboBox();
    if (camera==1) return ui->camConfig2->cameraComboBox();

    return NULL;
}

QFCameraConfigComboBoxStartResume* QFESPIMB040OpticsSetup::getStopRelease(int camera) const {
    if (camera==0) return ui->camConfig1;
    if (camera==1) return ui->camConfig2;

    return NULL;
}

void QFESPIMB040OpticsSetup::setMainIlluminationShutter(bool opened) {
    ui->shutterMainIllumination->setShutter(opened);
}

bool QFESPIMB040OpticsSetup::getMainIlluminationShutter() {
    return ui->shutterMainIllumination->getShutterState();
}

bool QFESPIMB040OpticsSetup::isMainIlluminationShutterAvailable()  {
    return ui->shutterMainIllumination->isShutterConnected();
}
