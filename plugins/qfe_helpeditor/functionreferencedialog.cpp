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

#include "functionreferencedialog.h"
#include "ui_functionreferencedialog.h"

FunctionReferenceDialog::FunctionReferenceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FunctionReferenceDialog)
{
    ui->setupUi(this);
}

FunctionReferenceDialog::~FunctionReferenceDialog()
{
    delete ui;
}

QString FunctionReferenceDialog::insertText() const
{
    return QString("$$funcref_start$$<a name=\"%1\"/><!-- func:%1 -->\n  <b><tt><!-- template -->%2<!-- /template --></tt> - <i> %3 </i>:</b>\n$$funcref_description$$\n    %4  <!-- /func:%1 -->\n$$funcref_end$$").arg(ui->edtName->text()).arg(ui->edtTemplate->text()).arg(ui->edtShort->text()).arg(ui->edtLong->toPlainText());
}
