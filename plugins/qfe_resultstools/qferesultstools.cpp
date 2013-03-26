#include "qferesultstools.h"
#include <QtGui>
#include <QtPlugin>
#include <iostream>
#include "qfertresultcalculator.h"
#include "jkmathparser.h"

#define LOG_PREFIX QString("qfe_resultstools >>> ").toUpper()

QFEResultsTools::QFEResultsTools(QObject* parent):
    QObject(parent)
{
	logService=NULL;
    dlgCalc=NULL;
}

QFEResultsTools::~QFEResultsTools() {

}


void QFEResultsTools::deinit() {
	/* add code for cleanup here */
}

void QFEResultsTools::projectChanged(QFProject* oldProject, QFProject* project) {
	/* usually cameras do not have to react to a change of the project in QuickFit .. so you don't need to do anything here
	   But: possibly you could read config information from the project here
	 */
}

void QFEResultsTools::initExtension() {
    /* do initializations here but do not yet connect to the camera! */
    

	// some example code that may be used to register a menu and a tool button:
	services->log_global_text(tr("initializing extension '%1' ...\n").arg(getName()));
    
    QAction* actStartResultsCalculator=new QAction(QIcon(getIconFilename()), tr("results &calculator ..."), this);
    connect(actStartResultsCalculator, SIGNAL(triggered()), this, SLOT(startResultsCalculator()));
    QMenu* extm=services->getMenu("tools");
    if (extm) {
        QMenu* menu=extm->addMenu(tr("results tools ..."));
        menu->addAction(actStartResultsCalculator);
    }
    services->log_global_text(tr("initializing extension '%1' ... DONE\n").arg(getName()));

}


void QFEResultsTools::loadSettings(ProgramOptions* settingspo) {
	/* here you could read config information from the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini
	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFEResultsTools::storeSettings(ProgramOptions* settingspo) {
	/* here you could write config information to the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini

	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);


}

void QFEResultsTools::log_text(QString message) {
	if (logService) logService->log_text(LOG_PREFIX+message);
	else if (services) services->log_text(LOG_PREFIX+message);
}

void QFEResultsTools::log_warning(QString message) {
	if (logService) logService->log_warning(LOG_PREFIX+message);
	else if (services) services->log_warning(LOG_PREFIX+message);
}

void QFEResultsTools::log_error(QString message) {
	if (logService) logService->log_error(LOG_PREFIX+message);
    else if (services) services->log_error(LOG_PREFIX+message);
}

void QFEResultsTools::startResultsCalculator()
{
    if (!dlgCalc) dlgCalc=new QFERTResultCalculator(parentWidget);
    dlgCalc->show();
}


Q_EXPORT_PLUGIN2(qfe_resultstools, QFEResultsTools)
