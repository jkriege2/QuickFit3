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

#include "qffitfunctiongeneralpdfgaussianvar.h"
#include "qfmathtools.h"
#include <cmath>
#include "statistics_tools.h"

QFFitFunctionGeneralPDFGaussianVar::QFFitFunctionGeneralPDFGaussianVar() {
    //           type,         id,                        name,                                                    label (HTML),                      unit,          unitlabel (HTML),        fit,       userEditable, userRangeEditable, displayError, initialFix,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(FloatNumber,  "position",                "position",                                              "X<sub>0</sub>",                        "",         "",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 0,            -1e10,    1e10,  1  );
    #define PARAM_POSITION 0
    addParameter(FloatNumber,  "width",                   "1/sqrt(e) width",                                        "&sigma;",                           "",         "",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 1,            1e-10,    1e10,  1  );
    #define PARAM_WIDTH 1

}

double QFFitFunctionGeneralPDFGaussianVar::evaluate(double t, const double* data) const {
    const double position=data[PARAM_POSITION];
    const double width=data[PARAM_WIDTH];
    return exp(-0.5*qfSqr(t-position)/qfSqr(width))/sqrt(2.0*M_PI*qfSqr(width));
}


void QFFitFunctionGeneralPDFGaussianVar::calcParameter(double* data, double* error) const {
}

bool QFFitFunctionGeneralPDFGaussianVar::isParameterVisible(int parameter, const double* data) const {
    return true;
    // all parameters are visible at all times
}

unsigned int QFFitFunctionGeneralPDFGaussianVar::getAdditionalPlotCount(const double* params) {
    return 0;
    // we have one additional plot
}

QString QFFitFunctionGeneralPDFGaussianVar::transformParametersForAdditionalPlot(int plot, double* params) {
    return "";
}


bool QFFitFunctionGeneralPDFGaussianVar::estimateInitial(double *params, const double *dataX, const double *dataY, long N, const bool* fix)
{
    //statisticsMinMax(dataY, N, params[PARAM_BASE], params[PARAM_MAX]);
    if (params && dataX && dataY) {
        double pW=0;
        double pB=0;
        double pH=0;
        double pP=statisticsPeakFind(pW, dataX, dataY, N, 0.0, double(NAN), &pB, &pH);
        if (statisticsFloatIsOK(pP)) {
            double dx=0;
            statisticsMinDistance(dataX, N, &dx);
            if (dx>0) {
                pW=qMax(pW,2.0*dx);
            }
            params[PARAM_POSITION]=pP;
            params[PARAM_WIDTH]=pW/2.3548;
            return true;
        } else {
            return false;
        }
        return true;
    }

    return true;
}
