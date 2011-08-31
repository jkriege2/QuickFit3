#include "qffitalgorithmlmfitconfig.h"
#include "ui_qffitalgorithmlmfitconfig.h"

QFFitAlgorithmLMFitConfigDialog::QFFitAlgorithmLMFitConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFFitAlgorithmLMFitConfigDialog)
{
    ui->setupUi(this);
}

QFFitAlgorithmLMFitConfigDialog::~QFFitAlgorithmLMFitConfigDialog()
{
    delete ui;
}

void QFFitAlgorithmLMFitConfigDialog::init(double ftol, double xtol, double gtol, double epsilon, double stepbound, int max_iterations) {
    ui->spinEpsilon->setValue(epsilon);
    ui->spinFTol->setValue(ftol);
    ui->spinGTol->setValue(gtol);
    ui->spinIterations->setValue(max_iterations);
    ui->spinStepbound->setValue(stepbound);
    ui->spinXTol->setValue(xtol);
}

double QFFitAlgorithmLMFitConfigDialog::ftol() const {
    return ui->spinFTol->value();
}

double QFFitAlgorithmLMFitConfigDialog::xtol() const {
    return ui->spinXTol->value();
}

double QFFitAlgorithmLMFitConfigDialog::gtol() const {
    return ui->spinGTol->value();
}

double QFFitAlgorithmLMFitConfigDialog::epsilon() const {
    return ui->spinEpsilon->value();
}

double QFFitAlgorithmLMFitConfigDialog::stepbound() const {
    return ui->spinStepbound->value();
}

int QFFitAlgorithmLMFitConfigDialog::max_iterations() const {
    return ui->spinIterations->value();
}
