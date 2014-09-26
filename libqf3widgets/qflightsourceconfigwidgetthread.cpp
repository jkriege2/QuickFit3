/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


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
    setObjectName("thread_lightsoureconfigwidget_"+parent->objectName());
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
   //qDebug()<<this<<"FLightSourceConfigWidgetThread::run()";
    while (!stopped) {
        nextInstruction();
        msleep(THREAD_TIMEOUT);
    }
   //qDebug()<<this<<"FLightSourceConfigWidgetThread::run() ... finished";
}

void QFLightSourceConfigWidgetThread::stopThread() {
    stopped=true;
   //qDebug()<<this<<"waiting for thread to stop";
    while (isRunning()) {
        stopped=true;
        wait(50);
    }
   //qDebug()<<this<<"waiting for thread to stop ... done";
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
            for (unsigned int i=0; i<LightSource->getLightSourceLineCount(LightSourceID); i++) {
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
