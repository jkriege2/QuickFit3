#include "qffitfunctionfcssimplediff.h"


#define sqr(x) ((x)*(x))
#define cube(x) ((x)*(x)*(x))
#define pow4(x) ((x)*(x)*(x)*(x))
#define pow5(x) ((x)*(x)*(x)*(x)*(x))

#define NAVOGADRO (6.02214179e23)





















QFFitFunctionFCSSimpleDiff::QFFitFunctionFCSSimpleDiff() {
    //           type,         id,                        name,                                                    label,                      unit,          unitlabel,               fit,       userEditable, userRangeEditable, displayError,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(FloatNumber,  "nonfl_tau1",              "triplet decay time",                                    "&tau;<sub>trip</sub>",     "usec",        "&mu;s",                 true,      true,         true,              QFFitFunction::DisplayError, false, 3.0,          0,        10,       0.1, 0.1  );
    #define FCSSDiff_nonfl_tau1 0
    addParameter(FloatNumber,  "nonfl_theta1",            "triplet fraction",                                      "&theta;<sub>trip</sub>",   "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 0.2,          0,        0.99999,  0.1, 0,      1);
    #define FCSSDiff_nonfl_theta1 1
    addParameter(FloatNumber,  "n_particle",              "Particle number N",                                     "N",                        "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 10,           1e-10,    1e5,      1,   0);
    #define FCSSDiff_n_particle 2
    addParameter(FloatNumber,  "1n_particle",             "1/Particle number N",                                   "1/N",                      "",            "",                      false,     false,        false,             QFFitFunction::DisplayError, false, 0.1,          1e-10,    1e5,      0.1, 0);
    #define FCSSDiff_1n_particle 3
    addParameter(FloatNumber,  "diff_tau1",               "diffusion time of first component",                     "&tau;<sub>D,1</sub>",      "usec",        "&mu;s",                 true,      true,         true,              QFFitFunction::DisplayError, false, 30,           1,        1e5,      1,   0.1        );
    #define FCSSDiff_diff_tau1 4
    addParameter(FloatNumber,  "offset",                  "correlation offset",                                    "G<sub>&infin;</sub>",      "",           "",                       true,      true,         true,              QFFitFunction::DisplayError, true, 0,            -10,      10,       0.1  );
    #define FCSSDiff_offset 5
    addParameter(FloatNumber,  "focus_struct_fac",        "focus: axial ratio",                                    "&gamma;",                  "",           "",                       true,      true,         true,              QFFitFunction::EditError,    true, 6,            0.01,     100,      0.5  );
    #define FCSSDiff_focus_struct_fac 6
    addParameter(FloatNumber,  "focus_width",             "focus: lateral radius",                                 "w<sub>x,y</sub>",          "nm",         "nm",                     false,     true,        false,              QFFitFunction::EditError,    true, 250,          0,        1e4,      1    );
    #define FCSSDiff_focus_width 7
    addParameter(FloatNumber,  "focus_volume",            "focus: effective colume",                               "V<sub>eff</sub>",          "fl",         "fl",                     false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define FCSSDiff_focus_volume 8
    addParameter(FloatNumber,  "concentration",           "particle concentration in focus",                       "C<sub>all</sub>",          "nM",         "nM",                     false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define FCSSDiff_concentration 9
    addParameter(FloatNumber,  "diff_coeff1",             "diffusion coefficient of species 1",                    "D<sub>1</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    false,    false,        false,              QFFitFunction::DisplayError, false, 500,          0,        1e50,     1    );
    #define FCSSDiff_diff_coeff1 10
}

double QFFitFunctionFCSSimpleDiff::evaluate(double t, const double* data) const {
    const double N=data[FCSSDiff_n_particle];
    const double nf_tau1=data[FCSSDiff_nonfl_tau1]/1.0e6;
    const double nf_theta1=data[FCSSDiff_nonfl_theta1];
    const double tauD1=data[FCSSDiff_diff_tau1]/1.0e6;
    double gamma=data[FCSSDiff_focus_struct_fac];
    if (gamma==0) gamma=1;
    const double gamma2=sqr(gamma);

    const double offset=data[FCSSDiff_offset];

    double reltau1=0;
    if (tauD1!=0) reltau1=t/tauD1;
    double d1=1.0/(1.0+reltau1)/sqrt(1.0+reltau1/gamma2);
    double pre=(1.0-nf_theta1+nf_theta1*exp(-t/nf_tau1))/(1.0-nf_theta1);
    return offset+pre/N*d1;
}

void QFFitFunctionFCSSimpleDiff::evaluateDerivatives(double* derivatives, double x, const double* data) const {
    for (register int i=0; i<paramCount(); i++) derivatives[i]=0;
    const double t=x*1.0e6;
    const double N=data[FCSSDiff_n_particle];
    double nf_tau=data[FCSSDiff_nonfl_tau1];
    if (nf_tau==0) nf_tau=1e-20;
    double nf_theta=data[FCSSDiff_nonfl_theta1];
    if (nf_theta==1) nf_theta=0.999999999;
    double tauD=data[FCSSDiff_diff_tau1];
    if (tauD==0) tauD=1e-20;
    double gamma=data[FCSSDiff_focus_struct_fac];
    if (gamma==0) gamma=1;
    const double gamma2=sqr(gamma);
    //const double offset=data[FCSSDiff_offset];

    //double reltau=t/tauD;

    const double exptT=exp(-t/nf_tau);
    const double fT=(1.0+nf_theta/(1.0-nf_theta)*exptT);
    const double fD=1.0+t/tauD;
    //const double fD12=sqrt(fD);
    const double fDG=1.0+t/tauD/gamma2;
    const double fDG12=sqrt(fDG);

    derivatives[FCSSDiff_n_particle]=-1.0/N/N*fT/fD/fDG12;
    derivatives[FCSSDiff_nonfl_theta1]=1.0/N/fD/fDG12*exptT/sqr(1.0-nf_theta);
    derivatives[FCSSDiff_nonfl_tau1]=-1.0/N/fD/fDG12*(nf_theta/(1.0-nf_theta)/sqr(nf_tau)*t*exptT);
    derivatives[FCSSDiff_diff_tau1]=fT/N*(t/sqr(tauD)/fDG12/sqr(fD) + t/2.0/fD/cube(fDG12)/gamma2/sqr(tauD));
    derivatives[FCSSDiff_focus_struct_fac]=1.0/N*fT/fD/cube(fDG12)*t/tauD/cube(gamma);
    derivatives[FCSSDiff_offset]=1.0;

    //printf("der(t=%lg   N=%lg nf_theta=%lg nf_tau=%lg tauD=%lg gamma=%lg offset=%lg   expT=%lg) = [ %lg %lg %lg %lg %lg %lg]\n", t, N, nf_theta, nf_tau, tauD, gamma, offset, exptT, derivatives[FCSSDiff_n_particle], derivatives[FCSSDiff_nonfl_theta1], derivatives[FCSSDiff_nonfl_tau1], derivatives[FCSSDiff_diff_tau1], derivatives[FCSSDiff_focus_struct_fac], derivatives[FCSSDiff_offset]);
}




void QFFitFunctionFCSSimpleDiff::calcParameter(double* data, double* error) const {
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
    //double gamma2=sqr(gamma);
    double wxy=data[FCSSDiff_focus_width]/1.0e3;
    double ewxy=0;
    //double offset=data[FCSSDiff_offset];
    double eoffset=0;

    if (error) {
        eN=error[FCSSDiff_n_particle];
        enf_tau1=error[FCSSDiff_nonfl_tau1]/1.0e6;
        etauD1=error[FCSSDiff_diff_tau1]/1.0e6;
        egamma=error[FCSSDiff_focus_struct_fac];
        ewxy=error[FCSSDiff_focus_width]/1.0e3;
        eoffset=error[FCSSDiff_offset];
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
}

bool QFFitFunctionFCSSimpleDiff::isParameterVisible(int parameter, const double* data) const {
    return true;
}

unsigned int QFFitFunctionFCSSimpleDiff::getAdditionalPlotCount(const double* params) {
    return 1;
}

QString QFFitFunctionFCSSimpleDiff::transformParametersForAdditionalPlot(int plot, double* params) {
    params[FCSSDiff_nonfl_theta1]=0;
    return "without non-fluorescent";
}
