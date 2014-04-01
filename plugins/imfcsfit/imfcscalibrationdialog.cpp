#include "imfcscalibrationdialog.h"
#include "qfpluginservices.h"
#include "programoptions.h"
#include "ui_imfcscalibrationdialog.h"
#include "qffitfunction.h"
#include "qffitfunctionmanager.h"

ImFCSCalibrationDialog::ImFCSCalibrationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImFCSCalibrationDialog)
{
    ui->setupUi(this);
    if (ProgramOptions::getInstance()) {
        ui->edtValues->setPlainText(ProgramOptions::getConfigValue("ImFCSCalibrationDialog/values", ui->edtValues->toPlainText()).toString());
        ui->spinFocusHeight->setValue(ProgramOptions::getConfigValue("ImFCSCalibrationDialog/focusheight", ui->spinFocusHeight->value()).toDouble());
        ui->spinFocusHeightError->setValue(ProgramOptions::getConfigValue("ImFCSCalibrationDialog/focusheight_error", ui->spinFocusHeightError->value()).toDouble());
        ui->chkFixOffset->setChecked(ProgramOptions::getConfigValue("ImFCSCalibrationDialog/fixoffset", ui->chkFixOffset->isChecked()).toBool());
    }
}

ImFCSCalibrationDialog::~ImFCSCalibrationDialog()
{
    delete ui;
}

void ImFCSCalibrationDialog::closeEvent(QCloseEvent *event) {
    if (ProgramOptions::getInstance()) {
        ProgramOptions::setConfigValue("ImFCSCalibrationDialog/values", ui->edtValues->toPlainText());
        ProgramOptions::setConfigValue("ImFCSCalibrationDialog/focusheight", ui->spinFocusHeight->value());
        ProgramOptions::setConfigValue("ImFCSCalibrationDialog/focusheight_error", ui->spinFocusHeightError->value());
        ProgramOptions::setConfigValue("ImFCSCalibrationDialog/fixoffset", ui->chkFixOffset->isChecked());
    }
}

QList<double> ImFCSCalibrationDialog::getValues() const {
    QStringList v=ui->edtValues->toPlainText().split('\n');
    QList<double> vals;
    for (int i=0; i<v.size(); i++) {
        QString s=v[i].trimmed().toLower();
        if (!s.isEmpty()) {
            bool ok=false;
            double val=s.toDouble(&ok);
            if (ok && val>0) {
                vals.append(val);
            }
        }
    }
    return vals;
}

double ImFCSCalibrationDialog::getFocusHeight() const
{
    return ui->spinFocusHeight->value();
}

double ImFCSCalibrationDialog::getFocusHeightError() const
{
    return ui->spinFocusHeightError->value();
}

bool ImFCSCalibrationDialog::getFixOffset() const
{
    return ui->chkFixOffset->isChecked();
}

void ImFCSCalibrationDialog::setFitModels(const QStringList& models, const QString& defaultModel)
{
    ui->cmbFitModel->clear();
    if (!QFPluginServices::getInstance()) return;
    if (!QFPluginServices::getInstance()->getFitFunctionManager()) return;
    for (int i=0; i<models.size(); i++) {
        QFFitFunction* f=QFPluginServices::getInstance()->getFitFunctionManager()->createFunction(models[i], NULL);
        if (f) ui->cmbFitModel->addItem(f->name(), models[i]);
    }
    ui->cmbFitModel->setCurrentIndex(ui->cmbFitModel->findData(defaultModel));
}

QString ImFCSCalibrationDialog::getFitModel() const
{
    return ui->cmbFitModel->itemData(ui->cmbFitModel->currentIndex()).toString();
}

void ImFCSCalibrationDialog::on_btnHelp_clicked()
{
        QFPluginServices::getInstance()->displayPluginHelpWindow("imfcs_fit", "calibration.html");

}
