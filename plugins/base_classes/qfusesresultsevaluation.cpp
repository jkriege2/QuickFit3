/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg
	
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

#include "qfusesresultsevaluation.h"
#include <cfloat>

QFUsesResultsEvaluation::QFUsesResultsEvaluation(QFProject *parent, bool showRDRList, bool useSelection) :
    QFEvaluationItem(parent, showRDRList, useSelection)
{
    // open ini-file to superseede fit parameter defaults from QFFitFunction
    QString inifn=parent->getServices()->getOptions()->getConfigFileDirectory()+QString("/fitparams_%1.ini").arg(getType());
    fitParamSettings=new QSettings(inifn, QSettings::IniFormat);
    QString inifng=parent->getServices()->getOptions()->getGlobalConfigFileDirectory()+QString("/fitparams_%1.ini").arg(getType());
    fitParamGlobalSettings=new QSettings(inifng, QSettings::IniFormat);

    // clear internal parameter store
    parameterStore.clear();
}

QFUsesResultsEvaluation::~QFUsesResultsEvaluation()
{
}

void QFUsesResultsEvaluation::setPresetProperty(const QString &id, const QVariant &data, bool usereditable, bool visible)
{
    QString lid=id.toLower();
    if (lid.startsWith("preset_") && lid.endsWith("_error")) {
        QString idd=id;
        idd=idd.remove(0, QString("preset_").size());
        idd=idd.left(idd.size()-QString("_error").size());
        setInitFitValueError(idd, data.toDouble());
    } else if (lid.startsWith("preset_") && lid.endsWith("_fix")) {
        QString idd=id;
        idd=idd.remove(0, QString("preset_").size());
        idd=idd.left(idd.size()-QString("_fix").size());
        setInitFitFix(idd, data.toBool());
    } else if (lid.startsWith("preset_") && lid.endsWith("_min")) {
        QString idd=id;
        idd=idd.remove(0, QString("preset_").size());
        idd=idd.left(idd.size()-QString("_min").size());
        setInitFitValueMin(idd, data.toDouble());
    } else if (lid.startsWith("preset_") && lid.endsWith("_max")) {
        QString idd=id;
        idd=idd.remove(0, QString("preset_").size());
        idd=idd.left(idd.size()-QString("_max").size());
        setInitFitValueMax(idd, data.toDouble());
    } else if (lid.startsWith("preset_")) {
        QString idd=id;
        idd=idd.remove(0, QString("preset_").size());
        setInitFitValue(idd, data.toDouble());
    } else {
        QFEvaluationItem::setPresetProperty(id, data, usereditable, visible);
    }

}

bool QFUsesResultsEvaluation::hasResults(const QFRawDataRecord *r1) const {
    return hasResults(r1, getEvaluationResultID());
}

bool QFUsesResultsEvaluation::hasResults(const QFRawDataRecord *r1, QString resultID) const {
    if (!r1) return false;
    return r1->resultsExistsFromEvaluation(resultID);
}

bool QFUsesResultsEvaluation::hasResults() const {
    return hasResults(getHighlightedRecord());
}

QString QFUsesResultsEvaluation::getEvaluationResultID() const {
    return QString("%1_%2").arg(getType()).arg(getID());
}

