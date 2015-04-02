/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2014-09-26 12:42:55 +0200 (Fr, 26 Sep 2014) $  (revision $Rev: 3509 $)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "qfexporttotabledialog.h"
#include "ui_qfexporttotabledialog.h"
#include "qfpluginservices.h"
#include "programoptions.h"

QFExportToTableDialog::QFExportToTableDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFExportToTableDialog)
{
    ui->setupUi(this);
    ui->cmbRDR->setRequireColumnGraphics(true);
    ui->cmbRDR->setRequireNotReadonly(true);
    ui->cmbRDR->init(QFPluginServices::getInstance()->getCurrentProject());
    ui->edtNewTableName->setText(tr("table from plotter"));

    int idx=QFPluginServices::getInstance()->getCurrentProject()->getProperty("QFExportToTableDialog/rdr", 0).toInt();
    if (idx) ui->cmbRDR->setCurrentRDRID(idx);
    ui->chkShowTableEditor->setChecked(QFPluginServices::getInstance()->getCurrentProject()->getProperty("QFExportToTableDialog/showEditor", false).toBool());
    ui->labDescription->setVisible(false);
}

QFExportToTableDialog::~QFExportToTableDialog()
{
    QFPluginServices::getInstance()->getCurrentProject()->setQFProperty("QFExportToTableDialog/rdr", ui->cmbRDR->currentRDRID());
    QFPluginServices::getInstance()->getCurrentProject()->setQFProperty("QFExportToTableDialog/newtable", ui->chkNewTable->isChecked());
    QFPluginServices::getInstance()->getCurrentProject()->setQFProperty("QFExportToTableDialog/showEditor", ui->chkShowTableEditor->isChecked());
    delete ui;
}

QFRDRTableInterface *QFExportToTableDialog::getTable() const
{
    return ui->cmbRDR->currentTable();
}

QFRawDataRecord *QFExportToTableDialog::getRDR() const
{
    return ui->cmbRDR->currentRDR();
}


bool QFExportToTableDialog::getNewTable(QString &name) const
{
    if (ui->chkNewTable->isChecked()) {
        name=ui->edtNewTableName->text();
        return true;
    }
    return false;
}

bool QFExportToTableDialog::getShowEditor() const
{
    return ui->chkShowTableEditor->isChecked();
}

void QFExportToTableDialog::setDescription(const QString &text)
{
    ui->labDescription->setText(text);
    ui->labDescription->setVisible(!text.isEmpty());
}

QString QFExportToTableDialog::getPrefix()
{
    return ui->edtPrefix->text();
}

void QFExportToTableDialog::addWidget(const QString &label, QWidget *wid)
{
    wid->setParent(this);
    ui->formLayout->addRow(label, wid);
}

void QFExportToTableDialog::on_cmbRDR_refilled(bool full)
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

void QFExportToTableDialog::on_chkNewTable_toggled(bool checked)
{
    //qDebug()<<"on_chkNewTable_toggled(checked="<<checked<<")";
    ui->cmbRDR->setEnabled(!checked);
    ui->edtNewTableName->setEnabled(checked);
}

