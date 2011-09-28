#ifndef QFFITFUNCTIONSSPIMFCSDIFF_H
#define QFFITFUNCTIONSSPIMFCSDIFF_H
#include "qfpluginfitfunction.h"




/*! \brief QFFitFunction class for a SPIM-FCS fit model with pure diffusion
    \ingroup qf3fitfunp_fitfunctions_spimfcs

    \f[ g(\tau)=\frac{1}{4a^2\sqrt{\pi}N}\cdot\left[2a\cdot\mbox{erf}\left(\frac{a}{2\sqrt{D\tau+\sigma_{xy}^2}}\right)+\frac{4\sqrt{D\tau+\sigma_{xy}^2}}{\sqrt{\pi}}\cdot\left(\exp\left(-\frac{a^2}{4(D\tau+\sigma_{xy}^2)}\right)-1\right)\right]^2\cdot\left[1+\frac{D\tau}{\sigma_z^2}\right]^{-1/2}+G_\infty \f]

    \f[ V_{\text{eff}}=2\cdot a^2\cdot \sigma_z \f]
    \f[ \Delta V_{\text{eff}}=\sqrt{\left(\Delta\sigma_z\cdot 2a^2\right)^2+\left(\Delta a\cdot 4a\sigma_z\right)^2} \f]
    \f[ C=\frac{N}{V_{\text{eff}}}=\frac{N}{2\cdot a^2\cdot \sigma_z} \f]
    \f[ \Delta V=\sqrt{\left(\frac{\Delta N}{2a^2\cdot\sigma_z}\right)^2+\left(\frac{\Delta\sigma_z\cdot N}{2a^2\sigma_z^2}\right)^2+\left(\frac{\Delta a\cdot N}{a^3\cdot\sigma_z}\right)^2} \f]
*/
class QFFitFunctionsSPIMFCSDiff: public QFFitFunction {
    public:
        QFFitFunctionsSPIMFCSDiff();
        virtual ~QFFitFunctionsSPIMFCSDiff() {}
        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const { return QString("SPIM-FCS fit model 1-component diffusion"); };
        /** \copydoc QFFitFunction::shortName() */
        virtual QString shortName() const { return QObject::tr("SPIM-FCS: Diffusion"); };
        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const { return QString("fcs_spim_diff"); };

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
        virtual bool get_implementsDerivatives() { return false; };

        /*! \copydoc QFFitFunction::evaluateDerivatives()   */
        virtual void evaluateDerivatives(double* derivatives, double t, const double* parameters) const ;
};

#endif // QFFITFUNCTIONSSPIMFCSDIFF_H
