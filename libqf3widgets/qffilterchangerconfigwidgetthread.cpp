/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


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
