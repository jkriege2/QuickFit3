#include "qffitfunctiongeneralsine.h"
#include "qfmathtools.h"
#include "statistics_tools.h"
#include <cmath>


QFFitFunctionGeneralSine::QFFitFunctionGeneralSine() {
    //           type,         id,                        name,                                                    label (HTML),                      unit,          unitlabel (HTML),        fit,       userEditable, userRangeEditable, displayError, initialFix,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(FloatNumber,  "offset",                    "offset",                                                  "Y<sub>0</sub>",                   "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 0.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MIN,    DBL_MAX  );
    #define PARAM_OFFSET 0
    addParameter(FloatNumber,  "amplitude",                    "amplitude",                                                  "A",                   "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 1.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MIN,    DBL_MAX  );
    #define PARAM_AMPLITUDE 1
    addParameter(FloatNumber,  "frequency",                    "frequency",                                         "f",                  "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 10,          0, DBL_MAX,  1,   0,    DBL_MAX  );
    #define PARAM_FREQUENCY 2
    addParameter(FloatNumber,  "phase",                    "phase",                                         "&phi;",                  "2*pi",            "2&pi;",                      true,      true,         true,              QFFitFunction::DisplayError,       true, 0,          0, 2.0,  1,   -2.0,    2.0  );
    #define PARAM_PHASE 3

}

double QFFitFunctionGeneralSine::evaluate(double t, const double* data) const {
    return data[PARAM_OFFSET]+data[PARAM_AMPLITUDE]*sin(2.0*M_PI*(data[PARAM_FREQUENCY]*t+data[PARAM_PHASE]));

}


void QFFitFunctionGeneralSine::calcParameter(double* data, double* error) const {

}

bool QFFitFunctionGeneralSine::isParameterVisible(int parameter, const double* data) const {
    return true;
}
unsigned int QFFitFunctionGeneralSine::getAdditionalPlotCount(const double* params) {
    return 0;
}

QString QFFitFunctionGeneralSine::transformParametersForAdditionalPlot(int plot, double* params) {
    return "";
}

bool QFFitFunctionGeneralSine::get_implementsDerivatives()
{
    return false;
}

void QFFitFunctionGeneralSine::evaluateDerivatives(double* derivatives, double t, const double* data) const {
}

bool QFFitFunctionGeneralSine::estimateInitial(double *params, const double *dataX, const double *dataY, long N, const bool *fix)
{
    //statisticsMinMax(dataY, N, params[PARAM_BASE], params[PARAM_MAX]);

    return QFFitFunction::estimateInitial(params, dataX, dataY, N);
}
