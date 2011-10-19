#include "qffitfunctionsspimfcsdiff.h"

#include <cmath>
#define sqr(x) ((x)*(x))
#define cube(x) ((x)*(x)*(x))
#define pow4(x) ((x)*(x)*(x)*(x))
#define pow5(x) ((x)*(x)*(x)*(x)*(x))
#define NAVOGADRO (6.02214179e23)

QFFitFunctionsSPIMFCSDiff::QFFitFunctionsSPIMFCSDiff() {
    //           type,         id,                        name,                                                    label,                      unit,          unitlabel,               fit,       userEditable, userRangeEditable, displayError,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(FloatNumber,  "n_particle",              "Particle number N",                                     "N",                        "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, 100,          1e-10,    1e5,      1,   0);
    #define FCSSDiff_n_particle 0
    addParameter(FloatNumber,  "1n_particle",             "1/Particle number N",                                   "1/N",                      "",            "",                      false,     false,        false,             QFFitFunction::DisplayError, 0.01,         1e-10,    1e5,      0.1, 0);
    #define FCSSDiff_1n_particle 1
    addParameter(FloatNumber,  "diff_coeff1",             "diffusion coefficient of species 1",                    "D<sub>1</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    true,      true,         true,              QFFitFunction::DisplayError, 10,           0,        1e50,     1    );
    #define FCSSDiff_diff_coeff1 2
    addParameter(FloatNumber,  "offset",                  "correlation offset",                                    "G<sub>&infin;</sub>",      "",           "",                       true,      true,         true,              QFFitFunction::DisplayError, 0,            -10,      10,       0.1  );
    #define FCSSDiff_offset 3
    addParameter(FloatNumber,  "focus_hieght",            "PSF: axial radius",                                     "&sigma;<sub>z</sub>",      "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    2000,         0.01,     1e5,      10  );
    #define FCSSDiff_focus_height 4
    addParameter(FloatNumber,  "focus_width",             "PSF: lateral radius",                                   "&sigma;<sub>x,y</sub>",    "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    650,          0,        1e4,      10    );
    #define FCSSDiff_focus_width 5
    addParameter(FloatNumber,  "pixel_width",             "pixel width",                                           "a",                        "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    400,          0,        1e4,      10    );
    #define FCSSDiff_pixel_width 6
    addParameter(FloatNumber,  "focus_volume",            "focus: effective colume",                               "V<sub>eff</sub>",          "fl",         "fl",                     false,    false,        false,              QFFitFunction::DisplayError, 0.5,          0,        1e50,     1    );
    #define FCSSDiff_focus_volume 7
    addParameter(FloatNumber,  "concentration",           "particle concentration in focus",                       "C<sub>all</sub>",          "nM",         "nM",                     false,    false,        false,              QFFitFunction::DisplayError, 0.5,          0,        1e50,     1    );
    #define FCSSDiff_concentration 8
}

double QFFitFunctionsSPIMFCSDiff::evaluate(double t, const double* data) const {
    const double N=data[FCSSDiff_n_particle];
    const double D1=data[FCSSDiff_diff_coeff1];
    const double sigmaz=data[FCSSDiff_focus_height];
    const double wxy=data[FCSSDiff_focus_width]/1.0e3;
    const double a=data[FCSSDiff_pixel_width]/1.0e3;
    const double offset=data[FCSSDiff_offset];

    const double fac_z1=sqrt(a+D1*t/sqr(sigmaz));
    const double dt_sigma2=D1*t+sqr(wxy);
    const double fac_xy1=2.0*a*erf(a/2.0/sqrt(dt_sigma2))+4*sqrt(dt_sigma2/M_PI)*(exp(-0.25*sqr(a)/dt_sigma2)-1.0);
    const double d1=1.0/fac_z1*sqr(fac_xy1);
    const double pre=0.25/sqr(a)/sqrt(M_PI);
    return offset+pre/N*d1;
}

void QFFitFunctionsSPIMFCSDiff::evaluateDerivatives(double* derivatives, double t, const double* data) const {
}

void QFFitFunctionsSPIMFCSDiff::calcParameter(double* data, double* error) const {
    double N=data[FCSSDiff_n_particle];
    double eN=0;
    //double D1=data[FCSSDiff_diff_coeff1];
    double eD1=0;
    double sigmaz=data[FCSSDiff_focus_height]/1.0e3;
    double esigmaz=0;
    //double wxy=data[FCSSDiff_focus_width]/1.0e3;
    double ewxy=0;
    double a=data[FCSSDiff_pixel_width]/1.0e3;
    double ea=0;
    //double offset=data[FCSSDiff_offset];
    double eoffset=0;

    if (error) {
        eN=error[FCSSDiff_n_particle];
        eD1=error[FCSSDiff_diff_coeff1];
        esigmaz=error[FCSSDiff_focus_height]/1.0e3;
        ewxy=error[FCSSDiff_focus_width]/1.0e3;
        ea=error[FCSSDiff_pixel_width]/1.0e3;
        eoffset=error[FCSSDiff_offset];
    }

    // calculate 1/N
    data[FCSSDiff_1n_particle]=1.0/N;
    if (error) error[FCSSDiff_1n_particle]=fabs(eN/N/N);

    // calculate Veff = 2 * a^2 * sigmaz
    data[FCSSDiff_focus_volume]=2*a*a*sigmaz;
    if (error) error[FCSSDiff_focus_volume]=sqrt(sqr(esigmaz*2.0*a*a)+sqr(ea*4.0*a*sigmaz));

    // calculate C = N / Veff
    if (data[FCSSDiff_focus_volume]!=0) data[FCSSDiff_concentration]=N/data[FCSSDiff_focus_volume]/(NAVOGADRO * 1.0e-24); else data[FCSSDiff_concentration]=0;
    if (error) {
        if ((sigmaz!=0)&&(a!=0)) error[FCSSDiff_concentration]=sqrt( sqr(eN/sqr(a)/2.0/sigmaz) + sqr(esigmaz*N/2.0/sqr(a)/sqr(sigmaz)) + sqr(ea*N/cube(a)/sigmaz) )/(NAVOGADRO * 1.0e-24);
        else error[FCSSDiff_concentration]=0;
    }
}

bool QFFitFunctionsSPIMFCSDiff::isParameterVisible(int parameter, const double* data) const {
    return true;
}

unsigned int QFFitFunctionsSPIMFCSDiff::getAdditionalPlotCount(const double* params) {
    return 0;
}

QString QFFitFunctionsSPIMFCSDiff::transformParametersForAdditionalPlot(int plot, double* params) {
    return "";
}
