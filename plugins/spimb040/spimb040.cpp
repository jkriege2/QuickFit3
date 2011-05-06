#include "spimb040.h"
#include <QtGui>

QFESPIMB040::QFESPIMB040(QObject* parent):
    QObject(parent)
{
    main=NULL;

}

QFESPIMB040::~QFESPIMB040()
{
    //dtor
}


void QFESPIMB040::deinit() {
    if (settings && main) {
        main->storeSettings(settings);
    }
    if (main) {
        main->close();
        delete main;
    }
}

void QFESPIMB040::projectChanged(QFProject* oldProject, QFProject* project) {
}

void QFESPIMB040::initExtension() {
    services->log_global_text(tr("initializing extension '%1' ...\n").arg(getName()));
    actStartPlugin=new QAction(QIcon(":/spimb040_logo.png"), tr("Start B040 SPIM Control"), this);

    QDir d(services->getConfigFileDirectory());
    // make sure the directory for the config files of this extension exists
    d.mkpath(services->getConfigFileDirectory()+"/plugins/extensions/"+getID());

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
    if (main) main->loadSettings(settings);
}

void QFESPIMB040::storeSettings(ProgramOptions* settings) {
    if (main) main->storeSettings(settings);
}

void QFESPIMB040::startPlugin() {
    //QMessageBox::information(parentWidget, getName(), getDescription());
    if (!main) {
        main=new QFESPIMB040MainWindow(services, NULL);
    }
    if (settings) main->loadSettings(settings);
    main->show();

}







Q_EXPORT_PLUGIN2(spimb040, QFESPIMB040)
