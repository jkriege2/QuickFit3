#ifndef QFESPIMB040ShutterConfig_H
#define QFESPIMB040ShutterConfig_H

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
#include "../interfaces/qfextensionshutter.h"
#include "qfextension.h"
#include "tools.h"
#include "qfshuttercombobox.h"


/*! \brief SPIM Control Extension (B040, DKFZ Heidelberg) QWidget with a set of controls that allow to control a shutter
    \ingroup qf3ext_spimb040


 */
class QFESPIMB040ShutterConfig : public QWidget {
        Q_OBJECT
    public:
        /** Default constructor */
        QFESPIMB040ShutterConfig(QWidget* parent=NULL);
        /** Default destructor */
        virtual ~QFESPIMB040ShutterConfig();

        void init(QFPluginLogService* log, QFPluginServices* pluginServices);
        void setLog(QFPluginLogService* log);

        /** \brief load settings */
        void loadSettings(QSettings& settings, QString prefix);
        /** \brief save settings */
        void saveSettings(QSettings& settings, QString prefix);

        /** \brief connect to all selected Shutters */
        void connectShutter();
        /** \brief disconnect from all selected Shutters */
        void disconnectShutter();

        /*! \brief lock access to stages: stop the thread used for stage access by this widget

            \note call this, if you want to access the stage from any other method outside this widget!!! otherwise concurrent thread accesses are possible!!!
            \note You can release the lock y calling unlockStages().
          */
        void lockShutters();
        /** \brief unlock access to stages: restart the thread used for stage access by this widget  */
        void unlockShutters();

        bool getShutterState();


    public slots:
        void setReadOnly(bool readonly);

    protected slots:
        void updateStates();

    protected:
        //QFESPIMB040MainWindow* m_parent;
        QFPluginLogService* m_log;
        QFPluginServices* m_pluginServices;


        void createWidgets();
        void createActions();

        /** \brief combobox to select a shutter */
        QFShutterComboBox* cmbShutter;

        /** \brief tool button to connect to a shutter */
        QToolButton* btnConnect;

        /** \brief tool button to configure a shutter */
        QToolButton* btnConfigure;

        /** \brief button to display/change the shutter state */
        QToolButton* btnState;

        /** \brief action to connect to a shutter */
        QAction* actConnect;

        /** \brief action to configure to a shutter */
        QAction* actConfigure;
        QAction* actState;

        QTimer* timUpdate;

        bool locked;

        double shutterStateUpdateInterval;


        QTimer timerDisplayUpdate;

    public:

        /** \brief return a pointer to the shutter */
        QFExtensionShutter* getShutter();

        /** \brief return a pointer to the ahutter QFExtension class */
        QFExtension* getShutterExtension();

        QPixmap iconOpened;
        QPixmap iconClosed;


        /** \brief get the shutter number of the given axis */
        int getShutterID();

        bool isShutterConnected() const;

    public slots:
        void setShutter(bool shutterOpened);

    protected slots:
        void disConnect();
        void configure();
        void displayShutterStates();
    public slots:
        void shutterActionClicked(bool shutterOpened);
};

#endif // QFESPIMB040ShutterConfig_H
