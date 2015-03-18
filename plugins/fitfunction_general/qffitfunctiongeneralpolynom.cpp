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

#include "qffitfunctiongeneralpolynom.h"
#include "qfmathtools.h"
#include "statistics_tools.h"
#include <cmath>
#include <QDebug>

QFFitFunctionGeneralPolynom::QFFitFunctionGeneralPolynom() {
    //           type,         id,                        name,                                                    label (HTML),                      unit,          unitlabel (HTML),        fit,       userEditable, userRangeEditable, displayError, initialFix,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(IntCombo,  "degree",                    "degree",                                                  "degree",                   "",            "",                      false,      true,         false,              QFFitFunction::NoError,       false, 2,          1, 10,  1,   1,    10  );
    #define PARAM_DEGREE 0
    addParameter(FloatNumber,  "offset",                    "offset",                                                  "a<sub>0</sub>",                   "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 0.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_OFFSET 1
    addParameter(FloatNumber,  "factor1",                    "prefactor 1",                                                  "a<sub>1</sub>",                   "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 1.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_FACTOR1 2
    addParameter(FloatNumber,  "factor2",                    "prefactor 2",                                                  "a<sub>2</sub>",                   "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 1.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_FACTOR2 3
    addParameter(FloatNumber,  "factor3",                    "prefactor 3",                                                  "a<sub>3</sub>",                   "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 1.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_FACTOR3 4
    addParameter(FloatNumber,  "factor4",                    "prefactor 4",                                                  "a<sub>4</sub>",                   "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 1.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_FACTOR4 5
    addParameter(FloatNumber,  "factor5",                    "prefactor 5",                                                  "a<sub>5</sub>",                   "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 1.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_FACTOR5 6
    addParameter(FloatNumber,  "factor6",                    "prefactor 6",                                                  "a<sub>6</sub>",                   "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 1.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_FACTOR6 7
    addParameter(FloatNumber,  "factor7",                    "prefactor 7",                                                  "a<sub>7</sub>",                   "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 1.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_FACTOR7 8
    addParameter(FloatNumber,  "factor8",                    "prefactor 8",                                                  "a<sub>8</sub>",                   "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 1.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_FACTOR8 9
    addParameter(FloatNumber,  "factor9",                    "prefactor 9",                                                  "a<sub>9</sub>",                   "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 1.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_FACTOR9 10
    addParameter(FloatNumber,  "factor10",                    "prefactor 10",                                                  "a<sub>10</sub>",                   "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 1.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_FACTOR10 11

}

double QFFitFunctionGeneralPolynom::evaluate(double t, const double* data) const {
    double res= data[PARAM_OFFSET];
    int degree=data[PARAM_DEGREE];
    double x=t;
    for (int i=0; i<degree; i++) {
        res=res+x*data[PARAM_FACTOR1+i];
        x=x*t;
    }
    return res;
}


void QFFitFunctionGeneralPolynom::calcParameter(double* data, double* error) const {

}

bool QFFitFunctionGeneralPolynom::isParameterVisible(int parameter, const double* data) const {
    if (!data) return true;
    int degree=data[PARAM_DEGREE];
    //qDebug()<<"isVisible("<<parameter<<")="<<degree;
    switch(parameter) {
        case PARAM_FACTOR1: return degree>0;
        case PARAM_FACTOR2: return degree>1;
        case PARAM_FACTOR3: return degree>2;
        case PARAM_FACTOR4: return degree>3;
        case PARAM_FACTOR5: return degree>4;
        case PARAM_FACTOR6: return degree>5;
        case PARAM_FACTOR7: return degree>6;
        case PARAM_FACTOR8: return degree>7;
        case PARAM_FACTOR9: return degree>8;
        case PARAM_FACTOR10: return degree>9;
        default: return true;
    }

    return true;
}
unsigned int QFFitFunctionGeneralPolynom::getAdditionalPlotCount(const double* params) {

    Q_UNUSED(params);
    return 0;
}

QString QFFitFunctionGeneralPolynom::transformParametersForAdditionalPlot(int plot, double* params) {


    Q_UNUSED(plot);
    Q_UNUSED(params);
    return "";
}

bool QFFitFunctionGeneralPolynom::get_implementsDerivatives()
{
    return false;
}

void QFFitFunctionGeneralPolynom::evaluateDerivatives(double* derivatives, double t, const double* data) const {
}

bool QFFitFunctionGeneralPolynom::estimateInitial(double *params, const double *dataX, const double *dataY, long N, const bool *fix)
{
    if (params && dataX && dataY && N>0) {
        int degree=params[PARAM_DEGREE];
        if (N<degree-1) degree=N+1;
        degree=qBound(1,degree,10);
        params[PARAM_DEGREE]=degree;

        statisticsPolyFit(dataX, dataY, N, degree, &(params[PARAM_OFFSET]));
    }

    return true;
}
