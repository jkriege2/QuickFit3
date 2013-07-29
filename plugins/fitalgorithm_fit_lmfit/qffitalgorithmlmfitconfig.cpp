#include "qffitalgorithmlmfitconfig.h"
#include "ui_qffitalgorithmlmfitconfig.h"

QFFitAlgorithmLMFitConfigDialog::QFFitAlgorithmLMFitConfigDialog(QWidget *parent, bool IRLS) :
    QDialog(parent),
    ui(new Ui::QFFitAlgorithmLMFitConfigDialog)
{
    ui->setupUi(this);
    ui->grpIRLS->setVisible(IRLS);
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
    ui->grpIRLS->setVisible(false);
}

void QFFitAlgorithmLMFitConfigDialog::init(double ftol, double xtol, double gtol, double epsilon, double stepbound, int max_iterations, double irls_parameter, int irls_iterations) {
    init(ftol, xtol, gtol, epsilon, stepbound, max_iterations);
    ui->spinIRLSIterations->setValue(irls_iterations);
    ui->spinIRLSParameter->setValue(irls_parameter);
    ui->grpIRLS->setVisible(true);
}

double QFFitAlgorithmLMFitConfigDialog::irls_parameter() const {
    return ui->spinIRLSParameter->value();
}

int QFFitAlgorithmLMFitConfigDialog::irls_iterations() const {
    return ui->spinIRLSIterations->value();
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
