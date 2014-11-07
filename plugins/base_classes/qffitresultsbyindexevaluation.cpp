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

#include "qffitresultsbyindexevaluation.h"

QFFitResultsByIndexEvaluation::QFFitResultsByIndexEvaluation(const QString& fitFunctionPrefix, QFProject* parent, bool showRDRList, bool useSelection) :
    QFFitResultsEvaluation(fitFunctionPrefix, parent, showRDRList, useSelection)
{
    m_currentIndex=0;
    getEvaluationResultIDUsesFitFunction=true;
}


QFFitResultsByIndexEvaluation::~QFFitResultsByIndexEvaluation() {
}

void QFFitResultsByIndexEvaluation::setPresetProperty(const QString &id, const QVariant &data, bool usereditable, bool visible)
{
    QString lid=id.toLower();
    if (lid=="preset_current_index" || lid=="preset_current_run" || lid=="preset_current_pixel") {
        setCurrentIndex(data.toInt());
    } else {
        QFFitResultsEvaluation::setPresetProperty(id, data, usereditable, visible);
    }

}

QString QFFitResultsByIndexEvaluation::getEvaluationResultID(QString fitFunction, int currentIndex) const {
    if (getEvaluationResultIDUsesFitFunction) {
        //qDebug()<<"QFFitResultsByIndexEvaluation::getEvaluationResultID("<<fitFunction<<", "<<currentIndex<<")";
        if (currentIndex<0) return QString("%1_%2_%3_runavg").arg(getType()).arg(getID()).arg(fitFunction);
        return QString("%1_%2_%3_run%4").arg(getType()).arg(getID()).arg(fitFunction).arg(currentIndex);
    } else {
        if (currentIndex<0) return QString("%1_%2_runavg").arg(getType()).arg(getID());
        return QString("%1_%2_run%3").arg(getType()).arg(getID()).arg(currentIndex);
    }
}






QString QFFitResultsByIndexEvaluation::getEvaluationResultID(const QFRawDataRecord *r) const {
    return getEvaluationResultID(getFitFunctionID(r), m_currentIndex);
}

QString QFFitResultsByIndexEvaluation::getEvaluationResultID(QString fitFunction) const {
    return getEvaluationResultID(fitFunction, m_currentIndex);
}

QString QFFitResultsByIndexEvaluation::getEvaluationResultID(int currentIndex, const QFRawDataRecord *r) const {
    //qDebug()<<"QFFitResultsByIndexEvaluation::getEvaluationResultID("<<currentIndex<<", "<<r->getName()<<")";
    return getEvaluationResultID(getFitFunctionID(r), currentIndex);
}

bool QFFitResultsByIndexEvaluation::hasSpecial(const QFRawDataRecord *r, int index, const QString &paramid, double &value, double &error) const {
    return false;
}

void QFFitResultsByIndexEvaluation::calcChi2Landscape(double *chi2Landscape, int paramXFile, int paramXID, const QVector<double> &paramXValues, int paramYFile, int paramYID, const QVector<double> &paramYvalues, const QList<QFRawDataRecord *> &records, int run, int defaultMinDatarange, int defaultMaxDatarange)
{
}

void QFFitResultsByIndexEvaluation::calcChi2Landscape(double *chi2Landscape, int paramXFile, int paramXID, const QVector<double> &paramXValues, int paramYFile, int paramYID, const QVector<double> &paramYvalues, QFRawDataRecord *record, int run, int defaultMinDatarange, int defaultMaxDatarange)
{
    QList<QFRawDataRecord*> records;
    records<<record;
    calcChi2Landscape(chi2Landscape, paramXFile, paramXID, paramXValues, paramYFile, paramYID, paramYvalues, records, run, defaultMinDatarange, defaultMaxDatarange);
}










bool QFFitResultsByIndexEvaluation::hasFit(const QFRawDataRecord *r1, int index) const {
    const QFRawDataRecord* r=r1;
    if (getFitFunction(r)==NULL) return false;
    if (r==NULL) r=getHighlightedRecord();
    if (r==NULL) return false;
    QString rsid=transformResultID(getEvaluationResultID(index, r));
    return r->resultsExistsFromEvaluation(rsid);
}




void QFFitResultsByIndexEvaluation::setFitResultValue(QFRawDataRecord* r, int index, const QString& id, double value)  {
    setFitResultValue(r, getEvaluationResultID(index, r), id, value);
}
void QFFitResultsByIndexEvaluation::setFitResultValue(QFRawDataRecord* r, int index, const QString& id, double value, QString unit)  {
    setFitResultValue(r, getEvaluationResultID(index, r), id, value, unit);
}

