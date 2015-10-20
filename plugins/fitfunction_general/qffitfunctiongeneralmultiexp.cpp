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


#include "qffitfunctiongeneralmultiexp.h"
#include "qfmathtools.h"
#include <cmath>
#include "statistics_tools.h"

QFFitFunctionGeneralMultiExp::QFFitFunctionGeneralMultiExp() {
    //           type,         id,                        name,                                                    label (HTML),                      unit,          unitlabel (HTML),        fit,       userEditable, userRangeEditable, displayError, initialFix,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(IntCombo,  "components",              "components",                                            "components",                        "",            "",                      false,      true,         false,              QFFitFunction::NoError,       false, 1,          1,    6,  1,1,6  );
    #define PARAM_COMPONENTS 0
    addParameter(FloatNumber,  "offset",                  "offset",                                                "Y<sub>0</sub>",                     "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 0.0,          -1e10,    1e10,  1  );
    #define PARAM_OFFSET 1

    addParameter(FloatNumber,  "amplitude",                    "amplitude",                                                  "A",                     "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 1.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_AMPLITUDE1 2
    addParameter(FloatNumber,  "position",                    "position",                                           "X<sub>0</sub>",                  "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 0.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_POSITION1 3
    addParameter(FloatNumber,  "lifetime",                    "lifetime",                                           "&tau;",                          "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 1.0,          0, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_LIFETIME1 4    
    addParameter(FloatNumber,  "amplitude2",                    "amplitude2",                                                  "A<sub>2</sub>",                     "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 1.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_AMPLITUDE2 5
    addParameter(FloatNumber,  "position2",                    "position2",                                           "X<sub>2</sub>",                  "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 0.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_POSITION2 6
    addParameter(FloatNumber,  "lifetime2",                    "lifetime2",                                           "&tau;<sub>2</sub>",                          "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 100.0,          0, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_LIFETIME2 7
    addParameter(FloatNumber,  "amplitude3",                    "amplitude3",                                                  "A<sub>3</sub>",                     "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 1.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_AMPLITUDE3 8
    addParameter(FloatNumber,  "position3",                    "position3",                                           "X<sub>3</sub>",                  "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 0.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_POSITION3 9
    addParameter(FloatNumber,  "lifetime3",                    "lifetime3",                                           "&tau;<sub>3</sub>",                          "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 100.0,          0, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_LIFETIME3 10
    addParameter(FloatNumber,  "amplitude4",                    "amplitude4",                                                  "A<sub>4</sub>",                     "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 1.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_AMPLITUDE4 11
    addParameter(FloatNumber,  "position4",                    "position4",                                           "X<sub>4</sub>",                  "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 0.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_POSITION4 12
    addParameter(FloatNumber,  "lifetime4",                    "lifetime4",                                           "&tau;<sub>4</sub>",                          "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 100.0,          0, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_LIFETIME4 13
    addParameter(FloatNumber,  "amplitude5",                    "amplitude5",                                                  "A<sub>5</sub>",                     "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 1.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_AMPLITUDE5 14
    addParameter(FloatNumber,  "position5",                    "position5",                                           "X<sub>5</sub>",                  "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 0.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_POSITION5 15
    addParameter(FloatNumber,  "lifetime5",                    "lifetime5",                                           "&tau;<sub>5</sub>",                          "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 100.0,          0, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_LIFETIME5 16
    addParameter(FloatNumber,  "amplitude6",                    "amplitude6",                                                  "A<sub>6</sub>",                     "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 1.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_AMPLITUDE6 17
    addParameter(FloatNumber,  "position6",                    "position6",                                           "X<sub>6</sub>",                  "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 0.0,          -DBL_MAX, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_POSITION6 18
    addParameter(FloatNumber,  "lifetime6",                    "lifetime6",                                           "&tau;<sub>6</sub>",                          "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 100.0,          0, DBL_MAX,  1,   -DBL_MAX,    DBL_MAX  );
    #define PARAM_LIFETIME6 19
}

double QFFitFunctionGeneralMultiExp::evaluate(double t, const double* data) const {
    const int comps=data[PARAM_COMPONENTS];
    const double offset=data[PARAM_OFFSET];
    double res= offset;
    if (comps>0) res=res+data[PARAM_AMPLITUDE1]*exp(-(t-data[PARAM_POSITION1])/data[PARAM_LIFETIME1]);
    if (comps>1) res=res+data[PARAM_AMPLITUDE2]*exp(-(t-data[PARAM_POSITION2])/data[PARAM_LIFETIME2]);
    if (comps>2) res=res+data[PARAM_AMPLITUDE3]*exp(-(t-data[PARAM_POSITION3])/data[PARAM_LIFETIME3]);
    if (comps>3) res=res+data[PARAM_AMPLITUDE4]*exp(-(t-data[PARAM_POSITION4])/data[PARAM_LIFETIME4]);
    if (comps>4) res=res+data[PARAM_AMPLITUDE5]*exp(-(t-data[PARAM_POSITION5])/data[PARAM_LIFETIME5]);
    if (comps>5) res=res+data[PARAM_AMPLITUDE6]*exp(-(t-data[PARAM_POSITION6])/data[PARAM_LIFETIME6]);

    return res;
}


void QFFitFunctionGeneralMultiExp::calcParameter(double* /*data*/, double* /*error*/) const {
    
}

bool QFFitFunctionGeneralMultiExp::isParameterVisible(int parameter, const double* data) const {
    const int comps=data[PARAM_COMPONENTS];
    switch(parameter) {
        case PARAM_AMPLITUDE2: case PARAM_POSITION2: case PARAM_LIFETIME2: return comps>1;
        case PARAM_AMPLITUDE3: case PARAM_POSITION3: case PARAM_LIFETIME3: return comps>2;
        case PARAM_AMPLITUDE4: case PARAM_POSITION4: case PARAM_LIFETIME4: return comps>3;
        case PARAM_AMPLITUDE5: case PARAM_POSITION5: case PARAM_LIFETIME5: return comps>4;
        case PARAM_AMPLITUDE6: case PARAM_POSITION6: case PARAM_LIFETIME6: return comps>5;
        default: return true;
    }

    return true;
    // all parameters are visible at all times
}

unsigned int QFFitFunctionGeneralMultiExp::getAdditionalPlotCount(const double* params) const {

    const int comps=params[PARAM_COMPONENTS];
    if (comps>1) return comps;
    else return 0;
    // we have one additional plot
}

QString QFFitFunctionGeneralMultiExp::transformParametersForAdditionalPlot(int plot, double* data) const {
    if (plot==0) {
        //data[PARAM_AMPLITUDE1]=0;
        data[PARAM_AMPLITUDE2]=0;
        data[PARAM_AMPLITUDE3]=0;
        data[PARAM_AMPLITUDE4]=0;
        data[PARAM_AMPLITUDE5]=0;
        data[PARAM_AMPLITUDE6]=0;
        return QObject::tr("Decay %1").arg(plot+1);
    } else if (plot==1) {
        data[PARAM_AMPLITUDE1]=0;
        //data[PARAM_AMPLITUDE2]=0;
        data[PARAM_AMPLITUDE3]=0;
        data[PARAM_AMPLITUDE4]=0;
        data[PARAM_AMPLITUDE5]=0;
        data[PARAM_AMPLITUDE6]=0;
        return QObject::tr("Decay %1").arg(plot+1);
    } else if (plot==2) {
        data[PARAM_AMPLITUDE1]=0;
        data[PARAM_AMPLITUDE2]=0;
        //data[PARAM_AMPLITUDE3]=0;
        data[PARAM_AMPLITUDE4]=0;
        data[PARAM_AMPLITUDE5]=0;
        data[PARAM_AMPLITUDE6]=0;
        return QObject::tr("Decay %1").arg(plot+1);
    } else if (plot==3) {
        data[PARAM_AMPLITUDE1]=0;
        data[PARAM_AMPLITUDE2]=0;
        data[PARAM_AMPLITUDE3]=0;
        //data[PARAM_AMPLITUDE4]=0;
        data[PARAM_AMPLITUDE5]=0;
        data[PARAM_AMPLITUDE6]=0;
        return QObject::tr("Decay %1").arg(plot+1);
    } else if (plot==4) {
        data[PARAM_AMPLITUDE1]=0;
        data[PARAM_AMPLITUDE2]=0;
        data[PARAM_AMPLITUDE3]=0;
        data[PARAM_AMPLITUDE4]=0;
        //data[PARAM_AMPLITUDE5]=0;
        data[PARAM_AMPLITUDE6]=0;
        return QObject::tr("Decay %1").arg(plot+1);
    } else if (plot==5) {
        data[PARAM_AMPLITUDE1]=0;
        data[PARAM_AMPLITUDE2]=0;
        data[PARAM_AMPLITUDE3]=0;
        data[PARAM_AMPLITUDE4]=0;
        data[PARAM_AMPLITUDE5]=0;
        //data[PARAM_AMPLITUDE6]=0;
        return QObject::tr("Decay %1").arg(plot+1);
    }

    return "";
}

bool QFFitFunctionGeneralMultiExp::get_implementsDerivatives() const
{
    return false;
}

