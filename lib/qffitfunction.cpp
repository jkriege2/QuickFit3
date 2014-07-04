/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

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

#include "qffitfunction.h"
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include"qftools.h"

QVector<double> QFFitFunction::multiEvaluate(const QVector<double> &x, const double *parameters) const
{
    QVector<double> y(x.size(), NAN);
    multiEvaluate(y.data(), x.constData(), x.size(), parameters);
    return y;
}

void QFFitFunction::multiEvaluate(double *y, const double *x, uint64_t N, const double *parameters) const
{
    for (int i=0; i<N; i++) {
        y[i]=evaluate(x[i], parameters);
    }
}

bool QFFitFunction::estimateInitial(double *params, const double *dataX, const double *dataY, long N, const bool *fix)
{
    return false;
}

void QFFitFunction::evaluateNumericalDerivatives(double *derivatives, double x, const double *parameters, double stepsize) const
{
    int N=m_parameters.size();
    double* ptemp=duplicateArray(parameters, N);
    for (int i=0; i<N; i++) {
        derivatives[i]=0;
        const double oldpi=ptemp[i];
        ptemp[i]=oldpi+2.*stepsize;
        const double fp2h=evaluate(x, ptemp);
        ptemp[i]=oldpi+stepsize;
        const double fp1h=evaluate(x, ptemp);
        ptemp[i]=oldpi-stepsize;
        const double fm1h=evaluate(x, ptemp);
        ptemp[i]=oldpi-2.0*stepsize;
        const double fm2h=evaluate(x, ptemp);
        ptemp[i]=oldpi;
        derivatives[i]=(-fp2h+8.0*fp1h-8.0*fm1h+fm2h)/(12.0*stepsize);
    }
    free(ptemp);
}

QFFitStatistics QFFitFunction::calcFitStatistics(long N, double* tauvals, double* corrdata, double* weights, int datacut_min, int datacut_max, double* fullParams, double* /*errors*/, bool* paramsFix, int runAvgWidth, int residualHistogramBins) {
    int fitparamN=0;
    for (int i=0; i<paramCount(); i++) {
        if (isParameterVisible(i, fullParams) && (!paramsFix[i]) && getDescription(i).fit) {
            fitparamN++;
        }
    }

    QVector<double> model(N, 0);
    for (int i=0; i<N; i++) {
        model[i]=evaluate(tauvals[i], fullParams);
    }

    return calculateFitStatistics(N, tauvals, model.data(), corrdata, weights, datacut_min, datacut_max, fitparamN, runAvgWidth, residualHistogramBins);
}












