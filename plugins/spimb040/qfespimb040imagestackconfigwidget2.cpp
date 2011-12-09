#include "qfespimb040imagestackconfigwidget2.h"
#include "ui_qfespimb040imagestackconfigwidget2.h"

#include "qfespimb040mainwindow.h"
#include "qfpluginservices.h"
#include "qfstagecombobox.h"
#include "qfextensionmanager.h"
#include "../interfaces/qfextensionlinearstage.h"
#include "qfextension.h"
#include <QtGui>
#include <QtCore>
#include "qfespimb040opticssetup.h"

#define STAGE_INTERVAL_MS 1313

QFESPIMB040ImageStackConfigWidget2::QFESPIMB040ImageStackConfigWidget2(QWidget* parent, QFPluginServices* pluginServices, QFESPIMB040OpticsSetup* stageConfig, QString configDirectory) :
    QWidget(parent),
    ui(new Ui::QFESPIMB040ImageStackConfigWidget2)
{
    m_pluginServices=pluginServices;
    this->opticsSetup=stageConfig;
    ui->setupUi(this);
    ui->cmbStage->init(pluginServices->getExtensionManager());
    ui->cmbStage2->init(pluginServices->getExtensionManager());
    ui->cmbStage3->init(pluginServices->getExtensionManager());

    if (stageConfig) {
        ui->cmbCam1Settings->init(configDirectory);
        ui->cmbCam1Settings->setStopResume(opticsSetup->getStopRelease(0));
        ui->cmbCam1Settings->connectTo(opticsSetup->cameraComboBox(0));
        //qDebug()<<configDirectory<<opticsSetup->cameraComboBox(0)<<opticsSetup->cameraComboBox(1);

        ui->cmbCam2Settings->init(configDirectory);
        ui->cmbCam2Settings->setStopResume(opticsSetup->getStopRelease(1));
        ui->cmbCam2Settings->connectTo(opticsSetup->cameraComboBox(1));
    }

    QTimer::singleShot(STAGE_INTERVAL_MS, this, SLOT(checkStage()));
}

QFESPIMB040ImageStackConfigWidget2::~QFESPIMB040ImageStackConfigWidget2()
{
    delete ui;
}


void QFESPIMB040ImageStackConfigWidget2::loadSettings(QSettings& settings, QString prefix) {
    /*ui->cmbStage->setCurrentIndex(settings.value(prefix+"stage", 0).toInt());
    ui->cmbStage2->setCurrentIndex(settings.value(prefix+"stage2", 0).toInt());
    ui->cmbStage3->setCurrentIndex(settings.value(prefix+"stage3", 0).toInt());*/
    ui->cmbStage->loadSettings(settings, prefix+"stage/");
    ui->cmbStage2->loadSettings(settings, prefix+"stage2/");
    ui->cmbStage3->loadSettings(settings, prefix+"stage3/");

    ui->chkUse1->setChecked(settings.value(prefix+"use1", true).toBool());
    ui->chkUse2->setChecked(settings.value(prefix+"use2", true).toBool());
    ui->edtPrefix1->setText(settings.value(prefix+"prefix1", "stack_cam1_%counter%").toString());
    ui->edtPrefix2->setText(settings.value(prefix+"prefix2", "stack_cam2_%counter%").toString());

    ui->cmbCam1Settings->setCurrentConfig(settings.value(prefix+"acqsettings1", "default").toString());
    ui->cmbCam2Settings->setCurrentConfig(settings.value(prefix+"acqsettings2", "default").toString());

    ui->spinStart->setValue(settings.value(prefix+"start", 0).toDouble());
    ui->spinDelta->setValue(settings.value(prefix+"delta", 0).toDouble());
    ui->spinSteps->setValue(settings.value(prefix+"steps", 0).toInt());
    ui->spinDelay->setValue(settings.value(prefix+"delay", 15).toInt());

    ui->spinStart2->setValue(settings.value(prefix+"start2", 0).toDouble());
    ui->spinDelta2->setValue(settings.value(prefix+"delta2", 0).toDouble());
    ui->spinSteps2->setValue(settings.value(prefix+"steps2", 0).toInt());

    ui->spinStart3->setValue(settings.value(prefix+"start3", 0).toDouble());
    ui->spinDelta3->setValue(settings.value(prefix+"delta3", 0).toDouble());
    ui->spinSteps3->setValue(settings.value(prefix+"steps3", 0).toInt());

    ui->chkStage2->setChecked(settings.value(prefix+"stage2", false).toBool());
    ui->chkStage3->setChecked(settings.value(prefix+"stage3", false).toBool());

    on_chkUse1_clicked(true);
    on_chkUse2_clicked(true);
}


