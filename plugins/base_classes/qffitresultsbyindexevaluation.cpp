#include "qffitresultsbyindexevaluation.h"

QFFitResultsByIndexEvaluation::QFFitResultsByIndexEvaluation(const QString& fitFunctionPrefix, QFProject* parent) :
    QFFitResultsEvaluation(fitFunctionPrefix, parent)
{
    m_currentIndex=0;
}


QFFitResultsByIndexEvaluation::~QFFitResultsByIndexEvaluation() {

}





QString QFFitResultsByIndexEvaluation::getEvaluationResultID() {
    return getEvaluationResultID(m_fitFunction, m_currentIndex);
}

QString QFFitResultsByIndexEvaluation::getEvaluationResultID(QString fitFunction) {
    return getEvaluationResultID(fitFunction, m_currentIndex);
}

QString QFFitResultsByIndexEvaluation::getEvaluationResultID(int currentIndex) {
    return getEvaluationResultID(m_fitFunction, currentIndex);
}









bool QFFitResultsByIndexEvaluation::hasFit(QFRawDataRecord* r1, int index) {
    QFRawDataRecord* r=r1;
    if (getFitFunction()==NULL) return false;
    if (r==NULL) r=getHighlightedRecord();
    if (r==NULL) return false;
    QString rsid=getEvaluationResultID(index);
    return r->resultsExistsFromEvaluation(rsid);
}




void QFFitResultsByIndexEvaluation::setFitResultValue(QFRawDataRecord* r, int index, const QString& id, double value) {
    setFitResultValue(r, getEvaluationResultID(index), id, value);
}
void QFFitResultsByIndexEvaluation::setFitResultValue(QFRawDataRecord* r, int index, const QString& id, double value, QString unit) {
    setFitResultValue(r, getEvaluationResultID(index), id, value, unit);
}

void QFFitResultsByIndexEvaluation::setFitResultValueString(QFRawDataRecord* r, int index, const QString& id, QString value) {
    setFitResultValueString(r, getEvaluationResultID(index), id, value);
}

void QFFitResultsByIndexEvaluation::setFitResultValueBool(QFRawDataRecord* r, int index, const QString& id, bool value) {
    setFitResultValueBool(r, getEvaluationResultID(index), id, value);
}

void QFFitResultsByIndexEvaluation::setFitResultValueInt(QFRawDataRecord* r, int index, const QString& id, int64_t value) {
    setFitResultValueInt(r, getEvaluationResultID(index), id, value);
}

void QFFitResultsByIndexEvaluation::setFitResultValueInt(QFRawDataRecord* r, int index, const QString& id, int64_t value, QString unit) {
    setFitResultValueInt(r, getEvaluationResultID(index), id, value, unit);
}

void QFFitResultsByIndexEvaluation::setFitValue(QFRawDataRecord* r, int index, const QString& id, double value) {
    setFitValue(r, getEvaluationResultID(index), id, value);
}

void QFFitResultsByIndexEvaluation::setFitError(QFRawDataRecord* r, int index, const QString& id, double error) {
    setFitError(r, getEvaluationResultID(index), id, error);
}

void QFFitResultsByIndexEvaluation::setFitResultValue(QFRawDataRecord* r, int index, const QString& id, double value, double error) {
    setFitResultValue(r, getEvaluationResultID(index), id, value, error);
}

void QFFitResultsByIndexEvaluation::setFitResultError(QFRawDataRecord* r, int index, const QString& id, double error) {
    setFitResultError(r, getEvaluationResultID(index), id, error);
}

void QFFitResultsByIndexEvaluation::setFitResultValues(QFRawDataRecord* r, int index, double* values, double* errors) {
    setFitResultValues(r, getEvaluationResultID(index), values, errors);
}

void QFFitResultsByIndexEvaluation::setFitResultValuesVisible(QFRawDataRecord* r, int index, double* values, double* errors) {
    setFitResultValuesVisible(r, getEvaluationResultID(index),  values, errors);
}

void QFFitResultsByIndexEvaluation::setFitResultValuesVisibleWithGroupAndLabel(QFRawDataRecord* r, int index, double* values, double* errors, const QString& group, bool* fix) {
    setFitResultValuesVisibleWithGroupAndLabel(r, getEvaluationResultID(index),  values, errors, group, fix);
}

double QFFitResultsByIndexEvaluation::getFitValue(QFRawDataRecord* r, int index, const QString& id) {
    return getFitValue(r, getEvaluationResultID(index), id);
}

