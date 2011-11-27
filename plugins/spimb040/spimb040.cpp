#include "spimb040.h"
#include <QtGui>

QFESPIMB040::QFESPIMB040(QObject* parent):
    QObject(parent)
{
    main=NULL;
    main_old=NULL;
}

QFESPIMB040::~QFESPIMB040()
{
    //dtor
}


void QFESPIMB040::deinit() {
    if (settings && main) {
        main->storeSettings(settings);
    }
    if (settings && main_old) {
        main_old->storeSettings(settings);
    }
    if (main) {
        main->close();
        delete main;
    }
    if (main_old) {
        main_old->close();
        delete main_old;
    }
}

void QFESPIMB040::projectChanged(QFProject* oldProject, QFProject* project) {
}

void QFESPIMB040::initExtension() {
    services->log_global_text(tr("initializing extension '%1' ...\n").arg(getName()));
    actStartPlugin=new QAction(QIcon(":/spimb040_logo.png"), tr("Start B040 SPIM Control"), this);
    actStartPluginOld=new QAction(QIcon(":/spimb040_logo_shaded.png"), tr("Start old B040 SPIM Control"), this);

    QDir d(services->getConfigFileDirectory());
    // make sure the directory for the config files of this extension exists
    d.mkpath(services->getConfigFileDirectory()+"/plugins/"+getID());

    connect(actStartPlugin, SIGNAL(triggered()), this, SLOT(startPlugin()));
    connect(actStartPluginOld, SIGNAL(triggered()), this, SLOT(startPluginOld()));

    QToolBar* exttb=services->getToolbar("extensions");
    //std::cout<<"extensions toolbars: "<<exttb<<std::endl;
    if (exttb) {
        exttb->addAction(actStartPlugin);
        exttb->addAction(actStartPluginOld);
    }
    QMenu* extm=services->getMenu("extensions");
    //std::cout<<"extensions menu: "<<extm<<std::endl;
    if (extm) {
        extm->addAction(actStartPlugin);
        extm->addAction(actStartPluginOld);
    }
    services->log_global_text(tr("initializing extension '%1' ... DONE\n").arg(getName()));
}

void QFESPIMB040::loadSettings(ProgramOptions* settings) {
    if (main) main->loadSettings(settings);
    if (main_old) main_old->loadSettings(settings);
}

void QFESPIMB040::storeSettings(ProgramOptions* settings) {
    if (main) main->storeSettings(settings);
    if (main_old) main_old->storeSettings(settings);
}

void QFESPIMB040::startPlugin() {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    //QMessageBox::information(parentWidget, getName(), getDescription());
    if (!main) {
        main=new QFESPIMB040MainWindow2(services, NULL);
    }
    if (settings) main->loadSettings(settings);
    main->show();
    QApplication::restoreOverrideCursor();
}



void QFESPIMB040::startPluginOld() {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    //QMessageBox::information(parentWidget, getName(), getDescription());
    if (!main_old) {
        main_old=new QFESPIMB040MainWindow(services, NULL);
    }
    if (settings) main_old->loadSettings(settings);
    main_old->show();
    QApplication::restoreOverrideCursor();
}





Q_EXPORT_PLUGIN2(spimb040, QFESPIMB040)
