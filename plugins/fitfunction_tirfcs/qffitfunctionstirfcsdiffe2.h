#ifndef QFFitFunctionsTIRFCSDiffE2NewVeffE2_H
#define QFFitFunctionsTIRFCSDiffE2NewVeffE2_H
#include "qfpluginfitfunction.h"



/*! \brief QFFitFunction class for a TIR-FCS fit model with pure diffusion and lateral 1/e^2 width
    \ingroup qf3fitfunp_fitfunctions_tirfcs

*/
class QFFitFunctionsTIRFCSDiffE2: public QFFitFunction {
    public:
        QFFitFunctionsTIRFCSDiffE2();
        virtual ~QFFitFunctionsTIRFCSDiffE2() {}
        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const { return QString("TIR-FCS: 2D Diffusion (1/e^2 radii)"); }
        /** \copydoc QFFitFunction::shortName() */
        virtual QString shortName() const { return QObject::tr("TIR-FCS: 2D Diffusion (1/e^2 radii)"); }
        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const { return QString("fcs_tir_diffe2"); }

        /*! \copydoc QFFitFunction::evaluate()   */
        virtual double evaluate(double t, const double* parameters) const;

        /*! \copydoc QFFitFunction::calcParameter()   */
        virtual void calcParameter(double* parameterValues, double* error=NULL) const;

        /*! \copydoc QFFitFunction::isParameterVisible()   */
        virtual bool isParameterVisible(int parameter, const double* parameterValues) const;
        /*! \copydoc QFFitFunction::getAdditionalPlotCount()   */
        virtual unsigned int getAdditionalPlotCount(const double* params);

        /*! \copydoc QFFitFunction::transformParametersForAdditionalPlot()   */
        virtual QString transformParametersForAdditionalPlot(int plot, double* params);

        /*! \copydoc QFFitFunction::get_implementsDerivatives()   */
        virtual bool get_implementsDerivatives() { return false; }

        /*! \copydoc QFFitFunction::evaluateDerivatives()   */
        virtual void evaluateDerivatives(double* derivatives, double t, const double* parameters) const ;

        /*! \copydoc QFFitFunction::sortParameter()   */
        virtual void sortParameter(double* parameterValues, double* error=NULL , bool *fix=NULL) const;
};

#endif // QFFitFunctionsTIRFCSDiffE2NewVeffE2_H