void QFESPIMB040ImageStackConfigWidget2::storeSettings(QSettings& settings, QString prefix) const {
    /*settings.setValue(prefix+"stage", ui->cmbStage->currentIndex());
    settings.setValue(prefix+"stage2", ui->cmbStage2->currentIndex());
    settings.setValue(prefix+"stage3", ui->cmbStage3->currentIndex());*/
    ui->cmbStage->storeSettings(settings, prefix+"stage/");
    ui->cmbStage2->storeSettings(settings, prefix+"stage2/");
    ui->cmbStage3->storeSettings(settings, prefix+"stage3/");
    settings.setValue(prefix+"use1", ui->chkUse1->isChecked());
    settings.setValue(prefix+"use2", ui->chkUse2->isChecked());
    settings.setValue(prefix+"prefix1", ui->edtPrefix1->text());
    settings.setValue(prefix+"prefix2", ui->edtPrefix2->text());
    settings.setValue(prefix+"acqsettings1", ui->cmbCam1Settings->currentConfigName());
    settings.setValue(prefix+"acqsettings2", ui->cmbCam2Settings->currentConfigName());

    settings.setValue(prefix+"start", ui->spinStart->value());
    settings.setValue(prefix+"delta", ui->spinDelta->value());
    settings.setValue(prefix+"steps", ui->spinSteps->value());
    settings.setValue(prefix+"delay", ui->spinDelay->value());

    settings.setValue(prefix+"start2", ui->spinStart2->value());
    settings.setValue(prefix+"delta2", ui->spinDelta2->value());
    settings.setValue(prefix+"steps2", ui->spinSteps2->value());

    settings.setValue(prefix+"start3", ui->spinStart3->value());
    settings.setValue(prefix+"delta3", ui->spinDelta3->value());
    settings.setValue(prefix+"steps3", ui->spinSteps3->value());

    settings.setValue(prefix+"stage2", ui->chkStage2->isChecked());
    settings.setValue(prefix+"stage3", ui->chkStage3->isChecked());
}


int QFESPIMB040ImageStackConfigWidget2::counter() const {
    return ui->spinCounter->value();
}

void QFESPIMB040ImageStackConfigWidget2::incCounter() {
    ui->spinCounter->setValue(ui->spinCounter->value()+1);
}

QString QFESPIMB040ImageStackConfigWidget2::prefix1() const {
    QString filename= ui->edtPrefix1->text();
    filename=filename.replace("%counter%", QString::number(counter()));
    return filename;
}

QString QFESPIMB040ImageStackConfigWidget2::prefix2() const {
    QString filename= ui->edtPrefix2->text();
    filename=filename.replace("%counter%", QString::number(counter()));
    return filename;
}

bool QFESPIMB040ImageStackConfigWidget2::use1() const {
    return ui->chkUse1->isChecked();
}

bool QFESPIMB040ImageStackConfigWidget2::use2() const {
    return ui->chkUse2->isChecked();
}

bool QFESPIMB040ImageStackConfigWidget2::useStage2() const {
    return ui->chkStage2->isChecked();
}

bool QFESPIMB040ImageStackConfigWidget2::useStage3() const {
    return ui->chkStage3->isChecked();
}


