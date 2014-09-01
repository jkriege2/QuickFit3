#ifndef QFFITFUNCTIONFCSDISTRIBUTIONLOGGAUSSIAN_H
#define QFFITFUNCTIONFCSDISTRIBUTIONLOGGAUSSIAN_H
#include "qfpluginfitfunction.h"
#include <QVector>
#include <QPair>

#include <stdint.h>


/*! \brief QFFitFunction class for FCS fit with an assumed log-normal distribution of diffusion correlation times
    \ingroup qf3fitfunp_fitfunctions_fcsdistribution

    This QFFitFunction implements a 3D normal diffusion FCS model, where the diffusion times are
    log-normally distributed:
      \f[ g(\tau)=G_\infty+\frac{1}{N}\cdot X_{nf}(\tau)\cdot\sum\limits_{k=0}^KA\cdot\exp\left[-\frac{1}{2}\cdot\left(\frac{\ln(\tau_{k})-\ln(\tau_{diff,c})}{b}\right)^2\right]\left(1+\frac{\tau}{\tau_{k}}\right)^{-1}\cdot\left(1+\frac{\tau}{\gamma^2\tau_{k}}\right)^{-1/2} \f]
    with
      \f[ N_{nf}(\tau)= \frac{1-\Theta_{non}+\Theta_{non}\mathrm{e}^{-\tau/\tau_{non}}-\Theta_{trip}+\Theta_{trip}\mathrm{e}^{-\tau/\tau_{trip}}}{1-\Theta_{non}-\Theta_{trip}} \f]

    The parameters are:
      - \f$ G_\infty \f$: offset of the correlation function
      - \f$ N \f$: overall particle number (including currently dark particles, e.g. in triplet state)
      - \f$ \Theta_{trip}, \Theta_{non} \f$: fraction of the particles in one of the first two non-fluorescent states (priplet, ...)
      - \f$ \tau_{trip}, \tau_{non} \f$: decay time of the first two non-fluorescent states (triplet, ...)
      - \f$ \tau_{diff,c} \f$: center diffusion decay time
      - \f$ b \f$: width parameter of the diffusion time distribution
      - \f$ \gamma=\frac{z_0}{w_{xy}} \f$: aspect ratio of the gaussian used to approximate the focus
      - \f$ w_{xy} \f$: lateral half axis of the focus gaussian
      - \f$ z_0 \f$: longitudinal half axis of the focus gaussian
    .

    The values \f$ \tau_k \f$ used in the sum of this fitting function are beeing semi-logarithmically spaced between the given minimum and maximum
    values.


    This model also calculates the diffusion coefficient, if the width \f$ w_{xy} \f$ of the laser focus (xy plane) is known, as:
      \f[ D=\frac{w_{xy}^2}{4\tau_D} \f]
    where \f$ \tau_D \f$ is the diffusion time. Also the effective focal volume \f$ V_{eff} \f$ is calculated:
      \f[ V_{eff}=\pi^{3/2}\cdot\gamma\cdot w_{xy}^3 \f]
    Given this focal volume, the plugin may also calculate the particle concentration in the sample:
      \f[ C=\frac{N}{V_{eff}} \f]

    The plugin also calculates some parameter errors:
      \f[ \Delta \left(\frac{1}{N}\right)=\left|\frac{\Delta N}{N^2}\right| \f]
      \f[ \Delta D=\sqrt{\left(\Delta w_{xy}\cdot\frac{w_{xy}}{2\tau_D}\right)^2+\left(\Delta \tau_D\cdot\frac{w_{xy}^2}{4\tau_D^2}\right)^2} \f]
      \f[ \Delta V_{eff}=\sqrt{\left(\Delta\gamma\cdot\pi^{3/2}\cdot w_{xy}^3\right)^2+\left(\Delta w_{xy}\cdot 3\pi^{3/2}\cdot \gamma\cdot w_{xy}^2\right)^2} \f]
      \f[ \Delta C=\sqrt{\left(\frac{\Delta N}{\pi^{3/2}\cdot\gamma\cdot w_{xy}^{3}}\right)^2+\left(\Delta\gamma\cdot\frac{N}{\pi^{3/2}\cdot w{xy}^3\cdot\gamma^2}\right)^2+\left(\Delta w_{xy}\cdot\frac{3\cdot N}{\pi^{3/2}\cdot\gamma\cdot w_{xy}^4}\right)^2} \f]

*/
class QFFitFunctionFCSDistributionLogGaussian: public QFFitFunction {
    public:
        QFFitFunctionFCSDistributionLogGaussian();
        virtual ~QFFitFunctionFCSDistributionLogGaussian() {}
        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const { return QString("FCS SD: Normal Diffuion 3D with log-normal size distribution"); }
        /** \copydoc QFFitFunction::shortName() */
        virtual QString shortName() const { return QObject::tr("FCS SD: log-norm"); }
        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const { return QString("fcs_dist_lognorm"); }

        /** \copydoc QFFitFunction::isDeprecated() */
        virtual bool isDeprecated() const { return true; }

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


    protected:

        /*double last_tau_min;
        double last_tau_max;
        double last_logc;
        double last_b;*/

        QVector<QPair<double, double> >  tau_val;

        /** \brief make sure that tau_val is filled with semi-log spaced tau from tau_min to \a tau_max
         *
         * If (last_tau_min==tau_min and last_tau_max==tau_max) nothing has to be done! */
        QVector<QPair<double, double> >  fillTauVal(double tau_min, double tau_max, double logc, double b) const;
};

#endif // QFFITFUNCTIONFCSDISTRIBUTIONLOGGAUSSIAN_H
