/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

    last modification: $LastChangedDate: 2014-09-02 10:24:20 +0200 (Di, 02 Sep 2014) $  (revision $Rev: 3422 $)

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

#include "qffitfunctionstirffccsfw2dadiffxy2coloracfg.h"

#include "qfmathtools.h"
#include <cmath>
#include "imfcstools.h"
QFFitFunctionsTIRFFCCSFW2DADiffXY2ColorACFG::QFFitFunctionsTIRFFCCSFW2DADiffXY2ColorACFG() {
    //           type,         id,                        name,                                                   label,                      unit,          unitlabel,               fit,       userEditable, userRangeEditable, displayError,               initialFIx,  initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(FloatNumber,  "concentration_a",         "concentration of species a in focus",         "C<sub>a</sub>",           "nmol/dm^2",         "nmol/dm<sup>2</sup>",                              true,      true,          true,              QFFitFunction::DisplayError, false, 10,          0,        1e50,     1    );
    #define FCCSDiff_concentrationa 0
    addParameter(FloatNumber,  "concentration_ab",         "concentration of species ab in focus",       "C<sub>ab</sub>",           "nmol/dm^2",         "nmol/dm<sup>2</sup>",                             true,      true,          true,              QFFitFunction::DisplayError, false, 5,          0,        1e50,     1    );
    #define FCCSDiff_concentrationab 1

    addParameter(FloatNumber,  "diff_acoeff_a",            "diffusion coefficient of species a",                  "&Gamma;<sub>a</sub>",            "micron^2/s^alpha", "&mu;m<sup>2</sup>/s<sup>&alpha;</sup>",    true,      true,         true,              QFFitFunction::DisplayError, false, 10,           1e-10,    1e50,     1    );
    #define FCCSDiff_diff_acoeffa 2
    addParameter(FloatNumber,  "diff_alpha_a",            "anomality parameter of species a",                  "&alpha;<sub>a</sub>",            "", "",                             true,      true,         true,              QFFitFunction::DisplayError, false, 1,            0.01,     100,      0.1, 0       );
    #define FCCSDiff_diff_alphaa 3
    addParameter(FloatNumber,  "diff_acoeff_ab",            "diffusion coefficient of species ab",                  "&Gamma;<sub>ab</sub>",            "micron^2/s^alpha", "&mu;m<sup>2</sup>/s<sup>&alpha;</sup>",    true,      true,         true,              QFFitFunction::DisplayError, false, 10,           1e-10,    1e50,     1    );
    #define FCCSDiff_diff_acoeffab 4
    addParameter(FloatNumber,  "diff_alpha_ab",            "anomality parameter of species ab",                  "&alpha;<sub>ab</sub>",            "", "",                             true,      true,         true,              QFFitFunction::DisplayError, false, 1,            0.01,     100,      0.1, 0       );
    #define FCCSDiff_diff_alphaab 5

    addParameter(FloatNumber,  "offset",                  "correlation offset",                                   "G<sub>&infin;</sub>",      "",           "",                       true,      true,         true,              QFFitFunction::DisplayError, true, 0,            -10,      10,       0.1  );
    #define FCCSDiff_offset 6
    addParameter(FloatNumber,  "focus_width1",             "green PSF: lateral radius (1/e^2 radius)",             "w<sub>g</sub>",    "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 600,          0,        1e4,      10    );
    #define FCCSDiff_focus_width1 7
    addParameter(FloatNumber,  "pixel_width",             "pixel width",                                           "a",                        "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 400,          0,        1e4,      10    );
    #define FCCSDiff_pixel_width 8
    addParameter(FloatNumber,  "effective_area1",            "green focus: effective area",                               "A<sub>eff,g</sub>",          "micron^2",         "&mu;m<sup>2</sup>",                     false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define FCSSDiff_focus_volume 9
    addParameter(FloatNumber,  "count_rate1",              "count rate green, during measurement",                      "&lang;F<sub>g</sub>&rang;",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError,    false, 0,            0,        1e50,     1    );
    #define FCCSDiff_count_rate1 10
    addParameter(FloatNumber,  "background1",              "background count rate green, during measurement",           "B<sub>g</sub>",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError  ,  false, 0,            0,        1e50,     1    );
    #define FCCSDiff_background1 11
    addParameter(FloatNumber,  "brightness_a",             "molar brightness of fluorophore on A",           "&eta;<sub>a</sub>",            "counts/(nmol/dm^2)",           "counts/(nmol/dm<sup>2</sup>)",    false,      false,          false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e-50,     1     );
    #define FCCSDiff_brightness_a 12
    addParameter(FloatNumber,  "molbrightness_a",             "molecular brightness of fluorophore on A",           "CPM<sub>a</sub>",            "cpm",           "cpm",    false,      false,          false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e-50,     1     );
    #define FCCSDiff_molbrightness_a 13

}


double QFFitFunctionsTIRFFCCSFW2DADiffXY2ColorACFG::evaluate(double t, const double* data) const {
    const double cab=data[FCCSDiff_concentrationab]*6.022e-1; // FCCSDiff_concentrationab given in 1e-9*6.022e23 particles/litre but c should be in particles per µm³= particles/10^{-15}litres=1e15 particles/litre
    const double ca=data[FCCSDiff_concentrationa]*6.022e-1; // FCCSDiff_concentrationa given in 1e-9*6.022e23 particles/litre but c should be in particles per µm³= particles/10^{-15}litres=1e15 particles/litre

    const double Da=data[FCCSDiff_diff_acoeffa];
    const double Dab=data[FCCSDiff_diff_acoeffab];
    const double alphaa=data[FCCSDiff_diff_alphaa];
    const double alphaab=data[FCCSDiff_diff_alphaab];

    //const double dx=data[FCCSDiff_focus_distancex]/1000.0;
    //const double dy=data[FCCSDiff_focus_distancey]/1000.0;
    //const double dz=data[FCCSDiff_focus_distancez]/1000.0;

    //const double wzG=data[FCCSDiff_focus_height1]/1.0e3;
    const double wxyG=data[FCCSDiff_focus_width1]/1.0e3;


    const double a=data[FCCSDiff_pixel_width]/1.0e3;

    const double offset=data[FCCSDiff_offset];


    const double background1=data[FCCSDiff_background1];
    const double cr1=data[FCCSDiff_count_rate1];

    double backfactor=qfSqr((cr1-background1)/(cr1));
    if (fabs(cr1)<1e-15 || (fabs(background1)<1e-15)) backfactor=1;

    const double etaG=(cr1-background1)/(ca+cab);
    const double Fg=etaG*(ca+cab);

    //double Grr_b=etaR*etaR*cb*QFFitFunctionsSPIMFCCSFWDiff2ColorCCF_corrfactor(a, 0, 0, Db, t, wxyR, wxyR);
    //double Grr_ab=etaR*etaR*cab*QFFitFunctionsSPIMFCCSFWDiff2ColorCCF_corrfactor(a, 0, 0, Dab, t, wxyR, wxyR);
    double Ggg_a=etaG*etaG*ca*QFFitFunctionsTIRFCCSFWADiff2ColorCCF_corrfactor_2Dxy(a, 0, 0, Da, alphaa, t, wxyG, wxyG);
    double Ggg_ab=etaG*etaG*cab*QFFitFunctionsTIRFCCSFWADiff2ColorCCF_corrfactor_2Dxy(a, 0, 0, Dab, alphaab, t, wxyG, wxyG);
    //double Ggr_ab=etaG*etaR*cab*QFFitFunctionsTIRFCCSFWDiff2ColorCCF_corrfactor_2Dxy(a, dx, dy, Dab, t, wxyG, wxyR);

    if (fabs(ca)<1e-15) Ggg_a=0;
    //if (fabs(cb)<1e-15) Grr_b=0;
    if (fabs(cab)<1e-15) /*Grr_ab=*/Ggg_ab=/*Ggr_ab=*/0;

    const double cfac=Ggg_a+Ggg_ab;

    return offset+backfactor*cfac/(Fg*Fg);
}

void QFFitFunctionsTIRFFCCSFW2DADiffXY2ColorACFG::calcParameter(double* data, double* error) const {
    const double cab=data[FCCSDiff_concentrationab]*6.022e-1; // FCCSDiff_concentrationab given in 1e-9*6.022e23 particles/litre but c should be in particles per µm³= particles/10^{-15}litres=1e15 particles/litre
    double ecab=0;
    const double ca=data[FCCSDiff_concentrationa]*6.022e-1; // FCCSDiff_concentrationa given in 1e-9*6.022e23 particles/litre but c should be in particles per µm³= particles/10^{-15}litres=1e15 particles/litre
    double eca=0;
//    const double cb=data[FCCSDiff_concentrationb]*6.022e-1; // FCCSDiff_concentrationb given in 1e-9*6.022e23 particles/litre but c should be in particles per µm³= particles/10^{-15}litres=1e15 particles/litre
//    double ecb=0;
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
//    const double wxyR=data[FCCSDiff_focus_width2]/1.0e3;
//    double ewxyR=0;

    const double a=data[FCCSDiff_pixel_width]/1.0e3;
    double ea=0;

//    const double offset=data[FCCSDiff_offset];
//    double eoffset=0;

//    const double kappa=data[FCCSDiff_crosstalk];
//    double ekappa=0;

    const double background1=data[FCCSDiff_background1];
    double ebackground1=0;
    const double cr1=data[FCCSDiff_count_rate1];
    double ecr1=0;
//    const double background2=data[FCCSDiff_background2];
//    double ebackground2=0;
//    const double cr2=data[FCCSDiff_count_rate2];
//    double ecr2=0;
    if (error) {
        ecab=error[FCCSDiff_concentrationab]*6.022e-1; // FCCSDiff_concentrationab given in 1e-9*6.022e23 particles/litre but c should be in particles per µm³= particles/10^{-15}litres=1e15 particles/litre
        eca=error[FCCSDiff_concentrationa]*6.022e-1; // FCCSDiff_concentrationa given in 1e-9*6.022e23 particles/litre but c should be in particles per µm³= particles/10^{-15}litres=1e15 particles/litre
//        ecb=error[FCCSDiff_concentrationb]*6.022e-1; // FCCSDiff_concentrationb given in 1e-9*6.022e23 particles/litre but c should be in particles per µm³= particles/10^{-15}litres=1e15 particles/litre
//        eDa=error[FCCSDiff_diff_coeffa];
//        eDb=error[FCCSDiff_diff_coeffb];
//        eDab=error[FCCSDiff_diff_coeffab];
//        edx=error[FCCSDiff_focus_distancex]/1000.0;
//        edy=error[FCCSDiff_focus_distancey]/1000.0;
//        edz=error[FCCSDiff_focus_distancez]/1000.0;
//        ewzG=error[FCCSDiff_focus_height1]/1.0e3;
//        ewxyG=error[FCCSDiff_focus_width1]/1.0e3;
//        ewzR=error[FCCSDiff_focus_height2]/1.0e3;
//        ewxyR=error[FCCSDiff_focus_width2]/1.0e3;
        ea=error[FCCSDiff_pixel_width]/1.0e3;
//        eoffset=error[FCCSDiff_offset];
//        ekappa=error[FCCSDiff_crosstalk];
        ebackground1=error[FCCSDiff_background1];
        ecr1=error[FCCSDiff_count_rate1];
//        ebackground2=error[FCCSDiff_background2];
//        ecr2=error[FCCSDiff_count_rate2];
    }

    data[FCCSDiff_brightness_a]=(cr1-background1)/(ca+cab);
    if (error) error[FCCSDiff_brightness_a]=0;

    data[FCSSDiff_focus_volume]=TIRFCS_newAeff(a, wxyG);
    if (error) error[FCSSDiff_focus_volume]=TIRFCS_newAeffError(a, ea, wxyG, ewxyG);

    data[FCCSDiff_molbrightness_a]=data[FCCSDiff_brightness_a]/(QF_NAVOGADRO*1e-19*data[FCSSDiff_focus_volume]);
    if (error) error[FCCSDiff_molbrightness_a]=0;
}

bool QFFitFunctionsTIRFFCCSFW2DADiffXY2ColorACFG::isParameterVisible(int parameter, const double* data) const {
    return true;
}

unsigned int QFFitFunctionsTIRFFCCSFW2DADiffXY2ColorACFG::getAdditionalPlotCount(const double* params) {
    return 0;
}

QString QFFitFunctionsTIRFFCCSFW2DADiffXY2ColorACFG::transformParametersForAdditionalPlot(int plot, double* params) {
    return "";
}


void QFFitFunctionsTIRFFCCSFW2DADiffXY2ColorACFG::sortParameter(double *parameterValues, double *error, bool *fix) const {

}
