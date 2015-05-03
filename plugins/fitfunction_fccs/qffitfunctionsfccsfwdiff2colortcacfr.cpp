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

#include "qffitfunctionsfccsfwdiff2colortcacfr.h"
#include "qfmathtools.h"
#include <cmath>
#include "fcstools.h"
#include <QDebug>
QFFitFunctionsFCCSFWDiff2ColorTCACFR::QFFitFunctionsFCCSFWDiff2ColorTCACFR() {
    //           type,         id,                        name,                                                   label,                      unit,          unitlabel,               fit,       userEditable, userRangeEditable, displayError,               initialFIx,  initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(IntCombo,     "n_nonfluorescent",        "number of nonfluorescent components (triplet ...)",     "# non-fluorescent",        "",            "",                      false,     true,         false,             QFFitFunction::NoError,      false, 1,            0,        1,        1,   0,      1);
    #define FCCSDiff_n_nonfluorescent 0

    addParameter(FloatNumber,  "nonfl_tau_a",              "triplet decay time of species a",              "&tau;<sub>trip,a</sub>",     "usec",        "&mu;s",                 true,      true,         true,              QFFitFunction::DisplayError, false, 3.0,          0,        10,       0.1, 0  );
    #define FCCSDiff_nonfl_taua 1
    addParameter(FloatNumber,  "nonfl_theta_a",            "triplet fraction of species a",                "&theta;<sub>trip,a</sub>",   "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 0.2,          0,        0.99999,  0.1, 0,      1);
    #define FCCSDiff_nonfl_thetaa 2
    addParameter(FloatNumber,  "nonfl_tau_b",              "triplet decay time of species b",              "&tau;<sub>trip,b</sub>",     "usec",        "&mu;s",                 true,      true,         true,              QFFitFunction::DisplayError, false, 3.0,          0,        10,       0.1, 0  );
    #define FCCSDiff_nonfl_taub 3
    addParameter(FloatNumber,  "nonfl_theta_b",            "triplet fraction of species b",                "&theta;<sub>trip,b</sub>",   "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 0.2,          0,        0.99999,  0.1, 0,      1);
    #define FCCSDiff_nonfl_thetab 4
    addParameter(FloatNumber,  "nonfl_tau_ab",              "triplet decay time of species ab",              "&tau;<sub>trip,ab</sub>",     "usec",        "&mu;s",                 true,      true,         true,              QFFitFunction::DisplayError, false, 3.0,          0,        10,       0.1, 0  );
    #define FCCSDiff_nonfl_tauab 5
    addParameter(FloatNumber,  "nonfl_theta_ab",            "triplet fraction of species ab",                "&theta;<sub>trip,ab</sub>",   "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 0.2,          0,        0.99999,  0.1, 0,      1);
    #define FCCSDiff_nonfl_thetaab 6

    addParameter(FloatNumber,  "concentration_a",         "concentration of species a in focus",         "C<sub>a</sub>",           "nM",         "nM",                              true,      true,          true,              QFFitFunction::DisplayError, false, 10,          0,        1e50,     1    );
    #define FCCSDiff_concentrationa 7
    addParameter(FloatNumber,  "concentration_b",         "concentration of species b in focus",         "C<sub>b</sub>",           "nM",         "nM",                              true,      true,          true,              QFFitFunction::DisplayError, false, 10,          0,        1e50,     1    );
    #define FCCSDiff_concentrationb 8
    addParameter(FloatNumber,  "concentration_ab",         "concentration of species ab in focus",       "C<sub>ab</sub>",           "nM",         "nM",                             true,      true,          true,              QFFitFunction::DisplayError, false, 5,          0,        1e50,     1    );
    #define FCCSDiff_concentrationab 9
    addParameter(FloatNumber,  "diff_coeff_a",            "diffusion coefficient of species a",                  "D<sub>a</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    true,      true,         true,              QFFitFunction::DisplayError, false, 10,           1e-10,    1e50,     1    );
    #define FCCSDiff_diff_coeffa 10
    addParameter(FloatNumber,  "diff_rho2_a",            "fraction of 2. D of species a",                  "&rho;<sub>a,2</sub>",            "", "",                                 true,      true,         true,              QFFitFunction::DisplayError, false, 0.5,           0,    1,     1, 0, 1    );
    #define FCCSDiff_diff_rho2a 11
    addParameter(FloatNumber,  "diff_coeff2_a",            "diffusion 2 coefficient of species a",                  "D<sub>a,2</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    true,      true,         true,              QFFitFunction::DisplayError, false, 10,           1e-10,    1e50,     1    );
    #define FCCSDiff_diff_coeff2a 12
    addParameter(FloatNumber,  "diff_coeff_b",            "diffusion coefficient of species b",                  "D<sub>b</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    true,      true,         true,              QFFitFunction::DisplayError, false, 10,           1e-10,    1e50,     1    );
    #define FCCSDiff_diff_coeffb 13
    addParameter(FloatNumber,  "diff_rho2_b",            "fraction of 2. D of species b",                  "&rho;<sub>b,2</sub>",            "", "",                                 true,      true,         true,              QFFitFunction::DisplayError, false, 0.5,           0,    1,     1, 0, 1    );
    #define FCCSDiff_diff_rho2b 14
    addParameter(FloatNumber,  "diff_coeff2_b",            "diffusion 2 coefficient of species b",                  "D<sub>b,2</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    true,      true,         true,              QFFitFunction::DisplayError, false, 10,           1e-10,    1e50,     1    );
    #define FCCSDiff_diff_coeff2b 15
    addParameter(FloatNumber,  "diff_coeff_ab",            "diffusion coefficient of species ab",                  "D<sub>ab</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    true,      true,         true,              QFFitFunction::DisplayError, false, 10,           1e-10,    1e50,     1    );
    #define FCCSDiff_diff_coeffab 16
    addParameter(FloatNumber,  "diff_rho2_ab",            "fraction of 2. D of species ab",                  "&rho;<sub>ab,2</sub>",            "", "",                                 true,      true,         true,              QFFitFunction::DisplayError, false, 0.5,           0,    1,     1, 0, 1    );
    #define FCCSDiff_diff_rho2ab 17
    addParameter(FloatNumber,  "diff_coeff2_ab",            "diffusion 2 coefficient of species ab",                  "D<sub>ab,2</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    true,      true,         true,              QFFitFunction::DisplayError, false, 10,           1e-10,    1e50,     1    );
    #define FCCSDiff_diff_coeff2ab 18



    addParameter(FloatNumber,  "offset",                  "correlation offset",                                   "G<sub>&infin;</sub>",      "",           "",                       true,      true,         true,              QFFitFunction::DisplayError, true, 0,            -10,      10,       0.1  );
    #define FCCSDiff_offset 19
    addParameter(FloatNumber,  "crosstalk",               "crosstalk coefficient",                                "&kappa;",                  "",         "",                         true,      true,         true,             QFFitFunction::EditError,    true, 0,           0,     1,      0.1, 0, 1  );
    #define FCCSDiff_crosstalk 20
    addParameter(FloatNumber,  "focus_distance_x",         "foci: lateral distance in x-direction",               "d<sub>x</sub>",            "nm",         "nm",                     true,      true,         true,             QFFitFunction::EditError,    true, 0,           -1e6,     1e6,      10  );
    #define FCCSDiff_focus_distancex 21
    addParameter(FloatNumber,  "focus_distance_y",         "foci: lateral distance in y-direction",               "d<sub>y</sub>",            "nm",         "nm",                     true,      true,         true,             QFFitFunction::EditError,    true, 0,              -1e6,     1e6,      10  );
    #define FCCSDiff_focus_distancey 22
    addParameter(FloatNumber,  "focus_distance_z",         "foci: longitudinal distance in z-direction",          "d<sub>z</sub>",            "nm",         "nm",                     true,      true,         true,             QFFitFunction::EditError,    true, 0,              -1e6,     1e6,      10  );
    #define FCCSDiff_focus_distancez 23
    addParameter(FloatNumber,  "focus_struct_fac1",        "green PSF: structure factor",                        "&gamma;<sub>g</sub>",      "",         "",                     true,      true,         true,              QFFitFunction::EditError,    true, 6,         1e-5,     1e5,      1  );
    #define FCCSDiff_focus_structfac1 24
    addParameter(FloatNumber,  "focus_width1",             "green PSF: lateral radius (1/e^2 radius)",             "w<sub>g</sub>",    "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 250,          0,        1e4,      10    );
    #define FCCSDiff_focus_width1 25
    addParameter(FloatNumber,  "focus_struct_fac2",        "red PSF: structure factor",                        "&gamma;<sub>r</sub>",      "",         "",                     true,      true,         true,              QFFitFunction::EditError,    true, 6,         1e-5,     1e5,      1  );
    #define FCCSDiff_focus_structfac2 26
    addParameter(FloatNumber,  "focus_width2",             "red PSF: lateral radius (1/e^2 radius)",             "w<sub>r</sub>",    "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 270,          0,        1e4,      10    );
    #define FCCSDiff_focus_width2 27
    addParameter(FloatNumber,  "focus_volume1",            "green focus: effective volume",                               "V<sub>eff,g</sub>",          "fl",         "fl",                     false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define FCSSDiff_focus_volume1 28
    addParameter(FloatNumber,  "focus_volume2",            "red focus: effective volume",                               "V<sub>eff,r</sub>",          "fl",         "fl",                     false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define FCSSDiff_focus_volume2 29
    addParameter(FloatNumber,  "count_rate1",              "count rate green, during measurement",                      "&lang;F<sub>g</sub>&rang;",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError,    false, 0,            0,        1e50,     1    );
    #define FCCSDiff_count_rate1 30
    addParameter(FloatNumber,  "background1",              "background count rate green, during measurement",           "B<sub>g</sub>",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError  ,  false, 0,            0,        1e50,     1    );
    #define FCCSDiff_background1 31
    addParameter(FloatNumber,  "count_rate2",              "count rate red, during measurement",                      "&lang;F<sub>r</sub>&rang;",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError,    false, 0,            0,        1e50,     1    );
    #define FCCSDiff_count_rate2 32
    addParameter(FloatNumber,  "background2",              "background count rate red, during measurement",           "B<sub>r</sub>",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError  ,  false, 0,            0,        1e50,     1    );
    #define FCCSDiff_background2 33
    addParameter(FloatNumber,  "brightness_a",             "molar brightness of fluorophore on A",           "&eta;<sub>a</sub>",            "cpm",           "cpm",    false,      false,          false,              QFFitFunction::DisplayError, false, 0.5           );
    #define FCCSDiff_brightness_a 34
    addParameter(FloatNumber,  "brightness_b",             "molar brightness of fluorophore on B",           "&eta;<sub>b</sub>",            "cpm",           "cpm",    false,      false,          false,              QFFitFunction::DisplayError, false, 0.5          );
    #define FCCSDiff_brightness_b 35

}


double QFFitFunctionsFCCSFWDiff2ColorTCACFR::evaluate(double t, const double* data) const {
    const double cab=data[FCCSDiff_concentrationab]*6.022e-1; // FCCSDiff_concentrationab given in 1e-9*6.022e23 particles/litre but c should be in particles per µm³= particles/10^{-15}litres=1e15 particles/litre
    const double ca=data[FCCSDiff_concentrationa]*6.022e-1; // FCCSDiff_concentrationa given in 1e-9*6.022e23 particles/litre but c should be in particles per µm³= particles/10^{-15}litres=1e15 particles/litre
    const double cb=data[FCCSDiff_concentrationb]*6.022e-1; // FCCSDiff_concentrationb given in 1e-9*6.022e23 particles/litre but c should be in particles per µm³= particles/10^{-15}litres=1e15 particles/litre
    const double Da=data[FCCSDiff_diff_coeffa];
    const double Db=data[FCCSDiff_diff_coeffb];
    const double Dab=data[FCCSDiff_diff_coeffab];
    const double D2a=data[FCCSDiff_diff_coeff2a];
    const double D2ab=data[FCCSDiff_diff_coeff2ab];
    const double D2b=data[FCCSDiff_diff_coeff2b];
    const double rho2a=data[FCCSDiff_diff_rho2a];
    const double rho2ab=data[FCCSDiff_diff_rho2ab];
    const double rho2b=data[FCCSDiff_diff_rho2b];

    const double rho1a=1.0-rho2a;
    const double rho1ab=1.0-rho2ab;
    const double rho1b=1.0-rho2b;

    const double dx=data[FCCSDiff_focus_distancex]/1000.0;
    const double dy=data[FCCSDiff_focus_distancey]/1000.0;
    const double dz=data[FCCSDiff_focus_distancez]/1000.0;

    const double wxyG=data[FCCSDiff_focus_width1]/1.0e3;
    const double gammaG=data[FCCSDiff_focus_structfac1];
    const double wzG=gammaG*wxyG;

    const double wxyR=data[FCCSDiff_focus_width2]/1.0e3;
    const double gammaR=data[FCCSDiff_focus_structfac2];
    const double wzR=gammaR*wxyR;

    const double offset=data[FCCSDiff_offset];

    const double kappa=data[FCCSDiff_crosstalk];

    const double background1=data[FCCSDiff_background1];
    const double cr1=data[FCCSDiff_count_rate1];
    const double background2=data[FCCSDiff_background2];
    const double cr2=data[FCCSDiff_count_rate2];
    const int nNonFl=qBound(0, (int)round(data[FCCSDiff_n_nonfluorescent]), 1);
    const double tauT_a=data[FCCSDiff_nonfl_taua]*1e-6;
    const double tauT_b=data[FCCSDiff_nonfl_taub]*1e-6;
    const double tauT_ab=data[FCCSDiff_nonfl_tauab]*1e-6;
    const double thetaT_a=data[FCCSDiff_nonfl_thetaa];
    const double thetaT_b=data[FCCSDiff_nonfl_thetab];
    const double thetaT_ab=data[FCCSDiff_nonfl_thetaab];

    double backfactor=(cr1-background1)/(cr1) * (cr2-background2)/(cr2);
    if (fabs(cr1)<1e-15 || fabs(cr2)<1e-15 || (fabs(background1)<1e-15 && fabs(background2)<1e-15)) backfactor=1;

    const double etaG=(cr1-background1)/(ca+cab);
    const double etaR=(cr2-background2-kappa*(cr1-background1))/(cb+cab);
    const double Fg=etaG*(ca+cab);
    const double Fr=etaR*(cb+cab)+kappa*Fg;

    double Grr_b  =etaR*etaR*cb*   QFFitFunctionFCCSFWTriplet(nNonFl, t, thetaT_b, tauT_b)*    (rho1b* QFFitFunctionsFCCSFWDiff2ColorCCF_corrfactor(0, 0, 0, Db, t, wxyR, wxyR, wzR, wzR)+   rho2b* QFFitFunctionsFCCSFWDiff2ColorCCF_corrfactor(0, 0, 0, D2b, t, wxyR, wxyR, wzR, wzR));
    double Grr_ab =etaR*etaR*cab*  QFFitFunctionFCCSFWTriplet(nNonFl, t, thetaT_ab, tauT_ab)*  (rho1ab*QFFitFunctionsFCCSFWDiff2ColorCCF_corrfactor(0, 0, 0, Dab, t, wxyR, wxyR, wzR, wzR)+  rho2ab*QFFitFunctionsFCCSFWDiff2ColorCCF_corrfactor(0, 0, 0, D2ab, t, wxyR, wxyR, wzR, wzR));
    double Ggg_a  =etaG*etaG*ca*   QFFitFunctionFCCSFWTriplet(nNonFl, t, thetaT_a, tauT_a)*    (rho1a* QFFitFunctionsFCCSFWDiff2ColorCCF_corrfactor(0, 0, 0, Da, t, wxyG, wxyG, wzG, wzG)+   rho2a* QFFitFunctionsFCCSFWDiff2ColorCCF_corrfactor(0, 0, 0, D2a, t, wxyG, wxyG, wzG, wzG));
    double Ggg_ab =etaG*etaG*cab*  QFFitFunctionFCCSFWTriplet(nNonFl, t, thetaT_ab, tauT_ab)*  (rho1ab*QFFitFunctionsFCCSFWDiff2ColorCCF_corrfactor(0, 0, 0, Dab, t, wxyG, wxyG, wzG, wzG)+  rho2ab*QFFitFunctionsFCCSFWDiff2ColorCCF_corrfactor(0, 0, 0, D2ab, t, wxyG, wxyG, wzG, wzG));
    double Ggr_ab =etaG*etaR*cab*                                                              (rho1ab*QFFitFunctionsFCCSFWDiff2ColorCCF_corrfactor(dx, dy, dz, Dab, t, wxyG, wxyR, wzG, wzR)+  rho2ab*QFFitFunctionsFCCSFWDiff2ColorCCF_corrfactor(dx, dy, dz, D2ab, t, wxyG, wxyR, wzG, wzR));
    if (fabs(ca)<1e-15) Ggg_a=0;
    if (fabs(cb)<1e-15) Grr_b=0;
    if (fabs(cab)<1e-15) Grr_ab=Ggg_ab=Ggr_ab=0;

    const double cfac=Grr_b+Grr_ab+qfSqr(kappa)*(Ggg_a+Ggg_ab)+2.0*kappa*Ggr_ab;

    //qDebug()<<"red("<<t<<"): offset="<<offset<<"  backfactor="<<backfactor<<"  cfac="<<cfac;
    //qDebug()<<"          Fr="<<Fr<<"  Fg="<<Fg<<"  etaR="<<etaR<<"  etaG="<<etaG;
    //qDebug()<<"          cr1="<<cr1<<"  cr2="<<cr2<<"  background1="<<background1<<"  background2="<<background2;
    return offset+backfactor*cfac/(Fr*Fr);
}

void QFFitFunctionsFCCSFWDiff2ColorTCACFR::calcParameter(double* data, double* error) const {
    const double cab=data[FCCSDiff_concentrationab]*6.022e-1; // FCCSDiff_concentrationab given in 1e-9*6.022e23 particles/litre but c should be in particles per µm³= particles/10^{-15}litres=1e15 particles/litre
    double ecab=0;
    const double ca=data[FCCSDiff_concentrationa]*6.022e-1; // FCCSDiff_concentrationa given in 1e-9*6.022e23 particles/litre but c should be in particles per µm³= particles/10^{-15}litres=1e15 particles/litre
    double eca=0;
    const double cb=data[FCCSDiff_concentrationb]*6.022e-1; // FCCSDiff_concentrationb given in 1e-9*6.022e23 particles/litre but c should be in particles per µm³= particles/10^{-15}litres=1e15 particles/litre
    double ecb=0;
//    const double Da=data[FCCSDiff_diff_coeffa];
//    double eDa=0;
//    const double Db=data[FCCSDiff_diff_coeffb];
//    double eDb=0;
//    const double Dab=data[FCCSDiff_diff_coeffab];
//    double eDab=0;

//    const double dx=data[FCCSDiff_focus_distancex]/1000.0;
//    double edx=0;
//    const double dy=data[FCCSDiff_focus_distancey]/1000.0;
//    double edy=0;
//    const double dz=data[FCCSDiff_focus_distancez]/1000.0;
//    double edz=0;

    const double gammaG=data[FCCSDiff_focus_structfac1];
    const double wxyG=data[FCCSDiff_focus_width1]/1.0e3;
    double ewxyG=0;
    const double wzG=gammaG*wxyG;
    double ewzG=0, egammaG=0;

    const double gammaR=data[FCCSDiff_focus_structfac2];
    const double wxyR=data[FCCSDiff_focus_width2]/1.0e3;
    double ewxyR=0;
    const double wzR=gammaR*wxyR;
    double ewzR=0, egammaR=0;


//    const double offset=data[FCCSDiff_offset];
//    double eoffset=0;

    const double kappa=data[FCCSDiff_crosstalk];
    double ekappa=0;

    const double background1=data[FCCSDiff_background1];
    double ebackground1=0;
    const double cr1=data[FCCSDiff_count_rate1];
    double ecr1=0;
    const double background2=data[FCCSDiff_background2];
    double ebackground2=0;
    const double cr2=data[FCCSDiff_count_rate2];
    double ecr2=0;
    if (error) {
//        ecab=error[FCCSDiff_concentrationab]*6.022e-1; // FCCSDiff_concentrationab given in 1e-9*6.022e23 particles/litre but c should be in particles per µm³= particles/10^{-15}litres=1e15 particles/litre
//        eca=error[FCCSDiff_concentrationa]*6.022e-1; // FCCSDiff_concentrationa given in 1e-9*6.022e23 particles/litre but c should be in particles per µm³= particles/10^{-15}litres=1e15 particles/litre
//        ecb=error[FCCSDiff_concentrationb]*6.022e-1; // FCCSDiff_concentrationb given in 1e-9*6.022e23 particles/litre but c should be in particles per µm³= particles/10^{-15}litres=1e15 particles/litre
//        eDa=error[FCCSDiff_diff_coeffa];
//        eDb=error[FCCSDiff_diff_coeffb];
//        eDab=error[FCCSDiff_diff_coeffab];
//        edx=error[FCCSDiff_focus_distancex]/1000.0;
//        edy=error[FCCSDiff_focus_distancey]/1000.0;
//        edz=error[FCCSDiff_focus_distancez]/1000.0;
        egammaG=error[FCCSDiff_focus_structfac1]/1.0e3;
        ewzG=qfErrorMul(wxyG, ewxyG, gammaG, egammaG);
        ewxyG=error[FCCSDiff_focus_width1]/1.0e3;
        egammaR=error[FCCSDiff_focus_structfac2]/1.0e3;
        ewzR=qfErrorMul(wxyR, ewxyR, gammaR, egammaR);
        ewxyR=error[FCCSDiff_focus_width2]/1.0e3;//        eoffset=error[FCCSDiff_offset];
        ekappa=error[FCCSDiff_crosstalk];
        ebackground1=error[FCCSDiff_background1];
        ecr1=error[FCCSDiff_count_rate1];
        ebackground2=error[FCCSDiff_background2];
        ecr2=error[FCCSDiff_count_rate2];
    }
    data[FCCSDiff_brightness_a]=(cr1-background1)/((ca+cab)*FCS_newVeff( wxyG, wzG));
    if (error) error[FCCSDiff_brightness_a]=0;
    data[FCCSDiff_brightness_b]=(cr2-background2-kappa*(cr1-background1))/((cb+cab)*FCS_newVeff( wxyR, wzR));
    if (error) error[FCCSDiff_brightness_b]=0;

    data[FCSSDiff_focus_volume1]=FCS_newVeff(wxyG, wzG);
    if (error) error[FCSSDiff_focus_volume1]=FCS_newVeffError(wxyG, ewxyG, wzG, ewzG);

    data[FCSSDiff_focus_volume2]=FCS_newVeff(wxyR, wzR);
    if (error) error[FCSSDiff_focus_volume2]=FCS_newVeffError(wxyR, ewxyR, wzR, ewzR);
}

bool QFFitFunctionsFCCSFWDiff2ColorTCACFR::isParameterVisible(int parameter, const double* data) const {
    if (data) {
        switch(parameter) {
            case FCCSDiff_nonfl_taua:
            case FCCSDiff_nonfl_thetaa:
            case FCCSDiff_nonfl_taub:
            case FCCSDiff_nonfl_thetab:
            case FCCSDiff_nonfl_tauab:
            case FCCSDiff_nonfl_thetaab:
                return data[FCCSDiff_n_nonfluorescent]>0;

            default:
                return true;
        }
    }
    return true;
}

unsigned int QFFitFunctionsFCCSFWDiff2ColorTCACFR::getAdditionalPlotCount(const double* /*params*/) {
    return 1;
 }

QString QFFitFunctionsFCCSFWDiff2ColorTCACFR::transformParametersForAdditionalPlot(int /*plot*/, double* params) {
    params[FCCSDiff_n_nonfluorescent]=0;
    return QObject::tr("only diffusion");
}


void QFFitFunctionsFCCSFWDiff2ColorTCACFR::sortParameter(double *parameterValues, double *error, bool *fix) const {
    const double Da=parameterValues[FCCSDiff_diff_coeffa];
    const double Db=parameterValues[FCCSDiff_diff_coeffb];
    const double Dab=parameterValues[FCCSDiff_diff_coeffab];
    const double D2a=parameterValues[FCCSDiff_diff_coeff2a];
    const double D2ab=parameterValues[FCCSDiff_diff_coeff2ab];
    const double D2b=parameterValues[FCCSDiff_diff_coeff2b];

    if (Da<D2a) {
        qSwap(parameterValues[FCCSDiff_diff_coeffa],parameterValues[FCCSDiff_diff_coeff2a]);
        if (fix) qSwap(fix[FCCSDiff_diff_coeffa],fix[FCCSDiff_diff_coeff2a]);
        if (error) qSwap(error[FCCSDiff_diff_coeffa],error[FCCSDiff_diff_coeff2a]);
    }
    if (Db<D2b) {
        qSwap(parameterValues[FCCSDiff_diff_coeffb],parameterValues[FCCSDiff_diff_coeff2b]);
        if (fix) qSwap(fix[FCCSDiff_diff_coeffb],fix[FCCSDiff_diff_coeff2b]);
        if (error) qSwap(error[FCCSDiff_diff_coeffb],error[FCCSDiff_diff_coeff2b]);
    }
    if (Dab<D2ab) {
        qSwap(parameterValues[FCCSDiff_diff_coeffab],parameterValues[FCCSDiff_diff_coeff2ab]);
        if (fix) qSwap(fix[FCCSDiff_diff_coeffab],fix[FCCSDiff_diff_coeff2ab]);
        if (error) qSwap(error[FCCSDiff_diff_coeffab],error[FCCSDiff_diff_coeff2ab]);
    }

}
