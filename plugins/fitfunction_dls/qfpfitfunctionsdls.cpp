#include "qfpfitfunctionsdls.h"
#include "qffitfunctiondlsg2ind.h"
#include "qffitfunctiondlsg2.h"
#include "qffitfunctiondlsg2indext.h"
#include "qffitfunctiondlsg2_gaussiantaudist.h"
#include "qffitfunctiondlsg2_loggaussiantaudist.h"

QStringList QFPFitFunctionsDLS::getIDs() const {
    QStringList res;
    res<<"dls_g2"<<"dls_g2_ind"<<"dls_g2_ind_ext"<<"dls_g2_int_taunormdist"<<"dls_g2_int_taulognormdist";
    return res;
}

QFFitFunction* QFPFitFunctionsDLS::get(QString id, QObject* parent) const  {
    if (id=="dls_g2") {
        return new QFFitFunctionDLSG2();
    } else if (id=="dls_g2_int_taunormdist") {
        return new QFFitFunctionDLSG2NormTauDist();
    } else if (id=="dls_g2_int_taulognormdist") {
        return new QFFitFunctionDLSG2LogNormTauDist();
    } else if (id=="dls_g2_ind") {
        return new QFFitFunctionDLSG2inD();
    } else if (id=="dls_g2_ind_ext") {
        return new QFFitFunctionDLSG2inDExt();
    }
    return NULL;
}

Q_EXPORT_PLUGIN2(fitfunctions_dls, QFPFitFunctionsDLS)
