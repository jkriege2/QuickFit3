#include "qfespimb040scriptedacquisitiontools.h"
#include "qfpluginservices.h"
#include "qfstagecombobox.h"
#include "qfextensionmanager.h"
#include "../interfaces/qfextensionlinearstage.h"
#include "qfextension.h"
#include <QtGui>
#include <QtCore>
#include "qfcompleterfromfile.h"
#include "qfespimb040mainwindow2.h"
#include "qfespimb040scriptedacquisition.h"




QFESPIMB040ScriptedAcquisitionTools::QFESPIMB040ScriptedAcquisitionTools(QFESPIMB040ScriptedAcquisition* widAcquisition, QFESPIMB040MainWindow2 *mainWindow, QFESPIMB040AcquisitionTools *acqTools, QFPluginLogService *log, QObject *parent, QFPluginServices *pluginServices, QFESPIMB040OpticsSetupBase *opticsSetup, QFESPIMB040AcquisitionDescription *acqDescription, QFESPIMB040ExperimentDescription *expDescription):
    QObject(parent)
{
    this->widAcquisition=widAcquisition;
    this->mainWindow=mainWindow;
    this->acqTools=acqTools;
    this->log=log;
    this->m_pluginServices= pluginServices;
    this->opticsSetup=opticsSetup;
    this->acqDescription=acqDescription;
    this->expDescription=expDescription;
}

void QFESPIMB040ScriptedAcquisitionTools::logText(const QString &text)
{
    log->log_text(text);
    QApplication::processEvents();
}

void QFESPIMB040ScriptedAcquisitionTools::logError(const QString &text)
{
    log->log_error(text);
    QApplication::processEvents();
}

void QFESPIMB040ScriptedAcquisitionTools::logWarning(const QString &text)
{
    log->log_warning(text);
    QApplication::processEvents();
}

void QFESPIMB040ScriptedAcquisitionTools::setStatus(const QString &text)
{
    widAcquisition->setStatus(text);
    QApplication::processEvents();
}

void QFESPIMB040ScriptedAcquisitionTools::sleepMS(int duration)
{
    QElapsedTimer t;
    t.start();
    while (t.elapsed()<duration) {
        QApplication::processEvents();
    }
}

void QFESPIMB040ScriptedAcquisitionTools::createTextFile(const QString &filename)
{
    QFile f(filename);
    if (f.open(QIODevice::WriteOnly|QIODevice::Text)) {
        f.close();
    } else {
        throwEngineException(tr("\ncreateTextFile(): Could not write to file '%1':\n   %2\n").arg(filename).arg(f.errorString()));
    }
}

void QFESPIMB040ScriptedAcquisitionTools::appendTextFile(const QString &filename, const QString &data)
{
    QFile f(filename);
    if (f.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Append)) {
        QTextStream out(&f);
        out<<data;
        f.close();
    } else {
        throwEngineException(tr("\ncreateTextFile(): Could not write to file '%1':\n   %2\n").arg(filename).arg(f.errorString()));
    }
}

void QFESPIMB040ScriptedAcquisitionTools::showMessageBox(const QString &text)
{
    QMessageBox::information(widAcquisition, tr("JavaScript Message Box"), text);
}







QFESPIMB040ScriptedAcquisitionInstrumentControl::QFESPIMB040ScriptedAcquisitionInstrumentControl(QFESPIMB040ScriptedAcquisition* widAcquisition, QFESPIMB040MainWindow2 *mainWindow, QFESPIMB040AcquisitionTools *acqTools, QFPluginLogService *log, QObject *parent, QFPluginServices *pluginServices, QFESPIMB040OpticsSetupBase *opticsSetup, QFESPIMB040AcquisitionDescription *acqDescription, QFESPIMB040ExperimentDescription *expDescription):
    QObject(parent)
{
    this->widAcquisition=widAcquisition;
    this->mainWindow=mainWindow;
    this->acqTools=acqTools;
    this->log=log;
    this->m_pluginServices= pluginServices;
    this->opticsSetup=opticsSetup;
    this->acqDescription=acqDescription;
    this->expDescription=expDescription;
}

