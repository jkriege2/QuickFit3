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
    saveWidgetGeometry(*(ProgramOptions::getInstance()->getQSettings()), this, "ImFCSCalibrationWizard");
    delete ui;
}

QFPlotter *ImFCSCalibrationWizard::getPltD() const
{
    return ui->pltD;
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
}

void ImFCSCalibrationWizard::on_btnUnlock_clicked()
{
    ui->widget2->setEnabled(true);
    ui->widget3->setEnabled(true);
    ui->widget4->setEnabled(true);
    ui->widget5->setEnabled(true);
}
