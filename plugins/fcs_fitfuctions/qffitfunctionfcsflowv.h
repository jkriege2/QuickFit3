#ifndef QFFitFunctionFCSFlowV_H
#define QFFitFunctionFCSFlowV_H
#include "qfpluginfitfunction.h"
#include "qffitfunction.h"


/*! \brief plugin class for FCS Normal Diffusion Fit Functions
    \ingroup qf3fitfunp_fcs

    This class implements a 3D diffusion model for the fitting routines in QuickFit2. The model may be denoted as:
      \f[ G(\tau)=G_\infty+\frac{1}{N}\cdot\frac{1}{(1+b/\mbox{cps})^2}\cdot\left(\frac{1-\Theta_{trip}+\Theta_{trip}\mathrm{e}^{-\tau/\tau_{trip}}}{1-\Theta_{trip}}\right)\cdot\left(1+\frac{4D\tau}{w_{xy}^2}\right)^{-1}\cdot\left(1+\frac{4D\tau}{\gamma^2w_{xy}^2}\right)^{-1/2}\cdot\exp\left(-\frac{v\tau}{w_{xy}\cdot\left(1+\frac{4D\tau}{w_{xy}^2}\right)\right) \f]
    The parameters are:
      - \f$ G_\infty \f$: offset of the correlation function
      - \f$ N \f$: overall particle number (including currently dark particles, e.g. in triplet state)
      - \f$ \Theta_{trip} \f$: fraction of the particles in one of the first two non-fluorescent states (priplet, ...)
      - \f$ \tau_{trip} \f$: decay time of the first two non-fluorescent states (triplet, ...)
      - \f$ D \f$: diffusion coefficient
      - \f$ v \f$: flow velocity
      - \f$ \gamma=\frac{z_0}{w_{xy}} \f$: aspect ratio of the gaussian used to approximate the focus
      - \f$ w_{xy} \f$: lateral half axis of the focus gaussian
      - \f$ z_0 \f$: longitudinal half axis of the focus gaussian
    .


    where \f$ \tau_D \f$ is the diffusion time. Also the effective focal volume \f$ V_{eff} \f$ is calculated:
      \f[ V_{eff}=\pi^{3/2}\cdot\gamma\cdot w_{xy}^3 \f]
    Given this focal volume, the plugin may also calculate the particle concentration in the sample:
      \f[ C=\frac{N}{V_{eff}} \f]

    Given the count rate cps and the background count rate \f$ b \f$, we can calculate the counts per molecule:
      \f[ \mbox{cpm}=\frac{\mbox{cps}-b}{N} \f]

    The plugin also calculates some parameter errors:
      \f[ \Delta \left(\frac{1}{N}\right)=\left|\frac{\Delta N}{N^2}\right| \f]
      \f[ \Delta V_{eff}=\sqrt{\left(\Delta\gamma\cdot\pi^{3/2}\cdot w_{xy}^3\right)^2+\left(\Delta w_{xy}\cdot 3\pi^{3/2}\cdot \gamma\cdot w_{xy}^2\right)^2} \f]
      \f[ \Delta C=\sqrt{\left(\frac{\Delta N}{\pi^{3/2}\cdot\gamma\cdot w_{xy}^{3}}\right)^2+\left(\Delta\gamma\cdot\frac{N}{\pi^{3/2}\cdot w{xy}^3\cdot\gamma^2}\right)^2+\left(\Delta w_{xy}\cdot\frac{3\cdot N}{\pi^{3/2}\cdot\gamma\cdot w_{xy}^4}\right)^2} \f]
      \f[ \Delta \mbox{cpm}=\sqrt{\left(\Delta\mbox{cps}\cdot\frac{1}{N}\right)^2+\left(\Delta b\cdot\frac{1}{N}\right)^2+\left(\Delta N\cdot\frac{\mbox{cps}-b}{N^2}\right)^2} \f]

*/
class QFFitFunctionFCSFlowV: public QFFitFunction {
    public:
        QFFitFunctionFCSFlowV();
        virtual ~QFFitFunctionFCSFlowV() {}

        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const { return QString("FCS: Normal Diffusion 3D with flow term (D,v,wxy as fit params)"); }
        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const { return QString("fcs_diffflowv"); }

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

#endif // QFFitFunctionFCSFlowV_H
