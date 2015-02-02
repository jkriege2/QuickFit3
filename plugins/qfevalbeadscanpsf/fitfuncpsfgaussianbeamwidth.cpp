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

#include "fitfuncpsfgaussianbeamwidth.h"
#include "qfmathtools.h"
#include <cmath>
#include "statistics_tools.h"

QFFitFunctionPSFGaussianBeamWidth::QFFitFunctionPSFGaussianBeamWidth() {
    //           type,         id,                        name,                                                    label (HTML),                      unit,          unitlabel (HTML),        fit,       userEditable, userRangeEditable, displayError, initialFix,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(FloatNumber,  "zR",                "rayleigh range zR",                                              "z<sub>R</sub>",                        "nm",         "",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 1200,            100,    1e10,  1  );
    #define PARAM_Z0 0
    addParameter(FloatNumber,  "w0",                   "beam diameter w0",                                        "w<sub>0</sub>",                           "nm",         "",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 500,            50,    1e10,  1  );
    #define PARAM_W0 1
    addParameter(FloatNumber,  "position",                "position",                                              "Z<sub>0</sub>",                        "nm",         "",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 0,            -1e10,    1e10,  1  );
    #define PARAM_POSITION 2

}

double QFFitFunctionPSFGaussianBeamWidth::evaluate(double t, const double* data) const {
    const double w0=data[PARAM_W0];
    const double z0=data[PARAM_Z0];
    const double pos=data[PARAM_POSITION];
    return w0*sqrt(1.0+qfSqr((t-pos)/z0));
}



void QFFitFunctionPSFGaussianBeamWidth::calcParameter(double* data, double* error) const {
}

bool QFFitFunctionPSFGaussianBeamWidth::isParameterVisible(int parameter, const double* data) const {
    return true;
    // all parameters are visible at all times
}

unsigned int QFFitFunctionPSFGaussianBeamWidth::getAdditionalPlotCount(const double* params) {
    return 0;
    // we have one additional plot
}

QString QFFitFunctionPSFGaussianBeamWidth::transformParametersForAdditionalPlot(int plot, double* params) {
    return "";
}


bool QFFitFunctionPSFGaussianBeamWidth::estimateInitial(double *params, const double *dataX, const double *dataY, long N, const bool* fix)
{
    //statisticsMinMax(dataY, N, params[PARAM_BASE], params[PARAM_MAX]);
    if (params && dataX && dataY) {
        long long mpos=0;
        double pW0=statisticsMin(dataY, N, &mpos);
        mpos=qBound((long long)0, (long long)mpos, (long long)N-1);
        if (statisticsFloatIsOK(pW0)) {

            params[PARAM_W0]=pW0;
            params[PARAM_Z0]=M_PI*pW0*pW0/525.0;
            params[PARAM_POSITION]=dataX[mpos];
            return true;
        } else {
            return false;
        }
        return true;
    }

    return true;
}
