/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2014-09-26 12:40:44 +0200 (Fr, 26 Sep 2014) $  (revision $Rev: 3508 $)

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

#include "qffitfunctiongeneralsegmentedpowerlaw.h"

#include "qfmathtools.h"
#include "statistics_tools.h"
#include <cmath>


QFFitFunctionGeneralSegmentedPowerLaw::QFFitFunctionGeneralSegmentedPowerLaw(bool logoptimized) {
    this->logoptimized=logoptimized;
    //           type,         id,                        name,                                                    label (HTML),                      unit,          unitlabel (HTML),        fit,       userEditable, userRangeEditable, displayError, initialFix,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(IntCombo,  "segments",                   "segments",                                                "segments",                   "",            "",                      false,      true,         false,              QFFitFunction::NoError,       false, 2,          2, 3,  1,   1,    10  );
    #define PARAM_SEGMENTS 0
    addParameter(FloatNumber,  "offset",                    "offset",                                                  "Y<sub>0</sub>",                   "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       true, 0.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_OFFSET 1
    addParameter(FloatNumber,  "amplitude",                  "amplitude 1",                                                "A<sub>1</sub>",               "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false,1.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX );
    #define PARAM_AMPLITUDE1 2
    addParameter(FloatNumber,  "power",                   "power 1",                                                 "p<sub>1</sub>",               "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 1,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_POWER1 3
    if (logoptimized) addLogParameter(FloatNumber,  "intersection12",          "intersection x2",                                         "x<sub>2</sub>",               "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 1,          0, DBL_MAX,  1,   0,    DBL_MAX  );
    else addParameter(FloatNumber,  "intersection12",          "intersection x2",                                         "x<sub>2</sub>",               "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 1,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_INTERSECT2 4
    addParameter(FloatNumber,  "amplitude2",                 "amplitude 2",                                                "A<sub>2</sub>",               "",            "",                      false,     false,        false,             QFFitFunction::DisplayError,       false, 1.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_AMPLITUDE2 5
    addParameter(FloatNumber,  "power2",                  "power 2",                                                 "p<sub>2</sub>",               "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 2.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_POWER2 6
    if (logoptimized) addLogParameter(FloatNumber,  "intersection23",          "intersection x3",                                         "x<sub>3</sub>",               "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 10.0,          0, DBL_MAX,  1,   0,    DBL_MAX  );
    else addParameter(FloatNumber,  "intersection23",          "intersection x3",                                         "x<sub>3</sub>",               "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 10.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_INTERSECT3 7
    addParameter(FloatNumber,  "amplitude3",                 "amplitude 3",                                                "A<sub>3</sub>",               "",            "",                      false,     false,        false,             QFFitFunction::DisplayError,       false, 1.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_AMPLITUDE3 8
    addParameter(FloatNumber,  "power3",                  "power 3",                                                 "p<sub>3</sub>",               "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 4.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_POWER3 9


}

QString QFFitFunctionGeneralSegmentedPowerLaw::id() const
{
    if (logoptimized) return QString("gen_segpowerlaw");
    else return QString("gen_segpowerlaw_nonopt");
}

double QFFitFunctionGeneralSegmentedPowerLaw::evaluate(double t, const double* data) const {
    const int segs=data[PARAM_SEGMENTS];
    if (t<data[PARAM_INTERSECT2] || segs<=1) return data[PARAM_OFFSET]+data[PARAM_AMPLITUDE1]*pow(t, data[PARAM_POWER1]);
    else {
        const double a2=data[PARAM_AMPLITUDE1]*pow(data[PARAM_INTERSECT2], (data[PARAM_POWER1]-data[PARAM_POWER2]));
        if (t<data[PARAM_INTERSECT3] || segs<=2) {
            return data[PARAM_OFFSET]+a2*pow(t, data[PARAM_POWER2]);
        } else {
            const double a3=data[PARAM_AMPLITUDE2]*pow(data[PARAM_INTERSECT3], (data[PARAM_POWER2]-data[PARAM_POWER3]));
            return data[PARAM_OFFSET]+a3*pow(t, data[PARAM_POWER3]);
        }
    }
}


void QFFitFunctionGeneralSegmentedPowerLaw::calcParameter(double* data, double* error) const {
    const int segs=data[PARAM_SEGMENTS];
    const double p1=data[PARAM_POWER1];
    const double p2=data[PARAM_POWER2];
    const double x2=data[PARAM_INTERSECT2];
    const double a1=data[PARAM_AMPLITUDE1];
    const double p3=data[PARAM_POWER3];
    const double x3=data[PARAM_INTERSECT3];

    const double a2=pow(x2, (p1-p2))*a1;
    data[PARAM_AMPLITUDE2]=a2;
    if (segs>=3) {
        data[PARAM_AMPLITUDE3]=pow(x3, (p2-p3))*a2;
    }
    if (error) {
        const double eb1=error[PARAM_POWER1];
        const double eb2=error[PARAM_POWER2];
        const double ex2=error[PARAM_INTERSECT2];
        const double ea1=error[PARAM_AMPLITUDE1];
        const double eb3=error[PARAM_POWER3];
        const double ex3=error[PARAM_INTERSECT3];
        const double ea2=0;
        error[PARAM_AMPLITUDE2]=ea2;
        if (segs>=3) {
            error[PARAM_AMPLITUDE3]=0;
        }
    }
}

bool QFFitFunctionGeneralSegmentedPowerLaw::isParameterVisible(int parameter, const double* data) const {
    const int segs=data[PARAM_SEGMENTS];
    switch(parameter) {
        case PARAM_POWER2: case PARAM_INTERSECT2: case PARAM_AMPLITUDE2: return segs>=2;
        case PARAM_POWER3: case PARAM_INTERSECT3: case PARAM_AMPLITUDE3: return segs>=3;
        default: return true;
    }
    return true;
}
unsigned int QFFitFunctionGeneralSegmentedPowerLaw::getAdditionalPlotCount(const double* params) {
    return 0;
}

QString QFFitFunctionGeneralSegmentedPowerLaw::transformParametersForAdditionalPlot(int plot, double* params) {
    return "";
}

bool QFFitFunctionGeneralSegmentedPowerLaw::get_implementsDerivatives()
{
    return false;
}

void QFFitFunctionGeneralSegmentedPowerLaw::evaluateDerivatives(double* derivatives, double t, const double* data) const {
}

bool QFFitFunctionGeneralSegmentedPowerLaw::estimateInitial(double *params, const double *dataX, const double *dataY, long N, const bool *fix)
{
    //statisticsMinMax(dataY, N, params[PARAM_BASE], params[PARAM_MAX]);

    return QFFitFunction::estimateInitial(params, dataX, dataY, N);
}
