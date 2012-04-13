#include "qfespimb040opticssetup.h"
#include "objectives.h"
#include "ui_qfespimb040opticssetup.h"
#include "objectives.h"
#include "filters.h"
#include "qfespimb040mainwindow.h"
#include "qfespimb040shortcutconfigdialog.h"

QFESPIMB040OpticsSetup::QFESPIMB040OpticsSetup(QWidget* pluginMainWidget, QWidget* parent,  QFPluginLogService* log, QFPluginServices* pluginServices) :
    QWidget(parent),
    ui(new Ui::QFESPIMB040OpticsSetup)
{
    setting_lightpath=false;
    m_pluginServices=pluginServices;
    m_pluginMainWidget=pluginMainWidget;
    m_log=log;
    ui->setupUi(this);
    ui->camConfig1->init(0, m_pluginServices, m_pluginServices->getGlobalConfigFileDirectory());
    ui->camConfig1->setLog(m_log);
    ui->camConfig2->init(1, m_pluginServices, m_pluginServices->getGlobalConfigFileDirectory());
    ui->camConfig2->setLog(m_log);
    ui->stageSetup->init(m_log, m_pluginServices);
    ui->filtTransmission->setFilterINI(m_pluginServices->getGlobalConfigFileDirectory()+"/spimb040_filters.ini", m_pluginServices->getConfigFileDirectory()+"/spimb040_filters.ini");
    ui->filtDetection->setFilterINI(m_pluginServices->getGlobalConfigFileDirectory()+"/spimb040_filters.ini", m_pluginServices->getConfigFileDirectory()+"/spimb040_filters.ini");
    ui->filtSplitter->setFilterINI(m_pluginServices->getGlobalConfigFileDirectory()+"/spimb040_filters.ini", m_pluginServices->getConfigFileDirectory()+"/spimb040_filters.ini");
    ui->filtDetection11->setFilterINI(m_pluginServices->getGlobalConfigFileDirectory()+"/spimb040_filters.ini", m_pluginServices->getConfigFileDirectory()+"/spimb040_filters.ini");
    ui->filtDetection21->setFilterINI(m_pluginServices->getGlobalConfigFileDirectory()+"/spimb040_filters.ini", m_pluginServices->getConfigFileDirectory()+"/spimb040_filters.ini");
    ui->objDetection->setObjectivesINI(m_pluginServices->getGlobalConfigFileDirectory()+"/spimb040_objectives.ini", m_pluginServices->getConfigFileDirectory()+"/spimb040_objectives.ini");
    ui->objProjection->setObjectivesINI(m_pluginServices->getGlobalConfigFileDirectory()+"/spimb040_objectives.ini", m_pluginServices->getConfigFileDirectory()+"/spimb040_objectives.ini");
    ui->objTube1->setObjectivesINI(m_pluginServices->getGlobalConfigFileDirectory()+"/spimb040_objectives.ini", m_pluginServices->getConfigFileDirectory()+"/spimb040_objectives.ini");
    ui->objTube2->setObjectivesINI(m_pluginServices->getGlobalConfigFileDirectory()+"/spimb040_objectives.ini", m_pluginServices->getConfigFileDirectory()+"/spimb040_objectives.ini");
    ui->shutterMainIllumination->init(m_log, m_pluginServices);
    ui->shutterLaser1->init(m_log, m_pluginServices);
    ui->shutterLaser2->init(m_log, m_pluginServices);
    ui->filtcDetection->init(m_log, m_pluginServices);
    ui->cmbLightpathConfig->init(m_pluginServices->getConfigFileDirectory()+"/plugins/ext_spimb040/", "lpc");
    ui->cmbLightpathConfig->setIcon(QIcon(":/spimb040/lightpath.png"));
    ui->btnLockFiltersEtc->setChecked(true);
    connect(ui->cmbLightpathConfig, SIGNAL(configsChanged(QFESPIMB040OpticsSetupItems)), this, SLOT(configsChanged(QFESPIMB040OpticsSetupItems)));

    // create shortcuts
    connect(addShortCut("stage_x2", "translation stage: joystick speed x2"), SIGNAL(activated()), ui->stageSetup, SLOT(speedX2()));
    connect(addShortCut("stage_x10", "translation stage: joystick speed x10"), SIGNAL(activated()), ui->stageSetup, SLOT(speedX10()));
    connect(addShortCut("stage_d2", "translation stage: joystick speed /2"), SIGNAL(activated()), ui->stageSetup, SLOT(speedD2()));
    connect(addShortCut("stage_d10", "translation stage: joystick speed /10"), SIGNAL(activated()), ui->stageSetup, SLOT(speedD10()));
    connect(addShortCut("stage_stepx", "translation stage: step x"), SIGNAL(activated()), ui->stageSetup, SLOT(stepX()));
    connect(addShortCut("stage_stepmx", "translation stage: step -x"), SIGNAL(activated()), ui->stageSetup, SLOT(stepMinusX()));
    connect(addShortCut("stage_stepy", "translation stage: step y"), SIGNAL(activated()), ui->stageSetup, SLOT(stepY()));
    connect(addShortCut("stage_stepmy", "translation stage: step -y"), SIGNAL(activated()), ui->stageSetup, SLOT(stepMinusY()));
    connect(addShortCut("stage_stepz", "translation stage: step z"), SIGNAL(activated()), ui->stageSetup, SLOT(stepZ()));
    connect(addShortCut("stage_stepmz", "translation stage: step -z"), SIGNAL(activated()), ui->stageSetup, SLOT(stepMinusZ()));
    connect(addShortCut("stage_joysticktoggle", "translation stage: toggle joystick"), SIGNAL(activated()), ui->stageSetup, SLOT(toggleJoystick()));
    connect(addShortCut("stage_joystick_on", "translation stage: joystick on"), SIGNAL(activated()), ui->stageSetup, SLOT(joystickOn()));
    connect(addShortCut("stage_joystick_off", "translation stage: joystick off"), SIGNAL(activated()), ui->stageSetup, SLOT(joystickOff()));

    connect(addShortCut("mainshutter_toggle", "main shutter: toggle"), SIGNAL(activated()), ui->shutterMainIllumination, SLOT(toggleShutter()));
    connect(addShortCut("mainshutter_on", "main shutter: on"), SIGNAL(activated()), ui->shutterMainIllumination, SLOT(shutterOn()));
    connect(addShortCut("mainshutter_off", "main shutter: off"), SIGNAL(activated()), ui->shutterMainIllumination, SLOT(shutterOff()));

    connect(addShortCut("shutter_laser1_toggle", "laser 1 shutter: toggle"), SIGNAL(activated()), ui->shutterLaser1, SLOT(toggleShutter()));
    connect(addShortCut("shutter_laser1_on", "laser 1 shutter: on"), SIGNAL(activated()), ui->shutterLaser1, SLOT(shutterOn()));
    connect(addShortCut("shutter_laser1_off", "laser 1 shutter: off"), SIGNAL(activated()), ui->shutterLaser1, SLOT(shutterOff()));

    connect(addShortCut("shutter_laser2_toggle", "laser 2 shutter: toggle"), SIGNAL(activated()), ui->shutterLaser2, SLOT(toggleShutter()));
    connect(addShortCut("shutter_laser2_on", "laser 2 shutter: on"), SIGNAL(activated()), ui->shutterLaser2, SLOT(shutterOn()));
    connect(addShortCut("shutter_laser2_off", "laser 2 shutter: off"), SIGNAL(activated()), ui->shutterLaser2, SLOT(shutterOff()));

    //connect(addShortCut("filterchanger_detection_filter1", "detection filter changer: filter 1"), SIGNAL(activated()), ui->shutterLaser2, SLOT(shutterOff()));

    connect(addShortCut("cam1_acquire_single", "camera 1: acquire single frame"), SIGNAL(activated()), ui->camConfig1, SLOT(previewSingle()));
    connect(addShortCut("cam1_acquire_continuous_toggle", "camera 1: toggle preview acquisition"), SIGNAL(activated()), ui->camConfig1, SLOT(startStopPreview()));
    connect(addShortCut("cam1_acquire_continuous_stop", "camera 1: stop preview"), SIGNAL(activated()), ui->camConfig1, SLOT(stopPreview()));

    connect(addShortCut("cam2_acquire_single", "camera 2: acquire single frame"), SIGNAL(activated()), ui->camConfig2, SLOT(previewSingle()));
    connect(addShortCut("cam2_acquire_continuous_toggle", "camera 2: toggle preview acquisition"), SIGNAL(activated()), ui->camConfig2, SLOT(startStopPreview()));
    connect(addShortCut("cam2_acquire_continuous_stop", "camera 2: stop preview"), SIGNAL(activated()), ui->camConfig2, SLOT(stopPreview()));

}

