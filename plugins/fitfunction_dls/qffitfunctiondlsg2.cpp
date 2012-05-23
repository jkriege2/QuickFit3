#include "qffitfunctiondlsg2.h"

#include <cmath>
#include "qfmathtools.h"
#define sqr(x) ((x)*(x))
#define cube(x) ((x)*(x)*(x))
#define pow4(x) ((x)*(x)*(x)*(x))
#define pow5(x) ((x)*(x)*(x)*(x)*(x))


QFFitFunctionDLSG2::QFFitFunctionDLSG2() {
    //           type,         id,                        name,                                                    label,                      unit,          unitlabel,               fit,       userEditable, userRangeEditable, displayError,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(IntCombo,     "n_components",            "number of diffusing components",                        "components",               "",            "",                      false,     true,         false,             QFFitFunction::NoError,      1,            1,        3,        1,   1,      2);
    #define DLSG2_n_components 0
    addParameter(FloatNumber,  "epsilon",                    "baseline",                                              "&epsilon;",                   "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, 0,            -1e50,    1e50,     0.01);
    #define DLSG2_epsilon 1

    addParameter(FloatNumber,  "frac1",                   "fraction of component 1",                               "&alpha;<sub>1</sub>",      "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, 1,            0,        1e10,     0.1, 0 );
    #define DLSG2_frac1 2
    addParameter(FloatNumber,  "tau1",                    "correlation time 1",                                    "&tau;<sub>1</sub>",        "µs",         "&mu;s",                  true,      true,         true,              QFFitFunction::DisplayError, 100,          1e-10,    1e50,     1    );
    #define DLSG2_tau1 3
    addParameter(FloatNumber,  "frac2",                   "fraction of component 2",                               "&alpha;<sub>2</sub>",      "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, 1,            0,        1e10,     0.1, 0);
    #define DLSG2_frac2 4
    addParameter(FloatNumber,  "tau2",                    "correlation time 2",                                    "&tau;<sub>2</sub>",        "µs",         "&mu;s",                  true,      true,         true,              QFFitFunction::DisplayError, 1000,         1e-10,    1e50,     1    );
    #define DLSG2_tau2 5
    addParameter(FloatNumber,  "frac3",                   "fraction of component 3",                               "&alpha;<sub>3</sub>",      "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, 1,            0,        1e10,     0.1, 0);
    #define DLSG2_frac3 6
    addParameter(FloatNumber,  "tau3",                    "correlation time 3",                                    "&tau;<sub>3</sub>",        "µs",         "&mu;s",                  true,      true,         true,              QFFitFunction::DisplayError, 10000,        1e-10,    1e50,     1    );
    #define DLSG2_tau3 7

    addParameter(FloatNumber,  "offset",                  "correlation offset",                                    "G<sub>&infin;</sub>",      "",           "",                       true,      true,         true,              QFFitFunction::DisplayError, 1,            -10,      10,       0.1  );
    #define DLSG2_offset 8
}

double QFFitFunctionDLSG2::evaluate(double t, const double* data) const {
    const int comp=data[DLSG2_n_components];
    const double epsilon=data[DLSG2_epsilon];
    const double tau1=data[DLSG2_tau1]/1e6;
    const double frac1=data[DLSG2_frac1];
    const double tau2=data[DLSG2_tau2]/1e6;
    const double frac2=data[DLSG2_frac2];
    const double tau3=data[DLSG2_tau3]/1e6;
    const double frac3=data[DLSG2_frac3];
    const double offset=data[DLSG2_offset];

    double g1=frac1*exp(-t/tau1);
    if (comp>1) g1+=frac2*exp(-t/tau2);
    if (comp>2) g1+=frac3*exp(-t/tau3);

    return offset+epsilon+sqr(g1);
}


void QFFitFunctionDLSG2::calcParameter(double* data, double* error) const {
}

bool QFFitFunctionDLSG2::isParameterVisible(int parameter, const double* data) const {
    int comp=data[DLSG2_n_components];
    switch(parameter) {
        case DLSG2_frac2:  case DLSG2_tau2: return comp>1;
        case DLSG2_frac3:  case DLSG2_tau3: return comp>2;
    }
    return true;
}


