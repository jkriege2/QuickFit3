#include "qfpfitfunctionsspimfcs.h"

QStringList QFPFitFunctionsSPIMFCS::getIDs() const {
    QStringList res;
    res<<"fcs_spim_diff";
    return res;
}

QFFitFunction* QFPFitFunctionsSPIMFCS::get(QString id, QObject* parent) const  {
    if (id=="fcs_spim_diff") {
        return new QFFitFunctionsSPIMFCSDiff();
    } 
    return NULL;
}

Q_EXPORT_PLUGIN2(fitfunctions_spimfcs, QFPFitFunctionsSPIMFCS)
