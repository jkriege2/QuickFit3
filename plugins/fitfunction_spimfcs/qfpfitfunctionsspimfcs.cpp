#include "qfpfitfunctionsspimfcs.h"

QStringList QFPFitFunctionsSPIMFCS::getIDs() const {
    QStringList res;
    res<<"fcs_spim_diff";
    res<<"fcs_spim_diffe2";
    res<<"fcs_spim_adiff";
    return res;
}

QFFitFunction* QFPFitFunctionsSPIMFCS::get(QString id, QObject* parent) const  {
    if (id=="fcs_spim_diff") {
        return new QFFitFunctionsSPIMFCSDiff();
    } else if (id=="fcs_spim_diffe2") {
            return new QFFitFunctionsSPIMFCSDiffE2();
    } else if (id=="fcs_spim_adiff") {
            return new QFFitFunctionsSPIMFCSADiff();
    }
    return NULL;
}

Q_EXPORT_PLUGIN2(fitfunctions_spimfcs, QFPFitFunctionsSPIMFCS)
