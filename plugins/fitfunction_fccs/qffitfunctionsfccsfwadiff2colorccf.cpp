#include "qffitfunctionsfccsfwadiff2colorccf.h"
#include "qfmathtools.h"
#include <cmath>
#include "fcstools.h"
QFFitFunctionsFCCSFWADiff2ColorCCF::QFFitFunctionsFCCSFWADiff2ColorCCF() {
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
    addParameter(FloatNumber,  "concentration_b",         "concentration of species b in focus",         "C<sub>b</sub>",           "nM",         "nM",                              true,      true,          true,              QFFitFunction::DisplayError, false, 10,          0,        1e50,     1    );
    #define FCCSDiff_concentrationb 6
    addParameter(FloatNumber,  "concentration_ab",         "concentration of species ab in focus",       "C<sub>ab</sub>",           "nM",         "nM",                             true,      true,          true,              QFFitFunction::DisplayError, false, 5,          0,        1e50,     1    );
    #define FCCSDiff_concentrationab 7
    addParameter(FloatNumber,  "rel_concentration_ab",     "relative concentration of species ab in focus cAB/(cA+cB+cAB)",      "C<sub>ab</sub>/C<sub>all</sub>", "",  "",          false,      false,          false,              QFFitFunction::DisplayError, false, 0.2,           0,     1,      0.1, 0, 1     );
    #define FCCSDiff_relconcentration 8
    addParameter(FloatNumber,  "fraction_ab",     "relative concentration of species ab in focus cAB/min(cA,cB)",      "C<sub>ab</sub>/min(C<sub>a</sub>, C<sub>b</sub>)", "",  "",            false,      false,          false,              QFFitFunction::DisplayError, false, 0.2,           0,     1,      0.1, 0, 1     );
    #define FCCSDiff_fraction_ab 9
    addParameter(FloatNumber,  "disscciation_constant",     "disscciation constant KD of reaction ab <-> a+b",      "K<sub>d</sub>",            "nM",           "nM",    false,      false,          false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e-50,     1     );
    #define FCCSDiff_KD 10
    addParameter(FloatNumber,  "diff_acoeff_a",            "diffusion coefficient of species a",                  "&Gamma;<sub>a</sub>",            "micron^2/s^alpha", "&mu;m<sup>2</sup>/s<sup>&alpha;</sup>",    true,      true,         true,              QFFitFunction::DisplayError, false, 10,           1e-10,    1e50,     1    );
    #define FCCSDiff_diff_acoeffa 11
    addParameter(FloatNumber,  "diff_alpha_a",            "anomality parameter of species a",                  "&alpha;<sub>a</sub>",            "", "",                             true,      true,         true,              QFFitFunction::DisplayError, false, 1,            0.01,     100,      0.1, 0       );
    #define FCCSDiff_diff_alphaa 12
    addParameter(FloatNumber,  "diff_acoeff_b",            "diffusion coefficient of species b",                  "&Gamma;<sub>b</sub>",            "micron^2/s^alpha", "&mu;m<sup>2</sup>/s<sup>&alpha;</sup>",    true,      true,         true,              QFFitFunction::DisplayError, false, 10,           1e-10,    1e50,     1    );
    #define FCCSDiff_diff_acoeffb 13
    addParameter(FloatNumber,  "diff_alpha_b",            "anomality parameter of species b",                  "&alpha;<sub>b</sub>",            "", "",                             true,      true,         true,              QFFitFunction::DisplayError, false, 1,            0.01,     100,      0.1, 0       );
    #define FCCSDiff_diff_alphab 14
    addParameter(FloatNumber,  "diff_acoeff_ab",            "diffusion coefficient of species ab",                  "&Gamma;<sub>ab</sub>",            "micron^2/s^alpha", "&mu;m<sup>2</sup>/s<sup>&alpha;</sup>",    true,      true,         true,              QFFitFunction::DisplayError, false, 10,           1e-10,    1e50,     1    );
    #define FCCSDiff_diff_acoeffab 15
    addParameter(FloatNumber,  "diff_alpha_ab",            "anomality parameter of species ab",                  "&alpha;<sub>ab</sub>",            "", "",                             true,      true,         true,              QFFitFunction::DisplayError, false, 1,            0.01,     100,      0.1, 0       );
    #define FCCSDiff_diff_alphaab 16
    addParameter(FloatNumber,  "offset",                  "correlation offset",                                   "G<sub>&infin;</sub>",      "",           "",                       true,      true,         true,              QFFitFunction::DisplayError, true, 0,            -10,      10,       0.1  );
    #define FCCSDiff_offset 17
    addParameter(FloatNumber,  "crosstalk",               "crosstalk coefficient",                                "&kappa;",                  "",         "",                         true,      true,         true,             QFFitFunction::EditError,    true, 0,           0,     1,      0.1, 0, 1  );
    #define FCCSDiff_crosstalk 18
    addParameter(FloatNumber,  "focus_distance_x",         "foci: lateral distance in x-direction",               "d<sub>x</sub>",            "nm",         "nm",                     true,      true,         true,             QFFitFunction::EditError,    true, 0,           -1e6,     1e6,      10  );
    #define FCCSDiff_focus_distancex 19
    addParameter(FloatNumber,  "focus_distance_y",         "foci: lateral distance in y-direction",               "d<sub>y</sub>",            "nm",         "nm",                     true,      true,         true,             QFFitFunction::EditError,    true, 0,              -1e6,     1e6,      10  );
    #define FCCSDiff_focus_distancey 20
    addParameter(FloatNumber,  "focus_distance_z",         "foci: longitudinal distance in z-direction",          "d<sub>z</sub>",            "nm",         "nm",                     true,      true,         true,             QFFitFunction::EditError,    true, 0,              -1e6,     1e6,      10  );
    #define FCCSDiff_focus_distancez 21
    addParameter(FloatNumber,  "focus_struct_fac1",        "green PSF: structure factor",                        "&gamma;<sub>g</sub>",      "",         "",                     true,      true,         true,              QFFitFunction::EditError,    true, 6,         1e-5,     1e5,      1  );
    #define FCCSDiff_focus_structfac1 22
    addParameter(FloatNumber,  "focus_width1",             "green PSF: lateral radius (1/e² radius)",             "w<sub>g</sub>",    "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 250,          0,        1e4,      10    );
    #define FCCSDiff_focus_width1 23
    addParameter(FloatNumber,  "focus_struct_fac2",        "red PSF: structure factor",                        "&gamma;<sub>r</sub>",      "",         "",                     true,      true,         true,              QFFitFunction::EditError,    true, 6,         1e-5,     1e5,      1  );
    #define FCCSDiff_focus_structfac2 24
    addParameter(FloatNumber,  "focus_width2",             "red PSF: lateral radius (1/e² radius)",             "w<sub>r</sub>",    "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 270,          0,        1e4,      10    );
    #define FCCSDiff_focus_width2 25
    addParameter(FloatNumber,  "count_rate1",              "count rate green, during measurement",                      "&lang;F<sub>g</sub>&rang;",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError,    false, 0,            0,        1e50,     1    );
    #define FCCSDiff_count_rate1 26
    addParameter(FloatNumber,  "background1",              "background count rate green, during measurement",           "B<sub>g</sub>",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError  ,  false, 0,            0,        1e50,     1    );
    #define FCCSDiff_background1 27
    addParameter(FloatNumber,  "count_rate2",              "count rate red, during measurement",                      "&lang;F<sub>r</sub>&rang;",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError,    false, 0,            0,        1e50,     1    );
    #define FCCSDiff_count_rate2 28
    addParameter(FloatNumber,  "background2",              "background count rate red, during measurement",           "B<sub>r</sub>",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError  ,  false, 0,            0,        1e50,     1    );
    #define FCCSDiff_background2 29
    addParameter(FloatNumber,  "brightness_a",             "molar brightness of fluorophore on A",           "&eta;<sub>a</sub>",            "counts/nM",           "counts/nM",    false,      false,          false,              QFFitFunction::DisplayError, false, 0.5             );
    #define FCCSDiff_brightness_a 30
    addParameter(FloatNumber,  "brightness_b",             "molar brightness of fluorophore on B",           "&eta;<sub>b</sub>",            "counts/nM",           "counts/nM",    false,      false,          false,              QFFitFunction::DisplayError, false, 0.5             );
    #define FCCSDiff_brightness_b 31

}


double QFFitFunctionsFCCSFWADiff2ColorCCF::evaluate(double t, const double* data) const {
    const double cab=data[FCCSDiff_concentrationab]*6.022e-1; // FCCSDiff_concentrationab given in 1e-9*6.022e23 particles/litre but c should be in particles per µm³= particles/10^{-15}litres=1e15 particles/litre
    const double ca=data[FCCSDiff_concentrationa]*6.022e-1; // FCCSDiff_concentrationa given in 1e-9*6.022e23 particles/litre but c should be in particles per µm³= particles/10^{-15}litres=1e15 particles/litre
    const double cb=data[FCCSDiff_concentrationb]*6.022e-1; // FCCSDiff_concentrationb given in 1e-9*6.022e23 particles/litre but c should be in particles per µm³= particles/10^{-15}litres=1e15 particles/litre
    const double Da=data[FCCSDiff_diff_acoeffa];
    const double Db=data[FCCSDiff_diff_acoeffb];
    const double Dab=data[FCCSDiff_diff_acoeffab];
    const double alphaa=data[FCCSDiff_diff_alphaa];
    const double alphab=data[FCCSDiff_diff_alphab];
    const double alphaab=data[FCCSDiff_diff_alphaab];


    const double dx=data[FCCSDiff_focus_distancex]/1000.0;
    const double dy=data[FCCSDiff_focus_distancey]/1000.0;
    const double dz=data[FCCSDiff_focus_distancez]/1000.0;

    const double wxyG=data[FCCSDiff_focus_width1]/1.0e3;
    const double gammaG=data[FCCSDiff_focus_structfac1];
    const double wzG=gammaG*wxyG;

    const double wxyR=data[FCCSDiff_focus_width2]/1.0e3;
    const double gammaR=data[FCCSDiff_focus_structfac2];
    const double wzR=gammaR*wxyR;

    const int nNonFl=qBound(0, (int)round(data[FCCSDiff_n_nonfluorescent]), 1);
    const double tauT_a=data[FCCSDiff_nonfl_taua]*1e-6;
    //const double tauT_b=data[FCCSDiff_nonfl_taub]*1e-6;
    const double tauT_ab=data[FCCSDiff_nonfl_tauab]*1e-6;
    const double thetaT_a=data[FCCSDiff_nonfl_thetaa];
    //const double thetaT_b=data[FCCSDiff_nonfl_thetab];
    const double thetaT_ab=data[FCCSDiff_nonfl_thetaab];

    const double offset=data[FCCSDiff_offset];

    const double kappa=data[FCCSDiff_crosstalk];

    const double background1=data[FCCSDiff_background1];
    const double cr1=data[FCCSDiff_count_rate1];
    const double background2=data[FCCSDiff_background2];
    const double cr2=data[FCCSDiff_count_rate2];
    double backfactor=(cr1-background1)/(cr1) * (cr2-background2)/(cr2);
    if (fabs(cr1)<1e-15 || fabs(cr2)<1e-15 || (fabs(background1)<1e-15 && fabs(background2)<1e-15)) backfactor=1;

    const double etaG=(cr1-background1)/(ca+cab);
    const double etaR=(cr2-background2-kappa*(cr1-background1))/(cb+cab);
    const double Fg=etaG*(ca+cab);
    const double Fr=etaR*(cb+cab)+kappa*Fg;

    // double Grr_b=etaR*etaR*cb*QFFitFunctionsFCCSFWADiff2ColorCCF_corrfactor(a, dx, dy, dz, Db, alphab, t, wxyR, wxyR, wzR, wzR);
    // double Grr_ab=etaR*etaR*cab*QFFitFunctionsFCCSFWADiff2ColorCCF_corrfactor(a, dx, dy, dz, Dab, alphaab, t, wxyR, wxyR, wzR, wzR);
     double Ggg_a=etaG*etaG*ca*QFFitFunctionFCCSFWTriplet(nNonFl, t, thetaT_a, tauT_a)*QFFitFunctionsFCCSFWADiff2ColorCCF_corrfactor(dx, dy, dz, Da, alphaa ,t, wxyG, wxyG, wzG, wzG);
     double Ggg_ab=etaG*etaG*cab*QFFitFunctionFCCSFWTriplet(nNonFl, t, thetaT_ab, tauT_ab)*QFFitFunctionsFCCSFWADiff2ColorCCF_corrfactor(dx, dy, dz, Dab, alphaab, t, wxyG, wxyG, wzG, wzG);
     double Ggr_ab=etaG*etaR*cab*QFFitFunctionsFCCSFWADiff2ColorCCF_corrfactor(dx, dy, dz, Dab, alphaab, t, wxyG, wxyR, wzG, wzR);
    if (fabs(ca)<1e-15) Ggg_a=0;
    //if (fabs(cb)<1e-15) Grr_b=0;
    if (fabs(cab)<1e-15) /*Grr_ab=*/Ggg_ab=Ggr_ab=0;

    const double cfac=Ggr_ab+kappa*(Ggg_a+Ggg_ab);

    return offset+backfactor*cfac/(Fg*Fr);
}

void QFFitFunctionsFCCSFWADiff2ColorCCF::calcParameter(double* data, double* error) const {
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
        ecab=error[FCCSDiff_concentrationab]*6.022e-1; // FCCSDiff_concentrationab given in 1e-9*6.022e23 particles/litre but c should be in particles per µm³= particles/10^{-15}litres=1e15 particles/litre
        eca=error[FCCSDiff_concentrationa]*6.022e-1; // FCCSDiff_concentrationa given in 1e-9*6.022e23 particles/litre but c should be in particles per µm³= particles/10^{-15}litres=1e15 particles/litre
        ecb=error[FCCSDiff_concentrationb]*6.022e-1; // FCCSDiff_concentrationb given in 1e-9*6.022e23 particles/litre but c should be in particles per µm³= particles/10^{-15}litres=1e15 particles/litre
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
//        eoffset=error[FCCSDiff_offset];
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


    data[FCCSDiff_relconcentration]=cab/(ca+cb+cab);
    if (error) error[FCCSDiff_relconcentration]=sqrt(qfSqr(eca*cab/qfSqr(ca+cb+cab))+qfSqr(ecb*cab/qfSqr(ca+cb+cab))+qfSqr(ecab*(ca+cb)/qfSqr(ca+cb+cab)));

    if (ca<cb) {
        data[FCCSDiff_fraction_ab]=cab/ca;
        if (error) error[FCCSDiff_fraction_ab]=qfErrorDiv(cab, ecab, ca, eca);
    } else {
        data[FCCSDiff_fraction_ab]=cab/cb;
        if (error) error[FCCSDiff_fraction_ab]=qfErrorDiv(cab, ecab, cb, ecb);
    }

    data[FCCSDiff_KD]=ca*cb/cab/6.022e-1;
    if (error) error[FCCSDiff_KD]=sqrt(qfSqr(eca*cb/cab)+qfSqr(ecb*ca/cab)+qfSqr(ecab*ca*cb/qfSqr(cab)))/6.022e-1;

}

bool QFFitFunctionsFCCSFWADiff2ColorCCF::isParameterVisible(int parameter, const double* data) const {
    if (data) {
        switch(parameter) {
            case FCCSDiff_nonfl_taua:
            case FCCSDiff_nonfl_thetaa:
            //case FCCSDiff_nonfl_taub:
            //case FCCSDiff_nonfl_thetab:
            case FCCSDiff_nonfl_tauab:
            case FCCSDiff_nonfl_thetaab:
                return data[FCCSDiff_n_nonfluorescent]>0;

            default:
                return true;
        }
    }

    return true;
}

unsigned int QFFitFunctionsFCCSFWADiff2ColorCCF::getAdditionalPlotCount(const double* params) {
    return 0;
}

QString QFFitFunctionsFCCSFWADiff2ColorCCF::transformParametersForAdditionalPlot(int plot, double* params) {
    return "";
}