int QFESPIMB040ImageStackConfigWidget2::currentAxisID() const {
    return ui->cmbStage->currentAxisID();
}

QFExtensionLinearStage* QFESPIMB040ImageStackConfigWidget2::stage() const {
    return ui->cmbStage->currentExtensionLinearStage();
}

QFExtension* QFESPIMB040ImageStackConfigWidget2::stageExtension() const {
    return ui->cmbStage->currentExtension();
}

int QFESPIMB040ImageStackConfigWidget2::stackCount() const {
    return ui->spinSteps->value();
}

double QFESPIMB040ImageStackConfigWidget2::stackStart() const {
    return ui->spinStart->value();
}

double QFESPIMB040ImageStackConfigWidget2::stackDelta() const {
    return ui->spinDelta->value();
}

int QFESPIMB040ImageStackConfigWidget2::delay() const {
    return ui->spinDelay->value();
}

void QFESPIMB040ImageStackConfigWidget2::on_btnAcquire_clicked() {
    emit doStack();
}

void QFESPIMB040ImageStackConfigWidget2::on_btnGetCurrent_clicked() {
    if (stage()!=NULL) {
        if (stage()->isConnected(currentAxisID())) {
            ui->spinStart->setValue(stage()->getPosition(currentAxisID()));
        }
    }
}

void QFESPIMB040ImageStackConfigWidget2::on_btnGetCurrentAround_clicked() {
    if (stage()!=NULL) {
        if (stage()->isConnected(currentAxisID())) {
            ui->spinStart->setValue(stage()->getPosition(currentAxisID())-ui->spinDelta->value()*(double)ui->spinSteps->value());
        }
    }
}
void QFESPIMB040ImageStackConfigWidget2::on_spinStart_valueChanged(double value) {
    updateLabel();
}

void QFESPIMB040ImageStackConfigWidget2::on_spinDelta_valueChanged(double value) {
    updateLabel();
}

void QFESPIMB040ImageStackConfigWidget2::on_spinSteps_valueChanged(int value) {
    updateLabel();
}

void QFESPIMB040ImageStackConfigWidget2::updateLabel() {
    double start=stackStart();
    double delta=stackDelta();
    int steps=stackCount()-1;
    double end=start+delta*(double)steps;
    ui->labStage->setText(tr("scan end = %1 microns     scan length = %2 microns").arg(end).arg(end-start));


    start=stackStart2();
    delta=stackDelta2();
    steps=stackCount2()-1;
    end=start+delta*(double)steps;
    ui->labStage2->setText(tr("scan end = %1 microns     scan length = %2 microns").arg(end).arg(end-start));

    start=stackStart3();
    delta=stackDelta3();
    steps=stackCount3()-1;
    end=start+delta*(double)steps;
    ui->labStage3->setText(tr("scan end = %1 microns     scan length = %2 microns").arg(end).arg(end-start));
}

void QFESPIMB040ImageStackConfigWidget2::on_btnConfig_clicked() {
    if (opticsSetup) opticsSetup->lockStages();
    if (stage()) stage()->showSettingsDialog(currentAxisID());
    if (opticsSetup) opticsSetup->unlockStages();
}

void QFESPIMB040ImageStackConfigWidget2::on_btnConnect_clicked() {
    if (opticsSetup) opticsSetup->lockStages();
    if (stage()) {
        if (stage()->isConnected(currentAxisID())) {
            stage()->disconnectDevice(currentAxisID());
        } else {
            stage()->connectDevice(currentAxisID());
        }
    }
    if (opticsSetup) opticsSetup->unlockStages();
}

