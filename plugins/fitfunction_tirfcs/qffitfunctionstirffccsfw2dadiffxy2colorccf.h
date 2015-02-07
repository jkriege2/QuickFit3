/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2014-12-02 18:02:53 +0100 (Di, 02 Dez 2014) $  (revision $Rev: 3657 $)

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

#ifndef QFFitFunctionsTIRFFCCSFW2DADiffXY2ColorCCF_H
#define QFFitFunctionsTIRFFCCSFW2DADiffXY2ColorCCF_H
#include "qfpluginfitfunction.h"




/*! \brief QFFitFunction class for a TIRF-FCCS fit model with pure diffusion for 2-color crosscorrelation and 1/e^2 lateral width
    \ingroup qf3fitfunp_fitfunctions_TIRFfcs
*/
class QFFitFunctionsTIRFFCCSFW2DADiffXY2ColorCCF: public QFFitFunction {
    public:
        QFFitFunctionsTIRFFCCSFW2DADiffXY2ColorCCF();
        virtual ~QFFitFunctionsTIRFFCCSFW2DADiffXY2ColorCCF() {}
        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const { return QString("2-color TIR-FCCS: 1-comp. 2D anomalous diffusion (xy), species A+B+AB, c/D per species (rect. pixel, 1/e^2 radii), CCF"); }
        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const { return QString("fccs_tir_fw_2dadiffxy2colorccf"); }
        /*! \copydoc QFFitFunction::category()   */
        virtual QString category() const { return QObject::tr("2-color TIR-FCCS"); }

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
        void sortParameter(double *parameterValues, double *error, bool *fix) const;
};

#endif // QFFitFunctionsTIRFFCCSFW2DADiffXY2ColorCCF_H
