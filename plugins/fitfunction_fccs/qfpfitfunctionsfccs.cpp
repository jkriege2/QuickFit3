#include "qfpfitfunctionsfccs.h"

QStringList QFPFitFunctionsFCCS::getIDs() const {
    QStringList res;
    res<<"fccs_diff3d";
    return res;
}

QFFitFunction* QFPFitFunctionsFCCS::get(QString id, QObject* parent) const  {
    if (id=="fccs_diff3d") {
        return new QFFitFunctionFCCSNormalDiff3D();
    }
    return NULL;
}

Q_EXPORT_PLUGIN2(fitfunctions_fccs, QFPFitFunctionsFCCS)