void QFESPIMB040ImageStackConfigWidget2::on_chkUse1_clicked(bool enabled) {
    ui->btnAcquire->setEnabled(ui->chkUse1->isChecked() || ui->chkUse2->isChecked());
    ui->widStage->setEnabled(ui->chkUse1->isChecked() || ui->chkUse2->isChecked());
    ui->chkStage2->setEnabled(ui->chkUse1->isChecked() || ui->chkUse2->isChecked());
    ui->chkStage3->setEnabled((ui->chkUse1->isChecked() || ui->chkUse2->isChecked()) && ui->chkStage2->isChecked());
    ui->widStage2->setEnabled((ui->chkUse1->isChecked() || ui->chkUse2->isChecked()) && ui->chkStage2->isChecked());
    ui->widStage3->setEnabled((ui->chkUse1->isChecked() || ui->chkUse2->isChecked()) && ui->chkStage3->isChecked());
    ui->edtPrefix1->setEnabled(ui->chkUse1->isChecked());
    ui->cmbCam1Settings->setEnabled(ui->chkUse1->isChecked());
}

void QFESPIMB040ImageStackConfigWidget2::on_chkUse2_clicked(bool enabled) {
    ui->btnAcquire->setEnabled(ui->chkUse1->isChecked() || ui->chkUse2->isChecked());
    ui->widStage->setEnabled(ui->chkUse1->isChecked() || ui->chkUse2->isChecked());
    ui->chkStage2->setEnabled(ui->chkUse1->isChecked() || ui->chkUse2->isChecked());
    ui->chkStage3->setEnabled((ui->chkUse1->isChecked() || ui->chkUse2->isChecked()) && ui->chkStage2->isChecked());
    ui->widStage2->setEnabled((ui->chkUse1->isChecked() || ui->chkUse2->isChecked()) && ui->chkStage2->isChecked());
    ui->widStage3->setEnabled((ui->chkUse1->isChecked() || ui->chkUse2->isChecked()) && ui->chkStage3->isChecked());
    ui->edtPrefix2->setEnabled(ui->chkUse2->isChecked());
    ui->cmbCam2Settings->setEnabled(ui->chkUse2->isChecked());
}

void QFESPIMB040ImageStackConfigWidget2::on_chkStage2_clicked(bool enabled) {
    ui->chkStage2->setEnabled(ui->chkUse1->isChecked() || ui->chkUse2->isChecked());
    ui->widStage2->setEnabled((ui->chkUse1->isChecked() || ui->chkUse2->isChecked()) && ui->chkStage2->isChecked());
    ui->chkStage3->setEnabled((ui->chkUse1->isChecked() || ui->chkUse2->isChecked()) && ui->chkStage2->isChecked());
}

void QFESPIMB040ImageStackConfigWidget2::on_chkStage3_clicked(bool enabled) {
    ui->chkStage3->setEnabled((ui->chkUse1->isChecked() || ui->chkUse2->isChecked()) && ui->chkStage2->isChecked());
    ui->widStage3->setEnabled((ui->chkUse1->isChecked() || ui->chkUse2->isChecked()) && ui->chkStage3->isChecked());
}