QFESPIMB040OpticsSetup::~QFESPIMB040OpticsSetup()
{
    delete ui;
}

void QFESPIMB040OpticsSetup::lockStages() {
    ui->stageSetup->lockStages();
}

void QFESPIMB040OpticsSetup::unlockStages() {
    ui->stageSetup->unlockStages();
}

void QFESPIMB040OpticsSetup::closeEvent(QCloseEvent * event) {
    ui->camConfig1->close();
    ui->camConfig2->close();
    event->accept();
}

void QFESPIMB040OpticsSetup::showEvent( QShowEvent * event ) {
    ui->camConfig1->show();
    ui->camConfig2->show();
}

QShortcut *QFESPIMB040OpticsSetup::addShortCut(const QString &id, const QString &label, const QKeySequence& sequence) {
    QFESPIMB040OpticsSetup::shortcutItem itm;
    itm.shortcut=new QShortcut(m_pluginMainWidget);
    itm.shortcut->setKey(sequence);
    itm.shortcut->setWhatsThis(label);
    itm.shortcut->setContext(Qt::ApplicationShortcut);
    itm.shortcut->setEnabled(!sequence.isEmpty());
    itm.id=id;
    itm.label=label;
    shortcuts.append(itm);
    return itm.shortcut;
}

void QFESPIMB040OpticsSetup::loadSettings(QSettings& settings, QString prefix) {
    ui->camConfig1->loadSettings(settings, prefix+"cam_config1/");
    ui->camConfig2->loadSettings(settings, prefix+"cam_config2/");
    ui->stageSetup->loadSettings(settings, prefix+"stages/");
    ui->filtDetection11->loadSettings(settings, prefix+"filters/detection11");
    ui->filtDetection21->loadSettings(settings, prefix+"filters/detection21");
    ui->filtSplitter->loadSettings(settings, prefix+"filters/detection_splitter");
    ui->filtTransmission->loadSettings(settings, prefix+"filters/illumination_transmission");
    ui->filtDetection->loadSettings(settings, prefix+"filters/detection");
    ui->objDetection->loadSettings(settings, prefix+"objectives/detection");
    ui->objProjection->loadSettings(settings, prefix+"objectives/projection");
    ui->objTube1->loadSettings(settings, prefix+"objectives/tubelens1");
    ui->objTube2->loadSettings(settings, prefix+"objectives/tubelens2");
    ui->shutterMainIllumination->loadSettings(settings, prefix+"main_illumination_shutter");
    ui->shutterLaser1->loadSettings(settings, prefix+"laser1_shutter");
    ui->shutterLaser2->loadSettings(settings, prefix+"laser2_shutter");
    ui->filtcDetection->loadSettings(settings, prefix+"filterchanger_detection");
    ui->chkDetectionFilterWheel->setChecked(settings.value(prefix+"filterchanger_detection", false).toBool());

    for (int i=0; i<shortcuts.size(); i++) {
        shortcuts[i].shortcut->setKey(QKeySequence(settings.value(prefix+"shortcut_"+shortcuts[i].id, shortcuts[i].shortcut->key().toString()).toString()));
    }
}

