/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

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

#include "qfradhard2flashtool.h"
#include "ui_qfradhard2flashtool.h"
#include "cam_radhard2.h"
#include "qftools.h"

QFRadhard2Flashtool::QFRadhard2Flashtool(QFExtensionCameraRadhard2 *ext, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::QFRadhard2Flashtool)
{
    this->radhard2extension=ext;
    ui->setupUi(this);
}

QFRadhard2Flashtool::~QFRadhard2Flashtool()
{
    delete ui;
}


void QFRadhard2Flashtool::on_btnFlash_clicked(){
    if (!radhard2extension) return;
    if (!QFile::exists(ui->edtFile->text())) {
        QMessageBox::critical(this, tr("Radhard2 driver"), tr("Bitfile '%1' does not exist!").arg(ui->edtFile->text()));
        return;
    }
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QString message;
    ui->edtSuccess->setText(tr("flashing master FPGA: '%1' ...").arg(ui->edtFile->text()));
    QApplication::processEvents();
    bool ok=radhard2extension->flashFPGA(ui->edtFile->text(), 'm', message, ui->spinRetries->value(), ui->spinRetryDelay->value());
    ui->edtSuccess->setText(message);
    ui->edtSuccess->moveCursor(QTextCursor::End);
    if (!ok) {
        QMessageBox::critical(this, tr("Radhard2 driver"), tr("Could not program Radhard2 FPGA, see dialog for error message!"));
    }
    QApplication::restoreOverrideCursor();
}

void QFRadhard2Flashtool::on_btnLoad_clicked(){
    QString fileName = qfGetOpenFileName(this, tr("Select Bitfile ..."), QFileInfo(ui->edtFile->text()).absolutePath(), tr("FPGA Bitfiles (*.bit)"));
    if (!fileName.isEmpty()) {
        ui->edtFile->setText(fileName);
    }
}

void QFRadhard2Flashtool::on_btnLoad1_clicked(){
    QString fileName = qfGetOpenFileName(this, tr("Select Bitfile ..."), QFileInfo(ui->edtAutoBitfile->text()).absolutePath(), tr("FPGA Bitfiles (*.bit)"));
    if (!fileName.isEmpty()) {
        ui->edtAutoBitfile->setText(fileName);
    }
}

void QFRadhard2Flashtool::setBitfile(QString bitfile) {
    ui->edtFile->setText(bitfile);
}

void QFRadhard2Flashtool::setAutoBitfile(QString bitfile) {
    ui->edtAutoBitfile->setText(bitfile);
}

void QFRadhard2Flashtool::setAutoFlash(bool v) {
    ui->chkAutoload->setChecked(v);
}

void QFRadhard2Flashtool::setRetries(int v) {
    ui->spinRetries->setValue(v);
}

int QFRadhard2Flashtool::retries() const {
    return ui->spinRetries->value();
}

bool QFRadhard2Flashtool::autoflash() const {
    return ui->chkAutoload->isChecked();
}

QString QFRadhard2Flashtool::bitfile() const {
    return ui->edtFile->text();
}

QString QFRadhard2Flashtool::autoBitfile() const {
    return ui->edtAutoBitfile->text();
}

void QFRadhard2Flashtool::setRetryDelayMS(int milliseconds) {
    ui->spinRetryDelay->setValue(milliseconds);
}

int QFRadhard2Flashtool::retryDelayMS() const {
    return ui->spinRetryDelay->value();
}
