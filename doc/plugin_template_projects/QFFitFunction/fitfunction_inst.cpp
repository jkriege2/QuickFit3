#include "fitfunction_inst.h"

QStringList QFFitAlgorithmInst::getIDs() const {
    QStringList res;
    res<<"ff_id";
    return res;
}

QFFitFunction* QFFitAlgorithmInst::get(QString id, QObject* parent) const  {
    if (id=="ff_id") {
        return new QFFitAlgorithmInst_F1();
    } 
    return NULL;
}

Q_EXPORT_PLUGIN2(target_id, QFFitAlgorithmInst)
