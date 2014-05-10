#include "qffitfunctionsspimfcsadiffe2.h"
#include "qfpreprocessormagic.h"
#include "imfcstools.h"
#include <QDebug>
#include <cmath>
#define sqr(x) ((x)*(x))
#define cube(x) ((x)*(x)*(x))
#define pow4(x) ((x)*(x)*(x)*(x))
#define pow5(x) ((x)*(x)*(x)*(x)*(x))
#define NAVOGADRO (6.02214179e23)


QFFitFunctionsSPIMFCSADiffE2::QFFitFunctionsSPIMFCSADiffE2() {



    //           type,         id,                        name,                                                    label,                      unit,          unitlabel,               fit,       userEditable, userRangeEditable, displayError,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(IntCombo,     "n_components",            "number of diffusing components",                        "components",               "",            "",                      false,     true,         false,             QFFitFunction::NoError,      false, 1,            1,        3,        1,   1,      2);
    #define SPIMFCSADIFF_n_components QFPPM_COUNT_START
    addParameter(FloatNumber,  "n_particle",              "particle number N",                                     "N",                        "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 1,            1e-10,    1e5,      1,   0);
    #define SPIMFCSADIFF_n_particle QFPPM_INC(SPIMFCSADIFF_n_components)
    addParameter(FloatNumber,  "1n_particle",             "1/particle number N",                                   "1/N",                      "",            "",                      false,     false,        false,             QFFitFunction::DisplayError, false, 0.01,         1e-10,    1e5,      0.1, 0);
    #define SPIMFCSADIFF_1n_particle QFPPM_INC(SPIMFCSADIFF_n_particle)
    addParameter(FloatNumber,  "diff_rho1",               "fraction of first component",                           "&rho;<sub>1</sub>",        "",            "",                      false,     false,        false,             QFFitFunction::DisplayError, false, 0.5,          0,        0.99999,  0.1, 0,      1);
    #define SPIMFCSADIFF_diff_rho1 QFPPM_INC(SPIMFCSADIFF_1n_particle)
    addParameter(FloatNumber,  "diff_acoeff1",            "anomalous diffusion coefficient of species 1",          "&Gamma;<sub>1</sub>",      "micron^2/s^{\alpha_1}", "&mu;m<sup>2</sup>/s<sup>&alpha;1</sup>",    true,      true,         true,              QFFitFunction::DisplayError, false, 10,           1e-10,    1e50,     1    );
    #define SPIMFCSADIFF_diff_acoeff1 QFPPM_INC(SPIMFCSADIFF_diff_rho1)
    addParameter(FloatNumber,  "diff_alpha1",             "anomality parameter of species 1",                      "&alpha;<sub>1</sub>",      "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 1,            0.01,     100,      0.1, 0        );
    #define SPIMFCSADIFF_diff_alpha1 QFPPM_INC(SPIMFCSADIFF_diff_acoeff1)
    addParameter(FloatNumber,  "diff_rho2",               "fraction of second component",                          "&rho;<sub>2</sub>",        "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 0.5,          0,        0.99999,  0.1, 0,      1  );
    #define SPIMFCSADIFF_diff_rho2 QFPPM_INC(SPIMFCSADIFF_diff_alpha1)
    addParameter(FloatNumber,  "diff_acoeff2",            "anomalous diffusion coefficient of species 2",          "&Gamma;<sub>2</sub>",      "micron^2/s^{\alpha_2}", "&mu;m<sup>2</sup>/s<sup>&alpha;2</sup>",    true,      true,         true,              QFFitFunction::DisplayError, false, 10,           1e-10,    1e50,     1    );
    #define SPIMFCSADIFF_diff_acoeff2 QFPPM_INC(SPIMFCSADIFF_diff_rho2)
    addParameter(FloatNumber,  "diff_alpha2",             "anomality parameter of species 2",                      "&alpha;<sub>2</sub>",      "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 1,            0.01,     100,      0.1, 0        );
    #define SPIMFCSADIFF_diff_alpha2 QFPPM_INC(SPIMFCSADIFF_diff_acoeff2)
    addParameter(FloatNumber,  "diff_rho3",               "fraction of third component",                           "&rho;<sub>3</sub>",        "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 0.5,          0,        0.99999,  0.1, 0,      1  );
    #define SPIMFCSADIFF_diff_rho3 QFPPM_INC(SPIMFCSADIFF_diff_alpha2)
    addParameter(FloatNumber,  "diff_acoeff3",            "anomalous diffusion coefficient of species 3",          "&Gamma;<sub>3</sub>",      "micron^2/s^{\alpha_3}", "&mu;m<sup>2</sup>/s<sup>&alpha;3</sup>",    true,      true,         true,              QFFitFunction::DisplayError, false, 10,           1e-10,    1e50,     1    );
    #define SPIMFCSADIFF_diff_acoeff3 QFPPM_INC(SPIMFCSADIFF_diff_rho3)
    addParameter(FloatNumber,  "diff_alpha3",             "anomality parameter of species 3",                      "&alpha;<sub>3</sub>",      "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 1,            0.01,     100,      0.1, 0        );
    #define SPIMFCSADIFF_diff_alpha3 QFPPM_INC(SPIMFCSADIFF_diff_acoeff3)

    addParameter(FloatNumber,  "offset",                  "correlation offset",                                    "G<sub>&infin;</sub>",      "",           "",                       true,      true,         true,              QFFitFunction::DisplayError, true, 0,            -10,      10,       0.1  );
    #define SPIMFCSADIFF_offset QFPPM_INC(SPIMFCSADIFF_diff_alpha3)
    addParameter(FloatNumber,  "focus_hieght",            "PSF: axial radius (1/e²)",                              "w<sub>z</sub>",            "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 620,         0.01,     1e5,      10  );
    #define SPIMFCSADIFF_focus_height QFPPM_INC(SPIMFCSADIFF_offset)
    addParameter(FloatNumber,  "focus_width",             "PSF: lateral radius (1/e²)",                            "w<sub>x,y</sub>",          "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 300,          0,        1e4,      10    );
    #define SPIMFCSADIFF_focus_width QFPPM_INC(SPIMFCSADIFF_focus_height)
    addParameter(FloatNumber,  "pixel_width",             "pixel width",                                           "a",                        "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 400,          0,        1e4,      10    );
    #define SPIMFCSADIFF_pixel_width QFPPM_INC(SPIMFCSADIFF_focus_width)
    addParameter(FloatNumber,  "focus_volume",            "focus: effective colume",                               "V<sub>eff</sub>",          "fl",         "fl",                     false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define SPIMFCSADIFF_focus_volume QFPPM_INC(SPIMFCSADIFF_pixel_width)
    addParameter(FloatNumber,  "concentration",           "particle concentration in focus",                       "C<sub>all</sub>",          "nM",         "nM",                     false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define SPIMFCSADIFF_concentration QFPPM_INC(SPIMFCSADIFF_focus_volume)
    addParameter(FloatNumber,  "count_rate",              "count rate during measurement",                         "count rate",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError,    false, 0,            0,        1e50,     1    );
    #define SPIMFCSADIFF_count_rate QFPPM_INC(SPIMFCSADIFF_concentration)
    addParameter(FloatNumber,  "background",              "background count rate during measurement",              "background",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError  ,  false, 0,            0,        1e50,     1    );
    #define SPIMFCSADIFF_background QFPPM_INC(SPIMFCSADIFF_count_rate)
    addParameter(FloatNumber,  "cpm",                     "photon counts per molecule",                            "cnt/molec",                "Hz",         "Hz",                     false,    false,        false,              QFFitFunction::DisplayError, false, 0,            0,        1e50,     1    );
    #define SPIMFCSADIFF_cpm QFPPM_INC(SPIMFCSADIFF_background)


    addParameter(FloatNumber,  "msd_tau1",                "evaluate MSD at this time",                             "&tau;<sub>MSD</sub>",      "ms",         "ms",                     false,    true,         false,              QFFitFunction::NoError  ,   false, 1,            1e-10,        1e50,     1    );
    #define SPIMFCSADIFF_msd_tau QFPPM_INC(SPIMFCSADIFF_cpm)
    addParameter(FloatNumber,  "msd_at_tau1",              "MSD1 evaluated at given time tau_MSD",                  "MSD1(&tau;<sub>MSD</sub>)",  "micron^2",         "&mu;m<sup>2</sup>",             false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define SPIMFCSADIFF_msd_at_tau1 QFPPM_INC(SPIMFCSADIFF_msd_tau)
    addParameter(FloatNumber,  "msd_at_tau2",              "MSD2 evaluated at given time tau_MSD",                  "MSD2(&tau;<sub>MSD</sub>)",  "micron^2",         "&mu;m<sup>2</sup>",             false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define SPIMFCSADIFF_msd_at_tau2 QFPPM_INC(SPIMFCSADIFF_msd_at_tau1)
    addParameter(FloatNumber,  "msd_at_tau3",              "MSD3 evaluated at given time tau_MSD",                  "MSD3(&tau;<sub>MSD</sub>)",  "micron^2",         "&mu;m<sup>2</sup>",             false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define SPIMFCSADIFF_msd_at_tau3 QFPPM_INC(SPIMFCSADIFF_msd_at_tau2)
    addParameter(FloatNumber,  "effective_area",            "focus: effective area (xy)",                          "A<sub>eff</sub>",          "micron^2",         "&mu;m<sup>2</sup>",       false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define SPIMFCSADIFF_focus_effA QFPPM_INC(SPIMFCSADIFF_msd_at_tau3)
    addParameter(FloatNumber,  "msd_time_aeff1",              "time to leave effective focus area with MSD1",      "&tau;<sub>1</sub>(A<sub>eff</sub>)",  "ms",         "ms",             false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define SPIMFCSADIFF_msd_time_aeff1 QFPPM_INC(SPIMFCSADIFF_focus_effA)
    addParameter(FloatNumber,  "msd_time_aeff2",              "time to leave effective focus area with MSD2",      "&tau;<sub>2</sub>(A<sub>eff</sub>)",  "ms",         "ms",             false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define SPIMFCSADIFF_msd_time_aeff2 QFPPM_INC(SPIMFCSADIFF_msd_time_aeff1)
    addParameter(FloatNumber,  "msd_time_aeff3",              "time to leave effective focus area with MSD3",      "&tau;<sub>3</sub>(A<sub>eff</sub>)",  "ms",         "ms",             false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define SPIMFCSADIFF_msd_time_aeff3 QFPPM_INC(SPIMFCSADIFF_msd_time_aeff2)

}

double QFFitFunctionsSPIMFCSADiffE2::evaluate(double t, const double* data) const {
    const int comp=data[SPIMFCSADIFF_n_components];
    const double N=data[SPIMFCSADIFF_n_particle];
    const double GAMMA1=data[SPIMFCSADIFF_diff_acoeff1];
    const double GAMMA2=data[SPIMFCSADIFF_diff_acoeff2];
    const double GAMMA3=data[SPIMFCSADIFF_diff_acoeff3];
    const double a1=data[SPIMFCSADIFF_diff_alpha1];
    const double a2=data[SPIMFCSADIFF_diff_alpha2];
    const double a3=data[SPIMFCSADIFF_diff_alpha3];
    const double rho2=data[SPIMFCSADIFF_diff_rho2];
    const double rho3=data[SPIMFCSADIFF_diff_rho3];
    const double sigmaz=data[SPIMFCSADIFF_focus_height]/1.0e3;
    const double wxy=data[SPIMFCSADIFF_focus_width]/1.0e3;
    const double a=data[SPIMFCSADIFF_pixel_width]/1.0e3;
    const double offset=data[SPIMFCSADIFF_offset];
    const double background=data[SPIMFCSADIFF_background];
    const double cr=data[SPIMFCSADIFF_count_rate];
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
    const double sqpi=sqrt(M_PI);

    if (comp>0) {
        const double ta1=pow(t, a1);
        const double fac_z1=sqrt(1.0+4.0*GAMMA1*ta1/sqr(sigmaz));
        const double dt_sigma2=4.0*GAMMA1*ta1+sqr(wxy);
        const double fac_xy1=erf(a/sqrt(dt_sigma2))+sqrt(dt_sigma2)/(sqpi*a)*(exp(-sqr(a)/dt_sigma2)-1.0);
        const double d1=1.0/fac_z1*sqr(fac_xy1);
        cfac=rho1*d1;
    }

    if (comp>1) {
        const double ta2=pow(t, a2);
        const double fac_z2=sqrt(1.0+4.0*GAMMA2*ta2/sqr(sigmaz));
        const double dt_sigma22=4.0*GAMMA2*ta2+sqr(wxy);
        const double fac_xy2=erf(a/sqrt(dt_sigma22))+sqrt(dt_sigma22)/(sqpi*a)*(exp(-sqr(a)/dt_sigma22)-1.0);
        const double d2=1.0/fac_z2*sqr(fac_xy2);
        cfac=cfac+rho2*d2;
    }

    if (comp>2) {
        const double ta3=pow(t, a3);
        const double fac_z3=sqrt(1.0+4.0*GAMMA3*ta3/sqr(sigmaz));
        const double dt_sigma23=4.0*GAMMA3*ta3+sqr(wxy);
        const double fac_xy3=erf(a/sqrt(dt_sigma23))+sqrt(dt_sigma23)/(sqpi*a)*(exp(-sqr(a)/dt_sigma23)-1.0);
        const double d3=1.0/fac_z3*sqr(fac_xy3);
        cfac=cfac+rho3*d3;
    }

    const double Veff=sqpi*sqr(a)*sigmaz/sqr(erf(a/wxy)+wxy/sqpi/a*(exp(-sqr(a/wxy))-1.0));
    const double pre=1.0/sqr(a)/sqpi/sigmaz;
    return offset+pre/(N/Veff)*cfac*backfactor;

}



void QFFitFunctionsSPIMFCSADiffE2::sortParameter(double *parameterValues, double *error, bool *fix) const {


}

void QFFitFunctionsSPIMFCSADiffE2::calcParameter(double* data, double* error) const {
    double N=data[SPIMFCSADIFF_n_particle];
    double eN=0;
    //double D1=data[SPIMFCSADIFF_diff_acoeff1];
    double eD1=0;
    double sigmaz=data[SPIMFCSADIFF_focus_height]/1.0e3;
    double esigmaz=0;
    double wxy=data[SPIMFCSADIFF_focus_width]/1.0e3;
    double ewxy=0;
    double a=data[SPIMFCSADIFF_pixel_width]/1.0e3;
    double ea=0;
    //double offset=data[SPIMFCSADIFF_offset];
    double eoffset=0;

    int comp=data[SPIMFCSADIFF_n_components];
    double rho2=data[SPIMFCSADIFF_diff_rho2];
    double erho2=0;
    double rho3=data[SPIMFCSADIFF_diff_rho3];
    double erho3=0;
    double cps=data[SPIMFCSADIFF_count_rate];
    double ecps=0;
    double ecpm=0;
    double background=data[SPIMFCSADIFF_background];
    double ebackground=0;

    double tau_msd=data[SPIMFCSADIFF_msd_tau];
    double etau_msd=0;
    const double GAMMA1=data[SPIMFCSADIFF_diff_acoeff1];
    const double GAMMA2=data[SPIMFCSADIFF_diff_acoeff2];
    const double GAMMA3=data[SPIMFCSADIFF_diff_acoeff3];
    const double a1=data[SPIMFCSADIFF_diff_alpha1];
    const double a2=data[SPIMFCSADIFF_diff_alpha2];
    const double a3=data[SPIMFCSADIFF_diff_alpha3];

    if (error) {
        eN=error[SPIMFCSADIFF_n_particle];
        eD1=error[SPIMFCSADIFF_diff_acoeff1];
        esigmaz=error[SPIMFCSADIFF_focus_height]/1.0e3;
        ewxy=error[SPIMFCSADIFF_focus_width]/1.0e3;
        ea=error[SPIMFCSADIFF_pixel_width]/1.0e3;
        eoffset=error[SPIMFCSADIFF_offset];
        erho2=error[SPIMFCSADIFF_diff_rho2];
        erho3=error[SPIMFCSADIFF_diff_rho3];
        ecps=error[SPIMFCSADIFF_count_rate];
        ecpm=error[SPIMFCSADIFF_cpm];
        ebackground=error[SPIMFCSADIFF_background];
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

    data[SPIMFCSADIFF_diff_rho1]=rho1;
    data[SPIMFCSADIFF_diff_rho2]=rho2;
    data[SPIMFCSADIFF_diff_rho3]=rho3;
    if (error) {
        error[SPIMFCSADIFF_diff_rho1]=erho1;
        error[SPIMFCSADIFF_diff_rho2]=erho2;
        error[SPIMFCSADIFF_diff_rho3]=erho3;
    }


    // calculate 1/N
    data[SPIMFCSADIFF_1n_particle]=1.0/N;
    if (error) error[SPIMFCSADIFF_1n_particle]=fabs(eN/N/N);

    const double sqpi=sqrt(M_PI);
    // calculate Veff
    data[SPIMFCSADIFF_focus_volume]=SPIMFCS_newVeff(a, wxy, sigmaz);
    if (error) error[SPIMFCSADIFF_focus_volume]=SPIMFCS_newVeffError(a, ea, wxy, ewxy, sigmaz, esigmaz);

    if (data[SPIMFCSADIFF_focus_volume]!=0) {
        data[SPIMFCSADIFF_concentration]=N/data[SPIMFCSADIFF_focus_volume]/(NAVOGADRO * 1.0e-24);
    } else {
        data[SPIMFCSADIFF_concentration]=0;
    }
    if (data[SPIMFCSADIFF_focus_volume]!=0 && error) {
        error[SPIMFCSADIFF_concentration]=sqrt(qfSqr(eN/data[SPIMFCSADIFF_focus_volume]/(NAVOGADRO * 1.0e-24))+qfSqr(error[SPIMFCSADIFF_focus_volume]*N/qfSqr(data[SPIMFCSADIFF_focus_volume])/(NAVOGADRO * 1.0e-24)));//N/data[SPIMFCSADIFF_focus_volume]/(NAVOGADRO * 1.0e-24)
    }


    // calculate CPM = (CPS-background)/N
    data[SPIMFCSADIFF_cpm]=(cps-background)/N;
    error[SPIMFCSADIFF_cpm]=sqrt(sqr(ecps/N)+sqr(ebackground/N)+sqr(eN*(cps-background)/sqr(N)));

    double Aeff=data[SPIMFCSADIFF_focus_effA]=TIRFCS_newAeff(a, wxy);
    if (error) error[SPIMFCSADIFF_focus_effA]=TIRFCS_newAeffError(a, ea, wxy, ewxy);
    data[SPIMFCSADIFF_msd_at_tau1]=6.0*GAMMA1*pow(tau_msd*1e-3, a1);
    if (error) error[SPIMFCSADIFF_msd_at_tau1]=0;
    data[SPIMFCSADIFF_msd_at_tau2]=6.0*GAMMA2*pow(tau_msd*1e-3, a2);
    if (error) error[SPIMFCSADIFF_msd_at_tau2]=0;
    data[SPIMFCSADIFF_msd_at_tau3]=6.0*GAMMA3*pow(tau_msd*1e-3, a3);
    if (error) error[SPIMFCSADIFF_msd_at_tau3]=0;
    data[SPIMFCSADIFF_msd_time_aeff1]=pow(Aeff/(4.0*GAMMA1), 1.0/a1)*1e3;
    if (error) error[SPIMFCSADIFF_msd_time_aeff1]=0;
    data[SPIMFCSADIFF_msd_time_aeff2]=pow(Aeff/(4.0*GAMMA2), 1.0/a2)*1e3;
    if (error) error[SPIMFCSADIFF_msd_time_aeff2]=0;
    data[SPIMFCSADIFF_msd_time_aeff3]=pow(Aeff/(4.0*GAMMA3), 1.0/a3)*1e3;
    if (error) error[SPIMFCSADIFF_msd_time_aeff3]=0;

}

bool QFFitFunctionsSPIMFCSADiffE2::isParameterVisible(int parameter, const double* data) const {
    int comp=data[SPIMFCSADIFF_n_components];
    switch(parameter) {
        case SPIMFCSADIFF_diff_rho1:  return comp>1;
        case SPIMFCSADIFF_diff_acoeff1: case SPIMFCSADIFF_diff_alpha1: return comp>0;
        case SPIMFCSADIFF_diff_rho2: case SPIMFCSADIFF_diff_acoeff2: case SPIMFCSADIFF_diff_alpha2: case SPIMFCSADIFF_msd_at_tau2: case SPIMFCSADIFF_msd_time_aeff2: return comp>1;
        case SPIMFCSADIFF_diff_rho3: case SPIMFCSADIFF_diff_acoeff3: case SPIMFCSADIFF_diff_alpha3: case SPIMFCSADIFF_msd_at_tau3: case SPIMFCSADIFF_msd_time_aeff3: return comp>2;
    }
    return true;
}

unsigned int QFFitFunctionsSPIMFCSADiffE2::getAdditionalPlotCount(const double* params) {
    return 0;
}

QString QFFitFunctionsSPIMFCSADiffE2::transformParametersForAdditionalPlot(int plot, double* params) {
    return "";
}
