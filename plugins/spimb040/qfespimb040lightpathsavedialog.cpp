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

#include "qfespimb040lightpathsavedialog.h"
#include "ui_qfespimb040lightpathsavedialog.h"

QFESPIMB040LightPathSaveDialog::QFESPIMB040LightPathSaveDialog(QString name, QStringList names, QFTableModel *model, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFESPIMB040LightPathSaveDialog)
{
    m_model=model;
    ui->setupUi(this);
    ui->cmbName->clear();
    ui->cmbName->addItems(names);
    ui->cmbName->setEditable(true);
    ui->cmbName->setEditText(name);
    if (names.contains(name)) ui->cmbName->setCurrentIndex(names.indexOf(name));
    ui->tableView->setModel(model);
}

QFESPIMB040LightPathSaveDialog::~QFESPIMB040LightPathSaveDialog()
{
    delete ui;
}

QString QFESPIMB040LightPathSaveDialog::getName() const {
    return ui->cmbName->currentText();
}

QList<bool> QFESPIMB040LightPathSaveDialog::getCheckedItems() const {
    QList<bool> result;
    for (int i=0; i<m_model->rowCount(); i++) {
        result.append(m_model->data(m_model->index(i, 0), Qt::CheckStateRole).toInt()==Qt::Checked);
    }
    return result;
}
