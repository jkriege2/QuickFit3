#include "qfedefaultmathparserextensions.h"
#include "qfedefaultmathparserextensionfunctions.h"
#include <QtGui>
#include <QtPlugin>
#include <iostream>
#include "qfpluginservices.h"
#include "qfmathparser.h"

#define LOG_PREFIX QString("qfe_defaultmathparserextensions >>> ").toUpper()

QFEDefaultMathParserExtensions::QFEDefaultMathParserExtensions(QObject* parent):
    QObject(parent)
{
	logService=NULL;
}

QFEDefaultMathParserExtensions::~QFEDefaultMathParserExtensions() {

}


void QFEDefaultMathParserExtensions::deinit() {
	/* add code for cleanup here */
}

void QFEDefaultMathParserExtensions::projectChanged(QFProject* oldProject, QFProject* project) {
	/* usually cameras do not have to react to a change of the project in QuickFit .. so you don't need to do anything here
	   But: possibly you could read config information from the project here
	 */
}

void QFEDefaultMathParserExtensions::initExtension() {
    /* do initializations here but do not yet connect to the camera! */
    
    // make the additional documentation known to the main help
    QFPluginServices::getInstance()->appendOrAddHTMLReplacement("qfmathparser_ref", QString("$$insert:%1/parserref.inc$$").arg(QFPluginServices::getInstance()->getPluginHelpDirectory(getID())));
    QStringList sl=QFPluginServices::getInstance()->getGlobalConfigValue("QFMathParser_ref").toStringList();
    sl.append(QFPluginServices::getInstance()->getPluginHelpDirectory(getID())+QString("/parserreference/"));
    QFPluginServices::getInstance()->setGlobalConfigValue("QFMathParser_ref", sl);


    QFMathParser::addGlobalFunction("regression", fRegression);
    QFMathParser::addGlobalFunction("weighted_regression", fWeightedRegression);
    QFMathParser::addGlobalFunction("irls", fIRLS);



    QFMathParser::addGlobalFunction("eadd", fErrorAdd);
    QFMathParser::addGlobalFunction("esub", fErrorSub);
    QFMathParser::addGlobalFunction("emul", fErrorMul);
    QFMathParser::addGlobalFunction("ediv", fErrorDiv);
    QFMathParser::addGlobalFunction("epow", fErrorPow);
}


void QFEDefaultMathParserExtensions::loadSettings(ProgramOptions* settingspo) {
	/* here you could read config information from the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini
	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFEDefaultMathParserExtensions::storeSettings(ProgramOptions* settingspo) {
	/* here you could write config information to the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini

	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFEDefaultMathParserExtensions::log_text(QString message) {
	if (logService) logService->log_text(LOG_PREFIX+message);
	else if (services) services->log_text(LOG_PREFIX+message);
}

void QFEDefaultMathParserExtensions::log_warning(QString message) {
	if (logService) logService->log_warning(LOG_PREFIX+message);
	else if (services) services->log_warning(LOG_PREFIX+message);
}

void QFEDefaultMathParserExtensions::log_error(QString message) {
	if (logService) logService->log_error(LOG_PREFIX+message);
	else if (services) services->log_error(LOG_PREFIX+message);
}


Q_EXPORT_PLUGIN2(qfe_defaultmathparserextensions, QFEDefaultMathParserExtensions)
