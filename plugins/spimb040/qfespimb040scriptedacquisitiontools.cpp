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




QFESPIMB040ScriptedAcquisitionTools::QFESPIMB040ScriptedAcquisitionTools(QFESPIMB040ScriptedAcquisition* widAcquisition, QFESPIMB040MainWindow2 *mainWindow, QFESPIMB040AcquisitionTools *acqTools, QFPluginLogService *log, QObject *parent, QFPluginServices *pluginServices, QFESPIMB040OpticsSetup *opticsSetup, QFESPIMB040AcquisitionDescription *acqDescription, QFESPIMB040ExperimentDescription *expDescription):
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
}

void QFESPIMB040ScriptedAcquisitionTools::logError(const QString &text)
{
    log->log_error(text);
}

void QFESPIMB040ScriptedAcquisitionTools::logWarning(const QString &text)
{
    log->log_warning(text);
}

void QFESPIMB040ScriptedAcquisitionTools::setStatus(const QString &text)
{
    widAcquisition->setStatus(text);
}

void QFESPIMB040ScriptedAcquisitionTools::sleepMS(int duration)
{
    QElapsedTimer t;
    t.start();
    while (t.elapsed()<duration) {
        QApplication::processEvents();
    }
}







QFESPIMB040ScriptedAcquisitionInstrumentControl::QFESPIMB040ScriptedAcquisitionInstrumentControl(QFESPIMB040ScriptedAcquisition* widAcquisition, QFESPIMB040MainWindow2 *mainWindow, QFESPIMB040AcquisitionTools *acqTools, QFPluginLogService *log, QObject *parent, QFPluginServices *pluginServices, QFESPIMB040OpticsSetup *opticsSetup, QFESPIMB040AcquisitionDescription *acqDescription, QFESPIMB040ExperimentDescription *expDescription):
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
    if (laser==1)  {
        opticsSetup->getLaser1()->setLightSourcePower(opticsSetup->getLaser1ID(), line, intensity);
    } else if (laser==2)  {
        opticsSetup->getLaser2()->setLightSourcePower(opticsSetup->getLaser2ID(), line, intensity);
    } else {
        throwEngineException(QObject::tr("setLaserIntensity(laser=%1, %2, %3): unknown laser!").arg(laser).arg(line).arg(intensity));
    }
}

void QFESPIMB040ScriptedAcquisitionInstrumentControl::setTransmissionIntensity(int line, double intensity)
{
    opticsSetup->getTransmissionLightSource()->setLightSourcePower(opticsSetup->getTransmissionLightSourceID(), line, intensity);
}

void QFESPIMB040ScriptedAcquisitionInstrumentControl::setShutter(const QString& shutter, bool state)
{
    if (shutter=="main") {
        opticsSetup->setMainIlluminationShutter(state, true);
    } else if (shutter=="laser1") {
        opticsSetup->setShutter(QFESPIMB040OpticsSetup::ShutterLaser1, state, true);
    } else if (shutter=="laser2") {
        opticsSetup->setShutter(QFESPIMB040OpticsSetup::ShutterLaser2, state, true);
    } else if (shutter=="transmission") {
        opticsSetup->setShutter(QFESPIMB040OpticsSetup::ShutterTransmission, state, true);
    } else {
        throwEngineException(QObject::tr("setShutter('%1'): unknown shutter!").arg(shutter));
    }
}

void QFESPIMB040ScriptedAcquisitionInstrumentControl::moveStageRel(const QString &stage, double newPos)
{
    QFExtensionLinearStage*  stageE=NULL;
    int axis=0;
    if (stage=="x") {
        stageE=opticsSetup->getXStage();
        axis=opticsSetup->getXStageAxis();
    } else if (stage=="y") {
        stageE=opticsSetup->getYStage();
        axis=opticsSetup->getYStageAxis();
    } else if (stage=="z") {
        stageE=opticsSetup->getZStage();
        axis=opticsSetup->getZStageAxis();
    } else {
        throwEngineException(QObject::tr("moveStageRel('%1', %2): unknown stage!").arg(stage).arg(newPos));
        return;
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
        stageE=opticsSetup->getXStage();
        axis=opticsSetup->getXStageAxis();
    } else if (stage=="y") {
        stageE=opticsSetup->getYStage();
        axis=opticsSetup->getYStageAxis();
    } else if (stage=="z") {
        stageE=opticsSetup->getZStage();
        axis=opticsSetup->getZStageAxis();
    } else {
        throwEngineException(QObject::tr("moveStageAbs('%1', %2): unknown stage!").arg(stage).arg(newPos));
        return;
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
        changer=opticsSetup->getFilterChangerDetection();
        ID=opticsSetup->getFilterChangerDetectionID();
    } else {
        throwEngineException(QObject::tr("setFilterWheel('%1', %2): unknown filter wheel!").arg(wheel).arg(filter));
        return;
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
















QFESPIMB040ScriptedAcquisitionAcquisitionControl::QFESPIMB040ScriptedAcquisitionAcquisitionControl(QFESPIMB040ScriptedAcquisition* widAcquisition, QFESPIMB040MainWindow2 *mainWindow, QFESPIMB040AcquisitionTools *acqTools, QFPluginLogService *log, QObject *parent, QFPluginServices *pluginServices, QFESPIMB040OpticsSetup *opticsSetup, QFESPIMB040AcquisitionDescription *acqDescription, QFESPIMB040ExperimentDescription *expDescription):
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
