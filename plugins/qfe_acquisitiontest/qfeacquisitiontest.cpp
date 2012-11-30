#include "qfeacquisitiontest.h"
#include <QtGui>
#include <QtPlugin>
#include <iostream>

#define LOG_PREFIX QString("qfe_acquisitiontest >>> ").toUpper()

QFEAcquisitionTest::QFEAcquisitionTest(QObject* parent):
    QObject(parent)
{
	logService=NULL;
    dialog=NULL;
}

QFEAcquisitionTest::~QFEAcquisitionTest() {
    if (dialog) delete dialog;
}


void QFEAcquisitionTest::deinit() {
	/* add code for cleanup here */
}

void QFEAcquisitionTest::projectChanged(QFProject* oldProject, QFProject* project) {
	/* usually cameras do not have to react to a change of the project in QuickFit .. so you don't need to do anything here
	   But: possibly you could read config information from the project here
	 */
}

void QFEAcquisitionTest::initExtension() {
    /* do initializations here but do not yet connect to the camera! */
    
	// some example code that may be used to register a menu and a tool button:
    
    QAction* actStartPlugin=new QAction(QIcon(getIconFilename()), tr("Test Acquisitions"), this);
    connect(actStartPlugin, SIGNAL(triggered()), this, SLOT(startPlugin()));
    QToolBar* exttb=services->getToolbar("extensions");
    if (exttb) {
        exttb->addAction(actStartPlugin);
    }
    /*QMenu* extm=services->getMenu("extensions");
    if (extm) {
        extm->addAction(actStartPlugin);
    }*/
}

void QFEAcquisitionTest::startPlugin() {
    if (!dialog) {
        dialog=new QFEAcquisitiontestDialog(NULL);
    }
    if (dialog) {
        dialog->show();
    }
}

void QFEAcquisitionTest::loadSettings(ProgramOptions* settingspo) {
	/* here you could read config information from the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini
	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFEAcquisitionTest::storeSettings(ProgramOptions* settingspo) {
	/* here you could write config information to the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini

	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFEAcquisitionTest::log_text(QString message) {
	if (logService) logService->log_text(LOG_PREFIX+message);
	else if (services) services->log_text(LOG_PREFIX+message);
}

void QFEAcquisitionTest::log_warning(QString message) {
	if (logService) logService->log_warning(LOG_PREFIX+message);
	else if (services) services->log_warning(LOG_PREFIX+message);
}

void QFEAcquisitionTest::log_error(QString message) {
	if (logService) logService->log_error(LOG_PREFIX+message);
	else if (services) services->log_error(LOG_PREFIX+message);
}


Q_EXPORT_PLUGIN2(qfe_acquisitiontest, QFEAcquisitionTest)
