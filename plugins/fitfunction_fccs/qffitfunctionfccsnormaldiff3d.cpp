/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

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

#include "qffitfunctionfccsnormaldiff3d.h"
#include "qftools.h"
#include <cmath>
#include <QDebug>
#define sqr(x) ((x)*(x))
#define cube(x) ((x)*(x)*(x))
#define pow4(x) ((x)*(x)*(x)*(x))
#define pow5(x) ((x)*(x)*(x)*(x)*(x))

#define NAVOGADRO (6.02214179e23)

QFFitFunctionFCCSNormalDiff3D::QFFitFunctionFCCSNormalDiff3D() {
    //           type,         id,                        name,                                                    label,                      unit,         unitlabel,               fit,       userEditable, userRangeEditable, displayError,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(IntCombo,     "n_reaction",              "number of rection components",                          "# reaction",               "",           "",                      false,     true,         false,             QFFitFunction::NoError,      false, 0,            0,        1,        1,   0,      2);
    #define FCCS3DDiff_n_reaction 0
    addParameter(FloatNumber,  "nonfl_tau1",              "triplet decay time",                                    "&tau;<sub>trip</sub>",     "usec",       "&mu;s",                 true,      true,         true,              QFFitFunction::DisplayError, false, 20,           0,        1e8,      0.1, 0  );
    #define FCCS3DDiff_nonfl_tau1 1
    addParameter(FloatNumber,  "nonfl_theta1",            "triplet fraction",                                      "&theta;<sub>trip</sub>",   "",           "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 0.2,          0,        0.99999,  0.1, 0,      1);
    #define FCCS3DDiff_nonfl_theta1 2
    addParameter(FloatNumber,  "n_particle_ab",           "particle number N(ab)",                                 "N<sub>ab</sub>",           "",           "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 5,            1e-10,    1e5,      1,   0);
    #define FCCS3DDiff_n_particle10 3
    addParameter(FloatNumber,  "appn_particle_a",         "apparent particle number from green ACF",               "N<sub>a</sub>+N<sub>ab</sub>", "",       "",                      false,     true,        false,              QFFitFunction::EditError,    false, 10,           1e-10,    1e5,      1,   0);
    #define FCCS3DDiff_appn_particle0 4
    addParameter(FloatNumber,  "n_particle_a",           "corrected particle number from green ACF",               "N<sub>a</sub>",            "",       "",                          false,     false,        false,              QFFitFunction::DisplayError,false, 5,           1e-10,    1e5,      1,   0);
    #define FCCS3DDiff_n_particle0 5
    addParameter(FloatNumber,  "appn_particle_b",         "apparent particle number from red ACF",                 "N<sub>b</sub>+N<sub>ab</sub>", "",       "",                      false,     true,        false,              QFFitFunction::EditError,    false, 10,           1e-10,    1e5,      1,   0);
    #define FCCS3DDiff_appn_particle1 6
    addParameter(FloatNumber,  "n_particle_b",           "corrected particle number from green ACF",               "N<sub>a</sub>",            "",       "",                          false,     false,        false,              QFFitFunction::DisplayError,false, 5,           1e-10,    1e5,      1,   0);
    #define FCCS3DDiff_n_particle1 7
    addParameter(FloatNumber,  "diff_coeffc",             "diffusion coefficient",                                 "D<sub>ab</sub>",           "micron^2/s", "&mu;m<sup>2</sup>/s",    true,      true,         true,              QFFitFunction::EditError,   false, 500,          0,        1e50,     1    );
    #define FCCS3DDiff_diff_coeff1 8
    addParameter(FloatNumber,  "offset",                  "correlation offset",                                    "G<sub>&infin;</sub>",      "",           "",                       true,      true,         true,              QFFitFunction::DisplayError,true, 0,            -10,      10,       0.1  );
    #define FCCS3DDiff_offset 9
    addParameter(FloatNumber,  "focus_struct_fac1",       "focus: axial ratio ch. G",                              "&gamma;<sub>g</sub>",      "",           "",                       true,      true,         true,              QFFitFunction::EditError,   true, 6,            0.01,     100,      0.5  );
    #define FCCS3DDiff_focus_struct_fac1 10
    addParameter(FloatNumber,  "focus_struct_fac2",       "focus: axial ratio ch. R",                              "&gamma;<sub>r</sub>",      "",           "",                       true,      true,         true,              QFFitFunction::EditError,   true, 6,            0.01,     100,      0.5  );
    #define FCCS3DDiff_focus_struct_fac2 11
    addParameter(FloatNumber,  "focus_width1",            "focus: lateral radius ch. G",                           "w<sub>g</sub>",            "nm",         "nm",                     false,     true,        false,              QFFitFunction::EditError,   false, 250,          0,        1e4,      1    );
    #define FCCS3DDiff_focus_width1 12
    addParameter(FloatNumber,  "focus_width2",            "focus: lateral radius ch. R",                           "w<sub>r</sub>",            "nm",         "nm",                     false,     true,        false,              QFFitFunction::EditError,   false, 250,          0,        1e4,      1    );
    #define FCCS3DDiff_focus_width2 13
    addParameter(FloatNumber,  "focus_distance",          "focus: distance between ch. G and R",                   "d<sub>rg</sub>",           "nm",         "nm",                     true,     true,         true,               QFFitFunction::EditError,   true, 0,            0,        1e5,      1    );
    #define FCCS3DDiff_distance 14
    addParameter(FloatNumber,  "crosstalk",               "crosstalk coeficient",                                  "crosstalk &kappa;",        "",           "",                       false,    true,         false,              QFFitFunction::EditError,   false, 0.05,         0,        1,        .01, 0,       1 );
    #define FCCS3DDiff_crosstalk 15
    addParameter(FloatNumber,  "count_rate1",             "count rate during measurement, ch. G",                  "count rate I<sub>G</sub>", "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError,   false, 0,            0,        1e50,     1    );
    #define FCCS3DDiff_count_rate1 16
    addParameter(FloatNumber,  "background1",             "background count rate during measurement, ch. G",       "background B<sub>G</sub>", "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError,   false, 0,            0,        1e50,     1    );
    #define FCCS3DDiff_background1 17
    addParameter(FloatNumber,  "count_rate2",             "count rate during measurement, ch. R",                  "count rate I<sub>R</sub>", "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError,   false, 0,            0,        1e50,     1    );
    #define FCCS3DDiff_count_rate2 18
    addParameter(FloatNumber,  "background2",             "background count rate during measurement, ch. R",       "background B<sub>R</sub>", "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError,   false, 0,            0,        1e50,     1    );
    #define FCCS3DDiff_background2 19
    addParameter(FloatNumber,  "focus_volume",            "focus: effective colume",                               "V<sub>eff,rg</sub>",       "fl",         "fl",                     false,    false,        false,              QFFitFunction::DisplayError,false, 0.5,          0,        1e50,     1    );
    #define FCCS3DDiff_focus_volume 20
    addParameter(FloatNumber,  "concentration",           "concentration of species ab in focus",                  "C<sub>ab</sub>",           "nM",         "nM",                     false,    false,        false,              QFFitFunction::DisplayError,false, 0.5,          0,        1e50,     1    );
    #define FCCS3DDiff_concentration 21
    addParameter(FloatNumber,  "equilibconst",            "equilibrium constant",                                  "K<sub>ab</sub>",           "1/M",       "M<sup>-1</sup>",        false,    false,        false,              QFFitFunction::DisplayError,  false, 0.5,          0,        1e50,     1    );
    #define FCCS3DDiff_equilibconst 22
}

double QFFitFunctionFCCSNormalDiff3D::evaluate(double t, const double* data) const {
    const double react=data[FCCS3DDiff_n_reaction];
    const double nf_tau1=data[FCCS3DDiff_nonfl_tau1];
    const double nf_theta1=data[FCCS3DDiff_nonfl_theta1];
    const double Nab=data[FCCS3DDiff_n_particle10];
    const double Na=data[FCCS3DDiff_appn_particle0];
    const double Nb=data[FCCS3DDiff_appn_particle1];
    const double D1=data[FCCS3DDiff_diff_coeff1];
    const double offset=data[FCCS3DDiff_offset];
    const double gamma1=data[FCCS3DDiff_focus_struct_fac1];
    const double gamma2=data[FCCS3DDiff_focus_struct_fac2];
    const double w1=data[FCCS3DDiff_focus_width1]*1e-3;
    const double w2=data[FCCS3DDiff_focus_width2]*1e-3;
    const double CR1=data[FCCS3DDiff_count_rate1];
    const double B1=data[FCCS3DDiff_background1];
    const double CR2=data[FCCS3DDiff_count_rate2];
    const double B2=data[FCCS3DDiff_background2];
    const double crosstalk=data[FCCS3DDiff_crosstalk];
    const double distance=data[FCCS3DDiff_distance]*1e-3;

    const double pi32=pow(M_PI, 3.0/2.0);
    const double pih32=pow(M_PI/2.0, 3.0/2.0);
    const double Vg=pi32*cube(w1)*gamma1;
    const double Vr=pi32*cube(w2)*gamma2;
    const double Veff=pih32*(sqr(w1)+sqr(w2))*sqrt(sqr(w1*gamma1)+sqr(w2*gamma2));

    const double eta1=(CR1-B1)/(Na);
    const double eta2=(CR2-B2-crosstalk*(CR1-B1))/(Nb);

    const double dist=exp(-2.0*sqr(distance)/(8*D1*t+sqr(w1*gamma1)+sqr(w2*gamma2)));
    const double d=Nab/(1.0+8.0*D1*t/(sqr(w1)+sqr(w2)))/sqrt(1.0+8.0*D1*t/(sqr(w1*gamma1)+sqr(w2*gamma2)));
    double brightcorr=eta1*eta2/(eta1*crosstalk*Na+eta2*Nb)/(eta1*Na);


    double pre=1.0;
    if (react==1) {
        pre=(1.0-nf_theta1+nf_theta1*exp(-t/nf_tau1))/(1.0-nf_theta1);
    }


    double r=offset+pre*brightcorr*d*dist;
    if (QFFloatIsOK(r))  return r;
    return 0;
}

void QFFitFunctionFCCSNormalDiff3D::calcParameter(double* data, double* error) const {
    const double react=data[FCCS3DDiff_n_reaction];
    const double nf_tau1=data[FCCS3DDiff_nonfl_tau1];
    const double nf_theta1=data[FCCS3DDiff_nonfl_theta1];
    const double Nab=data[FCCS3DDiff_n_particle10];
    const double Na=data[FCCS3DDiff_appn_particle0];
    const double Nb=data[FCCS3DDiff_appn_particle1];
    const double D1=data[FCCS3DDiff_diff_coeff1];
    const double offset=data[FCCS3DDiff_offset];
    const double gamma1=data[FCCS3DDiff_focus_struct_fac1];
    const double gamma2=data[FCCS3DDiff_focus_struct_fac2];
    const double w1=data[FCCS3DDiff_focus_width1]*1e-3;
    const double w2=data[FCCS3DDiff_focus_width2]*1e-3;
    const double CR1=data[FCCS3DDiff_count_rate1];
    const double B1=data[FCCS3DDiff_background1];
    const double CR2=data[FCCS3DDiff_count_rate2];
    const double B2=data[FCCS3DDiff_background2];
    const double crosstalk=data[FCCS3DDiff_crosstalk];
    const double distance=data[FCCS3DDiff_distance];

    double enf_tau1=0;
    double enf_theta1=0;
    double eNab=0;
    double eNa=0;
    double eNb=0;
    double eD1=0;
    double eoffset=0;
    double egamma1=0;
    double egamma2=0;
    double ew1=0;
    double ew2=0;
    double eCR1=0;
    double eB1=0;
    double eCR2=0;
    double eB2=0;
    double ecrosstalk=0;
    double edistance=0;

    if (error) {
        enf_tau1=error[FCCS3DDiff_nonfl_tau1];
        enf_theta1=error[FCCS3DDiff_nonfl_theta1];
        eNab=error[FCCS3DDiff_n_particle10];
        eNa=error[FCCS3DDiff_appn_particle0];
        eNb=error[FCCS3DDiff_appn_particle1];
        eD1=error[FCCS3DDiff_diff_coeff1];
        eoffset=error[FCCS3DDiff_offset];
        egamma1=error[FCCS3DDiff_focus_struct_fac1];
        egamma2=error[FCCS3DDiff_focus_struct_fac2];
        ew1=error[FCCS3DDiff_focus_width1]*1e-3;
        ew2=error[FCCS3DDiff_focus_width2]*1e-3;
        eCR1=error[FCCS3DDiff_count_rate1];
        eB1=error[FCCS3DDiff_background1];
        eCR2=error[FCCS3DDiff_count_rate2];
        eB2=error[FCCS3DDiff_background2];
        ecrosstalk=error[FCCS3DDiff_crosstalk];
        edistance=error[FCCS3DDiff_distance];
    }

    const double pi32=pow(M_PI, 3.0/2.0);
    const double pih32=pow(M_PI/2.0, 3.0/2.0);
    const double Vg=pi32*cube(w1)*gamma1;
    const double Vr=pi32*cube(w2)*gamma2;
    const double Veff=pih32*(sqr(w1)+sqr(w2))*sqrt(sqr(w1*gamma1)+sqr(w2*gamma2));

    data[FCCS3DDiff_focus_volume]=Veff;
    data[FCCS3DDiff_concentration]=Nab/Veff;

    data[FCCS3DDiff_n_particle0]=Na-Nab;
    if (error) error[FCCS3DDiff_n_particle0]=sqrt(sqr(eNa)+sqr(eNab));
    data[FCCS3DDiff_n_particle1]=Nb-Nab;
    if (error) error[FCCS3DDiff_n_particle1]=sqrt(sqr(eNb)+sqr(eNab));

    data[FCCS3DDiff_equilibconst]=Nab/(Veff)/((Na-Nab)/(Vg)*(Nb-Nab)/(Vr))*NAVOGADRO*1e-15*1e-9;
}

bool QFFitFunctionFCCSNormalDiff3D::isParameterVisible(int parameter, const double* data) const {
    int reactioncomponents=data[FCCS3DDiff_n_reaction];
    switch(parameter) {
        case FCCS3DDiff_nonfl_tau1: case FCCS3DDiff_nonfl_theta1: return reactioncomponents>0;
    }
    return true;
}

unsigned int QFFitFunctionFCCSNormalDiff3D::getAdditionalPlotCount(const double* params) {
    return 1;
}

QString QFFitFunctionFCCSNormalDiff3D::transformParametersForAdditionalPlot(int plot, double* params) {
    params[FCCS3DDiff_n_reaction]=0;
    return "without reaction";
}