void QFESPIMB040ScriptedAcquisitionInstrumentControl::setLaserIntensity(int laser, int line, double intensity)
{
    if (laser>=0 && laser<opticsSetup->getLaserCount()) {
        opticsSetup->getLaser(laser)->setLightSourcePower(opticsSetup->getLaserID(laser), line, intensity);
    } else {
        throwEngineException(QObject::tr("setLaserIntensity(laser=%1, %2, %3): unknown laser!").arg(laser).arg(line).arg(intensity));
    }
}

void QFESPIMB040ScriptedAcquisitionInstrumentControl::setBrightfieldIntensity(int lightsource, int line, double intensity)
{
    if (lightsource>=0 && lightsource<opticsSetup->getBrightfieldLightSourceCount()) {
        opticsSetup->getBrightfieldLightSource(lightsource)->setLightSourcePower(opticsSetup->getBrightfieldLightSourceID(lightsource), line, intensity);
    } else {
        throwEngineException(QObject::tr("setBrightfieldIntensity(lightSource=%1, %2, %3): unknown light source!").arg(lightsource).arg(line).arg(intensity));
    }
}

void QFESPIMB040ScriptedAcquisitionInstrumentControl::setShutter(const QString& shutter, bool state)
{
    if (shutter=="main") {
        opticsSetup->setMainIlluminationShutter(state, true);
    } else if (shutter=="laser1") {
        opticsSetup->setSpecialShutter(QFESPIMB040OpticsSetupBase::ShutterLaser1, state, true);
    } else if (shutter=="laser2") {
        opticsSetup->setSpecialShutter(QFESPIMB040OpticsSetupBase::ShutterLaser2, state, true);
    } else if (shutter=="transmission") {
        opticsSetup->setSpecialShutter(QFESPIMB040OpticsSetupBase::ShutterTransmission, state, true);
    } else {
        bool ok=false;
        int ID=shutter.toInt(&ok);
        if (ok && ID>=0 && ID<opticsSetup->getShutterCount()) {
            opticsSetup->setSpecialShutter(ID, state, true);
        } else {
            throwEngineException(QObject::tr("setShutter('%1'): unknown shutter!").arg(shutter));
        }
    }
}


void QFESPIMB040ScriptedAcquisitionInstrumentControl::moveStageRel(const QString &stage, double newPos)
{
    QFExtensionLinearStage*  stageE=NULL;
    int axis=0;
    if (stage=="x") {
        stageE=opticsSetup->getStage(QFESPIMB040OpticsSetupBase::StageX);
        axis=opticsSetup->getStageAxis(QFESPIMB040OpticsSetupBase::StageX);
    } else if (stage=="y") {
        stageE=opticsSetup->getStage(QFESPIMB040OpticsSetupBase::StageY);
        axis=opticsSetup->getStageAxis(QFESPIMB040OpticsSetupBase::StageY);
    } else if (stage=="z") {
        stageE=opticsSetup->getStage(QFESPIMB040OpticsSetupBase::StageZ);
        axis=opticsSetup->getStageAxis(QFESPIMB040OpticsSetupBase::StageZ);
    } else {
        bool ok=false;
        int ID=stage.toInt(&ok);
        if (ok && ID>=0 && ID<opticsSetup->getStageCount()) {
            stageE=opticsSetup->getStage(ID);
            axis=opticsSetup->getStageAxis(ID);
        } else {
            throwEngineException(QObject::tr("moveStageRel('%1', %2): unknown stage!").arg(stage).arg(newPos));
            return;
        }
    }

    stageE->move(axis, stageE->getPosition(axis)+newPos);
    QElapsedTimer t;
    t.start();
    while (stageE->getAxisState(axis)==QFExtensionLinearStage::Moving && t.elapsed()<20000) {
        QApplication::processEvents();
    }
    if (t.elapsed()>=20000) {
        throwEngineException(QObject::tr("moveStageRel('%1', %2): move timed out after 20s!").arg(stage).arg(newPos));
    }

}

