#include "qfrdrimagingfcssimulator.h"
#include "ui_qfrdrimagingfcssimulator.h"

QFRDRImagingFCSSimulator::QFRDRImagingFCSSimulator(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFRDRImagingFCSSimulator)
{
    ui->setupUi(this);
}

QFRDRImagingFCSSimulator::~QFRDRImagingFCSSimulator()
{
    delete ui;
}

QString QFRDRImagingFCSSimulator::getSimulationFilename() const
{
    return ui->edtFilename->text();
}
