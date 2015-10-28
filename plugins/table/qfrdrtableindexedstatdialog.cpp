/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg



    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "qfrdrtableindexedstatdialog.h"
#include "ui_qfrdrtableindexedstatdialog.h"

QFRDRTableIndexedStatDialog::QFRDRTableIndexedStatDialog(QWidget *parent) :
    QFDialog(parent),
    ui(new Ui::QFRDRTableIndexedStatDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags()|Qt::WindowMinMaxButtonsHint);
    ui->lstData->setModel(&cols);

    ProgramOptions::getConfigQCheckBox(ui->chkMax, "QFRDRTableIndexedStatDialog/chkMax");
    ProgramOptions::getConfigQCheckBox(ui->chkMean, "QFRDRTableIndexedStatDialog/chkMean");
    ProgramOptions::getConfigQCheckBox(ui->chkMedian, "QFRDRTableIndexedStatDialog/chkMedian");
    ProgramOptions::getConfigQCheckBox(ui->chkMin, "QFRDRTableIndexedStatDialog/chkMin");
    ProgramOptions::getConfigQCheckBox(ui->chkNMAD, "QFRDRTableIndexedStatDialog/chkNMAD");
    ProgramOptions::getConfigQCheckBox(ui->chkQ25, "QFRDRTableIndexedStatDialog/chkQ25");
    ProgramOptions::getConfigQCheckBox(ui->chkQ75, "QFRDRTableIndexedStatDialog/chkQ75");
    ProgramOptions::getConfigQCheckBox(ui->chkStdDev, "QFRDRTableIndexedStatDialog/chkStdDev");
    ProgramOptions::getConfigQRadioButton(ui->radAddToPlot, "QFRDRTableIndexedStatDialog/radAddToPlot");
    ProgramOptions::getConfigQRadioButton(ui->raddAddPlots, "QFRDRTableIndexedStatDialog/raddAddPlots");
    ProgramOptions::getConfigQRadioButton(ui->radNoPlots, "QFRDRTableIndexedStatDialog/radNoPlots");
}

QFRDRTableIndexedStatDialog::~QFRDRTableIndexedStatDialog()
{
    ProgramOptions::setConfigQCheckBox(ui->chkMax, "QFRDRTableIndexedStatDialog/chkMax");
    ProgramOptions::setConfigQCheckBox(ui->chkMean, "QFRDRTableIndexedStatDialog/chkMean");
    ProgramOptions::setConfigQCheckBox(ui->chkMedian, "QFRDRTableIndexedStatDialog/chkMedian");
    ProgramOptions::setConfigQCheckBox(ui->chkMin, "QFRDRTableIndexedStatDialog/chkMin");
    ProgramOptions::setConfigQCheckBox(ui->chkNMAD, "QFRDRTableIndexedStatDialog/chkNMAD");
    ProgramOptions::setConfigQCheckBox(ui->chkQ25, "QFRDRTableIndexedStatDialog/chkQ25");
    ProgramOptions::setConfigQCheckBox(ui->chkQ75, "QFRDRTableIndexedStatDialog/chkQ75");
    ProgramOptions::setConfigQCheckBox(ui->chkStdDev, "QFRDRTableIndexedStatDialog/chkStdDev");
    ProgramOptions::setConfigQRadioButton(ui->radAddToPlot, "QFRDRTableIndexedStatDialog/radAddToPlot");
    ProgramOptions::setConfigQRadioButton(ui->raddAddPlots, "QFRDRTableIndexedStatDialog/raddAddPlots");
    ProgramOptions::setConfigQRadioButton(ui->radNoPlots, "QFRDRTableIndexedStatDialog/radNoPlots");

    delete ui;
}

void QFRDRTableIndexedStatDialog::setTable(QFRDRTable *plugin)
{
    QFTablePluginModel* table=plugin->model();
    this->table=table;
    this->plugin=plugin;
    ui->lstData->setModel(NULL);
    cols.setStringList(table->getColumnTitles());
    cols.unCheckAll();
    ui->lstData->setModel(&cols);

    ui->cmbIndex->clear();
    ui->cmbResults->clear();
    ui->cmbResults->addItem(tr("--- add columns ---"), -1);
    for (int i=0; i<table->columnCount(); i++) {
        ui->cmbResults->addItem(table->headerData(i, Qt::Horizontal).toString(), QVariant(i));
        ui->cmbIndex->addItem(table->headerData(i, Qt::Horizontal).toString(), QVariant(i));
    }
    ui->cmbResults->setCurrentIndex(0);

    ui->cmbPlots->clear();
    //ui->cmbPlots->addItem(tr("--- no ---"), -2);
    //ui->cmbPlots->addItem(tr("--- add new graph ---"), -1);
    for (int i=0; i<plugin->colgraphGetPlotCount(); i++) {
        ui->cmbPlots->addItem(tr("add to: %1").arg(plugin->colgraphGetPlotTitle(i)), i);
    }
    ui->cmbPlots->setCurrentIndex(1);
}

void QFRDRTableIndexedStatDialog::setSelectedColumns(const QList<int> &cols)
{
    if (cols.size()>1) {
        for (int i=1; i<cols.size(); i++) {
            this->cols.setChecked(cols[i]);
        }
    }
    ui->cmbIndex->setCurrentIndex(cols.value(0, -1));

}


int QFRDRTableIndexedStatDialog::indexColumn() const
{
    return ui->cmbIndex->currentIndex();
}


bool QFRDRTableIndexedStatDialog::avg() const
{
    return ui->chkMean->isChecked();
}

