#include "fitalgorithm_inst_a1config.h"
#include "ui_fitalgorithm_inst_a1config.h"


QFFitAlgorithmInst_A1ConfigDialog:: QFFitAlgorithmInst_A1ConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui:: QFFitAlgorithmInst_A1ConfigDialog)
{
    ui->setupUi(this);
}

 QFFitAlgorithmInst_A1ConfigDialog::~ QFFitAlgorithmInst_A1ConfigDialog()
{
    delete ui;
}

void  QFFitAlgorithmInst_A1ConfigDialog::init(const QFFitAlgorithm *algorithm)
{
    ui->spinParam1->setValue(algorithm->getParameter("param").toDouble());
}

void  QFFitAlgorithmInst_A1ConfigDialog::storeParams(QFFitAlgorithm *algorithm) const
{
    algorithm->setParameter("param", ui->spinParam1->value());
}

