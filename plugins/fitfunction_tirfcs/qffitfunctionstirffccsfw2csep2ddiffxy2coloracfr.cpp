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

#include "qffitfunctionstirffccsfw2csep2ddiffxy2coloracfr.h"
#include "qfmathtools.h"
#include <cmath>
#include "imfcstools.h"
#include <QDebug>
QFFitFunctionsTIRFFCCSFW2CSep2DDiffXY2ColorACFR::QFFitFunctionsTIRFFCCSFW2CSep2DDiffXY2ColorACFR() {
    //           type,         id,                        name,                                                   label,                      unit,          unitlabel,               fit,       userEditable, userRangeEditable, displayError,               initialFIx,  initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(FloatNumber,  "concentration_a",         "concentration of species a in focus",         "C<sub>a</sub>",           "nmol/dm^2",         "nmol/dm<sup>2</sup>",                              true,      true,          true,              QFFitFunction::DisplayError, false, 10,          0,        1e50,     1    );
    #define FCCSDiff_concentrationa 0
    addParameter(FloatNumber,  "concentration_b",         "concentration of species b in focus",         "C<sub>b</sub>",           "nmol/dm^2",         "nmol/dm<sup>2</sup>",                              true,      true,          true,              QFFitFunction::DisplayError, false, 10,          0,        1e50,     1    );
    #define FCCSDiff_concentrationb 1
    addParameter(FloatNumber,  "concentration_ab",         "concentration of species ab in focus",       "C<sub>ab</sub>",           "nmol/dm^2",         "nmol/dm<sup>2</sup>",                             true,      true,          true,              QFFitFunction::DisplayError, false, 5,          0,        1e50,     1    );
    #define FCCSDiff_concentrationab 2


    addParameter(FloatNumber,  "diff_coeff1",            "red ACF: diffusion coefficient of component 1",                  "D<sub>1</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    true,      true,         true,              QFFitFunction::DisplayError, false, 10,           1e-10,    1e50,     1    );
    #define FCCSDiff_diff_coeff1 3
    addParameter(FloatNumber,  "diff_coeff2",            "red ACF: diffusion coefficient of component 2",                  "D<sub>2</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    true,      true,         true,              QFFitFunction::DisplayError, false, 100,           1e-10,    1e50,     1    );
    #define FCCSDiff_diff_coeff2 4
    addParameter(FloatNumber,  "diff_rho2",            "red ACF: fraction of diffusion component  2",   "&rho;<sub>2</sub>",            "", "",    true,      true,         true,              QFFitFunction::DisplayError, false, 0.5,           0,    1,     1    );
    #define FCCSDiff_diff_rho2 5

    addParameter(FloatNumber,  "offset",                  "correlation offset",                                   "G<sub>&infin;</sub>",      "",           "",                       true,      true,         true,              QFFitFunction::DisplayError, true, 0,            -10,      10,       0.1  );
    #define FCCSDiff_offset 6
    addParameter(FloatNumber,  "crosstalk",               "crosstalk coefficient",                                "&kappa;",                  "",         "",                         true,      true,         true,             QFFitFunction::EditError,    true, 0,           0,     1,      0.1, 0, 1  );
    #define FCCSDiff_crosstalk 7
    addParameter(FloatNumber,  "focus_width1",             "green PSF: lateral radius (1/e² radius)",             "w<sub>g</sub>",    "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 600,          0,        1e4,      10    );
    #define FCCSDiff_focus_width1 8
    addParameter(FloatNumber,  "focus_width2",             "red PSF: lateral radius (1/e² radius)",             "w<sub>r</sub>",    "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 660,          0,        1e4,      10    );
    #define FCCSDiff_focus_width2 9
    addParameter(FloatNumber,  "pixel_width",             "pixel width",                                           "a",                        "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 400,          0,        1e4,      10    );
    #define FCCSDiff_pixel_width 10
    addParameter(FloatNumber,  "effective_area1",            "green focus: effective area",                               "A<sub>eff,g</sub>",          "micron^2",         "&mu;m<sup>2</sup>",                     false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define FCSSDiff_focus_volume1 11
    addParameter(FloatNumber,  "effective_area2",            "red focus: effective area",                               "A<sub>eff,r</sub>",          "micron^2",         "&mu;m<sup>2</sup>",                     false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define FCSSDiff_focus_volume2 12
    addParameter(FloatNumber,  "count_rate1",              "count rate green, during measurement",                      "&lang;F<sub>g</sub>&rang;",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError,    false, 0,            0,        1e50,     1    );
    #define FCCSDiff_count_rate1 13
    addParameter(FloatNumber,  "background1",              "background count rate green, during measurement",           "B<sub>g</sub>",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError  ,  false, 0,            0,        1e50,     1    );
    #define FCCSDiff_background1 14
    addParameter(FloatNumber,  "count_rate2",              "count rate red, during measurement",                      "&lang;F<sub>r</sub>&rang;",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError,    false, 0,            0,        1e50,     1    );
    #define FCCSDiff_count_rate2 15
    addParameter(FloatNumber,  "background2",              "background count rate red, during measurement",           "B<sub>r</sub>",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError  ,  false, 0,            0,        1e50,     1    );
    #define FCCSDiff_background2 16
    addParameter(FloatNumber,  "brightness_a",             "molar brightness of fluorophore on A",           "&eta;<sub>a</sub>",            "counts/(nmol/dm^2)",           "counts/(nmol/dm<sup>2</sup>",    false,      false,          false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e-50,     1     );
    #define FCCSDiff_brightness_a 17
    addParameter(FloatNumber,  "brightness_b",             "molar brightness of fluorophore on B",           "&eta;<sub>b</sub>",            "counts/(nmol/dm^2)",           "counts/(nmol/dm<sup>2</sup>",    false,      false,          false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e-50,     1     );
    #define FCCSDiff_brightness_b 18

    addParameter(FloatNumber,  "molbrightness_a",             "molecular brightness of fluorophore on A",           "CPM<sub>a</sub>",            "cpm",           "cpm",    false,      false,          false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e-50,     1     );
    #define FCCSDiff_molbrightness_a 19
    addParameter(FloatNumber,  "molbrightness_b",             "molecular brightness of fluorophore on B",           "CPM<sub>b</sub>",            "cpm",           "cpm",    false,      false,          false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e-50,     1     );
    #define FCCSDiff_molbrightness_b 20
}


double QFFitFunctionsTIRFFCCSFW2CSep2DDiffXY2ColorACFR::evaluate(double t, const double* data) const {
    const double cab=data[FCCSDiff_concentrationab]*6.022e-1; // FCCSDiff_concentrationab given in 1e-9*6.022e23 particles/litre but c should be in particles per µm³= particles/10^{-15}litres=1e15 particles/litre
    const double ca=data[FCCSDiff_concentrationa]*6.022e-1; // FCCSDiff_concentrationa given in 1e-9*6.022e23 particles/litre but c should be in particles per µm³= particles/10^{-15}litres=1e15 particles/litre
    const double cb=data[FCCSDiff_concentrationb]*6.022e-1; // FCCSDiff_concentrationb given in 1e-9*6.022e23 particles/litre but c should be in particles per µm³= particles/10^{-15}litres=1e15 particles/litre

    const double D1=data[FCCSDiff_diff_coeff1];
    const double D2=data[FCCSDiff_diff_coeff2];
    const double rho2=data[FCCSDiff_diff_rho2];
    const double rho1=1.0-rho2;


    /*const double dx=data[FCCSDiff_focus_distancex]/1000.0;
    const double dy=data[FCCSDiff_focus_distancey]/1000.0;
    const double dz=data[FCCSDiff_focus_distancez]/1000.0;

    const double wzG=data[FCCSDiff_focus_height1]/1.0e3;
    const double wxyG=data[FCCSDiff_focus_width1]/1.0e3;*/

    //const double wzR=data[FCCSDiff_focus_height2]/1.0e3;
    const double wxyR=data[FCCSDiff_focus_width2]/1.0e3;

    const double a=data[FCCSDiff_pixel_width]/1.0e3;

    const double offset=data[FCCSDiff_offset];

    const double kappa=data[FCCSDiff_crosstalk];

    const double background1=data[FCCSDiff_background1];
    const double cr1=data[FCCSDiff_count_rate1];
    const double background2=data[FCCSDiff_background2];
    const double cr2=data[FCCSDiff_count_rate2];

    double backfactor=(cr1-background1)/(cr1) * (cr2-background2)/(cr2);
    if (fabs(cr1)<1e-15 || fabs(cr2)<1e-15 || (fabs(background1)<1e-15 && fabs(background2)<1e-15)) backfactor=1;

    const double etag=(cr1-background1)/(ca+cab);
    const double etar=(cr2-background2-kappa*(cr1-background1))/(cb+cab);
    const double Fg=etag*(ca+cab);
    const double Fr=etar*(cb+cab)+kappa*Fg;


    const double corrfunction=(  rho1  *QFFitFunctionsTIRFCCSFWDiff2ColorCCF_corrfactor_2Dxy(a, 0, 0, D1, t, wxyR, wxyR)
                               + rho2  *QFFitFunctionsTIRFCCSFWDiff2ColorCCF_corrfactor_2Dxy(a, 0, 0, D2, t, wxyR, wxyR));

    return offset+backfactor*(etar*etar*(cb+cab)+kappa*kappa*etag*etag*(ca+cab)+2.0*kappa*etag*etar*cab)*corrfunction/(Fr*Fr);

}

void QFFitFunctionsTIRFFCCSFW2CSep2DDiffXY2ColorACFR::calcParameter(double* data, double* error) const {
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

//    const double wzG=data[FCCSDiff_focus_height1]/1.0e3;
//    double ewzG=0;
    const double wxyG=data[FCCSDiff_focus_width1]/1.0e3;
    double ewxyG=0;

//    const double wzR=data[FCCSDiff_focus_height2]/1.0e3;
//    double ewzR=0;
    const double wxyR=data[FCCSDiff_focus_width2]/1.0e3;
    double ewxyR=0;

    const double a=data[FCCSDiff_pixel_width]/1.0e3;
    double ea=0;

    const double offset=data[FCCSDiff_offset];
    double eoffset=0;

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
        //ewzG=error[FCCSDiff_focus_height1]/1.0e3;
        ewxyG=error[FCCSDiff_focus_width1]/1.0e3;
        //ewzR=error[FCCSDiff_focus_height2]/1.0e3;
        ewxyR=error[FCCSDiff_focus_width2]/1.0e3;
        ea=error[FCCSDiff_pixel_width]/1.0e3;
//        eoffset=error[FCCSDiff_offset];
        ekappa=error[FCCSDiff_crosstalk];
        ebackground1=error[FCCSDiff_background1];
        ecr1=error[FCCSDiff_count_rate1];
        ebackground2=error[FCCSDiff_background2];
        ecr2=error[FCCSDiff_count_rate2];
    }
    data[FCCSDiff_brightness_a]=(cr1-background1)/(ca+cab);
    if (error) error[FCCSDiff_brightness_a]=0;
    data[FCCSDiff_brightness_b]=(cr2-background2-kappa*(cr1-background1))/(cb+cab);
    if (error) error[FCCSDiff_brightness_b]=0;

    data[FCSSDiff_focus_volume1]=TIRFCS_newAeff(a, wxyG);
    if (error) error[FCSSDiff_focus_volume1]=TIRFCS_newAeffError(a, ea, wxyG, ewxyG);

    data[FCSSDiff_focus_volume2]=TIRFCS_newAeff(a, wxyR);
    if (error) error[FCSSDiff_focus_volume2]=TIRFCS_newAeffError(a, ea, wxyR, ewxyR);

    data[FCCSDiff_molbrightness_a]=data[FCCSDiff_brightness_a]/(QF_NAVOGADRO*1e-19*data[FCSSDiff_focus_volume1]);
    if (error) error[FCCSDiff_molbrightness_a]=0;
    data[FCCSDiff_molbrightness_b]=data[FCCSDiff_brightness_b]/(QF_NAVOGADRO*1e-19*data[FCSSDiff_focus_volume2]);
    if (error) error[FCCSDiff_molbrightness_b]=0;

}

bool QFFitFunctionsTIRFFCCSFW2CSep2DDiffXY2ColorACFR::isParameterVisible(int parameter, const double* data) const {
    return true;
}

unsigned int QFFitFunctionsTIRFFCCSFW2CSep2DDiffXY2ColorACFR::getAdditionalPlotCount(const double* params) {
    return 0;
}

QString QFFitFunctionsTIRFFCCSFW2CSep2DDiffXY2ColorACFR::transformParametersForAdditionalPlot(int plot, double* params) {
    return "";
}

void QFFitFunctionsTIRFFCCSFW2CSep2DDiffXY2ColorACFR::sortParameter(double *parameterValues, double *error, bool *fix) const {
    const double D1=parameterValues[FCCSDiff_diff_coeff1];
    const double D2=parameterValues[FCCSDiff_diff_coeff2];
    const double rho2=parameterValues[FCCSDiff_diff_rho2];

    if (D1<D2 && (fix && !fix[FCCSDiff_diff_coeff1] && !fix[FCCSDiff_diff_coeff2] && !fix[FCCSDiff_diff_rho2])) {
        qSwap(parameterValues[FCCSDiff_diff_coeff1], parameterValues[FCCSDiff_diff_coeff2]);
        if (error) qSwap(error[FCCSDiff_diff_coeff1], error[FCCSDiff_diff_coeff2]);
        if (fix) qSwap(fix[FCCSDiff_diff_coeff1], fix[FCCSDiff_diff_coeff2]);
        parameterValues[FCCSDiff_diff_rho2]=1.0-rho2;
    }

}
