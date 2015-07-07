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

#include "qffitfunctionfcsdiffDcAfterpulse.h"

#define sqr(x) ((x)*(x))
#define cube(x) ((x)*(x)*(x))
#define pow4(x) ((x)*(x)*(x)*(x))
#define pow5(x) ((x)*(x)*(x)*(x)*(x))

#define NAVOGADRO (6.02214179e23)

QFFitFunctionFCSDiffDcAfterPulse::QFFitFunctionFCSDiffDcAfterPulse(bool hasGamma) {
    QStringList afterpulsing_names;
    afterpulsing_names<<QString("none")<<QString("power law")<<QString("exponential");
    //           type,         id,                        name,                                                    label,                      unit,          unitlabel,               fit,       userEditable, userRangeEditable, displayError,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(IntCombo,     "n_nonfluorescent",        "number of nonfluorescent components (triplet ...)",     "# non-fluorescent",        "",            "",                      false,     true,         false,             QFFitFunction::NoError,      false, 1,            0,        2,        1,   0,      2);
    #define FCSDiff_n_nonfluorescent 0
    addParameter(IntCombo,     "n_components",            "number of diffusing components",                        "components",               "",            "",                      false,     true,         false,             QFFitFunction::NoError,      false, 1,            1,        3,        1,   1,      2);
    #define FCSDiff_n_components 1
    addParameter(IntCombo,     "afterpulsing_type",       "decay type of afterpulsing",                            "afterpulsing",             "",            "",                      false,     true,         false,             QFFitFunction::NoError,      false, 1,            0,        2,        1,   0,      2, afterpulsing_names);
    #define FCSDiff_afterpulsing 2
    addParameter(FloatNumber,  "afterpulsing_amplitude",  "afterpulsing amplitude",                                "a<sub>pulse</sub>",            "",        "",                          true,      true,         true,              QFFitFunction::DisplayError, false, 1.0,          0,        1e10  );
    #define FCSDiff_pre1 3
    addParameter(FloatNumber,  "afterpulsing_exponent",   "afterpulsing exponent b",                               "b<sub>pulse</sub>",            "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, true, 1.1,          -1e10,        1e10,  1);
    #define FCSDiff_exp1 4
    addParameter(FloatNumber,  "afterpulsing_tau",        "afterpulsing decay time tau",                           "&tau;<sub>after</sub>",        "us",      "&mu;s",                     true,      true,         true,              QFFitFunction::DisplayError, false, 10.0,          1e10,        1e10,  1);
    #define FCSDiff_tau1 5
    addParameter(FloatNumber,  "nonfl_tau1",              "triplet decay time",                                    "&tau;<sub>trip</sub>",     "usec",        "&mu;s",                 true,      true,         true,              QFFitFunction::DisplayError, false, 3.0,          0,        10,       0.1, 0  );
    #define FCSDiff_nonfl_tau1 6
    addParameter(FloatNumber,  "nonfl_theta1",            "triplet fraction",                                      "&theta;<sub>trip</sub>",   "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 0.2,          0,        0.99999,  0.1, 0,      1);
    #define FCSDiff_nonfl_theta1 7
    addParameter(FloatNumber,  "nonfl_tau2",              "dark component 2 decay time",                           "&tau;<sub>dark,2</sub>",   "usec",        "&mu;s",                 true,      true,         true,              QFFitFunction::DisplayError, false, 300,          1e-10,    1e5,      1,   0);
    #define FCSDiff_nonfl_tau2 8
    addParameter(FloatNumber,  "nonfl_theta2",            "dark component 2 fraction",                             "&theta;<sub>dark,2</sub>", "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 0.2,          0,        0.99999,  0.1, 0,      1);
    #define FCSDiff_nonfl_theta2 9
    addParameter(FloatNumber,  "concentration",           "particle concentration in focus",                       "C<sub>all</sub>",          "nM",         "nM",                     true,      true,         true,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define FCSDiff_concentration 10
    addParameter(FloatNumber,  "diff_rho1",               "fraction of first component",                           "&rho;<sub>1</sub>",        "",            "",                      false,     false,        false,             QFFitFunction::DisplayError, false, 0.5,          0,        0.99999,  0.1, 0,      1);
    #define FCSDiff_diff_rho1 11
    addParameter(FloatNumber,  "diff_coeff1",             "diffusion coefficient of species 1",                    "D<sub>1</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    true,      true,         true,              QFFitFunction::EditError, false, 500,          0,        1e50,     1    );
    #define FCSDiff_diff_coeff1 12
    addParameter(FloatNumber,  "diff_rho2",               "fraction of second component",                          "&rho;<sub>2</sub>",        "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 0.5,          0,        0.99999,  0.1, 0,      1  );
    #define FCSDiff_diff_rho2 13
    addParameter(FloatNumber,  "diff_coeff2",             "diffusion coefficient of species 2",                    "D<sub>2</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    true,      true,         true,              QFFitFunction::EditError, false, 500,          0,        1e50,     1    );
    #define FCSDiff_diff_coeff2 14
    addParameter(FloatNumber,  "diff_rho3",               "fraction of third component",                           "&rho;<sub>3</sub>",        "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 0.5,          0,        0.99999,  0.1, 0,      1  );
    #define FCSDiff_diff_rho3 15
    addParameter(FloatNumber,  "diff_coeff3",             "diffusion coefficient of species 3",                    "D<sub>3</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    true,      true,         true,              QFFitFunction::EditError, false, 500,          0,        1e50,     1    );
    #define FCSDiff_diff_coeff3 16
    addParameter(FloatNumber,  "offset",                  "correlation offset",                                    "G<sub>&infin;</sub>",      "",           "",                       true,      true,         true,              QFFitFunction::DisplayError, true, 0,            -10,      10,       0.1  );
    #define FCSDiff_offset 17

    this->hasGamma=hasGamma;
    if (hasGamma) {
        addParameter(FloatNumber,  "focus_struct_fac",        "focus: axial ratio",                                    "&gamma;",                  "",           "",                       true,      true,         true,              QFFitFunction::EditError,    true, 6,            0.01,     100,      0.5  );
        #define FCSDiff_focus_struct_fac 18
        addParameter(FloatNumber,  "focus_width",             "focus: lateral radius",                                 "w<sub>x,y</sub>",          "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 250,          0,        1e4,      1    );
        #define FCSDiff_focus_width 19
        addParameter(FloatNumber,  "focus_height",            "focus: longitudinal radius",                            "w<sub>z</sub>",          "nm",         "nm",                     false,      false,         false,              QFFitFunction::DisplayError,    true, 6*250,          0,        1e4,      1    );
        #define FCSDiff_focus_height 20
    } else {
        addParameter(FloatNumber,  "focus_struct_fac",        "focus: axial ratio",                                    "&gamma;",                  "",           "",                       false,      false,         false,              QFFitFunction::DisplayError,    true, 6,            0.01,     100,      0.5  );
        #define FCSDiff_focus_struct_fac 18
        addParameter(FloatNumber,  "focus_width",             "focus: lateral radius",                                 "w<sub>x,y</sub>",          "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 250,          0,        1e4,      1    );
        #define FCSDiff_focus_width 19
        addParameter(FloatNumber,  "focus_height",            "focus: longitudinal radius",                            "w<sub>z</sub>",          "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 6*250,          0,        1e4,      1    );
        #define FCSDiff_focus_height 20
    }

    addParameter(FloatNumber,  "focus_volume",            "focus: effective volume",                               "V<sub>eff</sub>",          "fl",         "fl",                     false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define FCSDiff_focus_volume 21

    addParameter(FloatNumber,  "n_particle",              "particle number N",                                     "N",                        "",            "",                      false,     false,        false,             QFFitFunction::DisplayError, false, 10,           1e-10,    1e5,      1,   0);
    #define FCSDiff_n_particle 22
    addParameter(FloatNumber,  "1n_particle",             "1/particle number N",                                   "1/N",                      "",            "",                      false,     false,        false,             QFFitFunction::DisplayError, false, 0.1,          1e-10,    1e5,      0.1, 0);
    #define FCSDiff_1n_particle 23

    addParameter(FloatNumber,  "diff_tau1",               "diffusion time of first component",                     "&tau;<sub>D,1</sub>",      "usec",        "&mu;s",                 false,    false,        false,              QFFitFunction::DisplayError, false, 30,           1,        1e5,      1,   0        );
    #define FCSDiff_diff_tau1 24
    addParameter(FloatNumber,  "diff_tau2",               "diffusion time of second component",                    "&tau;<sub>D,2</sub>",      "usec",        "&mu;s",                 false,    false,        false,              QFFitFunction::DisplayError, false, 300,          1,        1e8,      1,   0    );
    #define FCSDiff_diff_tau2 25
    addParameter(FloatNumber,  "diff_tau3",               "diffusion time of third component",                     "&tau;<sub>D,3</sub>",      "usec",        "&mu;s",                 false,    false,        false,              QFFitFunction::DisplayError, false, 300,          1,        1e8,      1    );
    #define FCSDiff_diff_tau3 26
    addParameter(FloatNumber,  "count_rate",              "count rate during measurement",                         "count rate",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError,    false, 0,            0,        1e50,     1    );
    #define FCSDiff_count_rate 27
    addParameter(FloatNumber,  "background",              "background count rate during measurement",              "background",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError  ,  false, 0,            0,        1e50,     1    );
    #define FCSDiff_background 28
    addParameter(FloatNumber,  "cpm",                     "photon counts per molecule",                            "cnt/molec",                "Hz",         "Hz",                     false,    false,        false,              QFFitFunction::DisplayError, false, 0,            0,        1e50,     1    );
    #define FCSDiff_cpm 29
    addParameter(FloatNumber,  "nodiff",                  "show diffusion",                                        "show_diffusion",                "",         "",                     false,    false,        false,              QFFitFunction::DisplayError, false, 1,            0,        1,     1    );
    #define FCSDiff_nodiff 30
}

QString QFFitFunctionFCSDiffDcAfterPulse::id() const {
    if (hasGamma) return QString("fcs_diff_d_c_afterpulse");
    return QString("fcs_diff_d_wz_c_afterpulse");
}

QString QFFitFunctionFCSDiffDcAfterPulse::name() const {
    if (hasGamma) return  QString("FCS: 3D Normal Diffusion, D, c, wxy & gamma, afterpulsing");
    return QString("FCS: 3D Normal Diffusion, D, c, wxy, wz, afterpulsing");
}
double QFFitFunctionFCSDiffDcAfterPulse::evaluate(double t, const double* data) const {
    const int comp=data[FCSDiff_n_components];
    const int nonfl_comp=data[FCSDiff_n_nonfluorescent];

    const int afterpulsing=data[FCSDiff_afterpulsing];
    const double apre=data[FCSDiff_pre1];
    const double aexp=data[FCSDiff_exp1];
    const double atau=data[FCSDiff_tau1]*1e-6;
    const double nodiff=data[FCSDiff_nodiff];

    const double c=data[FCSDiff_concentration];
    const double nf_tau1=data[FCSDiff_nonfl_tau1]/1.0e6;
    const double nf_theta1=data[FCSDiff_nonfl_theta1];
    const double nf_tau2=data[FCSDiff_nonfl_tau2]/1.0e6;
    const double nf_theta2=data[FCSDiff_nonfl_theta2];
    const double D1=data[FCSDiff_diff_coeff1]*1.0e6; // in nm/s
    const double rho2=data[FCSDiff_diff_rho2];
    const double D2=data[FCSDiff_diff_coeff2]*1.0e6;
    const double rho3=data[FCSDiff_diff_rho3];
    const double D3=data[FCSDiff_diff_coeff3]*1.0e6;
    const double wxy=data[FCSDiff_focus_width];
    const double wz=data[FCSDiff_focus_height];
    const double background=data[FCSDiff_background];
    const double cr=data[FCSDiff_count_rate];
    double backfactor=sqr(cr-background)/sqr(cr);
    if (fabs(cr)<1e-15) backfactor=1;
    double gamma=data[FCSDiff_focus_struct_fac];
    if (!hasGamma) {
        gamma=wz/wxy;
    }
    if (gamma==0) gamma=1;
    const double gamma2=sqr(gamma);
    const double wxy2=sqr(wxy);
    //qDebug()<<"OLD: wxy="<<wxy<<" ,wz="<<wz<<", gamma="<<gamma<<" ,D="<<D1;
    const double offset=data[FCSDiff_offset];

    const double pi32=sqrt(cube(M_PI));
    const double Veff=pi32*wxy2*wxy*gamma;
    const double N=c*Veff*0.6022/1e9;

    double relD1=0;
    if (D1!=0) relD1=4.0*D1*t/wxy2;
    double rho1=1.0;
    double d1=1.0/(1.0+relD1)/sqrt(1.0+relD1/gamma2);
    double d2=0;
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
    }
    double pre=1.0;
    if (nonfl_comp==1) {
        pre=(1.0-nf_theta1+nf_theta1*exp(-t/nf_tau1))/(1.0-nf_theta1);
    } else if (nonfl_comp==2) {
        pre=(1.0-nf_theta1+nf_theta1*exp(-t/nf_tau1)-nf_theta2+nf_theta2*exp(-t/nf_tau2))/(1.0-nf_theta1-nf_theta2);
    }

    double afterp=0.0;
    if (afterpulsing==1) {
        afterp=apre*pow(t, -aexp);
    } else if (afterpulsing==1) {
        afterp=apre*exp(-t/atau);
    }

    return offset+afterp+nodiff*pre/N*(rho1*d1+d2+d3)*backfactor;

}




void QFFitFunctionFCSDiffDcAfterPulse::calcParameter(double* data, double* error) const {
    int comp=data[FCSDiff_n_components];
    //int nonfl_comp=data[FCSDiff_n_nonfluorescent];
    double C=data[FCSDiff_concentration];
    double eC=0;
    //double nf_tau1=data[FCSDiff_nonfl_tau1];
    double enf_tau1=0;
    double nf_theta1=data[FCSDiff_nonfl_theta1];
    double enf_theta1=0;
    //double nf_tau2=data[FCSDiff_nonfl_tau2];
    double enf_tau2=0;
    double nf_theta2=data[FCSDiff_nonfl_theta2];
    double enf_theta2=0;
    double rho1=1.0;
    double erho1=0;
    double D1=data[FCSDiff_diff_coeff1]; // in m/s
    double eD1=0;
    double rho2=data[FCSDiff_diff_rho2];
    double erho2=0;
    double D2=data[FCSDiff_diff_coeff2];
    double eD2=0;
    double rho3=data[FCSDiff_diff_rho3];
    double erho3=0;
    double D3=data[FCSDiff_diff_coeff3];
    double eD3=0;
    double gamma=data[FCSDiff_focus_struct_fac];
    double egamma=0;
    //double gamma2=sqr(gamma);
    double wxy=data[FCSDiff_focus_width]/1.0e3; // in m
    double ewxy=0;
    double wz=data[FCSDiff_focus_height]/1.0e3; // in m
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
        eC=error[FCSDiff_concentration];
        enf_tau1=error[FCSDiff_nonfl_tau1];
        enf_theta1=error[FCSDiff_nonfl_theta1];
        enf_tau2=error[FCSDiff_nonfl_tau2];
        enf_theta2=error[FCSDiff_nonfl_theta2];
        erho1=0;
        eD1=error[FCSDiff_diff_coeff1];
        erho2=error[FCSDiff_diff_rho2];
        eD2=error[FCSDiff_diff_coeff2];
        erho3=error[FCSDiff_diff_rho3];
        eD3=error[FCSDiff_diff_coeff3];
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

    // calculate diffusion fractions
    if (comp==1) {
        rho1=1.0;
        erho1=0;
        rho2=0;
        erho2=0;
        rho3=0;
        erho3=0;
        data[FCSDiff_diff_coeff1]=D1;
        data[FCSDiff_diff_rho1]=rho1;
        data[FCSDiff_diff_coeff2]=D2;
        data[FCSDiff_diff_rho2]=rho2;
        data[FCSDiff_diff_coeff3]=D3;
        data[FCSDiff_diff_rho3]=rho3;
        if (error) {
            error[FCSDiff_diff_coeff1]=eD1;
            error[FCSDiff_diff_rho1]=erho1;
            error[FCSDiff_diff_coeff2]=eD2;
            error[FCSDiff_diff_rho2]=erho2;
            error[FCSDiff_diff_coeff3]=eD3;
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
    D1=data[FCSDiff_diff_coeff1];
    //rho2=data[FCSDiff_diff_rho2];
    D2=data[FCSDiff_diff_coeff2];
    //rho3=data[FCSDiff_diff_rho3];
    D3=data[FCSDiff_diff_coeff3];
    if (error) {
        //erho1=error[FCSDiff_diff_rho1];
        eD1=error[FCSDiff_diff_coeff1];
        //erho2=error[FCSDiff_diff_rho2];
        eD2=error[FCSDiff_diff_coeff2];
        //erho3=error[FCSDiff_diff_rho3];
        eD3=error[FCSDiff_diff_coeff3];
    }

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


    // calculate Veff = pi^(3/2) * gamma * wxy^3
    const double pi32=sqrt(cube(M_PI));
    data[FCSDiff_focus_volume]=pi32*gamma*cube(wxy);
    if (error) error[FCSDiff_focus_volume]=sqrt(sqr(egamma*pi32*cube(wxy))+sqr(ewxy*3.0*pi32*gamma*sqr(wxy)));

    // calculate N = C * Veff
    double N=0, eN=0;
    if (data[FCSDiff_focus_volume]!=0) data[FCSDiff_n_particle]=N=C*data[FCSDiff_focus_volume]*(NAVOGADRO * 1.0e-24);
    else data[FCSDiff_n_particle]=N=0;
    if (error) {
        if ((wxy!=0)&&(gamma!=0)) error[FCSDiff_n_particle]=eN=qfErrorMul(C, eC, data[FCSDiff_focus_volume], error[FCSDiff_focus_volume], (NAVOGADRO * 1.0e-24), 0.0);
        else error[FCSDiff_n_particle]=eN=0;
    }

    // calculate 1/N
    data[FCSDiff_1n_particle]=1.0/N;
    if (error) error[FCSDiff_1n_particle]=fabs(eN/N/N);

    // calculate tauD1 = wxy^2 / (4*D1)
    if (D1!=0) data[FCSDiff_diff_tau1]=sqr(wxy)/4.0/D1*1.0e6; else data[FCSDiff_diff_tau1]=0;
    if (error) {
        if (D1!=0) error[FCSDiff_diff_tau1]=sqrt( sqr(eD1*sqr(wxy)/sqr(D1)/4.0) + sqr(ewxy*2.0*wxy/D1/4.0) )*1.0e6;
        else error[FCSDiff_diff_tau1]=0;
    }

    // calculate tauD2 = wxy^2 / (4*tauD2)
    if (D2!=0) data[FCSDiff_diff_tau2]=sqr(wxy)/4.0/D2*1.0e6; else data[FCSDiff_diff_tau2]=0;
    if (error) {
        if (D2!=0) error[FCSDiff_diff_tau2]=sqrt( sqr(eD2*sqr(wxy)/sqr(D2)/4.0) + sqr(ewxy*2.0*wxy/D2/4.0) )*1.0e6;
        else error[FCSDiff_diff_tau2]=0;
    }

    // calculate tauD3 = wxy^2 / (4*D3)
    if (D3!=0) data[FCSDiff_diff_tau3]=sqr(wxy)/4.0/D3*1.0e6; else data[FCSDiff_diff_tau3]=0;
    if (error) {
        if (D3!=0) error[FCSDiff_diff_tau3]=sqrt( sqr(eD3*sqr(wxy)/sqr(D3)/4.0) + sqr(ewxy*2.0*wxy/D3/4.0) )*1.0e6;
        else error[FCSDiff_diff_tau3]=0;
    }


    // calculate CPM = (CPS-background)/N
    data[FCSDiff_cpm]=(cps-background)/N;
    error[FCSDiff_cpm]=sqrt(sqr(ecps/N)+sqr(ebackground/N)+sqr(eN*(cps-background)/sqr(N)));

 }

bool QFFitFunctionFCSDiffDcAfterPulse::isParameterVisible(int parameter, const double* data) const {
    int comp=data[FCSDiff_n_components];
    int nonfl_comp=data[FCSDiff_n_nonfluorescent];
    const int afterpulsing=data[FCSDiff_afterpulsing];
    switch(parameter) {
        case FCSDiff_nonfl_tau1: case FCSDiff_nonfl_theta1: return nonfl_comp>0;
        case FCSDiff_nonfl_tau2: case FCSDiff_nonfl_theta2: return nonfl_comp>1;
        case FCSDiff_diff_rho1:  return comp>1;
        case FCSDiff_diff_coeff1: case FCSDiff_diff_tau1: return comp>0;
        case FCSDiff_diff_rho2: case FCSDiff_diff_coeff2: case FCSDiff_diff_tau2: return comp>1;
        case FCSDiff_diff_rho3: case FCSDiff_diff_coeff3: case FCSDiff_diff_tau3: return comp>2;
        case FCSDiff_pre1: return afterpulsing>0;
        case FCSDiff_exp1: return afterpulsing==1;
        case FCSDiff_tau1: return afterpulsing==2;
        case FCSDiff_nodiff: return false;
    }

    return true;
}

unsigned int QFFitFunctionFCSDiffDcAfterPulse::getAdditionalPlotCount(const double* /*params*/) const {
    return 2;
}

QString QFFitFunctionFCSDiffDcAfterPulse::transformParametersForAdditionalPlot(int plot, double* params) const {
    if (plot==0) {
        params[FCSDiff_n_nonfluorescent]=0;
        params[FCSDiff_afterpulsing]=0;
        return "diffusion only";
    }
    if (plot==1) {
        params[FCSDiff_n_nonfluorescent]=0;
        params[FCSDiff_nodiff]=0;
        return "afterpulsing";
    }
    return QString("");
}
