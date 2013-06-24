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

bool QFFitFunctionGeneralDblExp::estimateInitial(double *params, const double *dataX, const double *dataY, long N)
{
    //statisticsMinMax(dataY, N, params[PARAM_BASE], params[PARAM_MAX]);

    return QFFitFunction::estimateInitial(params, dataX, dataY, N);
}
