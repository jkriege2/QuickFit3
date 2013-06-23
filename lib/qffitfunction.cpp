#include "qffitfunction.h"
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include"qftools.h"

bool QFFitFunction::estimateInitial(double *params, const double *dataX, const double *dataY, long N)
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












