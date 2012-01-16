#include "qfrdrimagingfcsdataexplorer.h"
#include "ui_qfrdrimagingfcsdataexplorer.h"

QFRDRImagingFCSDataExplorer::QFRDRImagingFCSDataExplorer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFRDRImagingFCSDataExplorer)
{
    ui->setupUi(this);
}

QFRDRImagingFCSDataExplorer::~QFRDRImagingFCSDataExplorer()
{
    delete ui;
}

void QFRDRImagingFCSDataExplorer::setBleachDecay(double frames) {
    ui->spinDecay->setValue(frames);
}

double QFRDRImagingFCSDataExplorer::getBleachDecay() const {
    return ui->spinDecay->value();
}
