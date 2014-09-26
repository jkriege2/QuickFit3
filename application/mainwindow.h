/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

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
#include "qfversion.h"
#include "qvisiblehandlesplitter.h"
#include "qrecentfilesmenu.h"
#include "qfhistogramservice.h"
#include "qfhistogramview.h"
#include "qfelidedlabel.h"
#include "qfmathparser.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include "qmodernprogresswidget.h"
#include "userfitfunctionseditor.h"
#include "qfparametercorrelationservice.h"
#include "qfparametercorrelationview.h"
#include "qfsimpleplotservice.h"
#include "qfsimpleplotview.h"
#include "jkqtpimagetools.h"

/*! \brief main widget for QuickFit
    \ingroup qf3app
*/
class MainWindow : public QMainWindow, public QFPluginServices, public QFHistogramService, public QFParameterCorrelationService, public QFSimplePlotService {
        Q_OBJECT
        Q_INTERFACES(QFPluginServices)
    public:
        /** \brief class constructor
         *  \param splash a splash screen to use for status output during startup/construction
         */
        explicit MainWindow(ProgramOptions* s, QSplashScreen* splash);
        virtual ~MainWindow();


        /** \copydoc QFPluginServices::reloadCurrentProject()  */
        virtual void reloadCurrentProject();
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
        virtual QFFitFunctionManager* getFitFunctionManager() const;
        /** \copydoc QFPluginServices::getFitAlgorithmManager() */
        virtual QFFitAlgorithmManager* getFitAlgorithmManager()  const;
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
        /** \copydoc QFPluginServices::getHTMLReplacement() */
        virtual QString getHTMLReplacement(const QString& name);
        /** \copydoc QFPluginServices::setOrAddHTMLReplacement() */
        virtual void setOrAddHTMLReplacement(const QString& name, const QString& value);
        /** \copydoc QFPluginServices::appendOrAddHTMLReplacement() */
        virtual void appendOrAddHTMLReplacement(const QString& name, const QString& appendValue, const QString& separator=QString("\n"));


        /** \copydocQFPluginServices::getPluginHelpList() */
        virtual QList<QFHelpDirectoryInfo>* getPluginHelpList();
        /** \copydoc QFPluginServices::getAssetsDirectory() */
        virtual QString getAssetsDirectory();
        /** \copydoc QFPluginServices::getExamplesDirectory() */
        virtual QString getExamplesDirectory();
        /** \copydoc QFPluginServices::getMainHelpDirectory() */
        virtual QString getMainHelpDirectory();
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

        /** \copydoc QFPluginServices::registerWizard() */
        virtual void registerWizard(const QString& menu, QAction* action);

        /** \copydoc QFPluginServices::registerWizard() */
        virtual void registerWizard(const QString& menu, const QString &title, const QObject * receiver, const char * method, QAction** actOut=NULL);
        /** \copydoc QFPluginServices::registerWizard() */
        virtual void registerWizard(const QString& menu, const QString &title, QIcon icon, const QObject * receiver, const char * method, QAction** actOut=NULL);

        virtual void addQFMathParserRefernceDir(const QString& directory);
        virtual QStringList getQFMathParserRefernceDirs();

        /** \copydoc QFPluginServices::getExtensionManager() */
        virtual QFExtensionManager* getExtensionManager() const;

        /** \copydoc QFPluginServices::getImporterManager() */
        virtual QFImporterManager* getImporterManager() const;

        /** \copydoc QFPluginServices::getEvaluationItemFactory() */
        virtual QFEvaluationItemFactory* getEvaluationItemFactory() const;
        /** \copydoc QFPluginServices::getRawDataRecordFactory() */
        virtual QFRawDataRecordFactory* getRawDataRecordFactory() const;

        /** \brief QFPluginServices::displayHelpWindow() */
        virtual void displayHelpWindow(const QString& helpfile=QString(""));
        /** \brief QFPluginServices::displayHelpWindow() */
        virtual void displayMainHelpWindow(const QString& helpfile=QString(""));
        /** \brief QFPluginServices::displayPluginHelpWindow() */
        virtual void displayPluginHelpWindow(const QString& plugin_id, const QString& helpfile=QString(""));

