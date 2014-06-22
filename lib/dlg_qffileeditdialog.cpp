/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

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

#include "dlg_qffileeditdialog.h"
#include "ui_dlg_qffileeditdialog.h"
#include "qfstyledbutton.h"
#include "qfcompleterfromfile.h"
#include "programoptions.h"

QFFileEditDialog::QFFileEditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFFileEditDialog)
{
    ui->setupUi(this);

    QFStyledButton* btn=new QFStyledButton(QFStyledButton::SelectFile, ui->edtFile, ui->edtFile);
    ui->edtFile->addButton(btn);

    /*QFCompleterFromFile* c1=new QFCompleterFromFile(this);
    c1->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"completers/rdr_fileedit_filetype.txt");*/
    QFCompleterFromFile* c2=new QFCompleterFromFile(this);
    c2->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"completers/rdr_fileedit_filedescription.txt");

    ui->edtDescription->setCompleter(c2);
    //ui->edtType->setCompleter(c1);
}

QFFileEditDialog::~QFFileEditDialog()
{
    delete ui;
}

void QFFileEditDialog::init(const QString &file, const QString &type, const QString &description, const QStringList& filesTypes)
{
    ui->edtFile->setText(file);
    ui->cmbType->clear();
    ui->cmbType->addItems(filesTypes);
    ui->cmbType->setEditText(type);
    ui->edtDescription->setText(description);
}

QString QFFileEditDialog::getFile() const
{
    return ui->edtFile->text();
}

QString QFFileEditDialog::getType() const
{
    return ui->cmbType->currentText();
}

QString QFFileEditDialog::getDescription() const
{
    return ui->edtDescription->text();
}
