#include "qffitfunction.h"
#include <cstdlib>
#include <cstdio>
#include <cmath>


QFFitStatistics QFFitFunction::calcFitStatistics(long N, double* tauvals, double* corrdata, double* weights, int datacut_min, int datacut_max, double* fullParams, double* /*errors*/, bool* paramsFix, int runAvgWidth, int residualHistogramBins) {
    int fitparamN=0;
    for (int i=0; i<paramCount(); i++) {
        if (isParameterVisible(i, fullParams) && (!paramsFix[i]) && getDescription(i).fit) {
            fitparamN++;
        }
    }

    double* model=(double*)malloc(N*sizeof(double));
    for (int i=0; i<N; i++) {
        model[i]=evaluate(tauvals[i], fullParams);
    }

    return calculateFitStatistics(N, tauvals, model, corrdata, weights, datacut_min, datacut_max, fitparamN, runAvgWidth, residualHistogramBins);
}












