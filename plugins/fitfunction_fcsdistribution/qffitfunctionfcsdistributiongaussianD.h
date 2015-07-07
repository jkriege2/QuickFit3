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

#ifndef QFFitFunctionFCSDistributionGaussianD_H
#define QFFitFunctionFCSDistributionGaussianD_H
#include "qfpluginfitfunction.h"
#include <QVector>
#include <QPair>
#include <stdint.h>



/*! \brief QFFitFunction class for FCS fit with an assumed gaussian distribution of diffusion coefficients
    \ingroup qf3fitfunp_fitfunctions_fcsdistribution

*/
class QFFitFunctionFCSDistributionGaussianD: public QFFitFunction {
    public:
        QFFitFunctionFCSDistributionGaussianD();
        virtual ~QFFitFunctionFCSDistributionGaussianD() {}
        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const { return QString("FCS SD: Normal Diffuion 3D with gaussian D-distribution"); }
        /** \copydoc QFFitFunction::shortName() */
        virtual QString shortName() const { return QObject::tr("FCS SD: gaussian D"); }
        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const { return QString("fcs_dist_norm_d"); }
        /*! \copydoc QFFitFunction::category()   */
        virtual QString category() const { return QObject::tr("Confocal FCS"); }

        /*! \copydoc QFFitFunction::evaluate()   */
        virtual double evaluate(double t, const double* parameters) const;

        /*! \copydoc QFFitFunction::calcParameter()   */
        virtual void calcParameter(double* parameterValues, double* error=NULL) const;

        /*! \copydoc QFFitFunction::isParameterVisible()   */
        virtual bool isParameterVisible(int parameter, const double* parameterValues) const;
        /*! \copydoc QFFitFunction::getAdditionalPlotCount()   */
        virtual unsigned int getAdditionalPlotCount(const double* params) const;

        /*! \copydoc QFFitFunction::transformParametersForAdditionalPlot()   */
        virtual QString transformParametersForAdditionalPlot(int plot, double* params) const;

        /*! \copydoc QFFitFunction::get_implementsDerivatives()   */
        virtual bool get_implementsDerivatives() const { return false; };


    protected:

        /*double last_tau_min;
        double last_tau_max;
        double last_logc;
        double last_b;*/

        QVector<QPair<double, double> >  tau_val;

        /** \brief make sure that tau_val is filled with semi-log spaced tau from tau_min to \a tau_max
         *
         * If (last_tau_min==tau_min and last_tau_max==tau_max) nothing has to be done! */
        QVector<QPair<double, double> >  fillDVal(double Dmin, double Dmax, uint32_t Dvalues, double Dc, double Dsigma) const;
};

#endif // QFFitFunctionFCSDistributionGaussianD_H
