#include "qflightsourceconfigwidgetthread.h"
#include "qfextensionlightsource.h"
#include "qflightsourceconfigwidget.h"

#define THREAD_TIMEOUT 233

QFLightSourceConfigWidgetThread::QFLightSourceConfigWidgetThread(QFLightSourceConfigWidget *parent) :
    QThread(parent)
{
    m_parent=parent;
    InstructionMutex=new QMutex();
    stopped=false;
    qRegisterMetaType<QList<bool> >("QList<bool>");
    qRegisterMetaType<QList<double> >("QList<double>");
}


QFLightSourceConfigWidgetThread::~QFLightSourceConfigWidgetThread() {
    stopThread();
    delete InstructionMutex;
}

void QFLightSourceConfigWidgetThread::setLineEnabled(int line, bool enabled) {
    QFLightSourceConfigWidgetThread::Instruction inst;
    inst.type=QFLightSourceConfigWidgetThread::SetEnabled;
    inst.enabled=enabled;
    inst.line=line;
    InstructionMutex->lock();
    instructions.enqueue(inst);
    InstructionMutex->unlock();
}


void QFLightSourceConfigWidgetThread::setLinePower(int line, double power) {
    QFLightSourceConfigWidgetThread::Instruction inst;
    inst.type=QFLightSourceConfigWidgetThread::SetEnabled;
    inst.value=power;
    inst.line=line;
    InstructionMutex->lock();
    instructions.enqueue(inst);
    InstructionMutex->unlock();

}


void QFLightSourceConfigWidgetThread::run() {
    stopped=false;
    while (!stopped) {
        nextInstruction();
        msleep(THREAD_TIMEOUT);
    }
}

void QFLightSourceConfigWidgetThread::stopThread() {
    stopped=true;
    if (isRunning()) wait();
    //qDebug()<<"thread stopped";
}


void QFLightSourceConfigWidgetThread::nextInstruction() {
    if (!stopped) {
        //checkInstructions();
        QFExtensionLightSource* LightSource;
        int LightSourceID;
        LightSource=m_parent->getLightSource();
        LightSourceID=m_parent->getLightSourceID();
        QList<bool> enabled;
        QList<bool> widgetsEnabled;
        QList<double> setValues;
        QList<double> measuredValues;
        QStringList powerUnits;
        QStringList lineNames;
        if (LightSource) {
            for (int i=0; i<LightSource->getLightSourceLineCount(LightSourceID); i++) {
                //checkInstructions();
                enabled.append(LightSource->getLightSourceLineEnabled(LightSourceID, i));
                widgetsEnabled.append(LightSource->isLastLightSourceActionFinished(LightSourceID));
                setValues.append(LightSource->getLightSourceCurrentSetPower(LightSourceID, i));
                measuredValues.append(LightSource->getLightSourceCurrentMeasuredPower(LightSourceID, i));
                powerUnits.append(LightSource->getLightSourceLinePowerUnit(LightSourceID, i));
                lineNames.append(LightSource->getLightSourceLineDescription(LightSourceID, i));
            }
        }
        emit linesChanged(QTime::currentTime(), enabled, setValues, measuredValues, powerUnits, lineNames, widgetsEnabled);
    }

}

void QFLightSourceConfigWidgetThread::checkInstructions() {
    if (!stopped) {
        InstructionMutex->lock();
        if (!instructions.isEmpty()) {
            Instruction inst=instructions.dequeue();
            InstructionMutex->unlock();
            if (inst.type==QFLightSourceConfigWidgetThread::SetEnabled) {
                /////////////////////////////////////////////////////////////////////////////
                // enable line
                /////////////////////////////////////////////////////////////////////////////

                QFExtensionLightSource* ls=m_parent->getLightSource();
                int id=m_parent->getLightSourceID();

                if (ls) ls->setLightSourceLineEnabled(id, inst.line, inst.enabled);
            } else if (inst.type==QFLightSourceConfigWidgetThread::SetPower) {
                /////////////////////////////////////////////////////////////////////////////
                // set line power
                /////////////////////////////////////////////////////////////////////////////

                QFExtensionLightSource* ls=m_parent->getLightSource();
                int id=m_parent->getLightSourceID();

                if (ls) ls->setLightSourcePower(id, inst.line, inst.value);
            }
        }
    }

}
