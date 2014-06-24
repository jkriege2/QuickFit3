#include "qfusesresultsbyindexevaluationeditor.h"
#include "qfrawdatarecord.h"
#include "qfevaluationitem.h"
#include "qfselectrdrdialog.h"
#include "qfoverlayplotdialog.h"
#include <QtGui>
#include <QtCore>

QFUsesResultsByIndexEvaluationEditor::QFUsesResultsByIndexEvaluationEditor(QFPluginServices *services, QFEvaluationPropertyEditor *propEditor, QWidget *parent, const QString& iniPrefix) :
    QFEvaluationEditor(services, propEditor, parent)
{
    m_iniPrefix=iniPrefix;


    actOverlayPlot=new QAction(tr("&Overlay plot"), this);
    connect(actOverlayPlot, SIGNAL(triggered()), this, SLOT(createOverlayPlot()));

}

void QFUsesResultsByIndexEvaluationEditor::readSettings()
{
}

void QFUsesResultsByIndexEvaluationEditor::writeSettings()
{
}


void QFUsesResultsByIndexEvaluationEditor::getPlotData(QList<QFGetPlotdataInterface::GetPlotDataItem> &data, int option)
{
    QFUsesResultsByIndexEvaluation* fcs=qobject_cast<QFUsesResultsByIndexEvaluation*>(current);
    if (fcs) getPlotData(fcs->getHighlightedRecord(), fcs->getCurrentIndex(), data, option);
}


void QFUsesResultsByIndexEvaluationEditor::getPlotData(int index, QList<QFGetPlotdataInterface::GetPlotDataItem> &data, int option)
{
    QFUsesResultsByIndexEvaluation* fcs=qobject_cast<QFUsesResultsByIndexEvaluation*>(current);
    if (fcs) getPlotData(fcs->getHighlightedRecord(), index, data, option);
}

void QFUsesResultsByIndexEvaluationEditor::getPlotData(QFRawDataRecord *rec, QList<QFGetPlotdataInterface::GetPlotDataItem> &data, int option)
{
    QFUsesResultsByIndexEvaluation* fcs=qobject_cast<QFUsesResultsByIndexEvaluation*>(current);
    if (fcs) getPlotData(rec, fcs->getCurrentIndex(), data, option);

}


void QFUsesResultsByIndexEvaluationEditor::getPlotData(QFRawDataRecord *rec, int index, QList<QFGetPlotdataInterface::GetPlotDataItem> &data, int option)
{

}

bool QFUsesResultsByIndexEvaluationEditor::getPlotDataSpecs(QStringList *optionNames, QList<QFGetPlotdataInterface::GetPlotPlotOptions> *listPlotOptions)
{
    return false;
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

void QFUsesResultsByIndexEvaluationEditor::connectDefaultWidgets(QFEvaluationItem *current, QFEvaluationItem *old)
{
    actOverlayPlot->setEnabled(this->getPlotDataSpecs());
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

void QFUsesResultsByIndexEvaluationEditor::createOverlayPlot()
{
    QFMatchRDRFunctorSelectApplicable* functor=new QFMatchRDRFunctorSelectApplicable(current);
    QFSelectRDRDialog* dlg=new QFSelectRDRDialog(functor, true, this);
    dlg->setAllowCreateNew(false);
    dlg->setAllowMultiSelect(true);
    QComboBox* cmbOptions=new QComboBox(dlg);
    QStringList optionNames;
    QList<QFGetPlotdataInterface::GetPlotPlotOptions> options;
    getPlotDataSpecs(&optionNames, &options);
    if (optionNames.size()>0) {
        cmbOptions->addItems(optionNames);
    } else {
        cmbOptions->addItem(tr("all data"));
    }
    dlg->addWidget(tr("which plots:"), cmbOptions);
    if (dlg->exec()) {
        QList<QPointer<QFRawDataRecord> > rdr=dlg->getSelectedRDRs();
        QList<QFGetPlotdataInterface::GetPlotDataItem> data;
        for (int i=0; i<rdr.size(); i++) {
            if (rdr[i]) {
                getPlotData(rdr[i], data, cmbOptions->currentIndex());
            }
        }
        if (data.size()>0) {
            //qDebug()<<"plot data"<<data.size();
            QFOverlayPlotDialog* dlgOvl=new QFOverlayPlotDialog(this);
            dlgOvl->startAddingPlots();
            dlgOvl->setPlotOptions(options.value(cmbOptions->currentIndex(), QFGetPlotdataInterface::GetPlotPlotOptions()));
            for (int i=0; i<data.size(); i++) {
                dlgOvl->addPlot(data[i]);
            }
            dlgOvl->endAddingPlots();
            dlgOvl->show();

        } else {
            QMessageBox::information(this, tr("Overlay Plot"), tr("No plot data available!"));
        }
        delete dlg;
    }
}

int QFUsesResultsByIndexEvaluationEditor::getUserMinAbsMax(QFRawDataRecord *rec, int index) {
    return 0;
}
