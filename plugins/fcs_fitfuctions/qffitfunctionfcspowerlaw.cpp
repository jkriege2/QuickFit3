#include "qffitfunctionfcspowerlaw.h"


#define sqr(x) ((x)*(x))
#define cube(x) ((x)*(x)*(x))
#define pow4(x) ((x)*(x)*(x)*(x))
#define pow5(x) ((x)*(x)*(x)*(x)*(x))

#define NAVOGADRO (6.02214179e23)

QFFitFunctionFCSPowerLaw::QFFitFunctionFCSPowerLaw() {
    //           type,         id,                        name,                                                    label,                      unit,          unitlabel,               fit,       userEditable, userRangeEditable, displayError,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(IntCombo,     "n_components",            "number of components",                                  "# components",             "",            "",                      false,     true,         false,             QFFitFunction::NoError,      false, 1,            1,        2,        1,   1,      2);
    #define FCSDiff_n_components 0
    addParameter(FloatNumber,  "pre1",                    "prefactor a1",                                           "a<sub>1</sub>",            "",        "",                          true,      true,         true,              QFFitFunction::DisplayError, false, 1.0,          0,        1e10  );
    #define FCSDiff_pre1 1
    addParameter(FloatNumber,  "exponent1",               "exponent b1",                                            "b<sub>1</sub>",            "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 1.0,          -1e10,        1e10,  1);
    #define FCSDiff_exp1 2
    addParameter(FloatNumber,  "pre2",                    "prefactor a2",                                           "a<sub>2</sub>",            "",        "",                          true,      true,         true,              QFFitFunction::DisplayError, false, 1.0,          0,        1e10  );
    #define FCSDiff_pre2 3
    addParameter(FloatNumber,  "exponent2",               "exponent b2",                                            "b<sub>2</sub>",            "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 1.0,          -1e10,        1e10,  1);
    #define FCSDiff_exp2 4
    addParameter(FloatNumber,  "offset",                  "correlation offset",                                    "G<sub>&infin;</sub>",      "",           "",                       true,      true,         true,              QFFitFunction::DisplayError, true, 0,            -10,      10,       0.1  );
    #define FCSDiff_offset 5
}

double QFFitFunctionFCSPowerLaw::evaluate(double t, const double* data) const {
    const int nonfl_comp=data[FCSDiff_n_components];
    const double pre1=data[FCSDiff_pre1];
    const double exp1=data[FCSDiff_exp1];
    const double pre2=data[FCSDiff_pre2];
    const double exp2=data[FCSDiff_exp2];
    const double offset=data[FCSDiff_offset];

    double pre=0.0;
    if (nonfl_comp>=1) {
        pre=pre+pre1*pow(t, -exp1);
    }
    if (nonfl_comp>=2) {
        pre=pre+pre2*pow(t, -exp2);
    }
    return offset+pre;
}




void QFFitFunctionFCSPowerLaw::calcParameter(double* data, double* error) const {

 }

bool QFFitFunctionFCSPowerLaw::isParameterVisible(int parameter, const double* data) const {
    const int nonfl_comp=data[FCSDiff_n_components];
    switch(parameter) {
        case FCSDiff_pre1: case FCSDiff_exp1: return nonfl_comp>0;
        case FCSDiff_pre2: case FCSDiff_exp2: return nonfl_comp>1;
    }
    return true;
}

unsigned int QFFitFunctionFCSPowerLaw::getAdditionalPlotCount(const double* params) {
    return 0;
}

QString QFFitFunctionFCSPowerLaw::transformParametersForAdditionalPlot(int plot, double* params) {
    return QString();
}
