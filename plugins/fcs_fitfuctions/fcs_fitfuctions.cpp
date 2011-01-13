#include "fcs_fitfuctions.h"
#include <cmath>

QFFitFunctionFCSDiff::QFFitFunctionFCSDiff() {
    //           type,         id,                name,                                 label,                         unit,       fit,       userEditable, initialValue, minValue, maxValue, inc
    addParameter(FloatNumber,  "n_particle",      "Particle number N",                  "N = ",                        "",         true,      true,         10,           1e-10,    1e5,      1    );
    addParameter(FloatNumber,  "1n_particle",     "1/Particle number N",                "1/N = ",                      "",         false,     false,        0.1,          1e-10,    1e5,      0.1  );
    addParameter(FloatNumber,  "diff_tau1",       "Diffusion time of first component",  "&tau;<sub>D,1</sub> = ",      "usec",     true,      true,         30,           1e-10,    1e5,      1    );
    addParameter(FloatNumber,  "focus_struct_fac","Focus: axial ratio",                 "&gamma;<sub>Laser</sub> = ",  "",         true,      true,         6,            0.01,     100,      0.5  );
}

double QFFitFunctionFCSDiff::evaluate(double t, const double* data) const {
    double N=data[0];
    double tauD=data[2];
    double gamma2=data[3]*data[3];

    double reltau=t/tauD;
    return 1.0/N/(1.0+reltau)/sqrt(1.0+reltau/gamma2);
}

void QFFitFunctionFCSDiff::calcParameter(double* data) const {
    data[1]=1.0/data[0];
}






QFFitFunctionFCSADiff::QFFitFunctionFCSADiff() {
    //           type,         id,                name,                                 label,                         unit,       fit,       userEditable, initialValue, minValue, maxValue, inc
    addParameter(FloatNumber,  "n_particle",      "Particle number N",                  "N = ",                        "",         true,      true,         10,           1e-10,    1e5,      1    );
    addParameter(FloatNumber,  "1n_particle",     "1/Particle number N",                "1/N = ",                      "",         false,     false,        0.1,          1e-10,    1e5,      0.1  );
    addParameter(FloatNumber,  "diff_tau1",       "Diffusion time of first component",  "&tau;<sub>D,1</sub> = ",      "usec",     true,      true,         30,           1e-10,    1e5,      1    );
    addParameter(FloatNumber,  "alpha",           "Anomality parameter",                "&alpha; = ",                  ""    ,     true,      true,         1,            0.000001, 100,      0.1  );
    addParameter(FloatNumber,  "focus_struct_fac","Focus: axial ratio",                 "&gamma;<sub>Laser</sub> = ",  "",         true,      true,         6,            0.01,     100,      0.5  );
}

double QFFitFunctionFCSADiff::evaluate(double t, const double* data) const {
    double N=data[0];
    double tauD=data[2];
    double gamma2=data[4]*data[4];
    double alpha=data[3];
    double reltau=t/tauD;
    return 1.0/N/(1.0+pow(reltau, alpha))/sqrt(1.0+pow(reltau/gamma2, alpha));
}

void QFFitFunctionFCSADiff::calcParameter(double* data) const {
    data[1]=1.0/data[0];
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
