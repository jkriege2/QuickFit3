#include "qfpfitfunctions2ffcs.h"
#include "qffitfunction2ffccsnormaldiff3d.h"
#include "qffitfunction2ffccsnormaldiff3dflow.h"
#include "qffitfunction2ffccsnormaldiff2d.h"
#include "qffitfunction2ffccsnormaldiff2dflow.h"

QStringList QFPFitFunctions2FFCS::getIDs() const {
    QStringList res;
    res<<"fccs_2f_diff3d"<<"fccs_2f_diff3d_wz"<<"fccs_2f_diff3dflow"<<"fccs_2f_diff3dflow_wz"
       <<"fccs_2f_diff2d"<<"fccs_2f_diff2dflow";
    return res;
}

QFFitFunction* QFPFitFunctions2FFCS::get(QString id, QObject* parent) const  {
    if (id=="fccs_2f_diff3d") {
        return new QFFitFunctionFCCSNormalDiff3D2Focus(true);
    } else if (id=="fccs_2f_diff2d") {
        return new QFFitFunctionFCCSNormalDiff2D2Focus();
    } else if (id=="fccs_2f_diff3d_wz") {
        return new QFFitFunctionFCCSNormalDiff3D2Focus(false);
    } else if (id=="fccs_2f_diff3dflow") {
        return new QFFitFunctionFCCSNormalDiff3DFlow2Focus(true);
    } else if (id=="fccs_2f_diff2dflow") {
        return new QFFitFunctionFCCSNormalDiff2DFlow2Focus();
    } else if (id=="fccs_2f_diff3dflow_wz") {
        return new QFFitFunctionFCCSNormalDiff3DFlow2Focus(false);
    }
    return NULL;
}

Q_EXPORT_PLUGIN2(fitfunctions_2ffcs, QFPFitFunctions2FFCS)