void QFESPIMB040OpticsSetup::storeSettings(QSettings& settings, QString prefix) {
    ui->camConfig1->storeSettings(settings, prefix+"cam_config1/");
    ui->camConfig2->storeSettings(settings, prefix+"cam_config2/");
    ui->stageSetup->storeSettings(settings, prefix+"stages/");
    ui->filtDetection11->saveSettings(settings, prefix+"filters/detection11");
    ui->filtDetection21->saveSettings(settings, prefix+"filters/detection21");
    ui->filtSplitter->saveSettings(settings, prefix+"filters/detection_splitter");
    ui->filtTransmission->saveSettings(settings, prefix+"filters/illumination_transmission");
    ui->filtDetection->saveSettings(settings, prefix+"filters/detection");
    ui->objDetection->saveSettings(settings, prefix+"objectives/detection");
    ui->objProjection->saveSettings(settings, prefix+"objectives/projection");
    ui->objTube1->saveSettings(settings, prefix+"objectives/tubelens1");
    ui->objTube2->saveSettings(settings, prefix+"objectives/tubelens2");
    ui->shutterMainIllumination->saveSettings(settings, prefix+"main_illumination_shutter");
    ui->shutterLaser1->saveSettings(settings, prefix+"laser1_shutter");
    ui->shutterLaser2->saveSettings(settings, prefix+"laser2_shutter");
    ui->filtcDetection->saveSettings(settings, prefix+"filterchanger_detection");
    settings.setValue(prefix+"filterchanger_detection", ui->chkDetectionFilterWheel->isChecked());

    for (int i=0; i<shortcuts.size(); i++) {
        settings.setValue(prefix+"shortcut_"+shortcuts[i].id, shortcuts[i].shortcut->key().toString());
    }
}

