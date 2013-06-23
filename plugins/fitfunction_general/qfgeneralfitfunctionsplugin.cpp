#include "qfgeneralfitfunctionsplugin.h"
#include "qffitfunctiongeneralgaussianvar.h"
#include "qffitfunctiongeneralsigmoid.h"
#include "qffitfunctiongenerallorentzian.h"


QStringList QFGeneralFitFunctionsPlugin::getIDs() const {
    QStringList res;
    res<<"gen_sigmoid";
    res<<"gen_gaussian_sqrte";
    res<<"gen_lorentzian";
    return res;
}

QFFitFunction* QFGeneralFitFunctionsPlugin::get(QString id, QObject* parent) const  {
    if (id=="gen_sigmoid") {
        return new QFFitFunctionGeneralSigmoid();
    } else if (id=="gen_gaussian_sqrte") {
        return new QFFitFunctionGeneralGaussianVar();
    } else if (id=="gen_lorentzian") {
        return new QFFitFunctionGeneralLorentzian();
    }
    return NULL;
}

Q_EXPORT_PLUGIN2(fitfunction_general, QFGeneralFitFunctionsPlugin)
