#include "qffitfunctiongeneralgaussiandistvar.h"
#include "qfmathtools.h"
#include <cmath>
#include "statistics_tools.h"

QFFitFunctionGeneralGaussianDistVar::QFFitFunctionGeneralGaussianDistVar() {
    //           type,         id,                        name,                                                    label (HTML),                      unit,          unitlabel (HTML),        fit,       userEditable, userRangeEditable, displayError, initialFix,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(FloatNumber,  "offset",                  "offset",                                                "Y<sub>0</sub>",                          "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 0.0,          -1e10,    1e10,  1  );
    #define PARAM_OFFSET 0
    addParameter(FloatNumber,  "amplitude",               "amplitude",                                             "A",                       "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 1.0,       -1e10,    1e10,  1  );
    #define PARAM_AMPLITUDE 1
    addParameter(FloatNumber,  "position",                "position",                                              "X<sub>0</sub>",                        "",         "",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 0,            -1e10,    1e10,  1  );
    #define PARAM_POSITION 2
    addParameter(FloatNumber,  "width",                   "1/sqrt(e) width",                                        "&sigma;",                           "",         "",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 1,            -1e10,    1e10,  1  );
    #define PARAM_WIDTH 3

}

double QFFitFunctionGeneralGaussianDistVar::evaluate(double t, const double* data) const {
    const double offset=data[PARAM_OFFSET];
    const double amplitude=data[PARAM_AMPLITUDE];
    const double position=data[PARAM_POSITION];
    const double width=data[PARAM_WIDTH];
    return offset+amplitude*0.5*(1.0+erf((t-position)/sqrt(2.0*qfSqr(width))));
}

void QFFitFunctionGeneralGaussianDistVar::evaluateDerivatives(double* derivatives, double t, const double* data) const {
    for (register int i=0; i<paramCount(); i++) derivatives[i]=0;

}

void QFFitFunctionGeneralGaussianDistVar::calcParameter(double* data, double* error) const {
}

bool QFFitFunctionGeneralGaussianDistVar::isParameterVisible(int parameter, const double* data) const {
    return true;
    // all parameters are visible at all times
}

unsigned int QFFitFunctionGeneralGaussianDistVar::getAdditionalPlotCount(const double* params) {
    return 0;
    // we have one additional plot
}

QString QFFitFunctionGeneralGaussianDistVar::transformParametersForAdditionalPlot(int plot, double* params) {
    return "";
}

bool QFFitFunctionGeneralGaussianDistVar::get_implementsDerivatives()
{
    return false;
}

bool QFFitFunctionGeneralGaussianDistVar::estimateInitial(double *params, const double *dataX, const double *dataY, long N, const bool* fix)
{
    //statisticsMinMax(dataY, N, params[PARAM_BASE], params[PARAM_MAX]);

    if (params && dataX && dataY) {

        StatisticsScopedPointer<double> dX=statisticsDuplicateArray(dataX, N);
        StatisticsScopedPointer<double> dY=statisticsDuplicateArray(dataY, N);

        statisticsSort2(dX.data(), dY.data(), N);

        double mi=0, ma=0;

        statisticsMinMax(dY.data(), N, mi, ma);

        double p=statisticsXatY50Sorted(dX.data(), dY.data(), N);
        double range=(dX[N-1]-dX[0])/5.0;
        if (dY[N-1]-dY[0]<0) {
            range=range*-1.0;
        }

        params[PARAM_OFFSET]=mi;
        params[PARAM_AMPLITUDE]=ma-mi;
        params[PARAM_POSITION]=p;
        params[PARAM_WIDTH]=range/5.0;


    }


    return true;
}