double QFESPIMB040OpticsSetup::getCameraMagnification(int setup_cam) const {
    if (setup_cam==0) {
        return ui->objDetection->objective().magnification*ui->objTube1->objective().magnification;
    } else if (setup_cam==1) {
        return ui->objDetection->objective().magnification*ui->objTube2->objective().magnification;
    }
    return 1;
}

QMap<QString, QVariant> QFESPIMB040OpticsSetup::getSetup(int setup_cam ) const {
    QMap<QString, QVariant> setup;

    FilterDescription filter=ui->filtDetection->filter();
    if (filter.isValid) {
        setup["filters/detection/name"]=filter.name;
        setup["filters/detection/type"]=filter.type;
        setup["filters/detection/manufacturer"]=filter.manufacturer;
    }

    if (setup_cam<0 || setup_cam==0) {
        filter=ui->filtDetection11->filter();
        if (filter.isValid) {
            setup["filters/detection_cam1/name"]=filter.name;
            setup["filters/detection_cam1/type"]=filter.type;
            setup["filters/detection_cam1/manufacturer"]=filter.manufacturer;
        }
    }

    if (setup_cam<0 || setup_cam==1) {
        filter=ui->filtDetection21->filter();
        if (filter.isValid) {
            setup["filters/detection_cam2/name"]=filter.name;
            setup["filters/detection_cam2/type"]=filter.type;
            setup["filters/detection_cam2/manufacturer"]=filter.manufacturer;
        }
    }

    filter=ui->filtSplitter->filter();
    if (filter.isValid) {
        setup["filters/detection_splitter/name"]=filter.name;
        setup["filters/detection_splitter/type"]=filter.type;
        setup["filters/detection_splitter/manufacturer"]=filter.manufacturer;
    }

    filter=ui->filtTransmission->filter();
    if (filter.isValid) {
        setup["filters/illumination_transmission/name"]=filter.name;
        setup["filters/illumination_transmission/type"]=filter.type;
        setup["filters/illumination_transmission/manufacturer"]=filter.manufacturer;
    }

    ObjectiveDescription objective=ui->objDetection->objective();
    setup["objectives/detection/name"]=objective.name;
    setup["objectives/detection/manufacturer"]=objective.manufacturer;
    setup["objectives/detection/NA"]=objective.NA;
    setup["objectives/detection/magnification"]=objective.magnification;

    objective=ui->objProjection->objective();
    setup["objectives/projection/name"]=objective.name;
    setup["objectives/projection/manufacturer"]=objective.manufacturer;
    setup["objectives/projection/NA"]=objective.NA;
    setup["objectives/projection/magnification"]=objective.magnification;

    if (setup_cam<0 || setup_cam==0) {
        objective=ui->objTube1->objective();
        setup["objectives/tube_lens1/name"]=objective.name;
        setup["objectives/tube_lens1/manufacturer"]=objective.manufacturer;
        setup["objectives/tube_lens1/NA"]=objective.NA;
        setup["objectives/tube_lens1/magnification"]=objective.magnification;
    }

    if (setup_cam<0 || setup_cam==1) {
        objective=ui->objTube2->objective();
        setup["objectives/tube_lens2/name"]=objective.name;
        setup["objectives/tube_lens2/manufacturer"]=objective.manufacturer;
        setup["objectives/tube_lens2/NA"]=objective.NA;
        setup["objectives/tube_lens2/magnification"]=objective.magnification;
    }

    setup["shutters/transmission_illumination/state"]=ui->shutterMainIllumination->getShutterState();
    setup["shutters/laser1/state"]=ui->shutterLaser1->getShutterState();
    setup["shutters/laser2/state"]=ui->shutterLaser2->getShutterState();
    if (ui->chkDetectionFilterWheel->isChecked()) setup["filterchangers/detection/filter"]=ui->filtcDetection->getFilterChangerState();


    return setup;
}


