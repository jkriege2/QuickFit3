#include "qfespectraviewer.h"
#include <QtGui>
#include <QtPlugin>
#include <iostream>
#include "qfespectraviewerdialog.h"
#include "optionswidget.h"
#include "spectrummanager.h"


#define LOG_PREFIX QString("qfe_spectraviewer >>> ").toUpper()

QFESpectraViewer::QFESpectraViewer(QObject* parent):
    QObject(parent)
{
	logService=NULL;
    dlg=NULL;
    manager=new SpectrumManager();
    loadThread=new QFESpectraViewerLoadThread(manager, this);
    connect(loadThread, SIGNAL(slog_text(QString)), this, SLOT(doLog(QString)));
}

QFESpectraViewer::~QFESpectraViewer() {
    if (dlg) {
        dlg->close();
        delete dlg;
        dlg=NULL;
    }
    delete manager;
}


void QFESpectraViewer::deinit() {
    /* add code for cleanup here */
    if (dlg) dlg->writeSettings();
}

QString QFESpectraViewer::pluginOptionsName() const
{
    return getName();
}

QIcon QFESpectraViewer::pluginOptionsIcon() const
{
    return QIcon(getIconFilename());
}

QFPluginOptionsWidget *QFESpectraViewer::createOptionsWidget(QWidget *parent)
{
    return new OptionsWidget(this, parent);
}

SpectrumManager *QFESpectraViewer::getSpectrumManager() const
{
    return manager;
}

void QFESpectraViewer::reloadDatabases()
{
    log_text(intReloadDatabases(manager, getID()));
}

QString QFESpectraViewer::intReloadDatabases(SpectrumManager *manager, const QString& id)
{
    QString result="";
    result+=(tr("   loading databases from:\n"));
    QStringList directories;
    QList<bool> readonly;
    directories<<QFPluginServices::getInstance()->getPluginAssetsDirectory(id);
    readonly<<true;
    directories<<ProgramOptions::getConfigValue("qfe_spectraviewer/user_database", QFPluginServices::getInstance()->getPluginConfigDirectory("qfe_spectraviewer")).toString();
    readonly<<false;
    for (int i=0; i<directories.size(); i++) {
        QString ro="";
        if (readonly.value(i, false)) ro=tr(" [readonly]");
        result+=(tr("     - %1%2\n").arg(directories[i]).arg(ro));
    }

    manager->clear();
    result+=(tr("   loading fluorophore database ...\n"));
    manager->loadFluorophoreDatabase("fluorophors.ini", directories);
    result+=(tr("   loading filters database ...\n"));
    manager->loadFilterDatabase("filters.ini", directories);
    result+=(tr("   loading lightsources database ...\n"));
    manager->loadLightSourceDatabase("ligtsources.ini", directories);
    result+=(tr("   loading detectors database ...\n"));
    manager->loadDetectorDatabase("detectors.ini", directories);
    result+=(tr("   loading laserline database ...\n"));
    manager->loadLaserlinesDatabase("laserlines.ini", directories);

    result+=(tr("   loaded %1 fluorophores\n").arg(manager->getFluorophoreCount()));
    result+=(tr("   loaded %1 filters\n").arg(manager->getFilterCount()));
    result+=(tr("   loaded %1 lightsources\n").arg(manager->getLightSourceCount()));
    result+=(tr("   loaded %1 detectors\n").arg(manager->getDetectorCount()));
    result+=(tr("   loaded %1 spectra\n").arg(manager->getSpectraCount()));
    return result;
}

void QFESpectraViewer::projectChanged(QFProject* oldProject, QFProject* project) {
	/* usually cameras do not have to react to a change of the project in QuickFit .. so you don't need to do anything here
	   But: possibly you could read config information from the project here
	 */
}

void QFESpectraViewer::initExtension() {
    /* do initializations here but do not yet connect to the camera! */
    
	// some example code that may be used to register a menu and a tool button:
    log_text(tr("initializing ...\n"));
    log_text(tr("   registering plugin ...\n"));
    loadThread->start();
    QAction* actStartPlugin=new QAction(QIcon(getIconFilename()), tr("Spectra Viewer"), this);
    connect(actStartPlugin, SIGNAL(triggered()), this, SLOT(showViewer()));
    QToolBar* exttb=services->getToolbar("tools");
    if (exttb) {
        exttb->addAction(actStartPlugin);
    }
    QMenu* extm=services->getMenu("tools");
    if (extm) {
        extm->addAction(actStartPlugin);
    }

    log_text(tr("initializing ... DONE\n"));
}

void QFESpectraViewer::loadSettings(ProgramOptions* settingspo) {
	/* here you could read config information from the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini
	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFESpectraViewer::storeSettings(ProgramOptions* settingspo) {
	/* here you could write config information to the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini

	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFESpectraViewer::log_text(QString message) {
    if (services) services->log_global_text(LOG_PREFIX+message);
}

void QFESpectraViewer::log_warning(QString message) {
    if (services) services->log_global_warning(LOG_PREFIX+message);
}

void QFESpectraViewer::log_error(QString message) {
    if (services) services->log_global_error(LOG_PREFIX+message);
}

void QFESpectraViewer::showViewer()
{
    if (loadThread->isRunning()) return;
    if (!dlg) dlg=new QFESpectraViewerDialog(this, NULL);
    dlg->show();
    dlg->raise();
}

void QFESpectraViewer::doLog(const QString &text)
{
    log_text(text);
}



QFESpectraViewerLoadThread::QFESpectraViewerLoadThread(SpectrumManager *manager, QFESpectraViewer *parent): QThread(parent)
{
    this->manager=manager;
    sv=parent;
}

void QFESpectraViewerLoadThread::log_text(const QString &text)
{
    emit slog_text(text);
}

void QFESpectraViewerLoadThread::run()
{
    try {
        log_text(QFESpectraViewer::intReloadDatabases(manager, sv->getID()));
    } catch(std::exception& E) {
        log_text(tr("error loading spectra viewer database: %1\n").arg(E.what()));
    }
}



Q_EXPORT_PLUGIN2(qfe_spectraviewer, QFESpectraViewer)

