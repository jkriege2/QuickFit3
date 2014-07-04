#include "qflibraryfitfunction.h"

QFSharedFitFunction::QFSharedFitFunction(QLibrary *library)
{
    this->library=library;
    lib_getName=NULL;
    lib_getParams=NULL;
    lib_getParamCount=NULL;
    lib_eval=NULL;
    lib_evalMulti=NULL;
    libOK=false;
    // resolve library symbols
    if (library) {
        lib_getName = (QF3SimpleFFGetNameFunc) library->resolve("getModelName");
        lib_getParams = (QF3SimpleFFGetParameterDescriptionFunc) library->resolve("getParameterProperties");
        lib_getParamCount = (QF3SimpleFFGetParameterCountFunc) library->resolve("getParameterCount");
        lib_eval = (QF3SimpleFFEvaluateFunc) library->resolve("evaluate");
        lib_evalMulti = (QF3SimpleFFMultiEvaluateFunc) library->resolve("multiEvaluate");
        libOK=((lib_getName!=NULL)&&(lib_getParams!=NULL)&&(lib_getParamCount!=NULL)&&(lib_eval!=NULL));
    }
    // read fit function  properties
    if (libOK) {
        m_id=lib_getName(1);
        m_name=lib_getName(0);
        m_shortName=lib_getName(2);
        m_help=lib_getName(3);
        if (m_id.isEmpty()) libOK=false;
        if (m_name.isEmpty()) m_name=m_id;
        if (m_shortName.isEmpty()) m_shortName=m_name;
    }
    // read fit function parameter properties
    if (libOK) {
        uint16_t pc=lib_getParamCount();
        const QF3SimpleFFParameter* params=lib_getParams();
        for (int i=0; i<pc; i++) {
            //TODO: GO ON HERE, IMPLEMENT addParameter(...)
            //addParameter();
        }
    }
}

QString QFSharedFitFunction::helpFile() const
{
    return m_help;
}

bool QFSharedFitFunction::isValid() const
{
    return libOK;
}

QString QFSharedFitFunction::name() const
{
    return m_name;
}

QString QFSharedFitFunction::shortName() const
{
    return m_shortName;
}

QString QFSharedFitFunction::id() const
{
    return m_id;
}

double QFSharedFitFunction::evaluate(double x, const double *parameters) const
{
    if (lib_eval) return lib_eval(x, parameters);
    return NAN;
}

void QFSharedFitFunction::multiEvaluate(double *y, const double *x, uint64_t N, const double *parameters) const
{
    if (lib_evalMulti) lib_evalMulti(y, x, N, parameters);
    else QFFitFunction::multiEvaluate(y, x, N, parameters);
}

void QFSharedFitFunction::evaluateDerivatives(double *derivatives, double x, const double *parameters) const
{
    QFFitFunction::evaluateDerivatives(derivatives, x, parameters);
}

bool QFSharedFitFunction::get_implementsDerivatives()
{
    return QFFitFunction::get_implementsDerivatives();
}

void QFSharedFitFunction::calcParameter(double *parameterValues, double *error) const
{
    QFFitFunction::calcParameter(parameterValues, error);
}

void QFSharedFitFunction::sortParameter(double *parameterValues, double *error, bool *fix) const
{
    QFFitFunction::sortParameter(parameterValues, error, fix);
}

bool QFSharedFitFunction::isParameterVisible(int parameter, const double *parameterValues) const
{
    return QFFitFunction::isParameterVisible(parameter, parameterValues);
}

unsigned int QFSharedFitFunction::getAdditionalPlotCount(const double *params) {
    return QFFitFunction::getAdditionalPlotCount(params);
}

QString QFSharedFitFunction::transformParametersForAdditionalPlot(int plot, double *params)
{
    return QFFitFunction::transformParametersForAdditionalPlot(plot, params);
}

bool QFSharedFitFunction::estimateInitial(double *params, const double *dataX, const double *dataY, long N, const bool *fix)
{
    return QFFitFunction::estimateInitial(params, dataX, dataY, N, fix);
}
