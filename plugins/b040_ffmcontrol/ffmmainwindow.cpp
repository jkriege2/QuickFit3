#include "ffmmainwindow.h"


ffmMainWindow::ffmMainWindow(QObject *parent)
{
        setUpUi();
        setUpAct();
}

ffmMainWindow::~ffmMainWindow()
{
}

void ffmMainWindow::setUpUi()
{

    resize(1024, 800);
    move(0,0);
    ml = new QGridLayout(this);
    vertsplit =  new QVisibleHandleSplitter(Qt::Vertical, this);
    horsplit = new QVisibleHandleSplitter(Qt::Horizontal, this);
    ml->addWidget(vertsplit);
    expdescr = new FFMExpDescriptionWidget(this);
    expcontrol = new FFMExpControlWidget(this);
    expimage = new FFMImageWidget(this);
    logwidget = new QWidget(this);
    vertsplit->addWidget(horsplit);
    vertsplit->addWidget(logwidget);
    horsplit->addWidget(expdescr);
    horsplit->addWidget(expcontrol);
    horsplit->addWidget(expimage);
    loglay = new QGridLayout(logwidget);
    logfile = new QtLogFile(logwidget);
    loglay->addWidget(logfile, 1,0);
    loglay->addWidget((new QLabel(tr("<b>Log Messages:</b>"))), 0, 0, 1, 1, Qt::AlignLeft);

    expcontrol->setLog(logfile); // enable scanner, autocorrelator...to log into the file;

}

void ffmMainWindow::setUpAct()
{

}

MulticontrolStage* ffmMainWindow::get_stage()
{
    return stage;
}

void ffmMainWindow::log_error(QString message)
{
}

void ffmMainWindow::log_warning(QString message)
{
}

void ffmMainWindow::log_text(QString message)
{
    logfile->log_text(message);
}

/*
void ffmMainWindow::loadSettings(ProgramOptions* settings) {
    jkloadWidgetGeometry((*settings->getQSettings()), this, "plugin_ffm_b040/");
    jkloadSplitter((*settings->getQSettings()), splitter, "plugin_ffm_b040/");
    if (optSetup) optSetup->loadSettings((*settings->getQSettings()), "plugin_ffm_b040/instrument/");
    if (widExperimentDescription) widExperimentDescription->loadSettings((*settings->getQSettings()), "plugin_spim_b040/expdescription/");
    if (widImageStack) widImageStack->loadSettings((*settings->getQSettings()), "plugin_ffm_b040/image_stack/");
    if (widAcquisition) widAcquisition->loadSettings((*settings->getQSettings()), "plugin_ffm_b040/acquisition/");
    if (widCamParamScan) widCamParamScan->loadSettings((*settings->getQSettings()), "plugin_ffm_b040/camparamscan/");
}

void ffmMainWindow::storeSettings(ProgramOptions* settings) {
    jksaveWidgetGeometry((*settings->getQSettings()), this, "plugin_ffm_b040/");
    jksaveSplitter((*settings->getQSettings()), splitter, "plugin_ffm_b040/");
    if (optSetup) optSetup->storeSettings((*settings->getQSettings()), "plugin_ffm_b040/instrument/");
    if (widExperimentDescription) widExperimentDescription->storeSettings((*settings->getQSettings()), "plugin_ffm_b040/expdescription/");
    if (widImageStack) widImageStack->storeSettings((*settings->getQSettings()), "plugin_ffm_b040/image_stack/");
    if (widAcquisition) widAcquisition->storeSettings((*settings->getQSettings()), "plugin_ffm_b040/acquisition/");
    if (widCamParamScan) widCamParamScan->storeSettings((*settings->getQSettings()), "plugin_ffm_b040/camparamscan/");
}

*/
