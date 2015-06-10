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

#ifndef QFPLUGINSERVICES_H
#define QFPLUGINSERVICES_H
#include <QTextStream>
#include "lib_imexport.h"
#include <QString>
#include <QSettings>
#include <QToolBar>
#include <QMenu>


class QFFitAlgorithmManager; // forward declarations
class QFFitFunctionManager;
class QFExtensionManager;
class ProgramOptions;
class QFPlugin;
class QFPluginOptionsDialogInterface;
class QFImporterManager;
class QFExporterManager;
class QFEvaluationItem;
class QFEvaluationPropertyEditor;
class QFRawDataRecord;
class QFRawDataPropertyEditor;
class QFProject;
class QFMathParser;
struct qfmpResult;
class QFEvaluationItemFactory;
class QFRawDataRecordFactory;

typedef qfmpResult (*qfmpCFunction)(const qfmpResult*, unsigned int, QFMathParser*);
typedef QList<QPair<QString, QString> > QF3HelpReplacesList;


/*! \brief wrapper class that allows plugins to access basic logging services
    \ingroup qf3plugintools
*/
class QFLIB_EXPORT QFPluginLogService {
    public:
        /** Default destructor */
        virtual ~QFPluginLogService() {}

        /** \brief indent all following lines in the logging pane */
        //virtual void log_indent()=0;
        /** \brief undo former log_indent() */
        //virtual void log_unindent()=0;

        /** \brief log project text message
         *  \param message the message to log
         */
        virtual void log_text(QString message)=0;
        /** \brief log project warning message
         *  \param message the warning message to log
         */
        virtual void log_warning(QString message)=0;
        /** \brief log project error message
         *  \param message the error message to log
         */
        virtual void log_error(QString message)=0;
};


Q_DECLARE_INTERFACE(QFPluginLogService,
                     "www.dkfz.de.b040.quickfit3.QFPluginLogService/1.0")

struct QFLIB_EXPORT QFToolTipsData{
    QString tooltip;
    QString tooltipfile;
};

struct QFLIB_EXPORT  FAQEntry {
        QString question;
        QString answer;
        QString link;
};

typedef QList<FAQEntry> QFFAQData;

struct QFLIB_EXPORT  QFHelpDirectoryInfo {
        QString directory;
        QFPlugin* plugin;
        QString tutorial;
        QString settings;
        QString mainhelp;
        QString faq;
        QString faq_parser;
        QString plugintypehelp;
        QString plugintypename;
        QString pluginDLLbasename;
        QString pluginDLLSuffix;
};

struct QFLIB_EXPORT QFPluginHelpData {
    QList<QFHelpDirectoryInfo> pluginHelpList;
    QMap<QString, QFToolTipsData> tooltips;
    QMap<QString, QString> autolinks;
    QMap<QString, QFFAQData> faqs;
};

/*! \brief wrapper class that allows plugins to access basic services of the QuickFit application
    \ingroup qf3plugintools

    This class is a singleton in the context of QuickFit3, so only one instance exists per running QuickFit3 instance. You can get a
    pointer to this instance using the static function getInstance().
*/
class QFLIB_EXPORT QFPluginServices {
    public:


        QFPluginServices();

        /** Default destructor */
        virtual ~QFPluginServices();

        /** \brief reload the current project */
        virtual void reloadCurrentProject()=0;
        /** \brief indent all following lines in the logging pane */
        virtual void log_indent()=0;
        /** \brief undo former log_indent() */
        virtual void log_unindent()=0;

