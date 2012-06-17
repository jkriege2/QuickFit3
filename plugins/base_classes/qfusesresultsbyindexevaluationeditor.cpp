#include "qfusesresultsbyindexevaluationeditor.h"
#include "qfrawdatarecord.h"
#include "qfevaluationitem.h"

#include <QtGui>
#include <QtCore>

QFUsesResultsByIndexEvaluationEditor::QFUsesResultsByIndexEvaluationEditor(QFPluginServices *services, QFEvaluationPropertyEditor *propEditor, QWidget *parent) :
    QFEvaluationEditor(services, propEditor, parent)
{
}

void QFUsesResultsByIndexEvaluationEditor::readSettings()
{
}

void QFUsesResultsByIndexEvaluationEditor::writeSettings()
{
}


int QFUsesResultsByIndexEvaluationEditor::getUserMin(QFRawDataRecord* rec, int index, int defaultMin) {
    QFUsesResultsByIndexEvaluation* data=qobject_cast<QFUsesResultsByIndexEvaluation*>(current);
    if (!data) return defaultMin;
    const QString resultID=data->getEvaluationResultID(index);
    if (!rec) return defaultMin;


    return rec->getProperty(resultID+"_datacut_min", defaultMin).toInt();
}

int QFUsesResultsByIndexEvaluationEditor::getUserMax(QFRawDataRecord* rec, int index, int defaultMax) {
    QFUsesResultsByIndexEvaluation* data=qobject_cast<QFUsesResultsByIndexEvaluation*>(current);
    if (!data) return defaultMax;
    const QString resultID=data->getEvaluationResultID(index);

    if (!rec) return defaultMax;

    return rec->getProperty(resultID+"_datacut_max", defaultMax).toInt();
}


int QFUsesResultsByIndexEvaluationEditor::getUserMin(QFRawDataRecord* rec) {
    QFUsesResultsByIndexEvaluation* data=qobject_cast<QFUsesResultsByIndexEvaluation*>(current);
    if (!data) return 0;
    return getUserMin(rec, data->getCurrentIndex());
}

int QFUsesResultsByIndexEvaluationEditor::getUserMax(QFRawDataRecord* rec) {
    QFUsesResultsByIndexEvaluation* data=qobject_cast<QFUsesResultsByIndexEvaluation*>(current);
    if (!data) return 0;
    return getUserMax(rec, data->getCurrentIndex());
}


int QFUsesResultsByIndexEvaluationEditor::getUserMin(QFRawDataRecord* rec, int index) {
    QFUsesResultsByIndexEvaluation* data=qobject_cast<QFUsesResultsByIndexEvaluation*>(current);
    if (!data) return 0;
    return getUserMin(rec, index, 0);
}

int QFUsesResultsByIndexEvaluationEditor::getUserMax(QFRawDataRecord* rec, int index) {
    QFUsesResultsByIndexEvaluation* data=qobject_cast<QFUsesResultsByIndexEvaluation*>(current);
    if (!data) return 0;
    //QFRDRFCSDataInterface* dintf=qobject_cast<QFRDRFCSDataInterface*>(current->getHighlightedRecord());
    //if (dintf) return getUserMax(rec, index, dintf->getCorrelationN()-1);
    //return getUserMax(rec, index, 0);
    return getUserMax(rec, index, getUserMinAbsMax(rec, index));
}

void QFUsesResultsByIndexEvaluationEditor::setUserMin(int userMin) {
    QFUsesResultsByIndexEvaluation* data=qobject_cast<QFUsesResultsByIndexEvaluation*>(current);
    if (!data) return;
    QFRawDataRecord* rdr=current->getHighlightedRecord();
    const QString resultID=data->getEvaluationResultID(data->getCurrentIndex());
    rdr->setQFProperty(resultID+"_datacut_min", userMin, false, false);
}

void QFUsesResultsByIndexEvaluationEditor::setUserMax(int userMax) {
    if (!current) return;
    QFUsesResultsByIndexEvaluation* data=qobject_cast<QFUsesResultsByIndexEvaluation*>(current);
    if (!data) return;
    QFRawDataRecord* rdr=current->getHighlightedRecord();
    const QString resultID=data->getEvaluationResultID(data->getCurrentIndex());
    rdr->setQFProperty(resultID+"_datacut_max", userMax, false, false);
}

void QFUsesResultsByIndexEvaluationEditor::setUserMinMax(int userMin, int userMax) {
    if (!current) return;
    QFUsesResultsByIndexEvaluation* data=qobject_cast<QFUsesResultsByIndexEvaluation*>(current);
    if (!data) return;
    QFRawDataRecord* rdr=current->getHighlightedRecord();
    const QString resultID=data->getEvaluationResultID(data->getCurrentIndex());
    rdr->disableEmitPropertiesChanged();
    rdr->setQFProperty(resultID+"_datacut_min", userMin, false, false);
    rdr->setQFProperty(resultID+"_datacut_max", userMax, false, false);
    rdr->enableEmitPropertiesChanged(true);
}





void QFUsesResultsByIndexEvaluationEditor::copyUserMinToAll(int userMin) {
    if (!current) return;
    QFUsesResultsByIndexEvaluation* data=qobject_cast<QFUsesResultsByIndexEvaluation*>(current);
    if (!data) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QList<QPointer<QFRawDataRecord> > recs=current->getApplicableRecords();

    for (int i=0; i<recs.size(); i++) {
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
    QApplication::restoreOverrideCursor();
}

void QFUsesResultsByIndexEvaluationEditor::copyUserMaxToAll(int userMax) {
    if (!current) return;
    QFUsesResultsByIndexEvaluation* data=qobject_cast<QFUsesResultsByIndexEvaluation*>(current);
    if (!data) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QList<QPointer<QFRawDataRecord> > recs=current->getApplicableRecords();

    for (int i=0; i<recs.size(); i++) {
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
    QApplication::restoreOverrideCursor();
}


void QFUsesResultsByIndexEvaluationEditor::copyUserMinMaxToAll(int userMin, int userMax) {
    if (!current) return;
    QFUsesResultsByIndexEvaluation* data=qobject_cast<QFUsesResultsByIndexEvaluation*>(current);
    if (!data) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QList<QPointer<QFRawDataRecord> > recs=current->getApplicableRecords();

    for (int i=0; i<recs.size(); i++) {
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
    QApplication::restoreOverrideCursor();
}

void QFUsesResultsByIndexEvaluationEditor::copyUserMinToAllRuns(int userMin) {
    if (!current) return;
    QFUsesResultsByIndexEvaluation* data=qobject_cast<QFUsesResultsByIndexEvaluation*>(current);
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



void QFUsesResultsByIndexEvaluationEditor::copyUserMaxToAllRuns(int userMax) {
    if (!current) return;
    QFUsesResultsByIndexEvaluation* data=qobject_cast<QFUsesResultsByIndexEvaluation*>(current);
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


void QFUsesResultsByIndexEvaluationEditor::copyUserMinMaxToAllRuns(int userMin, int userMax) {
    if (!current) return;
    QFUsesResultsByIndexEvaluation* data=qobject_cast<QFUsesResultsByIndexEvaluation*>(current);
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

int QFUsesResultsByIndexEvaluationEditor::getUserMinAbsMax(QFRawDataRecord *rec, int index) {
    return 0;
}
