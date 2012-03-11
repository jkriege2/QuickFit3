#include "importer_inst.h"

QStringList QFFitAlgorithmInst::getIDs() const {
    QStringList res;
    res<<"ff_id";
    return res;
}

QFImporter* QFFitAlgorithmInst::createImporter(QString id) const  {
    if (id=="ff_id") {
        return new QFFitAlgorithmInst_F1();
    } 
    return NULL;
}

Q_EXPORT_PLUGIN2(target_id, QFFitAlgorithmInst)
