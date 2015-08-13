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


#include "qffitfunctiongeneralgaussiandistvar.h"
#include "qfmathtools.h"
#include <cmath>
#include "statistics_tools.h"

QFFitFunctionGeneralGaussianDistVar::QFFitFunctionGeneralGaussianDistVar() {
    //           type,         id,                        name,                                                    label (HTML),                      unit,          unitlabel (HTML),        fit,       userEditable, userRangeEditable, displayError, initialFix,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(FloatNumber,  "offset",                  "offset",                                                "Y<sub>0</sub>",                          "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 0.0,          -1e10,    1e10,  1  );
    #define PARAM_OFFSET 0
    addParameter(FloatNumber,  "amplitude",               "amplitude",                                             "A",                       "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 1.0,       -1e10,    1e10,  1  );
    #define PARAM_AMPLITUDE 1
    addParameter(FloatNumber,  "position",                "position",                                              "X<sub>0</sub>",                        "",         "",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 0,            -1e10,    1e10,  1  );
    #define PARAM_POSITION 2
    addParameter(FloatNumber,  "width",                   "1/sqrt(e) width",                                        "&sigma;",                           "",         "",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 1,            1e-10,    1e10,  1  );
    #define PARAM_WIDTH 3

}

double QFFitFunctionGeneralGaussianDistVar::evaluate(double t, const double* data) const {
    const double offset=data[PARAM_OFFSET];
    const double amplitude=data[PARAM_AMPLITUDE];
    const double position=data[PARAM_POSITION];
    const double width=data[PARAM_WIDTH];
    return offset+amplitude*0.5*(1.0+erf((t-position)/sqrt(2.0*qfSqr(width))));
}


void QFFitFunctionGeneralGaussianDistVar::calcParameter(double* /*data*/, double* /*error*/) const {
}

bool QFFitFunctionGeneralGaussianDistVar::isParameterVisible(int parameter, const double* data) const {

    Q_UNUSED(parameter);
    Q_UNUSED(data);
    return true;
    // all parameters are visible at all times
}

unsigned int QFFitFunctionGeneralGaussianDistVar::getAdditionalPlotCount(const double* params) const {

    Q_UNUSED(params);
    return 0;
    // we have one additional plot
}

QString QFFitFunctionGeneralGaussianDistVar::transformParametersForAdditionalPlot(int plot, double* params) const {


    Q_UNUSED(plot);
    Q_UNUSED(params);
    return "";
}

bool QFFitFunctionGeneralGaussianDistVar::get_implementsDerivatives() const
{
    return false;
}

bool QFFitFunctionGeneralGaussianDistVar::estimateInitial(double *params, const double *dataX, const double *dataY, long N, const bool* /*fix*/) const
{
    //statisticsMinMax(dataY, N, params[PARAM_BASE], params[PARAM_MAX]);

    if (params && dataX && dataY) {

        StatisticsScopedPointer<double> dX(statisticsDuplicateArray(dataX, N));
        StatisticsScopedPointer<double> dY(statisticsDuplicateArray(dataY, N));

        statisticsSort2(dX.data(), dY.data(), N);

        double mi=0, ma=0;

        statisticsMinMax(dY.data(), N, mi, ma);

        double p=statisticsXatY50Sorted(dX.data(), dY.data(), N);
        double range=(dX[N-1]-dX[0])/5.0;
        if (dY[N-1]-dY[0]<0) {
            range=range*-1.0;
        }

        params[PARAM_OFFSET]=mi;
        params[PARAM_AMPLITUDE]=ma-mi;
        params[PARAM_POSITION]=p;
        params[PARAM_WIDTH]=range/5.0;


    }


    return true;
}
