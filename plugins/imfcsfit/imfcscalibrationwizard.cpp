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

#include "imfcscalibrationwizard.h"
#include "ui_imfcscalibrationwizard.h"
#include "qfrdrimfcscorrelatorremote.h"
#include "programoptions.h"
#include "qftools.h"
#include "qfproject.h"
#include "qfpluginservices.h"
#include "qfrawdatarecordfactory.h"

ImFCSCalibrationWizard::ImFCSCalibrationWizard(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImFCSCalibrationWizard)
{
    ui->setupUi(this);
    QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
    ui->btnLoadFile->setEnabled(p&&p->getRawDataRecordFactory()->contains("imaging_fcs")&&dynamic_cast<QFRDRIMFCSCorrelatorRemote*>(p->getRawDataRecordFactory()->getPlugin("imaging_fcs")));
    ui->btnCorrelate->setEnabled(p&&p->getRawDataRecordFactory()->contains("imaging_fcs")&&dynamic_cast<QFRDRIMFCSCorrelatorRemote*>(p->getRawDataRecordFactory()->getPlugin("imaging_fcs")));
    loadWidgetGeometry(*(ProgramOptions::getInstance()->getQSettings()), this, "ImFCSCalibrationWizard");}

ImFCSCalibrationWizard::~ImFCSCalibrationWizard()
{
    delete ui;
}

void ImFCSCalibrationWizard::closeEvent(QCloseEvent *event) {
    saveWidgetGeometry(*(ProgramOptions::getInstance()->getQSettings()), this, "ImFCSCalibrationWizard");
    QWidget::closeEvent(event);
}

QFPlotter *ImFCSCalibrationWizard::getPltD() const
{
    return ui->pltD;
}

QFPlotter *ImFCSCalibrationWizard::getPltWxy() const
{
    return ui->pltWxy;
}

void ImFCSCalibrationWizard::enableStep2()
{
    ui->widget2->setEnabled(true);
}

void ImFCSCalibrationWizard::hideStep01(bool hide)
{
    ui->widStep01->setVisible(!hide);
    if (hide) {
        ui->lab2->setText("1.");
        ui->lab3->setText("2.");
        ui->lab4->setText("3.");
        ui->lab5->setText("4.");
    } else {
        ui->lab2->setText("2.");
        ui->lab3->setText("3.");
        ui->lab4->setText("4.");
        ui->lab5->setText("5.");
    }
}


void ImFCSCalibrationWizard::on_btnClose_clicked()
{
    close();
}

void ImFCSCalibrationWizard::on_btnStep1_clicked()
{
    emit run1();
    ui->widget2->setEnabled(true);
}

void ImFCSCalibrationWizard::on_btnStep2_clicked()
{
    emit run2();
    ui->widget3->setEnabled(true);
}

void ImFCSCalibrationWizard::on_btnStep3_clicked()
{
    emit run3();
    ui->widget4->setEnabled(true);
}

void ImFCSCalibrationWizard::on_btnStep4_clicked()
{
    emit run4();
    ui->widget5->setEnabled(true);
}

void ImFCSCalibrationWizard::on_btnStep5_clicked()
{
    emit run5();
}

void ImFCSCalibrationWizard::on_btnLoadFile_clicked()
{
    emit loadFile();
}

void ImFCSCalibrationWizard::on_btnCorrelate_clicked()
{
    emit correlate();
}

void ImFCSCalibrationWizard::on_btnHelp_clicked()
{
    emit showTutorial();
}


void ImFCSCalibrationWizard::on_btnReset_clicked()
{
    ui->widget2->setEnabled(false);
    ui->widget3->setEnabled(false);
    ui->widget4->setEnabled(false);
    ui->widget5->setEnabled(false);
    ui->pltD->clearGraphs(true);
}

void ImFCSCalibrationWizard::on_btnUnlock_clicked()
{
    ui->widget2->setEnabled(true);
    ui->widget3->setEnabled(true);
    ui->widget4->setEnabled(true);
    ui->widget5->setEnabled(true);
}
