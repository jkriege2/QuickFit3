#include "qfespimb040imagestackconfigwidget.h"
#include "ui_qfespimb040imagestackconfigwidget.h"

#include "qfespimb040mainwindow.h"
#include "qfpluginservices.h"
#include "qfstagecombobox.h"
#include "qfextensionmanager.h"
#include "../interfaces/qfextensionlinearstage.h"
#include "qfextension.h"
#include <QtGui>
#include <QtCore>

#define STAGE_INTERVAL_MS 100

QFESPIMB040ImageStackConfigWidget::QFESPIMB040ImageStackConfigWidget(QFESPIMB040MainWindow* parent, QFExtensionServices* pluginServices) :
    QWidget(parent),
    ui(new Ui::QFESPIMB040ImageStackConfigWidget)
{
    m_parent=parent;
    m_pluginServices=pluginServices;
    ui->setupUi(this);
    ui->cmbStage->init(pluginServices->getExtensionManager());

    QTimer::singleShot(STAGE_INTERVAL_MS, this, SLOT(checkStage()));
}

QFESPIMB040ImageStackConfigWidget::~QFESPIMB040ImageStackConfigWidget()
{
    delete ui;
}


void QFESPIMB040ImageStackConfigWidget::loadSettings(QSettings& settings, QString prefix) {
    ui->cmbStage->setCurrentIndex(settings.value(prefix+"stage", 0).toInt());
    ui->chkUse1->setChecked(settings.value(prefix+"use1", true).toBool());
    ui->chkUse2->setChecked(settings.value(prefix+"use2", true).toBool());
    ui->edtPrefix1->setText(settings.value(prefix+"prefix1", "stack_cam1_%counter%").toString());
    ui->edtPrefix2->setText(settings.value(prefix+"prefix2", "stack_cam2_%counter%").toString());
    ui->spinStart->setValue(settings.value(prefix+"start", 0).toDouble());
    ui->spinDelta->setValue(settings.value(prefix+"delta", 0).toDouble());
    ui->spinSteps->setValue(settings.value(prefix+"steps", 0).toInt());
}


void QFESPIMB040ImageStackConfigWidget::storeSettings(QSettings& settings, QString prefix) const {
    settings.setValue(prefix+"stage", ui->cmbStage->currentIndex());
    settings.setValue(prefix+"use1", ui->chkUse1->isChecked());
    settings.setValue(prefix+"use2", ui->chkUse2->isChecked());
    settings.setValue(prefix+"prefix1", ui->edtPrefix1->text());
    settings.setValue(prefix+"prefix2", ui->edtPrefix2->text());
    settings.setValue(prefix+"start", ui->spinStart->value());
    settings.setValue(prefix+"delta", ui->spinDelta->value());
    settings.setValue(prefix+"steps", ui->spinSteps->value());
}



int QFESPIMB040ImageStackConfigWidget::counter() const {
    return ui->spinCounter->value();
}

void QFESPIMB040ImageStackConfigWidget::incCounter() {
    ui->spinCounter->setValue(ui->spinCounter->value()+1);
}

QString QFESPIMB040ImageStackConfigWidget::prefix1() const {
    QString filename= ui->edtPrefix1->text();
    filename=filename.replace("%counter%", QString::number(counter()));
    return filename;
}

QString QFESPIMB040ImageStackConfigWidget::prefix2() const {
    QString filename= ui->edtPrefix2->text();
    filename=filename.replace("%counter%", QString::number(counter()));
    return filename;
}

bool QFESPIMB040ImageStackConfigWidget::use1() const {
    return ui->chkUse1->isChecked();
}

bool QFESPIMB040ImageStackConfigWidget::use2() const {
    return ui->chkUse2->isChecked();
}

int QFESPIMB040ImageStackConfigWidget::currentAxisID() const {
    return ui->cmbStage->currentAxisID();
}

QFExtensionLinearStage* QFESPIMB040ImageStackConfigWidget::stage() const {
    return ui->cmbStage->currentExtensionLinearStage();
}

QFExtension* QFESPIMB040ImageStackConfigWidget::stageExtension() const {
    return ui->cmbStage->currentExtension();
}

int QFESPIMB040ImageStackConfigWidget::stackCount() const {
    return ui->spinSteps->value();
}

double QFESPIMB040ImageStackConfigWidget::stackStart() const {
    return ui->spinStart->value();
}

double QFESPIMB040ImageStackConfigWidget::stackDelta() const {
    return ui->spinDelta->value();
}

void QFESPIMB040ImageStackConfigWidget::on_btnAcquire_clicked() {
    emit doStack();
}

void QFESPIMB040ImageStackConfigWidget::on_btnGetCurrent_clicked() {
    if (stage()!=NULL) {
        if (stage()->isConnected(currentAxisID())) {
            ui->spinStart->setValue(stage()->getPosition(currentAxisID()));
        }
    }
}

void QFESPIMB040ImageStackConfigWidget::on_spinStart_valueChanged(double value) {
    updateLabel();
}

void QFESPIMB040ImageStackConfigWidget::on_spinDelta_valueChanged(double value) {
    updateLabel();
}

void QFESPIMB040ImageStackConfigWidget::on_spinSteps_valueChanged(int value) {
    updateLabel();
}

void QFESPIMB040ImageStackConfigWidget::updateLabel() {
    double start=stackStart();
    double delta=stackDelta();
    int steps=stackCount();
    double end=start+delta*(double)steps;
    ui->labStage->setText(tr("scan end = %1 microns     scan length = %2 microns").arg(end).arg(end-start));
}

void QFESPIMB040ImageStackConfigWidget::on_btnConfig_clicked() {

}

void QFESPIMB040ImageStackConfigWidget::on_btnConnect_clicked() {
    if (stage()) {
        if (stage()->isConnected(currentAxisID())) {
            stage()->disconnectDevice(currentAxisID());
        } else {
            stage()->connectDevice(currentAxisID());
        }
    }
}

void QFESPIMB040ImageStackConfigWidget::checkStage() {
    bool conn=false;
    if (stage()) {
        conn=stage()->isConnected(currentAxisID());
        if (conn) {
            ui->btnConnect->setChecked(true);
            ui->btnConnect->setIcon(QIcon(":/spimb040/stagedisconnect.png"));
            ui->btnConnect->setToolTip(tr("Disconnect from axis ..."));
        } else {
            ui->btnConnect->setChecked(false);
            ui->btnConnect->setIcon(QIcon(":/spimb040/stageconnect.png"));
            ui->btnConnect->setToolTip(tr("Connect to axis ..."));
        }
    }
    ui->btnConfig->setEnabled(true);

    QTimer::singleShot(STAGE_INTERVAL_MS, this, SLOT(checkStage()));
}