void QFESPIMB040ScriptedAcquisitionInstrumentControl::moveStageAbs(const QString &stage, double newPos)
{
    QFExtensionLinearStage*  stageE=NULL;
    int axis=0;
    if (stage=="x") {
        stageE=opticsSetup->getStage(QFESPIMB040OpticsSetupBase::StageX);
        axis=opticsSetup->getStageAxis(QFESPIMB040OpticsSetupBase::StageX);
    } else if (stage=="y") {
        stageE=opticsSetup->getStage(QFESPIMB040OpticsSetupBase::StageY);
        axis=opticsSetup->getStageAxis(QFESPIMB040OpticsSetupBase::StageY);
    } else if (stage=="z") {
        stageE=opticsSetup->getStage(QFESPIMB040OpticsSetupBase::StageZ);
        axis=opticsSetup->getStageAxis(QFESPIMB040OpticsSetupBase::StageZ);
    } else {
        bool ok=false;
        int ID=stage.toInt(&ok);
        if (ok && ID>=0 && ID<opticsSetup->getStageCount()) {
            stageE=opticsSetup->getStage(ID);
            axis=opticsSetup->getStageAxis(ID);
        } else {
            throwEngineException(QObject::tr("moveStageAbs('%1', %2): unknown stage!").arg(stage).arg(newPos));
            return;
        }
    }

    stageE->move(axis, newPos);
    QElapsedTimer t;
    t.start();
    while (stageE->getAxisState(axis)==QFExtensionLinearStage::Moving && t.elapsed()<20000) {
        QApplication::processEvents();
    }
    if (t.elapsed()>=20000) {
        throwEngineException(QObject::tr("moveStageAbs('%1', %2): move timed out after 20s!").arg(stage).arg(newPos));
    }
}

QVariant QFESPIMB040ScriptedAcquisitionInstrumentControl::getStagePos(const QString &stage)
{
    QFExtensionLinearStage*  stageE=NULL;
    int axis=0;
    if (stage=="x") {
        stageE=opticsSetup->getStage(QFESPIMB040OpticsSetupBase::StageX);
        axis=opticsSetup->getStageAxis(QFESPIMB040OpticsSetupBase::StageX);
    } else if (stage=="y") {
        stageE=opticsSetup->getStage(QFESPIMB040OpticsSetupBase::StageY);
        axis=opticsSetup->getStageAxis(QFESPIMB040OpticsSetupBase::StageY);
    } else if (stage=="z") {
        stageE=opticsSetup->getStage(QFESPIMB040OpticsSetupBase::StageZ);
        axis=opticsSetup->getStageAxis(QFESPIMB040OpticsSetupBase::StageZ);
    } else {
        bool ok=false;
        int ID=stage.toInt(&ok);
        if (ok && ID>=0 && ID<opticsSetup->getStageCount()) {
            stageE=opticsSetup->getStage(ID);
            axis=opticsSetup->getStageAxis(ID);
        } else {
            throwEngineException(QObject::tr("getStagePos('%1'): unknown stage!").arg(stage));
            return QVariant();
        }
    }

    return stageE->getPosition(axis);
}

