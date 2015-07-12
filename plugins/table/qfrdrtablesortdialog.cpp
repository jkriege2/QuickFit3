/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    

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

#include "qfrdrtablesortdialog.h"
#include "ui_qfrdrtablesortdialog.h"

QFRDRTableSortDialog::QFRDRTableSortDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFRDRTableSortDialog)
{
    ui->setupUi(this);
}

QFRDRTableSortDialog::~QFRDRTableSortDialog()
{
    delete ui;
}

void QFRDRTableSortDialog::setColumns(QList<QPair<int, QString> > cols)
{
    ui->cmbColumn->clear();
    for (int i=0; i<cols.size(); i++) {
        ui->cmbColumn->addItem(QString("%1: %2").arg(cols[i].first+1).arg(cols[i].second), cols[i].first);
    }
}

void QFRDRTableSortDialog::setCurrentColumn(int col)
{
    for (int i=0; i<ui->cmbColumn->count(); i++) {
        if (ui->cmbColumn->itemData(i).toInt()==col) {
            ui->cmbColumn->setCurrentIndex(i);
        }
    }
}

bool QFRDRTableSortDialog::sortAscending() const
{
    return ui->cmbOrder->currentIndex()==0;
}

int QFRDRTableSortDialog::sortColumn() const
{
    return ui->cmbColumn->itemData(ui->cmbColumn->currentIndex()).toInt();
}
