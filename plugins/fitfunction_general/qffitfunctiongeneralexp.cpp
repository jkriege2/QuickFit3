#include "qffitfunctiongeneralexp.h"
#include "qfmathtools.h"
#include "statistics_tools.h"
#include <cmath>


QFFitFunctionGeneralExp::QFFitFunctionGeneralExp() {
    //           type,         id,                        name,                                                    label (HTML),                      unit,          unitlabel (HTML),        fit,       userEditable, userRangeEditable, displayError, initialFix,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(FloatNumber,  "offset",                    "offset",                                              "Y<sub>0</sub>",                   "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       true, 0.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MIN,    DBL_MAX  );
    #define PARAM_OFFSET 0
    addParameter(FloatNumber,  "amplitude",                    "amplitude",                                                  "A",                     "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 1.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MIN,    DBL_MAX  );
    #define PARAM_AMPLITUDE 1
    addParameter(FloatNumber,  "position",                    "position",                                           "X<sub>0</sub>",                  "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 0.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MIN,    DBL_MAX  );
    #define PARAM_POSITION 2
    addParameter(FloatNumber,  "lifetime",                    "lifetime",                                           "&tau;",                          "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 1.0,          0, DBL_MAX,  1,   -DBL_MIN,    DBL_MAX  );
    #define PARAM_LIFETIME 3

}

double QFFitFunctionGeneralExp::evaluate(double t, const double* data) const {
    return data[PARAM_OFFSET]+data[PARAM_AMPLITUDE]*exp(-(t-data[PARAM_POSITION])/data[PARAM_LIFETIME]);

}


void QFFitFunctionGeneralExp::calcParameter(double* data, double* error) const {

}

bool QFFitFunctionGeneralExp::isParameterVisible(int parameter, const double* data) const {
    return true;
}
unsigned int QFFitFunctionGeneralExp::getAdditionalPlotCount(const double* params) {
    return 0;
}

QString QFFitFunctionGeneralExp::transformParametersForAdditionalPlot(int plot, double* params) {
    return "";
}

bool QFFitFunctionGeneralExp::get_implementsDerivatives()
{
    return false;
}

void QFFitFunctionGeneralExp::evaluateDerivatives(double* derivatives, double t, const double* data) const {
}


bool QFFitFunctionGeneralExp::estimateInitial(double *params, const double *dataX, const double *dataY, long N, const bool* fix)
{
    //statisticsMinMax(dataY, N, params[PARAM_BASE], params[PARAM_MAX]);
    if (params && dataX && dataY) {

        StatisticsScopedPointer<double> dY=statisticsDuplicateAndApply(dataY, N, log);


        double a=log(params[PARAM_AMPLITUDE]);
        double b=-1.0/params[PARAM_LIFETIME];


        if (fix) statisticsLinearRegression(dataX, dY.data(), N, a, b, fix[PARAM_AMPLITUDE], fix[PARAM_LIFETIME]);
        else statisticsLinearRegression(dataX, dY.data(), N, a, b, false, false);

        params[PARAM_OFFSET]=0;
        params[PARAM_POSITION]=0;
        params[PARAM_AMPLITUDE]=exp(a);
        params[PARAM_LIFETIME]=-1.0/b;
    }

    return true;

}