void QFESPIMB040ScriptedAcquisitionInstrumentControl::setFilterWheel(const QString &wheel, int filter)
{
    QFExtensionFilterChanger* changer=NULL;
    int ID=0;
    if (wheel=="detection") {
        changer=opticsSetup->getFilterChanger(QFESPIMB040OpticsSetupBase::FilterChangerDetection);
        ID=opticsSetup->getFilterChangerID(QFESPIMB040OpticsSetupBase::FilterChangerDetection);
    } else {
        bool ok=false;
        int ID=wheel.toInt(&ok);
        if (ok && ID>=0 && ID<opticsSetup->getFilterChangerCount()) {
            changer=opticsSetup->getFilterChanger(ID);
            ID=opticsSetup->getFilterChangerID(ID);
        } else {
            throwEngineException(QObject::tr("setFilterWheel('%1', %2): unknown filter wheel!").arg(wheel).arg(filter));
            return;
        }
    }
    changer->setFilterChangerFilter(ID, filter);
    QElapsedTimer t;
    t.start();
    while (!changer->isLastFilterChangerActionFinished(ID) && t.elapsed()<20000) {

    }
    if (t.elapsed()>=20000) {
        throwEngineException(QObject::tr("setFilterWheel('%1', %2): move timed out after 20s!").arg(wheel).arg(filter));
    }
}

void QFESPIMB040ScriptedAcquisitionInstrumentControl::setAcqDescCurrentSample(const QString &name)
{
    acqDescription->setCurrentSample(name);
}

void QFESPIMB040ScriptedAcquisitionInstrumentControl::setAcqDescPrefix(const QString &name)
{
    acqDescription->setPrefix(name);
}

void QFESPIMB040ScriptedAcquisitionInstrumentControl::setAcqDescPlate(int p)
{
    acqDescription->setPlate(p);
}

void QFESPIMB040ScriptedAcquisitionInstrumentControl::setAcqDescWell(int p)
{
    acqDescription->setWell(p);
}

void QFESPIMB040ScriptedAcquisitionInstrumentControl::setAcqDescCell(int p)
{
    acqDescription->setCell(p);
}

void QFESPIMB040ScriptedAcquisitionInstrumentControl::setAcqDescComment(const QString &name)
{
    acqDescription->setComment(name);
}

int QFESPIMB040ScriptedAcquisitionInstrumentControl::getAcqDescCell() const
{
    return acqDescription->getCell();
}

int QFESPIMB040ScriptedAcquisitionInstrumentControl::getAcqDescWell() const
{
    return acqDescription->getWell();
}

int QFESPIMB040ScriptedAcquisitionInstrumentControl::getAcqDescPlate() const
{
    return acqDescription->getPlate();
}

QString QFESPIMB040ScriptedAcquisitionInstrumentControl::getAcqDescComment() const
{
    return acqDescription->getComment();
}

QString QFESPIMB040ScriptedAcquisitionInstrumentControl::getAcqDescCurrentSample() const
{
    return acqDescription->getSample();
}

QString QFESPIMB040ScriptedAcquisitionInstrumentControl::getAcqDescPrefix() const
{
    return acqDescription->getPrefix();
}

void QFESPIMB040ScriptedAcquisitionInstrumentControl::MDConnect(const QString &device_name, int id)
{
    QFExtensionMeasurementAndControlDevice* md=getMDevice(device_name);
    if (md && id<md->getMeasurementDeviceCount()) {
        md->connectMeasurementDevice(id);
        if (!md->isMeasurementDeviceConnected(id)) {
            throwEngineException(tr("could not connect to measurement dveice '%1', %2").arg(device_name).arg(id));
        }
    } else {
        throwEngineException(tr("no measurement device '%1', %2 available").arg(device_name).arg(id));
    }
}

void QFESPIMB040ScriptedAcquisitionInstrumentControl::MDDisconnect(const QString &device_name, int id)
{
    QFExtensionMeasurementAndControlDevice* md=getMDevice(device_name);
    if (md && id<md->getMeasurementDeviceCount()) {
        md->disconnectMeasurementDevice(id);
    } else {
        throwEngineException(tr("no measurement device '%1', %2 available").arg(device_name).arg(id));
    }
}

QVariant QFESPIMB040ScriptedAcquisitionInstrumentControl::MDGet(const QString &device_name, int id, int parameter)
{
    QFExtensionMeasurementAndControlDevice* md=getMDevice(device_name);
    if (md && id<md->getMeasurementDeviceCount()) {
        return md->getMeasurementDeviceValue(id, parameter);
    } else {
        throwEngineException(tr("no measurement device '%1', %2 available").arg(device_name).arg(id));
    }
    return QVariant();
}

