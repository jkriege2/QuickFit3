#include "qfecalculator.h"
#include <QtGui>
#include <QtPlugin>
#include <iostream>

#define LOG_PREFIX QString("qfe_calculator >>> ").toUpper()

QFECalculator::QFECalculator(QObject* parent):
    QObject(parent)
{
	logService=NULL;
    dlg=NULL;
    actStartPlugin=NULL;

}

QFECalculator::~QFECalculator() {

}


void QFECalculator::deinit() {
    if (dlg) {
        dlg->close();
        delete dlg;
        dlg=NULL;
    }
}

void QFECalculator::setHistory(const QString &history)
{
}

void QFECalculator::projectChanged(QFProject* oldProject, QFProject* project) {
	/* usually cameras do not have to react to a change of the project in QuickFit .. so you don't need to do anything here
	   But: possibly you could read config information from the project here
	 */
}

void QFECalculator::initExtension() {
    /* do initializations here but do not yet connect to the camera! */
    
    actStartPlugin=new QAction(QIcon(getIconFilename()), tr("Calculator"), this);
    connect(actStartPlugin, SIGNAL(triggered()), this, SLOT(startPlugin()));
    QMenu* extm=services->getMenu("tools");
    if (extm) {
        extm->addAction(actStartPlugin);
    }

    QToolBar* tb=services->getToolbar("tools");
    if (tb) {
        tb->addAction(actStartPlugin);
    }

}

void QFECalculator::startPlugin() {
    if (!dlg) dlg=new QFECalculatorDialog(this, NULL);
    dlg->show();
}

void QFECalculator::loadSettings(ProgramOptions* settingspo) {
	/* here you could read config information from the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini
	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFECalculator::storeSettings(ProgramOptions* settingspo) {
	/* here you could write config information to the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini

	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFECalculator::log_text(QString message) {
	if (logService) logService->log_text(LOG_PREFIX+message);
	else if (services) services->log_text(LOG_PREFIX+message);
}

void QFECalculator::log_warning(QString message) {
	if (logService) logService->log_warning(LOG_PREFIX+message);
	else if (services) services->log_warning(LOG_PREFIX+message);
}

void QFECalculator::log_error(QString message) {
	if (logService) logService->log_error(LOG_PREFIX+message);
	else if (services) services->log_error(LOG_PREFIX+message);
}


Q_EXPORT_PLUGIN2(qfe_calculator, QFECalculator)
