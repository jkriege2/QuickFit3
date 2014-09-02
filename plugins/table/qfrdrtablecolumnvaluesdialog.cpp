/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "qfrdrtablecolumnvaluesdialog.h"
#include "ui_qfrdrtablecolumnvaluesdialog.h"
#include "programoptions.h"
#include "qftools.h"

QFRDRTableColumnValuesDialog::QFRDRTableColumnValuesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFRDRTableColumnValuesDialog)
{
    ui->setupUi(this);
    ui->edtStart->setCheckBounds(false, false);
    ui->edtStart->setValue(ProgramOptions::getConfigValue("QFRDRTableColumnValuesDialog/start", 0).toDouble());
    ui->edtEnd->setCheckBounds(false, false);
    ui->edtEnd->setValue(ProgramOptions::getConfigValue("QFRDRTableColumnValuesDialog/end", 10).toDouble());
    ui->spinCount->setValue(ProgramOptions::getConfigValue("QFRDRTableColumnValuesDialog/count", 11).toInt());
    ui->cmbType->setCurrentIndex(ProgramOptions::getConfigValue("QFRDRTableColumnValuesDialog/type", 0).toInt());
    ui->radColumn->setChecked(ProgramOptions::getConfigValue("QFRDRTableColumnValuesDialog/column", true).toBool());
    ui->radRow->setChecked(ProgramOptions::getConfigValue("QFRDRTableColumnValuesDialog/row", false).toBool());
    updatePreview();
}

QFRDRTableColumnValuesDialog::~QFRDRTableColumnValuesDialog()
{
    ProgramOptions::setConfigValue("QFRDRTableColumnValuesDialog/start", ui->edtStart->value());
    ProgramOptions::setConfigValue("QFRDRTableColumnValuesDialog/end", ui->edtEnd->value());
    ProgramOptions::setConfigValue("QFRDRTableColumnValuesDialog/count", ui->spinCount->value());
    ProgramOptions::setConfigValue("QFRDRTableColumnValuesDialog/type", ui->cmbType->currentIndex());
    ProgramOptions::setConfigValue("QFRDRTableColumnValuesDialog/column", ui->radColumn->isChecked());
    ProgramOptions::setConfigValue("QFRDRTableColumnValuesDialog/row", ui->radRow->isChecked());

    delete ui;
}

void QFRDRTableColumnValuesDialog::updatePreview()
{
    QString p=0;
    QList<double> l=getValues();
    for (int i=0; i<qMin(10, l.size()); i++) {
        if (!p.isEmpty()) p.append("; ");
        p.append(QString::number(l[i]));
    }
    if (l.size()>10) {
        p.append(" ... ");
        bool ok=false;
        for (int i=qMax(l.size()-5, qMin(10, l.size())); i<l.size(); i++) {
            if (ok) p.append("; ");
            ok=true;
            p.append(QString::number(l[i]));
        }
    }

    ui->labPreview->setText(p);
}

bool QFRDRTableColumnValuesDialog::addAsColumn() const
{
    return ui->radColumn->isChecked();
}

void QFRDRTableColumnValuesDialog::setCount(int count)
{
    ui->spinCount->setValue(count);
}

void QFRDRTableColumnValuesDialog::setColumn(bool column)
{
    ui->radColumn->setChecked(column);
    ui->radRow->setChecked(!column);
}

QList<double> QFRDRTableColumnValuesDialog::getValues() const {
    QList<double> l;
    if (ui->cmbType->currentIndex()==0) { // linear
        double inc=(ui->edtEnd->value()-ui->edtStart->value())/double(ui->spinCount->value()-1);
        for (int i=0; i<ui->spinCount->value(); i++) {
            l.append(ui->edtStart->value()+double(i)*inc);
        }
    } else { // logarithmic
        double log_start=log(ui->edtStart->value())/log(10.0);
        double log_end=log(ui->edtEnd->value())/log(10.0);
        if (QFFloatIsOK(log_start) && QFFloatIsOK(log_end)) {
            for (int i=0; i<ui->spinCount->value(); i++) {
                double v=double(i)*(log_end-log_start)/double(ui->spinCount->value()-1)+log_start;
                l.append(pow(10.0, v));
            }
        }
    }
    return l;
}
