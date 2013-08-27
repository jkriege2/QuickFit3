#include "qffitalgorithmgslderivconfigdialog.h"
#include "ui_qffitalgorithmgslderivconfigdialog.h"

QFFitAlgorithmGSLDerivativeBaseConfigDialog::QFFitAlgorithmGSLDerivativeBaseConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFFitAlgorithmGSLDerivativeBaseConfigDialog)
{
    ui->setupUi(this);
}

QFFitAlgorithmGSLDerivativeBaseConfigDialog::~QFFitAlgorithmGSLDerivativeBaseConfigDialog()
{
    delete ui;
}

void QFFitAlgorithmGSLDerivativeBaseConfigDialog::init(const QFFitAlgorithm *algorithm)
{
    ui->spinStepsize->setValue(algorithm->getParameter("stepsize").toDouble());
    ui->spinMaxIter->setValue(algorithm->getParameter("max_iterations").toInt());
    ui->spinTolerance->setValue(algorithm->getParameter("tol").toDouble());
    ui->spinGTolerance->setValue(algorithm->getParameter("gtol").toDouble());
}

void QFFitAlgorithmGSLDerivativeBaseConfigDialog::storeParams(QFFitAlgorithm *algorithm) const
{
    algorithm->setParameter("stepsize", ui->spinStepsize->value());
    algorithm->setParameter("max_iterations", ui->spinMaxIter->value());
    algorithm->setParameter("tol", ui->spinTolerance->value());
    algorithm->setParameter("gtol", ui->spinGTolerance->value());
}
