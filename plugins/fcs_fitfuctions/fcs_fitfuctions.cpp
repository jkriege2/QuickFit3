#include "fcs_fitfuctions.h"
#include <cmath>

#define sqr(x) ((x)*(x))
#define cube(x) ((x)*(x)*(x))
#define pow4(x) ((x)*(x)*(x)*(x))
#define pow5(x) ((x)*(x)*(x)*(x)*(x))

#define NAVOGADRO (6.02214179e23)

QFFitFunctionFCSDiff::QFFitFunctionFCSDiff() {
    //           type,         id,                        name,                                                    label,                      unit,          unitlabel,               fit,       userEditable, userRangeEditable, displayError,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(IntCombo,     "n_nonfluorescent",        "number of nonfluorescent components (triplet ...)",     "# non-fluorescent",        "",            "",                      false,     true,         false,             QFFitFunction::NoError,      1,            0,        2,        1,   0,      2);
    #define FCSDiff_n_nonfluorescent 0
    addParameter(IntCombo,     "n_components",            "number of diffusing components",                        "components",               "",            "",                      false,     true,         false,             QFFitFunction::NoError,      1,            1,        3,        1,   1,      2);
    #define FCSDiff_n_components 1
    addParameter(FloatNumber,  "nonfl_tau1",              "triplet decay time",                                    "&tau;<sub>trip</sub>",     "usec",        "&mu;s",                 true,      true,         true,              QFFitFunction::DisplayError, 3.0,          0,        10,       0.1, 0  );
    #define FCSDiff_nonfl_tau1 2
    addParameter(FloatNumber,  "nonfl_theta1",            "triplet fraction",                                      "&theta;<sub>trip</sub>",   "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, 0.2,          0,        0.99999,  0.1, 0,      1);
    #define FCSDiff_nonfl_theta1 3
    addParameter(FloatNumber,  "nonfl_tau2",              "dark component 2 decay time",                           "&tau;<sub>dark,2</sub>",   "usec",        "&mu;s",                 true,      true,         true,              QFFitFunction::DisplayError, 300,          1e-10,    1e5,      1,   0);
    #define FCSDiff_nonfl_tau2 4
    addParameter(FloatNumber,  "nonfl_theta2",            "dark component 2 fraction",                             "&theta;<sub>dark,2</sub>", "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, 0.2,          0,        0.99999,  0.1, 0,      1);
    #define FCSDiff_nonfl_theta2 5
    addParameter(FloatNumber,  "n_particle",              "Particle number N",                                     "N",                        "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, 10,           1e-10,    1e5,      1,   0);
    #define FCSDiff_n_particle 6
    addParameter(FloatNumber,  "1n_particle",             "1/Particle number N",                                   "1/N",                      "",            "",                      false,     false,        false,             QFFitFunction::DisplayError, 0.1,          1e-10,    1e5,      0.1, 0);
    #define FCSDiff_1n_particle 7
    addParameter(FloatNumber,  "diff_rho1",               "fraction of first component",                           "&rho;<sub>1</sub>",        "",            "",                      false,     false,        false,             QFFitFunction::DisplayError, 0.5,          0,        0.99999,  0.1, 0,      1);
    #define FCSDiff_diff_rho1 8
    addParameter(FloatNumber,  "diff_tau1",               "diffusion time of first component",                     "&tau;<sub>D,1</sub>",      "usec",        "&mu;s",                 true,      true,         true,              QFFitFunction::DisplayError, 30,           1,        1e5,      1,   0        );
    #define FCSDiff_diff_tau1 9
    addParameter(FloatNumber,  "diff_rho2",               "fraction of second component",                          "&rho;<sub>2</sub>",        "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, 0.5,          0,        0.99999,  0.1, 0,      1  );
    #define FCSDiff_diff_rho2 10
    addParameter(FloatNumber,  "diff_tau2",               "diffusion time of second component",                    "&tau;<sub>D,2</sub>",      "usec",        "&mu;s",                 true,      true,         true,              QFFitFunction::DisplayError, 300,          1,        1e8,      1,   0    );
    #define FCSDiff_diff_tau2 11
    addParameter(FloatNumber,  "diff_rho3",               "fraction of third component",                           "&rho;<sub>3</sub>",        "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, 0.5,          0,        0.99999,  0.1, 0,      1  );
    #define FCSDiff_diff_rho3 12
    addParameter(FloatNumber,  "diff_tau3",               "diffusion time of third component",                     "&tau;<sub>D,3</sub>",      "usec",        "&mu;s",                 true,      true,         true,              QFFitFunction::DisplayError, 300,          1,        1e8,      1    );
    #define FCSDiff_diff_tau3 13
    addParameter(FloatNumber,  "offset",                  "correlation offset",                                    "G<sub>&infin;</sub>",      "",           "",                       true,      true,         true,              QFFitFunction::DisplayError, 0,            -10,      10,       0.1  );
    #define FCSDiff_offset 14
    addParameter(FloatNumber,  "focus_struct_fac",        "focus: axial ratio",                                    "&gamma;",                  "",           "",                       true,      true,         true,              QFFitFunction::EditError,    6,            0.01,     100,      0.5  );
    #define FCSDiff_focus_struct_fac 15
    addParameter(FloatNumber,  "focus_width",             "focus: lateral radius",                                 "w<sub>x,y</sub>",          "nm",         "nm",                     false,     true,        false,              QFFitFunction::EditError,    250,          0,        1e4,      1    );
    #define FCSDiff_focus_width 16
    addParameter(FloatNumber,  "focus_volume",            "focus: effective colume",                               "V<sub>eff</sub>",          "fl",         "fl",                     false,    false,        false,              QFFitFunction::DisplayError, 0.5,          0,        1e50,     1    );
    #define FCSDiff_focus_volume 17
    addParameter(FloatNumber,  "concentration",           "particle concentration in focus",                       "C<sub>all</sub>",          "nM",         "nM",                     false,    false,        false,              QFFitFunction::DisplayError, 0.5,          0,        1e50,     1    );
    #define FCSDiff_concentration 18
    addParameter(FloatNumber,  "diff_coeff1",             "diffusion coefficient of species 1",                    "D<sub>1</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    false,    false,        false,              QFFitFunction::DisplayError, 500,          0,        1e50,     1    );
    #define FCSDiff_diff_coeff1 19
    addParameter(FloatNumber,  "diff_coeff2",             "diffusion coefficient of species 2",                    "D<sub>2</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    false,    false,        false,              QFFitFunction::DisplayError, 500,          0,        1e50,     1    );
    #define FCSDiff_diff_coeff2 20
    addParameter(FloatNumber,  "diff_coeff3",             "diffusion coefficient of species 3",                    "D<sub>3</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    false,    false,        false,              QFFitFunction::DisplayError, 500,          0,        1e50,     1    );
    #define FCSDiff_diff_coeff3 21
}

double QFFitFunctionFCSDiff::evaluate(double t, const double* data) const {
    const int comp=data[FCSDiff_n_components];
    const int nonfl_comp=data[FCSDiff_n_nonfluorescent];
    const double N=data[FCSDiff_n_particle];
    const double nf_tau1=data[FCSDiff_nonfl_tau1]/1.0e6;
    const double nf_theta1=data[FCSDiff_nonfl_theta1];
    const double nf_tau2=data[FCSDiff_nonfl_tau2]/1.0e6;
    const double nf_theta2=data[FCSDiff_nonfl_theta2];
    const double tauD1=data[FCSDiff_diff_tau1]/1.0e6;
    const double rho2=data[FCSDiff_diff_rho2];
    const double tauD2=data[FCSDiff_diff_tau2]/1.0e6;
    const double rho3=data[FCSDiff_diff_rho3];
    const double tauD3=data[FCSDiff_diff_tau3]/1.0e6;
    double gamma=data[FCSDiff_focus_struct_fac];
    if (gamma==0) gamma=1;
    const double gamma2=sqr(gamma);

    const double offset=data[FCSDiff_offset];

    double reltau1=0;
    if (tauD1!=0) reltau1=t/tauD1;
    double rho1=1.0;
    double d1=1.0/(1.0+reltau1)/sqrt(1.0+reltau1/gamma2);
    double d2=0;
    double d3=0;
    if (comp>1) {
        double reltau2=0;
        if (tauD2!=0) reltau2=t/tauD2;
        d2=rho2/(1.0+reltau2)/sqrt(1.0+reltau2/gamma2);
        rho1=1.0-rho2;
    }
    if (comp>2) {
        double reltau3=0;
        if (tauD3!=0) reltau3=t/tauD3;
        d3=rho3/(1.0+reltau3)/sqrt(1.0+reltau3/gamma2);
        rho1=1.0-rho2-rho3;
    }
    double pre=1.0;
    if (nonfl_comp==1) {
        pre=(1.0-nf_theta1+nf_theta1*exp(-t/nf_tau1))/(1.0-nf_theta1);
    } else if (nonfl_comp==2) {
        pre=(1.0-nf_theta1+nf_theta1*exp(-t/nf_tau1)-nf_theta2+nf_theta2*exp(-t/nf_tau2))/(1.0-nf_theta1-nf_theta2);
    }
    return offset+pre/N*(rho1*d1+d2+d3);
}




void QFFitFunctionFCSDiff::calcParameter(double* data, double* error) const {
    int comp=data[FCSDiff_n_components];
    int nonfl_comp=data[FCSDiff_n_nonfluorescent];
    double N=data[FCSDiff_n_particle];
    double eN=0;
    double nf_tau1=data[FCSDiff_nonfl_tau1];
    double enf_tau1=0;
    double nf_theta1=data[FCSDiff_nonfl_theta1];
    double enf_theta1=0;
    double nf_tau2=data[FCSDiff_nonfl_tau2];
    double enf_tau2=0;
    double nf_theta2=data[FCSDiff_nonfl_theta2];
    double enf_theta2=0;
    double rho1=1.0;
    double erho1=0;
    double tauD1=data[FCSDiff_diff_tau1];
    double etauD1=0;
    double rho2=data[FCSDiff_diff_rho2];
    double erho2=0;
    double tauD2=data[FCSDiff_diff_tau2];
    double etauD2=0;
    double rho3=data[FCSDiff_diff_rho3];
    double erho3=0;
    double tauD3=data[FCSDiff_diff_tau3];
    double etauD3=0;
    double gamma=data[FCSDiff_focus_struct_fac];
    double egamma=0;
    double gamma2=sqr(gamma);
    double wxy=data[FCSDiff_focus_width]/1.0e3;
    double ewxy=0;
    double offset=data[FCSDiff_offset];
    double eoffset=0;

    if (error) {
        eN=error[FCSDiff_n_particle];
        enf_tau1=error[FCSDiff_nonfl_tau1];
        enf_theta1=error[FCSDiff_nonfl_theta1];
        enf_tau2=error[FCSDiff_nonfl_tau2];
        enf_theta2=error[FCSDiff_nonfl_theta2];
        erho1=0;
        etauD1=error[FCSDiff_diff_tau1];
        erho2=error[FCSDiff_diff_rho2];
        etauD2=error[FCSDiff_diff_tau2];
        erho3=error[FCSDiff_diff_rho3];
        etauD3=error[FCSDiff_diff_tau3];
        egamma=error[FCSDiff_focus_struct_fac];
        ewxy=error[FCSDiff_focus_width]/1.0e3;
        eoffset=error[FCSDiff_offset];
    }

    // correct for invalid fractions
    if (nf_theta1>1.0) nf_theta1=1.0;
    if (nf_theta2>1.0) nf_theta2=1.0;
    if (nf_theta1+nf_theta2>1.0) {
        nf_theta2=1.0-nf_theta1;
    }
    data[FCSDiff_nonfl_theta1]=nf_theta1;
    data[FCSDiff_nonfl_theta2]=nf_theta2;

    // calculate diffusion fractions
    if (comp==1) {
        rho1=1.0;
        erho1=0;
        rho2=0;
        erho2=0;
        rho3=0;
        erho3=0;
        data[FCSDiff_diff_tau1]=tauD1;
        data[FCSDiff_diff_rho1]=rho1;
        data[FCSDiff_diff_tau2]=tauD2;
        data[FCSDiff_diff_rho2]=rho2;
        data[FCSDiff_diff_tau3]=tauD3;
        data[FCSDiff_diff_rho3]=rho3;
        if (error) {
            error[FCSDiff_diff_tau1]=etauD1;
            error[FCSDiff_diff_rho1]=erho1;
            error[FCSDiff_diff_tau2]=etauD2;
            error[FCSDiff_diff_rho2]=erho2;
            error[FCSDiff_diff_tau3]=etauD3;
            error[FCSDiff_diff_rho3]=erho3;
        }
    } else if (comp==2) {
        if (rho2>1.0) rho2=1.0;
        if (rho2<0.0) rho2=0.0;
        rho1=1.0-rho2;
        erho1=erho2;
        rho3=0;
        erho3=0;
    } else if (comp==3) {
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

    data[FCSDiff_diff_rho1]=rho1;
    data[FCSDiff_diff_rho2]=rho2;
    data[FCSDiff_diff_rho3]=rho3;
    if (error) {
        error[FCSDiff_diff_rho1]=erho1;
        error[FCSDiff_diff_rho2]=erho2;
        error[FCSDiff_diff_rho3]=erho3;
    }

    // reread to variables after sort
    //rho1=data[FCSDiff_diff_rho1];
    tauD1=data[FCSDiff_diff_tau1]/1.0e6;
    //rho2=data[FCSDiff_diff_rho2];
    tauD2=data[FCSDiff_diff_tau2]/1.0e6;
    //rho3=data[FCSDiff_diff_rho3];
    tauD3=data[FCSDiff_diff_tau3]/1.0e6;
    if (error) {
        //erho1=error[FCSDiff_diff_rho1];
        etauD1=error[FCSDiff_diff_tau1]/1.0e6;
        //erho2=error[FCSDiff_diff_rho2];
        etauD2=error[FCSDiff_diff_tau2]/1.0e6;
        //erho3=error[FCSDiff_diff_rho3];
        etauD3=error[FCSDiff_diff_tau3]/1.0e6;
    }

    // calculate 1/N
    data[FCSDiff_1n_particle]=1.0/N;
    if (error) error[FCSDiff_1n_particle]=fabs(eN/N/N);

    // calculate Veff = pi^(3/2) * gamma * wxy^3
    const double pi32=sqrt(cube(M_PI));
    data[FCSDiff_focus_volume]=pi32*gamma*cube(wxy);
    if (error) error[FCSDiff_focus_volume]=sqrt(sqr(egamma*pi32*cube(wxy))+sqr(ewxy*3.0*pi32*gamma*sqr(wxy)));

    // calculate C = N / Veff
    const double pim32=1.0/sqrt(cube(M_PI));
    if (data[FCSDiff_focus_volume]!=0) data[FCSDiff_concentration]=N/data[FCSDiff_focus_volume]/(NAVOGADRO * 1.0e-24); else data[FCSDiff_concentration]=0;
    if (error) {
        if ((wxy!=0)&&(gamma!=0)) error[FCSDiff_concentration]=sqrt( sqr(egamma*pim32*N/cube(wxy)/sqr(gamma)) + sqr(ewxy*3.0*pim32*N/gamma/pow4(wxy)) + sqr(eN*pim32/gamma/cube(wxy)) )/(NAVOGADRO * 1.0e-24);
        else error[FCSDiff_concentration]=0;
    }

    // calculate D1 = wxy^2 / (4*tauD1)
    if (tauD1!=0) data[FCSDiff_diff_coeff1]=sqr(wxy)/4.0/tauD1; else data[FCSDiff_diff_coeff1]=0;
    if (error) {
        if (tauD1!=0) error[FCSDiff_diff_coeff1]=sqrt( sqr(etauD1*sqr(wxy)/sqr(tauD1)/4.0) + sqr(ewxy*2.0*wxy/tauD1/4.0) );
        else error[FCSDiff_diff_coeff1]=0;
    }

    // calculate D2 = wxy^2 / (4*tauD2)
    if (tauD2!=0) data[FCSDiff_diff_coeff2]=sqr(wxy)/4.0/tauD2; else data[FCSDiff_diff_coeff2]=0;
    if (error) {
        if (tauD2!=0) error[FCSDiff_diff_coeff2]=sqrt( sqr(etauD2*sqr(wxy)/sqr(tauD2)/4.0) + sqr(ewxy*2.0*wxy/tauD2/4.0) );
        else error[FCSDiff_diff_coeff2]=0;
    }

    // calculate D3 = wxy^2 / (4*tauD3)
    if (tauD3!=0) data[FCSDiff_diff_coeff3]=sqr(wxy)/4.0/tauD3; else data[FCSDiff_diff_coeff3]=0;
    if (error) {
        if (tauD3!=0) error[FCSDiff_diff_coeff3]=sqrt( sqr(etauD3*sqr(wxy)/sqr(tauD3)/4.0) + sqr(ewxy*2.0*wxy/tauD3/4.0) );
        else error[FCSDiff_diff_coeff3]=0;
    }
}

bool QFFitFunctionFCSDiff::isParameterVisible(int parameter, double* data) const {
    int comp=data[FCSDiff_n_components];
    int nonfl_comp=data[FCSDiff_n_nonfluorescent];
    switch(parameter) {
        case FCSDiff_nonfl_tau1: case FCSDiff_nonfl_theta1: return nonfl_comp>0;
        case FCSDiff_nonfl_tau2: case FCSDiff_nonfl_theta2: return nonfl_comp>1;
        case FCSDiff_diff_rho1:  return comp>1;
        case FCSDiff_diff_tau1: case FCSDiff_diff_coeff1: return comp>0;
        case FCSDiff_diff_rho2: case FCSDiff_diff_tau2: case FCSDiff_diff_coeff2: return comp>1;
        case FCSDiff_diff_rho3: case FCSDiff_diff_tau3: case FCSDiff_diff_coeff3: return comp>2;
    }
    return true;
}

unsigned int QFFitFunctionFCSDiff::getAdditionalPlotCount(const double* params) {
    return 1;
}

QString QFFitFunctionFCSDiff::transformParametersForAdditionalPlot(int plot, double* params) {
    params[FCSDiff_n_nonfluorescent]=0;
    return "without non-fluorescent";
}






QFFitFunctionFCSADiff::QFFitFunctionFCSADiff() {
    //           type,         id,                        name,                                                    label,                      unit,          unitlabel,               fit,       userEditable, userRangeEditable, displayError,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(IntCombo,     "n_nonfluorescent",        "number of nonfluorescent components (triplet ...)",     "# non-fluorescent",        "",            "",                      false,     true,         false,             QFFitFunction::NoError,      1,            0,        2,        1,   0,      2);
    #define FCSADiff_n_nonfluorescent 0
    addParameter(IntCombo,     "n_components",            "number of diffusing components",                        "components",               "",            "",                      false,     true,         false,             QFFitFunction::NoError,      1,            1,        3,        1,   1,      2);
    #define FCSADiff_n_components 1
    addParameter(FloatNumber,  "nonfl_tau1",              "triplet decay time",                                    "&tau;<sub>trip</sub>",     "usec",        "&mu;s",                 true,      true,         true,              QFFitFunction::DisplayError, 3.0,          0,        10,       0.1, 0  );
    #define FCSADiff_nonfl_tau1 2
    addParameter(FloatNumber,  "nonfl_theta1",            "triplet fraction",                                      "&theta;<sub>trip</sub>",   "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, 0.2,          0,        0.99999,  0.1, 0,      1);
    #define FCSADiff_nonfl_theta1 3
    addParameter(FloatNumber,  "nonfl_tau2",              "dark component 2 decay time",                           "&tau;<sub>dark,2</sub>",   "usec",        "&mu;s",                 true,      true,         true,              QFFitFunction::DisplayError, 300,          1e-10,    1e5,      1,   0);
    #define FCSADiff_nonfl_tau2 4
    addParameter(FloatNumber,  "nonfl_theta2",            "dark component 2 fraction",                             "&theta;<sub>dark,2</sub>", "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, 0.2,          0,        0.99999,  0.1, 0,      1);
    #define FCSADiff_nonfl_theta2 5
    addParameter(FloatNumber,  "n_particle",              "Particle number N",                                     "N",                        "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, 10,           1e-10,    1e5,      1,   0);
    #define FCSADiff_n_particle 6
    addParameter(FloatNumber,  "1n_particle",             "1/Particle number N",                                   "1/N",                      "",            "",                      false,     false,        false,             QFFitFunction::DisplayError, 0.1,          1e-10,    1e5,      0.1, 0);
    #define FCSADiff_1n_particle 7
    addParameter(FloatNumber,  "diff_rho1",               "fraction of first component",                           "&rho;<sub>1</sub>",        "",            "",                      false,     false,        false,             QFFitFunction::DisplayError, 0.5,          0,        0.99999,  0.1, 0,      1);
    #define FCSADiff_diff_rho1 8
    addParameter(FloatNumber,  "diff_tau1",               "diffusion time of first component",                     "&tau;<sub>D,1</sub>",      "usec",        "&mu;s",                 true,      true,         true,              QFFitFunction::DisplayError, 30,           1,        1e5,      1,   0        );
    #define FCSADiff_diff_tau1 9
    addParameter(FloatNumber,  "diff_alpha1",             "anomality parameter of first component",                "&alpha;<sub>1</sub>",      "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, 1,            1e-5,     1e5,      0.1, 0        );
    #define FCSADiff_diff_alpha1 10
    addParameter(FloatNumber,  "diff_rho2",               "fraction of second component",                          "&rho;<sub>2</sub>",        "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, 0.5,          0,        0.99999,  0.1, 0,      1  );
    #define FCSADiff_diff_rho2 11
    addParameter(FloatNumber,  "diff_tau2",               "diffusion time of second component",                    "&tau;<sub>D,2</sub>",      "usec",        "&mu;s",                 true,      true,         true,              QFFitFunction::DisplayError, 300,          1,        1e8,      1,   0    );
    #define FCSADiff_diff_tau2 12
    addParameter(FloatNumber,  "diff_alpha2",             "anomality parameter of second component",               "&alpha;<sub>2</sub>",      "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, 1,            1e-5,     1e5,      0.1, 0        );
    #define FCSADiff_diff_alpha2 13
    addParameter(FloatNumber,  "diff_rho3",               "fraction of third component",                           "&rho;<sub>3</sub>",        "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, 0.5,          0,        0.99999,  0.1, 0,      1  );
    #define FCSADiff_diff_rho3 14
    addParameter(FloatNumber,  "diff_tau3",               "diffusion time of third component",                     "&tau;<sub>D,3</sub>",      "usec",        "&mu;s",                 true,      true,         true,              QFFitFunction::DisplayError, 300,          1,        1e8,      1    );
    #define FCSADiff_diff_tau3 15
    addParameter(FloatNumber,  "diff_alpha3",             "anomality parameter of third component",                "&alpha;<sub>3</sub>",      "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, 1,            1e-5,     1e5,      0.1, 0        );
    #define FCSADiff_diff_alpha3 16
    addParameter(FloatNumber,  "offset",                  "correlation offset",                                    "G<sub>&infin;</sub>",      "",           "",                       true,      true,         true,              QFFitFunction::DisplayError, 0,            -10,      10,       0.1  );
    #define FCSADiff_offset 17
    addParameter(FloatNumber,  "focus_struct_fac",        "focus: axial ratio",                                    "&gamma;",                  "",           "",                       true,      true,         true,              QFFitFunction::DisplayError, 6,            0.01,     100,      0.5  );
    #define FCSADiff_focus_struct_fac 18
    addParameter(FloatNumber,  "focus_width",             "focus: lateral radius",                                 "w<sub>x,y</sub>",          "nm",         "nm",                     false,     true,        false,              QFFitFunction::DisplayError, 250,          0,        1e4,      1    );
    #define FCSADiff_focus_width 19
    addParameter(FloatNumber,  "focus_volume",            "focus: effective colume",                               "V<sub>eff</sub>",          "fl",         "fl",                     false,    false,        false,              QFFitFunction::DisplayError, 0.5,          0,        1e50,     1    );
    #define FCSADiff_focus_volume 20
    addParameter(FloatNumber,  "concentration",           "particle concentration in focus",                       "C<sub>all</sub>",          "nM",         "nM",                     false,    false,        false,              QFFitFunction::DisplayError, 0.5,          0,        1e50,     1    );
    #define FCSADiff_concentration 21
}

double QFFitFunctionFCSADiff::evaluate(double t, const double* data) const {
    const int comp=data[FCSADiff_n_components];
    const int nonfl_comp=data[FCSADiff_n_nonfluorescent];
    const double N=data[FCSADiff_n_particle];
    const double nf_tau1=data[FCSADiff_nonfl_tau1]/1.0e6;
    const double nf_theta1=data[FCSADiff_nonfl_theta1];
    const double nf_tau2=data[FCSADiff_nonfl_tau2]/1.0e6;
    const double nf_theta2=data[FCSADiff_nonfl_theta2];
    const double tauD1=data[FCSADiff_diff_tau1]/1.0e6;
    const double rho2=data[FCSADiff_diff_rho2];
    const double tauD2=data[FCSADiff_diff_tau2]/1.0e6;
    const double rho3=data[FCSADiff_diff_rho3];
    const double tauD3=data[FCSADiff_diff_tau3]/1.0e6;
    const double alpha1=data[FCSADiff_diff_alpha1];
    const double alpha2=data[FCSADiff_diff_alpha2];
    const double alpha3=data[FCSADiff_diff_alpha3];
    double gamma=data[FCSADiff_focus_struct_fac];
    if (gamma==0) gamma=1;
    const double gamma2=sqr(gamma);

    const double offset=data[FCSADiff_offset];

    double reltau1=0;
    if (tauD1!=0) reltau1=pow(t/tauD1, alpha1);
    double rho1=1.0;
    double d1=1.0/(1.0+reltau1)/sqrt(1.0+reltau1/gamma2);
    double d2=0;
    double d3=0;
    if (comp>1) {
        double reltau2=0;
        if (tauD2!=0) reltau2=pow(t/tauD2, alpha2);
        d2=rho2/(1.0+reltau2)/sqrt(1.0+reltau2/gamma2);
        rho1=1.0-rho2;
    }
    if (comp>2) {
        double reltau3=0;
        if (tauD3!=0) reltau3=pow(t/tauD3, alpha3);
        d3=rho3/(1.0+reltau3)/sqrt(1.0+reltau3/gamma2);
        rho1=1.0-rho2-rho3;
    }
    double pre=1.0;
    if (nonfl_comp==1) {
        pre=(1.0-nf_theta1+nf_theta1*exp(-t/nf_tau1))/(1.0-nf_theta1);
    } else if (nonfl_comp==2) {
        pre=(1.0-nf_theta1+nf_theta1*exp(-t/nf_tau1)-nf_theta2+nf_theta2*exp(-t/nf_tau2))/(1.0-nf_theta1-nf_theta2);
    }
    return offset+pre/N*(rho1*d1+d2+d3);
}


void QFFitFunctionFCSADiff::calcParameter(double* data, double* error) const {
    int comp=data[FCSADiff_n_components];
    int nonfl_comp=data[FCSADiff_n_nonfluorescent];
    double N=data[FCSADiff_n_particle];
    double eN=0;
    double nf_tau1=data[FCSADiff_nonfl_tau1];
    double enf_tau1=0;
    double nf_theta1=data[FCSADiff_nonfl_theta1];
    double enf_theta1=0;
    double nf_tau2=data[FCSADiff_nonfl_tau2];
    double enf_tau2=0;
    double nf_theta2=data[FCSADiff_nonfl_theta2];
    double enf_theta2=0;
    double rho1=1.0;
    double erho1=0;
    double tauD1=data[FCSADiff_diff_tau1];
    double etauD1=0;
    double rho2=data[FCSADiff_diff_rho2];
    double erho2=0;
    double tauD2=data[FCSADiff_diff_tau2];
    double etauD2=0;
    double rho3=data[FCSADiff_diff_rho3];
    double erho3=0;
    double tauD3=data[FCSADiff_diff_tau3];
    double etauD3=0;
    double gamma=data[FCSADiff_focus_struct_fac];
    double egamma=0;
    double gamma2=sqr(gamma);
    double wxy=data[FCSADiff_focus_width]/1.0e3;
    double ewxy=0;
    double offset=data[FCSADiff_offset];
    double eoffset=0;
    double alpha1=data[FCSADiff_diff_alpha1];
    double ealpha1=0;
    double alpha2=data[FCSADiff_diff_alpha2];
    double ealpha2=0;
    double alpha3=data[FCSADiff_diff_alpha3];
    double ealpha3=0;

    if (error) {
        eN=error[FCSADiff_n_particle];
        enf_tau1=error[FCSADiff_nonfl_tau1];
        enf_theta1=error[FCSADiff_nonfl_theta1];
        enf_tau2=error[FCSADiff_nonfl_tau2];
        enf_theta2=error[FCSADiff_nonfl_theta2];
        erho1=0;
        etauD1=error[FCSADiff_diff_tau1];
        erho2=error[FCSADiff_diff_rho2];
        etauD2=error[FCSADiff_diff_tau2];
        erho3=error[FCSADiff_diff_rho3];
        etauD3=error[FCSADiff_diff_tau3];
        egamma=error[FCSADiff_focus_struct_fac];
        ewxy=error[FCSADiff_focus_width]/1.0e3;
        eoffset=error[FCSADiff_offset];
        ealpha1=error[FCSADiff_diff_alpha1];
        ealpha2=error[FCSADiff_diff_alpha2];
        ealpha3=error[FCSADiff_diff_alpha3];
    }

    // correct for invalid fractions
    if (nf_theta1>1.0) nf_theta1=1.0;
    if (nf_theta2>1.0) nf_theta2=1.0;
    if (nf_theta1+nf_theta2>1.0) {
        nf_theta2=1.0-nf_theta1;
    }
    data[FCSADiff_nonfl_theta1]=nf_theta1;
    data[FCSADiff_nonfl_theta2]=nf_theta2;

    // calculate diffusion fractions
    if (comp==1) {
        rho1=1.0;
        erho1=0;
        rho2=0;
        erho2=0;
        rho3=0;
        erho3=0;
    } else if (comp==2) {
        if (rho2>1.0) rho2=1.0;
        if (rho2<0.0) rho2=0.0;
        rho1=1.0-rho2;
        erho1=erho2;
        rho3=0;
        erho3=0;
    } else if (comp==3) {
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

    data[FCSADiff_diff_rho1]=rho1;
    data[FCSADiff_diff_rho2]=rho2;
    data[FCSADiff_diff_rho3]=rho3;
    if (error) {
        error[FCSADiff_diff_rho1]=erho1;
        error[FCSADiff_diff_rho2]=erho2;
        error[FCSADiff_diff_rho3]=erho3;
    }


    // reread to variables after sort
    //rho1=data[FCSADiff_diff_rho1];
    tauD1=data[FCSADiff_diff_tau1]/1.0e6;
    //rho2=data[FCSADiff_diff_rho2];
    tauD2=data[FCSADiff_diff_tau2]/1.0e6;
    //rho3=data[FCSADiff_diff_rho3];
    tauD3=data[FCSADiff_diff_tau3]/1.0e6;
    alpha1=data[FCSADiff_diff_alpha1];
    alpha2=data[FCSADiff_diff_alpha2];
    alpha3=data[FCSADiff_diff_alpha3];
    if (error) {
        //erho1=error[FCSADiff_diff_rho1];
        etauD1=error[FCSADiff_diff_tau1]/1.0e6;
        //erho2=error[FCSADiff_diff_rho2];
        etauD2=error[FCSADiff_diff_tau2]/1.0e6;
        //erho3=error[FCSADiff_diff_rho3];
        etauD3=error[FCSADiff_diff_tau3]/1.0e6;
        ealpha1=error[FCSADiff_diff_alpha1];
        ealpha2=error[FCSADiff_diff_alpha2];
        ealpha3=error[FCSADiff_diff_alpha3];
    }

    // calculate 1/N
    data[FCSADiff_1n_particle]=1.0/N;
    if (error) error[FCSADiff_1n_particle]=fabs(eN/N/N);

    // calculate Veff = pi^(3/2) * gamma * wxy^3
    const double pi32=sqrt(cube(M_PI));
    data[FCSADiff_focus_volume]=pi32*gamma*cube(wxy);
    if (error) error[FCSADiff_focus_volume]=sqrt(sqr(egamma*pi32*cube(wxy))+sqr(ewxy*3.0*pi32*gamma*sqr(wxy)));

    // calculate C = N / Veff
    const double pim32=1.0/sqrt(cube(M_PI));
    if (data[FCSADiff_focus_volume]!=0) data[FCSADiff_concentration]=N/data[FCSADiff_focus_volume]/(NAVOGADRO * 1.0e-24); else data[FCSADiff_concentration]=0;
    if (error) {
        if ((wxy!=0)&&(gamma!=0)) error[FCSADiff_concentration]=sqrt( sqr(egamma*pim32*N/cube(wxy)/sqr(gamma)) + sqr(ewxy*3.0*pim32*N/gamma/pow4(wxy)) + sqr(eN*pim32/gamma/cube(wxy)) )/(NAVOGADRO * 1.0e-24);
        else error[FCSADiff_concentration]=0;
    }

}

bool QFFitFunctionFCSADiff::isParameterVisible(int parameter, double* data) const {
    int comp=data[FCSADiff_n_components];
    int nonfl_comp=data[FCSADiff_n_nonfluorescent];
    switch(parameter) {
        case FCSADiff_nonfl_tau1: case FCSADiff_nonfl_theta1: return nonfl_comp>0;
        case FCSADiff_nonfl_tau2: case FCSADiff_nonfl_theta2: return nonfl_comp>1;
        case FCSADiff_diff_tau1:
        case FCSADiff_diff_alpha1: return comp>0;
        case FCSADiff_diff_rho1:
        case FCSADiff_diff_rho2:
        case FCSADiff_diff_tau2:
        case FCSADiff_diff_alpha2: return comp>1;
        case FCSADiff_diff_rho3:
        case FCSADiff_diff_tau3:
        case FCSADiff_diff_alpha3: return comp>2;
    }
    return true;
}

unsigned int QFFitFunctionFCSADiff::getAdditionalPlotCount(const double* params) {
    return 1;
}

QString QFFitFunctionFCSADiff::transformParametersForAdditionalPlot(int plot, double* params) {
    params[FCSADiff_n_nonfluorescent]=0;
    return "without non-fluorescent";
}








QStringList QFPFitFunctionFCS::getIDs() const {
    QStringList res;
    res<<"fcs_diff"<<"fcs_adiff";
    return res;
}

QFFitFunction* QFPFitFunctionFCS::get(QString id, QObject* parent) const  {
    if (id=="fcs_diff") {
        return new QFFitFunctionFCSDiff();
    } else if (id=="fcs_adiff") {
        return new QFFitFunctionFCSADiff();
    }
    return NULL;
}

Q_EXPORT_PLUGIN2(qffcsfitfuncs, QFPFitFunctionFCS)