void QFESPIMB040ScriptedAcquisitionInstrumentControl::MDSet(const QString &device_name, int id, int parameter, const QVariant &value)
{
    QFExtensionMeasurementAndControlDevice* md=getMDevice(device_name);
    if (md && id<md->getMeasurementDeviceCount()) {
        if (md->isMeasurementDeviceValueEditable(id, parameter)) {
            md->setMeasurementDeviceValue(id, parameter, value);
        } else {
            throwEngineException(tr("in measurement device '%1', %2: parameter '%3' is not editable!").arg(device_name).arg(id).arg(md->getMeasurementDeviceValueShortName(id, parameter)));
        }
    } else {
        throwEngineException(tr("no measurement device '%1', %2 available").arg(device_name).arg(id));
    }
}

QVariant QFESPIMB040ScriptedAcquisitionInstrumentControl::MDGetN(const QString &device_name, int id, const QString & parameter)
{
    QFExtensionMeasurementAndControlDevice* md=getMDevice(device_name);
    if (md) {
        for (int i=0; i<md->getMeasurementDeviceValueCount(id); i++) {
            if (parameter==md->getMeasurementDeviceValueShortName(id, i)) {
                return md->getMeasurementDeviceValue(id, i);
            }
        }
        throwEngineException(tr("measurement device '%1', %2 does not contain a parameter named '%3'").arg(device_name).arg(id).arg(parameter));
    } else {
        throwEngineException(tr("no measurement device '%1', %2 available").arg(device_name).arg(id));
    }
    return QVariant();
}

void QFESPIMB040ScriptedAcquisitionInstrumentControl::MDSetN(const QString &device_name, int id, const QString &parameter, const QVariant &value)
{
    QFExtensionMeasurementAndControlDevice* md=getMDevice(device_name);
    if (md) {
        for (int i=0; i<md->getMeasurementDeviceValueCount(id); i++) {
            if (parameter==md->getMeasurementDeviceValueShortName(id, i)) {
                if (md->isMeasurementDeviceValueEditable(id, i)) {
                    md->setMeasurementDeviceValue(id, i, value);
                    return;
                } else {
                    throwEngineException(tr("in measurement device '%1', %2: parameter '%3' is not editable!").arg(device_name).arg(id).arg(md->getMeasurementDeviceValueShortName(id, i)));
                }
            }
        }
        throwEngineException(tr("measurement device '%1', %2 does not contain a parameter named '%3'").arg(device_name).arg(id).arg(parameter));
    } else {
        throwEngineException(tr("no measurement device '%1', %2 available").arg(device_name).arg(id));
    }

}

int QFESPIMB040ScriptedAcquisitionInstrumentControl::MDGetParamCount(const QString &device_name, int id)
{
    QFExtensionMeasurementAndControlDevice* md=getMDevice(device_name);
    if (md && id<md->getMeasurementDeviceCount()) {
        return md->getMeasurementDeviceValueCount(id);
    } else {
        throwEngineException(tr("no measurement device '%1', %2 available").arg(device_name).arg(id));
    }
    return 0;
}

QString QFESPIMB040ScriptedAcquisitionInstrumentControl::MDGetParamName(const QString &device_name, int id, int parameter)
{
    QFExtensionMeasurementAndControlDevice* md=getMDevice(device_name);
    if (md && id<md->getMeasurementDeviceCount()) {
        return md->getMeasurementDeviceValueShortName(id, parameter);
    } else {
        throwEngineException(tr("no measurement device '%1', %2 available").arg(device_name).arg(id));
    }
    return QString("");

}

