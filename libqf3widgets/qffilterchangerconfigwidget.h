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


#ifndef QFFILTERCHANGERCONFIGWIDGET_H
#define QFFILTERCHANGERCONFIGWIDGET_H

#include <QGroupBox>


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
#include "programoptions.h"
#include "jkqttools.h"
#include "qtlogfile.h"
#include "qfextensionmanager.h"
#include "qfextensionfilterchanger.h"
#include "qfextension.h"
#include "tools.h"
#include "qffilterchangercombobox.h"
#include "filters.h"
#include "libwid_imexport.h"
#include "qffilterchangerconfigwidgetthread.h"
#include "qfmanyfilessettings.h"

/*! \brief SPIM Control Extension (B040, DKFZ Heidelberg) QWidget with a set of controls that allow to control a FilterChanger
    \ingroup qf3ext_spimb040


 */
class QFWIDLIB_EXPORT QFFilterChangerConfigWidget : public QFrame {
        Q_OBJECT
    public:
        /** Default constructor */
        QFFilterChangerConfigWidget(QWidget* parent=NULL);
        /** Default destructor */
        virtual ~QFFilterChangerConfigWidget();

        void init(const QString &globalfilterconfig, const QString& filterconfig, QFPluginLogService* log, QFPluginServices* pluginServices);
        void setLog(QFPluginLogService* log);

        /** \brief load settings */
        void loadSettings(QSettings& settings, QString prefix);
        void loadSettings(QFManyFilesSettings& settings, QString prefix);
        /** \brief save settings */
        void saveSettings(QSettings& settings, QString prefix);
        void saveSettings(QFManyFilesSettings& settings, QString prefix);

        /** \brief connect to all selected FilterChangers */
        void connectFilterChanger();
        /** \brief disconnect from all selected FilterChangers */
        void disconnectFilterChanger();

        /*! \brief lock access to stages: stop the thread used for stage access by this widget

            \note call this, if you want to access the stage from any other method outside this widget!!! otherwise concurrent thread accesses are possible!!!
            \note You can release the lock y calling unlockStages().
          */
        void lockFilterChangers();
        /** \brief unlock access to stages: restart the thread used for stage access by this widget  */
        void unlockFilterChangers();

        int getFilterChangerState();

        QString getCurrentFilter() const;
        FilterDescription getCurrentFilterDescription() const;


    public slots:
        void setReadOnly(bool readonly);
        void setFilter(int filter);
        void setFilter0();
        void setFilter1();
        void setFilter2();
        void setFilter3();
        void setFilter4();
        void setFilter5();
        void setFilter6();
        void setFilter7();
        void setFilter8();
        void setFilter9();

    protected slots:
        void updateStates();
        void updateFilters();
        void selectFilters();

    protected:
        //QFESPIMB040MainWindow* m_parent;
        QFPluginLogService* m_log;
        QFPluginServices* m_pluginServices;
        QString m_filterconfig;
        QString m_globalfilterconfig;

        QFFilterChangerConfigWidgetThread* m_thread;
        bool useThread;


        void createWidgets();
        void createActions();

        /** \brief combobox to select a FilterChanger */
        QFFilterChangerComboBox* cmbFilterChanger;

        /** \brief tool button to connect to a FilterChanger */
        QToolButton* btnConnect;

        /** \brief tool button to configure a FilterChanger */
        QToolButton* btnConfigure;

        QToolButton* btnSelectFilters;

        /** \brief button to display/change the FilterChanger state */
        QComboBox* cmbFilter;

        /** \brief action to connect to a FilterChanger */
        QAction* actConnect;

        /** \brief action to configure to a FilterChanger */
        QAction* actConfigure;

        /** \brief action to select the filters in the FilterChanger */
        QAction* actSetFilters;

        QTimer* timUpdate;

        bool locked;

        bool moving;


        double FilterChangerStateUpdateInterval;


        QTimer timerDisplayUpdate;

        QStringList filters;

    public:

        /** \brief return a pointer to the FilterChanger */
        QFExtensionFilterChanger* getFilterChanger() const;

        /** \brief return a pointer to the ahutter QFExtension class */
        QFExtension* getFilterChangerExtension() const;
        QObject* getFilterChangerExtensionObject() const;


        /** \brief get the FilterChanger number of the given axis */
        int getFilterChangerID() const;

        bool isFilterChangerConnected() const;

        bool isLastActionComplete() const;

    public slots:
        void setFilterChanger(int filter);

    protected slots:
        void disConnect();
        void configure();
        void displayFilterChangerStates();
        void filterChanged(int filter);
    public slots:
        void FilterChangerNewFilterSelected(int index);
};

#endif // QFFILTERCHANGERCONFIGWIDGET_H
