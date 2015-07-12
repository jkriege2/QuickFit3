/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

    

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

#include "qffitfunctionsspimfcsdiffe2_newveff_relbright.h"
#include <cmath>
#include "qftools.h"
#include "imfcstools.h"
#define sqr(x) qfSqr(x)
#define cube(x) qfCube(x)
#define pow4(x) qfPow4(x)
#define NAVOGADRO QF_NAVOGADRO

QFFitFunctionsSPIMFCSDiffE2NewVeffDiffBright::QFFitFunctionsSPIMFCSDiffE2NewVeffDiffBright() {
    //           type,         id,                        name,                                                    label,                      unit,          unitlabel,               fit,       userEditable, userRangeEditable, displayError,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(IntCombo,     "n_components",            "number of diffusing components",                        "components",               "",            "",                      false,     true,         false,             QFFitFunction::NoError,      false, 1,            1,        3,        1,   1,      2);
    #define FCSSDiff_n_components 0
    addParameter(FloatNumber,  "n_particle",              "particle number N",                                     "N",                        "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 100,          1e-10,    1e5,      1,   0);
    #define FCSSDiff_n_particle 1
    addParameter(FloatNumber,  "1n_particle",             "1/particle number N",                                   "1/N",                      "",            "",                      false,     false,        false,             QFFitFunction::DisplayError, false, 0.01,         1e-10,    1e5,      0.1, 0);
    #define FCSSDiff_1n_particle 2

    addParameter(FloatNumber,  "diff_rho1",               "fraction of first component",                           "&rho;<sub>1</sub>",        "",            "",                      false,     false,        false,             QFFitFunction::DisplayError, false, 0.5,          0,        0.99999,  0.1, 0,      1);
    #define FCSSDiff_diff_rho1 3
    addParameter(FloatNumber,  "diff_coeff1",             "diffusion coefficient of species 1",                    "D<sub>1</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    true,      true,         true,              QFFitFunction::DisplayError, false, 10,           1e-10,    1e50,     1    );
    #define FCSSDiff_diff_coeff1 4
    addParameter(FloatNumber,  "diff_rho2",               "fraction of second component",                          "&rho;<sub>2</sub>",        "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 0.5,          0,        0.99999,  0.1, 0,      1  );
    #define FCSSDiff_diff_rho2 5
    addParameter(FloatNumber,  "rel_brightness2",         "relative brightness of species 2",                      "q<sub>2</sub>",            "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, true, 1,             0,    1e50,    0.1    );
    #define FCSDiff_REL_BRIGHTNESS2 6
    addParameter(FloatNumber,  "diff_coeff2",             "diffusion coefficient of species 2",                    "D<sub>2</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    true,      true,         true,              QFFitFunction::DisplayError, false, 10,           1e-10,    1e50,     1    );
    #define FCSSDiff_diff_coeff2 7
    addParameter(FloatNumber,  "diff_rho3",               "fraction of third component",                           "&rho;<sub>3</sub>",        "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 0.5,          0,        0.99999,  0.1, 0,      1  );
    #define FCSSDiff_diff_rho3 8
    addParameter(FloatNumber,  "rel_brightness3",         "relative brightness of species 3",                      "q<sub>3</sub>",            "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, true, 1,             0,    1e50,     0.1    );
    #define FCSDiff_REL_BRIGHTNESS3 9
    addParameter(FloatNumber,  "diff_coeff3",             "diffusion coefficient of species 3",                    "D<sub>3</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    true,      true,         true,              QFFitFunction::DisplayError, false, 10,           1e-10,    1e50,     1    );
    #define FCSSDiff_diff_coeff3 10

    addParameter(FloatNumber,  "offset",                  "correlation offset",                                    "G<sub>&infin;</sub>",      "",           "",                       true,      true,         true,              QFFitFunction::DisplayError, true, 0,            -10,      10,       0.1  );
    #define FCSSDiff_offset 11
    addParameter(FloatNumber,  "focus_hieght",            "PSF: axial radius (1/e^2 radius)",                                     "w<sub>z</sub>",      "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 1240,         0.01,     1e5,      10  );
    #define FCSSDiff_focus_height 12
    addParameter(FloatNumber,  "focus_width",             "PSF: lateral radius (1/e^2 radius)",                                   "w<sub>x,y</sub>",    "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 600,          0,        1e4,      10    );
    #define FCSSDiff_focus_width 13
    addParameter(FloatNumber,  "pixel_width",             "pixel width",                                           "a",                        "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 400,          0,        1e4,      10    );
    #define FCSSDiff_pixel_width 14
    addParameter(FloatNumber,  "focus_volume",            "focus: effective volume",                               "V<sub>eff</sub>",          "fl",         "fl",                     false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define FCSSDiff_focus_volume 15
    addParameter(FloatNumber,  "concentration",           "particle concentration in focus",                       "C<sub>all</sub>",          "nM",         "nM",                     false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define FCSSDiff_concentration 16
    addParameter(FloatNumber,  "count_rate",              "count rate during measurement",                         "count rate",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError,    false, 0,            0,        1e50,     1    );
    #define FCSSDiff_count_rate 17
    addParameter(FloatNumber,  "background",              "background count rate during measurement",              "background",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError  ,  false, 0,            0,        1e50,     1    );
    #define FCSSDiff_background 18
    addParameter(FloatNumber,  "cpm",                     "photon counts per molecule",                            "cnt/molec",                "Hz",         "Hz",                     false,    false,        false,              QFFitFunction::DisplayError, false, 0,            0,        1e50,     1    );
    #define FCSSDiff_cpm 19


    addParameter(FloatNumber,  "msd_tau1",                "evaluate MSD at this time",                             "&tau;<sub>MSD</sub>",      "ms",         "ms",                     false,    true,         false,              QFFitFunction::NoError  ,   false, 1,            1e-10,        1e50,     1    );
    #define SPIMFCSADIFF_msd_tau 20
    addParameter(FloatNumber,  "msd_at_tau1",              "MSD1 evaluated at given time tau_MSD",                  "MSD1(&tau;<sub>MSD</sub>)",  "micron^2",         "&mu;m<sup>2</sup>",             false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define SPIMFCSADIFF_msd_at_tau1 21
    addParameter(FloatNumber,  "msd_at_tau2",              "MSD2 evaluated at given time tau_MSD",                  "MSD2(&tau;<sub>MSD</sub>)",  "micron^2",         "&mu;m<sup>2</sup>",             false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define SPIMFCSADIFF_msd_at_tau2 22
    addParameter(FloatNumber,  "msd_at_tau3",              "MSD3 evaluated at given time tau_MSD",                  "MSD3(&tau;<sub>MSD</sub>)",  "micron^2",         "&mu;m<sup>2</sup>",             false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define SPIMFCSADIFF_msd_at_tau3 23
    addParameter(FloatNumber,  "effective_area",            "focus: effective area (xy)",                          "A<sub>eff</sub>",          "micron^2",         "&mu;m<sup>2</sup>",       false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define SPIMFCSADIFF_focus_effA 24
    addParameter(FloatNumber,  "msd_time_aeff1",              "time to leave effective focus area with MSD1",      "&tau;<sub>1</sub>(A<sub>eff</sub>)",  "ms",         "ms",             false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define SPIMFCSADIFF_msd_time_aeff1 25
    addParameter(FloatNumber,  "msd_time_aeff2",              "time to leave effective focus area with MSD2",      "&tau;<sub>2</sub>(A<sub>eff</sub>)",  "ms",         "ms",             false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define SPIMFCSADIFF_msd_time_aeff2 26
    addParameter(FloatNumber,  "msd_time_aeff3",              "time to leave effective focus area with MSD3",      "&tau;<sub>3</sub>(A<sub>eff</sub>)",  "ms",         "ms",             false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define SPIMFCSADIFF_msd_time_aeff3 27


}

double QFFitFunctionsSPIMFCSDiffE2NewVeffDiffBright::evaluate(double t, const double* data) const {
    const int comp=data[FCSSDiff_n_components];
    const double N=data[FCSSDiff_n_particle];
    const double D1=data[FCSSDiff_diff_coeff1];
    const double D2=data[FCSSDiff_diff_coeff2];
    const double rho2=data[FCSSDiff_diff_rho2];
    const double D3=data[FCSSDiff_diff_coeff3];
    const double rho3=data[FCSSDiff_diff_rho3];
    const double sigmaz=data[FCSSDiff_focus_height]/1.0e3;
    const double wxy=data[FCSSDiff_focus_width]/1.0e3;
    const double a=data[FCSSDiff_pixel_width]/1.0e3;
    const double offset=data[FCSSDiff_offset];
    const double background=data[FCSSDiff_background];
    const double cr=data[FCSSDiff_count_rate];
    const double q2=data[FCSDiff_REL_BRIGHTNESS2];
    const double q3=data[FCSDiff_REL_BRIGHTNESS3];
    double backfactor=sqr(cr-background)/sqr(cr);
    if (fabs(cr)<1e-15 || fabs(background)<1e-10) backfactor=1;

    double rho1=1;
    double cnorm=rho1;
    if (comp==2) {
        rho1=1.0-rho2;
        cnorm=rho1+rho2*q2;
    }
    if (comp==3) {
        rho1=1.0-rho2-rho3;
        cnorm=rho1+rho2*q2+rho3*q3;
    }

    double cfac=0;
    const double sqpi=sqrt(M_PI);

    if (comp>0) {
        const double fac_z1=sqrt(1.0+4.0*D1*t/sqr(sigmaz));
        const double dt_sigma2=4.0*D1*t+sqr(wxy);
        const double fac_xy1=erf(a/sqrt(dt_sigma2))+sqrt(dt_sigma2)/(sqpi*a)*(exp(-sqr(a)/dt_sigma2)-1.0);
        const double d1=1.0/fac_z1*sqr(fac_xy1);
        cfac=rho1*d1;
    }

    if (comp>1) {
        const double fac_z2=sqrt(1.0+4.0*D2*t/sqr(sigmaz));
        const double dt_sigma22=4.0*D2*t+sqr(wxy);
        const double fac_xy2=erf(a/sqrt(dt_sigma22))+sqrt(dt_sigma22)/(sqpi*a)*(exp(-sqr(a)/dt_sigma22)-1.0);
        const double d2=1.0/fac_z2*sqr(fac_xy2);
        cfac=cfac+rho2*q2*q2*d2;
    }

    if (comp>2) {
        const double fac_z3=sqrt(1.0+4.0*D3*t/sqr(sigmaz));
        const double dt_sigma23=4.0*D3*t+sqr(wxy);
        const double fac_xy3=erf(a/sqrt(dt_sigma23))+sqrt(dt_sigma23)/(sqpi*a)*(exp(-sqr(a)/dt_sigma23)-1.0);
        const double d3=1.0/fac_z3*sqr(fac_xy3);
        cfac=cfac+rho3*q3*q3*d3;
    }

    const double Veff=SPIMFCS_newVeff(a, wxy, sigmaz);
    const double pre=1.0/sqr(a)/sqpi/sigmaz;
    return offset+pre/(N/Veff*qfSqr(cnorm))*cfac*backfactor;
}


void QFFitFunctionsSPIMFCSDiffE2NewVeffDiffBright::calcParameter(double* data, double* error) const {
    double N=data[FCSSDiff_n_particle];
    double eN=0;
    //double D1=data[FCSSDiff_diff_coeff1];
    double eD1=0;
    double sigmaz=data[FCSSDiff_focus_height]/1.0e3;
    double esigmaz=0;
    double wxy=data[FCSSDiff_focus_width]/1.0e3;
    double ewxy=0;
    double a=data[FCSSDiff_pixel_width]/1.0e3;
    double ea=0;
    //double offset=data[FCSSDiff_offset];
    double eoffset=0;

    int comp=data[FCSSDiff_n_components];
    double rho2=data[FCSSDiff_diff_rho2];
    double erho2=0;
    double rho3=data[FCSSDiff_diff_rho3];
    double erho3=0;
    double cps=data[FCSSDiff_count_rate];
    double ecps=0;
    double ecpm=0;
    double background=data[FCSSDiff_background];
    double ebackground=0;

    double tau_msd=data[SPIMFCSADIFF_msd_tau];
    double etau_msd=0;
    const double D1=data[FCSSDiff_diff_coeff1];
    const double D2=data[FCSSDiff_diff_coeff2];
    const double D3=data[FCSSDiff_diff_coeff3];

    if (error) {
        eN=error[FCSSDiff_n_particle];
        eD1=error[FCSSDiff_diff_coeff1];
        esigmaz=error[FCSSDiff_focus_height]/1.0e3;
        ewxy=error[FCSSDiff_focus_width]/1.0e3;
        ea=error[FCSSDiff_pixel_width]/1.0e3;
        eoffset=error[FCSSDiff_offset];
        erho2=error[FCSSDiff_diff_rho2];
        erho3=error[FCSSDiff_diff_rho3];
        ecps=error[FCSSDiff_count_rate];
        ecpm=error[FCSSDiff_cpm];
        ebackground=error[FCSSDiff_background];
    }


    // calculate rho1
    double rho1=1;
    double erho1=0;
    if (comp==2) {
        if (rho2>1.0) rho2=1.0;
        if (rho2<0.0) rho2=0.0;
        if (rho2>1.0) {
            rho1=0;
            rho2=0.5;
        }
        rho1=1.0-rho2;
        erho1=erho2;
    }
    if (comp==3) {
        if (rho2>1.0) rho2=1.0;
        if (rho2<0.0) rho2=0.0;
        if (rho3>1.0) rho3=1.0;
        if (rho3<0.0) rho3=0.0;
        if (rho2+rho3>1.0) {
            rho1=0;
            rho2=rho3=0.5;
        }
        rho1=1.0-rho2-rho3;
        erho1=sqrt(erho2*erho2+erho3*erho3);
    }

    data[FCSSDiff_diff_rho1]=rho1;
    data[FCSSDiff_diff_rho2]=rho2;
    data[FCSSDiff_diff_rho3]=rho3;
    if (error) {
        error[FCSSDiff_diff_rho1]=erho1;
        error[FCSSDiff_diff_rho2]=erho2;
        error[FCSSDiff_diff_rho3]=erho3;
    }


    // calculate 1/N
    data[FCSSDiff_1n_particle]=1.0/N;
    if (error) error[FCSSDiff_1n_particle]=fabs(eN/N/N);

    //const double pi32=pow(M_PI, 3.0/2.0);
    const double sqpi=sqrt(M_PI);
    // calculate Veff
    data[FCSSDiff_focus_volume]=SPIMFCS_newVeff(a, wxy, sigmaz);;
    if (error) error[FCSSDiff_focus_volume]=SPIMFCS_newVeffError(a, ea, wxy, ewxy, sigmaz, esigmaz);

    // calculate C = N / Veff
    if (data[FCSSDiff_focus_volume]!=0) {
        data[FCSSDiff_concentration]=N/data[FCSSDiff_focus_volume]/(NAVOGADRO * 1.0e-24);
    } else {
        data[FCSSDiff_concentration]=0;
    }
    if (data[FCSSDiff_focus_volume]!=0 && error) {
        error[FCSSDiff_concentration]=sqrt(qfSqr(eN/data[FCSSDiff_focus_volume]/(NAVOGADRO * 1.0e-24))+qfSqr(error[FCSSDiff_focus_volume]*N/qfSqr(data[FCSSDiff_focus_volume])/(NAVOGADRO * 1.0e-24)));//N/data[FCSSDiff_focus_volume]/(NAVOGADRO * 1.0e-24)
    }


    // calculate CPM = (CPS-background)/N
    data[FCSSDiff_cpm]=(cps-background)/N;
    error[FCSSDiff_cpm]=sqrt(sqr(ecps/N)+sqr(ebackground/N)+sqr(eN*(cps-background)/sqr(N)));

    double Aeff=data[SPIMFCSADIFF_focus_effA]=TIRFCS_newAeff(a, wxy);
    if (error) error[SPIMFCSADIFF_focus_effA]=TIRFCS_newAeffError(a, ea, wxy, ewxy);
    data[SPIMFCSADIFF_msd_at_tau1]=6.0*D1*tau_msd*1e-3;
    if (error) error[SPIMFCSADIFF_msd_at_tau1]=0;
    data[SPIMFCSADIFF_msd_at_tau2]=6.0*D2*tau_msd*1e-3;
    if (error) error[SPIMFCSADIFF_msd_at_tau2]=0;
    data[SPIMFCSADIFF_msd_at_tau3]=6.0*D3*tau_msd*1e-3;
    if (error) error[SPIMFCSADIFF_msd_at_tau3]=0;
    data[SPIMFCSADIFF_msd_time_aeff1]=Aeff/(4.0*D1)*1e3;
    if (error) error[SPIMFCSADIFF_msd_time_aeff1]=0;
    data[SPIMFCSADIFF_msd_time_aeff2]=Aeff/(4.0*D2)*1e3;
    if (error) error[SPIMFCSADIFF_msd_time_aeff2]=0;
    data[SPIMFCSADIFF_msd_time_aeff3]=Aeff/(4.0*D3)*1e3;
    if (error) error[SPIMFCSADIFF_msd_time_aeff3]=0;

}

bool QFFitFunctionsSPIMFCSDiffE2NewVeffDiffBright::isParameterVisible(int parameter, const double* data) const {
    int comp=data[FCSSDiff_n_components];
    switch(parameter) {
        case FCSSDiff_diff_rho1:  return comp>1;
        case FCSSDiff_diff_coeff1: return comp>0;
        case FCSSDiff_diff_rho2: case FCSSDiff_diff_coeff2: case SPIMFCSADIFF_msd_at_tau2: case SPIMFCSADIFF_msd_time_aeff2: case FCSDiff_REL_BRIGHTNESS2: return comp>1;
        case FCSSDiff_diff_rho3: case FCSSDiff_diff_coeff3: case SPIMFCSADIFF_msd_at_tau3: case SPIMFCSADIFF_msd_time_aeff3: case FCSDiff_REL_BRIGHTNESS3: return comp>2;
    }
    return true;
}

unsigned int QFFitFunctionsSPIMFCSDiffE2NewVeffDiffBright::getAdditionalPlotCount(const double* params) const {

    Q_UNUSED(params);
    return 0;
}

QString QFFitFunctionsSPIMFCSDiffE2NewVeffDiffBright::transformParametersForAdditionalPlot(int plot, double* params) const {


    Q_UNUSED(plot);
    Q_UNUSED(params);
    return "";
}