void QFUsesResultsEvaluation::setFitResultSortPriority(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, bool sortPriority) {
    if (r!=NULL) {
        r->resultsSetSortPriority(transformResultID(resultID), getFitParamID(parameterID), sortPriority);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}

void QFUsesResultsEvaluation::setFitResultGroup(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, const QString &group) {
    if (r!=NULL) {
        r->resultsSetGroup(transformResultID(resultID), getFitParamID(parameterID), group);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}

void QFUsesResultsEvaluation::setFitResultLabel(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, const QString &label, const QString &label_richtext) {
    if (r!=NULL) {
        r->resultsSetLabel(transformResultID(resultID), getFitParamID(parameterID), label, label_richtext);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}

void QFUsesResultsEvaluation::setFitResultEvaluationGroup(QFRawDataRecord *r, const QString &resultID, const QString &group) {
    if (r!=NULL) {
        r->resultsSetEvaluationGroup(transformResultID(resultID), group);
        emitResultsChanged(r, resultID);
    }
}

void QFUsesResultsEvaluation::setFitResultEvaluationDescription(QFRawDataRecord *r, const QString &resultID, const QString &description) {
    if (r!=NULL) {
        r->resultsSetEvaluationDescription(transformResultID(resultID), description);
        emitResultsChanged(r, resultID);
    }
}

void QFUsesResultsEvaluation::resetAllFitResults(QFRawDataRecord *r, const QString &resultID) {
    if (!r) return;
    r->resultsClear(transformResultID(resultID));

}

void QFUsesResultsEvaluation::setFitValue(const QString &parameterID, double value, const QString &unit) {
    setFitValue(getHighlightedRecord(), getEvaluationResultID(), parameterID, value, unit);
}

void QFUsesResultsEvaluation::setFitValue(const QString &parameterID, double value)
{
    setFitValue(getHighlightedRecord(), getEvaluationResultID(), parameterID, value);
}


double QFUsesResultsEvaluation::getFitValue(const QString &parameterID) const
{
    return getFitValue(getHighlightedRecord(), getEvaluationResultID(), parameterID);
}

double QFUsesResultsEvaluation::getFitError(const QString &parameterID) const
{
    return getFitError(getHighlightedRecord(), getEvaluationResultID(), parameterID);
}

void QFUsesResultsEvaluation::setFitError(const QString &parameterID, double error)
{
    setFitError(getHighlightedRecord(), getEvaluationResultID(), parameterID, error);
}

void QFUsesResultsEvaluation::setFitFix(const QString &parameterID, bool fix)
{
    setFitFix(getHighlightedRecord(), getEvaluationResultID(), parameterID, fix);
}

bool QFUsesResultsEvaluation::getFitFix(const QString &parameterID) const
{
    return getFitFix(getHighlightedRecord(), getEvaluationResultID(), parameterID);
}

void QFUsesResultsEvaluation::setFitValue(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, double value, const QString& unit) {
    if (r!=NULL) {
        QString dsid=getParameterStoreID(parameterID);
        if (hasResults(r, resultID)) {
            setFitResultValue(r, resultID, parameterID, value, getFitError(r, resultID, parameterID), unit);
        } else {
            parameterStore[getParameterStoreID(parameterID)].value=value;
            parameterStore[getParameterStoreID(parameterID)].valueSet=true;
            //emitPropertiesChanged();
            project->setDataChanged();
        }

    }
}

void QFUsesResultsEvaluation::setInitFitValue(const QString &parameterID, double value) {
    const QString dsid=getParameterStoreID(parameterID);
    parameterStore[dsid].value=value;
    parameterStore[dsid].valueSet=true;
    //emitPropertiesChanged();
    project->setDataChanged();
}

void QFUsesResultsEvaluation::setInitFitValue(const QString &parameterID, double value, double error) {
    const QString dsid=getParameterStoreID(parameterID);
    parameterStore[dsid].value=value;
    parameterStore[dsid].valueSet=true;
    parameterStore[dsid].error=error;
    parameterStore[dsid].errorSet=true;
    //emitPropertiesChanged();
    project->setDataChanged();
}

void QFUsesResultsEvaluation::setInitFitValue(const QString &parameterID, double value, double error, bool fix) {
    const QString dsid=getParameterStoreID(parameterID);
    parameterStore[dsid].value=value;
    parameterStore[dsid].valueSet=true;
    parameterStore[dsid].error=error;
    parameterStore[dsid].errorSet=true;
    parameterStore[dsid].fix=fix;
    parameterStore[dsid].fixSet=true;
    //emitPropertiesChanged();
    project->setDataChanged();
}

void QFUsesResultsEvaluation::setInitFitValue(const QString &parameterID, double value, bool fix) {
    const QString dsid=getParameterStoreID(parameterID);
    parameterStore[dsid].value=value;
    parameterStore[dsid].valueSet=true;
    parameterStore[dsid].fix=fix;
    parameterStore[dsid].fixSet=true;
    //emitPropertiesChanged();
    project->setDataChanged();
}

void QFUsesResultsEvaluation::setInitFitValueError(const QString &parameterID, double error) {
    const QString dsid=getParameterStoreID(parameterID);
    parameterStore[dsid].error=error;
    parameterStore[dsid].errorSet=true;
    //emitPropertiesChanged();
    project->setDataChanged();
}

void QFUsesResultsEvaluation::setInitFitFix(const QString &parameterID, bool fix) {
    const QString dsid=getParameterStoreID(parameterID);
    parameterStore[dsid].fix=fix;
    parameterStore[dsid].fixSet=true;
    //emitPropertiesChanged();
    project->setDataChanged();
}

void QFUsesResultsEvaluation::setInitFitValueMin(const QString &parameterID, double min) {
    const QString dsid=getParameterStoreID(parameterID);
    parameterStore[dsid].min=min;
    parameterStore[dsid].minSet=true;
    //emitPropertiesChanged();
    project->setDataChanged();
}

void QFUsesResultsEvaluation::setInitFitValueMax(const QString &parameterID, double max) {
    const QString dsid=getParameterStoreID(parameterID);
    parameterStore[dsid].max=max;
    parameterStore[dsid].maxSet=true;
    //emitPropertiesChanged();
    project->setDataChanged();
}

void QFUsesResultsEvaluation::setFitValue(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, double value) {
    if (r!=NULL) {
        const QString dsid=getParameterStoreID(parameterID);
        if (hasResults(r, resultID)) {
            setFitResultValue(r, resultID, parameterID, value, getFitError(r, resultID, parameterID));
        } else {
            parameterStore[dsid].value=value;
            parameterStore[dsid].valueSet=true;
            //emitPropertiesChanged();
            project->setDataChanged();
        }

    }
}

void QFUsesResultsEvaluation::setFitResultValue(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, double value, const QString& unit) {
    if (r!=NULL) {
        //qDebug()<<transformResultID(resultID)<<": "<<getFitParamID(parameterID)<<" = "<<value<<unit;
        r->resultsSetNumberError(transformResultID(resultID), getFitParamID(parameterID), value, getFitError(r, resultID, parameterID), unit);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}

void QFUsesResultsEvaluation::setFitResultValueString(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, const QString& value) {
    if (r!=NULL) {
        r->resultsSetString(transformResultID(resultID), getFitParamID(parameterID), value);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}

void QFUsesResultsEvaluation::setFitResultValueInt(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, int64_t value, const QString& unit) {
    if (r!=NULL) {
        r->resultsSetInteger(transformResultID(resultID), getFitParamID(parameterID), value, unit);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}

void QFUsesResultsEvaluation::setFitResultValueBool(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, bool value) {
    if (r!=NULL) {
        r->resultsSetBoolean(transformResultID(resultID), getFitParamID(parameterID), value);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}

void QFUsesResultsEvaluation::setFitResultValue(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, double value, double error, const QString& unit) {
    if (r!=NULL) {
        r->resultsSetNumberError(transformResultID(resultID), getFitParamID(parameterID), value, error, unit);
        //qDebug()<<transformResultID(resultID)<<": "<<getFitParamID(parameterID)<<" = ("<<value<<"+/-"<<error<<")"<<unit;
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}

void QFUsesResultsEvaluation::setFitResultValue(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, double value, double error) {
    if (r!=NULL) {
        //qDebug()<<transformResultID(resultID)<<": "<<getFitParamID(parameterID)<<" = ("<<value<<"+/-"<<error<<")"<<r->resultsGet(transformResultID(resultID), getFitParamID(parameterID)).unit;
        r->resultsSetNumberError(transformResultID(resultID), getFitParamID(parameterID), value, error, r->resultsGet(transformResultID(resultID), getFitParamID(parameterID)).unit);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}

void QFUsesResultsEvaluation::setFitResultFitStatistics(QFRawDataRecord *record, const QString &evalID, const QFFitStatistics &result, const QString &prefix, const QString &group)
{
    setFitResultBasicFitStatistics(record, evalID, result, prefix, group);
}

void QFUsesResultsEvaluation::setFitResultBasicFitStatistics(QFRawDataRecord *record, const QString &evalID, const QFBasicFitStatistics &result, const QString &prefix, const QString &group)
{
    QString param="";
    setFitResultValue(record, evalID, param=prefix+"chisquared", result.residSqrSum, QString());
    setFitResultGroup(record, evalID, param, group);
    setFitResultLabel(record, evalID, param, tr("chi squared"), QString("<font size=\"+2\">&chi;<sup>2</sup></font>"));

    setFitResultValue(record, evalID, param=prefix+"chisquared_weighted", result.residWeightSqrSum, QString());
    setFitResultGroup(record, evalID, param, group);
    setFitResultLabel(record, evalID, param, tr("weighted chi squared"), QString("<font size=\"+2\">&chi;<sup>2</sup></font> (weighted)"));

    setFitResultValue(record, evalID, param=prefix+"residavg", result.residAverage, QString());
    setFitResultGroup(record, evalID, param, group);
    setFitResultLabel(record, evalID, param, tr("residual average"), QString("&lang;E&rang;"));

    setFitResultValue(record, evalID, param=prefix+"residavg_weighted", result.residWeightAverage, QString());
    setFitResultGroup(record, evalID, param, group);
    setFitResultLabel(record, evalID, param, tr("weighted residual average"), QString("&lang;E&rang; (weighted)"));

    setFitResultValue(record, evalID, param=prefix+"residstddev", result.residStdDev, QString());
    setFitResultGroup(record, evalID, param, group);
    setFitResultLabel(record, evalID, param, tr("residual stddev"), QString("&radic;&lang;E<sup><font size=\"+1\">2</font></sup>&rang; "));

    setFitResultValue(record, evalID, param=prefix+"residstddev_weighted", result.residWeightStdDev, QString());
    setFitResultGroup(record, evalID, param, group);
    setFitResultLabel(record, evalID, param, tr("weighted residual stddev"), QString("&radic;&lang;E<sup><font size=\"+1\">2</font></sup>&rang;  (weighted)"));

    setFitResultValue(record, evalID, param=prefix+"fitparams", result.fitparamN, QString());
    setFitResultGroup(record, evalID, param, group);
    setFitResultLabel(record, evalID, param, tr("fit params"));

    setFitResultValue(record, evalID, param=prefix+"det_cov", result.detCOV, QString());
    setFitResultGroup(record, evalID, param, group);
    setFitResultLabel(record, evalID, param, tr("det(fit cov matrix)"));


    setFitResultValue(record, evalID, param=prefix+"datapoints", result.dataSize, QString());
    setFitResultGroup(record, evalID, param, group);
    setFitResultLabel(record, evalID, param, tr("datapoints"));

    setFitResultValue(record, evalID, param=prefix+"dof", result.degFreedom, QString());
    setFitResultGroup(record, evalID, param, group);
    setFitResultLabel(record, evalID, param, tr("degrees of freedom"));

    setFitResultValue(record, evalID, param=prefix+"r2", result.Rsquared, QString());
    setFitResultGroup(record, evalID, param, group);
    setFitResultLabel(record, evalID, param, tr("R squared"), tr("R<sup>2</sup>"));

    setFitResultValue(record, evalID, param=prefix+"adjusted_r2", result.AdjustedRsquared,QString());
    setFitResultGroup(record, evalID, param, group);
    setFitResultLabel(record, evalID, param, tr("adjusted R squared"), tr("R<sup>2</sup><sub>adjusted</sub>"));

    setFitResultValue(record, evalID, param=prefix+"tss", result.TSS, QString());
    setFitResultGroup(record, evalID, param, group);
    setFitResultLabel(record, evalID, param, tr("total sum of squares"));

    setFitResultValue(record, evalID, param=prefix+"bayes_model_probability_paramrangesize", result.bayesProbabilityParamRangeSize, QString());
    setFitResultGroup(record, evalID, param, group);
    setFitResultLabel(record, evalID, param, tr("parameter range size for Bayes model probability"), tr("param_range<sub>Bayes</sub>"));

    setFitResultValue(record, evalID, param=prefix+"bayes_model_probability", result.bayesProbability, QString());
    setFitResultGroup(record, evalID, param, group);
    setFitResultLabel(record, evalID, param, tr("Bayes model probability"), tr("p<sub>Bayes</sub>(model|data)"));

    setFitResultValue(record, evalID, param=prefix+"bayes_model_probability_log10", result.bayesProbabilityLog10, QString());
    setFitResultGroup(record, evalID, param, group);
    setFitResultLabel(record, evalID, param, tr("log10 Bayes model probability"), tr("log<sub>10</sub>(p<sub>Bayes</sub>(model|data))"));

    setFitResultValue(record, evalID, param=prefix+"r2_weighted", result.RsquaredWeighted, QString());
    setFitResultGroup(record, evalID, param, group);
    setFitResultLabel(record, evalID, param, tr("weighted R squared"), tr("R<sup>2</sup> (weighted)"));

    setFitResultValue(record, evalID, param=prefix+"adjusted_r2_weighted", result.AdjustedRsquaredWeighted, QString());
    setFitResultGroup(record, evalID, param, group);
    setFitResultLabel(record, evalID, param, tr("weighted adjusted R squared"), tr("R<sup>2</sup><sub>adjusted</sub> (weighted)"));

    setFitResultValue(record, evalID, param=prefix+"aicc", result.AICc, QString());
    setFitResultGroup(record, evalID, param, group);
    setFitResultLabel(record, evalID, param, tr("Akaike's information criterion"), tr("AICc"));

    setFitResultValue(record, evalID, param=prefix+"aicc_weighted", result.AICcWeighted, QString());
    setFitResultGroup(record, evalID, param, group);
    setFitResultLabel(record, evalID, param, tr("weighted Akaike's information criterion"), tr("AICc (weighted)"));

    setFitResultValue(record, evalID, param=prefix+"bic", result.BIC, QString());
    setFitResultGroup(record, evalID, param, group);
    setFitResultLabel(record, evalID, param, tr("Bayesian information criterion"), tr("BIC"));

    setFitResultValue(record, evalID, param=prefix+"bic_weighted", result.BICweighted, QString());
    setFitResultGroup(record, evalID, param, group);
    setFitResultLabel(record, evalID, param, tr("weighted Bayesian information criterion"), tr("BIC (weighted)"));

    setFitResultValue(record, evalID, param=prefix+"max_rel_param_error", result.maxRelParamError, QString());
    setFitResultGroup(record, evalID, param, group);
    setFitResultLabel(record, evalID, param, tr("maximum rel. parameter error"), tr("max<sub>P</sub>(&sigma;<sub>P</sub>/|P|)"));
}

void QFUsesResultsEvaluation::setFitResultError(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, double error) {
    if (r!=NULL) {
        r->resultsSetNumberErrorError(transformResultID(resultID), getFitParamID(parameterID), error);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}

void QFUsesResultsEvaluation::setFitResultValueNumberArray(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, double *value, double *error, uint32_t N, const QString &unit) {
    if (r!=NULL) {
        r->resultsSetNumberErrorList(transformResultID(resultID), getFitParamID(parameterID), value, error, N, unit);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}

void QFUsesResultsEvaluation::setFitResultValueNumberArray(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, double *value, uint32_t N, const QString &unit) {
    if (r!=NULL) {
        r->resultsSetNumberList(transformResultID(resultID), getFitParamID(parameterID), value, N, unit);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}

QVector<double> QFUsesResultsEvaluation::getFitValueNumberArray(const QFRawDataRecord *r, const QString &resultID, const QString &parameterID) const {
    QVector<double> res;

    if (r) {
        QFUsesResultsEvaluation::FitParameterDefault defVal;
        if (getParameterDefault(r, resultID, parameterID, defVal)) {
            res=defVal.valueVector;
        }

        if (hasResults(r, resultID)) {
            QString tresultID=transformResultID(resultID);
            QString pid=getFitParamID(parameterID);
            if (r->resultsExists(tresultID, pid)) res=r->resultsGetAsDoubleList(tresultID, pid);
        }
    }
    return res;
}


QVector<double> QFUsesResultsEvaluation::getFitValueErrorArray(const QFRawDataRecord *r, const QString &resultID, const QString &parameterID) const {
    QVector<double> res;

    if (r) {
        QFUsesResultsEvaluation::FitParameterDefault defVal;
        if (getParameterDefault(r, resultID, parameterID, defVal)) {
            res=defVal.errorVector;
        }

        if (hasResults(r, resultID)) {
            QString tresultID=transformResultID(resultID);
            QString pid=getFitParamID(parameterID);
            if (r->resultsExists(tresultID, pid)) res=r->resultsGetErrorAsDoubleList(tresultID, pid);
        }
    }
    return res;
}

double QFUsesResultsEvaluation::getFitValue(const QFRawDataRecord *r, const QString &resultID, const QString &parameterID) const {
    //qDebug()<<"getFitValue("<<resultID<<", "<<parameterID<<")";
    double res=0;
    res=fitParamGlobalSettings->value(getParameterStoreIDInQSettings(parameterID), res).toDouble();
    res=fitParamSettings->value(getParameterStoreIDInQSettings(parameterID), res).toDouble();
    QFUsesResultsEvaluation::FitParameterDefault defVal;
    if (getParameterDefault(r, resultID, parameterID, defVal)) {
        res=defVal.value;
    }    QString psID=getParameterStoreID(parameterID);
    if (parameterStore.contains(psID)) {
        if (parameterStore[psID].valueSet) {
            res=parameterStore[psID].value;
        }
    }

    double sval=res, serr=0;
    if (hasSpecial(r, resultID, parameterID, sval, serr)) {
        //qDebug()<<"  - hasSpecial "<<sval;
        res=sval;
    }
    if (r && hasResults(r, resultID)) {
        QString tresultID=transformResultID(resultID);
        QString pid=getFitParamID(parameterID);
        //qDebug()<<"  - hasResults "<<tresultID<<pid;
        //qDebug()<<"               "<<r->resultsCalcNames(tresultID);
        if (r->resultsExists(tresultID, pid)) {
            res=r->resultsGetAsDouble(tresultID, pid);
            //qDebug()<<"    - resultsExists "<<res;
        }
    }
    //qDebug()<<"  getFitValue("<<resultID<<", "<<parameterID<<") = "<<res;
    return res;
}

bool QFUsesResultsEvaluation::fitValueExists(QFRawDataRecord *r, const QString &resultID, const QString &parameterID) const
{
    if (r && hasResults(r, resultID)) {
        QString tresultID=transformResultID(resultID);
        QString pid=getFitParamID(parameterID);
        if (r->resultsExists(tresultID, pid)) {
            return true;
        }
    }
    //qDebug()<<"  getFitValue("<<resultID<<", "<<parameterID<<") = "<<res;
    return false;
}


double QFUsesResultsEvaluation::getFitError(const QFRawDataRecord *r, const QString &resultID, const QString &parameterID) const {
    double sval=0, serr=0;
    if (hasResults(r, resultID)) {
        if (r!=NULL) {
            return r->resultsGetErrorAsDouble(transformResultID(resultID), getFitParamID(parameterID));
        }
    }
    if (hasSpecial(r, resultID, parameterID, sval, serr)) {
       return serr;
    }

    QString psID=getParameterStoreID(parameterID);
    if (parameterStore.contains(psID)) {
        if (parameterStore[psID].errorSet) {
            return parameterStore[psID].error;
        }
    }

    QFUsesResultsEvaluation::FitParameterDefault defVal;
    if (getParameterDefault(r, resultID, parameterID, defVal)) {
        return defVal.error;
    }
    return 0.0;
}

void QFUsesResultsEvaluation::setFitError(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, double error) {
    if (r!=NULL) {
        QString dsid=getParameterStoreID(parameterID);
        if (hasResults(r, resultID)) {
            setFitResultError(r, resultID, parameterID, error);
        } else {
            parameterStore[dsid].error=error;
            parameterStore[dsid].errorSet=true;
            //emitPropertiesChanged();
            project->setDataChanged();
        }
    }
}

void QFUsesResultsEvaluation::setFitFix(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, bool fix) {
    if (r!=NULL) {
        QString dsid=getParameterStoreID(parameterID);
        if (hasResults(r, resultID)) {
            setFitResultFix(r, resultID, parameterID, fix);
        } else {
            parameterStore[dsid].fix=fix;
            parameterStore[dsid].fixSet=true;
            //emitPropertiesChanged();
            project->setDataChanged();
        }

    }
}

void QFUsesResultsEvaluation::setFitResultFix(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, bool fix) {
    if (r!=NULL) {
        QString unit="";
        r->resultsSetBoolean(transformResultID(resultID), getFitParamFixID(parameterID), fix);
        emitResultsChanged(r, resultID, getFitParamFixID(parameterID));
    }
}

bool QFUsesResultsEvaluation::getFitFix(const QFRawDataRecord *r, const QString &resultID, const QString &parameterID) const {
    bool res=false;
    res=fitParamGlobalSettings->value(getParameterStoreIDInQSettings(parameterID)+"_fix", res).toBool();
    res=fitParamSettings->value(getParameterStoreIDInQSettings(parameterID)+"_fix", res).toBool();
    QFUsesResultsEvaluation::FitParameterDefault defVal;
    if (getParameterDefault(r, resultID, parameterID, defVal)) {
        res=defVal.fix;
    }
    QString psID=getParameterStoreID(parameterID);
    if (parameterStore.contains(psID)) {
        if (parameterStore[psID].fixSet) {
            res=parameterStore[psID].fix;
        }
    }

    if (r && hasResults(r, resultID)) {
        QString en=transformResultID(resultID);
        QString pid=getFitParamFixID(parameterID);
        if (r->resultsExists(en, pid)) res=r->resultsGetAsDouble(en, pid);
    }
    return res;
}

QString QFUsesResultsEvaluation::getParameterName(int model, int id, bool html) const
{
    Q_UNUSED(model);
    Q_UNUSED(id);
    Q_UNUSED(html);
    return QString("");
}

QString QFUsesResultsEvaluation::getParameterUnit(int model, int id, bool html) const
{
    Q_UNUSED(model);
    Q_UNUSED(id);
    Q_UNUSED(html);
    return QString("");
}

int QFUsesResultsEvaluation::getParameterCount(int model) const
{
    Q_UNUSED(model);
    return 0;
}

QString QFUsesResultsEvaluation::getParameterID(int model, int param) const
{
    Q_UNUSED(model);
    Q_UNUSED(param);
    return QString("");
}

QString QFUsesResultsEvaluation::getParameterStoreID(QString parameter) const {
    return parameter;
}

QString QFUsesResultsEvaluation::getParameterStoreIDInQSettings(QString parameter) const {
    return parameter;
}


void QFUsesResultsEvaluation::intWriteData(QXmlStreamWriter& w) const {
    w.writeStartElement("default_parameters");
    QHashIterator<QString, FitParameter> i(parameterStore);
    while (i.hasNext()) {
        i.next();
        w.writeStartElement("parameter");
        w.writeAttribute("id", i.key());
        if (i.value().valueSet) w.writeAttribute("value", CDoubleToQString(i.value().value));
        if (i.value().errorSet) w.writeAttribute("error", CDoubleToQString(i.value().error));
        if (i.value().minSet) w.writeAttribute("min", CDoubleToQString(i.value().min));
        if (i.value().maxSet) w.writeAttribute("max", CDoubleToQString(i.value().max));
        if (i.value().fixSet) w.writeAttribute("fix", QString((i.value().fix)?QString("1"):QString("0")));
        w.writeEndElement();
    }
    w.writeEndElement();
}

QString QFUsesResultsEvaluation::transformResultID(const QString &resultID) const {
    return resultID;
}

QString QFUsesResultsEvaluation::getFitParamID(QString fitparam) const {
    return QString("%1").arg(fitparam);
}

QString QFUsesResultsEvaluation::getFitParamFixID(QString fitparam) const {
    return QString("%1_fix").arg(fitparam);
}

bool QFUsesResultsEvaluation::hasSpecial(const QFRawDataRecord *r, const QString &resultID, const QString &paramid, double &value, double &error) const {
    Q_UNUSED(r);
    Q_UNUSED(resultID);
    Q_UNUSED(paramid);
    Q_UNUSED(value);
    Q_UNUSED(error);
    return false;
}

bool QFUsesResultsEvaluation::getParameterDefault(const QFRawDataRecord *r, const QString &resultID, const QString &parameterID, QFUsesResultsEvaluation::FitParameterDefault &defaultValue) const {
    Q_UNUSED(r);
    Q_UNUSED(resultID);
    Q_UNUSED(parameterID);
    Q_UNUSED(defaultValue);

    return false;
}

void QFUsesResultsEvaluation::intReadData(QDomElement* e) {

    QDomElement e1=e->firstChildElement("default_parameters");

    // read <parameter> tag
    QDomElement elt=e1.firstChildElement("parameter");
    parameterStore.clear();
    for (; !elt.isNull(); elt = elt.nextSiblingElement("parameter")) {
        if (elt.hasAttribute("id") && ( elt.hasAttribute("value") || elt.hasAttribute("fix") || elt.hasAttribute("min") || elt.hasAttribute("max") ) ) {
            QString a=elt.attribute("id");
            FitParameter p;
            if (elt.hasAttribute("fix")) {
                p.fix=(elt.attribute("fix").toLower()!="0");
                p.fixSet=true;
            }
            if (elt.hasAttribute("value")) {
                p.value=CQStringToDouble(elt.attribute("value"));
                p.valueSet=true;
            }
            if (elt.hasAttribute("error")) {
                p.error=CQStringToDouble(elt.attribute("error"));
                p.errorSet=true;
            }
            if (elt.hasAttribute("min")) {
                p.min=CQStringToDouble(elt.attribute("min"));
                p.minSet=true;
            }
            if (elt.hasAttribute("max")) {
                p.max=CQStringToDouble(elt.attribute("max"));
                p.maxSet=true;
            }
            parameterStore[a]=p;
        }
    }
}


QFUsesResultsEvaluation::FitParameterDefault::FitParameterDefault() {
    value=0;
    error=0;
    min=-DBL_MAX;
    max=DBL_MAX;
    fix=false;
}
