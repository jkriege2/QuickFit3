#include "qffitresultsbyindexevaluation.h"

QFFitResultsByIndexEvaluation::QFFitResultsByIndexEvaluation(const QString& fitFunctionPrefix, QFProject* parent, bool showRDRList, bool useSelection) :
    QFFitResultsEvaluation(fitFunctionPrefix, parent, showRDRList, useSelection)
{
    m_currentIndex=0;
}


QFFitResultsByIndexEvaluation::~QFFitResultsByIndexEvaluation() {
}

QString QFFitResultsByIndexEvaluation::getEvaluationResultID(QString fitFunction, int currentIndex) const {
    //qDebug()<<"QFFitResultsByIndexEvaluation::getEvaluationResultID("<<fitFunction<<", "<<currentIndex<<")";
    if (currentIndex<0) return QString("%1_%2_%3_runavg").arg(getType()).arg(getID()).arg(fitFunction);
    return QString("%1_%2_%3_run%4").arg(getType()).arg(getID()).arg(fitFunction).arg(currentIndex);
}

int QFFitResultsByIndexEvaluation::getIndexFromEvaluationResultID(const QString &resultID) const {
    if (resultID.size()<=0) return -1;
    if (resultID.endsWith("runavg")) return -1;
    int l=0;
    while (resultID[resultID.size()-1-l].isDigit()) {
        l++;
    }


    if (l>0) {
        //qDebug()<<"QFFitResultsByIndexEvaluation::getIndexFromEvaluationResultID("<<resultID<<"):  l="<<l<<"  => result="<<resultID.right(l).toInt();
        return resultID.right(l).toInt();
    }
    //qDebug()<<"QFFitResultsByIndexEvaluation::getIndexFromEvaluationResultID("<<resultID<<"):  l="<<l<<"  => result=-1";
    return -1;
}





QString QFFitResultsByIndexEvaluation::getEvaluationResultID( QFRawDataRecord *r) const {
    return getEvaluationResultID(getFitFunctionID(r), m_currentIndex);
}

QString QFFitResultsByIndexEvaluation::getEvaluationResultID(QString fitFunction) const {
    return getEvaluationResultID(fitFunction, m_currentIndex);
}

QString QFFitResultsByIndexEvaluation::getEvaluationResultID(int currentIndex, QFRawDataRecord *r) const {
    //qDebug()<<"QFFitResultsByIndexEvaluation::getEvaluationResultID("<<currentIndex<<", "<<r->getName()<<")";
    return getEvaluationResultID(getFitFunctionID(r), currentIndex);
}

bool QFFitResultsByIndexEvaluation::hasSpecial(const QFRawDataRecord *r, int index, const QString &paramid, double &value, double &error) const {
    return false;
}









bool QFFitResultsByIndexEvaluation::hasFit(QFRawDataRecord* r1, int index) const {
    QFRawDataRecord* r=r1;
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

void QFFitResultsByIndexEvaluation::setFitResultValuesVisibleWithGroupAndLabel(QFRawDataRecord* r, int index, double* values, double* errors, const QString& group, bool* fix, const QString& fixGroup, bool sortPriority)  {
    setFitResultValuesVisibleWithGroupAndLabel(r, getEvaluationResultID(index, r),  values, errors, group, fix, fixGroup, sortPriority);
}

double QFFitResultsByIndexEvaluation::getFitValue(QFRawDataRecord* r, int index, const QString& id) const {
    return getFitValue(r, getEvaluationResultID(index, r), id);
}

double QFFitResultsByIndexEvaluation::getFitError(QFRawDataRecord* r, int index, const QString& id) const  {
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

bool QFFitResultsByIndexEvaluation::getFitFix(QFRawDataRecord* r, int index, const QString& id) const {
    return getFitFix(r, getEvaluationResultID(index, r), id);
}

void QFFitResultsByIndexEvaluation::fillParameters(QFRawDataRecord* r, int index, double* param, QFFitFunction* function) const {
    fillParameters(r, getEvaluationResultID(index, r), param, function);
}

void QFFitResultsByIndexEvaluation::fillParameterErrors(QFRawDataRecord* r, int index, double* param, QFFitFunction* function) const {
    fillParameterErrors(r, getEvaluationResultID(index, r), param, function);
}

void QFFitResultsByIndexEvaluation::fillFix(QFRawDataRecord* r, int index, bool* param, QFFitFunction* function) const {
    fillFix(r, getEvaluationResultID(index, r), param, function);
}

double* QFFitResultsByIndexEvaluation::allocFillParameters(QFRawDataRecord* r, int index, QFFitFunction* function) const {
    //qDebug()<<"QFFitResultsByIndexEvaluation::allocFillParameters("<<r->getName()<<", "<<index<<", "<<function->id()<<")";
    return allocFillParameters(r, getEvaluationResultID(index, r), function);
}

double* QFFitResultsByIndexEvaluation::allocFillParameterErrors(QFRawDataRecord* r, int index, QFFitFunction* function) const {
    return allocFillParameterErrors(r, getEvaluationResultID(index, r), function);
}

bool* QFFitResultsByIndexEvaluation::allocFillFix(QFRawDataRecord* r, int index, QFFitFunction* function) const {
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

QString QFFitResultsByIndexEvaluation::getIndexName(QFRawDataRecord *rec, int index) const {
    return QString::number(index);
}


/*! \brief reset the all fit results to the initial/global/default value in the current file, but all indexs */
void QFFitResultsByIndexEvaluation::resetAllFitResultsCurrentFileAllIndices() {
    QFRawDataRecord* r=getHighlightedRecord();
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


