void QFFitResultsByIndexEvaluation::setFitResultValueString(QFRawDataRecord* r, int index, const QString& id, QString value)  {
    setFitResultValueString(r, getEvaluationResultID(index, r), id, value);
}

void QFFitResultsByIndexEvaluation::setFitResultValueBool(QFRawDataRecord* r, int index, const QString& id, bool value)  {
    setFitResultValueBool(r, getEvaluationResultID(index, r), id, value);
}

void QFFitResultsByIndexEvaluation::setFitResultValueInt(QFRawDataRecord* r, int index, const QString& id, int64_t value)  {
    setFitResultValueInt(r, getEvaluationResultID(index, r), id, value);
}

void QFFitResultsByIndexEvaluation::setFitResultValueInt(QFRawDataRecord* r, int index, const QString& id, int64_t value, QString unit)  {
    setFitResultValueInt(r, getEvaluationResultID(index, r), id, value, unit);
}

void QFFitResultsByIndexEvaluation::setFitValue(QFRawDataRecord* r, int index, const QString& id, double value) {
    setFitValue(r, getEvaluationResultID(index, r), id, value);
}

void QFFitResultsByIndexEvaluation::setFitError(QFRawDataRecord* r, int index, const QString& id, double error) {
    setFitError(r, getEvaluationResultID(index, r), id, error);
}

void QFFitResultsByIndexEvaluation::setFitResultValue(QFRawDataRecord* r, int index, const QString& id, double value, double error)  {
    setFitResultValue(r, getEvaluationResultID(index, r), id, value, error);
}

void QFFitResultsByIndexEvaluation::setFitResultError(QFRawDataRecord* r, int index, const QString& id, double error)  {
    setFitResultError(r, getEvaluationResultID(index, r), id, error);
}

void QFFitResultsByIndexEvaluation::setFitResultValues(QFRawDataRecord* r, int index, double* values, double* errors)  {
    setFitResultValues(r, getEvaluationResultID(index, r), values, errors);
}

void QFFitResultsByIndexEvaluation::setFitResultValuesVisible(QFRawDataRecord* r, int index, double* values, double* errors)  {
    setFitResultValuesVisible(r, getEvaluationResultID(index, r),  values, errors);
}

void QFFitResultsByIndexEvaluation::setFitResultValuesVisibleWithGroupAndLabel(QFRawDataRecord* r, int index, const double *values, const double *errors, const QString& group, const bool *fix, const QString& fixGroup, bool sortPriority)  {
    setFitResultValuesVisibleWithGroupAndLabel(r, getEvaluationResultID(index, r),  values, errors, group, fix, fixGroup, sortPriority);
}

double QFFitResultsByIndexEvaluation::getFitValue(const QFRawDataRecord* r, int index, const QString& id) const {
    return getFitValue(r, getEvaluationResultID(index, r), id);
}

double QFFitResultsByIndexEvaluation::getFitError(const QFRawDataRecord* r, int index, const QString& id) const  {
    return getFitError(r, getEvaluationResultID(index, r), id);
}

void QFFitResultsByIndexEvaluation::setFitResultGroup(QFRawDataRecord* r, int index, const QString& parameterID, const QString& group)  {
    setFitResultGroup(r, getEvaluationResultID(index, r), parameterID, group);
}

void QFFitResultsByIndexEvaluation::setFitResultLabel(QFRawDataRecord* r, int index, const QString& parameterID, const QString& label, const QString& label_richtext)  {
    setFitResultLabel(r, getEvaluationResultID(index, r), parameterID, label, label_richtext);
}

void QFFitResultsByIndexEvaluation::setFitResultEvaluationGroup(QFRawDataRecord* r, int index, const QString& group)  {
    setFitResultEvaluationGroup(r, getEvaluationResultID(index, r), group);
}

void QFFitResultsByIndexEvaluation::setFitResultEvaluationDescription(QFRawDataRecord* r, int index, const QString& description)  {
    setFitResultEvaluationDescription(r, getEvaluationResultID(index, r), description);
}

void QFFitResultsByIndexEvaluation::setFitFix(QFRawDataRecord* r, int index, const QString& id, bool fix) {
    setFitFix(r, getEvaluationResultID(index, r), id, fix);
}

