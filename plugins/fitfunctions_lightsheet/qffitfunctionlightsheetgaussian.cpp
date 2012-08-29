#include "qffitfunctionlightsheetgaussian.h"

#include <cmath>
#define sqr(x) ((x)*(x))
#define cube(x) ((x)*(x)*(x))
#define pow4(x) ((x)*(x)*(x)*(x))
#define pow5(x) ((x)*(x)*(x)*(x)*(x))


QFFitFunctionLightsheetGaussian::QFFitFunctionLightsheetGaussian() {
    //           type,         id,                        name,                                                    label (HTML),                      unit,          unitlabel (HTML),        fit,       userEditable, userRangeEditable, displayError, initialFix,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(FloatNumber,  "offset",                  "offset",                                                "offset",                          "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       true, 0.0,          -1e10,    1e10,  1  );
    #define PARAM_OFFSET 0
    addParameter(FloatNumber,  "amplitude",               "amplitude",                                             "amplitude",                       "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       true, 1000.0,       -1e10,    1e10,  1  );
    #define PARAM_AMPLITUDE 1
    addParameter(FloatNumber,  "position",                "position",                                              "position",                        "pix",         "pix",                   true,      true,         true,              QFFitFunction::DisplayError,       true, 64,            -1e10,    1e10,  1  );
    #define PARAM_POSITION 2
    addParameter(FloatNumber,  "width",                   "width",                                                 "width",                           "pix",         "pix",                   true,      true,         true,              QFFitFunction::DisplayError,       true, 10,            -1e10,    1e10,  1  );
    #define PARAM_WIDTH 3

}

double QFFitFunctionLightsheetGaussian::evaluate(double t, const double* data) const {
    const double offset=data[PARAM_OFFSET];
    const double amplitude=data[PARAM_AMPLITUDE];
    const double position=data[PARAM_POSITION];
    const double width=data[PARAM_WIDTH];
    return offset+amplitude*exp(-2.0*sqr(t-position)/sqr(width));
}

void QFFitFunctionLightsheetGaussian::evaluateDerivatives(double* derivatives, double t, const double* data) const {
    for (register int i=0; i<paramCount(); i++) derivatives[i]=0;

}

void QFFitFunctionLightsheetGaussian::calcParameter(double* data, double* error) const {
}

bool QFFitFunctionLightsheetGaussian::isParameterVisible(int parameter, const double* data) const {
    return true;
	// all parameters are visible at all times
}

unsigned int QFFitFunctionLightsheetGaussian::getAdditionalPlotCount(const double* params) {
    return 0;
	// we have one additional plot
}

QString QFFitFunctionLightsheetGaussian::transformParametersForAdditionalPlot(int plot, double* params) {
    return "";
}
