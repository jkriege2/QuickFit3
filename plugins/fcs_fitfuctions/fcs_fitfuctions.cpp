#include "fcs_fitfuctions.h"












QStringList QFPFitFunctionFCS::getIDs() const {
    QStringList res;
    res<<"fcs_diff"<<"fcs_diff1"<<"fcs_adiff"<<"fcs_multidiff";
    return res;
}

QFFitFunction* QFPFitFunctionFCS::get(QString id, QObject* parent) const  {
    if (id=="fcs_diff") {
        return new QFFitFunctionFCSDiff();
    } else if (id=="fcs_diff1") {
        return new QFFitFunctionFCSSimpleDiff();
    } else if (id=="fcs_adiff") {
        return new QFFitFunctionFCSADiff();
    } else if (id=="fcs_multidiff") {
        return new QFFitFunctionFCSMultiDiffusion();
    }
    return NULL;
}

Q_EXPORT_PLUGIN2(qffcsfitfuncs, QFPFitFunctionFCS)
