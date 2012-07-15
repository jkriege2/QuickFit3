#include "qfeacquisitiontestdialog.h"
#include "ui_qfeacquisitiontestdialog.h"
#include "qfpluginservices.h"
#include "programoptions.h"

QFEAcquisitiontestDialog::QFEAcquisitiontestDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QFEAcquisitiontestDialog)
{
    ui->setupUi(this);
    ui->log->set_log_file_append(true);
    ui->log->set_log_date_time(true);
    ui->log->open_logfile(ProgramOptions::getInstance()->getConfigFileDirectory()+"/acquisitiontest.log", false);

    ui->cmbDevice->init(QFPluginServices::getInstance()->getExtensionManager());
    ui->widAcquisitionConfig->init(ProgramOptions::getInstance()->getConfigFileDirectory());
    ui->widAcquisitionConfig->setEnabled(false);
    ui->widAcquisitionConfig->connectTo(ui->cmbDevice);
    ui->btnConnect->setCheckable(true);
}

QFEAcquisitiontestDialog::~QFEAcquisitiontestDialog() {
    delete ui;
}

void QFEAcquisitiontestDialog::log_text(QString message) {
    ui->log->log_text(message);
}

void QFEAcquisitiontestDialog::log_warning(QString message) {
    ui->log->log_warning(message);
}

void QFEAcquisitiontestDialog::log_error(QString message) {
    ui->log->log_error(message);
}

void QFEAcquisitiontestDialog::on_btnConnect_clicked(bool pressed) {
    if (pressed) {
        if (ui->cmbDevice->currentExtension()!=NULL) {
            if (ui->cmbDevice->currentExtensionAcquisition()->connectAcquisitionDevice(ui->cmbDevice->currentAcquisitionID())) {
                ui->cmbDevice->currentExtensionAcquisition()->setLogging(this);
                ui->cmbDevice->setEnabled(false);
                ui->widAcquisitionConfig->setEnabled(true);
            } else {
                ui->btnConnect->setChecked(false);
            }
        } else {
            ui->btnConnect->setChecked(false);
        }
    } else {
        if (ui->cmbDevice->currentExtension()!=NULL) {
            ui->cmbDevice->currentExtensionAcquisition()->disconnectAcquisitionDevice(ui->cmbDevice->currentAcquisitionID());
            ui->cmbDevice->currentExtensionAcquisition()->setLogging(NULL);
        }
        ui->widAcquisitionConfig->setEnabled(false);
    }
    ui->widAcquisitionConfig->setEnabled(pressed);
}
