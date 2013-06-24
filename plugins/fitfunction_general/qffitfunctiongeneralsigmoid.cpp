#include "qffitfunctiongeneralsigmoid.h"
#include "qfmathtools.h"
#include "statistics_tools.h"
#include <cmath>


QFFitFunctionGeneralSigmoid::QFFitFunctionGeneralSigmoid() {
    //           type,         id,                        name,                                                    label (HTML),                      unit,          unitlabel (HTML),        fit,       userEditable, userRangeEditable, displayError, initialFix,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(FloatNumber,  "offset",                    "offset",                                                  "Y<sub>0</sub>",                   "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 0.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MIN,    DBL_MAX  );
    #define PARAM_BASE 0
    addParameter(FloatNumber,  "max",                    "max",                                                  "Y<sub>max</sub>",                   "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 1.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MIN,    DBL_MAX  );
    #define PARAM_MAX 1
    addParameter(FloatNumber,  "position",                    "position",                                         "X<sub>1/2</sub>",                  "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 0.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MIN,    DBL_MAX  );
    #define PARAM_POSITION 2
    addParameter(FloatNumber,  "rate",                    "rate",                                                               "w",                  "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 1.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MIN,    DBL_MAX  );
    #define PARAM_RATE 3

}

double QFFitFunctionGeneralSigmoid::evaluate(double t, const double* data) const {
    return data[PARAM_BASE]+(data[PARAM_MAX]-data[PARAM_BASE])/(1.0+exp((data[PARAM_POSITION]-t)/data[PARAM_RATE]));

}


void QFFitFunctionGeneralSigmoid::calcParameter(double* data, double* error) const {

}

bool QFFitFunctionGeneralSigmoid::isParameterVisible(int parameter, const double* data) const {
    return true;
}
unsigned int QFFitFunctionGeneralSigmoid::getAdditionalPlotCount(const double* params) {
    return 0;
}

QString QFFitFunctionGeneralSigmoid::transformParametersForAdditionalPlot(int plot, double* params) {
    return "";
}

bool QFFitFunctionGeneralSigmoid::get_implementsDerivatives()
{
    return false;
}

void QFFitFunctionGeneralSigmoid::evaluateDerivatives(double* derivatives, double t, const double* data) const {
}

bool QFFitFunctionGeneralSigmoid::estimateInitial(double *params, const double *dataX, const double *dataY, long N)
{
    //statisticsMinMax(dataY, N, params[PARAM_BASE], params[PARAM_MAX]);

    return QFFitFunction::estimateInitial(params, dataX, dataY, N);
}