        /** \brief QFPluginServices::getPluginHelp() */
        virtual QString getPluginHelp(const QString& pluginID);
        /** \brief QFPluginServices::getPluginTutorial() */
        virtual QString getPluginTutorial(const QString& pluginID);
        /** \brief QFPluginServices::getPluginFAQ() */
        virtual QString getPluginFAQ(const QString& pluginID);
        /** \brief QFPluginServices::getPluginName() */
        virtual QString getPluginName(const QString& pluginID);

        /** \brief QFPluginServices::getPluginSettings() */
        virtual QString getPluginHelpSettings(const QString& pluginID);

        /** \brief QFPluginServices::getPluginHelpDirectory() */
        virtual QString getPluginHelpDirectory(const QString& pluginID);

        /** \brief QFPluginServices::getFitFunctionHelp() */
        virtual QString getFitFunctionHelp(const QString& pluginID);

        /** \brief QFPluginServices::getFitAlgorithmHelp() */
        virtual QString getFitAlgorithmHelp(const QString& pluginID);

        /** \brief QFPluginServices::addToHelpFurtherReading() */
        virtual void addToHelpFurtherReading(const QString& text);

        /** \brief QFPluginServices::getImporterHelp() */
        virtual QString getImporterHelp(const QString& pluginID);
        /** \brief register a configuration pane for a plugin in the main options dialog */
        virtual void registerSettingsPane(QFPluginOptionsDialogInterface* plugin);
        /** \copydoc QFPluginServices::openEvaluationEditor() */
        virtual QFEvaluationPropertyEditor* openEvaluationEditor(QFEvaluationItem* eval, bool alwaysCreateNew=true);
        /** \copydoc QFPluginServices::openRawDataEditor() */
        virtual QFRawDataPropertyEditor* openRawDataEditor(QFRawDataRecord* rdr, bool alwaysCreateNew=true);
        /** \copydoc QFPluginServices::getCurrentProject() */
        virtual QFProject* getCurrentProject() const;

        /** \copydoc QFPluginServices::addGlobalFunction() */
        virtual void addGlobalFunction(const QString& name, qfmpCFunction function);
        /** \copydoc QFPluginServices::addGlobalVariable() */
        virtual void addGlobalVariable(const QString& name, double value);
        /** \copydoc QFPluginServices::addGlobalVariable() */
        virtual void addGlobalVariable(const QString& name, const QVector<double>& value);
        /** \copydoc QFPluginServices::addGlobalVariable() */
        virtual void addGlobalVariable(const QString& name, const QString& value);
        /** \copydoc QFPluginServices::addGlobalVariable() */
        virtual void addGlobalVariable(const QString& name, bool value);
        /** \copydoc QFPluginServices::getGlobalConfigValue() */
        virtual QVariant getGlobalConfigValue(const QString& key);
        /** \copydoc QFPluginServices::setGlobalConfigValue() */
        virtual void setGlobalConfigValue(const QString& key, const QVariant& value);

        struct ContentsEntry {
            QList<int> num;
            QString header;
            QString id;
            QString prefix;
            ContentsEntry();
        };

        /** \copydoc QFPluginServices::transformQF3HelpHTML() */
        virtual QString transformQF3HelpHTML(const QString& input_html, const QString& filename, bool removeNonReplaced=true, const QF3HelpReplacesList& more_replaces=QF3HelpReplacesList(), bool insertTooltips=false, bool dontCreatePics=false, bool isMainHelp=false);

        /** \brief return the assets directory  of the online help for the given plugin ID */
        virtual QString getPluginAssetsDirectory(const QString& pluginID);
        /** \brief return the examples directory  of the online help for the given plugin ID */
        virtual QString getPluginExamplesDirectory(const QString& pluginID);
        /** \brief return the config file directory of the online help for the given plugin ID */
        virtual QString getPluginConfigDirectory(const QString& pluginID);

        virtual void setProjectMode(bool projectModeEnabled=true, const QString& nonProjectTitle=QString("non-project mode"));

        virtual QFPluginHelpData &getPluginHelpData();

