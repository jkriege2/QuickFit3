/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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

#ifndef QFFITFUNCTIONSSPIMFCSADIFF_H
#define QFFITFUNCTIONSSPIMFCSADIFF_H
#include "qfpluginfitfunction.h"




/*! \brief QFFitFunction class for a SPIM-FCS fit model with anomalous diffusion
    \ingroup qf3fitfunp_fitfunctions_spimfcs

    \f[ g(\tau)=G_\infty+\frac{1}{N}\cdot\frac{1}{4a^2\sqrt{\pi}}\cdot\sum\limits_{i=1}^3\rho_i\cdot\left[2a\cdot\mbox{erf}\left(\frac{a}{2\cdot\sqrt{\Gamma_i\tau^{\alpha_i}+\sigma_{xy}^2}}\right)+\frac{4\cdot\sqrt{\Gamma_i\tau^{\alpha_i}+\sigma_{xy}^2}}{\sqrt{\pi}}\cdot\left(\exp\left(-\frac{a^2}{4\cdot(\Gamma_i\tau^{\alpha_i}+\sigma_{xy}^2)}\right)-1\right)\right]^2\cdot\left[1+\frac{\Gamma_i\tau^{\alpha_i}}{\sigma_z^2}\right]^{-1/2} \f]

    \f[ V_{\text{eff}}=2\cdot a^2\cdot \sigma_z \f]
    \f[ \Delta V_{\text{eff}}=\sqrt{\left(\Delta\sigma_z\cdot 2a^2\right)^2+\left(\Delta a\cdot 4a\sigma_z\right)^2} \f]
    \f[ C=\frac{N}{V_{\text{eff}}}=\frac{N}{2\cdot a^2\cdot \sigma_z} \f]
    \f[ \Delta C=\sqrt{\left(\frac{\Delta N}{2a^2\cdot\sigma_z}\right)^2+\left(\frac{\Delta\sigma_z\cdot N}{2a^2\sigma_z^2}\right)^2+\left(\frac{\Delta a\cdot N}{a^3\cdot\sigma_z}\right)^2} \f]
*/
class QFFitFunctionsSPIMFCSADiff: public QFFitFunction {
    public:
        QFFitFunctionsSPIMFCSADiff();
        virtual ~QFFitFunctionsSPIMFCSADiff() {}
        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const { return QString("SPIM-FCS: Anomalous Diffusion (rect. pixel, 1/sqrt(e) radii)"); }

        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const { return QString("fcs_spim_adiff"); }

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
        virtual bool get_implementsDerivatives() const { return false; }

        /*! \copydoc QFFitFunction::sortParameter()   */
        virtual void sortParameter(double* parameterValues, double* error=NULL , bool *fix=NULL) const;
};

#endif // QFFITFUNCTIONSSPIMFCSADIFF_H
