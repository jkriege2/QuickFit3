#include "qfpfitfunctionsdls.h"

QStringList QFPFitFunctionsDLS::getIDs() const {
    QStringList res;
    res<<"dls_g2";
    return res;
}

QFFitFunction* QFPFitFunctionsDLS::get(QString id, QObject* parent) const  {
    if (id=="dls_g2") {
        return new QFFitFunctionDLSG2();
    } 
    return NULL;
}

Q_EXPORT_PLUGIN2(fitfunctions_dls, QFPFitFunctionsDLS)