void QFESPIMB040ImageStackConfigWidget2::checkStage() {
    //if (stageConfig) stageConfig->lockStages();

    bool conn=false;
    bool found;
    if (stage()) {
        found=false;
        if (opticsSetup) conn=opticsSetup->isStageConnected(stage(), currentAxisID(), found);
        if (!opticsSetup || !found) conn=stage()->isConnected(currentAxisID());
        //conn=stage()->isConnected(currentAxisID());
        if (conn) {
            ui->btnConnect->setChecked(true);
            ui->btnConnect->setIcon(QIcon(":/spimb040/stagedisconnect.png"));
            ui->btnConnect->setToolTip(tr("Disconnect from axis ..."));
        } else {
            ui->btnConnect->setChecked(false);
            ui->btnConnect->setIcon(QIcon(":/spimb040/stageconnect.png"));
            ui->btnConnect->setToolTip(tr("Connect to axis ..."));
        }
        ui->btnConfig->setEnabled(true);
    }

    if (stage2()) {
        found=false;
        if (opticsSetup) conn=opticsSetup->isStageConnected(stage2(), currentAxisID2(), found);
        if (!opticsSetup || !found) conn=stage2()->isConnected(currentAxisID2());
        //conn=stage2()->isConnected(currentAxisID2());
        if (conn) {
            ui->btnConnect2->setChecked(true);
            ui->btnConnect2->setIcon(QIcon(":/spimb040/stagedisconnect.png"));
            ui->btnConnect2->setToolTip(tr("Disconnect from axis ..."));
        } else {
            ui->btnConnect2->setChecked(false);
            ui->btnConnect2->setIcon(QIcon(":/spimb040/stageconnect.png"));
            ui->btnConnect2->setToolTip(tr("Connect to axis ..."));
        }
        ui->btnConfig2->setEnabled(true);
    }

    if (stage3()) {
        found=false;
        if (opticsSetup) conn=opticsSetup->isStageConnected(stage3(), currentAxisID3(), found);
        if (!opticsSetup || !found) conn=stage3()->isConnected(currentAxisID3());
        //conn=stage3()->isConnected(currentAxisID3());
        if (conn) {
            ui->btnConnect3->setChecked(true);
            ui->btnConnect3->setIcon(QIcon(":/spimb040/stagedisconnect.png"));
            ui->btnConnect3->setToolTip(tr("Disconnect from axis ..."));
        } else {
            ui->btnConnect3->setChecked(false);
            ui->btnConnect3->setIcon(QIcon(":/spimb040/stageconnect.png"));
            ui->btnConnect3->setToolTip(tr("Connect to axis ..."));
        }
        ui->btnConfig3->setEnabled(true);
    }
    //if (stageConfig) stageConfig->unlockStages();


    QTimer::singleShot(STAGE_INTERVAL_MS, this, SLOT(checkStage()));
}







int QFESPIMB040ImageStackConfigWidget2::currentAxisID2() const {
    return ui->cmbStage2->currentAxisID();
}

QFExtensionLinearStage* QFESPIMB040ImageStackConfigWidget2::stage2() const {
    return ui->cmbStage2->currentExtensionLinearStage();
}

QFExtension* QFESPIMB040ImageStackConfigWidget2::stageExtension2() const {
    return ui->cmbStage2->currentExtension();
}

int QFESPIMB040ImageStackConfigWidget2::stackCount2() const {
    return ui->spinSteps2->value();
}

double QFESPIMB040ImageStackConfigWidget2::stackStart2() const {
    return ui->spinStart2->value();
}

double QFESPIMB040ImageStackConfigWidget2::stackDelta2() const {
    return ui->spinDelta2->value();
}


void QFESPIMB040ImageStackConfigWidget2::on_btnGetCurrent2_clicked() {
    if (opticsSetup) opticsSetup->lockStages();
    if (stage2()!=NULL) {
        if (stage2()->isConnected(currentAxisID2())) {
            ui->spinStart2->setValue(stage2()->getPosition(currentAxisID2()));
        }
    }
    if (opticsSetup) opticsSetup->unlockStages();
}

void QFESPIMB040ImageStackConfigWidget2::on_btnGetCurrentAround2_clicked() {
    if (opticsSetup) opticsSetup->lockStages();
    if (stage2()!=NULL) {
        if (stage2()->isConnected(currentAxisID2())) {
            ui->spinStart2->setValue(stage2()->getPosition(currentAxisID2())-ui->spinDelta2->value()*(double)ui->spinSteps2->value());
        }
    }
    if (opticsSetup) opticsSetup->unlockStages();
}
void QFESPIMB040ImageStackConfigWidget2::on_spinStart2_valueChanged(double value) {
    updateLabel();
}

void QFESPIMB040ImageStackConfigWidget2::on_spinDelta2_valueChanged(double value) {
    updateLabel();
}

void QFESPIMB040ImageStackConfigWidget2::on_spinSteps2_valueChanged(int value) {
    updateLabel();
}

void QFESPIMB040ImageStackConfigWidget2::on_btnConfig2_clicked() {
    if (opticsSetup) opticsSetup->lockStages();
    if (stage2()) stage2()->showSettingsDialog(currentAxisID2());
    if (opticsSetup) opticsSetup->unlockStages();
}

