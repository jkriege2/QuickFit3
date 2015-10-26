/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

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

#include "fitfuncpsfgaussianbeamzintensity.h"
#include "qfmathtools.h"
#include <cmath>
#include "statistics_tools.h"

QFFitFunctionPSFGaussianBeamZIntensity::QFFitFunctionPSFGaussianBeamZIntensity() {
    //           type,         id,                        name,                                                    label (HTML),                      unit,          unitlabel (HTML),        fit,       userEditable, userRangeEditable, displayError, initialFix,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(FloatNumber,  "zR",                "rayleigh range zR",                                              "z<sub>R</sub>",                        "nm",         "",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 500,            -1e10,    1e10,  1  );
    #define PARAM_Z0 0
    addParameter(FloatNumber,  "position",                "position Z0",                                              "Z<sub>0</sub>",                        "nm",         "",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 0,            -1e10,    1e10,  1  );
    #define PARAM_POSITION 1
    addParameter(FloatNumber,  "amplitude",               "amplitude I0",                                             "I<sub>0</sub>",                       "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 1.0,       -1e10,    1e10,  1  );
    #define PARAM_AMPLITUDE 2

}

double QFFitFunctionPSFGaussianBeamZIntensity::evaluate(double t, const double* data) const {
    const double z0=data[PARAM_Z0];
    const double pos=data[PARAM_POSITION];
    const double I0=data[PARAM_AMPLITUDE];
    const double w= sqrt(1.0+qfSqr((t-pos)/z0));
    return I0*qfSqr(1.0/w);
}



void QFFitFunctionPSFGaussianBeamZIntensity::calcParameter(double* /*data*/, double* /*error*/) const {
}

bool QFFitFunctionPSFGaussianBeamZIntensity::isParameterVisible(int /*parameter*/, const double* /*data*/) const {
    return true;
    // all parameters are visible at all times
}

unsigned int QFFitFunctionPSFGaussianBeamZIntensity::getAdditionalPlotCount(const double* /*params*/) const {
    return 0;
    // we have one additional plot
}

QString QFFitFunctionPSFGaussianBeamZIntensity::transformParametersForAdditionalPlot(int /*plot*/, double* /*params*/) const {
    return "";
}