void QFESPIMB040OpticsSetup::setLogging(QFPluginLogService* log) {
    m_log=log;
    ui->camConfig1->setLog(m_log);
    ui->camConfig2->setLog(m_log);
    ui->stageSetup->setLog(m_log);
    ui->shutterMainIllumination->setLog(m_log);
    ui->shutterLaser1->setLog(m_log);
    ui->shutterLaser2->setLog(m_log);
    ui->filtcDetection->setLog(m_log);
}

bool QFESPIMB040OpticsSetup::lockCamera(int setup_cam, QFExtension** extension, QFExtensionCamera** ecamera, int* camera, QString* previewSettingsFilename) {
    switch(setup_cam) {
        case 0: return ui->camConfig1->lockCamera(extension, ecamera, camera,  previewSettingsFilename);
        case 1: return ui->camConfig2->lockCamera(extension, ecamera, camera,  previewSettingsFilename);
    }
    return false;
}

void QFESPIMB040OpticsSetup::releaseCamera(int setup_cam) {
    switch(setup_cam) {
        case 0: ui->camConfig1->releaseCamera(); break;
        case 1: ui->camConfig2->releaseCamera(); break;
    }
}

void QFESPIMB040OpticsSetup::on_btnConnectDevices_clicked() {
    ui->shutterMainIllumination->connectShutter();
    ui->shutterLaser1->connectShutter();
    ui->shutterLaser2->connectShutter();
    ui->stageSetup->connectStages();
    if (ui->chkDetectionFilterWheel->isChecked()) ui->filtcDetection->connectFilterChanger();
}

void QFESPIMB040OpticsSetup::on_btnConnectCameras_clicked() {
    ui->camConfig1->connectCamera();
    ui->camConfig2->connectCamera();
}

