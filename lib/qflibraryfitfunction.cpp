/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "qflibraryfitfunction.h"
#include <stdint.h>
#include "./sdk_fitfunctions/quickfit-model-tools.h"
#include "qftools.h"

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
            lib_calcParams=NULL;
            lib_isParamVisible=NULL;
            lib_sortParams=NULL;
            lib_evalDerivative=NULL;
            lib_estimateInitial=NULL;
            lib_getAdditionalPlots=NULL;
            lib_transformAdditionalPlot=NULL;
            lib_getVersion=NULL;
        }

        QLibrary* library;
        QString m_id;
        QString m_name;
        QString m_shortName;
        QString m_help;
        QString m_features;
        bool libOK;
        QString last_error;

        QF3SimpleFFGetNameFunc lib_getName;
        QF3SimpleFFMultiEvaluateFunc lib_evalMulti;
        QF3SimpleFFGetParameterDescriptionFunc lib_getParams;
        QF3SimpleFFGetParameterCountFunc lib_getParamCount;
        QF3SimpleFFEvaluateFunc lib_eval;
        QF3SimpleFFCalculateParameters lib_calcParams;
        QF3SimpleFFIsParameterVisible lib_isParamVisible;
        QF3SimpleFFSortParameters lib_sortParams;
        QF3SimpleFFEvaluateDerivatives lib_evalDerivative;
        QF3SimpleFFEstimateInitial lib_estimateInitial;
        QF3SimpleFFGetAdditionalPlotCount lib_getAdditionalPlots;
        QF3SimpleFFTransformParametersForAdditionalPlot lib_transformAdditionalPlot;
        QF3SimpleFFGetVersionFunc lib_getVersion;

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
        d->lib_getVersion = (QF3SimpleFFGetVersionFunc) d->library->resolve("getVersion");
        d->lib_getName = (QF3SimpleFFGetNameFunc) d->library->resolve("getModelName");
        d->lib_getParams = (QF3SimpleFFGetParameterDescriptionFunc) d->library->resolve("getParameterProperties");
        d->lib_getParamCount = (QF3SimpleFFGetParameterCountFunc) d->library->resolve("getParameterCount");
        d->lib_eval = (QF3SimpleFFEvaluateFunc) d->library->resolve("evaluate");
        d->lib_evalMulti = (QF3SimpleFFMultiEvaluateFunc) d->library->resolve("multiEvaluate");

        d->lib_calcParams=(QF3SimpleFFCalculateParameters) d->library->resolve("calulateParameters");
        d->lib_isParamVisible=(QF3SimpleFFIsParameterVisible) d->library->resolve("isParameterVisible");
        d->lib_sortParams=(QF3SimpleFFSortParameters) d->library->resolve("sortParameters");
        d->lib_evalDerivative=(QF3SimpleFFEvaluateDerivatives) d->library->resolve("evaluateDerivatives");
        d->lib_estimateInitial=(QF3SimpleFFEstimateInitial) d->library->resolve("estimateInitial");
        d->lib_getAdditionalPlots=(QF3SimpleFFGetAdditionalPlotCount) d->library->resolve("getAdditionalPlotCount");
        d->lib_transformAdditionalPlot=(QF3SimpleFFTransformParametersForAdditionalPlot) d->library->resolve("transformParametersForAdditionalPlot");

        d->libOK=((d->lib_getName!=NULL)&&(d->lib_getParams!=NULL)&&(d->lib_getParamCount!=NULL)&&(d->lib_eval!=NULL)&&(d->lib_getVersion!=NULL));
        if (!d->libOK) d->last_error=QString("not all symbols resolved: lib_getVersion=%6 lib_getName=%1 lib_getParams=%2 lib_getParamCount=%3 lib_eval=%4 lib_evalMulti=%5").arg(QString("0x")+pntToHexQString((void*)d->lib_getName)).arg(QString("0x")+pntToHexQString((void*)d->lib_getParams)).arg(QString("0x")+pntToHexQString((void*)d->lib_getParamCount)).arg(QString("0x")+pntToHexQString((void*)d->lib_eval)).arg(QString("0x")+pntToHexQString((void*)d->lib_evalMulti)).arg(QString("0x")+pntToHexQString((void*)d->lib_getVersion));
        else {
            QStringList additional;
            uint16_t vmajor=0, vminor=0;
            d->lib_getVersion(&vmajor, &vminor);
            additional<<QObject::tr("QF3SFF_API v.%1.%2").arg(vmajor).arg(vminor);
            if (d->lib_evalMulti) additional<<QString("multiEvaluate()");
            if (d->lib_calcParams) additional<<QString("calculateParameters()");
            if (d->lib_isParamVisible) additional<<QString("isParamVisible()");
            if (d->lib_sortParams) additional<<QString("sortParams()");
            if (d->lib_evalDerivative) additional<<QString("evalDerivative()");
            if (d->lib_estimateInitial) additional<<QString("estimateInitial()");
            if (d->lib_getAdditionalPlots) additional<<QString("getAdditionalPlots()");
            if (d->lib_transformAdditionalPlot) additional<<QString("transformAdditionalPlot()");
            d->m_features="";
            if (additional.size()>0) d->m_features=additional.join(", ");

        }
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