QFExtensionMeasurementAndControlDevice *QFESPIMB040ScriptedAcquisitionInstrumentControl::getMDevice(const QString &device_name)
{
    QFExtensionManager* em=QFPluginServices::getInstance()->getExtensionManager();
    if (em) {
        QFExtension* ex=em->getInstance(device_name);
        return dynamic_cast<QFExtensionMeasurementAndControlDevice*>(ex);
    }
    return NULL;
}
















QFESPIMB040ScriptedAcquisitionAcquisitionControl::QFESPIMB040ScriptedAcquisitionAcquisitionControl(QFESPIMB040ScriptedAcquisition* widAcquisition, QFESPIMB040MainWindow2 *mainWindow, QFESPIMB040AcquisitionTools *acqTools, QFPluginLogService *log, QObject *parent, QFPluginServices *pluginServices, QFESPIMB040OpticsSetupBase *opticsSetup, QFESPIMB040AcquisitionDescription *acqDescription, QFESPIMB040ExperimentDescription *expDescription):
    QObject(parent)
{
    this->widAcquisition=widAcquisition;
    this->mainWindow=mainWindow;
    this->acqTools=acqTools;
    this->log=log;
    this->m_pluginServices= pluginServices;
    this->opticsSetup=opticsSetup;
    this->acqDescription=acqDescription;
    this->expDescription=expDescription;
}

void QFESPIMB040ScriptedAcquisitionAcquisitionControl::doAcquisition()
{
    mainWindow->doAcquisition();
}

void QFESPIMB040ScriptedAcquisitionAcquisitionControl::doImageStack()
{
    mainWindow->doImageStack();
}

void QFESPIMB040ScriptedAcquisitionAcquisitionControl::acquirePreviewFrames(int camera, int N, const QString &filename)
{
    widAcquisition->updateReplaces();
    const QString fn=widAcquisition->transformFilename(filename);
    qDebug()<<filename;
    qDebug()<<fn;
    mainWindow->savePreviewMovie(camera, N, fn);
}

void QFESPIMB040ScriptedAcquisitionAcquisitionControl::acquirePreviewFramesWithParams(int camera, int N, const QString &filename)
{
    widAcquisition->updateReplaces();
    const QString fn=widAcquisition->transformFilename(filename);
    qDebug()<<filename;
    qDebug()<<fn;
    mainWindow->savePreviewMovie(camera, N, fn, preview_params);
}

void QFESPIMB040ScriptedAcquisitionAcquisitionControl::setAcquisitionConfigIndex(int camera, int id)
{
    mainWindow->getWidAcquisition()->setCameraConfigAcq(camera, id);
}

void QFESPIMB040ScriptedAcquisitionAcquisitionControl::setAcquisitionConfig(int camera, const QString &id)
{
    mainWindow->getWidAcquisition()->setCameraConfigAcq(camera, id);
}

void QFESPIMB040ScriptedAcquisitionAcquisitionControl::setAcquisitionOverviewConfigIndex(int camera, int overview, int id)
{
    mainWindow->getWidAcquisition()->setCameraConfigOverview(camera, overview, id);
}

void QFESPIMB040ScriptedAcquisitionAcquisitionControl::setAcquisitionOverviewConfig(int camera, int overview, const QString &id)
{
    mainWindow->getWidAcquisition()->setCameraConfigOverview(camera, overview, id);
}

void QFESPIMB040ScriptedAcquisitionAcquisitionControl::setPreviewConfigIndex(int camera, int id)
{
    if (opticsSetup->cameraConfig(camera)) {
        opticsSetup->cameraConfig(camera)->setCurrentPreview(id);
    }
}

void QFESPIMB040ScriptedAcquisitionAcquisitionControl::setPreviewConfig(int camera, const QString &id)
{
    if (opticsSetup->cameraConfig(camera)) {
        opticsSetup->cameraConfig(camera)->setCurrentPreview(id);
    }
}

QStringList QFESPIMB040ScriptedAcquisitionAcquisitionControl::getPreviewConfigs(int camera)
{
    if (opticsSetup->cameraConfig(camera)) {
        QStringList sl=opticsSetup->cameraConfig(camera)->getPreviewConfigs();
        return sl;
    }
    return QStringList();
}

