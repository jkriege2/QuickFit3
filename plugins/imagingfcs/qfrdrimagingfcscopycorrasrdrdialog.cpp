/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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

#include "qfrdrimagingfcscopycorrasrdrdialog.h"
#include "ui_qfrdrimagingfcscopycorrasrdrdialog.h"
#include "programoptions.h"

QFRDRImagingFCSCopyCorrAsRDRDialog::QFRDRImagingFCSCopyCorrAsRDRDialog(bool isFCCS, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFRDRImagingFCSCopyCorrAsRDRDialog)
{
    ui->setupUi(this);
    ui->chkCopyACF->setEnabled(isFCCS);
    ui->chkCopyACF->setChecked(true);
    if (ProgramOptions::getInstance()) {
        ui->edtSelName->setText(ProgramOptions::getInstance()->getQSettings()->value("QFRDRImagingFCSCopyCorrAsRDRDialog/selName", ui->edtSelName->text()).toString());
        ui->chkCopyACF->setChecked(ProgramOptions::getInstance()->getQSettings()->value("QFRDRImagingFCSCopyCorrAsRDRDialog/copyACF", ui->chkCopyACF->isChecked()).toBool());
        ui->chkCopyAllRuns->setChecked(ProgramOptions::getInstance()->getQSettings()->value("QFRDRImagingFCSCopyCorrAsRDRDialog/chkCopyAllRuns", ui->chkCopyAllRuns->isChecked()).toBool());
        ui->chkCreateNamedSel->setChecked(ProgramOptions::getInstance()->getQSettings()->value("QFRDRImagingFCSCopyCorrAsRDRDialog/chkCreateNamedSel", ui->chkCreateNamedSel->isChecked()).toBool());

    }
}

QFRDRImagingFCSCopyCorrAsRDRDialog::~QFRDRImagingFCSCopyCorrAsRDRDialog()
{
    if (ProgramOptions::getInstance()) {
        ProgramOptions::getInstance()->getQSettings()->setValue("QFRDRImagingFCSCopyCorrAsRDRDialog/selName", ui->edtSelName->text());
        ProgramOptions::getInstance()->getQSettings()->setValue("QFRDRImagingFCSCopyCorrAsRDRDialog/copyACF", ui->chkCopyACF->isChecked());
        ProgramOptions::getInstance()->getQSettings()->setValue("QFRDRImagingFCSCopyCorrAsRDRDialog/chkCopyAllRuns", ui->chkCopyAllRuns->isChecked());
        ProgramOptions::getInstance()->getQSettings()->setValue("QFRDRImagingFCSCopyCorrAsRDRDialog/chkCreateNamedSel", ui->chkCreateNamedSel->isChecked());
    }
    delete ui;
}

bool QFRDRImagingFCSCopyCorrAsRDRDialog::getCopyACF() const
{
    return ui->chkCopyACF->isChecked();
}

bool QFRDRImagingFCSCopyCorrAsRDRDialog::getCopyAllRuns() const
{
    return ui->chkCopyAllRuns->isChecked();
}

bool QFRDRImagingFCSCopyCorrAsRDRDialog::getCreateSelection() const
{
    return ui->chkCreateNamedSel->isChecked();
}

QString QFRDRImagingFCSCopyCorrAsRDRDialog::getSelectionName() const
{
    return ui->edtSelName->text();
}
