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

#include "qffitfunctionsspimfccsdiff.h"

#include <cmath>
#define sqr(x) ((x)*(x))
#define cube(x) ((x)*(x)*(x))
#define pow4(x) ((x)*(x)*(x)*(x))
#define pow5(x) ((x)*(x)*(x)*(x)*(x))
#define NAVOGADRO (6.02214179e23)

QFFitFunctionsSPIMFCCSDiff::QFFitFunctionsSPIMFCCSDiff() {
    //           type,         id,                        name,                                                    label,                      unit,          unitlabel,               fit,       userEditable, userRangeEditable, displayError,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(FloatNumber,  "n_particle",              "particle number N",                                     "N",                        "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 1,            1e-10,    1e5,      1,   0);
    #define FCCSDiff_n_particle 0
    addParameter(FloatNumber,  "1n_particle",             "1/particle number N",                                   "1/N",                      "",            "",                      false,     false,        false,             QFFitFunction::DisplayError, false, 0.01,         1e-10,    1e5,      0.1, 0);
    #define FCCSDiff_1n_particle 1
    addParameter(FloatNumber,  "diff_coeff1",             "diffusion coefficient of species 1",                    "D<sub>1</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    true,      true,         true,              QFFitFunction::DisplayError, false, 10,           1e-10,    1e50,     1    );
    #define FCCSDiff_diff_coeff1 2
    addParameter(FloatNumber,  "offset",                  "correlation offset",                                    "G<sub>&infin;</sub>",      "",           "",                       true,      true,         true,              QFFitFunction::DisplayError, true, 0,            -10,      10,       0.1  );
    #define FCCSDiff_offset 3
    addParameter(FloatNumber,  "focus_distance_x",         "foci: lateral distance in x-direction",                 "d<sub>x</sub>",            "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 1000,           -1e6,     1e6,      10  );
    #define FCCSDiff_focus_distancex 4
    addParameter(FloatNumber,  "focus_distance_y",         "foci: lateral distance in y-direction",                 "d<sub>y</sub>",            "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 0,              -1e6,     1e6,      10  );
    #define FCCSDiff_focus_distancey 5
    addParameter(FloatNumber,  "focus_distance",         "foci: lateral distance",                                  "d<sub>xy</sub>",            "nm",         "nm",                     false,      false,         false,              QFFitFunction::DisplayError,    true, 1000,              0,     1e6,      10  );
    #define FCCSDiff_focus_distance 6
    addParameter(FloatNumber,  "focus_height",            "PSF: axial radius (1/sqrt(e) radius)",                             "&sigma;<sub>z</sub>",      "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 620,         0.01,     1e5,      10  );
    #define FCCSDiff_focus_height 7
    addParameter(FloatNumber,  "focus_width",             "PSF: lateral radius (1/sqrt(e) radius)",                       "&sigma;<sub>x,y</sub>",    "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 300,          0,        1e4,      10    );
    #define FCCSDiff_focus_width 8
    addParameter(FloatNumber,  "pixel_width",             "pixel width",                                           "a",                        "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 400,          0,        1e4,      10    );
    #define FCCSDiff_pixel_width 9
    addParameter(FloatNumber,  "focus_volume",            "focus: effective colume",                               "V<sub>eff</sub>",          "fl",         "fl",                     false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define FCCSDiff_focus_volume 10
    addParameter(FloatNumber,  "concentration",           "particle concentration in focus",                       "C<sub>all</sub>",          "nM",         "nM",                     false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define FCCSDiff_concentration 11
    addParameter(FloatNumber,  "count_rate",              "count rate during measurement",                         "count rate",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError,    false, 0,            0,        1e50,     1    );
    #define FCCSDiff_count_rate 12
    addParameter(FloatNumber,  "background",              "background count rate during measurement",              "background",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError  ,  false, 0,            0,        1e50,     1    );
    #define FCCSDiff_background 13
    addParameter(FloatNumber,  "cpm",                     "photon counts per molecule",                            "cnt/molec",                "Hz",         "Hz",                     false,    false,        false,              QFFitFunction::DisplayError, false, 0,            0,        1e50,     1    );
    #define FCCSDiff_cpm 14

}

double QFFitFunctionsSPIMFCCSDiff::evaluate(double t, const double* data) const {
    const double N=data[FCCSDiff_n_particle];
    const double D1=data[FCCSDiff_diff_coeff1];

    const double dx=data[FCCSDiff_focus_distancex]/1000.0;
    const double dy=data[FCCSDiff_focus_distancey]/1000.0;

    const double sigmaz=data[FCCSDiff_focus_height]/1.0e3;
    const double wxy=data[FCCSDiff_focus_width]/1.0e3;
    const double a=data[FCCSDiff_pixel_width]/1.0e3;
    const double offset=data[FCCSDiff_offset];
    const double background=data[FCCSDiff_background];
    const double cr=data[FCCSDiff_count_rate];
    double backfactor=sqr(cr-background)/sqr(cr);
    if (fabs(cr)<1e-15 || fabs(background)<1e-10) backfactor=1;


    double cfac=0;

    const double fac_z1=sqrt(1.0+D1*t/sqr(sigmaz));
    const double dt_sigma2=D1*t+sqr(wxy);
    //const double fac_x=2.0*a*erf(a/2.0/sqrt(dt_sigma2))+4.0*sqrt(dt_sigma2/M_PI)*(exp(-0.25*sqr(a)/dt_sigma2)-1.0);
    const double fac_x=2.0*sqrt(dt_sigma2/M_PI)*(exp(-sqr(a+dx)/4.0/dt_sigma2)+exp(-sqr(a-dx)/4.0/dt_sigma2)-2.0*exp(-sqr(dx)/4.0/dt_sigma2))
                       +(a+dx)*erf((a+dx)/2.0/sqrt(dt_sigma2))
                       +(a-dx)*erf((a-dx)/2.0/sqrt(dt_sigma2))
                       -2.0*(dx)*erf((dx)/2.0/sqrt(dt_sigma2));
    const double fac_y=2.0*sqrt(dt_sigma2/M_PI)*(exp(-sqr(a+dy)/4.0/dt_sigma2)+exp(-sqr(a-dy)/4.0/dt_sigma2)-2.0*exp(-sqr(dy)/4.0/dt_sigma2))
                       +(a+dy)*erf((a+dy)/2.0/sqrt(dt_sigma2))
                       +(a-dy)*erf((a-dy)/2.0/sqrt(dt_sigma2))
                       -2.0*(dy)*erf((dy)/2.0/sqrt(dt_sigma2));
    const double d1=1.0/fac_z1*fac_x*fac_y;

    cfac=d1;

    const double pre=0.25/sqr(a);
    return offset+pre/N*cfac*backfactor;
}

void QFFitFunctionsSPIMFCCSDiff::calcParameter(double* data, double* error) const {
    double N=data[FCCSDiff_n_particle];
    double eN=0;
    //double D1=data[FCCSDiff_diff_coeff1];
    double eD1=0;
    double sigmaz=data[FCCSDiff_focus_height]/1.0e3;
    double esigmaz=0;
    //double wxy=data[FCCSDiff_focus_width]/1.0e3;
    double ewxy=0;
    double a=data[FCCSDiff_pixel_width]/1.0e3;
    double ea=0;
    //double offset=data[FCCSDiff_offset];
    double eoffset=0;

    double dx=data[FCCSDiff_focus_distancex];
    double edx=0;
    double dy=data[FCCSDiff_focus_distancey];
    double edy=0;

    double cps=data[FCCSDiff_count_rate];
    double ecps=0;
    double ecpm=0;
    double background=data[FCCSDiff_background];
    double ebackground=0;

    if (error) {
        eN=error[FCCSDiff_n_particle];
        eD1=error[FCCSDiff_diff_coeff1];
        esigmaz=error[FCCSDiff_focus_height]/1.0e3;
        ewxy=error[FCCSDiff_focus_width]/1.0e3;
        ea=error[FCCSDiff_pixel_width]/1.0e3;
        eoffset=error[FCCSDiff_offset];
        ecps=error[FCCSDiff_count_rate];
        ecpm=error[FCCSDiff_cpm];
        ebackground=error[FCCSDiff_background];
        edx=error[FCCSDiff_focus_distancex];
        edy=error[FCCSDiff_focus_distancey];
    }




    // calculate 1/N
    data[FCCSDiff_1n_particle]=1.0/N;
    if (error) error[FCCSDiff_1n_particle]=fabs(eN/N/N);

    // calculate Veff = 2 * a^2 * sigmaz
    data[FCCSDiff_focus_volume]=2.0*a*a*sigmaz;
    if (error) error[FCCSDiff_focus_volume]=sqrt(sqr(esigmaz*2.0*a*a)+sqr(ea*4.0*a*sigmaz));

    // calculate C = N / Veff
    if (data[FCCSDiff_focus_volume]!=0) data[FCCSDiff_concentration]=N/data[FCCSDiff_focus_volume]/(NAVOGADRO * 1.0e-24); else data[FCCSDiff_concentration]=0;
    if (error) {
        if ((sigmaz!=0)&&(a!=0)) error[FCCSDiff_concentration]=sqrt( sqr(eN/sqr(a)/2.0/sigmaz) + sqr(esigmaz*N/2.0/sqr(a)/sqr(sigmaz)) + sqr(ea*N/cube(a)/sigmaz) )/(NAVOGADRO * 1.0e-24);
        else error[FCCSDiff_concentration]=0;
    }

    // calculate dxy=sqrt(dx^2+dy^2)
    data[FCCSDiff_focus_distance]=sqrt(dx*dx+dy*dy);
    if (error) {
        error[FCCSDiff_focus_distance]=sqrt(sqr(edx*2.0*dx)/(dx*dx+dy*dy)+sqr(edy*2.0*dy)/(dx*dx+dy*dy));
        if (!QFFloatIsOK(error[FCCSDiff_focus_distance])) error[FCCSDiff_focus_distance]=0;
    }



    // calculate CPM = (CPS-background)/N
    data[FCCSDiff_cpm]=(cps-background)/N;
    error[FCCSDiff_cpm]=sqrt(sqr(ecps/N)+sqr(ebackground/N)+sqr(eN*(cps-background)/sqr(N)));

}

bool QFFitFunctionsSPIMFCCSDiff::isParameterVisible(int parameter, const double* data) const {
    return true;
}

unsigned int QFFitFunctionsSPIMFCCSDiff::getAdditionalPlotCount(const double* params) {
    return 0;
}

QString QFFitFunctionsSPIMFCCSDiff::transformParametersForAdditionalPlot(int plot, double* params) {
    return "";
}
