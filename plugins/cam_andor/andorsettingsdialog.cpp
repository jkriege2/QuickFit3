#include "andorsettingsdialog.h"
#include "ui_andorsettingsdialog.h"

AndorSettingsDialog::AndorSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AndorSettingsDialog)
{
    ui->setupUi(this);
}

AndorSettingsDialog::~AndorSettingsDialog()
{
    delete ui;
}

void AndorSettingsDialog::readSettings(const QSettings& settings, const QString& prefix) {

}

void AndorSettingsDialog::writeSettings(QSettings& settings, const QString& prefix) const {

}
