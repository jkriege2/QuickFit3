#include "qffitresultsbyindexasvectorevaluation.h"

QFFitResultsByIndexAsVectorEvaluation::QFFitResultsByIndexAsVectorEvaluation(const QString &fitFunctionPrefix, QFProject *parent):
    QFFitResultsByIndexEvaluation(fitFunctionPrefix, parent)
{
}

QFFitResultsByIndexAsVectorEvaluation::~QFFitResultsByIndexAsVectorEvaluation() {
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



void QFFitResultsByIndexAsVectorEvaluation::resetDefaultFitValue(const QString& id) {
    QFRawDataRecord* r=getHighlightedRecord();
    QString resultID=getEvaluationResultID();
    int index=getIndexFromEvaluationResultID(resultID);
    if (index<0) {
        QFFitResultsByIndexEvaluation::resetDefaultFitValue(id);
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

void QFFitResultsByIndexAsVectorEvaluation::resetDefaultFitFix(const QString& id) {
    QFRawDataRecord* r=getHighlightedRecord();
    QString resultID=getEvaluationResultID();
    int index=getIndexFromEvaluationResultID(resultID);
    if (index<0) {
        QFFitResultsByIndexEvaluation::resetDefaultFitFix(id);
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

void QFFitResultsByIndexAsVectorEvaluation::resetAllFitResultsCurrent() {
    resetAllFitResultsCurrent(getHighlightedRecord(), getEvaluationResultID());
}

void QFFitResultsByIndexAsVectorEvaluation::resetAllFitResults(QFRawDataRecord *r, const QString& resultID) {
    //QFRawDataRecord* r=getHighlightedRecord();
    //QString resultID=getEvaluationResultID();
    bool doEmit=r->isEmitResultsChangedEnabled();
    r->disableEmitResultsChanged();
    QString tresultID=transformResultID(resultID);
    int index=getIndexFromEvaluationResultID(resultID);
    if (index<0) {
        QFFitResultsByIndexEvaluation::resetAllFitResultsCurrent();
    } else {
        QList<QString> resids=r->resultsCalcNames(tresultID);
        for (int i=0; i<resids.size(); i++) {
            if (isParamNameLocalStore(resids[i])) {
                r->resultsSetInBooleanList(tresultID, resids[i], index, false);
            } else {
                r->resultsResetInList(tresultID, resids[i], index);
            }
        }

    }
    if (doEmit) r->enableEmitResultsChanged(true);
}

void QFFitResultsByIndexAsVectorEvaluation::resetAllFitResultsAllFiles() {
    QList<QPointer<QFRawDataRecord> > recs=getApplicableRecords();
    for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* r=recs[i];
        if (r) {
            resetAllFitResults(r, getEvaluationResultID());
        }
    }
}


bool QFFitResultsByIndexAsVectorEvaluation::hasFit(QFRawDataRecord* r1, const QString& evalID) {
    QString resultID=getEvaluationResultID();
    int index=getIndexFromEvaluationResultID(evalID);
    if (index<0) {
        return QFFitResultsByIndexEvaluation::hasFit(r1, evalID);
    } else {

        QFRawDataRecord* r=r1;
        if (getFitFunction()==NULL) return false;
        if (r==NULL) r=getHighlightedRecord();
        if (r==NULL) return false;
        if (!r->resultsExistsFromEvaluation(transformResultID(evalID))) {
            return false;
        } else {
            QList<QString> resids=r->resultsCalcNames(tresultID);
            bool anyfit=false;
            for (int i=0; i<resids.size(); i++) {
                if (isParamNameLocalStore(resids[i])) {
                    if (r->resultsGetInBooleanList(tresultID, resids[i], index, false)) return true;
                }
            }
        }
        return false;
    }
}


void QFFitResultsByIndexAsVectorEvaluation::setFitResultValue(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double value) {
    QFFitFunction* f=getFitFunction();
    QString unit="";
    if (f) {
        int pid=f->getParameterNum(parameterID);
        if (pid>-1) unit=f->getDescription(pid).unit;
    }
    setFitResultValue(r, resultID, parameterID, value, unit);
}

void QFFitResultsByIndexAsVectorEvaluation::setFitResultValue(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double value, QString unit) {
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

void QFFitResultsByIndexAsVectorEvaluation::setFitResultValue(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double value, double error) {
    int index=getIndexFromEvaluationResultID(resultID);
    QString tresultID=transformResultID(resultID);
    if (index<0) {
        QFFitResultsByIndexEvaluation::setFitResultValue(r, resultID, parameterID, value, error);
    } else if (r) {
        QFFitFunction* f=getFitFunction();
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

void QFFitResultsByIndexAsVectorEvaluation::setFitResultValue(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double value, double error, QString unit) {
    int index=getIndexFromEvaluationResultID(resultID);
    QString tresultID=transformResultID(resultID);
    if (index<0) {
        QFFitResultsByIndexEvaluation::setFitResultValue(r, resultID, parameterID, value, error);
    } else if (r) {
        QFFitFunction* f=getFitFunction();

        QString pid=getFitParamID(parameterID);
        bool doEmit=r->isEmitResultsChangedEnabled();
        r->disableEmitResultsChanged();
        r->resultsSetInBooleanList(tresultID, getParamNameLocalStore(pid), index, true);
        r->resultsSetInNumberErrorList(tresultID, pid, index, value, error, unit);
        if (doEmit) r->enableEmitResultsChanged(true);
        emitResultsChanged(r, resultID, getFitParamID(parameterID));
    }
}

void QFFitResultsByIndexAsVectorEvaluation::setFitResultError(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, double error) {
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

void QFFitResultsByIndexAsVectorEvaluation::setFitResultValueString(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, QString value) {
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

void QFFitResultsByIndexAsVectorEvaluation::setFitResultValueBool(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, bool value) {
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

void QFFitResultsByIndexAsVectorEvaluation::setFitResultValueInt(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, int64_t value) {
    QFFitFunction* f=getFitFunction();
    QString unit="";
    if (f) {
        int pid=f->getParameterNum(parameterID);
        if (pid>-1) unit=f->getDescription(pid).unit;
    }

    setFitResultValueInt(r, resultID, parameterID, value, unit);
}

void QFFitResultsByIndexAsVectorEvaluation::setFitResultValueInt(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, int64_t value, QString unit) {
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



void QFFitResultsByIndexAsVectorEvaluation::setFitResultFix(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, bool fix) {
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

void QFFitResultsByIndexAsVectorEvaluation::setFitResultGroup(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, const QString& group) {
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

void QFFitResultsByIndexAsVectorEvaluation::setFitResultLabel(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, const QString& label, const QString& label_richtext) {
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


void QFFitResultsByIndexAsVectorEvaluation::setFitResultValues(QFRawDataRecord* r, const QString& resultID, double* values, double* errors) {
    if (r!=NULL) {
        int index=getIndexFromEvaluationResultID(resultID);
        if (index<0) {
            QFFitResultsByIndexEvaluation::setFitResultValues(r, resultID, values, errors);
        } else {
            QFFitFunction* f=getFitFunction();
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
                emitPropertiesChanged();
                emitResultsChanged();
            }
        }
    }
}

void QFFitResultsByIndexAsVectorEvaluation::setFitResultValuesVisible(QFRawDataRecord* r, const QString& resultID, double* values, double* errors) {
    if (r!=NULL) {
        int index=getIndexFromEvaluationResultID(resultID);
        if (index<0) {
            QFFitResultsByIndexEvaluation::setFitResultValuesVisible(r, resultID, values, errors);
        } else {
            QFFitFunction* f=getFitFunction();
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
                emitPropertiesChanged();
                emitResultsChanged();
            }
        }
    }
}

void QFFitResultsByIndexAsVectorEvaluation::setFitResultValuesVisibleWithGroupAndLabel(QFRawDataRecord* r, const QString& resultID, double* values, double* errors, const QString& group, bool* fix, const QString& fixGroup, bool sortPriority) {
    if (r!=NULL) {
        int index=getIndexFromEvaluationResultID(resultID);
        if (index<0) {
            QFFitResultsByIndexEvaluation::setFitResultValuesVisibleWithGroupAndLabel(r, resultID, values, errors, group, fix, fixGroup, sortPriority);
        } else {
            QFFitFunction* f=getFitFunction();
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
                        r->resultsSetLabel(tresultID, fpid, f->getDescription(pid).name, f->getDescription(pid).label);
                        r->resultsSetSortPriority(tresultID, fpid, sortPriority);
                        if (fix) {
                            r->resultsSetInBooleanList(tresultID, ffid, index, fix[i]);
                            if (!fixGroup.isEmpty()) r->resultsSetGroup(tresultID, ffid, fixGroup);
                            r->resultsSetLabel(tresultID, ffid, f->getDescription(pid).name+tr(", fix"), f->getDescription(pid).label+tr(", fix"));
                            r->resultsSetSortPriority(tresultID,  ffid, sortPriority);
                        }
                    }
                }
                if (doEmit) r->enableEmitResultsChanged(true);
                doEmitResultsChanged=thisDoEmit;
                emitPropertiesChanged();
                emitResultsChanged();
            }
        }
    }
}


double QFFitResultsByIndexAsVectorEvaluation::getFitValue(QFRawDataRecord* r, const QString& resultID, const QString& parameterID) {
    double res=0;
    int index=getIndexFromEvaluationResultID(resultID);
    QString tresultID=transformResultID(resultID);
    if (index<0) {
        return QFFitResultsByIndexEvaluation::getFitValue(r, resultID, parameterID);
    } else if (r) {
        QFFitFunction* f=getFitFunction();
        if (f==NULL) {
            return 0;
        }
        if (!f->hasParameter(parameterID)) return 0;
        int pid=f->getParameterNum(parameterID);
        QString fpid=getFitParamID(parameterID);

        if (pid>-1) res=f->getDescription(pid).initialValue;
        res=fitParamGlobalSettings->value(QString(m_fitFunction+"/"+parameterID), res).toDouble();
        res=fitParamSettings->value(QString(m_fitFunction+"/"+parameterID), res).toDouble();
        QString psID=getParameterStoreID(parameterID);
        if (parameterStore.contains(psID)) {
            if (parameterStore[psID].valueSet) {
                res=parameterStore[psID].value;
            }
        }

        double sval=res, serr=0;
        if (hasSpecial(r, resultID, parameterID, sval, serr)) {
            res=sval;
        } else if (hasFit(r, resultID)) {
            if (r->resultsExists(tresultID, pid)) {
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

double QFFitResultsByIndexAsVectorEvaluation::getFitError(QFRawDataRecord* r, const QString& resultID, const QString& parameterID)  {
    double res=0;
    int index=getIndexFromEvaluationResultID(resultID);
    QString tresultID=transformResultID(resultID);
    if (index<0) {
        return QFFitResultsByIndexEvaluation::getFitError(r, resultID, parameterID);
    } else if (r) {
        QFFitFunction* f=getFitFunction();
        if (f==NULL) {
            return 0;
        }
        if (!f->hasParameter(parameterID)) return 0;
        int pid=f->getParameterNum(parameterID);
        QString fpid=getFitParamID(parameterID);

        if (pid>-1) res=f->getDescription(pid).initialValue;
        res=fitParamGlobalSettings->value(QString(m_fitFunction+"/"+parameterID), res).toDouble();
        res=fitParamSettings->value(QString(m_fitFunction+"/"+parameterID), res).toDouble();
        QString psID=getParameterStoreID(parameterID);
        if (parameterStore.contains(psID)) {
            if (parameterStore[psID].errorSet) {
                res=parameterStore[psID].error;
            }
        }

        double sval=0, serr=res;
        if (hasSpecial(r, resultID, parameterID, sval, serr)) {
            res=serr;
        } else if (hasFit(r, resultID)) {
            if (r->resultsExists(tresultID, pid)) {
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



bool QFFitResultsByIndexAsVectorEvaluation::getFitFix(QFRawDataRecord* r, const QString& resultID, const QString& parameterID) {
    bool res=false;
    int index=getIndexFromEvaluationResultID(resultID);
    QString tresultID=transformResultID(resultID);
    if (index<0) {
        return QFFitResultsByIndexEvaluation::getFitFix(r, resultID, parameterID);
    } else if (r) {
        QFFitFunction* f=getFitFunction();
        if (f==NULL) {
            return false;
        }
        if (!f->hasParameter(parameterID)) return false;
        int pid=f->getParameterNum(parameterID);
        QString fpid=getFitParamFixID(parameterID);

        res=fitParamGlobalSettings->value(QString(m_fitFunction+"/"+parameterID+"_fix"), res).toBool();
        res=fitParamSettings->value(QString(m_fitFunction+"/"+parameterID+"_fix"), res).toBool();
        QString psID=getParameterStoreID(parameterID);
        if (parameterStore.contains(psID)) {
            if (parameterStore[psID].fixSet) {
                res=parameterStore[psID].fix;
            }
        }

        if (hasFit(r, resultID)) {
            if (r->resultsExists(tresultID, pid)) {
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




