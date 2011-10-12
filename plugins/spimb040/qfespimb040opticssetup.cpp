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
    ui->camConfig1->setLog(parent);
    ui->camConfig2->init(1, m_pluginServices);
    ui->camConfig2->setLog(parent);
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

}

void QFESPIMB040OpticsSetup::storeSettings(QSettings& settings, QString prefix) {

}