void QFFitResultsByIndexEvaluation::setFitResultFix(QFRawDataRecord* r, int index, const QString& id, bool fix)  {
    setFitResultFix(r, getEvaluationResultID(index, r), id, fix);
}

bool QFFitResultsByIndexEvaluation::getFitFix(const QFRawDataRecord* r, int index, const QString& id) const {
    return getFitFix(r, getEvaluationResultID(index, r), id);
}

void QFFitResultsByIndexEvaluation::fillParameters(const QFRawDataRecord *r, int index, double* param, QFFitFunction* function) const {
    fillParameters(r, getEvaluationResultID(index, r), param, function);
}

void QFFitResultsByIndexEvaluation::fillParameterErrors(const QFRawDataRecord *r, int index, double* param, QFFitFunction* function) const {
    fillParameterErrors(r, getEvaluationResultID(index, r), param, function);
}

void QFFitResultsByIndexEvaluation::fillFix(const QFRawDataRecord *r, int index, bool* param, QFFitFunction* function) const {
    fillFix(r, getEvaluationResultID(index, r), param, function);
}

double* QFFitResultsByIndexEvaluation::allocFillParameters(const QFRawDataRecord *r, int index, QFFitFunction* function) const {
    //if (function) qDebug()<<"QFFitResultsByIndexEvaluation::allocFillParameters("<<r->getName()<<", "<<index<<", "<<function->id()<<")";
    //else qDebug()<<"QFFitResultsByIndexEvaluation::allocFillParameters("<<r->getName()<<", "<<index<<", NULL)";
    return allocFillParameters(r, getEvaluationResultID(index, r), function);
}

double* QFFitResultsByIndexEvaluation::allocFillParameterErrors(const QFRawDataRecord *r, int index, QFFitFunction* function) const {
    return allocFillParameterErrors(r, getEvaluationResultID(index, r), function);
}

bool* QFFitResultsByIndexEvaluation::allocFillFix(const QFRawDataRecord *r, int index, QFFitFunction* function) const {
    return allocFillFix(r, getEvaluationResultID(index, r), function);
}

void QFFitResultsByIndexEvaluation::setCurrentIndex(int index) {
    QFRawDataRecord* r=getHighlightedRecord();
    if ((r!=NULL)) {
        if (index<=getIndexMin(r)) m_currentIndex=getIndexMin(r);
        if (index<=getIndexMax(r)) m_currentIndex=index;
        if (index>getIndexMax(r)) m_currentIndex=getIndexMax(r);
        r->setQFProperty(QString(getType()+QString::number(getID())+"_last_index"), m_currentIndex, false, false);
        emit currentIndexChanged(m_currentIndex);
    }
}

int QFFitResultsByIndexEvaluation::getCurrentIndex() const {
    QFRawDataRecord* r=getHighlightedRecord();
    int index=m_currentIndex;
    if (r!=NULL) {
        /*m_currentIndex=*/index=r->getProperty(QString(getType()+QString::number(getID())+"_last_index"), index).toInt();
    }
    if (index<getIndexMin(r)) {
        /*m_currentIndex=*/index=getIndexMin(r);
    }
    if (index>getIndexMax(r)) {
        /*m_currentIndex=*/index=getIndexMax(r);
    }
    return index;
}

QString QFFitResultsByIndexEvaluation::getIndexName(const QFRawDataRecord *rec, int index) const {
    return QString::number(index);
}


/*! \brief reset the all fit results to the initial/global/default value in the current file, but all indexs */
void QFFitResultsByIndexEvaluation::resetAllFitResultsCurrentFileAllIndices() {
    resetAllFitResultsAllIndices(getHighlightedRecord());
}

void QFFitResultsByIndexEvaluation::resetAllFitResultsAllIndices(QFRawDataRecord *rec)
{
    QFRawDataRecord* r=rec;
    if (!r) r=getHighlightedRecord();
    if (!r) return;
    r->disableEmitResultsChanged();
    for(int idx=getIndexMin(r); idx<=getIndexMax(r); idx++) {
        QString en=transformResultID(getEvaluationResultID(idx, r));
        r->resultsClear(en);
    }
    r->enableEmitResultsChanged();
}




/*! \brief reset all fit results to the initial/global/default value in all files and all indexs */
void QFFitResultsByIndexEvaluation::resetAllFitResultsAllFilesAllIndices() {
    QList<QPointer<QFRawDataRecord> > recs=getApplicableRecords();
    for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* r=recs[i];
        if (r) {
            r->disableEmitResultsChanged();
            for(int idx=getIndexMin(r); idx<=getIndexMax(r); idx++) {
                QString en=transformResultID(getEvaluationResultID(idx, r));
                r->resultsClear(en);
            }
            r->enableEmitResultsChanged();
        }
    }
}

