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

#include "qffitfunctiondlsg2_loggaussiantaudist.h"

#include <cmath>
#include "qfmathtools.h"
#define sqr(x) qfSqr(x)
#define cube(x) qfCube(x)
#define pow4(x) ((x)*(x)*(x)*(x))
#define pow5(x) ((x)*(x)*(x)*(x)*(x))


QFFitFunctionDLSG2LogNormTauDist::QFFitFunctionDLSG2LogNormTauDist() {
    wN=1000;
    w = gsl_integration_workspace_alloc (wN);
    //           type,         id,                        name,                                                    label,                      unit,          unitlabel,               fit,       userEditable, userRangeEditable, displayError,                initialFix, initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(FloatNumber,  "epsilon",                 "baseline",                                              "&epsilon;",                "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 0,            -1e50,    1e50,     0.01);
    #define DLSG2_epsilon 0
    addParameter(FloatNumber,  "amplitude",               "amplitude",                                             "A",                        "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, true, 1,            -1e50,    1e50,     0.01);
    #define DLSG2_A 1

    addParameter(FloatNumber,  "tau1",                    "correlation time 1",                                    "&tau;<sub>1</sub>",        QLatin1String("\xB5s"),         "&mu;s",                  true,      true,         true,              QFFitFunction::DisplayError, false, 1000,         1e-10,    1e50,     1    );
    #define DLSG2_tau1 2
    addParameter(FloatNumber,  "b",                       "width parameter of tau distribution",                   "b",                        "ln(sec)",    "ln(sec)",                true,      true,         true,              QFFitFunction::DisplayError, false, 1,          1e-10,    1e50,     1    );
    #define DLSG2_b 3

    addParameter(FloatNumber,  "offset",                  "correlation offset",                                    "G<sub>&infin;</sub>",      "",           "",                       true,      true,         true,              QFFitFunction::DisplayError, true, 1,            -10,      10,       0.1  );
    #define DLSG2_offset 4

    addParameter(FloatNumber,  "dls_angle",               "scattering angle",                                      "&Theta;<sub>scatter</sub>", QLatin1String("\xB0"),         QLatin1String("\xB0"),                      false,     true,        false,              QFFitFunction::EditError,    false, 90,          0,        1e4,      1    );
    #define DLSG2_angle 5
    addParameter(FloatNumber,  "refractive_index",        "refractive index",                                      "n",                        "",           "",                       false,     true,        false,              QFFitFunction::EditError,    false, 1.33,        0,        1e4,      1    );
    #define DLSG2_refractive 6
    addParameter(FloatNumber,  "wavelength",              "scattering light wavelength",                           "&lambda;",                 "nm",         "nm",                     false,     true,        false,              QFFitFunction::EditError,    false, 488,         0,        1e4,      1    );
    #define DLSG2_wavelength 7
    addParameter(FloatNumber,  "q_square",                "q square",                                              "q<sup>2</sup>",            "1/nm^2",      "nm<sup>-2</sup>",        false,    false,        false,              QFFitFunction::DisplayError, false, 500,          -1e50,    1e50,     1    );
    #define DLSG2_diff_qsquare 8

    addParameter(FloatNumber,  "tau_range_min",           "smallest evaluated diffusion time",                     "&tau;<sub>min</sub>",      "usec",       "&mu;s",                  false,    true,        false,              QFFitFunction::NoError,      false, 0.001,         0,        1e50,     10,  0    );
    #define DLSG2_tau_range_min 9
    addParameter(FloatNumber,  "tau_range_max",           "largest evaluated diffusion time",                      "&tau;<sub>max</sub>",      "usec",       "&mu;s",                  false,    true,        false,              QFFitFunction::NoError,      false, 1e7,          0,        1e50,     10,  0    );
    #define DLSG2_tau_range_max 10

    addParameter(FloatNumber,  "diff_coeff1",             "center diffusion coefficient of distribution",          "D<sub>c</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    false,    false,        false,              QFFitFunction::DisplayError, false, 500,          0,        1e50,     1    );
    #define DLSG2_diff_coeff1 11

}

QFFitFunctionDLSG2LogNormTauDist::~QFFitFunctionDLSG2LogNormTauDist()
{
    if (w) gsl_integration_workspace_free (w);
}


struct QFFitFunctionDLSG2LogNormTauDist_intparam {
    double tau;
    double tauC;
    double b;
};

double QFFitFunctionDLSG2LogNormTauDist_f(double x, void * params) {
    const QFFitFunctionDLSG2LogNormTauDist_intparam* p = (QFFitFunctionDLSG2LogNormTauDist_intparam*)params;
    const double expx=exp(x);
    return exp(-0.5*qfSqr((x-log(p->tauC))/p->b))/sqrt(2.0*M_PI*qfSqr(p->b*expx))*exp(-p->tau/expx);
}


double QFFitFunctionDLSG2LogNormTauDist::evaluate(double t, const double* data) const {
    const double epsilon=data[DLSG2_epsilon];
    const double A=data[DLSG2_A];
    const double tau1=data[DLSG2_tau1]/1e6;
    const double offset=data[DLSG2_offset];
    const double b=data[DLSG2_b];
    const double tau_min=data[DLSG2_tau_range_min]/1.0e6;
    const double tau_max=data[DLSG2_tau_range_max]/1.0e6;


    if (A>0) {
        register double g1=0.0;
        double diff1=0.0;
        double diff2=0.0;
        double error=0;

        QFFitFunctionDLSG2LogNormTauDist_intparam p;
        p.tau=t;
        p.tauC=tau1;
        p.b=b;

        gsl_function F;
        F.function = &QFFitFunctionDLSG2LogNormTauDist_f;
        F.params = &p;

        gsl_error_handler_t * old_h=gsl_set_error_handler_off();

        gsl_integration_qags(&F, log(tau_min), log(tau1), 0, 1e-7, wN, w, &diff1, &error);
        gsl_integration_qags(&F, log(tau1), log(tau_max), 0, 1e-7, wN, w, &diff2, &error);
        g1=diff1+diff2;

        p.tau=0;
        gsl_integration_qags(&F, log(tau_min), log(tau1), 0, 1e-7, wN, w, &diff1, &error);
        gsl_integration_qags(&F, log(tau1), log(tau_max), 0, 1e-7, wN, w, &diff2, &error);
        g1=g1/(diff1+diff2);


        gsl_set_error_handler(old_h);

        return offset+epsilon+sqr(A*g1);
    } else {
        return -exp(-0.5*qfSqr((log(t)-log(tau1))/b))*A+offset+epsilon;
    }
}


void QFFitFunctionDLSG2LogNormTauDist::calcParameter(double* data, double* error) const {
    const double tau1=data[DLSG2_tau1]/1e6;
    double etau1=0;

    const double n=data[DLSG2_refractive];
    double en=0;
    const double theta=data[DLSG2_angle]/180.0*M_PI;
    double etheta=0;
    const double lambda=data[DLSG2_wavelength]/1e3;
    double elambda=0;
    if (error) {
        en=error[DLSG2_refractive];
        etheta=error[DLSG2_angle]/180*M_PI;
        elambda=error[DLSG2_wavelength]/1e3;
        etau1=error[DLSG2_tau1]/1e6;
    }

    const double q=4.0*M_PI*n/lambda*sin(theta/2.0);

    data[DLSG2_diff_qsquare]=sqr(q);
    double eq=0;
    if (error) {
        eq=sqrt(sqr(en*4.0*M_PI/lambda*sin(theta/2.0))+sqr(elambda*4.0*M_PI*n/lambda/lambda*sin(theta/2.0))+sqr(etheta*2.0*M_PI/lambda*cos(theta/2.0)));
        error[DLSG2_diff_qsquare]=2*q*eq;
    }


    data[DLSG2_diff_coeff1]=0;
    if (error) {
        error[DLSG2_diff_coeff1]=0;
    }

    data[DLSG2_diff_coeff1]=1.0/q/q/tau1;
    if (error) error[DLSG2_diff_coeff1]=sqrt(sqr(eq*2.0/(cube(q)*tau1))+sqr(etau1/(sqr(q)*sqr(tau1))));


}

bool QFFitFunctionDLSG2LogNormTauDist::isParameterVisible(int parameter, const double* data) const {

    Q_UNUSED(parameter);
    Q_UNUSED(data);
    return true;
}

unsigned int QFFitFunctionDLSG2LogNormTauDist::getAdditionalPlotCount(const double* params) const {
    Q_UNUSED(params);
    return 1;
}

QString QFFitFunctionDLSG2LogNormTauDist::transformParametersForAdditionalPlot(int plot, double* params) const {
    if (plot==0) {
        params[DLSG2_A]=-1.0*fabs(params[DLSG2_A]);
        return "distribution";
    }
    return "";
}

