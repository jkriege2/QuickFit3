#ifndef QFFitFunctionsFCCSFWDiff2ColorTCACFG_H
#define QFFitFunctionsFCCSFWDiff2ColorTCACFG_H
#include "qfpluginfitfunction.h"




/*! \brief QFFitFunction class for a SPIM-FCCS fit model with pure diffusion for 2-color crosscorrelation and 1/sqrt(e) lateral width
    \ingroup qf3fitfunp_fitfunctions_spimfcs
*/
class QFFitFunctionsFCCSFWDiff2ColorTCACFG: public QFFitFunction {
    public:
        QFFitFunctionsFCCSFWDiff2ColorTCACFG();
        virtual ~QFFitFunctionsFCCSFWDiff2ColorTCACFG() {}
        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const { return QString("FCCS: 2-comp. normal diffusion, species A+B+AB, c/D1/D2 per species, ACF green"); }
        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const { return QString("fccs_fw_diff2colortcacfg"); }

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
        /*! \copydoc QFFitFunction::sortParameter()   */
        virtual void sortParameter(double* parameterValues, double* error=NULL, bool* fix=NULL) const;
};

#endif // QFFitFunctionsFCCSFWDiff2ColorTCACFG_H
