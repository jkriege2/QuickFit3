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


#include "qffitfunctiongeneraldblexp.h"
#include "qfmathtools.h"
#include "statistics_tools.h"
#include <cmath>


QFFitFunctionGeneralDblExp::QFFitFunctionGeneralDblExp() {
    //           type,         id,                        name,                                                    label (HTML),                      unit,          unitlabel (HTML),        fit,       userEditable, userRangeEditable, displayError, initialFix,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(FloatNumber,  "offset",                    "offset",                                              "Y<sub>0</sub>",                   "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       true, 0.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_OFFSET 0
    addParameter(FloatNumber,  "amplitude",                    "amplitude",                                                  "A<sub>1</sub>",                     "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 1.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_AMPLITUDE 1
    addParameter(FloatNumber,  "position",                    "position",                                           "X<sub>0</sub>",                  "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 0.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_POSITION 2
    addParameter(FloatNumber,  "lifetime",                    "lifetime",                                           "&tau;<sub>1</sub>",                          "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 1.0,          0, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_LIFETIME 3
    addParameter(FloatNumber,  "amplitude2",                    "amplitude2",                                                  "A<sub>2</sub>",                     "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 1.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_AMPLITUDE2 4
    addParameter(FloatNumber,  "lifetime2",                    "lifetime2",                                           "&tau;<sub>2</sub>",                          "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 100.0,          0, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_LIFETIME2 5


}

double QFFitFunctionGeneralDblExp::evaluate(double t, const double* data) const {
    return data[PARAM_OFFSET]+data[PARAM_AMPLITUDE]*exp(-(t-data[PARAM_POSITION])/data[PARAM_LIFETIME])+data[PARAM_AMPLITUDE2]*exp(-(t-data[PARAM_POSITION])/data[PARAM_LIFETIME2]);

}



bool QFFitFunctionGeneralDblExp::isParameterVisible(int parameter, const double* data) const {

    Q_UNUSED(parameter);
    Q_UNUSED(data);
    return true;
}
unsigned int QFFitFunctionGeneralDblExp::getAdditionalPlotCount(const double* params) const {

    Q_UNUSED(params);
    return 0;
}

QString QFFitFunctionGeneralDblExp::transformParametersForAdditionalPlot(int plot, double* params) const {


    Q_UNUSED(plot);
    Q_UNUSED(params);
    return "";
}

bool QFFitFunctionGeneralDblExp::get_implementsDerivatives() const
{
    return false;
}