/*! \brief reset all fit results to the initial/global/default value in all files and all indexs */
void QFFitResultsByIndexEvaluation::resetAllFitResults() {
    resetAllFitResultsAllFilesAllIndices();
}

/*! \brief reset all parameters to the initial/global/default value in all files and all indexs */
void QFFitResultsByIndexEvaluation::resetAllFitValue()  {
    QList<QPointer<QFRawDataRecord> > recs=getApplicableRecords();
    for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* r=recs[i];
        QFFitFunction* f=getFitFunction(r);
        if (r&&f) {
            r->disableEmitResultsChanged();
            for(int idx=getIndexMin(r); idx<=getIndexMax(r); idx++) {
                QString en=transformResultID(getEvaluationResultID(idx, r));
                for (int j=0; j<f->paramCount(); j++) {
                    QString id=f->getParameterID(j);
                    QString pid=getFitParamID(id);
                    if (r->resultsExists(en, pid)) r->resultsRemove(en, pid);
                }
            }
            r->enableEmitResultsChanged();
        }
    }
}




/*! \brief reset all parameters to the initial/global/default fix in all files and all indexs */
void QFFitResultsByIndexEvaluation::resetAllFitFix() {
    QList<QPointer<QFRawDataRecord> > recs=getApplicableRecords();
    for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* r=recs[i];
        QFFitFunction* f=getFitFunction(r);
        if (r&&f) {
            r->disableEmitResultsChanged();
            for(int idx=getIndexMin(r); idx<=getIndexMax(r); idx++) {
                QString en=transformResultID(getEvaluationResultID(idx, r));
                for (int j=0; j<f->paramCount(); j++) {
                    QString id=f->getParameterID(j);
                    QString pid=getFitParamFixID(id);
                    if (r->resultsExists(en, pid)) r->resultsRemove(en, pid);
                }
            }
            r->enableEmitResultsChanged();
        }
    }
}

/*! \brief set the given parameter \a id to the given value (and error) in all files, hasFit for the file is \c true */
void QFFitResultsByIndexEvaluation::setAllFitValues(const QString& id, double value, double error, bool currentFileOnly) {
    QList<QPointer<QFRawDataRecord> > recs;
    recs.clear();
    if (currentFileOnly) {
        recs.append(getHighlightedRecord());
    } else {
        recs=getApplicableRecords();
    }
    for (int i=0; i<recs.size(); i++) {
        QFFitFunction* f=getFitFunction(recs[i]);
        if (recs[i]&&f) {
            int pid=f->getParameterNum(id);
            QString unit="";
            if (pid>-1) unit=f->getDescription(pid).unit;
            recs[i]->disableEmitResultsChanged();
            for(int idx=getIndexMin(recs[i]); idx<=getIndexMax(recs[i]); idx++) {
                QString en=transformResultID(getEvaluationResultID(idx, recs[i]));
                if (hasFit(recs[i], idx)) recs[i]->resultsSetNumberError(en, getFitParamID(id), value, error, unit);
            }
            recs[i]->enableEmitResultsChanged();
        }
    }
    emitResultsChanged();
}

/*! \brief set the given parameter \a id to the given fix value */
void QFFitResultsByIndexEvaluation::setAllFitFixes(const QString& id, bool fix, bool currentFileOnly) {
    QList<QPointer<QFRawDataRecord> > recs;
    recs.clear();
    if (currentFileOnly) {
        recs.append(getHighlightedRecord());
    } else {
        recs=getApplicableRecords();
    }
    for (int i=0; i<recs.size(); i++) {
        QFFitFunction* f=getFitFunction(recs[i]);
        if (recs[i]&&f) {
            int pid=f->getParameterNum(id);
            QString unit="";
            if (pid>-1) unit=f->getDescription(pid).unit;
            recs[i]->disableEmitResultsChanged();
            for(int idx=getIndexMin(recs[i]); idx<=getIndexMax(recs[i]); idx++) {
                QString en=transformResultID(getEvaluationResultID(idx, recs[i]));
                if (hasFit(recs[i], idx)) recs[i]->resultsSetBoolean(en, getFitParamFixID(id), fix);
            }
            recs[i]->enableEmitResultsChanged();
        }
    }
    emitResultsChanged();
}


















