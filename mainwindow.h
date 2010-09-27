
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "qfproject.h"
#include "qfprojecttreemodel.h"
#include "qfrawdatapropertyeditor.h"
#include "qfevaluationpropertyeditor.h"
#include "dlgcsvparameters.h"
#include "qfevaluationitem.h"
#include "qfevaluationeditor.h"
#include "qfevaluationitemfactory.h"
#include "programoptions.h"
#include "../lib/qt/qtlogfile.h"
#include "qfrawdatarecordfactory.h"


class MainWindow : public QMainWindow {
        Q_OBJECT

    public:
        MainWindow();
        ~MainWindow();

        void setSettings(ProgramOptions* s) {
            optionsAct->disconnect();
            settings=s;
            connect(optionsAct, SIGNAL(triggered()), settings, SLOT(openSettingsDialog()));
            readSettings();
        }

    protected:
        void closeEvent(QCloseEvent *event);

    private slots:
        void newProject();
        void openProject();
        bool saveProject();
        bool saveProjectAs();
        void about();
        /** \brief emitted when a project element (in the treeview) is doubl-clicked
         *
         * This will open a new window containing the elements properties and data.
         */
        void projectElementDoubleClicked ( const QModelIndex & index );

        /** \brief emitted when the project name changes */
        void projectNameChanged(const QString& text);

        /** \brief emitted when the project creator changes */
        void projectCreatorChanged(const QString& text);

        /** \brief emitted when the project description changes */
        void projectDescriptionChanged();

        /** \brief reads the project properties from the project into the edit widgets */
        void readProjectProperties();


        /** \brief delete the currently selected item */
        void deleteItem();

        void modelReset();

        void insertRawData();

        void insertEvaluation();


    private:
        void createWidgets();
        void createActions();
        void createMenus();
        void createToolBars();
        void createStatusBar();
        void readSettings();
        void writeSettings();
        bool maybeSave();
        void loadProject(const QString &fileName);
        bool saveProject(const QString &fileName);
        void setCurrentProject(const QString &fileName);
        QString strippedName(const QString &fullFileName);

        QString curFile;
        QFProject* project;

        QMenu *fileMenu;
        QMenu *dataMenu;
        QMenu *evaluationMenu;
        QMenu *helpMenu;
        QMenu* insertMenu;
        QToolBar *fileToolBar;
        QToolBar *dataToolBar;
        QAction *newProjectAct;
        QAction *openProjectAct;
        QAction *saveProjectAct;
        QAction *saveProjectAsAct;
        QAction *exitAct;
        QAction *aboutAct;
        QAction *aboutQtAct;
        QAction* optionsAct;
        QProgressBar* prgMainProgress;

        QAction* delItemAct;
        /*QAction* insertRDTableAct;
        QAction* insertFCSFileAct;
        QAction* insertRDTableFileAct;*/

        QTreeView* tvMain;
        QSplitter* spMain;
        QSplitter* spCenter;

        QLabel* labFile;
        QPlainTextEdit* pteDescription;
        QLineEdit* edtName;
        QLineEdit* edtCreator;

        QtLogFile* logFileProjectWidget;
        QtLogFile* logFileMainWidget;
        QTabWidget* tabLogs;


        QList<QPointer<QFRawDataPropertyEditor> > rawDataPropEditors;
        QList<QPointer<QFEvaluationPropertyEditor> > evaluationPropEditors;
        /** \brief the last project open/save directory */
        QString currentProjectDir;
        /** \brief the last rawdata insert directory */
        QString currentRawDataDir;
        /** \brief the last FCS fileformat filter */
        QString currentFCSFileFormatFilter;
        /** \brief ProgramOptions object used for the application settings */
        ProgramOptions* settings;
        QString column_separator;
        QString decimal_separator;
        QString comment_start;
        QString header_start;

        /** \brief list of functions to call when a QAction is triggered.. */
        QList<QFRawDataRecordFactory::QFRawDataRecordUserCreateFunc> qfrdrUserCreateList;
        /** \brief list of functions to call when a QAction is triggered.. */
        QList<QFEvaluationItemFactory::QFEvaluationItemUserCreateFunc> qfeiUserCreateList;
};

#endif
