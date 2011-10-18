#include "qffitresultsbyindexevaluationeditorbase.h"

QFFitResultsByIndexEvaluationEditorBase::QFFitResultsByIndexEvaluationEditorBase(QString iniPrefix, QFPluginServices* services, QWidget *parent) :
    QFFitResultsEvaluationEditorBase(iniPrefix, services, parent)
{
}














/////////////////////////////////////////////////////////////////////
// SETTING PROPERTIES IN OTHER FILES&RUNS&FILES^RUNS
/////////////////////////////////////////////////////////////////////

void QFFitResultsByIndexEvaluationEditorBase::copyUserMinToAll(int userMin) {
    if (!current) return;
    QFFitResultsByIndexEvaluation* data=qobject_cast<QFFitResultsByIndexEvaluation*>(current);
    if (!data) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QList<QFRawDataRecord*> recs=current->getProject()->getRawDataList();

    for (int i=0; i<recs.size(); i++) {
        if (current->isApplicable(recs[i])) {
            QFRawDataRecord* rdr=recs[i];
            rdr->disableEmitPropertiesChanged();
            for (int r=data->getIndexMin(rdr); r<=data->getIndexMax(rdr); r++) {
                const QString resultID=data->getEvaluationResultID(r);
                if (!((rdr==current->getHighlightedRecord())&&(r==data->getCurrentIndex()))) {
                    rdr->setQFProperty(resultID+"_datacut_min", userMin, false, false);
                }
            }
            rdr->enableEmitPropertiesChanged(true);
        }
    }
    QApplication::restoreOverrideCursor();
}

void QFFitResultsByIndexEvaluationEditorBase::copyUserMaxToAll(int userMax) {
    if (!current) return;
    QFFitResultsByIndexEvaluation* data=qobject_cast<QFFitResultsByIndexEvaluation*>(current);
    if (!data) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QList<QFRawDataRecord*> recs=current->getProject()->getRawDataList();

    for (int i=0; i<recs.size(); i++) {
        if (current->isApplicable(recs[i])) {
            QFRawDataRecord* rdr=recs[i];
            rdr->disableEmitPropertiesChanged();
            for (int r=data->getIndexMin(rdr); r<=data->getIndexMax(rdr); r++) {
                const QString resultID=data->getEvaluationResultID(r);
                if (!((rdr==current->getHighlightedRecord())&&(r==data->getCurrentIndex()))) {
                    rdr->setQFProperty(resultID+"_datacut_max", userMax, false, false);
                }
            }
            rdr->enableEmitPropertiesChanged(true);
        }
    }
    QApplication::restoreOverrideCursor();
}


void QFFitResultsByIndexEvaluationEditorBase::copyUserMinMaxToAll(int userMin, int userMax) {
    if (!current) return;
    QFFitResultsByIndexEvaluation* data=qobject_cast<QFFitResultsByIndexEvaluation*>(current);
    if (!data) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QList<QFRawDataRecord*> recs=current->getProject()->getRawDataList();

    for (int i=0; i<recs.size(); i++) {
        if (current->isApplicable(recs[i])) {
            QFRawDataRecord* rdr=recs[i];
            rdr->disableEmitPropertiesChanged();
            for (int r=data->getIndexMin(rdr); r<=data->getIndexMax(rdr); r++) {
                const QString resultID=data->getEvaluationResultID(r);
                if (!((rdr==current->getHighlightedRecord())&&(r==data->getCurrentIndex()))) {
                    rdr->setQFProperty(resultID+"_datacut_min", userMin, false, false);
                    rdr->setQFProperty(resultID+"_datacut_max", userMax, false, false);
                }
            }
            rdr->enableEmitPropertiesChanged(true);
        }
    }
    QApplication::restoreOverrideCursor();
}