QString QFLibraryFitFunction::features() const
{
    return d->m_features;
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
    if (d->lib_evalDerivative) d->lib_evalDerivative(derivatives,x,parameters);
    else QFFitFunction::evaluateDerivatives(derivatives, x, parameters);
}

bool QFLibraryFitFunction::get_implementsDerivatives()
{
    if (d->lib_evalDerivative) return true;
    return QFFitFunction::get_implementsDerivatives();
}

void QFLibraryFitFunction::calcParameter(double *parameterValues, double *error) const
{
    if (d->lib_calcParams) d->lib_calcParams(parameterValues,error);
    else QFFitFunction::calcParameter(parameterValues, error);
}

void QFLibraryFitFunction::sortParameter(double *parameterValues, double *error, bool *fix) const
{
    if (d->lib_sortParams) {
        if (fix) {
            const int pc=paramCount();
            QVector<int8_t> f(pc, QF3SFF_FALSE);
            for (int i=0; i<pc; i++) {
                if (fix[i]) f[i]=QF3SFF_TRUE;
            }
            d->lib_sortParams(parameterValues,error,f.data());
            for (int i=0; i<pc; i++) {
                fix[i]=(f[i]!=QF3SFF_FALSE);
            }
        } else {
            d->lib_sortParams(parameterValues,error,NULL);
        }
    } else QFFitFunction::sortParameter(parameterValues, error, fix);
}

bool QFLibraryFitFunction::isParameterVisible(int parameter, const double *parameterValues) const
{
    if (d->lib_isParamVisible) return d->lib_isParamVisible(parameter, parameterValues);
    else return QFFitFunction::isParameterVisible(parameter, parameterValues);
}

unsigned int QFLibraryFitFunction::getAdditionalPlotCount(const double *params) {
    if (d->lib_getAdditionalPlots) return d->lib_getAdditionalPlots(params);
    else return QFFitFunction::getAdditionalPlotCount(params);
}

QString QFLibraryFitFunction::transformParametersForAdditionalPlot(int plot, double *params)
{
    if (d->lib_transformAdditionalPlot) return d->lib_transformAdditionalPlot(plot, params);
    else return QFFitFunction::transformParametersForAdditionalPlot(plot, params);
}

bool QFLibraryFitFunction::estimateInitial(double *params, const double *dataX, const double *dataY, long N, const bool *fix)
{
    if (d->lib_estimateInitial) {
        if (fix) {
            const int pc=paramCount();
            QVector<int8_t> f(pc, QF3SFF_FALSE);
            for (int i=0; i<pc; i++) {
                if (fix[i]) f[i]=QF3SFF_TRUE;
            }
            return (d->lib_estimateInitial(params, dataX, dataY, N, f.data())!=QF3SFF_FALSE);
        } else {
            return d->lib_estimateInitial(params, dataX, dataY, N, NULL);
        }
    } else return QFFitFunction::estimateInitial(params, dataX, dataY, N, fix);
}
