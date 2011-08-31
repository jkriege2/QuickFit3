#ifndef QFFITFUNCTIONFCSADIFF_H
#define QFFITFUNCTIONFCSADIFF_H
#include "qfpluginfitfunction.h"



/*! \brief plugin class for FCS Anomalous Diffusion Fit Functions
    \ingroup qf3fitfunp_fcs


    This class implements a 3D anomalous diffusion model for the fitting routines in QuickFit2. The model may be denoted as:
      \f[ G(\tau)=G_\infty+\frac{1}{N}\cdot\left(\frac{1-\Theta_{non}+\Theta_{non}\mathrm{e}^{-\tau/\tau_{non}}-\Theta_{trip}+\Theta_{trip}\mathrm{e}^{-\tau/\tau_{trip}}}{1-\Theta_{non}-\Theta_{trip}}\right)\cdot\left[(1-\rho_2-\rho_3)\cdot g_1(\tau)+\rho_2\cdot g_2(\tau)+\rho_3\cdot g_3(\tau)\right] \f]
    with
      \f[ g_i(\tau)=\left(1+\left(\frac{\tau}{\tau_{diff,i}}\right)^{\alpha_i}\right)^{-1}\cdot\left(1+\frac{1}{\gamma^2}\cdot\left(\frac{\tau}{\tau_{diff,i}}\right)^{\alpha_i}\right)^{-1/2} \f]
    The parameters are:
      - \f$ G_\infty \f$: offset of the correlation function
      - \f$ N \f$: overall particle number (including currently dark particles, e.g. in triplet state)
      - \f$ \Theta_{trip}, \Theta_{non} \f$: fraction of the particles in one of the first two non-fluorescent states (priplet, ...)
      - \f$ \tau_{trip}, \tau_{non} \f$: decay time of the first two non-fluorescent states (triplet, ...)
      - \f$ \rho_1=1-\rho2-\rho3, \rho_2, \rho_3 \f$: fractions of the three diffusing components
      - \f$ \tau_{diff,i} \f$: diffusion decay time of the i-th diffusing component
      - \f$ \alpha_i \f$: anomality exponent of the i-th diffusing fraction
      - \f$ \gamma=\frac{z_0}{w_{xy}} \f$: aspect ratio of the gaussian used to approximate the focus
      - \f$ w_{xy} \f$: lateral half axis of the focus gaussian
      - \f$ z_0 \f$: longitudinal half axis of the focus gaussian
    .


    This model also calculates the effective focal volume:
      \f[ V_{eff}=\pi^{3/2}\cdot\gamma\cdot w_{xy}^3 \f]
    Given this focal volume, the plugin may also calculate the particle concentration in the sample:
      \f[ C=\frac{N}{V_{eff}} \f]

    The plugin also calculates some parameter errors:
      \f[ \Delta \left(\frac{1}{N}\right)=\left|\frac{\Delta N}{N^2}\right| \f]
      \f[ \Delta V_{eff}=\sqrt{\left(\Delta\gamma\cdot\pi^{3/2}\cdot w_{xy}^3\right)^2+\left(\Delta w_{xy}\cdot 3\pi^{3/2}\cdot \gamma\cdot w_{xy}^2\right)^2} \f]
      \f[ \Delta C=\sqrt{\left(\frac{\Delta N}{\pi^{3/2}\cdot\gamma\cdot w_{xy}^{3}}\right)^2+\left(\Delta\gamma\cdot\frac{N}{\pi^{3/2}\cdot w{xy}^3\cdot\gamma^2}\right)^2+\left(\Delta w_{xy}\cdot\frac{3\cdot N}{\pi^{3/2}\cdot\gamma\cdot w_{xy}^4}\right)^2} \f]

*/
class QFFitFunctionFCSADiff: public QFFitFunction {
    public:
        QFFitFunctionFCSADiff();
        virtual ~QFFitFunctionFCSADiff() {}
        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const { return QString("FCS: Anomalous Diffusion 3D"); };
        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const { return QString("fcs_adiff"); };

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




#endif // QFFITFUNCTIONFCSADIFF_H
