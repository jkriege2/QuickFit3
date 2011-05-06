#include "qfespimb040mainwindow.h"
#include <QtGui>





QFESPIMB040MainWindow::QFESPIMB040MainWindow(QFExtensionServices* pluginServices, QWidget* parent):
    QWidget(parent, Qt::Dialog|Qt::WindowMaximizeButtonHint|Qt::WindowCloseButtonHint|Qt::WindowSystemMenuHint)
{
    camConfig1=NULL;
    camConfig2=NULL;
    m_pluginServices=pluginServices;
    // create widgets and actions
    createWidgets(pluginServices->getExtensionManager());
    createActions();
}

QFESPIMB040MainWindow::~QFESPIMB040MainWindow()
{
    if (camConfig1) {
        camConfig1->close();
        delete camConfig1;
        camConfig1=NULL;
    }
    if (camConfig2) {
        camConfig2->close();
        delete camConfig2;
        camConfig2=NULL;
    }
}

void QFESPIMB040MainWindow::loadSettings(ProgramOptions* settings) {
    jkloadWidgetGeometry((*settings->getQSettings()), this, "plugin_spim_b040/");
    if (camConfig1) camConfig1->loadSettings(settings, "plugin_spim_b040/cam_config1/");
    if (camConfig2) camConfig2->loadSettings(settings, "plugin_spim_b040/cam_config2/");
    if (sampleStages) sampleStages->loadSettings(settings, "plugin_spim_b040/sample_stages/");
}

void QFESPIMB040MainWindow::storeSettings(ProgramOptions* settings) {
    jksaveWidgetGeometry((*settings->getQSettings()), this, "plugin_spim_b040/");
    if (camConfig1) camConfig1->storeSettings(settings, "plugin_spim_b040/cam_config1/");
    if (camConfig2) camConfig2->storeSettings(settings, "plugin_spim_b040/cam_config2/");
    if (sampleStages) sampleStages->storeSettings(settings, "plugin_spim_b040/sample_stages/");
}

void QFESPIMB040MainWindow::closeEvent ( QCloseEvent * event ) {
    if (camConfig1) {
        camConfig1->close();
    }
    if (camConfig2) {
        camConfig2->close();
    }
}

void QFESPIMB040MainWindow::showEvent( QShowEvent * event )  {
   if (camConfig1) {
        camConfig1->show();
    }
    if (camConfig2) {
        camConfig2->show();
    }
}

void QFESPIMB040MainWindow::createWidgets(QFExtensionManager* extManager) {
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // create main layout
    ////////////////////////////////////////////////////////////////////////////////////////////////
    QGridLayout* mainlayout=new QGridLayout(this);
    setLayout(mainlayout);



    ////////////////////////////////////////////////////////////////////////////////////////////////
    // create input widgets for camera devices
    ////////////////////////////////////////////////////////////////////////////////////////////////
    logMain=new QtLogFile(this);
    mainlayout->addWidget(logMain, 2,0,1,2);


    ////////////////////////////////////////////////////////////////////////////////////////////////
    // create camera config widgets
    ////////////////////////////////////////////////////////////////////////////////////////////////
    camConfig1=new QFESPIMB040CameraConfig(this, 0, m_pluginServices);
    mainlayout->addWidget(camConfig1, 0, 0);
    camConfig2=new QFESPIMB040CameraConfig(this, 1, m_pluginServices);
    mainlayout->addWidget(camConfig2, 0, 1);

    connect(camConfig1, SIGNAL(configFilesChanged()), camConfig2, SLOT(rereadConfigCombos()));
    connect(camConfig2, SIGNAL(configFilesChanged()), camConfig1, SLOT(rereadConfigCombos()));

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // create sample stage widget
    ////////////////////////////////////////////////////////////////////////////////////////////////
    sampleStages=new QFESPIMB040SampleStageConfig(this, m_pluginServices);
    mainlayout->addWidget(sampleStages, 1, 0);



}

void QFESPIMB040MainWindow::createActions() {

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





