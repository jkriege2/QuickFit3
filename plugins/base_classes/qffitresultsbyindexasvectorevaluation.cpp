#include "qffitresultsbyindexasvectorevaluation.h"

QFFitResultsByIndexAsVectorEvaluation::QFFitResultsByIndexAsVectorEvaluation(const QString &fitFunctionPrefix, QFProject *parent):
    QFFitResultsByIndexEvaluation(fitFunctionPrefix, parent)
{
}

QFFitResultsByIndexAsVectorEvaluation::~QFFitResultsByIndexAsVectorEvaluation() {
}





/*







void QFFitResultsByIndexAsVectorEvaluation::setAllFitValues(const QString& id, double value, double error, bool currentFileOnly) {
    QFFitFunction* f=getFitFunction();
    if (f==NULL) return ;
    QList<QPointer<QFRawDataRecord> > recs;
    recs.clear();
    if (currentFileOnly) {
        recs.append(getHighlightedRecord());
    } else {
        recs=getApplicableRecords();
    }
    QString en=getEvaluationResultID();
    int pid=f->getParameterNum(id);
    QString unit="";
    if (pid>-1) unit=f->getDescription(pid).unit;
    for (int i=0; i<recs.size(); i++) {
        if (recs[i]) {
            recs[i]->disableEmitResultsChanged();
            for(int idx=getIndexMin(recs[i]); idx<=getIndexMax(recs[i]); idx++) {
                QString en=getEvaluationResultID(idx);
                if (hasFit(recs[i], idx)) {
                    if (idx<0) recs[i]->resultsSetNumberError(en, getFitParamID(id), value, error, unit);
                    else  recs[i]->resultsSetInNumberErrorList(en, getFitParamID(id), idx, value, error, unit);
                }
            }
            recs[i]->enableEmitResultsChanged();
        }
    }
    emitResultsChanged();
}


void QFFitResultsByIndexAsVectorEvaluation::setAllFitFixes(const QString& id, bool fix, bool currentFileOnly) {
    QFFitFunction* f=getFitFunction();
    if (f==NULL) return ;
    QList<QPointer<QFRawDataRecord> > recs;
    recs.clear();
    if (currentFileOnly) {
        recs.append(getHighlightedRecord());
    } else {
        recs=getApplicableRecords();
    }
    QString en=getEvaluationResultID();
    int pid=f->getParameterNum(id);
    QString unit="";
    if (pid>-1) unit=f->getDescription(pid).unit;
    for (int i=0; i<recs.size(); i++) {
        if (recs[i]) {
            recs[i]->disableEmitResultsChanged();
            for(int idx=getIndexMin(recs[i]); idx<=getIndexMax(recs[i]); idx++) {
                QString en=getEvaluationResultID(idx);
                if (hasFit(recs[i], idx)) {
                    if (idx<0) recs[i]->resultsSetBoolean(en, getFitParamID(id), fix);
                    else  recs[i]->resultsSetInBooleanList(en, getFitParamID(id), idx, fix);
                }
            }
            recs[i]->enableEmitResultsChanged();
        }
    }
    emitResultsChanged();
}





















void QFFitResultsByIndexAsVectorEvaluation::setFitResultValue(QFRawDataRecord* r, int index, const QString& id, double value) {
    if (index<0) QFFitResultsByIndexEvaluation::setFitResultValue(r, getEvaluationResultID(index), id, value);
}
void QFFitResultsByIndexAsVectorEvaluation::setFitResultValue(QFRawDataRecord* r, int index, const QString& id, double value, QString unit) {
    if (index<0) QFFitResultsByIndexEvaluation::setFitResultValue(r, getEvaluationResultID(index), id, value, unit);
}

void QFFitResultsByIndexAsVectorEvaluation::setFitResultValueString(QFRawDataRecord* r, int index, const QString& id, QString value) {
    if (index<0) QFFitResultsByIndexEvaluation::setFitResultValueString(r, getEvaluationResultID(index), id, value);
}

void QFFitResultsByIndexAsVectorEvaluation::setFitResultValueBool(QFRawDataRecord* r, int index, const QString& id, bool value) {
    if (index<0) QFFitResultsByIndexEvaluation::setFitResultValueBool(r, getEvaluationResultID(index), id, value);
}

void QFFitResultsByIndexAsVectorEvaluation::setFitResultValueInt(QFRawDataRecord* r, int index, const QString& id, int64_t value) {
    if (index<0) QFFitResultsByIndexEvaluation::setFitResultValueInt(r, getEvaluationResultID(index), id, value);
}

void QFFitResultsByIndexAsVectorEvaluation::setFitResultValueInt(QFRawDataRecord* r, int index, const QString& id, int64_t value, QString unit) {
    if (index<0) QFFitResultsByIndexEvaluation::setFitResultValueInt(r, getEvaluationResultID(index), id, value, unit);
}

void QFFitResultsByIndexAsVectorEvaluation::setFitValue(QFRawDataRecord* r, int index, const QString& id, double value) {
    if (index<0) QFFitResultsByIndexEvaluation::setFitValue(r, getEvaluationResultID(index), id, value);
}

void QFFitResultsByIndexAsVectorEvaluation::setFitError(QFRawDataRecord* r, int index, const QString& id, double error) {
    if (index<0) QFFitResultsByIndexEvaluation::setFitError(r, getEvaluationResultID(index), id, error);
}

void QFFitResultsByIndexAsVectorEvaluation::setFitResultValue(QFRawDataRecord* r, int index, const QString& id, double value, double error) {
    if (index<0) QFFitResultsByIndexEvaluation::setFitResultValue(r, getEvaluationResultID(index), id, value, error);
}

void QFFitResultsByIndexAsVectorEvaluation::setFitResultError(QFRawDataRecord* r, int index, const QString& id, double error) {
    if (index<0) QFFitResultsByIndexEvaluation::setFitResultError(r, getEvaluationResultID(index), id, error);
}

void QFFitResultsByIndexAsVectorEvaluation::setFitResultValues(QFRawDataRecord* r, int index, double* values, double* errors) {
    if (index<0) QFFitResultsByIndexEvaluation::setFitResultValues(r, getEvaluationResultID(index), values, errors);
}

void QFFitResultsByIndexAsVectorEvaluation::setFitResultValuesVisible(QFRawDataRecord* r, int index, double* values, double* errors) {
    if (index<0) QFFitResultsByIndexEvaluation::setFitResultValuesVisible(r, getEvaluationResultID(index),  values, errors);
}

void QFFitResultsByIndexAsVectorEvaluation::setFitResultValuesVisibleWithGroupAndLabel(QFRawDataRecord* r, int index, double* values, double* errors, const QString& group, bool* fix, const QString& fixGroup, bool sortPriority) {
    if (index<0) QFFitResultsByIndexEvaluation::setFitResultValuesVisibleWithGroupAndLabel(r, getEvaluationResultID(index),  values, errors, group, fix, fixGroup, sortPriority);
}

double QFFitResultsByIndexAsVectorEvaluation::getFitValue(QFRawDataRecord* r, int index, const QString& id) {
    if (index<0) return QFFitResultsByIndexEvaluation::getFitValue(r, getEvaluationResultID(index), id);
}

double QFFitResultsByIndexAsVectorEvaluation::getFitError(QFRawDataRecord* r, int index, const QString& id)  {
    if (index<0) return QFFitResultsByIndexEvaluation::getFitError(r, getEvaluationResultID(index), id);
}

void QFFitResultsByIndexAsVectorEvaluation::setFitResultGroup(QFRawDataRecord* r, int index, const QString& parameterID, const QString& group) {
    if (index<0) QFFitResultsByIndexEvaluation::setFitResultGroup(r, getEvaluationResultID(index), parameterID, group);
}

void QFFitResultsByIndexAsVectorEvaluation::setFitResultLabel(QFRawDataRecord* r, int index, const QString& parameterID, const QString& label, const QString& label_richtext) {
    if (index<0) QFFitResultsByIndexEvaluation::setFitResultLabel(r, getEvaluationResultID(index), parameterID, label, label_richtext);
}

void QFFitResultsByIndexAsVectorEvaluation::setFitResultEvaluationGroup(QFRawDataRecord* r, int index, const QString& group) {
    if (index<0) QFFitResultsByIndexEvaluation::setFitResultEvaluationGroup(r, getEvaluationResultID(index), group);
}

void QFFitResultsByIndexAsVectorEvaluation::setFitResultEvaluationDescription(QFRawDataRecord* r, int index, const QString& description) {
    if (index<0) QFFitResultsByIndexEvaluation::setFitResultEvaluationDescription(r, getEvaluationResultID(index), description);
}

void QFFitResultsByIndexAsVectorEvaluation::setFitFix(QFRawDataRecord* r, int index, const QString& id, bool fix) {
    if (index<0) QFFitResultsByIndexEvaluation::setFitFix(r, getEvaluationResultID(index), id, fix);
}

void QFFitResultsByIndexAsVectorEvaluation::setFitResultFix(QFRawDataRecord* r, int index, const QString& id, bool fix) {
    if (index<0) QFFitResultsByIndexEvaluation::setFitResultFix(r, getEvaluationResultID(index), id, fix);
}

bool QFFitResultsByIndexAsVectorEvaluation::getFitFix(QFRawDataRecord* r, int index, const QString& id) {
    if (index<0) return QFFitResultsByIndexEvaluation::getFitFix(r, getEvaluationResultID(index), id);
}

void QFFitResultsByIndexAsVectorEvaluation::fillParameters(QFRawDataRecord* r, int index, double* param) {
    if (index<0) QFFitResultsByIndexEvaluation::fillParameters(r, getEvaluationResultID(index), param);
}

void QFFitResultsByIndexAsVectorEvaluation::fillParameterErrors(QFRawDataRecord* r, int index, double* param) {
    if (index<0) QFFitResultsByIndexEvaluation::fillParameterErrors(r, getEvaluationResultID(index), param);
}

void QFFitResultsByIndexAsVectorEvaluation::fillFix(QFRawDataRecord* r, int index, bool* param) {
    if (index<0) QFFitResultsByIndexEvaluation::fillFix(r, getEvaluationResultID(index), param);
}

double* QFFitResultsByIndexAsVectorEvaluation::allocFillParameters(QFRawDataRecord* r, int index) {
    if (index<0)return  QFFitResultsByIndexEvaluation::allocFillParameters(r, getEvaluationResultID(index));
}

double* QFFitResultsByIndexAsVectorEvaluation::allocFillParameterErrors(QFRawDataRecord* r, int index) {
    if (index<0) return QFFitResultsByIndexEvaluation::allocFillParameterErrors(r, getEvaluationResultID(index));
}

bool* QFFitResultsByIndexAsVectorEvaluation::allocFillFix(QFRawDataRecord* r, int index) {
    if (index<0) return QFFitResultsByIndexEvaluation::allocFillFix(r, getEvaluationResultID(index));
}







































void QFFitResultsEvaluation::setFitResultValue(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double value) {
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction();
        QString unit="";
        if (f) {
            int pid=f->getParameterNum(parameterID);
            if (pid>-1) unit=f->getDescription(pid).unit;
        }
        r->resultsSetNumber(resultID, getFitParamID(parameterID), value, unit);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}
void QFFitResultsEvaluation::setFitResultValue(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double value, QString unit) {
    if (r!=NULL) {
        //QFFitFunction* f=getFitFunction();
        r->resultsSetNumber(resultID, getFitParamID(parameterID), value, unit);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}

void QFFitResultsEvaluation::setFitResultValueString(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, QString value) {
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction();
        QString unit="";
        if (f) {
            int pid=f->getParameterNum(parameterID);
            if (pid>-1) unit=f->getDescription(pid).unit;
        }
        r->resultsSetString(resultID, getFitParamID(parameterID), value);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}

void QFFitResultsEvaluation::setFitResultValueBool(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, bool value) {
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction();
        QString unit="";
        if (f) {
            int pid=f->getParameterNum(parameterID);
            if (pid>-1) unit=f->getDescription(pid).unit;
        }
        r->resultsSetBoolean(resultID, getFitParamID(parameterID), value);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}

void QFFitResultsEvaluation::setFitResultValueInt(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, int64_t value) {
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction();
        QString unit="";
        if (f) {
            int pid=f->getParameterNum(parameterID);
            if (pid>-1) unit=f->getDescription(pid).unit;
        }
        r->resultsSetInteger(resultID, getFitParamID(parameterID), value, unit);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}

void QFFitResultsEvaluation::setFitResultValueInt(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, int64_t value, QString unit) {
    if (r!=NULL) {
        //QFFitFunction* f=getFitFunction();
        r->resultsSetInteger(resultID, getFitParamID(parameterID), value, unit);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}

void QFFitResultsEvaluation::setFitValue(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double value) {
    if (r!=NULL) {
        QString dsid=getParameterStoreID(parameterID);
        if (hasFit(r, resultID)) {
            setFitResultValue(r, resultID, parameterID, value, getFitError(parameterID));
        } else {
            QFFitFunction* f=getFitFunction();
            if (f) {
                QFFitFunction::ParameterDescription d=f->getDescription(parameterID);
                if (d.userEditable) {
                    parameterStore[getParameterStoreID(parameterID)].value=value;
                    parameterStore[getParameterStoreID(parameterID)].valueSet=true;
                    emitPropertiesChanged();
                }
            }
        }

    }
}

void QFFitResultsEvaluation::setFitError(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double error) {
    if (r!=NULL) {
        QString dsid=getParameterStoreID(parameterID);
        if (hasFit(r, resultID)) {
            setFitResultError(r, resultID, parameterID, error);
        } else {
            QFFitFunction* f=getFitFunction();
            if (f) {
                QFFitFunction::ParameterDescription d=f->getDescription(parameterID);
                if (d.userEditable) {
                    parameterStore[getParameterStoreID(parameterID)].error=error;
                    parameterStore[getParameterStoreID(parameterID)].errorSet=true;
                    emitPropertiesChanged();
                }
            }
        }

    }
}

void QFFitResultsEvaluation::setFitResultValue(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double value, double error) {
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction();
        QString unit="";
        if (f) {
            int pid=f->getParameterNum(parameterID);
            if (pid>-1) unit=f->getDescription(pid).unit;
        }
        r->resultsSetNumberError(resultID, getFitParamID(parameterID), value, error, unit);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}

void QFFitResultsEvaluation::setFitResultError(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double error) {
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction();
        QString unit="";
        if (f) {
            int pid=f->getParameterNum(parameterID);
            if (pid>-1) unit=f->getDescription(pid).unit;
        }
        r->resultsSetNumberError(resultID, getFitParamID(parameterID), getFitValue(parameterID), error, unit);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}

void QFFitResultsEvaluation::setFitResultValues(QFRawDataRecord* r, const QString& resultID, double* values, double* errors) {
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction();
        if (f) {
            for (int i=0; i<f->paramCount(); i++) {
                QString pid=f->getParameterID(i);
                //setFitResultValue(pid, values[i], errors[i]);
                QString unit=f->getDescription(pid).unit;
                r->resultsSetNumberError(resultID, getFitParamID(pid), values[i], errors[i], unit);
            }
            emitPropertiesChanged();
            emitResultsChanged();
        }

    }
}

void QFFitResultsEvaluation::setFitResultValuesVisible(QFRawDataRecord* r, const QString& resultID, double* values, double* errors) {
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction();
        if (f) {
            for (int i=0; i<f->paramCount(); i++) {
                if (f->isParameterVisible(i, values)) {
                    QString pid=f->getParameterID(i);
                    //setFitResultValue(pid, values[i], errors[i]);
                    QString unit=f->getDescription(pid).unit;
                    r->resultsSetNumberError(resultID, getFitParamID(pid), values[i], errors[i], unit);
                }
            }
            emitPropertiesChanged();
            emitResultsChanged();
        }

    }
}

void QFFitResultsEvaluation::setFitResultValuesVisibleWithGroupAndLabel(QFRawDataRecord* r, const QString& resultID, double* values, double* errors, const QString& group, bool* fix, const QString& fixGroup, bool sortPriority) {
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction();
        if (f) {
            for (int i=0; i<f->paramCount(); i++) {
                if (f->isParameterVisible(i, values)) {
                    QString pid=f->getParameterID(i);
                    //setFitResultValue(pid, values[i], errors[i]);
                    QString unit=f->getDescription(pid).unit;
                    QString fpid=getFitParamID(pid);
                    QString ffid= getFitParamFixID(pid);
                    r->resultsSetNumberError(resultID, fpid, values[i], errors[i], unit);
                    r->resultsSetGroup(resultID, fpid, group);
                    r->resultsSetLabel(resultID, fpid, f->getDescription(pid).name, f->getDescription(pid).label);
                    r->resultsSetSortPriority(resultID, fpid, sortPriority);
                    if (fix) {
                        r->resultsSetBoolean(resultID, ffid, fix[i]);
                        if (!fixGroup.isEmpty()) r->resultsSetGroup(resultID,ffid, fixGroup);
                        r->resultsSetLabel(resultID, ffid, f->getDescription(pid).name+tr(", fix"), f->getDescription(pid).label+tr(", fix"));
                        r->resultsSetSortPriority(resultID,  ffid, sortPriority);
                    }
                }
            }
            emitPropertiesChanged();
            emitResultsChanged();
        }

    }
}

bool QFFitResultsEvaluation::hasSpecial(QFRawDataRecord* r, const QString& id, const QString& paramid, double& value, double& error) {
    return false;
}

double QFFitResultsEvaluation::getFitValue(QFRawDataRecord* r, const QString& resultID, const QString& parameterID) {
    QFFitFunction* f=getFitFunction();
    if (f==NULL) {
        return 0;
    }
    if (!f->hasParameter(parameterID)) return 0;
    int pid=f->getParameterNum(parameterID);
    double res=0;
    if (pid>-1) res=f->getDescription(pid).initialValue;
    res=fitParamGlobalSettings->value(QString(m_fitFunction+"/"+parameterID), res).toDouble();
    res=fitParamSettings->value(QString(m_fitFunction+"/"+parameterID), res).toDouble();
    QString psID=getParameterStoreID(parameterID);
    if (parameterStore.contains(psID)) {
        if (parameterStore[psID].valueSet) {
            res=parameterStore[psID].value;
        }
    }
    //if (specials.contains(parameterID)) res=specials[parameterID];
    double sval=res, serr=0;
    if (hasSpecial(r, resultID, parameterID, sval, serr)) {
        res=sval;
    } else if (hasFit(r, resultID)) {
        QString en=resultID;
        QString pid=getFitParamID(parameterID);
        if (r->resultsExists(en, pid)) res=r->resultsGetAsDouble(en, pid);
    }
    return res;
}

double QFFitResultsEvaluation::getFitError(QFRawDataRecord* r, const QString& resultID, const QString& parameterID)  {

    //if (specials.contains(parameterID)) return specials[parameterID];
    double sval=0, serr=0;
    if (hasSpecial(r, resultID, parameterID, sval, serr)) {
       return serr;
    }
    if (hasFit(r, resultID)) {
        if (r!=NULL) {
            return r->resultsGetErrorAsDouble(resultID, getFitParamID(parameterID));
        }
    }
    QString psID=getParameterStoreID(parameterID);
    if (parameterStore.contains(psID)) {
        if (parameterStore[psID].errorSet) {
            return parameterStore[psID].error;
        }
    }
    return 0.0;
}

void QFFitResultsEvaluation::setFitFix(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, bool fix) {
    if (r!=NULL) {
        QString dsid=getParameterStoreID(parameterID);
        if (hasFit(r, resultID)) {
            setFitResultFix(r, resultID, parameterID, fix);
        } else {
            QFFitFunction* f=getFitFunction();
            if (f) {
                QFFitFunction::ParameterDescription d=f->getDescription(parameterID);
                if (d.userEditable) {
                    parameterStore[getParameterStoreID(parameterID)].fix=fix;
                    parameterStore[getParameterStoreID(parameterID)].fixSet=true;
                    emitPropertiesChanged();
                }
            }
        }

    }
}

void QFFitResultsEvaluation::setFitResultFix(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, bool fix) {
    if (r!=NULL) {
        QFFitFunction* f=getFitFunction();
        QString unit="";
        if (f) {
            int pid=f->getParameterNum(parameterID);
            if (pid>-1) unit=f->getDescription(pid).unit;
        }
        r->resultsSetBoolean(resultID, getFitParamFixID(parameterID), fix);
        emitResultsChanged(r, resultID, getFitParamFixID(parameterID));
    }
}

bool QFFitResultsEvaluation::getFitFix(QFRawDataRecord* r, const QString& resultID, const QString& parameterID) {
    QFFitFunction* f=getFitFunction();
    if (f==NULL) return 0;
    bool res=false;
    res=fitParamGlobalSettings->value(QString(m_fitFunction+"/"+parameterID+"_fix"), res).toBool();
    res=fitParamSettings->value(QString(m_fitFunction+"/"+parameterID+"_fix"), res).toBool();
    QString psID=getParameterStoreID(parameterID);
    if (parameterStore.contains(psID)) {
        if (parameterStore[psID].fixSet) {
            res=parameterStore[psID].fix;
        }
    }
    if (hasFit(r, resultID)) {
        QString en=resultID;
        QString pid=getFitParamFixID(parameterID);
        if (r->resultsExists(en, pid)) res=r->resultsGetAsDouble(en, pid);
    }
    return res;
}

void QFFitResultsEvaluation::setFitResultGroup(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, const QString& group) {
    if (r!=NULL) {
        //QFFitFunction* f=getFitFunction();
        QString unit="";
        r->resultsSetGroup(resultID, getFitParamID(parameterID), group);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}

void QFFitResultsEvaluation::setFitResultLabel(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, const QString& label, const QString& label_richtext) {
    if (r!=NULL) {
        //QFFitFunction* f=getFitFunction();
        QString unit="";
        r->resultsSetLabel(resultID, getFitParamID(parameterID), label, label_richtext);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}


void QFFitResultsEvaluation::setFitResultEvaluationGroup(QFRawDataRecord* r, const QString& resultID, const QString& group) {
    if (r!=NULL) {
        r->resultsSetEvaluationGroup(resultID, group);
        emitResultsChanged(r, resultID);
    }
}


void QFFitResultsEvaluation::setFitResultEvaluationDescription(QFRawDataRecord* r, const QString& resultID, const QString& description) {
    if (r!=NULL) {
        r->resultsSetEvaluationDescription(resultID, description);
        emitResultsChanged(r, resultID);
    }
}

void QFFitResultsEvaluation::setFitResultGroup(const QString& parameterID, const QString& group) {
    setFitResultGroup(getHighlightedRecord(), getEvaluationResultID(), parameterID, group);
}

void QFFitResultsEvaluation::setFitResultLabel(const QString& parameterID, const QString& label, const QString& label_richtext) {
    setFitResultLabel(getHighlightedRecord(), getEvaluationResultID(), parameterID, label, label_richtext);
}

void QFFitResultsEvaluation::setFitResultEvaluationGroup(const QString& group) {
    setFitResultEvaluationGroup(getHighlightedRecord(), getEvaluationResultID(), group);
}

void QFFitResultsEvaluation::setFitResultEvaluationDescription(const QString& description) {
    setFitResultEvaluationDescription(getHighlightedRecord(), getEvaluationResultID(), description);
}


void QFFitResultsEvaluation::setFitResultValue(const QString& id, double value) {
    setFitResultValue(getHighlightedRecord(), getEvaluationResultID(), id, value);
}


void QFFitResultsEvaluation::setFitResultValue(const QString& id, double value, QString unit) {
    setFitResultValue(getHighlightedRecord(), getEvaluationResultID(), id, value, unit);
}

void QFFitResultsEvaluation::setFitResultValueString(const QString& id, QString value) {
    setFitResultValueString(getHighlightedRecord(), getEvaluationResultID(), id, value);
}

void QFFitResultsEvaluation::setFitResultValueBool(const QString& id, bool value) {
    setFitResultValueBool(getHighlightedRecord(), getEvaluationResultID(), id, value);
}

void QFFitResultsEvaluation::setFitResultValueInt(const QString& id, int64_t value) {
    setFitResultValueInt(getHighlightedRecord(), getEvaluationResultID(), id, value);
}

void QFFitResultsEvaluation::setFitResultValueInt(const QString& id, int64_t value, QString unit) {
    setFitResultValueInt(getHighlightedRecord(), getEvaluationResultID(), id, value, unit);
}

void QFFitResultsEvaluation::setFitValue(const QString& id, double value) {
    setFitValue(getHighlightedRecord(), getEvaluationResultID(), id, value);
}

void QFFitResultsEvaluation::setFitError(const QString& id, double error) {
    setFitError(getHighlightedRecord(), getEvaluationResultID(), id, error);
}

void QFFitResultsEvaluation::setFitResultValue(const QString& id, double value, double error) {
    setFitResultValue(getHighlightedRecord(), getEvaluationResultID(), id, value, error);
}

void QFFitResultsEvaluation::setFitResultError(const QString& id, double error) {
    setFitResultError(getHighlightedRecord(), getEvaluationResultID(), id, error);
}

void QFFitResultsEvaluation::setFitResultValues(double* values, double* errors) {
    setFitResultValues(getHighlightedRecord(), getEvaluationResultID(), values, errors);
}

void QFFitResultsEvaluation::setFitResultValuesVisible(double* values, double* errors) {
    setFitResultValuesVisible(getHighlightedRecord(), getEvaluationResultID(), values, errors);
}

double QFFitResultsEvaluation::getFitValue(const QString& id) {
    return getFitValue(getHighlightedRecord(), getEvaluationResultID(), id);
}

double QFFitResultsEvaluation::getFitError(const QString& id)  {
    return getFitError(getHighlightedRecord(), getEvaluationResultID(), id);
}

void QFFitResultsEvaluation::setFitFix(const QString& id, bool fix) {
    setFitFix(getHighlightedRecord(), getEvaluationResultID(), id, fix);
}

void QFFitResultsEvaluation::setFitResultFix(const QString& id, bool fix) {
    setFitResultFix(getHighlightedRecord(), getEvaluationResultID(), id, fix);
}

bool QFFitResultsEvaluation::getFitFix(const QString& id) {
    return getFitFix(getHighlightedRecord(), getEvaluationResultID(), id);
}

void QFFitResultsEvaluation::fillParameters(double* param) {
    fillParameters(getHighlightedRecord(), getEvaluationResultID(), param);
}

void QFFitResultsEvaluation::fillParameterErrors(double* param) {
    fillParameterErrors(getHighlightedRecord(), getEvaluationResultID(), param);
}

void QFFitResultsEvaluation::fillFix(bool* param) {
    fillFix(getHighlightedRecord(), getEvaluationResultID(), param);
}


double* QFFitResultsEvaluation::allocFillParameters() {
    return allocFillParameters(getHighlightedRecord(), getEvaluationResultID());

}

double* QFFitResultsEvaluation::allocFillParameterErrors() {
    return allocFillParameterErrors(getHighlightedRecord(), getEvaluationResultID());

}

bool* QFFitResultsEvaluation::allocFillFix() {
    return allocFillFix(getHighlightedRecord(), getEvaluationResultID());
}















void QFFitResultsEvaluation::fillParameters(QFRawDataRecord* r, const QString& resultID, double* param) {
    QFFitFunction* f=getFitFunction();
    if (f!=NULL) {
        for (int i=0; i<f->paramCount(); i++) {
            QString id=f->getParameterID(i);
            param[i]=getFitValue(r, resultID, id);
        }
    }
}

void QFFitResultsEvaluation::fillParameterErrors(QFRawDataRecord* r, const QString& resultID, double* param) {
    QFFitFunction* f=getFitFunction();
    if (f!=NULL) {
        for (int i=0; i<f->paramCount(); i++) {
            QString id=f->getParameterID(i);
            param[i]=getFitError(r, resultID, id);
        }
    }
}


void QFFitResultsEvaluation::fillFix(QFRawDataRecord* r, const QString& resultID, bool* param) {
    QFFitFunction* f=getFitFunction();
    if (f!=NULL) {
        for (int i=0; i<f->paramCount(); i++) {
            QString id=f->getParameterID(i);
            param[i]=getFitFix(r, resultID, id);
        }
    }
}


double* QFFitResultsEvaluation::allocFillParameters(QFRawDataRecord* r, const QString& resultID) {
    QFFitFunction* f=getFitFunction();
    if (f!=NULL) {
        double* res=(double*)calloc(f->paramCount(), sizeof(double));
        fillParameters(r, resultID, res);
        return res;
    }
    return NULL;
}

double* QFFitResultsEvaluation::allocFillParameterErrors(QFRawDataRecord* r, const QString& resultID) {
    QFFitFunction* f=getFitFunction();
    if (f!=NULL) {
        double* res=(double*)calloc(f->paramCount(), sizeof(double));
        fillParameterErrors(r, resultID, res);
        return res;
    }
    return NULL;
}




bool* QFFitResultsEvaluation::allocFillFix(QFRawDataRecord* r, const QString& resultID) {
    QFFitFunction* f=getFitFunction();
    if (f!=NULL) {
        bool* res=(bool*)calloc(f->paramCount(), sizeof(bool));
        fillFix(r, resultID, res);
        return res;
    }
    return NULL;
}


double* QFFitResultsEvaluation::allocFillParametersMin() {
    QFFitFunction* f=getFitFunction();
    if (f!=NULL) {
        double* res=(double*)calloc(f->paramCount(), sizeof(double));
        fillParametersMin(res);
        return res;
    }
    return NULL;
}

double* QFFitResultsEvaluation::allocFillParametersMax() {
    QFFitFunction* f=getFitFunction();
    if (f!=NULL) {
        double* res=(double*)calloc(f->paramCount(), sizeof(double));
        fillParametersMax(res);
        return res;
    }
    return NULL;
}

void QFFitResultsEvaluation::fillParametersMin(double* param) {
    QFFitFunction* f=getFitFunction();
    if (f!=NULL) {
        for (int i=0; i<f->paramCount(); i++) {
            QString id=f->getParameterID(i);
            param[i]=getFitMin(id);
        }
    }
}

void QFFitResultsEvaluation::fillParametersMax(double* param) {
    QFFitFunction* f=getFitFunction();
    if (f!=NULL) {
        for (int i=0; i<f->paramCount(); i++) {
            QString id=f->getParameterID(i);
            param[i]=getFitMax(id);
        }
    }
}

double QFFitResultsEvaluation::getDefaultFitValue(const QString& id) {
    QFFitFunction* f=getFitFunction();
    if (f==NULL) {
        //std::cout<<"getFitValue("<<id.toStdString()<<") = "<<0<<" [getFitFunction()==NULL]\n";
        return 0;
    }
    if (!f->hasParameter(id)) return 0;
    int pid=f->getParameterNum(id);
    double res=0;
    if (pid>-1) res=f->getDescription(pid).initialValue;
    res=fitParamGlobalSettings->value(QString(m_fitFunction+"/"+id), res).toDouble();
    res=fitParamSettings->value(QString(m_fitFunction+"/"+id), res).toDouble();
    QString psID=getParameterStoreID(id);
    if (parameterStore.contains(psID)) {
        if (parameterStore[psID].valueSet) {
            res=parameterStore[psID].value;
        }
    }
    return res;
}

bool QFFitResultsEvaluation::getDefaultFitFix(const QString& id) {
    QFFitFunction* f=getFitFunction();
    if (f==NULL) return 0;
    bool res=false;
    res=fitParamGlobalSettings->value(QString(m_fitFunction+"/"+id+"_fix"), res).toBool();
    res=fitParamSettings->value(QString(m_fitFunction+"/"+id+"_fix"), res).toBool();
    QString psID=getParameterStoreID(id);
    if (parameterStore.contains(psID)) {
        if (parameterStore[psID].fixSet) {
            res=parameterStore[psID].fix;
        }
    }
    return res;
}

void QFFitResultsEvaluation::resetDefaultFitValue(const QString& id) {
    if (hasFit()) {
        QFRawDataRecord* r=getHighlightedRecord();
        QString en=getEvaluationResultID();
        QString pid=getFitParamID(id);
        if (r->resultsExists(en, pid)) r->resultsRemove(en, pid);
    }
}

void QFFitResultsEvaluation::resetDefaultFitFix(const QString& id) {
    if (hasFit()) {
        QFRawDataRecord* r=getHighlightedRecord();
        QString en=getEvaluationResultID();
        QString pid=getFitParamFixID(id);
        if (r->resultsExists(en, pid)) r->resultsRemove(en, pid);
    }
}

void QFFitResultsEvaluation::resetAllFitValueCurrent() {
    QFFitFunction* f=getFitFunction();
    if (f==NULL) return ;
    if (hasFit()) {
        QFRawDataRecord* r=getHighlightedRecord();
        r->disableEmitResultsChanged();
        QString en=getEvaluationResultID();
        for (int i=0; i<f->paramCount(); i++) {
            QString id=f->getParameterID(i);
            QString pid=getFitParamID(id);
            if (r->resultsExists(en, pid)) r->resultsRemove(en, pid);
        }
        r->enableEmitResultsChanged();
    }
}

void QFFitResultsEvaluation::resetAllFitFixCurrent() {
    QFFitFunction* f=getFitFunction();
    if (f==NULL) return ;
    if (hasFit()) {
        QFRawDataRecord* r=getHighlightedRecord();
        r->disableEmitResultsChanged();
        QString en=getEvaluationResultID();
        for (int i=0; i<f->paramCount(); i++) {
            QString id=f->getParameterID(i);
            QString pid=getFitParamFixID(id);
            if (r->resultsExists(en, pid)) r->resultsRemove(en, pid);
        }
        r->enableEmitResultsChanged();
    }
}


void QFFitResultsEvaluation::resetAllFitResultsCurrent() {
    QFRawDataRecord* re=getHighlightedRecord();
    if (!re) return;
    re->resultsClear(getEvaluationResultID());
}
*/
