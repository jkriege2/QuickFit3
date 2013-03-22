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

    ui->cmbRDR->setCurrentRDRID(QFPluginServices::getInstance()->getCurrentProject()->getProperty("QFPlotterCopyToTableDialog/rdr", 0).toInt());
    ui->cmbGraph->setCurrentIndex(QFPluginServices::getInstance()->getCurrentProject()->getProperty("QFPlotterCopyToTableDialog/graph", 0).toInt());
    ui->chCreateNewGraph->setChecked(QFPluginServices::getInstance()->getCurrentProject()->getProperty("QFPlotterCopyToTableDialog/newgraph", true).toBool());
    ui->chkNewTable->setChecked(QFPluginServices::getInstance()->getCurrentProject()->getProperty("QFPlotterCopyToTableDialog/newtable", false).toBool());
}

QFPlotterCopyToTableDialog::~QFPlotterCopyToTableDialog()
{
    QFPluginServices::getInstance()->getCurrentProject()->setQFProperty("QFPlotterCopyToTableDialog/rdr", ui->cmbRDR->currentRDRID());
    QFPluginServices::getInstance()->getCurrentProject()->setQFProperty("QFPlotterCopyToTableDialog/graph", ui->cmbGraph->currentIndex());
    QFPluginServices::getInstance()->getCurrentProject()->setQFProperty("QFPlotterCopyToTableDialog/newgraph", ui->chCreateNewGraph->isChecked());
    QFPluginServices::getInstance()->getCurrentProject()->setQFProperty("QFPlotterCopyToTableDialog/newtable", ui->chkNewTable->isChecked());
    delete ui;
}

QFRDRTableInterface *QFPlotterCopyToTableDialog::getTable() const
{
    return ui->cmbRDR->currentTable();
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

void QFPlotterCopyToTableDialog::on_cmbRDR_refilled(bool full)
{
    ui->chkNewTable->setEnabled(true);
    if (!full) {
        ui->chkNewTable->setChecked(true);
        //ui->chkNewTable->setEnabled(false);
    }
}

void QFPlotterCopyToTableDialog::on_cmbGraph_refilled(bool full)
{
    ui->chCreateNewGraph->setEnabled(true);
    if (!full) {
        ui->chCreateNewGraph->setChecked(true);
        //ui->chCreateNewGraph->setEnabled(false);
    }}
