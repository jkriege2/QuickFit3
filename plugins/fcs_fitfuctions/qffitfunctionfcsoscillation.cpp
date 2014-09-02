/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "qffitfunctionfcsoscillation.h"
#define sqr(x) ((x)*(x))
#define cube(x) ((x)*(x)*(x))
#define pow4(x) ((x)*(x)*(x)*(x))
#define pow5(x) ((x)*(x)*(x)*(x)*(x))


QFFitFunctionFCSOscillation::QFFitFunctionFCSOscillation() {
    //           type,         id,                        name,                                                    label,                      unit,          unitlabel,               fit,       userEditable, userRangeEditable, displayError,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(FloatNumber,  "amplitude",               "amplitude of oscillation",                              "A",                        "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 1,            1e-10,    1e10,      0.1, 0);
    #define FCSOSC_AMPLITUDE 0
    addParameter(FloatNumber,  "frequency",               "frequency of oscillation",                              "f",                        "Hz",          "Hz",                    true,      true,         true,              QFFitFunction::DisplayError, false, 50,           1e-10,    1e10,      1,   0);
    #define FCSOSC_freq 1
    addParameter(FloatNumber,  "period",                  "oscillation period",                                    "T",                        "ms",          "ms",                    false,    false,        false,              QFFitFunction::DisplayError, false, (1000.0/50.0),1e-20,    1e20,      1    );
    #define FCSOSC_period 2
    addParameter(FloatNumber,  "offset",                  "correlation offset",                                    "G<sub>&infin;</sub>",      "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 1,            -10,      10,        0.1  );
    #define FCSOSC_offset 3

}

QFFitFunctionFCSOscillation::~QFFitFunctionFCSOscillation() {
}

double QFFitFunctionFCSOscillation::evaluate(double x, const double *parameters) const {
    double A=parameters[FCSOSC_AMPLITUDE];
    double f=parameters[FCSOSC_freq];
    double Ginf=parameters[FCSOSC_offset];

    return Ginf+A*cos(2.0*M_PI*f*x);
}

void QFFitFunctionFCSOscillation::calcParameter(double *parameterValues, double *error) const {
    double f=parameterValues[FCSOSC_freq];
    parameterValues[FCSOSC_period]=1000.0/f;
    if (error) error[FCSOSC_period]=fabs(1000.0/sqr(f)*error[FCSOSC_freq]);
}

bool QFFitFunctionFCSOscillation::isParameterVisible(int parameter, const double *parameterValues) const {
    return true;
}

unsigned int QFFitFunctionFCSOscillation::getAdditionalPlotCount(const double *params) {
    return 0;
}

QString QFFitFunctionFCSOscillation::transformParametersForAdditionalPlot(int plot, double *params) {
    return QString("");
}
