#include "qffitfunctionfcsdiff2d_relbright.h"
#include "qftools.h"
#include <QDebug>

#define sqr(x) ((x)*(x))
#define cube(x) ((x)*(x)*(x))
#define pow4(x) ((x)*(x)*(x)*(x))
#define pow5(x) ((x)*(x)*(x)*(x)*(x))

#define NAVOGADRO (6.02214179e23)

QFFitFunctionFCSDiff2DDifferentBrightness::QFFitFunctionFCSDiff2DDifferentBrightness() {
    //           type,         id,                        name,                                                    label,                      unit,          unitlabel,               fit,       userEditable, userRangeEditable, displayError,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(IntCombo,     "n_nonfluorescent",        "number of nonfluorescent components (triplet ...)",     "# non-fluorescent",        "",            "",                      false,     true,         false,             QFFitFunction::NoError,      false, 1,            0,        4,        1,   0,      4);
    #define FCSDiff_n_nonfluorescent 0
    addParameter(IntCombo,     "n_components",            "number of diffusing components",                        "components",               "",            "",                      false,     true,         false,             QFFitFunction::NoError,      false, 2,            1,        3,        1,   1,      2);
    #define FCSDiff_n_components 1
    addParameter(FloatNumber,  "nonfl_tau1",              "triplet decay time",                                    "&tau;<sub>trip</sub>",     "usec",        "&mu;s",                 true,      true,         true,              QFFitFunction::DisplayError, false, 3.0,          0,        10,       0.1, 0  );
    #define FCSDiff_nonfl_tau1 2
    addParameter(FloatNumber,  "nonfl_theta1",            "triplet fraction",                                      "&theta;<sub>trip</sub>",   "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 0.2,          0,        0.99999,  0.1, 0,      1);
    #define FCSDiff_nonfl_theta1 3
    addParameter(FloatNumber,  "nonfl_tau2",              "dark component 2 decay time",                           "&tau;<sub>dark,2</sub>",   "usec",        "&mu;s",                 true,      true,         true,              QFFitFunction::DisplayError, false, 300,          1e-10,    1e5,      1,   0);
    #define FCSDiff_nonfl_tau2 4
    addParameter(FloatNumber,  "nonfl_theta2",            "dark component 2 fraction",                             "&theta;<sub>dark,2</sub>", "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 0.2,          0,        0.99999,  0.1, 0,      1);
    #define FCSDiff_nonfl_theta2 5
    addParameter(FloatNumber,  "nonfl_tau3",              "dark component 3 decay time",                           "&tau;<sub>dark,3</sub>",   "usec",        "&mu;s",                 true,      true,         true,              QFFitFunction::DisplayError, false, 300,          1e-10,    1e5,      1,   0);
    #define FCSDiff_nonfl_tau3 6
    addParameter(FloatNumber,  "nonfl_theta3",            "dark component 3 fraction",                             "&theta;<sub>dark,3</sub>", "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 0.2,          0,        0.99999,  0.1, 0,      1);
    #define FCSDiff_nonfl_theta3 7
    addParameter(FloatNumber,  "nonfl_tau4",              "dark component 4 decay time",                           "&tau;<sub>dark,4</sub>",   "usec",        "&mu;s",                 true,      true,         true,              QFFitFunction::DisplayError, false, 300,          1e-10,    1e5,      1,   0);
    #define FCSDiff_nonfl_tau4 8
    addParameter(FloatNumber,  "nonfl_theta4",            "dark component 4 fraction",                             "&theta;<sub>dark,4</sub>", "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 0.2,          0,        0.99999,  0.1, 0,      1);
    #define FCSDiff_nonfl_theta4 9
    addParameter(FloatNumber,  "n_particle",              "particle number N",                                     "N",                        "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 10,           1e-10,    1e5,      1,   0);
    #define FCSDiff_n_particle 10
    addParameter(FloatNumber,  "1n_particle",             "1/particle number N",                                   "1/N",                      "",            "",                      false,     false,        false,             QFFitFunction::DisplayError, false, 0.1,          1e-10,    1e5,      0.1, 0);
    #define FCSDiff_1n_particle 11
    addParameter(FloatNumber,  "diff_rho1",               "fraction of first component",                           "&rho;<sub>1</sub>",        "",            "",                      false,     false,        false,             QFFitFunction::DisplayError, false, 0.5,          0,        0.99999,  0.1, 0,      1);
    #define FCSDiff_diff_rho1 12
    addParameter(FloatNumber,  "diff_tau1",               "diffusion time of first component",                     "&tau;<sub>D,1</sub>",      "usec",        "&mu;s",                 true,      true,         true,              QFFitFunction::DisplayError, false, 30,           1,        1e5,      1,   0        );
    #define FCSDiff_diff_tau1 13
    addParameter(FloatNumber,  "diff_rho2",               "fraction of second component",                          "&rho;<sub>2</sub>",        "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 0.5,          0,        0.99999,  0.1, 0,      1  );
    #define FCSDiff_diff_rho2 14
    addParameter(FloatNumber,  "rel_brightness2",         "relative brightness of second component",               "q<sub>2</sub>",            "",            "",                      true,     true,         true,               QFFitFunction::EditError,     true, 1,            0,        1e10,  0.1);
    #define FCSDiff_REL_BRIGHTNESS2 15
    addParameter(FloatNumber,  "diff_tau2",               "diffusion time of second component",                    "&tau;<sub>D,2</sub>",      "usec",        "&mu;s",                 true,      true,         true,              QFFitFunction::DisplayError, false, 300,          1,        1e8,      1,   0    );
    #define FCSDiff_diff_tau2 16
    addParameter(FloatNumber,  "diff_rho3",               "fraction of third component",                           "&rho;<sub>3</sub>",        "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 0.5,          0,        0.99999,  0.1, 0,      1  );
    #define FCSDiff_diff_rho3 17
    addParameter(FloatNumber,  "rel_brightness3",         "relative brightness of third component",                "q<sub>3</sub>",            "",            "",                      true,     true,         true,               QFFitFunction::EditError,     true, 1,            0,        1e10,  0.1);
    #define FCSDiff_REL_BRIGHTNESS3 18
    addParameter(FloatNumber,  "diff_tau3",               "diffusion time of third component",                     "&tau;<sub>D,3</sub>",      "usec",        "&mu;s",                 true,      true,         true,              QFFitFunction::DisplayError, false, 300,          1,        1e8,      1    );
    #define FCSDiff_diff_tau3 19
    addParameter(FloatNumber,  "offset",                  "correlation offset",                                    "G<sub>&infin;</sub>",      "",           "",                       true,      true,         true,              QFFitFunction::DisplayError, true, 0,            -10,      10,       0.1  );
    #define FCSDiff_offset 20
    addParameter(FloatNumber,  "focus_struct_fac",        "focus: axial ratio",                                    "&gamma;",                  "",           "",                       true,      true,         true,              QFFitFunction::EditError,    true, 6,            0.01,     100,      0.5  );
    #define FCSDiff_focus_struct_fac 21
    addParameter(FloatNumber,  "focus_width",             "focus: lateral radius",                                 "w<sub>x,y</sub>",          "nm",         "nm",                     false,     true,        false,              QFFitFunction::EditError,    false, 250,          0,        1e4,      1    );
    #define FCSDiff_focus_width 22
    addParameter(FloatNumber,  "focus_area",            "focus: effective area",                               "A<sub>eff</sub>",          "micron^2",         "&mu;m<sup>2</sup>",                     false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define FCSDiff_focus_volume 23
    addParameter(FloatNumber,  "diff_coeff1",             "diffusion coefficient of species 1",                    "D<sub>1</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    false,    false,        false,              QFFitFunction::DisplayError, false, 500,          0,        1e50,     1    );
    #define FCSDiff_diff_coeff1 24
    addParameter(FloatNumber,  "diff_coeff2",             "diffusion coefficient of species 2",                    "D<sub>2</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    false,    false,        false,              QFFitFunction::DisplayError, false, 500,          0,        1e50,     1    );
    #define FCSDiff_diff_coeff2 25
    addParameter(FloatNumber,  "diff_coeff3",             "diffusion coefficient of species 3",                    "D<sub>3</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    false,    false,        false,              QFFitFunction::DisplayError, false, 500,          0,        1e50,     1    );
    #define FCSDiff_diff_coeff3 26
    addParameter(FloatNumber,  "count_rate",              "count rate during measurement",                         "count rate",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError,    false, 0,            0,        1e50,     1    );
    #define FCSDiff_count_rate 27
    addParameter(FloatNumber,  "background",              "background count rate during measurement",              "background",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError  ,  false, 0,            0,        1e50,     1    );
    #define FCSDiff_background 28
    addParameter(FloatNumber,  "cpm",                     "photon counts per molecule",                            "cnt/molec",                "Hz",         "Hz",                     false,    false,        false,              QFFitFunction::DisplayError, false, 0,            0,        1e50,     1    );
    #define FCSDiff_cpm 30

}

double QFFitFunctionFCSDiff2DDifferentBrightness::evaluate(double t, const double* data) const {
    const int comp=data[FCSDiff_n_components];
    const int nonfl_comp=data[FCSDiff_n_nonfluorescent];
    const double N=data[FCSDiff_n_particle];
    const double nf_tau1=data[FCSDiff_nonfl_tau1]/1.0e6;
    const double nf_theta1=data[FCSDiff_nonfl_theta1];
    const double nf_tau2=data[FCSDiff_nonfl_tau2]/1.0e6;
    const double nf_theta2=data[FCSDiff_nonfl_theta2];
    const double nf_tau3=data[FCSDiff_nonfl_tau3]/1.0e6;
    const double nf_theta3=data[FCSDiff_nonfl_theta3];
    const double nf_tau4=data[FCSDiff_nonfl_tau4]/1.0e6;
    const double nf_theta4=data[FCSDiff_nonfl_theta4];
    const double tauD1=data[FCSDiff_diff_tau1]/1.0e6;
    const double rho2=data[FCSDiff_diff_rho2];
    const double q2=data[FCSDiff_REL_BRIGHTNESS2];
    const double tauD2=data[FCSDiff_diff_tau2]/1.0e6;
    const double rho3=data[FCSDiff_diff_rho3];
    const double tauD3=data[FCSDiff_diff_tau3]/1.0e6;
    const double background=data[FCSDiff_background];
    const double q3=data[FCSDiff_REL_BRIGHTNESS3];
    const double cr=data[FCSDiff_count_rate];
    double backfactor=sqr(cr-background)/sqr(cr);
    if (fabs(cr)<1e-15) backfactor=1;


    const double offset=data[FCSDiff_offset];

    double reltau1=0;
    if (tauD1!=0) reltau1=t/tauD1;
    double rho1=1.0;
    double d1=1.0/(1.0+reltau1);
    double d2=0;
    double d3=0;
    double rhonorm=rho1;
    if (comp>1) {
        double reltau2=0;
        if (tauD2!=0) reltau2=t/tauD2;
        d2=rho2*q2*q2/(1.0+reltau2);
        rho1=1.0-rho2;
        rhonorm=rho1+rho2*q2;
    }
    if (comp>2) {
        double reltau3=0;
        if (tauD3!=0) reltau3=t/tauD3;
        d3=rho3*q3*q3/(1.0+reltau3);
        rho1=1.0-rho2-rho3;
        rhonorm=rho1+rho2*q2+rho3*q3;
    }
    double pre=1.0;
    if (nonfl_comp==1) {
        pre=(1.0-nf_theta1+nf_theta1*exp(-t/nf_tau1))/(1.0-nf_theta1);
    } else if (nonfl_comp==2) {
        pre=(1.0-nf_theta1+nf_theta1*exp(-t/nf_tau1)-nf_theta2+nf_theta2*exp(-t/nf_tau2))/(1.0-nf_theta1-nf_theta2);
    } else if (nonfl_comp==3) {
        pre=(1.0-nf_theta1+nf_theta1*exp(-t/nf_tau1)-nf_theta2+nf_theta2*exp(-t/nf_tau2)-nf_theta3+nf_theta3*exp(-t/nf_tau3))/(1.0-nf_theta1-nf_theta2-nf_theta3);
    } else if (nonfl_comp==4) {
        pre=(1.0-nf_theta1+nf_theta1*exp(-t/nf_tau1)-nf_theta2+nf_theta2*exp(-t/nf_tau2)-nf_theta3+nf_theta3*exp(-t/nf_tau3)-nf_theta4+nf_theta4*exp(-t/nf_tau4))/(1.0-nf_theta1-nf_theta2-nf_theta3-nf_theta4);
    }
    return offset+pre/(N*qfSqr(rhonorm))*(rho1*d1+d2+d3)*backfactor;
}




void QFFitFunctionFCSDiff2DDifferentBrightness::calcParameter(double* data, double* error) const {
    int comp=data[FCSDiff_n_components];
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
    //double gamma2=sqr(gamma);
    double wxy=data[FCSDiff_focus_width]/1.0e3;
    double ewxy=0;
    //double offset=data[FCSDiff_offset];
    double eoffset=0;
    double cps=data[FCSDiff_count_rate];
    double ecps=0;
    double background=data[FCSDiff_background];
    double ebackground=0;
    //double cpm=data[FCSDiff_cpm];
    double ecpm=0;
    double q2=data[FCSDiff_REL_BRIGHTNESS2];
    double q3=data[FCSDiff_REL_BRIGHTNESS3];
    double eq2=0;
    double eq3=0;

    if (error) {
        eq2=error[FCSDiff_REL_BRIGHTNESS2];
        eq3=error[FCSDiff_REL_BRIGHTNESS3];
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

    // calculate Aeff = pi * wxy^2
    data[FCSDiff_focus_volume]=M_PI*sqr(wxy);
    if (error) error[FCSDiff_focus_volume]=sqrt(sqr(ewxy*2.0*M_PI*wxy));



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


    // calculate CPM = (CPS-background)/N
    data[FCSDiff_cpm]=(cps-background)/N;
    error[FCSDiff_cpm]=sqrt(sqr(ecps/N)+sqr(ebackground/N)+sqr(eN*(cps-background)/sqr(N)));
 }

bool QFFitFunctionFCSDiff2DDifferentBrightness::isParameterVisible(int parameter, const double* data) const {
    int comp=data[FCSDiff_n_components];
    int nonfl_comp=data[FCSDiff_n_nonfluorescent];
    switch(parameter) {
        case FCSDiff_nonfl_tau1: case FCSDiff_nonfl_theta1: return nonfl_comp>0;
        case FCSDiff_nonfl_tau2: case FCSDiff_nonfl_theta2: return nonfl_comp>1;
        case FCSDiff_nonfl_tau3: case FCSDiff_nonfl_theta3: return nonfl_comp>2;
        case FCSDiff_nonfl_tau4: case FCSDiff_nonfl_theta4: return nonfl_comp>3;
        case FCSDiff_diff_rho1:  return comp>1;
        case FCSDiff_diff_tau1: case FCSDiff_diff_coeff1: return comp>0;
        case FCSDiff_diff_rho2: case FCSDiff_diff_tau2: case FCSDiff_diff_coeff2: case FCSDiff_REL_BRIGHTNESS2: return comp>1;
        case FCSDiff_diff_rho3: case FCSDiff_diff_tau3: case FCSDiff_diff_coeff3: case FCSDiff_REL_BRIGHTNESS3: return comp>2;
    }
    return true;
}

unsigned int QFFitFunctionFCSDiff2DDifferentBrightness::getAdditionalPlotCount(const double* params) {
    return 1;
}

QString QFFitFunctionFCSDiff2DDifferentBrightness::transformParametersForAdditionalPlot(int plot, double* params) {
    //double* dat=duplicateArray(params, paramCount());
    if (plot==0) {
        params[FCSDiff_n_nonfluorescent]=0;
        return "without non-fluorescent";
    }
    /*for (int i=0; i<paramCount(); i++) {
        qDebug()<<dat[i]<<" -> "<<params[i];
    }
    free(dat);*/
    return "";
}
