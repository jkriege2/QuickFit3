#include "qfmeasurementdeviceconfigwidgetthread.h"
#include "qfextensionmeasurementdevice.h"
#include "qfmeasurementdeviceconfigwidget.h"

#define THREAD_TIMEOUT 233

QFMeasurementDeviceConfigWidgetThread::QFMeasurementDeviceConfigWidgetThread(QFMeasurementDeviceConfigWidget *parent) :
    QThread(parent)
{
    m_parent=parent;
    InstructionMutex=new QMutex();
    stopped=false;
    qRegisterMetaType<QList<bool> >("QList<bool>");
    qRegisterMetaType<QList<double> >("QList<double>");
    qRegisterMetaType<QList<QVariant> >("QList<QVariant>");
    setObjectName("thread_measurementdeviceconfigwidget_"+parent->objectName());
}


QFMeasurementDeviceConfigWidgetThread::~QFMeasurementDeviceConfigWidgetThread() {
    stopThread();
    delete InstructionMutex;
}


void QFMeasurementDeviceConfigWidgetThread::setValue(int line, const QVariant &value) {
    QFMeasurementDeviceConfigWidgetThread::Instruction inst;
    inst.type=QFMeasurementDeviceConfigWidgetThread::SetValue;
    inst.value=value;
    inst.line=line;
    InstructionMutex->lock();
    instructions.enqueue(inst);
    InstructionMutex->unlock();

}


void QFMeasurementDeviceConfigWidgetThread::run() {
    stopped=false;
   //qDebug()<<this<<"FMeasurementDeviceConfigWidgetThread::run()";
    while (!stopped) {
        nextInstruction();
        msleep(THREAD_TIMEOUT);
    }
   //qDebug()<<this<<"FMeasurementDeviceConfigWidgetThread::run() ... finished";
}

void QFMeasurementDeviceConfigWidgetThread::stopThread() {
    stopped=true;
   //qDebug()<<this<<"waiting for thread to stop";
    while (isRunning()) {
        stopped=true;
        wait(50);
    }
   //qDebug()<<this<<"waiting for thread to stop ... done";
    //qDebug()<<"thread stopped";
}


void QFMeasurementDeviceConfigWidgetThread::nextInstruction() {
    if (!stopped) {
        //checkInstructions();
        QFExtensionMeasurementAndControlDevice* MeasurementDevice;
        int MeasurementDeviceID;
        MeasurementDevice=m_parent->getMeasurementDevice();
        MeasurementDeviceID=m_parent->getMeasurementDeviceID();
        QList<bool> widgetsEnabled;
        QList<QVariant> values;
        QStringList lineNames;
        if (MeasurementDevice) {
            for (unsigned int i=0; i<MeasurementDevice->getMeasurementDeviceValueCount(MeasurementDeviceID); i++) {
                //checkInstructions();
                widgetsEnabled.append(true);
                values.append(MeasurementDevice->getMeasurementDeviceValue(MeasurementDeviceID, i));
                lineNames.append(MeasurementDevice->getMeasurementDeviceValueName(MeasurementDeviceID, i));
            }
        }
        emit valuesChanged(QTime::currentTime(), values, lineNames, widgetsEnabled);
    }

}

void QFMeasurementDeviceConfigWidgetThread::checkInstructions() {
    if (!stopped) {
        InstructionMutex->lock();
        if (!instructions.isEmpty()) {
            Instruction inst=instructions.dequeue();
            InstructionMutex->unlock();
            if (inst.type==QFMeasurementDeviceConfigWidgetThread::SetValue) {
                /////////////////////////////////////////////////////////////////////////////
                // set value
                /////////////////////////////////////////////////////////////////////////////

                QFExtensionMeasurementAndControlDevice* ls=m_parent->getMeasurementDevice();
                int id=m_parent->getMeasurementDeviceID();

                if (ls) ls->setMeasurementDeviceValue(id, inst.line, inst.value);
            }
        }
    }

}
