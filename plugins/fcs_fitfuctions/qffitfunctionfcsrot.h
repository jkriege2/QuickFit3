#ifndef QFFITFUNCTIONFCSROT_H
#define QFFITFUNCTIONFCSROT_H
#include "qfpluginfitfunction.h"
#include "qffitfunction.h"




/*! \brief plugin class for FCS Normal Diffusion Fit Functions with rotation
    \ingroup qf3fitfunp_fcs

    This class implements a 3D diffusion model for the fitting routines in QuickFit3. The model may be denoted as:
      \f[ G(\tau)=G_\infty+\frac{1}{N}\cdot\left(\frac{1-\Theta_{trip}+\Theta_{trip}\mathrm{e}^{-\tau/\tau_{trip}}}{1-\Theta_{trip}}\right)\cdot\left(1+\frac{\tau}{\tau_{diff}}\right)^{-1}\cdot\left(1+\frac{\tau}{\gamma^2\tau_{diff}}\right)^{-1/2}\cdot\left[1-F_{rot}+F_{rot}\cdot\left(c_1\cdot\mathrm{e}^{-\tau/\tau_{rot}}+c_2\cdot\mathrm{e}^{-3\tau/(10\tau_{rot})}\right)\right]  \f]
    The parameters are:
      - \f$ G_\infty \f$: offset of the correlation function
      - \f$ N \f$: overall particle number (including currently dark particles, e.g. in triplet state)
      - \f$ \Theta_{trip} \f$: fraction of the particles in the triplet state
      - \f$ \tau_{trip} \f$: decay time of the triplet state
      - \f$ F_{rot} \f$: prefactor for the rotating particles
      - \f$ \tau_{rot} \f$: rotation decay time
      - \f$ c_1, c_2 \f$: prefactors for flow, see reference
      - \f$ \gamma=\frac{z_0}{w_{xy}} \f$: aspect ratio of the gaussian used to approximate the focus
      - \f$ w_{xy} \f$: lateral half axis of the focus gaussian
      - \f$ z_0 \f$: longitudinal half axis of the focus gaussian
    .

    This model also calculates the diffusion coefficient, if the width \f$ w_{xy} \f$ of the laser focus (xy plane) is known, as:
      \f[ D=\frac{w_{xy}^2}{4\tau_D} \f]
    where \f$ \tau_D \f$ is the diffusion time. Also the effective focal volume \f$ V_{eff} \f$ is calculated:
      \f[ V_{eff}=\pi^{3/2}\cdot\gamma\cdot w_{xy}^3 \f]
    Given this focal volume, the plugin may also calculate the particle concentration in the sample:
      \f[ C=\frac{N}{V_{eff}} \f]
    The rotational diffusion coefficient is defined as:
      \f[ \Theta_{rot}=\frac{1}{6\tau_{rot}} \f]


    The plugin also calculates some parameter errors:
      \f[ \Delta \left(\frac{1}{N}\right)=\left|\frac{\Delta N}{N^2}\right| \f]
      \f[ \Delta D=\sqrt{\left(\Delta w_{xy}\cdot\frac{w_{xy}}{2\tau_D}\right)^2+\left(\Delta \tau_D\cdot\frac{w_{xy}^2}{4\tau_D^2}\right)^2} \f]
      \f[ \Delta \Theta_{rot}=\left|\frac{\Delta\tau_{rot}}{6\tau_{rot}^2}\right| \f]
      \f[ \Delta V_{eff}=\sqrt{\left(\Delta\gamma\cdot\pi^{3/2}\cdot w_{xy}^3\right)^2+\left(\Delta w_{xy}\cdot 3\pi^{3/2}\cdot \gamma\cdot w_{xy}^2\right)^2} \f]
      \f[ \Delta C=\sqrt{\left(\frac{\Delta N}{\pi^{3/2}\cdot\gamma\cdot w_{xy}^{3}}\right)^2+\left(\Delta\gamma\cdot\frac{N}{\pi^{3/2}\cdot w{xy}^3\cdot\gamma^2}\right)^2+\left(\Delta w_{xy}\cdot\frac{3\cdot N}{\pi^{3/2}\cdot\gamma\cdot w_{xy}^4}\right)^2} \f]



    This model was taken from the paper:
       - S. R. Aragòn, R. Pecora: Fluorescence correlation spectroscopy and Brownian rotational diffusion, Biopolymers, Volume 14, Issue 1, pages 119-137, January 1975, DOI: 10.1002/bip.1975.360140110
    .

*/
class QFFitFunctionFCSRot: public QFFitFunction {
    public:
        QFFitFunctionFCSRot();
        virtual ~QFFitFunctionFCSRot() {}
        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const { return QString("FCS: Normal Diffusion 3D with rotation term"); }
        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const { return QString("fcs_diffrot"); }

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

        /*! \copydoc QFFitFunction::get_implementsDerivatives()   */
        virtual bool get_implementsDerivatives() { return false; };

};

#endif // QFFITFUNCTIONFCSROT_H
