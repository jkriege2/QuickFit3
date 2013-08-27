#include "qffitalgorithmgslsimplexconfigdialog.h"
#include "ui_qffitalgorithmgslsimplexconfigdialog.h"

QFFitAlgorithmGSLSimplexConfigDialog::QFFitAlgorithmGSLSimplexConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFFitAlgorithmGSLSimplexConfigDialog)
{
    ui->setupUi(this);
}

QFFitAlgorithmGSLSimplexConfigDialog::~QFFitAlgorithmGSLSimplexConfigDialog()
{
    delete ui;
}

void QFFitAlgorithmGSLSimplexConfigDialog::init(const QFFitAlgorithm *algorithm)
{
    ui->spinStepsize->setValue(algorithm->getParameter("stepsize").toDouble());
    ui->spinMaxIter->setValue(algorithm->getParameter("max_iterations").toInt());
}

void QFFitAlgorithmGSLSimplexConfigDialog::storeParams(QFFitAlgorithm *algorithm) const
{
    algorithm->setParameter("stepsize", ui->spinStepsize->value());
    algorithm->setParameter("max_iterations", ui->spinMaxIter->value());
}
