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


#ifndef QFMeasurementDeviceConfigWidget_H
#define QFMeasurementDeviceConfigWidget_H

#include <QWidget>


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
#include "qflightsourcecombobox.h"
#include "filters.h"
#include "libwid_imexport.h"
#include "qfmeasurementdeviceconfigwidgetthread.h"
#include "qftools.h"
#include "qfmanyfilessettings.h"
#include "qfmeasurementdevicecombobox.h"
#include <QLineEdit>

/*! \brief QWidget with a set of controls that allow to control a MeasurementDevice
    \ingroup qf3ext_spimb040


 */
class QFWIDLIB_EXPORT QFMeasurementDeviceConfigWidget : public QFrame {
        Q_OBJECT
    public:
        explicit QFMeasurementDeviceConfigWidget(QWidget *parent = 0, bool doUseThread=true);
        ~QFMeasurementDeviceConfigWidget();
        

        void init(QFPluginLogService* log, QFPluginServices* pluginServices);
        void setLog(QFPluginLogService* log);

        /** \brief load settings */
        void loadSettings(QSettings& settings, QString prefix);
        void loadSettings(QFManyFilesSettings& settings, QString prefix);
        /** \brief save settings */
        void saveSettings(QSettings& settings, QString prefix);
        void saveSettings(QFManyFilesSettings& settings, QString prefix);

        /** \brief connect to all selected MeasurementDevices */
        void connectMeasurementDevice();
        /** \brief disconnect from all selected MeasurementDevices */
        void disconnectMeasurementDevice();

        /*! \brief lock access to stages: stop the thread used for stage access by this widget

            \note call this, if you want to access the stage from any other method outside this widget!!! otherwise concurrent thread accesses are possible!!!
            \note You can release the lock y calling unlockStages().
          */
        void lockMeasurementDevice();
        /** \brief unlock access to stages: restart the thread used for stage access by this widget  */
        void unlockMeasurementDevice();

        /** \brief return a pointer to the MeasurementDevice */
        QFExtensionMeasurementAndControlDevice* getMeasurementDevice() const;

        /** \brief return a pointer to the ahutter QFExtension class */
        QFExtension* getMeasurementDeviceExtension() const;
        QObject* getMeasurementDeviceExtensionObject() const;

        QFMeasurementDeviceComboBox* getMeasurementDeviceComboBox() const;
        void disableDeviceCombobox();


        /** \brief get the MeasurementDevice number of the given axis */
        int getMeasurementDeviceID() const;

        bool isMeasurementDeviceConnected() const;

        bool isLastActionComplete() const;

        int getValueCount() const;
        QString getValueName(int line) const;
        QVariant getValue(int line) const;


    public slots:
        void setReadOnly(bool readonly);
        /** \brief set the given line power. While the widget is NOT locked, this function is blocking until the action completes  */
        void setValue(int line, const QVariant& power);

    protected:
        QFPluginLogService* m_log;
        QFPluginServices* m_pluginServices;
        QString m_filterconfig;

        QFMeasurementDeviceConfigWidgetThread* m_thread;


        void createWidgets();
        void createActions();

        /** \brief combobox to select a MeasurementDevice */
        QFMeasurementDeviceComboBox* cmbMeasurementDevice;

        /** \brief tool button to connect to a MeasurementDevice */
        QToolButton* btnConnect;

        /** \brief tool button to configure a MeasurementDevice */
        QToolButton* btnConfigure;

        QSpacerItem* stretch;

        /** \brief action to connect to a MeasurementDevice */
        QAction* actConnect;

        /** \brief action to configure to a MeasurementDevice */
        QAction* actConfigure;

        QTimer* timUpdate;

        bool locked;


        double stateUpdateInterval;


        QTimer timerDisplayUpdate;

        struct MDWidgets {
            QPointer<QLineEdit> lineedit;
            QPointer<QCheckBox> checkbox;
            QPointer<QDoubleSpinBox> spinDouble;
            QPointer<QSpinBox> spinInt;
            QPointer<QComboBox> cmbInt;
            QPointer<QLabel> label;
            QPointer<QLabel> labelName;

            void deleteLater();
            void deleteInputLater();
            void removeFromLayout(QGridLayout* layout);
            void removeInputFromLayout(QGridLayout* layout);
            void setValue(const QVariant& value);
            void setValueUpdate(const QVariant& value);
            void setTitle(const QString& title);
            void setRange(double mi, double ma, const QStringList& sl);
            QVariant getValue();
        };

        QList<MDWidgets> mdWidgets;

        QGridLayout* widgetLayout;
        QGridLayout* linesLayout;
        QWidget* linesLayoutWidget;




        void updateMDWidgets();

        int getMDByWidget(QObject *widget);

        bool useThread;
        bool dontAccessWidgets;


    protected slots:
        void disConnect();
        void configure();
        void displayStates();
        void updateStates();
        void setValueVChanged(const QVariant& value);
        void setValueChanged(double value);
        void setValueChanged(int value);
        void setValueChanged(bool value);
        void setValueChanged(const QString& value);
        void valuesChanged(QTime time, QList<QVariant> values, QStringList lineNames, QList<bool> widgetsEnabled);
        void setPowerEditingFinished();

        
};

#endif // QFMeasurementDeviceConfigWidget_H
