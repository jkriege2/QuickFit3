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


#include "qffitfunctiongeneralmultilognormal.h"
#include "qfmathtools.h"
#include "statistics_tools.h"
#include <cmath>


QFFitFunctionGeneralMultiLogNormal::QFFitFunctionGeneralMultiLogNormal():
    QFDistributionFitFunctionBase(1.0/2.3548, true)
{
    //           type,         id,                        name,                                                    label (HTML),                      unit,          unitlabel (HTML),        fit,       userEditable, userRangeEditable, displayError, initialFix,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(IntCombo,  "components",              "components",                                            "components",                        "",            "",                      false,      true,         false,              QFFitFunction::NoError,       false, 1,          1,    6,  1,1,6  );
    #define PARAM_COMPONENTS 0
    addParameter(FloatNumber,  "offset",                  "offset",                                                "Y<sub>0</sub>",                     "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 0.0,          -1e10,    1e10,  1  );
    #define PARAM_OFFSET 1

    addParameter(FloatNumber,  "amplitude",               "amplitude 1",                                             "A<sub>1</sub>",                                 "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 0.5,       -1e10,    1e10,  1  );
    #define PARAM_AMPLITUDE1 2
    addParameter(FloatNumber,  "position",                "position 1",                                              "X<sub>1</sub>",                     "",         "",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 0,0,    1e10,  1,0  );
    #define PARAM_POSITION1 3
    addParameter(FloatNumber,  "width",                   "1/sqrt(e) width 1",                                        "&sigma;<sub>1</sub>",                          "",         "",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 1,            1e-10,    1e10,  1, 0  );
    #define PARAM_WIDTH1 4


    addParameter(FloatNumber,  "amplitude2",               "amplitude 2",                                             "A<sub>2</sub>",                                 "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 0.5,       -1e10,    1e10,  1  );
    #define PARAM_AMPLITUDE2 5
    addParameter(FloatNumber,  "position2",                "position 2",                                              "X<sub>2</sub>",                     "",         "",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 2,0,    1e10,  1,0  );
    #define PARAM_POSITION2 6
    addParameter(FloatNumber,  "width2",                   "1/sqrt(e) width 2",                                        "&sigma;<sub>2</sub>",                          "",         "",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 1,            1e-10,    1e10,  1, 0  );
    #define PARAM_WIDTH2 7



    addParameter(FloatNumber,  "amplitude3",               "amplitude 3",                                             "A<sub>3</sub>",                                 "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 0.5,       -1e10,    1e10,  1  );
    #define PARAM_AMPLITUDE3 8
    addParameter(FloatNumber,  "position3",                "position 3",                                              "X<sub>3</sub>",                     "",         "",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 6,0,    1e10,  1,0  );
    #define PARAM_POSITION3 9
    addParameter(FloatNumber,  "width3",                   "1/sqrt(e) width 3",                                        "&sigma;<sub>3</sub>",                          "",         "",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 1,            1e-10,    1e10,  1, 0  );
    #define PARAM_WIDTH3 10

    addParameter(FloatNumber,  "amplitude4",               "amplitude 4",                                             "A<sub>4</sub>",                                 "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 0.5,       -1e10,    1e10,  1  );
    #define PARAM_AMPLITUDE4 11
    addParameter(FloatNumber,  "position4",                "position 4",                                              "X<sub>4</sub>",                     "",         "",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 8,0,    1e10,  1,0  );
    #define PARAM_POSITION4 12
    addParameter(FloatNumber,  "width4",                   "1/sqrt(e) width 4",                                        "&sigma;<sub>4</sub>",                          "",         "",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 1,            1e-10,    1e10,  1, 0  );
    #define PARAM_WIDTH4 13


    addParameter(FloatNumber,  "amplitude5",               "amplitude 5",                                             "A<sub>5</sub>",                                 "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 0.5,       -1e10,    1e10,  1  );
    #define PARAM_AMPLITUDE5 14
    addParameter(FloatNumber,  "position5",                "position 5",                                              "X<sub>5</sub>",                     "",         "",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 10,0,    1e10,  1,0  );
    #define PARAM_POSITION5 15
    addParameter(FloatNumber,  "width5",                   "1/sqrt(e) width 5",                                        "&sigma;<sub>5</sub>",                          "",         "",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 1,            1e-10,    1e10,  1, 0  );
    #define PARAM_WIDTH5 16


    addParameter(FloatNumber,  "amplitude6",               "amplitude 6",                                             "A<sub>6</sub>",                                 "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 0.6,       -1e10,    1e10,  1  );
    #define PARAM_AMPLITUDE6 17
    addParameter(FloatNumber,  "position6",                "position 6",                                              "X<sub>6</sub>",                     "",         "",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 12,0,    1e10,  1,0  );
    #define PARAM_POSITION6 18
    addParameter(FloatNumber,  "width6",                   "1/sqrt(e) width 6",                                        "&sigma;<sub>6</sub>",                          "",         "",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 1,            1e-10,    1e10,  1, 0  );
    #define PARAM_WIDTH6 19


}

double QFFitFunctionGeneralMultiLogNormal::evaluate(double t, const double* data) const {
    const int comps=data[PARAM_COMPONENTS];
    const double offset=data[PARAM_OFFSET];
    double res= offset;
    if (comps>0) res=res+data[PARAM_AMPLITUDE1]*exp(-0.5*qfSqr(log(t)-data[PARAM_POSITION1])/qfSqr(data[PARAM_WIDTH1]));
    if (comps>1) res=res+data[PARAM_AMPLITUDE2]*exp(-0.5*qfSqr(log(t)-data[PARAM_POSITION2])/qfSqr(data[PARAM_WIDTH2]));
    if (comps>2) res=res+data[PARAM_AMPLITUDE3]*exp(-0.5*qfSqr(log(t)-data[PARAM_POSITION3])/qfSqr(data[PARAM_WIDTH3]));
    if (comps>3) res=res+data[PARAM_AMPLITUDE4]*exp(-0.5*qfSqr(log(t)-data[PARAM_POSITION4])/qfSqr(data[PARAM_WIDTH4]));
    if (comps>4) res=res+data[PARAM_AMPLITUDE5]*exp(-0.5*qfSqr(log(t)-data[PARAM_POSITION5])/qfSqr(data[PARAM_WIDTH5]));
    if (comps>5) res=res+data[PARAM_AMPLITUDE6]*exp(-0.5*qfSqr(log(t)-data[PARAM_POSITION6])/qfSqr(data[PARAM_WIDTH6]));
    return res;
}


void QFFitFunctionGeneralMultiLogNormal::calcParameter(double* /*data*/, double* /*error*/) const {

}

bool QFFitFunctionGeneralMultiLogNormal::isParameterVisible(int parameter, const double* data) const {
    const int comps=data[PARAM_COMPONENTS];
    switch(parameter) {
        case PARAM_AMPLITUDE2: case PARAM_POSITION2: case PARAM_WIDTH2: return comps>1;
        case PARAM_AMPLITUDE3: case PARAM_POSITION3: case PARAM_WIDTH3: return comps>2;
        case PARAM_AMPLITUDE4: case PARAM_POSITION4: case PARAM_WIDTH4: return comps>3;
        case PARAM_AMPLITUDE5: case PARAM_POSITION5: case PARAM_WIDTH5: return comps>4;
        case PARAM_AMPLITUDE6: case PARAM_POSITION6: case PARAM_WIDTH6: return comps>5;
        default: return true;
    }

    return true;
}
unsigned int QFFitFunctionGeneralMultiLogNormal::getAdditionalPlotCount(const double* params) const {
    const int comps=params[PARAM_COMPONENTS];
    if (comps>1) return comps;
    else return 0;
    // we have one additional plot
}

QString QFFitFunctionGeneralMultiLogNormal::transformParametersForAdditionalPlot(int plot, double* data) const {
    if (plot==0) {
        //data[PARAM_AMPLITUDE1]=0;
        data[PARAM_AMPLITUDE2]=0;
        data[PARAM_AMPLITUDE3]=0;
        data[PARAM_AMPLITUDE4]=0;
        data[PARAM_AMPLITUDE5]=0;
        data[PARAM_AMPLITUDE6]=0;
        return QObject::tr("Peak %1").arg(plot+1);
    } else if (plot==1) {
        data[PARAM_AMPLITUDE1]=0;
        //data[PARAM_AMPLITUDE2]=0;
        data[PARAM_AMPLITUDE3]=0;
        data[PARAM_AMPLITUDE4]=0;
        data[PARAM_AMPLITUDE5]=0;
        data[PARAM_AMPLITUDE6]=0;
        return QObject::tr("Peak %1").arg(plot+1);
    } else if (plot==2) {
        data[PARAM_AMPLITUDE1]=0;
        data[PARAM_AMPLITUDE2]=0;
        //data[PARAM_AMPLITUDE3]=0;
        data[PARAM_AMPLITUDE4]=0;
        data[PARAM_AMPLITUDE5]=0;
        data[PARAM_AMPLITUDE6]=0;
        return QObject::tr("Peak %1").arg(plot+1);
    } else if (plot==3) {
        data[PARAM_AMPLITUDE1]=0;
        data[PARAM_AMPLITUDE2]=0;
        data[PARAM_AMPLITUDE3]=0;
        //data[PARAM_AMPLITUDE4]=0;
        data[PARAM_AMPLITUDE5]=0;
        data[PARAM_AMPLITUDE6]=0;
        return QObject::tr("Peak %1").arg(plot+1);
    } else if (plot==4) {
        data[PARAM_AMPLITUDE1]=0;
        data[PARAM_AMPLITUDE2]=0;
        data[PARAM_AMPLITUDE3]=0;
        data[PARAM_AMPLITUDE4]=0;
        //data[PARAM_AMPLITUDE5]=0;
        data[PARAM_AMPLITUDE6]=0;
        return QObject::tr("Peak %1").arg(plot+1);
    } else if (plot==5) {
        data[PARAM_AMPLITUDE1]=0;
        data[PARAM_AMPLITUDE2]=0;
        data[PARAM_AMPLITUDE3]=0;
        data[PARAM_AMPLITUDE4]=0;
        data[PARAM_AMPLITUDE5]=0;
        //data[PARAM_AMPLITUDE6]=0;
        return QObject::tr("Peak %1").arg(plot+1);
    }

    return "";
}

bool QFFitFunctionGeneralMultiLogNormal::get_implementsDerivatives() const
{
    return false;
}


