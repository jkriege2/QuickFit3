/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "qffitfunctionfcsdistributionloggaussianint.h"
#include <QDebug>
#include <cmath>
#define sqr(x) ((x)*(x))
#define cube(x) ((x)*(x)*(x))
#define pow4(x) ((x)*(x)*(x)*(x))
#define pow5(x) ((x)*(x)*(x)*(x)*(x))
#define NAVOGADRO (6.02214179e23)
#define KBOLTZ 1.3806488e-23


QFFitFunctionFCSDistributionIntLogGaussian::QFFitFunctionFCSDistributionIntLogGaussian() {
    wN=1000;
    w = gsl_integration_workspace_alloc (wN);

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
    addParameter(FloatNumber,  "n_particle",              "particle number N",                                     "N",                        "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 10,           1e-10,    1e5,      1,   0);
    #define FCSDLG_n_particle 5
    addParameter(FloatNumber,  "1n_particle",             "1/particle number N",                                   "1/N",                      "",            "",                      false,     false,        false,             QFFitFunction::DisplayError, false, 0.1,          1e-10,    1e5,      0.1, 0);
    #define FCSDLG_1n_particle 6
    addParameter(FloatNumber,  "diff_tau1",               "center diffusion time of distribution",                 "&tau;<sub>D,c</sub>",      "usec",        "&mu;s",                 true,      true,         true,              QFFitFunction::DisplayError, false, 300,           1,        1e10,     1,   0        );
    #define FCSDLG_diff_tau1 7
    addParameter(FloatNumber,  "b",                       "width parameter distribution",                          "b",                        "ln(sec)",    "ln(sec)",                true,      true,         true,              QFFitFunction::DisplayError, false, 1,            1e-10,    1e10,     1,   0);
    #define FCSDLG_b 8
    addParameter(FloatNumber,  "offset",                  "correlation offset",                                    "G<sub>&infin;</sub>",      "",           "",                       true,      true,         true,              QFFitFunction::DisplayError, true, 0,            -10,      10,       0.1  );
    #define FCSDLG_offset 9
    addParameter(FloatNumber,  "focus_struct_fac",        "focus: axial ratio",                                    "&gamma;",                  "",           "",                       true,      true,         true,              QFFitFunction::EditError,    true, 6,            0.01,     100,      0.5  );
    #define FCSDLG_focus_struct_fac 10
    addParameter(FloatNumber,  "focus_width",             "focus: lateral radius",                                 "w<sub>x,y</sub>",          "nm",         "nm",                     false,     true,        false,              QFFitFunction::EditError,    false, 250,          0,        1e4,      1    );
    #define FCSDLG_focus_width 11
    addParameter(FloatNumber,  "focus_volume",            "focus: effective colume",                               "V<sub>eff</sub>",          "fl",         "fl",                     false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define FCSDLG_focus_volume 12
    addParameter(FloatNumber,  "tau_range_min",           "smallest evaluated diffusion time",                     "&tau;<sub>min</sub>",      "usec",       "&mu;s",                  false,    true,        false,              QFFitFunction::NoError,      false, 0.001,         0,        1e50,     10,  0    );
    #define FCSDLG_tau_range_min 13
    addParameter(FloatNumber,  "tau_range_max",           "largest evaluated diffusion time",                      "&tau;<sub>max</sub>",      "usec",       "&mu;s",                  false,    true,        false,              QFFitFunction::NoError,      false, 1e7,          0,        1e50,     10,  0    );
    #define FCSDLG_tau_range_max 14
    addParameter(FloatNumber,  "concentration",           "particle concentration in focus",                       "C<sub>all</sub>",          "nM",         "nM",                     false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define FCSDLG_concentration 15
    addParameter(FloatNumber,  "diff_coeff1",             "center diffusion coefficient of distriubution",         "D<sub>c</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    false,    false,        false,              QFFitFunction::DisplayError, false, 500,          0,        1e50,     1    );
    #define FCSDLG_diff_coeff1 16
    addParameter(FloatNumber,  "count_rate",              "count rate during measurement",                         "count rate",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError,    false, 0,            0,        1e50,     1    );
    #define FCSDLG_count_rate 17
    addParameter(FloatNumber,  "background",              "background count rate during measurement",              "background",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError  ,  false, 0,            0,        1e50,     1    );
    #define FCSDiff_background 22
    addParameter(FloatNumber,  "cpm",                     "photon counts per molecule",                            "cnt/molec",                "Hz",         "Hz",                     false,    false,        false,              QFFitFunction::DisplayError, false, 0,            0,        1e50,     1    );
    #define FCSDLG_cpm 18
}

QFFitFunctionFCSDistributionIntLogGaussian::~QFFitFunctionFCSDistributionIntLogGaussian()
{
    if (w) gsl_integration_workspace_free (w);
}



struct QFFitFunctionFCSDistributionIntLogGaussian_intparam {
    double tau;
    double tauC;
    double b;
    double gamma;
};

double QFFitFunctionFCSDistributionIntLogGaussian_f(double x, void * params) {
    const QFFitFunctionFCSDistributionIntLogGaussian_intparam* p = (QFFitFunctionFCSDistributionIntLogGaussian_intparam*)params;
    const double expx=exp(x);
    return exp(-0.5*qfSqr((x-log(p->tauC))/p->b))/(1.0+p->tau/expx)/sqrt(1.0+p->tau/expx/p->gamma/p->gamma)/sqrt(2.0*M_PI*qfSqr(p->b*expx));
}

double QFFitFunctionFCSDistributionIntLogGaussian::evaluate(double t, const double* data) const {
    const int nonfl_comp=data[FCSDLG_n_nonfluorescent];
    const double N=data[FCSDLG_n_particle];
    const double nf_tau1=data[FCSDLG_nonfl_tau1]/1.0e6;
    const double nf_theta1=data[FCSDLG_nonfl_theta1];
    const double nf_tau2=data[FCSDLG_nonfl_tau2]/1.0e6;
    const double nf_theta2=data[FCSDLG_nonfl_theta2];
    const double tauD1=data[FCSDLG_diff_tau1]/1.0e6;
    const double b=data[FCSDLG_b];
    const double tau_min=data[FCSDLG_tau_range_min]/1.0e6;
    const double tau_max=data[FCSDLG_tau_range_max]/1.0e6;

    const double background=data[FCSDiff_background];
    const double cr=data[FCSDLG_count_rate];
    double backfactor=qfSqr(cr-background)/qfSqr(cr);
    if (fabs(cr)<1e-15) backfactor=1;


    double gamma=data[FCSDLG_focus_struct_fac];
    if (gamma==0) gamma=1;
    const double gamma2=sqr(gamma);

    const double offset=data[FCSDLG_offset];


    if (N>0) {
        register double diff=0.0;
        double diff1=0.0;
        double diff2=0.0;
        double error=0;

        QFFitFunctionFCSDistributionIntLogGaussian_intparam p = {t, tauD1, b, gamma};
        p.gamma=gamma;
        p.tau=t;
        p.tauC=tauD1;
        p.b=b;

        gsl_function F;
        F.function = &QFFitFunctionFCSDistributionIntLogGaussian_f;
        F.params = &p;

        gsl_error_handler_t * old_h=gsl_set_error_handler_off();



        gsl_integration_qags(&F, log(tau_min), log(tauD1), 0, 1e-7, wN, w, &diff1, &error);
        gsl_integration_qags(&F, log(tauD1), log(tau_max), 0, 1e-7, wN, w, &diff2, &error);
        diff=diff1+diff2;

        p.tau=0;
        gsl_integration_qags(&F, log(tau_min), log(tauD1), 0, 1e-7, wN, w, &diff1, &error);
        gsl_integration_qags(&F, log(tauD1), log(tau_max), 0, 1e-7, wN, w, &diff2, &error);
        diff=diff/(diff1+diff2);


        //qDebug()<<tauD1*1e6<<tauD1_sigma*1e6<<diff<<"="<<diff1<<"+"<<diff2<<"+"<<diff3<<"+"<<diff4;

        gsl_set_error_handler(old_h);

        double pre=1.0;
        if (nonfl_comp==1) {
            pre=(1.0-nf_theta1+nf_theta1*exp(-t/nf_tau1))/(1.0-nf_theta1);
        } else if (nonfl_comp==2) {
            pre=(1.0-nf_theta1+nf_theta1*exp(-t/nf_tau1)-nf_theta2+nf_theta2*exp(-t/nf_tau2))/(1.0-nf_theta1-nf_theta2);
        }
        return offset+pre/N*diff*backfactor;
    } else {
        return -1.0*exp(-0.5*qfSqr((log(t)-log(tauD1))/b))/N*backfactor;
    }
}

void QFFitFunctionFCSDistributionIntLogGaussian::calcParameter(double* data, double* error) const {
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
    //double b=data[FCSDLG_b];
    double eb=0;
    double gamma=data[FCSDLG_focus_struct_fac];
    double egamma=0;
    //double gamma2=sqr(gamma);
    double wxy=data[FCSDLG_focus_width]/1.0e3;
    double ewxy=0;
    //double offset=data[FCSDLG_offset];
    double eoffset=0;
    double cps=data[FCSDLG_count_rate];
    double ecps=0;
    //double cpm=data[FCSDLG_cpm];
    double ecpm=0;
    double background=data[FCSDiff_background];
    double ebackground=0;

    if (error) {
        eN=error[FCSDLG_n_particle];
        enf_tau1=error[FCSDLG_nonfl_tau1]/1.0e6;
        enf_theta1=error[FCSDLG_nonfl_theta1];
        enf_tau2=error[FCSDLG_nonfl_tau2]/1.0e6;
        enf_theta2=error[FCSDLG_nonfl_theta2];
        etauD1=error[FCSDLG_diff_tau1]/1.0e6;
        eb=error[FCSDLG_b];
        egamma=error[FCSDLG_focus_struct_fac];
        ewxy=error[FCSDLG_focus_width]/1.0e3;
        eoffset=error[FCSDLG_offset];
        ecps=error[FCSDLG_count_rate];
        ecpm=error[FCSDLG_cpm];
        ebackground=error[FCSDiff_background];
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

    // calculate CPM = CPS/N
    // calculate CPM = CPS/N
    data[FCSDLG_cpm]=(cps-background)/N;
    if (error) error[FCSDLG_cpm]=sqrt(sqr(ecps/N)+sqr(ebackground/N)+sqr(eN*(cps-background)/sqr(N)));
}

bool QFFitFunctionFCSDistributionIntLogGaussian::isParameterVisible(int parameter, const double* data) const {
    int nonfl_comp=data[FCSDLG_n_nonfluorescent];
    switch(parameter) {
        case FCSDLG_nonfl_tau1: case FCSDLG_nonfl_theta1: return nonfl_comp>0;
        case FCSDLG_nonfl_tau2: case FCSDLG_nonfl_theta2: return nonfl_comp>1;
    }
    return true;
}

unsigned int QFFitFunctionFCSDistributionIntLogGaussian::getAdditionalPlotCount(const double* params) {
    return 2;
}

QString QFFitFunctionFCSDistributionIntLogGaussian::transformParametersForAdditionalPlot(int plot, double* params) {
    if (plot==0) {
        params[FCSDLG_n_nonfluorescent]=0;
        return "without non-fluorescent";
    } else if (plot==1) {
        params[FCSDLG_n_particle]=-1.0*fabs(params[FCSDLG_n_particle]);
        return "distribution";
    }
    return "";
}
