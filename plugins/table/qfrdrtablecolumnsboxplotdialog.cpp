#include "qfrdrtablecolumnsboxplotdialog.h"
#include "ui_qfrdrtablecolumnsboxplotdialog.h"

QFRDRTableColumnsBoxplotDialog::QFRDRTableColumnsBoxplotDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFRDRTableColumnsBoxplotDialog)
{
    this->table=NULL;
    ui->setupUi(this);
    setWindowFlags(windowFlags()|Qt::WindowMinMaxButtonsHint);
    ui->lstColumns->setModel(&cols);
}

QFRDRTableColumnsBoxplotDialog::~QFRDRTableColumnsBoxplotDialog()
{
    delete ui;
}

void QFRDRTableColumnsBoxplotDialog::setTable(QFRDRTable *plugin)
{
    QFTablePluginModel* table=plugin->model();
    this->table=table;
    this->plugin=plugin;
    ui->lstColumns->setModel(NULL);
    cols.setStringList(table->getColumnTitles());
    cols.unCheckAll();
    ui->lstColumns->setModel(&cols);


    ui->cmbResults->clear();
    ui->cmbResults->addItem(tr("--- add columns ---"), -1);
    for (int i=0; i<table->columnCount(); i++) {
        ui->cmbResults->addItem(table->headerData(i, Qt::Horizontal).toString(), QVariant(i));
    }
    ui->cmbResults->setCurrentIndex(0);

    ui->cmbPlots->clear();
    ui->cmbPlots->addItem(tr("--- no ---"), -2);
    ui->cmbPlots->addItem(tr("--- add new graph ---"), -1);
    for (int i=0; i<plugin->colgraphGetPlotCount(); i++) {
        ui->cmbPlots->addItem(tr("add to: %1").arg(plugin->colgraphGetPlotTitle(i)), i);
    }

}

void QFRDRTableColumnsBoxplotDialog::setSelectedColumns(const QList<int> &cols)
{
    for (int i=0; i<cols.size(); i++) {
        this->cols.setChecked(cols[i]);
    }
}

int QFRDRTableColumnsBoxplotDialog::getResultStartColumn() const
{
    return ui->cmbResults->currentData().toInt();
}

int QFRDRTableColumnsBoxplotDialog::addToGraph() const
{
    return ui->cmbPlots->currentData().toInt();
}

bool QFRDRTableColumnsBoxplotDialog::createGraph() const
{
    return addToGraph()>-2;
}

bool QFRDRTableColumnsBoxplotDialog::addNewGraph() const
{
    return addToGraph()==-1;
}

QList<int> QFRDRTableColumnsBoxplotDialog::getDataColumns() const
{
    return cols.getChecked();
}

bool QFRDRTableColumnsBoxplotDialog::getPlotBoxplot() const
{
    return (ui->cmbPlots->currentIndex()>0) && (ui->cmbMode->currentIndex()==0);
}

bool QFRDRTableColumnsBoxplotDialog::getPlotSymbols() const
{
    return (ui->cmbPlots->currentIndex()>0) && (ui->cmbMode->currentIndex()==2);
}

bool QFRDRTableColumnsBoxplotDialog::getPlot2Symbols() const
{
    return (ui->cmbPlots->currentIndex()>0) && (ui->cmbMode->currentIndex()==1);
}

QString QFRDRTableColumnsBoxplotDialog::getDataColumnsExpression() const
{
    QList<int> data=getDataColumns();
    QString res;
    for (int i=0; i<data.size(); i++) {
        if (res.size()>0) res+=", ";
        res+=QString::number(data[i]+1);
    }
    return "["+res+"]";
}

QStringList QFRDRTableColumnsBoxplotDialog::getExpressions(QStringList& names, QString& addToPre) const
{
    QString pre=plugin->getParserPreScript();
    QString prename="boxplot_cols";
    int cnt=0;
    while (pre.contains(prename)) {
        cnt++;
        prename=QString("boxplot_cols%1").arg(cnt);
    }

    QStringList ex;
    QString c=getDataColumnsExpression();
    int coln=getDataColumns().size();

    addToPre=QString("%1 = %2; comment(\"list of columns used for boxplot No. %3\");\n").arg(prename).arg(c).arg(cnt+1);

    ex<<QString("1:length(%1)").arg(prename);
    names<<tr("index_cols(%1)").arg(prename);
    ex<<QString("%1").arg(prename);
    names<<tr("col_id(%1)").arg(prename);
    ex<<QString("for(c,%1,columntitle(c))").arg(prename);
    names<<tr("col_names(%1)").arg(prename);
    if (ui->cmbMode->currentIndex()==0) { // all
        ex<<QString("for(c,%1,min(column(c)))").arg(prename);
        names<<tr("min(cols(%1))").arg(prename);
        ex<<QString("for(c,%1,quantile(column(c), 0.25))").arg(prename);
        names<<tr("q25(cols(%1))").arg(prename);
        ex<<QString("for(c,%1,mean(column(c)))").arg(prename);
        names<<tr("mean(cols(%1))").arg(prename);
        ex<<QString("for(c,%1,median(column(c)))").arg(prename);
        names<<tr("median(cols(%1))").arg(prename);
        ex<<QString("for(c,%1,quantile(column(c), 0.75))").arg(prename);
        names<<tr("q75(cols(%1))").arg(prename);
        ex<<QString("for(c,%1,max(column(c)))").arg(prename);
        names<<tr("max(cols(%1))").arg(prename);
        ex<<QString("for(c,%1,std(column(c)))").arg(prename);
        names<<tr("std(cols(%1))").arg(prename);
    } else if (ui->cmbMode->currentIndex()==1) { // median, mean, std, nmad
        ex<<QString("for(c,%1,mean(column(c)))").arg(prename);
        names<<tr("mean(cols(%1))").arg(prename);
        ex<<QString("for(c,%1,median(column(c)))").arg(prename);
        names<<tr("median(cols(%1))").arg(prename);
        ex<<QString("for(c,%1,std(column(c)))").arg(prename);
        names<<tr("std(cols(%1))").arg(prename);
        ex<<QString("for(c,%1,nmad(column(c)))").arg(prename);
        names<<tr("nmad(cols(%1))").arg(prename);
    } else if (ui->cmbMode->currentIndex()==2) { //  mean, std
        ex<<QString("for(c,%1,mean(column(c)))").arg(prename);
        names<<tr("mean(cols(%1))").arg(prename);
        ex<<QString("for(c,%1,std(column(c)))").arg(prename);
        names<<tr("std(cols(%1))").arg(prename);
    }
    return ex;
}
