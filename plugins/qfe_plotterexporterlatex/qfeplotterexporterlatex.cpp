#include "qfeplotterexporterlatex.h"
#include <QtGui>
#include <QtPlugin>
#include <iostream>
#include "jkqtplatexengineadapter.h"
#ifdef HAS_EMF_ENGINE
#  include "jkqtpemfengineadapter.h"
#endif
#include "jkqtpbaseplotter.h"

#define LOG_PREFIX QString("qfe_plotterexporterlatex >>> ").toUpper()

QFEPlotterExporterLatex::QFEPlotterExporterLatex(QObject* parent):
    QObject(parent)
{
	logService=NULL;
}

QFEPlotterExporterLatex::~QFEPlotterExporterLatex() {

}


void QFEPlotterExporterLatex::deinit() {
	/* add code for cleanup here */
}

void QFEPlotterExporterLatex::projectChanged(QFProject* oldProject, QFProject* project) {
	/* usually cameras do not have to react to a change of the project in QuickFit .. so you don't need to do anything here
	   But: possibly you could read config information from the project here
	 */
}

void QFEPlotterExporterLatex::initExtension() {
    /* do initializations here but do not yet connect to the camera! */
    
    JKQtBasePlotter::registerPaintDeviceAdapter(new JKQTPLatexEngineAdapter(true, true, QTeXPaintDevice::Tikz));
    JKQtBasePlotter::registerPaintDeviceAdapter(new JKQTPLatexEngineAdapter(true, false, QTeXPaintDevice::Tikz));
    JKQtBasePlotter::registerPaintDeviceAdapter(new JKQTPLatexEngineAdapter(false, true, QTeXPaintDevice::Tikz));
    JKQtBasePlotter::registerPaintDeviceAdapter(new JKQTPLatexEngineAdapter(false, false, QTeXPaintDevice::Tikz));
    JKQtBasePlotter::registerPaintDeviceAdapter(new JKQTPLatexEngineAdapter(false, true, QTeXPaintDevice::Tikz, false));
    JKQtBasePlotter::registerPaintDeviceAdapter(new JKQTPLatexEngineAdapter(false, false, QTeXPaintDevice::Tikz, false));

    JKQtBasePlotter::registerPaintDeviceAdapter(new JKQTPLatexEngineAdapter(true, true, QTeXPaintDevice::Pgf));
    JKQtBasePlotter::registerPaintDeviceAdapter(new JKQTPLatexEngineAdapter(true, false, QTeXPaintDevice::Pgf));
    JKQtBasePlotter::registerPaintDeviceAdapter(new JKQTPLatexEngineAdapter(false, true, QTeXPaintDevice::Pgf));
    JKQtBasePlotter::registerPaintDeviceAdapter(new JKQTPLatexEngineAdapter(false, false, QTeXPaintDevice::Pgf));
    JKQtBasePlotter::registerPaintDeviceAdapter(new JKQTPLatexEngineAdapter(false, true, QTeXPaintDevice::Pgf, false));
    JKQtBasePlotter::registerPaintDeviceAdapter(new JKQTPLatexEngineAdapter(false, false, QTeXPaintDevice::Pgf, false));
#ifdef HAS_EMF_ENGINE
    JKQtBasePlotter::registerPaintDeviceAdapter(new JKQTPEMFEngineAdapter());
#endif
}


void QFEPlotterExporterLatex::loadSettings(ProgramOptions* settingspo) {
	/* here you could read config information from the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini
	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFEPlotterExporterLatex::storeSettings(ProgramOptions* settingspo) {
	/* here you could write config information to the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini

	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFEPlotterExporterLatex::log_text(QString message) {
	if (logService) logService->log_text(LOG_PREFIX+message);
	else if (services) services->log_text(LOG_PREFIX+message);
}

void QFEPlotterExporterLatex::log_warning(QString message) {
	if (logService) logService->log_warning(LOG_PREFIX+message);
	else if (services) services->log_warning(LOG_PREFIX+message);
}

void QFEPlotterExporterLatex::log_error(QString message) {
	if (logService) logService->log_error(LOG_PREFIX+message);
	else if (services) services->log_error(LOG_PREFIX+message);
}


Q_EXPORT_PLUGIN2(qfe_plotterexporterlatex, QFEPlotterExporterLatex)
