#include "qffitfunctionstirfcsadiffe2.h"
#include <cmath>
#include "qftools.h"
#include "imfcstools.h"
#define sqr(x) qfSqr(x)
#define cube(x) qfCube(x)
#define pow4(x) qfPow4(x)
#define NAVOGADRO QF_NAVOGADRO

QFFitFunctionsTIRFCSADiffE2::QFFitFunctionsTIRFCSADiffE2() {
    //           type,         id,                        name,                                                    label,                      unit,          unitlabel,               fit,       userEditable, userRangeEditable, displayError,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(IntCombo,     "n_components",            "number of diffusing components",                        "components",               "",            "",                      false,     true,         false,             QFFitFunction::NoError,      false, 1,            1,        3,        1,   1,      2);
    #define FCSSDiff_n_components 0
    addParameter(FloatNumber,  "n_particle",              "Particle number N",                                     "N",                        "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 100,          1e-10,    1e5,      1,   0);
    #define FCSSDiff_n_particle 1
    addParameter(FloatNumber,  "1n_particle",             "1/Particle number N",                                   "1/N",                      "",            "",                      false,     false,        false,             QFFitFunction::DisplayError, false, 0.01,         1e-10,    1e5,      0.1, 0);
    #define FCSSDiff_1n_particle 2




    addParameter(FloatNumber,  "diff_rho1",               "fraction of first component",                           "&rho;<sub>1</sub>",        "",            "",                      false,     false,        false,             QFFitFunction::DisplayError, false, 0.5,          0,        0.99999,  0.1, 0,      1);
    #define FCSSDiff_diff_rho1 3
    addParameter(FloatNumber,  "diff_acoeff1",             "diffusion coefficient of species 1",                    "&Gamma;<sub>1</sub>",            "micron^2/s^{\alpha_1}", "&mu;m<sup>2</sup>/s<sup>&alpha;1</sup>",    true,      true,         true,              QFFitFunction::DisplayError, false, 10,           1e-10,    1e50,     1    );
    #define FCSSDiff_diff_coeff1 4
    addParameter(FloatNumber,  "diff_alpha1",             "anomality parameter of species 1",                      "&alpha;<sub>1</sub>",      "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 1,            0.01,     100,      0.1, 0        );
    #define FCSDiff_diff_alpha1 5
    addParameter(FloatNumber,  "diff_rho2",               "fraction of second component",                          "&rho;<sub>2</sub>",        "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 0.5,          0,        0.99999,  0.1, 0,      1  );
    #define FCSSDiff_diff_rho2 6
    addParameter(FloatNumber,  "diff_acoeff2",             "diffusion coefficient of species 2",                    "&Gamma;<sub>2</sub>",            "micron^2/s^{\alpha_1}", "&mu;m<sup>2</sup>/s<sup>&alpha;1</sup>",    true,      true,         true,              QFFitFunction::DisplayError, false, 10,           1e-10,    1e50,     1    );
    #define FCSSDiff_diff_coeff2 7
    addParameter(FloatNumber,  "diff_alpha2",             "anomality parameter of species 2",                      "&alpha;<sub>2</sub>",      "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 1,            0.01,     100,      0.1, 0        );
    #define FCSDiff_diff_alpha2 8
    addParameter(FloatNumber,  "diff_rho3",               "fraction of third component",                           "&rho;<sub>3</sub>",        "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 0.5,          0,        0.99999,  0.1, 0,      1  );
    #define FCSSDiff_diff_rho3 9
    addParameter(FloatNumber,  "diff_acoeff3",             "diffusion coefficient of species 3",                    "&Gamma;<sub>3</sub>",            "micron^2/s^{\alpha_1}", "&mu;m<sup>2</sup>/s<sup>&alpha;1</sup>",    true,      true,         true,              QFFitFunction::DisplayError, false, 10,           1e-10,    1e50,     1    );
    #define FCSSDiff_diff_coeff3 10
    addParameter(FloatNumber,  "diff_alpha3",             "anomality parameter of species 3",                      "&alpha;<sub>3</sub>",      "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 1,            0.01,     100,      0.1, 0        );
    #define FCSDiff_diff_alpha3 11

    addParameter(FloatNumber,  "offset",                  "correlation offset",                                    "G<sub>&infin;</sub>",      "",           "",                       true,      true,         true,              QFFitFunction::DisplayError, true, 0,            -10,      10,       0.1  );
    #define FCSSDiff_offset 12
    addParameter(FloatNumber,  "focus_width",             "PSF: lateral radius (1/e² radius)",                     "w<sub>x,y</sub>",    "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 600,          0,        1e4,      10    );
    #define FCSSDiff_focus_width 13
    addParameter(FloatNumber,  "pixel_width",             "pixel width",                                           "a",                        "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 400,          0,        1e4,      10    );
    #define FCSSDiff_pixel_width 14
    addParameter(FloatNumber,  "efective_area",            "focus: effective area",                               "A<sub>eff</sub>",          "micron^2",         "&mu;m<sup>2</sup>",                     false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define FCSSDiff_focus_volume 15
    addParameter(FloatNumber,  "concentration",           "particle concentration in focus",                       "C<sub>all</sub>",          "particles/micron^2",         "particles/&mu;m<sup>2</sup>",                     false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define FCSSDiff_concentration 16
    addParameter(FloatNumber,  "count_rate",              "count rate during measurement",                         "count rate",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError,    false, 0,            0,        1e50,     1    );
    #define FCSSDiff_count_rate 17
    addParameter(FloatNumber,  "background",              "background count rate during measurement",              "background",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError  ,  false, 0,            0,        1e50,     1    );
    #define FCSSDiff_background 18
    addParameter(FloatNumber,  "cpm",                     "photon counts per molecule",                            "cnt/molec",                "Hz",         "Hz",                     false,    false,        false,              QFFitFunction::DisplayError, false, 0,            0,        1e50,     1    );
    #define FCSSDiff_cpm 19
}

double QFFitFunctionsTIRFCSADiffE2::evaluate(double t, const double* data) const {
    const int comp=data[FCSSDiff_n_components];
    const double N=data[FCSSDiff_n_particle];
    const double rho2=data[FCSSDiff_diff_rho2];
    const double rho3=data[FCSSDiff_diff_rho3];
    const double GAMMA1=data[FCSSDiff_diff_coeff1];
    const double GAMMA2=data[FCSSDiff_diff_coeff2];
    const double GAMMA3=data[FCSSDiff_diff_coeff3];
    const double a1=data[FCSDiff_diff_alpha1];
    const double a2=data[FCSDiff_diff_alpha2];
    const double a3=data[FCSDiff_diff_alpha3];
    const double wxy=data[FCSSDiff_focus_width]/1.0e3;
    const double a=data[FCSSDiff_pixel_width]/1.0e3;
    const double offset=data[FCSSDiff_offset];
    const double background=data[FCSSDiff_background];
    const double cr=data[FCSSDiff_count_rate];
    double backfactor=sqr(cr-background)/sqr(cr);
    if (fabs(cr)<1e-15 || fabs(background)<1e-10) backfactor=1;

    double rho1=1;
    if (comp==2) {
        rho1=1.0-rho2;
    }
    if (comp==3) {
        rho1=1.0-rho2-rho3;
    }

    double cfac=0;

    if (comp>0) {
        cfac=rho1*QFFitFunctionsTIRFCCSFWADiff2ColorCCF_corrfactor_2Dxy(a, 0, 0, GAMMA1,a1, t, wxy, wxy);
    }

    if (comp>1) {
        cfac=cfac+rho2*QFFitFunctionsTIRFCCSFWADiff2ColorCCF_corrfactor_2Dxy(a, 0, 0, GAMMA2,a2, t, wxy, wxy);
    }

    if (comp>2) {
        cfac=cfac+rho2*QFFitFunctionsTIRFCCSFWADiff2ColorCCF_corrfactor_2Dxy(a, 0, 0, GAMMA3,a3, t, wxy, wxy);
    }

    const double Veff=TIRFCS_newAeff(a, wxy);
    const double pre=1.0/sqr(a);
    return offset+pre/(N/Veff)*cfac*backfactor;
}

void QFFitFunctionsTIRFCSADiffE2::evaluateDerivatives(double* derivatives, double t, const double* data) const {
}

void QFFitFunctionsTIRFCSADiffE2::sortParameter(double *parameterValues, double *error, bool *fix) const {

}

void QFFitFunctionsTIRFCSADiffE2::calcParameter(double* data, double* error) const {
    double N=data[FCSSDiff_n_particle];
    double eN=0;
    //double D1=data[FCSSDiff_diff_coeff1];
    double eD1=0;
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

    if (error) {
        eN=error[FCSSDiff_n_particle];
        eD1=error[FCSSDiff_diff_coeff1];
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

    // calculate Veff
    data[FCSSDiff_focus_volume]=TIRFCS_newAeff(a, wxy);;
    if (error) error[FCSSDiff_focus_volume]=TIRFCS_newAeffError(a, ea, wxy, ewxy);

    // calculate C = N / Veff
    if (data[FCSSDiff_focus_volume]!=0) {
        data[FCSSDiff_concentration]=N/data[FCSSDiff_focus_volume];
    } else {
        data[FCSSDiff_concentration]=0;
    }
    if (data[FCSSDiff_focus_volume]!=0 && error) {
        error[FCSSDiff_concentration]=sqrt(qfSqr(eN/data[FCSSDiff_focus_volume])+qfSqr(error[FCSSDiff_focus_volume]*N/qfSqr(data[FCSSDiff_focus_volume])));//N/data[FCSSDiff_focus_volume]
    }


    // calculate CPM = (CPS-background)/N
    data[FCSSDiff_cpm]=(cps-background)/N;
    error[FCSSDiff_cpm]=sqrt(sqr(ecps/N)+sqr(ebackground/N)+sqr(eN*(cps-background)/sqr(N)));

}

bool QFFitFunctionsTIRFCSADiffE2::isParameterVisible(int parameter, const double* data) const {
    int comp=data[FCSSDiff_n_components];
    switch(parameter) {
        case FCSSDiff_diff_rho1:  return comp>1;
        case FCSSDiff_diff_coeff1: case FCSDiff_diff_alpha1: return comp>0;
        case FCSSDiff_diff_rho2: case FCSSDiff_diff_coeff2: case FCSDiff_diff_alpha2: return comp>1;
        case FCSSDiff_diff_rho3: case FCSSDiff_diff_coeff3: case FCSDiff_diff_alpha3: return comp>2;
    }
    return true;
}

unsigned int QFFitFunctionsTIRFCSADiffE2::getAdditionalPlotCount(const double* params) {
    return 0;
}

QString QFFitFunctionsTIRFCSADiffE2::transformParametersForAdditionalPlot(int plot, double* params) {
    return "";
}
