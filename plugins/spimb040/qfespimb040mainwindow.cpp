#include "qfespimb040mainwindow.h"
#include <QtGui>

QFESPIMB040MainWindow::QFESPIMB040MainWindow(QFExtensionManager* extManager, QWidget* parent):
    QWidget(parent)
{
    QHBoxLayout* hbl=new QHBoxLayout(this);
    setLayout(hbl);
    hbl->setContentsMargins(0,0,0,0);


    tabMain=new QTabWidget(this);
    hbl->addWidget(tabMain);


    camView1=new QFESPIMB040CameraView(extManager, this);
    tabMain->addTab(camView1, tr("Camera &1"));
}

QFESPIMB040MainWindow::~QFESPIMB040MainWindow()
{
    //dtor
}

void QFESPIMB040MainWindow::loadSettings(ProgramOptions* settings) {
    jkloadWidgetGeometry((*settings->getQSettings()), this, "plugin_spim_b040/");
    if (camView1) camView1->loadSettings(settings, "plugin_spim_b040/cam_view1_");
}

void QFESPIMB040MainWindow::storeSettings(ProgramOptions* settings) {
    jksaveWidgetGeometry((*settings->getQSettings()), this, "plugin_spim_b040/");
    if (camView1) camView1->storeSettings(settings, "plugin_spim_b040/cam_view1_");
}
