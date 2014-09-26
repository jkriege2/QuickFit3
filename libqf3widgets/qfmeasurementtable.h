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


#ifndef QFEMEASUREMENTTABLE_H
#define QFEMEASUREMENTTABLE_H

#include <QWidget>
#include "qfextensionmeasurementdevice.h"
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
#include "qfextension.h"
#include "tools.h"
#include "libwid_imexport.h"
#include "qftools.h"
#include "qfmanyfilessettings.h"
#include "libwid_imexport.h"

class QFWIDLIB_EXPORT QFMeasurementTable : public QWidget {
        Q_OBJECT
    public:
        explicit QFMeasurementTable(QWidget *parent = 0);
        void init(QFPluginLogService* log, QFPluginServices* pluginServices);
        void setLog(QFPluginLogService* log);
        /** \brief load settings */
        void loadSettings(QSettings& settings, QString prefix);
        void loadSettings(QFManyFilesSettings& settings, QString prefix);
        /** \brief save settings */
        void saveSettings(QSettings& settings, QString prefix);
        void saveSettings(QFManyFilesSettings& settings, QString prefix);

        /** \brief connect to all selected LightSources */
        void connectMeasurementDevices();
        /** \brief disconnect from all selected LightSources */
        void disconnectMeasurementDevices();
        bool areMeasurementDevicesConnected() const;

        /*! \brief lock access to stages: stop the thread used for stage access by this widget

            \note call this, if you want to access the stage from any other method outside this widget!!! otherwise concurrent thread accesses are possible!!!
            \note You can release the lock y calling unlockStages().
          */
        void lockMeasurementDevices();
        /** \brief unlock access to stages: restart the thread used for stage access by this widget  */
        void unlockMeasurementDevices();

        /** \brief return a pointer to the LightSource */
        QList<QFExtensionMeasurementAndControlDevice*> getMeasurementDevices() const;

        /** \brief return a pointer to the ahutter QFExtension class */
        QList<QFExtension*> getMeasurementDeviceExtensions() const;
        QList<QObject*> getMeasurementDeviceExtensionObjects() const;


        /** \brief get the LightSource number of the given axis */
        QList<int> getMeasurementDeviceIDs() const;



    signals:
        
    public slots:
        
};

#endif // QFEMEASUREMENTTABLE_H
