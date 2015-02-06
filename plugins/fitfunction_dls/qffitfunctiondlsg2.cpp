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

#include "qffitfunctiondlsg2.h"

#include <cmath>
#include "qfmathtools.h"
#define sqr(x) ((x)*(x))
#define cube(x) ((x)*(x)*(x))
#define pow4(x) ((x)*(x)*(x)*(x))
#define pow5(x) ((x)*(x)*(x)*(x)*(x))


QFFitFunctionDLSG2::QFFitFunctionDLSG2() {
    //           type,         id,                        name,                                                    label,                      unit,          unitlabel,               fit,       userEditable, userRangeEditable, displayError,                initialFix, initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(IntCombo,     "n_components",            "number of diffusing components",                        "components",               "",            "",                      false,     true,         false,             QFFitFunction::NoError,      false, 1,            1,        3,        1,   1,      2);
    #define DLSG2_n_components 0
    addParameter(FloatNumber,  "epsilon",                 "baseline",                                              "&epsilon;",                "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 0,            -1e50,    1e50,     0.01);
    #define DLSG2_epsilon 1

    addParameter(FloatNumber,  "frac1",                   "fraction of component 1",                               "&alpha;<sub>1</sub>",      "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 1,            0,        1e10,     0.1, 0 );
    #define DLSG2_frac1 2
    addParameter(FloatNumber,  "tau1",                    "correlation time 1",                                    "&tau;<sub>1</sub>",        "탎",         "&mu;s",                  true,      true,         true,              QFFitFunction::DisplayError, false, 100,          1e-10,    1e50,     1    );
    #define DLSG2_tau1 3
    addParameter(FloatNumber,  "frac2",                   "fraction of component 2",                               "&alpha;<sub>2</sub>",      "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 1,            0,        1e10,     0.1, 0);
    #define DLSG2_frac2 4
    addParameter(FloatNumber,  "tau2",                    "correlation time 2",                                    "&tau;<sub>2</sub>",        "탎",         "&mu;s",                  true,      true,         true,              QFFitFunction::DisplayError, false, 1000,         1e-10,    1e50,     1    );
    #define DLSG2_tau2 5
    addParameter(FloatNumber,  "frac3",                   "fraction of component 3",                               "&alpha;<sub>3</sub>",      "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 1,            0,        1e10,     0.1, 0);
    #define DLSG2_frac3 6
    addParameter(FloatNumber,  "tau3",                    "correlation time 3",                                    "&tau;<sub>3</sub>",        "탎",         "&mu;s",                  true,      true,         true,              QFFitFunction::DisplayError, false, 10000,        1e-10,    1e50,     1    );
    #define DLSG2_tau3 7
    addParameter(FloatNumber,  "frac4",                   "fraction of component 4",                               "&alpha;<sub>4</sub>",      "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 1,            0,        1e10,     0.1, 0);
    #define DLSG2_frac4 8
    addParameter(FloatNumber,  "tau4",                    "correlation time 4",                                    "&tau;<sub>4</sub>",        "탎",         "&mu;s",                  true,      true,         true,              QFFitFunction::DisplayError, false, 100000,       1e-10,    1e50,     1    );
    #define DLSG2_tau4 9

    addParameter(FloatNumber,  "offset",                  "correlation offset",                                    "G<sub>&infin;</sub>",      "",           "",                       true,      true,         true,              QFFitFunction::DisplayError, true, 1,            -10,      10,       0.1  );
    #define DLSG2_offset 10

    addParameter(FloatNumber,  "dls_angle",               "scattering angle",                                      "&Theta;<sub>scatter</sub>", "�",         "�",                      false,     true,        false,              QFFitFunction::EditError,    false, 90,          0,        1e4,      1    );
    #define DLSG2_angle 11
    addParameter(FloatNumber,  "refractive_index",        "refractive index",                                      "n",                        "",           "",                       false,     true,        false,              QFFitFunction::EditError,    false, 1.33,        0,        1e4,      1    );
    #define DLSG2_refractive 12
    addParameter(FloatNumber,  "wavelength",              "scattering light wavelength",                           "&lambda;",                 "nm",         "nm",                     false,     true,        false,              QFFitFunction::EditError,    false, 488,         0,        1e4,      1    );
    #define DLSG2_wavelength 13
    addParameter(FloatNumber,  "q_square",                "q square",                                              "q<sup>2</sup>",            "1/nm^2",      "nm<sup>-2</sup>",        false,    false,        false,              QFFitFunction::DisplayError, false, 500,          -1e50,    1e50,     1    );
    #define DLSG2_diff_qsquare 14

    addParameter(FloatNumber,  "diff_coeff1",             "diffusion coefficient of species 1",                    "D<sub>1</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    false,    false,        false,              QFFitFunction::DisplayError, false, 500,          0,        1e50,     1    );
    #define DLSG2_diff_coeff1 15
    addParameter(FloatNumber,  "diff_coeff2",             "diffusion coefficient of species 2",                    "D<sub>2</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    false,    false,        false,              QFFitFunction::DisplayError, false, 500,          0,        1e50,     1    );
    #define DLSG2_diff_coeff2 16
    addParameter(FloatNumber,  "diff_coeff3",             "diffusion coefficient of species 3",                    "D<sub>3</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    false,    false,        false,              QFFitFunction::DisplayError, false, 500,          0,        1e50,     1    );
    #define DLSG2_diff_coeff3 17
    addParameter(FloatNumber,  "diff_coeff4",             "diffusion coefficient of species 4",                    "D<sub>4</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    false,    false,        false,              QFFitFunction::DisplayError, false, 500,          0,        1e50,     1    );
    #define DLSG2_diff_coeff4 18

}

double QFFitFunctionDLSG2::evaluate(double t, const double* data) const {
    const int comp=data[DLSG2_n_components];
    const double epsilon=data[DLSG2_epsilon];
    const double tau1=data[DLSG2_tau1]/1e6;
    const double frac1=data[DLSG2_frac1];
    const double tau2=data[DLSG2_tau2]/1e6;
    const double frac2=data[DLSG2_frac2];
    const double tau3=data[DLSG2_tau3]/1e6;
    const double frac3=data[DLSG2_frac3];
    const double tau4=data[DLSG2_tau4]/1e6;
    const double frac4=data[DLSG2_frac4];
    const double offset=data[DLSG2_offset];

    double g1=frac1*exp(-t/tau1);
    if (comp>1) g1+=frac2*exp(-t/tau2);
    if (comp>2) g1+=frac3*exp(-t/tau3);
    if (comp>3) g1+=frac4*exp(-t/tau4);

    return offset+epsilon+sqr(g1);
}


void QFFitFunctionDLSG2::calcParameter(double* data, double* error) const {
    const int comp=data[DLSG2_n_components];
    const double tau1=data[DLSG2_tau1]/1e6;
    double etau1=0;
    const double tau2=data[DLSG2_tau2]/1e6;
    double etau2=0;
    const double tau3=data[DLSG2_tau3]/1e6;
    double etau3=0;
    const double tau4=data[DLSG2_tau4]/1e6;
    double etau4=0;

    const double n=data[DLSG2_refractive];
    double en=0;
    const double theta=data[DLSG2_angle]/180.0*M_PI;
    double etheta=0;
    const double lambda=data[DLSG2_wavelength]/1e3;
    double elambda=0;
    if (error) {
        en=error[DLSG2_refractive];
        etheta=error[DLSG2_angle]/180*M_PI;
        elambda=error[DLSG2_wavelength]/1e3;
        etau1=error[DLSG2_tau1]/1e6;
        etau2=error[DLSG2_tau2]/1e6;
        etau3=error[DLSG2_tau3]/1e6;
        etau4=error[DLSG2_tau4]/1e6;
    }

    const double q=4.0*M_PI*n/lambda*sin(theta/2.0);

    data[DLSG2_diff_qsquare]=sqr(q);
    double eq=0;
    if (error) {
        eq=sqrt(sqr(en*4.0*M_PI/lambda*sin(theta/2.0))+sqr(elambda*4.0*M_PI*n/lambda/lambda*sin(theta/2.0))+sqr(etheta*2.0*M_PI/lambda*cos(theta/2.0)));
        error[DLSG2_diff_qsquare]=2*q*eq;
    }


    data[DLSG2_diff_coeff1]=0;
    data[DLSG2_diff_coeff2]=0;
    data[DLSG2_diff_coeff3]=0;
    data[DLSG2_diff_coeff4]=0;
    if (error) {
        error[DLSG2_diff_coeff1]=0;
        error[DLSG2_diff_coeff2]=0;
        error[DLSG2_diff_coeff3]=0;
        error[DLSG2_diff_coeff4]=0;
    }

    data[DLSG2_diff_coeff1]=1.0/q/q/tau1;
    if (error) error[DLSG2_diff_coeff1]=sqrt(sqr(eq*2.0/(cube(q)*tau1))+sqr(etau1/(sqr(q)*sqr(tau1))));

    if (comp>1) {
        data[DLSG2_diff_coeff2]=1.0/q/q/tau2;
        if (error) error[DLSG2_diff_coeff2]=sqrt(sqr(eq*2.0/(cube(q)*tau2))+sqr(etau2/(sqr(q)*sqr(tau2))));

        if (comp>2) {
            data[DLSG2_diff_coeff3]=1.0/q/q/tau3;
            if (error) error[DLSG2_diff_coeff3]=sqrt(sqr(eq*2.0/(cube(q)*tau3))+sqr(etau3/(sqr(q)*sqr(tau3))));

            if (comp>3) {
                data[DLSG2_diff_coeff4]=1.0/q/q/tau4;
                if (error) error[DLSG2_diff_coeff4]=sqrt(sqr(eq*2.0/(cube(q)*tau4))+sqr(etau4/(sqr(q)*sqr(tau4))));
            }
        }
    }

}

bool QFFitFunctionDLSG2::isParameterVisible(int parameter, const double* data) const {
    int comp=data[DLSG2_n_components];
    switch(parameter) {
        case DLSG2_frac2:  case DLSG2_tau2:  case DLSG2_diff_coeff2: return comp>1;
        case DLSG2_frac3:  case DLSG2_tau3:  case DLSG2_diff_coeff3: return comp>2;
        case DLSG2_frac4:  case DLSG2_tau4:  case DLSG2_diff_coeff4: return comp>3;
    }
    return true;
}


