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


#include "qffitfunctiongeneralmultigaussianvar.h"
#include "qfmathtools.h"
#include <cmath>
#include "statistics_tools.h"

QFFitFunctionGeneralMultiGaussianVar::QFFitFunctionGeneralMultiGaussianVar() {
    //           type,         id,                        name,                                                    label (HTML),                      unit,          unitlabel (HTML),        fit,       userEditable, userRangeEditable, displayError, initialFix,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(IntCombo,  "components",              "components",                                            "components",                        "",            "",                      false,      true,         false,              QFFitFunction::NoError,       false, 1,          1,    6,  1,1,6  );
    #define PARAM_COMPONENTS 0
    addParameter(FloatNumber,  "offset",                  "offset",                                                "Y<sub>0</sub>",                     "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 0.0,          -1e10,    1e10,  1  );
    #define PARAM_OFFSET 1

    addParameter(FloatNumber,  "amplitude",               "amplitude 1",                                             "A<sub>1</sub>",                                 "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 0.5,       -1e10,    1e10,  1  );
    #define PARAM_AMPLITUDE1 2
    addParameter(FloatNumber,  "position",                "position 1",                                              "X<sub>1</sub>",                     "",         "",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 0,            -1e10,    1e10,  1  );
    #define PARAM_POSITION1 3
    addParameter(FloatNumber,  "width",                   "1/sqrt(e) width 1",                                        "&sigma;<sub>1</sub>",                          "",         "",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 1,            1e-10,    1e10,  1, 0  );
    #define PARAM_WIDTH1 4
    addParameter(FloatNumber,  "integral",                "integral 1",                                              "I<usb>1</sub>=&int;p<usb>1</sub>dx", "",         "",     false,      false,         false,            QFFitFunction::DisplayError,       false, 0);
    #define PARAM_INT1 5
    addParameter(FloatNumber,  "rel. integral",           "rel. integral 1",                                          "I<usb>1</sub>/&sum;I<sub>i</sub>", "",         "",     false,      false,         false,            QFFitFunction::DisplayError,       false, 0);
    #define PARAM_RINT1 6

    addParameter(FloatNumber,  "amplitude2",               "amplitude 2",                                             "A<sub>2</sub>",                                 "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 0.5,       -1e10,    1e10,  1  );
    #define PARAM_AMPLITUDE2 7
    addParameter(FloatNumber,  "position2",                "position 2",                                              "X<sub>2</sub>",                     "",         "",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 2,            -1e10,    1e10,  1  );
    #define PARAM_POSITION2 8
    addParameter(FloatNumber,  "width2",                   "1/sqrt(e) width 2",                                        "&sigma;<sub>2</sub>",                          "",         "",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 1,            1e-10,    1e10,  1, 0  );
    #define PARAM_WIDTH2 9
    addParameter(FloatNumber,  "integral2",                "integral 2",                                              "I<usb>2</sub>=&int;p<usb>2</sub>dx", "",         "",     false,      false,         false,            QFFitFunction::DisplayError,       false, 0);
    #define PARAM_INT2 10
    addParameter(FloatNumber,  "rel. integral 2",           "rel. integral 2",                                          "I<usb>2</sub>/&sum;I<sub>2</sub>", "",         "",     false,      false,         false,            QFFitFunction::DisplayError,       false, 0);
    #define PARAM_RINT2 11


    addParameter(FloatNumber,  "amplitude3",               "amplitude 3",                                             "A<sub>3</sub>",                                 "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 0.5,       -1e10,    1e10,  1  );
    #define PARAM_AMPLITUDE3 12
    addParameter(FloatNumber,  "position3",                "position 3",                                              "X<sub>3</sub>",                     "",         "",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 4,            -1e10,    1e10,  1  );
    #define PARAM_POSITION3 13
    addParameter(FloatNumber,  "width3",                   "1/sqrt(e) width 3",                                        "&sigma;<sub>3</sub>",                          "",         "",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 1,            1e-10,    1e10,  1, 0  );
    #define PARAM_WIDTH3 14
    addParameter(FloatNumber,  "integral3",                "integral 3",                                              "I<usb>3</sub>=&int;p<usb>3</sub>dx", "",         "",     false,      false,         false,            QFFitFunction::DisplayError,       false, 0);
    #define PARAM_INT3 15
    addParameter(FloatNumber,  "rel. integral 3",           "rel. integral 3",                                          "I<usb>3</sub>/&sum;I<sub>3</sub>", "",         "",     false,      false,         false,            QFFitFunction::DisplayError,       false, 0);
    #define PARAM_RINT3 16

    addParameter(FloatNumber,  "amplitude4",               "amplitude 4",                                             "A<sub>4</sub>",                                 "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 0.5,       -1e10,    1e10,  1  );
    #define PARAM_AMPLITUDE4 17
    addParameter(FloatNumber,  "position4",                "position 4",                                              "X<sub>4</sub>",                     "",         "",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 8,            -1e10,    1e10,  1  );
    #define PARAM_POSITION4 18
    addParameter(FloatNumber,  "width4",                   "1/sqrt(e) width 4",                                        "&sigma;<sub>4</sub>",                          "",         "",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 1,            1e-10,    1e10,  1, 0  );
    #define PARAM_WIDTH4 19
    addParameter(FloatNumber,  "integral4",                "integral 4",                                              "I<usb>4</sub>=&int;p<usb>4</sub>dx", "",         "",     false,      false,         false,            QFFitFunction::DisplayError,       false, 0);
    #define PARAM_INT4 20
    addParameter(FloatNumber,  "rel. integral 4",           "rel. integral 4",                                          "I<usb>4</sub>/&sum;I<sub>4</sub>", "",         "",     false,      false,         false,            QFFitFunction::DisplayError,       false, 0);
    #define PARAM_RINT4 21

    addParameter(FloatNumber,  "amplitude5",               "amplitude 5",                                             "A<sub>5</sub>",                                 "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 0.5,       -1e10,    1e10,  1  );
    #define PARAM_AMPLITUDE5 22
    addParameter(FloatNumber,  "position5",                "position 5",                                              "X<sub>5</sub>",                     "",         "",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 10,            -1e10,    1e10,  1  );
    #define PARAM_POSITION5 23
    addParameter(FloatNumber,  "width5",                   "1/sqrt(e) width 5",                                        "&sigma;<sub>5</sub>",                          "",         "",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 1,            1e-10,    1e10,  1, 0  );
    #define PARAM_WIDTH5 24
    addParameter(FloatNumber,  "integral5",                "integral 5",                                              "I<usb>5</sub>=&int;p<usb>5</sub>dx", "",         "",     false,      false,         false,            QFFitFunction::DisplayError,       false, 0);
    #define PARAM_INT5 25
    addParameter(FloatNumber,  "rel. integral 5",           "rel. integral 5",                                          "I<usb>5</sub>/&sum;I<sub>5</sub>", "",         "",     false,      false,         false,            QFFitFunction::DisplayError,       false, 0);
    #define PARAM_RINT5 26

    addParameter(FloatNumber,  "amplitude6",               "amplitude 6",                                             "A<sub>6</sub>",                                 "",            "",                      true,      true,         true,              QFFitFunction::DisplayError,       false, 0.6,       -1e10,    1e10,  1  );
    #define PARAM_AMPLITUDE6 27
    addParameter(FloatNumber,  "position6",                "position 6",                                              "X<sub>6</sub>",                     "",         "",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 12,            -1e10,    1e10,  1  );
    #define PARAM_POSITION6 28
    addParameter(FloatNumber,  "width6",                   "1/sqrt(e) width 6",                                        "&sigma;<sub>6</sub>",                          "",         "",                   true,      true,         true,              QFFitFunction::DisplayError,       false, 1,            1e-10,    1e10,  1, 0  );
    #define PARAM_WIDTH6 29
    addParameter(FloatNumber,  "integral6",                "integral 6",                                              "I<usb>6</sub>=&int;p<usb>6</sub>dx", "",         "",     false,      false,         false,            QFFitFunction::DisplayError,       false, 0);
    #define PARAM_INT6 30
    addParameter(FloatNumber,  "rel. integral 6",           "rel. integral 6",                                          "I<usb>6</sub>/&sum;I<sub>6</sub>", "",         "",     false,      false,         false,            QFFitFunction::DisplayError,       false, 0);
    #define PARAM_RINT6 31

    addParameter(FloatNumber,  "integral_xmin",             "integral: xmin",                                          "x<sub>min</sub>",                                "",         "",     false,      true,         false,            QFFitFunction::NoError,       false, -DBL_MAX);
    #define PARAM_INT_XMIN 32
    addParameter(FloatNumber,  "integral_xmax",             "integral: xmax",                                          "x<sub>max</sub>",                                "",         "",     false,      true,         false,            QFFitFunction::NoError,       false, DBL_MAX);
    #define PARAM_INT_XMAX 33
}

double QFFitFunctionGeneralMultiGaussianVar::evaluate(double t, const double* data) const {
    const int comps=data[PARAM_COMPONENTS];
    const double offset=data[PARAM_OFFSET];
    double res= offset;
    if (comps>0) res=res+data[PARAM_AMPLITUDE1]*exp(-0.5*qfSqr(t-data[PARAM_POSITION1])/qfSqr(data[PARAM_WIDTH1]));
    if (comps>1) res=res+data[PARAM_AMPLITUDE2]*exp(-0.5*qfSqr(t-data[PARAM_POSITION2])/qfSqr(data[PARAM_WIDTH2]));
    if (comps>2) res=res+data[PARAM_AMPLITUDE3]*exp(-0.5*qfSqr(t-data[PARAM_POSITION3])/qfSqr(data[PARAM_WIDTH3]));
    if (comps>3) res=res+data[PARAM_AMPLITUDE4]*exp(-0.5*qfSqr(t-data[PARAM_POSITION4])/qfSqr(data[PARAM_WIDTH4]));
    if (comps>4) res=res+data[PARAM_AMPLITUDE5]*exp(-0.5*qfSqr(t-data[PARAM_POSITION5])/qfSqr(data[PARAM_WIDTH5]));
    if (comps>5) res=res+data[PARAM_AMPLITUDE6]*exp(-0.5*qfSqr(t-data[PARAM_POSITION6])/qfSqr(data[PARAM_WIDTH6]));

    return res;
}


void QFFitFunctionGeneralMultiGaussianVar::calcParameter(double* data, double* /*error*/) const {
    if (data) {
        double xmin=data[PARAM_INT_XMIN];
        double xmax=data[PARAM_INT_XMAX];
        const double sqrt2pi=sqrt(M_PI*2.0);
        const int comps=data[PARAM_COMPONENTS];

        // int -inf ... inf
        data[PARAM_INT1]=sqrt2pi*data[PARAM_AMPLITUDE1]*data[PARAM_WIDTH1];
        data[PARAM_INT2]=sqrt2pi*data[PARAM_AMPLITUDE2]*data[PARAM_WIDTH2];
        data[PARAM_INT3]=sqrt2pi*data[PARAM_AMPLITUDE3]*data[PARAM_WIDTH3];
        data[PARAM_INT4]=sqrt2pi*data[PARAM_AMPLITUDE4]*data[PARAM_WIDTH4];
        data[PARAM_INT5]=sqrt2pi*data[PARAM_AMPLITUDE5]*data[PARAM_WIDTH5];
        data[PARAM_INT6]=sqrt2pi*data[PARAM_AMPLITUDE6]*data[PARAM_WIDTH6];
        if (xmin>-DBL_MAX/10.0 || xmax<DBL_MAX/10.0) {
            data[PARAM_INT1]=data[PARAM_INT1]*(0.5*(1.0+erf((xmax-data[PARAM_POSITION1])/(data[PARAM_WIDTH1]*M_SQRT2))) - 0.5*(1.0+erf((xmin-data[PARAM_POSITION1])/(data[PARAM_WIDTH1]*M_SQRT2))));
            data[PARAM_INT2]=data[PARAM_INT2]*(0.5*(1.0+erf((xmax-data[PARAM_POSITION2])/(data[PARAM_WIDTH2]*M_SQRT2))) - 0.5*(1.0+erf((xmin-data[PARAM_POSITION2])/(data[PARAM_WIDTH2]*M_SQRT2))));
            data[PARAM_INT3]=data[PARAM_INT3]*(0.5*(1.0+erf((xmax-data[PARAM_POSITION3])/(data[PARAM_WIDTH3]*M_SQRT2))) - 0.5*(1.0+erf((xmin-data[PARAM_POSITION3])/(data[PARAM_WIDTH3]*M_SQRT2))));
            data[PARAM_INT4]=data[PARAM_INT4]*(0.5*(1.0+erf((xmax-data[PARAM_POSITION4])/(data[PARAM_WIDTH4]*M_SQRT2))) - 0.5*(1.0+erf((xmin-data[PARAM_POSITION4])/(data[PARAM_WIDTH4]*M_SQRT2))));
            data[PARAM_INT5]=data[PARAM_INT5]*(0.5*(1.0+erf((xmax-data[PARAM_POSITION5])/(data[PARAM_WIDTH5]*M_SQRT2))) - 0.5*(1.0+erf((xmin-data[PARAM_POSITION5])/(data[PARAM_WIDTH5]*M_SQRT2))));
            data[PARAM_INT6]=data[PARAM_INT6]*(0.5*(1.0+erf((xmax-data[PARAM_POSITION6])/(data[PARAM_WIDTH6]*M_SQRT2))) - 0.5*(1.0+erf((xmin-data[PARAM_POSITION6])/(data[PARAM_WIDTH6]*M_SQRT2))));
        }

        double sum=0;
        if (comps>0) sum+=data[PARAM_INT1];
        if (comps>1) sum+=data[PARAM_INT2];
        if (comps>2) sum+=data[PARAM_INT3];
        if (comps>3) sum+=data[PARAM_INT4];
        if (comps>4) sum+=data[PARAM_INT5];
        if (comps>5) sum+=data[PARAM_INT6];
        data[PARAM_RINT1]=data[PARAM_INT1]/sum;
        data[PARAM_RINT2]=data[PARAM_INT2]/sum;
        data[PARAM_RINT3]=data[PARAM_INT3]/sum;
        data[PARAM_RINT4]=data[PARAM_INT4]/sum;
        data[PARAM_RINT5]=data[PARAM_INT5]/sum;
        data[PARAM_RINT6]=data[PARAM_INT6]/sum;
    }
}

bool QFFitFunctionGeneralMultiGaussianVar::isParameterVisible(int parameter, const double* data) const {
    const int comps=data[PARAM_COMPONENTS];
    switch(parameter) {
        case PARAM_AMPLITUDE2: case PARAM_POSITION2: case PARAM_WIDTH2: case PARAM_INT2: case PARAM_RINT2: return comps>1;
        case PARAM_AMPLITUDE3: case PARAM_POSITION3: case PARAM_WIDTH3: case PARAM_INT3: case PARAM_RINT3: return comps>2;
        case PARAM_AMPLITUDE4: case PARAM_POSITION4: case PARAM_WIDTH4: case PARAM_INT4: case PARAM_RINT4: return comps>3;
        case PARAM_AMPLITUDE5: case PARAM_POSITION5: case PARAM_WIDTH5: case PARAM_INT5: case PARAM_RINT5: return comps>4;
        case PARAM_AMPLITUDE6: case PARAM_POSITION6: case PARAM_WIDTH6: case PARAM_INT6: case PARAM_RINT6: return comps>5;
        default: return true;
    }

    return true;
    // all parameters are visible at all times
}

unsigned int QFFitFunctionGeneralMultiGaussianVar::getAdditionalPlotCount(const double* params) const {

    const int comps=params[PARAM_COMPONENTS];
    if (comps>1) return comps;
    else return 0;
    // we have one additional plot
}

QString QFFitFunctionGeneralMultiGaussianVar::transformParametersForAdditionalPlot(int plot, double* data) const {
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

bool QFFitFunctionGeneralMultiGaussianVar::get_implementsDerivatives() const
{
    return false;
}

