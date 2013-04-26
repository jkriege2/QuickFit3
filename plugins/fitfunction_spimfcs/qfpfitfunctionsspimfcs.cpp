#include "qfpfitfunctionsspimfcs.h"
#include "qffitfunctionsspimfccsdiffflow.h"
#include "qffitfunctionsspimfccsdiff2color.h"
#include "qffitfunctionsspimfcsdiffc.h"
#include "qffitfunctionsspimfcsdiffce2.h"
#include "qffitfunctionsspimfcsdiffe2_newveff.h"
#include "qffitfunctionsspimfcsadiffe2.h"
#include "qffitfunctionsspimfcsadiffce2.h"

QStringList QFPFitFunctionsSPIMFCS::getIDs() const {
    QStringList res;
    res<<"fcs_spim_diff";
    res<<"fcs_spim_diffc";
    res<<"fcs_spim_adiff";

    res<<"fcs_spim_diffe2";
    res<<"fcs_spim_diffe2_newveff";

    res<<"fcs_spim_diffce2";
    res<<"fcs_spim_adiffe2";
    res<<"fcs_spim_adiffce2";

    res<<"fccs_spim_diffflow";
    res<<"fccs_spim_diff2color";
    return res;
}

QFFitFunction* QFPFitFunctionsSPIMFCS::get(QString id, QObject* parent) const  {
    if (id=="fcs_spim_diff") {
        return new QFFitFunctionsSPIMFCSDiff();
    } else if (id=="fcs_spim_diffc") {
        return new QFFitFunctionsSPIMFCSDiffC();
    } else if (id=="fcs_spim_diffce2") {
        return new QFFitFunctionsSPIMFCSDiffCE2();
    } else if (id=="fcs_spim_diffe2") {
        return new QFFitFunctionsSPIMFCSDiffE2();
    } else if (id=="fcs_spim_diffe2_newveff") {
        return new QFFitFunctionsSPIMFCSDiffE2NewVeff();
    } else if (id=="fcs_spim_adiff") {
        return new QFFitFunctionsSPIMFCSADiff();
    } else if (id=="fcs_spim_adiffe2") {
        return new QFFitFunctionsSPIMFCSADiffE2();
    } else if (id=="fcs_spim_adiffce2") {
        return new QFFitFunctionsSPIMFCSADiffCE2();
    } else if (id=="fccs_spim_diffflow") {
        return new QFFitFunctionsSPIMFCCSDiffFlow();
    } else if (id=="fccs_spim_diff2color") {
        return new QFFitFunctionsSPIMFCCSDiff2Color();
    }
    return NULL;
}

Q_EXPORT_PLUGIN2(fitfunctions_spimfcs, QFPFitFunctionsSPIMFCS)
