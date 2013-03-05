#include "qfrdrimagingfcssimulator.h"
#include "ui_qfrdrimagingfcssimulator.h"
#include "qfpluginservices.h"
#include "programoptions.h"

QFRDRImagingFCSSimulator::QFRDRImagingFCSSimulator(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFRDRImagingFCSSimulator)
{
    ui->setupUi(this);
    readSettings();
}

QFRDRImagingFCSSimulator::~QFRDRImagingFCSSimulator()
{
    writeSettings();
    delete ui;
}

QString QFRDRImagingFCSSimulator::getSimulationFilename() const
{
    return ui->edtFilename->text();
}

void QFRDRImagingFCSSimulator::on_btnHelp_clicked()
{
    QFPluginServices::getInstance()->displayHelpWindow(QFPluginServices::getInstance()->getPluginHelpDirectory("imaging_fcs")+"/simulation.html");
}

void QFRDRImagingFCSSimulator::writeSettings() const {
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/D", ui->spinD->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/height", ui->spinHeight->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/width", ui->spinWidth->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/pixel_size", ui->spinPixelSize->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/walkers", ui->spinWalkers->value());
}

void QFRDRImagingFCSSimulator::readSettings()
{
    ui->spinD->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/D", 5).toDouble());
    ui->spinHeight->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/height", 10).toInt());
    ui->spinWidth->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/width", 10).toInt());
    ui->spinPixelSize->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/pixel_size", 400).toDouble());
    ui->spinWalkers->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/walkers", 20).toInt());
}
