#include "importer_inst_f1.h"
#include <QObject>

QFFitAlgorithmInst_F1::QFFitAlgorithmInst_F1() {
 
}

QString QFFitAlgorithmInst_F1::filter() const {
    return QObject::tr("Fileformat (*.dat)");
}

QString QFFitAlgorithmInst_F1::formatName() const {
    return QObject::tr("Fileformat");
}

