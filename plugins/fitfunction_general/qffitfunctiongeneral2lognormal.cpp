/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

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

#include "qffitfunctiongeneral2lognormal.h"
#include "qfmathtools.h"
#include "statistics_tools.h"
#include <cmath>


QFFitFunctionGeneral2LogNormal::QFFitFunctionGeneral2LogNormal() {
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

double QFFitFunctionGeneral2LogNormal::evaluate(double t, const double* data) const {
    const double offset=data[PARAM_OFFSET];
    const double amplitude=data[PARAM_AMPLITUDE];
    const double position=data[PARAM_POSITION];
    const double width=data[PARAM_WIDTH];
    const double amplitude2=data[PARAM_AMPLITUDE2];
    const double position2=data[PARAM_POSITION2];
    const double width2=data[PARAM_WIDTH2];
    return offset+amplitude*exp(-0.5*qfSqr(log(t)-position)/qfSqr(width))+amplitude2*exp(-0.5*qfSqr(log(t)-position2)/qfSqr(width2));
}


void QFFitFunctionGeneral2LogNormal::calcParameter(double* data, double* error) const {

}

bool QFFitFunctionGeneral2LogNormal::isParameterVisible(int parameter, const double* data) const {
    return true;
}
unsigned int QFFitFunctionGeneral2LogNormal::getAdditionalPlotCount(const double* params) {
    return 0;
}

QString QFFitFunctionGeneral2LogNormal::transformParametersForAdditionalPlot(int plot, double* params) {
    return "";
}

bool QFFitFunctionGeneral2LogNormal::get_implementsDerivatives()
{
    return false;
}

void QFFitFunctionGeneral2LogNormal::evaluateDerivatives(double* derivatives, double t, const double* data) const {
}

bool QFFitFunctionGeneral2LogNormal::estimateInitial(double *params, const double *dataX, const double *dataY, long N, const bool* fix)
{
    if (params && dataX && dataY) {
        StatisticsScopedPointer<double> dX=statisticsDuplicateAndApply(dataX, N, log);
        double pW=0;
        double pB=0;
        double pH=0;
        double pP=0;
        double pW2=0;
        double pH2=0;
        double pP2=0;
        if (statistics2PeakFind(pP, pW, pP2, pW2, dX.data(), dataY, N, 0.0, (double)NAN, &pB, &pH, &pH2)) {
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
