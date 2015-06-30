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

#include "qffitresultsbyindexasvectorevaluation.h"

QFFitResultsByIndexAsVectorEvaluation::QFFitResultsByIndexAsVectorEvaluation(const QString &fitFunctionPrefix, QFProject *parent, bool showRDRList, bool useSelection):
    QFFitResultsByIndexEvaluation(fitFunctionPrefix, parent, showRDRList, useSelection)
{
}

QFFitResultsByIndexAsVectorEvaluation::~QFFitResultsByIndexAsVectorEvaluation() {
}

void QFFitResultsByIndexAsVectorEvaluation::setPresetProperty(const QString &id, const QVariant &data, bool usereditable, bool visible)
{
    QFFitResultsByIndexEvaluation::setPresetProperty(id, data, usereditable, visible);
}

QString QFFitResultsByIndexAsVectorEvaluation::transformResultID(const QString &resultID) const {
    QRegExp rx("(.+)(\\_run|\\_r)(.+)");
    rx.setCaseSensitivity(Qt::CaseInsensitive);
    if (rx.indexIn(resultID)>-1) {
        bool runOK=false;
        int run=rx.cap(3).toInt(&runOK);
        if (rx.cap(3).toLower()=="avg" || !runOK || (runOK && run<0)) return resultID;
        if (runOK && run>=0) return rx.cap(1);
    }
    return resultID;
}

QString QFFitResultsByIndexAsVectorEvaluation::getParamNameLocalStore(const QString &paramID) const {
    return paramID+"_islocal";
}

bool QFFitResultsByIndexAsVectorEvaluation::isParamNameLocalStore(const QString &paramID) const {
    return paramID.endsWith("_islocal");
}

void QFFitResultsByIndexAsVectorEvaluation::setFitResultFitStatisticsInResultStore(QFRawDataRecord::QFFitFitResultsStore &fitresult, const QFFitStatistics &result, const QString &group, const QString &prefix) const
{
    QString param="";
    fitresult.resultsSetNumberAndBool( param=prefix+"chisquared",  result.residSqrSum, QString(""), getParamNameLocalStore(param),  true, fitresult.index>=0);
    fitresult.resultsSetGroupAndLabels( param, group, tr("chi squared (RSS)"), QString("<font size=\"+2\">&chi;<sup>2</sup></font>"));

    fitresult.resultsSetNumberAndBool( param=prefix+"chisquared_weighted",  result.residWeightSqrSum, QString(""), getParamNameLocalStore(param),  true, fitresult.index>=0);
    fitresult.resultsSetGroupAndLabels( param, group, tr("weighted chi squared"), QString("<font size=\"+2\">&chi;<sup>2</sup></font> (weighted)"));

    fitresult.resultsSetNumberAndBool( param=prefix+"residavg",  result.residAverage, QString(""), getParamNameLocalStore(param),  true, fitresult.index>=0);
    fitresult.resultsSetGroupAndLabels( param, group, tr("residual average"), QString("&lang;E&rang;"));

    fitresult.resultsSetNumberAndBool( param=prefix+"residavg_weighted",  result.residWeightAverage, QString(""), getParamNameLocalStore(param),  true, fitresult.index>=0);
    fitresult.resultsSetGroupAndLabels( param, group, tr("weighted residual average"), QString("&lang;E&rang; (weighted)"));

    fitresult.resultsSetNumberAndBool( param=prefix+"residstddev",  result.residStdDev, QString(""), getParamNameLocalStore(param),  true, fitresult.index>=0);
    fitresult.resultsSetGroupAndLabels( param, group, tr("residual stddev"), QString("&radic;&lang;E<sup><font size=\"+1\">2</font></sup>&rang; "));

    fitresult.resultsSetNumberAndBool( param=prefix+"residstddev_weighted",  result.residWeightStdDev, QString(""), getParamNameLocalStore(param),  true, fitresult.index>=0);
    fitresult.resultsSetGroupAndLabels( param, group, tr("weighted residual stddev"), QString("&radic;&lang;E<sup><font size=\"+1\">2</font></sup>&rang;  (weighted)"));

    fitresult.resultsSetIntegerAndBool( param=prefix+"fitparams",  result.fitparamN, QString(""), getParamNameLocalStore(param),  true, fitresult.index>=0);
    fitresult.resultsSetGroupAndLabels( param, group, tr("fit params"));

    fitresult.resultsSetIntegerAndBool( param=prefix+"det_cov",  result.detCOV, QString(""), getParamNameLocalStore(param),  true, fitresult.index>=0);
    fitresult.resultsSetGroupAndLabels( param, group, tr("det(fit cov matrix)"));





    fitresult.resultsSetIntegerAndBool( param=prefix+"datapoints",  result.dataSize, QString(""), getParamNameLocalStore(param),  true, fitresult.index>=0);
    fitresult.resultsSetGroupAndLabels( param, group, tr("datapoints"));

    fitresult.resultsSetIntegerAndBool( param=prefix+"dof",  result.degFreedom, QString(""), getParamNameLocalStore(param),  true, fitresult.index>=0);
    fitresult.resultsSetGroupAndLabels( param, group, tr("degrees of freedom (DOF)"));

    fitresult.resultsSetNumberAndBool( param=prefix+"r2",  result.Rsquared, QString(""), getParamNameLocalStore(param),  true, fitresult.index>=0);
    fitresult.resultsSetGroupAndLabels( param, group, tr("R squared (R2)"), tr("R<sup>2</sup>"));

    fitresult.resultsSetNumberAndBool( param=prefix+"adjusted_r2",  result.AdjustedRsquared, QString(""), getParamNameLocalStore(param),  true, fitresult.index>=0);
    fitresult.resultsSetGroupAndLabels( param, group, tr("adjusted R squared (R2)"), tr("R<sup>2</sup><sub>adjusted</sub>"));

    fitresult.resultsSetNumberAndBool( param=prefix+"tss",  result.TSS, QString(""), getParamNameLocalStore(param),  true, fitresult.index>=0);
    fitresult.resultsSetGroupAndLabels( param, group, tr("total sum of squares (TSS)"));



    fitresult.resultsSetNumberAndBool( param=prefix+"bayes_model_probability_paramrangesize",  result.bayesProbabilityParamRangeSize, QString(""), getParamNameLocalStore(param),  true, fitresult.index>=0);
    fitresult.resultsSetGroupAndLabels( param, group, tr("Bayes model probability"), tr("p<sub>Bayes</sub>(model|data)"));

    fitresult.resultsSetNumberAndBool( param=prefix+"bayes_model_probability",  result.bayesProbability, QString(""), getParamNameLocalStore(param),  true, fitresult.index>=0);
    fitresult.resultsSetGroupAndLabels( param, group, tr("parameter range size for Bayes model probability"), tr("param_range<sub>Bayes</sub>"));
    fitresult.resultsSetNumberAndBool( param=prefix+"bayes_model_probability_log10",  result.bayesProbabilityLog10, QString(""), getParamNameLocalStore(param),  true, fitresult.index>=0);
    fitresult.resultsSetGroupAndLabels( param, group, tr("log10 Bayes model probability"), tr("log<sub>10</sub>(p<sub>Bayes</sub>(model|data))"));

    fitresult.resultsSetNumberAndBool( param=prefix+"r2_weighted",  result.RsquaredWeighted, QString(""), getParamNameLocalStore(param),  true, fitresult.index>=0);
    fitresult.resultsSetGroupAndLabels( param, group, tr("weighted R squared"), tr("R<sup>2</sup> (weighted)"));

    fitresult.resultsSetNumberAndBool( param=prefix+"adjusted_r2_weighted",  result.AdjustedRsquaredWeighted, QString(""), getParamNameLocalStore(param),  true, fitresult.index>=0);
    fitresult.resultsSetGroupAndLabels( param, group, tr("weighted adjusted R squared"), tr("R<sup>2</sup><sub>adjusted</sub> (weighted)"));

    fitresult.resultsSetNumberAndBool( param=prefix+"aicc",  result.AICc, QString(""), getParamNameLocalStore(param),  true, fitresult.index>=0);
    fitresult.resultsSetGroupAndLabels( param, group, tr("Akaike's information criterion"), tr("AICc"));

    fitresult.resultsSetNumberAndBool( param=prefix+"aicc_weighted",  result.AICcWeighted, QString(""), getParamNameLocalStore(param),  true, fitresult.index>=0);
    fitresult.resultsSetGroupAndLabels( param, group, tr("weighted Akaike's information criterion"), tr("AICc (weighted)"));


    fitresult.resultsSetNumberAndBool( param=prefix+"bic",  result.BIC, QString(""), getParamNameLocalStore(param),  true, fitresult.index>=0);
    fitresult.resultsSetGroupAndLabels( param, group, tr("Bayesian information criterion"), tr("BIC"));

    fitresult.resultsSetNumberAndBool( param=prefix+"bic_weighted",  result.BICweighted, QString(""), getParamNameLocalStore(param),  true, fitresult.index>=0);
    fitresult.resultsSetGroupAndLabels( param, group, tr("weighted Bayesian information criterion"), tr("BIC (weighted)"));

    fitresult.resultsSetNumberAndBool( param=prefix+"max_rel_param_error",  result.maxRelParamError, QString(""), getParamNameLocalStore(param),  true, fitresult.index>=0);
    fitresult.resultsSetGroupAndLabels( param, group, tr("maximum rel. parameter error"), tr("max<sub>P</sub>(&sigma;<sub>P</sub>/|P|)"));

}

