#include "qffitfunctionsspimfccsfwfactordiff2colorccf.h"
#include "qfmathtools.h"
#include <cmath>
#include "imfcstools.h"
#include <QDebug>


QFFitFunctionsSPIMFCCSFWFactorDiff2ColorCCF::QFFitFunctionsSPIMFCCSFWFactorDiff2ColorCCF() {
    //           type,         id,                        name,                                                   label,                      unit,          unitlabel,               fit,       userEditable, userRangeEditable, displayError,               initialFIx,  initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(FloatNumber,  "concentration_a",         "concentration of species a in focus",         "C<sub>a</sub>",           "nM",         "nM",                              true,      true,          true,              QFFitFunction::DisplayError, false, 10,          0,        1e50,     1    );
    #define FCCSDiff_concentrationa 0
    addParameter(FloatNumber,  "concentration_b",         "concentration of species b in focus",         "C<sub>b</sub>",           "nM",         "nM",                              true,      true,          true,              QFFitFunction::DisplayError, false, 10,          0,        1e50,     1    );
    #define FCCSDiff_concentrationb 1
    addParameter(FloatNumber,  "concentration_ab",         "concentration of species ab in focus",       "C<sub>ab</sub>",           "nM",         "nM",                             true,      true,          true,              QFFitFunction::DisplayError, false, 5,          0,        1e50,     1    );
    #define FCCSDiff_concentrationab 2
    addParameter(FloatNumber,  "rel_concentration_ab",     "relative concentration of species ab in focus cAB/(cA+cB+cAB)",      "C<sub>ab</sub>/C<sub>all</sub>", "",  "",          false,      false,          false,              QFFitFunction::DisplayError, false, 0.2,           0,     1,      0.1, 0, 1     );
    #define FCCSDiff_relconcentration 3
    addParameter(FloatNumber,  "fraction_ab",     "relative concentration of species ab in focus cAB/(cA+cB)",      "C<sub>ab</sub>/(C<sub>a</sub>+C<sub>b</sub>)", "",  "",            false,      false,          false,              QFFitFunction::DisplayError, false, 0.2,           0,     1,      0.1, 0, 1     );
    #define FCCSDiff_fraction_ab 4
    addParameter(FloatNumber,  "fraction_ab_min",     "relative concentration of species ab in focus cAB/min(cA,cB)",      "C<sub>ab</sub>/min(C<sub>a</sub>, C<sub>b</sub>)", "",  "",            false,      false,          false,              QFFitFunction::DisplayError, false, 0.2,           0,     1,      0.1, 0, 1     );
    #define FCCSDiff_fraction_ab_min 5
    addParameter(FloatNumber,  "fraction_abmax",     "relative concentration of species ab in focus cAB/max(cA,cB)",      "C<sub>ab</sub>/max(C<sub>a</sub>, C<sub>b</sub>)", "",  "",            false,      false,          false,              QFFitFunction::DisplayError, false, 0.2,           0,     1,      0.1, 0, 1     );
    #define FCCSDiff_fraction_abmax 6

    addParameter(FloatNumber,  "disscciation_constant",     "disscciation constant KD of reaction ab <-> a+b",      "K<sub>d</sub>",            "nM",           "nM",    false,      false,          false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e-50,     1     );
    #define FCCSDiff_KD 7
    addParameter(FloatNumber,  "diff_coeff_a",            "diffusion coefficient of species a",                  "D<sub>a</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    true,      true,         true,              QFFitFunction::DisplayError, false, 10,           1e-10,    1e50,     1    );
    #define FCCSDiff_diff_coeffa 8
    addParameter(FloatNumber,  "diff_coefffactor_b",     "diffusion coefficient factor of species b",           "f<sub>b</sub>=D<sub>b</sub>/D<sub>a</sub>",            "", "",    true,      true,         true,              QFFitFunction::DisplayError, false, 1,           0,    1e50,     0.1    );
    #define FCCSDiff_diff_factorb 9
    addParameter(FloatNumber,  "diff_coeff_b",            "diffusion coefficient of species b",                  "D<sub>b</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    false,    false,        false,              QFFitFunction::DisplayError, false, 10,           1e-10,    1e50,     1    );
    #define FCCSDiff_diff_coeffb 10
    addParameter(FloatNumber,  "diff_coefffactor_ab",     "diffusion coefficient factor of species ab",           "f<sub>ab</sub>=D<sub>ab</sub>/D<sub>a</sub>",            "", "",    true,      true,         true,              QFFitFunction::DisplayError, false, 1,           0,    1e50,     0.1    );
    #define FCCSDiff_diff_factorab 11
    addParameter(FloatNumber,  "diff_coeff_ab",            "diffusion coefficient of species ab",                  "D<sub>ab</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    false,    false,        false,              QFFitFunction::DisplayError, false, 10,           1e-10,    1e50,     1    );
    #define FCCSDiff_diff_coeffab 12
    addParameter(FloatNumber,  "offset",                  "correlation offset",                                   "G<sub>&infin;</sub>",      "",           "",                       true,      true,         true,              QFFitFunction::DisplayError, true, 0,            -10,      10,       0.1  );
    #define FCCSDiff_offset 13
    addParameter(FloatNumber,  "crosstalk",               "crosstalk coefficient",                                "&kappa;",                  "",         "",                         true,      true,         true,             QFFitFunction::EditError,    true, 0,           0,     1,      0.1, 0, 1  );
    #define FCCSDiff_crosstalk 14
    addParameter(FloatNumber,  "focus_distance_x",         "foci: lateral distance in x-direction",               "d<sub>x</sub>",            "nm",         "nm",                     true,      true,         true,             QFFitFunction::EditError,    true, 0,           -1e6,     1e6,      10  );
    #define FCCSDiff_focus_distancex 15
    addParameter(FloatNumber,  "focus_distance_y",         "foci: lateral distance in y-direction",               "d<sub>y</sub>",            "nm",         "nm",                     true,      true,         true,             QFFitFunction::EditError,    true, 0,              -1e6,     1e6,      10  );
    #define FCCSDiff_focus_distancey 16
    addParameter(FloatNumber,  "focus_distance_z",         "foci: longitudinal distance in z-direction",          "d<sub>z</sub>",            "nm",         "nm",                     true,      true,         true,             QFFitFunction::EditError,    true, 0,              -1e6,     1e6,      10  );
    #define FCCSDiff_focus_distancez 17
    addParameter(FloatNumber,  "focus_height1",            "green PSF: axial radius (1/e² radius)",               "z<sub>g</sub>",      "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 1240,         0.01,     1e5,      10  );
    #define FCCSDiff_focus_height1 18
    addParameter(FloatNumber,  "focus_width1",             "green PSF: lateral radius (1/e² radius)",             "w<sub>g</sub>",    "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 600,          0,        1e4,      10    );
    #define FCCSDiff_focus_width1 19
    addParameter(FloatNumber,  "focus_height2",            "red PSF: axial radius (1/e² radius)",               "z<sub>r</sub>",      "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 1300,         0.01,     1e5,      10  );
    #define FCCSDiff_focus_height2 20
    addParameter(FloatNumber,  "focus_width2",             "red PSF: lateral radius (1/e² radius)",             "w<sub>r</sub>",    "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 660,          0,        1e4,      10    );
    #define FCCSDiff_focus_width2 21
    addParameter(FloatNumber,  "pixel_width",             "pixel width",                                           "a",                        "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 400,          0,        1e4,      10    );
    #define FCCSDiff_pixel_width 22
    addParameter(FloatNumber,  "count_rate1",              "count rate green, during measurement",                      "&lang;F<sub>g</sub>&rang;",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError,    false, 0,            0,        1e50,     1    );
    #define FCCSDiff_count_rate1 23
    addParameter(FloatNumber,  "background1",              "background count rate green, during measurement",           "B<sub>g</sub>",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError  ,  false, 0,            0,        1e50,     1    );
    #define FCCSDiff_background1 24
    addParameter(FloatNumber,  "count_rate2",              "count rate red, during measurement",                      "&lang;F<sub>r</sub>&rang;",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError,    false, 0,            0,        1e50,     1    );
    #define FCCSDiff_count_rate2 25
    addParameter(FloatNumber,  "background2",              "background count rate red, during measurement",           "B<sub>r</sub>",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError  ,  false, 0,            0,        1e50,     1    );
    #define FCCSDiff_background2 26
    addParameter(FloatNumber,  "brightness_a",             "molar brightness of fluorophore on A",           "&eta;<sub>a</sub>",            "counts/nM",           "counts/nM",    false,      false,          false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e-50,     1     );
    #define FCCSDiff_brightness_a 27
    addParameter(FloatNumber,  "brightness_b",             "molar brightness of fluorophore on B",           "&eta;<sub>b</sub>",            "counts/nM",           "counts/nM",    false,      false,          false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e-50,     1     );
    #define FCCSDiff_brightness_b 28

}


double QFFitFunctionsSPIMFCCSFWFactorDiff2ColorCCF::evaluate(double t, const double* data) const {
    const double cab=data[FCCSDiff_concentrationab]*6.022e-1; // FCCSDiff_concentrationab given in 1e-9*6.022e23 particles/litre but c should be in particles per µm³= particles/10^{-15}litres=1e15 particles/litre
    const double ca=data[FCCSDiff_concentrationa]*6.022e-1; // FCCSDiff_concentrationa given in 1e-9*6.022e23 particles/litre but c should be in particles per µm³= particles/10^{-15}litres=1e15 particles/litre
    const double cb=data[FCCSDiff_concentrationb]*6.022e-1; // FCCSDiff_concentrationb given in 1e-9*6.022e23 particles/litre but c should be in particles per µm³= particles/10^{-15}litres=1e15 particles/litre
    const double Da=data[FCCSDiff_diff_coeffa];
    //const double Db=data[FCCSDiff_diff_factorb]*Da;
    const double Dab=data[FCCSDiff_diff_factorab]*Da;

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

    // double Grr_b=etaR*etaR*cb*QFFitFunctionsSPIMFCCSFWDiff2ColorCCF_corrfactor(a, dx, dy, dz, Db, t, wxyR, wxyR, wzR, wzR);
    // double Grr_ab=etaR*etaR*cab*QFFitFunctionsSPIMFCCSFWDiff2ColorCCF_corrfactor(a, dx, dy, dz, Dab, t, wxyR, wxyR, wzR, wzR);
     double Ggg_a=etaG*etaG*ca*QFFitFunctionsSPIMFCCSFWDiff2ColorCCF_corrfactor(a, dx, dy, dz, Da, t, wxyG, wxyG, wzG, wzG);
     double Ggg_ab=etaG*etaG*cab*QFFitFunctionsSPIMFCCSFWDiff2ColorCCF_corrfactor(a, dx, dy, dz, Dab, t, wxyG, wxyG, wzG, wzG);
     double Ggr_ab=etaG*etaR*cab*QFFitFunctionsSPIMFCCSFWDiff2ColorCCF_corrfactor(a, dx, dy, dz, Dab, t, wxyG, wxyR, wzG, wzR);
    if (fabs(ca)<1e-15) Ggg_a=0;
    //if (fabs(cb)<1e-15) Grr_b=0;
    if (fabs(cab)<1e-15) /*Grr_ab=*/Ggg_ab=Ggr_ab=0;


    //qDebug()<<"CCF: etaG="<<etaG<<" etaR="<<etaR<<" ca="<<ca<<" cab="<<cab<<" Fg="<<Fg<<" Fr="<<Fr<<" Ggg_a="<<Ggg_a<<" Ggg_ab="<<Ggg_ab<<" Ggr_ab="<<Ggr_ab;
    const double cfac=Ggr_ab+kappa*(Ggg_a+Ggg_ab);

    return offset+backfactor*cfac/(Fg*Fr);
}

void QFFitFunctionsSPIMFCCSFWFactorDiff2ColorCCF::calcParameter(double* data, double* error) const {
    const double cab=data[FCCSDiff_concentrationab]*6.022e-1; // FCCSDiff_concentrationab given in 1e-9*6.022e23 particles/litre but c should be in particles per µm³= particles/10^{-15}litres=1e15 particles/litre
    double ecab=0;
    const double ca=data[FCCSDiff_concentrationa]*6.022e-1; // FCCSDiff_concentrationa given in 1e-9*6.022e23 particles/litre but c should be in particles per µm³= particles/10^{-15}litres=1e15 particles/litre
    double eca=0;
    const double cb=data[FCCSDiff_concentrationb]*6.022e-1; // FCCSDiff_concentrationb given in 1e-9*6.022e23 particles/litre but c should be in particles per µm³= particles/10^{-15}litres=1e15 particles/litre
    double ecb=0;
    const double Da=data[FCCSDiff_diff_coeffa];
    double eDa=0;
    const double Fb=data[FCCSDiff_diff_factorb];
    double eFb=0;
    const double Fab=data[FCCSDiff_diff_factorab];
    double eFab=0;

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
        eDa=error[FCCSDiff_diff_coeffa];
        eFb=error[FCCSDiff_diff_factorb];
        eFab=error[FCCSDiff_diff_factorab];
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
        ekappa=error[FCCSDiff_crosstalk];
        ebackground1=error[FCCSDiff_background1];
        ecr1=error[FCCSDiff_count_rate1];
        ebackground2=error[FCCSDiff_background2];
        ecr2=error[FCCSDiff_count_rate2];
    }

    data[FCCSDiff_diff_coeffb]=Da*Fb;
    if (error) error[FCCSDiff_diff_coeffb]=qfErrorMul(Da, eDa, Fb, eFb);
    data[FCCSDiff_diff_coeffab]=Da*Fab;
    if (error) error[FCCSDiff_diff_coeffab]=qfErrorMul(Da, eDa, Fab, eFab);

    data[FCCSDiff_brightness_a]=(cr1-background1)/(ca+cab);
    if (error) error[FCCSDiff_brightness_a]=0;
    data[FCCSDiff_brightness_b]=(cr2-background2-kappa*(cr1-background1))/(cb+cab);
    if (error) error[FCCSDiff_brightness_b]=0;

    data[FCCSDiff_relconcentration]=cab/(ca+cb+cab);
    if (error) error[FCCSDiff_relconcentration]=sqrt(qfSqr(eca*cab/qfSqr(ca+cb+cab))+qfSqr(ecb*cab/qfSqr(ca+cb+cab))+qfSqr(ecab*(ca+cb)/qfSqr(ca+cb+cab)));

    if (ca<cb) {
        data[FCCSDiff_fraction_ab_min]=cab/ca;
        if (error) error[FCCSDiff_fraction_ab_min]=qfErrorDiv(cab, ecab, ca, eca);
    } else {
        data[FCCSDiff_fraction_ab_min]=cab/cb;
        if (error) error[FCCSDiff_fraction_ab_min]=qfErrorDiv(cab, ecab, cb, ecb);
    }
    if (ca>cb) {
        data[FCCSDiff_fraction_abmax]=cab/ca;
        if (error) error[FCCSDiff_fraction_abmax]=qfErrorDiv(cab, ecab, ca, eca);
    } else {
        data[FCCSDiff_fraction_abmax]=cab/cb;
        if (error) error[FCCSDiff_fraction_abmax]=qfErrorDiv(cab, ecab, cb, ecb);
    }

    data[FCCSDiff_fraction_ab]=cab/(ca+cb);
    if (error) error[FCCSDiff_fraction_ab]=sqrt(qfSqr(eca*cab/qfSqr(ca+cb))+qfSqr(ecb*cab/qfSqr(ca+cb))+qfSqr(ecab/(ca+cb)));

    data[FCCSDiff_KD]=ca*cb/cab/6.022e-1;
    if (error) error[FCCSDiff_KD]=sqrt(qfSqr(eca*cb/cab)+qfSqr(ecb*ca/cab)+qfSqr(ecab*ca*cb/qfSqr(cab)))/6.022e-1;

}

bool QFFitFunctionsSPIMFCCSFWFactorDiff2ColorCCF::isParameterVisible(int parameter, const double* data) const {
    return true;
}

unsigned int QFFitFunctionsSPIMFCCSFWFactorDiff2ColorCCF::getAdditionalPlotCount(const double* params) {
    return 0;
}

QString QFFitFunctionsSPIMFCCSFWFactorDiff2ColorCCF::transformParametersForAdditionalPlot(int plot, double* params) {
    return "";
}
