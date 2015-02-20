/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef QFEDIFFUSIONCOEFFICIENTCALCULATOR_H
#define QFEDIFFUSIONCOEFFICIENTCALCULATOR_H

#include <time.h>
#include <QObject>
#include <cmath>
#include "qfextension.h"
#include "dlgcalcdiffcoeff.h"
#include "qfextensiontool.h"
#include "qfextensionreportingtool.h"
#include "qfpluginoptionsdialog.h"
#include <QList>
#include <QStringList>
/*!
    \defgroup qf3ext_qfe_calc_diffcoeff QFExtension implementation
    \ingroup qf3extensionplugins
*/

/*! \brief QFExtension implementation
    \ingroup qf3ext_qfe_calc_diffcoeff
 */
class QFEDiffusionCoefficientCalculator : public QObject, public QFExtensionBase, public QFExtensionTool, public QFExtensionReportingTool, public QFPluginOptionsDialogInterface {
        Q_OBJECT
        Q_INTERFACES(QFExtension QFExtensionTool QFExtensionReportingTool QFPluginOptionsDialogInterface)
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        Q_PLUGIN_METADATA(IID "www.dkfz.de.QuickFit3.Plugins.QFEDiffusionCoefficientCalculator")
#endif

    public:
        /** Default constructor */
        QFEDiffusionCoefficientCalculator(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFEDiffusionCoefficientCalculator();


    /////////////////////////////////////////////////////////////////////////////
    // QFExtension routines
    /////////////////////////////////////////////////////////////////////////////
        /** \copydoc QFExtension::getID() */
        virtual QString getID() const  { return QString("calc_diffcoeff"); };
        /** \copydoc QFExtension::getName() */
        virtual QString getName() const  { return tr("Diffusion Coefficient Calculator"); };
        /** \copydoc QFExtension::getDescription() */
        virtual QString getDescription() const  { return tr("estimates the diffusion coefficient of a sample under different conditions"); };
        /** \copydoc QFExtension::getAuthor() */
        virtual QString getAuthor() const  { return tr("Jan W. Krieger"); };
        /** \copydoc QFExtension::getCopyright() */
        virtual QString getCopyright() const  { return tr("(c) 2012-2013 by Jan W. Krieger"); };
        /** \copydoc QFExtension::getWeblink() */
        virtual QString getWeblink() const  { return tr("http://www.dkfz.de/Macromol/quickfit/"); };
        /** \copydoc QFExtension::getIconFilename() */
        virtual QString getIconFilename() const  { return QString(":/calc_diffcoeff/qfediffusioncoefficientcalculator.png"); };
        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=2;
        };
        /** \copydoc QFExtension::deinit() */
        virtual void deinit();


        int getComponentCount() const;
        QString getComponentName(int index) const;
        QString getComponentReference(int index) const;
        QString getComponentUnit(int index) const;
        double getComponentMolarMass(int index) const;
        double getComponentCMax(int index) const;
        double evaluateComponentViscosity20degC(int index, double concentration) const;
        QString getViscosityModelFunction(int index, bool HTML=false) const;
        QString getComponentDatafile(int index) const;
        QString getComponentComment(int index, bool html=false) const;
        int getComponentModelID(int index) const;
        QStringList getComponentNames() const;
        QVector<double> getComponentModelParamaters(int index) const;

        struct Component {
            int id;
            double concentration_molar;
        };

        enum SpheroidType {
            Ellipsoid=0,
            Cylinder=1,
            Sphere=2,
            GlobularProtein=3
        };


        /** \brief returns the density of the specified solution im kg/l */
        double getSolutionDensity(int solution, double temperature_K, QList<Component> components=QList<Component>());
        /** \brief returns the viscosity of the specified solution im Pa*s */
        double getSolutionViscosity(int solution, double temperature_K, QList<Component> components=QList<Component>());
        /** \brief returns the diffusion coefficient in m^2/s */
        double getSphereDCoeff(int solution, double diameter_meter, double at_temperature_K, QList<Component> components=QList<Component>(), double viscosity_factor=1.0);
        /** \brief returns the rotational diffusion coefficient in rad^2/s */
        double getSphereDRotCoeff(int solution, double diameter_meter, double at_temperature_K, QList<Component> components=QList<Component>(), double viscosity_factor=1.0);
        /** \brief returns the diffusion coefficient in m^2/s */
        double getDCoeff_from_D20W(int solution, double D20W, double at_temperature_K, QList<Component> components=QList<Component>(), double viscosity_factor=1.0);
        /** \brief returns the diffusion coefficient in m^2/s */
        double getDCoeff_from_D(int solution, double D, double viscosity, double temp_K, double at_temperature_K, QList<Component> components=QList<Component>(), double viscosity_factor=1.0);
        /** \brief returns the diffusion coefficient in m^2/s */
        double getShapeDCoeff(int solution, double rotation_axis_or_length_meter_orMolMassDa, double second_axis_or_diameter_meter, SpheroidType type, double at_temperature_K, QList<Component> components=QList<Component>(), double viscosity_factor=1.0, double* Dsphere=NULL, double* volume=NULL);
        /** \brief returns the hydrodynamic radius in m^2 */
        double getHydrodynRadius_from_D(double D, int solution, double at_temperature_K, QList<Component> components=QList<Component>(), double viscosity_factor=1.0);
        /** \brief returns the hydrodynamic radius in m^2 */
        double getHydrodynRadius_from_DEtaT(double D, double viscosity, double at_temperature_K);

        virtual QAction* getToolStartAction();
        virtual void startTool();
        virtual QVariant getReportingToolValue(const QString& name=QString(""));

        void setReportVal(const QString& name, QVariant val);
        void clearReportVals();

        QString pluginOptionsName() const;
        QIcon pluginOptionsIcon() const;
        QFPluginOptionsWidget *createOptionsWidget(QWidget *parent);

    protected:
        /** \copydoc QFExtensionBase::projectChanged() */
        virtual void projectChanged(QFProject* oldProject, QFProject* project);
        /** \copydoc QFExtensionBase::initExtension() */
        virtual void initExtension();
        /** \copydoc QFExtensionBase::loadSettings() */
        virtual void loadSettings(ProgramOptions* settings);
        /** \copydoc QFExtensionBase::storeSettings() */
        virtual void storeSettings(ProgramOptions* settings);


        void loadComponents(QSettings &settings, bool clear=true);
        void loadComponents();

        /** \brief log project text message
         *  \param message the message to log
         */
        virtual void log_text(QString message);
        /** \brief log project warning message
         *  \param message the warning message to log
         */
        virtual void log_warning(QString message);
        /** \brief log project error message
         *  \param message the error message to log
         */
        virtual void log_error(QString message);

	protected:
        QFPluginLogService* logService;
        DlgCalcDiffCoeff* dlg;
        QAction* actStartPlugin;
        QMap<QString, QVariant> reportVals;

        struct ComponentData {
            QString unit;
            QString name;
            QString reference;
            double molar_mass;
            QString datafile;
            QString comment;
            QString comment_html;
            int model;
            double c_max;
            QVector<double> parameters;
        };
        QList<ComponentData> components;
		
	protected slots:
	    /** \brief target, used in example code in initExtension() */
        void startPlugin();

};

#endif // QFEDIFFUSIONCOEFFICIENTCALCULATOR_H
