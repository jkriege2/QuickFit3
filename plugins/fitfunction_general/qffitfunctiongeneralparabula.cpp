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


#include "qffitfunctiongeneralparabula.h"
#include "qfmathtools.h"
#include "statistics_tools.h"
#include <cmath>
#include <QDebug>

QFFitFunctionGeneralParabula::QFFitFunctionGeneralParabula() {
    //           type,         id,                        name,                                                    label (HTML),                      unit,          unitlabel (HTML),        fit,       userEditable, userRangeEditable, displayError, initialFix,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(FloatNumber,  "offset",                    "offset",                                                  "y<sub>0</sub>",                   "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 0.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_OFFSET 0
    addParameter(FloatNumber,  "position",                    "position",                                                  "x<sub>0</sub>",                   "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 0.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_POSITION 1
    addParameter(FloatNumber,  "factor2",                    "prefactor",                                                  "a<sub>2</sub>",                   "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 1.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_FACTOR2 2
    addParameter(FloatNumber,  "factor1",                    "prefactor 1",                                                "a<sub>1</sub>",                   "",            "",                      false,      false,         false,              QFFitFunction::DisplayError,       false, 1.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_FACTOR1 3
    addParameter(FloatNumber,  "factor0",                    "standard form offset",                                       "a<sub>0</sub>",                   "",            "",                      false,      false,         false,              QFFitFunction::DisplayError,       false, 1.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_FACTOR0 4

}

double QFFitFunctionGeneralParabula::evaluate(double t, const double* data) const {
    double y0= data[PARAM_OFFSET];
    double x0= data[PARAM_POSITION];
    double a= data[PARAM_FACTOR2];

    return a*qfSqr(t-x0)+y0;
}


void QFFitFunctionGeneralParabula::calcParameter(double* data, double* error) const {
    double y0= data[PARAM_OFFSET];
    double x0= data[PARAM_POSITION];
    double a= data[PARAM_FACTOR2];

    if (data) {
        data[PARAM_FACTOR1]=-2.0*a*x0;
        data[PARAM_FACTOR0]=a*x0*x0+y0;
    }
    if (error) {
        error[PARAM_FACTOR1]=qfErrorMul(-2.0, 0, a, error[PARAM_FACTOR2], x0, error[PARAM_POSITION]);
        error[PARAM_FACTOR0]=sqrt(qfSqr(x0*x0*error[PARAM_FACTOR2])+qfSqr(2*a*x0*error[PARAM_POSITION])+qfSqr(error[PARAM_OFFSET]));
    }
}

bool QFFitFunctionGeneralParabula::isParameterVisible(int parameter, const double* data) const {

    Q_UNUSED(parameter);
    Q_UNUSED(data);
    return true;
}
unsigned int QFFitFunctionGeneralParabula::getAdditionalPlotCount(const double* params) const {

    Q_UNUSED(params);
    return 0;
}

QString QFFitFunctionGeneralParabula::transformParametersForAdditionalPlot(int plot, double* params) const {


    Q_UNUSED(plot);
    Q_UNUSED(params);
    return "";
}

bool QFFitFunctionGeneralParabula::get_implementsDerivatives() const
{
    return false;
}


bool QFFitFunctionGeneralParabula::estimateInitial(double *params, const double *dataX, const double *dataY, long N, const bool */*fix*/) const
{
    if (params && dataX && dataY && N>0)  {
        double p[3]={0,0,0};
        if (N<3) return false;

        statisticsPolyFit(dataX, dataY, N, 2, p);
        params[PARAM_FACTOR2]=p[2];
        params[PARAM_POSITION]=p[1]/(-2.0*p[2]);
        params[PARAM_OFFSET]=p[0]-p[2]*qfSqr(params[PARAM_POSITION]);
    }

    return true;
}
