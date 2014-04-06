#include "qfespimlightsheetevaluationcopyconfigdialog.h"
#include "ui_qfespimlightsheetevaluationcopyconfigdialog.h"
#include "programoptions.h"

QFESPIMLightSheetEvaluationCopyConfigDialog::QFESPIMLightSheetEvaluationCopyConfigDialog(QWidget *parent) :
    QDialog(parent),
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
