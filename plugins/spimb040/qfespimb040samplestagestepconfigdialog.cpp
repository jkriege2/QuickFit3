#include "qfespimb040samplestagestepconfigdialog.h"
#include "ui_qfespimb040samplestagestepconfigdialog.h"

QFESPIMB040SampleStageStepConfigDialog::QFESPIMB040SampleStageStepConfigDialog(double x, double y, double z, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFESPIMB040SampleStageStepConfigDialog)
{
    ui->setupUi(this);
    ui->spinX->setValue(x);
    ui->spinY->setValue(y);
    ui->spinZ->setValue(z);
}

QFESPIMB040SampleStageStepConfigDialog::~QFESPIMB040SampleStageStepConfigDialog()
{
    delete ui;
}

double QFESPIMB040SampleStageStepConfigDialog::x() const {
    return ui->spinX->value();
}

double QFESPIMB040SampleStageStepConfigDialog::y() const {
    return ui->spinY->value();
}

double QFESPIMB040SampleStageStepConfigDialog::z() const {
    return ui->spinZ->value();
}
