
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QTimer>
#include "../lib/qfproject.h"
#include "../lib/qfprojecttreemodel.h"
#include "../lib/qfrawdatapropertyeditor.h"
#include "../lib/qfevaluationpropertyeditor.h"
#include "../lib/qfevaluationitem.h"
#include "../lib/qfevaluationeditor.h"
#include "../lib/qfevaluationitemfactory.h"
#include "../lib/programoptions.h"
#include "qt/qtlogfile.h"
#include "../lib/qfrawdatarecordfactory.h"
#include "../lib/qfpluginservices.h"
#include "../lib/qffitfunctionmanager.h"
#include "../lib/qffitalgorithmmanager.h"
#include "../lib/qfhtmlhelpwindow.h"
#include "../lib/qfextensionmanager.h"
#include "../lib/qfextension.h"


#define QF_THANKS_TO "Dr. Nicolas Dross, Dr. György Vámosi, Prof. Jörg Langowski, Dr. Katalin Tòth, Vera Böhm, Tabea Elbel"
#define QF_COPYRIGHT "&copy; 2010 by Jan Krieger"

/*! \brief main widget for QuickFit
    \ingroup qf3app
*/
class MainWindow : public QMainWindow, public QFExtensionServices {
        Q_OBJECT

    public:
        /** \brief class constructor
         *  \param splash a splash screen to use for status output during startup/construction
         */
        MainWindow(ProgramOptions* s, QSplashScreen* splash);
        ~MainWindow();



        /** \copydoc QFPluginServices::log_text()  */
        virtual void log_text(QString message);
        /** \copydoc QFPluginServices::log_warning()  */
        virtual void log_warning(QString message);
        /** \copydoc QFPluginServices::log_error()  */
        virtual void log_error(QString message);
        /** \copydoc QFPluginServices::log_indent() */
        virtual void log_indent();
        /** \copydoc QFPluginServices::log_unindent() */
        virtual void log_unindent();

        /** \copydoc QFExtensionServices::log_global_text() */
        virtual void log_global_text(QString message);
        /** \copydoc QFExtensionServices::log_global_warning() */
        virtual void log_global_warning(QString message);
        /** \copydoc QFExtensionServices::log_global_error() */
        virtual void log_global_error(QString message);
        /** \copydoc QFExtensionServices::log_global_indent() */
        virtual void log_global_indent();
        /** \copydoc QFExtensionServices::log_global_unindent() */
        virtual void log_global_unindent();

        /** \copydoc QFPluginServices::setStatusMessage()  */
        virtual void setStatusMessage(QString message);
        /** \copydoc QFPluginServices::setProgressRange()  */
        virtual void setProgressRange(int minP, int maxP);
        /** \copydoc QFPluginServices::setProgress()  */
        virtual void setProgress(int value);
        /** \copydoc QFPluginServices::incProgress()  */
        virtual void incProgress();
        /** \copydoc QFPluginServices::getFitFunctionManager() */
        virtual QFFitFunctionManager* getFitFunctionManager();
        /** \copydoc QFPluginServices::getFitAlgorithmManager() */
        virtual QFFitAlgorithmManager* getFitAlgorithmManager();
        /** \copydoc QFPluginServices::getSettings() */
        virtual QSettings* getSettings();
        /** \copydoc QFPluginServices::getOptions() */
        virtual ProgramOptions* getOptions();
        /** \copydoc QFPluginServices::getConfigFileDirectory() */
        virtual QString getConfigFileDirectory();

        /** \copydoc QFPluginServices::getAssetsDirectory() */
        virtual QString getAssetsDirectory();
        /** \copydoc QFPluginServices::getPluginsDirectory() */
        virtual QString getPluginsDirectory();

        /** \copydoc QFExtensionServices::getMenu() */
        virtual QMenu* getMenu(QString menu);

        /** \copydoc QFExtensionServices::getToolbar() */
        virtual QToolBar* getToolbar(QString toolbar);

        /** \copydoc QFExtensionServices::insertMenu() */
        virtual void insertMenu(QString menuname, QMenu* newMenu, QMenu* before=NULL);

