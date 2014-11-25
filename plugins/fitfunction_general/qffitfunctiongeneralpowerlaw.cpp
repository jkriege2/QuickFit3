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

#include "qffitfunctiongeneralpowerlaw.h"
#include "qfmathtools.h"
#include "statistics_tools.h"
#include <cmath>


QFFitFunctionGeneralPowerLaw::QFFitFunctionGeneralPowerLaw() {
    //           type,         id,                        name,                                                    label (HTML),                      unit,          unitlabel (HTML),        fit,       userEditable, userRangeEditable, displayError, initialFix,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(FloatNumber,  "offset",                    "offset",                                                  "Y<sub>0</sub>",                   "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 0.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_OFFSET 0
    addParameter(FloatNumber,  "amplitude",                    "amplitude",                                                  "A",                   "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 1.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_AMPLITUDE 1
    addParameter(FloatNumber,  "power",                    "power",                                         "p",                  "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 1,          -DBL_MAX, DBL_MAX,  1,  -DBL_MAX,    DBL_MAX  );
    #define PARAM_POWER 2

}

double QFFitFunctionGeneralPowerLaw::evaluate(double t, const double* data) const {
    return data[PARAM_OFFSET]+data[PARAM_AMPLITUDE]*pow(t, data[PARAM_POWER]);

}


void QFFitFunctionGeneralPowerLaw::calcParameter(double* data, double* error) const {

}

bool QFFitFunctionGeneralPowerLaw::isParameterVisible(int parameter, const double* data) const {
    return true;
}
unsigned int QFFitFunctionGeneralPowerLaw::getAdditionalPlotCount(const double* params) {
    return 0;
}

QString QFFitFunctionGeneralPowerLaw::transformParametersForAdditionalPlot(int plot, double* params) {
    return "";
}

bool QFFitFunctionGeneralPowerLaw::get_implementsDerivatives()
{
    return false;
}

void QFFitFunctionGeneralPowerLaw::evaluateDerivatives(double* derivatives, double t, const double* data) const {
}

bool QFFitFunctionGeneralPowerLaw::estimateInitial(double *params, const double *dataX, const double *dataY, long N, const bool *fix)
{
    //statisticsMinMax(dataY, N, params[PARAM_BASE], params[PARAM_MAX]);

    return QFFitFunction::estimateInitial(params, dataX, dataY, N);
}
