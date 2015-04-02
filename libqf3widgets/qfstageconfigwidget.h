/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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




#ifndef QFStageConfigWidget_H
#define QFStageConfigWidget_H

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
#include <QFrame>
#include <QTimer>
#include <QThread>
#include <QQueue>
#include <QMutex>

#include "programoptions.h"
#include "jkqttools.h"
#include "qtlogfile.h"
#include "qfextensionmanager.h"
#include "qfextensionlinearstage.h"
#include "qfextension.h"
#include "tools.h"
#include "qfstagecombobox.h"
#include "qfmanyfilessettings.h"
#include "libwid_imexport.h"

class QFStageConfigWidgetThread; // forward

/*! \brief SPIM Control Extension (B040, DKFZ Heidelberg) QGropBox with a set of controls that allow to control a sample translation stage
    \ingroup qf3ext_spimb040


 */
class QFWIDLIB_EXPORT QFStageConfigWidget : public QFrame {
        Q_OBJECT
    public:
        /** Default constructor */
        QFStageConfigWidget(QWidget* parent, bool useThread=true);
        //QFStageConfigWidget(QFESPIMB040MainWindow* parent, QFPluginServices* pluginServices);
        /** Default destructor */
        virtual ~QFStageConfigWidget();

        void init(QFPluginLogService* log, QFPluginServices* pluginServices);
        void setLog(QFPluginLogService* log);

        /** \brief load settings */
        void loadSettings(QSettings& settings, QString prefix, bool dontLoadDevices=false);
        void loadSettings(QFManyFilesSettings& settings, QString prefix, bool dontLoadDevices=false);
        /** \brief save settings */
        void saveSettings(QSettings& settings, QString prefix);
        void saveSettings(QFManyFilesSettings& settings, QString prefix);

        /** \brief connect to all selected stages */
        void connectStages();
        /** \brief disconnect from all selected stages */
        void disconnectStages();

        /*! \brief lock access to stages: stop the thread used for stage access by this widget

            \note call this, if you want to access the stage from any other method outside this widget!!! otherwise concurrent thread accesses are possible!!!
            \note You can release the lock y calling unlockStages().
          */
        void lockStages();
        /** \brief unlock access to stages: restart the thread used for stage access by this widget  */
        void unlockStages();

        bool isJoystickChecked() const;
        double joystickMaxSpeed();

    public slots:
        void setReadOnly(bool readonly);

        void speedX2();
        void speedX10();
        void speedD2();
        void speedD10();
        void toggleJoystick();
        void joystickOn();
        void joystickOff();

        void stepX();
        void stepMinusX();

    protected slots:
        void updateStates();

        void stageXMoved(QFExtensionLinearStage::AxisState state, double position, double velocity);
        void joystickStateChanged(bool enabled);
        void stagesConnectedChanged(bool connX);
        void threadStarted();
        void threadFinished();
    protected:
        //QFESPIMB040MainWindow* m_parent;
        QFPluginLogService* m_log;
        QFPluginServices* m_pluginServices;
        /** \brief list of all available QFExtensionLinearStage plugins, initialized by findCameras() */
        //QList<QObject*> stages;

        /** \brief fill stages */
        //void findStages(QFExtensionManager* extManager);

        void createWidgets();
        void createActions();

        /** \brief combobox to select a x-stage */
        QFStageComboBox* cmbStageX;

        /** \brief tool button to connect to stage for axis x */
        QToolButton* btnConnectX;

        /** \brief tool button to configuzre stage for axis x */
        QToolButton* btnConfigureX;

        /** \brief checkbox to en-/disable the joystic for all axes at once */
        QCheckBox* chkJoystick;
        /** \brief edit to give the maximum speed of the joystick in micron/sec */
        QDoubleSpinBox* spinJoystickMaxSpeed;

        /** \brief edit to give x coordinate for movement */
        QDoubleSpinBox* spinMoveX;
        /** \brief button for absolute movement */
        QPushButton* btnMoveAbsolute;
        /** \brief button for relative movement */
        QPushButton* btnMoveRelative;



        /** \brief label for x-axis state */
        QLabel* labXState;
        /** \brief label for x-axis position */
        QLabel* labXPosition;
        /** \brief label for x-axis position */
        QLabel* labXSpeed;

        /** \brief label to display joystick status */
        QLabel* labJoystick;
        //QLabel* labThread;
        QToolButton* btnX2;
        QToolButton* btnX10;
        QToolButton* btnD2;
        QToolButton* btnD10;


        /** \brief action to connect to stage for axis x */
        QAction* actConnectX;

        /** \brief action to configuzre stage for axis x */
        QAction* actConfigureX;

        QAction* actConfigSteps;
        QToolButton* btnConfigSteps;

        QTimer* timUpdate;

        bool locked;

        double stageStateUpdateInterval;

        QPixmap iconDisconnected;
        QPixmap iconReady;
        QPixmap iconError;
        QPixmap iconMoving;
        QPixmap iconMovingOpposite;
        QPixmap iconJoystick;
        QPixmap iconNoJoystick;

        QTimer timerDisplayUpdate;
        QFStageConfigWidgetThread* stageThread;

        double m_stepX;

        bool useThread;

        void updateStageStateWidgets(QLabel* labPos, QLabel* labSpeed, QLabel* labState, bool present, QFExtensionLinearStage::AxisState state, double position, double speed);
        void updateStageSettings(QFExtensionLinearStage::StageInfo info);

    public:

        /** \brief return a pointer to the x-axis stage class */
        QFExtensionLinearStage* getXStage();


        /** \brief return a pointer to the x-axis QFExtension class */
        QFExtension* getXStageExtension();


        /** \brief return a pointer to the x-axis QObject class */
        QObject* getXStageExtensionObject();


        /** \brief get the axis number of x-axis stage inside its class */
        int getXStageAxis();

        bool isXStageConnected() const;

        void moveRelative(double x);
    protected slots:
        void disConnectX();
        void configureX();
        void updateJoystick();
        void displayAxisStates();
        void moveAbsolute();
        void moveRelative();
        void configSteps();


};

#endif // QFStageConfigWidget_H
