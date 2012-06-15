#ifndef QFEDIFFUSIONCOEFFICIENTCALCULATOR_H
#define QFEDIFFUSIONCOEFFICIENTCALCULATOR_H

#include <time.h>
#include <QObject>
#include "qfextension.h"
#include "dlgcalcdiffcoeff.h"

/*!
    \defgroup qf3ext_qfe_calc_diffcoeff QFExtension implementation
    \ingroup qf3extensionplugins
*/

/*! \brief QFExtension implementation
    \ingroup qf3ext_qfe_calc_diffcoeff
 */
class QFEDiffusionCoefficientCalculator : public QObject, public QFExtensionBase {
        Q_OBJECT
        Q_INTERFACES(QFExtension)
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
        virtual QString getCopyright() const  { return tr("(c) 2012 by Jan W. Krieger"); };
        /** \copydoc QFExtension::getWeblink() */
        virtual QString getWeblink() const  { return tr("http://www.dkfz.de/Macromol/quickfit/"); };
        /** \copydoc QFExtension::getIconFilename() */
        virtual QString getIconFilename() const  { return QString(":/calc_diffcoeff/qfediffusioncoefficientcalculator.png"); };
        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=0;
        };
        /** \copydoc QFExtension::deinit() */
        virtual void deinit();

        double getSolutionDensity(int solution, double temperature_K);
        double getSolutionViscosity(int solution, double temperature_K);
        double getSphereDCoeff(int solution, double diameter, double at_temperature_K);
        double getDCoeff_from_D20W(int solution, double D20W, double at_temperature_K);
        double getDCoeff_from_D(int solution, double D, double viscosity, double temp_K, double at_temperature_K);

    protected:
        /** \copydoc QFExtensionBase::projectChanged() */
        virtual void projectChanged(QFProject* oldProject, QFProject* project);
        /** \copydoc QFExtensionBase::initExtension() */
        virtual void initExtension();
        /** \copydoc QFExtensionBase::loadSettings() */
        virtual void loadSettings(ProgramOptions* settings);
        /** \copydoc QFExtensionBase::storeSettings() */
        virtual void storeSettings(ProgramOptions* settings);



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
		
	protected slots:
	    /** \brief target, used in example code in initExtension() */
        void startPlugin();

};

#endif // QFEDIFFUSIONCOEFFICIENTCALCULATOR_H
