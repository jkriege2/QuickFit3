#include "qfplottercopytotabledialog.h"
#include "ui_qfplottercopytotabledialog.h"
#include "qfpluginservices.h"
#include "programoptions.h"

QFPlotterCopyToTableDialog::QFPlotterCopyToTableDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFPlotterCopyToTableDialog)
{
    ui->setupUi(this);
    ui->cmbRDR->setRequireColumnGraphics(true);
    ui->cmbRDR->setRequireNotReadonly(true);
    ui->cmbRDR->init(QFPluginServices::getInstance()->getCurrentProject());
    ui->cmbGraph->setColumnGraphs(ui->cmbRDR->currentColumnGraphics());
    ui->edtNewGraphName->setText(tr("new graph"));
    ui->edtNewTableName->setText(tr("table from plotter"));

    int idx=QFPluginServices::getInstance()->getCurrentProject()->getProperty("QFPlotterCopyToTableDialog/rdr", 0).toInt();
    if (idx) ui->cmbRDR->setCurrentRDRID(idx);
    idx=QFPluginServices::getInstance()->getCurrentProject()->getProperty("QFPlotterCopyToTableDialog/graph", 0).toInt();
    if (idx>=0) ui->cmbGraph->setCurrentIndex(idx);
    //if (ui->cmbRDR->count()>0) ui->chkNewTable->setChecked(QFPluginServices::getInstance()->getCurrentProject()->getProperty("QFPlotterCopyToTableDialog/newtable", false).toBool());
    //if (ui->cmbGraph->count()>0) ui->chCreateNewGraph->setChecked(QFPluginServices::getInstance()->getCurrentProject()->getProperty("QFPlotterCopyToTableDialog/newgraph", true).toBool());
    ui->radAlsoAddGraphs->setChecked(QFPluginServices::getInstance()->getCurrentProject()->getProperty("QFPlotterCopyToTableDialog/alsoaddgraph", true).toBool());
    ui->chkShowTableEditor->setChecked(QFPluginServices::getInstance()->getCurrentProject()->getProperty("QFPlotterCopyToTableDialog/showEditor", false).toBool());
    ui->chkOnlyForPlots->setChecked(QFPluginServices::getInstance()->getCurrentProject()->getProperty("QFPlotterCopyToTableDialog/chkOnlyForPlots", false).toBool());
    ui->labDescription->setVisible(false);
}

QFPlotterCopyToTableDialog::~QFPlotterCopyToTableDialog()
{
    QFPluginServices::getInstance()->getCurrentProject()->setQFProperty("QFPlotterCopyToTableDialog/rdr", ui->cmbRDR->currentRDRID());
    QFPluginServices::getInstance()->getCurrentProject()->setQFProperty("QFPlotterCopyToTableDialog/graph", ui->cmbGraph->currentIndex());
    QFPluginServices::getInstance()->getCurrentProject()->setQFProperty("QFPlotterCopyToTableDialog/alsoaddgraph", ui->radAlsoAddGraphs->isChecked());
    QFPluginServices::getInstance()->getCurrentProject()->setQFProperty("QFPlotterCopyToTableDialog/newgraph", ui->chCreateNewGraph->isChecked());
    QFPluginServices::getInstance()->getCurrentProject()->setQFProperty("QFPlotterCopyToTableDialog/newtable", ui->chkNewTable->isChecked());
    QFPluginServices::getInstance()->getCurrentProject()->setQFProperty("QFPlotterCopyToTableDialog/showEditor", ui->chkShowTableEditor->isChecked());
    QFPluginServices::getInstance()->getCurrentProject()->setQFProperty("QFPlotterCopyToTableDialog/chkOnlyForPlots", ui->chkOnlyForPlots->isChecked());
    delete ui;
}

QFRDRTableInterface *QFPlotterCopyToTableDialog::getTable() const
{
    return ui->cmbRDR->currentTable();
}

QFRawDataRecord *QFPlotterCopyToTableDialog::getRDR() const
{
    return ui->cmbRDR->currentRDR();
}

