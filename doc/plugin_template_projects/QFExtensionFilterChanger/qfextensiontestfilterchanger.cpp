#include "qfextensiontestfilterchanger.h"
#include <QtGui>
#include <QtPlugin>
#include <iostream>

#define LOG_PREFIX QString("filterc_test >>> ").toUpper()

QFExtensionTestFilterChanger::QFExtensionTestFilterChanger(QObject* parent):
    QObject(parent)
{
	logService=NULL;
}

QFExtensionTestFilterChanger::~QFExtensionTestFilterChanger() {

}


void QFExtensionTestFilterChanger::projectChanged(QFProject* oldProject, QFProject* project) {
	/* usually cameras do not have to react to a change of the project in QuickFit .. so you don't need to do anything here
	   But: possibly you could read config information from the project here
	 */
}

void QFExtensionTestFilterChanger::initExtension() {
    /* do initializations here but do not yet connect to the camera! */
    

}


void QFExtensionTestFilterChanger::loadSettings(ProgramOptions* settingspo) {
	/* here you could read config information from the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini
	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFExtensionTestFilterChanger::storeSettings(ProgramOptions* settingspo) {
	/* here you could write config information to the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini

	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

	}
}

void QFExtensionTestFilterChanger::log_text(QString message) {
	if (logService) logService->log_text(LOG_PREFIX+message);
	else if (services) services->log_text(LOG_PREFIX+message);
}

void QFExtensionTestFilterChanger::log_warning(QString message) {
	if (logService) logService->log_warning(LOG_PREFIX+message);
	else if (services) services->log_warning(LOG_PREFIX+message);
}

void QFExtensionTestFilterChanger::log_error(QString message) {
	if (logService) logService->log_error(LOG_PREFIX+message);
    else if (services) services->log_error(LOG_PREFIX+message);
}






void QFExtensionTestFilterChanger::deinit() {
    /* add code for cleanup here */
}

unsigned int QFExtensionTestFilterChanger::getFilterChangerCount() {
}

void QFExtensionTestFilterChanger::filterChangerConnect(unsigned int filterChanger) {
}

void QFExtensionTestFilterChanger::filterChangerDisonnect(unsigned int filterChanger) {
}

void QFExtensionTestFilterChanger::setFilterChangerLogging(QFPluginLogService *logService) {
    this->logService=logService;
}


bool QFExtensionTestFilterChanger::isFilterChangerConnected(unsigned int filterChanger) {
}

unsigned int QFExtensionTestFilterChanger::getFilterChangerFilterCount(unsigned int filterChanger) {
}

void QFExtensionTestFilterChanger::setFilterChangerFilter(unsigned int filterChanger, unsigned int filter) {
}

unsigned int QFExtensionTestFilterChanger::getFilterChangerCurrentFilter(unsigned int filterChanger) {
}

bool QFExtensionTestFilterChanger::isLastFilterChangerActionFinished(unsigned int filterChanger) {
}

QString QFExtensionTestFilterChanger::getFilterChangerDescription(unsigned int filterChanger) {
}

QString QFExtensionTestFilterChanger::getFilterChangerShortName(unsigned int filterChanger) {
}

void QFExtensionTestFilterChanger::showFilterChangerSettingsDialog(unsigned int filterChanger, QWidget *parent) {
}



Q_EXPORT_PLUGIN2(filterc_test, QFExtensionTestFilterChanger)
