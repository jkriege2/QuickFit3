/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef QFFITFUNCTIONFCSSIMPLEDIFF_H
#define QFFITFUNCTIONFCSSIMPLEDIFF_H
#include "qfpluginfitfunction.h"
#include "qffitfunction.h"
#include "qfgeneralfitfunctionbase.h"



/*! \brief plugin class for FCS Normal Diffusion Fit Functions with analytical derivatives
    \ingroup qf3fitfunp_fcs

    This class implements a 3D diffusion model for the fitting routines in QuickFit3. The model may be denoted as:
      \f[ G(\tau)=G_\infty+\frac{1}{N}\cdot\left(\frac{1-\Theta_{trip}+\Theta_{trip}\mathrm{e}^{-\tau/\tau_{trip}}}{1-\Theta_{trip}}\right)\cdot\left(1+\frac{\tau}{\tau_{diff}}\right)^{-1}\cdot\left(1+\frac{\tau}{\gamma^2\tau_{diff}}\right)^{-1/2}  \f]
    The parameters are:
      - \f$ G_\infty \f$: offset of the correlation function
      - \f$ N \f$: overall particle number (including currently dark particles, e.g. in triplet state)
      - \f$ \Theta_{trip} \f$: fraction of the particles in the triplet state
      - \f$ \tau_{trip} \f$: decay time of the triplet state
      - \f$ \tau_{diff} \f$: diffusion decay time
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

    The plugin also calculates some parameter errors:
      \f[ \Delta \left(\frac{1}{N}\right)=\left|\frac{\Delta N}{N^2}\right| \f]
      \f[ \Delta D=\sqrt{\left(\Delta w_{xy}\cdot\frac{w_{xy}}{2\tau_D}\right)^2+\left(\Delta \tau_D\cdot\frac{w_{xy}^2}{4\tau_D^2}\right)^2} \f]
      \f[ \Delta V_{eff}=\sqrt{\left(\Delta\gamma\cdot\pi^{3/2}\cdot w_{xy}^3\right)^2+\left(\Delta w_{xy}\cdot 3\pi^{3/2}\cdot \gamma\cdot w_{xy}^2\right)^2} \f]
      \f[ \Delta C=\sqrt{\left(\frac{\Delta N}{\pi^{3/2}\cdot\gamma\cdot w_{xy}^{3}}\right)^2+\left(\Delta\gamma\cdot\frac{N}{\pi^{3/2}\cdot w{xy}^3\cdot\gamma^2}\right)^2+\left(\Delta w_{xy}\cdot\frac{3\cdot N}{\pi^{3/2}\cdot\gamma\cdot w_{xy}^4}\right)^2} \f]

    The analytical derivatives of \f$ g(\tau) \f$ are:

    \f[ \frac{\partial g}{\partial N}= -\frac{1}{N^2}\cdot\left(\frac{1-\Theta_{trip}+\Theta_{trip}\mathrm{e}^{-\tau/\tau_{trip}}}{1-\Theta_{trip}}\right)\cdot\left(1+\frac{\tau}{\tau_{diff}}\right)^{-1}\cdot\left(1+\frac{\tau}{\gamma^2\tau_{diff}}\right)^{-1/2} \f]
    \f[ \frac{\partial g}{\partial \Theta_{trip}}= \frac{1}{N}\cdot\left(\frac{\mathrm{e}^{-\tau/\tau_{trip}}}{(1-\Theta_{trip})^2}\right)\cdot\left(1+\frac{\tau}{\tau_{diff}}\right)^{-1}\cdot\left(1+\frac{\tau}{\gamma^2\tau_{diff}}\right)^{-1/2} \f]
    \f[ \frac{\partial g}{\partial \tau_{trip}}= -\frac{1}{N}\cdot\left(\frac{\Theta_{trip}\cdot \tau\cdot \mathrm{e}^{-\tau/\tau_{trip}}}{(1-\Theta_{trip})\cdot \tau_{trip}^2}\right)\cdot\left(1+\frac{\tau}{\tau_{diff}}\right)^{-1}\cdot\left(1+\frac{\tau}{\gamma^2\tau_{diff}}\right)^{-1/2} \f]
    \f[ \frac{\partial g}{\partial \tau_{diff}}= \frac{1}{N}\cdot\left(\frac{1-\Theta_{trip}+\Theta_{trip}\mathrm{e}^{-\tau/\tau_{trip}}}{1-\Theta_{trip}}\right)\cdot \left\{\frac{\tau}{\tau_{diff}^2}\left(1+\frac{\tau}{\tau_{diff}}\right)^{-2}\cdot\left(1+\frac{\tau}{\gamma^2\tau_{diff}}\right)^{-1/2} + \frac{\tau}{2\gamma^2\tau_{diff}^2}\cdot\left(1+\frac{\tau}{\tau_{diff}}\right)^{-1}\cdot\left(1+\frac{\tau}{\gamma^2\tau_{diff}}\right)^{-3/2}\right\} \f]
    \f[ \frac{\partial g}{\partial \gamma}= \frac{1}{N}\cdot\left(\frac{1-\Theta_{trip}+\Theta_{trip}\mathrm{e}^{-\tau/\tau_{trip}}}{1-\Theta_{trip}}\right)\cdot\left(1+\frac{\tau}{\tau_{diff}}\right)^{-1}\cdot\left(1+\frac{\tau}{\gamma^2\tau_{diff}}\right)^{-3/2}\cdot\frac{\tau}{\gamma^3\tau_{diff}}  \f]
    \f[ \frac{\partial g}{\partial G_\infty}= 1 \f]


                                                                                                                                                                                                                                                                                                           derivatives[FCSSDiff_diff_tau1]=fT/N*(t/sqr(tauD)/fDG12/sqr(fD) + t/2.0/fD/cube(fDG12)/gamma2/sqr(tauD));

*/
class QFFitFunctionFCSSimpleDiff: public QFFCSFitFunctionBase {
    public:
        QFFitFunctionFCSSimpleDiff();
        virtual ~QFFitFunctionFCSSimpleDiff() {}
        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const { return QObject::tr("FCS: 3D 1-component Normal Diffusion, analytical derivatives"); };
        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const { return QString("fcs_diff1"); };
        /*! \copydoc QFFitFunction::category()   */
        virtual QString category() const {
            return QObject::tr("Confocal FCS");
        }
        /*! \copydoc QFFitFunction::evaluate()   */
        virtual double evaluate(double x, const double* parameters) const;

        /*! \copydoc QFFitFunction::calcParameter()   */
        virtual void calcParameter(double* parameterValues, double* error=NULL) const;

        /*! \copydoc QFFitFunction::isParameterVisible()   */
        virtual bool isParameterVisible(int parameter, const double* parameterValues) const;
        /*! \copydoc QFFitFunction::getAdditionalPlotCount()   */
        virtual unsigned int getAdditionalPlotCount(const double* params) const;

        /*! \copydoc QFFitFunction::transformParametersForAdditionalPlot()   */
        virtual QString transformParametersForAdditionalPlot(int plot, double* params) const;

        /*! \copydoc QFFitFunction::get_implementsDerivatives()   */
        virtual bool get_implementsDerivatives() const { return true; };

        /*! \copydoc QFFitFunction::evaluateDerivatives()   */
        virtual void evaluateDerivatives(double* derivatives, double x, const double* parameters) const ;
};

#endif // QFFITFUNCTIONFCSSIMPLEDIFF_H
