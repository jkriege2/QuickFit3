#include "fcs_fitfuctions.h"
#include <cmath>

QFFitFunctionFCSDiff::QFFitFunctionFCSDiff() {
    //           type,         id,                name,                                 label,                      unit,       fit,       userEditable, userRangeEditable, displayError, initialValue, minValue, maxValue, inc
    addParameter(IntCombo,     "n_components",    "number of diffusing components",     "components",               "",         false,     true,         false,             false,        1,            1,        2,        1    );
    addParameter(FloatNumber,  "n_particle",      "Particle number N",                  "N",                        "",         true,      true,         true,              true,         10,           1e-10,    1e5,      1    );
    addParameter(FloatNumber,  "1n_particle",     "1/Particle number N",                "1/N",                      "",         false,     false,        false,             true,         0.1,          1e-10,    1e5,      0.1  );
    addParameter(FloatNumber,  "diff_rho1",       "fraction of first component",        "&rho;<sub>1</sub>",        "",         false,     false,        false,             false,        0.5,          0,        1,        0.1  );
    addParameter(FloatNumber,  "diff_tau1",       "Diffusion time of first component",  "&tau;<sub>D,1</sub>",      "usec",     true,      true,         true,              true,         30,           1,        1e5,      1    );
    addParameter(FloatNumber,  "diff_rho2",       "fraction of second component",       "&rho;<sub>2</sub>",        "",         true,      true,         false,             true,         0.5,          0,        1,        0.1  );
    addParameter(FloatNumber,  "diff_tau2",       "Diffusion time of second component", "&tau;<sub>D,2</sub>",      "usec",     true,      true,         true,              true,         300,          1,        1e8,      1    );
    addParameter(FloatNumber,  "focus_struct_fac","Focus: axial ratio",                 "&gamma;<sub>Laser</sub>",  "",         true,      true,         true,              true,         6,            0.01,     100,      0.5  );
}

double QFFitFunctionFCSDiff::evaluate(double t, const double* data) const {
    int comp=data[0];
    double N=data[1];
    double rho2=data[5];
    double tauD1=data[4]/1.0e6;
    double tauD2=data[6]/1.0e6;
    double gamma2=data[7]*data[7];

    double reltau1=t/tauD1;
    double reltau2=t/tauD2;
    double d1=1.0/(1.0+reltau1)/sqrt(1.0+reltau1/gamma2);
    double d2=0;
    if (comp>1) {
        d2=rho2/(1.0+reltau2)/sqrt(1.0+reltau2/gamma2);
        d1=d1*(1.0-rho2);
    }
    return (d1+d2)/N;
}

void QFFitFunctionFCSDiff::calcParameter(double* data, double* error) const {
    int comp=data[0];
    data[2]=1.0/data[1];
    if (comp==1) {
        data[3]=1.0;
        data[5]=0;
    } else if (comp==2) {
        data[3]=1.0-data[5];
    }

    if (error) {
        error[2]=fabs(error[1]/data[1]/data[1]);
        if (comp==1) {
            error[3]=error[5]=0;
        } else {
            error[3]=error[5];
        }
    }
}

bool QFFitFunctionFCSDiff::isParameterVisible(int parameter, double* data) const {
    int comp=data[0];
    if (parameter==3) return comp>1;
    if (parameter==5) return comp>1;
    if (parameter==6) return comp>1;
    return true;
}







QFFitFunctionFCSADiff::QFFitFunctionFCSADiff() {
    //           type,         id,                name,                                 label,                      unit,       fit,       userEditable, userRangeEditable, displayError, initialValue, minValue, maxValue, inc
    addParameter(FloatNumber,  "n_particle",      "Particle number N",                  "N",                        "",         true,      true,         true,              true,         10,           1e-10,    1e5,      1    );
    addParameter(FloatNumber,  "1n_particle",     "1/Particle number N",                "1/N",                      "",         false,     false,        false,             true,         0.1,          1e-10,    1e5,      0.1  );
    addParameter(FloatNumber,  "diff_tau1",       "Diffusion time of first component",  "&tau;<sub>D,1</sub>",      "usec",     true,      true,         true,              true,         30,           1e-10,    1e5,      1    );
    addParameter(FloatNumber,  "alpha",           "Anomality parameter",                "&alpha;",                  ""    ,     true,      true,         true,              true,         1,            0.000001, 100,      0.1  );
    addParameter(FloatNumber,  "focus_struct_fac","Focus: axial ratio",                 "&gamma;<sub>Laser</sub>",  "",         true,      true,         true,              true,         6,            0.01,     100,      0.5  );
}

double QFFitFunctionFCSADiff::evaluate(double t, const double* data) const {
    double N=data[0];
    double tauD=data[2]/1.0e6;
    double gamma2=data[4]*data[4];
    double alpha=data[3];
    double reltau=t/tauD;
    return 1.0/N/(1.0+pow(reltau, alpha))/sqrt(1.0+pow(reltau/gamma2, alpha));
}

void QFFitFunctionFCSADiff::calcParameter(double* data, double* error) const {
    data[1]=1.0/data[0];
    if (error) {
        error[1]=fabs(error[0]/data[0]/data[0]);
    }
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