        /** \copydoc QFExtensionServices::insertToolBar() */
        virtual void insertToolBar(QString toolbarname, QToolBar* newToolbar);

        /** \copydoc QFExtensionServices::getExtensionManager() */
        virtual QFExtensionManager* getExtensionManager();
    protected:
        void closeEvent(QCloseEvent *event);

    private slots:
        /** \brief create a new project */
        void newProject();
        /** \brief close the current project */
        void closeProject();
        /** \brief display an open project dialog and open the selected project */
        void openProject();
        /** \brief save current project */
        bool saveProject();
        /** \brief save current project with a new filename */
        bool saveProjectAs();
        /** \brief display informaion about QuickFit */
        void about();
        /** \brief display information about plugins */
        void aboutPlugins();
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

        /** \brief output a message to the application logging widget */
        void showLogMessage(const QString& message);

        /** \brief open settings dialog */
        void openSettingsDialog();

        /** \brief this slot is executed wvery minute and is used to store the program settings */
        void autoWriteSettings();

        /** \brief autosave the current project */
        void autosaveProject();

        /** \brief display help */
        void displayHelp();

        /** \brief whenever a new project is created this is called every 500ms, until the project has been save at least once. This function asks the suer to save the project when he adds the first items */
        void saveProjectFirstTime();
    private:
        void createWidgets();
        void createActions();
        void createMenus();
        void createToolBars();
        void createStatusBar();
        void searchAndRegisterPlugins();
        void readSettings();
        void writeSettings();
        bool maybeSave();
        QString createPluginDoc(bool docLinks=false);
        void loadProject(const QString &fileName);
        bool saveProject(const QString &fileName);
        void setCurrentProject(const QString &fileName);
        QString strippedName(const QString &fullFileName);

        QString curFile;
        QPointer<QFProject> project;

        QTimer newProjectTimer;

        QMenu *fileMenu;
        QMenu *dataMenu;
        QMenu *evaluationMenu;
        QMenu *helpMenu;
        QMenu* insertItemMenu;
        QMenu* extensionMenu;
        QToolBar *fileToolBar;
        QToolBar *dataToolBar;
        QToolBar* extensionToolBar;
        QAction *newProjectAct;
        QAction *openProjectAct;
        QAction *saveProjectAct;
        QAction *saveProjectAsAct;
        QAction *exitAct;
        QAction *aboutAct;
        QAction *aboutQtAct;
        QAction *aboutPluginsAct;
        QAction* helpAct;
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

        QTimer* timerAutosave;

        QMap<QString, QPointer<QMenu> > menus;
        QMap<QString, QPointer<QToolBar> > toolbars;


        QList<QPointer<QFRawDataPropertyEditor> > rawDataPropEditors;
        QList<QPointer<QFEvaluationPropertyEditor> > evaluationPropEditors;
        /** \brief the last project open/save directory */
        QString currentProjectDir;
        /** \brief the last rawdata insert directory */
        QString currentRawDataDir;
        /** \brief ProgramOptions object used for the application settings */
        ProgramOptions* settings;

        /** \brief evaluation item factory */
        QFEvaluationItemFactory* evaluationFactory;
        /** \brief raw data record item factory */
        QFRawDataRecordFactory* rawDataFactory;
        /** \brief fit function manager class */
        QFFitFunctionManager* fitFunctionManager;
        /** \brief fit algorithm manager class */
        QFFitAlgorithmManager* fitAlgorithmManager;
        /** \brief QFExtension manager */
        QFExtensionManager* extensionManager;

        /** \brief help display widget */
        QFHTMLHelpWindow* helpWindow;

        /** \brief return a pointer to the raw data record factry object */
        inline QFRawDataRecordFactory* getRawDataRecordFactory() { return rawDataFactory; };
        /** \brief return a pointer to the evaluation item factry object */
        inline QFEvaluationItemFactory* getEvaluationItemFactory() { return evaluationFactory; };
        /** \brief used to store the splash screen pixmap, as given to the constructor. This pixmap may be used for info dialogs. */
        QPixmap splashPix;
};

#endif
