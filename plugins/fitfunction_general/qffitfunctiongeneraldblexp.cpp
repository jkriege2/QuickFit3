#include "qffitfunctiongeneraldblexp.h"
#include "qfmathtools.h"
#include "statistics_tools.h"
#include <cmath>


QFFitFunctionGeneralDblExp::QFFitFunctionGeneralDblExp() {
    //           type,         id,                        name,                                                    label (HTML),                      unit,          unitlabel (HTML),        fit,       userEditable, userRangeEditable, displayError, initialFix,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(FloatNumber,  "offset",                    "offset",                                              "Y<sub>0</sub>",                   "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       true, 0.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MIN,    DBL_MAX  );
    #define PARAM_OFFSET 0
    addParameter(FloatNumber,  "amplitude",                    "amplitude",                                                  "A<sub>1</sub>",                     "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 1.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MIN,    DBL_MAX  );
    #define PARAM_AMPLITUDE 1
    addParameter(FloatNumber,  "position",                    "position",                                           "X<sub>0</sub>",                  "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       true, 0.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MIN,    DBL_MAX  );
    #define PARAM_POSITION 2
    addParameter(FloatNumber,  "lifetime",                    "lifetime",                                           "&tau;<sub>1</sub>",                          "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 1.0,          0, DBL_MAX,  1,   -DBL_MIN,    DBL_MAX  );
    #define PARAM_LIFETIME 3
    addParameter(FloatNumber,  "amplitude2",                    "amplitude2",                                                  "A<sub>2</sub>",                     "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 1.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MIN,    DBL_MAX  );
    #define PARAM_AMPLITUDE2 4
    addParameter(FloatNumber,  "lifetime2",                    "lifetime2",                                           "&tau;<sub>2</sub>",                          "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 100.0,          0, DBL_MAX,  1,   -DBL_MIN,    DBL_MAX  );
    #define PARAM_LIFETIME2 5


}

double QFFitFunctionGeneralDblExp::evaluate(double t, const double* data) const {
    return data[PARAM_OFFSET]+data[PARAM_AMPLITUDE]*exp(-(t-data[PARAM_POSITION])/data[PARAM_LIFETIME])+data[PARAM_AMPLITUDE2]*exp(-(t-data[PARAM_POSITION])/data[PARAM_LIFETIME2]);

}


void QFFitFunctionGeneralDblExp::calcParameter(double* data, double* error) const {

}

bool QFFitFunctionGeneralDblExp::isParameterVisible(int parameter, const double* data) const {
    return true;
}
unsigned int QFFitFunctionGeneralDblExp::getAdditionalPlotCount(const double* params) {
    return 0;
}

QString QFFitFunctionGeneralDblExp::transformParametersForAdditionalPlot(int plot, double* params) {
    return "";
}

bool QFFitFunctionGeneralDblExp::get_implementsDerivatives()
{
    return false;
}

void QFFitFunctionGeneralDblExp::evaluateDerivatives(double* derivatives, double t, const double* data) const {
}


bool QFFitFunctionGeneralDblExp::estimateInitial(double *params, const double *dataX, const double *dataY, long N, const bool* fix)
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
        params[PARAM_AMPLITUDE2]=params[PARAM_AMPLITUDE]=exp(a);
        params[PARAM_LIFETIME2]=params[PARAM_LIFETIME]=-1.0/b;
    }

    return true;

}


