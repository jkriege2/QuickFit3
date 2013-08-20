#include "qffitfunctionsfccsfwadiff2coloracfg.h"
#include "qfmathtools.h"
#include <cmath>
#include "fcstools.h"
QFFitFunctionsFCCSFWADiff2ColorACFG::QFFitFunctionsFCCSFWADiff2ColorACFG() {
    //           type,         id,                        name,                                                   label,                      unit,          unitlabel,               fit,       userEditable, userRangeEditable, displayError,               initialFIx,  initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(IntCombo,     "n_nonfluorescent",        "number of nonfluorescent components (triplet ...)",     "# non-fluorescent",        "",            "",                      false,     true,         false,             QFFitFunction::NoError,      false, 1,            0,        1,        1,   0,      1);
    #define FCCSDiff_n_nonfluorescent 0

    addParameter(FloatNumber,  "nonfl_tau_a",              "triplet decay time of species a",              "&tau;<sub>trip,a</sub>",     "usec",        "&mu;s",                 true,      true,         true,              QFFitFunction::DisplayError, false, 3.0,          0,        10,       0.1, 0  );
    #define FCCSDiff_nonfl_taua 1
    addParameter(FloatNumber,  "nonfl_theta_a",            "triplet fraction of species a",                "&theta;<sub>trip,a</sub>",   "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 0.2,          0,        0.99999,  0.1, 0,      1);
    #define FCCSDiff_nonfl_thetaa 2
    addParameter(FloatNumber,  "nonfl_tau_ab",              "triplet decay time of species ab",              "&tau;<sub>trip,ab</sub>",     "usec",        "&mu;s",                 true,      true,         true,              QFFitFunction::DisplayError, false, 3.0,          0,        10,       0.1, 0  );
    #define FCCSDiff_nonfl_tauab 3
    addParameter(FloatNumber,  "nonfl_theta_ab",            "triplet fraction of species ab",                "&theta;<sub>trip,ab</sub>",   "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 0.2,          0,        0.99999,  0.1, 0,      1);
    #define FCCSDiff_nonfl_thetaab 4

    addParameter(FloatNumber,  "concentration_a",         "concentration of species a in focus",         "C<sub>a</sub>",           "nM",         "nM",                              true,      true,          true,              QFFitFunction::DisplayError, false, 10,          0,        1e50,     1    );
    #define FCCSDiff_concentrationa 5
    addParameter(FloatNumber,  "concentration_ab",         "concentration of species ab in focus",       "C<sub>ab</sub>",           "nM",         "nM",                             true,      true,          true,              QFFitFunction::DisplayError, false, 5,          0,        1e50,     1    );
    #define FCCSDiff_concentrationab 6
    addParameter(FloatNumber,  "diff_acoeff_a",            "diffusion coefficient of species a",                  "&Gamma;<sub>a</sub>",            "micron^2/s^alpha", "&mu;m<sup>2</sup>/s<sup>&alpha;</sup>",    true,      true,         true,              QFFitFunction::DisplayError, false, 10,           1e-10,    1e50,     1    );
    #define FCCSDiff_diff_acoeffa 7
    addParameter(FloatNumber,  "diff_alpha_a",            "anomality parameter of species a",                  "&alpha;<sub>a</sub>",            "", "",                             true,      true,         true,              QFFitFunction::DisplayError, false, 1,            0.01,     100,      0.1, 0       );
    #define FCCSDiff_diff_alphaa 8
    addParameter(FloatNumber,  "diff_acoeff_ab",            "diffusion coefficient of species ab",                  "&Gamma;<sub>ab</sub>",            "micron^2/s^alpha", "&mu;m<sup>2</sup>/s<sup>&alpha;</sup>",    true,      true,         true,              QFFitFunction::DisplayError, false, 10,           1e-10,    1e50,     1    );
    #define FCCSDiff_diff_acoeffab 9
    addParameter(FloatNumber,  "diff_alpha_ab",            "anomality parameter of species ab",                  "&alpha;<sub>ab</sub>",            "", "",                             true,      true,         true,              QFFitFunction::DisplayError, false, 1,            0.01,     100,      0.1, 0       );
    #define FCCSDiff_diff_alphaab 10
    addParameter(FloatNumber,  "offset",                  "correlation offset",                                   "G<sub>&infin;</sub>",      "",           "",                       true,      true,         true,              QFFitFunction::DisplayError, true, 0,            -10,      10,       0.1  );
    #define FCCSDiff_offset 11
    addParameter(FloatNumber,  "focus_distance_x",         "foci: lateral distance in x-direction",               "d<sub>x</sub>",            "nm",         "nm",                     true,      true,         true,             QFFitFunction::EditError,    true, 0,           -1e6,     1e6,      10  );
    #define FCCSDiff_focus_distancex 12
    addParameter(FloatNumber,  "focus_distance_y",         "foci: lateral distance in y-direction",               "d<sub>y</sub>",            "nm",         "nm",                     true,      true,         true,             QFFitFunction::EditError,    true, 0,              -1e6,     1e6,      10  );
    #define FCCSDiff_focus_distancey 13
    addParameter(FloatNumber,  "focus_distance_z",         "foci: longitudinal distance in z-direction",          "d<sub>z</sub>",            "nm",         "nm",                     true,      true,         true,             QFFitFunction::EditError,    true, 0,              -1e6,     1e6,      10  );
    #define FCCSDiff_focus_distancez 14
    addParameter(FloatNumber,  "focus_struct_fac1",        "green PSF: structure factor",                        "&gamma;<sub>g</sub>",      "",         "",                     true,      true,         true,              QFFitFunction::EditError,    true, 6,         1e-5,     1e5,      1  );
    #define FCCSDiff_focus_structfac1 15
    addParameter(FloatNumber,  "focus_width1",             "green PSF: lateral radius (1/e² radius)",             "w<sub>g</sub>",    "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 250,          0,        1e4,      10    );
    #define FCCSDiff_focus_width1 16
    addParameter(FloatNumber,  "focus_volume1",            "green focus: effective colume",                               "V<sub>eff,g</sub>",          "fl",         "fl",                     false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define FCSSDiff_focus_volume 17
    addParameter(FloatNumber,  "count_rate1",              "count rate green, during measurement",                      "&lang;F<sub>g</sub>&rang;",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError,    false, 0,            0,        1e50,     1    );
    #define FCCSDiff_count_rate1 18
    addParameter(FloatNumber,  "background1",              "background count rate green, during measurement",           "B<sub>g</sub>",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError  ,  false, 0,            0,        1e50,     1    );
    #define FCCSDiff_background1 19
    addParameter(FloatNumber,  "brightness_a",             "molar brightness of fluorophore on A",           "&eta;<sub>a</sub>",            "cpm",           "cpm",    false,      false,          false,              QFFitFunction::DisplayError, false, 0.5             );
    #define FCCSDiff_brightness_a 20

}


double QFFitFunctionsFCCSFWADiff2ColorACFG::evaluate(double t, const double* data) const {
    const double cab=data[FCCSDiff_concentrationab]*6.022e-1; // FCCSDiff_concentrationab given in 1e-9*6.022e23 particles/litre but c should be in particles per µm³= particles/10^{-15}litres=1e15 particles/litre
    const double ca=data[FCCSDiff_concentrationa]*6.022e-1; // FCCSDiff_concentrationa given in 1e-9*6.022e23 particles/litre but c should be in particles per µm³= particles/10^{-15}litres=1e15 particles/litre
    const double Da=data[FCCSDiff_diff_acoeffa];
    const double Dab=data[FCCSDiff_diff_acoeffab];
    const double alphaa=data[FCCSDiff_diff_alphaa];
    const double alphaab=data[FCCSDiff_diff_alphaab];

    const double dx=data[FCCSDiff_focus_distancex]/1000.0;
    const double dy=data[FCCSDiff_focus_distancey]/1000.0;
    const double dz=data[FCCSDiff_focus_distancez]/1000.0;

    const double wxyG=data[FCCSDiff_focus_width1]/1.0e3;
    const double gammaG=data[FCCSDiff_focus_structfac1];
    const double wzG=gammaG*wxyG;

    const int nNonFl=qBound(0, (int)round(data[FCCSDiff_n_nonfluorescent]), 1);
    const double tauT_a=data[FCCSDiff_nonfl_taua]*1e-6;
    //const double tauT_b=data[FCCSDiff_nonfl_taub]*1e-6;
    const double tauT_ab=data[FCCSDiff_nonfl_tauab]*1e-6;
    const double thetaT_a=data[FCCSDiff_nonfl_thetaa];
    //const double thetaT_b=data[FCCSDiff_nonfl_thetab];
    const double thetaT_ab=data[FCCSDiff_nonfl_thetaab];

    const double offset=data[FCCSDiff_offset];


    const double background1=data[FCCSDiff_background1];
    const double cr1=data[FCCSDiff_count_rate1];

    double backfactor=qfSqr((cr1-background1)/(cr1));
    if (fabs(cr1)<1e-15 || (fabs(background1)<1e-15)) backfactor=1;

    const double etaG=(cr1-background1)/(ca+cab);
    const double Fg=etaG*(ca+cab);

    // double Grr_b=etaR*etaR*cb*QFFitFunctionsFCCSFWADiff2ColorCCF_corrfactor(a, 0, 0, 0, Db, alphab, t, wxyR, wxyR, wzR, wzR);
    // double Grr_ab=etaR*etaR*cab*QFFitFunctionsFCCSFWADiff2ColorCCF_corrfactor(a, 0, 0, 0, Dab, alphaab, t, wxyR, wxyR, wzR, wzR);
     double Ggg_a=etaG*etaG*ca*QFFitFunctionFCCSFWTriplet(nNonFl, t, thetaT_a, tauT_a)*QFFitFunctionsFCCSFWADiff2ColorCCF_corrfactor( 0, 0, 0, Da, alphaa, t, wxyG, wxyG, wzG, wzG);
     double Ggg_ab=etaG*etaG*cab*QFFitFunctionFCCSFWTriplet(nNonFl, t, thetaT_ab, tauT_ab)*QFFitFunctionsFCCSFWADiff2ColorCCF_corrfactor( 0, 0, 0, Dab, alphaab, t, wxyG, wxyG, wzG, wzG);
    // double Ggr_ab=etaG*etaR*cab*QFFitFunctionsFCCSFWADiff2ColorCCF_corrfactor(a, dx, dy, dz, Dab, alphaab, t, wxyG, wxyR, wzG, wzR);
    if (fabs(ca)<1e-15) Ggg_a=0;
    //if (fabs(cb)<1e-15) Grr_b=0;
    if (fabs(cab)<1e-15) /*Grr_ab=*/Ggg_ab=/*Ggr_ab=*/0;

    const double cfac=Ggg_a+Ggg_ab;

    return offset+backfactor*cfac/(Fg*Fg);
}

void QFFitFunctionsFCCSFWADiff2ColorACFG::calcParameter(double* data, double* error) const {
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

bool QFFitFunctionsFCCSFWADiff2ColorACFG::isParameterVisible(int parameter, const double* data) const {
    if (data) {
        switch(parameter) {
            case FCCSDiff_nonfl_taua:
            case FCCSDiff_nonfl_thetaa:
//            case FCCSDiff_nonfl_taub:
//            case FCCSDiff_nonfl_thetab:
            case FCCSDiff_nonfl_tauab:
            case FCCSDiff_nonfl_thetaab:
                return data[FCCSDiff_n_nonfluorescent]>0;

            default:
                return true;
        }
    }
    return true;
}

unsigned int QFFitFunctionsFCCSFWADiff2ColorACFG::getAdditionalPlotCount(const double* params) {
    return 0;
}

QString QFFitFunctionsFCCSFWADiff2ColorACFG::transformParametersForAdditionalPlot(int plot, double* params) {
    return "";
}
