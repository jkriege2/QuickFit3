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
    ui->setupUi(this);
    ui->camConfig1->init(0, m_pluginServices);
    ui->camConfig1->setLog(this);
    ui->camConfig2->init(1, m_pluginServices);
    ui->camConfig2->setLog(this);
    ui->stageSetup->init(this, m_pluginServices);
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


void QFESPIMB040OpticsSetup::log_text(QString message) {
    ui->logWidget->log_text(message);
}

void QFESPIMB040OpticsSetup::log_warning(QString message) {
    ui->logWidget->log_warning(message);
}

void QFESPIMB040OpticsSetup::log_error(QString message) {
    ui->logWidget->log_error(message);
}