        /** \brief searches for the given inifile (or \c PLUGINID.ini if \a inifiles is empty) in a list of subdirectories:
         *
         *  This function searches in these directories, in this order:
         *    - global configuration directory
         *    - plugin assets directory
         *    - \c /plugins/PLUGINID/ subdirectory of config directory
         *    - config directory
         *
         *  Returns the path to the first file that exists. If no file exists, the path to the last file is returned
         */
        virtual QString getPluginMaybeGlobalSettings(const QString& pluginID, const QString& inifile=QString(""));


        virtual QWidget* getCreateView(const QString& name, const QString& title);
        virtual void clearView(const QString& name);
        virtual void addHistogramToView(const QString& name, const Histogram& histogram);


        virtual QWidget* getCreateParameterCorrelationView(const QString& name, const QString& title);
        virtual void clearParameterCorrelationView(const QString& name);
        virtual void addCorrelationToView(const QString& name, const ParameterCorrelation& data);

        virtual QWidget* getCreateSimplePlotView(const QString& name, const QString& title);
        virtual void clearSimplePlotView(const QString& name);
        virtual void addPlotToView(const QString& name, const PlotData& data);

        virtual QMap<QString, QFToolTipsData> getTooltips() const;
    protected:
        void closeEvent(QCloseEvent *event);
        virtual void showEvent(QShowEvent* event);

        QString projectFileFilter;
        QString projectSaveFileFilter;

    protected slots:
        void displayHelpCiting();
    private slots:
        /** \brief create a new project */
        void newProject();
        /** \brief close the current project */
        void closeProject();
        /** \brief display an open project dialog and open the selected project */
        void openProject();
        void openExample();

        /** \brief display an open project dialog and open the selected project, but let the user first select a subset of nodes to load */
        void openProjectSubset();
        /** \brief reload the current project file */
        void reloadProject();
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
        /** \brief emitted when a project element (in the treeview) is double-clicked
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
        /** \brief display FAQs */
        void displayHelpFAQ();
        /** \brief display help index for plugins */
        void displayPluginHelp();

        /** \brief whenever a new project is created this is called every 500ms, until the project has been save at least once. This function asks the suer to save the project when he adds the first items */
        void saveProjectFirstTime();

        void rdrReplace();
        void rdrUndoReplace();
        void rdrSetProperty();

        void projectPerformanceTest();

        void contactAuhtors();
        void contactMailinglist();
        void openWebpage();

        void fixFilesPathesInProject();

        void renameGroups();
        void setRDRPropertyByRegExp();
        void setRDRPropertyByExpression();

        void editUserFitFunctions();
        void closeUserFitEditor();

        /** \brief check the update.xml file on the QF3 webpage for a new version. If \a userRequest is \c true, the method waits until the request completes, before returning control to the user, otherwise it runs in the background */
        void checkUpdates(bool userRequest=true);
        void checkUpdatesAutomatic();
        void showUpdateInfo(QNetworkReply *reply);
        void openLabelLink(const QString& link);

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
        QString createFAQ();
        QString createPluginDocTutorials(QString mainitem_before=QObject::tr("<h2>%1 Tutorials:</h2><ul>"), QString mainitem_after=QString("</ul>"));
        QString createPluginDocSettings(QString mainitem_before=QObject::tr("<h2>%1 Settings:</h2><ul>"), QString mainitem_after=QString("</ul>"));
        QString createPluginDocCopyrights(QString mainitem_before=QObject::tr("<h2>%1 Copyrights:</h2><ul>"), QString mainitem_after=QString("</ul>"));
        QString createPluginDocHelp(QString mainitem_before=QObject::tr("<h2>%1 Help:</h2><ul>"), QString mainitem_after=QString("</ul>"));
        /** \brief create one item in the plugin listing, created in createPluginDoc(). Provide all the basic data. If you want additional row, supply them in additional where line i contains the i-th addition name and line i+1 contains the i-th addition value */
        QString createPluginDocItem(bool docLink, QString id, QString name, QString description, QString iconfilename, QString author, QString copyright, QString weblink, QString file, int verMajor, int verMinor, QStringList additional);
        void loadProject(const QString &fileName, bool subsetMode=false, const QSet<int>& rdrSelected=QSet<int>(), const QSet<int>& evalSelected=QSet<int>());
        bool saveProject(const QString &fileName);
        void setCurrentProject(const QString &fileName);
        QString strippedName(const QString &fullFileName);

