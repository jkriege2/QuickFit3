#include "qfe_gslmathparserextensions.h"
#include "qfe_gslmathparserextensionsfunctions.h"
#include <QtGui>
#include <QtPlugin>
#include <iostream>
#include "qfpluginservices.h"
#include "qfmathparser.h"


#define LOG_PREFIX QString("qfe_gslmathparserextensions >>> ").toUpper()

QFEGSLMathParserExtensions::QFEGSLMathParserExtensions(QObject* parent):
    QObject(parent)
{
	logService=NULL;
}

QFEGSLMathParserExtensions::~QFEGSLMathParserExtensions() {

}


void QFEGSLMathParserExtensions::deinit() {
	/* add code for cleanup here */
}

void QFEGSLMathParserExtensions::projectChanged(QFProject* oldProject, QFProject* project) {
	/* usually cameras do not have to react to a change of the project in QuickFit .. so you don't need to do anything here
	   But: possibly you could read config information from the project here
	 */
}

void QFEGSLMathParserExtensions::initExtension() {
    /* do initializations here but do not yet connect to the camera! */
    
    // make the additional documentation known to the main help
    QFPluginServices::getInstance()->appendOrAddHTMLReplacement("qfmathparser_ref", QString("$$insert:%1/parserref.inc$$").arg(QFPluginServices::getInstance()->getPluginHelpDirectory(getID())));
    QStringList sl=QFPluginServices::getInstance()->getGlobalConfigValue("QFMathParser_ref").toStringList();
    sl.append(QFPluginServices::getInstance()->getPluginHelpDirectory(getID())+QString("/parserreference/"));
    QFPluginServices::getInstance()->setGlobalConfigValue("QFMathParser_ref", sl);

    QFMathParser::addGlobalFunction("hydrogenicR1",fmp_gsl_sf_hydrogenicR_1);
    QFMathParser::addGlobalFunction("hydrogenicR", fmp_gsl_sf_hydrogenicR);
    QFMathParser::addGlobalFunction("debye1", fmp_gsl_sf_debye_1);
    QFMathParser::addGlobalFunction("debye2", fmp_gsl_sf_debye_2);
    QFMathParser::addGlobalFunction("debye3", fmp_gsl_sf_debye_3);
    QFMathParser::addGlobalFunction("debye4", fmp_gsl_sf_debye_4);
    QFMathParser::addGlobalFunction("debye5", fmp_gsl_sf_debye_5);
    QFMathParser::addGlobalFunction("debye6", fmp_gsl_sf_debye_6);
    QFMathParser::addGlobalFunction("dilog", fmp_gsl_sf_dilog);
    QFMathParser::addGlobalFunction("fact", fmp_gsl_sf_fact);
    QFMathParser::addGlobalFunction("doublefact", fmp_gsl_sf_doublefact);
    QFMathParser::addGlobalFunction("lnfact", fmp_gsl_sf_lnfact );
    QFMathParser::addGlobalFunction("lndoublefact", fmp_gsl_sf_lndoublefact );
    QFMathParser::addGlobalFunction("taylorcoeff", fmp_gsl_sf_taylorcoeff  );
    QFMathParser::addGlobalFunction("choose", fmp_gsl_sf_choose  );
    QFMathParser::addGlobalFunction("lnchoose", fmp_gsl_sf_lnchoose  );
    QFMathParser::addGlobalFunction("beta", fmp_gsl_sf_beta  );
    QFMathParser::addGlobalFunction("lnbeta", fmp_gsl_sf_lnbeta  );
    QFMathParser::addGlobalFunction("lngamma", fmp_gsl_sf_lngamma  );
    QFMathParser::addGlobalFunction("gamma", fmp_gsl_sf_gamma  );

    QFMathParser::addGlobalFunction("laguerre1", fmp_gsl_sf_laguerre_1  );
    QFMathParser::addGlobalFunction("laguerre2", fmp_gsl_sf_laguerre_2  );
    QFMathParser::addGlobalFunction("laguerre3", fmp_gsl_sf_laguerre_3  );
    QFMathParser::addGlobalFunction("laguerre", fmp_gsl_sf_laguerre_n  );

    QFMathParser::addGlobalFunction("angle_restrict_symm", fmp_gsl_sf_angle_restrict_symm   );
    QFMathParser::addGlobalFunction("angle_restrict_pos", fmp_gsl_sf_angle_restrict_pos   );
}


void QFEGSLMathParserExtensions::loadSettings(ProgramOptions* settingspo) {
	/* here you could read config information from the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini
	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFEGSLMathParserExtensions::storeSettings(ProgramOptions* settingspo) {
	/* here you could write config information to the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini

	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFEGSLMathParserExtensions::log_text(QString message) {
	if (logService) logService->log_text(LOG_PREFIX+message);
	else if (services) services->log_text(LOG_PREFIX+message);
}

void QFEGSLMathParserExtensions::log_warning(QString message) {
	if (logService) logService->log_warning(LOG_PREFIX+message);
	else if (services) services->log_warning(LOG_PREFIX+message);
}

void QFEGSLMathParserExtensions::log_error(QString message) {
	if (logService) logService->log_error(LOG_PREFIX+message);
	else if (services) services->log_error(LOG_PREFIX+message);
}


Q_EXPORT_PLUGIN2(qfe_defaultmathparserextensions, QFEGSLMathParserExtensions)
