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

#include "qffitfunctioncsfcs.h"


#define sqr(x) ((x)*(x))
#define cube(x) ((x)*(x)*(x))
#define pow4(x) ((x)*(x)*(x)*(x))
#define pow5(x) ((x)*(x)*(x)*(x)*(x))

#define NAVOGADRO (6.02214179e23)

QFFitFunctionCSFCSDiffD::QFFitFunctionCSFCSDiffD(bool hasGamma) {
    //           type,         id,                        name,                                                    label,                      unit,          unitlabel,               fit,       userEditable, userRangeEditable, displayError,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(IntCombo,     "n_nonfluorescent",        "number of nonfluorescent components (triplet ...)",     "# non-fluorescent",        "",            "",                      false,     true,         false,             QFFitFunction::NoError,      false, 0,            0,        2,        1,   0,      2);
    #define FCSDiff_n_nonfluorescent 0
    addParameter(FloatNumber,  "nonfl_tau1",              "triplet decay time",                                    "&tau;<sub>trip</sub>",     "usec",        "&mu;s",                 true,      true,         true,              QFFitFunction::DisplayError, false, 3.0,          0,        10,       0.1, 0  );
    #define FCSDiff_nonfl_tau1 1
    addParameter(FloatNumber,  "nonfl_theta1",            "triplet fraction",                                      "&theta;<sub>trip</sub>",   "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 0.2,          0,        0.99999,  0.1, 0,      1);
    #define FCSDiff_nonfl_theta1 2
    addParameter(FloatNumber,  "nonfl_tau2",              "dark component 2 decay time",                           "&tau;<sub>dark,2</sub>",   "usec",        "&mu;s",                 true,      true,         true,              QFFitFunction::DisplayError, false, 300,          1e-10,    1e5,      1,   0);
    #define FCSDiff_nonfl_tau2 3
    addParameter(FloatNumber,  "nonfl_theta2",            "dark component 2 fraction",                             "&theta;<sub>dark,2</sub>", "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 0.2,          0,        0.99999,  0.1, 0,      1);
    #define FCSDiff_nonfl_theta2 4
    addParameter(FloatNumber,  "n_particle",              "particle number N",                                     "N",                        "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 10,           1e-10,    1e5,      1,   0);
    #define FCSDiff_n_particle 5
    addParameter(FloatNumber,  "1n_particle",             "1/particle number N",                                   "1/N",                      "",            "",                      false,     false,        false,             QFFitFunction::DisplayError, false, 0.1,          1e-10,    1e5,      0.1, 0);
    #define FCSDiff_1n_particle 6
    addParameter(FloatNumber,  "diff_coeff1",             "diffusion coefficient of species 1",                    "D<sub>1</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    true,      true,         true,              QFFitFunction::EditError, false, 500,          0,        1e50,     1    );
    #define FCSDiff_diff_coeff1 7
    addParameter(FloatNumber,  "offset",                  "correlation offset",                                    "G<sub>&infin;</sub>",      "",           "",                       true,      true,         true,              QFFitFunction::DisplayError, true, 0,            -10,      10,       0.1  );
    #define FCSDiff_offset 8

    addParameter(FloatNumber,  "circle_radius",                  "circle radius",                                    "R",      "micron",           "&mu;m",                       true,      true,         true,              QFFitFunction::EditError, true, 0.5,            0,      100000,       0.1  );
    #define FCSDiff_circleradius 9

    addParameter(FloatNumber,  "scanning_frequency",                  "scanning frequency",                                    "f",      "Hz",           "Hz",                       true,      true,         true,              QFFitFunction::EditError, true, 1000,            0,      1000000,       0.1  );
    #define FCSDiff_scanningfrequency 10

    this->hasGamma=hasGamma;
    if (hasGamma) {
        addParameter(FloatNumber,  "focus_struct_fac",        "focus: axial ratio",                                    "&gamma;",                  "",           "",                       true,      true,         true,              QFFitFunction::EditError,    true, 6,            0.01,     100,      0.5  );
        #define FCSDiff_focus_struct_fac 11
        addParameter(FloatNumber,  "focus_width",             "focus: lateral radius",                                 "w<sub>x,y</sub>",          "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    false, 250,          0,        1e4,      1    );
        #define FCSDiff_focus_width 12
        addParameter(FloatNumber,  "focus_height",            "focus: longitudinal radius",                            "w<sub>z</sub>",          "nm",         "nm",                     false,      false,         false,              QFFitFunction::DisplayError,    false, 6*250,          0,        1e4,      1    );
        #define FCSDiff_focus_height 13
    } else {
        addParameter(FloatNumber,  "focus_struct_fac",        "focus: axial ratio",                                    "&gamma;",                  "",           "",                       false,      false,         false,              QFFitFunction::DisplayError,    true, 6,            0.01,     100,      0.5  );
        #define FCSDiff_focus_struct_fac 11
        addParameter(FloatNumber,  "focus_width",             "focus: lateral radius",                                 "w<sub>x,y</sub>",          "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    false, 250,          0,        1e4,      1    );
        #define FCSDiff_focus_width 12
        addParameter(FloatNumber,  "focus_height",            "focus: longitudinal radius",                            "w<sub>z</sub>",          "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    false, 6*250,          0,        1e4,      1    );
        #define FCSDiff_focus_height 13
    }

    addParameter(FloatNumber,  "focus_volume",            "focus: effective volume",                               "V<sub>eff</sub>",          "fl",         "fl",                     false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define FCSDiff_focus_volume 14
    addParameter(FloatNumber,  "concentration",           "particle concentration in focus",                       "C<sub>all</sub>",          "nM",         "nM",                     false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define FCSDiff_concentration 15
    addParameter(FloatNumber,  "diff_tau1",               "diffusion time of first component",                     "&tau;<sub>D,1</sub>",      "usec",        "&mu;s",                 false,    false,        false,              QFFitFunction::DisplayError, false, 30,           1,        1e5,      1,   0        );
    #define FCSDiff_diff_tau1 16
    addParameter(FloatNumber,  "count_rate",              "count rate during measurement",                         "count rate",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError,    false, 0,            0,        1e50,     1    );
    #define FCSDiff_count_rate 17
    addParameter(FloatNumber,  "background",              "background count rate during measurement",              "background",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError  ,  false, 0,            0,        1e50,     1    );
    #define FCSDiff_background 19
    addParameter(FloatNumber,  "cpm",                     "photon counts per molecule",                            "cnt/molec",                "Hz",         "Hz",                     false,    false,        false,              QFFitFunction::DisplayError, false, 0,            0,        1e50,     1    );
    #define FCSDiff_cpm 19
}

QString QFFitFunctionCSFCSDiffD::id() const {
    if (hasGamma) return QString("fcs_cs_diff_d");
    return QString("fcs_diff_cs_d_wz");
}

QString QFFitFunctionCSFCSDiffD::name() const {
    if (hasGamma) return  QString("csFCS: 3D Normal Diffusion, D and gamma as parameter, circle-scanning");
    return QString("csFCS: 3D Normal Diffusion, D and wz as parameter, circle-scanning");
}
double QFFitFunctionCSFCSDiffD::evaluate(double t, const double* data) const {
    const int comp=1;//data[FCSDiff_n_components];
    const int nonfl_comp=data[FCSDiff_n_nonfluorescent];
    const double N=data[FCSDiff_n_particle];
    const double nf_tau1=data[FCSDiff_nonfl_tau1]/1.0e6;
    const double nf_theta1=data[FCSDiff_nonfl_theta1];
    const double nf_tau2=data[FCSDiff_nonfl_tau2]/1.0e6;
    const double nf_theta2=data[FCSDiff_nonfl_theta2];
    const double D1=data[FCSDiff_diff_coeff1]*1.0e6; // in nm^2/s
    //const double rho2=data[FCSDiff_diff_rho2];
    //const double D2=data[FCSDiff_diff_coeff2]*1.0e6;
    //const double rho3=data[FCSDiff_diff_rho3];
    //const double D3=data[FCSDiff_diff_coeff3]*1.0e6;
    const double wxy=data[FCSDiff_focus_width];
    const double wz=data[FCSDiff_focus_height];
    const double background=data[FCSDiff_background];
    const double cr=data[FCSDiff_count_rate];

    const double r=data[FCSDiff_circleradius]*1000.0; //nm
    const double f=data[FCSDiff_scanningfrequency];

    double backfactor=sqr(cr-background)/sqr(cr);
    if (fabs(cr)<1e-15) backfactor=1;
    double gamma=data[FCSDiff_focus_struct_fac];
    if (!hasGamma) {
        gamma=wz/wxy;
    }
    if (gamma==0) gamma=1;
    const double gamma2=sqr(gamma);
    const double wxy2=sqr(wxy);

    const double offset=data[FCSDiff_offset];

    double relD1=0;
    if (D1!=0) relD1=4.0*D1*t/wxy2;
    //double rho1=1.0;
    double d1=1.0/(1.0+relD1)/sqrt(1.0+relD1/gamma2);
    /*double d2=0;
    double d3=0;
    if (comp>1) {
        double relD2=0;
        if (D2!=0) relD2=4.0*D2*t/wxy2;
        d2=rho2/(1.0+relD2)/sqrt(1.0+relD2/gamma2);
        rho1=1.0-rho2;
    }
    if (comp>2) {
        double relD3=0;
        if (D3!=0) relD3=4.0*D3*t/wxy2;
        d3=rho3/(1.0+relD3)/sqrt(1.0+relD3/gamma2);
        rho1=1.0-rho2-rho3;
    }*/
    double pre=1.0;
    if (nonfl_comp==1) {
        pre=(1.0-nf_theta1+nf_theta1*exp(-t/nf_tau1))/(1.0-nf_theta1);
    } else if (nonfl_comp==2) {
        pre=(1.0-nf_theta1+nf_theta1*exp(-t/nf_tau1)-nf_theta2+nf_theta2*exp(-t/nf_tau2))/(1.0-nf_theta1-nf_theta2);
    }
	
	double sfactor=1;
	if (r>0.0 && f>0.0) {
        sfactor=exp(-1.0*sqr(r*sin(M_PI*f*t))/(sqr(wxy/2.0)+D1*t));
	}
    //return offset+pre/N*(rho1*d1+d2+d3)*backfactor*sfactor;
	return offset+pre/N*d1*backfactor*sfactor;
}




void QFFitFunctionCSFCSDiffD::calcParameter(double* data, double* error) const {
    //int nonfl_comp=data[FCSDiff_n_nonfluorescent];
    double N=data[FCSDiff_n_particle];
    double eN=0;
    //double nf_tau1=data[FCSDiff_nonfl_tau1];
    double enf_tau1=0;
    double nf_theta1=data[FCSDiff_nonfl_theta1];
    double enf_theta1=0;
    //double nf_tau2=data[FCSDiff_nonfl_tau2];
    double enf_tau2=0;
    double nf_theta2=data[FCSDiff_nonfl_theta2];
    double enf_theta2=0;

    const double D1=data[FCSDiff_diff_coeff1]*1.0e6; // in nm^2/s
    double eD1=0;

    double gamma=data[FCSDiff_focus_struct_fac];
    double egamma=0;
    //double gamma2=sqr(gamma);
    double wxy=data[FCSDiff_focus_width]/1.0e3; // in µm
    double ewxy=0;
    double wz=data[FCSDiff_focus_height]/1.0e3; // in µm
    double ewz=0;
    //double offset=data[FCSDiff_offset];
    double eoffset=0;
    double cps=data[FCSDiff_count_rate];
    double ecps=0;
    double background=data[FCSDiff_background];
    double ebackground=0;
    //double cpm=data[FCSDiff_cpm];
    double ecpm=0;

    if (error) {
        eN=error[FCSDiff_n_particle];
        enf_tau1=error[FCSDiff_nonfl_tau1];
        enf_theta1=error[FCSDiff_nonfl_theta1];
        enf_tau2=error[FCSDiff_nonfl_tau2];
        enf_theta2=error[FCSDiff_nonfl_theta2];
        eD1=error[FCSDiff_diff_coeff1]*1.0e6; // in nm^2/s

        egamma=error[FCSDiff_focus_struct_fac];
        ewxy=error[FCSDiff_focus_width]/1.0e3;
        ewz=error[FCSDiff_focus_height]/1.0e3;
        eoffset=error[FCSDiff_offset];
        ecps=error[FCSDiff_count_rate];
        ecpm=error[FCSDiff_cpm];
        ebackground=error[FCSDiff_background];
    }

    // correct for invalid fractions
    if (nf_theta1>1.0) nf_theta1=1.0;
    if (nf_theta2>1.0) nf_theta2=1.0;
    if (nf_theta1+nf_theta2>1.0) {
        nf_theta2=1.0-nf_theta1;
    }
    data[FCSDiff_nonfl_theta1]=nf_theta1;
    data[FCSDiff_nonfl_theta2]=nf_theta2;

    

    if (hasGamma) {
        wz=gamma*wxy;
        ewz=sqrt(sqr(ewxy*gamma)+sqr(egamma*wxy));
        data[FCSDiff_focus_height]=wz*1000.0;
        if (error) error[FCSDiff_focus_height]=ewz*1000.0;
    } else {
        gamma=wz/wxy;
        egamma=sqrt(sqr(ewz/wxy)+sqr(wz*ewxy/sqr(wxy)));
        data[FCSDiff_focus_struct_fac]=gamma;
        if (error) error[FCSDiff_focus_struct_fac]=egamma;
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

    // calculate tauD1 = wxy^2 / (4*D1)
    if (D1!=0) data[FCSDiff_diff_tau1]=sqr(wxy)/4.0/D1*1.0e6; else data[FCSDiff_diff_tau1]=0;
    if (error) {
        if (D1!=0) error[FCSDiff_diff_tau1]=sqrt( sqr(eD1*sqr(wxy)/sqr(D1)/4.0) + sqr(ewxy*2.0*wxy/D1/4.0) )*1.0e6;
        else error[FCSDiff_diff_tau1]=0;
    }


    // calculate CPM = (CPS-background)/N
    data[FCSDiff_cpm]=(cps-background)/N;
    error[FCSDiff_cpm]=sqrt(sqr(ecps/N)+sqr(ebackground/N)+sqr(eN*(cps-background)/sqr(N)));
 }

bool QFFitFunctionCSFCSDiffD::isParameterVisible(int parameter, const double* data) const {
    int nonfl_comp=data[FCSDiff_n_nonfluorescent];
    switch(parameter) {
        case FCSDiff_nonfl_tau1: case FCSDiff_nonfl_theta1: return nonfl_comp>0;
        case FCSDiff_nonfl_tau2: case FCSDiff_nonfl_theta2: return nonfl_comp>1;
    }
    return true;
}

unsigned int QFFitFunctionCSFCSDiffD::getAdditionalPlotCount(const double* /*params*/) {
    return 2;
}

QString QFFitFunctionCSFCSDiffD::transformParametersForAdditionalPlot(int plot, double* params) {
	if (plot==0) {
		params[FCSDiff_n_nonfluorescent]=0;
		return "without non-fluorescent";
	} else if (plot==1) {
		params[FCSDiff_circleradius]=0;
		params[FCSDiff_scanningfrequency]=0;
		return "non-scanning (evelope)";
	}
	return QString();
}
