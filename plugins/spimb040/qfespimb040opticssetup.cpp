#include "qfespimb040opticssetup.h"
#include "objectives.h"
#include "ui_qfespimb040opticssetup.h"
#include "objectives.h"
#include "filters.h"
#include "qfespimb040mainwindow.h"

QFESPIMB040OpticsSetup::QFESPIMB040OpticsSetup(QFESPIMB040MainWindow* parent, QFPluginServices* pluginServices) :
    QWidget(parent),
    ui(new Ui::QFESPIMB040OpticsSetup)
{
    m_parent=parent;
    m_pluginServices=pluginServices;
    m_log=parent;
    ui->setupUi(this);
    ui->camConfig1->init(0, m_pluginServices);
    ui->camConfig1->setLog(m_log);
    ui->camConfig2->init(1, m_pluginServices);
    ui->camConfig2->setLog(m_log);
    ui->stageSetup->init(m_log, m_pluginServices);
    ui->filtTransmission->setFilterINI(m_pluginServices->getGlobalConfigFileDirectory()+"/spimb040_filters.ini", m_pluginServices->getConfigFileDirectory()+"/spimb040_filters.ini");
    ui->filtSplitter->setFilterINI(m_pluginServices->getGlobalConfigFileDirectory()+"/spimb040_filters.ini", m_pluginServices->getConfigFileDirectory()+"/spimb040_filters.ini");
    ui->filtDetection11->setFilterINI(m_pluginServices->getGlobalConfigFileDirectory()+"/spimb040_filters.ini", m_pluginServices->getConfigFileDirectory()+"/spimb040_filters.ini");
    ui->filtDetection21->setFilterINI(m_pluginServices->getGlobalConfigFileDirectory()+"/spimb040_filters.ini", m_pluginServices->getConfigFileDirectory()+"/spimb040_filters.ini");
    ui->objDetection->setObjectivesINI(m_pluginServices->getGlobalConfigFileDirectory()+"/spimb040_objectives.ini", m_pluginServices->getConfigFileDirectory()+"/spimb040_objectives.ini");
    ui->objProjection->setObjectivesINI(m_pluginServices->getGlobalConfigFileDirectory()+"/spimb040_objectives.ini", m_pluginServices->getConfigFileDirectory()+"/spimb040_objectives.ini");
    ui->objTube1->setObjectivesINI(m_pluginServices->getGlobalConfigFileDirectory()+"/spimb040_objectives.ini", m_pluginServices->getConfigFileDirectory()+"/spimb040_objectives.ini");
    ui->objTube2->setObjectivesINI(m_pluginServices->getGlobalConfigFileDirectory()+"/spimb040_objectives.ini", m_pluginServices->getConfigFileDirectory()+"/spimb040_objectives.ini");
}

QFESPIMB040OpticsSetup::~QFESPIMB040OpticsSetup()
{
    delete ui;
}

void QFESPIMB040OpticsSetup::loadSettings(QSettings& settings, QString prefix) {
    ui->camConfig1->loadSettings(settings, prefix+"cam_config1/");
    ui->camConfig2->loadSettings(settings, prefix+"cam_config2/");
    ui->stageSetup->loadSettings(settings, prefix+"stages/");
    ui->filtDetection11->loadSettings(settings, prefix+"filters/detection11");
    ui->filtDetection21->loadSettings(settings, prefix+"filters/detection21");
    ui->filtSplitter->loadSettings(settings, prefix+"filters/detection_splitter");
    ui->filtTransmission->loadSettings(settings, prefix+"filters/illumination_transmission");
    ui->objDetection->loadSettings(settings, prefix+"objectives/detection");
    ui->objProjection->loadSettings(settings, prefix+"objectives/projection");
    ui->objTube1->loadSettings(settings, prefix+"objectives/tubelens1");
    ui->objTube2->loadSettings(settings, prefix+"objectives/tubelens1");

}

void QFESPIMB040OpticsSetup::storeSettings(QSettings& settings, QString prefix) {
    ui->camConfig1->storeSettings(settings, prefix+"cam_config1/");
    ui->camConfig2->storeSettings(settings, prefix+"cam_config2/");
    ui->stageSetup->storeSettings(settings, prefix+"stages/");
    ui->filtDetection11->saveSettings(settings, prefix+"filters/detection11");
    ui->filtDetection21->saveSettings(settings, prefix+"filters/detection21");
    ui->filtSplitter->saveSettings(settings, prefix+"filters/detection_splitter");
    ui->filtTransmission->saveSettings(settings, prefix+"filters/illumination_transmission");
    ui->objDetection->saveSettings(settings, prefix+"objectives/detection");
    ui->objProjection->saveSettings(settings, prefix+"objectives/projection");
    ui->objTube1->saveSettings(settings, prefix+"objectives/tubelens1");
    ui->objTube2->saveSettings(settings, prefix+"objectives/tubelens1");

}



void QFESPIMB040OpticsSetup::setLogging(QFPluginLogService* log) {
    m_log=log;
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