double QFFitResultsByIndexEvaluation::getFitError(QFRawDataRecord* r, int index, const QString& id)  {
    return getFitError(r, getEvaluationResultID(index), id);
}

void QFFitResultsByIndexEvaluation::setFitResultGroup(QFRawDataRecord* r, int index, const QString& parameterID, const QString& group) {
    setFitResultGroup(r, getEvaluationResultID(index), parameterID, group);
}

void QFFitResultsByIndexEvaluation::setFitResultLabel(QFRawDataRecord* r, int index, const QString& parameterID, const QString& label, const QString& label_richtext) {
    setFitResultLabel(r, getEvaluationResultID(index), parameterID, label, label_richtext);
}

void QFFitResultsByIndexEvaluation::setFitResultEvaluationGroup(QFRawDataRecord* r, int index, const QString& group) {
    setFitResultEvaluationGroup(r, getEvaluationResultID(index), group);
}

void QFFitResultsByIndexEvaluation::setFitResultEvaluationDescription(QFRawDataRecord* r, int index, const QString& description) {
    setFitResultEvaluationDescription(r, getEvaluationResultID(index), description);
}

void QFFitResultsByIndexEvaluation::setFitFix(QFRawDataRecord* r, int index, const QString& id, bool fix) {
    setFitFix(r, getEvaluationResultID(index), id, fix);
}

void QFFitResultsByIndexEvaluation::setFitResultFix(QFRawDataRecord* r, int index, const QString& id, bool fix) {
    setFitResultFix(r, getEvaluationResultID(index), id, fix);
}

bool QFFitResultsByIndexEvaluation::getFitFix(QFRawDataRecord* r, int index, const QString& id) {
    return getFitFix(r, getEvaluationResultID(index), id);
}

void QFFitResultsByIndexEvaluation::fillParameters(QFRawDataRecord* r, int index, double* param) {
    fillParameters(r, getEvaluationResultID(index), param);
}

void QFFitResultsByIndexEvaluation::fillParameterErrors(QFRawDataRecord* r, int index, double* param) {
    fillParameterErrors(r, getEvaluationResultID(index), param);
}

void QFFitResultsByIndexEvaluation::fillFix(QFRawDataRecord* r, int index, bool* param) {
    fillFix(r, getEvaluationResultID(index), param);
}

double* QFFitResultsByIndexEvaluation::allocFillParameters(QFRawDataRecord* r, int index) {
    return allocFillParameters(r, getEvaluationResultID(index));
}

double* QFFitResultsByIndexEvaluation::allocFillParameterErrors(QFRawDataRecord* r, int index) {
    return allocFillParameterErrors(r, getEvaluationResultID(index));
}

bool* QFFitResultsByIndexEvaluation::allocFillFix(QFRawDataRecord* r, int index) {
    return allocFillFix(r, getEvaluationResultID(index));
}

void QFFitResultsByIndexEvaluation::setCurrentIndex(int index) {
    QFRawDataRecord* r=getHighlightedRecord();
    if ((r!=NULL)) {
        if (index<=getIndexMin(r)) m_currentIndex=getIndexMin(r);
        if (index<=getIndexMax(r)) m_currentIndex=index;
        if (index>getIndexMax(r)) m_currentIndex=getIndexMax(r);
        r->setQFProperty(QString(getType()+QString::number(getID())+"_last_index"), m_currentIndex, false, false);
    }
}

int QFFitResultsByIndexEvaluation::getCurrentIndex() {
    QFRawDataRecord* r=getHighlightedRecord();
    int index=m_currentIndex;
    if (r!=NULL) {
        m_currentIndex=index=r->getProperty(QString(getType()+QString::number(getID())+"_last_index"), index).toInt();
    }
    if (index<getIndexMin(r)) {
        m_currentIndex=index=getIndexMin(r);
    }
    if (index>getIndexMax(r)) {
        m_currentIndex=getIndexMax(r);
    }
    return index;
}


/*! \brief reset the all fit results to the initial/global/default value in the current file, but all indexs */
void QFFitResultsByIndexEvaluation::resetAllFitResultsCurrentFileAllIndices() {
    QFRawDataRecord* r=getHighlightedRecord();
    if (!r) return;

    for(int idx=getIndexMin(r); idx<=getIndexMax(r); idx++) {
        QString en=getEvaluationResultID(idx);
        r->resultsClear(en);
    }
}




