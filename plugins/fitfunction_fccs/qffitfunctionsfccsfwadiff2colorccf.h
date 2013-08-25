#ifndef QFFitFunctionsFCCSFWADiff2ColorCCF_H
#define QFFitFunctionsFCCSFWADiff2ColorCCF_H
#include "qfpluginfitfunction.h"




/*! \brief QFFitFunction class for a SPIM-FCCS fit model with pure diffusion for 2-color crosscorrelation and 1/sqrt(e) lateral width
    \ingroup qf3fitfunp_fitfunctions_spimfcs
*/
class QFFitFunctionsFCCSFWADiff2ColorCCF: public QFFitFunction {
    public:
        QFFitFunctionsFCCSFWADiff2ColorCCF();
        virtual ~QFFitFunctionsFCCSFWADiff2ColorCCF() {}
        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const { return QString("FCCS: anomalous diffusion, species A+B+AB, c/D per species, CCF"); }
        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const { return QString("fccs_fw_adiff2colorccf"); }

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
};

#endif // QFFitFunctionsFCCSFWADiff2ColorCCF_H
