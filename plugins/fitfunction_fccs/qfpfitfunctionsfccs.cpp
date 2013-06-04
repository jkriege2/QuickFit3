#include "qfpfitfunctionsfccs.h"
#include "qffitfunctionsfccsfwadiff2coloracfg.h"
#include "qffitfunctionsfccsfwadiff2coloracfr.h"
#include "qffitfunctionsfccsfwadiff2colorccf.h"
#include "qffitfunctionsfccsfwdiff2coloracfg.h"
#include "qffitfunctionsfccsfwdiff2coloracfr.h"
#include "qffitfunctionsfccsfwdiff2colorccf.h"

QStringList QFPFitFunctionsFCCS::getIDs() const {
    QStringList res;
    res<<"fccs_diff3d";
    res<<"fccs_fw_diff2colorccf";
    res<<"fccs_fw_diff2coloracfg";
    res<<"fccs_fw_diff2coloracfr";
    res<<"fccs_fw_adiff2colorccf";
    res<<"fccs_fw_adiff2coloracfg";
    res<<"fccs_fw_adiff2coloracfr";
    return res;
}

QFFitFunction* QFPFitFunctionsFCCS::get(QString id, QObject* parent) const  {
    if (id=="fccs_diff3d") {
        return new QFFitFunctionFCCSNormalDiff3D();
    } else if (id=="fccs_fw_diff2coloracfg") {
        return new QFFitFunctionsFCCSFWDiff2ColorACFG();
    } else if (id=="fccs_fw_diff2coloracfr") {
        return new QFFitFunctionsFCCSFWDiff2ColorACFR();
    } else if (id=="fccs_fw_diff2colorccf") {
        return new QFFitFunctionsFCCSFWDiff2ColorCCF();
    } else if (id=="fccs_fw_adiff2coloracfg") {
        return new QFFitFunctionsFCCSFWADiff2ColorACFG();
    } else if (id=="fccs_fw_adiff2coloracfr") {
        return new QFFitFunctionsFCCSFWADiff2ColorACFR();
    } else if (id=="fccs_fw_adiff2colorccf") {
        return new QFFitFunctionsFCCSFWADiff2ColorCCF();
    }
    return NULL;
}

Q_EXPORT_PLUGIN2(fitfunctions_fccs, QFPFitFunctionsFCCS)
