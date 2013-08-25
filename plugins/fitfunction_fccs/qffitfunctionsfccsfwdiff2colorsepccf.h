#ifndef QFFitFunctionsFCCSFWDiff2ColorSepCCF_H
#define QFFitFunctionsFCCSFWDiff2ColorSepCCF_H
#include "qfpluginfitfunction.h"




/*! \brief QFFitFunction class for a SPIM-FCCS fit model with pure diffusion for 2-color crosscorrelation and 1/sqrt(e) lateral width
    \ingroup qf3fitfunp_fitfunctions_spimfcs
*/
class QFFitFunctionsFCCSFWDiff2ColorSepCCF: public QFFitFunction {
    public:
        QFFitFunctionsFCCSFWDiff2ColorSepCCF();
        virtual ~QFFitFunctionsFCCSFWDiff2ColorSepCCF() {}
        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const { return QString("FCCS: normal diffusion, species A+B+AB, c per species, D per channel, CCF"); }
        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const { return QString("fccs_fw_diff2colorsepccf"); }

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

#endif // QFFitFunctionsFCCSFWDiff2ColorSepCCF_H