void QFESPIMB040OpticsSetup::on_btnDisconnectDevices_clicked() {
    ui->stageSetup->disconnectStages();
    ui->shutterMainIllumination->disconnectShutter();
    ui->shutterLaser1->disconnectShutter();
    ui->shutterLaser2->disconnectShutter();
    if (ui->chkDetectionFilterWheel->isChecked()) ui->filtcDetection->disconnectFilterChanger();
}

void QFESPIMB040OpticsSetup::on_btnDisconnectCameras_clicked() {
    ui->camConfig1->disconnectCamera();
    ui->camConfig2->disconnectCamera();
}

void QFESPIMB040OpticsSetup::on_chkDetectionFilterWheel_toggled(bool checked) {
    if (!checked) {
        ui->filtcDetection->disconnectFilterChanger();
    }
}

void QFESPIMB040OpticsSetup::configsChanged(QFESPIMB040OpticsSetupItems configs) {
    emit lightpathesChanged(configs);
}

void QFESPIMB040OpticsSetup::configShortcuts() {
    QFESPIMB040ShortcutConfigDialog* dlg=new QFESPIMB040ShortcutConfigDialog(this);

    for (int i=0; i<shortcuts.size(); i++) {
        dlg->addShortcut(shortcuts[i].label, shortcuts[i].shortcut->key());
    }

    if (dlg->exec()==QDialog::Accepted) {
        for (int i=0; i<shortcuts.size(); i++) {
            QKeySequence seq=dlg->getSequence(i);
            shortcuts[i].shortcut->setKey(seq);
            shortcuts[i].shortcut->setEnabled(!seq.isEmpty());
        }
    }
    delete dlg;
}


void QFESPIMB040OpticsSetup::updateMagnifications() {
    double m=ui->objDetection->objective().magnification*ui->objTube1->objective().magnification;
    ui->camConfig1->setMagnification(m);
    m=ui->objDetection->objective().magnification*ui->objTube2->objective().magnification;
    ui->camConfig2->setMagnification(m);
}


QFExtensionLinearStage* QFESPIMB040OpticsSetup::getXStage() {
    return ui->stageSetup->getXStage();
}

QFExtensionLinearStage* QFESPIMB040OpticsSetup::getYStage() {
    return ui->stageSetup->getYStage();
}

QFExtensionLinearStage* QFESPIMB040OpticsSetup::getZStage() {
    return ui->stageSetup->getZStage();
}

QFExtension* QFESPIMB040OpticsSetup::getXStageExtension() {
    return ui->stageSetup->getXStageExtension();
}

QFExtension* QFESPIMB040OpticsSetup::getYStageExtension() {
    return ui->stageSetup->getYStageExtension();
}

QFExtension* QFESPIMB040OpticsSetup::getZStageExtension() {
    return ui->stageSetup->getZStageExtension();
}

int QFESPIMB040OpticsSetup::getXStageAxis() {
    return ui->stageSetup->getXStageAxis();
}

int QFESPIMB040OpticsSetup::getYStageAxis() {
    return ui->stageSetup->getYStageAxis();
}

int QFESPIMB040OpticsSetup::getZStageAxis() {
    return ui->stageSetup->getZStageAxis();
}

bool QFESPIMB040OpticsSetup::isXStageConnected() const {
    return ui->stageSetup->isXStageConnected();
}

bool QFESPIMB040OpticsSetup::isYStageConnected() const {
    return ui->stageSetup->isYStageConnected();
}

bool QFESPIMB040OpticsSetup::isZStageConnected() const {
    return ui->stageSetup->isZStageConnected();
}

bool QFESPIMB040OpticsSetup::isStageConnected(QFExtensionLinearStage* stage, int id, bool& found) {
    found=false;
    if (!stage || id<0) return false;
    found=true;
    if (stage==getXStage() && id==getXStageAxis()) return isXStageConnected();
    if (stage==getYStage() && id==getYStageAxis()) return isYStageConnected();
    if (stage==getZStage() && id==getZStageAxis()) return isZStageConnected();
    found=false;
    return false;
}

