#include "qfe_dataexportbasics.h"
#include <QtGui>
#include <QtPlugin>
#include <iostream>
#include "datatools.h"
#include "qfedataxlslibexport.h"

#define LOG_PREFIX QString("qfe_dataexportbasics >>> ").toUpper()

QFEDataExportBasics::QFEDataExportBasics(QObject* parent):
    QObject(parent)
{
	logService=NULL;
}

QFEDataExportBasics::~QFEDataExportBasics() {

}


void QFEDataExportBasics::deinit() {
	/* add code for cleanup here */
}

void QFEDataExportBasics::projectChanged(QFProject* oldProject, QFProject* project) {
	/* usually cameras do not have to react to a change of the project in QuickFit .. so you don't need to do anything here
	   But: possibly you could read config information from the project here
	 */
}

void QFEDataExportBasics::initExtension() {
    /* do initializations here but do not yet connect to the camera! */
    
    QFDataExportHandler::registerDataWriter(new QFEDataXlsLibExport(true));
    QFDataExportHandler::registerDataWriter(new QFEDataXlsLibExport(false));
}



void QFEDataExportBasics::loadSettings(ProgramOptions* settingspo) {
	/* here you could read config information from the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini
	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFEDataExportBasics::storeSettings(ProgramOptions* settingspo) {
	/* here you could write config information to the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini

	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFEDataExportBasics::log_text(QString message) {
	if (logService) logService->log_text(LOG_PREFIX+message);
	else if (services) services->log_text(LOG_PREFIX+message);
}

void QFEDataExportBasics::log_warning(QString message) {
	if (logService) logService->log_warning(LOG_PREFIX+message);
	else if (services) services->log_warning(LOG_PREFIX+message);
}

void QFEDataExportBasics::log_error(QString message) {
	if (logService) logService->log_error(LOG_PREFIX+message);
	else if (services) services->log_error(LOG_PREFIX+message);
}


Q_EXPORT_PLUGIN2(qfe_dataexportbasics, QFEDataExportBasics)
