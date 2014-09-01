#include "qffitfunctiongeneralskewgaussianvar.h"
#include "qfmathtools.h"
#include <cmath>
#include "statistics_tools.h"

QFFitFunctionGeneralSkewGaussianVar::QFFitFunctionGeneralSkewGaussianVar() {
    //           type,         id,                        name,                                                    label (HTML),                      unit,          unitlabel (HTML),        fit,       userEditable, userRangeEditable, displayError, initialFix,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(FloatNumber,  "offset",                  "offset",                                                "Y<sub>0</sub>",                          "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 0.0,          -1e10,    1e10,  1  );
    #define PARAM_OFFSET 0
    addParameter(FloatNumber,  "amplitude",               "amplitude",                                             "A",                       "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 1.0,       -1e10,    1e10,  1  );
    #define PARAM_AMPLITUDE 1
    addParameter(FloatNumber,  "position",                "position",                                              "X<sub>0</sub>",                        "",         "",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 0,            -1e10,    1e10,  1  );
    #define PARAM_POSITION 2
    addParameter(FloatNumber,  "width",                   "1/sqrt(e) width",                                       "&sigma;",                           "",         "",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 1,            1e-10,    1e10,  1  );
    #define PARAM_WIDTH 3
    addParameter(FloatNumber,  "skewness",                "skewness",                                              "&alpha;",                           "",         "",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 0,            -1e10,    1e10,  1  );
    #define PARAM_SKEW 4

}

double QFFitFunctionGeneralSkewGaussianVar::evaluate(double t, const double* data) const {
    const double offset=data[PARAM_OFFSET];
    const double amplitude=data[PARAM_AMPLITUDE];
    const double position=data[PARAM_POSITION];
    const double width=data[PARAM_WIDTH];
    const double alpha=data[PARAM_SKEW];
    const double x=(t-position)/width;
    return offset+amplitude*exp(-0.5*qfSqr(x))*(1+erf(alpha*x/sqrt(2)));
}

void QFFitFunctionGeneralSkewGaussianVar::evaluateDerivatives(double* derivatives, double t, const double* data) const {
    for (register int i=0; i<paramCount(); i++) derivatives[i]=0;

}

void QFFitFunctionGeneralSkewGaussianVar::calcParameter(double* data, double* error) const {
}

bool QFFitFunctionGeneralSkewGaussianVar::isParameterVisible(int parameter, const double* data) const {
    return true;
    // all parameters are visible at all times
}

unsigned int QFFitFunctionGeneralSkewGaussianVar::getAdditionalPlotCount(const double* params) {
    return 0;
    // we have one additional plot
}

QString QFFitFunctionGeneralSkewGaussianVar::transformParametersForAdditionalPlot(int plot, double* params) {
    return "";
}

bool QFFitFunctionGeneralSkewGaussianVar::get_implementsDerivatives()
{
    return false;
}

bool QFFitFunctionGeneralSkewGaussianVar::estimateInitial(double *params, const double *dataX, const double *dataY, long N, const bool* fix)
{
    //statisticsMinMax(dataY, N, params[PARAM_BASE], params[PARAM_MAX]);
    if (params && dataX && dataY) {
        double pW=0;
        double pB=0;
        double pH=0;
        double pP=statisticsPeakFind(pW, dataX, dataY, N, 0.0, double(NAN), &pB, &pH);
        if (statisticsFloatIsOK(pP)) {
            params[PARAM_OFFSET]=pB;
            params[PARAM_AMPLITUDE]=pH;
            params[PARAM_POSITION]=pP;
            params[PARAM_WIDTH]=pW/2.3548;
            params[PARAM_SKEW]=0;
            return true;
        } else {
            return false;
        }
        return true;
    }

    return true;
}