        /** \brief log project text message
         *  \param message the message to log
         */
        virtual void log_text(QString message)=0;
        /** \brief log project warning message
         *  \param message the warning message to log
         */
        virtual void log_warning(QString message)=0;
        /** \brief log project error message
         *  \param message the error message to log
         */
        virtual void log_error(QString message)=0;
        /** \brief set status message */
        virtual void setStatusMessage(QString message)=0;
        /** \brief set range of progress bar
         *  \param minP minimum of progress range
         *  \param maxP maximum of progress range
         */
        virtual void setProgressRange(int minP, int maxP)=0;
        /** \brief set progress barto \a value
         *  \param value the progress value (in the range given by setProgressRange() )
         */
        virtual void setProgress(int value)=0;
        /** increment progress bar by 1  */
        virtual void incProgress()=0;
        /** \brief return a pointer to a fit function manager object */
        virtual QFFitFunctionManager* getFitFunctionManager() const=0;
        /** \brief return a pointer to a fit function manager object */
        virtual QFFitAlgorithmManager* getFitAlgorithmManager() const=0;
        /** \brief returns a QFExtensionManager object that allows access to all currently loaded QFExtensions */
        virtual QFExtensionManager* getExtensionManager() const=0;
        /** \brief returns a QFImporterManager object that allows access to all currently loaded QFImporter s */
        virtual QFImporterManager* getImporterManager() const=0;
        /** \brief returns a QFExporterManager object that allows access to all currently loaded QFExporter s */
        virtual QFExporterManager* getExporterManager() const=0;
        /** \brief returns the QFEvaluationItemFactory of the current QF3 instance */
        virtual QFEvaluationItemFactory* getEvaluationItemFactory() const=0;
        /** \brief returns the QFRawDataRecordFactory of the current QF3 instance */
        virtual QFRawDataRecordFactory* getRawDataRecordFactory() const=0;

        /** \brief return a pointer to a valid QSettings object for the main settings INI file */
        virtual QSettings* getSettings()=0;
        /** \brief return a pointer to the applications ProgramOptions object */
        virtual ProgramOptions* getOptions()=0;
        /** \brief return a base path under which to store additional configuration files */
        virtual QString getConfigFileDirectory()=0;
        /** \brief return a base path from which to read global configuration files */
        virtual QString getGlobalConfigFileDirectory()=0;
        /** \brief return a base path where to find assets files */
        virtual QString getAssetsDirectory()=0;
        /** \brief return a base path where to find examples files */
        virtual QString getExamplesDirectory()=0;
        /** \brief return a base path where to find QuickFits main help files */
        virtual QString getMainHelpDirectory()=0;
        /** \brief return a base path where to find plugins */
        virtual QString getPluginsDirectory()=0;
        /** \brief a list of replacements for the online help system.
         *
         *  Each entry in this list has a \c key, and some text. The QFHTMLHelpWindow will search through the html code for an
         *  occurence of \c $$key and replace any such occurence by the given value.
         */
        virtual QList<QPair<QString, QString> >* getHTMLReplacementList()=0;
        /** \brief returns the first element with name \a name from the HTML help replacement list
          *
          * \note edits the HTML help replacement list, accessed by getHTMLReplacementList() (i.e. this is a convenience function!)
          */
        virtual QString getHTMLReplacement(const QString& name)=0;
        /** \brief either replaces the value of the first element with name \a name in the HTML help replacement list with \a value, or append the new value.
          *
          * \note edits the HTML help replacement list, accessed by getHTMLReplacementList() (i.e. this is a convenience function!)
          */
        virtual void setOrAddHTMLReplacement(const QString& name, const QString& value)=0;
        /** \brief either appends the value \a appendValue to the first element with name \a name in the HTML help replacement list, or append the new value \a appendValue.
          *        The \a separator is used to separate \a appendValue and the old value, if the latter one exists.
          *
          * \note edits the HTML help replacement list, accessed by getHTMLReplacementList() (i.e. this is a convenience function!)
          */
        virtual void appendOrAddHTMLReplacement(const QString& name, const QString& appendValue, const QString& separator=QString("\n"))=0;

        /** \brief add a directory, which contains global math parser online-help
          */
        virtual void addQFMathParserRefernceDir(const QString& directory)=0;
        /** \brief get a list of directories, which contains global math parser online-help
          */
        virtual QStringList getQFMathParserRefernceDirs()=0;


        /** \brief a list of the online help directories of all plugins with metadata.
         */
        virtual QList<QFHelpDirectoryInfo>* getPluginHelpList()=0;
        /** \brief returns a list of tooltips for the online-help */
        virtual QMap<QString, QFToolTipsData> getTooltips() const=0;

        virtual QFPluginHelpData& getPluginHelpData() =0;

        /** \brief display the help window and open the given file. If no file is given, des QuickFit main help page is shown. */
        virtual void displayHelpWindow(const QString& helpfile=QString(""))=0;

