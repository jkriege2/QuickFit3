#include "qfpfitfunctionsspimfcs.h"
#include "qffitfunctionsspimfcsdiff.h"
#include "qffitfunctionsspimfcsdiffe2.h"
#include "qffitfunctionsspimfcsadiff.h"
#include "qffitfunctionsspimfccsdiffflow.h"
#include "qffitfunctionsspimfccsdiff.h"
#include "qffitfunctionsspimfccsdiff2color.h"
#include "qffitfunctionsspimfcsdiffc.h"
#include "qffitfunctionsspimfcsdiffce2.h"
#include "qffitfunctionsspimfcsdiffe2_newveff.h"
#include "qffitfunctionsspimfcsadiffe2.h"
#include "qffitfunctionsspimfcsadiffce2.h"
#include "qffitfunctionsspimfccsfwdiff2colorccf.h"
#include "qffitfunctionsspimfccsfwdiff2coloracfg.h"
#include "qffitfunctionsspimfccsfwdiff2coloracfr.h"
#include "qffitfunctionsspimfccsfwfactordiff2colorccf.h"
#include "qffitfunctionsspimfccsfwfactordiff2coloracfg.h"
#include "qffitfunctionsspimfccsfwfactordiff2coloracfr.h"
#include "qffitfunctionsspimfccsfw2cdiff2colorccf.h"
#include "qffitfunctionsspimfccsfw2cdiff2coloracfg.h"
#include "qffitfunctionsspimfccsfw2cdiff2coloracfr.h"
#include "qffitfunctionsspimfccsfwadiff2colorccf.h"
#include "qffitfunctionsspimfccsfwadiff2coloracfg.h"
#include "qffitfunctionsspimfccsfwadiff2coloracfr.h"

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

    res<<"fccs_spim_diff";
    res<<"fccs_spim_diffflow";
    res<<"fccs_spim_diff2color";

    res<<"fccs_spim_fw_diff2colorccf";
    res<<"fccs_spim_fw_diff2coloracfg";
    res<<"fccs_spim_fw_diff2coloracfr";
    res<<"fccs_spim_fw_factordiff2colorccf";
    res<<"fccs_spim_fw_factordiff2coloracfg";
    res<<"fccs_spim_fw_factordiff2coloracfr";
    res<<"fccs_spim_fw_2cdiff2colorccf";
    res<<"fccs_spim_fw_2cdiff2coloracfg";
    res<<"fccs_spim_fw_2cdiff2coloracfr";
    res<<"fccs_spim_fw_adiff2colorccf";
    res<<"fccs_spim_fw_adiff2coloracfg";
    res<<"fccs_spim_fw_adiff2coloracfr";
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
    } else if (id=="fccs_spim_diff") {
        return new QFFitFunctionsSPIMFCCSDiff();
    } else if (id=="fccs_spim_diffflow") {
        return new QFFitFunctionsSPIMFCCSDiffFlow();
    } else if (id=="fccs_spim_diff2color") {
        return new QFFitFunctionsSPIMFCCSDiff2Color();
    } else if (id=="fccs_spim_fw_diff2coloracfg") {
        return new QFFitFunctionsSPIMFCCSFWDiff2ColorACFG();
    } else if (id=="fccs_spim_fw_diff2coloracfr") {
        return new QFFitFunctionsSPIMFCCSFWDiff2ColorACFR();
    } else if (id=="fccs_spim_fw_diff2colorccf") {
        return new QFFitFunctionsSPIMFCCSFWDiff2ColorCCF();
    } else if (id=="fccs_spim_fw_factordiff2coloracfg") {
        return new QFFitFunctionsSPIMFCCSFWFactorDiff2ColorACFG();
    } else if (id=="fccs_spim_fw_factordiff2coloracfr") {
        return new QFFitFunctionsSPIMFCCSFWFactorDiff2ColorACFR();
    } else if (id=="fccs_spim_fw_factordiff2colorccf") {
        return new QFFitFunctionsSPIMFCCSFWFactorDiff2ColorCCF();
    } else if (id=="fccs_spim_fw_2cdiff2coloracfg") {
        return new QFFitFunctionsSPIMFCCSFW2CDiff2ColorACFG();
    } else if (id=="fccs_spim_fw_2cdiff2coloracfr") {
        return new QFFitFunctionsSPIMFCCSFW2CDiff2ColorACFR();
    } else if (id=="fccs_spim_fw_2cdiff2colorccf") {
        return new QFFitFunctionsSPIMFCCSFW2CDiff2ColorCCF();
    } else if (id=="fccs_spim_fw_adiff2coloracfg") {
        return new QFFitFunctionsSPIMFCCSFWADiff2ColorACFG();
    } else if (id=="fccs_spim_fw_adiff2coloracfr") {
        return new QFFitFunctionsSPIMFCCSFWADiff2ColorACFR();
    } else if (id=="fccs_spim_fw_adiff2colorccf") {
        return new QFFitFunctionsSPIMFCCSFWADiff2ColorCCF();
    }
    return NULL;
}

Q_EXPORT_PLUGIN2(fitfunctions_spimfcs, QFPFitFunctionsSPIMFCS)
