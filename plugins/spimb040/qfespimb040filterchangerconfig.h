#ifndef QFESPIMB040FilterChangerConfig_H
#define QFESPIMB040FilterChangerConfig_H

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
#include "../interfaces/qfextensionfilterchanger.h"
#include "qfextension.h"
#include "tools.h"
#include "qffilterchangercombobox.h"


/*! \brief SPIM Control Extension (B040, DKFZ Heidelberg) QWidget with a set of controls that allow to control a FilterChanger
    \ingroup qf3ext_spimb040


 */
class QFESPIMB040FilterChangerConfig : public QWidget {
        Q_OBJECT
    public:
        /** Default constructor */
        QFESPIMB040FilterChangerConfig(QWidget* parent=NULL);
        /** Default destructor */
        virtual ~QFESPIMB040FilterChangerConfig();

        void init(QFPluginLogService* log, QFPluginServices* pluginServices);
        void setLog(QFPluginLogService* log);

        /** \brief load settings */
        void loadSettings(QSettings& settings, QString prefix);
        /** \brief save settings */
        void saveSettings(QSettings& settings, QString prefix);

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


    public slots:
        void setReadOnly(bool readonly);

    protected slots:
        void updateStates();
        void updateFilters();

    protected:
        //QFESPIMB040MainWindow* m_parent;
        QFPluginLogService* m_log;
        QFPluginServices* m_pluginServices;


        void createWidgets();
        void createActions();

        /** \brief combobox to select a FilterChanger */
        QFFilterChangerComboBox* cmbFilterChanger;

        /** \brief tool button to connect to a FilterChanger */
        QToolButton* btnConnect;

        /** \brief tool button to configure a FilterChanger */
        QToolButton* btnConfigure;

        /** \brief button to display/change the FilterChanger state */
        QComboBox* cmbFilter;

        /** \brief action to connect to a FilterChanger */
        QAction* actConnect;

        /** \brief action to configure to a FilterChanger */
        QAction* actConfigure;

        QTimer* timUpdate;

        bool locked;

        double FilterChangerStateUpdateInterval;


        QTimer timerDisplayUpdate;

    public:

        /** \brief return a pointer to the FilterChanger */
        QFExtensionFilterChanger* getFilterChanger();

        /** \brief return a pointer to the ahutter QFExtension class */
        QFExtension* getFilterChangerExtension();


        bool moving;


        /** \brief get the FilterChanger number of the given axis */
        int getFilterChangerID();

        bool isFilterChangerConnected() const;

    public slots:
        void setFilterChanger(int filter);

    protected slots:
        void disConnect();
        void configure();
        void displayFilterChangerStates();
    public slots:
        void FilterChangerNewFilterSelected(int index);
};

#endif // QFESPIMB040FilterChangerConfig_H
