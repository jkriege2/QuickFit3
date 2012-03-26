#ifndef QFESPIMB040SAMPLESTAGECONFIG_H
#define QFESPIMB040SAMPLESTAGECONFIG_H

#include <QGroupBox>

class QFESPIMB040MainWindow; // forward

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

#include "programoptions.h"
#include "jkqttools.h"
#include "qtlogfile.h"
#include "qfextensionmanager.h"
#include "../interfaces/qfextensionlinearstage.h"
#include "qfextension.h"
#include "tools.h"
#include "qfstagecombobox.h"

class QFESPIMB040SampleStageConfigThread; // forward

/*! \brief SPIM Control Extension (B040, DKFZ Heidelberg) QGropBox with a set of controls that allow to control a sample translation stage
    \ingroup qf3ext_spimb040


 */
class QFESPIMB040SampleStageConfig : public QGroupBox {
        Q_OBJECT
    public:
        /** Default constructor */
        QFESPIMB040SampleStageConfig(QWidget* parent, bool useThread=true);
        //QFESPIMB040SampleStageConfig(QFESPIMB040MainWindow* parent, QFPluginServices* pluginServices);
        /** Default destructor */
        virtual ~QFESPIMB040SampleStageConfig();

        void init(QFPluginLogService* log, QFPluginServices* pluginServices);
        void setLog(QFPluginLogService* log);

        /** \brief load settings */
        void loadSettings(QSettings& settings, QString prefix);
        /** \brief save settings */
        void storeSettings(QSettings& settings, QString prefix);

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

    protected slots:
        void updateStates();

        void stageXMoved(QFExtensionLinearStage::AxisState state, double position, double velocity);
        void stageYMoved(QFExtensionLinearStage::AxisState state, double position, double velocity);
        void stageZMoved(QFExtensionLinearStage::AxisState state, double position, double velocity);
        void joystickStateChanged(bool enabled);
        void stagesConnectedChanged(bool connX, bool connY, bool connZ);
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
        /** \brief combobox to select a y-stage */
        QFStageComboBox* cmbStageY;
        /** \brief combobox to select a z-stage */
        QFStageComboBox* cmbStageZ;

        /** \brief tool button to connect to stage for axis x */
        QToolButton* btnConnectX;
        /** \brief tool button to connect to stage for axis y */
        QToolButton* btnConnectY;
        /** \brief tool button to connect to stage for axis z */
        QToolButton* btnConnectZ;

        /** \brief tool button to configuzre stage for axis x */
        QToolButton* btnConfigureX;
        /** \brief tool button to configuzre stage for axis y */
        QToolButton* btnConfigureY;
        /** \brief tool button to configuzre stage for axis z */
        QToolButton* btnConfigureZ;

        /** \brief checkbox to en-/disable the joystic for all axes at once */
        QCheckBox* chkJoystick;
        /** \brief edit to give the maximum speed of the joystick in micron/sec */
        QDoubleSpinBox* spinJoystickMaxSpeed;

        /** \brief edit to give x coordinate for movement */
        QDoubleSpinBox* spinMoveX;
        /** \brief edit to give y coordinate for movement */
        QDoubleSpinBox* spinMoveY;
        /** \brief edit to give z coordinate for movement */
        QDoubleSpinBox* spinMoveZ;
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

        /** \brief label for y-axis state */
        QLabel* labYState;
        /** \brief label for y-axis position */
        QLabel* labYPosition;
        /** \brief label for y-axis position */
        QLabel* labYSpeed;

        /** \brief label for z-axis state */
        QLabel* labZState;
        /** \brief label for z-axis position */
        QLabel* labZPosition;
        /** \brief label for z-axis position */
        QLabel* labZSpeed;
        /** \brief label to display joystick status */
        QLabel* labJoystick;
        QLabel* labThread;
        QToolButton* btnX2;
        QToolButton* btnX10;
        QToolButton* btnD2;
        QToolButton* btnD10;


        /** \brief action to connect to stage for axis x */
        QAction* actConnectX;
        /** \brief action to connect to stage for axis y */
        QAction* actConnectY;
        /** \brief action to connect to stage for axis z */
        QAction* actConnectZ;

        /** \brief action to configuzre stage for axis x */
        QAction* actConfigureX;
        /** \brief action to configuzre stage for axis y */
        QAction* actConfigureY;
        /** \brief action to configuzre stage for axis z */
        QAction* actConfigureZ;

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
        QFESPIMB040SampleStageConfigThread* stageThread;

        bool useThread;

        void updateStageStateWidgets(QLabel* labPos, QLabel* labSpeed, QLabel* labState, bool present, QFExtensionLinearStage::AxisState state, double position, double speed);

    public:

        /** \brief return a pointer to the x-axis stage class */
        QFExtensionLinearStage* getXStage();

        /** \brief return a pointer to the y-axis stage class */
        QFExtensionLinearStage* getYStage();

        /** \brief return a pointer to the z-axis stage class */
        QFExtensionLinearStage* getZStage();

        /** \brief return a pointer to the x-axis QFExtension class */
        QFExtension* getXStageExtension();

        /** \brief return a pointer to the y-axis QFExtension class */
        QFExtension* getYStageExtension();

        /** \brief return a pointer to the z-axis QFExtension class */
        QFExtension* getZStageExtension();



        /** \brief get the axis number of x-axis stage inside its class */
        int getXStageAxis();

        /** \brief get the axis number of y-axis stage inside its class */
        int getYStageAxis();

        /** \brief get the axis number of z-axis stage inside its class */
        int getZStageAxis();

        bool isXStageConnected() const;
        bool isYStageConnected() const;
        bool isZStageConnected() const;

    protected slots:
        void disConnectX();
        void disConnectY();
        void disConnectZ();
        void configureX();
        void configureY();
        void configureZ();
        void updateJoystick();
        void displayAxisStates();
        void moveAbsolute();
        void moveRelative();

        void speedX2();
        void speedX10();
        void speedD2();
        void speedD10();
};

#endif // QFESPIMB040SAMPLESTAGECONFIG_H
