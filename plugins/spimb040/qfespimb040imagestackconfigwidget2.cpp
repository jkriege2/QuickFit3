#include "qfespimb040imagestackconfigwidget2.h"
#include "ui_qfespimb040imagestackconfigwidget2.h"


#include "qfstyledbutton.h"
#include "qfpluginservices.h"
#include "qfstagecombobox.h"
#include "qfextensionmanager.h"
#include "../interfaces/qfextensionlinearstage.h"
#include "qfextension.h"
#include <QtGui>
#include <QtCore>
#include "qfespimb040opticssetup.h"
#include "qfcompleterfromfile.h"
#include "qfstyledbutton.h"
#include <tiffio.h>
#include "libtiff_tools.h"

#define STAGE_INTERVAL_MS 1313
#define PROCESS_EVENTS_TIMEOUT_MS 20

QFESPIMB040ImageStackConfigWidget2::QFESPIMB040ImageStackConfigWidget2(QFESPIMB040AcquisitionTools *acqTools, QFPluginLogService* log, QWidget* parent, QFPluginServices* pluginServices, QFESPIMB040OpticsSetupBase* stageConfig, QFESPIMB040AcquisitionDescription* acqDescription, QFESPIMB040ExperimentDescription* expDescription, QString configDirectory) :
    QWidget(parent),
    ui(new Ui::QFESPIMB040ImageStackConfigWidget2)
{
    this->acqTools=acqTools;
    this->log=log;
    m_pluginServices=pluginServices;
    this->opticsSetup=stageConfig;
    this->acqDescription=acqDescription;
    this->expDescription=expDescription;
    ui->setupUi(this);

    actGetCurrent=new QAction(QIcon(":/spimb040/get_current_pos.png"), tr("stage 1: scan starts at current position"), this);
    actGetCurrentAround=new QAction(QIcon(":/spimb040/get_current_pos_around.png"), tr("stage 1: scan around current position"), this);
    actGetCurrent2=new QAction(QIcon(":/spimb040/get_current_pos.png"), tr("stage 1: scan starts at current position"), this);
    actGetCurrentAround2=new QAction(QIcon(":/spimb040/get_current_pos_around.png"), tr("stage 1: scan around current position"), this);
    actGetCurrent3=new QAction(QIcon(":/spimb040/get_current_pos.png"), tr("stage 1: scan starts at current position"), this);
    actGetCurrentAround3=new QAction(QIcon(":/spimb040/get_current_pos_around.png"), tr("stage 1: scan around current position"), this);

    actGetCurrentG=new QActionGroup(this);
    actGetCurrentG->setExclusive(false);
    actGetCurrentG2=new QActionGroup(this);
    actGetCurrentG2->setExclusive(false);
    actGetCurrentG3=new QActionGroup(this);
    actGetCurrentG3->setExclusive(false);
    actGetCurrent->setActionGroup(actGetCurrentG);
    actGetCurrentAround->setActionGroup(actGetCurrentG);
    actGetCurrent2->setActionGroup(actGetCurrentG2);
    actGetCurrentAround2->setActionGroup(actGetCurrentG2);
    actGetCurrent3->setActionGroup(actGetCurrentG3);
    actGetCurrentAround3->setActionGroup(actGetCurrentG3);


    ui->btnGetCurrent->setDefaultAction(actGetCurrent);
    ui->btnGetCurrentAround->setDefaultAction(actGetCurrentAround);
    ui->btnGetCurrent2->setDefaultAction(actGetCurrent2);
    ui->btnGetCurrentAround2->setDefaultAction(actGetCurrentAround2);
    ui->btnGetCurrent3->setDefaultAction(actGetCurrent3);
    ui->btnGetCurrentAround3->setDefaultAction(actGetCurrentAround3);


    on_chkStackRelative_toggled(false);

    QDir().mkpath(ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/ext_spimb040/completers/");
    QFCompleterFromFile* c1=new QFCompleterFromFile(this);
    c1->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/ext_spimb040/completers/stack_prefix1.txt");
    QFCompleterFromFile* c2=new QFCompleterFromFile(this);
    c2->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/ext_spimb040/completers/stack_prefix2.txt");
    ui->edtPrefix1->setCompleter(c1);
    ui->edtPrefix2->setCompleter(c2);
    ui->edtPrefix1->addButton(new QFStyledButton(QFStyledButton::SelectFromCompleter, ui->edtPrefix1, ui->edtPrefix1));
    ui->edtPrefix2->addButton(new QFStyledButton(QFStyledButton::SelectFromCompleter, ui->edtPrefix2, ui->edtPrefix2));


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
    updateReplaces();
    bindLineEdit(ui->edtPrefix1);
    bindLineEdit(ui->edtPrefix2);

    connect(actGetCurrent, SIGNAL(triggered()), this, SLOT(actGetCurrent_clicked()));
    connect(actGetCurrentAround, SIGNAL(triggered()), this, SLOT(actGetCurrentAround_clicked()));
    connect(actGetCurrent2, SIGNAL(triggered()), this, SLOT(actGetCurrent2_clicked()));
    connect(actGetCurrentAround2, SIGNAL(triggered()), this, SLOT(actGetCurrentAround2_clicked()));
    connect(actGetCurrent3, SIGNAL(triggered()), this, SLOT(actGetCurrent3_clicked()));
    connect(actGetCurrentAround3, SIGNAL(triggered()), this, SLOT(actGetCurrentAround3_clicked()));

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

    ui->chkLightpath1->setChecked(settings.value(prefix+"lightpath1", false).toBool());
    ui->chkLightpath2->setChecked(settings.value(prefix+"lightpath2", false).toBool());
    ui->chkLightpath3->setChecked(settings.value(prefix+"lightpath3", false).toBool());
    ui->cmbLightpath1->setCurrentIndex(settings.value(prefix+"lightpath1idx", -1).toInt());
    ui->cmbLightpath2->setCurrentIndex(settings.value(prefix+"lightpath2idx", -1).toInt());
    ui->cmbLightpath3->setCurrentIndex(settings.value(prefix+"lightpath3idx", -1).toInt());

    ui->chkCycling2->setChecked(settings.value(prefix+"cycling2", false).toBool());
    ui->chkCycling3->setChecked(settings.value(prefix+"cycling3", false).toBool());

    ui->chkStackRelative->setChecked(settings.value(prefix+"stacks_relative", false).toBool());

    on_chkStackRelative_toggled(ui->chkStackRelative->isChecked());

    actGetCurrent->setChecked(lastGetC=settings.value(prefix+"getCurrent", false).toBool());
    actGetCurrentAround->setChecked(!lastGetC);
    actGetCurrent2->setChecked(lastGetC2=settings.value(prefix+"getCurrent2", false).toBool());
    actGetCurrentAround2->setChecked(!lastGetC2);
    actGetCurrent3->setChecked(lastGetC3=settings.value(prefix+"getCurrent3", false).toBool());
    actGetCurrentAround3->setChecked(!lastGetC3);

    ui->chkSaveMeasurements->setChecked(settings.value(prefix+"savemeasurements", false).toBool());
    ui->spinImages->setValue(settings.value(prefix+"images", 1).toInt());
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

    settings.setValue(prefix+"images", ui->spinImages->value());

    settings.setValue(prefix+"lightpath1idx", ui->cmbLightpath1->currentIndex());
    settings.setValue(prefix+"lightpath2idx", ui->cmbLightpath2->currentIndex());
    settings.setValue(prefix+"lightpath3idx", ui->cmbLightpath3->currentIndex());
    settings.setValue(prefix+"lightpath1", ui->chkLightpath1->isChecked());
    settings.setValue(prefix+"lightpath2", ui->chkLightpath2->isChecked());
    settings.setValue(prefix+"lightpath3", ui->chkLightpath3->isChecked());
    settings.setValue(prefix+"savemeasurements", ui->chkSaveMeasurements->isChecked());

    settings.setValue(prefix+"stacks_relative", ui->chkStackRelative->isChecked());
    settings.setValue(prefix+"getCurrent", lastGetC);
    settings.setValue(prefix+"getCurrent2", lastGetC2);
    settings.setValue(prefix+"getCurrent3", lastGetC3);

    settings.setValue(prefix+"cycling2", ui->chkCycling2->isChecked());
    settings.setValue(prefix+"cycling3", ui->chkCycling3->isChecked());

}

int QFESPIMB040ImageStackConfigWidget2::images() const {
    return ui->spinImages->value();
}
void QFESPIMB040ImageStackConfigWidget2::updateReplaces()
{
    QString stack="";
    stack=opticsSetup->getAxisNameForStage(ui->cmbStage->currentExtensionLinearStage(), ui->cmbStage->currentAxisID());
    if (ui->chkStage2->isChecked()) stack=stack+opticsSetup->getAxisNameForStage(ui->cmbStage2->currentExtensionLinearStage(), ui->cmbStage2->currentAxisID());
    if (ui->chkStage3->isChecked()) stack=stack+opticsSetup->getAxisNameForStage(ui->cmbStage3->currentExtensionLinearStage(), ui->cmbStage3->currentAxisID());
    setReplaceValue("stack", stack);
    setGlobalReplaces(opticsSetup, expDescription, acqDescription);
    setReplaceValue("acquisition1_name",  cleanStringForFilename(ui->cmbCam1Settings->currentConfigName()));
    setReplaceValue("acquisition2_name",  cleanStringForFilename(ui->cmbCam2Settings->currentConfigName()));
}


QString QFESPIMB040ImageStackConfigWidget2::prefix1()  {
    QString filename= ui->edtPrefix1->text();
    updateReplaces();
    filename=transformFilename(filename);
    return filename;
}

QString QFESPIMB040ImageStackConfigWidget2::prefix2()  {
    QString filename= ui->edtPrefix2->text();
    updateReplaces();
    filename=transformFilename(filename);
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

bool QFESPIMB040ImageStackConfigWidget2::stage2Cycling() const
{
    return ui->chkCycling2->isChecked();
}

bool QFESPIMB040ImageStackConfigWidget2::stage3Cycling() const
{
    return ui->chkCycling3->isChecked();
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
    if (ui->chkStackRelative->isChecked() && stage() && stage()->isConnected(currentAxisID())) {
        if (actGetCurrent->isChecked()) {
            return stage()->getPosition(currentAxisID());
        } else {
            return stage()->getPosition(currentAxisID())-(ui->spinDelta->value()*double(ui->spinSteps->value()))/2.0;
        }
    }
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

void QFESPIMB040ImageStackConfigWidget2::on_btnAcquireNext_clicked()
{
    acqDescription->nextCell();
    on_btnAcquire_clicked();
}

void QFESPIMB040ImageStackConfigWidget2::actGetCurrent_clicked() {
    if (ui->chkStackRelative->isChecked()) { lastGetC=actGetCurrent->isChecked(); updateLabel(); return; }

    //if (opticsSetup) opticsSetup->lockStages();
    if (stage()!=NULL) {
        if (stage()->isConnected(currentAxisID())) {
            ui->spinStart->setValue(stage()->getPosition(currentAxisID()));
        }
    }
    //if (opticsSetup) opticsSetup->unlockStages();
}

void QFESPIMB040ImageStackConfigWidget2::actGetCurrentAround_clicked() {
    if (ui->chkStackRelative->isChecked()) { lastGetC=actGetCurrent->isChecked(); updateLabel(); return; }

    //if (opticsSetup) opticsSetup->lockStages();
    if (stage()!=NULL) {
        if (stage()->isConnected(currentAxisID())) {
            ui->spinStart->setValue(stage()->getPosition(currentAxisID())-ui->spinDelta->value()*(double)ui->spinSteps->value()/2.0);
        }
    }
    //if (opticsSetup) opticsSetup->unlockStages();
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
    bool updt=updatesEnabled();
    if (updt) setUpdatesEnabled(false);
    double start=stackStart();
    double delta=stackDelta();
    int steps=stackCount()-1;
    double end=start+delta*(double)steps;
    ui->labStage->setText(tr("scan end = %1 microns     scan length = %2 microns").arg(end).arg(end-start));


    if (useStage2()) {
        start=stackStart2();
        delta=stackDelta2();
        steps=stackCount2()-1;
        end=start+delta*(double)steps;
        ui->labStage2->setText(tr("scan end = %1 microns     scan length = %2 microns").arg(end).arg(end-start));
    } else {
        ui->labStage2->setText(tr("---"));
    }

    if (useStage3()) {
        start=stackStart3();
        delta=stackDelta3();
        steps=stackCount3()-1;
        end=start+delta*(double)steps;
        ui->labStage3->setText(tr("scan end = %1 microns     scan length = %2 microns").arg(end).arg(end-start));
    } else {
        ui->labStage3->setText(tr("---"));
    }
    if (updt) setUpdatesEnabled(updt);
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
    bool updt=updatesEnabled();
    setUpdatesEnabled(false);
    ui->btnAcquire->setEnabled(ui->chkUse1->isChecked() || ui->chkUse2->isChecked());
    ui->widStage->setEnabled(ui->chkUse1->isChecked() || ui->chkUse2->isChecked());
    ui->chkStage2->setEnabled(ui->chkUse1->isChecked() || ui->chkUse2->isChecked());
    ui->chkStage3->setEnabled((ui->chkUse1->isChecked() || ui->chkUse2->isChecked()) && ui->chkStage2->isChecked());
    ui->widStage2->setEnabled((ui->chkUse1->isChecked() || ui->chkUse2->isChecked()) && ui->chkStage2->isChecked());
    ui->widStage3->setEnabled((ui->chkUse1->isChecked() || ui->chkUse2->isChecked()) && ui->chkStage3->isChecked());
    ui->edtPrefix1->setEnabled(ui->chkUse1->isChecked());
    ui->cmbCam1Settings->setEnabled(ui->chkUse1->isChecked());
    setUpdatesEnabled(updt);
}

void QFESPIMB040ImageStackConfigWidget2::on_chkUse2_clicked(bool enabled) {
    bool updt=updatesEnabled();
    setUpdatesEnabled(false);
    ui->btnAcquire->setEnabled(ui->chkUse1->isChecked() || ui->chkUse2->isChecked());
    ui->widStage->setEnabled(ui->chkUse1->isChecked() || ui->chkUse2->isChecked());
    ui->chkStage2->setEnabled(ui->chkUse1->isChecked() || ui->chkUse2->isChecked());
    ui->chkStage3->setEnabled((ui->chkUse1->isChecked() || ui->chkUse2->isChecked()) && ui->chkStage2->isChecked());
    ui->widStage2->setEnabled((ui->chkUse1->isChecked() || ui->chkUse2->isChecked()) && ui->chkStage2->isChecked());
    ui->widStage3->setEnabled((ui->chkUse1->isChecked() || ui->chkUse2->isChecked()) && ui->chkStage3->isChecked());
    ui->edtPrefix2->setEnabled(ui->chkUse2->isChecked());
    ui->cmbCam2Settings->setEnabled(ui->chkUse2->isChecked());
    setUpdatesEnabled(updt);
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

    bool updt=updatesEnabled();
    setUpdatesEnabled(false);
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
            if (ui->chkStackRelative->isChecked()) {
                ui->spinStart->setValue(stackStart());
            }
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
            if (ui->chkStackRelative->isChecked()) {
                ui->spinStart2->setValue(stackStart2());
            }
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
            if (ui->chkStackRelative->isChecked()) {
                ui->spinStart3->setValue(stackStart3());
            }

        } else {
            ui->btnConnect3->setChecked(false);
            ui->btnConnect3->setIcon(QIcon(":/spimb040/stageconnect.png"));
            ui->btnConnect3->setToolTip(tr("Connect to axis ..."));
        }
        ui->btnConfig3->setEnabled(true);
    }
    //if (stageConfig) stageConfig->unlockStages();

    setUpdatesEnabled(updt);

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
    if (ui->chkStackRelative->isChecked() && useStage2() && stage2() && stage2()->isConnected(currentAxisID2())) {
        if (actGetCurrent2->isChecked()) {
            return stage2()->getPosition(currentAxisID2());
        } else {
            return stage2()->getPosition(currentAxisID2())-(ui->spinDelta2->value()*double(ui->spinSteps2->value()))/2.0;
        }
    }
    return ui->spinStart2->value();
}

double QFESPIMB040ImageStackConfigWidget2::stackDelta2() const {
    return ui->spinDelta2->value();
}


void QFESPIMB040ImageStackConfigWidget2::actGetCurrent2_clicked() {
    if (ui->chkStackRelative->isChecked()) { lastGetC2=actGetCurrent2->isChecked(); updateLabel(); return; }
    //if (opticsSetup) opticsSetup->lockStages();
    if (stage2()!=NULL) {
        if (stage2()->isConnected(currentAxisID2()) && useStage2()) {
            ui->spinStart2->setValue(stage2()->getPosition(currentAxisID2()));
        }
    }
    //if (opticsSetup) opticsSetup->unlockStages();
}

void QFESPIMB040ImageStackConfigWidget2::actGetCurrentAround2_clicked() {
    if (ui->chkStackRelative->isChecked()) { lastGetC2=actGetCurrent2->isChecked(); updateLabel(); return; }
    //if (opticsSetup) opticsSetup->lockStages();
    if (stage2()!=NULL) {
        if (stage2()->isConnected(currentAxisID2()) && useStage2()) {
            ui->spinStart2->setValue(stage2()->getPosition(currentAxisID2())-ui->spinDelta2->value()*(double)ui->spinSteps2->value()/2.0);
        }
    }
    //if (opticsSetup) opticsSetup->unlockStages();
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
    if (ui->chkStackRelative->isChecked() && useStage3() && stage3() && stage3()->isConnected(currentAxisID3())) {
        if (actGetCurrent3->isChecked()) {
            return stage3()->getPosition(currentAxisID3());
        } else {
            return stage3()->getPosition(currentAxisID3())-(ui->spinDelta3->value()*double(ui->spinSteps3->value()))/2.0;
        }
    }
    return ui->spinStart3->value();
}

double QFESPIMB040ImageStackConfigWidget2::stackDelta3() const {
    return ui->spinDelta3->value();
}

bool QFESPIMB040ImageStackConfigWidget2::lightpath1Activated() const {
    return ui->chkLightpath1->isChecked();
}

bool QFESPIMB040ImageStackConfigWidget2::lightpath2Activated() const {
    return ui->chkLightpath2->isChecked();
}

bool QFESPIMB040ImageStackConfigWidget2::lightpath3Activated() const {
    return ui->chkLightpath3->isChecked();
}

QString QFESPIMB040ImageStackConfigWidget2::lightpath1() const {
    return ui->cmbLightpath1->currentText();
}

QString QFESPIMB040ImageStackConfigWidget2::lightpath2() const {
    return ui->cmbLightpath2->currentText();
}

QString QFESPIMB040ImageStackConfigWidget2::lightpath3() const {
    return ui->cmbLightpath3->currentText();
}

QString QFESPIMB040ImageStackConfigWidget2::lightpath1Filename() const {
    return ui->cmbLightpath1->itemData(ui->cmbLightpath1->currentIndex()).toString();
}

QString QFESPIMB040ImageStackConfigWidget2::lightpath2Filename() const {
    return ui->cmbLightpath2->itemData(ui->cmbLightpath2->currentIndex()).toString();
}

QString QFESPIMB040ImageStackConfigWidget2::lightpath3Filename() const {
    return ui->cmbLightpath3->itemData(ui->cmbLightpath3->currentIndex()).toString();
}

void QFESPIMB040ImageStackConfigWidget2::actGetCurrent3_clicked() {
    if (ui->chkStackRelative->isChecked()) { lastGetC2=actGetCurrent3->isChecked(); updateLabel(); return; }
    //if (opticsSetup) opticsSetup->lockStages();
    if (stage3()!=NULL) {
        if (stage3()->isConnected(currentAxisID3()) && useStage3()) {
            ui->spinStart3->setValue(stage3()->getPosition(currentAxisID3()));
        }
    }
    //if (opticsSetup) opticsSetup->unlockStages();
}

void QFESPIMB040ImageStackConfigWidget2::actGetCurrentAround3_clicked() {
    if (ui->chkStackRelative->isChecked()) { lastGetC2=actGetCurrent3->isChecked(); updateLabel(); return; }

    //if (opticsSetup) opticsSetup->lockStages();
    if (stage3()!=NULL) {
        if (stage3()->isConnected(currentAxisID3()) && useStage3()) {
            ui->spinStart3->setValue(stage3()->getPosition(currentAxisID3())-ui->spinDelta3->value()*(double)ui->spinSteps3->value()/2.0);
        }
    }
    //if (opticsSetup) opticsSetup->unlockStages();
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

void QFESPIMB040ImageStackConfigWidget2::on_chkStackRelative_toggled(bool checked) {
    ui->spinStart->setEnabled(!checked);
    ui->spinStart2->setEnabled(!checked);
    ui->spinStart3->setEnabled(!checked);

    actGetCurrentG->setExclusive(checked);
    actGetCurrentG2->setExclusive(checked);
    actGetCurrentG3->setExclusive(checked);
    actGetCurrent->setCheckable(checked);
    actGetCurrentAround->setCheckable(checked);
    actGetCurrent2->setCheckable(checked);
    actGetCurrentAround2->setCheckable(checked);
    actGetCurrent3->setCheckable(checked);
    actGetCurrentAround3->setCheckable(checked);

    if (checked)  {
        actGetCurrent->setChecked(lastGetC);
        actGetCurrentAround->setChecked(!lastGetC);
        actGetCurrent2->setChecked(lastGetC2);
        actGetCurrentAround2->setChecked(!lastGetC2);
        actGetCurrent3->setChecked(lastGetC3);
        actGetCurrentAround3->setChecked(!lastGetC3);
    }

}

void QFESPIMB040ImageStackConfigWidget2::on_btnSaveTemplate_clicked()
{
    QDir().mkpath(ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/ext_spimb040/acq_templates/");
    QString dir=ProgramOptions::getInstance()->getQSettings()->value("QFESPIMB040ImageStackConfigWidget2/lasttemplatedir", ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/ext_spimb040/acq_templates/").toString();
    QString filename=qfGetSaveFileName(this, tr("save as template ..."), dir, tr("image stack configuration (*.isc)"))    ;
    if (!filename.isEmpty()) {
        bool ok=true;
        if (QFile::exists(filename)) {
            ok=false;
            if (QMessageBox::question(this, tr("save as template ..."), tr("The file\n  '%1'\nalready exists. Overwrite?").arg(filename), QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::Yes) {
                ok=true;
            }
        }
        if (ok) {
            QSettings set(filename, QSettings::IniFormat);
            storeSettings(set, "image_stack_settings/");
            dir=QFileInfo(filename).absolutePath();
        }
    }
    ProgramOptions::getInstance()->getQSettings()->setValue("QFESPIMB040ImageStackConfigWidget2/lasttemplatedir", dir);
}

void QFESPIMB040ImageStackConfigWidget2::on_btnLoadTemplate_clicked()
{
    QDir().mkpath(ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/ext_spimb040/acq_templates/");
    QString dir=ProgramOptions::getInstance()->getQSettings()->value("QFESPIMB040ImageStackConfigWidget2/lasttemplatedir", ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/ext_spimb040/acq_templates/").toString();
    QString filename=qfGetOpenFileName(this, tr("open template ..."), dir, tr("image stack configuration (*.isc)"))    ;
    if (!filename.isEmpty()) {
        QSettings set(filename, QSettings::IniFormat);
        loadSettings(set, "image_stack_settings/");
        dir=QFileInfo(filename).absolutePath();
    }
    ProgramOptions::getInstance()->getQSettings()->setValue("QFESPIMB040ImageStackConfigWidget2/lasttemplatedir", dir);
}

void QFESPIMB040ImageStackConfigWidget2::on_btnTestAcq1_pressed()
{
    opticsSetup->overrideCameraPreview(0, ui->cmbCam1Settings->currentConfigFilename(), "");
}

void QFESPIMB040ImageStackConfigWidget2::on_btnTestAcq1_released()
{
    opticsSetup->resetCameraPreview(0);
}

void QFESPIMB040ImageStackConfigWidget2::on_btnTestAcq2_pressed()
{
    opticsSetup->overrideCameraPreview(1, ui->cmbCam2Settings->currentConfigFilename(), "");
}

void QFESPIMB040ImageStackConfigWidget2::on_btnTestAcq2_released()
{
    opticsSetup->resetCameraPreview(1);
}

void QFESPIMB040ImageStackConfigWidget2::on_btnTestAcqLP1_pressed()
{
    opticsSetup->overrideCameraPreview(0, ui->cmbCam1Settings->currentConfigFilename(), lightpath1Filename());
    opticsSetup->overrideCameraPreview(1, ui->cmbCam2Settings->currentConfigFilename(), lightpath1Filename());
}

void QFESPIMB040ImageStackConfigWidget2::on_btnTestAcqLP1_released()
{
    opticsSetup->resetCameraPreview(0);
    opticsSetup->resetCameraPreview(1);
}

void QFESPIMB040ImageStackConfigWidget2::on_btnTestAcqLP2_pressed()
{
    opticsSetup->overrideCameraPreview(0, ui->cmbCam1Settings->currentConfigFilename(), lightpath2Filename());
    opticsSetup->overrideCameraPreview(1, ui->cmbCam2Settings->currentConfigFilename(), lightpath2Filename());
}

void QFESPIMB040ImageStackConfigWidget2::on_btnTestAcqLP2_released()
{
    opticsSetup->resetCameraPreview(0);
    opticsSetup->resetCameraPreview(1);
}

void QFESPIMB040ImageStackConfigWidget2::on_btnTestAcqLP3_pressed()
{
    opticsSetup->overrideCameraPreview(0, ui->cmbCam1Settings->currentConfigFilename(), lightpath3Filename());
    opticsSetup->overrideCameraPreview(1, ui->cmbCam2Settings->currentConfigFilename(), lightpath3Filename());
}

void QFESPIMB040ImageStackConfigWidget2::on_btnTestAcqLP3_released()
{
    opticsSetup->resetCameraPreview(0);
    opticsSetup->resetCameraPreview(1);
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


void QFESPIMB040ImageStackConfigWidget2::lightpathesChanged(QFESPIMB040OpticsSetupItems lightpathes) {
    bool updt=updatesEnabled();
    setUpdatesEnabled(false);
    QString idx1=ui->cmbLightpath1->currentText();
    QString idx2=ui->cmbLightpath2->currentText();
    QString idx3=ui->cmbLightpath3->currentText();
    ui->cmbLightpath1->clear();
    ui->cmbLightpath2->clear();
    ui->cmbLightpath3->clear();
    //qDebug()<<"QFESPIMB040CamParamStackConfigWidget2::lightpathesChanged "<<lightpathes.size();
    for (int i=0; i<lightpathes.size(); i++) {
        QTriple<QIcon, QString, QString> p=lightpathes[i];
        ui->cmbLightpath1->addItem(p.first, p.second, p.third);
        ui->cmbLightpath2->addItem(p.first, p.second, p.third);
        ui->cmbLightpath3->addItem(p.first, p.second, p.third);
    }
    ui->cmbLightpath1->setCurrentIndex(qMax(0, ui->cmbLightpath1->findText(idx1)));
    ui->cmbLightpath2->setCurrentIndex(qMax(0, ui->cmbLightpath2->findText(idx2)));
    ui->cmbLightpath3->setCurrentIndex(qMax(0, ui->cmbLightpath3->findText(idx3)));
    setUpdatesEnabled(updt);

}


bool QFESPIMB040ImageStackConfigWidget2::saveMeasurements() const {
    return ui->chkSaveMeasurements->isChecked();
}











#define IMAGESTACK_ERROR(message) \
    log->log_error(QString("  - ")+(message)+QString("\n")); \
    QMessageBox::critical(this, tr("B040SPIM: Image Stack Acquisition Error"), (message));


void QFESPIMB040ImageStackConfigWidget2::performStack()
{
    if (!(use1() || use2())) {
        QMessageBox::critical(this, tr("B040SPIM: Image Stack Acquisition"), tr("Cannot start image acquisition: No camera selected!"));
        return;
    }

    QDateTime startDateTime=QDateTime::currentDateTime();
    QList<QFESPIMB040OpticsSetupBase::measuredValues> measured;


    QProgressListDialog progress(tr("Image Stack Acquisition"), tr("&Cancel"), 0, 100, this);
    progress.setWindowModality(Qt::WindowModal);
    //progress.setMinimumDuration(0);
    progress.setValue(0);
    progress.addItem(tr("initializing stages"));
    progress.addItem(tr("preparing cameras"));
    progress.addItem(tr("performing acquisition"));
    progress.addItem(tr("storing data to disk"));
    progress.addItem(tr("clean up"));
    progress.setHasProgressBar(true);
    progress.show();


    log->log_text(tr("starting image stack acquisition:\n"));
    log->log_text(tr("  - locking stages\n"));
    progress.start();
    opticsSetup->lockStages();
    opticsSetup->lockLightpath();

    bool ok=true;
    int axisCount=1; // number of axes to use for scan

    //////////////////////////////////////////////////////////////////////////////////////
    // CHECK/CONNECT SELECTED STAGE 1
    //////////////////////////////////////////////////////////////////////////////////////
    QFExtensionLinearStage* stage=this->stage();
    int stageAxis=currentAxisID();
    double stageInitialPos=0;
    progress.setProgressText(tr("locking stage 1 ..."));
    ok=acqTools->connectStageForAcquisition(stage, stageAxis, stageInitialPos, tr("B040SPIM: Image Stack Acquisition"),1);


    if (ok) {

        //////////////////////////////////////////////////////////////////////////////////////
        // CHECK/CONNECT SELECTED STAGE 2 (if selected)
        //////////////////////////////////////////////////////////////////////////////////////
        QFExtensionLinearStage* stage2=this->stage2();
        int stageAxis2=currentAxisID2();
        double stageInitialPos2=0;
        if (useStage2()) {
            progress.setProgressText(tr("locking stage 2 ..."));
            ok=acqTools->connectStageForAcquisition(stage2, stageAxis2, stageInitialPos2, tr("B040SPIM: Image Stack Acquisition"),2);
            if (ok) axisCount++;

        }

        if (!ok) {
            opticsSetup->unlockStages();
            opticsSetup->unlockLightpath();
            return;
        }

        //////////////////////////////////////////////////////////////////////////////////////
        // CHECK/CONNECT SELECTED STAGE 3 (if selected)
        //////////////////////////////////////////////////////////////////////////////////////
        QFExtensionLinearStage* stage3=this->stage3();
        int stageAxis3=currentAxisID3();
        double stageInitialPos3=0;
        if (useStage3()) {
            progress.setProgressText(tr("locking stage 3 ..."));
            ok=acqTools->connectStageForAcquisition(stage3, stageAxis3, stageInitialPos3, tr("B040SPIM: Image Stack Acquisition"),3);
            if (ok) axisCount++;

        }

        if (!ok) {
            opticsSetup->unlockStages();
            opticsSetup->unlockLightpath();
            return;
        }


        progress.nextItem();

        //////////////////////////////////////////////////////////////////////////////////////
        // LOCK/INIT CAMERA 1
        //////////////////////////////////////////////////////////////////////////////////////
        bool useCam1=false;
        QFExtension* extension1=NULL;
        QFExtensionCamera* ecamera1=NULL;
        int camera1=0;
        QString acquisitionSettingsFilename1="", previewSettingsFilename1="";
        QString acquisitionPrefix1=prefix1();
        QStringList TIFFFIlename1;;
        QList<TIFF*> tiff1;
        TIFF* tiff1_background=NULL;
        QString TIFFFIlenameBackground1;
        if (use1()) {
            progress.setProgressText(tr("locking camera 1 ..."));
            if (!(useCam1=opticsSetup->lockCamera(0, &extension1, &ecamera1, &camera1, &previewSettingsFilename1))) {
                IMAGESTACK_ERROR(tr("error locking camera 1!\n"));
            }
        }
        if (QFile::exists(currentConfigFilename(0))) acquisitionSettingsFilename1=currentConfigFilename(0);

        //////////////////////////////////////////////////////////////////////////////////////
        // LOCK/INIT CAMERA 2
        //////////////////////////////////////////////////////////////////////////////////////
        bool useCam2=false;
        QFExtension* extension2=NULL;
        QFExtensionCamera* ecamera2=NULL;
        QString acquisitionSettingsFilename2="", previewSettingsFilename2="";
        QString acquisitionPrefix2=prefix2();
        QStringList TIFFFIlename2;
        QList<TIFF*> tiff2;
        TIFF* tiff2_background=NULL;
        QString TIFFFIlenameBackground2;

        int camera2=0;
        if (use2()) {
            progress.setProgressText(tr("locking camera 2 ..."));
            if(!(useCam2=opticsSetup->lockCamera(1, &extension2, &ecamera2, &camera2, &previewSettingsFilename2))) {
                IMAGESTACK_ERROR(tr("error locking camer 2!\n"));
            }
        }
        if (QFile::exists(currentConfigFilename(1))) acquisitionSettingsFilename2=currentConfigFilename(1);

        if (ok && !useCam1 && !useCam2) {
            IMAGESTACK_ERROR(tr("Cannot start image acquisition: No camera selected, or both cameras not usable!"));
            ok=false;
        }

        if (ok && useCam1) log->log_text(tr("  - storing files with prefix 1: '%1'\n").arg(acquisitionPrefix1));
        if (ok && useCam2) log->log_text(tr("  - storing files with prefix 2: '%1'\n").arg(acquisitionPrefix2));

        //////////////////////////////////////////////////////////////////////////////////////
        // PREPARE CAMERA 1
        //////////////////////////////////////////////////////////////////////////////////////
        int width1=0, height1=0;
        uint32_t* buffer1=NULL;
        if (ok && useCam1) {
            progress.setLabelText(tr("preparing camera 1 ..."));
            ok=acqTools->prepareCamera(1, camera1, ecamera1, acquisitionSettingsFilename1, width1, height1, &buffer1);

        }

        //////////////////////////////////////////////////////////////////////////////////////
        // PREPARE CAMERA 2
        //////////////////////////////////////////////////////////////////////////////////////
        int width2=0, height2=0;
        uint32_t* buffer2=NULL;
        if (ok && useCam2) {
            progress.setLabelText(tr("preparing camera 2 ..."));
            ok=acqTools->prepareCamera(2, camera2, ecamera2, acquisitionSettingsFilename2, width2, height2, &buffer2);


        }


        progress.setLabelText(tr("preparing lightpathes ..."));


        //////////////////////////////////////////////////////////////////////////////////////
        // COUNT LIGHTPATHS
        //////////////////////////////////////////////////////////////////////////////////////
        QStringList lightpathList;
        QStringList lightpathNames;
        TIFFFIlenameBackground1=acquisitionPrefix1+".background.tif";
        TIFFFIlenameBackground2=acquisitionPrefix2+".background.tif";
        if (lightpath1Activated()) {
            if (QFile::exists(lightpath1Filename())) {
                lightpathList.append(lightpath1Filename());
                lightpathNames.append(lightpath1());
                TIFFFIlename1.append(acquisitionPrefix1+".lightpath1.tif");
                tiff1.append(NULL);
                TIFFFIlename2.append(acquisitionPrefix2+".lightpath1.tif");
                tiff2.append(NULL);
            } else {
                ok=false;
                IMAGESTACK_ERROR(tr("acquisition lightpath 1 '%1' configuration not found!").arg(lightpath1()));
            }
        }
        if (lightpath2Activated()) {
            if (QFile::exists(lightpath2Filename())) {
                lightpathList.append(lightpath2Filename());
                lightpathNames.append(lightpath2());
                TIFFFIlename1.append(acquisitionPrefix1+".lightpath2.tif");
                tiff1.append(NULL);
                TIFFFIlename2.append(acquisitionPrefix2+".lightpath2.tif");
                tiff2.append(NULL);
            } else {
                ok=false;
                IMAGESTACK_ERROR(tr("acquisition lightpath 2 '%1' configuration not found!").arg(lightpath2()));
            }
        }
        if (lightpath3Activated()) {
            if (QFile::exists(lightpath3Filename())) {
                lightpathList.append(lightpath3Filename());
                lightpathNames.append(lightpath3());
                TIFFFIlename1.append(acquisitionPrefix1+".lightpath3.tif");
                tiff1.append(NULL);
                TIFFFIlename2.append(acquisitionPrefix2+".lightpath3.tif");
                tiff2.append(NULL);
            } else {
                ok=false;
                IMAGESTACK_ERROR(tr("acquisition lightpath 3 '%1' configuration not found!").arg(lightpath3()));
            }
        }
        if (ok && lightpathList.isEmpty()) {
            lightpathList.append("");
            lightpathNames.append("default");
            TIFFFIlename1.append(acquisitionPrefix1+".tif");
            tiff1.append(NULL);
            TIFFFIlename2.append(acquisitionPrefix2+".tif");
            tiff2.append(NULL);
        }

        progress.setLabelText(tr("opening/creating output files ..."));

        //////////////////////////////////////////////////////////////////////////////////////
        // OPEN OUTPUT TIFF FILES
        //////////////////////////////////////////////////////////////////////////////////////
        progress.setLabelText(tr("opening output files ..."));
        QApplication::processEvents();
        if (ok && useCam1) {
            QDir().mkpath(QFileInfo(TIFFFIlenameBackground1.toAscii().data()).absolutePath());
            tiff1_background=TIFFOpen(TIFFFIlenameBackground1.toAscii().data(), "w");;
            if (!tiff1_background) {
                ok=false;
                IMAGESTACK_ERROR(tr("error opening TIFF file (camera 1) '%1'!").arg(TIFFFIlenameBackground1));
            }

            for (int i=0; i<TIFFFIlename1.size(); i++) {
                QDir().mkpath(QFileInfo(TIFFFIlename1[i].toAscii().data()).absolutePath());
                tiff1[i]=TIFFOpen(TIFFFIlename1[i].toAscii().data(), "w");
                if (!tiff1[i]) {
                    ok=false;
                    IMAGESTACK_ERROR(tr("error opening TIFF file (camera 1) '%1'!").arg(TIFFFIlename1[i]));
                    break;
                }
            }
        }
        if (ok && useCam2) {
            QDir().mkpath(QFileInfo(TIFFFIlenameBackground2.toAscii().data()).absolutePath());
            tiff2_background=TIFFOpen(TIFFFIlenameBackground2.toAscii().data(), "w");;
            if (!tiff2_background) {
                ok=false;
                IMAGESTACK_ERROR(tr("error opening TIFF file (camera 2) '%1'!").arg(TIFFFIlenameBackground2));
            }
            for (int i=0; i<TIFFFIlename2.size(); i++) {
                QDir().mkpath(QFileInfo(TIFFFIlename2[i].toAscii().data()).absolutePath());
                tiff2[i]=TIFFOpen(TIFFFIlename2[i].toAscii().data(), "w");
                if (!tiff2[i]) {
                    ok=false;
                    IMAGESTACK_ERROR(tr("error opening TIFF file (camera 2) '%1'!").arg(TIFFFIlename2[i]));
                }
            }
        }

        progress.setLabelText(tr("switching main shutter on ..."));


        //////////////////////////////////////////////////////////////////////////////////////
        // switch off light
        //////////////////////////////////////////////////////////////////////////////////////
        bool formerMainShutterState=opticsSetup->getMainIlluminationShutter();
        if (opticsSetup->isMainIlluminationShutterAvailable()){
            log->log_text(tr("  - switch main shutter off for background frames!\n"));
            opticsSetup->setMainIlluminationShutter(false, true);
        }
        //////////////////////////////////////////////////////////////////////////////////////
        // acquire backrgound images
        //////////////////////////////////////////////////////////////////////////////////////
            log->log_text(tr("  - acquiring background images ...\n"));

            if (useCam1) {
                if (ecamera1->acquireOnCamera(camera1, buffer1, NULL)) {
                    TIFFTWriteUint16from32(tiff1_background, buffer1, width1, height1, false);
                    log->log_text(tr("  - acquired overview image background from camera 1!\n"));
                } else {
                    ok=false;
                    IMAGESTACK_ERROR(tr("error acquiring background image on camera 1!\n"));
                }
            }
            //QApplication::processEvents();
            if (useCam2) {
                if (ecamera2->acquireOnCamera(camera2, buffer2, NULL)) {
                    TIFFTWriteUint16from32(tiff2_background, buffer2, width2, height2, false);
                    log->log_text(tr("  - acquired overview image background from camera 2!\n"));
                } else {
                    ok=false;
                    IMAGESTACK_ERROR(tr("error acquiring background image on camera 2!\n"));
                }
            }

        //////////////////////////////////////////////////////////////////////////////////////
        // switch on light
        //////////////////////////////////////////////////////////////////////////////////////
        if (opticsSetup->isMainIlluminationShutterAvailable()){
            log->log_text(tr("  - switch main shutter on!\n"));
            opticsSetup->setMainIlluminationShutter(true, true);
        }


        if (progress.wasCanceled()) {
            log->log_warning(tr("canceled by user!\n"));
            ok=false;
        }


        //////////////////////////////////////////////////////////////////////////////////////
        // CALCULATE A LIST WITH ALL POSITIONS TO MOVE TO
        //////////////////////////////////////////////////////////////////////////////////////
        progress.setLabelText(tr("preparing list of stage positions ..."));
        double stageStart=stackStart();
        double stageDelta=stackDelta();
        int stageCount=stackCount();

        double stageStart2=stackStart2();
        double stageDelta2=stackDelta2();
        int stageCount2=stackCount2();
        bool stageReturn2=stage2Cycling();

        double stageStart3=stackStart3();
        double stageDelta3=stackDelta3();
        int stageCount3=stackCount3();
        bool stageReturn3=stage3Cycling();

        QList<QTriple<double, double, double> > moveTo;

        if (axisCount==1) {
            double pos=stageStart;
            for (int x=0; x<stageCount; x++) {
                moveTo.append(qMakeTriple(pos, 0.0, 0.0));
                pos=pos+stageDelta;
            }
        } else if (axisCount==2) {
            double pos=stageStart;
            double pos2=stageStart2;
            for (int x=0; x<stageCount; x++) {
                if (stageReturn2) pos2=stageStart2;
                for (int y=0; y<stageCount2; y++) {
                    moveTo.append(qMakeTriple(pos, pos2, 0.0));
                    pos2=pos2+stageDelta2;
                }
                pos=pos+stageDelta;
            }
        } else if (axisCount==3) {
            double pos=stageStart;
            double pos2=stageStart2;
            double pos3=stageStart3;
            for (int x=0; x<stageCount; x++) {
                if (stageReturn2) pos2=stageStart2;
                for (int y=0; y<stageCount2; y++) {
                    if (stageReturn3) pos3=stageStart3;
                    for (int z=0; z<stageCount3; z++) {
                        moveTo.append(qMakeTriple(pos, pos2, pos3));
                        pos3=pos3+stageDelta3;
                    }
                    pos2=pos2+stageDelta2;
                }
                pos=pos+stageDelta;
            }
        }


        int images=moveTo.size()*this->images()*lightpathList.size();


        //////////////////////////////////////////////////////////////////////////////////////
        // ACQUIRE IMAGE, MOVE, ACQUIRE IMAGE, MOVE, ...
        //    images are stored in TIFF files using libtiff and they are (possibly) downscaled to 16-bit
        //////////////////////////////////////////////////////////////////////////////////////
        progress.nextItem();

        QMap<QString, QVariant> acquisitionDescription, acquisitionDescription1, acquisitionDescription2;
        QList<QVariant> positions, positions2, positions3;
        QTime timAcquisition=QTime::currentTime();
        QDateTime timStart;
        QString estimation="";
        QString fps="";
        double duration=0;
        if (ok) {
            progress.setLabelText(tr("acquiring images ..."));
            bool running=ok;
            //double newPos=stageStart;
            int posIdx=0;
            int imageIdx=0;
            measured.append(opticsSetup->getMeasuredValues());
            while (running && (posIdx<=moveTo.size())) {
                double newPos=stageInitialPos;
                double newPos2=stageInitialPos2;
                double newPos3=stageInitialPos3;
                if (posIdx<moveTo.size()) {
                    newPos=moveTo[posIdx].first;
                    newPos2=moveTo[posIdx].second;
                    newPos3=moveTo[posIdx].third;
                }
                if (axisCount==1) {
                    log->log_text(tr("  - moving to position %1 micron ...").arg(newPos));
                    stage->move(stageAxis, newPos);
                    QTime t1;
                    t1.start();
                    while (stage->getAxisState(stageAxis)==QFExtensionLinearStage::Moving) {
                        if (t1.elapsed()>PROCESS_EVENTS_TIMEOUT_MS) {
                            progress.setLabelText(tr("moving stage to %1 microns (distance: %2) ...%3%4").arg(newPos).arg(fabs(stage->getPosition(stageAxis)-newPos)).arg(estimation).arg(fps));
                            QApplication::processEvents(QEventLoop::AllEvents, 2);
                            if (progress.wasCanceled()) break;
                            t1.start();
                        }
                    }

                    // wait additional time-span after moving stages!
                    QTime t;
                    int DeltaT=qMin(5000,qMax(1,delay()));
                    t.start();
                    t1.start();
                    while (t.elapsed()<DeltaT) {
                        if (t1.elapsed()>PROCESS_EVENTS_TIMEOUT_MS)  {
                            progress.setLabelText(tr("moving stage to %1 microns (distance: %2) ... waiting%3%4").arg(newPos).arg(fabs(stage->getPosition(stageAxis)-newPos)).arg(estimation).arg(fps));
                            QApplication::processEvents(QEventLoop::AllEvents, 2);
                            if (progress.wasCanceled()) break;
                            t1.start();
                        }
                    }
                    if (ok) {
                        if (stage->getAxisState(stageAxis)==QFExtensionLinearStage::Ready) {
                            log->log_text(tr(" OK\n"));
                            positions.append(stage->getPosition(stageAxis));
                        } else {
                            IMAGESTACK_ERROR(tr("error moving to position %1 micron!\n").arg(newPos));
                            ok=false;
                        }
                    }

                } else if (axisCount==2) {
                    log->log_text(tr("  - moving to position (%1, %2) micron ...").arg(newPos).arg(newPos2));
                    stage->move(stageAxis, newPos);
                    stage2->move(stageAxis2, newPos2);
                    QTime t1;
                    t1.start();
                    while (stage->getAxisState(stageAxis)==QFExtensionLinearStage::Moving || stage2->getAxisState(stageAxis2)==QFExtensionLinearStage::Moving) {
                        if (t1.elapsed()>PROCESS_EVENTS_TIMEOUT_MS) {
                            double dist=sqrt(qfSqr(stage->getPosition(stageAxis)-newPos)+qfSqr(stage2->getPosition(stageAxis2)-newPos2));
                            progress.setLabelText(tr("moving stage to (%1, %2) microns (distance: %3) ...%4%5").arg(newPos).arg(newPos2).arg(dist).arg(estimation).arg(fps));
                            QApplication::processEvents(QEventLoop::AllEvents, 2);
                            if (progress.wasCanceled()) break;
                            t1.start();
                        }
                    }

                    // wait additional time-span after moving stages!
                    QTime t;
                    int DeltaT=qMin(5000,qMax(1,delay()));
                    t.start();
                    t1.start();
                    while (t.elapsed()<DeltaT) {
                        if (t1.elapsed()>PROCESS_EVENTS_TIMEOUT_MS) {
                            double dist=sqrt(qfSqr(stage->getPosition(stageAxis)-newPos)+qfSqr(stage2->getPosition(stageAxis2)-newPos2));
                            progress.setLabelText(tr("moving stage to (%1, %2) microns (distance: %3) ... waiting%4%5").arg(newPos).arg(newPos2).arg(dist).arg(estimation).arg(fps));
                            QApplication::processEvents(QEventLoop::AllEvents, 2);
                            if (progress.wasCanceled()) break;
                            t1.start();
                        }
                    }
                    if (ok) {
                        if (stage->getAxisState(stageAxis)==QFExtensionLinearStage::Ready && stage2->getAxisState(stageAxis2)==QFExtensionLinearStage::Ready) {
                            log->log_text(tr(" OK\n"));
                            positions.append(stage->getPosition(stageAxis));
                            positions2.append(stage2->getPosition(stageAxis2));
                        } else {
                            IMAGESTACK_ERROR(tr("error moving to position (%1, %2) micron!\n").arg(newPos).arg(newPos2));
                            ok=false;
                        }
                    }
                } else if (axisCount==3) {
                    log->log_text(tr("  - moving to position (%1, %2, %3) micron ...").arg(newPos).arg(newPos2).arg(newPos3));
                    stage->move(stageAxis, newPos);
                    stage2->move(stageAxis2, newPos2);
                    stage3->move(stageAxis3, newPos3);
                    QTime t1;
                    t1.start();
                    while (stage->getAxisState(stageAxis)==QFExtensionLinearStage::Moving || stage2->getAxisState(stageAxis2)==QFExtensionLinearStage::Moving || stage3->getAxisState(stageAxis3)==QFExtensionLinearStage::Moving) {
                        if (t1.elapsed()>PROCESS_EVENTS_TIMEOUT_MS) {
                            double dist=sqrt(qfSqr(stage->getPosition(stageAxis)-newPos) + qfSqr(stage2->getPosition(stageAxis2)-newPos2) + qfSqr(stage3->getPosition(stageAxis3)-newPos3));
                            progress.setLabelText(tr("moving stage to (%1, %2, %3) microns (distance: %4) ...%5%6").arg(newPos).arg(newPos2).arg(newPos3).arg(dist).arg(estimation).arg(fps));
                            QApplication::processEvents(QEventLoop::AllEvents, 2);
                            if (progress.wasCanceled()) break;
                            t1.start();
                        }
                    }

                    // wait additional time-span after moving stages!
                    QTime t;
                    int DeltaT=qMin(5000,qMax(1,delay()));
                    t.start();
                    t1.start();
                    while (t.elapsed()<DeltaT) {
                        if (t1.elapsed()>PROCESS_EVENTS_TIMEOUT_MS) {
                            double dist=sqrt(qfSqr(stage->getPosition(stageAxis)-newPos) + qfSqr(stage2->getPosition(stageAxis2)-newPos2) + qfSqr(stage3->getPosition(stageAxis3)-newPos3));
                            progress.setLabelText(tr("moving stage to (%1, %2, %3) microns (distance: %4) ... waiting%5%6").arg(newPos).arg(newPos2).arg(newPos3).arg(dist).arg(estimation).arg(fps));
                            QApplication::processEvents(QEventLoop::AllEvents, 2);
                            if (progress.wasCanceled()) break;
                            t1.start();
                        }
                    }
                    if (ok) {
                        if (stage->getAxisState(stageAxis)==QFExtensionLinearStage::Ready && stage2->getAxisState(stageAxis2)==QFExtensionLinearStage::Ready && stage3->getAxisState(stageAxis3)==QFExtensionLinearStage::Ready) {
                            log->log_text(tr(" OK\n"));
                            positions.append(stage->getPosition(stageAxis));
                            positions2.append(stage2->getPosition(stageAxis2));
                            positions3.append(stage3->getPosition(stageAxis3));
                        } else {
                            IMAGESTACK_ERROR(tr("error moving to position (%1, %2, %3) micron!\n").arg(newPos).arg(newPos2).arg(newPos3));
                            ok=false;
                        }
                    }
                }


                QApplication::processEvents(QEventLoop::AllEvents, 2);
                if (posIdx<moveTo.size()) {
                    if (progress.wasCanceled()) {
                        running=false;
                        log->log_warning(tr("  - acquisition canceled by user!\n"));
                    } else {
                        if (saveMeasurements()) measured.append(opticsSetup->getMeasuredValues());
                        for (int lp=0; lp<lightpathList.size(); lp++) {
                            if (lightpathList.size()>1 || lp==0) {
                                if (!lightpathList[lp].isEmpty() && QFile::exists(lightpathList[lp])) {
                                    log->log_text(tr("  - setting lightpath '%1' (%2) ...").arg(lightpathNames[lp]).arg(lp));
                                    opticsSetup->loadLightpathConfig(lightpathList[lp], true);
                                    log->log_text(tr(" DONE\n"));
                                }
                                if (posIdx<=0) {
                                    opticsSetup->saveLightpathConfig(acquisitionDescription, lightpathNames[lp], QString("lightpath%1/").arg(lp+1), QList<bool>(), true);
                                }
                            }
                            for (int img=0; img<this->images(); img++) {
                                            log->log_text(tr("  - acquiring images (%1/%2) ...\n").arg(imageIdx+1).arg(images));
                                            if (posIdx>3) {
                                                double duration=double(timAcquisition.elapsed())/1000.0;
                                                double eta=duration/double(posIdx+1.0)*double(moveTo.size());
                                                double etc=eta-duration;
                                    uint mini=floor(etc/60.0);
                                    uint secs=round(etc-double(mini)*60.0);
                                    estimation=tr("\nest. remaining duration (min:secs): %1:%2 ").arg(mini, 2, 10, QChar('0')).arg(secs, 2, 10, QChar('0'));

                                    fps=tr("\nacquisition rate: %1fps").arg(double(imageIdx+1)/duration, 0, 'f', 2);
                                }
                                progress.setLabelText(tr("acquiring images (%1/%2) ...%3%4").arg(imageIdx+1).arg(images).arg(estimation).arg(fps));
                                QApplication::processEvents(QEventLoop::AllEvents, 2);
                                if (progress.wasCanceled()) {
                                    running=false;
                                    log->log_warning(tr("  - acquisition canceled by user!\n"));
                                    break;
                                }
                                if (posIdx==0) {
                                    timAcquisition.start();
                                    timStart=QDateTime::currentDateTime();
                                }
                                uint64_t timestampDummy=0;
                                if (useCam1) {
                                    if (ecamera1->acquireOnCamera(camera1, buffer1, NULL, &acquisitionDescription1)) {
                                        TIFFTWriteUint16from32(tiff1[lp], buffer1, width1, height1, false);
                                        TIFFWriteDirectory(tiff1[lp]);
                                    } else {
                                        ok=false;
                                        IMAGESTACK_ERROR(tr("error acquiring image %1/%2 on camera 1!\n").arg(imageIdx+1).arg(images));
                                    }
                                }
                                //QApplication::processEvents();
                                if (useCam2) {
                                    if (ecamera2->acquireOnCamera(camera2, buffer2, NULL, &acquisitionDescription2)) {
                                        TIFFTWriteUint16from32(tiff2[lp], buffer2, width2, height2, false);
                                        TIFFWriteDirectory(tiff2[lp]);
                                    } else {
                                        ok=false;
                                        IMAGESTACK_ERROR(tr("error acquiring image %1/%2 on camera 2!\n").arg(imageIdx+1).arg(images));
                                    }
                                }
                                imageIdx++;

                            }
                        }

                        //QApplication::processEvents();
                    }
                }
                if (!ok) running=false;

                posIdx++;
                newPos+=stageDelta;
                progress.setValue((int)round((double)posIdx/(double)moveTo.size()*100.0));
                QApplication::processEvents(QEventLoop::AllEvents, 10);
                if (progress.wasCanceled()) {
                    break;
                }
            }
            duration=timAcquisition.elapsed()/1000.0;
        }
        progress.setValue(100);
        if (saveMeasurements()) measured.append(opticsSetup->getMeasuredValues());

        progress.nextItem();
        progress.setProgressText(tr("switching main shutter off ..."));


        //////////////////////////////////////////////////////////////////////////////////////
        // switch on/off light
        //////////////////////////////////////////////////////////////////////////////////////
        if (opticsSetup->isMainIlluminationShutterAvailable()){
            log->log_text(tr("  - switch main shutter %1!\n").arg((formerMainShutterState)?tr("on"):tr("off")));
            opticsSetup->setMainIlluminationShutter(formerMainShutterState, true);
        }


        progress.setProgressText(tr("closing TIF files ..."));
        //////////////////////////////////////////////////////////////////////////////////////
        // close tiff files and free buffers
        //////////////////////////////////////////////////////////////////////////////////////
        progress.setLabelText(tr("closing output files ..."));
        QApplication::processEvents();
        if (tiff1_background) TIFFClose(tiff1_background);
        if (tiff2_background) TIFFClose(tiff2_background);

        for (int lp=0; lp<tiff1.size(); lp++) if (tiff1[lp]) TIFFClose(tiff1[lp]);
        for (int lp=0; lp<tiff2.size(); lp++) if (tiff2[lp]) TIFFClose(tiff2[lp]);
        tiff1.clear();
        tiff2.clear();
        if (buffer1) free(buffer1);
        if (buffer2) free(buffer2);
        buffer1=buffer2=NULL;

        progress.setProgressText(tr("collecting acquisition data ..."));
        //////////////////////////////////////////////////////////////////////////////////////
        // collect acquisition data common to all cameras
        //////////////////////////////////////////////////////////////////////////////////////
        QString positionsCSV;
        if (ok) {
            acquisitionDescription["type"]="stack 1 axis";
            acquisitionDescription["axis1/type"]="other";
            if (opticsSetup->getStage(QFESPIMB040OpticsSetupBase::StageX)==stage && opticsSetup->getStageAxis(QFESPIMB040OpticsSetupBase::StageX)==stageAxis) acquisitionDescription["axis1/type"]="x";
            if (opticsSetup->getStage(QFESPIMB040OpticsSetupBase::StageY)==stage && opticsSetup->getStageAxis(QFESPIMB040OpticsSetupBase::StageY)==stageAxis) acquisitionDescription["axis1/type"]="y";
            if (opticsSetup->getStage(QFESPIMB040OpticsSetupBase::StageZ)==stage && opticsSetup->getStageAxis(QFESPIMB040OpticsSetupBase::StageZ)==stageAxis) acquisitionDescription["axis1/type"]="z";
            acquisitionDescription["axis1/stage_name"]=stageExtension()->getName();
            acquisitionDescription["axis1/stage_axis"]=stageAxis;
            acquisitionDescription["axis1/stack_start"]=stageStart;
            acquisitionDescription["axis1/stack_delta"]=stageDelta;
            acquisitionDescription["axis1/stack_count"]=stageCount;
            acquisitionDescription["sequence_overall_length"]=images;
            acquisitionDescription["images_per_position"]=this->images();
            acquisitionDescription["lightpath_count"]=lightpathList.count();
            acquisitionDescription["lightpaths"]=lightpathNames;
            acquisitionDescription["start_time"]=timStart;
            acquisitionDescription["duration"]=duration;
            acquisitionDescription["stack_positions"]=positions;
            if (useStage2() && useStage3()) {
                acquisitionDescription["type"]="stack 3 axis";
                acquisitionDescription["axis2/stack_positions"]=positions2;
                acquisitionDescription["axis3/stack_positions"]=positions3;
            } else if (useStage2() && !useStage3()) {
                acquisitionDescription["type"]="stack 2 axis";
                acquisitionDescription["axis1/stack_positions"]=positions2;
            }

            if (useStage2()) {
                acquisitionDescription["axis2/type"]="other";
                if (opticsSetup->getStage(QFESPIMB040OpticsSetupBase::StageX)==stage2 && opticsSetup->getStageAxis(QFESPIMB040OpticsSetupBase::StageX)==stageAxis2) acquisitionDescription["axis2/type"]="x";
                if (opticsSetup->getStage(QFESPIMB040OpticsSetupBase::StageY)==stage2 && opticsSetup->getStageAxis(QFESPIMB040OpticsSetupBase::StageY)==stageAxis2) acquisitionDescription["axis2/type"]="y";
                if (opticsSetup->getStage(QFESPIMB040OpticsSetupBase::StageZ)==stage2 && opticsSetup->getStageAxis(QFESPIMB040OpticsSetupBase::StageZ)==stageAxis2) acquisitionDescription["axis2/type"]="z";
                acquisitionDescription["axis2/stage_name"]=stageExtension2()->getName();
                acquisitionDescription["axis2/stage_axis"]=stageAxis2;
                acquisitionDescription["axis2/stack_start"]=stageStart2;
                acquisitionDescription["axis2/stack_delta"]=stageDelta2;
                acquisitionDescription["axis2/stack_count"]=stageCount2;
            }

            if (useStage3()) {
                acquisitionDescription["axis3/type"]="other";
                if (opticsSetup->getStage(QFESPIMB040OpticsSetupBase::StageX)==stage3 && opticsSetup->getStageAxis(QFESPIMB040OpticsSetupBase::StageX)==stageAxis3) acquisitionDescription["axis3/type"]="x";
                if (opticsSetup->getStage(QFESPIMB040OpticsSetupBase::StageY)==stage3 && opticsSetup->getStageAxis(QFESPIMB040OpticsSetupBase::StageY)==stageAxis3) acquisitionDescription["axis3/type"]="y";
                if (opticsSetup->getStage(QFESPIMB040OpticsSetupBase::StageZ)==stage3 && opticsSetup->getStageAxis(QFESPIMB040OpticsSetupBase::StageZ)==stageAxis3) acquisitionDescription["axis3/type"]="z";
                acquisitionDescription["axis3/stage_name"]=stageExtension3()->getName();
                acquisitionDescription["axis3/stage_axis"]=stageAxis3;
                acquisitionDescription["axis3/stack_start"]=stageStart3;
                acquisitionDescription["axis3/stack_delta"]=stageDelta3;
                acquisitionDescription["axis3/stack_count"]=stageCount3;
            }


            QTextStream pf(&positionsCSV);
            if (axisCount==3) {
                pf<<"# number, position 1[micrometer], position 2 [micrometer], position 3 [micrometer], ideal position 1 [micrometer], ideal position 2 [micrometer], ideal position 3 [micrometer] \n";
                for (int i=0; i<qMin(positions.size(), moveTo.size()); i++) {
                    pf<<i<<", "<<CDoubleToQString(positions[i].toDouble())<<", "<<CDoubleToQString(positions2[i].toDouble())<<", "<<CDoubleToQString(positions3[i].toDouble())<<", "<<CDoubleToQString(moveTo[i].first)<<", "<<CDoubleToQString(moveTo[i].second)<<", "<<CDoubleToQString(moveTo[i].third)<<"\n";
                }
            } else if (axisCount==2) {
                pf<<"# number, position 1[micrometer], position 2 [micrometer], ideal position 1 [micrometer], ideal position 2 [micrometer] \n";
                for (int i=0; i<qMin(positions.size(), moveTo.size()); i++) {
                    pf<<i<<", "<<CDoubleToQString(positions[i].toDouble())<<", "<<CDoubleToQString(positions2[i].toDouble())<<", "<<CDoubleToQString(moveTo[i].first)<<", "<<CDoubleToQString(moveTo[i].second)<<"\n";
                }
            } else if (axisCount==1) {
                pf<<"# number, position 1[micrometer], ideal position 1 [micrometer] \n";
                for (int i=0; i<qMin(positions.size(), moveTo.size()); i++) {
                    pf<<i<<", "<<CDoubleToQString(positions[i].toDouble())<<", "<<CDoubleToQString(moveTo[i].first)<<"\n";
                }
            }
        }

        //////////////////////////////////////////////////////////////////////////////////////
        // write image stack properties to files, also collects camera specific information
        //////////////////////////////////////////////////////////////////////////////////////
        if (ok && useCam1) {
            QMap<QString, QVariant> acquisitionDescription11=acquisitionDescription;
            acquisitionDescription11=acquisitionDescription11.unite(acquisitionDescription1);
            QList<QFExtensionCamera::CameraAcquititonFileDescription> files;
            for (int lp=0; lp<TIFFFIlename1.size(); lp++) {
                QFExtensionCamera::CameraAcquititonFileDescription d;
                d.name=TIFFFIlename1[lp];
                d.description="image stack from camera 1, lightpath "+QString(lp+1)+" '"+lightpathNames[lp]+"'";
                d.type="TIFF16";
                files.append(d);
            }


            QString PositionsFilename=acquisitionPrefix1+".positions.dat";
            QFile posFile(PositionsFilename);
            if (posFile.open(QIODevice::WriteOnly)) {
                posFile.write(positionsCSV.toAscii().data());
                posFile.close();
                QFExtensionCamera::CameraAcquititonFileDescription d;
                d.name=PositionsFilename;
                d.description="positions of image stack from camera 1";
                d.type="CSV";
                files.append(d);
            } else {
                log->log_error(tr("  - could not write positions file '%1' for camera 1: %2 ...").arg(PositionsFilename).arg(posFile.errorString()));
            }

            QString MeasurementsFilename=acqTools->saveMeasuredData(acquisitionPrefix1, measured);
            if (!MeasurementsFilename.isEmpty() && QFile::exists(MeasurementsFilename)) {
                QFExtensionCamera::CameraAcquititonFileDescription d;
                d.name=MeasurementsFilename;
                d.description="measureable properties of setup";
                d.type="CSV";
                files.append(d);
            }

            if (useCam1){
                QFExtensionCamera::CameraAcquititonFileDescription d;
                d.name=TIFFFIlenameBackground1;
                d.description="background frame from camera 1";
                d.type="TIFF16";
                files.append(d);
            }
            if (useCam2){
                QFExtensionCamera::CameraAcquititonFileDescription d;
                d.name=TIFFFIlenameBackground2;
                d.description="background frame from camera 2";
                d.type="TIFF16";
                files.append(d);
            }
            log->log_text(tr("  - writing acquisition description 1 ..."));
            acqTools->savePreviewDescription(0, extension1, ecamera1, camera1, acquisitionPrefix1, acquisitionDescription11, files, startDateTime);
            log->log_text(tr(" DONE!\n"));
        }
        if (ok && useCam2) {
            QMap<QString, QVariant> acquisitionDescription22=acquisitionDescription;
            acquisitionDescription22=acquisitionDescription22.unite(acquisitionDescription2);
            QList<QFExtensionCamera::CameraAcquititonFileDescription> files;
            for (int lp=0; lp<TIFFFIlename1.size(); lp++) {
                QFExtensionCamera::CameraAcquititonFileDescription d;
                d.name=TIFFFIlename2[lp];
                d.description="image stack from camera 2, lightpath "+QString(lp+1)+" '"+lightpathNames[lp]+"'";
                d.type="TIFF16";
                files.append(d);
            }


            QString PositionsFilename=acquisitionPrefix2+".positions.dat";
            QFile posFile(PositionsFilename);
            if (posFile.open(QIODevice::WriteOnly)) {
                posFile.write(positionsCSV.toAscii().data());
                posFile.close();
                QFExtensionCamera::CameraAcquititonFileDescription d;
                d.name=PositionsFilename;
                d.description="positions of image stack from camera 1";
                d.type="CSV";
                files.append(d);
            } else {
                log->log_error(tr("  - could not write positions file '%1' for camera 2: %2 ...").arg(PositionsFilename).arg(posFile.errorString()));
            }

            QString MeasurementsFilename=acqTools->saveMeasuredData(acquisitionPrefix2, measured);
            if (!MeasurementsFilename.isEmpty() && QFile::exists(MeasurementsFilename)) {
                QFExtensionCamera::CameraAcquititonFileDescription d;
                d.name=MeasurementsFilename;
                d.description="measureable properties of setup";
                d.type="CSV";
                files.append(d);
            }

            log->log_text(tr("  - writing acquisition description 2 ..."));
            acqTools->savePreviewDescription(1, extension2, ecamera2, camera2, acquisitionPrefix2, acquisitionDescription22, files, startDateTime);
            log->log_text(tr(" DONE!\n"));
        }

        progress.nextItem();
        progress.setProgressText(tr("releasing cameras ..."));
        //////////////////////////////////////////////////////////////////////////////////////
        // release cameras
        //////////////////////////////////////////////////////////////////////////////////////
        if (useCam1) {
            opticsSetup->releaseCamera(0);
            log->log_text(tr("  - released camera 1!\n"));
        }
        if (useCam2) {
            opticsSetup->releaseCamera(1);
            log->log_text(tr("  - released camera 2!\n"));
        }

        if (ok && useCam1) log->log_text(tr("  - stored files with prefix 1: '%1'\n").arg(acquisitionPrefix1));
        if (ok && useCam2) log->log_text(tr("  - stored files with prefix 2: '%1'\n").arg(acquisitionPrefix2));

        if (ok) log->log_text(tr("image stack acquisition DONE!\n"));
    }
    progress.setProgressText(tr("releasing stages and lightpath ..."));
    opticsSetup->unlockStages();
    opticsSetup->unlockLightpath();
    opticsSetup->ensureLightpath();
    progress.close();
}

