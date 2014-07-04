#include "qflibraryfitfunction.h"
#include <stdint.h>
#include "./sdk_fitfunctions/quickfit-model-tools.h"


class QFLibraryFitFunction_private {
    public:
        explicit QFLibraryFitFunction_private() {
            libOK=false;
            library=NULL;
            lib_getName=NULL;
            lib_evalMulti=NULL;
            lib_getParams=NULL;
            lib_getParamCount=NULL;
            lib_eval=NULL;
        }

        QLibrary* library;
        QString m_id;
        QString m_name;
        QString m_shortName;
        QString m_help;
        bool libOK;
        QString last_error;

        QF3SimpleFFGetNameFunc lib_getName;
        QF3SimpleFFMultiEvaluateFunc lib_evalMulti;
        QF3SimpleFFGetParameterDescriptionFunc lib_getParams;
        QF3SimpleFFGetParameterCountFunc lib_getParamCount;
        QF3SimpleFFEvaluateFunc lib_eval;

        friend class QFLibraryFitFunction;
};


QFLibraryFitFunction::QFLibraryFitFunction(QLibrary *library)
{
    d=new QFLibraryFitFunction_private();
    d->library=library;
    d->lib_getName=NULL;
    d->lib_getParams=NULL;
    d->lib_getParamCount=NULL;
    d->lib_eval=NULL;
    d->lib_evalMulti=NULL;
    d->libOK=false;
    // resolve library symbols
    if (library) {
        d->lib_getName = (QF3SimpleFFGetNameFunc) d->library->resolve("getModelName");
        d->lib_getParams = (QF3SimpleFFGetParameterDescriptionFunc) d->library->resolve("getParameterProperties");
        d->lib_getParamCount = (QF3SimpleFFGetParameterCountFunc) d->library->resolve("getParameterCount");
        d->lib_eval = (QF3SimpleFFEvaluateFunc) d->library->resolve("evaluate");
        d->lib_evalMulti = (QF3SimpleFFMultiEvaluateFunc) d->library->resolve("multiEvaluate");
        d->libOK=((d->lib_getName!=NULL)&&(d->lib_getParams!=NULL)&&(d->lib_getParamCount!=NULL)&&(d->lib_eval!=NULL));
        if (!d->libOK) d->last_error=QString("not all symbols resolved: lib_getName=%1 lib_getParams=%2 lib_getParamCount=%3 lib_eval=%4 lib_evalMulti=%5").arg((int64_t)(d->lib_getName)).arg((int64_t)(d->lib_getParams)).arg((int64_t)(d->lib_getParamCount)).arg((int64_t)(d->lib_eval)).arg((int64_t)(d->lib_evalMulti));
    } else {
        d->last_error="NO LIBRARY GIVEN";
    }
    // read fit function  properties
    if (d->libOK) {
        d->m_id=d->lib_getName(1);
        d->m_name=d->lib_getName(0);
        d->m_shortName=d->lib_getName(2);
        d->m_help=d->lib_getName(3);
        if (!d->m_help.isEmpty()) {
            d->m_help=QFileInfo(d->library->fileName()).absoluteDir().absoluteFilePath(d->m_help);
        }
        if (d->m_id.isEmpty()) {
            d->libOK=false;
            d->last_error="NO ID GIVEN";
        }
        if (d->m_name.isEmpty()) d->m_name=d->m_id;
        if (d->m_shortName.isEmpty()) d->m_shortName=d->m_name;
    }
    // read fit function parameter properties
    if (d->libOK) {
        uint16_t pc=d->lib_getParamCount();
        const QF3SimpleFFParameter* params=d->lib_getParams();
        for (int i=0; i<pc; i++) {
            addParameter((QFFitFunction::ParameterType)params[i].type,
                         params[i].id,
                         params[i].name,
                         params[i].label,
                         params[i].unit,
                         params[i].unitLabel,
                         params[i].fit!=QF3SFF_FALSE,
                         params[i].userEditable!=QF3SFF_FALSE,
                         params[i].userRangeEditable!=QF3SFF_FALSE,
                         (QFFitFunction::ErrorDisplayMode)params[i].displayError,
                         params[i].initialFix!=QF3SFF_FALSE,
                         params[i].initialValue,
                         params[i].minValue,
                         params[i].maxValue,
                         params[i].inc,
                         params[i].absMinValue,
                         params[i].absMaxValue);
        }
    }
}

QFLibraryFitFunction::~QFLibraryFitFunction()
{
    if (d) delete d;
}

QString QFLibraryFitFunction::helpFile() const
{
    return d->m_help;
}

bool QFLibraryFitFunction::isValid() const
{
    return d->libOK;
}

QString QFLibraryFitFunction::lastError() const
{
    return d->last_error;
}

QString QFLibraryFitFunction::name() const
{
    return d->m_name;
}

QString QFLibraryFitFunction::shortName() const
{
    return d->m_shortName;
}

QString QFLibraryFitFunction::id() const
{
    return d->m_id;
}

double QFLibraryFitFunction::evaluate(double x, const double *parameters) const
{
    if (d->lib_eval) return d->lib_eval(x, parameters);
    return NAN;
}

void QFLibraryFitFunction::multiEvaluate(double *y, const double *x, uint64_t N, const double *parameters) const
{
    if (d->lib_evalMulti) d->lib_evalMulti(y, x, N, parameters);
    else QFFitFunction::multiEvaluate(y, x, N, parameters);
}

void QFLibraryFitFunction::evaluateDerivatives(double *derivatives, double x, const double *parameters) const
{
    QFFitFunction::evaluateDerivatives(derivatives, x, parameters);
}

bool QFLibraryFitFunction::get_implementsDerivatives()
{
    return QFFitFunction::get_implementsDerivatives();
}

void QFLibraryFitFunction::calcParameter(double *parameterValues, double *error) const
{
    QFFitFunction::calcParameter(parameterValues, error);
}

void QFLibraryFitFunction::sortParameter(double *parameterValues, double *error, bool *fix) const
{
    QFFitFunction::sortParameter(parameterValues, error, fix);
}

bool QFLibraryFitFunction::isParameterVisible(int parameter, const double *parameterValues) const
{
    return QFFitFunction::isParameterVisible(parameter, parameterValues);
}

unsigned int QFLibraryFitFunction::getAdditionalPlotCount(const double *params) {
    return QFFitFunction::getAdditionalPlotCount(params);
}

QString QFLibraryFitFunction::transformParametersForAdditionalPlot(int plot, double *params)
{
    return QFFitFunction::transformParametersForAdditionalPlot(plot, params);
}

bool QFLibraryFitFunction::estimateInitial(double *params, const double *dataX, const double *dataY, long N, const bool *fix)
{
    return QFFitFunction::estimateInitial(params, dataX, dataY, N, fix);
}
