#include "qffitfunctionsspimfccsfwdiff2colorccf.h"
#include "qfmathtools.h"
#include <cmath>
#include "imfcstools.h"
QFFitFunctionsSPIMFCCSFWDiff2ColorCCF::QFFitFunctionsSPIMFCCSFWDiff2ColorCCF() {
    //           type,         id,                        name,                                                   label,                      unit,          unitlabel,               fit,       userEditable, userRangeEditable, displayError,               initialFIx,  initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(FloatNumber,  "concentration_a",         "concentration of species a in focus",         "C<sub>a</sub>",           "nM",         "nM",                              true,      true,          true,              QFFitFunction::DisplayError, false, 10,          0,        1e50,     1    );
    #define FCCSDiff_concentrationa 0
    addParameter(FloatNumber,  "concentration_b",         "concentration of species b in focus",         "C<sub>b</sub>",           "nM",         "nM",                              true,      true,          true,              QFFitFunction::DisplayError, false, 10,          0,        1e50,     1    );
    #define FCCSDiff_concentrationb 1
    addParameter(FloatNumber,  "concentration_ab",         "concentration of species ab in focus",       "C<sub>ab</sub>",           "nM",         "nM",                             true,      true,          true,              QFFitFunction::DisplayError, false, 5,          0,        1e50,     1    );
    #define FCCSDiff_concentrationab 2
    addParameter(FloatNumber,  "rel_concentration_ab",     "relative concentration of species ab in focus",      "C<sub>ab</sub>/C<sub>all</sub>", "",  "",                          false,      false,          false,              QFFitFunction::DisplayError, false, 0.2,           0,     1,      0.1, 0, 1     );
    #define FCCSDiff_relconcentration 3
    addParameter(FloatNumber,  "equilibrium_constant",     "equilibrium constant KD of reaction a+b -> ab",      "K<sub>D</sub>",            "1/nM",           "nM<sup>-1</sup>",    false,      false,          false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e-50,     1     );
    #define FCCSDiff_relconcentration 4
    addParameter(FloatNumber,  "diff_coeff_a",            "diffusion coefficient of species a",                  "D<sub>a</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    true,      true,         true,              QFFitFunction::DisplayError, false, 10,           1e-10,    1e50,     1    );
    #define FCCSDiff_diff_coeffa 5
    addParameter(FloatNumber,  "diff_coeff_b",            "diffusion coefficient of species b",                  "D<sub>b</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    true,      true,         true,              QFFitFunction::DisplayError, false, 10,           1e-10,    1e50,     1    );
    #define FCCSDiff_diff_coeffb 6
    addParameter(FloatNumber,  "diff_coeff_ab",            "diffusion coefficient of species ab",                  "D<sub>ab</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    true,      true,         true,              QFFitFunction::DisplayError, false, 10,           1e-10,    1e50,     1    );
    #define FCCSDiff_diff_coeffab 7
    addParameter(FloatNumber,  "offset",                  "correlation offset",                                   "G<sub>&infin;</sub>",      "",           "",                       true,      true,         true,              QFFitFunction::DisplayError, true, 0,            -10,      10,       0.1  );
    #define FCCSDiff_offset 8
    addParameter(FloatNumber,  "crosstalk",               "crosstalk coefficient",                                "&kappa;",                  "",         "",                         true,      true,         true,             QFFitFunction::EditError,    true, 0,           0,     1,      0.1, 0, 1  );
    #define FCCSDiff_crosstalk 9
    addParameter(FloatNumber,  "focus_distance_x",         "foci: lateral distance in x-direction",               "d<sub>x</sub>",            "nm",         "nm",                     true,      true,         true,             QFFitFunction::EditError,    true, 0,           -1e6,     1e6,      10  );
    #define FCCSDiff_focus_distancex 10
    addParameter(FloatNumber,  "focus_distance_y",         "foci: lateral distance in y-direction",               "d<sub>y</sub>",            "nm",         "nm",                     true,      true,         true,             QFFitFunction::EditError,    true, 0,              -1e6,     1e6,      10  );
    #define FCCSDiff_focus_distancey 11
    addParameter(FloatNumber,  "focus_distance_z",         "foci: longitudinal distance in z-direction",          "d<sub>z</sub>",            "nm",         "nm",                     true,      true,         true,             QFFitFunction::EditError,    true, 0,              -1e6,     1e6,      10  );
    #define FCCSDiff_focus_distancez 12
    addParameter(FloatNumber,  "focus_height1",            "PSF1: axial radius (1/sqrt(e) radius)",               "&sigma;<sub>1,z</sub>",      "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 620,         0.01,     1e5,      10  );
    #define FCCSDiff_focus_height1 13
    addParameter(FloatNumber,  "focus_width1",             "PSF1: lateral radius (1/sqrt(e) radius)",             "&sigma;<sub>1,xy</sub>",    "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 300,          0,        1e4,      10    );
    #define FCCSDiff_focus_width1 14
    addParameter(FloatNumber,  "focus_height2",            "PSF2: axial radius (1/sqrt(e) radius)",               "&sigma;<sub>2,z</sub>",      "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 650,         0.01,     1e5,      10  );
    #define FCCSDiff_focus_height2 15
    addParameter(FloatNumber,  "focus_width2",             "PSF2: lateral radius (1/sqrt(e) radius)",             "&sigma;<sub>2,xy</sub>",    "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 330,          0,        1e4,      10    );
    #define FCCSDiff_focus_width2 16
    addParameter(FloatNumber,  "pixel_width",             "pixel width",                                           "a",                        "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 400,          0,        1e4,      10    );
    #define FCCSDiff_pixel_width 17
    addParameter(FloatNumber,  "count_rate1",              "count rate 1 during measurement",                      "count rate 1",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError,    false, 0,            0,        1e50,     1    );
    #define FCCSDiff_count_rate1 18
    addParameter(FloatNumber,  "background1",              "background count rate 1 during measurement",           "background 1",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError  ,  false, 0,            0,        1e50,     1    );
    #define FCCSDiff_background1 19
    addParameter(FloatNumber,  "count_rate2",              "count rate 2 during measurement",                      "count rate 2",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError,    false, 0,            0,        1e50,     1    );
    #define FCCSDiff_count_rate2 20
    addParameter(FloatNumber,  "background2",              "background count rate 2 during measurement",           "background 2",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError  ,  false, 0,            0,        1e50,     1    );
    #define FCCSDiff_background2 21

}


double QFFitFunctionsSPIMFCCSFWDiff2ColorCCF::evaluate(double t, const double* data) const {
    const double cab=data[FCCSDiff_concentrationab]*6.022e-1; // FCCSDiff_concentrationab given in 1e-9*6.022e23 particles/litre but c should be in particles per µm³= particles/10^{-15}litres=1e15 particles/litre
    const double ca=data[FCCSDiff_concentrationa]*6.022e-1; // FCCSDiff_concentrationa given in 1e-9*6.022e23 particles/litre but c should be in particles per µm³= particles/10^{-15}litres=1e15 particles/litre
    const double cb=data[FCCSDiff_concentrationb]*6.022e-1; // FCCSDiff_concentrationb given in 1e-9*6.022e23 particles/litre but c should be in particles per µm³= particles/10^{-15}litres=1e15 particles/litre
    const double D1=data[FCCSDiff_diff_coeffab];

    const double dx=data[FCCSDiff_focus_distancex]/1000.0;
    const double dy=data[FCCSDiff_focus_distancey]/1000.0;
    const double dz=data[FCCSDiff_focus_distancez]/1000.0;

    const double sigmaz1=data[FCCSDiff_focus_height1]/1.0e3;
    const double wxy1=data[FCCSDiff_focus_width1]/1.0e3;
    const double sigmaz2=data[FCCSDiff_focus_height2]/1.0e3;
    const double wxy2=data[FCCSDiff_focus_width2]/1.0e3;
    const double a=data[FCCSDiff_pixel_width]/1.0e3;
    const double offset=data[FCCSDiff_offset];
    const double background1=data[FCCSDiff_background1];
    const double cr1=data[FCCSDiff_count_rate1];
    const double background2=data[FCCSDiff_background2];
    const double cr2=data[FCCSDiff_count_rate2];
    //TODO backfactor!!!
    double backfactor=(cr1-background1)/(cr1) * (cr2-background2)/(cr2);
    if (fabs(cr1)<1e-15 || fabs(cr2)<1e-15 || (fabs(background1)<1e-15 && fabs(background2)<1e-15)) backfactor=1;


    double cfac=QFFitFunctionsSPIMFCCSFWDiff2ColorCCF_pixelcorrfactor(a, dx, D1, t, wxy1, wxy2)*
                QFFitFunctionsSPIMFCCSFWDiff2ColorCCF_pixelcorrfactor(a, dy, D1, t, wxy1, wxy2)*
                QFFitFunctionsSPIMFCCSFWDiff2ColorCCF_zcorrfactor(dz, D1, t, sigmaz1, sigmaz2);


    return offset+backfactor*cab/(ca*cb)*cfac;
}

void QFFitFunctionsSPIMFCCSFWDiff2ColorCCF::calcParameter(double* data, double* error) const {

}

bool QFFitFunctionsSPIMFCCSFWDiff2ColorCCF::isParameterVisible(int parameter, const double* data) const {
    return true;
}

unsigned int QFFitFunctionsSPIMFCCSFWDiff2ColorCCF::getAdditionalPlotCount(const double* params) {
    return 0;
}

QString QFFitFunctionsSPIMFCCSFWDiff2ColorCCF::transformParametersForAdditionalPlot(int plot, double* params) {
    return "";
}