void QFFitResultsByIndexEvaluationEditorBase::copyUserMinToAllRuns(int userMin) {
    if (!current) return;
    QFFitResultsByIndexEvaluation* data=qobject_cast<QFFitResultsByIndexEvaluation*>(current);
    if (!data) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFRawDataRecord* rdr=data->getHighlightedRecord();
    rdr->disableEmitPropertiesChanged();
    for (int r=data->getIndexMin(rdr); r<=data->getIndexMax(rdr); r++) {
        const QString resultID=data->getEvaluationResultID(r);
        if (!((rdr==current->getHighlightedRecord())&&(r==data->getCurrentIndex()))) {
            rdr->setQFProperty(resultID+"_datacut_min", userMin, false, false);
        }
    }
    rdr->enableEmitPropertiesChanged(true);
    QApplication::restoreOverrideCursor();
}



void QFFitResultsByIndexEvaluationEditorBase::copyUserMaxToAllRuns(int userMax) {
    if (!current) return;
    QFFitResultsByIndexEvaluation* data=qobject_cast<QFFitResultsByIndexEvaluation*>(current);
    if (!data) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFRawDataRecord* rdr=data->getHighlightedRecord();
    rdr->disableEmitPropertiesChanged();
    for (int r=data->getIndexMin(rdr); r<=data->getIndexMax(rdr); r++) {
        const QString resultID=data->getEvaluationResultID(r);
        if (!((rdr==current->getHighlightedRecord())&&(r==data->getCurrentIndex()))) {
            rdr->setQFProperty(resultID+"_datacut_max", userMax, false, false);
        }
    }
    rdr->enableEmitPropertiesChanged(true);
    QApplication::restoreOverrideCursor();
}


void QFFitResultsByIndexEvaluationEditorBase::copyUserMinMaxToAllRuns(int userMin, int userMax) {
    if (!current) return;
    QFFitResultsByIndexEvaluation* data=qobject_cast<QFFitResultsByIndexEvaluation*>(current);
    if (!data) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFRawDataRecord* rdr=data->getHighlightedRecord();
    rdr->disableEmitPropertiesChanged();
    for (int r=data->getIndexMin(rdr); r<=data->getIndexMax(rdr); r++) {
        const QString resultID=data->getEvaluationResultID(r);
        if (!((rdr==current->getHighlightedRecord())&&(r==data->getCurrentIndex()))) {
            rdr->setQFProperty(resultID+"_datacut_min", userMin, false, false);
            rdr->setQFProperty(resultID+"_datacut_max", userMax, false, false);
        }
    }
    rdr->enableEmitPropertiesChanged(true);
    QApplication::restoreOverrideCursor();
}




int QFFitResultsByIndexEvaluationEditorBase::getUserMin(QFRawDataRecord* rec, int index, int defaultMin) {
    QFFitResultsByIndexEvaluation* data=qobject_cast<QFFitResultsByIndexEvaluation*>(current);
    if (!data) return defaultMin;
    const QString resultID=data->getEvaluationResultID(index);

    // WORKROUND FOR OLD PROPERTY NAMES
    int defaultM=rec->getProperty(QString(resultID+"_datacut_min").replace(QString("_")+data->getFitFunction()->id()+QString("_run"), "_r").replace(data->getType()+"_", data->getType()), defaultMin).toInt();

    return rec->getProperty(resultID+"_datacut_min", defaultM).toInt();
}

int QFFitResultsByIndexEvaluationEditorBase::getUserMax(QFRawDataRecord* rec, int index, int defaultMax) {
    QFFitResultsByIndexEvaluation* data=qobject_cast<QFFitResultsByIndexEvaluation*>(current);
    if (!data) return defaultMax;
    const QString resultID=data->getEvaluationResultID(index);

    // WORKROUND FOR OLD PROPERTY NAMES
    int defaultM=rec->getProperty(QString(resultID+"_datacut_max").replace(QString("_")+data->getFitFunction()->id()+QString("_run"), "_r").replace(data->getType()+"_", data->getType()), defaultMax).toInt();

    return rec->getProperty(resultID+"_datacut_max", defaultM).toInt();
    //return rec->getProperty(resultID+"_datacut_max", defaultMax).toInt();
}

