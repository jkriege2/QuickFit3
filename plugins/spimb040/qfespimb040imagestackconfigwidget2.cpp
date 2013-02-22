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

#define STAGE_INTERVAL_MS 1313

QFESPIMB040ImageStackConfigWidget2::QFESPIMB040ImageStackConfigWidget2(QWidget* parent, QFPluginServices* pluginServices, QFESPIMB040OpticsSetup* stageConfig, QFESPIMB040AcquisitionDescription* acqDescription, QFESPIMB040ExperimentDescription* expDescription, QString configDirectory) :
    QWidget(parent),
    ui(new Ui::QFESPIMB040ImageStackConfigWidget2)
{
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
    setUpdatesEnabled(false);
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
    setUpdatesEnabled(updt);
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