QFCameraComboBox* QFESPIMB040OpticsSetup::cameraComboBox(int camera) const {
    if (camera==0) return ui->camConfig1->cameraComboBox();
    if (camera==1) return ui->camConfig2->cameraComboBox();

    return NULL;
}

QFCameraConfigComboBoxStartResume* QFESPIMB040OpticsSetup::getStopRelease(int camera) const {
    if (camera==0) return ui->camConfig1;
    if (camera==1) return ui->camConfig2;

    return NULL;
}

void QFESPIMB040OpticsSetup::setMainIlluminationShutter(bool opened) {
    ui->shutterMainIllumination->setShutter(opened);
}

bool QFESPIMB040OpticsSetup::getMainIlluminationShutter() {
    return ui->shutterMainIllumination->getShutterState();
}

bool QFESPIMB040OpticsSetup::lightpathLoaded(const QString &filename) {
    if (setting_lightpath) {
        bool ok=true;
        QSettings set(filename, QSettings::IniFormat);
        if (ok && set.contains("laser1/shutter/state")) ok=ok&&ui->shutterLaser1->isShutterDone()&&(ui->shutterLaser1->getShutterState()==set.value("laser1/shutter/state").toBool());
        if (ok && set.contains("laser2/shutter/state")) ok=ok&&ui->shutterLaser2->isShutterDone()&&(ui->shutterLaser2->getShutterState()==set.value("laser2/shutter/state").toBool());
        if (ok && set.contains("detection/filterchanger/filter") && ui->chkDetectionFilterWheel->isChecked())
            ok=ok&&(ui->filtcDetection->getFilterChangerState()==set.value("detection/filterchanger/filter").toInt());
        return ok;
    }
    return true;
}

QString QFESPIMB040OpticsSetup::getCurrentLightpathFilename() const {
    return ui->cmbLightpathConfig->currentConfigFilename();
}

QString QFESPIMB040OpticsSetup::getCurrentLightpath() const {
    return ui->cmbLightpathConfig->currentText();
}

void QFESPIMB040OpticsSetup::loadLightpathConfig(const QString &filename, bool waiting) {
    if (!QFile::exists(filename)) return;
    //qDebug()<<"loadLightpathConfig("<<filename<<")";
    QSettings set(filename, QSettings::IniFormat);

    // LOAD RELEVANT WIDGETS HERE
    if (set.contains("laser1/shutter/state")) ui->shutterLaser1->setShutter(set.value("laser1/shutter/state").toBool());
    if (set.contains("laser2/shutter/state")) ui->shutterLaser2->setShutter(set.value("laser2/shutter/state").toBool());
    if (set.contains("detection/filterchanger/filter") && ui->chkDetectionFilterWheel->isChecked()) ui->filtcDetection->setFilterChanger(set.value("detection/filterchanger/filter").toInt());

    if (waiting) {
        setting_lightpath=true;
        QTime t1;
        t1.start();
        while (!lightpathLoaded(filename)) {
            if (t1.elapsed()>20) {
                QApplication::processEvents();
                t1.start();
            }
        }
        setting_lightpath=false;
    } else {
        setting_lightpath=true;
    }

}

void QFESPIMB040OpticsSetup::saveLightpathConfig(const QString &filename, const QString& name) {
    { // this block ensures that set is destroyed (and the file written) before updateItems() is called
        QSettings set(filename, QSettings::IniFormat);
        set.setValue("name", name);


        QMap<QString, QVariant> data;
        saveLightpathConfig(data, name);
        QMap<QString, QVariant>::iterator i=data.begin();
        for (i = data.begin(); i != data.end(); ++i) {
            set.setValue(i.key(), i.value());
        }

    }
    ui->cmbLightpathConfig->updateItems(name);
}