        bool projectModeEnabled;
        QString nonprojectTitle;

        QString curFile;
        QPointer<QFProject> project;

        QList<QPair<QString, QString> > htmlReplaceList;
        //QList<QFPluginServices::HelpDirectoryInfo> pluginHelpList;
        //QMap<QString, QFToolTipsData> tooltips;
        //QMap<QString, QFFAQData> faqs;

        QFPluginHelpData helpdata;


        QList<QFPluginOptionsDialogInterface*> pluginOptionDialogs;
        QMap<QString, QFHistogramView*> histograms;
        QMap<QString, QFParameterCorrelationView*> correlationViews;
        QMap<QString, QFSimplePlotView*> plotViews;
        QMap<QString, QVariant> globalParameterStore;
        QString lastHistogram;
        QString lastParamCorr;

        QPointer<UserFitFunctionsEditor> dlgUserFitFunctionEditor;
        QTimer newProjectTimer;

        QMenu *toolsMenu;
        QMenu *projectToolsMenu;
        QMenu *debugToolsMenu;
        QMenu *fileMenu;
        QMenu* projectSpecialMenu;
        QMenu *dataMenu;
        QMenu *evaluationMenu;
        QMenu *helpMenu;
        QMenu* wizardsMenu;
        QMenu* projectWizardsMenu;
        QMenu* rdrWizardsMenu;
        QMenu* evalWizardsMenu;
        QMenu* insertItemMenu;
        QMenu* extensionMenu;
        QToolBar *fileToolBar;
        QToolBar *dataToolBar;
        QToolBar* extensionToolBar;
        QToolBar* toolsToolBar;
        QAction *newProjectAct;
        QAction *openProjectAct;
        QAction *openProjectSubsetAct;
        QAction *openExampleAct;
        QAction *saveProjectAct;
        QAction *saveProjectAsAct;
        QAction* actReloadProject;
        QAction *exitAct;
        QAction *aboutAct;
        QAction *aboutQtAct;
        QAction *aboutPluginsAct;
        QAction* helpAct;
        QAction* helpCitingAct;
        QList<QAction*> helpActList;

        QAction* helpCopyrightAct;
        QAction* helpPluginCopyrightAct;
        QAction* helpTutorialsAct;
        QAction* helpFAQAct;
        QAction* helpPluginAct;
        QAction* helpContactAuthors;
        QAction* helpContactMaillinglist;
        QAction* helpOpenWebpageAct;
        QAction* actCheckUpdate;


        QAction* actUserFitfunctionsEditor;
        QAction* optionsAct;
        QAction* actRDRReplace;
        QAction* actRDRUndoReplace;
        QAction* actPerformanceTest;
        QAction* actRDRSetProperty;
        QAction* actFixFilesPathes;
        QAction* actRenameGroups;
        QAction* actSetRDRPropertyByRegExp;
        QAction* actSetRDRPropertyByExpression;
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

        QFElidedLabel* labFile;
        QPlainTextEdit* pteDescription;
        QLineEdit* edtName;
        QLineEdit* edtCreator;

        QtLogFile* logFileProjectWidget;
        QtLogFile* logFileQDebugWidget;
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

        QStringList mathParserRefDirs;

        /** \brief help display widget */
        QFHTMLHelpWindow* helpWindow;

        /** \brief used to store the splash screen pixmap, as given to the constructor. This pixmap may be used for info dialogs. */
        QPixmap splashPix;

        QNetworkAccessManager networkManager;
        QNetworkReply* lastUpdateRequest;
        QNetworkReply* lastUpdateRequestUser;
        QLabel* labUpgrade;

        struct updateWarning {
            int warnSince;
            QString message;
        };

        struct updateInfo {
            QString description;
            int latestVersion;
            QString date;
            QString releasenotes;
            QString link;
            QString download;
            QString os;
            QList<updateWarning> warnings;
            bool valid;
        };

        updateInfo readUpdateInfo(QIODevice* io);
};



#endif