void QFFitResultsByIndexAsVectorEvaluation::setFitResultFitStatisticsInVector(QFRawDataRecord *record, const QString& evalID, int run, const QFFitStatistics &fit_stat, const QString &prefix, const QString &group) const
{
    if (run<0) record->resultsSetFitStatistics(fit_stat, evalID, prefix, group);
    else {
        QString param="";
        record->resultsSetInNumberListAndBool(evalID, param=prefix+"chisquared", run, fit_stat.residSqrSum, QString(""), getParamNameLocalStore(param),  true);
        record->resultsSetGroupAndLabels(evalID, param, group, tr("chi squared"), QString("<font size=\"+2\">&chi;<sup>2</sup></font>"));

        record->resultsSetInNumberListAndBool(evalID, param=prefix+"chisquared_weighted", run, fit_stat.residWeightSqrSum, QString(""), getParamNameLocalStore(param),  true);
        record->resultsSetGroupAndLabels(evalID, param, group, tr("weighted chi squared"), QString("<font size=\"+2\">&chi;<sup>2</sup></font> (weighted)"));

        record->resultsSetInNumberListAndBool(evalID, param=prefix+"residavg", run, fit_stat.residAverage, QString(""), getParamNameLocalStore(param),  true);
        record->resultsSetGroupAndLabels(evalID, param, group, tr("residual average"), QString("&lang;E&rang;"));

        record->resultsSetInNumberListAndBool(evalID, param=prefix+"residavg_weighted", run, fit_stat.residWeightAverage, QString(""), getParamNameLocalStore(param),  true);
        record->resultsSetGroupAndLabels(evalID, param, group, tr("weighted residual average"), QString("&lang;E&rang; (weighted)"));

        record->resultsSetInNumberListAndBool(evalID, param=prefix+"residstddev", run, fit_stat.residStdDev, QString(""), getParamNameLocalStore(param),  true);
        record->resultsSetGroupAndLabels(evalID, param, group, tr("residual stddev"), QString("&radic;&lang;E<sup><font size=\"+1\">2</font></sup>&rang; "));

        record->resultsSetInNumberListAndBool(evalID, param=prefix+"det_cov", run, fit_stat.residStdDev, QString(""), getParamNameLocalStore(param),  true);
        record->resultsSetGroupAndLabels(evalID, param, group, tr("det(fit cov matrix)"));

        record->resultsSetInNumberListAndBool(evalID, param=prefix+"residstddev_weighted", run, fit_stat.residWeightStdDev, QString(""), getParamNameLocalStore(param),  true);
        record->resultsSetGroupAndLabels(evalID, param, group, tr("weighted residual stddev"), QString("&radic;&lang;E<sup><font size=\"+1\">2</font></sup>&rang;  (weighted)"));

        record->resultsSetInIntegerListAndBool(evalID, param=prefix+"fitparams", run, fit_stat.fitparamN, QString(""), getParamNameLocalStore(param),  true);
        record->resultsSetGroupAndLabels(evalID, param, group, tr("fit params"));

        record->resultsSetInIntegerListAndBool(evalID, param=prefix+"datapoints", run, fit_stat.dataSize, QString(""), getParamNameLocalStore(param),  true);
        record->resultsSetGroupAndLabels(evalID, param, group, tr("datapoints"));

        record->resultsSetInIntegerListAndBool(evalID, param=prefix+"dof", run, fit_stat.degFreedom, QString(""), getParamNameLocalStore(param),  true);
        record->resultsSetGroupAndLabels(evalID, param, group, tr("degrees of freedom"));

        record->resultsSetInNumberListAndBool(evalID, param=prefix+"r2", run, fit_stat.Rsquared, QString(""), getParamNameLocalStore(param),  true);
        record->resultsSetGroupAndLabels(evalID, param, group, tr("R squared"), tr("R<sup>2</sup>"));

        record->resultsSetInNumberListAndBool(evalID, param=prefix+"tss", run, fit_stat.TSS, QString(""), getParamNameLocalStore(param),  true);
        record->resultsSetGroupAndLabels(evalID, param, group, tr("total sum of squares"));

        record->resultsSetInNumberListAndBool(evalID, param=prefix+"bayes_model_probability_paramrangesize", run, fit_stat.bayesProbabilityParamRangeSize, QString(""), getParamNameLocalStore(param),  true);
        record->resultsSetGroupAndLabels(evalID, param, group, tr("Bayes model probability"), tr("p<sub>Bayes</sub>(model|data)"));

        record->resultsSetInNumberListAndBool(evalID, param=prefix+"bayes_model_probability_log10", run, fit_stat.bayesProbabilityLog10, QString(""), getParamNameLocalStore(param),  true);
        record->resultsSetGroupAndLabels(evalID, param, group, tr("log10 Bayes model probability"), tr("log<sub>10</sub>(p<sub>Bayes</sub>(model|data))"));


        record->resultsSetInNumberListAndBool(evalID, param=prefix+"bayes_model_probability", run, fit_stat.bayesProbability, QString(""), getParamNameLocalStore(param),  true);
        record->resultsSetGroupAndLabels(evalID, param, group, tr("parameter range size for Bayes model probability"), tr("param_range<sub>Bayes</sub>"));

        record->resultsSetInNumberListAndBool(evalID, param=prefix+"r2_weighted", run, fit_stat.RsquaredWeighted, QString(""), getParamNameLocalStore(param),  true);
        record->resultsSetGroupAndLabels(evalID, param, group, tr("weighted R squared"), tr("R<sup>2</sup> (weighted)"));

        record->resultsSetInNumberListAndBool(evalID, param=prefix+"aicc", run, fit_stat.AICc, QString(""), getParamNameLocalStore(param),  true);
        record->resultsSetGroupAndLabels(evalID, param, group, tr("Akaike's information criterion"), tr("AICc"));

        record->resultsSetInNumberListAndBool(evalID, param=prefix+"aicc_weighted", run, fit_stat.AICcWeighted, QString(""), getParamNameLocalStore(param),  true);
        record->resultsSetGroupAndLabels(evalID, param, group, tr("weighted Akaike's information criterion"), tr("AICc (weighted)"));

        record->resultsSetInNumberListAndBool(evalID, param=prefix+"bic", run, fit_stat.BIC, QString(""), getParamNameLocalStore(param),  true);
        record->resultsSetGroupAndLabels(evalID, param, group, tr("Bayesian information criterion"), tr("BIC"));

        record->resultsSetInNumberListAndBool(evalID, param=prefix+"bic_weighted", run, fit_stat.BICweighted, QString(""), getParamNameLocalStore(param),  true);
        record->resultsSetGroupAndLabels(evalID, param, group, tr("weighted Bayesian information criterion"), tr("BIC (weighted)"));

        record->resultsSetInNumberListAndBool(evalID, param=prefix+"max_rel_param_error", run, fit_stat.maxRelParamError, QString(""), getParamNameLocalStore(param),  true);
        record->resultsSetGroupAndLabels(evalID, param, group, tr("maximum rel. parameter error"), tr("max<sub>P</sub>(&sigma;<sub>P</sub>/|P|)"));

    }
}



