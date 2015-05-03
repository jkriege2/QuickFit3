/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

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

#include "qfrdrreplacedialog.h"
#include "programoptions.h"
#include "ui_qfrdrreplacedialog.h"
#include "qfpluginservices.h"

QFRDRReplaceDialog::QFRDRReplaceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFRDRReplaceDialog)
{
    ui->setupUi(this);
    ui->chkReplaceInName->setChecked(ProgramOptions::getConfigValue("QFRDRReplaceDialog/innames", true).toBool());
    ui->chkReplaceInFolders->setChecked(ProgramOptions::getConfigValue("QFRDRReplaceDialog/infolders", false).toBool());
    ui->chkCaseSensitive->setChecked(ProgramOptions::getConfigValue("QFRDRReplaceDialog/casesense", false).toBool());
    ui->edtFind->setText(ProgramOptions::getConfigValue("QFRDRReplaceDialog/find", "").toString());
    ui->edtReplace->setText(ProgramOptions::getConfigValue("QFRDRReplaceDialog/replace", "").toString());
    setWindowFlags(windowFlags()|Qt::WindowMinMaxButtonsHint);
}

QFRDRReplaceDialog::~QFRDRReplaceDialog()
{
    ProgramOptions::setConfigValue("QFRDRReplaceDialog/innames", ui->chkReplaceInName->isChecked());
    ProgramOptions::setConfigValue("QFRDRReplaceDialog/infolders", ui->chkReplaceInFolders->isChecked());
    ProgramOptions::setConfigValue("QFRDRReplaceDialog/casesense", ui->chkCaseSensitive->isChecked());
    ProgramOptions::setConfigValue("QFRDRReplaceDialog/find", ui->edtFind->text());
    ProgramOptions::setConfigValue("QFRDRReplaceDialog/replace", ui->edtReplace->text());
    delete ui;
}

bool QFRDRReplaceDialog::getReplaceName() const
{
    return ui->chkReplaceInName->isChecked();
}

bool QFRDRReplaceDialog::getReplaceFolder() const
{
    return ui->chkReplaceInFolders->isChecked();
}

QString QFRDRReplaceDialog::getFindString() const
{
    return ui->edtFind->text();
}

QString QFRDRReplaceDialog::getReplaceString() const
{
    return ui->edtReplace->text();
}

bool QFRDRReplaceDialog::getCaseSensitivity() const
{
    return ui->chkCaseSensitive->isChecked();
}

void QFRDRReplaceDialog::showHelp()
{
    QFPluginServices::getInstance()->displayMainHelpWindow("dlgrdrreplace.html");

}
