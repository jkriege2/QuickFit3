#include "qfespimb040acquisitionconfigwidget.h"
#include "ui_qfespimb040acquisitionconfigwidget.h"
#include "qfespimb040mainwindow.h"
#include "qfpluginservices.h"
#include "qfstagecombobox.h"
#include "qfextensionmanager.h"
#include "../interfaces/qfextensionlinearstage.h"
#include "qfextension.h"
#include <QtGui>
#include <QtCore>


QFESPIMB040AcquisitionConfigWidget::QFESPIMB040AcquisitionConfigWidget(QWidget* parent, QFPluginServices* pluginServices) :
    QWidget(parent),
    ui(new Ui::QFESPIMB040AcquisitionConfigWidget)
{
    m_pluginServices=pluginServices;
    ui->setupUi(this);
}

QFESPIMB040AcquisitionConfigWidget::~QFESPIMB040AcquisitionConfigWidget()
{
    delete ui;
}


void QFESPIMB040AcquisitionConfigWidget::loadSettings(QSettings& settings, QString prefix) {
    ui->chkOverview->setChecked(settings.value(prefix+"overview", true).toBool());
    ui->chkUse1->setChecked(settings.value(prefix+"use1", true).toBool());
    ui->chkUse2->setChecked(settings.value(prefix+"use2", true).toBool());
    ui->edtPrefix1->setText(settings.value(prefix+"prefix1", "stack_cam1_%counter%").toString());
    ui->edtPrefix2->setText(settings.value(prefix+"prefix2", "stack_cam2_%counter%").toString());
    on_chkUse1_clicked(true);
}


void QFESPIMB040AcquisitionConfigWidget::storeSettings(QSettings& settings, QString prefix) const {
    settings.setValue(prefix+"use1", ui->chkUse1->isChecked());
    settings.setValue(prefix+"use2", ui->chkUse2->isChecked());
    settings.setValue(prefix+"prefix1", ui->edtPrefix1->text());
    settings.setValue(prefix+"prefix2", ui->edtPrefix2->text());
    settings.setValue(prefix+"overview", ui->chkOverview->isChecked());
}



int QFESPIMB040AcquisitionConfigWidget::counter() const {
    return ui->spinCounter->value();
}

void QFESPIMB040AcquisitionConfigWidget::incCounter() {
    ui->spinCounter->setValue(ui->spinCounter->value()+1);
}

QString QFESPIMB040AcquisitionConfigWidget::prefix1() const {
    QString filename= ui->edtPrefix1->text();
    filename=filename.replace("%counter%", QString::number(counter()));
    return filename;
}

QString QFESPIMB040AcquisitionConfigWidget::prefix2() const {
    QString filename= ui->edtPrefix2->text();
    filename=filename.replace("%counter%", QString::number(counter()));
    return filename;
}

bool QFESPIMB040AcquisitionConfigWidget::use1() const {
    return ui->chkUse1->isChecked();
}

bool QFESPIMB040AcquisitionConfigWidget::use2() const {
    return ui->chkUse2->isChecked();
}

bool QFESPIMB040AcquisitionConfigWidget::overview() const {
    return ui->chkOverview->isChecked();
}

void QFESPIMB040AcquisitionConfigWidget::on_btnAcquire_clicked() {
    emit doAcquisition();
}

void QFESPIMB040AcquisitionConfigWidget::on_chkUse1_clicked(bool enabled) {
    ui->btnAcquire->setEnabled(ui->chkUse1->isChecked() || ui->chkUse2->isChecked());
    ui->chkOverview->setEnabled(ui->chkUse1->isChecked() || ui->chkUse2->isChecked());
    ui->edtPrefix1->setEnabled(ui->chkUse1->isChecked());
}

void QFESPIMB040AcquisitionConfigWidget::on_chkUse2_clicked(bool enabled) {
    ui->btnAcquire->setEnabled(ui->chkUse1->isChecked() || ui->chkUse2->isChecked());
    ui->chkOverview->setEnabled(ui->chkUse1->isChecked() || ui->chkUse2->isChecked());
    ui->edtPrefix2->setEnabled(ui->chkUse2->isChecked());
}
