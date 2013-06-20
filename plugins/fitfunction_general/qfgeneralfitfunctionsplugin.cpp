#include "qfgeneralfitfunctionsplugin.h"

QStringList QFGeneralFitFunctionsPlugin::getIDs() const {
    QStringList res;
    res<<"gen_sigmoid";
    return res;
}

QFFitFunction* QFGeneralFitFunctionsPlugin::get(QString id, QObject* parent) const  {
    if (id=="gen_sigmoid") {
        return new QFFitFunctionGeneralSigmoid();
    } 
    return NULL;
}

Q_EXPORT_PLUGIN2(fitfunction_general, QFGeneralFitFunctionsPlugin)
