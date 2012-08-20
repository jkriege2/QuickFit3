#include "qfediffusioncoefficientcalculator.h"
#include <QtGui>
#include <QtPlugin>
#include <iostream>
#include "dlgcalcdiffcoeff.h"

#define LOG_PREFIX QString("calc_diffcoeff >>> ").toUpper()

#define K_BOLTZ 1.3806488e-23

QFEDiffusionCoefficientCalculator::QFEDiffusionCoefficientCalculator(QObject* parent):
    QObject(parent)
{
	logService=NULL;
    dlg=NULL;
    actStartPlugin=NULL;
}

QFEDiffusionCoefficientCalculator::~QFEDiffusionCoefficientCalculator() {

}


void QFEDiffusionCoefficientCalculator::deinit() {
	/* add code for cleanup here */
    if (dlg) {
        dlg->close();
        delete dlg;
        dlg=NULL;
    }
}

double QFEDiffusionCoefficientCalculator::getSolutionDensity(int solution, double temperature_K) {
    if (solution==0) {
    }
    return 0;
}

double QFEDiffusionCoefficientCalculator::getSolutionViscosity(int solution, double temperature_K) {
    if (solution==0) { // WATER
        const double A=2.41e-5;
        const double B=247.8;
        const double C=140;

        return A*pow(10.0, B/(temperature_K-C));
    }
    return 0;
}

double QFEDiffusionCoefficientCalculator::getSphereDCoeff(int solution, double diameter, double at_temperature_K)
{
    double eta=getSolutionViscosity(solution, at_temperature_K);
    return K_BOLTZ*at_temperature_K/(6.0*M_PI*eta*diameter/2.0);
}

double QFEDiffusionCoefficientCalculator::getDCoeff_from_D20W(int solution, double D20W, double at_temperature_K) {
    return getDCoeff_from_D(solution, D20W, 1.002e-3, 20.0+273.15, at_temperature_K);
}

double QFEDiffusionCoefficientCalculator::getDCoeff_from_D(int solution, double D, double viscosity, double temp_K, double at_temperature_K) {
    double eta=getSolutionViscosity(solution, at_temperature_K);
    return D*(at_temperature_K/temp_K)*viscosity/eta;
}

QAction *QFEDiffusionCoefficientCalculator::getToolStartAction()
{
    return actStartPlugin;
}

void QFEDiffusionCoefficientCalculator::projectChanged(QFProject* oldProject, QFProject* project) {
	/* usually cameras do not have to react to a change of the project in QuickFit .. so you don't need to do anything here
	   But: possibly you could read config information from the project here
	 */
}

void QFEDiffusionCoefficientCalculator::initExtension() {
    /* do initializations here but do not yet connect to the camera! */
    
    actStartPlugin=new QAction(QIcon(getIconFilename()), tr("Diffusion Coefficient Calculator"), this);
    connect(actStartPlugin, SIGNAL(triggered()), this, SLOT(startPlugin()));
    QMenu* extm=services->getMenu("tools");
    if (extm) {
        extm->addAction(actStartPlugin);
    }

}

void QFEDiffusionCoefficientCalculator::startPlugin() {
    if (!dlg) dlg=new DlgCalcDiffCoeff(this, NULL);
    dlg->show();
    clearReportVals();
}

void QFEDiffusionCoefficientCalculator::loadSettings(ProgramOptions* settingspo) {
	/* here you could read config information from the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini
	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFEDiffusionCoefficientCalculator::storeSettings(ProgramOptions* settingspo) {
	/* here you could write config information to the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini

	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFEDiffusionCoefficientCalculator::log_text(QString message) {
	if (logService) logService->log_text(LOG_PREFIX+message);
	else if (services) services->log_text(LOG_PREFIX+message);
}

void QFEDiffusionCoefficientCalculator::log_warning(QString message) {
	if (logService) logService->log_warning(LOG_PREFIX+message);
	else if (services) services->log_warning(LOG_PREFIX+message);
}

void QFEDiffusionCoefficientCalculator::log_error(QString message) {
	if (logService) logService->log_error(LOG_PREFIX+message);
    else if (services) services->log_error(LOG_PREFIX+message);
}

void QFEDiffusionCoefficientCalculator::startTool()
{
    startPlugin();
}

QVariant QFEDiffusionCoefficientCalculator::getReportingToolValue(const QString &name)
{
    return reportVals.value(name, QVariant());
}

void QFEDiffusionCoefficientCalculator::setReportVal(const QString &name, QVariant val) {
    reportVals[name]=val;
}

void QFEDiffusionCoefficientCalculator::clearReportVals()
{
    reportVals.clear();
}


Q_EXPORT_PLUGIN2(calc_diffcoeff, QFEDiffusionCoefficientCalculator)
