#include "qffitfunctiongeneralsegmentedline.h"
#include "qfmathtools.h"
#include "statistics_tools.h"
#include <cmath>


QFFitFunctionGeneralSegmentedLine::QFFitFunctionGeneralSegmentedLine() {
    //           type,         id,                        name,                                                    label (HTML),                      unit,          unitlabel (HTML),        fit,       userEditable, userRangeEditable, displayError, initialFix,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(IntCombo,  "segments",                   "segments",                                                "segments",                   "",            "",                      false,      true,         false,              QFFitFunction::NoError,       false, 2,          2, 3,  1,   1,    10  );
    #define PARAM_SEGMENTS 0
    addParameter(FloatNumber,  "offset",                  "offset 1",                                                "a<sub>1</sub>",               "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 0.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_OFFSET1 1
    addParameter(FloatNumber,  "slope",                   "slope 1",                                                 "b<sub>1</sub>",               "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 1.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_SLOPE1 2
    addParameter(FloatNumber,  "intersection12",          "intersection x2",                                         "x<sub>2</sub>",               "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 0.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_INTERSECT2 3
    addParameter(FloatNumber,  "offset2",                 "offset 2",                                                "a<sub>2</sub>",               "",            "",                      false,     false,        false,             QFFitFunction::DisplayError,       false, 0.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_OFFSET2 4
    addParameter(FloatNumber,  "slope2",                  "slope 2",                                                 "b<sub>2</sub>",               "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 2.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_SLOPE2 5
    addParameter(FloatNumber,  "intersection23",          "intersection x3",                                         "x<sub>3</sub>",               "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 0.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_INTERSECT3 6
    addParameter(FloatNumber,  "offset3",                 "offset 3",                                                "a<sub>3</sub>",               "",            "",                      false,     false,        false,             QFFitFunction::DisplayError,       false, 0.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_OFFSET3 7
    addParameter(FloatNumber,  "slope3",                  "slope 3",                                                 "b<sub>3</sub>",               "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 2.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_SLOPE3 8

}

double QFFitFunctionGeneralSegmentedLine::evaluate(double t, const double* data) const {
    const int segs=data[PARAM_SEGMENTS];
    if (t<data[PARAM_INTERSECT2] || segs<=1) return data[PARAM_OFFSET1]+data[PARAM_SLOPE1]*t;
    else {
        const double a2=(data[PARAM_SLOPE1]-data[PARAM_SLOPE2])*data[PARAM_INTERSECT2]+data[PARAM_OFFSET1];
        if (t<data[PARAM_INTERSECT3] || segs<=2) {
            return a2+data[PARAM_SLOPE2]*t;
        } else {
            const double a3=(data[PARAM_SLOPE2]-data[PARAM_SLOPE3])*data[PARAM_INTERSECT3]+a2;
            return a3+data[PARAM_SLOPE3]*t;
        }
    }

}


void QFFitFunctionGeneralSegmentedLine::calcParameter(double* data, double* error) const {
    const int segs=data[PARAM_SEGMENTS];
    const double b1=data[PARAM_SLOPE1];
    const double b2=data[PARAM_SLOPE2];
    const double x2=data[PARAM_INTERSECT2];
    const double a1=data[PARAM_OFFSET1];
    const double b3=data[PARAM_SLOPE3];
    const double x3=data[PARAM_INTERSECT3];

    const double a2=(b1-b2)*x2+a1;
    data[PARAM_OFFSET2]=a2;
    if (segs>=3) {
        data[PARAM_OFFSET3]=(b2-b3)*x3+a2;
    }
    if (error) {
        const double eb1=error[PARAM_SLOPE1];
        const double eb2=error[PARAM_SLOPE2];
        const double ex2=error[PARAM_INTERSECT2];
        const double ea1=error[PARAM_OFFSET1];
        const double eb3=error[PARAM_SLOPE3];
        const double ex3=error[PARAM_INTERSECT3];
        const double ea2=qfErrorSqrtSumSqr(eb1*x2, eb2*x2, ea1, (b1-b2)*ex2);
        error[PARAM_OFFSET2]=ea2;
        if (segs>=3) {
            error[PARAM_OFFSET3]=qfErrorSqrtSumSqr(eb2*x3, eb3*x3, ea2, (b2-b3)*ex3);
        }
    }
}

bool QFFitFunctionGeneralSegmentedLine::isParameterVisible(int parameter, const double* data) const {
    const int segs=data[PARAM_SEGMENTS];
    switch(parameter) {
        case PARAM_SLOPE2: case PARAM_INTERSECT2: case PARAM_OFFSET2: return segs>=2;
        case PARAM_SLOPE3: case PARAM_INTERSECT3: case PARAM_OFFSET3: return segs>=3;
        default: return true;
    }
    return true;
}
unsigned int QFFitFunctionGeneralSegmentedLine::getAdditionalPlotCount(const double* params) {
    return 0;
}

QString QFFitFunctionGeneralSegmentedLine::transformParametersForAdditionalPlot(int plot, double* params) {
    return "";
}

bool QFFitFunctionGeneralSegmentedLine::get_implementsDerivatives()
{
    return false;
}

void QFFitFunctionGeneralSegmentedLine::evaluateDerivatives(double* derivatives, double t, const double* data) const {
}

bool QFFitFunctionGeneralSegmentedLine::estimateInitial(double *params, const double *dataX, const double *dataY, long N, const bool* fix)
{
    return false;
}
