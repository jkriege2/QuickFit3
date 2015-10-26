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

#ifndef QFFitFunctionFCSDistributionIntLogGaussian_H
#define QFFitFunctionFCSDistributionIntLogGaussian_H
#include "qfpluginfitfunction.h"
#include <QVector>
#include <QPair>
#include <stdint.h>
#include <gsl/gsl_integration.h>
#include <gsl/gsl_errno.h>
#include "qfgeneralfitfunctionbase.h"
/*! \brief QFFitFunction class for FCS fit with an assumed gaussian distribution of diffusion times
    \ingroup qf3fitfunp_fitfunctions_fcsdistribution

*/
class QFFitFunctionFCSDistributionIntLogGaussian: public QFFCSFitFunctionBase {
    public:
        QFFitFunctionFCSDistributionIntLogGaussian();
        virtual ~QFFitFunctionFCSDistributionIntLogGaussian();
        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const { return QString("FCS Dist: Normal Diffuion 3D with log-normal diffusion time distribution [num_int]"); }
        /** \copydoc QFFitFunction::shortName() */
        virtual QString shortName() const { return name(); }
        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const { return QString("fcs_dist_int_lognorm"); }
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

        gsl_integration_workspace * w;
        size_t wN;

};

#endif // QFFitFunctionFCSDistributionIntLogGaussian_H
