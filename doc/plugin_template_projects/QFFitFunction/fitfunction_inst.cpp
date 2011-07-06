#include "fcs_fitfuctions.h"

QStringList QFPFitFunctionInst::getIDs() const {
    QStringList res;
    res<<"ff_id";
    return res;
}

QFFitFunction* QFPFitFunctionInst::get(QString id, QObject* parent) const  {
    if (id=="ff_id") {
        return new QFFitAlgorithmInst_F1();
    } 
    return NULL;
}

Q_EXPORT_PLUGIN2(target_id, QFPFitFunctionInst)
