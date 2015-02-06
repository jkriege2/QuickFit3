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

#include "qffitfunctionsfccsfwdiff2colorsepacfg.h"
#include "qfmathtools.h"
#include <cmath>
#include "fcstools.h"
QFFitFunctionsFCCSFWDiff2ColorSepACFG::QFFitFunctionsFCCSFWDiff2ColorSepACFG() {
    //           type,         id,                        name,                                                   label,                      unit,          unitlabel,               fit,       userEditable, userRangeEditable, displayError,               initialFIx,  initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(IntCombo,     "n_nonfluorescent",        "number of nonfluorescent components (triplet ...)",     "# non-fluorescent",        "",            "",                      false,     true,         false,             QFFitFunction::NoError,      false, 1,            0,        1,        1,   0,      1);
    #define FCCSDiff_n_nonfluorescent 0

    addParameter(FloatNumber,  "nonfl_tau",              "triplet decay time of green channel",              "&tau;<sub>trip,g</sub>",     "usec",        "&mu;s",                 true,      true,         true,              QFFitFunction::DisplayError, false, 3.0,          0,        10,       0.1, 0  );
    #define FCCSDiff_nonfl_tau 1
    addParameter(FloatNumber,  "nonfl_theta",            "triplet fraction of green channel",                "&theta;<sub>trip,g</sub>",   "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 0.2,          0,        0.99999,  0.1, 0,      1);
    #define FCCSDiff_nonfl_theta 2

    addParameter(FloatNumber,  "concentration_a",         "concentration of species a in focus",         "C<sub>a</sub>",           "nM",         "nM",                              true,      true,          true,              QFFitFunction::DisplayError, false, 10,          0,        1e50,     1    );
    #define FCCSDiff_concentrationa 3
    addParameter(FloatNumber,  "concentration_ab",         "concentration of species ab in focus",       "C<sub>ab</sub>",           "nM",         "nM",                             true,      true,          true,              QFFitFunction::DisplayError, false, 5,          0,        1e50,     1    );
    #define FCCSDiff_concentrationab 4

    addParameter(FloatNumber,  "diff_coeff",            "diffusion coefficient 1 of green channel",                  "D<sub>g,1</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    true,      true,         true,              QFFitFunction::DisplayError, false, 10,           1e-10,    1e50,     1    );
    #define FCCSDiff_diff_coeff 5


    addParameter(FloatNumber,  "offset",                  "correlation offset",                                   "G<sub>&infin;</sub>",      "",           "",                       true,      true,         true,              QFFitFunction::DisplayError, true, 0,            -10,      10,       0.1  );
    #define FCCSDiff_offset 6
    addParameter(FloatNumber,  "focus_distance_x",         "foci: lateral distance in x-direction",               "d<sub>x</sub>",            "nm",         "nm",                     true,      true,         true,             QFFitFunction::EditError,    true, 0,           -1e6,     1e6,      10  );
    #define FCCSDiff_focus_distancex 7
    addParameter(FloatNumber,  "focus_distance_y",         "foci: lateral distance in y-direction",               "d<sub>y</sub>",            "nm",         "nm",                     true,      true,         true,             QFFitFunction::EditError,    true, 0,              -1e6,     1e6,      10  );
    #define FCCSDiff_focus_distancey 8
    addParameter(FloatNumber,  "focus_distance_z",         "foci: longitudinal distance in z-direction",          "d<sub>z</sub>",            "nm",         "nm",                     true,      true,         true,             QFFitFunction::EditError,    true, 0,              -1e6,     1e6,      10  );
    #define FCCSDiff_focus_distancez 9
    addParameter(FloatNumber,  "focus_struct_fac1",        "green PSF: structure factor",                        "&gamma;<sub>g</sub>",      "",         "",                     true,      true,         true,              QFFitFunction::EditError,    true, 6,         1e-5,     1e5,      1  );
    #define FCCSDiff_focus_structfac1 10
    addParameter(FloatNumber,  "focus_width1",             "green PSF: lateral radius (1/e^2 radius)",             "w<sub>g</sub>",    "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 250,          0,        1e4,      10    );
    #define FCCSDiff_focus_width1 11
    addParameter(FloatNumber,  "focus_volume1",            "green focus: effective volume",                               "V<sub>eff,g</sub>",          "fl",         "fl",                     false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define FCSSDiff_focus_volume 12
    addParameter(FloatNumber,  "count_rate1",              "count rate green, during measurement",                      "&lang;F<sub>g</sub>&rang;",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError,    false, 0,            0,        1e50,     1    );
    #define FCCSDiff_count_rate1 13
    addParameter(FloatNumber,  "background1",              "background count rate green, during measurement",           "B<sub>g</sub>",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError  ,  false, 0,            0,        1e50,     1    );
    #define FCCSDiff_background1 14
    addParameter(FloatNumber,  "brightness_a",             "molar brightness of fluorophore on A",           "&eta;<sub>a</sub>",            "cpm",           "cpm",    false,      false,          false,              QFFitFunction::DisplayError, false, 0.5             );
    #define FCCSDiff_brightness_a 15

}


double QFFitFunctionsFCCSFWDiff2ColorSepACFG::evaluate(double t, const double* data) const {
    const double cab=data[FCCSDiff_concentrationab]*6.022e-1; // FCCSDiff_concentrationab given in 1e-9*6.022e23 particles/litre but c should be in particles per µm³= particles/10^{-15}litres=1e15 particles/litre
    const double ca=data[FCCSDiff_concentrationa]*6.022e-1; // FCCSDiff_concentrationa given in 1e-9*6.022e23 particles/litre but c should be in particles per µm³= particles/10^{-15}litres=1e15 particles/litre


    const double D1=data[FCCSDiff_diff_coeff];
    const int nNonFl=qBound(0, (int)round(data[FCCSDiff_n_nonfluorescent]), 1);
    const double tauT=data[FCCSDiff_nonfl_tau]*1e-6;
    const double thetaT=data[FCCSDiff_nonfl_theta];


    const double dx=data[FCCSDiff_focus_distancex]/1000.0;
    const double dy=data[FCCSDiff_focus_distancey]/1000.0;
    const double dz=data[FCCSDiff_focus_distancez]/1000.0;

    const double wxyG=data[FCCSDiff_focus_width1]/1.0e3;
    const double gammaG=data[FCCSDiff_focus_structfac1];
    const double wzG=gammaG*wxyG;



    const double offset=data[FCCSDiff_offset];


    const double background1=data[FCCSDiff_background1];
    const double cr1=data[FCCSDiff_count_rate1];

    double backfactor=qfSqr((cr1-background1)/(cr1));
    if (fabs(cr1)<1e-15 || (fabs(background1)<1e-15)) backfactor=1;

    const double etag=(cr1-background1)/(ca+cab);
    const double Fg=etag*(ca+cab);



    const double corrfunction=QFFitFunctionFCCSFWTriplet(nNonFl, t, thetaT, tauT)
                              *QFFitFunctionsFCCSFWDiff2ColorCCF_corrfactor(0, 0, 0, D1, t, wxyG, wxyG, wzG, wzG);

    return offset+backfactor*(etag*etag*ca+etag*etag*cab)*corrfunction/(Fg*Fg);
}

void QFFitFunctionsFCCSFWDiff2ColorSepACFG::calcParameter(double* data, double* error) const {
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

    const double gammaG=data[FCCSDiff_focus_structfac1];
    const double wxyG=data[FCCSDiff_focus_width1]/1.0e3;
    double ewxyG=0;
    const double wzG=gammaG*wxyG;
    double ewzG=0, egammaG=0;

//    const double gammaR=data[FCCSDiff_focus_structfac2];
//    const double wxyR=data[FCCSDiff_focus_width2]/1.0e3;
//    double ewxyR=0;
//    const double wzR=gammaR*wxyR;
//    double ewzR=0, egammaR=0;


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
        egammaG=error[FCCSDiff_focus_structfac1]/1.0e3;
        ewzG=qfErrorMul(wxyG, ewxyG, gammaG, egammaG);
        ewxyG=error[FCCSDiff_focus_width1]/1.0e3;
//        ewzR=error[FCCSDiff_focus_height2]/1.0e3;
//        ewxyR=error[FCCSDiff_focus_width2]/1.0e3;
//        eoffset=error[FCCSDiff_offset];
//        ekappa=error[FCCSDiff_crosstalk];
        ebackground1=error[FCCSDiff_background1];
        ecr1=error[FCCSDiff_count_rate1];
//        ebackground2=error[FCCSDiff_background2];
//        ecr2=error[FCCSDiff_count_rate2];
    }

    data[FCCSDiff_brightness_a]=(cr1-background1)/((ca+cab)*FCS_newVeff( wxyG, wzG));
    if (error) error[FCCSDiff_brightness_a]=0;

    data[FCSSDiff_focus_volume]=FCS_newVeff( wxyG, wzG);
    if (error) error[FCSSDiff_focus_volume]=FCS_newVeffError( wxyG, ewxyG, wzG, ewzG);
}

bool QFFitFunctionsFCCSFWDiff2ColorSepACFG::isParameterVisible(int parameter, const double* data) const {
    if (data) {
        switch(parameter) {
            case FCCSDiff_nonfl_tau:
            case FCCSDiff_nonfl_theta:
                return data[FCCSDiff_n_nonfluorescent]>0;

            default:
                return true;
        }
    }
    return true;
}

unsigned int QFFitFunctionsFCCSFWDiff2ColorSepACFG::getAdditionalPlotCount(const double* params) {
    return 1;
}

QString QFFitFunctionsFCCSFWDiff2ColorSepACFG::transformParametersForAdditionalPlot(int plot, double* params) {
    params[FCCSDiff_n_nonfluorescent]=0;
    return QObject::tr("only diffusion");
}


void QFFitFunctionsFCCSFWDiff2ColorSepACFG::sortParameter(double *parameterValues, double *error, bool *fix) const {

}
