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
    if (lightsource>0 && lightsource<opticsSetup->getBrightfieldLightSourceCount()) {
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
        opticsSetup->setShutter(QFESPIMB040OpticsSetupBase::ShutterLaser1, state, true);
    } else if (shutter=="laser2") {
        opticsSetup->setShutter(QFESPIMB040OpticsSetupBase::ShutterLaser2, state, true);
    } else if (shutter=="transmission") {
        opticsSetup->setShutter(QFESPIMB040OpticsSetupBase::ShutterTransmission, state, true);
    } else {
        bool ok=false;
        int ID=shutter.toInt(&ok);
        if (ok && ID>=0 && ID<opticsSetup->getShutterCount()) {
            opticsSetup->setShutter(ID, state, true);
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

QFExtensionCamera::CameraSetting QStringToSetting(const QString& which) {
    QString whichl=which.toLower().trimmed();
    if (whichl=="exposuretime") return QFExtensionCamera::CamSetExposureTime;
    else if (whichl=="frames") return QFExtensionCamera::CamSetNumberFrames;
    else if (whichl=="gain") return QFExtensionCamera::CamSetGain;
    else if (whichl=="frametime") return QFExtensionCamera::CamSetFrametime;
    else if (whichl=="emgain") return QFExtensionCamera::CamSetEMGAIN;
    else if (whichl=="hbinning") return QFExtensionCamera::CamSetHorizontalBinning;
    else if (whichl=="vbinning") return QFExtensionCamera::CamSetVerticalBinning;
    else if (whichl=="tbinning") return QFExtensionCamera::CamSetTemporalBinning;
    return QFExtensionCamera::CamSetUnknown;
}

void QFESPIMB040ScriptedAcquisitionAcquisitionControl::setAcquisitionCamera1Setting(const QString which, QVariant value)
{
    QFExtensionCamera::CameraSetting set=QStringToSetting(which);
    if (set!=QFExtensionCamera::CamSetUnknown) {
        mainWindow->getWidAcquisition()->setCameraSetting1(set, value);
    }

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



void QFESPIMB040ScriptedAcquisitionAcquisitionControl::setAcquisitionCamera2Setting(const QString which, QVariant value)
{
    QFExtensionCamera::CameraSetting set=QStringToSetting(which);
    if (set!=QFExtensionCamera::CamSetUnknown) {
        mainWindow->getWidAcquisition()->setCameraSetting2(set, value);
    }

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