bool QFRDRTableIndexedStatDialog::std() const
{
    return ui->chkStdDev->isChecked();
}

bool QFRDRTableIndexedStatDialog::median() const
{
    return ui->chkMedian->isChecked();
}

bool QFRDRTableIndexedStatDialog::q25() const
{
    return ui->chkQ25->isChecked();
}

bool QFRDRTableIndexedStatDialog::q75() const
{
    return ui->chkQ75->isChecked();
}

bool QFRDRTableIndexedStatDialog::min() const
{
    return ui->chkMin->isChecked();
}

bool QFRDRTableIndexedStatDialog::max() const
{
    return ui->chkMax->isChecked();
}

bool QFRDRTableIndexedStatDialog::nmad() const
{
    return ui->chkNMAD->isChecked();
}

int QFRDRTableIndexedStatDialog::getResultStartColumn() const
{
    return ui->cmbResults->currentData().toInt();
}

int QFRDRTableIndexedStatDialog::addToGraph() const
{
    return ui->cmbPlots->currentData().toInt();
}

bool QFRDRTableIndexedStatDialog::addToExistingGraph() const
{
    return ui->radAddToPlot->isChecked();
}

bool QFRDRTableIndexedStatDialog::addNewGraphs() const
{
    return ui->raddAddPlots->isChecked();
}

bool QFRDRTableIndexedStatDialog::createGraphs() const
{
    return !(ui->radNoPlots->isChecked());
}

QList<int> QFRDRTableIndexedStatDialog::getDataColumns() const
{
    return cols.getChecked();
}

QString QFRDRTableIndexedStatDialog::getDataColumnsExpression() const
{
    QList<int> data=getDataColumns();
    QString res;
    for (int i=0; i<data.size(); i++) {
        if (res.size()>0) res+=", ";
        res+=QString::number(data[i]+1);
    }
    return "["+res+"]";
}

QStringList QFRDRTableIndexedStatDialog::getExpressions(QStringList &names, QString &addToPre, QList<DataProps> &props) const
{
    props.clear();
    names.clear();
    addToPre.clear();
    QString pre=plugin->getParserPreScript();
    QString prename_idx="indexedstats_index";
    int cnt=0;
    while (pre.contains(prename_idx)) {
        cnt++;
        prename_idx=QString("indexedstats_index%1").arg(cnt);
    }

    QStringList ex;
    QString c=getDataColumnsExpression();
    int coln=getDataColumns().size();
    QList<int> dat=getDataColumns();

    addToPre=QString("%1 = %2; comment(\"list of columns used for boxplot No. %3\");\n").arg(prename_idx).arg(indexColumn()+1).arg(cnt+1);


    ex<<QString("unique(column(%1))").arg(prename_idx);
    names<<tr("unique_index_value(%1)").arg(prename_idx);
    ex<<QString("indexedcount(column(%1),column(%1))").arg(prename_idx);
    names<<tr("data_count(%1)").arg(prename_idx);

    for (int i=0; i<coln; i++) {
        DataProps p;
        p.dataname=table->headerData(dat[i], Qt::Horizontal).toString();
        p.cid=0;
        if (ui->chkMean->isChecked()) {
            ex<<QString("indexedmean(column(%2),column(%1))").arg(prename_idx).arg(dat[i]+1);
            names<<tr("mean(#%2: %1)").arg(p.dataname).arg(dat[i]+1);
            p.cmean=ex.size()-1;
        }
        if (ui->chkStdDev->isChecked()) {
            ex<<QString("indexedstd(column(%2),column(%1))").arg(prename_idx).arg(dat[i]+1);
            names<<tr("std(#%2: %1)").arg(p.dataname).arg(dat[i]+1);
            p.cstd=ex.size()-1;
        }
        if (ui->chkMin->isChecked()) {
            ex<<QString("indexedmin(column(%2),column(%1))").arg(prename_idx).arg(dat[i]+1);
            names<<tr("min(#%2: %1)").arg(p.dataname).arg(dat[i]+1);
            p.cmin=ex.size()-1;
        }
        if (ui->chkQ25->isChecked()) {
            ex<<QString("indexedquantile(column(%2),column(%1),0.25)").arg(prename_idx).arg(dat[i]+1);
            names<<tr("q25(#%2: %1)").arg(p.dataname).arg(dat[i]+1);
            p.cq25=ex.size()-1;
        }
        if (ui->chkMedian->isChecked()) {
            ex<<QString("indexedmedian(column(%2),column(%1))").arg(prename_idx).arg(dat[i]+1);
            names<<tr("median(#%2: %1)").arg(p.dataname).arg(dat[i]+1);
            p.cmedian=ex.size()-1;
        }
        if (ui->chkQ75->isChecked()) {
            ex<<QString("indexedquantile(column(%2),column(%1),0.75)").arg(prename_idx).arg(dat[i]+1);
            names<<tr("q75(#%2: %1)").arg(p.dataname).arg(dat[i]+1);
            p.cq75=ex.size()-1;
        }
        if (ui->chkMax->isChecked()) {
            ex<<QString("indexedmax(column(%2),column(%1))").arg(prename_idx).arg(dat[i]+1);
            names<<tr("max(#%2: %1)").arg(p.dataname).arg(dat[i]+1);
            p.cmax=ex.size()-1;
        }
        if (ui->chkNMAD->isChecked()) {
            ex<<QString("indexednmad(column(%2),column(%1))").arg(prename_idx).arg(dat[i]+1);
            names<<tr("max(#%2: %1)").arg(p.dataname).arg(dat[i]+1);
            p.cnmad=ex.size()-1;
        }
        props.append(p);
    }

    return ex;
}
