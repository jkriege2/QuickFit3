#include "qfpfitfunctions2ffcs.h"
#include "qffitfunction2ffccsnormaldiff3d.h"
#include "qffitfunction2ffccsnormaldiff3dflow.h"

QStringList QFPFitFunctions2FFCS::getIDs() const {
    QStringList res;
    res<<"fcs_2f_diff3d"<<"fcs_2f_diff3d_wz"<<"fcs_2f_diff3dflow"<<"fcs_2f_diff3dflow_wz";
    return res;
}

QFFitFunction* QFPFitFunctions2FFCS::get(QString id, QObject* parent) const  {
    if (id=="fcs_2f_diff3d") {
        return new QFFitFunctionFCCSNormalDiff3D2Focus(true);
    } else if (id=="fcs_2f_diff3d_wz") {
        return new QFFitFunctionFCCSNormalDiff3D2Focus(false);
    } else if (id=="fcs_2f_diff3dflow") {
        return new QFFitFunctionFCCSNormalDiff3DFlow2Focus(true);
    } else if (id=="fcs_2f_diff3dflow_wz") {
        return new QFFitFunctionFCCSNormalDiff3DFlow2Focus(false);
    }
    return NULL;
}

Q_EXPORT_PLUGIN2(fitfunctions_2ffcs, QFPFitFunctions2FFCS)