/*! \brief reset all fit results to the initial/global/default value in all files and all indexs */
void QFFitResultsByIndexEvaluation::resetAllFitResultsAllFilesAllIndices() {
    QFFitFunction* f=getFitFunction();
    if (f==NULL) return ;
    QList<QPointer<QFRawDataRecord> > recs=getApplicableRecords();
    for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* r=recs[i];
        if (r) {
            for(int idx=getIndexMin(r); idx<=getIndexMax(r); idx++) {
                QString en=getEvaluationResultID(idx);
                r->resultsClear(en);
            }
        }
    }
}

/*! \brief reset all parameters to the initial/global/default value in all files and all indexs */
void QFFitResultsByIndexEvaluation::resetAllFitValue()  {
    QFFitFunction* f=getFitFunction();
    if (f==NULL) return ;
    QList<QPointer<QFRawDataRecord> > recs=getApplicableRecords();
    for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* r=recs[i];
        if (r) {
            for(int idx=getIndexMin(r); idx<=getIndexMax(r); idx++) {
                QString en=getEvaluationResultID(idx);
                for (int j=0; j<f->paramCount(); j++) {
                    QString id=f->getParameterID(j);
                    QString pid=getFitParamID(id);
                    if (r->resultsExists(en, pid)) r->resultsRemove(en, pid);
                }
            }
        }
    }
}


/*! \brief reset all fit results to the initial/global/default value in all files and all indexs */
void QFFitResultsByIndexEvaluation::resetAllFitResults() {
    QList<QPointer<QFRawDataRecord> > recs=getApplicableRecords();
    for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* r=recs[i];
        if (r) {
            for(int idx=getIndexMin(r); idx<=getIndexMax(r); idx++) {
                QString en=getEvaluationResultID(idx);
                r->resultsClear(en);
            }
        }
    }
}

/*! \brief reset all parameters to the initial/global/default fix in all files and all indexs */
void QFFitResultsByIndexEvaluation::resetAllFitFix() {
    QFFitFunction* f=getFitFunction();
    if (f==NULL) return ;
    QList<QPointer<QFRawDataRecord> > recs=getApplicableRecords();
    for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* r=recs[i];
        if (r) {
            for(int idx=getIndexMin(r); idx<=getIndexMax(r); idx++) {
                QString en=getEvaluationResultID(idx);
                for (int j=0; j<f->paramCount(); j++) {
                    QString id=f->getParameterID(j);
                    QString pid=getFitParamFixID(id);
                    if (r->resultsExists(en, pid)) r->resultsRemove(en, pid);
                }
            }
        }
    }
}

/*! \brief set the given parameter \a id to the given value (and error) in all files, hasFit for the file is \c true */
void QFFitResultsByIndexEvaluation::setAllFitValues(const QString& id, double value, double error) {
    QFFitFunction* f=getFitFunction();
    if (f==NULL) return ;
    QList<QPointer<QFRawDataRecord> > recs=getApplicableRecords();
    QString en=getEvaluationResultID();
    int pid=f->getParameterNum(id);
    QString unit="";
    if (pid>-1) unit=f->getDescription(pid).unit;
    for (int i=0; i<recs.size(); i++) {
        if (recs[i]) {
            for(int idx=getIndexMin(recs[i]); idx<=getIndexMax(recs[i]); idx++) {
                QString en=getEvaluationResultID(idx);
                if (hasFit(recs[i], idx)) recs[i]->resultsSetNumberError(en, getFitParamID(id), value, error, unit);
            }
        }
    }
    emit resultsChanged();
}

/*! \brief set the given parameter \a id to the given fix value */
void QFFitResultsByIndexEvaluation::setAllFitFixes(const QString& id, bool fix) {
    QFFitFunction* f=getFitFunction();
    if (f==NULL) return ;
    QList<QPointer<QFRawDataRecord> > recs=getApplicableRecords();
    QString en=getEvaluationResultID();
    int pid=f->getParameterNum(id);
    QString unit="";
    if (pid>-1) unit=f->getDescription(pid).unit;
    for (int i=0; i<recs.size(); i++) {
        if (recs[i]) {
            for(int idx=getIndexMin(recs[i]); idx<=getIndexMax(recs[i]); idx++) {
                QString en=getEvaluationResultID(idx);
                if (hasFit(recs[i], idx)) recs[i]->resultsSetBoolean(en, getFitParamFixID(id), fix);
            }
        }
    }
    emit resultsChanged();
}

