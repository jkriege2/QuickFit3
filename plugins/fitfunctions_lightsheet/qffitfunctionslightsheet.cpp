#include "qffitfunctionslightsheet.h"
#include "qffitfunctionlightsheetgaussian.h"
#include "qffitfunctionlightsheetgaussianvar.h"
#include "qffitfunctionlightsheetslit.h"

QStringList QFFitFunctionsLightsheet::getIDs() const {
    QStringList res;
    res<<"lightsheet_gaussian";
    res<<"lightsheet_gaussian_sqrte";
    res<<"lightsheet_slit";
    return res;
}

QFFitFunction* QFFitFunctionsLightsheet::get(QString id, QObject* parent) const  {
    if (id=="lightsheet_gaussian") {
        return new QFFitFunctionLightsheetGaussian();
    } else if (id=="lightsheet_gaussian_sqrte") {
        return new QFFitFunctionLightsheetGaussianVar();
    } else if (id=="lightsheet_slit") {
        return new QFFitFunctionLightsheetSlit();
    }
    return NULL;
}

Q_EXPORT_PLUGIN2(fitfunctions_lightsheet, QFFitFunctionsLightsheet)
