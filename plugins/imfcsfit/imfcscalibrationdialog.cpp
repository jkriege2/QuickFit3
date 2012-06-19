#include "imfcscalibrationdialog.h"
#include "qfpluginservices.h"
#include "programoptions.h"
#include "ui_imfcscalibrationdialog.h"

ImFCSCalibrationDialog::ImFCSCalibrationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImFCSCalibrationDialog)
{
    ui->setupUi(this);
    if (ProgramOptions::getInstance()) {
        ui->edtValues->setPlainText(ProgramOptions::getInstance()->getQSettings()->value("ImFCSCalibrationDialog/values", ui->edtValues->toPlainText()).toString());
    }
}

ImFCSCalibrationDialog::~ImFCSCalibrationDialog()
{
    delete ui;
}

void ImFCSCalibrationDialog::closeEvent(QCloseEvent *event) {
    if (ProgramOptions::getInstance()) {
        ProgramOptions::getInstance()->getQSettings()->setValue("ImFCSCalibrationDialog/values", ui->edtValues->toPlainText());

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
