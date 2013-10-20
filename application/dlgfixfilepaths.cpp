#include "dlgfixfilepaths.h"
#include "ui_dlgfixfilepaths.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include "qfpluginservices.h"

DlgFixFilepaths::DlgFixFilepaths(const QString& oldName, const QString& lastDir, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgFixFilepaths)
{
    ui->setupUi(this);
    init(oldName, lastDir);
}

DlgFixFilepaths::~DlgFixFilepaths()
{
    delete ui;
}

void DlgFixFilepaths::selectFile()
{
    QString fn=QFileInfo(ui->edtOldname->text()).fileName();
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select File"), lastDir, tr("File (%1)").arg(fn));
    if (!fileName.isEmpty()) {
        ui->edtNewname->setText(fileName);
    }
}

void DlgFixFilepaths::on_btnApply_clicked() {
    bool ok=true;
    if (!QFile::exists(getNewFilename())) {
        ok=QMessageBox::warning(this, tr("Error"), tr("You selected a non-existent file! Replace directory anyways?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::Yes;
    }
    if (ok) {
        accept();
    }
}

void DlgFixFilepaths::on_btnIgnore_clicked() {
    done(100);
}

void DlgFixFilepaths::on_btnCancel_clicked() {
    reject();
}

QString DlgFixFilepaths::getNewFilename() const
{
    return ui->edtNewname->text();
}

QString DlgFixFilepaths::getLastDir() const
{
    return lastDir;
}

void DlgFixFilepaths::init(const QString &oldName, const QString &lastDir)
{
    ui->edtOldname->setText(oldName);
    ui->labFilename->setText(QFileInfo(oldName).fileName());
    ui->edtNewname->setText("");
    if (lastDir.isEmpty()) {
        this->lastDir=QFileInfo(oldName).absolutePath();
    } else {
        this->lastDir=lastDir;
    }

}

void DlgFixFilepaths::showHelp()
{
    QFPluginServices::getInstance()->displayMainHelpWindow("dlgfixfilepaths.html");

}
