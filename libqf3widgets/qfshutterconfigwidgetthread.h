/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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


#ifndef QFSHUTTERCONFIGWIDGETTHREAD_H
#define QFSHUTTERCONFIGWIDGETTHREAD_H

#include <QThread>
#include <QQueue>
#include "libwid_imexport.h"
#include <QMutex>
#include "qfextensionshutter.h"

class QFShutterConfigWidget;

class QFWIDLIB_EXPORT QFShutterConfigWidgetThread : public QThread
{
    Q_OBJECT
public:
    explicit QFShutterConfigWidgetThread(QFShutterConfigWidget *parent = 0);
    QFShutterConfigWidgetThread();

public slots:
    void stopThread();
    /** \brief sets the shutter state of the given shutter */
    void setState(bool state);
signals:
    void stateChanged(bool state);
protected:
    virtual void run();

    void nextInstruction();
    void checkInstructions();

    QFShutterConfigWidget* m_parent;
    QMutex* InstructionMutex;
    volatile bool stopped;
    int readCounter;

    enum InstructionType { SetState };
    struct Instruction {
        InstructionType type;
        bool state;
    };

    QQueue<Instruction> instructions;

};

#endif // QFSHUTTERCONFIGWIDGETTHREAD_H