void QFFitResultsByIndexAsVectorEvaluation::resetDefaultFitValue(const QString& id,  QFRawDataRecord *r) {
    resetDefaultFitValue(r, getEvaluationResultID(r), id);
}

void QFFitResultsByIndexAsVectorEvaluation::resetDefaultFitValue(QFRawDataRecord* r, const QString& resultID, const QString& id) {
    int index=getIndexFromEvaluationResultID(resultID);
    if (index<0) {
        QFFitResultsEvaluation::resetDefaultFitValue(r, resultID, id);
    } else {
        QString en=transformResultID(resultID);
        QString pid=getFitParamID(id);
        //if (hasFit(r, index)) {
            if (r->resultsExists(en, pid)) {
                bool doEmit=r->isEmitResultsChangedEnabled();
                r->disableEmitResultsChanged();
                r->resultsSetInNumberErrorList(en, pid, index, 0, 0);
                r->resultsSetInBooleanList(en, getParamNameLocalStore(pid), index, false);
                if (doEmit) r->enableEmitResultsChanged(true);
            }
        //}
    }
}

void QFFitResultsByIndexAsVectorEvaluation::resetAllDefaultFitFix(QFRawDataRecord *r, const QString &id)
{
    bool doEmit=r->isEmitResultsChangedEnabled();
    r->disableEmitResultsChanged();
    for (int index=getIndexMin(r); index<=getIndexMax(r); index++) {
        resetDefaultFitFix(r, getEvaluationResultID(index, r), id);
    }
    if (doEmit) r->enableEmitResultsChanged(true);
}

void QFFitResultsByIndexAsVectorEvaluation::resetAllDefaultFitValue(QFRawDataRecord *r, const QString &id)
{
    bool doEmit=r->isEmitResultsChangedEnabled();
    r->disableEmitResultsChanged();
    for (int index=getIndexMin(r); index<=getIndexMax(r); index++) {
        resetDefaultFitValue(r, getEvaluationResultID(index, r), id);
    }
    if (doEmit) r->enableEmitResultsChanged(true);
}

void QFFitResultsByIndexAsVectorEvaluation::resetDefaultFitFix(const QString& id,  QFRawDataRecord *r) {
    resetDefaultFitFix(r, getEvaluationResultID(r), id);
}

void QFFitResultsByIndexAsVectorEvaluation::resetDefaultFitFix(QFRawDataRecord* r, const QString& resultID, const QString& id) {
    int index=getIndexFromEvaluationResultID(resultID);
    if (index<0) {
        QFFitResultsEvaluation::resetDefaultFitFix(r, resultID, id);
    } else {
        QString en=transformResultID(resultID);
        QString pid=getFitParamFixID(id);
        //if (hasFit(r, index)) {
            if (r->resultsExists(en, pid)) {
                bool doEmit=r->isEmitResultsChangedEnabled();
                r->disableEmitResultsChanged();
                r->resultsSetInBooleanList(en, pid, index, false);
                r->resultsSetInBooleanList(en, getParamNameLocalStore(pid), index, false);
                if (doEmit) r->enableEmitResultsChanged(true);
            }
        //}
    }
}


/*! \brief reset the all fit results to the initial/global/default value in the current file, but all indexs */
void QFFitResultsByIndexAsVectorEvaluation::resetAllFitResultsCurrentFileAllIndices() {
    QFRawDataRecord* r=getHighlightedRecord();
    if (!r) return;
    bool doEmit=r->isEmitResultsChangedEnabled();
    r->disableEmitResultsChanged();
    for(int idx=getIndexMin(r); idx<=getIndexMax(r); idx++) {
        resetAllFitResults(r, getEvaluationResultID(idx, r));
    }
    if (doEmit) r->enableEmitResultsChanged(true);
}




/*! \brief reset all fit results to the initial/global/default value in all files and all indexs */
void QFFitResultsByIndexAsVectorEvaluation::resetAllFitResultsAllFilesAllIndices() {
    QList<QPointer<QFRawDataRecord> > recs=getApplicableRecords();
    for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* r=recs[i];
        if (!r) return;
        bool doEmit=r->isEmitResultsChangedEnabled();
        r->disableEmitResultsChanged();
        for(int idx=getIndexMin(r); idx<=getIndexMax(r); idx++) {
            //qDebug()<<"resetAllFitResultsAllFilesAllIndices(): idx="<<idx<<" getEvaluationResultID(idx)="<<getEvaluationResultID(idx);
            resetAllFitResults(r, getEvaluationResultID(idx, r));
        }

        if (doEmit) r->enableEmitResultsChanged(true);
    }
}

void QFFitResultsByIndexAsVectorEvaluation::resetAllFitResultsCurrent(QFRawDataRecord *rec) {
    QFRawDataRecord *r=rec;
    if (!r) r=getHighlightedRecord();

    if (r) {
        int index=getCurrentIndex();
        QString resultID=getEvaluationResultID(index, r);
        bool doEmit=r->isEmitResultsChangedEnabled();
        r->disableEmitResultsChanged();
        QString tresultID=transformResultID(resultID);
        if (index<0) {
            QFFitResultsByIndexEvaluation::resetAllFitResults(r, resultID);
        } else {
            QList<QString> resids=r->resultsCalcNames(tresultID);
            for (int i=0; i<resids.size(); i++) {
                //qDebug()<<"resetAllFitResults():  resids["<<i<<"]="<<resids[i]<<"  tresultID="<<tresultID<<"  index="<<index;
                if (isParamNameLocalStore(resids[i])) {
                    r->resultsSetInBooleanList(tresultID, resids[i], index, false);
                } else {
                    r->resultsResetInList(tresultID, resids[i], index);
                }
                //r->resultsRemove(tresultID, resids[i], false);
            }

        }
        if (doEmit) r->enableEmitResultsChanged(true);
    }
}

/*! \brief reset all parameters to the initial/global/default value in all files and all indexs */
void QFFitResultsByIndexAsVectorEvaluation::resetAllFitValue()  {
    QList<QPointer<QFRawDataRecord> > recs=getApplicableRecords();
    for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* r=recs[i];
        if (r) {
            QFFitFunction* f=getFitFunction(r);
            bool doEmit=r->isEmitResultsChangedEnabled();
            r->disableEmitResultsChanged();
            for(int idx=getIndexMin(r); idx<=getIndexMax(r); idx++) {
                QString resultID=getEvaluationResultID(idx, r);
                for (int j=0; j<f->paramCount(); j++) {
                    QString id=f->getParameterID(j);
                    resetDefaultFitValue(r, resultID, id);
                }
            }
            if (doEmit) r->enableEmitResultsChanged(true);
        }
    }
}