int QFESPIMB040ScriptedAcquisitionAcquisitionControl::getPreviewConfigsCount(int camera)
{
    if (opticsSetup->cameraConfig(camera)) {
        QStringList sl=opticsSetup->cameraConfig(camera)->getPreviewConfigs();
        return sl.size();
    }
    return 0;
}

QString QFESPIMB040ScriptedAcquisitionAcquisitionControl::getPreviewConfig(int camera, int id)
{
    if (opticsSetup->cameraConfig(camera)) {
        QStringList sl=opticsSetup->cameraConfig(camera)->getPreviewConfigs();
        return sl.value(id, "");
    }
    return QString();
}

void QFESPIMB040ScriptedAcquisitionAcquisitionControl::clearPreviewParams()
{
    preview_params.clear();
}

void QFESPIMB040ScriptedAcquisitionAcquisitionControl::setPreviewParam(const QString &name, const QVariant &value)
{
    preview_params.insert(name, value);
}


void QFESPIMB040ScriptedAcquisitionAcquisitionControl::setAcquisitionCamera1Setting(const QString which, QVariant value)
{
    QFExtensionCamera::CameraSetting set=QStringToSetting(which);
    if (set!=QFExtensionCamera::CamSetUnknown) {
        mainWindow->getWidAcquisition()->setCameraSetting1(set, value);
    }

}

void QFESPIMB040ScriptedAcquisitionAcquisitionControl::setAcquisitionCamera1SettingN(const QString which, QVariant value)
{
    mainWindow->getWidAcquisition()->setCameraSetting1(which, value);
}

void QFESPIMB040ScriptedAcquisitionAcquisitionControl::clearAcquisitionCamera1Settings()
{
    mainWindow->getWidAcquisition()->clearCameraSettings1();
}

QVariant QFESPIMB040ScriptedAcquisitionAcquisitionControl::getAcquisitionCamera1Setting(const QString which)
{
    QFExtensionCamera::CameraSetting set=QStringToSetting(which);
    if (set!=QFExtensionCamera::CamSetUnknown) {
        return mainWindow->getWidAcquisition()->getCameraSetting1(set);
    }
    return QVariant();
}


QVariant QFESPIMB040ScriptedAcquisitionAcquisitionControl::getAcquisitionCamera1SettingN(const QString which)
{
    return mainWindow->getWidAcquisition()->getCameraSetting1(which);
}



void QFESPIMB040ScriptedAcquisitionAcquisitionControl::setAcquisitionCamera2Setting(const QString which, QVariant value)
{
    QFExtensionCamera::CameraSetting set=QStringToSetting(which);
    if (set!=QFExtensionCamera::CamSetUnknown) {
        mainWindow->getWidAcquisition()->setCameraSetting2(set, value);
    }

}

void QFESPIMB040ScriptedAcquisitionAcquisitionControl::setAcquisitionCamera2SettingN(const QString which, QVariant value)
{
    qDebug()<<which<<" = "<<value;
    mainWindow->getWidAcquisition()->setCameraSetting2(which, value);
}

void QFESPIMB040ScriptedAcquisitionAcquisitionControl::clearAcquisitionCamera2Settings()
{
    mainWindow->getWidAcquisition()->clearCameraSettings2();
}

QVariant QFESPIMB040ScriptedAcquisitionAcquisitionControl::getAcquisitionCamera2Setting(const QString which)
{
    QFExtensionCamera::CameraSetting set=QStringToSetting(which);
    if (set!=QFExtensionCamera::CamSetUnknown) {
        return mainWindow->getWidAcquisition()->getCameraSetting2(set);
    }
    return QVariant();
}

QVariant QFESPIMB040ScriptedAcquisitionAcquisitionControl::getAcquisitionCamera2SettingN(const QString which)
{
    return mainWindow->getWidAcquisition()->getCameraSetting2(which);
}
