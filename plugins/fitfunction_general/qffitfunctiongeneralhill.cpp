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

#include "qffitfunctiongeneralhill.h"
#include "qfmathtools.h"
#include "statistics_tools.h"
#include <cmath>


QFFitFunctionGeneralHill::QFFitFunctionGeneralHill() {
    //           type,         id,                        name,                                                    label (HTML),                      unit,          unitlabel (HTML),        fit,       userEditable, userRangeEditable, displayError, initialFix,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(FloatNumber,  "offset",                 "offset",                                                  "Y<sub>0</sub>",                   "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 0.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_BASE 0
    addParameter(FloatNumber,  "max",                    "max",                                                     "Y<sub>max</sub>",                 "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 1.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_MAX 1
    addParameter(FloatNumber,  "position",               "position",                                                "X<sub>1/2</sub>",                 "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 0.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_POSITION 2
    addParameter(FloatNumber,  "rate",                   "rate",                                                    "m",                               "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 1.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_RATE 3
    addParameter(FloatNumber,  "mid_slope",              "slope at position",                                       "df/dx(x=X<sub>1/2</sub>=",        "",            "",                      false,      false,         false,           QFFitFunction::DisplayError,       false, 1.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_MID_SLOPE 4

}

double QFFitFunctionGeneralHill::evaluate(double t, const double* data) const {
    return data[PARAM_BASE]+(data[PARAM_MAX]-data[PARAM_BASE])/(1.0+pow(10, (data[PARAM_POSITION]-t)*data[PARAM_RATE]));//exp((data[PARAM_POSITION]-t)/data[PARAM_RATE]));

}


void QFFitFunctionGeneralHill::calcParameter(double* data, double* error) const {
    if (data) {
        data[PARAM_MID_SLOPE]=(data[PARAM_MAX]-data[PARAM_BASE])*data[PARAM_RATE]*log(10)/4.0;
    }
}

bool QFFitFunctionGeneralHill::isParameterVisible(int parameter, const double* data) const {
    return true;
}
unsigned int QFFitFunctionGeneralHill::getAdditionalPlotCount(const double* params) {
    return 0;
}

QString QFFitFunctionGeneralHill::transformParametersForAdditionalPlot(int plot, double* params) {
    return "";
}

bool QFFitFunctionGeneralHill::get_implementsDerivatives()
{
    return false;
}

void QFFitFunctionGeneralHill::evaluateDerivatives(double* derivatives, double t, const double* data) const {
}

bool QFFitFunctionGeneralHill::estimateInitial(double *params, const double *dataX, const double *dataY, long N, const bool *fix)
{
    //statisticsMinMax(dataY, N, params[PARAM_BASE], params[PARAM_MAX]);
    if (params && dataX && dataY) {

        StatisticsScopedPointer<double> dX=statisticsDuplicateArray(dataX, N);
        StatisticsScopedPointer<double> dY=statisticsDuplicateArray(dataY, N);

        statisticsSort2(dX.data(), dY.data(), N);

        double mi=0, ma=0;

        statisticsMinMax(dY.data(), N, mi, ma);

        double p=statisticsXatY50Sorted(dX.data(), dY.data(), N);

        double rate=(ma-mi)/(dX[N-1]-dX[0]);
        rate=rate/log(10)*4.0/(ma-mi);
        if (dY[N-1]-dY[0]<0) {
            rate=rate*(-1.0);
        }

        params[PARAM_BASE]=mi;
        params[PARAM_MAX]=ma;
        params[PARAM_POSITION]=p;
        params[PARAM_RATE]=rate;



    }

    return true;

}