/*! \brief reset all parameters to the initial/global/default fix in all files and all indexs */
void QFFitResultsByIndexAsVectorEvaluation::resetAllFitFix() {
    QList<QPointer<QFRawDataRecord> > recs=getApplicableRecords();
    for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* r=recs[i];
        if (r) {
            QFFitFunction* f=getFitFunction(r);
            bool doEmit=r->isEmitResultsChangedEnabled();
            r->disableEmitResultsChanged();
            for(int idx=getIndexMin(r); idx<=getIndexMax(r); idx++) {
                QString resultID=transformResultID(getEvaluationResultID(idx, r));
                for (int j=0; j<f->paramCount(); j++) {
                    QString id=f->getParameterID(j);
                    resetDefaultFitFix(r, resultID, id);
                }
            }
            if (doEmit) r->enableEmitResultsChanged(true);
        }
    }
}

void QFFitResultsByIndexAsVectorEvaluation::resetAllFitResults(QFRawDataRecord *r, const QString& resultID) {
    //QFRawDataRecord* r=getHighlightedRecord();
    //QString resultID=getEvaluationResultID();
    bool doEmit=r->isEmitResultsChangedEnabled();
    r->disableEmitResultsChanged();
    QString tresultID=transformResultID(resultID);
    int index=getIndexFromEvaluationResultID(resultID);
    if (index<0) {
        QFFitResultsByIndexEvaluation::resetAllFitResults(r, resultID);
    } else {
        QList<QString> resids=r->resultsCalcNames(tresultID);
        for (int i=0; i<resids.size(); i++) {
            /*qDebug()<<"resetAllFitResults():  resids["<<i<<"]="<<resids[i]<<"  tresultID="<<tresultID<<"  index="<<index;
            if (isParamNameLocalStore(resids[i])) {
                r->resultsSetInBooleanList(tresultID, resids[i], index, false);
            } else {
                r->resultsResetInList(tresultID, resids[i], index);
            }*/
            r->resultsRemove(tresultID, resids[i], false);
        }

    }
    if (doEmit) r->enableEmitResultsChanged(true);
}

void QFFitResultsByIndexAsVectorEvaluation::resetAllFitResultsAllFiles() {
    QList<QPointer<QFRawDataRecord> > recs=getApplicableRecords();
    for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* r=recs[i];
        if (r) {
            resetAllFitResults(r, getEvaluationResultID(r));
        }
    }
}


bool QFFitResultsByIndexAsVectorEvaluation::hasFit(const QFRawDataRecord *r1, const QString& evalID) const {
    QString resultID=getEvaluationResultID(r1);
    int index=getIndexFromEvaluationResultID(evalID);
    if (index<0) {
        return QFFitResultsByIndexEvaluation::hasFit(r1, evalID);
    } else {

        const QFRawDataRecord* r=r1;
        if (r==NULL) r=getHighlightedRecord();
        if (r==NULL) return false;
        if (getFitFunction(r)==NULL) return false;
        QString tresultID=transformResultID(evalID);
        if (!r->resultsExistsFromEvaluation(tresultID)) {
            return false;
        } else {
            QList<QString> resids=r->resultsCalcNames(tresultID);
            bool anyfit=false;
            for (int i=0; i<resids.size(); i++) {
                if (!resids[i].isEmpty() && isParamNameLocalStore(resids[i])) {
                    if (r->resultsGetInBooleanList(tresultID, resids[i], index, false)) return true;
                }
            }
        }
        return false;
    }
}


/*! \brief set the given parameter \a id to the given value (and error) in all files, hasFit for the file is \c true */
void QFFitResultsByIndexAsVectorEvaluation::setAllFitValues(const QString& id, double value, double error, bool currentFileOnly) {

    QList<QPointer<QFRawDataRecord> > recs;
    recs.clear();
    if (currentFileOnly) {
        recs.append(getHighlightedRecord());
    } else {
        recs=getApplicableRecords();
    }
    for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* r=recs[i];
        QFFitFunction* f=getFitFunction(r);
        if (r&&f) {
            int pid=f->getParameterNum(id);
            QString unit="";
            if (pid>-1) unit=f->getDescription(pid).unit;
            bool doEmit=r->isEmitResultsChangedEnabled();
            r->disableEmitResultsChanged();
            for(int idx=getIndexMin(r); idx<=getIndexMax(r); idx++) {
                QString resultID=getEvaluationResultID(idx, r);
                QString en=transformResultID(resultID);
                if (hasFit(r, idx)) {
                    if (idx<0) {
                        r->resultsSetNumberError(resultID, getFitParamID(id), value, error, unit);
                    } else {
                        r->resultsSetInNumberErrorList(en, getFitParamID(id), idx, value, error, unit);
                        r->resultsSetInBooleanList(en, getParamNameLocalStore(getFitParamID(id)), idx, true);
                    }
                }
            }
            if (doEmit) r->enableEmitResultsChanged(true);
        }
    }
    emitResultsChanged();
}

/*! \brief set the given parameter \a id to the given fix value */
void QFFitResultsByIndexAsVectorEvaluation::setAllFitFixes(const QString& id, bool fix, bool currentFileOnly) {
    QList<QPointer<QFRawDataRecord> > recs;
    recs.clear();
    if (currentFileOnly) {
        recs.append(getHighlightedRecord());
    } else {
        recs=getApplicableRecords();
    }
    for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* r=recs[i];
        QFFitFunction* f=getFitFunction(r);
        if (r&&f) {
            bool doEmit=r->isEmitResultsChangedEnabled();
            r->disableEmitResultsChanged();
            for(int idx=getIndexMin(recs[i]); idx<=getIndexMax(recs[i]); idx++) {
                QString resultID=getEvaluationResultID(idx, r);
                QString en=transformResultID(resultID);
                if (hasFit(r, idx)) {
                    if (idx<0) {
                        r->resultsSetBoolean(resultID, getFitParamFixID(id), fix);
                    } else {
                        r->resultsSetInBooleanList(en, getFitParamFixID(id), idx, fix);
                        r->resultsSetInBooleanList(en, getParamNameLocalStore(getFitParamFixID(id)), idx, true);
                    }
                }
            }
            if (doEmit) r->enableEmitResultsChanged(true);
        }
    }
    emitResultsChanged();
}



void QFFitResultsByIndexAsVectorEvaluation::setFitResultValue(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double value)  {
    QFFitFunction* f=getFitFunction(r);
    QString unit="";
    if (f) {
        int pid=f->getParameterNum(parameterID);
        if (pid>-1) unit=f->getDescription(pid).unit;
    }
    setFitResultValue(r, resultID, parameterID, value, unit);
}
void QFFitResultsByIndexAsVectorEvaluation::setFitResultValueNoParamTransform(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double value)  {
    QFFitFunction* f=getFitFunction(r);
    QString unit="";
    if (f) {
        int pid=f->getParameterNum(parameterID);
        if (pid>-1) unit=f->getDescription(pid).unit;
    }
    setFitResultValueNoParamTransform(r, resultID, parameterID, value, unit);
}

void QFFitResultsByIndexAsVectorEvaluation::setAllFitResultValue(QFRawDataRecord *r, const QString &parameterID, double value)
{

    bool doEmit=r->isEmitResultsChangedEnabled();
    r->disableEmitResultsChanged();
    QFFitFunction* f=getFitFunction(r);
    int index0=getIndexMin(r);
    while (index0<0 && index0<=getIndexMax(r)) index0++;
    //int index=getIndexFromEvaluationResultID(resultID);
    QString unit;
    if (f) {
        int pid=f->getParameterNum(parameterID);
        if (pid>-1) unit=f->getDescription(pid).unit;
    }
    const QString pid=getFitParamID(parameterID);    const QString resultID=getEvaluationResultID(index0, r);
    const QString tresultID=transformResultID(resultID);
    QVector<bool> tparam=r->resultsGetAsBooleanList(tresultID, getParamNameLocalStore(pid));
    QVector<double> param=r->resultsGetAsDoubleList(tresultID, pid);
    QVector<double> eparam=r->resultsGetErrorAsDoubleList(tresultID, pid);

    for (int index=getIndexMin(r); index<=getIndexMax(r); index++){
        if (index<0) {
            const QString resultID=getEvaluationResultID(index0, r);
            QFFitResultsByIndexEvaluation::setFitResultValue(r, resultID, parameterID, value, getFitError(r, resultID, parameterID));
        } else if (r) {
            while (index>=tparam.size()) tparam<<false;
            while (index>=param.size()) param<<0;
            while (index>=eparam.size()) eparam<<0;
            tparam[index]=true;
            param[index]=value;
        }
    }
    r->resultsSetBooleanList(tresultID, getParamNameLocalStore(pid), tparam);
    r->resultsSetNumberErrorList(tresultID, pid, param, eparam, unit);
    emitResultsChanged(r, resultID, getFitParamID(parameterID));
    if (doEmit) r->enableEmitResultsChanged(true);
}