int QFFitResultsByIndexEvaluationEditorBase::getUserMin(int defaultMin) {
    QFFitResultsByIndexEvaluation* data=qobject_cast<QFFitResultsByIndexEvaluation*>(current);
    if (!data) return defaultMin;
    QFRawDataRecord* rdr=data->getHighlightedRecord();
    const QString resultID=data->getEvaluationResultID();

    // WORKROUND FOR OLD PROPERTY NAMES
    int defaultM=rdr->getProperty(QString(resultID+"_datacut_min").replace(QString("_")+data->getFitFunction()->id()+QString("_run"), "_r").replace(data->getType()+"_", data->getType()), defaultMin).toInt();

    qDebug()<<"get: "<<QString(resultID+"_datacut_min")<<"   "<<QString(resultID+"_datacut_min").replace(QString("_")+data->getFitFunction()->id()+QString("_run"), "_r").replace(data->getType()+"_", data->getType())<<"\n";

    return rdr->getProperty(resultID+"_datacut_min", defaultM).toInt();
    //return rdr->getProperty(resultID+"_datacut_min", defaultMin).toInt();
}

int QFFitResultsByIndexEvaluationEditorBase::getUserMax(int defaultMax) {
    QFFitResultsByIndexEvaluation* data=qobject_cast<QFFitResultsByIndexEvaluation*>(current);
    if (!data) return defaultMax;
    QFRawDataRecord* rdr=data->getHighlightedRecord();
    const QString resultID=data->getEvaluationResultID();



    // WORKROUND FOR OLD PROPERTY NAMES
    int defaultM=rdr->getProperty(QString(resultID+"_datacut_max").replace(QString("_")+data->getFitFunction()->id()+QString("_run"), "_r").replace(data->getType()+"_", data->getType()), defaultMax).toInt();

    qDebug()<<"get: "<<QString(resultID+"_datacut_max")<<"   "<<QString(resultID+"_datacut_max").replace(QString("_")+data->getFitFunction()->id()+QString("_run"), "_r").replace(data->getType()+"_", data->getType())<<"\n";

    return rdr->getProperty(resultID+"_datacut_max", defaultM).toInt();
    //return rdr->getProperty(resultID+"_datacut_max", defaultMax).toInt();
}

void QFFitResultsByIndexEvaluationEditorBase::setUserMin(int userMin) {
    QFFitResultsByIndexEvaluation* data=qobject_cast<QFFitResultsByIndexEvaluation*>(current);
    if (!data) return;
    QFRawDataRecord* rdr=data->getHighlightedRecord();
    const QString resultID=data->getEvaluationResultID();
    rdr->setQFProperty(resultID+"_datacut_min", userMin, false, false);
}

void QFFitResultsByIndexEvaluationEditorBase::setUserMax(int userMax) {
    if (!current) return;
    QFFitResultsByIndexEvaluation* data=qobject_cast<QFFitResultsByIndexEvaluation*>(current);
    if (!data) return;
    QFRawDataRecord* rdr=data->getHighlightedRecord();
    const QString resultID=data->getEvaluationResultID();
    rdr->setQFProperty(resultID+"_datacut_max", userMax, false, false);
}

void QFFitResultsByIndexEvaluationEditorBase::setUserMinMax(int userMin, int userMax) {
    if (!current) return;
    QFFitResultsByIndexEvaluation* data=qobject_cast<QFFitResultsByIndexEvaluation*>(current);
    if (!data) return;
    QFRawDataRecord* rdr=data->getHighlightedRecord();
    const QString resultID=data->getEvaluationResultID();
    rdr->disableEmitPropertiesChanged();
    rdr->setQFProperty(resultID+"_datacut_min", userMin, false, false);
    rdr->setQFProperty(resultID+"_datacut_max", userMax, false, false);
    rdr->enableEmitPropertiesChanged(true);
}




