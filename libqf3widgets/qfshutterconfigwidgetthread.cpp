#include "qfshutterconfigwidgetthread.h"
#include "qfshutterconfigwidget.h"


#define THREAD_TIMEOUT 111

QFShutterConfigWidgetThread::QFShutterConfigWidgetThread(QFShutterConfigWidget *parent):
    QThread(parent)
{
    m_parent=parent;
    InstructionMutex=new QMutex();
    stopped=false;
}

QFShutterConfigWidgetThread::QFShutterConfigWidgetThread()
{
    stopThread();
    delete InstructionMutex;
}

void QFShutterConfigWidgetThread::stopThread()
{
    stopped=true;
   //qDebug()<<this<<"waiting for thread to stop";
    while (isRunning()) {
        stopped=true;
        wait(50);
    }
}

void QFShutterConfigWidgetThread::setState(bool state)
{
    QFShutterConfigWidgetThread::Instruction inst;
    inst.type=QFShutterConfigWidgetThread::SetState;
    inst.state=state;
    InstructionMutex->lock();
    instructions.enqueue(inst);
    InstructionMutex->unlock();
}

void QFShutterConfigWidgetThread::run()
{
    stopped=false;
    while (!stopped) {
        nextInstruction();
        msleep(THREAD_TIMEOUT);
    }
}

void QFShutterConfigWidgetThread::nextInstruction()
{
    if (!stopped) {
        //checkInstructions();
        QFExtensionShutter* Device;
        int DeviceID;
        Device=m_parent->getShutter();
        DeviceID=m_parent->getShutterID();
        bool state=false;
        if (Device) {
            state=Device->isShutterOpen(DeviceID);
        }
        emit stateChanged(state);
    }
}

void QFShutterConfigWidgetThread::checkInstructions()
{
    if (!stopped) {
        InstructionMutex->lock();
        if (!instructions.isEmpty()) {
            Instruction inst=instructions.dequeue();
            InstructionMutex->unlock();
            if (inst.type==QFShutterConfigWidgetThread::SetState) {
                /////////////////////////////////////////////////////////////////////////////
                // set shutter state
                /////////////////////////////////////////////////////////////////////////////

                QFExtensionShutter* Device=m_parent->getShutter();
                int DeviceIDd=m_parent->getShutterID();

                if (Device) Device->setShutterState(DeviceIDd, inst.state);
            }
        }
    }
}