        /** \brief display the help window and open the given file inside the given plugin's help directory, or the plugins main help page, if \a helpfile is left blank. */
        virtual void displayPluginHelpWindow(const QString& plugin_id, const QString& helpfile=QString(""))=0;

        /** \brief display the help window and open the given file. If no file is given, des QuickFit main help page is shown. */
        virtual void displayMainHelpWindow(const QString& helpfile=QString(""))=0;

        /** \brief return the directory of the online help for the given plugin ID */
        virtual QString getPluginHelpDirectory(const QString& pluginID)=0;

        /** \brief return the main help file for the given plugin ID */
        virtual QString getPluginHelp(const QString& pluginID)=0;

        /** \brief return the help file for the given ft function ID */
        virtual QString getFitFunctionHelp(const QString& pluginID)=0;

        /** \brief return the help file for the given ft function ID */
        virtual QString getFitAlgorithmHelp(const QString& pluginID)=0;

        /** \brief return the help file for the given ft function ID */
        virtual QString getImporterHelp(const QString& pluginID)=0;

        /** \brief return the help file for the given ft function ID */
        virtual QString getExporterHelp(const QString& pluginID)=0;


        /** \brief return the tutorial for the given plugin ID */
        virtual QString getPluginTutorial(const QString& pluginID)=0;

        /** \brief return the FAQ for the given plugin ID */
        virtual QString getPluginFAQ(const QString& pluginID)=0;

        /** \brief return the name for the given plugin ID */
        virtual QString getPluginName(const QString& pluginID)=0;

        /** \brief return the settings for the given plugin ID */
        virtual QString getPluginHelpSettings(const QString& pluginID)=0;
        /*! \brief transforms some special tags in the given input html
            \ingroup qf3lib_tools

            This functions performs SOME of the transformations described in \pageref qf3helpsystem .

            It does not (amongst others):
              - add standard headers/footers
              - literature references
              - table of contents
            .

            It does (amongst others):
              -
            .
         */
        virtual QString transformQF3HelpHTML(const QString& input_html, const QString& filename, bool removeNonReplaced=true, const QF3HelpReplacesList& more_replaces=QF3HelpReplacesList(), bool insertTooltips=false, bool dontCreatePics=false, bool isMainHelp=false)=0;


        /** \brief add a substring to the further reading list in the onine-help
         *
         * The substring should contain one or more $$invisible_ref::$$ entry/ies, which will be displayed on the further_reading page */
        virtual void addToHelpFurtherReading(const QString& text)=0;

        /** \brief returns a global configuration value. This can be used to implement an application wide communication/exchange
          *
          * \see setGlobalConfigValue()
          */
        virtual QVariant getGlobalConfigValue(const QString& key)=0;
        /** \brief sets a global configuration value. This can be used to implement an application wide communication/exchange
          *
          * \see getGlobalConfigValue()
          */
        virtual void setGlobalConfigValue(const QString& key, const QVariant& value)=0;



        /** \brief return the assets directory for the given plugin ID */
        virtual QString getPluginAssetsDirectory(const QString& pluginID)=0;
        /** \brief return the examples directory for the given plugin ID */
        virtual QString getPluginExamplesDirectory(const QString& pluginID)=0;
        /** \brief return the config directory for the given plugin ID */
        virtual QString getPluginConfigDirectory(const QString& pluginID)=0;

        /** \brief searches for the given inifile (or \c PLUGINID.ini if \a inifiles is empty) in a list of subdirectories:
         *
         *  This function searches in these directories, in this order:
         *    - global configuration directory
         *    - plugin assets directory
         *    - config directory
         *    - \c /plugins/PLUGINID/ subdirectory of config directory
         */
        virtual QString getPluginMaybeGlobalSettings(const QString& pluginID, const QString& inifile=QString(""))=0;



        /*! \brief return a pointer to a given menu, or NULL

            \param menu the menu to return. Some common menus are predefined: \c \"file\", \c \"data\",
                   \c \"help\", \c \"extensions\"
            \return a pointer to the given menu, or NULL if an invalid \a menu name was given.


         */
        virtual QMenu* getMenu(QString menu)=0;

