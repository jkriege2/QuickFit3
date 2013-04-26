#ifndef QFFITFUNCTIONSSPIMFCSADIFFCE2_H
#define QFFITFUNCTIONSSPIMFCSADIFFCE2_H
#include "qfpluginfitfunction.h"




/*! \brief QFFitFunction class for a SPIM-FCS fit model with anomalous diffusion and e²-radii, new Veff
    \ingroup qf3fitfunp_fitfunctions_spimfcs

\f[ g(\tau)=G_\infty+\frac{1}{C}\cdot\frac{1}{\sqrt{\pi}\cdot w_za^2}\cdot\sum\limits_{i=1}^3\rho_i\cdot\left[\mbox{erf}\left(\frac{a}{\sqrt{4\Gamma_i\tau^{\alpha_i}+w_{xy}^2}}\right)+\sqrt{4\Gamma_i\tau^{\alpha_i}+w_{xy}^2}\cdot\left[\exp\left(-\frac{a^2}{4\Gamma_i\tau^{\alpha_i}+w_{xy}^2}\right)-1\right]\right]^2\cdot\left[1+\frac{4\Gamma_i\tau^{\alpha_i}}{w_z^2}\right]^{-1/2} \f]
    \f[ V_{\text{eff}}=\frac{\sqrt{\pi}\cdot a^2w_z}{\left(\mbox{erf}\left(\frac{a}{w_{xy}}\right)+\frac{w_{xy}}{\sqrt{\pi}\cdot a}\cdot\left(e^{-(a/w_{xy})^2}-1\right)\right)^2} \f]

*/
class QFFitFunctionsSPIMFCSADiffCE2: public QFFitFunction {
    public:
        QFFitFunctionsSPIMFCSADiffCE2();
        virtual ~QFFitFunctionsSPIMFCSADiffCE2() {}
        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const { return QString("SPIM-FCS: Anomalous Diffusion, c as parameter (1/e² radii, new Veff)"); }
        /** \copydoc QFFitFunction::shortName() */
        virtual QString shortName() const { return QObject::tr("SPIM-FCS: Anomalous Diffusion, c as parameter (1/e² radii, new Veff)"); }
        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const { return QString("fcs_spim_adiffce2"); }

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
        virtual void sortParameter(double* parameterValues, double* error=NULL , bool *fix=NULL) const;
};

#endif // QFFITFUNCTIONSSPIMFCSADIFFCE2_H