void QFFitResultsByIndexAsVectorEvaluation::setAllFitResultError(QFRawDataRecord *r, const QString &parameterID, double error)
{
    bool doEmit=r->isEmitResultsChangedEnabled();
    r->disableEmitResultsChanged();
    QFFitFunction* f=getFitFunction(r);
    int index0=getIndexMin(r);
    while (index0<0 && index0<=getIndexMax(r)) index0++;
    //int index=getIndexFromEvaluationResultID(resultID);
    QString unit="";
    if (f) {
        int pid=f->getParameterNum(parameterID);
        if (pid>-1) unit=f->getDescription(pid).unit;
    }
    const QString pid=getFitParamID(parameterID);    const QString resultID=getEvaluationResultID(index0, r);
    const QString tresultID=transformResultID(resultID);
    QVector<bool> tparam=r->resultsGetAsBooleanList(tresultID, getParamNameLocalStore(pid));
    QVector<double> param=r->resultsGetAsDoubleList(tresultID, pid);
    QVector<double> eparam=r->resultsGetErrorAsDoubleList(tresultID, pid);

    for (int index=getIndexMin(r); index<=getIndexMax(r); index++){
        if (index<0) {
             const QString resultID=getEvaluationResultID(index0, r);
             QFFitResultsByIndexEvaluation::setFitResultError(r, resultID, parameterID, error);
        } else if (r) {
            while (index>=tparam.size()) tparam<<false;
            while (index>=param.size()) param<<0;
            while (index>=eparam.size()) eparam<<0;
            tparam[index]=true;
            eparam[index]=error;
        }
    }
    r->resultsSetBooleanList(tresultID, getParamNameLocalStore(pid), tparam);
    r->resultsSetNumberErrorList(tresultID, pid, param, eparam, unit);
    emitResultsChanged(r, resultID, getFitParamID(parameterID));
    if (doEmit) r->enableEmitResultsChanged(true);

}

void QFFitResultsByIndexAsVectorEvaluation::setAllFitResultFix(QFRawDataRecord *r, const QString &parameterID, bool fix)
{
    bool doEmit=r->isEmitResultsChangedEnabled();
    r->disableEmitResultsChanged();
    QFFitFunction* f=getFitFunction(r);
    int index0=getIndexMin(r);
    while (index0<0 && index0<=getIndexMax(r)) index0++;
    //int index=getIndexFromEvaluationResultID(resultID);
    QString unit="";
    if (f) {
        int pid=f->getParameterNum(parameterID);
        if (pid>-1) unit=f->getDescription(pid).unit;
    }
    const QString pid=getFitParamFixID(parameterID);
    const QString resultID=getEvaluationResultID(index0, r);
    const QString tresultID=transformResultID(resultID);
    QVector<bool> tparam=r->resultsGetAsBooleanList(tresultID, getParamNameLocalStore(pid));
    QVector<bool> param=r->resultsGetAsBooleanList(tresultID, pid);

    for (int index=getIndexMin(r); index<=getIndexMax(r); index++){
        if (index<0) {
             const QString resultID=getEvaluationResultID(index0, r);
             QFFitResultsByIndexEvaluation::setFitResultFix(r, resultID, parameterID, fix);
        } else if (r) {
            while (index>=tparam.size()) tparam<<false;
            while (index>=param.size()) param<<false;
            tparam[index]=true;
            param[index]=fix;
        }
    }
    r->resultsSetBooleanList(tresultID, getParamNameLocalStore(pid), tparam);
    r->resultsSetBooleanList(tresultID, pid, param);
    emitResultsChanged(r, resultID, getFitParamID(parameterID));
    if (doEmit) r->enableEmitResultsChanged(true);


}

void QFFitResultsByIndexAsVectorEvaluation::setFitResultValue(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double value, QString unit)  {
    int index=getIndexFromEvaluationResultID(resultID);
    QString tresultID=transformResultID(resultID);
    if (index<0) {
        QFFitResultsByIndexEvaluation::setFitResultValue(r, resultID, parameterID, value, unit);
    } else if (r) {
        bool doEmit=r->isEmitResultsChangedEnabled();
        r->disableEmitResultsChanged();
        QString pid=getFitParamID(parameterID);
        r->resultsSetInBooleanList(tresultID, getParamNameLocalStore(pid), index, true);
        r->resultsSetInNumberList(tresultID, pid, index, value, unit);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
        if (doEmit) r->enableEmitResultsChanged(true);
    }
}

void QFFitResultsByIndexAsVectorEvaluation::setFitResultValueNoParamTransform(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double value, QString unit)  {
    int index=getIndexFromEvaluationResultID(resultID);
    QString tresultID=transformResultID(resultID);
    if (index<0) {
        QFFitResultsByIndexEvaluation::setFitResultValueNoParamTransform(r, resultID, parameterID, value, unit);
    } else if (r) {
        bool doEmit=r->isEmitResultsChangedEnabled();
        r->disableEmitResultsChanged();
        QString pid=getFitParamID(parameterID);
        r->resultsSetInBooleanList(tresultID, getParamNameLocalStore(pid), index, true);
        r->resultsSetInNumberList(tresultID, pid, index, value, unit);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
        if (doEmit) r->enableEmitResultsChanged(true);
    }
}
void QFFitResultsByIndexAsVectorEvaluation::setFitResultValue(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double value, double error)  {
    int index=getIndexFromEvaluationResultID(resultID);
    QString tresultID=transformResultID(resultID);
    if (index<0) {
        QFFitResultsByIndexEvaluation::setFitResultValue(r, resultID, parameterID, value, error);
    } else if (r) {
        QFFitFunction* f=getFitFunction(r);
        QString unit="";
        if (f) {
            int pid=f->getParameterNum(parameterID);
            if (pid>-1) unit=f->getDescription(pid).unit;
        }
        QString pid=getFitParamID(parameterID);
        bool doEmit=r->isEmitResultsChangedEnabled();
        r->disableEmitResultsChanged();
        r->resultsSetInBooleanList(tresultID, getParamNameLocalStore(pid), index, true);
        r->resultsSetInNumberErrorList(tresultID, pid, index, value, error, unit);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
        if (doEmit) r->enableEmitResultsChanged(true);
    }
}