        /*! \brief return a pointer to a given toolbar, or NULL

            \param toolbar the toolbar to return. Some common toolbars are predefined: \c \"file\", \c \"data\",
                   \c \"extensions\", \c \"data/rdr\", \c \"data/eval\"
            \return a pointer to the given toolbar, or NULL if an invalid \a toolbar name was given.

         */
        virtual QToolBar* getToolbar(QString toolbar)=0;

        /** \brief insert a new menu on the right of \a before, or at the left border when \c before=NULL
         *  \param menuname a name designator used to access the menu later on by getMenu()
         *  \param newMenu the menu to insert
         *  \param before insert \a newMenu before this menu
         */
        virtual void insertMenu(QString menuname, QMenu* newMenu, QMenu* before=NULL)=0;

        /** \brief insert a new menu on the right of \a before, or at the left border when \c before=NULL
         *  \param toolbarname a name designator used to access the toolbar later on by getToolbar()
         *  \param newToolbar the toolbar to insert
         */
        virtual void insertToolBar(QString toolbarname, QToolBar* newToolbar)=0;

        /** \brief indent all following lines in the global logging pane */
        virtual void log_global_indent()=0;
        /** \brief undo former log_global_indent() */
        virtual void log_global_unindent()=0;

        /** \brief log global text message
         *  \param message the message to log
         */
        virtual void log_global_text(QString message)=0;
        /** \brief log global warning message
         *  \param message the warning message to log
         */
        virtual void log_global_warning(QString message)=0;
        /** \brief log global error message
         *  \param message the error message to log
         */
        virtual void log_global_error(QString message)=0;

        /** \brief returns the application wide instance of the plugin services */
        static QFPluginServices* getInstance()  {
            return inst;
        }

        /** \brief register a configuration pane for a plugin in the main options dialog */
        virtual void registerSettingsPane(QFPluginOptionsDialogInterface* plugin)=0;

        /** \brief open a new evaluation editor window for the given evaluation
         *
         *   If \a alwaysCreateNew is \c false and an editor for the given evaluation is already opened, this opened editor is displayed and now new editor is created.
         *   If \a alwaysCreateNew is \c true a new editor window is always created.
         */
        virtual QFEvaluationPropertyEditor* openEvaluationEditor(QFEvaluationItem* eval, bool alwaysCreateNew=true)=0;
        /** \brief open a new raw data record editor window for the given record
         *
         *   If \a alwaysCreateNew is \c false and an editor for the given record is already opened, this opened editor is displayed and now new editor is created.
         *   If \a alwaysCreateNew is \c true a new editor window is always created.
         */
        virtual QFRawDataPropertyEditor* openRawDataEditor(QFRawDataRecord* rdr, bool alwaysCreateNew=true)=0;
        /** \brief returns a pointer to the currently loaded QFProject */
        virtual QFProject* getCurrentProject() const=0;

        /** \brief  add a function to the list of global functions available in <u>all</u> QFMathParser objects, that are defined by a call its addStandardFunctions() (or the constructor) */
        virtual void addGlobalFunction(const QString& name, qfmpCFunction function)=0;
        /** \brief  add a variablen to the list of global variables available in <u>all</u> QFMathParser objects, that are defined by a call to addStandardVariables() (or the constructor) */
        virtual void addGlobalVariable(const QString& name, double value)=0;
        /** \brief  add a variablen to the list of global variables available in <u>all</u> QFMathParser objects, that are defined by a call to addStandardVariables() (or the constructor) */
        virtual void addGlobalVariable(const QString& name, const QVector<double>& value)=0;
        /** \brief  add a variablen to the list of global variables available in <u>all</u> QFMathParser objects, that are defined by a call to addStandardVariables() (or the constructor) */
        virtual void addGlobalVariable(const QString& name, const QString& value)=0;
        /** \brief  add a variablen to the list of global variables available in <u>all</u> QFMathParser objects, that are defined by a call to addStandardVariables() (or the constructor) */
        virtual void addGlobalVariable(const QString& name, bool value)=0;
        /** \brief disables all project editing capabilities for the QuickFit main window ... may be used if QF3 is used to control instruments to prevent the user from using projects! */
        virtual void setProjectMode(bool projectModeEnabled=true, const QString& nonProjectTitle=QString("non-project mode"))=0;

