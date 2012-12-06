#include "qfehelpeditor.h"
#include <QtGui>
#include <QtPlugin>
#include <iostream>

#define LOG_PREFIX QString("qfe_helpeditor >>> ").toUpper()

QFEHelpEditor::QFEHelpEditor(QObject* parent):
    QObject(parent)
{
	logService=NULL;
    dlg=NULL;
}

QFEHelpEditor::~QFEHelpEditor() {
    if (dlg) delete dlg;
    dlg=NULL;
}


void QFEHelpEditor::deinit() {
	/* add code for cleanup here */
    if (dlg) dlg->storeSettings(*(ProgramOptions::getInstance()->getQSettings()), getID());
}

void QFEHelpEditor::projectChanged(QFProject* oldProject, QFProject* project) {
	/* usually cameras do not have to react to a change of the project in QuickFit .. so you don't need to do anything here
	   But: possibly you could read config information from the project here
	 */
}

void QFEHelpEditor::initExtension() {
    /* do initializations here but do not yet connect to the camera! */
    
	// some example code that may be used to register a menu and a tool button:
	services->log_global_text(tr("initializing extension '%1' ...\n").arg(getName()));
    
    actStartPlugin=new QAction(QIcon(getIconFilename()), tr("Online-Help Editor"), this);
    connect(actStartPlugin, SIGNAL(triggered()), this, SLOT(startPlugin()));
    QMenu* extm=services->getMenu("tools");
    if (extm) {
        extm->addAction(actStartPlugin);
    }
    services->log_global_text(tr("initializing extension '%1' ... DONE\n").arg(getName()));
}

void QFEHelpEditor::startPlugin() {
    if (!dlg) {
        dlg=new QFEHelpEditorWidget(NULL);
        dlg->loadSettings(*(ProgramOptions::getInstance()->getQSettings()), getID());
    }
    dlg->show();
    dlg->raise();
}

void QFEHelpEditor::loadSettings(ProgramOptions* settingspo) {
	/* here you could read config information from the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini
	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

    if (dlg) dlg->loadSettings(*(ProgramOptions::getInstance()->getQSettings()), getID());
}

void QFEHelpEditor::storeSettings(ProgramOptions* settingspo) {
	/* here you could write config information to the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini

	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);
    if (dlg) dlg->storeSettings(*(ProgramOptions::getInstance()->getQSettings()), getID());
}

void QFEHelpEditor::log_text(QString message) {
	if (logService) logService->log_text(LOG_PREFIX+message);
	else if (services) services->log_text(LOG_PREFIX+message);
}

void QFEHelpEditor::log_warning(QString message) {
	if (logService) logService->log_warning(LOG_PREFIX+message);
	else if (services) services->log_warning(LOG_PREFIX+message);
}

void QFEHelpEditor::log_error(QString message) {
	if (logService) logService->log_error(LOG_PREFIX+message);
	else if (services) services->log_error(LOG_PREFIX+message);
}


Q_EXPORT_PLUGIN2(qfe_helpeditor, QFEHelpEditor)
