#include "qfpfitfunctionsdls.h"
#include "qffitfunctiondlsg2ind.h"

QStringList QFPFitFunctionsDLS::getIDs() const {
    QStringList res;
    res<<"dls_g2"<<"dls_g2ind";
    return res;
}

QFFitFunction* QFPFitFunctionsDLS::get(QString id, QObject* parent) const  {
    if (id=="dls_g2") {
        return new QFFitFunctionDLSG2();
    }
    if (id=="dls_g2_ind") {
        return new QFFitFunctionDLSG2inD();
    }
    return NULL;
}

Q_EXPORT_PLUGIN2(fitfunctions_dls, QFPFitFunctionsDLS)
