#include "qfgeneralfitfunctionsplugin.h"
#include "qffitfunctiongeneralgaussianvar.h"
#include "qffitfunctiongeneral2gaussianvar.h"
#include "qffitfunctiongeneral3gaussianvar.h"
#include "qffitfunctiongeneralsigmoid.h"
#include "qffitfunctiongenerallorentzian.h"
#include "qffitfunctiongeneralexp.h"
#include "qffitfunctiongeneraldblexp.h"
#include "qffitfunctiongeneralpowerlaw.h"
#include "qffitfunctiongeneralsine.h"
#include "qffitfunctiongeneralcosine.h"
#include "qffitfunctiongeneralline.h"
#include "qffitfunctiongeneralpolynom.h"
#include "qffitfunctiongenerallognormal.h"
#include "qffitfunctiongeneralmodhill5p.h"
#include "qffitfunctiongeneralhill.h"



QStringList QFGeneralFitFunctionsPlugin::getIDs() const {
    QStringList res;
    res<<"gen_gaussian_sqrte";
    res<<"gen_2gaussian_sqrte";
    res<<"gen_3gaussian_sqrte";
    res<<"gen_lorentzian";
    res<<"gen_lognormal";
    res<<"gen_sine";
    res<<"gen_cos";
    res<<"gen_sigmoid";
    res<<"gen_exp";
    res<<"gen_dblexp";
    res<<"gen_powerlaw";
    res<<"gen_line";
    res<<"gen_polynom";
    res<<"gen_hill";
    res<<"gen_modhill5p";
    return res;
}

QFFitFunction* QFGeneralFitFunctionsPlugin::get(QString id, QObject* parent) const  {
    if (id=="gen_sigmoid") {
        return new QFFitFunctionGeneralSigmoid();
    } else if (id=="gen_gaussian_sqrte") {
        return new QFFitFunctionGeneralGaussianVar();
    } else if (id=="gen_2gaussian_sqrte") {
        return new QFFitFunctionGeneral2GaussianVar();
    } else if (id=="gen_3gaussian_sqrte") {
        return new QFFitFunctionGeneral3GaussianVar();
    } else if (id=="gen_lorentzian") {
        return new QFFitFunctionGeneralLorentzian();
    } else if (id=="gen_exp") {
        return new QFFitFunctionGeneralExp();
    } else if (id=="gen_dblexp") {
        return new QFFitFunctionGeneralDblExp();
    } else if (id=="gen_powerlaw") {
        return new QFFitFunctionGeneralPowerLaw();
    } else if (id=="gen_sine") {
        return new QFFitFunctionGeneralSine();
    } else if (id=="gen_cos") {
        return new QFFitFunctionGeneralCosine();
    } else if (id=="gen_lognormal") {
        return new QFFitFunctionGeneralLogNormal();
    } else if (id=="gen_line") {
        return new QFFitFunctionGeneralLine();
    } else if (id=="gen_polynom") {
        return new QFFitFunctionGeneralPolynom();
    } else if (id=="gen_hill") {
        return new QFFitFunctionGeneralHill();
    } else if (id=="gen_modhill5p") {
        return new QFFitFunctionGeneralModHill5P();
    }
    return NULL;
}

Q_EXPORT_PLUGIN2(fitfunction_general, QFGeneralFitFunctionsPlugin)
