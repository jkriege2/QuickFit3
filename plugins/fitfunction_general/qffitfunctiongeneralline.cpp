/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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

#include "qffitfunctiongeneralline.h"
#include "qfmathtools.h"
#include "statistics_tools.h"
#include <cmath>


QFFitFunctionGeneralLine::QFFitFunctionGeneralLine() {
    //           type,         id,                        name,                                                    label (HTML),                      unit,          unitlabel (HTML),        fit,       userEditable, userRangeEditable, displayError, initialFix,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(FloatNumber,  "offset",                    "offset",                                                  "a",                   "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 0.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_OFFSET 0
    addParameter(FloatNumber,  "slope",                    "slope",                                                  "b",                   "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 1.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_SLOPE 1

}

double QFFitFunctionGeneralLine::evaluate(double t, const double* data) const {
    return data[PARAM_OFFSET]+data[PARAM_SLOPE]*t;

}


void QFFitFunctionGeneralLine::calcParameter(double* /*data*/, double* /*error*/) const {

}

bool QFFitFunctionGeneralLine::isParameterVisible(int parameter, const double* data) const {

    Q_UNUSED(parameter);
    Q_UNUSED(data);
    return true;
}
unsigned int QFFitFunctionGeneralLine::getAdditionalPlotCount(const double* params) const {

    Q_UNUSED(params);
    return 0;
}

QString QFFitFunctionGeneralLine::transformParametersForAdditionalPlot(int plot, double* params) const {


    Q_UNUSED(plot);
    Q_UNUSED(params);
    return "";
}

bool QFFitFunctionGeneralLine::get_implementsDerivatives() const
{
    return false;
}



bool QFFitFunctionGeneralLine::estimateInitial(double *params, const double *dataX, const double *dataY, long N, const bool* fix) const
{
    //statisticsMinMax(dataY, N, params[PARAM_BASE], params[PARAM_MAX]);
    if (params && dataX && dataY) {
        double a=params[PARAM_OFFSET];
        double b=params[PARAM_SLOPE];
        if (fix) statisticsLinearRegression(dataX, dataY, N, a, b, fix[PARAM_OFFSET], fix[PARAM_SLOPE]);
        else statisticsLinearRegression(dataX, dataY, N, a, b, false, false);

        params[PARAM_OFFSET]=a;
        params[PARAM_SLOPE]=b;
    }

    return true;
}
