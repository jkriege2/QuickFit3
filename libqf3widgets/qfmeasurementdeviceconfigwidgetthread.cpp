/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


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
