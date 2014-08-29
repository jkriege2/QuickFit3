#ifndef QFFitFunctionsTIRFFCCSFW2DDiffXY2ColorACFG_H
#define QFFitFunctionsTIRFFCCSFW2DDiffXY2ColorACFG_H
#include "qfpluginfitfunction.h"




/*! \brief QFFitFunction class for a TIRF-FCCS fit model with pure diffusion for 2-color crosscorrelation and 1/e^2 lateral width
    \ingroup qf3fitfunp_fitfunctions_TIRFfcs
*/
class QFFitFunctionsTIRFFCCSFW2DDiffXY2ColorACFG: public QFFitFunction {
    public:
        QFFitFunctionsTIRFFCCSFW2DDiffXY2ColorACFG();
        virtual ~QFFitFunctionsTIRFFCCSFW2DDiffXY2ColorACFG() {}
        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const { return QString("TIR-FCCS: 1-comp. 2D diffusion (xy), species A+B+AB, c/D per species, ACF green (1/e^2 radii)"); }
        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const { return QString("fccs_tir_fw_2ddiffxy2coloracfg"); }

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
        void sortParameter(double *parameterValues, double *error, bool *fix) const;
};

#endif // QFFitFunctionsTIRFFCCSFW2DDiffXY2ColorACFG_H
