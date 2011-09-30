#include "qfradhard2flashtool.h"
#include "ui_qfradhard2flashtool.h"
#include "cam_radhard2.h"

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
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Bitfile ..."), QFileInfo(ui->edtFile->text()).absolutePath(), tr("FPGA Bitfiles (*.bit)"));
    if (!fileName.isEmpty()) {
        ui->edtFile->setText(fileName);
    }
}

void QFRadhard2Flashtool::on_btnLoad1_clicked(){
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Bitfile ..."), QFileInfo(ui->edtAutoBitfile->text()).absolutePath(), tr("FPGA Bitfiles (*.bit)"));
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
