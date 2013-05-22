#include "qfrdrtablecurvefitdialog.h"
#include "ui_qfrdrtablecurvefitdialog.h"

QFRDRTableCurveFitDialog::QFRDRTableCurveFitDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFRDRTableCurveFitDialog)
{
    ui->setupUi(this);
}

QFRDRTableCurveFitDialog::~QFRDRTableCurveFitDialog()
{
    delete ui;
}
