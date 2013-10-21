#ifndef QFFitFunctionsTIRFFCCSFW2CSep2DDiffXY2ColorCCF_H
#define QFFitFunctionsTIRFFCCSFW2CSep2DDiffXY2ColorCCF_H
#include "qfpluginfitfunction.h"




/*! \brief QFFitFunction class for a TIRF-FCCS fit model with pure diffusion for 2-color crosscorrelation and 1/e^2 lateral width
    \ingroup qf3fitfunp_fitfunctions_TIRFfcs
*/
class QFFitFunctionsTIRFFCCSFW2CSep2DDiffXY2ColorCCF: public QFFitFunction {
    public:
        QFFitFunctionsTIRFFCCSFW2CSep2DDiffXY2ColorCCF();
        virtual ~QFFitFunctionsTIRFFCCSFW2CSep2DDiffXY2ColorCCF() {}
        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const { return QString("TIR-FCCS: 2-comp. 2D diffusion (xy), species A+B+AB, c per species, D1/D2 per channel, CCF (1/e² radii)"); }
        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const { return QString("fccs_tir_fw_2csep2ddiffxy2colorccf"); }

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

#endif // QFFitFunctionsTIRFFCCSFW2CSep2DDiffXY2ColorCCF_H
