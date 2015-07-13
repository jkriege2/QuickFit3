#include "qfrdrimagingfcspixelfromobjective.h"
#include "ui_qfrdrimagingfcspixelfromobjective.h"
#include "programoptions.h"

QFRDRImagingFCSPixelFromObjective::QFRDRImagingFCSPixelFromObjective(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFRDRImagingFCSPixelFromObjective)
{
    ui->setupUi(this);
    ProgramOptions::getConfigQDoubleSpinBox(ui->spinMagnification, tr("imaging_fcs/QFRDRImagingFCSPixelFromObjective/spinMagnification"));
    ProgramOptions::getConfigQDoubleSpinBox(ui->spinPixelSize, tr("imaging_fcs/QFRDRImagingFCSPixelFromObjective/spinPixelSize"));
    ProgramOptions::getConfigQDoubleSpinBox(ui->spinTubeLens, tr("imaging_fcs/QFRDRImagingFCSPixelFromObjective/spinTubeLens"));
}

QFRDRImagingFCSPixelFromObjective::~QFRDRImagingFCSPixelFromObjective()
{
    ProgramOptions::setConfigQDoubleSpinBox(ui->spinMagnification, tr("imaging_fcs/QFRDRImagingFCSPixelFromObjective/spinMagnification"));
    ProgramOptions::setConfigQDoubleSpinBox(ui->spinPixelSize, tr("imaging_fcs/QFRDRImagingFCSPixelFromObjective/spinPixelSize"));
    ProgramOptions::setConfigQDoubleSpinBox(ui->spinTubeLens, tr("imaging_fcs/QFRDRImagingFCSPixelFromObjective/spinTubeLens"));
    delete ui;
}

double QFRDRImagingFCSPixelFromObjective::getPixelSize() const
{
    return ui->spinFinalPixelSize->value();
}

void QFRDRImagingFCSPixelFromObjective::updateM()
{
    ui->spinFinalPixelSize->setValue(ui->spinPixelSize->value()/(ui->spinMagnification->value()*ui->spinTubeLens->value()));
}
