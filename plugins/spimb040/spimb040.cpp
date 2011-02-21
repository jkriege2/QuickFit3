#include "spimb040.h"
#include <QtGui>

QFESPIMB040::QFESPIMB040(QObject* parent):
    QObject(parent)
{
    //ctor
}

QFESPIMB040::~QFESPIMB040()
{
    //dtor
}


void QFESPIMB040::deinit() {
}

void QFESPIMB040::projectChanged(QFProject* oldProject, QFProject* project) {
}

void QFESPIMB040::initExtension() {
    services->log_global_text(tr("initializing extension '%1' ...\n").arg(getName()));
    actStartPlugin=new QAction(QIcon(":/spimb040_logo.png"), tr("Start B040 SPIM Control"), this);
    connect(actStartPlugin, SIGNAL(triggered()), this, SLOT(startPlugin()));
    QToolBar* exttb=services->getToolbar("extensions");
    if (exttb) {
        exttb->addAction(actStartPlugin);
    }
    QMenu* extm=services->getMenu("extensions");
    if (extm) {
        extm->addAction(actStartPlugin);
    }
    services->log_global_text(tr("initializing extension '%1' ... DONE\n").arg(getName()));
}

void QFESPIMB040::loadSettings(ProgramOptions* settings) {
}

void QFESPIMB040::storeSettings(ProgramOptions* settings) {
}

void QFESPIMB040::startPlugin() {
    QMessageBox::information(parentWidget, getName(), getDescription());
}







Q_EXPORT_PLUGIN2(spimb040, QFESPIMB040)
