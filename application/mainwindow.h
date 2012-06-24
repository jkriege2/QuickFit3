
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QTimer>
#include "qfproject.h"
#include "qfprojecttreemodel.h"
#include "qfrawdatapropertyeditor.h"
#include "qfevaluationpropertyeditor.h"
#include "qfevaluationitem.h"
#include "qfevaluationeditor.h"
#include "qfevaluationitemfactory.h"
#include "programoptions.h"
#include "qtlogfile.h"
#include "qfrawdatarecordfactory.h"
#include "qfpluginservices.h"
#include "qffitfunctionmanager.h"
#include "qffitalgorithmmanager.h"
#include "qfhtmlhelpwindow.h"
#include "qfextensionmanager.h"
#include "qfimportermanager.h"
#include "qfextension.h"
#include "../version.h"
#include "qvisiblehandlesplitter.h"
#include "qrecentfilesmenu.h"



/*! \brief main widget for QuickFit
    \ingroup qf3app
*/
class MainWindow : public QMainWindow, public QFPluginServices {
        Q_OBJECT
        Q_INTERFACES(QFPluginServices)
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

        /** \copydoc QFPluginServices::log_global_text() */
        virtual void log_global_text(QString message);
        /** \copydoc QFPluginServices::log_global_warning() */
        virtual void log_global_warning(QString message);
        /** \copydoc QFPluginServices::log_global_error() */
        virtual void log_global_error(QString message);
        /** \copydoc QFPluginServices::log_global_indent() */
        virtual void log_global_indent();
        /** \copydoc QFPluginServices::log_global_unindent() */
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
        /** \copydoc QFPluginServices::getGlobalConfigFileDirectory() */
        virtual QString getGlobalConfigFileDirectory();

        /** \copydoc QFPluginServices::getHTMLReplacementList() */
        virtual QList<QPair<QString, QString> >* getHTMLReplacementList();
        /** \copydocQFPluginServices::getPluginHelpList() */
        virtual QList<QFPluginServices::HelpDirectoryInfo>* getPluginHelpList();
        /** \copydoc QFPluginServices::getAssetsDirectory() */
        virtual QString getAssetsDirectory();
        /** \copydoc QFPluginServices::getPluginsDirectory() */
        virtual QString getPluginsDirectory();

        /** \copydoc QFPluginServices::getMenu() */
        virtual QMenu* getMenu(QString menu);

        /** \copydoc QFPluginServices::getToolbar() */
        virtual QToolBar* getToolbar(QString toolbar);

        /** \copydoc QFPluginServices::insertMenu() */
        virtual void insertMenu(QString menuname, QMenu* newMenu, QMenu* before=NULL);

        /** \copydoc QFPluginServices::insertToolBar() */
        virtual void insertToolBar(QString toolbarname, QToolBar* newToolbar);

        /** \copydoc QFPluginServices::getExtensionManager() */
        virtual QFExtensionManager* getExtensionManager();

        /** \copydoc QFPluginServices::getImporterManager() */
        virtual QFImporterManager* getImporterManager();

        /** \brief QFPluginServices::displayHelpWindow() */
        virtual void displayHelpWindow(const QString& helpfile=QString(""));

        /** \brief QFPluginServices::getPluginHelp() */
        virtual QString getPluginHelp(const QString& pluginID);

        /** \brief QFPluginServices::getPluginTutorial() */
        virtual QString getPluginTutorial(const QString& pluginID);

        /** \brief QFPluginServices::getPluginSettings() */
        virtual QString getPluginSettings(const QString& pluginID);

        /** \brief QFPluginServices::getPluginHelpDirectory() */
        virtual QString getPluginHelpDirectory(const QString& pluginID);

        /** \brief register a configuration pane for a plugin in the main options dialog */
        virtual void registerSettingsPane(QFPluginOptionsDialogInterface* plugin);
    protected:
        void closeEvent(QCloseEvent *event);

    private slots:
        /** \brief create a new project */
        void newProject();
        /** \brief close the current project */
        void closeProject();
        /** \brief display an open project dialog and open the selected project */
        void openProject();
        /** \brief load project from recent projects menu */
        void openRecentProject();
        void openRecentProject(const QString &filename);
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
        /** \brief display copyright help */
        void displayHelpCopyright();
        /** \brief display plugin copyright help */
        void displayHelpPluginCopyright();
        /** \brief display tutorials */
        void displayHelpTutorials();
        /** \brief display tutorials */
        void displayPluginHelp();

        /** \brief whenever a new project is created this is called every 500ms, until the project has been save at least once. This function asks the suer to save the project when he adds the first items */
        void saveProjectFirstTime();

        void rdrReplace();
        void rdrUndoReplace();

        void projectPerformanceTest();
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
        //void updateRecentFileActions();
        QString createPluginDoc(bool docLinks=false);
        QString createPluginDocTutorials(QString mainitem_before=QObject::tr("<h2>%1 Tutorials:</h2><ul>"), QString mainitem_after=QString("</ul>"));
        QString createPluginDocSettings(QString mainitem_before=QObject::tr("<h2>%1 Settings:</h2><ul>"), QString mainitem_after=QString("</ul>"));
        QString createPluginDocCopyrights(QString mainitem_before=QObject::tr("<h2>%1 Copyrights:</h2><ul>"), QString mainitem_after=QString("</ul>"));
        QString createPluginDocHelp(QString mainitem_before=QObject::tr("<h2>%1 Help:</h2><ul>"), QString mainitem_after=QString("</ul>"));
        /** \brief create one item in the plugin listing, created in createPluginDoc(). Provide all the basic data. If you want additional row, supply them in additional where line i contains the i-th addition name and line i+1 contains the i-th addition value */
        QString createPluginDocItem(bool docLink, QString id, QString name, QString description, QString iconfilename, QString author, QString copyright, QString weblink, QString file, int verMajor, int verMinor, QStringList additional);
        void loadProject(const QString &fileName);
        bool saveProject(const QString &fileName);
        void setCurrentProject(const QString &fileName);
        QString strippedName(const QString &fullFileName);

        QString curFile;
        QPointer<QFProject> project;

        QList<QPair<QString, QString> > htmlReplaceList;
        QList<QFPluginServices::HelpDirectoryInfo> pluginHelpList;

        QList<QFPluginOptionsDialogInterface*> pluginOptionDialogs;

        QTimer newProjectTimer;

        QMenu *toolsMenu;
        QMenu *projectToolsMenu;
        QMenu *debugToolsMenu;
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
        QList<QAction*> helpActList;

        QAction* helpCopyrightAct;
        QAction* helpPluginCopyrightAct;
        QAction* helpTutorialsAct;
        QAction* helpPluginAct;


        QAction* optionsAct;
        QAction* actRDRReplace;
        QAction* actRDRUndoReplace;
        QAction* actPerformanceTest;
        QProgressBar* prgMainProgress;

        /** \brief length of the list of recently opened files */
        QRecentFilesMenu* recentMenu;
        /*enum { MaxRecentFiles = 5 };
        QAction *recentFileActs[MaxRecentFiles];
        QMenu* recentMenu;*/


        QAction* delItemAct;
        /*QAction* insertRDTableAct;
        QAction* insertFCSFileAct;
        QAction* insertRDTableFileAct;*/

        QTreeView* tvMain;
        QVisibleHandleSplitter* spMain;
        QVisibleHandleSplitter* spCenter;

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
        /** \brief QFImporter manager */
        QFImporterManager* importerManager;

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