void QFESPIMB040OpticsSetup::saveLightpathConfig(QMap<QString, QVariant> &data, const QString &name, const QString& prefix) {
    data[prefix+"name"]=name;

    // SAVE RELEVANT WIDGETS HERE
    if (ui->shutterLaser1->getShutter() && ui->shutterLaser1->getShutter()->isShutterConnected(ui->shutterLaser1->getShutterID()))
        data["laser1/shutter/state"]=ui->shutterLaser1->getShutterState();
    if (ui->shutterLaser2->getShutter() && ui->shutterLaser2->getShutter()->isShutterConnected(ui->shutterLaser2->getShutterID()))
        data["laser2/shutter/state"]=ui->shutterLaser2->getShutterState();
    if (ui->chkDetectionFilterWheel->isChecked() && ui->filtcDetection->getFilterChanger() && ui->filtcDetection->getFilterChanger()->isFilterChangerConnected(ui->filtcDetection->getFilterChangerID()))
        data["detection/filterchanger/filter"]=ui->filtcDetection->getFilterChangerState();

}

void QFESPIMB040OpticsSetup::saveCurrentLightpatConfig() {
    bool dlgOK=true;
    QString name="";
    while (true) {
        QStringList l=ui->cmbLightpathConfig->getConfigs();
        int idx=-1;
        if (!name.isEmpty() && !l.contains(name)) {
            l.append(name);
        }
        idx=l.indexOf(name);
        name=QInputDialog::getItem(this, tr("B040 SPIM"),
                                      tr("Enter a name for the lightpath:"), l,idx, true, &dlgOK);
        if (dlgOK) {
            if (!name.isEmpty()) {
                QString filename=m_pluginServices->getConfigFileDirectory()+"/plugins/ext_spimb040/"+cleanStringForFilename(name)+".lpc";
                if (QFile::exists(filename)) {
                    QMessageBox::StandardButton res=QMessageBox::question(this, tr("B040 SPIM"),
                                                                          tr("A lightpath config with the name '%1' already exists.\n  filename: '%2'\nOverwrite?").arg(name).arg(filename),
                                                                          QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel, QMessageBox::No);
                    if (res==QMessageBox::Yes) {
                        saveLightpathConfig(filename, name);
                        return;
                    } else if (res==QMessageBox::Cancel) {
                        return;
                    }
                } else {
                    saveLightpathConfig(filename, name);
                    return;
                }
            }
        }
    }
}

void QFESPIMB040OpticsSetup::deleteCurrentLightpatConfig() {
    QString fn=ui->cmbLightpathConfig->currentConfigFilename();
    QString name=ui->cmbLightpathConfig->currentConfig();
    if (QFile::exists(fn)) {
        if (QMessageBox::question(this, tr("B040 SPIM"), tr("Do you really want to delete lightpath '%1'?\nfile: '%2'").arg(name).arg(fn),
                                  QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::Yes)
        {
            QFile::remove(fn);
            ui->cmbLightpathConfig->updateItems();
        }
    }
}

void QFESPIMB040OpticsSetup::emitLighpathesChanged() {
    ui->cmbLightpathConfig->updateItems(ui->cmbLightpathConfig->currentText());
}

bool QFESPIMB040OpticsSetup::isMainIlluminationShutterAvailable()  {
    return ui->shutterMainIllumination->isShutterConnected();
}

void QFESPIMB040OpticsSetup::lockLightpath() {
    ui->shutterLaser1->lockShutters();
    ui->shutterLaser2->lockShutters();
    ui->filtcDetection->lockFilterChangers();
}

void QFESPIMB040OpticsSetup::unlockLightpath() {
    ui->shutterLaser1->unlockShutters();
    ui->shutterLaser2->unlockShutters();
    ui->filtcDetection->unlockFilterChangers();
}
