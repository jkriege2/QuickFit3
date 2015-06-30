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

#include "qffitfunctiongeneralcdfgaussiandistvar.h"
#include "qfmathtools.h"
#include <cmath>
#include "statistics_tools.h"

QFFitFunctionGeneralCDFGaussianDistVar::QFFitFunctionGeneralCDFGaussianDistVar() {
    //           type,         id,                        name,                                                    label (HTML),                      unit,          unitlabel (HTML),        fit,       userEditable, userRangeEditable, displayError, initialFix,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(FloatNumber,  "position",                "position",                                              "X<sub>0</sub>",                        "",         "",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 0,            -1e10,    1e10,  1  );
    #define PARAM_POSITION 0
    addParameter(FloatNumber,  "width",                   "1/sqrt(e) width",                                        "&sigma;",                           "",         "",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 1,            1e-10,    1e10,  1  );
    #define PARAM_WIDTH 1

}

double QFFitFunctionGeneralCDFGaussianDistVar::evaluate(double t, const double* data) const {
    const double position=data[PARAM_POSITION];
    const double width=data[PARAM_WIDTH];
    return 0.5*(1.0+erf((t-position)/sqrt(2.0*qfSqr(width))));
}


void QFFitFunctionGeneralCDFGaussianDistVar::calcParameter(double* /*data*/, double* /*error*/) const {
}

bool QFFitFunctionGeneralCDFGaussianDistVar::isParameterVisible(int parameter, const double* data) const {

    Q_UNUSED(parameter);
    Q_UNUSED(data);
    return true;
    // all parameters are visible at all times
}

unsigned int QFFitFunctionGeneralCDFGaussianDistVar::getAdditionalPlotCount(const double* params) {

    Q_UNUSED(params);
    return 0;
    // we have one additional plot
}

QString QFFitFunctionGeneralCDFGaussianDistVar::transformParametersForAdditionalPlot(int plot, double* params) {


    Q_UNUSED(plot);
    Q_UNUSED(params);
    return "";
}

bool QFFitFunctionGeneralCDFGaussianDistVar::get_implementsDerivatives()
{
    return false;
}

bool QFFitFunctionGeneralCDFGaussianDistVar::estimateInitial(double *params, const double *dataX, const double *dataY, long N, const bool* /*fix*/)
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

        params[PARAM_POSITION]=p;
        params[PARAM_WIDTH]=range/5.0;


    }


    return true;
}
