#include "qfediffusioncoefficientcalculator.h"
#include <QtGui>
#include <QtPlugin>
#include <iostream>
#include "dlgcalcdiffcoeff.h"
#include "optionswidget.h"
#include "qfmathtools.h"

#define LOG_PREFIX QString("calc_diffcoeff >>> ").toUpper()

#define K_BOLTZ QF_K_BOLTZ

QFEDiffusionCoefficientCalculator::QFEDiffusionCoefficientCalculator(QObject* parent):
    QObject(parent)
{
	logService=NULL;
    dlg=NULL;
    actStartPlugin=NULL;
     QFPluginServices::getInstance()->registerSettingsPane(this);
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

int QFEDiffusionCoefficientCalculator::getComponentCount() const
{
    return components.size();
}

QString QFEDiffusionCoefficientCalculator::getComponentName(int index) const {
    if (index>=0 && index<components.size()) return components[index].name;
    return "";
}
QStringList QFEDiffusionCoefficientCalculator::getComponentNames() const {
    QStringList result;
    for (int i=0; i<components.size(); i++)
        result.append(components[i].name);
    return result;
}

QString QFEDiffusionCoefficientCalculator::getComponentReference(int index) const {
    if (index>=0 && index<components.size()) return components[index].reference;
    return "";
}

QString QFEDiffusionCoefficientCalculator::getComponentUnit(int index) const
{
    if (index>=0 && index<components.size()) return components[index].unit;
    return "M";
}

QVector<double> QFEDiffusionCoefficientCalculator::getComponentModelParamaters(int index) const {
    if (index>=0 && index<components.size()) return components[index].parameters;
    return QVector<double>();
}

QString QFEDiffusionCoefficientCalculator::getComponentDatafile(int index) const {
    if (index>=0 && index<components.size()) return components[index].datafile;
    return "";
}

QString QFEDiffusionCoefficientCalculator::getComponentComment(int index, bool html) const {
    if (!html && index>=0 && index<components.size()) return components[index].comment;
    if (html && index>=0 && index<components.size()) return components[index].comment_html;
    return "";
}

int QFEDiffusionCoefficientCalculator::getComponentModelID(int index) const {
    if (index>=0 && index<components.size()) return components[index].model;
    return -1;
}

double QFEDiffusionCoefficientCalculator::getComponentMolarMass(int index) const {
    if (index>=0 && index<components.size()) return components[index].molar_mass;
    return 0.0;
}

double QFEDiffusionCoefficientCalculator::getComponentCMax(int index) const {
    if (index>=0 && index<components.size()) return components[index].c_max;
    return 0.0;
}

double QFEDiffusionCoefficientCalculator::evaluateComponentViscosity20degC(int index, double concentration) const {
    if (index>=0 && index<components.size()) {
        switch (components[index].model) {
            case 1: return 1e-3*(components[index].parameters.value(0,0.0)+
                        components[index].parameters.value(1,0.0)*1e-3*sqrt(concentration)+
                        components[index].parameters.value(2,0.0)*1e-2*concentration+
                        components[index].parameters.value(3,0.0)*1e-3*concentration*concentration+
                        components[index].parameters.value(4,0.0)*1e-3*concentration*concentration*concentration); break;
            case 2: return 1e-3*(components[index].parameters.value(0,0.0)+
                        components[index].parameters.value(1,0.0)*1e-3*sqrt(concentration)+
                        components[index].parameters.value(2,0.0)*1e-2*concentration+
                        components[index].parameters.value(3,0.0)*1e-3*concentration*concentration+
                        components[index].parameters.value(4,0.0)*1e-3*concentration*concentration*concentration+
                        components[index].parameters.value(5,0.0)*1e-3*concentration*concentration*concentration*concentration+
                        components[index].parameters.value(6,0.0)*1e-3*concentration*concentration*concentration*concentration*concentration+
                        components[index].parameters.value(7,0.0)*1e-3*concentration*concentration*concentration*concentration*concentration*concentration+
                        components[index].parameters.value(8,0.0)*1e-3*concentration*concentration*concentration*concentration*concentration*concentration*concentration); break;
            default: return 0.0;
        }
    }
    return 0.0;
}

QString QFEDiffusionCoefficientCalculator::getViscosityModelFunction(int index, bool HTML) const {
    if (index>=0 && index<components.size()) {
        if (HTML) {
            switch (components[index].model) {
                case 1: return QString("m<sub>1</sub>(c) = %1 + %2&middot;10<sup>-3</sup>&middot;sqrt(c) + %3&middot;10<sup>-2</sup>&middot;c + %4&middot;10<sup>-3</sup>&middot;c<sup>2</sup> + %5&middot;10<sup>-3</sup>&middot;c<sup>3</sup>")
                            .arg(components[index].parameters.value(0,0.0))
                            .arg(components[index].parameters.value(1,0.0))
                            .arg(components[index].parameters.value(2,0.0))
                            .arg(components[index].parameters.value(3,0.0))
                            .arg(components[index].parameters.value(4,0.0)); break;
                case 2: return QString("m<sub>2</sub>(c) = %1 + %2&middot;10<sup>-3</sup>&middot;sqrt(c) + %3&middot;10<sup>-2</sup>&middot;c + %4&middot;10<sup>-3</sup>&middot;c<sup>2</sup> + %5&middot;10<sup>-3</sup>&middot;c<sup>3</sup>+ %6&middot;10<sup>-3</sup>&middot;c<sup>4</sup>+ %7&middot;10<sup>-5</sup>&middot;c<sup>3</sup>+ %8&middot;10<sup>-6</sup>&middot;c<sup>3</sup>+ %9&middot;10<sup>-7</sup>&middot;c<sup>7</sup>")
                            .arg(components[index].parameters.value(0,0.0))
                            .arg(components[index].parameters.value(1,0.0))
                            .arg(components[index].parameters.value(2,0.0))
                            .arg(components[index].parameters.value(3,0.0))
                            .arg(components[index].parameters.value(4,0.0))
                            .arg(components[index].parameters.value(5,0.0))
                            .arg(components[index].parameters.value(6,0.0))
                            .arg(components[index].parameters.value(7,0.0))
                            .arg(components[index].parameters.value(8,0.0)); break;
                default: return QString("???");
            }
        } else {
            switch (components[index].model) {
                case 1: return QString("m1(c) = %1 + %2*1e-3*sqrt(c) + %3*1e-2*c + %4*1e-3*c^2 + %5*1e-3*c^3")
                            .arg(components[index].parameters.value(0,0.0))
                            .arg(components[index].parameters.value(1,0.0))
                            .arg(components[index].parameters.value(2,0.0))
                            .arg(components[index].parameters.value(3,0.0))
                            .arg(components[index].parameters.value(4,0.0)); break;
                case 2: return QString("m1(c) = %1 + %2*1e-3*sqrt(c) + %3*1e-2*c + %4*1e-3*c^2 + %5*1e-3*c^3 + %6*1e-3*c^4 + %7*1e-3*c^5 + %8*1e-3*c^6 + %9*1e-3*c^7")
                            .arg(components[index].parameters.value(0,0.0))
                            .arg(components[index].parameters.value(1,0.0))
                            .arg(components[index].parameters.value(2,0.0))
                            .arg(components[index].parameters.value(3,0.0))
                            .arg(components[index].parameters.value(4,0.0))
                            .arg(components[index].parameters.value(5,0.0))
                            .arg(components[index].parameters.value(6,0.0))
                            .arg(components[index].parameters.value(7,0.0))
                            .arg(components[index].parameters.value(8,0.0)); break;
                default: return QString("???");
            }
        }
    }
    return QString("???");
}

double QFEDiffusionCoefficientCalculator::getSolutionDensity(int solution, double temperature_K, QList<QFEDiffusionCoefficientCalculator::Component> components) {
    if (solution==0) {
    }
    return 0;
}

double QFEDiffusionCoefficientCalculator::getSolutionViscosity(int solution, double temperature_K, QList<QFEDiffusionCoefficientCalculator::Component> components) {
    double eta=0;
    if (components.size()==0) {
        if (solution==0) { // WATER
            const double A=2.41e-5;
            const double B=247.8;
            const double C=140;

            eta= A*pow(10.0, B/(temperature_K-C));
        }
    } else if (components.size()>0) {
        if (solution==0) {
            double etaTW=getSolutionViscosity(0, temperature_K);
            double eta20W=getSolutionViscosity(0, 293.15);
            eta=0;
            for (int c=0; c<components.size(); c++) {
                if (components[c].concentration_molar>0) eta=eta+(evaluateComponentViscosity20degC(components[c].id, components[c].concentration_molar)-eta20W);
            }
            //qDebug()<<"T="<<temperature_K<<"   eta20W="<<eta20W<<"   etaTW="<<etaTW<<"   deta="<<eta;
            eta=(eta+eta20W)*etaTW/eta20W;
            //qDebug()<<"  => eta="<<eta;
        }
    }
    return eta;
}


double QFEDiffusionCoefficientCalculator::getDCoeff_from_D20W(int solution, double D20W, double at_temperature_K, QList<QFEDiffusionCoefficientCalculator::Component> components, double viscosity_factor) {
    return getDCoeff_from_D(solution, D20W, 1.002e-3, 20.0+273.15, at_temperature_K, components, viscosity_factor);
}

double QFEDiffusionCoefficientCalculator::getDCoeff_from_D(int solution, double D, double viscosity, double temp_K, double at_temperature_K, QList<QFEDiffusionCoefficientCalculator::Component> components, double viscosity_factor) {
    double eta=getSolutionViscosity(solution, at_temperature_K, components)*viscosity_factor;
    return D*(at_temperature_K/temp_K)*viscosity/eta;
}

double QFEDiffusionCoefficientCalculator::getSphereDCoeff(int solution, double diameter_meter, double at_temperature_K, QList<QFEDiffusionCoefficientCalculator::Component> components, double viscosity_factor) {
    double eta=getSolutionViscosity(solution, at_temperature_K, components)*viscosity_factor;
    return K_BOLTZ*at_temperature_K/(6.0*M_PI*eta*diameter_meter/2.0);
}

double QFEDiffusionCoefficientCalculator::getShapeDCoeff(int solution, double rotation_axis_or_length_meter, double second_axis_or_diameter_meter, QFEDiffusionCoefficientCalculator::SpheroidType type, double at_temperature_K, QList<QFEDiffusionCoefficientCalculator::Component> components, double viscosity_factor, double* Dsphere, double* volume) {
    double eta=getSolutionViscosity(solution, at_temperature_K, components)*viscosity_factor;

    double Re=0;
    double Ft=1;
    double p=rotation_axis_or_length_meter/second_axis_or_diameter_meter;
    double q=1.0/p;
    if (type==QFEDiffusionCoefficientCalculator::Ellipsoid) {
        Re=pow(second_axis_or_diameter_meter/2.0*second_axis_or_diameter_meter/2.0*rotation_axis_or_length_meter/2.0, 1.0/3.0);
        if (p<1) { // oblate, p<1 => q>1
            Ft=sqrt(q*q-1.0)/(pow(q, 2.0/3.0)*atan(sqrt(q*q-1.0)));
        } else if (p>1) { // prolate, p>1 => q<1
            Ft=sqrt(1.0-q*q)/(pow(q, 2.0/3.0)*log((1.0+sqrt(1.0-q*q))/q));
        } else if (p==1) { // sphere
            if (volume) *volume=4.0/3.0*M_PI*rotation_axis_or_length_meter*rotation_axis_or_length_meter*rotation_axis_or_length_meter/8.0;
            double DDD=getSphereDCoeff(solution, rotation_axis_or_length_meter, at_temperature_K, components);
            if (Dsphere) *Dsphere=DDD;
            return DDD;
        }
    } else if (type==QFEDiffusionCoefficientCalculator::Cylinder){
        double gamma=0.312+0.565/p+0.100/p/p;
        Re=pow(3.0/(2.0*p*p), 1.0/3.0)*rotation_axis_or_length_meter/2.0;
        Ft=pow(2.0*p*p/3.0, 1.0/3.0)/(log(p)+gamma);
    } else if (type==QFEDiffusionCoefficientCalculator::Sphere){
        Ft=1;
        Re=rotation_axis_or_length_meter/2.0;
    }
    if (volume) *volume=4.0/3.0*M_PI*Re*Re*Re;

    double fperrin=6.0*M_PI*eta*Re*Ft;

    if (Dsphere) *Dsphere=K_BOLTZ*at_temperature_K/(6.0*M_PI*eta*Re);

    return K_BOLTZ*at_temperature_K/fperrin;
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
    
    loadComponents();

    actStartPlugin=new QAction(QIcon(getIconFilename()), tr("Diffusion Coefficient Calculator"), this);
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
    // QSettings settings(services->getConfigFileDirectory()+"/plugins/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFEDiffusionCoefficientCalculator::storeSettings(ProgramOptions* settingspo) {
	/* here you could write config information to the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini

	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
    // QSettings settings(services->getConfigFileDirectory()+"/plugins/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFEDiffusionCoefficientCalculator::loadComponents() {
    QSettings set1(services->getAssetsDirectory()+"/plugins/"+getID()+"/solutioncomponents.ini", QSettings::IniFormat);
    QSettings set2(services->getConfigFileDirectory()+"/plugins/"+getID()+"/solutioncomponents.ini", QSettings::IniFormat);
    loadComponents(set1, true);
    loadComponents(set2, false);
}

void QFEDiffusionCoefficientCalculator::loadComponents(QSettings& settings, bool clear) {
    if (clear) components.clear();
    QStringList groups=settings.childGroups();
    //qDebug()<<services->getAssetsDirectory()+"/plugins/"+getID()+"/solutioncomponents.ini"<<groups;
    for (int g=0; g<groups.size(); g++) {
        settings.beginGroup(groups[g]);
        QFEDiffusionCoefficientCalculator::ComponentData cdata;
        cdata.name=settings.value("name", tr("component '%1'").arg(groups[g])).toString();
        cdata.reference=settings.value("reference", tr("---")).toString();
        QDir d=QFileInfo(settings.fileName()).absoluteDir();
        cdata.datafile=d.absoluteFilePath(settings.value("datafile", "").toString());
        cdata.model=settings.value("model", 1).toInt();
        cdata.molar_mass=settings.value("molar_mass", 0).toDouble();
        cdata.c_max=settings.value("c_max", 0).toDouble();
        cdata.comment=settings.value("comment", "").toString();
        cdata.comment_html=settings.value("comment_html", "").toString();
        cdata.unit=settings.value("unit", "M").toString();
        QStringList keys=settings.childKeys();
        QString key="p1";
        int id=1;
        cdata.parameters.clear();
        while (keys.contains(key)) {
            cdata.parameters.append(settings.value(key, 0.0).toDouble());
            id++;
            key=QString("p%1").arg(id);
        }
        components.append(cdata);
        settings.endGroup();
    }
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

QString QFEDiffusionCoefficientCalculator::pluginOptionsName() const
{
    return getName();
}

QIcon QFEDiffusionCoefficientCalculator::pluginOptionsIcon() const
{
    return QIcon(getIconFilename());
}

QFPluginOptionsWidget *QFEDiffusionCoefficientCalculator::createOptionsWidget(QWidget *parent)
{
    return new OptionsWidget(this, parent);
}


Q_EXPORT_PLUGIN2(calc_diffcoeff, QFEDiffusionCoefficientCalculator)
