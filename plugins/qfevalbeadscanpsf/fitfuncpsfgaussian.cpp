/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2015-01-17 17:54:12 +0100 (Sa, 17 Jan 2015) $  (revision $Rev: 3725 $)

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

#include "fitfuncpsfgaussian.h"
#include "statistics_tools.h"

#include <cmath>
#define sqr(x) ((x)*(x))
#define cube(x) ((x)*(x)*(x))
#define pow4(x) ((x)*(x)*(x)*(x))
#define pow5(x) ((x)*(x)*(x)*(x)*(x))


QFFitFunctionPSFGaussian::QFFitFunctionPSFGaussian() {
    //           type,         id,                        name,                                                    label (HTML),                      unit,          unitlabel (HTML),        fit,       userEditable, userRangeEditable, displayError, initialFix,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(FloatNumber,  "offset",                  "offset",                                                "offset",                          "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 0.0,          -1e10,    1e10,  1  );
    #define PARAM_OFFSET 0
    addParameter(FloatNumber,  "amplitude",               "amplitude",                                             "amplitude",                       "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 1000.0,       0,    1e10,  1  );
    #define PARAM_AMPLITUDE 1
    addParameter(FloatNumber,  "position",                "position",                                              "position",                        "pix",         "pix",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 64,            -1e10,    1e10,  1  );
    #define PARAM_POSITION 2
    addParameter(FloatNumber,  "width",                   "width",                                                 "width",                           "pix",         "pix",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 10,            0,    1e10,  1  );
    #define PARAM_WIDTH 3

}

double QFFitFunctionPSFGaussian::evaluate(double t, const double* data) const {
    const double offset=data[PARAM_OFFSET];
    const double amplitude=data[PARAM_AMPLITUDE];
    const double position=data[PARAM_POSITION];
    const double width=data[PARAM_WIDTH];
    return offset+amplitude*exp(-2.0*sqr(t-position)/sqr(width));
}

void QFFitFunctionPSFGaussian::evaluateDerivatives(double* derivatives, double /*t*/, const double* /*data*/) const {
    for (register int i=0; i<paramCount(); i++) derivatives[i]=0;

}

void QFFitFunctionPSFGaussian::calcParameter(double* /*data*/, double* /*error*/) const {
}

bool QFFitFunctionPSFGaussian::isParameterVisible(int /*parameter*/, const double* /*data*/) const {
    return true;
	// all parameters are visible at all times
}

unsigned int QFFitFunctionPSFGaussian::getAdditionalPlotCount(const double* /*params*/) {
    return 0;
	// we have one additional plot
}

QString QFFitFunctionPSFGaussian::transformParametersForAdditionalPlot(int plot, double* params) {
    return "";
}

bool QFFitFunctionPSFGaussian::estimateInitial(double *params, const double *dataX, const double *dataY, long N, const bool* fix)
{
    //statisticsMinMax(dataY, N, params[PARAM_BASE], params[PARAM_MAX]);
    if (params && dataX && dataY) {
        double pW=0;
        double pB=0;
        double pH=0;
        double pP=statisticsPeakFind(pW, dataX, dataY, N, 0.0, double(NAN), &pB, &pH);
        if (statisticsFloatIsOK(pP)) {
            params[PARAM_OFFSET]=pB;
            params[PARAM_AMPLITUDE]=pH;
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
