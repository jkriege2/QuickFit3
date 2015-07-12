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

#include "functionreferencedialog.h"
#include "ui_functionreferencedialog.h"
#include "programoptions.h"
#include "qftools.h"


FunctionReferenceDialog::FunctionReferenceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FunctionReferenceDialog)
{
    ui->setupUi(this);

    loadWidgetGeometry(ProgramOptions::getInstance(), this, "FunctionReferenceDialog");

    ProgramOptions::getConfigQPlainTextEdit(ui->edtLong, "FunctionReferenceDialog/edtLong");
    ProgramOptions::getConfigQLineEdit(ui->edtTemplate, "FunctionReferenceDialog/edtTemplate");
    ProgramOptions::getConfigQLineEdit(ui->edtName, "FunctionReferenceDialog/edtName");
    ProgramOptions::getConfigQLineEdit(ui->edtShort, "FunctionReferenceDialog/edtShort");

}

FunctionReferenceDialog::~FunctionReferenceDialog()
{
    saveWidgetGeometry(ProgramOptions::getInstance(), this, "FunctionReferenceDialog");

    ProgramOptions::setConfigQPlainTextEdit(ui->edtLong, "FunctionReferenceDialog/edtLong");
    ProgramOptions::setConfigQLineEdit(ui->edtName, "FunctionReferenceDialog/edtName");
    ProgramOptions::setConfigQLineEdit(ui->edtShort, "FunctionReferenceDialog/edtShort");
    ProgramOptions::setConfigQLineEdit(ui->edtTemplate, "FunctionReferenceDialog/edtTemplate");
    delete ui;
}

QString FunctionReferenceDialog::insertText() const
{
    return QString("$$funcref_start$$<a name=\"%1\"/><!-- func:%1 -->\n  <b><tt><!-- template -->%2<!-- /template --></tt> - <i> %3 </i>:</b>\n$$funcref_description$$\n    %4  <!-- /func:%1 -->\n$$funcref_end$$").arg(ui->edtName->text()).arg(ui->edtTemplate->text()).arg(ui->edtShort->text()).arg(ui->edtLong->toPlainText());
}

void FunctionReferenceDialog::on_edtName_textChanged(const QString &text)
{
    if (!text.isEmpty()) {
        if (ui->edtTemplate->text().isEmpty()) {
            ui->edtTemplate->setText(text+"()");
        } else {
            QString s=ui->edtTemplate->text();
            int idx=s.indexOf('(');
            if (idx>=0) {
                ui->edtTemplate->setText(text+s.right(s.size()-idx));
            } else {
                ui->edtTemplate->setText(text);
            }
        }
    }
}