QFRDRColumnGraphsInterface *QFPlotterCopyToTableDialog::getGraph() const
{
    return ui->cmbRDR->currentColumnGraphics();
}

int QFPlotterCopyToTableDialog::getGraphID() const
{
    return ui->cmbGraph->currentIndex();
}

bool QFPlotterCopyToTableDialog::getNewGraph(QString &name) const
{
    if (ui->chCreateNewGraph->isChecked()) {
        name=ui->edtNewGraphName->text();
        return true;
    }
    return false;
}

bool QFPlotterCopyToTableDialog::getNewTable(QString &name) const
{
    if (ui->chkNewTable->isChecked()) {
        name=ui->edtNewTableName->text();
        return true;
    }
    return false;
}

bool QFPlotterCopyToTableDialog::getAddGraphs() const
{
    return ui->radAlsoAddGraphs->isChecked();
}

bool QFPlotterCopyToTableDialog::getShowEditor() const
{
    return ui->chkShowTableEditor->isChecked();
}

void QFPlotterCopyToTableDialog::setDescription(const QString &text)
{
    ui->labDescription->setPlainText(text);
    ui->labDescription->setVisible(!text.isEmpty());
}

bool QFPlotterCopyToTableDialog::copyOnlyPlotData() const
{
    return ui->chkOnlyForPlots->isChecked();
}

QString QFPlotterCopyToTableDialog::getPrefix()
{
    return ui->edtPrefix->text();
}

void QFPlotterCopyToTableDialog::on_cmbRDR_refilled(bool full)
{
    //qDebug()<<"on_cmbRDR_refilled(full="<<full<<")";
    ui->chkNewTable->setEnabled(true);
    if (!full) {
        ui->chkNewTable->setChecked(true);
        ui->chkNewTable->setEnabled(false);
    } else {
        //qDebug()<<"   ui->cmbRDR->currentIndex()="<<ui->cmbRDR->currentIndex();
        if (ui->cmbRDR->currentIndex()<0) ui->cmbRDR->setCurrentIndex(0);
        ui->chkNewTable->setChecked(false);
    }
}

void QFPlotterCopyToTableDialog::on_chkNewTable_toggled(bool checked)
{
    //qDebug()<<"on_chkNewTable_toggled(checked="<<checked<<")";
    ui->cmbRDR->setEnabled(!checked);
    ui->edtNewTableName->setEnabled(checked);
    if (checked) {
        ui->chCreateNewGraph->setChecked(true);
        ui->chCreateNewGraph->setEnabled(false);
    } else {
        ui->chCreateNewGraph->setEnabled(true);
    }
}

void QFPlotterCopyToTableDialog::on_cmbGraph_refilled(bool full)
{
    //qDebug()<<"on_cmbGraph_refilled(full="<<full<<")";
    ui->chCreateNewGraph->setEnabled(true);

    if (!full) {
        ui->chCreateNewGraph->setChecked(true);
        ui->chCreateNewGraph->setEnabled(false);
    } else {
        //qDebug()<<"   ui->chCreateNewGraph->isChecked()="<<ui->chCreateNewGraph->isChecked();
        //qDebug()<<"   ui->cmbGraph->currentIndex()="<<ui->cmbGraph->currentIndex();
        //qDebug()<<"   ui->cmbGraph->currentColumnGraphics()="<<ui->cmbGraph->currentColumnGraphics();
        //qDebug()<<"   ui->cmbGraph->count()="<<ui->cmbGraph->count();
        if (!ui->cmbGraph->currentColumnGraphics()) ui->cmbGraph->setCurrentIndex(0);
        ui->chCreateNewGraph->setChecked(false);
    }
}

void QFPlotterCopyToTableDialog::on_chCreateNewGraph_toggled(bool checked)
{
    //qDebug()<<"on_chCreateNewGraph_toggled(checked="<<checked<<")";
    ui->cmbGraph->setEnabled(!checked);
    ui->edtNewGraphName->setEnabled(checked);
}
