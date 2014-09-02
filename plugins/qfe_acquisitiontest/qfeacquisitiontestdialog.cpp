/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
