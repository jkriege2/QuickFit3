#include "qffitfunctiongeneralgaussianvar.h"
#include "qfmathtools.h"
#include <cmath>

QFFitFunctionGeneralGaussianVar::QFFitFunctionGeneralGaussianVar() {
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

double QFFitFunctionGeneralGaussianVar::evaluate(double t, const double* data) const {
    const double offset=data[PARAM_OFFSET];
    const double amplitude=data[PARAM_AMPLITUDE];
    const double position=data[PARAM_POSITION];
    const double width=data[PARAM_WIDTH];
    return offset+amplitude*exp(-0.5*qfSqr(t-position)/qfSqr(width));
}

void QFFitFunctionGeneralGaussianVar::evaluateDerivatives(double* derivatives, double t, const double* data) const {
    for (register int i=0; i<paramCount(); i++) derivatives[i]=0;

}

void QFFitFunctionGeneralGaussianVar::calcParameter(double* data, double* error) const {
}

bool QFFitFunctionGeneralGaussianVar::isParameterVisible(int parameter, const double* data) const {
    return true;
    // all parameters are visible at all times
}

unsigned int QFFitFunctionGeneralGaussianVar::getAdditionalPlotCount(const double* params) {
    return 0;
    // we have one additional plot
}

QString QFFitFunctionGeneralGaussianVar::transformParametersForAdditionalPlot(int plot, double* params) {
    return "";
}

bool QFFitFunctionGeneralGaussianVar::get_implementsDerivatives()
{
    return false;
}
