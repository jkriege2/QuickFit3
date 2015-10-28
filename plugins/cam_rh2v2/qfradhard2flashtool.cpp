/*
Copyright (c) 2008-2015 Jan Buchholz, German Cancer Research Center

    

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

#include "qfradhard2flashtool.h"
#include "ui_qfradhard2flashtool.h"
#include "cam_rh2v2.h"
#include "qftools.h"

QFRadhard2FlashtoolV2::QFRadhard2FlashtoolV2(QFExtensionCameraRh2v2 *ext, QWidget* parent) :
    QFDialog(parent),
    ui(new Ui::QFRadhard2FlashtoolV2)
{
    this->radhard2extension=ext;
    ui->setupUi(this);
}

QFRadhard2FlashtoolV2::~QFRadhard2FlashtoolV2()
{
    delete ui;
}


void QFRadhard2FlashtoolV2::on_btnFlash_clicked(){
    if (!radhard2extension) return;
    if (!bitfileMaster().isEmpty() && !QFile::exists(bitfileMaster())) {
        QMessageBox::critical(this, tr("Radhard2 driver"), tr("Master bitfile '%1' does not exist!").arg(bitfileMaster()));
        return;
    }
    if (!bitfileSlave().isEmpty() && !QFile::exists(bitfileSlave())) {
        QMessageBox::critical(this, tr("Radhard2 driver"), tr("Slave bitfile '%1' does not exist!").arg(bitfileSlave()));
        return;
    }

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QString message;
    ui->edtSuccess->setText(tr("flashing master FPGA: '%1' ...\nflashing slave FPGA: '%2' ...").arg(bitfileMaster()).arg(bitfileSlave()));
    QApplication::processEvents();
    bool ok = true;
    if (!bitfileMaster().isEmpty()) ok = ok && radhard2extension->flashFPGA(bitfileMaster(), 'm', message, ui->spinRetries->value(), ui->spinRetryDelay->value());
    if (!bitfileSlave().isEmpty()) ok = ok && radhard2extension->flashFPGA(bitfileSlave(), 's', message, ui->spinRetries->value(), ui->spinRetryDelay->value());
    ui->edtSuccess->setText(message);
    ui->edtSuccess->moveCursor(QTextCursor::End);
    QApplication::restoreOverrideCursor();
    if (!ok) {
        QMessageBox::critical(this, tr("Radhard2 driver"), tr("Could not program Radhard2 FPGA, see dialog for error message!"));
        return;
    }
}

void QFRadhard2FlashtoolV2::on_btnLoad_clicked(){
    QString fileName = qfGetOpenFileName(this, tr("Select Bitfile ..."), QFileInfo(ui->edtMasterBitfile->text()).absolutePath(), tr("FPGA Bitfiles (*.bit)"));
    if (!fileName.isEmpty()) {
        ui->edtMasterBitfile->setText(fileName);
    }
}

void QFRadhard2FlashtoolV2::on_btnLoad1_clicked(){
    QString fileName = qfGetOpenFileName(this, tr("Select Bitfile ..."), QFileInfo(ui->edtAutoBitfileMaster->text()).absolutePath(), tr("FPGA Bitfiles (*.bit)"));
    if (!fileName.isEmpty()) {
        ui->edtAutoBitfileMaster->setText(fileName);
    }
}


void QFRadhard2FlashtoolV2::on_btnLoad2_clicked(){
    QString fileName = qfGetOpenFileName(this, tr("Select Bitfile ..."), QFileInfo(ui->edtAutoBitfileSlave->text()).absolutePath(), tr("FPGA Bitfiles (*.bit)"));
    if (!fileName.isEmpty()) {
        ui->edtAutoBitfileSlave->setText(fileName);
    }
}

void QFRadhard2FlashtoolV2::on_btnLoad3_clicked(){
    QString fileName = qfGetOpenFileName(this, tr("Select Bitfile ..."), QFileInfo(ui->edtSlaveBitfile->text()).absolutePath(), tr("FPGA Bitfiles (*.bit)"));
    if (!fileName.isEmpty()) {
        ui->edtSlaveBitfile->setText(fileName);
    }
}

void QFRadhard2FlashtoolV2::setBitfileMaster(QString bitfile) {
    ui->edtMasterBitfile->setText(bitfile);
}

void QFRadhard2FlashtoolV2::setAutoBitfileMaster(QString bitfile) {
    ui->edtAutoBitfileMaster->setText(bitfile);
}

void QFRadhard2FlashtoolV2::setBitfileSlave(QString bitfile) {
    ui->edtSlaveBitfile->setText(bitfile);
}

void QFRadhard2FlashtoolV2::setAutoBitfileSlave(QString bitfile) {
    ui->edtAutoBitfileSlave->setText(bitfile);
}

void QFRadhard2FlashtoolV2::setAutoFlash(bool v) {
    ui->chkAutoload->setChecked(v);
}

void QFRadhard2FlashtoolV2::setRetries(int v) {
    ui->spinRetries->setValue(v);
}

int QFRadhard2FlashtoolV2::retries() const {
    return ui->spinRetries->value();
}

bool QFRadhard2FlashtoolV2::autoflash() const {
    return ui->chkAutoload->isChecked();
}

QString QFRadhard2FlashtoolV2::bitfileMaster() const {
    return ui->edtMasterBitfile->text();
}

QString QFRadhard2FlashtoolV2::autoBitfileMaster() const {
    return ui->edtAutoBitfileMaster->text();
}

QString QFRadhard2FlashtoolV2::bitfileSlave() const {
    return ui->edtSlaveBitfile->text();
}

QString QFRadhard2FlashtoolV2::autoBitfileSlave() const {
    return ui->edtAutoBitfileSlave->text();
}

void QFRadhard2FlashtoolV2::setRetryDelayMS(int milliseconds) {
    ui->spinRetryDelay->setValue(milliseconds);
}

int QFRadhard2FlashtoolV2::retryDelayMS() const {
    return ui->spinRetryDelay->value();
}
