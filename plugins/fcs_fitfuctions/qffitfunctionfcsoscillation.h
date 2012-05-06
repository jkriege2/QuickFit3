#ifndef QFFITFUNCTIONFCSOSCILLATION_H
#define QFFITFUNCTIONFCSOSCILLATION_H

#include "qffitfunction.h"


/*! \brief plugin class for oscillatory FCS correlation functions
    \ingroup qf3fitfunp_fcs

    This class implements a oscillatory FCS correlation function.
        \f[ G(\tau)=G_\infty+A\cdot\cos(2\pi\cdot f\cdot \tau) \f]
    The parameters are:
      - \f$ G_\infty \f$: offset of the correlation function
      - \f$ A \f$: oscillation amplitude
      - \f$ f \f$: oscillation frequency in hertz
    .
                                                                                                                                                                                                                                                                                                          derivatives[FCSSDiff_diff_tau1]=fT/N*(t/sqr(tauD)/fDG12/sqr(fD) + t/2.0/fD/cube(fDG12)/gamma2/sqr(tauD));

*/
class QFFitFunctionFCSOscillation : public QFFitFunction
{
    public:
        QFFitFunctionFCSOscillation();
        ~QFFitFunctionFCSOscillation();

        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const { return QString("FCS: Oscillation"); }
        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const { return QString("fcs_oscillation"); }

        /*! \copydoc QFFitFunction::evaluate()   */
        virtual double evaluate(double x, const double* parameters) const;

        /*! \copydoc QFFitFunction::calcParameter()   */
        virtual void calcParameter(double* parameterValues, double* error=NULL) const;

        /*! \copydoc QFFitFunction::isParameterVisible()   */
        virtual bool isParameterVisible(int parameter, const double* parameterValues) const;
        /*! \copydoc QFFitFunction::getAdditionalPlotCount()   */
        virtual unsigned int getAdditionalPlotCount(const double* params);

        /*! \copydoc QFFitFunction::transformParametersForAdditionalPlot()   */
        virtual QString transformParametersForAdditionalPlot(int plot, double* params);

};

#endif // QFFITFUNCTIONFCSOSCILLATION_H
