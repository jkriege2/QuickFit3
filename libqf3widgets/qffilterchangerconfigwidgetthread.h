/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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


#ifndef QFFILTERCHANGERCONFIGWIDGETTHREAD_H
#define QFFILTERCHANGERCONFIGWIDGETTHREAD_H

#include <QThread>
#include <QQueue>
#include "libwid_imexport.h"
#include <QMutex>
#include "qfextensionfilterchanger.h"

class QFFilterChangerConfigWidget;

class QFWIDLIB_EXPORT QFFilterChangerConfigWidgetThread : public QThread
{
    Q_OBJECT
public:
    explicit QFFilterChangerConfigWidgetThread(QFFilterChangerConfigWidget *parent = 0);
    QFFilterChangerConfigWidgetThread();

public slots:
    void stopThread();
    /** \brief sets the shutter state of the given shutter */
    void setFilter(int filter);
signals:
    void filterChanged(int filter);
protected:
    virtual void run();

    void nextInstruction();
    void checkInstructions();

    QFFilterChangerConfigWidget* m_parent;
    QMutex* InstructionMutex;
    volatile bool stopped;
    int readCounter;

    enum InstructionType { SetFilter };
    struct Instruction {
        InstructionType type;
        int filter;
    };

    int lastState;

    QQueue<Instruction> instructions;

};

#endif // QFFILTERCHANGERCONFIGWIDGETTHREAD_H
