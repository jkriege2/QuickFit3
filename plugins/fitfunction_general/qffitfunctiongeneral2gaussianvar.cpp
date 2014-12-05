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

#include "qffitfunctiongeneral2gaussianvar.h"
#include "qfmathtools.h"
#include <cmath>
#include "statistics_tools.h"

QFFitFunctionGeneral2GaussianVar::QFFitFunctionGeneral2GaussianVar() {
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
    addParameter(FloatNumber,  "width2",                   "1/sqrt(e) width 2",                                        "&sigma;<sub>2</sub>",                           "",         "",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 1,            -1e10,    1e10,  1  );
    #define PARAM_WIDTH2 6

}

double QFFitFunctionGeneral2GaussianVar::evaluate(double t, const double* data) const {
    return data[PARAM_OFFSET]+data[PARAM_AMPLITUDE]*exp(-0.5*qfSqr(t-data[PARAM_POSITION])/qfSqr(data[PARAM_WIDTH]))
                             +data[PARAM_AMPLITUDE2]*exp(-0.5*qfSqr(t-data[PARAM_POSITION2])/qfSqr(data[PARAM_WIDTH2]));
}

void QFFitFunctionGeneral2GaussianVar::evaluateDerivatives(double* derivatives, double t, const double* data) const {
    for (register int i=0; i<paramCount(); i++) derivatives[i]=0;

}

void QFFitFunctionGeneral2GaussianVar::calcParameter(double* data, double* error) const {
}

bool QFFitFunctionGeneral2GaussianVar::isParameterVisible(int parameter, const double* data) const {
    return true;
    // all parameters are visible at all times
}

unsigned int QFFitFunctionGeneral2GaussianVar::getAdditionalPlotCount(const double* params) {
    return 2;
    // we have one additional plot
}

QString QFFitFunctionGeneral2GaussianVar::transformParametersForAdditionalPlot(int plot, double* params) {
    if (plot==0) {
        params[PARAM_AMPLITUDE2]=0;
        return QObject::tr("Gauss 1");
    } else if (plot==1) {
        params[PARAM_AMPLITUDE]=0;
        return QObject::tr("Gauss 2");
    }
    return "";
}

bool QFFitFunctionGeneral2GaussianVar::get_implementsDerivatives()
{
    return false;
}

bool QFFitFunctionGeneral2GaussianVar::estimateInitial(double *params, const double *dataX, const double *dataY, long N, const bool* fix)
{
    if (params && dataX && dataY) {
        double pW=0;
        double pB=0;
        double pH=0;
        double pP=0;
        double pW2=0;
        double pH2=0;
        double pP2=0;
        if (statistics2PeakFind(pP, pW, pP2, pW2, dataX, dataY, N, 0.0, (double)NAN, &pB, &pH, &pH2)) {
            double dx=0;
            statisticsMinDistance(dataX, N, &dx);
            if (dx>0) {
                pW=qMax(pW,6.0*dx);
                pW2=qMax(pW2,6.0*dx);
            }
            params[PARAM_OFFSET]=pB;
            params[PARAM_AMPLITUDE]=pH;
            params[PARAM_POSITION]=pP;
            params[PARAM_WIDTH]=pW/2.3548;
            if (statisticsFloatIsOK(pP2)) {
                params[PARAM_AMPLITUDE2]=pH2;
                params[PARAM_POSITION2]=pP2;
                params[PARAM_WIDTH2]=pW2/2.3548;
            }
            return true;
        } else {
            return false;
        }
        return true;
    }

    return true;
}
