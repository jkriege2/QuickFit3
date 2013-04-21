#include "imfcscalibrationwizard.h"
#include "ui_imfcscalibrationwizard.h"

ImFCSCalibrationWizard::ImFCSCalibrationWizard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImFCSCalibrationWizard)
{
    ui->setupUi(this);
}

ImFCSCalibrationWizard::~ImFCSCalibrationWizard()
{
    delete ui;
}

void ImFCSCalibrationWizard::on_btnClose_clicked()
{
    close();
}

void ImFCSCalibrationWizard::on_btnStep1_clicked()
{
    emit run1();
}

void ImFCSCalibrationWizard::on_btnStep2_clicked()
{
    emit run2();
}

void ImFCSCalibrationWizard::on_btnStep3_clicked()
{
    emit run3();
}

void ImFCSCalibrationWizard::on_btnStep4_clicked()
{
    emit run4();
}

void ImFCSCalibrationWizard::on_btnStep5_clicked()
{
    emit run5();
}

void ImFCSCalibrationWizard::on_btnHelp_clicked()
{
    emit showTutorial();
}
