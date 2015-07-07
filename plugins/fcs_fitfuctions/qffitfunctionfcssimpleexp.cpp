/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

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

#include "qffitfunctionfcssimpleexp.h"


#define sqr(x) ((x)*(x))
#define cube(x) ((x)*(x)*(x))
#define pow4(x) ((x)*(x)*(x)*(x))
#define pow5(x) ((x)*(x)*(x)*(x)*(x))

#define NAVOGADRO (6.02214179e23)

QFFitFunctionFCSSimpleExp::QFFitFunctionFCSSimpleExp() {
    //           type,         id,                        name,                                                    label,                      unit,          unitlabel,               fit,       userEditable, userRangeEditable, displayError,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(IntCombo,     "n_components",            "number of components",                                  "# components",             "",            "",                      false,     true,         false,             QFFitFunction::NoError,      false, 1,            1,        2,        1,   1,      2);
    #define FCSDiff_n_components 0
    addParameter(FloatNumber,  "pre1",                    "prefactor a1",                                           "a<sub>1</sub>",            "",        "",                          true,      true,         true,              QFFitFunction::DisplayError, false, 1.0,          0,        1e10  );
    #define FCSDiff_pre1 1
    addParameter(FloatNumber,  "tau1",                    "decay time tau1",                                        "&tau;<sub>1</sub>",        "us",      "&mu;s",                     true,      true,         true,              QFFitFunction::DisplayError, false, 10.0,          1e-10,        1e10,  1);
    #define FCSDiff_tau1 2
    addParameter(FloatNumber,  "pre2",                    "prefactor a2",                                           "a<sub>2</sub>",            "",        "",                          true,      true,         true,              QFFitFunction::DisplayError, false, 1.0,          0,        1e10  );
    #define FCSDiff_pre2 3
    addParameter(FloatNumber,  "tau2",                    "decay time tau2",                                        "&tau;<sub>2</sub>",        "us",      "&mu;s",                     true,      true,         true,              QFFitFunction::DisplayError, false, 1000.0,          1e-10,        1e10,  1);
    #define FCSDiff_tau2 4
    addParameter(FloatNumber,  "offset",                  "correlation offset",                                    "G<sub>&infin;</sub>",      "",           "",                       true,      true,         true,              QFFitFunction::DisplayError, true, 0,            -10,      10,       0.1  );
    #define FCSDiff_offset 5
}

double QFFitFunctionFCSSimpleExp::evaluate(double t, const double* data) const {
    const int nonfl_comp=data[FCSDiff_n_components];
    const double pre1=data[FCSDiff_pre1];
    const double tau1=data[FCSDiff_tau1]*1e-6;
    const double pre2=data[FCSDiff_pre2];
    const double tau2=data[FCSDiff_tau2]*1e-6;
    const double offset=data[FCSDiff_offset];

    double pre=0.0;
    if (nonfl_comp>=1) {
        pre=pre+pre1*exp(-t/tau1);
    }
    if (nonfl_comp>=2) {
        pre=pre+pre2*exp(-t/tau2);
    }
    return offset+pre;
}




void QFFitFunctionFCSSimpleExp::calcParameter(double* /*data*/, double* /*error*/) const {

 }

bool QFFitFunctionFCSSimpleExp::isParameterVisible(int parameter, const double* data) const {
    const int nonfl_comp=data[FCSDiff_n_components];
    switch(parameter) {
        case FCSDiff_pre1: case FCSDiff_tau1: return nonfl_comp>0;
        case FCSDiff_pre2: case FCSDiff_tau2: return nonfl_comp>1;
    }
    return true;
}

unsigned int QFFitFunctionFCSSimpleExp::getAdditionalPlotCount(const double* /*params*/) const {
    return 0;
}

QString QFFitFunctionFCSSimpleExp::transformParametersForAdditionalPlot(int /*plot*/, double* /*params*/) const {
    return QString();
}

void QFFitFunctionFCSSimpleExp::sortParameter(double *data, double *error, bool *fix) const
{
    const double tau1=data[FCSDiff_tau1]*1e-6;
    const double tau2=data[FCSDiff_tau2]*1e-6;
    const int nonfl_comp=data[FCSDiff_n_components];
    if (nonfl_comp==2 && tau1>tau2) {
        qSwap(data[FCSDiff_tau1], data[FCSDiff_tau2]);
        qSwap(data[FCSDiff_pre1], data[FCSDiff_pre2]);
        if (error) {
            qSwap(error[FCSDiff_tau1], error[FCSDiff_tau2]);
            qSwap(error[FCSDiff_pre1], error[FCSDiff_pre2]);
        }
        if (fix) {
            qSwap(fix[FCSDiff_tau1], fix[FCSDiff_tau2]);
            qSwap(fix[FCSDiff_pre1], fix[FCSDiff_pre2]);
        }
    }

}
