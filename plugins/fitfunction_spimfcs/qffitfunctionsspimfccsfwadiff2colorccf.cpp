#include "qffitfunctionsspimfccsfwadiff2colorccf.h"
#include "qfmathtools.h"
#include <cmath>
#include "imfcstools.h"
QFFitFunctionsSPIMFCCSFWADiff2ColorCCF::QFFitFunctionsSPIMFCCSFWADiff2ColorCCF() {
    //           type,         id,                        name,                                                   label,                      unit,          unitlabel,               fit,       userEditable, userRangeEditable, displayError,               initialFIx,  initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(FloatNumber,  "concentration_a",         "concentration of species a in focus",         "C<sub>a</sub>",           "nM",         "nM",                              true,      true,          true,              QFFitFunction::DisplayError, false, 10,          0,        1e50,     1    );
    #define FCCSDiff_concentrationa 0
    addParameter(FloatNumber,  "concentration_b",         "concentration of species b in focus",         "C<sub>b</sub>",           "nM",         "nM",                              true,      true,          true,              QFFitFunction::DisplayError, false, 10,          0,        1e50,     1    );
    #define FCCSDiff_concentrationb 1
    addParameter(FloatNumber,  "concentration_ab",         "concentration of species ab in focus",       "C<sub>ab</sub>",           "nM",         "nM",                             true,      true,          true,              QFFitFunction::DisplayError, false, 5,          0,        1e50,     1    );
    #define FCCSDiff_concentrationab 2
    addParameter(FloatNumber,  "rel_concentration_ab",     "relative concentration of species ab in focus cAB/(cA+cB+cAB)",      "C<sub>ab</sub>/C<sub>all</sub>", "",  "",          false,      false,          false,              QFFitFunction::DisplayError, false, 0.2,           0,     1,      0.1, 0, 1     );
    #define FCCSDiff_relconcentration 3
    addParameter(FloatNumber,  "fraction_ab",     "relative concentration of species ab in focus cAB/min(cA,cB)",      "C<sub>ab</sub>/min(C<sub>a</sub>, C<sub>b</sub>)", "",  "",            false,      false,          false,              QFFitFunction::DisplayError, false, 0.2,           0,     1,      0.1, 0, 1     );
    #define FCCSDiff_fraction_ab 4
    addParameter(FloatNumber,  "disscciation_constant",     "disscciation constant KD of reaction ab <-> a+b",      "K<sub>d</sub>",            "nM",           "nM",    false,      false,          false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e-50,     1     );
    #define FCCSDiff_KD 5
    addParameter(FloatNumber,  "diff_acoeff_a",            "diffusion coefficient of species a",                  "&Gamma;<sub>a</sub>",            "micron^2/s^alpha", "&mu;m<sup>2</sup>/s<sup>&alpha;</sup>",    true,      true,         true,              QFFitFunction::DisplayError, false, 10,           1e-10,    1e50,     1    );
    #define FCCSDiff_diff_acoeffa 6
    addParameter(FloatNumber,  "diff_alpha_a",            "anomality parameter of species a",                  "&alpha;<sub>a</sub>",            "", "",                             true,      true,         true,              QFFitFunction::DisplayError, false, 1,            0.01,     100,      0.1, 0       );
    #define FCCSDiff_diff_alphaa 7
    addParameter(FloatNumber,  "diff_acoeff_b",            "diffusion coefficient of species b",                  "&Gamma;<sub>b</sub>",            "micron^2/s^alpha", "&mu;m<sup>2</sup>/s<sup>&alpha;</sup>",    true,      true,         true,              QFFitFunction::DisplayError, false, 10,           1e-10,    1e50,     1    );
    #define FCCSDiff_diff_acoeffb 8
    addParameter(FloatNumber,  "diff_alpha_b",            "anomality parameter of species b",                  "&alpha;<sub>b</sub>",            "", "",                             true,      true,         true,              QFFitFunction::DisplayError, false, 1,            0.01,     100,      0.1, 0       );
    #define FCCSDiff_diff_alphab 9
    addParameter(FloatNumber,  "diff_acoeff_ab",            "diffusion coefficient of species ab",                  "&Gamma;<sub>ab</sub>",            "micron^2/s^alpha", "&mu;m<sup>2</sup>/s<sup>&alpha;</sup>",    true,      true,         true,              QFFitFunction::DisplayError, false, 10,           1e-10,    1e50,     1    );
    #define FCCSDiff_diff_acoeffab 10
    addParameter(FloatNumber,  "diff_alpha_ab",            "anomality parameter of species ab",                  "&alpha;<sub>ab</sub>",            "", "",                             true,      true,         true,              QFFitFunction::DisplayError, false, 1,            0.01,     100,      0.1, 0       );
    #define FCCSDiff_diff_alphaab 11
    addParameter(FloatNumber,  "offset",                  "correlation offset",                                   "G<sub>&infin;</sub>",      "",           "",                       true,      true,         true,              QFFitFunction::DisplayError, true, 0,            -10,      10,       0.1  );
    #define FCCSDiff_offset 12
    addParameter(FloatNumber,  "crosstalk",               "crosstalk coefficient",                                "&kappa;",                  "",         "",                         true,      true,         true,             QFFitFunction::EditError,    true, 0,           0,     1,      0.1, 0, 1  );
    #define FCCSDiff_crosstalk 13
    addParameter(FloatNumber,  "focus_distance_x",         "foci: lateral distance in x-direction",               "d<sub>x</sub>",            "nm",         "nm",                     true,      true,         true,             QFFitFunction::EditError,    true, 0,           -1e6,     1e6,      10  );
    #define FCCSDiff_focus_distancex 14
    addParameter(FloatNumber,  "focus_distance_y",         "foci: lateral distance in y-direction",               "d<sub>y</sub>",            "nm",         "nm",                     true,      true,         true,             QFFitFunction::EditError,    true, 0,              -1e6,     1e6,      10  );
    #define FCCSDiff_focus_distancey 15
    addParameter(FloatNumber,  "focus_distance_z",         "foci: longitudinal distance in z-direction",          "d<sub>z</sub>",            "nm",         "nm",                     true,      true,         true,             QFFitFunction::EditError,    true, 0,              -1e6,     1e6,      10  );
    #define FCCSDiff_focus_distancez 16
    addParameter(FloatNumber,  "focus_height1",            "green PSF: axial radius (1/e² radius)",               "z<sub>g</sub>",      "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 1240,         0.01,     1e5,      10  );
    #define FCCSDiff_focus_height1 17
    addParameter(FloatNumber,  "focus_width1",             "green PSF: lateral radius (1/e² radius)",             "w<sub>g</sub>",    "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 600,          0,        1e4,      10    );
    #define FCCSDiff_focus_width1 18
    addParameter(FloatNumber,  "focus_height2",            "red PSF: axial radius (1/e² radius)",               "z<sub>r</sub>",      "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 1300,         0.01,     1e5,      10  );
    #define FCCSDiff_focus_height2 19
    addParameter(FloatNumber,  "focus_width2",             "red PSF: lateral radius (1/e² radius)",             "w<sub>r</sub>",    "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 660,          0,        1e4,      10    );
    #define FCCSDiff_focus_width2 20
    addParameter(FloatNumber,  "pixel_width",             "pixel width",                                           "a",                        "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 400,          0,        1e4,      10    );
    #define FCCSDiff_pixel_width 21
    addParameter(FloatNumber,  "count_rate1",              "count rate green, during measurement",                      "&lang;F<sub>g</sub>&rang;",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError,    false, 0,            0,        1e50,     1    );
    #define FCCSDiff_count_rate1 22
    addParameter(FloatNumber,  "background1",              "background count rate green, during measurement",           "B<sub>g</sub>",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError  ,  false, 0,            0,        1e50,     1    );
    #define FCCSDiff_background1 23
    addParameter(FloatNumber,  "count_rate2",              "count rate red, during measurement",                      "&lang;F<sub>r</sub>&rang;",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError,    false, 0,            0,        1e50,     1    );
    #define FCCSDiff_count_rate2 24
    addParameter(FloatNumber,  "background2",              "background count rate red, during measurement",           "B<sub>r</sub>",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError  ,  false, 0,            0,        1e50,     1    );
    #define FCCSDiff_background2 25

}


double QFFitFunctionsSPIMFCCSFWADiff2ColorCCF::evaluate(double t, const double* data) const {
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

    const double wzG=data[FCCSDiff_focus_height1]/1.0e3;
    const double wxyG=data[FCCSDiff_focus_width1]/1.0e3;

    const double wzR=data[FCCSDiff_focus_height2]/1.0e3;
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

    const double etaG=(cr1-background1)/(ca+cab);
    const double etaR=(cr2-background2-kappa*(cr1-background1))/(cb+cab);
    const double Fg=etaG*(ca+cab);
    const double Fr=etaR*(cb+cab)+kappa*Fg;

    // double Grr_b=etaR*etaR*cb*QFFitFunctionsSPIMFCCSFWADiff2ColorCCF_corrfactor(a, dx, dy, dz, Db, alphab, t, wxyR, wxyR, wzR, wzR);
    // double Grr_ab=etaR*etaR*cab*QFFitFunctionsSPIMFCCSFWADiff2ColorCCF_corrfactor(a, dx, dy, dz, Dab, alphaab, t, wxyR, wxyR, wzR, wzR);
     double Ggg_a=etaG*etaG*ca*QFFitFunctionsSPIMFCCSFWADiff2ColorCCF_corrfactor(a, dx, dy, dz, Da, alphaa ,t, wxyG, wxyG, wzG, wzG);
     double Ggg_ab=etaG*etaG*cab*QFFitFunctionsSPIMFCCSFWADiff2ColorCCF_corrfactor(a, dx, dy, dz, Dab, alphaab, t, wxyG, wxyG, wzG, wzG);
     double Ggr_ab=etaG*etaR*cab*QFFitFunctionsSPIMFCCSFWADiff2ColorCCF_corrfactor(a, dx, dy, dz, Dab, alphaab, t, wxyG, wxyR, wzG, wzR);
    if (fabs(ca)<1e-15) Ggg_a=0;
    //if (fabs(cb)<1e-15) Grr_b=0;
    if (fabs(cab)<1e-15) /*Grr_ab=*/Ggg_ab=Ggr_ab=0;

    const double cfac=Ggr_ab+kappa*(Ggg_a+Ggg_ab);

    return offset+backfactor*cfac/(Fg*Fr);
}

void QFFitFunctionsSPIMFCCSFWADiff2ColorCCF::calcParameter(double* data, double* error) const {
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
//    const double wxyG=data[FCCSDiff_focus_width1]/1.0e3;
//    double ewxyG=0;

//    const double wzR=data[FCCSDiff_focus_height2]/1.0e3;
//    double ewzR=0;
//    const double wxyR=data[FCCSDiff_focus_width2]/1.0e3;
//    double ewxyR=0;

//    const double a=data[FCCSDiff_pixel_width]/1.0e3;
//    double ea=0;

//    const double offset=data[FCCSDiff_offset];
//    double eoffset=0;

//    const double kappa=data[FCCSDiff_crosstalk];
//    double ekappa=0;

//    const double background1=data[FCCSDiff_background1];
//    double ebackground1=0;
//    const double cr1=data[FCCSDiff_count_rate1];
//    double ecr1=0;
//    const double background2=data[FCCSDiff_background2];
//    double ebackground2=0;
//    const double cr2=data[FCCSDiff_count_rate2];
//    double ecr2=0;
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
//        ea=error[FCCSDiff_pixel_width]/1.0e3;
//        eoffset=error[FCCSDiff_offset];
//        ekappa=error[FCCSDiff_crosstalk];
//        ebackground1=error[FCCSDiff_background1];
//        ecr1=error[FCCSDiff_count_rate1];
//        ebackground2=error[FCCSDiff_background2];
//        ecr2=error[FCCSDiff_count_rate2];
    }

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

bool QFFitFunctionsSPIMFCCSFWADiff2ColorCCF::isParameterVisible(int parameter, const double* data) const {
    return true;
}

unsigned int QFFitFunctionsSPIMFCCSFWADiff2ColorCCF::getAdditionalPlotCount(const double* params) {
    return 0;
}

QString QFFitFunctionsSPIMFCCSFWADiff2ColorCCF::transformParametersForAdditionalPlot(int plot, double* params) {
    return "";
}
