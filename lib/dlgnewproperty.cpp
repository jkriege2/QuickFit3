/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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

#include "dlgnewproperty.h"
#include "ui_dlg_newproperty.h"

dlgNewProperty::dlgNewProperty(QWidget* parent):
    QDialog(parent), ui(new Ui::dlgNewProperty)
{
    ui->setupUi(this);


    ui->cmbName->setFocus(Qt::MouseFocusReason);
    ui->cmbName->lineEdit()->selectAll();
}

dlgNewProperty::~dlgNewProperty()
{
    delete ui;
}


QString dlgNewProperty::propertyName() {
    return ui->cmbName->currentText();
}

QString dlgNewProperty::propertyType() {
    return ui->cmbType->currentText();
}

int dlgNewProperty::propertyTypeInt() {
    return ui->cmbType->currentIndex();
}

QString dlgNewProperty::propertyValue() {
    return ui->edtValue->text();
}

void dlgNewProperty::setPropertyName(QString name) {
    ui->cmbName->setEditText(name);
}

void dlgNewProperty::addPropertyNames(const QStringList& names) {
    ui->cmbName->addItems(names);
}

void dlgNewProperty::setPropertyValue(QString value) {
    ui->edtValue->setText(value);
}

void dlgNewProperty::setPropertyType(int i) {
    ui->cmbType->setCurrentIndex(i);
}
