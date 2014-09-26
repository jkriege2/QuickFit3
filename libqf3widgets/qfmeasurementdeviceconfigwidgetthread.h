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


#ifndef QFMeasurementDeviceConfigWidgetThread_H
#define QFMeasurementDeviceConfigWidgetThread_H

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
#include "qfextensionmeasurementdevice.h"
#include "qfextension.h"
#include "tools.h"
#include "qfmeasurementdevicecombobox.h"
#include "filters.h"
#include "libwid_imexport.h"



class QFMeasurementDeviceConfigWidget; //forward

/*! \brief thread used by QFMeasurementDeviceConfigWidget to control measurement devices
    \ingroup qf3ext_spimb040


 */
class QFWIDLIB_EXPORT QFMeasurementDeviceConfigWidgetThread : public QThread {
        Q_OBJECT
    public:

        explicit QFMeasurementDeviceConfigWidgetThread(QFMeasurementDeviceConfigWidget *parent = 0);
        ~QFMeasurementDeviceConfigWidgetThread();

    public slots:
        void stopThread();
        void setValue(int line, const QVariant& value);
    signals:
        void valuesChanged(QTime time, QList<QVariant> values, QStringList lineNames, QList<bool> widgetsEnabled);
    protected:
        virtual void run();

        void nextInstruction();
        void checkInstructions();

        QFMeasurementDeviceConfigWidget* m_parent;
        QMutex* InstructionMutex;
        volatile bool stopped;
        int readCounter;

        enum InstructionType { SetValue };
        struct Instruction {
            InstructionType type;
            QVariant value;
            int line;
        };

        QQueue<Instruction> instructions;
};

#endif // QFMeasurementDeviceConfigWidgetThread_H
