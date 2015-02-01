#include "qffitfunctionpsf3dgaussian.h"
#include <cmath>
#include "qftools.h"
#include "statistics_tools.h"

QFFitFunctionPSF3DGaussian::QFFitFunctionPSF3DGaussian()
{
    //           type,         id,                        name,                                                    label (HTML),                      unit,          unitlabel (HTML),        fit,       userEditable, userRangeEditable, displayError, initialFix,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(FloatNumber,  "offset",                  "offset",                                                "offset",                          "",            "",                      true,      true,         true,              QFFitFunctionBase::DisplayError,       false, 0.0,          -1e10,    1e10,  1  );
    #define PARAM_OFFSET 0
    addParameter(FloatNumber,  "amplitude",               "amplitude",                                             "amplitude",                       "",            "",                      true,      true,         true,              QFFitFunctionBase::DisplayError,       false, 1000.0,       0,    1e10,  1  );
    #define PARAM_AMPLITUDE 1
    addParameter(FloatNumber,  "position_x",              "x0",                                                    "x<sub>0</sub>",                   "pix",         "pix",                   true,      true,         true,              QFFitFunctionBase::DisplayError,       false, 0,            -1e10,    1e10,  1  );
    #define PARAM_POSITIONX 2
    addParameter(FloatNumber,  "position_y",              "y0",                                                    "y<sub>0</sub>",                   "pix",         "pix",                   true,      true,         true,              QFFitFunctionBase::DisplayError,       false, 0,            -1e10,    1e10,  1  );
    #define PARAM_POSITIONY 3
    addParameter(FloatNumber,  "position_z",              "z0",                                                    "z<sub>0</sub>",                   "pix",         "pix",                   true,      true,         true,              QFFitFunctionBase::DisplayError,       false, 0,            -1e10,    1e10,  1  );
    #define PARAM_POSITIONZ 4
    addParameter(FloatNumber,  "width1",                  "width1",                                                "width1",                          "pix",         "pix",                   true,      true,         true,              QFFitFunctionBase::DisplayError,       false, 1,            0,    1e10,  1  );
    #define PARAM_WIDTH1 5
    addParameter(FloatNumber,  "width2",                  "width2",                                                "width2",                          "pix",         "pix",                   true,      true,         true,              QFFitFunctionBase::DisplayError,       false, 1,            0,    1e10,  1  );
    #define PARAM_WIDTH2 6
    addParameter(FloatNumber,  "width3",                  "width3",                                                "width3",                          "pix",         "pix",                   true,      true,         true,              QFFitFunctionBase::DisplayError,       false, 5,            0,    1e10,  1  );
    #define PARAM_WIDTH3 7

}

double QFFitFunctionPSF3DGaussian::evaluate(double x, double y, double z, const double *parameters) const
{
    const double offset=parameters[PARAM_OFFSET];
    const double amplitude=parameters[PARAM_AMPLITUDE];
    const double x0=parameters[PARAM_POSITIONX];
    const double y0=parameters[PARAM_POSITIONY];
    const double z0=parameters[PARAM_POSITIONZ];
    const double w1=parameters[PARAM_WIDTH1];
    const double w2=parameters[PARAM_WIDTH2];
    const double w3=parameters[PARAM_WIDTH3];
    return offset+amplitude*exp(-2.0*qfSqr(x-x0)/qfSqr(w1)-2.0*qfSqr(y-y0)/qfSqr(w2)-2.0*qfSqr(z-z0)/qfSqr(w3));
}
