/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

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


#ifndef QFLIGHTSOURCECONFIGWIDGETTHREAD_H
#define QFLIGHTSOURCECONFIGWIDGETTHREAD_H

#include <QWidget>
#include "qvisiblehandlesplitter.h"
#include <QSplitter>
#include <QComboBox>
#include <QSettings>
#include <QMenu>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <cmath>
#include <QLabel>
#include <QPushButton>
#include <QToolBar>
#include <QTime>
#include <QTabWidget>
#include <QGroupBox>
#include <QTimer>
#include <QThread>
#include <QQueue>
#include <QMutex>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include "programoptions.h"
#include "jkqttools.h"
#include "qtlogfile.h"
#include "qfextensionmanager.h"
#include "qfextensionlightsource.h"
#include "qfextension.h"
#include "tools.h"
#include "qflightsourcecombobox.h"
#include "filters.h"
#include "libwid_imexport.h"



class QFLightSourceConfigWidget; //forward

/*! \brief thread used by QFLightSourceConfigWidget to control light sources
    \ingroup qf3ext_spimb040


 */
class QFWIDLIB_EXPORT QFLightSourceConfigWidgetThread : public QThread {
        Q_OBJECT
    public:

        explicit QFLightSourceConfigWidgetThread(QFLightSourceConfigWidget *parent = 0);
        ~QFLightSourceConfigWidgetThread();

    public slots:
        void stopThread();
        /** \brief switch the given line on or off. While the widget is NOT locked, this function is blocking until the action completes  */
        void setLineEnabled(int line, bool enabled);
        /** \brief set the given line power. While the widget is NOT locked, this function is blocking until the action completes  */
        void setLinePower(int line, double power);
    signals:
        void linesChanged(QTime time, QList<bool> enabled, QList<double> setValues, QList<double> measuredValues, QStringList powerUnits, QStringList lineNames, QList<bool> widgetsEnabled);
    protected:
        virtual void run();

        void nextInstruction();
        void checkInstructions();

        QFLightSourceConfigWidget* m_parent;
        QMutex* InstructionMutex;
        volatile bool stopped;
        int readCounter;

        enum InstructionType { SetEnabled, SetPower };
        struct Instruction {
            InstructionType type;
            double value;
            bool enabled;
            int line;
        };

        QQueue<Instruction> instructions;
};

#endif // QFLIGHTSOURCECONFIGWIDGETTHREAD_H
