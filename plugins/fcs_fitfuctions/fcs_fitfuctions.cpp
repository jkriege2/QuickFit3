#include "fcs_fitfuctions.h"


#include "qffitfunctionfcsmultidiffusion.h"
#include "qffitfunctionfcsadiff.h"
#include "qffitfunctionfcsdiff.h"
#include "qffitfunctionfcssimplediff.h"
#include "qffitfunctionfcsflow.h"
#include "qffitfunctionfcsrot.h"
#include "qffitfunctionfcsoscillation.h"
#include "qffitfunctionfcsdiffD.h"









QStringList QFPFitFunctionFCS::getIDs() const {
    QStringList res;
    res<<"fcs_diff"<<"fcs_diff_d"<<"fcs_diff_d_wz"<<"fcs_diff1"<<"fcs_adiff"<<"fcs_multidiff"<<"fcs_diffflow"<<"fcs_diffrot"<<"fcs_oscillation";
    return res;
}

QFFitFunction* QFPFitFunctionFCS::get(QString id, QObject* parent) const  {
    if (id=="fcs_diff") {
        return new QFFitFunctionFCSDiff();
    } else if (id=="fcs_diff_d") {
        return new QFFitFunctionFCSDiffD();
    } else if (id=="fcs_diff_d_wz") {
        return new QFFitFunctionFCSDiffD(false);
    } else if (id=="fcs_diff1") {
        return new QFFitFunctionFCSSimpleDiff();
    } else if (id=="fcs_adiff") {
        return new QFFitFunctionFCSADiff();
    } else if (id=="fcs_multidiff") {
        return new QFFitFunctionFCSMultiDiffusion();
    } else if (id=="fcs_diffflow") {
        return new QFFitFunctionFCSFlow();
    } else if (id=="fcs_diffrot") {
        return new QFFitFunctionFCSRot();
    } else if (id=="fcs_oscillation") {
        return new QFFitFunctionFCSOscillation();
    }
    return NULL;
}

Q_EXPORT_PLUGIN2(qffcsfitfuncs, QFPFitFunctionFCS)