        /** \brief register a wizard */
        virtual void registerWizard(const QString& menu, QAction* action)=0;

        /** \brief register a wizard */
        virtual void registerWizard(const QString& menu, const QString &title, const QObject * receiver, const char * method, QAction** actOut=NULL)=0;
        /** \brief register a wizard */
        virtual void registerWizard(const QString& menu, const QString &title, QIcon icon, const QObject * receiver, const char * method, QAction** actOut=NULL)=0;
    private:
        static QFPluginServices* inst;
};


Q_DECLARE_INTERFACE(QFPluginServices,
                     "www.dkfz.de.b040.quickfit3.QFPluginServices/1.0")


/*! \brief this class provides static methods that allow to do status output in the main window (progress etc.)
    \ingroup qf3plugintools
*/
class QFLIB_EXPORT QFPluginProgressTools {
    public:
        ~QFPluginProgressTools() {};
        /** \brief set status message */
        static void setStatusMessage(QString message) {
            if (QFPluginServices::getInstance()) QFPluginServices::getInstance()->setStatusMessage(message);
        }
        /** \brief set range of progress bar
         *  \param minP minimum of progress range
         *  \param maxP maximum of progress range
         */
        static void setProgressRange(int minP, int maxP) {
            if (QFPluginServices::getInstance()) QFPluginServices::getInstance()->setProgressRange(minP, maxP);
        }
        /** \brief set progress barto \a value
         *  \param value the progress value (in the range given by setProgressRange() )
         */
        static void setProgress(int value) {
            if (QFPluginServices::getInstance()) QFPluginServices::getInstance()->setProgress(value);
        }
        /** increment progress bar by 1  */
        static void incProgress() {
            if (QFPluginServices::getInstance()) QFPluginServices::getInstance()->incProgress();
        }
};


/*! \brief this class provides static methods that allow to do logging output to the applications logging pane
    \ingroup qf3plugintools

    \note These functions are guarateed to be thread safe!!!
*/
class QFLIB_EXPORT QFPluginLogTools {
    public:
        ~QFPluginLogTools() {};

        /** \brief indent all following lines in the logging pane */
        static void log_indent() {
            if (QFPluginServices::getInstance()) QFPluginServices::getInstance()->log_indent();
        }
        /** \brief undo former log_indent() */
        static void log_unindent() {
            if (QFPluginServices::getInstance()) QFPluginServices::getInstance()->log_unindent();
        }

        /** \brief log project text message
         *  \param message the message to log
         */
        static void log_text(QString message) {
            if (QFPluginServices::getInstance()) QFPluginServices::getInstance()->log_text(message);
        }
        /** \brief log project warning message
         *  \param message the warning message to log
         */
        static void log_warning(QString message) {
            if (QFPluginServices::getInstance()) QFPluginServices::getInstance()->log_warning(message);
        }
        /** \brief log project error message
         *  \param message the error message to log
         */
        static void log_error(QString message) {
            if (QFPluginServices::getInstance()) QFPluginServices::getInstance()->log_error(message);
        }

        /** \brief indent all following lines in the global logging pane */
        static void log_global_indent() {
            if (QFPluginServices::getInstance()) QFPluginServices::getInstance()->log_global_indent();
        }
        /** \brief undo former log_global_indent() */
        static void log_global_unindent() {
            if (QFPluginServices::getInstance()) QFPluginServices::getInstance()->log_global_unindent();
        }

        /** \brief log global text message
         *  \param message the message to log
         */
        static void log_global_text(QString message) {
            if (QFPluginServices::getInstance()) QFPluginServices::getInstance()->log_global_text(message);
        }
        /** \brief log global warning message
         *  \param message the warning message to log
         */
        static void log_global_warning(QString message) {
            if (QFPluginServices::getInstance()) QFPluginServices::getInstance()->log_global_warning(message);
        }
        /** \brief log global error message
         *  \param message the error message to log
         */
        static void log_global_error(QString message) {
            if (QFPluginServices::getInstance()) QFPluginServices::getInstance()->log_global_error(message);
        }

};




#endif // QFPLUGINSERVICES_H
