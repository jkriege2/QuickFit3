#include "qfpfitfunctions2ffcs.h"
#include "qffitfunction2ffccsnormaldiff3d.h"

QStringList QFPFitFunctions2FFCS::getIDs() const {
    QStringList res;
    //res<<"fcs_2f_diff3d";
    return res;
}

QFFitFunction* QFPFitFunctions2FFCS::get(QString id, QObject* parent) const  {
    if (id=="fcs_2f_diff3d") {
        return new QFFitFunctionFCCSNormalDiff3D2Focus();
    }
    return NULL;
}

Q_EXPORT_PLUGIN2(fitfunctions_2ffcs, QFPFitFunctions2FFCS)