void QFFitResultsByIndexAsVectorEvaluation::setFitResultValueNoParamTransform(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double value, double error)  {
    int index=getIndexFromEvaluationResultID(resultID);
    QString tresultID=transformResultID(resultID);
    if (index<0) {
        QFFitResultsByIndexEvaluation::setFitResultValueNoParamTransform(r, resultID, parameterID, value, error);
    } else if (r) {
        QFFitFunction* f=getFitFunction(r);
        QString unit="";
        if (f) {
            int pid=f->getParameterNum(parameterID);
            if (pid>-1) unit=f->getDescription(pid).unit;
        }
        QString pid=(parameterID);
        bool doEmit=r->isEmitResultsChangedEnabled();
        r->disableEmitResultsChanged();
        r->resultsSetInBooleanList(tresultID, getParamNameLocalStore(pid), index, true);
        r->resultsSetInNumberErrorList(tresultID, pid, index, value, error, unit);
        emitResultsChanged(r, resultID, (parameterID));
        if (doEmit) r->enableEmitResultsChanged(true);
    }
}
void QFFitResultsByIndexAsVectorEvaluation::setFitResultValue(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double value, double error, QString unit)  {
    int index=getIndexFromEvaluationResultID(resultID);
    QString tresultID=transformResultID(resultID);
    if (index<0) {
        QFFitResultsByIndexEvaluation::setFitResultValue(r, resultID, parameterID, value, error);
    } else if (r) {
        QFFitFunction* f=getFitFunction(r);

        QString pid=getFitParamID(parameterID);
        bool doEmit=r->isEmitResultsChangedEnabled();
        r->disableEmitResultsChanged();
        r->resultsSetInBooleanList(tresultID, getParamNameLocalStore(pid), index, true);
        r->resultsSetInNumberErrorList(tresultID, pid, index, value, error, unit);
        if (doEmit) r->enableEmitResultsChanged(true);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}
void QFFitResultsByIndexAsVectorEvaluation::setFitResultValueNoParamTransform(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double value, double error, QString unit)  {
    int index=getIndexFromEvaluationResultID(resultID);
    QString tresultID=transformResultID(resultID);
    if (index<0) {
        QFFitResultsByIndexEvaluation::setFitResultValueNoParamTransform(r, resultID, parameterID, value, error);
    } else if (r) {
        QFFitFunction* f=getFitFunction(r);

        QString pid=(parameterID);
        bool doEmit=r->isEmitResultsChangedEnabled();
        r->disableEmitResultsChanged();
        r->resultsSetInBooleanList(tresultID, getParamNameLocalStore(pid), index, true);
        r->resultsSetInNumberErrorList(tresultID, pid, index, value, error, unit);
        if (doEmit) r->enableEmitResultsChanged(true);
        emitResultsChanged(r, resultID, (parameterID));
    }
}
void QFFitResultsByIndexAsVectorEvaluation::setFitResultValue(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, QVector<double> value, QVector<double> error, QString unit)
{
    int index=getIndexFromEvaluationResultID(resultID);
    QString tresultID=transformResultID(resultID);
    if (index<0) {
        QFFitResultsByIndexEvaluation::setFitResultValue(r, resultID, parameterID, value, error);
    } else if (r) {
        QFFitFunction* f=getFitFunction(r);

        QString pid=getFitParamID(parameterID);
        bool doEmit=r->isEmitResultsChangedEnabled();
        r->disableEmitResultsChanged();
        QVector<bool> b;
        for (int i=0; i<value.size(); i++) b<<true;
        r->resultsSetBooleanList(tresultID, getParamNameLocalStore(pid), b);
        r->resultsSetNumberErrorList(tresultID, pid, value, error, unit);
        if (doEmit) r->enableEmitResultsChanged(true);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}

void QFFitResultsByIndexAsVectorEvaluation::setFitResultValueNoParamTransform(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, QVector<double> value, QVector<double> error, QString unit)
{
    int index=getIndexFromEvaluationResultID(resultID);
    QString tresultID=transformResultID(resultID);
    if (index<0) {
        QFFitResultsByIndexEvaluation::setFitResultValueNoParamTransform(r, resultID, parameterID, value, error);
    } else if (r) {
        QFFitFunction* f=getFitFunction(r);

        QString pid=(parameterID);
        bool doEmit=r->isEmitResultsChangedEnabled();
        r->disableEmitResultsChanged();
        QVector<bool> b;
        for (int i=0; i<value.size(); i++) b<<true;
        r->resultsSetBooleanList(tresultID, getParamNameLocalStore(pid), b);
        r->resultsSetNumberErrorList(tresultID, pid, value, error, unit);
        if (doEmit) r->enableEmitResultsChanged(true);
        emitResultsChanged(r, resultID, (parameterID));
    }
}
void QFFitResultsByIndexAsVectorEvaluation::setFitResultValue(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, QVector<double> value, QString unit)
{
    int index=getIndexFromEvaluationResultID(resultID);
    QString tresultID=transformResultID(resultID);
    if (index<0) {
        QFFitResultsByIndexEvaluation::setFitResultValue(r, resultID, parameterID, value, unit);
    } else if (r) {
        QFFitFunction* f=getFitFunction(r);

        QString pid=getFitParamID(parameterID);
        bool doEmit=r->isEmitResultsChangedEnabled();
        r->disableEmitResultsChanged();
        QVector<bool> b;
        for (int i=0; i<value.size(); i++) b<<true;
        r->resultsSetBooleanList(tresultID, getParamNameLocalStore(pid), b);
        r->resultsSetNumberList(tresultID, pid, value,unit);
        if (doEmit) r->enableEmitResultsChanged(true);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}
void QFFitResultsByIndexAsVectorEvaluation::setFitResultValueNoParamTransform(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, QVector<double> value, QString unit)
{
    int index=getIndexFromEvaluationResultID(resultID);
    QString tresultID=transformResultID(resultID);
    if (index<0) {
        QFFitResultsByIndexEvaluation::setFitResultValueNoParamTransform(r, resultID, parameterID, value, unit);
    } else if (r) {
        QFFitFunction* f=getFitFunction(r);

        QString pid=(parameterID);
        bool doEmit=r->isEmitResultsChangedEnabled();
        r->disableEmitResultsChanged();
        QVector<bool> b;
        for (int i=0; i<value.size(); i++) b<<true;
        r->resultsSetBooleanList(tresultID, getParamNameLocalStore(pid), b);
        r->resultsSetNumberList(tresultID, pid, value,unit);
        if (doEmit) r->enableEmitResultsChanged(true);
        emitResultsChanged(r, resultID, (parameterID));
    }
}

void QFFitResultsByIndexAsVectorEvaluation::setFitResultError(QFRawDataRecord* r, const QString& resultID,  const QString& parameterID, double error)  {
    int index=getIndexFromEvaluationResultID(resultID);
    QString tresultID=transformResultID(resultID);
    if (index<0) {
        QFFitResultsByIndexEvaluation::setFitResultError(r, resultID, parameterID, error);
    } else if (r) {
        QString pid=getFitParamID(parameterID);
        bool doEmit=r->isEmitResultsChangedEnabled();
        r->disableEmitResultsChanged();
        r->resultsSetInBooleanList(tresultID, getParamNameLocalStore(pid), index, true);
        r->resultsSetErrorInNumberErrorList(tresultID, pid, index, error);
        if (doEmit) r->enableEmitResultsChanged(true);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}


void QFFitResultsByIndexAsVectorEvaluation::setFitResultValueString(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, QString value)  {
    int index=getIndexFromEvaluationResultID(resultID);
    QString tresultID=transformResultID(resultID);
    if (index<0) {
        QFFitResultsByIndexEvaluation::setFitResultValueString(r, resultID, parameterID, value);
    } else if (r) {
        QString pid=getFitParamID(parameterID);
        bool doEmit=r->isEmitResultsChangedEnabled();
        r->disableEmitResultsChanged();
        r->resultsSetInBooleanList(tresultID, getParamNameLocalStore(pid), index, true);
        r->resultsSetInStringList(tresultID, pid, index, value);
        if (doEmit) r->enableEmitResultsChanged(true);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}

void QFFitResultsByIndexAsVectorEvaluation::setFitResultValueBool(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, bool value)  {
    int index=getIndexFromEvaluationResultID(resultID);
    QString tresultID=transformResultID(resultID);
    if (index<0) {
        QFFitResultsByIndexEvaluation::setFitResultValueBool(r, resultID, parameterID, value);
    } else if (r) {
        QString pid=getFitParamID(parameterID);
        bool doEmit=r->isEmitResultsChangedEnabled();
        r->disableEmitResultsChanged();
        r->resultsSetInBooleanList(tresultID, getParamNameLocalStore(pid), index, true);
        r->resultsSetInBooleanList(tresultID, pid, index, value);
        if (doEmit) r->enableEmitResultsChanged(true);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }

}

void QFFitResultsByIndexAsVectorEvaluation::setFitResultValueInt(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, int64_t value)  {
    QFFitFunction* f=getFitFunction(r);
    QString unit="";
    if (f) {
        int pid=f->getParameterNum(parameterID);
        if (pid>-1) unit=f->getDescription(pid).unit;
    }

    setFitResultValueInt(r, resultID, parameterID, value, unit);
}

void QFFitResultsByIndexAsVectorEvaluation::setFitResultValueInt(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, int64_t value, QString unit)  {
    int index=getIndexFromEvaluationResultID(resultID);
    QString tresultID=transformResultID(resultID);
    if (index<0) {
        QFFitResultsByIndexEvaluation::setFitResultValueInt(r, resultID, parameterID, value, unit);
    } else if (r) {
        QString pid=getFitParamID(parameterID);
        bool doEmit=r->isEmitResultsChangedEnabled();
        r->disableEmitResultsChanged();
        r->resultsSetInBooleanList(tresultID, getParamNameLocalStore(pid), index, true);
        r->resultsSetInIntegerList(tresultID, pid, index, value, unit);
        if (doEmit) r->enableEmitResultsChanged(true);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}



void QFFitResultsByIndexAsVectorEvaluation::setFitResultFix(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, bool fix)  {
    int index=getIndexFromEvaluationResultID(resultID);
    QString tresultID=transformResultID(resultID);
    if (index<0) {
        QFFitResultsByIndexEvaluation::setFitResultFix(r, resultID, parameterID, fix);
    } else if (r) {
        QString pid=getFitParamFixID(parameterID);
        bool doEmit=r->isEmitResultsChangedEnabled();
        r->disableEmitResultsChanged();
        r->resultsSetInBooleanList(tresultID, getParamNameLocalStore(pid), index, true);
        r->resultsSetInBooleanList(tresultID, pid, index, fix);
        if (doEmit) r->enableEmitResultsChanged(true);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}


void QFFitResultsByIndexAsVectorEvaluation::setFitResultGroup(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, const QString& group)  {
    int index=getIndexFromEvaluationResultID(resultID);
    QString tresultID=transformResultID(resultID);
    if (index<0) {
        QFFitResultsByIndexEvaluation::setFitResultGroup(r, resultID, parameterID, group);
    } else if (r) {
        QString pid=getFitParamID(parameterID);
        bool doEmit=r->isEmitResultsChangedEnabled();
        r->disableEmitResultsChanged();
        r->resultsSetInBooleanList(tresultID, getParamNameLocalStore(pid), index, true);
        r->resultsSetGroup(tresultID, pid, group);
        if (doEmit) r->enableEmitResultsChanged(true);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}

void QFFitResultsByIndexAsVectorEvaluation::setFitResultGroupNoParamTransform(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, const QString& group)  {
    int index=getIndexFromEvaluationResultID(resultID);
    QString tresultID=transformResultID(resultID);
    if (index<0) {
        QFFitResultsByIndexEvaluation::setFitResultGroupNoParamTransform(r, resultID, parameterID, group);
    } else if (r) {
        QString pid=(parameterID);
        bool doEmit=r->isEmitResultsChangedEnabled();
        r->disableEmitResultsChanged();
        r->resultsSetInBooleanList(tresultID, getParamNameLocalStore(pid), index, true);
        r->resultsSetGroup(tresultID, pid, group);
        if (doEmit) r->enableEmitResultsChanged(true);
        emitResultsChanged(r, resultID, (parameterID));
    }
}

void QFFitResultsByIndexAsVectorEvaluation::setFitResultLabel(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, const QString& label, const QString& label_richtext)  {
    int index=getIndexFromEvaluationResultID(resultID);
    QString tresultID=transformResultID(resultID);
    if (index<0) {
        QFFitResultsByIndexEvaluation::setFitResultLabel(r, resultID, parameterID, label, label_richtext);
    } else if (r) {
        QString pid=getFitParamID(parameterID);
        bool doEmit=r->isEmitResultsChangedEnabled();
        r->disableEmitResultsChanged();
        r->resultsSetInBooleanList(tresultID, getParamNameLocalStore(pid), index, true);
        r->resultsSetLabel(tresultID, pid, label, label_richtext);
        if (doEmit) r->enableEmitResultsChanged(true);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}

void QFFitResultsByIndexAsVectorEvaluation::setFitResultLabelNoParamTransform(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, const QString& label, const QString& label_richtext)  {
    int index=getIndexFromEvaluationResultID(resultID);
    QString tresultID=transformResultID(resultID);
    if (index<0) {
        QFFitResultsByIndexEvaluation::setFitResultLabelNoParamTransform(r, resultID, parameterID, label, label_richtext);
    } else if (r) {
        QString pid=(parameterID);
        bool doEmit=r->isEmitResultsChangedEnabled();
        r->disableEmitResultsChanged();
        r->resultsSetInBooleanList(tresultID, getParamNameLocalStore(pid), index, true);
        r->resultsSetLabel(tresultID, pid, label, label_richtext);
        if (doEmit) r->enableEmitResultsChanged(true);
        emitResultsChanged(r, resultID, (parameterID));
    }
}


void QFFitResultsByIndexAsVectorEvaluation::setFitResultValues(QFRawDataRecord* r, const QString& resultID, double* values, double* errors)  {
    if (r!=NULL) {
        int index=getIndexFromEvaluationResultID(resultID);
        if (index<0) {
            QFFitResultsByIndexEvaluation::setFitResultValues(r, resultID, values, errors);
        } else {
            QFFitFunction* f=getFitFunction(r);
            if (f) {
                bool doEmit=r->isEmitResultsChangedEnabled();
                bool thisDoEmit=doEmitResultsChanged;
                doEmitResultsChanged=false;
                r->disableEmitResultsChanged();
                for (int i=0; i<f->paramCount(); i++) {
                    QString pid=f->getParameterID(i);
                    QString unit=f->getDescription(pid).unit;
                    setFitResultValue(r, resultID, pid, values[i], errors[i], unit);
                }
                if (doEmit) r->enableEmitResultsChanged(true);
                doEmitResultsChanged=thisDoEmit;
                //emitPropertiesChanged();
                emitResultsChanged();
            }
        }
    }
}

void QFFitResultsByIndexAsVectorEvaluation::setFitResultValuesVisible(QFRawDataRecord* r, const QString& resultID, double* values, double* errors)  {
    if (r!=NULL) {
        int index=getIndexFromEvaluationResultID(resultID);
        if (index<0) {
            QFFitResultsByIndexEvaluation::setFitResultValuesVisible(r, resultID, values, errors);
        } else {
            QFFitFunction* f=getFitFunction(r);
            if (f) {
                bool doEmit=r->isEmitResultsChangedEnabled();
                bool thisDoEmit=doEmitResultsChanged;
                doEmitResultsChanged=false;
                r->disableEmitResultsChanged();
                for (int i=0; i<f->paramCount(); i++) {
                    if (f->isParameterVisible(i, values)) {
                        QString pid=f->getParameterID(i);
                        QString unit=f->getDescription(pid).unit;
                        setFitResultValue(r, resultID, pid, values[i], errors[i], unit);
                    }
                }
                if (doEmit) r->enableEmitResultsChanged(true);
                doEmitResultsChanged=thisDoEmit;
                //emitPropertiesChanged();
                emitResultsChanged();
            }
        }
    }
}

void QFFitResultsByIndexAsVectorEvaluation::setFitResultValuesVisibleWithGroupAndLabel(QFRawDataRecord* r, const QString& resultID, const double *values, const double *errors, const QString& group, const bool *fix, const QString& fixGroup, bool sortPriority)  {
    if (r!=NULL) {
        int index=getIndexFromEvaluationResultID(resultID);
        if (index<0) {
            QFFitResultsByIndexEvaluation::setFitResultValuesVisibleWithGroupAndLabel(r, resultID, values, errors, group, fix, fixGroup, sortPriority);
        } else {
            QFFitFunction* f=getFitFunction(r);
            if (f) {
                bool doEmit=r->isEmitResultsChangedEnabled();
                bool thisDoEmit=doEmitResultsChanged;
                QString tresultID=transformResultID(resultID);
                doEmitResultsChanged=false;
                r->disableEmitResultsChanged();
                for (int i=0; i<f->paramCount(); i++) {
                    if (f->isParameterVisible(i, values)) {
                        QString pid=f->getParameterID(i);
                        QString unit=f->getDescription(pid).unit;
                        QString fpid=getFitParamID(pid);
                        QString ffid= getFitParamFixID(pid);

                        r->resultsSetInBooleanList(tresultID, getParamNameLocalStore(fpid), index, true);
                        r->resultsSetInNumberErrorList(tresultID, fpid, index, values[i], errors[i], unit);
                        r->resultsSetGroup(tresultID, fpid, group);
                        r->resultsSetLabel(tresultID, fpid, pid+": "+f->getDescription(pid).name, f->getDescription(pid).label);
                        r->resultsSetSortPriority(tresultID, fpid, sortPriority);
                        if (fix) {
                            r->resultsSetInBooleanList(tresultID, ffid, index, fix[i]);
                            r->resultsSetInBooleanList(tresultID, getParamNameLocalStore(ffid), index, true);
                            if (!fixGroup.isEmpty()) r->resultsSetGroup(tresultID, ffid, fixGroup);
                            r->resultsSetLabel(tresultID, ffid, "fix_"+pid+": "+f->getDescription(pid).name+tr(", fix"), f->getDescription(pid).label+tr(", fix"));
                            r->resultsSetSortPriority(tresultID,  ffid, sortPriority);
                        }
                    }
                }
                if (doEmit) r->enableEmitResultsChanged(true);
                doEmitResultsChanged=thisDoEmit;
                //emitPropertiesChanged();
                emitResultsChanged();
            }
        }
    }
}


double QFFitResultsByIndexAsVectorEvaluation::getFitValue(const QFRawDataRecord *r, const QString& resultID, const QString& parameterID) const {
    double res=0;
    int index=getIndexFromEvaluationResultID(resultID);
    QString tresultID=transformResultID(resultID);
    if (index<0) {
        return QFFitResultsByIndexEvaluation::getFitValue(r, resultID, parameterID);
    } else if (r) {
        QFFitFunction* f=getFitFunction(r);
        if (f==NULL) {
            return 0;
        }
        if (!f->hasParameter(parameterID)) return 0;
        int pid=f->getParameterNum(parameterID);
        QString fpid=getFitParamID(parameterID);

        if (pid>-1) res=f->getDescription(pid).initialValue;

        double value=0;
        if (overrideFitFunctionPreset(r, parameterID, value)) res=value;

        res=fitParamGlobalSettings->value(QString(m_fitFunction+"/"+parameterID), res).toDouble();
        res=fitParamSettings->value(QString(m_fitFunction+"/"+parameterID), res).toDouble();


        QString psID=getParameterStoreID(r, parameterID);
        //qDebug()<<"QFFitResultsByIndexAsVectorEvaluation::getFitValue: "<<r<<parameterID<<psID;
        if (parameterStore.contains(psID)) {
            if (parameterStore[psID].valueSet) {
                res=parameterStore[psID].value;
            }
        }

        double sval=res, serr=0;
        if (hasSpecial(r, resultID, parameterID, sval, serr)) {
            res=sval;
        }
        if (hasFit(r, resultID)) {
            if (r->resultsExists(tresultID, fpid)) {
                if (r->resultsExists(tresultID, getParamNameLocalStore(fpid))) {
                    if (r->resultsGetInBooleanList(tresultID, getParamNameLocalStore(fpid), index, false)) {
                        res=r->resultsGetInNumberList(tresultID, fpid, index,  res);
                    }
                } else {
                    res=r->resultsGetInNumberList(tresultID, fpid, index,  res);
                }
            }
        }
    }
    return res;
}

double QFFitResultsByIndexAsVectorEvaluation::getFitError(const QFRawDataRecord* r, const QString& resultID, const QString& parameterID) const  {
    double res=0;
    int index=getIndexFromEvaluationResultID(resultID);
    QString tresultID=transformResultID(resultID);
    if (index<0) {
        return QFFitResultsByIndexEvaluation::getFitError(r, resultID, parameterID);
    } else if (r) {
        QFFitFunction* f=getFitFunction(r);
        if (f==NULL) {
            return 0;
        }
        if (!f->hasParameter(parameterID)) return 0;
        int pid=f->getParameterNum(parameterID);
        QString fpid=getFitParamID(parameterID);

        if (pid>-1) res=f->getDescription(pid).initialValue;

        double value=0;
        if (overrideFitFunctionPresetError(r, parameterID, value)) res=value;

        res=fitParamGlobalSettings->value(QString(m_fitFunction+"/"+parameterID), res).toDouble();
        res=fitParamSettings->value(QString(m_fitFunction+"/"+parameterID), res).toDouble();

        QString psID=getParameterStoreID(r, parameterID);
        if (parameterStore.contains(psID)) {
            if (parameterStore[psID].errorSet) {
                res=parameterStore[psID].error;
            }
        }
        double sval=0, serr=res;
        if (hasSpecial(r, resultID, parameterID, sval, serr)) {
            res=serr;
        }

        if (hasFit(r, resultID)) {
            if (r->resultsExists(tresultID, fpid)) {
                if (r->resultsExists(tresultID, getParamNameLocalStore(fpid))) {
                    if (r->resultsGetInBooleanList(tresultID, getParamNameLocalStore(fpid), index, false)) {
                        res=r->resultsGetErrorInNumberErrorList(tresultID, fpid, index,  res);
                    }
                } else {
                    res=r->resultsGetErrorInNumberErrorList(tresultID, fpid, index,  res);
                }
            }
        }
    }
    return res;
}



bool QFFitResultsByIndexAsVectorEvaluation::getFitFix(const QFRawDataRecord* r, const QString& resultID, const QString& parameterID) const {
    bool res=false;
    int index=getIndexFromEvaluationResultID(resultID);
    QString tresultID=transformResultID(resultID);
    if (index<0) {
        return QFFitResultsByIndexEvaluation::getFitFix(r, resultID, parameterID);
    } else if (r) {
        QFFitFunction* f=getFitFunction(r);
        if (f==NULL) {
            return false;
        }
        if (!f->hasParameter(parameterID)) return false;
        QString fpid=getFitParamFixID(parameterID);
        int pid=f->getParameterNum(parameterID);
        if (pid>-1) res=f->getDescription(pid).initialFix;

        bool value=0;
        if (overrideFitFunctionPresetFix(r, parameterID, value)) res=value;

        res=fitParamGlobalSettings->value(QString(m_fitFunction+"/"+parameterID+"_fix"), res).toBool();
        res=fitParamSettings->value(QString(m_fitFunction+"/"+parameterID+"_fix"), res).toBool();
        QString psID=getParameterStoreID(r, parameterID);
        if (parameterStore.contains(psID)) {
            if (parameterStore[psID].fixSet) {
                res=parameterStore[psID].fix;
            }
        }

        if (hasFit(r, resultID)) {
            if (r->resultsExists(tresultID, fpid)) {
                if (r->resultsExists(tresultID, getParamNameLocalStore(fpid))) {
                    if (r->resultsGetInBooleanList(tresultID, getParamNameLocalStore(fpid), index, false)) {
                        res=r->resultsGetInBooleanList(tresultID, fpid, index, res);
                    }
                } else {
                    res=r->resultsGetInBooleanList(tresultID, fpid, index, res);
                }
            }
        }
    }
    return res;
}




