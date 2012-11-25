#include "qffitfunctionfcsdistributiongaussian.h"

#include <cmath>
#define sqr(x) ((x)*(x))
#define cube(x) ((x)*(x)*(x))
#define pow4(x) ((x)*(x)*(x)*(x))
#define pow5(x) ((x)*(x)*(x)*(x)*(x))
#define NAVOGADRO (6.02214179e23)
#define KBOLTZ 1.3806488e-23


QFFitFunctionFCSDistributionGaussian::QFFitFunctionFCSDistributionGaussian() {
    //           type,         id,                        name,                                                    label,                      unit,          unitlabel,               fit,       userEditable, userRangeEditable, displayError,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(IntCombo,     "n_nonfluorescent",        "number of nonfluorescent components (triplet ...)",     "# non-fluorescent",        "",            "",                      false,     true,         false,             QFFitFunction::NoError,      false, 1,            0,        2,        1,   0,      2);
    #define FCSDLG_n_nonfluorescent 0
    addParameter(FloatNumber,  "nonfl_tau1",              "triplet decay time",                                    "&tau;<sub>trip</sub>",     "usec",        "&mu;s",                 true,      true,         true,              QFFitFunction::DisplayError, false, 3.0,          0,        10,       0.1, 0  );
    #define FCSDLG_nonfl_tau1 1
    addParameter(FloatNumber,  "nonfl_theta1",            "triplet fraction",                                      "&theta;<sub>trip</sub>",   "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 0.2,          0,        0.99999,  0.1, 0,      1);
    #define FCSDLG_nonfl_theta1 2
    addParameter(FloatNumber,  "nonfl_tau2",              "dark component 2 decay time",                           "&tau;<sub>dark,2</sub>",   "usec",        "&mu;s",                 true,      true,         true,              QFFitFunction::DisplayError, false, 300,          1e-10,    1e5,      1,   0);
    #define FCSDLG_nonfl_tau2 3
    addParameter(FloatNumber,  "nonfl_theta2",            "dark component 2 fraction",                             "&theta;<sub>dark,2</sub>", "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 0.2,          0,        0.99999,  0.1, 0,      1);
    #define FCSDLG_nonfl_theta2 4
    addParameter(FloatNumber,  "n_particle",              "Particle number N",                                     "N",                        "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 10,           1e-10,    1e5,      1,   0);
    #define FCSDLG_n_particle 5
    addParameter(FloatNumber,  "1n_particle",             "1/Particle number N",                                   "1/N",                      "",            "",                      false,     false,        false,             QFFitFunction::DisplayError, false, 0.1,          1e-10,    1e5,      0.1, 0);
    #define FCSDLG_1n_particle 6
    addParameter(FloatNumber,  "diff_tau1",               "center diffusion time of distribution",                 "&tau;<sub>D,c</sub>",      "usec",        "&mu;s",                 true,      true,         true,              QFFitFunction::DisplayError, false, 30,           1,        1e10,     1,   0        );
    #define FCSDLG_diff_tau1 7
    addParameter(FloatNumber,  "diff_tau_sigma",          "width of tauD  distribution",                           "&sigma;(&tau;<sub>D</sub>)", "usec",        "&mu;s",                  true,      true,         true,              QFFitFunction::DisplayError, false, 1,            1e-10,    1e10,     1,   0);
    #define FCSDLG_dif_tau_sigma 8
    addParameter(FloatNumber,  "offset",                  "correlation offset",                                    "G<sub>&infin;</sub>",      "",           "",                       true,      true,         true,              QFFitFunction::DisplayError, true, 0,            -10,      10,       0.1  );
    #define FCSDLG_offset 9
    addParameter(FloatNumber,  "focus_struct_fac",        "focus: axial ratio",                                    "&gamma;",                  "",           "",                       true,      true,         true,              QFFitFunction::EditError,    true, 6,            0.01,     100,      0.5  );
    #define FCSDLG_focus_struct_fac 10
    addParameter(FloatNumber,  "focus_width",             "focus: lateral radius",                                 "w<sub>x,y</sub>",          "nm",         "nm",                     false,     true,        false,              QFFitFunction::EditError,    false, 250,          0,        1e4,      1    );
    #define FCSDLG_focus_width 11
    addParameter(FloatNumber,  "focus_volume",            "focus: effective colume",                               "V<sub>eff</sub>",          "fl",         "fl",                     false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define FCSDLG_focus_volume 12
    addParameter(FloatNumber,  "tau_range_min",           "smallest evaluated diffusion time",                     "&tau;<sub>min</sub>",      "usec",       "&mu;s",                  false,    true,        false,              QFFitFunction::NoError,      false, 1,            0,        1e50,     10,  0    );
    #define FCSDLG_tau_range_min 13
    addParameter(FloatNumber,  "tau_range_max",           "largest evaluated diffusion time",                      "&tau;<sub>max</sub>",      "usec",       "&mu;s",                  false,    true,        false,              QFFitFunction::NoError,      false, 1e7,          0,        1e50,     10,  0    );
    #define FCSDLG_tau_range_max 14
    addParameter(IntNumber,  "D_values_per_decade",       "D samples per decade in distribution",                    "K<sub>D</sub>",              "",           "",                   false,    true,        false,          QFFitFunction::NoError,      false, 200,           1,        10000,     10,  0    );
    #define FCSDLG_K 15
    addParameter(FloatNumber,  "concentration",           "particle concentration in focus",                       "C<sub>all</sub>",          "nM",         "nM",                     false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define FCSDLG_concentration 16
    addParameter(FloatNumber,  "diff_coeff1",             "center diffusion coefficient of distribution",          "D<sub>c</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    false,    false,        false,              QFFitFunction::DisplayError, false, 500,          0,        1e50,     1    );
    #define FCSDLG_diff_coeff1 17
    addParameter(FloatNumber,  "viscosity",               "sample viscosity",                                      "&eta;",                    "mPa*s",      "mPa&middot;s",           false,    true,        false,              QFFitFunction::NoError, false, 1.002,        0,        1e50,     0.02    );
    #define FCSDLG_viscosity 18
    addParameter(FloatNumber,  "temperature",             "sample temperature",                                      "&thetasym;",                 "°C",      "°C",                       false,    true,        false,              QFFitFunction::NoError, false, 20,        0,        1e50,     1    );
    #define FCSDLG_temperature 19
    addParameter(FloatNumber,  "hydrodyn_radius1",        "center hydrodynamic radius",                            "R<sub>H,c</sub>",          "nm",        "nm",                      false,    false,        false,              QFFitFunction::DisplayError, false, 1,           1,        1e10,     1,   0        );
    #define FCSDLG_RH 20
    addParameter(FloatNumber,  "hydrodyn_radius_sigma",   "distibution width of hydrodynamic radius",              "&sigma;(R<sub>H,c</sub>)", "nm",        "nm",                      false,    false,        false,              QFFitFunction::DisplayError, false, 1,           1,        1e10,     1,   0        );
    #define FCSDLG_RH_sigma 21
    addParameter(FloatNumber,  "count_rate",              "count rate during measurement",                         "count rate",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError,    false, 0,            0,        1e50,     1    );
    #define FCSDLG_count_rate 22
    addParameter(FloatNumber,  "cpm",                     "photon counts per molecule",                            "cnt/molec",                "Hz",         "Hz",                     false,    false,        false,              QFFitFunction::DisplayError, false, 0,            0,        1e50,     1    );
    #define FCSDLG_cpm 23
    /*last_tau_min=0;
    last_tau_max=0;
    last_logc=0;
    last_b=0;*/
}

QVector<QPair<double, double> >  QFFitFunctionFCSDistributionGaussian::fillTauVal(double tau_min, double tau_max, uint32_t tau_values, double tauC, double tauSigma) const {
    QVector<QPair<double, double> >  tau_val;
    const double l10=log(10.0);
    double ltau_max=log(tau_max)/l10;
    const double tau_inc=1.0/double(tau_values);//(log(tau_max)-log(Dmin))/(l10*tau_values);
    const double tau_sigma2=tauSigma*tauSigma;

    double sum=0;
    for (double tau=log(tau_min)/l10; tau<=ltau_max; tau+=tau_inc) {
        const double tt=pow(10.0, tau);
        const double val=exp(-0.5*sqr(tt-tauC)/tau_sigma2);
        sum=sum+val;
        tau_val.append(qMakePair(tt, val));
    }
    const double tt=pow(10.0, tauC);
    const double val=1;
    sum=sum+val;
    tau_val.append(qMakePair(tt, val));

    // normalize distribution
    for (int i=0; i<tau_val.size(); i++) {
        QPair<double, double> p=tau_val[i];
        p.second=p.second/sum;
        tau_val[i]=p;
    }
    return tau_val;
}



double QFFitFunctionFCSDistributionGaussian::evaluate(double t, const double* data) const {
    const int nonfl_comp=data[FCSDLG_n_nonfluorescent];
    const double N=data[FCSDLG_n_particle];
    const double nf_tau1=data[FCSDLG_nonfl_tau1]/1.0e6;
    const double nf_theta1=data[FCSDLG_nonfl_theta1];
    const double nf_tau2=data[FCSDLG_nonfl_tau2]/1.0e6;
    const double nf_theta2=data[FCSDLG_nonfl_theta2];
    const double tauD1=data[FCSDLG_diff_tau1]/1.0e6;
    const double tauD1_sigma=data[FCSDLG_dif_tau_sigma]/1.0e6;
    const double tau_min=data[FCSDLG_tau_range_min]/1.0e6;
    const double tau_max=data[FCSDLG_tau_range_max]/1.0e6;
    uint32_t tau_values=data[FCSDLG_K];
    if (data[FCSDLG_K]<=0) tau_values=10;

    double gamma=data[FCSDLG_focus_struct_fac];
    if (gamma==0) gamma=1;
    const double gamma2=sqr(gamma);

    const double offset=data[FCSDLG_offset];


    if (N>0) {
        QVector<QPair<double, double> >  tau_val=fillTauVal(tau_min, tau_max, tau_values, tauD1, tauD1_sigma);
        register double diff=0;
        for (int i=0; i<tau_val.size(); i++) {
            const QPair<double, double>& p=tau_val[i];
            register double reltau1=t/p.first;
            diff=diff+p.second/(1.0+reltau1)/sqrt(1.0+reltau1/gamma2);
        }

        double pre=1.0;
        if (nonfl_comp==1) {
            pre=(1.0-nf_theta1+nf_theta1*exp(-t/nf_tau1))/(1.0-nf_theta1);
        } else if (nonfl_comp==2) {
            pre=(1.0-nf_theta1+nf_theta1*exp(-t/nf_tau1)-nf_theta2+nf_theta2*exp(-t/nf_tau2))/(1.0-nf_theta1-nf_theta2);
        }
        return offset+pre/N*diff;
    } else {
        if (t>=tau_min && t<=tau_max) return -1.0*exp(-0.5*sqr(t-tauD1)/tauD1_sigma/tauD1_sigma)/N;
        else return 0;
    }
}

void QFFitFunctionFCSDistributionGaussian::calcParameter(double* data, double* error) const {
    //int comp=data[FCSDLG_n_components];
    //int nonfl_comp=data[FCSDLG_n_nonfluorescent];
    double N=data[FCSDLG_n_particle];
    double eN=0;
    //double nf_tau1=data[FCSDLG_nonfl_tau1];
    double enf_tau1=0;
    double nf_theta1=data[FCSDLG_nonfl_theta1];
    double enf_theta1=0;
    //double nf_tau2=data[FCSDLG_nonfl_tau2];
    double enf_tau2=0;
    double nf_theta2=data[FCSDLG_nonfl_theta2];
    double enf_theta2=0;
    double tauD1=data[FCSDLG_diff_tau1]/1.0e6;
    double etauD1=0;
    double tauD1_sigma=data[FCSDLG_dif_tau_sigma]/1.0e6;
    double etauD1_sigma=0;
    double gamma=data[FCSDLG_focus_struct_fac];
    double egamma=0;
    //double gamma2=sqr(gamma);
    double wxy=data[FCSDLG_focus_width]/1.0e3;
    double ewxy=0;
    double viscosity=data[FCSDLG_viscosity]/1.0e3;
    double eviscosity=0;
    double temperature=273.15+data[FCSDLG_temperature];
    double etemperature=0;
    //double offset=data[FCSDLG_offset];
    double eoffset=0;
    double cps=data[FCSDLG_count_rate];
    double ecps=0;
    //double cpm=data[FCSDLG_cpm];
    double ecpm=0;

    if (error) {
        eN=error[FCSDLG_n_particle];
        enf_tau1=error[FCSDLG_nonfl_tau1]/1.0e6;
        enf_theta1=error[FCSDLG_nonfl_theta1];
        enf_tau2=error[FCSDLG_nonfl_tau2]/1.0e6;
        enf_theta2=error[FCSDLG_nonfl_theta2];
        etauD1=error[FCSDLG_diff_tau1]/1.0e6;
        egamma=error[FCSDLG_focus_struct_fac];
        ewxy=error[FCSDLG_focus_width]/1.0e3;
        eoffset=error[FCSDLG_offset];
        eviscosity=error[FCSDLG_viscosity]/1.0e3;
        etemperature=error[FCSDLG_temperature];
        ecps=error[FCSDLG_count_rate];
        ecpm=error[FCSDLG_cpm];
        etauD1_sigma=error[FCSDLG_dif_tau_sigma]/1.0e6;
    }

    // correct for invalid fractions
    if (nf_theta1>1.0) nf_theta1=1.0;
    if (nf_theta2>1.0) nf_theta2=1.0;
    if (nf_theta1+nf_theta2>1.0) {
        nf_theta2=1.0-nf_theta1;
    }
    data[FCSDLG_nonfl_theta1]=nf_theta1;
    data[FCSDLG_nonfl_theta2]=nf_theta2;


    // calculate 1/N
    data[FCSDLG_1n_particle]=1.0/N;
    if (error) error[FCSDLG_1n_particle]=fabs(eN/N/N);

    // calculate Veff = pi^(3/2) * gamma * wxy^3
    const double pi32=sqrt(cube(M_PI));
    data[FCSDLG_focus_volume]=pi32*gamma*cube(wxy);
    if (error) error[FCSDLG_focus_volume]=sqrt(sqr(egamma*pi32*cube(wxy))+sqr(ewxy*3.0*pi32*gamma*sqr(wxy)));

    // calculate C = N / Veff
    const double pim32=1.0/sqrt(cube(M_PI));
    if (data[FCSDLG_focus_volume]!=0) data[FCSDLG_concentration]=N/data[FCSDLG_focus_volume]/(NAVOGADRO * 1.0e-24); else data[FCSDLG_concentration]=0;
    if (error) {
        if ((wxy!=0)&&(gamma!=0)) error[FCSDLG_concentration]=sqrt( sqr(egamma*pim32*N/cube(wxy)/sqr(gamma)) + sqr(ewxy*3.0*pim32*N/gamma/pow4(wxy)) + sqr(eN*pim32/gamma/cube(wxy)) )/(NAVOGADRO * 1.0e-24);
        else error[FCSDLG_concentration]=0;
    }

    // calculate D1 = wxy^2 / (4*tauD1)
    if (tauD1!=0) data[FCSDLG_diff_coeff1]=sqr(wxy)/4.0/tauD1; else data[FCSDLG_diff_coeff1]=0;
    if (error) {
        if (tauD1!=0) error[FCSDLG_diff_coeff1]=sqrt( sqr(etauD1*sqr(wxy)/sqr(tauD1)/4.0) + sqr(ewxy*2.0*wxy/tauD1/4.0) );
        else error[FCSDLG_diff_coeff1]=0;
    }

    // calculate hydrodyn radii
    data[FCSDLG_RH]=4.0*KBOLTZ*temperature*tauD1/sqr(wxy*1e-6)/6.0/M_PI/viscosity*1.0e9;
    data[FCSDLG_RH_sigma]=4.0*KBOLTZ*temperature*tauD1_sigma/sqr(wxy*1e-6)/6.0/M_PI/viscosity*1.0e9;

    // calculate CPM = CPS/N
    data[FCSDLG_cpm]=cps/N;
    error[FCSDLG_cpm]=sqrt(sqr(ecps/N)+sqr(eN*cps/sqr(N)));
}

bool QFFitFunctionFCSDistributionGaussian::isParameterVisible(int parameter, const double* data) const {
    int nonfl_comp=data[FCSDLG_n_nonfluorescent];
    switch(parameter) {
        case FCSDLG_nonfl_tau1: case FCSDLG_nonfl_theta1: return nonfl_comp>0;
        case FCSDLG_nonfl_tau2: case FCSDLG_nonfl_theta2: return nonfl_comp>1;
    }
    return true;
}

unsigned int QFFitFunctionFCSDistributionGaussian::getAdditionalPlotCount(const double* params) {
    return 2;
}

QString QFFitFunctionFCSDistributionGaussian::transformParametersForAdditionalPlot(int plot, double* params) {
    if (plot==0) {
        params[FCSDLG_n_nonfluorescent]=0;
        return "without non-fluorescent";
    } else if (plot==1) {
        params[FCSDLG_n_particle]=-1.0*fabs(params[FCSDLG_n_particle]);
        return "distribution";
    }
    return "";
}