void QFESPIMB040ImageStackConfigWidget2::on_btnConnect2_clicked() {
    if (opticsSetup) opticsSetup->lockStages();
    if (stage2()) {
        if (stage2()->isConnected(currentAxisID2())) {
            stage2()->disconnectDevice(currentAxisID2());
        } else {
            stage2()->connectDevice(currentAxisID2());
        }
    }
    if (opticsSetup) opticsSetup->unlockStages();

}









int QFESPIMB040ImageStackConfigWidget2::currentAxisID3() const {
    return ui->cmbStage3->currentAxisID();
}

QFExtensionLinearStage* QFESPIMB040ImageStackConfigWidget2::stage3() const {
    return ui->cmbStage3->currentExtensionLinearStage();
}

QFExtension* QFESPIMB040ImageStackConfigWidget2::stageExtension3() const {
    return ui->cmbStage3->currentExtension();
}

int QFESPIMB040ImageStackConfigWidget2::stackCount3() const {
    return ui->spinSteps3->value();
}

double QFESPIMB040ImageStackConfigWidget2::stackStart3() const {
    return ui->spinStart3->value();
}

double QFESPIMB040ImageStackConfigWidget2::stackDelta3() const {
    return ui->spinDelta3->value();
}


void QFESPIMB040ImageStackConfigWidget2::on_btnGetCurrent3_clicked() {
    if (opticsSetup) opticsSetup->lockStages();
    if (stage3()!=NULL) {
        if (stage3()->isConnected(currentAxisID3())) {
            ui->spinStart3->setValue(stage3()->getPosition(currentAxisID3()));
        }
    }
    if (opticsSetup) opticsSetup->unlockStages();
}

void QFESPIMB040ImageStackConfigWidget2::on_btnGetCurrentAround3_clicked() {
    if (opticsSetup) opticsSetup->lockStages();
    if (stage3()!=NULL) {
        if (stage3()->isConnected(currentAxisID3())) {
            ui->spinStart3->setValue(stage3()->getPosition(currentAxisID3())-ui->spinDelta3->value()*(double)ui->spinSteps3->value());
        }
    }
    if (opticsSetup) opticsSetup->unlockStages();
}

void QFESPIMB040ImageStackConfigWidget2::on_spinStart3_valueChanged(double value) {
    updateLabel();
}

void QFESPIMB040ImageStackConfigWidget2::on_spinDelta3_valueChanged(double value) {
    updateLabel();
}

void QFESPIMB040ImageStackConfigWidget2::on_spinSteps3_valueChanged(int value) {
    updateLabel();
}

void QFESPIMB040ImageStackConfigWidget2::on_btnConfig3_clicked() {
    if (opticsSetup) opticsSetup->lockStages();
    if (stage3()) stage3()->showSettingsDialog(currentAxisID3());
    if (opticsSetup) opticsSetup->unlockStages();
}

void QFESPIMB040ImageStackConfigWidget2::on_btnConnect3_clicked() {
    if (opticsSetup) opticsSetup->lockStages();
    if (stage3()) {
        if (stage3()->isConnected(currentAxisID3())) {
            stage3()->disconnectDevice(currentAxisID3());
        } else {
            stage3()->connectDevice(currentAxisID3());
        }
    }
    if (opticsSetup) opticsSetup->unlockStages();
}

QString QFESPIMB040ImageStackConfigWidget2::currentConfigFilename(int camera) const {
    if (camera==0) return ui->cmbCam1Settings->currentConfigFilename();
    if (camera==1) return ui->cmbCam2Settings->currentConfigFilename();
    return "";
}

QString QFESPIMB040ImageStackConfigWidget2::currentConfigName(int camera) const {
    if (camera==0) return ui->cmbCam1Settings->currentConfigName();
    if (camera==1) return ui->cmbCam2Settings->currentConfigName();
    return "";
}
