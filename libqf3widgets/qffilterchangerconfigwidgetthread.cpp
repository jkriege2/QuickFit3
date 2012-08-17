#include "qffilterchangerconfigwidgetthread.h"
#include "qffilterchangerconfigwidget.h"


#define THREAD_TIMEOUT 131

QFFilterChangerConfigWidgetThread::QFFilterChangerConfigWidgetThread(QFFilterChangerConfigWidget *parent):
    QThread(parent)
{
    m_parent=parent;
    InstructionMutex=new QMutex();
    stopped=false;
}

QFFilterChangerConfigWidgetThread::QFFilterChangerConfigWidgetThread()
{
    stopThread();
    delete InstructionMutex;
}

void QFFilterChangerConfigWidgetThread::stopThread()
{
    stopped=true;
   //qDebug()<<this<<"waiting for thread to stop";
    while (isRunning()) {
        stopped=true;
        wait(50);
    }
}

void QFFilterChangerConfigWidgetThread::setFilter(int filter)
{
    QFFilterChangerConfigWidgetThread::Instruction inst;
    inst.type=QFFilterChangerConfigWidgetThread::SetFilter;
    inst.filter=filter;
    InstructionMutex->lock();
    instructions.enqueue(inst);
    InstructionMutex->unlock();
}

void QFFilterChangerConfigWidgetThread::run()
{
    stopped=false;
    while (!stopped) {
        nextInstruction();
        msleep(THREAD_TIMEOUT);
    }
}

void QFFilterChangerConfigWidgetThread::nextInstruction()
{
    if (!stopped) {
        //checkInstructions();
        QFExtensionFilterChanger* Device;
        int DeviceID;
        Device=m_parent->getFilterChanger();
        DeviceID=m_parent->getFilterChangerID();
        int state=0;
        if (Device) {
            state=Device->getFilterChangerCurrentFilter(DeviceID);
        }
        emit filterChanged(state);
    }
}

void QFFilterChangerConfigWidgetThread::checkInstructions()
{
    if (!stopped) {
        InstructionMutex->lock();
        if (!instructions.isEmpty()) {
            Instruction inst=instructions.dequeue();
            InstructionMutex->unlock();
            if (inst.type==QFFilterChangerConfigWidgetThread::SetFilter) {
                /////////////////////////////////////////////////////////////////////////////
                // set shutter state
                /////////////////////////////////////////////////////////////////////////////

                QFExtensionFilterChanger* Device=m_parent->getFilterChanger();
                int DeviceIDd=m_parent->getFilterChangerID();

                if (Device) Device->setFilterChangerFilter(DeviceIDd, inst.filter);
            }
        }
    }
}
