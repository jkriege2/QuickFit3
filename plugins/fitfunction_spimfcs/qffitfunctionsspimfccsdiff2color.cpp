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

#include "qffitfunctionsspimfccsdiff2color.h"
#include "qfmathtools.h"
#include <cmath>
#define sqr(x) qfSqr(x)
#define cube(x) qfCube(x)
#define pow4(x) qfPow4(x)
#define NAVOGADRO QF_NAVOGADRO

QFFitFunctionsSPIMFCCSDiff2Color::QFFitFunctionsSPIMFCCSDiff2Color() {
    //           type,         id,                        name,                                                   label,                      unit,          unitlabel,               fit,       userEditable, userRangeEditable, displayError,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(FloatNumber,  "concentration_a",         "particle concentration of species a in focus",         "C<sub>a</sub>",           "nM",         "nM",                     true,      true,          true,              QFFitFunction::DisplayError, false, 10,          0,        1e50,     1    );
    #define FCCSDiff_concentrationa 0
    addParameter(FloatNumber,  "concentration_b",         "particle concentration of species b in focus",         "C<sub>b</sub>",           "nM",         "nM",                     true,      true,          true,              QFFitFunction::DisplayError, false, 10,          0,        1e50,     1    );
    #define FCCSDiff_concentrationb 1
    addParameter(FloatNumber,  "concentration_ab",         "particle concentration of species ab in focus",       "C<sub>ab</sub>",           "nM",         "nM",                     true,      true,          true,              QFFitFunction::DisplayError, false, 5,          0,        1e50,     1    );
    #define FCCSDiff_concentrationab 2
    addParameter(FloatNumber,  "diff_coeffc",             "diffusion coefficient of species ab",                  "D<sub>ab</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    true,      true,         true,              QFFitFunction::DisplayError, false, 10,           1e-10,    1e50,     1    );
    #define FCCSDiff_diff_coeff1 3
    addParameter(FloatNumber,  "offset",                  "correlation offset",                                   "G<sub>&infin;</sub>",      "",           "",                       true,      true,         true,              QFFitFunction::DisplayError, true, 0,            -10,      10,       0.1  );
    #define FCCSDiff_offset 4
    addParameter(FloatNumber,  "focus_distance_x",         "foci: lateral distance in x-direction",               "d<sub>x</sub>",            "nm",         "nm",                     true,      true,         true,             QFFitFunction::EditError,    true, 0,           -1e6,     1e6,      10  );
    #define FCCSDiff_focus_distancex 5
    addParameter(FloatNumber,  "focus_distance_y",         "foci: lateral distance in y-direction",               "d<sub>y</sub>",            "nm",         "nm",                     true,      true,         true,             QFFitFunction::EditError,    true, 0,              -1e6,     1e6,      10  );
    #define FCCSDiff_focus_distancey 6
    addParameter(FloatNumber,  "focus_distance_z",         "foci: longitudinal distance in z-direction",          "d<sub>z</sub>",            "nm",         "nm",                     true,      true,         true,             QFFitFunction::EditError,    true, 0,              -1e6,     1e6,      10  );
    #define FCCSDiff_focus_distancez 7
    addParameter(FloatNumber,  "focus_height1",            "PSF1: axial radius (1/sqrt(e) radius)",               "&sigma;<sub>1,z</sub>",      "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 620,         0.01,     1e5,      10  );
    #define FCCSDiff_focus_height1 8
    addParameter(FloatNumber,  "focus_width1",             "PSF1: lateral radius (1/sqrt(e) radius)",             "&sigma;<sub>1,xy</sub>",    "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 300,          0,        1e4,      10    );
    #define FCCSDiff_focus_width1 9
    addParameter(FloatNumber,  "focus_height2",            "PSF2: axial radius (1/sqrt(e) radius)",               "&sigma;<sub>2,z</sub>",      "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 650,         0.01,     1e5,      10  );
    #define FCCSDiff_focus_height2 10
    addParameter(FloatNumber,  "focus_width2",             "PSF2: lateral radius (1/sqrt(e) radius)",             "&sigma;<sub>2,xy</sub>",    "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 330,          0,        1e4,      10    );
    #define FCCSDiff_focus_width2 11
    addParameter(FloatNumber,  "pixel_width",             "pixel width",                                           "a",                        "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 400,          0,        1e4,      10    );
    #define FCCSDiff_pixel_width 12
    addParameter(FloatNumber,  "count_rate1",              "count rate 1 during measurement",                      "count rate 1",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError,    false, 0,            0,        1e50,     1    );
    #define FCCSDiff_count_rate1 13
    addParameter(FloatNumber,  "background1",              "background count rate 1 during measurement",           "background 1",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError  ,  false, 0,            0,        1e50,     1    );
    #define FCCSDiff_background1 14
    addParameter(FloatNumber,  "count_rate2",              "count rate 2 during measurement",                      "count rate 2",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError,    false, 0,            0,        1e50,     1    );
    #define FCCSDiff_count_rate2 15
    addParameter(FloatNumber,  "background2",              "background count rate 2 during measurement",           "background 2",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError  ,  false, 0,            0,        1e50,     1    );
    #define FCCSDiff_background2 16

}

double QFFitFunctionsSPIMFCCSDiff2Color_pixelcorrfactor(double a, double dx, double D, double t, double wg, double wr) {
    const double dt_sigma=M_SQRT2*sqrt(2.0*D*t+wg*wg+wr*wr);
    const double dt_sigma2=qfSqr(dt_sigma);
    return ( (a-dx)*erf((a-dx)/dt_sigma)-2.0*dx*erf(dx/dt_sigma)+(a+dx)*erf((a+dx)/dt_sigma)+
            dt_sigma/QF_SQRTPI*(exp(-qfSqr(a-dx)/dt_sigma2)-2.0*exp(-dx*dx/dt_sigma2)+exp(-qfSqr(a+dx)/dt_sigma2)) )/(2.0*a*a);
}

double QFFitFunctionsSPIMFCCSDiff2Color_zcorrfactor(double dz, double D, double t, double wg, double wr) {
    const double dt_sigma=sqrt(2.0*D*t+wg*wg+wr*wr);
    return exp(-dz*dz/2.0/dt_sigma)/dt_sigma/sqrt(2.0*M_PI);
}

double QFFitFunctionsSPIMFCCSDiff2Color::evaluate(double t, const double* data) const {
    const double cab=data[FCCSDiff_concentrationab]*6.022e-1; // FCCSDiff_concentrationab given in 1e-9*6.022e23 particles/litre but c should be in particles per µm³= particles/10^{-15}litres=1e15 particles/litre
    const double ca=data[FCCSDiff_concentrationa]*6.022e-1; // FCCSDiff_concentrationa given in 1e-9*6.022e23 particles/litre but c should be in particles per µm³= particles/10^{-15}litres=1e15 particles/litre
    const double cb=data[FCCSDiff_concentrationb]*6.022e-1; // FCCSDiff_concentrationb given in 1e-9*6.022e23 particles/litre but c should be in particles per µm³= particles/10^{-15}litres=1e15 particles/litre
    const double D1=data[FCCSDiff_diff_coeff1];

    const double dx=data[FCCSDiff_focus_distancex]/1000.0;
    const double dy=data[FCCSDiff_focus_distancey]/1000.0;
    const double dz=data[FCCSDiff_focus_distancez]/1000.0;

    const double sigmaz1=data[FCCSDiff_focus_height1]/1.0e3;
    const double wxy1=data[FCCSDiff_focus_width1]/1.0e3;
    const double sigmaz2=data[FCCSDiff_focus_height2]/1.0e3;
    const double wxy2=data[FCCSDiff_focus_width2]/1.0e3;
    const double a=data[FCCSDiff_pixel_width]/1.0e3;
    const double offset=data[FCCSDiff_offset];
    const double background1=data[FCCSDiff_background1];
    const double cr1=data[FCCSDiff_count_rate1];
    const double background2=data[FCCSDiff_background2];
    const double cr2=data[FCCSDiff_count_rate2];
    double backfactor=(cr1-background1)/(cr1) * (cr2-background2)/(cr2);
    if (fabs(cr1)<1e-15 || fabs(cr2)<1e-15 || (fabs(background1)<1e-15 && fabs(background2)<1e-15)) backfactor=1;


    double cfac=QFFitFunctionsSPIMFCCSDiff2Color_pixelcorrfactor(a, dx, D1, t, wxy1, wxy2)*
                QFFitFunctionsSPIMFCCSDiff2Color_pixelcorrfactor(a, dy, D1, t, wxy1, wxy2)*
                QFFitFunctionsSPIMFCCSDiff2Color_zcorrfactor(dz, D1, t, sigmaz1, sigmaz2);


    return offset+backfactor*cab/(ca*cb)*cfac;
}

void QFFitFunctionsSPIMFCCSDiff2Color::calcParameter(double* /*data*/, double* /*error*/) const {

}

bool QFFitFunctionsSPIMFCCSDiff2Color::isParameterVisible(int parameter, const double* data) const {

    Q_UNUSED(parameter);
    Q_UNUSED(data);
    return true;
}

unsigned int QFFitFunctionsSPIMFCCSDiff2Color::getAdditionalPlotCount(const double* params) {

    Q_UNUSED(params);
    return 0;
}

QString QFFitFunctionsSPIMFCCSDiff2Color::transformParametersForAdditionalPlot(int plot, double* params) {


    Q_UNUSED(plot);
    Q_UNUSED(params);
    return "";
}
