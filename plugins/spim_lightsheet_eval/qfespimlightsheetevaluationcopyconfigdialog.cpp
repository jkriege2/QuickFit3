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

#include "qfespimlightsheetevaluationcopyconfigdialog.h"
#include "ui_qfespimlightsheetevaluationcopyconfigdialog.h"
#include "programoptions.h"

QFESPIMLightSheetEvaluationCopyConfigDialog::QFESPIMLightSheetEvaluationCopyConfigDialog(QWidget *parent) :
    QFDialog(parent),
    ui(new Ui::QFESPIMLightSheetEvaluationCopyConfigDialog)
{
    ui->setupUi(this);
    ui->cmbOutput->setCurrentIndex(ProgramOptions::getConfigValue("QFESPIMLightSheetEvaluationCopyConfigDialog/cmbOutput", 0).toInt());
    ui->cmbUnits->setCurrentIndex(ProgramOptions::getConfigValue("QFESPIMLightSheetEvaluationCopyConfigDialog/cmbUnits", 0).toInt());
    ui->chkSaveFit->setChecked(ProgramOptions::getConfigValue("QFESPIMLightSheetEvaluationCopyConfigDialog/chkSaveFit", true).toBool());
    ui->chkSaveNormalized->setChecked(ProgramOptions::getConfigValue("QFESPIMLightSheetEvaluationCopyConfigDialog/chkSaveNormalized", true).toBool());
    ui->chkShiftCenter->setChecked(ProgramOptions::getConfigValue("QFESPIMLightSheetEvaluationCopyConfigDialog/chkShiftCenter", true).toBool());
}

QFESPIMLightSheetEvaluationCopyConfigDialog::~QFESPIMLightSheetEvaluationCopyConfigDialog()
{
    ProgramOptions::setConfigValue("QFESPIMLightSheetEvaluationCopyConfigDialog/cmbOutput", ui->cmbOutput->currentIndex());
    ProgramOptions::setConfigValue("QFESPIMLightSheetEvaluationCopyConfigDialog/cmbUnits", ui->cmbUnits->currentIndex());
    ProgramOptions::setConfigValue("QFESPIMLightSheetEvaluationCopyConfigDialog/chkSaveNormalized", ui->chkSaveNormalized->isChecked());
    ProgramOptions::setConfigValue("QFESPIMLightSheetEvaluationCopyConfigDialog/chkSaveFit", ui->chkSaveFit->isChecked());
    ProgramOptions::setConfigValue("QFESPIMLightSheetEvaluationCopyConfigDialog/chkShiftCenter", ui->chkShiftCenter->isChecked());
    delete ui;
}

int QFESPIMLightSheetEvaluationCopyConfigDialog::getOutput() const {
    return ui->cmbOutput->currentIndex();
}

int QFESPIMLightSheetEvaluationCopyConfigDialog::getUnits() const {
    return ui->cmbUnits->currentIndex();
}

bool QFESPIMLightSheetEvaluationCopyConfigDialog::getSaveNormalized() const {
    return ui->chkSaveNormalized->isChecked();
}

bool QFESPIMLightSheetEvaluationCopyConfigDialog::getSaveFit() const {
    return ui->chkSaveFit->isChecked();
}

bool QFESPIMLightSheetEvaluationCopyConfigDialog::getShiftCenter() const {
    return ui->chkShiftCenter->isChecked();
}
