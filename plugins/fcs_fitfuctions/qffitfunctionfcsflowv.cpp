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

#include "qffitfunctionfcsflowv.h"


#define sqr(x) ((x)*(x))
#define cube(x) ((x)*(x)*(x))
#define pow4(x) ((x)*(x)*(x)*(x))
#define pow5(x) ((x)*(x)*(x)*(x)*(x))

#define NAVOGADRO (6.02214179e23)






QFFitFunctionFCSFlowV::QFFitFunctionFCSFlowV() {
    //           type,         id,                        name,                                                    label,                      unit,          unitlabel,               fit,       userEditable, userRangeEditable, displayError,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(FloatNumber,  "nonfl_tau1",              "triplet decay time",                                    "&tau;<sub>trip</sub>",     "usec",        "&mu;s",                 true,      true,         true,              QFFitFunction::DisplayError, false, 3.0,          0,        10,       0.1, 0.1  );
    #define FCSSDiff_nonfl_tau1 0
    addParameter(FloatNumber,  "nonfl_theta1",            "triplet fraction",                                      "&theta;<sub>trip</sub>",   "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 0.2,          0,        0.99999,  0.1, 0,      1);
    #define FCSSDiff_nonfl_theta1 1
    addParameter(FloatNumber,  "n_particle",              "particle number N",                                     "N",                        "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 10,           1e-10,    1e5,      1,   0);
    #define FCSSDiff_n_particle 2
    addParameter(FloatNumber,  "1n_particle",             "1/particle number N",                                   "1/N",                      "",            "",                      false,     false,        false,             QFFitFunction::DisplayError, false, 0.1,          1e-10,    1e5,      0.1, 0);
    #define FCSSDiff_1n_particle 3
    addParameter(FloatNumber,  "diff_coeff1",             "diffusion coefficient of species 1",                    "D<sub>1</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    true,      true,         true,              QFFitFunction::EditError, false, 500,          0,        1e50,     1    );
    #define FCSDiff_diff_coeff1 4
    addParameter(FloatNumber,  "vflow",                   "flow speed of species 1",                               "v<sub>flow</sub>",         "micron/s", "&mu;m/s",                  true,    true,        true,              QFFitFunction::EditError, false, 500,          0,        1e50,     1    );
    #define FCSSDiff_vflow 5
    addParameter(FloatNumber,  "offset",                  "correlation offset",                                    "G<sub>&infin;</sub>",      "",           "",                       true,      true,         true,              QFFitFunction::DisplayError, true, 0,            -10,      10,       0.1  );
    #define FCSSDiff_offset 6
    addParameter(FloatNumber,  "focus_struct_fac",        "focus: axial ratio",                                    "&gamma;",                  "",           "",                       true,      true,         true,              QFFitFunction::EditError,    true, 6,            0.01,     100,      0.5  );
    #define FCSSDiff_focus_struct_fac 7
    addParameter(FloatNumber,  "focus_width",             "focus: lateral radius",                                 "w<sub>x,y</sub>",          "nm",         "nm",                     true,     true,        true,              QFFitFunction::EditError,    false, 250,          0,        1e4,      1    );
    #define FCSSDiff_focus_width 8
    addParameter(FloatNumber,  "focus_volume",            "focus: effective colume",                               "V<sub>eff</sub>",          "fl",         "fl",                     false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define FCSSDiff_focus_volume 9
    addParameter(FloatNumber,  "concentration",           "particle concentration in focus",                       "C<sub>all</sub>",          "nM",         "nM",                     false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define FCSSDiff_concentration 10
    addParameter(FloatNumber,  "diff_coeff1",             "diffusion coefficient of species 1",                    "D<sub>1</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    false,    false,        false,              QFFitFunction::DisplayError, false, 500,          0,        1e50,     1    );
    #define FCSSDiff_diff_coeff1 11
    addParameter(FloatNumber,  "count_rate",              "count rate during measurement",                         "count rate",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError,    false, 0,            0,        1e50,     1    );
    #define FCSSDiff_count_rate 12
    addParameter(FloatNumber,  "background",              "background count rate during measurement",              "background",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError,    false, 0,            0,        1e50,     1    );
    #define FCSSDiff_background 13
    addParameter(FloatNumber,  "cpm",                     "photon counts per molecule",                            "cnt/molec",                "Hz",         "Hz",                     false,    false,        false,              QFFitFunction::DisplayError, false, 0,            0,        1e50,     1    );
    #define FCSSDiff_cpm 14

}

double QFFitFunctionFCSFlowV::evaluate(double t, const double* data) const {
    const double N=data[FCSSDiff_n_particle];
    const double nf_tau1=data[FCSSDiff_nonfl_tau1]/1.0e6;
    const double nf_theta1=data[FCSSDiff_nonfl_theta1];
    const double D=data[FCSDiff_diff_coeff1];
    const double v=data[FCSSDiff_vflow];
    const double wxy=data[FCSSDiff_focus_width]*1e-3;
    const double background=data[FCSSDiff_background];
    const double cr=data[FCSSDiff_count_rate];
    double backfactor=sqr(cr-background)/sqr(cr);
    if (fabs(cr)<1e-15) backfactor=1;
    double gamma=data[FCSSDiff_focus_struct_fac];
    if (gamma==0) gamma=1;
    const double gamma2=sqr(gamma);

    const double offset=data[FCSSDiff_offset];

    double reltau1=0;
    if (D!=0) reltau1=4.0*D*t/wxy/wxy;
    double reltauf=0;
    if (v!=0) reltauf=v*t/wxy;
    double d1=1.0/(1.0+reltau1)/sqrt(1.0+reltau1/gamma2);
    double flow=exp(-sqr(reltauf)/(1.0+reltau1));
    double pre=(1.0-nf_theta1+nf_theta1*exp(-t/nf_tau1))/(1.0-nf_theta1);
    return offset+pre/N*d1*flow*backfactor;
}





void QFFitFunctionFCSFlowV::calcParameter(double* data, double* error) const {
    double N=data[FCSSDiff_n_particle];
    double eN=0;
    //double nf_tau1=data[FCSSDiff_nonfl_tau1]/1.0e6;
    double enf_tau1=0;
    double nf_theta1=data[FCSSDiff_nonfl_theta1];
    //double enf_theta1=0;
    double gamma=data[FCSSDiff_focus_struct_fac];
    double egamma=0;
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
        egamma=error[FCSSDiff_focus_struct_fac];
        ewxy=error[FCSSDiff_focus_width]/1.0e3;
        eoffset=error[FCSSDiff_offset];
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




    // calculate CPM = (CPS-background)/N
    data[FCSSDiff_cpm]=(cps-background)/N;
    error[FCSSDiff_cpm]=sqrt(sqr(ecps/N)+sqr(ebackground/N)+sqr(eN*(cps-background)/sqr(N)));

}

bool QFFitFunctionFCSFlowV::isParameterVisible(int parameter, const double* data) const {
    return true;
}

unsigned int QFFitFunctionFCSFlowV::getAdditionalPlotCount(const double* params) {
    return 2;
}

QString QFFitFunctionFCSFlowV::transformParametersForAdditionalPlot(int plot, double* params) {
    if (plot==0) {
        params[FCSSDiff_nonfl_theta1]=0;
        return "without non-fluorescent";
    } else if (plot==1) {
        params[FCSSDiff_nonfl_theta1]=0;
        params[FCSSDiff_vflow]=0;
        return "without non-fl. + flow";
    }
    return "";
}
