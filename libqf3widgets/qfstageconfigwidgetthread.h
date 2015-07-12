/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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




#ifndef QFStageConfigWidgetTHREAD_H
#define QFStageConfigWidgetTHREAD_H

#include <QThread>
#include <QMutex>
#include <QQueue>
#include "qfextensionlinearstage.h"
#include "libwid_imexport.h"

class QFStageConfigWidget; // forward

/*! \brief SPIM Control Extension (B040, DKFZ Heidelberg): thread that reads three stages and sends signals for the current states
    \ingroup qf3ext_spimb040


 */
class QFWIDLIB_EXPORT QFStageConfigWidgetThread: public QThread {
        Q_OBJECT
    public:
        QFStageConfigWidgetThread(QFStageConfigWidget* parent);
        ~QFStageConfigWidgetThread();
        void run();
        void move(double x);
        void moveRel(double x);
        void setJoystick(bool enabled, double maxSpeed);
        /** \brief stop the thread and block until it is stopped! */
        void stopThread();
        bool anyConnected() const;
    public slots:
        void start(Priority priority = InheritPriority );
    signals:
        void stageXMoved(QFExtensionLinearStage::AxisState state, double position, double velocity);
        void joystickStateChanged(bool enabled);
        void stagesConnectedChanged(bool connX);
    protected:
        QFStageConfigWidget* m_parent;
        QMutex* InstructionMutex;
        bool stopped;

        enum InstructionType { Move, MoveRel, SetJoystick };
        struct Instruction {
            InstructionType type;
            double pd1;
            double pd2;
            double pd3;
            bool pb1;
        };

        QQueue<Instruction> instructions;

        int readCounter;
        bool connX;
    protected slots:
        void nextInstruction();

};

#endif // QFStageConfigWidgetTHREAD_H
