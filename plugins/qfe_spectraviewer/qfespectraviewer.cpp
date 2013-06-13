#include "qfespectraviewer.h"
#include <QtGui>
#include <QtPlugin>
#include <iostream>
#include "qfespectraviewerdialog.h"
#include "optionswidget.h"

#define LOG_PREFIX QString("qfe_spectraviewer >>> ").toUpper()

QFESpectraViewer::QFESpectraViewer(QObject* parent):
    QObject(parent)
{
	logService=NULL;
    dlg=NULL;
}

QFESpectraViewer::~QFESpectraViewer() {
    if (dlg) {
        dlg->close();
        delete dlg;
        dlg=NULL;
    }
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

void QFESpectraViewer::projectChanged(QFProject* oldProject, QFProject* project) {
	/* usually cameras do not have to react to a change of the project in QuickFit .. so you don't need to do anything here
	   But: possibly you could read config information from the project here
	 */
}

void QFESpectraViewer::initExtension() {
    /* do initializations here but do not yet connect to the camera! */
    
	// some example code that may be used to register a menu and a tool button:
    
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
	if (logService) logService->log_text(LOG_PREFIX+message);
	else if (services) services->log_text(LOG_PREFIX+message);
}

void QFESpectraViewer::log_warning(QString message) {
	if (logService) logService->log_warning(LOG_PREFIX+message);
	else if (services) services->log_warning(LOG_PREFIX+message);
}

void QFESpectraViewer::log_error(QString message) {
	if (logService) logService->log_error(LOG_PREFIX+message);
    else if (services) services->log_error(LOG_PREFIX+message);
}

void QFESpectraViewer::showViewer()
{
    if (!dlg) dlg=new QFESpectraViewerDialog(this, NULL);
    dlg->show();
    dlg->raise();
}


Q_EXPORT_PLUGIN2(qfe_spectraviewer, QFESpectraViewer)
