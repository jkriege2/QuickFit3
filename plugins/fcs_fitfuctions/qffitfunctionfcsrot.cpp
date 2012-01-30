#include "qffitfunctionfcsrot.h"


#define sqr(x) ((x)*(x))
#define cube(x) ((x)*(x)*(x))
#define pow4(x) ((x)*(x)*(x)*(x))
#define pow5(x) ((x)*(x)*(x)*(x)*(x))

#define NAVOGADRO (6.02214179e23)






QFFitFunctionFCSRot::QFFitFunctionFCSRot() {
    //           type,         id,                        name,                                                    label,                      unit,          unitlabel,               fit,       userEditable, userRangeEditable, displayError,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(FloatNumber,  "nonfl_tau1",              "triplet decay time",                                    "&tau;<sub>trip</sub>",     "usec",        "&mu;s",                 true,      true,         true,              QFFitFunction::DisplayError, 3.0,          0,        10,       0.1, 0.1  );
    #define FCSSDiff_nonfl_tau1 0
    addParameter(FloatNumber,  "nonfl_theta1",            "triplet fraction",                                      "&theta;<sub>trip</sub>",   "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, 0.2,          0,        0.99999,  0.1, 0,      1);
    #define FCSSDiff_nonfl_theta1 1
    addParameter(FloatNumber,  "n_particle",              "Particle number N",                                     "N",                        "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, 10,           1e-10,    1e5,      1,   0);
    #define FCSSDiff_n_particle 2
    addParameter(FloatNumber,  "1n_particle",             "1/Particle number N",                                   "1/N",                      "",            "",                      false,     false,        false,             QFFitFunction::DisplayError, 0.1,          1e-10,    1e5,      0.1, 0);
    #define FCSSDiff_1n_particle 3
    addParameter(FloatNumber,  "diff_tau1",               "diffusion time of first component",                     "&tau;<sub>D,1</sub>",      "usec",        "&mu;s",                 true,      true,         true,              QFFitFunction::DisplayError, 30,           1,        1e5,      1,   0.1        );
    #define FCSSDiff_diff_tau1 4
    addParameter(FloatNumber,  "rot_tau",                 "rotation decay time",                                   "&tau;<sub>rot</sub>",      "ns",          "ns",                    true,      true,         true,              QFFitFunction::DisplayError, 300,           1e-10,    1e10,    1);
    #define FCSSDiff_rot_tau 5
    addParameter(FloatNumber,  "rot_f",                   "rotation fraction",                                     "F<sub>rot</sub>",          "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, 0.2,          0,        0.99999,  0.1, 0,      1);
    #define FCSSDiff_rot_f 6
    addParameter(FloatNumber,  "rot_c1",                  "rotation prefactor 1",                                  "c<sub>1</sub>",            "",            "",                      false,     true,        false,              QFFitFunction::NoError,      860.0/9.0,    0,        1e10,     0.1  );
    #define FCSSDiff_rot_c1 7
    addParameter(FloatNumber,  "rot_c2",                  "rotation prefactor 2",                                  "c<sub>2</sub>",            "",            "",                      false,     true,        false,              QFFitFunction::NoError,      4,            0,        1e10,     0.1  );
    #define FCSSDiff_rot_c2 8
    addParameter(FloatNumber,  "offset",                  "correlation offset",                                    "G<sub>&infin;</sub>",      "",           "",                       true,      true,         true,              QFFitFunction::DisplayError, 0,            -10,      10,       0.1  );
    #define FCSSDiff_offset 9
    addParameter(FloatNumber,  "focus_struct_fac",        "focus: axial ratio",                                    "&gamma;",                  "",           "",                       true,      true,         true,              QFFitFunction::EditError,    6,            0.01,     100,      0.5  );
    #define FCSSDiff_focus_struct_fac 10
    addParameter(FloatNumber,  "focus_width",             "focus: lateral radius",                                 "w<sub>x,y</sub>",          "nm",         "nm",                     false,     true,        false,              QFFitFunction::EditError,    250,          0,        1e4,      1    );
    #define FCSSDiff_focus_width 11
    addParameter(FloatNumber,  "focus_volume",            "focus: effective colume",                               "V<sub>eff</sub>",          "fl",         "fl",                     false,    false,        false,              QFFitFunction::DisplayError, 0.5,          0,        1e50,     1    );
    #define FCSSDiff_focus_volume 12
    addParameter(FloatNumber,  "concentration",           "particle concentration in focus",                       "C<sub>all</sub>",          "nM",         "nM",                     false,    false,        false,              QFFitFunction::DisplayError, 0.5,          0,        1e50,     1    );
    #define FCSSDiff_concentration 13
    addParameter(FloatNumber,  "diff_coeff1",             "diffusion coefficient of species 1",                    "D<sub>1</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    false,    false,        false,              QFFitFunction::DisplayError, 500,          0,        1e50,     1    );
    #define FCSSDiff_diff_coeff1 14
    addParameter(FloatNumber,  "diff_rot",                "rotation diffusion coefficient",                        "&Theta;<sub>rot</sub>",    "1/s",        "s<sup>-1</sup>",         false,    false,        false,              QFFitFunction::DisplayError, 500,          0,        1e50,     1    );
    #define FCSSDiff_rot_diff 15
    addParameter(FloatNumber,  "count_rate",              "count rate during measurement",                         "count rate",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError,    0,            0,        1e50,     1    );
    #define FCSSDiff_count_rate 16
    addParameter(FloatNumber,  "background",              "background count rate during measurement",              "background",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError  ,    0,            0,        1e50,     1    );
    #define FCSSDiff_background 17
    addParameter(FloatNumber,  "cpm",                     "photon counts per molecule",                            "cnt/molec",                "Hz",         "Hz",                     false,    false,        false,              QFFitFunction::DisplayError, 0,            0,        1e50,     1    );
    #define FCSSDiff_cpm 18
}

double QFFitFunctionFCSRot::evaluate(double t, const double* data) const {
    const double N=data[FCSSDiff_n_particle];
    const double nf_tau1=data[FCSSDiff_nonfl_tau1]/1.0e6;
    const double nf_theta1=data[FCSSDiff_nonfl_theta1];
    const double tauD1=data[FCSSDiff_diff_tau1]/1.0e6;
    const double taur=data[FCSSDiff_rot_tau]/1.0e9;
    const double Fr=data[FCSSDiff_rot_f];
    const double c1=data[FCSSDiff_rot_c1];
    const double c2=data[FCSSDiff_rot_c2];
    double gamma=data[FCSSDiff_focus_struct_fac];
    if (gamma==0) gamma=1;
    const double gamma2=sqr(gamma);
    const double background=data[FCSSDiff_background];
    const double cr=data[FCSSDiff_count_rate];
    double backfactor=1.0/sqr(1.0+background/cr);

    const double offset=data[FCSSDiff_offset];

    double reltau1=0;
    if (tauD1!=0) reltau1=t/tauD1;
    double d1=1.0/(1.0+reltau1)/sqrt(1.0+reltau1/gamma2);
    double rot=(1.0-Fr)+Fr*(c1*exp(-t/taur)+c2*exp(-20.0*t/6.0/taur));
    double pre=(1.0-nf_theta1+nf_theta1*exp(-t/nf_tau1))/(1.0-nf_theta1);
    return offset+pre/N*d1*rot*backfactor;
}





void QFFitFunctionFCSRot::calcParameter(double* data, double* error) const {
    double N=data[FCSSDiff_n_particle];
    double eN=0;
    //double nf_tau1=data[FCSSDiff_nonfl_tau1]/1.0e6;
    double enf_tau1=0;
    double nf_theta1=data[FCSSDiff_nonfl_theta1];
    //double enf_theta1=0;
    double tauD1=data[FCSSDiff_diff_tau1]/1.0e6;
    double etauD1=0;
    double gamma=data[FCSSDiff_focus_struct_fac];
    double egamma=0;
    double taur=data[FCSSDiff_rot_tau]/1.0e9;
    double etaur=0;
    //double gamma2=sqr(gamma);
    double wxy=data[FCSSDiff_focus_width]/1.0e3;
    double ewxy=0;
    //double offset=data[FCSSDiff_offset];
    double eoffset=0;
    double cps=data[FCSSDiff_count_rate];
    double ecps=0;
    double background=data[FCSSDiff_background];
    double ebackground=0;

    if (error) {
        eN=error[FCSSDiff_n_particle];
        enf_tau1=error[FCSSDiff_nonfl_tau1]/1.0e6;
        etauD1=error[FCSSDiff_diff_tau1]/1.0e6;
        egamma=error[FCSSDiff_focus_struct_fac];
        ewxy=error[FCSSDiff_focus_width]/1.0e3;
        eoffset=error[FCSSDiff_offset];
        etaur=error[FCSSDiff_rot_tau]/1.0e9;
        ecps=error[FCSSDiff_count_rate];
        ebackground=error[FCSSDiff_background];
    }

    data[FCSSDiff_nonfl_theta1]=nf_theta1;



    // calculate 1/N
    data[FCSSDiff_1n_particle]=1.0/N;
    if (error) error[FCSSDiff_1n_particle]=fabs(eN/N/N);

    // calculate Veff = pi^(3/2) * gamma * wxy^3
    const double pi32=sqrt(cube(M_PI));
    data[FCSSDiff_focus_volume]=pi32*gamma*cube(wxy);
    if (error) error[FCSSDiff_focus_volume]=sqrt(sqr(egamma*pi32*cube(wxy))+sqr(ewxy*3.0*pi32*gamma*sqr(wxy)));

    // calculate C = N / Veff
    const double pim32=1.0/sqrt(cube(M_PI));
    if (data[FCSSDiff_focus_volume]!=0) data[FCSSDiff_concentration]=N/data[FCSSDiff_focus_volume]/(NAVOGADRO * 1.0e-24); else data[FCSSDiff_concentration]=0;
    if (error) {
        if ((wxy!=0)&&(gamma!=0)) error[FCSSDiff_concentration]=sqrt( sqr(egamma*pim32*N/cube(wxy)/sqr(gamma)) + sqr(ewxy*3.0*pim32*N/gamma/pow4(wxy)) + sqr(eN*pim32/gamma/cube(wxy)) )/(NAVOGADRO * 1.0e-24);
        else error[FCSSDiff_concentration]=0;
    }

    // calculate D1 = wxy^2 / (4*tauD1)
    if (tauD1!=0) data[FCSSDiff_diff_coeff1]=sqr(wxy)/4.0/tauD1; else data[FCSSDiff_diff_coeff1]=0;
    if (error) {
        if (tauD1!=0) error[FCSSDiff_diff_coeff1]=sqrt( sqr(etauD1*sqr(wxy)/sqr(tauD1)/4.0) + sqr(ewxy*2.0*wxy/tauD1/4.0) );
        else error[FCSSDiff_diff_coeff1]=0;
    }

    // calculate ThetaR=1/(6*taur)
    if (taur!=0) data[FCSSDiff_rot_diff]=1.0/6.0/taur; else data[FCSSDiff_rot_diff]=0;
    if (error) {
        if (taur!=0) error[FCSSDiff_rot_diff]=fabs(etaur/6.0/sqr(taur));
        else error[FCSSDiff_rot_diff]=0;
    }

    // calculate CPM = (CPS-background)/N
    data[FCSSDiff_cpm]=(cps-background)/N;
    error[FCSSDiff_cpm]=sqrt(sqr(ecps/N)+sqr(ebackground/N)+sqr(eN*(cps-background)/sqr(N)));

}

bool QFFitFunctionFCSRot::isParameterVisible(int parameter, const double* data) const {
    return true;
}

unsigned int QFFitFunctionFCSRot::getAdditionalPlotCount(const double* params) {
    return 2;
}

QString QFFitFunctionFCSRot::transformParametersForAdditionalPlot(int plot, double* params) {
    if (plot==0) {
        params[FCSSDiff_nonfl_theta1]=0;
        return "without non-fluorescent";
    } else if (plot==1) {
        params[FCSSDiff_nonfl_theta1]=0;
        params[FCSSDiff_rot_f]=0;
        return "without non-fl. + rot";
    }
    return "";
}
