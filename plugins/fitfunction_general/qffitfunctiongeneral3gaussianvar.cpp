/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg



    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "qffitfunctiongeneral3gaussianvar.h"
#include "qfmathtools.h"
#include <cmath>
#include "statistics_tools.h"

QFFitFunctionGeneral3GaussianVar::QFFitFunctionGeneral3GaussianVar() {
    //           type,         id,                        name,                                                    label (HTML),                      unit,          unitlabel (HTML),        fit,       userEditable, userRangeEditable, displayError, initialFix,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(FloatNumber,  "offset",                  "offset",                                                "Y<sub>0</sub>",                          "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 0.0,          -1e10,    1e10,  1  );
    #define PARAM_OFFSET 0
    addParameter(FloatNumber,  "amplitude",               "amplitude",                                             "A<sub>1</sub>",                       "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 1.0,       -1e10,    1e10,  1  );
    #define PARAM_AMPLITUDE 1
    addParameter(FloatNumber,  "position",                "position",                                              "X<sub>1</sub>",                        "",         "",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 0,            -1e10,    1e10,  1  );
    #define PARAM_POSITION 2
    addParameter(FloatNumber,  "width",                   "1/sqrt(e) width",                                        "&sigma;<sub>1</sub>",                           "",         "",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 1,            1e-10,    1e10,  1  );
    #define PARAM_WIDTH 3
    addParameter(FloatNumber,  "amplitude2",               "amplitude 2",                                             "A<sub>2</sub>",                       "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 0.5,       -1e10,    1e10,  1  );
    #define PARAM_AMPLITUDE2 4
    addParameter(FloatNumber,  "position2",                "position 2",                                              "X<sub>2</sub>",                        "",         "",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 2,            -1e10,    1e10,  1  );
    #define PARAM_POSITION2 5
    addParameter(FloatNumber,  "width2",                   "1/sqrt(e) width 2",                                        "&sigma;<sub>2</sub>",                           "",         "",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 1,            1e-10,    1e10,  1  );
    #define PARAM_WIDTH2 6
    addParameter(FloatNumber,  "amplitude3",               "amplitude 3",                                             "A<sub>3</sub>",                       "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 0.25,       -1e10,    1e10,  1  );
    #define PARAM_AMPLITUDE3 7
    addParameter(FloatNumber,  "position3",                "position 3",                                              "X<sub>3</sub>",                        "",         "",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 4,            -1e10,    1e10,  1  );
    #define PARAM_POSITION3 8
    addParameter(FloatNumber,  "width3",                   "1/sqrt(e) width 3",                                        "&sigma;<sub>3</sub>",                           "",         "",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 1,            -1e10,    1e10,  1  );
    #define PARAM_WIDTH3 9

}

double QFFitFunctionGeneral3GaussianVar::evaluate(double t, const double* data) const {
    return data[PARAM_OFFSET]+data[PARAM_AMPLITUDE]*exp(-0.5*qfSqr(t-data[PARAM_POSITION])/qfSqr(data[PARAM_WIDTH]))
                             +data[PARAM_AMPLITUDE2]*exp(-0.5*qfSqr(t-data[PARAM_POSITION2])/qfSqr(data[PARAM_WIDTH2]))
                             +data[PARAM_AMPLITUDE3]*exp(-0.5*qfSqr(t-data[PARAM_POSITION3])/qfSqr(data[PARAM_WIDTH3]));
}



bool QFFitFunctionGeneral3GaussianVar::isParameterVisible(int parameter, const double* data) const {

    Q_UNUSED(parameter);
    Q_UNUSED(data);
    return true;
    // all parameters are visible at all times
}

unsigned int QFFitFunctionGeneral3GaussianVar::getAdditionalPlotCount(const double* /*params*/) const {
    return 3;
    // we have one additional plot
}

QString QFFitFunctionGeneral3GaussianVar::transformParametersForAdditionalPlot(int plot, double* params) const {
    if (plot==0) {
        params[PARAM_AMPLITUDE2]=0;
        return QObject::tr("Gauss 1");
    } else if (plot==1) {
        params[PARAM_AMPLITUDE]=0;
        return QObject::tr("Gauss 2");
    } else if (plot==2) {
        params[PARAM_AMPLITUDE]=0;
        return QObject::tr("Gauss 3");
    }
    return "";
}

bool QFFitFunctionGeneral3GaussianVar::get_implementsDerivatives() const
{
    return false;
}


