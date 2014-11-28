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

#include "mainwindow.h"
#include "optionsdialog.h"
#include "ui_about.h"
#include "ui_aboutplugins.h"
#include "qftools.h"
#include "qfrdrreplacedialog.h"
#include "statistics_tools.h"
#include "dlgcontactauthors.h"
#include "dlgnewversion.h"
#include "dlgrdrsetproperty.h"
#include "dlgselectprojectsubset.h"
#include "dlgfixfilepaths.h"
#include "dlgsetrdrpropertybyregexp.h"
#include "jkmathparser.h"
#include "qfhtmlhelptools.h"
#include "renamegroupsdialog.h"
#include "userfitfunctionseditor.h"
#include "qfparametercorrelationview.h"
#include "dlgsetrdrpropertybyexpression.h"
#include <QNetworkRequest>
#include <QNetworkProxy>
#include "qflistprogressdialog.h"
#include "qfimporterimageseries.h"
#include "qftcspcreader.h"
#include "dlgwelcomescreen.h"
#include "qfhelpaction.h"

static QPointer<QtLogFile> appLogFileQDebugWidget=NULL;


#if defined(Q_OS_WIN)
extern Q_CORE_EXPORT void qWinMsgHandler(QtMsgType t, const char* str);
#endif


void myMessageOutput(QtMsgType type, const char *msg)
 {
    QString m=msg;
    while (m.startsWith('\n') && m.size()>0) {
        m=m.remove(0,1);
    }
    if (appLogFileQDebugWidget) {
         switch (type) {
         case QtDebugMsg:
             appLogFileQDebugWidget->log_text_linebreak_queue(m);
             break;
         case QtWarningMsg:
             appLogFileQDebugWidget->log_warning_linebreak_queue(m);
             break;
         case QtCriticalMsg:
             appLogFileQDebugWidget->log_error_linebreak_queue(m);
             break;
         case QtFatalMsg:
             appLogFileQDebugWidget->log_error_linebreak_queue(QObject::tr("FATAL ERROR: %1\n   APPLICATION WILL BE CLOSED/ABORTED!").arg(m));
             //abort();
             break;
         }
    }

    #if defined(Q_OS_WIN)
     qWinMsgHandler(type, msg);
    #else
     //qt_message_output(type, msg);
    #endif
    if (type==QtFatalMsg) abort();
 }

MainWindow::MainWindow(ProgramOptions* s, QSplashScreen* splash):
    QMainWindow(NULL)
{

    dlgUserFitFunctionEditor=NULL;
    projectModeEnabled=true;
    nonprojectTitle=tr("non-project mode");
    projectFileFilter=tr("QuickFit Project (*.qfp *.qfpz *.qfp.gz);;QuickFit Project Autosave (*.qfp.autosave *.qfp.autosave.backup *.qfp.autosave.backup_old *.qfpz.autosave *.qfpz.autosave.backup *.qfpz.autosave.backup_old);;QuickFit Project backup (*.qfp.backup *.qfpz.backup *.qfp.backup_old *.qfpz.backup_old)");
    projectSaveFileFilter=tr("QuickFit Project (*.qfp);;Zipped QuickFit Project (*.qfpz)");
    settings=s;
    splashPix=splash->pixmap();
    project=NULL;
    helpWindow=NULL;
    lastUpdateRequest=NULL;
    lastUpdateRequestUser=NULL;
    connect(&networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(showUpdateInfo(QNetworkReply*)));

    QNetworkProxy proxy;
    ProgramOptionsSetQNetworkProxy(proxy);
    networkManager.setProxy(proxy);

    newProjectTimer.setInterval(2500);
    newProjectTimer.stop();
    connect(&newProjectTimer, SIGNAL(timeout()), this, SLOT(saveProjectFirstTime()));

    rawDataFactory=new QFRawDataRecordFactory(settings, QApplication::instance());
    evaluationFactory=new QFEvaluationItemFactory(settings, QApplication::instance());
    fitFunctionManager=new QFFitFunctionManager(settings, QApplication::instance());
    fitAlgorithmManager=new QFFitAlgorithmManager(settings, QApplication::instance());
    extensionManager=new QFExtensionManager(settings, QApplication::instance());
    importerManager=new QFImporterManager(settings, QApplication::instance());

    //settings=NULL;
    project=NULL;
    currentProjectDir="";
    currentProjectFilter="";

    timerAutosave=new QTimer(this);
    timerAutosave->setInterval(5*60*1000);


    createWidgets();
    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    //setUnifiedTitleAndToolBarOnMac(true);


    // search for plugin
    connect(rawDataFactory, SIGNAL(showMessage(const QString&)), splash, SLOT(showMessage(const QString&)));
    connect(evaluationFactory, SIGNAL(showMessage(const QString&)), splash, SLOT(showMessage(const QString&)));
    connect(fitAlgorithmManager, SIGNAL(showMessage(const QString&)), splash, SLOT(showMessage(const QString&)));
    connect(fitFunctionManager, SIGNAL(showMessage(const QString&)), splash, SLOT(showMessage(const QString&)));
    connect(extensionManager, SIGNAL(showMessage(const QString&)), splash, SLOT(showMessage(const QString&)));
    connect(importerManager, SIGNAL(showMessage(const QString&)), splash, SLOT(showMessage(const QString&)));

    connect(rawDataFactory, SIGNAL(showLongMessage(const QString&)), logFileMainWidget, SLOT(log_text_linebreak(QString)));
    connect(evaluationFactory, SIGNAL(showLongMessage(const QString&)), logFileMainWidget, SLOT(log_text_linebreak(QString)));
    connect(fitFunctionManager, SIGNAL(showLongMessage(const QString&)), logFileMainWidget, SLOT(log_text_linebreak(QString)));
    connect(fitAlgorithmManager, SIGNAL(showLongMessage(const QString&)), logFileMainWidget, SLOT(log_text_linebreak(QString)));
    connect(extensionManager, SIGNAL(showLongMessage(const QString&)), logFileMainWidget, SLOT(log_text_linebreak(QString)));
    connect(importerManager, SIGNAL(showLongMessage(const QString&)), logFileMainWidget, SLOT(log_text_linebreak(QString)));

    connect(timerAutosave, SIGNAL(timeout()), this, SLOT(autosaveProject()));


    logFileMainWidget->log_header(tr("searching for plugins ..."));
    logFileMainWidget->inc_indent();
    searchAndRegisterPlugins();
    logFileMainWidget->dec_indent();

    splash->showMessage(tr("%1 Plugins loaded successfully ... prepring online-help ... ").arg(rawDataFactory->getIDList().size()+evaluationFactory->getIDList().size()+fitFunctionManager->pluginCount()+fitAlgorithmManager->pluginCount()+extensionManager->getIDList().size()+importerManager->pluginCount()));

    logFileMainWidget->log_header(tr("preparing online-help ..."));
    logFileMainWidget->inc_indent();
    parseFAQ(settings->getMainHelpDirectory()+"/faq.html", "quickfit", helpdata.faqs);


    htmlReplaceList.append(qMakePair(QString("version.svnrevision"), QString(qfInfoSVNVersion()).trimmed()));
    htmlReplaceList.append(qMakePair(QString("version.status"), QString(qfInfoVersionStatus())));
    htmlReplaceList.append(qMakePair(QString("version.date"), QString(qfInfoCompileDate()).trimmed()));
    htmlReplaceList.append(qMakePair(QString("version"), QString(qfInfoVersionFull())));
    htmlReplaceList.append(qMakePair(QString("version_full"), QApplication::applicationVersion()));
    htmlReplaceList.append(qMakePair(QString("thanksto"), QString(qfInfoThanksTo())));
    htmlReplaceList.append(qMakePair(QString("copyright"), QString(qfInfoCopyright())));
    htmlReplaceList.append(qMakePair(QString("author"), QString(qfInfoAuthor())));
    htmlReplaceList.append(qMakePair(QString("email"), QString(qfInfoEmail())));
    htmlReplaceList.append(qMakePair(QString("jankrieger_phdthesis"), tr("Jan W. Krieger (2014): <b>\"Mapping diffusion Properties in Living Cells\"</b>, University Heidelberg, <i><a href=\"http://www.ub.uni-heidelberg.de/archiv/17187\">http://www.ub.uni-heidelberg.de/archiv/17187</a>.</i>")));
    htmlReplaceList.append(qMakePair(QString("jankrieger_phdthesis_ref"), tr("$$ref:JKRIEGR_PHD:Jan W. Krieger (2014): <b>\"Mapping diffusion Properties in Living Cells\"</b>, University Heidelberg, <i><a href=\"http://www.ub.uni-heidelberg.de/archiv/17187\">http://www.ub.uni-heidelberg.de/archiv/17187</a>.</i>$$")));
    htmlReplaceList.append(qMakePair(QString("jankrieger_phdthesis_invisibleref"), tr("$$invisibleref:JKRIEGR_PHD:Jan W. Krieger (2014): <b>\"Mapping diffusion Properties in Living Cells\"</b>, dissertational thesis, University Heidelberg, <i><a href=\"http://www.ub.uni-heidelberg.de/archiv/17187\">http://www.ub.uni-heidelberg.de/archiv/17187</a>.</i>$$")));
    htmlReplaceList.append(qMakePair(QString("qfcitation"), QString(qfInfoCitationHTML())));
    htmlReplaceList.append(qMakePair(QString("qfcitation_bibtex"), QString(qfInfoCitationBiBTeX())));
    htmlReplaceList.append(qMakePair(QString("maillist"), QString(qfInfoMaillist())));
    htmlReplaceList.append(qMakePair(QString("maillistrequest"), QString(qfInfoMaillistRequest())));
    htmlReplaceList.append(qMakePair(QString("weblink"), QString(qfInfoWeblink())));
    htmlReplaceList.append(qMakePair(QString("license"), qfInfoLicense()));
    htmlReplaceList.append(qMakePair(QString("plugin_list"), createPluginDoc(true)));
    htmlReplaceList.append(qMakePair(QString("pluginhelp_list"), createPluginDocHelp()));
    htmlReplaceList.append(qMakePair(QString("plugintutorials_list"), createPluginDocTutorials()));
    htmlReplaceList.append(qMakePair(QString("faq_list"), createFAQ()));
    htmlReplaceList.append(qMakePair(QString("pluginsettings_list"), createPluginDocSettings()));
    htmlReplaceList.append(qMakePair(QString("plugincopyright_list"), createPluginDocCopyrights()));
    htmlReplaceList.append(qMakePair(QString("maindir"), settings->getApplicationDirectory()));
    htmlReplaceList.append(qMakePair(QString("sourcedir"), settings->getSourceDirectory()));
    htmlReplaceList.append(qMakePair(QString("qf3dir"), settings->getApplicationDirectory()));
    htmlReplaceList.append(qMakePair(QString("applicationdir"), settings->getApplicationDirectory()));
    htmlReplaceList.append(qMakePair(QString("mainhelpdir"), settings->getMainHelpDirectory()));
    htmlReplaceList.append(qMakePair(QString("mainhelppicdir"), settings->getMainHelpDirectory()+"pic/"));
    htmlReplaceList.append(qMakePair(QString("assetsdir"), settings->getAssetsDirectory()));
    htmlReplaceList.append(qMakePair(QString("examplesdir"), settings->getExamplesDirectory()));
    htmlReplaceList.append(qMakePair(QString("configdir"), settings->getConfigFileDirectory()));
    htmlReplaceList.append(qMakePair(QString("tutorials_contents"), QString("<ul>")+createPluginDocTutorials("<li>%1 tutorial:<ul>", "</ul></li>")+QString("/<ul>")));
    htmlReplaceList.append(qMakePair(QString("settings_contents"), QString("<ul>")+createPluginDocSettings("<li>%1 settings:<ul>", "</ul></li>")+QString("/<ul>")));
    htmlReplaceList.append(qMakePair(QString("help_contents"), QString("<ul>")+createPluginDocHelp("<li>%1 help:<ul>", "</ul></li>")+QString("</ul>")));
    htmlReplaceList.append(qMakePair(QString("qf_commondoc_backtop"), tr("<div style=\"background-color: lightsteelblue;  border-color: midnightblue; border-style: solid; padding-top:5px; padding-left:5px; padding-right:5px; padding-bottom:5px; margin: 5px;\"> <a href=\"#top_page\"><img src=\":/lib/help/help_top.png\"></a></div>")));

    htmlReplaceList.append(qMakePair(QString("qf_commondoc_header.start"),
         tr("<table width=\"100%\" border=\"1\" cellspacing=\"0\" cellpadding=\"2\" style=\"background-color: lightsteelblue;  border-color: midnightblue;\" ><tr><td align=\"left\">"
            "<table width=\"100%\">"
            "<tr><td align=\"center\" colspan=\"3\">$$local_plugin_icon$$</td><td>&nbsp;&nbsp;&nbsp;&nbsp;</td><td width=\"90%\" align=\"left\"><b>$$local_plugin_name$$</b> $$local_plugin_id_decorated$$  </td></tr> "
            "<tr><td align=\"left\" rowspan=\"2\">$$rel_prev$$</td><td align=\"left\" rowspan=\"2\">$$rel_contents$$</td><td align=\"left\" rowspan=\"2\">$$rel_next$$</td><td>&nbsp;&nbsp;&nbsp;&nbsp;</td><td width=\"90%\" align=\"left\">$$qf_commondoc_header.default_links$$  ")));
    htmlReplaceList.append(qMakePair(QString("qf_commondoc_header.end_notitle"), QString("</td></tr></table></td></tr></table>")));// </div>")));


    htmlReplaceList.append(qMakePair(QString("qf_commondoc_header.end"), tr("$$qf_commondoc_header.end_notitle$$ <font size=\"+1\"><h1 id=\"TITLE\"><!-- title -->$$title$$</h1>$$local_plugin_autostartdescription$$")));
    htmlReplaceList.append(qMakePair(QString("qf_commondoc_header.separator"), QString(" | ")));

    htmlReplaceList.append(qMakePair(QString("qf_commondoc_header.default_links"), tr("<a href=\"%1quickfit.html\">QuickFit</a> $$local_plugin_typehelp_link$$ $$local_plugin_mainhelp_link$$ $$local_plugin_tutorial_link$$ $$local_plugin_faq_link$$").arg(settings->getMainHelpDirectory())));
    htmlReplaceList.append(qMakePair(QString("qf_commondoc_header.simplest"), tr("back: <a href=\"%1quickfit.html\">QuickFit Basics</a>").arg(settings->getMainHelpDirectory())));
    htmlReplaceList.append(qMakePair(QString("qf_commondoc_header.rdr"), tr("$$qf_commondoc_header.separator$$ <a href=\"$$qf_ui_rdr_helpfil$$\">$$qf_ui_rdr_helpfiletitle$$</a>")));
    htmlReplaceList.append(qMakePair(QString("qf_ui_rdr_helpfile"), tr("%1qf3_rdrscreen.html").arg(settings->getMainHelpDirectory())));
    htmlReplaceList.append(qMakePair(QString("qf_ui_rdr_helpfiletitle"), tr("Basic Raw Data Record Dialog Help")));
    htmlReplaceList.append(qMakePair(QString("qf_commondoc_header.eval"), tr("$$qf_commondoc_header.separator$$ <a href=\"%1qf3_evalscreen.html\">Basic Evaluation Dialog Help</a>").arg(settings->getMainHelpDirectory())));
    htmlReplaceList.append(qMakePair(QString("qf_ui_eval_helpfile"), tr("%1qf3_evalscreen.html").arg(settings->getMainHelpDirectory())));
    htmlReplaceList.append(qMakePair(QString("qf_ui_eval_helpfiletitle"), tr("Basic Evaluation Dialog Help")));
    htmlReplaceList.append(qMakePair(QString("qf_ui_jkqtplotter_helpfile"), tr("%1jkqtplotter.html").arg(settings->getMainHelpDirectory())));
    htmlReplaceList.append(qMakePair(QString("qf_faqfile"), tr("%1qf3_faq.html").arg(settings->getMainHelpDirectory())));
    htmlReplaceList.append(qMakePair(QString("qf_ui_jkqtplotter_helpfiletitle"), tr("Plotter/Graphing Component Help")));
    htmlReplaceList.append(qMakePair(QString("qf_ui_latex_helpfile"), tr("%1jkqtmathtext.html").arg(settings->getMainHelpDirectory())));
    htmlReplaceList.append(qMakePair(QString("qf_ui_latex_helpfiletitle"), tr("Latex Parser Help")));
    htmlReplaceList.append(qMakePair(QString("qf_mathparser_helpfile"), tr("%1mathparser.html").arg(settings->getMainHelpDirectory())));
    htmlReplaceList.append(qMakePair(QString("qf_mathparser_helpfiletitle"), tr("Expression Parser Help")));
    htmlReplaceList.append(qMakePair(QString("qf_commondoc_header.extension"), tr("")));
    htmlReplaceList.append(qMakePair(QString("qf_commondoc_header.fitfunc"), tr("$$qf_commondoc_header.separator$$ <a href=\"%1qf3_evalscreen.html\">Fit Functions Help</a>")));
    htmlReplaceList.append(qMakePair(QString("qf_commondoc_header.fitalg"), tr("$$qf_commondoc_header.separator$$ <a href=\"%1qf3_fitalg.html\">Fit Algorithms Help</a>")));
    htmlReplaceList.append(qMakePair(QString("qf_further_reading"), QString("")));

    htmlReplaceList.append(qMakePair(QString("qf_css_mainfile"), readFile(settings->getMainHelpDirectory()+"/qf3style.css")));
    htmlReplaceList.append(qMakePair(QString("qf_css_mainfile_block"), QString("<style>\n$$qf_css_mainfile$$\n</style>\n")));

    htmlReplaceList.append(qMakePair(QString("qf_commondoc_footer.start"),
         tr("$$DEFAULTREF$$</font>\n$$local_plugin_autodescription$$\n<a name=\"#footer\"><table width=\"100%\" border=\"1\" cellspacing=\"0\" cellpadding=\"2\" style=\"background-color: lightsteelblue;  border-color: midnightblue\" ><tr><td align=\"left\">"
            "<table width=\"100%\">"
            "<tr><td align=\"center\" ><a href=\"#top_page\" font-size: $$main_fontsize:-2$$;\"><img src=\":/lib/help/help_top.png\"></a>&nbsp;&nbsp;&nbsp;</td><td align=\"left\" style=\" font-size: $$main_fontsize:-2$$;\">$$local_plugin_icon$$&nbsp;&nbsp;&nbsp;</td><td align=\"right\" width=\"90%\" font-size: $$main_fontsize:-2$$;\">  <b>$$local_plugin_name$$</b> <i>$$local_plugin_copyright$$</i><br>$$local_plugin_weblink$$<br>")));
    htmlReplaceList.append(qMakePair(QString("qf_commondoc_footer.end"), QString("</td></tr></table></td></tr></table>")));// </div>")));


    parseGlobalreplaces(settings->getMainHelpDirectory());
    parseAutolinks(settings->getMainHelpDirectory(), helpdata.autolinks);
    parseTooltips(settings->getMainHelpDirectory(), helpdata.tooltips);

    mathParserRefDirs<<QString(settings->getMainHelpDirectory()+"/parserreference/");



    //qDebug()<<tooltips;
    helpWindow->setTooltips(helpdata.tooltips);






    logFileMainWidget->dec_indent();

    logFileMainWidget->log_text(tr("QuickFit started succesfully!\n"));


    newProject();

    readSettings();
    rawDataFactory->distribute(project, settings, this, this);
    evaluationFactory->distribute(project, settings, this, this);

    // scan program arguments for a project file (has to be the last argument)
    /*QStringList args=QApplication::arguments();
    if (args.size()>0) {
        QString lastarg=args[args.size()-1];
        if (QFile::exists(lastarg) && ((QFileInfo(lastarg).suffix()=="qfp")||(QFileInfo(lastarg).suffix()=="qfpz")||(QFileInfo(lastarg).suffix()=="qfp.gz"))) {
            currentProjectDir=QFileInfo(lastarg).dir().absolutePath();
            loadProject(lastarg);
        }
    }*/

    autoWriteSettings();
    timerAutosave->start();

    setWindowIcon(QIcon(":/icon_large.png"));

    if (settings->getConfigValue("quickfit/checkupdates", true).toBool() ) QTimer::singleShot(2000, this, SLOT(checkUpdatesAutomatic()));
    QTimer::singleShot(100, this, SLOT(checkCallArguments()));
    connect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(clipboardDataChanged()));
    clipboardDataChanged();
}


MainWindow::~MainWindow() {

    //std::cout<<"deleting MainWindow\n";
    if (project) delete project;
    project=NULL;
    //std::cout<<"deleting MainWindow ... OK\n";
}

void MainWindow::reloadCurrentProject()
{
    reloadProject();
}

void MainWindow::searchAndRegisterPlugins() {
    // find plugins
    rawDataFactory->searchPlugins(settings->getPluginDirectory(), helpdata);//&pluginHelpList, tooltips, faqs);
    evaluationFactory->searchPlugins(settings->getPluginDirectory(), helpdata);//&pluginHelpList, tooltips, faqs);
    fitFunctionManager->searchPlugins(settings->getPluginDirectory(), helpdata);//&pluginHelpList, tooltips, faqs);
    fitAlgorithmManager->searchPlugins(settings->getPluginDirectory(), helpdata);//&pluginHelpList, tooltips, faqs);
    importerManager->searchPlugins(settings->getPluginDirectory(), helpdata);//&pluginHelpList, tooltips, faqs);
    extensionManager->searchPlugins(settings->getPluginDirectory(), helpdata);//&pluginHelpList, tooltips, faqs);


    // distribute application hooks
    rawDataFactory->distribute(project, settings, this, this);
    evaluationFactory->distribute(project, settings, this, this);
    extensionManager->distribute(project);

    // init other plugins
    evaluationFactory->init();
    rawDataFactory->init();
    importerManager->init();
    fitFunctionManager->init();
    fitAlgorithmManager->init();

    // init extensions
    extensionManager->init(this, this);

    // register plugins to menus
    QStringList sl=getRawDataRecordFactory()->getIDList();
    for (int i=0; i<sl.size(); i++) {
        getRawDataRecordFactory()->registerMenu(sl[i], insertItemMenu);
    }

    sl=getEvaluationItemFactory()->getIDList();
    for (int i=0; i<sl.size(); i++) {
        getEvaluationItemFactory()->registerMenu(sl[i], evaluationMenu);
    }

}

void MainWindow::showLogMessage(const QString& message) {
    logFileProjectWidget->log_text(message);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    //qDebug()<<"main: closeEvent";
    if (maybeSave()) {
        writeSettings();
        closeProject();
        rawDataFactory->distribute(NULL, settings, this, this);
        rawDataFactory->deinit();
        evaluationFactory->distribute(NULL, settings, this, this);
        evaluationFactory->deinit();
        extensionManager->distribute(NULL);
        extensionManager->deinit();
        importerManager->deinit();
        fitFunctionManager->deinit();
        fitAlgorithmManager->deinit();

        newProjectTimer.stop();
        helpWindow->close();
        {
            QMapIterator<QString, QFHistogramView*> ii(histograms);
            while (ii.hasNext()) {
                ii.next();
                QString name=ii.key();
                histograms[name]->writeSettings(*(ProgramOptions::getInstance()->getQSettings()), "histograms/"+cleanStringForFilename(name)+"/");
                histograms[name]->close();
                histograms[name]->deleteLater();
            }
            histograms.clear();
        }
        {
            QMapIterator<QString, QFParameterCorrelationView*> ii(correlationViews);
            while (ii.hasNext()) {
                ii.next();
                QString name=ii.key();
                correlationViews[name]->writeSettings(*(ProgramOptions::getInstance()->getQSettings()), "paramcorr/"+cleanStringForFilename(name)+"/");
                correlationViews[name]->close();
                correlationViews[name]->deleteLater();
            }
            correlationViews.clear();
        }
        {
            QMapIterator<QString, QFSimplePlotView*> ii(plotViews);
            while (ii.hasNext()) {
                ii.next();
                QString name=ii.key();
                plotViews[name]->writeSettings(*(ProgramOptions::getInstance()->getQSettings()), "simpleplotview/"+cleanStringForFilename(name)+"/");
                plotViews[name]->close();
                plotViews[name]->deleteLater();
            }
            plotViews.clear();
        }
        ProgramOptions::setConfigValue("quickfit/lastrunsvn", qfInfoSVNVersion());
        event->accept();
        //qDebug()<<"main: exiting application";
        QApplication::exit();
    } else {
        //qDebug()<<"main: ingoring close event";
        event->ignore();
    }
}

void MainWindow::showEvent(QShowEvent *event) {
    QString SVN=qfInfoSVNVersion();
    QString lastSVN=ProgramOptions::getConfigValue("quickfit/lastrunsvn", "").toString();

    if (SVN!=lastSVN && !ProgramOptions::getConfigValue("quickfit/nosvncheck", false).toBool()) {
        QTimer::singleShot(200,this, SLOT(displayNewVersionDlg()));
    } else  {
        QTimer::singleShot(200,this, SLOT(displayWelcomeDlg()));
    }
}

void MainWindow::closeProject() {
    if (project) {
        logFileProjectWidget->clearLog();
        logFileProjectWidget->close_logfile();
        for (int i=0; i<rawDataPropEditors.size(); i++) {
            if (rawDataPropEditors[i]) {
                rawDataPropEditors[i]->writeSettings();
                rawDataPropEditors[i]->setCurrent(NULL);
                rawDataPropEditors[i]->close();
            }
        }
        for (int i=0; i<evaluationPropEditors.size(); i++) {
            if (evaluationPropEditors[i]) {
                evaluationPropEditors[i]->writeSettings();
                evaluationPropEditors[i]->setCurrent(NULL);
                evaluationPropEditors[i]->close();
            }
        }
        evaluationPropEditors.clear();
        rawDataPropEditors.clear();

        project->disconnect();
        tvMain->setModel(NULL);
        rawDataFactory->distribute(NULL, settings, this, this);
        evaluationFactory->distribute(NULL, settings, this, this);
        extensionManager->distribute(NULL);

        delete project;
				project=NULL;
    }
}

void MainWindow::newProject() {
    if (maybeSave()) {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        setCurrentProject("");
        closeProject();
        tvMain->setModel(NULL);

        project=new QFProject(getEvaluationItemFactory(), getRawDataRecordFactory(), this, this);
        logFileMainWidget->log_text(tr("created new project!\n"));
        tabLogs->setCurrentWidget(logFileProjectWidget);
        connect(project, SIGNAL(propertiesChanged()), this, SLOT(readProjectProperties()));
        readProjectProperties();
        connect(project, SIGNAL(wasChanged(bool)), this, SLOT(setWindowModified(bool)));
        connect(project->getTreeModel(), SIGNAL(modelReset()), this, SLOT(modelReset()));
        tvMain->setModel(project->getTreeModel());
        connect(project->getTreeModel(), SIGNAL(modelReset()), tvMain, SLOT(expandAll()));
        tvMain->expandAll();
        connect(project, SIGNAL(sortOrderChanged(int)), this, SLOT(setProjectSortOrderActions(int)));

        rawDataFactory->distribute(project, settings, this, this);
        evaluationFactory->distribute(project, settings, this, this);
        extensionManager->distribute(project);

        newProjectTimer.start();
        QApplication::restoreOverrideCursor();
    }
}

void MainWindow::openProject() {
    if (maybeSave()) {
        QString fileName = qfGetOpenFileName(this, tr("Open Project ..."), currentProjectDir, projectFileFilter);
        if (!fileName.isEmpty()) {
            currentProjectDir=QFileInfo(fileName).dir().absolutePath();
            QApplication::setOverrideCursor(Qt::WaitCursor);
            loadProject(fileName);
            QApplication::restoreOverrideCursor();
        }
    }
}

void MainWindow::openExample()
{
    if (maybeSave()) {
        QString fileName = qfGetOpenFileName(this, tr("Open Example Project ..."), settings->getExamplesDirectory(), projectFileFilter);
        if (!fileName.isEmpty()) {
            currentProjectDir=QFileInfo(fileName).dir().absolutePath();
            QApplication::setOverrideCursor(Qt::WaitCursor);
            loadProject(fileName);
            QApplication::restoreOverrideCursor();
        }
    }

}

void MainWindow::zipProject()
{
    if (maybeSave()) {
        QString fileName = qfGetSaveFileNameSet("MainWindow/zipProject/", this, tr("ZIP Project ..."), currentProjectDir, tr("ZIP-archives (*.zip)"));
        if (!fileName.isEmpty()) {
            QApplication::setOverrideCursor(Qt::WaitCursor);
            QFListProgressDialog* pdlg=new QFListProgressDialog(this);
            pdlg->show();
            project->exportProjectToZIP(fileName, pdlg);
            /*pdlg->close();
            delete pdlg;*/
            QApplication::restoreOverrideCursor();
            project->resetError();
        }
    }
}

void MainWindow::openProjectSubset()
{
    if (maybeSave()) {
        QString fileName = qfGetOpenFileName(this, tr("Open Project ..."), currentProjectDir, projectFileFilter);
        if (!fileName.isEmpty()) {
            QApplication::setOverrideCursor(Qt::WaitCursor);
            log_indent();
            log_text(tr("---------------------------------------------------------------------------\n"));
            log_text(tr("-- loading project '%1' in dummy mode!!! \n").arg(fileName));
            log_text(tr("---------------------------------------------------------------------------\n\n"));
            QFProject* dummy=new QFProject(getEvaluationItemFactory(), getRawDataRecordFactory(), this, this);
            dummy->readXMLDummy(fileName);
            bool load=false;
            QSet<int> rdrSel, evalSel;
            QApplication::restoreOverrideCursor();
            if (!dummy->error()) {
                DlgSelectProjectSubset* dlg=new DlgSelectProjectSubset(dummy, this);
                if (dlg->exec()) {
                    load=true;
                    rdrSel=dummy->getTreeModel()->getSelectedRDR();
                    evalSel=dummy->getTreeModel()->getSelectedEvaluations();
                    log_indent();
                    log_text(tr("- loading only %1 raw data records\n").arg(rdrSel.size()));
                    log_text(tr("- loading only %1 evaluation data records\n").arg(evalSel.size()));
                    log_text(tr("\n"));
                    log_unindent();
                }
                delete dlg;
            } else {
                QMessageBox::critical(this, tr("QuickFit %1").arg(qfInfoVersionFull()), dummy->errorDescription());
                logFileProjectWidget->log_error(dummy->errorDescription()+"\n");
                load=false;
            }
            delete dummy;
            log_unindent();
            //currentProjectDir=QFileInfo(fileName).dir().absolutePath();
            if (load) {
                QApplication::setOverrideCursor(Qt::WaitCursor);
                loadProject(fileName, true, rdrSel, evalSel);
                QApplication::restoreOverrideCursor();
            }
        }
    }
}

void MainWindow::reloadProject() {
    if (maybeSave()) {
        if (QFile::exists(curFile)) loadProject(curFile);
        else QMessageBox::warning(this, tr("QuickFit %1").arg(qfInfoVersionFull()),
                                  tr("No project loaded that could be reloaded!"),
                                  QMessageBox::Ok);
    }
}

void MainWindow::openRecentProject() {
    if (maybeSave()) {
        QAction *action = qobject_cast<QAction *>(sender());
        if (action) {
            QApplication::setOverrideCursor(Qt::WaitCursor);
            currentProjectDir=QFileInfo(action->data().toString()).dir().absolutePath();
            loadProject(action->data().toString());
            QApplication::restoreOverrideCursor();
        }
    }
}

void MainWindow::openRecentProject(const QString& filename) {
    if (maybeSave()) {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        currentProjectDir=QFileInfo(filename).dir().absolutePath();
        loadProject(filename);
        QApplication::restoreOverrideCursor();
    }
}

bool MainWindow::saveProject() {
    if (curFile.isEmpty()) {
        return saveProjectAs();
    } else {
        return saveProject(curFile);
    }
}

bool MainWindow::saveProjectAs() {
    QString fileName = qfGetSaveFileName(this, tr("Save Project As ..."), currentProjectDir, projectSaveFileFilter, &currentProjectFilter);
    if (fileName.isEmpty())
        return false;
    currentProjectDir=QFileInfo(fileName).dir().absolutePath();
    QApplication::setOverrideCursor(Qt::WaitCursor);

    bool res=saveProject(fileName);
    QApplication::restoreOverrideCursor();
    return res;
}


void MainWindow::about() {
    QDialog *widget = new QDialog(this);
    Ui::About ui;
    ui.setupUi(widget);
    QTextBrowser* ui_textEdit = qFindChild<QTextBrowser*>(widget, "edtInfo");
    QTextBrowser* ui_releasenotes = qFindChild<QTextBrowser*>(widget, "edtReleaseNotes");
    QTextBrowser* ui_license = qFindChild<QTextBrowser*>(widget, "edtLicense");
    QTextBrowser* ui_citing = qFindChild<QTextBrowser*>(widget, "edtCiting");
    QLabel* ui_label = qFindChild<QLabel*>(widget, "labSplash");
    QLabel* ui_labelLic = qFindChild<QLabel*>(widget, "labLicense");
    ui_label->setPixmap(splashPix);
    ui_textEdit->setText(tr("<b>Copyright:</b><blockquote>%3</blockquote>"
                            "<b>many thanks to:</b><blockquote>%2</blockquote>"
                            "<b>contact the authors:</b><blockquote><a href=\"mailto:%6\">%6</a></blockquote>"
                            "<b>mailing list:</b><blockquote><a href=\"mailto:%8\">%8</a> (<a href=\"%9\">subscribe</a>)</blockquote>"
                            "<b>QuickFit version:</b><blockquote>version %10 (%11) %14-bit, SVN: %12, compiledate: %13</blockquote>"
                            "<b>QuickFit bit-depth:</b><blockquote>%14-bit</blockquote>"
                            "<b>libraries, used by QuickFit:</b><ul><li>QuickFit library v%4.%5</li><li>Qt %1 (<a href=\"http://qt.nokia.com/\">http://qt.nokia.com/</a>)</li></ul>"
                            "<b>compiler used for this version:</b><blockquote>%7</blockquote>"
                            "<b>operating system:</b><blockquote>%15</blockquote><br><br>").arg(QT_VERSION_STR).arg(qfInfoThanksTo()).arg(qfInfoCopyright()).arg(QF3LIB_APIVERSION_MAJOR).arg(QF3LIB_APIVERSION_MINOR).arg(qfInfoEmail()).arg(Qt::escape(qfInfoCompiler())).arg(qfInfoMaillist()).arg(qfInfoMaillistRequest()).arg(qfInfoVersion()).arg(qfInfoVersionStatus()).arg(qfInfoSVNVersion()).arg(qfInfoCompileDate()).arg(getApplicationBitDepth()).arg(getOSName()));
    ui_labelLic->setText(qfInfoLicense());
    ui_citing->setText(tr("If you used QuickFit for your data evaluation, please cite it in your publication. You can use a citation like this:"
                          "<blockquote>%1</blockquote>"
                          "<br><br>"
                          "... or the BiBTeX record provided here:"
                          "<blockquote><code><pre>%2</pre></code></blockquote><br><br>").arg(qfInfoCitationHTML()).arg(qfInfoCitationBiBTeX()));
    QFile f(":/quickfit3/releasenotes.html");
    if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QString text=f.readAll();
        text=text.replace("$$SVN$$", qfInfoSVNVersion());
        text=text.replace("$$COMPILEDATE$$", qfInfoCompileDate());
        ui_releasenotes->setText(text);
    } else {
        ui_releasenotes->setPlainText(tr("none available :-((("));
    }
    QFile f2(":/quickfit3/gpl3-0.html");
    if (f2.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QByteArray ba=f2.readAll();
        ui_license->setText(QString::fromUtf8(ba.data(), ba.size()));
    } else {
        ui_license->setVisible(false);
    }
    widget->exec();
    delete widget;
}

QString MainWindow::createPluginDoc(bool docLinks) {
    QString text=tr("<h2>Raw Data Record Plugins:</h2><center><table border=\"0\" bgcolor=\"darkgray\" width=\"90%\">");
    // gather information about plugins
    for (int i=0; i<getRawDataRecordFactory()->getIDList().size(); i++) {
        QString id=getRawDataRecordFactory()->getIDList().at(i);
        text+=createPluginDocItem(docLinks, id, getRawDataRecordFactory()->getName(id), getRawDataRecordFactory()->getDescription(id), getRawDataRecordFactory()->getIconFilename(id), getRawDataRecordFactory()->getAuthor(id), getRawDataRecordFactory()->getCopyright(id), getRawDataRecordFactory()->getWeblink(id), getRawDataRecordFactory()->getPluginFilename(id), getRawDataRecordFactory()->getMajorVersion(id), getRawDataRecordFactory()->getMinorVersion(id), QStringList());
    }
    text+="</table></center>";
    text+=tr("<br><br><h2>Data Evaluation Plugins:</h2><center><table border=\"0\" bgcolor=\"darkgray\" width=\"90%\">");
    // gather information about plugins
    for (int i=0; i<getEvaluationItemFactory()->getIDList().size(); i++) {
        QString id=getEvaluationItemFactory()->getIDList().at(i);
        text+=createPluginDocItem(docLinks, id, getEvaluationItemFactory()->getName(id), getEvaluationItemFactory()->getDescription(id), getEvaluationItemFactory()->getIconFilename(id), getEvaluationItemFactory()->getAuthor(id), getEvaluationItemFactory()->getCopyright(id), getEvaluationItemFactory()->getWeblink(id), getEvaluationItemFactory()->getPluginFilename(id), getEvaluationItemFactory()->getMajorVersion(id), getEvaluationItemFactory()->getMinorVersion(id), QStringList());
    }
    text+="</table></center>";

    text+=tr("<br><br><h2><a href=\"$$mainhelpdir$$qf3_fitalg.html\">Fit Algorithm Plugins</a>:</h2><center><table border=\"0\" bgcolor=\"darkgray\" width=\"90%\">");
    // gather information about plugins
    for (int i=0; i<fitAlgorithmManager->pluginCount(); i++) {
        int id=i;
        QStringList additional;
        additional<<tr("implemented ids:")<<fitAlgorithmManager->getIDList(id).join(", ");
        text+=createPluginDocItem(docLinks, fitAlgorithmManager->getID(id), fitAlgorithmManager->getName(id), fitAlgorithmManager->getDescription(id), fitAlgorithmManager->getIconFilename(id), fitAlgorithmManager->getAuthor(id), fitAlgorithmManager->getCopyright(id), fitAlgorithmManager->getWeblink(id), fitAlgorithmManager->getPluginFilename(id), fitAlgorithmManager->getMajorVersion(id), fitAlgorithmManager->getMinorVersion(id), additional);
    }
    text+="</table></center>";
    text+=tr("<br><br><h2><a href=\"$$mainhelpdir$$qf3_fitfunc.html\">Fit Function Plugins</a>:</h2><center><table border=\"0\" bgcolor=\"darkgray\" width=\"90%\">");
    // gather information about plugins
    for (int i=0; i<fitFunctionManager->pluginCount(); i++) {
        int id=i;
        QStringList additional;
        additional<<tr("implemented ids:")<<fitFunctionManager->getIDList(id).join(", ");
        text+=createPluginDocItem(docLinks, fitFunctionManager->getID(id), fitFunctionManager->getName(id), fitFunctionManager->getDescription(id), fitFunctionManager->getIconFilename(id), fitFunctionManager->getAuthor(id), fitFunctionManager->getCopyright(id), fitFunctionManager->getWeblink(id), fitFunctionManager->getPluginFilename(id), fitFunctionManager->getMajorVersion(id), fitFunctionManager->getMinorVersion(id), additional);
    }
    text+="</table></center>";

    text+=tr("<br><br><h2><a href=\"$$mainhelpdir$$qf3_fitfunc.html\">Importer Plugins</a>:</h2><center><table border=\"0\" bgcolor=\"darkgray\" width=\"90%\">");
    // gather information about plugins
    for (int i=0; i<importerManager->pluginCount(); i++) {
        int id=i;
        QStringList additional;
        additional<<tr("implemented ids:")<<importerManager->getIDList(id).join(", ");
        text+=createPluginDocItem(docLinks, importerManager->getID(id), importerManager->getName(id), importerManager->getDescription(id), importerManager->getIconFilename(id), importerManager->getAuthor(id), importerManager->getCopyright(id), importerManager->getWeblink(id), importerManager->getPluginFilename(id), importerManager->getMajorVersion(id), importerManager->getMinorVersion(id), additional);
    }
    text+="</table></center>";

    text+=tr("<br><br><h2><a href=\"$$mainhelpdir$$qf3_extension.html\">Extension Plugins</a>:</h2><center><table border=\"0\" bgcolor=\"darkgray\" width=\"90%\">");
    // gather information about plugins
    for (int i=0; i<getExtensionManager()->getIDList().size(); i++) {
        QString id=getExtensionManager()->getIDList().at(i);
        text+=createPluginDocItem(docLinks, id, getExtensionManager()->getName(id), getExtensionManager()->getDescription(id), getExtensionManager()->getIconFilename(id), getExtensionManager()->getAuthor(id), getExtensionManager()->getCopyright(id), getExtensionManager()->getWeblink(id), getExtensionManager()->getPluginFilename(id), getExtensionManager()->getMajorVersion(id), getExtensionManager()->getMinorVersion(id), QStringList());
    }
    text+="</table></center>";

    return text;
}

QString MainWindow::createFAQ()
{
    QString text="";
    QString textd="";
    int cnt=0;
    QMapIterator<QString, QFFAQData> it(helpdata.faqs);
    while (it.hasNext()) {
        it.next();
        QString faq="";
        QString faqdir="";
        QString faql="";
        for (int i=0; i<it.value().size(); i++) {
            QString q=it.value().at(i).question;
            //QString a=it.value().at(i).answer;
            QString l=it.value().at(i).link;
            //qDebug()<<q<<l;
            if (!q.isEmpty() && !l.isEmpty()) {
                faql+=tr("<li><a href=\"%2\"><i>%1</i></a></li>").arg(q).arg(l);
                //faqdir+=tr("<li><a href=\"#faq%2\"><i>%1</i></a></li>").arg(q).arg(cnt);
                //faq+=tr("<li><a name=\"faq%3\"><i>%1</i><clockquote>%2</blockquote></li>").arg(q).arg(a).arg(cnt);
                cnt++;
            }
        }
        if (it.key()=="quickfit") {
            if (!faql.isEmpty()) text+=tr("<li><b>QuickFit $$version$$:</b><ul>%1</ul></li>").arg(faql);
            //textd+=tr("<li><b>%1:</b><ul>%2</ul></li>").arg(getPluginName(it.key())).arg(faqdir);
        } else {
            if (!faql.isEmpty()) text+=tr("<li><b>%1:</b><ul>%2</ul></li>").arg(getPluginName(it.key())).arg(faql);
            //textd+=tr("<li><b>%1:</b><ul>%2</ul></li>").arg(getPluginName(it.key())).arg(faqdir);
        }
    }
    //return textd+tr("\n\n<hr>\n\n")+text;
    return text;
}

QString MainWindow::createPluginDocItem(bool docLink, QString id, QString name, QString description, QString iconfilename, QString author, QString copyright, QString weblink, QString file, int verMajor, int verMinor, QStringList additional) {
    QString text="";
    text+=QString("<tr><td>&nbsp;</td><td></td></tr><tr><td colspan=\"2\" bgcolor=\"silver\"><img src=\"%2\">&nbsp;<b>%1</b></td></tr>").arg(name).arg(iconfilename);
    text+=QString("<tr><td><i>%1</i></td><td bgcolor=\"silver\">%2.%3</td></tr>").arg(tr("version:")).arg(verMajor).arg(verMinor);
    text+=QString("<tr><td><i>%1</i></td><td bgcolor=\"silver\">%2</td></tr>").arg(tr("description:")).arg(description);
    text+=QString("<tr><td><i>%1</i></td><td bgcolor=\"silver\">%2</td></tr>").arg(tr("author:")).arg(author);
    text+=QString("<tr><td><i>%1</i></td><td bgcolor=\"silver\">%2</td></tr>").arg(tr("copyright:")).arg(copyright);
    text+=QString("<tr><td><i>%1</i></td><td bgcolor=\"silver\">%2</td></tr>").arg(tr("weblink:")).arg(weblink);
    text+=QString("<tr><td><i>%1</i></td><td bgcolor=\"silver\">%2</td></tr>").arg(tr("file:")).arg(file);
    text+=QString("<tr><td><i>%1</i></td><td bgcolor=\"silver\">%2</td></tr>").arg(tr("id:")).arg(id);
    for (int i=0; i<additional.size(); i+=2) {
        if (i<=additional.size()-2) {
            text+=QString("<tr><td><i>%1</i></td><td bgcolor=\"silver\">%2</td></tr>").arg(additional[i]).arg(additional[i+1]);
        }
    }
    return text;
}

QString MainWindow::createPluginDocCopyrights(QString mainitem_before, QString mainitem_after) {
    QString item_template=QString("<li><a href=\"%3\"><img width=\"16\" height=\"16\" src=\"%1\"></a>&nbsp;<a href=\"%3\">%2</a></li>");
    QString text=mainitem_before.arg(tr("Raw Data Record"));
    // gather information about plugins
    for (int i=0; i<getRawDataRecordFactory()->getIDList().size(); i++) {
        QString id=getRawDataRecordFactory()->getIDList().at(i);
        QString dir=getRawDataRecordFactory()->getPluginCopyrightFile(id);
        if (QFile::exists(dir)) text+=item_template.arg(getRawDataRecordFactory()->getIconFilename(id)).arg(getRawDataRecordFactory()->getName(id)).arg(dir);
    }
    text+=mainitem_after;

    text+=mainitem_before.arg(tr("Data Evaluation"));
    // gather information about plugins
    for (int i=0; i<getEvaluationItemFactory()->getIDList().size(); i++) {
        QString id=getEvaluationItemFactory()->getIDList().at(i);
        QString dir=getEvaluationItemFactory()->getPluginCopyrightFile(id);
        if (QFile::exists(dir)) text+=item_template.arg(getEvaluationItemFactory()->getIconFilename(id)).arg(getEvaluationItemFactory()->getName(id)).arg(dir);
    }
    text+=mainitem_after;

    text+=mainitem_before.arg(tr("<a href=\"$$mainhelpdir$$qf3_fitalg.html\">Fit Algorithm</a>"));
    // gather information about plugins
    for (int i=0; i<fitAlgorithmManager->pluginCount(); i++) {
        int id=i;
        QString dir=fitAlgorithmManager->getPluginCopyrightFile(id);
        if (QFile::exists(dir)) text+=item_template.arg(fitAlgorithmManager->getIconFilename(id)).arg(fitAlgorithmManager->getName(id)).arg(dir);
    }
    text+=mainitem_after;
    text+=mainitem_before.arg(tr("<a href=\"$$mainhelpdir$$qf3_fitfunc.html\">Fit Function</a>"));
    // gather information about plugins
    for (int i=0; i<fitFunctionManager->pluginCount(); i++) {
        int id=i;
        QString dir=fitFunctionManager->getPluginCopyrightFile(id);
        if (QFile::exists(dir)) text+=item_template.arg(fitFunctionManager->getIconFilename(id)).arg(fitFunctionManager->getName(id)).arg(dir);
    }
    text+=mainitem_after;

    text+=mainitem_before.arg(tr("<a href=\"$$mainhelpdir$$qf3_importer.html\">Importers</a>"));
    // gather information about plugins
    for (int i=0; i<importerManager->pluginCount(); i++) {
        int id=i;
        QString dir=importerManager->getPluginCopyrightFile(id);
        if (QFile::exists(dir)) text+=item_template.arg(importerManager->getIconFilename(id)).arg(importerManager->getName(id)).arg(dir);
    }
    text+=mainitem_after;

    text+=mainitem_before.arg(tr("<a href=\"$$mainhelpdir$$qf3_extension.html\">Extension</a>"));
    // gather information about plugins
    for (int i=0; i<getExtensionManager()->getIDList().size(); i++) {
        QString id=getExtensionManager()->getIDList().at(i);
        QString dir=getExtensionManager()->getPluginCopyrightFile(id);
        if (QFile::exists(dir)) text+=item_template.arg(getExtensionManager()->getIconFilename(id)).arg(getExtensionManager()->getName(id)).arg(dir);
    }
    text+=mainitem_after;

    return text;
}



QString MainWindow::createPluginDocTutorials(QString mainitem_before, QString mainitem_after) {
    QString subitem_template=QString("<li><a href=\"%2\">%1</a></li>");
    QString sub_template=QString("<ol>%1</ol>");
    QString mainitem_template=QString("<li><a href=\"%3\"><img width=\"16\" height=\"16\" src=\"%1\"></a>&nbsp;<a href=\"%3\">%2</a>%4</li>");
    QString text=mainitem_before.arg(tr("Raw Data Record"));
    // gather information about plugins
    for (int i=0; i<getRawDataRecordFactory()->getIDList().size(); i++) {
        QString id=getRawDataRecordFactory()->getIDList().at(i);
        QString dir=getRawDataRecordFactory()->getPluginTutorialMain(id);
        QStringList names, links;
        getRawDataRecordFactory()->getPluginTutorials(id, names, links);
        int subCnt=qMax(names.size(), links.size());
        QString subTxt="";
        if (subCnt>0) {
            for (int i=0; i<subCnt; i++) {
                if (!links.value(i, "").isEmpty()) subTxt+=subitem_template.arg(names.value(i, tr("Tutorial for %1").arg(getRawDataRecordFactory()->getName(id)))).arg(links.value(i, ""));
            }
            if (!subTxt.isEmpty()) subTxt=sub_template.arg(subTxt); // embed in <ol>...</ol>
        }
        if (QFile::exists(dir)) text+=mainitem_template.arg(getRawDataRecordFactory()->getIconFilename(id)).arg(getRawDataRecordFactory()->getName(id)).arg(dir).arg(subTxt);
    }
    text+=mainitem_after;

    text+=mainitem_before.arg(tr("Data Evaluation"));
    // gather information about plugins
    for (int i=0; i<getEvaluationItemFactory()->getIDList().size(); i++) {
        QString id=getEvaluationItemFactory()->getIDList().at(i);
        QString dir=getEvaluationItemFactory()->getPluginTutorialMain(id);
        QStringList names, links;
        getEvaluationItemFactory()->getPluginTutorials(id, names, links);
        int subCnt=qMax(names.size(), links.size());
        QString subTxt="";
        if (subCnt>0) {
            for (int i=0; i<subCnt; i++) {
                if (!links.value(i, "").isEmpty()) subTxt+=subitem_template.arg(names.value(i, tr("Tutorial for %1").arg(getEvaluationItemFactory()->getName(id)))).arg(links.value(i, ""));
            }
            if (!subTxt.isEmpty()) subTxt=sub_template.arg(subTxt); // embed in <ol>...</ol>
        }
        if (QFile::exists(dir)) text+=mainitem_template.arg(getEvaluationItemFactory()->getIconFilename(id)).arg(getEvaluationItemFactory()->getName(id)).arg(dir).arg(subTxt);
    }
    text+=mainitem_after;

    text+=mainitem_before.arg(tr("<a href=\"$$mainhelpdir$$qf3_fitalg.html\">Fit Algorithm</a>"));
    // gather information about plugins
    for (int i=0; i<fitAlgorithmManager->pluginCount(); i++) {
        int id=i;
        QString dir=fitAlgorithmManager->getPluginTutorialMain(id);
        QStringList names, links;
        fitAlgorithmManager->getPluginTutorials(id, names, links);
        int subCnt=qMax(names.size(), links.size());
        QString subTxt="";
        if (subCnt>0) {
            for (int i=0; i<subCnt; i++) {
                if (!links.value(i, "").isEmpty()) subTxt+=subitem_template.arg(names.value(i, tr("Tutorial for %1").arg(fitAlgorithmManager->getName(id)))).arg(links.value(i, ""));
            }
            if (!subTxt.isEmpty()) subTxt=sub_template.arg(subTxt); // embed in <ol>...</ol>
        }
        if (QFile::exists(dir)) text+=mainitem_template.arg(fitAlgorithmManager->getIconFilename(id)).arg(fitAlgorithmManager->getName(id)).arg(dir).arg(subTxt);
    }
    text+=mainitem_after;
    text+=mainitem_before.arg(tr("<a href=\"$$mainhelpdir$$qf3_fitfunc.html\">Fit Function</a>"));
    // gather information about plugins
    for (int i=0; i<fitFunctionManager->pluginCount(); i++) {
        int id=i;
        QString dir=fitFunctionManager->getPluginTutorialMain(id);
        QStringList names, links;
        fitFunctionManager->getPluginTutorials(id, names, links);
        int subCnt=qMax(names.size(), links.size());
        QString subTxt="";
        if (subCnt>0) {
            for (int i=0; i<subCnt; i++) {
                if (!links.value(i, "").isEmpty()) subTxt+=subitem_template.arg(names.value(i, tr("Tutorial for %1").arg(fitFunctionManager->getName(id)))).arg(links.value(i, ""));
            }
            if (!subTxt.isEmpty()) subTxt=sub_template.arg(subTxt); // embed in <ol>...</ol>
        }
        if (QFile::exists(dir)) text+=mainitem_template.arg(fitFunctionManager->getIconFilename(id)).arg(fitFunctionManager->getName(id)).arg(dir).arg(subTxt);
    }
    text+=mainitem_after;

    text+=mainitem_before.arg(tr("<a href=\"$$mainhelpdir$$qf3_importer.html\">Importers</a>"));
    // gather information about plugins
    for (int i=0; i<importerManager->pluginCount(); i++) {
        int id=i;
        QString dir=importerManager->getPluginTutorialMain(id);
        QStringList names, links;
        importerManager->getPluginTutorials(id, names, links);
        int subCnt=qMax(names.size(), links.size());
        QString subTxt="";
        if (subCnt>0) {
            for (int i=0; i<subCnt; i++) {
                if (!links.value(i, "").isEmpty()) subTxt+=subitem_template.arg(names.value(i, tr("Tutorial for %1").arg(importerManager->getName(id)))).arg(links.value(i, ""));
            }
            if (!subTxt.isEmpty()) subTxt=sub_template.arg(subTxt); // embed in <ol>...</ol>
        }
        if (QFile::exists(dir)) text+=mainitem_template.arg(importerManager->getIconFilename(id)).arg(importerManager->getName(id)).arg(dir).arg(subTxt);
    }
    text+=mainitem_after;

    text+=mainitem_before.arg(tr("<a href=\"$$mainhelpdir$$qf3_extension.html\">Extension</a>"));
    // gather information about plugins
    for (int i=0; i<getExtensionManager()->getIDList().size(); i++) {
        QString id=getExtensionManager()->getIDList().at(i);
        QString dir=getExtensionManager()->getPluginTutorialMain(id);
        QStringList names, links;
        getExtensionManager()->getPluginTutorials(id, names, links);
        int subCnt=qMax(names.size(), links.size());
        QString subTxt="";
        if (subCnt>0) {
            for (int i=0; i<subCnt; i++) {
                if (!links.value(i, "").isEmpty()) subTxt+=subitem_template.arg(names.value(i, tr("Tutorial for %1").arg(getExtensionManager()->getName(id)))).arg(links.value(i, ""));
            }
            if (!subTxt.isEmpty()) subTxt=sub_template.arg(subTxt); // embed in <ol>...</ol>
        }
        if (QFile::exists(dir)) text+=mainitem_template.arg(getExtensionManager()->getIconFilename(id)).arg(getExtensionManager()->getName(id)).arg(dir).arg(subTxt);
    }
    text+=mainitem_after;

    return text;
}

QList<QAction *> MainWindow::getPluginTutorials()
{
    QList<QAction *> res;
    for (int i=0; i<getRawDataRecordFactory()->getIDList().size(); i++) {
        QString id=getRawDataRecordFactory()->getIDList().at(i);
        QString dir=getRawDataRecordFactory()->getPluginTutorialMain(id);
        QStringList names, links;
        getRawDataRecordFactory()->getPluginTutorials(id, names, links);
        int subCnt=qMax(names.size(), links.size());
        if (subCnt>0) {
            if (QFile::exists(dir)) {
                QFHelpAction* act=new QFHelpAction(this);
                act->setHelp(dir);
                act->setText(tr("Tutorial(s) for %1").arg(getRawDataRecordFactory()->getName(id)));
                act->setIcon(QIcon(getRawDataRecordFactory()->getIconFilename(id)));
                res.append(act);


                for (int i=0; i<subCnt; i++) {
                    if (!links.value(i, "").isEmpty()) {
                        act=new QFHelpAction(this);
                        act->setHelp(links.value(i, ""));
                        act->setText(QString("    ")+names.value(i, tr("Tutorial for %1").arg(getRawDataRecordFactory()->getName(id))));
                        res.append(act);
                    }
                }
            }
        }
    }


    for (int i=0; i<getEvaluationItemFactory()->getIDList().size(); i++) {
        QString id=getEvaluationItemFactory()->getIDList().at(i);
        QString dir=getEvaluationItemFactory()->getPluginTutorialMain(id);
        QStringList names, links;
        getEvaluationItemFactory()->getPluginTutorials(id, names, links);
        int subCnt=qMax(names.size(), links.size());
        if (subCnt>0) {
            if (QFile::exists(dir)) {
                QFHelpAction* act=new QFHelpAction(this);
                act->setHelp(dir);
                act->setText(tr("Tutorial(s) for %1").arg(getEvaluationItemFactory()->getName(id)));
                act->setIcon(QIcon(getEvaluationItemFactory()->getIconFilename(id)));
                res.append(act);


                for (int i=0; i<subCnt; i++) {
                    if (!links.value(i, "").isEmpty()) {
                        act=new QFHelpAction(this);
                        act->setHelp(links.value(i, ""));
                        act->setText(QString("    ")+names.value(i, tr("Tutorial for %1").arg(getEvaluationItemFactory()->getName(id))));
                        //act->setIcon(getEvaluationItemFactory()->getIconFilename(id));
                        res.append(act);
                    }
                }
            }
        }
    }


    for (int i=0; i<getExtensionManager()->getIDList().size(); i++) {
        QString id=getExtensionManager()->getIDList().at(i);
        QString dir=getExtensionManager()->getPluginTutorialMain(id);
        QStringList names, links;
        getExtensionManager()->getPluginTutorials(id, names, links);
        int subCnt=qMax(names.size(), links.size());
        if (subCnt>0) {
            if (QFile::exists(dir)) {
                QFHelpAction* act=new QFHelpAction(this);
                act->setHelp(dir);
                act->setText(tr("Tutorial(s) for %1").arg(getExtensionManager()->getName(id)));
                act->setIcon(QIcon(getExtensionManager()->getIconFilename(id)));
                res.append(act);


                for (int i=0; i<subCnt; i++) {
                    if (!links.value(i, "").isEmpty()) {
                        act=new QFHelpAction(this);
                        act->setHelp(links.value(i, ""));
                        act->setText(QString("    ")+names.value(i, tr("Tutorial for %1").arg(getExtensionManager()->getName(id))));
                        //act->setIcon(getExtensionManager()->getIconFilename(id));
                        res.append(act);
                    }
                }
            }
        }
    }






    for (int id=0; id<fitAlgorithmManager->pluginCount(); id++) {
        QString dir=fitAlgorithmManager->getPluginTutorialMain(id);
        QStringList names, links;
        fitAlgorithmManager->getPluginTutorials(id, names, links);
        int subCnt=qMax(names.size(), links.size());
        if (subCnt>0) {
            if (QFile::exists(dir)) {
                QFHelpAction* act=new QFHelpAction(this);
                act->setHelp(dir);
                act->setText(tr("Tutorial(s) for %1").arg(fitAlgorithmManager->getName(id)));
                act->setIcon(QIcon(fitAlgorithmManager->getIconFilename(id)));
                res.append(act);


                for (int i=0; i<subCnt; i++) {
                    if (!links.value(i, "").isEmpty()) {
                        act=new QFHelpAction(this);
                        act->setHelp(links.value(i, ""));
                        act->setText(QString("    ")+names.value(i, tr("Tutorial for %1").arg(fitAlgorithmManager->getName(id))));
                        res.append(act);
                    }
                }
            }
        }
    }


    for (int id=0; id<fitFunctionManager->pluginCount(); id++) {
        QString dir=fitFunctionManager->getPluginTutorialMain(id);
        QStringList names, links;
        fitFunctionManager->getPluginTutorials(id, names, links);
        int subCnt=qMax(names.size(), links.size());
        if (subCnt>0) {
            if (QFile::exists(dir)) {
                QFHelpAction* act=new QFHelpAction(this);
                act->setHelp(dir);
                act->setText(tr("Tutorial(s) for %1").arg(fitFunctionManager->getName(id)));
                act->setIcon(QIcon(fitFunctionManager->getIconFilename(id)));
                res.append(act);


                for (int i=0; i<subCnt; i++) {
                    if (!links.value(i, "").isEmpty()) {
                        act=new QFHelpAction(this);
                        act->setHelp(links.value(i, ""));
                        act->setText(QString("    ")+names.value(i, tr("Tutorial for %1").arg(fitFunctionManager->getName(id))));
                        res.append(act);
                    }
                }
            }
        }
    }


    for (int id=0; id<importerManager->pluginCount(); id++) {
        QString dir=importerManager->getPluginTutorialMain(id);
        QStringList names, links;
        importerManager->getPluginTutorials(id, names, links);
        int subCnt=qMax(names.size(), links.size());
        if (subCnt>0) {
            if (QFile::exists(dir)) {
                QFHelpAction* act=new QFHelpAction(this);
                act->setHelp(dir);
                act->setText(tr("Tutorial(s) for %1").arg(importerManager->getName(id)));
                act->setIcon(QIcon(importerManager->getIconFilename(id)));
                res.append(act);


                for (int i=0; i<subCnt; i++) {
                    if (!links.value(i, "").isEmpty()) {
                        act=new QFHelpAction(this);
                        act->setHelp(links.value(i, ""));
                        act->setText(QString("    ")+names.value(i, tr("Tutorial for %1").arg(importerManager->getName(id))));
                        res.append(act);
                    }
                }
            }
        }
    }


    return res;
}

QString MainWindow::createPluginDocSettings(QString mainitem_before, QString mainitem_after) {
    QString item_template=QString("<li><a href=\"%3\"><img width=\"16\" height=\"16\" src=\"%1\"></a>&nbsp;<a href=\"%3\">%2</a></li>");
    QString text=mainitem_before.arg(tr("Raw Data Record"));
    // gather information about plugins
    for (int i=0; i<getRawDataRecordFactory()->getIDList().size(); i++) {
        QString id=getRawDataRecordFactory()->getIDList().at(i);
        QString dir=getRawDataRecordFactory()->getPluginSettings(id);
        if (QFile::exists(dir)) text+=item_template.arg(getRawDataRecordFactory()->getIconFilename(id)).arg(getRawDataRecordFactory()->getName(id)).arg(dir);
    }
    text+=mainitem_after;

    text+=mainitem_before.arg(tr("Data Evaluation"));
    // gather information about plugins
    for (int i=0; i<getEvaluationItemFactory()->getIDList().size(); i++) {
        QString id=getEvaluationItemFactory()->getIDList().at(i);
        QString dir=getEvaluationItemFactory()->getPluginSettings(id);
        if (QFile::exists(dir)) text+=item_template.arg(getEvaluationItemFactory()->getIconFilename(id)).arg(getEvaluationItemFactory()->getName(id)).arg(dir);
    }
    text+=mainitem_after;

    text+=mainitem_before.arg(tr("<a href=\"$$mainhelpdir$$qf3_fitalg.html\">Fit Algorithm</a>"));
    // gather information about plugins
    for (int i=0; i<fitAlgorithmManager->pluginCount(); i++) {
        int id=i;
        QString dir=fitAlgorithmManager->getPluginSettings(id);
        if (QFile::exists(dir)) text+=item_template.arg(fitAlgorithmManager->getIconFilename(id)).arg(fitAlgorithmManager->getName(id)).arg(dir);
    }
    text+=mainitem_after;
    text+=mainitem_before.arg(tr("<a href=\"$$mainhelpdir$$qf3_fitfunc.html\">Fit Function</a>"));
    // gather information about plugins
    for (int i=0; i<fitFunctionManager->pluginCount(); i++) {
        int id=i;
        QString dir=fitFunctionManager->getPluginSettings(id);
        if (QFile::exists(dir)) text+=item_template.arg(fitFunctionManager->getIconFilename(id)).arg(fitFunctionManager->getName(id)).arg(dir);
    }
    text+=mainitem_after;

    text+=mainitem_before.arg(tr("<a href=\"$$mainhelpdir$$qf3_importer.html\">Importers</a>"));
    // gather information about plugins
    for (int i=0; i<importerManager->pluginCount(); i++) {
        int id=i;
        QString dir=importerManager->getPluginSettings(id);
        if (QFile::exists(dir)) text+=item_template.arg(importerManager->getIconFilename(id)).arg(importerManager->getName(id)).arg(dir);
    }
    text+=mainitem_after;

    text+=mainitem_before.arg(tr("<a href=\"$$mainhelpdir$$qf3_extension.html\">Extension</a>"));
    // gather information about plugins
    for (int i=0; i<getExtensionManager()->getIDList().size(); i++) {
        QString id=getExtensionManager()->getIDList().at(i);
        QString dir=getExtensionManager()->getPluginSettings(id);
        if (QFile::exists(dir)) text+=item_template.arg(getExtensionManager()->getIconFilename(id)).arg(getExtensionManager()->getName(id)).arg(dir);
    }
    text+=mainitem_after;

    return text;
}

QString MainWindow::createPluginDocHelp(QString mainitem_before, QString mainitem_after) {
    QString item_template=QString("<li><a href=\"%3\"><img width=\"16\" height=\"16\" src=\"%1\"></a>&nbsp;<a href=\"%3\">%2</a></li>");
    QString text=mainitem_before.arg(tr("Raw Data Record"));
    // gather information about plugins
    for (int i=0; i<getRawDataRecordFactory()->getIDList().size(); i++) {
        QString id=getRawDataRecordFactory()->getIDList().at(i);
        QString dir=getRawDataRecordFactory()->getPluginHelp(id);
        if (QFile::exists(dir)) text+=item_template.arg(getRawDataRecordFactory()->getIconFilename(id)).arg(getRawDataRecordFactory()->getName(id)).arg(dir);
    }
    text+=mainitem_after;

    text+=mainitem_before.arg(tr("Data Evaluation"));
    // gather information about plugins
    for (int i=0; i<getEvaluationItemFactory()->getIDList().size(); i++) {
        QString id=getEvaluationItemFactory()->getIDList().at(i);
        QString dir=getEvaluationItemFactory()->getPluginHelp(id);
        if (QFile::exists(dir)) text+=item_template.arg(getEvaluationItemFactory()->getIconFilename(id)).arg(getEvaluationItemFactory()->getName(id)).arg(dir);
    }
    text+=mainitem_after;

    text+=mainitem_before.arg(tr("<a href=\"$$mainhelpdir$$qf3_fitalg.html\">Fit Algorithm</a>"));
    // gather information about plugins
    for (int i=0; i<fitAlgorithmManager->pluginCount(); i++) {
        int id=i;
        QString dir=fitAlgorithmManager->getPluginHelp(id);
        if (QFile::exists(dir)) text+=item_template.arg(fitAlgorithmManager->getIconFilename(id)).arg(fitAlgorithmManager->getName(id)).arg(dir);
    }
    text+=mainitem_after;
    text+=mainitem_before.arg(tr("<a href=\"$$mainhelpdir$$qf3_fitfunc.html\">Fit Function</a>"));
    // gather information about plugins
    for (int i=0; i<fitFunctionManager->pluginCount(); i++) {
        int id=i;
        QString dir=fitFunctionManager->getPluginHelp(id);
        if (QFile::exists(dir)) text+=item_template.arg(fitFunctionManager->getIconFilename(id)).arg(fitFunctionManager->getName(id)).arg(dir);
    }
    text+=mainitem_after;

    text+=mainitem_before.arg(tr("<a href=\"$$mainhelpdir$$qf3_importer.html\">Importers</a>"));
    // gather information about plugins
    for (int i=0; i<importerManager->pluginCount(); i++) {
        int id=i;
        QString dir=importerManager->getPluginHelp(id);
        if (QFile::exists(dir)) text+=item_template.arg(importerManager->getIconFilename(id)).arg(importerManager->getName(id)).arg(dir);
    }
    text+=mainitem_after;

    text+=mainitem_before.arg(tr("<a href=\"$$mainhelpdir$$qf3_extension.html\">Extension</a>"));
    // gather information about plugins
    for (int i=0; i<getExtensionManager()->getIDList().size(); i++) {
        QString id=getExtensionManager()->getIDList().at(i);
        QString dir=getExtensionManager()->getPluginHelp(id);
        if (QFile::exists(dir)) text+=item_template.arg(getExtensionManager()->getIconFilename(id)).arg(getExtensionManager()->getName(id)).arg(dir);
    }
    text+=mainitem_after;

    return text;


}

void MainWindow::aboutPlugins() {
    QDialog *widget = new QDialog(this);
    Ui::AboutPlugins ui;
    ui.setupUi(widget);
    QTextEdit* ui_textEdit = qFindChild<QTextEdit*>(widget, "edtInfo");

    QString text=createPluginDoc(false);


    ui_textEdit->setText(text);
    widget->exec();
    delete widget;
}

void MainWindow::createWidgets() {
    QWidget* widgetMain=new QWidget(this);
    QVBoxLayout* layMain=new QVBoxLayout();
    widgetMain->setLayout(layMain);
    layMain->setContentsMargins(2,2,2,2);
    labUpgrade=new QLabel("", this);
    labUpgrade->setVisible(false);
    labUpgrade->setMargin(9);
    labUpgrade->setStyleSheet("background: khaki; border: 1px solid blue; font: black ");
    labUpgrade->setTextInteractionFlags(Qt::TextBrowserInteraction);
    labUpgrade->setWordWrap(true);
    connect(labUpgrade, SIGNAL(linkActivated(QString)), this, SLOT(openLabelLink(QString)));
    layMain->addWidget(labUpgrade);
    spCenter=new QVisibleHandleSplitter(Qt::Horizontal, this);
    spCenter->setOrientation(Qt::Horizontal);
    spMain=new QVisibleHandleSplitter(Qt::Vertical, this);
    spMain->setOrientation(Qt::Vertical);
    layMain->addWidget(spMain);
    tvMain=new QTreeView(this);
    tvMain->setHeaderHidden(true);
    tvMain->setContextMenuPolicy(Qt::ActionsContextMenu);
    tvMain->setEditTriggers(QAbstractItemView::SelectedClicked|QAbstractItemView::EditKeyPressed);
    connect(tvMain, SIGNAL(doubleClicked ( const QModelIndex &)), this, SLOT(projectElementDoubleClicked(const QModelIndex&)));
    spCenter->addWidget(tvMain);
    QShortcut* shortcut = new QShortcut(QKeySequence(tr("Del")), tvMain);
    connect(shortcut, SIGNAL(activated()), this, SLOT(deleteItem()));
    setCentralWidget(widgetMain);

    QWidget* w=new QWidget(spCenter);
    QFormLayout* fl=new QFormLayout();
    w->setLayout(fl);
    spCenter->addWidget(w);
    spMain->addWidget(spCenter);

    edtName=new QLineEdit(w);
    fl->addRow(tr("Project &Name:"), edtName);
    connect(edtName, SIGNAL(textChanged(const QString&)), this, SLOT(projectNameChanged(const QString&)));
    labFile=new QFElidedLabel(w);
    labFile->setWordWrap(true);
    labFile->setTextInteractionFlags(Qt::TextSelectableByMouse);
    labFile->setSizePolicy(QSizePolicy::Ignored, labFile->sizePolicy().verticalPolicy());
    fl->addRow(tr("Project File:"), labFile);
    edtCreator=new QLineEdit(w);
    fl->addRow(tr("Project &Creator:"), edtCreator);
    connect(edtCreator, SIGNAL(textChanged(const QString&)), this, SLOT(projectCreatorChanged(const QString&)));
    pteDescription=new QPlainTextEdit(w);
    fl->addRow(tr("Project &Description:"), pteDescription);
    connect(pteDescription, SIGNAL(textChanged()), this, SLOT(projectDescriptionChanged()));

    QFileInfo fi(QApplication::applicationFilePath());

    tabLogs=new QTabWidget(this);
    logFileProjectWidget=new QtLogFile(tabLogs);
    logFileProjectWidget->set_log_date_time(true);
    logFileQDebugWidget=new QtLogFile(tabLogs);
    logFileQDebugWidget->set_log_file_append(true);
    logFileQDebugWidget->set_log_date_time(true);
    qDebug()<<"opening log: "<<QString(settings->getConfigFileDirectory()+"/"+fi.completeBaseName()+"_qdebug.log");
    logFileQDebugWidget->open_logfile(QString(settings->getConfigFileDirectory()+"/"+fi.completeBaseName()+"_qdebug.log"), false);
    appLogFileQDebugWidget=logFileQDebugWidget;

    logFileMainWidget=new QtLogFile(tabLogs);
    logFileMainWidget->set_log_file_append(true);
    logFileMainWidget->set_log_date_time(true);
    logFileProjectWidget->set_log_file_append(true);
    tabLogs->addTab(logFileMainWidget, tr("QuickFit Log"));
    qDebug()<<"opening log: "<<QString(settings->getConfigFileDirectory()+"/"+fi.completeBaseName()+".log");
    logFileMainWidget->open_logfile(QString(settings->getConfigFileDirectory()+"/"+fi.completeBaseName()+".log"), false);
    logFileMainWidget->log_text(tr("starting up QuickFit %1 (SVN: %2 COMILEDATE: %3), %4-bit ...\n").arg(qfInfoVersionFull()).arg(qfInfoSVNVersion()).arg(qfInfoCompileDate()).arg(getApplicationBitDepth()));
    logFileMainWidget->log_text(tr("logging to '%1' ...\n").arg(settings->getConfigFileDirectory()+"/"+fi.completeBaseName()+".log"));
    logFileMainWidget->log_text(tr("configuration directory: '%1' ...\n").arg(settings->getConfigFileDirectory()));
    logFileMainWidget->log_text(tr("global configuration directory: '%1' ...\n").arg(settings->getGlobalConfigFileDirectory()));
    logFileMainWidget->log_text(tr("plugin directory: '%1' ...\n").arg(settings->getPluginDirectory()));
    logFileMainWidget->log_text(tr("assets directory: '%1' ...\n").arg(settings->getAssetsDirectory()));
    tabLogs->addTab(logFileProjectWidget, tr("Project Log"));
    tabLogs->addTab(logFileQDebugWidget, tr("Debug Log"));
    tabLogs->setCurrentWidget(logFileMainWidget);
    spMain->addWidget(tabLogs);

    logFileQDebugWidget->setVisible(settings->debugLogVisible());
    tabLogs->setTabEnabled(tabLogs->indexOf(logFileQDebugWidget), settings->debugLogVisible());

    prgMainProgress=new QProgressBar(this);
    statusBar()->addPermanentWidget(prgMainProgress);

    helpWindow=new QFHTMLHelpWindow(0);
    //helpWindow->setHtmlReplacementList(&htmlReplaceList);
    helpWindow->initFromPluginServices(this);
    helpWindow->close();
    //helpWindow->setParent(NULL, Qt::Tool);
    //if (settings->getHelpWindowsStayOnTop()) helpWindow->setWindowFlags(Qt::Tool/*|Qt::WindowStaysOnTopHint|Qt::X11BypassWindowManagerHint*/ );
    //else helpWindow->setWindowFlags(Qt::Tool);


}


void MainWindow::createActions() {
    newProjectAct = new QAction(QIcon(":/project_new.png"), tr("&New Project"), this);
    newProjectAct->setShortcuts(QKeySequence::New);
    newProjectAct->setStatusTip(tr("Create a new project"));
    connect(newProjectAct, SIGNAL(triggered()), this, SLOT(newProject()));


    openProjectAct = new QAction(QIcon(":/project_open.png"), tr("&Open Project..."), this);
    openProjectAct->setShortcuts(QKeySequence::Open);
    openProjectAct->setStatusTip(tr("Open an existing project"));
    connect(openProjectAct, SIGNAL(triggered()), this, SLOT(openProject()));

    openProjectSubsetAct = new QAction(tr("&Open Project Subset"), this);
    openProjectSubsetAct->setStatusTip(tr("Let the user select a subset of records and evaluations to load from a project"));
    connect(openProjectSubsetAct, SIGNAL(triggered()), this, SLOT(openProjectSubset()));

    openExampleAct = new QAction(tr("&Open Examples Projects ..."), this);
    openExampleAct->setStatusTip(tr("lets the user open one of the example projects distributed together with the software"));
    connect(openExampleAct, SIGNAL(triggered()), this, SLOT(openExample()));

    actReloadProject = new QAction(QIcon(":/reload_project.png"), tr("&Reload Current Project"), this);
    actReloadProject->setStatusTip(tr("Reload the currently opened project"));
    connect(actReloadProject, SIGNAL(triggered()), this, SLOT(reloadProject()));

    saveProjectAct = new QAction(QIcon(":/project_save.png"), tr("&Save Project"), this);
    saveProjectAct->setShortcuts(QKeySequence::Save);
    saveProjectAct->setStatusTip(tr("Save the project to disk"));
    connect(saveProjectAct, SIGNAL(triggered()), this, SLOT(saveProject()));

    saveProjectAsAct = new QAction(QIcon(":/project_saveas.png"), tr("Save Project &As..."), this);
    saveProjectAsAct->setShortcuts(QKeySequence::SaveAs);
    saveProjectAsAct->setStatusTip(tr("Save the project under a new name"));
    connect(saveProjectAsAct, SIGNAL(triggered()), this, SLOT(saveProjectAs()));

    zipProjectAct = new QAction(QIcon(":/project_zip.png"), tr("&Compress Project"), this);
    zipProjectAct->setStatusTip(tr("compress project, icluding data files into a ZIP-archive"));
    connect(zipProjectAct, SIGNAL(triggered()), this, SLOT(zipProject()));

    optionsAct = new QAction(QIcon(":/configure.png"), tr("&Preferences ..."), this);
    optionsAct->setStatusTip(tr("Application settings dialog"));
    optionsAct->setMenuRole(QAction::PreferencesRole);
    connect(optionsAct, SIGNAL(triggered()), this, SLOT(openSettingsDialog()));

    exitAct = new QAction(QIcon(":/exit.png"), tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    exitAct->setStatusTip(tr("Exit the application"));
    exitAct->setMenuRole(QAction::QuitRole);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));


    helpAct=new QAction(QIcon(":/help.png"), tr("&Help"), this);
    connect(helpAct, SIGNAL(triggered()), this, SLOT(displayHelp()));

    helpCopyrightAct=new QAction(QIcon(":/help_copyright.png"), tr("QuickFit &Copyright"), this);
    connect(helpCopyrightAct, SIGNAL(triggered()), this, SLOT(displayHelpCopyright()));
    helpWelcomeScreenAct=new QAction(tr("Show Welcome Screen"), this);
    connect(helpWelcomeScreenAct, SIGNAL(triggered()), this, SLOT(openWelcomeScreen()));
    helpPluginCopyrightAct=new QAction(QIcon(":/lib/help/help_copyright.png"), tr("Plugin C&opyright"), this);
    connect(helpPluginCopyrightAct, SIGNAL(triggered()), this, SLOT(displayHelpPluginCopyright()));
    helpCitingAct=new QAction(QIcon(":/help_copyright.png"), tr("Citing QuickFit ..."), this);
    connect(helpCitingAct, SIGNAL(triggered()), this, SLOT(displayHelpCiting()));
    helpFAQAct=new QAction(QIcon(":/lib/help/help_faq.png"), tr("&Frequently asked questions (FAQs)"), this);
    connect(helpFAQAct, SIGNAL(triggered()), this, SLOT(displayHelpFAQ()));
    helpTutorialsAct=new QAction(QIcon(":/lib/help/help_tutorial.png"), tr("Plugin &Tutorials"), this);
    connect(helpTutorialsAct, SIGNAL(triggered()), this, SLOT(displayHelpTutorials()));
    helpPluginAct=new QAction(QIcon(":/lib/help/help_contents.png"), tr("&Plugin Help"), this);
    connect(helpPluginAct, SIGNAL(triggered()), this, SLOT(displayPluginHelp()));
    helpContactAuthors=new QAction(QIcon(":/lib/mail.png"), tr("&Contact Authors (bugreport, question, ...)"), this);
    connect(helpContactAuthors, SIGNAL(triggered()), this, SLOT(contactAuhtors()));
    helpContactMaillinglist=new QAction(QIcon(":/lib/mail.png"), tr("&Contact QuickFit Mailinglist"), this);
    connect(helpContactMaillinglist, SIGNAL(triggered()), this, SLOT(contactMailinglist()));
    helpOpenWebpageAct=new QAction(QIcon(":/lib/help/www.png"), tr("QuickFit &Webpage"), this);
    connect(helpOpenWebpageAct, SIGNAL(triggered()), this, SLOT(openWebpage()));
    actCheckUpdate=new QAction(QIcon(":/lib/help/www.png"), tr("Check for updates ..."), this);
    connect(actCheckUpdate, SIGNAL(triggered()), this, SLOT(checkUpdates()));

    helpActList.append(helpAct);
    helpActList.append(helpCopyrightAct);
    helpActList.append(helpPluginAct);
    helpActList.append(helpCitingAct);
    helpActList.append(helpFAQAct);
    helpActList.append(helpTutorialsAct);
    helpActList.append(helpPluginCopyrightAct);


    QAction* actSort;
    actsSort=new QActionGroup(this);
    actsSort->setExclusive(true);
    actsSort->addAction(actSort=new QAction(tr("... by ID"), this));
    actSort->setCheckable(true);
    actSort->setChecked(true);
    actsSort->addAction(actSort=new QAction(tr("... by Name"), this));
    actSort->setCheckable(true);
    actsSort->addAction(actSort=new QAction(tr("... by Folder-Type-Name-Role"), this));
    actSort->setCheckable(true);
    actsSort->addAction(actSort=new QAction(tr("... by Folder-Name-Type-Role"), this));
    actSort->setCheckable(true);
    actsSort->addAction(actSort=new QAction(tr("... by Folder-Role-Type-Name"), this));
    actSort->setCheckable(true);
    connect(actsSort, SIGNAL(triggered(QAction*)), this, SLOT(setProjectSortOrder(QAction*)));





    aboutAct = new QAction(QIcon(":/about.png"), tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    aboutAct->setMenuRole(QAction::AboutRole);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutPluginsAct = new QAction(QIcon(":/about.png"), tr("About &Plugins"), this);
    aboutPluginsAct->setStatusTip(tr("Show a list with all registered plugins"));
    connect(aboutPluginsAct, SIGNAL(triggered()), this, SLOT(aboutPlugins()));


    aboutQtAct = new QAction(QIcon(":/aboutqt.png"), tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    aboutQtAct->setMenuRole(QAction::AboutQtRole);
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));



    dupItemAct = new QAction(QIcon(":/lib/item_duplicate.png"), tr("&Duplicate Current Item(s)"), this);
    dupItemAct->setStatusTip(tr("dupicates the currently selected items.\nIf a folder is selected, all files in the folder are duplicated"));
    dupItemAct->setShortcut(QKeySequence("Ctrl+D"));
    connect(dupItemAct, SIGNAL(triggered()), this, SLOT(duplicateItem()));

    tvMain->addAction(dupItemAct);


    delItemAct = new QAction(QIcon(":/lib/item_delete.png"), tr("&Delete Current Item(s)"), this);
    delItemAct->setStatusTip(tr("delete the currently selected item (if deletable) ..."));
    connect(delItemAct, SIGNAL(triggered()), this, SLOT(deleteItem()));
    //delItemAct->setShortcut(QKeySequence::Delete);

    tvMain->addAction(delItemAct);

    tvMain->addAction(getSeparatorAction(this));

    copyItemAct = new QAction(QIcon(":/lib/item_copy.png"), tr("&Copy Current Item(s)"), this);
    copyItemAct->setStatusTip(tr("copies the currently selected items into the clipboard"));
    copyItemAct->setShortcut(QKeySequence::Copy);
    connect(copyItemAct, SIGNAL(triggered()), this, SLOT(copyItem()));
    tvMain->addAction(copyItemAct);

    cutItemAct = new QAction(QIcon(":/lib/item_cut.png"), tr("&Cut Current Item(s)"), this);
    cutItemAct->setStatusTip(tr("cuts the currently selected items into the clipboard"));
    cutItemAct->setShortcut(QKeySequence::Cut);
    connect(cutItemAct, SIGNAL(triggered()), this, SLOT(cutItem()));
    tvMain->addAction(cutItemAct);

    pastItemAct = new QAction(QIcon(":/lib/item_paste.png"), tr("&Paste Item(s)"), this);
    pastItemAct->setStatusTip(tr("paste items from the clipboard into the current project"));
    connect(pastItemAct, SIGNAL(triggered()), this, SLOT(pasteItem()));
    pastItemAct->setShortcut(QKeySequence::Paste);
    tvMain->addAction(pastItemAct);



    actRDRReplace=new QAction(tr("find/replace in raw data record names/folders"), this);
    connect(actRDRReplace, SIGNAL(triggered()), this, SLOT(rdrReplace()));
    actRDRUndoReplace=new QAction(tr("undo last find/replace"), this);
    connect(actRDRUndoReplace, SIGNAL(triggered()), this, SLOT(rdrUndoReplace()));
    actRDRSetProperty=new QAction(tr("set property in multiple RDRs"), this);
    connect(actRDRSetProperty, SIGNAL(triggered()), this, SLOT(rdrSetProperty()));
    actFixFilesPathes=new QAction(tr("fix files paths in project"), this);
    connect(actFixFilesPathes, SIGNAL(triggered()), this, SLOT(fixFilesPathesInProject()));
    actRenameGroups=new QAction(tr("rename RDR groups"), this);
    connect(actRenameGroups, SIGNAL(triggered()), this, SLOT(renameGroups()));
    actSetRDRPropertyByRegExp=new QAction(tr("set RDR property by RegExp"), this);
    connect(actSetRDRPropertyByRegExp, SIGNAL(triggered()), this, SLOT(setRDRPropertyByRegExp()));
    actSetRDRPropertyByExpression=new QAction(tr("caluclate RDR property"), this);
    connect(actSetRDRPropertyByExpression, SIGNAL(triggered()), this, SLOT(setRDRPropertyByExpression()));

    actUserFitfunctionsEditor=new QAction(QIcon(":/lib/edit_fitfunction.png"), tr("edit user fit functions"), this);
    connect(actUserFitfunctionsEditor, SIGNAL(triggered()), this, SLOT(editUserFitFunctions()));
    actPerformanceTest=new QAction(tr("test QFProject performance"), this);
    connect(actPerformanceTest, SIGNAL(triggered()), this, SLOT(projectPerformanceTest()));


}

void MainWindow::createMenus() {
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newProjectAct);

    fileMenu->addAction(openProjectAct);


    /*recentMenu=fileMenu->addMenu(QIcon(":/project_open_recent.png"), tr("&Recent Files"));
    for (int i = 0; i < MaxRecentFiles; ++i)
        recentMenu->addAction(recentFileActs[i]);
    updateRecentFileActions();*/
    recentMenu=new QRecentFilesMenu(fileMenu);
    recentMenu->setDefaultIcon(QIcon(":/qf3fileicon.png"));
    recentMenu->setUseSystemFileIcons(false);
    recentMenu->setIcon(QIcon(":/project_open_recent.png"));
    recentMenu->setTitle(tr("&Recent Files"));
    recentMenu->setMaxRecentFilesCount(20);
    QSettings* s=settings->getQSettings();
    recentMenu->readSettings(*s, "mainwindow/recentfilelist");
    connect(recentMenu, SIGNAL(openRecentFile(QString)), this, SLOT(openRecentProject(QString)));
    fileMenu->addMenu(recentMenu);
    fileMenu->addAction(actReloadProject);
    projectSpecialMenu=fileMenu->addMenu(tr("&Special Project Tools ..."));
    projectSpecialMenu->addAction(openProjectSubsetAct);
    projectSpecialMenu->addAction(openExampleAct);

    fileMenu->addSeparator();
    fileMenu->addAction(saveProjectAct);
    fileMenu->addAction(saveProjectAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(zipProjectAct);
    fileMenu->addSeparator();
    fileMenu->addAction(optionsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    dataMenu= menuBar()->addMenu(tr("&Data Items"));
    insertItemMenu=new QMenu(tr("Insert &Raw Data"), dataMenu);
    insertItemMenu->setIcon(QIcon(":/rawdata_insert.png"));
    evaluationMenu= new QMenu(tr("Insert &Evaluation"), dataMenu);
    evaluationMenu->setIcon(QIcon(":/evaluation_insert.png"));


    dataMenu->addMenu(insertItemMenu);
    dataMenu->addMenu(evaluationMenu);
    dataMenu->addSeparator();
    dataMenu->addAction(dupItemAct);
    dataMenu->addAction(delItemAct);
    dataMenu->addSeparator();
    dataMenu->addAction(copyItemAct);
    dataMenu->addAction(cutItemAct);
    dataMenu->addAction(pastItemAct);


    extensionMenu=menuBar()->addMenu(tr("&Extensions"));
    toolsMenu=menuBar()->addMenu(tr("&Tools"));
    toolsMenu->addAction(actUserFitfunctionsEditor);
    projectToolsMenu=toolsMenu->addMenu(tr("&Project Tools"));
    projectToolsMenu->addSeparator();
    projectToolsMenu->addAction(actRDRReplace);
    projectToolsMenu->addAction(actRDRUndoReplace);
    projectToolsMenu->addSeparator();
    projectToolsMenu->addAction(actRDRSetProperty);
    projectToolsMenu->addSeparator();
    projectToolsMenu->addAction(actRenameGroups);
    projectToolsMenu->addSeparator();
    projectToolsMenu->addAction(actSetRDRPropertyByExpression);
    projectToolsMenu->addAction(actSetRDRPropertyByRegExp);
    projectToolsMenu->addSeparator();
    projectToolsMenu->addAction(actFixFilesPathes);
    debugToolsMenu=toolsMenu->addMenu(tr("&Debug Tools"));
    debugToolsMenu->addAction(actPerformanceTest);
    toolsMenu->addSeparator();
    wizardsMenu=toolsMenu->addMenu(tr("&Wizards"));
    wizardsMenu->setIcon(QIcon(":/wizard.png"));
    projectWizardsMenu=wizardsMenu->addMenu(tr("&Projects Wizards"));
    projectWizardsMenu->setIcon(QIcon(":/project_wizard.png"));
    rdrWizardsMenu=wizardsMenu->addMenu(tr("&Raw Data Wizards"));
    rdrWizardsMenu->setIcon(QIcon(":/rdr_wizard.png"));
    evalWizardsMenu=wizardsMenu->addMenu(tr("&Evaluation Wizards"));
    evalWizardsMenu->setIcon(QIcon(":/eval_wizard.png"));
    dataMenu->addSeparator();
    dataMenu->addMenu(rdrWizardsMenu);
    dataMenu->addMenu(evalWizardsMenu);
    fileMenu->insertAction(openProjectAct, projectWizardsMenu->menuAction());
    fileMenu->insertSeparator(openProjectAct);
    wizardsMenu->addSeparator();
    toolsMenu->addSeparator();



    viewMenu= menuBar()->addMenu(tr("&View"));
    menuProjectSort=viewMenu->addMenu(tr("project tree &sort order ..."));
    menuProjectSort->addActions(actsSort->actions());


    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(helpAct);
    helpMenu->addAction(helpCopyrightAct);
    helpMenu->addAction(helpCitingAct);
    helpMenu->addSeparator();
    helpMenu->addAction(helpFAQAct);
    helpMenu->addSeparator();
    helpMenu->addAction(helpPluginAct);
    helpMenu->addAction(helpTutorialsAct);
    helpMenu->addAction(helpPluginCopyrightAct);

    helpMenu->addSeparator();
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutPluginsAct);
    helpMenu->addAction(aboutQtAct);
    helpMenu->addSeparator();
    helpMenu->addAction(actCheckUpdate);
    helpMenu->addSeparator();
    helpMenu->addAction(helpWelcomeScreenAct);
    helpMenu->addSeparator();
    helpMenu->addAction(helpOpenWebpageAct);
    helpMenu->addAction(helpContactAuthors);
    helpMenu->addAction(helpContactMaillinglist);

    menus["file"]=fileMenu;
    menus["data"]=dataMenu;
    menus["view"]=viewMenu;
    menus["data/rdr"]=insertItemMenu;
    menus["data/eval"]=evaluationMenu;
    menus["extensions"]=extensionMenu;
    menus["help"]=helpMenu;
    menus["tools"]=toolsMenu;
    menus["tools/project"]=projectToolsMenu;
    menus["tools/debug"]=debugToolsMenu;
    menus["tools/wizards"]=wizardsMenu;
    menus["tools/project_wizards"]=projectWizardsMenu;
    menus["tools/rdr_wizards"]=rdrWizardsMenu;
    menus["tools/eval_wizards"]=evalWizardsMenu;
    menus["wizards"]=wizardsMenu;
    menus["project_wizards"]=projectWizardsMenu;
    menus["rdr_wizards"]=rdrWizardsMenu;
    menus["eval_wizards"]=evalWizardsMenu;

    tvMain->addAction(getSeparatorAction(this));
    tvMain->addAction(insertItemMenu->menuAction());
    tvMain->addAction(evaluationMenu->menuAction());

}

void MainWindow::insertRawData() {
    QVariant id=sender()->property("call_id");
    if (id.isValid() && project) {
        int pc=project->getRawDataCount()+project->getEvaluationCount();
        std::cout<<"pc="<<pc<<std::endl;
        getRawDataRecordFactory()->createRecord(id.toString(), project);
        if ((pc<=0)&&(project->getRawDataCount()+project->getEvaluationCount()>0)) saveProject();
    }
    tvMain->expandToDepth(2);
    prgMainProgress->setRange(0,1);
    prgMainProgress->setValue(0);
    prgMainProgress->reset();
}

void MainWindow::insertEvaluation() {
    QVariant id=sender()->property("call_id");
    //std::cout<<"insertEvaluation("<<id.toString().toStdString()<<")\n";
    if (id.isValid() && project) {
        int pc=project->getRawDataCount()+project->getEvaluationCount();
        getEvaluationItemFactory()->createRecord(id.toString(), this, project);
        if ((pc<=0)&&(project->getRawDataCount()+project->getEvaluationCount()>0)) saveProject();
    }
    tvMain->expandToDepth(2);
    prgMainProgress->setRange(0,1);
    prgMainProgress->setValue(0);
    prgMainProgress->reset();
}

void MainWindow::createToolBars()
{
    QToolButton* tb;

    fileToolBar = addToolBar(tr("File"));
    fileToolBar->setObjectName("toolbar_file");
    fileToolBar->addAction(newProjectAct);

    tb=new QToolButton(fileToolBar);
    tb->setMenu(projectWizardsMenu);
    tb->setIcon(QIcon(":/project_wizard.png"));
    tb->setToolTip(tr("project wizards ..."));
    tb->setPopupMode(QToolButton::InstantPopup);
    fileToolBar->addWidget(tb);

    fileToolBar->addAction(openProjectAct);
    fileToolBar->addAction(saveProjectAct);
    fileToolBar->addSeparator();
    fileToolBar->addAction(helpAct);

    dataToolBar = addToolBar(tr("Data"));

    tb=new QToolButton(dataToolBar);
    tb->setMenu(insertItemMenu);
    tb->setIcon(QIcon(":/rawdata_insert.png"));
    tb->setToolTip(tr("insert a new raw data item ..."));
    tb->setPopupMode(QToolButton::InstantPopup);
    dataToolBar->addWidget(tb);

    tb=new QToolButton(dataToolBar);
    tb->setMenu(evaluationMenu);
    tb->setIcon(QIcon(":/evaluation_insert.png"));
    tb->setToolTip(tr("insert a new evaluation ..."));
    tb->setPopupMode(QToolButton::InstantPopup);
    dataToolBar->addWidget(tb);

    tb=new QToolButton(dataToolBar);
    tb->setMenu(rdrWizardsMenu);
    tb->setIcon(QIcon(":/rdr_wizard.png"));
    tb->setToolTip(tr("raw data record wizards ..."));
    tb->setPopupMode(QToolButton::InstantPopup);
    dataToolBar->addWidget(tb);

    tb=new QToolButton(dataToolBar);
    tb->setMenu(evalWizardsMenu);
    tb->setIcon(QIcon(":/eval_wizard.png"));
    tb->setToolTip(tr("evaluation wizards ..."));
    tb->setPopupMode(QToolButton::InstantPopup);
    dataToolBar->addWidget(tb);

    dataToolBar->addAction(delItemAct);
    dataToolBar->setObjectName("toolbar_data");
    extensionToolBar=addToolBar(tr("Extensions"));
    extensionToolBar->setObjectName("toolbar_extensions");
    toolsToolBar=addToolBar(tr("Tools"));
    toolsToolBar->setObjectName("toolbar_tools");

    toolbars["file"]=fileToolBar;
    toolbars["data"]=dataToolBar;
    toolbars["extensions"]=extensionToolBar;
    toolbars["tools"]=toolsToolBar;

}

void MainWindow::createStatusBar() {
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings() {
    if (!settings) return;
    //logFileMainWidget->log_text(tr("reading settings from '%1' ...\n").arg(settings->getIniFilename()));

    //restoreState(settings->getQSettings()->value("mainwindow/state", saveState()).toByteArray());
    //restoreGeometry(settings->getQSettings()->value("mainwindow/geometry", saveGeometry()).toByteArray());

    //spMain->restoreState(settings->getQSettings()->value("mainwindow/splitterSizesMain").toByteArray());
    //spCenter->restoreState(settings->getQSettings()->value("mainwindow/splitterSizesCenter").toByteArray());

    loadWidgetGeometry(*(settings->getQSettings()), this, QPoint(5,5), QSize(800,600), "mainwindow/");
    loadSplitter(*(settings->getQSettings()), spMain, "mainwindow/splitterSizesMain/");
    loadSplitter(*(settings->getQSettings()), spCenter, "mainwindow/splitterSizesCenter/");
    recentMenu->readSettings(*(settings->getQSettings()), "mainwindow/recentfilelist");


    currentProjectDir=settings->getQSettings()->value("mainwindow/currentProjectDir", currentProjectDir).toString();
    currentProjectFilter=settings->getQSettings()->value("mainwindow/currentProjectFilter", currentProjectFilter).toString();
    currentRawDataDir=settings->getQSettings()->value("mainwindow/currentRawDataDir", currentRawDataDir).toString();
    //column_separator=settings->getQSettings()->value("csvimport/column_separator", column_separator).toString();
    //decimal_separator=settings->getQSettings()->value("csvimport/decimal_separator", decimal_separator).toString();
    //comment_start=settings->getQSettings()->value("csvimport/comment_start", comment_start).toString();
    //header_start=settings->getQSettings()->value("csvimport/header_start", header_start).toString();
    //currentFCSFileFormatFilter=settings->getQSettings()->value("mainwindow/currentFCSFileFormatFilter", currentFCSFileFormatFilter).toString();

    logFileQDebugWidget->readSettings(*(settings->getQSettings()), "mainwindow/logDebug");
    logFileMainWidget->readSettings(*(settings->getQSettings()), "mainwindow/logMain");
    logFileProjectWidget->readSettings(*(settings->getQSettings()), "mainwindow/logProject");

    helpWindow->readSettings(*settings->getQSettings(), "mainwindow/help_");

    int idxd=tabLogs->indexOf(logFileQDebugWidget);
    if (settings->debugLogVisible()) {
        if (idxd<0) tabLogs->addTab(logFileQDebugWidget, tr("Debug Log"));
        tabLogs->setTabEnabled(tabLogs->indexOf(logFileQDebugWidget), settings->debugLogVisible());
    } else {
        if (idxd>=0) tabLogs->removeTab(idxd);

    }
    logFileQDebugWidget->setVisible(settings->debugLogVisible());

    if (settings->debugLogVisible()) qInstallMsgHandler(myMessageOutput);
    else qInstallMsgHandler(0);

    extensionManager->readPluginSettings(settings);

    //std::cout<<"autosave="<<settings->getAutosave()<<"\n";
    if (settings->getAutosave()<=0) {
        timerAutosave->stop();
    } else {
        timerAutosave->setInterval(settings->getAutosave()*60000);
        timerAutosave->start();
    }

    //if (settings->getProjectWindowsStayOnTop()) setWindowFlags(Qt::Window/*|Qt::WindowStaysOnTopHint|Qt::X11BypassWindowManagerHint */);
    //else setWindowFlags(Qt::Window);
    //show();

    //if (helpWindow) {
    //    if (settings->getHelpWindowsStayOnTop()) helpWindow->setWindowFlags(Qt::Tool/*|Qt::WindowStaysOnTopHint|Qt::X11BypassWindowManagerHint*/ );
    //    else helpWindow->setWindowFlags(Qt::Tool);
    //}

}

void MainWindow::writeSettings() {
    if (!settings) return;
    //logFileMainWidget->log_text(tr("writing settings to '%1' ...\n").arg(settings->getIniFilename()));
    //settings->getQSettings()->setValue("mainwindow/pos", pos());
    //settings->getQSettings()->setValue("mainwindow/size", size());
    /*(settings->getQSettings())->setValue("mainwindow/state", saveState());
    settings->getQSettings()->setValue("mainwindow/geometry", saveGeometry());
    settings->getQSettings()->setValue("mainwindow/splitterSizesMain", spMain->saveState());
    settings->getQSettings()->setValue("mainwindow/splitterSizesCenter", spCenter->saveState());*/

    saveWidgetGeometry(*(settings->getQSettings()), this, "mainwindow/");
    saveSplitter(*(settings->getQSettings()), spMain, "mainwindow/splitterSizesMain/");
    saveSplitter(*(settings->getQSettings()), spCenter, "mainwindow/splitterSizesCenter/");
    recentMenu->storeSettings(*(settings->getQSettings()), "mainwindow/recentfilelist");

    logFileMainWidget->saveSettings(*(settings->getQSettings()), "mainwindow/logMain");
    logFileQDebugWidget->saveSettings(*(settings->getQSettings()), "mainwindow/logDebug");
    logFileProjectWidget->saveSettings(*(settings->getQSettings()), "mainwindow/logProject");

    settings->getQSettings()->setValue("mainwindow/currentProjectDir", currentProjectDir);
    settings->getQSettings()->setValue("mainwindow/currentProjectFilter", currentProjectFilter);
    settings->getQSettings()->setValue("mainwindow/currentRawDataDir", currentRawDataDir);

    helpWindow->writeSettings(*settings->getQSettings(), "mainwindow/help_");
    //settings->getQSettings()->setValue("mainwindow/currentFCSFileFormatFilter", currentFCSFileFormatFilter);
    //settings->getQSettings()->setValue("csvimport/column_separator", column_separator);
    //settings->getQSettings()->setValue("csvimport/decimal_separator", decimal_separator);
    //settings->getQSettings()->setValue("csvimport/comment_start", comment_start);
    //settings->getQSettings()->setValue("csvimport/header_start", header_start);
    settings->getQSettings()->sync();

    extensionManager->writePluginSettings(settings);

    for (int i=0; i<rawDataPropEditors.size(); i++) {
        if (rawDataPropEditors[i]) rawDataPropEditors[i]->writeSettings();
    }
    for (int i=0; i<evaluationPropEditors.size(); i++) {
        if (evaluationPropEditors[i]) evaluationPropEditors[i]->writeSettings();
    }
    settings->getQSettings()->sync();

}

bool MainWindow::maybeSave() {
    if (!project) return true;
    if (project->hasChanged()) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("QuickFit %1").arg(qfInfoVersionFull()),
                     tr("The project or the data in it has been modified.\n"
                        "Do you want to save your changes?"),
                     QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return saveProject();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void MainWindow::loadProject(const QString &fileName, bool subsetMode, const QSet<int> &rdrSelected, const QSet<int> &evalSelected) {
    newProjectTimer.stop();
    tabLogs->setCurrentWidget(logFileProjectWidget);
    logFileProjectWidget->close_logfile();
    logFileProjectWidget->clearLog();
    logFileProjectWidget->open_logfile(fileName+".log", false);
    logFileProjectWidget->log_line();
    logFileProjectWidget->log_text(tr("loading project file '%1' ...\n").arg(fileName));
    statusBar()->showMessage(tr("loading project file '%1' ...").arg(fileName), 2000);
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::critical(this, tr("QuickFit %1").arg(qfInfoVersionFull()),
                             tr("Cannot access project file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));

        logFileProjectWidget->log_error(tr("Cannot access project file %1:\n    %2.\n")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    closeProject();
    QString fn=fileName;
    logFileProjectWidget->open_logfile(tr("%1.log").arg(fn), false);
    logFileProjectWidget->clearLogStore();
    //project=new QFProject(fn, getEvaluationItemFactory(), getRawDataRecordFactory(), this, this);
    project=new QFProject(getEvaluationItemFactory(), getRawDataRecordFactory(), this, this);



    if (subsetMode) {
        project->readXMLSubSet(fn, rdrSelected, evalSelected);
    } else {
        project->readXML(fn);
    }


    prgMainProgress->reset();
    prgMainProgress->setRange(0,1);
    prgMainProgress->setValue(0);
    connect(project, SIGNAL(propertiesChanged()), this, SLOT(readProjectProperties()));
    readProjectProperties();
    connect(project, SIGNAL(wasChanged(bool)), this, SLOT(setWindowModified(bool)));
    connect(project, SIGNAL(structureChanged()), this, SLOT(modelReset()));
    connect(project, SIGNAL(sortOrderChanged(int)), this, SLOT(setProjectSortOrderActions(int)));
    if (project->error()) {
        QMessageBox::critical(this, tr("QuickFit %1").arg(qfInfoVersionFull()), project->errorDescription());
        logFileProjectWidget->log_error(project->errorDescription()+"\n");
    }
    tvMain->setModel(project->getTreeModel());
    connect(project->getTreeModel(), SIGNAL(modelReset()), tvMain, SLOT(expandAll()));
    tvMain->expandToDepth(2);
    rawDataFactory->distribute(project, settings, this, this);
    evaluationFactory->distribute(project, settings, this, this);
    extensionManager->distribute(project);
    QApplication::restoreOverrideCursor();

    setCurrentProject(project->getFile());
    statusBar()->showMessage(tr("Project file '%1' loaded!").arg(project->getFile()), 2000);
    logFileProjectWidget->log_text(tr("loading project file '%1' ... DONE!\n").arg(project->getFile()));
    logFileProjectWidget->log_line();
}

bool MainWindow::saveProject(const QString &fileName) {
    newProjectTimer.stop();
    logFileMainWidget->log_text(tr("saving project to file '%1' ...\n").arg(fileName));
    tabLogs->setCurrentWidget(logFileProjectWidget);
    statusBar()->showMessage(tr("saving project file '%1' ...").arg(fileName), 2000);
    logFileProjectWidget->log_text(tr("saving project file '%1' ...\n").arg(fileName));
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QElapsedTimer time;
    double elapsed=-1;
    bool ok=true;
    if (project) {

        writeSettings();

        tvMain->setModel(NULL);
        for (int i=0; i<rawDataPropEditors.size(); i++) {
            if (rawDataPropEditors[i]) rawDataPropEditors[i]->writeSettings();
        }
        for (int i=0; i<evaluationPropEditors.size(); i++) {
            if (evaluationPropEditors[i]) evaluationPropEditors[i]->writeSettings();
        }
        logFileProjectWidget->open_logfile(tr("%1.log").arg(fileName), true);
        logFileProjectWidget->clearLogStore();
        project->resetError();
        time.start();

        project->writeXML(fileName);
        ok=!project->error();

        /*if (fileName.toLower().contains(".qfpz") || fileName.toLower().contains(".qfp.gz")) {
            logFileProjectWidget->log_text("SAVING TO GZIPPED PROJECT!\n");
            QuaGzipFile gzf(fileName);
            if (gzf.open(QIODevice::WriteOnly)) {
                project->writeXML(&gzf, true, fileName);
                gzf.close();
            } else {
                QMessageBox::critical(this, tr("QuickFit %1").arg(qfInfoVersionFull()), tr("Could not open GZip-file for output!\n   file: '%1'\n   error: '%2'").arg(fileName).arg(gzf.errorString()));
                logFileProjectWidget->log_error(tr("Could not open GZip-file for output (file: '%1', error: '%2')").arg(fileName).arg(gzf.errorString())+"\n");
            }
        } else {
            project->writeXML(fileName);
        }*/



#if QT_VERSION >= 0x040800
   elapsed=double(time.nsecsElapsed())/1.0e9;
#else
    elapsed=double(time.elapsed())/1000.0;
#endif
        elapsed=double(time.elapsed())/1000.0;
        readProjectProperties();
        if (project->error()) {
            QMessageBox::critical(this, tr("QuickFit %1").arg(qfInfoVersionFull()), project->errorDescription());
            logFileProjectWidget->log_error(project->errorDescription()+"\n");
            ok=false;
        }
        tvMain->setModel(project->getTreeModel());
        connect(project->getTreeModel(), SIGNAL(modelReset()), tvMain, SLOT(expandAll()));
        tvMain->expandAll();
    }
    QApplication::restoreOverrideCursor();

    if (ok) {
        setCurrentProject(fileName);
        statusBar()->showMessage(tr("Project file '%1' saved!").arg(fileName), 2000);
        logFileProjectWidget->log_text(tr("Project file '%1' saved after %2 secs (Filesize: %3 MB)!\n").arg(fileName).arg(elapsed).arg(double(QFileInfo(fileName).size())/1024.0/1024.0));
    }
    return ok;
}

void MainWindow::setCurrentProject(const QString &fileName) {
    curFile = fileName;
    setWindowModified(false);
    tvMain->expandToDepth(2);

    QString shownName;
    if (curFile.isEmpty())
        shownName = tr("untitled.qfp");
    else
        shownName = strippedName(curFile);

    if (projectModeEnabled) setWindowTitle(tr("%1[*] - %2").arg(shownName).arg(tr("QuickFit %1").arg(qfInfoVersionFull())));
    else  setWindowTitle(tr("%2 - %1").arg(nonprojectTitle).arg(tr("QuickFit %1").arg(qfInfoVersionFull())));

    // update recent files list in ini file
    recentMenu->addRecentFile(fileName);
    QSettings* s=settings->getQSettings();
    recentMenu->storeSettings(*s, "mainwindow/recentfilelist");

}


QString MainWindow::strippedName(const QString &fullFileName) {
    return QFileInfo(fullFileName).fileName();
}


void MainWindow::projectElementDoubleClicked ( const QModelIndex & index ) {
    if (project) {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        QFProjectTreeModelNode::nodeType nt=project->getTreeModel()->classifyIndex(tvMain->selectionModel()->currentIndex());
        Qt::WindowFlags f=Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint;
        //if (settings->getChildWindowsStayOnTop()) f=f|Qt::WindowStaysOnTopHint|Qt::X11BypassWindowManagerHint;
        if (nt==QFProjectTreeModelNode::qfpntRawDataRecord) {
            QFRawDataRecord* rec=project->getTreeModel()->getRawDataByIndex(index);
            if (rec) {
                QFRawDataPropertyEditor* edt=new QFRawDataPropertyEditor(this, settings, rec, rawDataPropEditors.size(), NULL, f);
                edt->setAttribute(Qt::WA_DeleteOnClose);
                rawDataPropEditors.append(edt);
                edt->show();
                edt->activateWindow();
            }
        } else if (nt==QFProjectTreeModelNode::qfpntEvaluationRecord) {
            QFEvaluationItem* rec=project->getTreeModel()->getEvaluationByIndex(tvMain->selectionModel()->currentIndex());
            if (rec) {
                QFEvaluationPropertyEditor* edt=new QFEvaluationPropertyEditor(this, settings, rec, evaluationPropEditors.size(), NULL, f);
                edt->setAttribute(Qt::WA_DeleteOnClose);
                evaluationPropEditors.append(edt);
                edt->show();
                edt->activateWindow();
            }
        }
        QApplication::restoreOverrideCursor();
    }
}

void MainWindow::projectNameChanged(const QString& text) {
    if (project) {
        project->setName(text);
    }
}

void MainWindow::projectCreatorChanged(const QString& text) {
    if (project) {
        project->setCreator(text);
    }
}

void MainWindow::projectDescriptionChanged() {
    if (project) {
        project->setDescription(pteDescription->toPlainText());
    }
}

void MainWindow::readProjectProperties() {
    if (project) {
        if (edtName->text()!=project->getName()) edtName->setText(project->getName());
        if (edtCreator->text()!=project->getCreator()) edtCreator->setText(project->getCreator());
        if (pteDescription->toPlainText()!=project->getDescription()) pteDescription->setPlainText(project->getDescription());
        labFile->setText(project->getFile());
    }
}



void MainWindow::deleteItem() {
    if (project) {
        QFProjectTreeModelNode::nodeType nt=project->getTreeModel()->classifyIndex(tvMain->selectionModel()->currentIndex());
        if (nt==QFProjectTreeModelNode::qfpntRawDataRecord) {
            QFRawDataRecord* rec=project->getTreeModel()->getRawDataByIndex(tvMain->selectionModel()->currentIndex());
            if (rec) {
                QFRawDataRecord* nrec=project->getTreeModel()->getNextRecord(rec);
                //qDebug()<<"rec "<<rec<<"   next "<<nrec<<"   index "<<project->getTreeModel()->index(nrec);
                project->deleteRawData(rec->getID());
                QApplication::processEvents();
                QApplication::processEvents();
                tvMain->expandToDepth(2);
                QApplication::processEvents();
                QApplication::processEvents();
                tvMain->setCurrentIndex(project->getTreeModel()->index(nrec));
                //qDebug()<<"    index "<<project->getTreeModel()->index(nrec);
            }
        } else if (nt==QFProjectTreeModelNode::qfpntEvaluationRecord) {
            QFEvaluationItem* rec=project->getTreeModel()->getEvaluationByIndex(tvMain->selectionModel()->currentIndex());
            if (rec) {
                QFEvaluationItem* nrec=project->getTreeModel()->getNextRecord(rec);
                //qDebug()<<"rec "<<rec<<"   next "<<nrec<<"   index "<<project->getTreeModel()->index(nrec);
                project->deleteEvaluation(rec->getID());
                QApplication::processEvents();
                QApplication::processEvents();
                tvMain->expandToDepth(2);
                QApplication::processEvents();
                QApplication::processEvents();
                tvMain->selectionModel()->select(project->getTreeModel()->index(nrec), QItemSelectionModel::Current|QItemSelectionModel::Select);
                //qDebug()<<"    index "<<project->getTreeModel()->index(nrec);
            }
        } else if (nt==QFProjectTreeModelNode::qfpntDirectory) {
            QFProjectTreeModelNode* dir=project->getTreeModel()->getTreeNodeByIndex(tvMain->selectionModel()->currentIndex());
            if (dir) {
                QList<QFProjectTreeModelNode*> children=dir->getAllChildrenRDRandEval();
                bool ok=true;
                if (children.size()>1) {
                    ok = (QMessageBox::question(this, tr("QuickFit 3.0"), tr("Dou you really want to delete all %1 child items of the selected node?").arg(children.size()), QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::Yes);
                }
                if (ok) {
                    // build lists of the IDs to delete
                    QList<int> evalIDs, rdrIDs;
                    for (int i=0; i<children.size(); i++) {
                        if (children[i]->type()==QFProjectTreeModelNode::qfpntEvaluationRecord && children[i]->evaluationItem())
                            evalIDs<<children[i]->evaluationItem()->getID();
                            //project->deleteEvaluation(children[i]->evaluationItem()->getID());
                        if (children[i]->type()==QFProjectTreeModelNode::qfpntRawDataRecord && children[i]->rawDataRecord())
                            rdrIDs<<children[i]->rawDataRecord()->getID();
                            //project->deleteRawData(children[i]->rawDataRecord()->getID());
                    }
                    // delete IDs
                    for (int i=0; i<evalIDs.size(); i++)  {
                        project->deleteEvaluation(evalIDs[i]);
                    }
                    for (int i=0; i<rdrIDs.size(); i++)  {
                        project->deleteRawData(rdrIDs[i]);
                    }
                }
            }
        }
    }
}

void MainWindow::duplicateItem()
{
    if (project) {
        QFProjectTreeModelNode::nodeType nt=project->getTreeModel()->classifyIndex(tvMain->selectionModel()->currentIndex());
        if (nt==QFProjectTreeModelNode::qfpntRawDataRecord) {
            QFRawDataRecord* rec=project->getTreeModel()->getRawDataByIndex(tvMain->selectionModel()->currentIndex());
            if (rec) {
                project->duplicateRawData(rec->getID());
            }
        } else if (nt==QFProjectTreeModelNode::qfpntEvaluationRecord) {
            QFEvaluationItem* rec=project->getTreeModel()->getEvaluationByIndex(tvMain->selectionModel()->currentIndex());
            if (rec) {
                project->duplicateEvaluation(rec->getID());
            }
        } else if (nt==QFProjectTreeModelNode::qfpntDirectory) {
            QFProjectTreeModelNode* dir=project->getTreeModel()->getTreeNodeByIndex(tvMain->selectionModel()->currentIndex());
            if (dir) {
                QList<QFProjectTreeModelNode*> children=dir->getAllChildrenRDRandEval();
                // build lists of the IDs to delete
                QList<int> evalIDs, rdrIDs;
                for (int i=0; i<children.size(); i++) {
                    if (children[i]->type()==QFProjectTreeModelNode::qfpntEvaluationRecord && children[i]->evaluationItem())
                        evalIDs<<children[i]->evaluationItem()->getID();
                        //project->deleteEvaluation(children[i]->evaluationItem()->getID());
                    if (children[i]->type()==QFProjectTreeModelNode::qfpntRawDataRecord && children[i]->rawDataRecord())
                        rdrIDs<<children[i]->rawDataRecord()->getID();
                        //project->deleteRawData(children[i]->rawDataRecord()->getID());
                }
                // delete IDs
                for (int i=0; i<rdrIDs.size(); i++)  {
                    project->duplicateRawData(rdrIDs[i]);
                }
                for (int i=0; i<evalIDs.size(); i++)  {
                    project->duplicateEvaluation(evalIDs[i]);
                }
            }
        }
    }
}

void MainWindow::copyItem()
{
    QString mimetype="quickfit3/projectcopyxml";
    if (project) {

        QList<int> rdrs, evals;
        QFProjectTreeModelNode::nodeType nt=project->getTreeModel()->classifyIndex(tvMain->selectionModel()->currentIndex());
        if (nt==QFProjectTreeModelNode::qfpntRawDataRecord) {
            QFRawDataRecord* rec=project->getTreeModel()->getRawDataByIndex(tvMain->selectionModel()->currentIndex());
            if (rec) {
                rdrs.append(rec->getID());
            }
        } else if (nt==QFProjectTreeModelNode::qfpntEvaluationRecord) {
            QFEvaluationItem* rec=project->getTreeModel()->getEvaluationByIndex(tvMain->selectionModel()->currentIndex());
            if (rec) {
                evals.append(rec->getID());
            }
        } else if (nt==QFProjectTreeModelNode::qfpntDirectory) {
            QFProjectTreeModelNode* dir=project->getTreeModel()->getTreeNodeByIndex(tvMain->selectionModel()->currentIndex());
            if (dir) {
                QList<QFProjectTreeModelNode*> children=dir->getAllChildrenRDRandEval();
                // build lists of the IDs to delete
                for (int i=0; i<children.size(); i++) {
                    if (children[i]->type()==QFProjectTreeModelNode::qfpntEvaluationRecord && children[i]->evaluationItem()){
                        int id=children[i]->evaluationItem()->getID();
                        if (!evals.contains(id)) evals.append(id);
                        //project->deleteEvaluation(children[i]->evaluationItem()->getID());
                    }
                    if (children[i]->type()==QFProjectTreeModelNode::qfpntRawDataRecord && children[i]->rawDataRecord()){
                        int id=children[i]->rawDataRecord()->getID();
                        if (!rdrs.contains(id)) rdrs.append(id);
                        //project->deleteRawData(children[i]->rawDataRecord()->getID());
                    }
                }
            }
        }


        //qDebug()<<rdrs<<evals;

        if(rdrs.size()>0 || evals.size()>0) {
            QString xml;
            {
                QXmlStreamWriter w(&xml);
                w.writeStartDocument();
                w.writeStartElement("quickfit3_projectcopyxml");
                for (int i=0; i<rdrs.size(); i++) {
                    QFRawDataRecord* r=project->getRawDataByID(rdrs[i]);
                    if (r) {
                        r->writeXML(w, QApplication::applicationFilePath());
                    }
                }
                for (int i=0; i<evals.size(); i++) {
                    QFEvaluationItem* r=project->getEvaluationByID(evals[i]);
                    if (r) {
                        r->writeXML(w, QApplication::applicationFilePath());
                    }
                }
                w.writeEndElement();
                w.writeEndDocument();
            }
            QClipboard *clipboard = QApplication::clipboard();
            if (xml.size()>0) {
                QMimeData* mime=new QMimeData();
                mime->setData(mimetype, xml.toUtf8());
                clipboard->setMimeData(mime);
                //qDebug()<<"copied XML: "<<xml;
            }
        }
    }
}

void MainWindow::cutItem()
{
    copyItem();
    deleteItem();
}

void MainWindow::pasteItem()
{
    QString mimetype="quickfit3/projectcopyxml";
    if (project && clipboardContainsProjectXML()) {
        QClipboard *clipboard = QApplication::clipboard();
        const QMimeData* mime=clipboard->mimeData();
        if (mime) {
            QString xml=QString::fromUtf8(mime->data(mimetype));
            qDebug()<<"XML: "<<xml<<"\n\n";
            QDomDocument doc;
            if (doc.setContent(xml)) {
                qDebug()<<"XML-Doc: "<<doc.toString();
                QDomElement base=doc.firstChildElement("quickfit3_projectcopyxml");
                QDomElement rdre=base.firstChildElement("rawdataelement");
                while (!rdre.isNull()) {
                    QString t=rdre.attribute("type");
                    QFRawDataRecord* rdr=rawDataFactory->createRecord(t, project);
                    if (rdr) {
                        rdr->initNewID(rdre);
                    }
                    rdre=rdre.nextSiblingElement("rawdataelement");
                }
                QDomElement evale=base.firstChildElement("evaluationelement");
                while (!evale.isNull()) {
                    QString t=evale.attribute("type");
                    QFEvaluationItem* eval=evaluationFactory->createRecord(t, this, project);
                    if (eval) {
                        eval->initNewID(evale);
                    }
                    evale=evale.nextSiblingElement("evaluationelement");
                }
            }

        }
    }
}

void MainWindow::modelReset() {
    tvMain->expandToDepth(2);
}

void MainWindow::log_global_indent() {
    logFileMainWidget->inc_indent();
    QApplication::processEvents();
}

void MainWindow::log_global_unindent() {
    logFileMainWidget->dec_indent();
    QApplication::processEvents();
}

void MainWindow::log_indent() {
    logFileProjectWidget->inc_indent();
    QApplication::processEvents();
}

void MainWindow::log_unindent() {
    logFileProjectWidget->dec_indent();
    QApplication::processEvents();
}


void MainWindow::log_text(QString message) {
    logFileProjectWidget->log_text(message);
    QApplication::processEvents();
}

void MainWindow::log_warning(QString message) {
    logFileProjectWidget->log_warning(message);
    QApplication::processEvents();
}

void MainWindow::log_error(QString message) {
    logFileProjectWidget->log_error(message);
    QApplication::processEvents();
}

void MainWindow::log_global_text(QString message) {
    logFileMainWidget->log_text(message);
    QApplication::processEvents();
}

void MainWindow::log_global_warning(QString message) {
    logFileMainWidget->log_warning(message);
    QApplication::processEvents();
}

void MainWindow::log_global_error(QString message) {
    logFileMainWidget->log_error(message);
    QApplication::processEvents();
}

void MainWindow::setStatusMessage(QString message) {
    statusBar()->showMessage(message);
    QApplication::processEvents();
}

void MainWindow::setProgressRange(int minP, int maxP) {
    prgMainProgress->setRange(minP, maxP);
    QApplication::processEvents();
}

void MainWindow::setProgress(int value) {
    prgMainProgress->setValue(value);
    QApplication::processEvents();
}

void MainWindow::incProgress() {
    prgMainProgress->setValue(prgMainProgress->value()+1);
    QApplication::processEvents();
}

void MainWindow::openSettingsDialog() {
    OptionsDialog* opt=new OptionsDialog(this);
    opt->setPlugins(pluginOptionDialogs);
    //std::cout<<"opening settings\n";
    opt->open(settings);
    //std::cout<<"autosave="<<settings->getAutosave()<<"\n";
    readSettings();
    if (settings->getAutosave()<=0) {
        timerAutosave->stop();
    } else {
        timerAutosave->setInterval(settings->getAutosave()*60000);
        timerAutosave->start();
    }
    delete opt;
}

void MainWindow::autoWriteSettings() {
    writeSettings();
    QTimer::singleShot(30000, this, SLOT(autoWriteSettings()));
}

QFFitFunctionManager* MainWindow::getFitFunctionManager() const {
    return fitFunctionManager;
}

QFFitAlgorithmManager* MainWindow::getFitAlgorithmManager() const {
    return fitAlgorithmManager;
}

QSettings* MainWindow::getSettings() {
    return settings->getQSettings();
}

ProgramOptions* MainWindow::getOptions() {
    return settings;
}

void MainWindow::autosaveProject() {
    if (!project->hasChanged()) return;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QString autosaveFilename=curFile;
    if (autosaveFilename.isEmpty()) autosaveFilename="untitled.qfp";
    autosaveFilename=autosaveFilename+".autosave";

    statusBar()->showMessage(tr("%2: autosaving project file '%1' ...").arg(autosaveFilename).arg(QTime::currentTime().toString("hh:mm:ss")), 500);
    logFileProjectWidget->log_text(tr("%2: autosaving project file '%1' ...\n").arg(autosaveFilename).arg(QTime::currentTime().toString("hh:mm:ss")));
    QTime time;
    time.start();
    project->writeXML(autosaveFilename, false);
    logFileProjectWidget->log_text(tr("%2: autosaving project file '%1' ... done after %3 sec\n").arg(autosaveFilename).arg(QTime::currentTime().toString("hh:mm:ss")).arg(double(time.elapsed())/1000.0));
    QApplication::restoreOverrideCursor();

}

QString MainWindow::getPluginHelpDirectory(const QString& pluginID) {
    QString hlp=getPluginHelp(pluginID);
    if (hlp.isEmpty()) return settings->getAssetsDirectory()+QString("/help/");
    else return QFileInfo(hlp).absolutePath()+"/";
}


void MainWindow::registerSettingsPane(QFPluginOptionsDialogInterface *plugin)
{
    pluginOptionDialogs.append(plugin);
}

QFEvaluationPropertyEditor *MainWindow::openEvaluationEditor(QFEvaluationItem *rec, bool alwaysCreateNew)
{
/*     Qt::WindowFlags f=Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint;
    if (rec) {
        QFEvaluationPropertyEditor* edt=new QFEvaluationPropertyEditor(this, settings, rec, evaluationPropEditors.size(), NULL, f);
        edt->setAttribute(Qt::WA_DeleteOnClose);
        evaluationPropEditors.append(edt);
        edt->show();
        edt->activateWindow();
        return edt;
    }
    return NULL;*/

    Qt::WindowFlags f=Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint;


    if (rec) {
        bool showNew=alwaysCreateNew;
        QFEvaluationPropertyEditor* edt=NULL;
        if (!alwaysCreateNew) {
            for (int i=0; i<evaluationPropEditors.size(); i++) {
                if (evaluationPropEditors[i]) {
                    if (evaluationPropEditors[i]->getCurrent()==rec) {
                        showNew=false;
                        edt=evaluationPropEditors[i];
                        break;
                    }
                }
            }
        }
        if (showNew || !edt) {
            edt=new QFEvaluationPropertyEditor(this, settings, rec, evaluationPropEditors.size(), NULL, f);
            edt->setAttribute(Qt::WA_DeleteOnClose);
            evaluationPropEditors.append(edt);
            edt->show();
            edt->activateWindow();
            edt->raise();
        } else {
            edt->show();
            edt->activateWindow();
            edt->raise();
        }
        return edt;
    }
    return NULL;

}

QFRawDataPropertyEditor *MainWindow::openRawDataEditor(QFRawDataRecord *rec, bool alwaysCreateNew)
{
    Qt::WindowFlags f=Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint;


    if (rec) {
        bool showNew=alwaysCreateNew;
        QFRawDataPropertyEditor* edt=NULL;
        if (!alwaysCreateNew) {
            for (int i=0; i<rawDataPropEditors.size(); i++) {
                if (rawDataPropEditors[i]) {
                    if (rawDataPropEditors[i]->getCurrent()==rec) {
                        showNew=false;
                        edt=rawDataPropEditors[i];
                        break;
                    }
                }
            }
        }
        if (showNew || !edt) {
            edt=new QFRawDataPropertyEditor(this, settings, rec, rawDataPropEditors.size(), NULL, f);
            edt->setAttribute(Qt::WA_DeleteOnClose);
            rawDataPropEditors.append(edt);
            edt->show();
            edt->activateWindow();
            edt->raise();
        } else {
            edt->show();
            edt->activateWindow();
            edt->raise();
        }
        return edt;
    }
    return NULL;
}

QFProject *MainWindow::getCurrentProject() const
{
    return project;
}

void MainWindow::addGlobalFunction(const QString &name, qfmpCFunction function)
{
    QFMathParser::addGlobalFunction(name, function);
}

void MainWindow::addGlobalVariable(const QString &name, double value)
{
    QFMathParser::addGlobalVariable(name, value);
}

void MainWindow::addGlobalVariable(const QString &name, const QVector<double> &value)
{
    QFMathParser::addGlobalVariable(name, value);
}

void MainWindow::addGlobalVariable(const QString &name, const QString &value)
{
    QFMathParser::addGlobalVariable(name, value);
}

void MainWindow::addGlobalVariable(const QString &name, bool value)
{
    QFMathParser::addGlobalVariable(name, value);
}

QVariant MainWindow::getGlobalConfigValue(const QString &key)
{
    return globalParameterStore.value(key, QVariant());
}

void MainWindow::setGlobalConfigValue(const QString &key, const QVariant &value)
{
    globalParameterStore[key]=value;
}

QString MainWindow::getPluginAssetsDirectory(const QString &pluginID)
{
    return settings->getAssetsDirectory()+QString("/plugins/")+pluginID+QString("/");

}

QString MainWindow::getPluginExamplesDirectory(const QString &pluginID)
{
    return settings->getExamplesDirectory()+QString("/")+pluginID+QString("/");
}

QString MainWindow::getPluginConfigDirectory(const QString &pluginID)
{
    return settings->getConfigFileDirectory()+QString("/plugins/")+pluginID+QString("/");

}

void MainWindow::setProjectMode(bool projectModeEnabled, const QString &nonProjectTitle)
{
    this->projectModeEnabled=projectModeEnabled;
    this->nonprojectTitle=nonProjectTitle;
    bool widVisible=isVisible(); if (widVisible) setUpdatesEnabled(false);
    spMain->setEnabled(projectModeEnabled);
    tvMain->setEnabled(projectModeEnabled);
    tabLogs->setTabEnabled(tabLogs->indexOf(logFileProjectWidget), projectModeEnabled);
    if (!projectModeEnabled) tabLogs->setCurrentWidget(logFileMainWidget);
    if (projectModeEnabled) logFileMainWidget->log_header(tr("activating project mode"));
    else logFileMainWidget->log_header(tr("disabling project mode"));

    projectToolsMenu->setEnabled(projectModeEnabled);
    projectSpecialMenu->setEnabled(projectModeEnabled);
    dataMenu->setEnabled(projectModeEnabled);
    viewMenu->setEnabled(projectModeEnabled);
    evaluationMenu->setEnabled(projectModeEnabled);
    insertItemMenu->setEnabled(projectModeEnabled);
    fileToolBar->setEnabled(projectModeEnabled);
    dataToolBar->setEnabled(projectModeEnabled);
    newProjectAct->setEnabled(projectModeEnabled);
    openProjectAct->setEnabled(projectModeEnabled);
    openProjectSubsetAct->setEnabled(projectModeEnabled);
    openExampleAct->setEnabled(projectModeEnabled);
    saveProjectAct->setEnabled(projectModeEnabled);
    zipProjectAct->setEnabled(projectModeEnabled);
    saveProjectAsAct->setEnabled(projectModeEnabled);
    actReloadProject->setEnabled(projectModeEnabled);
    actRenameGroups->setEnabled(projectModeEnabled);
    actSetRDRPropertyByRegExp->setEnabled(projectModeEnabled);
    actSetRDRPropertyByExpression->setEnabled(projectModeEnabled);
    actRDRReplace->setEnabled(projectModeEnabled);
    actUserFitfunctionsEditor->setEnabled(projectModeEnabled);
    actRDRUndoReplace->setEnabled(projectModeEnabled);
    actPerformanceTest->setEnabled(projectModeEnabled);
    actRDRSetProperty->setEnabled(projectModeEnabled);
    actFixFilesPathes->setEnabled(projectModeEnabled);
    delItemAct->setEnabled(projectModeEnabled);
    dupItemAct->setEnabled(projectModeEnabled);
    copyItemAct->setEnabled(projectModeEnabled);
    cutItemAct->setEnabled(projectModeEnabled);
    pastItemAct->setEnabled(clipboardContainsProjectXML() && projectModeEnabled);
    recentMenu->setMenuEnabled(projectModeEnabled);

    if (widVisible) setUpdatesEnabled(true);
    if (project) setCurrentProject(project->getFile());
    else setCurrentProject("");
}

QFPluginHelpData &MainWindow::getPluginHelpData()
{
    return helpdata;
}




QWidget *MainWindow::getCreateView(const QString &name, const QString &title)
{
    if (!histograms.contains(name)) {
        histograms[name]=new QFHistogramView(NULL);
    }
    histograms[name]->setWindowTitle(title);
    histograms[name]->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
    histograms[name]->setWindowIcon(QIcon(":/lib/result_statistics_icon.png"));
    histograms[name]->show();
    histograms[name]->raise();
    //histograms[name]->setAttribute(Qt::WA_DeleteOnClose);
    if (histograms.size()>1) {
        QSize s=ProgramOptions::getConfigValue("QFHistogramView/size", QSize(800,600)).toSize();
        QPoint p=histograms[name]->pos();
        if (histograms.contains(lastHistogram)) {
            p=histograms[lastHistogram]->pos();
            s=histograms[lastHistogram]->size();
        }

        histograms[name]->move(p+QPoint(32,32));
        histograms[name]->resize(s);
    } else {
        histograms[name]->resize(QSize(500,300));
    }
    histograms[name]->readSettings(*(ProgramOptions::getInstance()->getQSettings()), "histograms/"+cleanStringForFilename(name)+"/");
    histograms[name]->setAutorange(true);
    histograms[name]->setNormalized(true);
    histograms[name]->setLog(false);
    lastHistogram=name;
    return histograms[name];
}


void MainWindow::clearView(const QString &name)
{
    if (histograms.contains(name)) {
        histograms[name]->clear();
        histograms[name]->updateHistogram(true);
        //qDebug()<<"cleared histogram "<<name;
    }
}

void MainWindow::addHistogramToView(const QString &name, const QFHistogramService::Histogram &histogram)
{
    if (histograms.contains(name)) {
        histograms[name]->addCopiedHistogram(histogram.name, histogram.data.data(), histogram.data.size());
        histograms[name]->updateHistogram(true);
        //qDebug()<<"added "<<histogram.data.size()<<" to histogram "<<name<<" (name: "<<histogram.name<<")";
    }
}

QWidget *MainWindow::getCreateTableView(const QString &name, const QString &title)
{
    if (!tables.contains(name)) {
        tables[name]=new QFTableView(NULL);
    }
    tables[name]->setWindowTitle(title);
    tables[name]->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
    tables[name]->setWindowIcon(QIcon(":/lib/result_table.png"));
    tables[name]->show();
    tables[name]->raise();
    //tables[name]->setAttribute(Qt::WA_DeleteOnClose);
    if (tables.size()>1) {
        QSize s=ProgramOptions::getConfigValue("QFTableView/size", QSize(800,600)).toSize();
        QPoint p=tables[name]->pos();
        if (tables.contains(lastHistogram)) {
            p=tables[lastHistogram]->pos();
            s=tables[lastHistogram]->size();
        }

        tables[name]->move(p+QPoint(32,32));
        tables[name]->resize(s);
    } else {
        tables[name]->resize(QSize(500,300));
    }
    tables[name]->readSettings(*(ProgramOptions::getInstance()->getQSettings()), "tables/"+cleanStringForFilename(name)+"/");

    lastHistogram=name;
    return tables[name];
}

void MainWindow::clearTableView(const QString &name)
{
    if (tables.contains(name)) {
        tables[name]->clear();
        //qDebug()<<"cleared histogram "<<name;
    }
}

void MainWindow::addColumnToTableView(const QString &name, const QFTableService::TableColumn &column)
{
    if (tables.contains(name)) {
        tables[name]->addColumn(column.name, column.data);
    }
}

QWidget *MainWindow::getCreateParameterCorrelationView(const QString &name, const QString &title)
{
    if (!correlationViews.contains(name)) {
        correlationViews[name]=new QFParameterCorrelationView(NULL);
    }
    correlationViews[name]->setWindowTitle(title);
    correlationViews[name]->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
    correlationViews[name]->setWindowIcon(QIcon(":/lib/result_correlation_icon.png"));
    correlationViews[name]->show();
    correlationViews[name]->raise();
    //correlationViews[name]->setAttribute(Qt::WA_DeleteOnClose);
    if (correlationViews.size()>1) {
        QSize s=ProgramOptions::getConfigValue("QFParameterCorrelationView/size", QSize(800,600)).toSize();
        QPoint p=correlationViews[name]->pos();
        if (correlationViews.contains(lastParamCorr)) {
            p=correlationViews[lastParamCorr]->pos();
            s=correlationViews[lastParamCorr]->size();
        }

        correlationViews[name]->move(p+QPoint(32,32));
        correlationViews[name]->resize(s);
    } else {
        correlationViews[name]->resize(QSize(500,300));
    }
    correlationViews[name]->readSettings(*(ProgramOptions::getInstance()->getQSettings()), "paramcorr/"+cleanStringForFilename(name)+"/");
    lastParamCorr=name;
    return correlationViews[name];

}

void MainWindow::clearParameterCorrelationView(const QString &name)
{
    if (correlationViews.contains(name)) {
        correlationViews[name]->clear();
        correlationViews[name]->updateCorrelation(true);
    }

}

void MainWindow::addCorrelationToView(const QString &name, const QFParameterCorrelationService::ParameterCorrelation &data)
{
    if (correlationViews.contains(name)) {
        correlationViews[name]->addCopiedCorrelation(data.nameX+tr(" vs. ")+data.nameY, data.dataX.data(), data.dataY.data(), qMin(data.dataX.size(), data.dataY.size()));
        correlationViews[name]->setCorrelation1Label(data.nameX);
        correlationViews[name]->setCorrelation2Label(data.nameY);
        correlationViews[name]->updateCorrelation(true);
    }

}

void MainWindow::addPlotToView(const QString &name, const QFSimplePlotService::PlotData &data)
{
    /*if (correlationViews.contains(name)) {
        correlationViews[name]->addCopiedCorrelation(data.nameX+tr(" vs. ")+data.nameY, data.dataX.data(), data.dataY.data(), qMin(data.dataX.size(), data.dataY.size()));
        correlationViews[name]->setCorrelation1Label(data.nameX);
        correlationViews[name]->setCorrelation2Label(data.nameY);
        correlationViews[name]->updateCorrelation(true);
    }*/

}

void MainWindow::clearSimplePlotView(const QString &name)
{
    /*if (correlationViews.contains(name)) {
        correlationViews[name]->clear();
        correlationViews[name]->updateCorrelation(true);
    }*/

}

QWidget *MainWindow::getCreateSimplePlotView(const QString &name, const QString &title)
{
    /*if (!correlationViews.contains(name)) {
        correlationViews[name]=new QFParameterCorrelationView(NULL);
    }
    correlationViews[name]->setWindowTitle(title);
    correlationViews[name]->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
    correlationViews[name]->setWindowIcon(QIcon(":/lib/result_correlation_icon.png"));
    correlationViews[name]->show();
    correlationViews[name]->raise();
    //correlationViews[name]->setAttribute(Qt::WA_DeleteOnClose);
    if (correlationViews.size()>1) {
        QSize s=ProgramOptions::getConfigValue("QFParameterCorrelationView/size", QSize(800,600)).toSize();
        QPoint p=correlationViews[name]->pos();
        if (correlationViews.contains(lastParamCorr)) {
            p=correlationViews[lastParamCorr]->pos();
            s=correlationViews[lastParamCorr]->size();
        }

        correlationViews[name]->move(p+QPoint(32,32));
        correlationViews[name]->resize(s);
    } else {
        correlationViews[name]->resize(QSize(500,300));
    }
    correlationViews[name]->readSettings(*(ProgramOptions::getInstance()->getQSettings()), "paramcorr/"+cleanStringForFilename(name)+"/");
    lastParamCorr=name;
    return correlationViews[name];*/
    return NULL;
}

QString MainWindow::getPluginHelp(const QString& pluginID) {
    if (evaluationFactory->contains(pluginID)) return evaluationFactory->getPluginHelp(pluginID);
    if (rawDataFactory->contains(pluginID)) return rawDataFactory->getPluginHelp(pluginID);
    if (extensionManager->contains(pluginID)) return extensionManager->getPluginHelp(pluginID);
    if (fitFunctionManager->contains(pluginID)) return fitFunctionManager->getPluginHelp( fitFunctionManager->getPluginForID(pluginID));
    if (fitAlgorithmManager->contains(pluginID)) return fitAlgorithmManager->getPluginHelp(fitAlgorithmManager->getPluginForID(pluginID));
    if (importerManager->contains(pluginID)) return importerManager->getPluginHelp(importerManager->getPluginForID(pluginID));
    return "";
}


QString MainWindow::getFitFunctionHelp(const QString &pluginID) {
    if (fitFunctionManager->hasPluginForID(pluginID)) return fitFunctionManager->getPluginHelp(fitFunctionManager->getPluginForID(pluginID), pluginID);
    return "";
}

QString MainWindow::getFitAlgorithmHelp(const QString &pluginID) {
    if (fitAlgorithmManager->hasPluginForID(pluginID)) return fitAlgorithmManager->getPluginHelp(fitAlgorithmManager->getPluginForID(pluginID), pluginID);
    return "";
}

void MainWindow::addToHelpFurtherReading(const QString &text)
{
    //* The substring should contain one or more $$invisible_ref::$$ entry/ies, which will be displayed on the further_reading page */
    bool found =false;
    for (int i=0; i<htmlReplaceList.size(); i++) {
        if (htmlReplaceList[i].first=="qf_further_reading") {
            htmlReplaceList[i].second=htmlReplaceList[i].second+text;
            //qDebug()<<"qf_further_reading="<<htmlReplaceList[i].second;
            found=true;
            break;
        }
    }
    if (!found) {
        htmlReplaceList.append(qMakePair(QString("qf_further_reading"), text));
        //qDebug()<<"qf_further_reading="<<text;
    }

}

QString MainWindow::getImporterHelp(const QString &pluginID) {
    if (importerManager->hasPluginForID(pluginID)) return importerManager->getPluginHelp(importerManager->getPluginForID(pluginID), pluginID);
    return "";
}

QString MainWindow::getPluginTutorial(const QString& pluginID) {
    if (evaluationFactory->contains(pluginID)) return evaluationFactory->getPluginTutorialMain(pluginID);
    if (rawDataFactory->contains(pluginID)) return rawDataFactory->getPluginTutorialMain(pluginID);
    if (extensionManager->contains(pluginID)) return extensionManager->getPluginTutorialMain(pluginID);
    if (fitFunctionManager->contains(pluginID)) return fitFunctionManager->getPluginTutorialMain( fitFunctionManager->getPluginForID(pluginID));
    if (fitAlgorithmManager->contains(pluginID)) return fitAlgorithmManager->getPluginTutorialMain(fitAlgorithmManager->getPluginForID(pluginID));
    if (importerManager->contains(pluginID)) return importerManager->getPluginTutorialMain(importerManager->getPluginForID(pluginID));
    return "";
}

QString MainWindow::getPluginFAQ(const QString &pluginID)
{
    if (evaluationFactory->contains(pluginID)) return evaluationFactory->getPluginFAQ(pluginID);
    if (rawDataFactory->contains(pluginID)) return rawDataFactory->getPluginFAQ(pluginID);
    if (extensionManager->contains(pluginID)) return extensionManager->getPluginFAQ(pluginID);
    if (fitFunctionManager->contains(pluginID)) return fitFunctionManager->getPluginFAQ( fitFunctionManager->getPluginForID(pluginID));
    if (fitAlgorithmManager->contains(pluginID)) return fitAlgorithmManager->getPluginFAQ(fitAlgorithmManager->getPluginForID(pluginID));
    if (importerManager->contains(pluginID)) return importerManager->getPluginFAQ(importerManager->getPluginForID(pluginID));
    return "";
}

QString MainWindow::getPluginName(const QString &pluginID)
{
    if (evaluationFactory->contains(pluginID)) return evaluationFactory->getName(pluginID);
    if (rawDataFactory->contains(pluginID)) return rawDataFactory->getName(pluginID);
    if (extensionManager->contains(pluginID)) return extensionManager->getName(pluginID);
    if (fitFunctionManager->contains(pluginID)) return fitFunctionManager->getName(fitFunctionManager->getPluginForID(pluginID));
    if (fitAlgorithmManager->contains(pluginID)) return fitAlgorithmManager->getName(fitAlgorithmManager->getPluginForID(pluginID));
    if (importerManager->contains(pluginID)) return importerManager->getName(importerManager->getPluginForID(pluginID));
    return "";
}

QString MainWindow::getPluginHelpSettings(const QString& pluginID) {
    if (evaluationFactory->contains(pluginID)) return evaluationFactory->getPluginSettings(pluginID);
    if (rawDataFactory->contains(pluginID)) return rawDataFactory->getPluginSettings(pluginID);
    if (extensionManager->contains(pluginID)) return extensionManager->getPluginSettings(pluginID);
    if (fitFunctionManager->contains(pluginID)) return fitFunctionManager->getPluginSettings(fitFunctionManager->getPluginForID(pluginID));
    if (fitAlgorithmManager->contains(pluginID)) return fitFunctionManager->getPluginSettings(fitFunctionManager->getPluginForID(pluginID));
    if (importerManager->contains(pluginID)) return importerManager->getPluginSettings(importerManager->getPluginForID(pluginID));
    return "";
}

void MainWindow::displayHelpWindow(const QString& helpfile) {
    helpWindow->clear();
    helpWindow->setContentsMenuActions(helpActList);
    if (helpfile.isEmpty()) helpWindow->updateHelp(settings->getAssetsDirectory()+QString("/help/quickfit.html"));
    else helpWindow->updateHelp(helpfile);
    helpWindow->show();
    helpWindow->raise();
    helpWindow->activateWindow();

}

void MainWindow::displayMainHelpWindow(const QString &helpfile)
{
    displayHelpWindow(settings->getAssetsDirectory()+QString("/help/")+helpfile);
}

void MainWindow::displayPluginHelpWindow(const QString &plugin_id, const QString &helpfile)
{
    if (helpfile.isEmpty()) displayHelpWindow(getPluginHelp(plugin_id));
    else displayHelpWindow(getPluginHelpDirectory(plugin_id)+helpfile);
}

void MainWindow::displayHelp() {
    displayHelpWindow("");
}

void MainWindow::displayHelpCopyright()
{
     displayHelpWindow(settings->getAssetsDirectory()+QString("/help/copyright.html"));
}

void MainWindow::displayHelpPluginCopyright()
{
    displayHelpWindow(settings->getAssetsDirectory()+QString("/help/plugin_copyrights.html"));
}

void MainWindow::displayHelpTutorials()
{
    displayHelpWindow(settings->getAssetsDirectory()+QString("/help/plugin_tutorials.html"));
}

void MainWindow::displayHelpFAQ()
{
    displayHelpWindow(settings->getAssetsDirectory()+QString("/help/qf3_faq.html"));
}

void MainWindow::displayHelpCiting()
{
    displayHelpWindow(settings->getAssetsDirectory()+QString("/help/citing.html"));
}

void MainWindow::displayNewVersionDlg()
{
    DlgNewVersion* dlg=new DlgNewVersion(this);
    dlg->exec();
    delete dlg;
    ProgramOptions::setConfigValue("quickfit/lastrunsvn", qfInfoSVNVersion());
    QTimer::singleShot(200,this, SLOT(displayWelcomeDlg()));
}

void MainWindow::displayWelcomeDlg()
{
    if (ProgramOptions::getConfigValue("quickfit/welcomescreen", true).toBool()) {
        openWelcomeScreen();
    }
}

void MainWindow::openWelcomeScreen()
{
    DlgWelcomeScreen* dlg=new DlgWelcomeScreen(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->addAction(DlgWelcomeScreen::scFirst, newProjectAct, 24);
    dlg->addAction(DlgWelcomeScreen::scFirst, openProjectAct, 24);
    dlg->addAction(DlgWelcomeScreen::scFirst, openExampleAct, 24);
    dlg->addSeparator(DlgWelcomeScreen::scFirst);
    QList<QAction*> acts=projectWizardsMenu->actions();
    for (int i=0; i<acts.size(); i++) {
        if (acts[i] && !acts[i]->isSeparator()) {
            dlg->addAction(DlgWelcomeScreen::scFirst, acts[i], 24);
        }
    }
    dlg->addSeparator(DlgWelcomeScreen::scFirst);
    dlg->addAction(DlgWelcomeScreen::scFirst, helpAct, 24, false);
    dlg->addAction(DlgWelcomeScreen::scFirst, helpFAQAct, 24, false);
    dlg->addAction(DlgWelcomeScreen::scFirst, helpTutorialsAct, 24, false);

    acts=recentMenu->actions();
    for (int i=0; i<acts.size(); i++) {
        if (acts[i] && !acts[i]->isSeparator()) {
            dlg->addAction(DlgWelcomeScreen::scRecent, acts[i]);
        }
    }

    dlg->addAction(DlgWelcomeScreen::scTutorials, helpAct, 24, false);
    dlg->addAction(DlgWelcomeScreen::scTutorials, helpFAQAct, 24, false);
    dlg->addAction(DlgWelcomeScreen::scTutorials, helpTutorialsAct, 24, false);
    dlg->addSeparator(DlgWelcomeScreen::scTutorials);

    acts=getPluginTutorials();
    for (int i=0; i<acts.size(); i++) {
        if (acts[i] && !acts[i]->isSeparator()) {
            dlg->addAction(DlgWelcomeScreen::scTutorials, acts[i], 16,false);
        }
    }
    dlg->finalizeLayout();
    dlg->show();
    dlg->raise();
}
void MainWindow::displayPluginHelp()
{
    displayHelpWindow(settings->getAssetsDirectory()+QString("/help/plugin_help.html"));
}

QMenu* MainWindow::getMenu(QString menu) {
    QMenu* m=NULL;
    if (menus.contains(menu.toLower())) m=menus[menu.toLower()];
    //std::cout<<">>>> getMenu("<<menu.toStdString()<<") = "<<m<<std::endl;
    return m;
}

QToolBar* MainWindow::getToolbar(QString toolbar) {
    QToolBar* m=NULL;
    if (toolbars.contains(toolbar.toLower())) m=toolbars[toolbar.toLower()];
    //std::cout<<">>>> getToolbar("<<toolbar.toStdString()<<") = "<<m<<std::endl;
    return m;
}

void MainWindow::insertMenu(QString menuname, QMenu* newMenu, QMenu* before) {
    menus[menuname.toLower()]=newMenu;
    menuBar()->insertMenu(before->menuAction(), newMenu);
}

void MainWindow::insertToolBar(QString toolbarname, QToolBar* newToolbar) {
    toolbars[toolbarname.toLower()]=newToolbar;
    addToolBar(newToolbar);
}

void MainWindow::registerWizard(const QString &menu, QAction *action)
{
    menus[menu.toLower()]->addAction(action);
}

void MainWindow::registerWizard(const QString &menu, const QString& title, const QObject *receiver, const char *method, QAction **actOut)
{
    registerWizard(menu, title, QIcon(), receiver, method, actOut);
}

void MainWindow::registerWizard(const QString &menu, const QString &title, QIcon icon, const QObject *receiver, const char *method, QAction **actOut)
{
    QAction* actW=new QAction(icon, title, this);
    connect(actW, SIGNAL(triggered()), receiver, method);
    registerWizard(menu, actW);
    if (actOut) *actOut=actW;
}

void MainWindow::addQFMathParserRefernceDir(const QString &directory)
{
    if (!mathParserRefDirs.contains(directory)) mathParserRefDirs.append(mathParserRefDirs);
}

QStringList MainWindow::getQFMathParserRefernceDirs()
{
    return mathParserRefDirs;
}

QFExtensionManager* MainWindow::getExtensionManager() const {
    return extensionManager;
}

QFImporterManager *MainWindow::getImporterManager() const {
    return importerManager;
}

QFEvaluationItemFactory *MainWindow::getEvaluationItemFactory() const
{
    return evaluationFactory;
}

QString MainWindow::getConfigFileDirectory() {
    return settings->getConfigFileDirectory();
}

QString MainWindow::getGlobalConfigFileDirectory() {
    return settings->getGlobalConfigFileDirectory();
}
QString MainWindow::getAssetsDirectory() {
    return settings->getAssetsDirectory();
}

QString MainWindow::getExamplesDirectory()
{
    return settings->getExamplesDirectory();
}

QString MainWindow::getMainHelpDirectory() {
    return settings->getMainHelpDirectory();
}

QString MainWindow::getPluginsDirectory() {
    return settings->getPluginDirectory();
}

void MainWindow::saveProjectFirstTime() {
    if (!settings->getUserSaveAfterFirstEdit()) return;
    if (!project) return;
    if (project->getRawDataCount()+project->getEvaluationCount()>0) {
        newProjectTimer.stop();
        int ret = QMessageBox::question(this, tr("QuickFit %1").arg(qfInfoVersionFull()),
                                tr("You added the first data or evaluation item to an empty project.\n"
                                   "Do you want to save your project now (no further reminders will be shown)?"),
                                QMessageBox::Yes | QMessageBox::No,
                                QMessageBox::Yes);
        if (ret==QMessageBox::Yes) saveProject();
    }
}

void MainWindow::rdrReplace() {
    QFRDRReplaceDialog* dlg=new QFRDRReplaceDialog(this);
    if (dlg->exec() && project) {
        for (int i=0; i<project->getRawDataCount(); i++) {
            QFRawDataRecord* rec=project->getRawDataByNum(i);
            if (dlg->getReplaceName()) {
                rec->setQFProperty("LAST_REPLACENAME", rec->getName(), false, false);
                QString n=rec->getName().replace(dlg->getFindString(), dlg->getReplaceString(), (dlg->getCaseSensitivity())?Qt::CaseSensitive:Qt::CaseInsensitive);
                rec->setName(n);
            } else {
                rec->setQFProperty("LAST_REPLACENAME", "", false, false);
            }
            if (dlg->getReplaceFolder()) {
                rec->setQFProperty("LAST_REPLACEFOLDER", rec->getFolder(), false, false);
                QString n=rec->getFolder().replace(dlg->getFindString(), dlg->getReplaceString(), (dlg->getCaseSensitivity())?Qt::CaseSensitive:Qt::CaseInsensitive);
                rec->setFolder(n);
            } else {
                rec->setQFProperty("LAST_REPLACEFOLDER", "", false, false);
            }
        }
    }
    delete dlg;
}
void MainWindow::rdrUndoReplace() {
    for (int i=0; i<project->getRawDataCount(); i++) {
        QFRawDataRecord* rec=project->getRawDataByNum(i);
        rec->setName(rec->getProperty("LAST_REPLACENAME", rec->getName()).toString());
        rec->setFolder(rec->getProperty("LAST_REPLACEFOLDER", rec->getFolder()).toString());
    }
}

void MainWindow::rdrSetProperty()
{
    if (project) {
        dlgRDRSetProperty* dlg=new dlgRDRSetProperty(this);

        dlg->init(project);

        if (dlg->exec()) {
            QList<QPointer<QFRawDataRecord> > rdrs=dlg->getSelectedRDRs();
            QStringList propNames=dlg->getNewPropNames();
            QList<QVariant> propValues=dlg->getNewPropValues();
            QString p0str=propValues.value(0, "").toString();
            for (int i=0; i<rdrs.size(); i++) {
                rdrs[i]->disableEmitPropertiesChanged();

                switch (dlg->whichToSet()) {
                    case 0: {
                            for (int j=0; j<propNames.size(); j++) {
                                //qDebug()<<rdrs[i]->getName()<<"\n   set "<<propNames[j]<<" = "<<propValues.value(j, QVariant(QString("")));
                                if (!propNames.value(j, "").isEmpty()) {
                                    bool ex=rdrs[i]->propertyExists(propNames.value(j));
                                    if ((ex && dlg->doOverwrite()) || (!ex && dlg->doCreateNew())) rdrs[i]->setQFProperty(propNames[j], propValues.value(j, QVariant(QString(""))), true, true);
                                }
                            }
                        } break;
                    case 1: {
                            if (!p0str.isEmpty()) {
                                rdrs[i]->setName(p0str);
                            }
                        } break;
                    case 2: {
                            if (!p0str.isEmpty()) {
                                rdrs[i]->setFolder(p0str);
                            }
                        } break;
                    case 3: {
                            if (!p0str.isEmpty()) {
                                rdrs[i]->setGroup(rdrs[i]->getProject()->addOrFindRDRGroup(p0str));
                            }
                        } break;
                    case 4: {
                            if (!p0str.isEmpty()) {
                                rdrs[i]->setRole(p0str);
                            }
                        } break;
                }


                rdrs[i]->enableEmitPropertiesChanged(true);
            }
        }

        delete dlg;
    }
}

void MainWindow::projectPerformanceTest() {
    if (project) {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        log_text(tr("\n\n-----------------------------------------------------------------\n-- PERFORMING QFProject PERFORMANCE TEST ...\n-----------------------------------------------------------------\n"));
        log_text(tr("  * adding new record for test ... "));
        QFRawDataRecord* record=project->addRawData("table", tr("performance test table"));
        QFRawDataPropertyEditor* edt=NULL;
        if (record) {
            log_text(tr("OK\n"));
            QElapsedTimer timer;

            for (int s=0; s<2; s++) {
                int propCnt=200;
                int repeats=50;
                double durW[repeats], durR[repeats];
                if (s==1) {
                    log_text(tr("  * opening raw data record editor\n"));
                    edt=new QFRawDataPropertyEditor(this, settings, record, rawDataPropEditors.size(), this, Qt::Dialog|Qt::WindowMaximizeButtonHint|Qt::WindowCloseButtonHint|Qt::WindowSystemMenuHint);
                    edt->setAttribute(Qt::WA_DeleteOnClose);
                    rawDataPropEditors.append(edt);
                    edt->show();
                    repeats=10;
                    QApplication::processEvents();
                    QApplication::processEvents();
                    QApplication::processEvents();
                    QApplication::processEvents();
                }


                log_text(tr("  * writing/reading properties with signals test (operations=%1  repeats=%2): ").arg(propCnt).arg(repeats));
                for (int r=0; r<repeats; r++) {
                    record->clearProperties();
                    //log_text(tr("    * writing %1 properties ... ").arg(propCnt));
                    timer.start();
                    for (int i=0; i<propCnt; i++) {
                        record->setQFProperty(QString("testprop%1").arg(i), i);
                    }
                    #if QT_VERSION >= 0x040800
                        double duration=double(timer.nsecsElapsed())/1.0e6;
                    #else
                        double duration=double(timer.elapsed());
                    #endif
                    durW[r]=duration;
                    //log_text(tr("OK [%1 ms]\n").arg(duration));

                    //log_text(tr("    * reading %1 properties ... ").arg(propCnt));
                    timer.start();
                    for (int i=0; i<propCnt; i++) {
                        int ii=record->getProperty(QString("testprop%1").arg(i), i).toInt();
                        ii=i+1;
                    }
                    #if QT_VERSION >= 0x040800
                        duration=double(timer.nsecsElapsed())/1.0e6;
                    #else
                        duration=double(timer.elapsed());
                    #endif
                    durR[r]=duration;
                    //log_text(tr("OK [%1 ms]\n").arg(duration));
                    log_text(".");
                    QApplication::processEvents();
                }
                log_text("\n");
                log_text(tr("    => per write operation ( %1 +/- %2 ) s\n").arg(statisticsAverage(durW, repeats)/double(propCnt)*1000.0).arg(statisticsStdDev(durW, repeats)/double(propCnt)*1000.0));
                log_text(tr("    => per read operation ( %1 +/- %2 ) s\n").arg(statisticsAverage(durR, repeats)/double(propCnt)*1000.0).arg(statisticsStdDev(durR, repeats)/double(propCnt)*1000.0));


                propCnt=200;
                repeats=50;

                log_text(tr("  * writing/reading properties without signals test (operations=%1  repeats=%2): ").arg(propCnt).arg(repeats));
                record->disableEmitPropertiesChanged();
                for (int r=0; r<repeats; r++) {
                    //log_text(tr("    * writing %1 properties ... ").arg(propCnt));
                    record->clearProperties();
                    timer.start();
                    for (int i=0; i<propCnt; i++) {
                        record->setQFProperty(QString("testprop%1").arg(i), i);
                    }
                    #if QT_VERSION >= 0x040800
                        double duration=double(timer.nsecsElapsed())/1.0e6;
                    #else
                        double duration=double(timer.elapsed());
                    #endif
                    durR[r]=duration;
                    durW[r]=duration;
                    //log_text(tr("OK [%1 ms]\n").arg(duration));

                    //log_text(tr("    * reading %1 properties ... ").arg(propCnt));
                    timer.start();
                    for (int i=0; i<propCnt; i++) {
                        int ii=record->getProperty(QString("testprop%1").arg(i), i).toInt();
                        ii=ii+i;
                    }
                    #if QT_VERSION >= 0x040800
                        duration=double(timer.nsecsElapsed())/1.0e6;
                    #else
                        duration=double(timer.elapsed());
                    #endif
                    durR[r]=duration;
                    durR[r]=duration;
                    //log_text(tr("OK [%1 ms]\n").arg(duration));
                    log_text(".");
                    QApplication::processEvents();
                }
                log_text("\n");
                record->enableEmitPropertiesChanged();
                log_text(tr("    => per write operation ( %1 +/- %2 ) s\n").arg(statisticsAverage(durW, repeats)/double(propCnt)*1000.0).arg(statisticsStdDev(durW, repeats)/double(propCnt)*1000.0));
                log_text(tr("    => per read operation ( %1 +/- %2 ) s\n").arg(statisticsAverage(durR, repeats)/double(propCnt)*1000.0).arg(statisticsStdDev(durR, repeats)/double(propCnt)*1000.0));
                QApplication::processEvents();


                //if (s==0) {
                    log_text(tr("  * writing/reading simple results (operations=%1  repeats=%2): ").arg(propCnt).arg(repeats));
                    record->disableEmitPropertiesChanged();
                    for (int r=0; r<repeats; r++) {
                        //log_text(tr("    * writing %1 results ... ").arg(propCnt));
                        record->resultsClearAll();
                        timer.start();
                        for (int i=0; i<propCnt; i++) {
                            record->resultsSetNumberError("testeval", QString("testprop%1").arg(i), i, double(i)/10.0, "unit");
                        }
                        #if QT_VERSION >= 0x040800
                            double duration=double(timer.nsecsElapsed())/1.0e6;
                        #else
                            double duration=double(timer.elapsed());
                        #endif
                        durR[r]=duration;
                        durW[r]=duration;
                        //log_text(tr("OK [%1 ms]\n").arg(duration));

                        //log_text(tr("    * reading %1 results ... ").arg(propCnt));
                        timer.start();
                        for (int i=0; i<propCnt; i++) {
                            double ii=record->resultsGetAsDouble("testeval", QString("testprop%1").arg(i));
                            ii=ii+i;
                        }
                        #if QT_VERSION >= 0x040800
                            duration=double(timer.nsecsElapsed())/1.0e6;
                        #else
                            duration=double(timer.elapsed());
                        #endif
                        durR[r]=duration;

                        durR[r]=duration;
                        //log_text(tr("OK [%1 ms]\n").arg(duration));
                        log_text(".");
                        QApplication::processEvents();
                    }
                    log_text("\n");
                    record->enableEmitPropertiesChanged();
                    log_text(tr("    => per write operation ( %1 +/- %2 ) s\n").arg(statisticsAverage(durW, repeats)/double(propCnt)*1000.0).arg(statisticsStdDev(durW, repeats)/double(propCnt)*1000.0));
                    log_text(tr("    => per read operation ( %1 +/- %2 ) s\n").arg(statisticsAverage(durR, repeats)/double(propCnt)*1000.0).arg(statisticsStdDev(durR, repeats)/double(propCnt)*1000.0));
                    QApplication::processEvents();



                    log_text(tr("  * writing/reading array results (operations=%1  repeats=%2): ").arg(propCnt).arg(repeats));
                    record->disableEmitPropertiesChanged();
                    for (int r=0; r<repeats; r++) {
                        //log_text(tr("    * writing %1 results ... ").arg(propCnt));
                        record->resultsClearAll();
                        timer.start();
                        QVector<double> v(propCnt, M_PI);
                        QVector<double> e(propCnt, M_PI/2.0);
                        for (int i=0; i<propCnt; i++) {
                            record->resultsSetNumberErrorList("testeval", QString("testprop%1").arg(i), v, e, "unit");
                        }
                        #if QT_VERSION >= 0x040800
                            double duration=double(timer.nsecsElapsed())/1.0e6;
                        #else
                            double duration=double(timer.elapsed());
                        #endif
                        durW[r]=duration;
                        //log_text(tr("OK [%1 ms]\n").arg(duration));

                        //log_text(tr("    * reading %1 results ... ").arg(propCnt));
                        timer.start();
                        for (int i=0; i<propCnt; i++) {
                            QVector<double> vv=record->resultsGetAsDoubleList("testeval", QString("testprop%1").arg(i));
                            vv.append(1);
                        }
                        #if QT_VERSION >= 0x040800
                            duration=double(timer.nsecsElapsed())/1.0e6;
                        #else
                            duration=double(timer.elapsed());
                        #endif
                        durR[r]=duration;
                        //log_text(tr("OK [%1 ms]\n").arg(duration));
                        log_text(".");
                        QApplication::processEvents();
                    }
                    log_text("\n");
                    record->enableEmitPropertiesChanged();
                    log_text(tr("    => per write operation ( %1 +/- %2 ) s\n").arg(statisticsAverage(durW, repeats)/double(propCnt)*1000.0).arg(statisticsStdDev(durW, repeats)/double(propCnt)*1000.0));
                    log_text(tr("    => per read operation ( %1 +/- %2 ) s\n").arg(statisticsAverage(durR, repeats)/double(propCnt)*1000.0).arg(statisticsStdDev(durR, repeats)/double(propCnt)*1000.0));
                    QApplication::processEvents();


                    log_text(tr("  * writing/reading in array results (operations=%1  repeats=%2): ").arg(propCnt).arg(repeats));
                    record->disableEmitPropertiesChanged();
                    for (int r=0; r<repeats; r++) {
                        //log_text(tr("    * writing %1 results ... ").arg(propCnt));
                        timer.start();
                        for (int i=0; i<propCnt; i++) {
                            record->resultsSetInNumberErrorList("testeval", QString("testprop%1").arg(i), i, double(i), double(i)/10.0, "unit");
                        }
                        double duration=0;
                        #if QT_VERSION >= 0x040800
                            duration=double(timer.nsecsElapsed())/1.0e6;
                        #else
                            duration=double(timer.elapsed());
                        #endif
                        durW[r]=duration;
                        //log_text(tr("OK [%1 ms]\n").arg(duration));

                        //log_text(tr("    * reading %1 results ... ").arg(propCnt));
                        timer.start();
                        for (int i=0; i<propCnt; i++) {
                            double vv=record->resultsGetAsDouble("testeval", QString("testprop%1").arg(i), i);
                            vv=vv+i;
                        }
                        #if QT_VERSION >= 0x040800
                            duration=double(timer.nsecsElapsed())/1.0e6;
                        #else
                            duration=double(timer.elapsed());
                        #endif
                        durR[r]=duration;
                        //log_text(tr("OK [%1 ms]\n").arg(duration));
                        log_text(".");
                        QApplication::processEvents();
                    }
                    log_text("\n");
                    record->enableEmitPropertiesChanged();
                    log_text(tr("    => per write operation ( %1 +/- %2 ) s\n").arg(statisticsAverage(durW, repeats)/double(propCnt)*1000.0).arg(statisticsStdDev(durW, repeats)/double(propCnt)*1000.0));
                    log_text(tr("    => per read operation ( %1 +/- %2 ) s\n").arg(statisticsAverage(durR, repeats)/double(propCnt)*1000.0).arg(statisticsStdDev(durR, repeats)/double(propCnt)*1000.0));
                    QApplication::processEvents();
                //}

                if (edt) {
                    edt->close();
                    edt=NULL;
                }
            }





        } else {
            log_text(tr("ERROR ... could not add record\n"));
        }


        log_text(tr("\n\n-----------------------------------------------------------------\n-- QFProject PERFORMANCE TEST FINISHED\n-----------------------------------------------------------------\n"));
        QApplication::restoreOverrideCursor();
    }
}

void MainWindow::contactAuhtors()
{
    DlgContactAuthors* dlg=new DlgContactAuthors(this);
    dlg->exec();
    delete dlg;
}

void MainWindow::contactMailinglist()
{
    DlgContactAuthors* dlg=new DlgContactAuthors(this, true);
    dlg->exec();
    delete dlg;
}

void MainWindow::openWebpage()
{
    QDesktopServices::openUrl(QUrl(qfInfoWeblink()));
}

QList<QPair<QString, QString> >* MainWindow::getHTMLReplacementList() {
    return &htmlReplaceList;
}

QString MainWindow::getHTMLReplacement(const QString &name)
{
    for (int i=0; i<htmlReplaceList.size(); i++) {
        if (htmlReplaceList[i].first==name) return htmlReplaceList[i].second;
    }
    return QString();
}

void MainWindow::setOrAddHTMLReplacement(const QString &name, const QString &value)
{
    for (int i=0; i<htmlReplaceList.size(); i++) {
        if (htmlReplaceList[i].first==name) {
            htmlReplaceList[i].second=value;
            return;
        }
    }
    htmlReplaceList.append(qMakePair(name, value));
}

void MainWindow::appendOrAddHTMLReplacement(const QString &name, const QString &appendValue, const QString &separator)
{
    for (int i=0; i<htmlReplaceList.size(); i++) {
        if (htmlReplaceList[i].first==name) {
            if (htmlReplaceList[i].second.isEmpty()) htmlReplaceList[i].second=appendValue;
            else  htmlReplaceList[i].second=htmlReplaceList[i].second+separator+appendValue;
            return;
        }
    }
    htmlReplaceList.append(qMakePair(name, appendValue));
}

QMap<QString, QFToolTipsData> MainWindow::getTooltips() const {
    return helpdata.tooltips;
}

QList<QFHelpDirectoryInfo>* MainWindow::getPluginHelpList() {
    return &(helpdata.pluginHelpList);

}

QString MainWindow::getPluginMaybeGlobalSettings(const QString &pluginID, const QString &inifile)
{
    QString ini=getGlobalConfigFileDirectory()+inifile;
    if (!QFile::exists(ini)) ini=getPluginConfigDirectory(pluginID)+inifile;
    if (!QFile::exists(ini)) ini=getPluginAssetsDirectory(pluginID)+inifile;
    if (!QFile::exists(ini)) ini=getConfigFileDirectory()+inifile;
    return ini;
}

MainWindow::ContentsEntry::ContentsEntry()
{
    num.clear();
    header="";
    id="";
    prefix="";
}




void MainWindow::fixFilesPathesInProject()
{
    int replaced=0;
    QMap<QString, QString> pathReplaces;
    if (!project) return;
    log_text(tr("fixing filepaths in raw data records ...\n"));

    QProgressDialog progress(tr("fixing filepaths in raw data records..."), "&Cancel", 0, project->getRawDataCount(), this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(0);
    DlgFixFilepaths* dlg=NULL;
    QString lastDir=QFileInfo(project->getFile()).absolutePath();
    bool canceled=false;
    for (int i=0; i<project->getRawDataCount(); i++) {
        QFRawDataRecord* rdr=project->getRawDataByNum(i);
        if (rdr) {
            progress.setValue(i);

            int oldreplaced=replaced;
            QStringList files=rdr->getFiles();
            for (int j=0; j<files.size(); j++) {
                QString file=files[j];
                if (!QFile::exists(file)) {
                    QFileInfo fi(file);
                    QString oldPath=fi.absolutePath();
                    if (pathReplaces.contains(oldPath) && QFile::exists(pathReplaces.value(oldPath, "./")+"/"+fi.fileName())) {
                        files[j]=pathReplaces.value(oldPath, "./")+"/"+fi.fileName();
                        log_text(tr("%1 (%4): replaced file '%2'\n"
                                    "%1 (%4):            => '%3'\n").arg(j).arg(file).arg(files[j]).arg(replaced));
                        //qDebug()<<replaced<<"replace "<<file<<" \n"
                        //          "     => "<<files[j];
                        replaced++;
                        rdr->setFileName(j, files[j]);
                    } else {
                        if (!dlg) dlg=new DlgFixFilepaths(file, lastDir, &progress);
                        else dlg->init(file, lastDir);
                        dlg->setParent(&progress);
                        dlg->setWindowModality(Qt::WindowModal);

                        int res=dlg->exec();
                        if (res==QDialog::Rejected) {
                            canceled=true;
                            break;
                        } else if (res==QDialog::Accepted) {
                            QString newFile=dlg->getNewFilename();
                            QString newPath=QFileInfo(newFile).absolutePath();
                            lastDir=dlg->getLastDir();
                            pathReplaces[oldPath]=newPath;
                            files[j]=pathReplaces.value(oldPath, "./")+"/"+fi.fileName();
                            log_text(tr("%1 (%4): replaced file '%2'\n"
                                        "%1 (%4):            => '%3'\n").arg(j).arg(file).arg(files[j]).arg(replaced));
                            //qDebug()<<replaced<<"replace "<<file<<" \n"
                            //          "     => "<<files[j];

                            replaced++;
                            rdr->setFileName(j, files[j]);
                        }
                    }
                }
            }

            if (replaced==oldreplaced) {
                log_text(tr("%1: all files OK (no replaces necessary)!\n").arg(rdr->getName()));

            }


            QApplication::processEvents(QEventLoop::AllEvents, 5);
            if (progress.wasCanceled() || canceled)  break;
        }
    }
    if (dlg) delete dlg;
    progress.setValue(project->getRawDataCount());
    QApplication::processEvents();
    QApplication::processEvents();
    QApplication::processEvents();
    progress.close();

    log_text(tr("fixing filepaths in raw data records ... DONE! (replaced %1 filenames)\n").arg(replaced));

    if (replaced>0) {
        if (QMessageBox::information(this, tr("Fix Filenames"), tr("Fixed %1 filenames.\n  You will have to save and reload the project before the changes take full effect. Do that now?").arg(replaced), QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::Yes) {
            log_text(tr("fixing filepaths in raw data records ... RELOADING PROJECT after %1 replaces!").arg(replaced));
            QApplication::processEvents();
            QApplication::processEvents();
            QApplication::processEvents();
            reloadProject();
        }
    }
}

void MainWindow::renameGroups()
{
    if (!project) return;
    RenameGroupsDialog* dlg=new RenameGroupsDialog(this);
    dlg->setStrings(project->getRDRGroupNames());
    if (dlg->exec()) {
        QStringList sl=dlg->getStrings();
        for (int i=0; i<sl.size(); i++) {
            project->setRDRGroupName(i, sl[i]);
        }
    }
    delete dlg;
}

void MainWindow::setRDRPropertyByRegExp()
{
    if (!project) return;
    DlgSetRDRPropertyByRegExp* dlg=new DlgSetRDRPropertyByRegExp(this);
    dlg->setProject(project);
    if (dlg->exec()) {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        QList<QPointer<QFRawDataRecord> > rdrs=dlg->getSelectedRDRs();
        setProgressRange(0, rdrs.size());
        for (int i=0; i<rdrs.size(); i++) {
            if (rdrs[i]) dlg->applyResult(rdrs[i]);
            if (i%10==0) {
                setProgress(i);
                QApplication::processEvents();
            }
        }
        setProgressRange(0, 100);
        setProgress(0);
        QApplication::restoreOverrideCursor();

    }
    delete dlg;
}

void MainWindow::setRDRPropertyByExpression()
{
    if (!project) return;
    DlgSetRDRPropertyByExpression* dlg=new DlgSetRDRPropertyByExpression(this);
    dlg->setProject(project);
    if (dlg->exec()) {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        QList<QPointer<QFRawDataRecord> > rdrs=dlg->getSelectedRDRs();
        QList<DlgSetRDRPropertyByExpressionEditor*> editors=dlg->getEditors();
        setProgressRange(0, rdrs.size()*editors.size());
        for (int j=0; j<editors.size(); j++) {
            for (int i=0; i<rdrs.size(); i++) {
                if (rdrs[i]) dlg->applyResult(editors[j], rdrs[i]);
                if (i%10==0) {
                    setProgress(j*rdrs.size()+i);
                    QApplication::processEvents();
                }
            }
        }
        setProgressRange(0, 100);
        setProgress(0);
        QApplication::restoreOverrideCursor();

    }
    delete dlg;
}

void MainWindow::editUserFitFunctions()
{
    if (!dlgUserFitFunctionEditor) {
        dlgUserFitFunctionEditor=new UserFitFunctionsEditor(NULL);
        connect(dlgUserFitFunctionEditor, SIGNAL(accepted()), this, SLOT(closeUserFitEditor()));
    }
    dlgUserFitFunctionEditor->show();

}

void MainWindow::closeUserFitEditor()
{
    QFFitFunctionManager::getInstance()->reloadUserFitFunctions();
}

void MainWindow::checkUpdates(bool userRequest)
{
    //networkManager.setProxy(settings->getProxy());
    QNetworkProxy proxy;
    ProgramOptionsSetQNetworkProxy(proxy);
    networkManager.setProxy(proxy);

    QUrl url = qfUpdateXMLURL();
    QNetworkRequest request(url);
    //qDebug()<<"request updates from: "<<qfUpdateXMLURL();
    //qDebug()<<"user request "<<userRequest;
    QNetworkReply *reply = networkManager.get(request);
    if (!userRequest) lastUpdateRequest=reply;
    else {
        //qDebug()<<"  user request !";
        lastUpdateRequestUser=reply;
        QModernProgressDialog progress(tr("getting update information ..."), tr("Cancel"), this);
        progress.open();
        QElapsedTimer time;
        time.start();
        while (!reply->atEnd() && time.elapsed()<60000) {
            QApplication::processEvents();
            if (progress.wasCanceled()) {
                lastUpdateRequestUser=NULL;
                reply->abort();
                //delete reply;
            }
            //qDebug()<<"  user request ! "<<time.elapsed();
        }
        progress.hide();
    }
}

void MainWindow::checkUpdatesAutomatic()
{
    checkUpdates(false);
}

void MainWindow::checkCallArguments()
{
    // scan program arguments for a project file (has to be the last argument)
    QStringList args=QApplication::arguments();
    if (args.size()>1) {

        QString lastarg=args[args.size()-1];
        if (QFile::exists(lastarg) && ((QFileInfo(lastarg).suffix()=="qfp")||(QFileInfo(lastarg).suffix()=="qfpz")||(QFileInfo(lastarg).suffix()=="qfp.gz"))) {
            currentProjectDir=QFileInfo(lastarg).dir().absolutePath();
            loadProject(lastarg);
        }
    }
}

void MainWindow::showUpdateInfo(QNetworkReply* reply) {
    if (reply) {
        if (reply->error()==QNetworkReply::NoError) {
            MainWindow::updateInfo info=MainWindow::readUpdateInfo(reply);
            bool ok=false;
            int svn=qfReadFirstInt(qfInfoSVNVersion(), &ok);

            if (info.valid) {
                log_global_text("update info from "+reply->url().toString()+":\n");
                log_global_text("   current version: "+QString::number(svn)+"\n");
                log_global_text("   newest version "+QString::number(info.latestVersion)+":\n");
                log_global_text("      date:         "+info.date+"\n");
                log_global_text("      os:           "+info.os+"\n");
                log_global_text("      description:  "+info.description+"\n");
                log_global_text("      download:     "+info.download+"\n");
                log_global_text("      link:         "+info.link+"\n");
                log_global_text("      releasenotes: "+info.releasenotes+"\n");
                for (int i=0; i<info.warnings.size(); i++) {
                    if (svn<=info.warnings[i].warnSince) log_global_text("      warning (cur. version <= "+QString::number(info.warnings[i].warnSince)+"): "+info.warnings[i].message+"\n");
                }
            }
            if (ok && info.valid && svn<info.latestVersion) {
                if (lastUpdateRequestUser==reply) {
                    QString warning="";
                    if (info.warnings.size()>0) {
                        warning+=tr("\n\nIt is no longer advised to use your current version (SVN%1). Reasons:").arg(svn);
                    }
                    for (int i=0; i<info.warnings.size(); i++) {
                        if (svn<=info.warnings[i].warnSince) {
                            warning+=tr("\n   warning (older than SVN%1): %2").arg(info.warnings[i].warnSince).arg(info.warnings[i].message);
                        }
                    }

                    if (QMessageBox::information(this, tr("QuickFit updates"), tr("new QuickFit 3.0 version (SVN version: %3, date: %2) available:\n\n  %4%5\n\n  Link: %1\n Go to Download site?").arg(info.link).arg(info.date).arg(info.latestVersion).arg(info.description).arg(warning), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes) {
                        QDesktopServices::openUrl(info.link);
                    }
                    lastUpdateRequestUser=NULL;
                } else if (lastUpdateRequest==reply) {
                    QString message=tr("<b>new QuickFit 3.0 version (SVN version: %3, date: %2) available: <a href=\"%1\">go to download</a></b>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<small><a href=\"close_labupdate\">close message</a></small><br>&nbsp;&nbsp;&nbsp;&nbsp;description: <i>%4</i>").arg(info.link).arg(info.date).arg(info.latestVersion).arg(info.description);
                    if (info.warnings.size()>0) {
                        message+=tr("<br>&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"darkred\">It is no longer advised to use your current version (SVN%1). Reasons:</font>").arg(svn);
                    }
                    for (int i=0; i<info.warnings.size(); i++) {
                        if (svn<=info.warnings[i].warnSince) {
                            message+=tr("<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"darkred\">warning (older than %1): <i>%2</i></font>").arg(info.warnings[i].warnSince).arg(info.warnings[i].message);
                        }
                    }
                    labUpgrade->setText(message);
                    labUpgrade->setVisible(true);
                    lastUpdateRequest=NULL;
                }
            } else {
                labUpgrade->setText("");
                labUpgrade->setVisible(false);
                if (lastUpdateRequestUser==reply) {
                    QMessageBox::information(this, tr("QuickFit updates"), tr("no updates available"));
                    lastUpdateRequestUser=NULL;
                } else if (lastUpdateRequest==reply) {
                    lastUpdateRequest=NULL;
                }
            }
        } else {
            log_warning(tr("\n could not acquire update information: %1\n\n").arg(reply->errorString()));
        }
        //delete reply;
    }
}

void MainWindow::openLabelLink(const QString &link)
{
    if (link=="close_labupdate") {
        labUpgrade->setVisible(false);
    } else {
        if (QMessageBox::question(this, tr("open URL"), tr("opne the URL\n   %1\nin the system's main webbrowser?").arg(link), QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::Yes) QDesktopServices::openUrl(link);
    }

}

void MainWindow::clipboardDataChanged()
{
    pastItemAct->setEnabled(clipboardContainsProjectXML() && this->projectModeEnabled);
}

void MainWindow::setProjectSortOrder(QAction *act)
{
    if (project) {
        QList<QAction*> actsProjectSort=actsSort->actions();
        if (actsProjectSort.contains(act)) project->setSortOrder(actsProjectSort.indexOf(act));
    }
}

void MainWindow::setProjectSortOrderActions(int order)
{
    QList<QAction*> actsProjectSort=actsSort->actions();
    if (project && order>=0 && order<actsProjectSort.size()) {
        actsProjectSort[order]->setChecked(true);
    }
}

bool MainWindow::clipboardContainsProjectXML() const
{
     QClipboard *clipboard = QApplication::clipboard();
     const QMimeData* mime=clipboard->mimeData();
     return mime && mime->hasFormat("quickfit3/projectcopyxml");
}

QFRawDataRecordFactory *MainWindow::getRawDataRecordFactory() const
{
    return rawDataFactory;
}

MainWindow::updateInfo MainWindow::readUpdateInfo(QIODevice *io)
{
    MainWindow::updateInfo res;
    res.valid=false;
    QDomDocument xml;
    xml.setContent(io);
    QDomElement e=xml.firstChildElement("updates");
    if (!e.isNull()) {
        e=e.firstChildElement("product");
        if (!e.isNull()) {
            QString pname=e.attribute("name").toLower().trimmed();
            int version=e.attribute("version").toLower().trimmed().toInt();
            if (pname=="quickfit") {
                res.valid=true;
                res.latestVersion=version;
                res.date=e.firstChildElement("date").text();
                res.releasenotes=e.firstChildElement("releasenotes").text();
                res.link=e.firstChildElement("link").text();
                res.description=e.firstChildElement("description").text();
                QString os=getOSShortName();
                QDomElement ee=e.firstChildElement("directlink");
                while (!ee.isNull()) {
                    if (ee.attribute("os").toLower().trimmed()==os) {
                        res.download=ee.text();
                        res.os=ee.attribute("os");
                        break;
                    }
                    ee=ee.nextSiblingElement("directlink");
                }
                ee=e.firstChildElement("warning");
                while (!ee.isNull()) {
                    updateWarning w;
                    w.warnSince=ee.attribute("fromversion").toLower().trimmed().toInt();
                    w.message=ee.text();
                    res.warnings.append(w);
                    ee=ee.nextSiblingElement("warning");
                }
            }
        }
    }
    return res;
}































QString MainWindow::transformQF3HelpHTML(const QString& input_html, const QString& filename, bool removeNonReplaced, const QF3HelpReplacesList& more_replaces, bool insertTooltips, bool dontCreatePics, bool isMainHelp) {
    JKQTmathText mathParser(this);
    mathParser.set_fontSize(ProgramOptions::getConfigValue("quickfit/math_pointsize", 14).toInt());
    mathParser.useXITS();
    mathParser.set_fontSize(ProgramOptions::getConfigValue("quickfit/math_pointsize", 14).toInt());

    QString result=removeHTMLComments(input_html);
    QString fileDir=QFileInfo(filename).absolutePath();
    QString basepath=QFileInfo(filename).absolutePath();
    QDir basedir=QFileInfo(filename).absoluteDir();

    if (!fileDir.endsWith("/")) fileDir.append("/");
    QList<QPair<QString, QString> > fromHTML_replaces;
    QList<QPair<QString, QString> >* replaces=QFPluginServices::getInstance()->getHTMLReplacementList();
    QList<QFHelpDirectoryInfo>* pluginList=QFPluginServices::getInstance()->getPluginHelpList();
    QF3HelpReplacesList localreplaces;
    localreplaces<<qMakePair<QString, QString>("startbox", "<blockquote>"
                                               "<table width=\"100%\" border=\"1\" style=\"background-color: white ;  border-color: midnightblue\" ><tr><td>"
                                               "<table width=\"100%\" border=\"0\" cellspacing=\"0\" cellpadding=\"2\"><tr><td align=\"left\">");
    localreplaces<<qMakePair<QString, QString>("startbox_info", "<blockquote>"
                                               "<table width=\"100%\" border=\"1\" cellspacing=\"0\" cellpadding=\"2\" style=\"background-color: lightcyan ;  border-color: midnightblue\" ><tr><td>"
                                               "<table width=\"100%\" border=\"0\" cellspacing=\"0\" cellpadding=\"2\"><tr><td width=\"35\" align=\"left\"><img src=\":/lib/help/helpboxlogo_info.png\"></td><td align=\"left\">");
    localreplaces<<qMakePair<QString, QString>("startbox_note", "<blockquote>"
                                               "<table width=\"100%\" border=\"1\" cellspacing=\"0\" cellpadding=\"2\" style=\"background-color: lightcyan ;  border-color: midnightblue\" ><tr><td>"
                                               "<table width=\"100%\" border=\"0\" cellspacing=\"0\" cellpadding=\"2\"><tr><td width=\"35\" align=\"left\"><img src=\":/lib/help/helpboxlogo_note.png\"></td><td align=\"left\">");
    localreplaces<<qMakePair<QString, QString>("startbox_see", "<blockquote>"
                                               "<table width=\"100%\" border=\"1\" cellspacing=\"0\" cellpadding=\"2\" style=\"background-color: cornsilk ;  border-color: darkgreen\" ><tr><td>"
                                               "<table width=\"100%\" border=\"0\" cellspacing=\"0\" cellpadding=\"2\"><tr><td width=\"35\" align=\"left\"><img src=\":/lib/help/helpboxlogo_see.png\"></td><td align=\"left\">");
    localreplaces<<qMakePair<QString, QString>("startbox_warning", "<blockquote>"
                                               "<table width=\"100%\" border=\"1\" cellspacing=\"0\" cellpadding=\"2\" style=\"background-color: navajowhite ;  border-color: orangered\" ><tr><td>"
                                               "<table width=\"100%\" border=\"0\" cellspacing=\"0\" cellpadding=\"2\"><tr><td width=\"35\" align=\"left\"><img src=\":/lib/help/helpboxlogo_warning.png\"></td><td align=\"left\">");
    localreplaces<<qMakePair<QString, QString>("startbox_example", "<blockquote>"
                                               "<table width=\"100%\" border=\"1\" cellspacing=\"0\" cellpadding=\"2\" style=\"background-color: lightgrey ;  border-color: midnightblue\" ><tr><td>"
                                               "<table width=\"100%\" border=\"0\" cellspacing=\"0\" cellpadding=\"2\"><tr><td align=\"left\">");
    localreplaces<<qMakePair<QString, QString>("endbox", "</td></tr></table></td></tr></table></blockquote>");
    localreplaces<<qMakePair<QString, QString>("faq_start", "<blockquote>"
                                               "<table width=\"100%\" border=\"1\" cellspacing=\"0\" cellpadding=\"2\" style=\"background-color: white ;  border-color: midnightblue\" >"
                                               "<tr><td align=\"left\"  style=\"background-color: lightgrey\">");
    localreplaces<<qMakePair<QString, QString>("faq_answer", "</td></tr><tr><td>");
    localreplaces<<qMakePair<QString, QString>("faq_end", "</td></tr></table></blockquote>");

    localreplaces<<qMakePair<QString, QString>("funcref_start", "<blockquote>"
                                               "<table width=\"100%\" border=\"1\" cellspacing=\"0\" cellpadding=\"2\" style=\"background-color: white ;  border-color: midnightblue\" >"
                                               "<tr><td align=\"left\"  style=\"background-color: lightgrey\">");
    localreplaces<<qMakePair<QString, QString>("funcref_main", "</td></tr><tr><td>");
    localreplaces<<qMakePair<QString, QString>("funcref_description", "$$funcref_main$$");
    localreplaces<<qMakePair<QString, QString>("funcref_end", "</td></tr></table></blockquote>");
    localreplaces<<qMakePair<QString, QString>("main_fontsize", QString("%1pt").arg(settings->getConfigValue("quickfit/help_pointsize", 11).toInt()));
    localreplaces<<qMakePair<QString, QString>("math_fontsize", QString("%1pt").arg(settings->getConfigValue("quickfit/math_pointsize", 14).toInt()));
    localreplaces<<qMakePair<QString, QString>("main_font", QString("%1").arg(settings->getConfigValue("quickfit/help_font", font().family()).toString()));

    if (QFile::exists(basedir.absoluteFilePath("localreplaces.ini"))) {
        QSettings setLocalReplace(basedir.absoluteFilePath("localreplaces.ini"), QSettings::IniFormat);

        QStringList keys=setLocalReplace.childKeys();
        for (int i=0; i<keys.size(); i++) {
            localreplaces.append(qMakePair(keys[i], setLocalReplace.value(keys[i], "").toString()));
        }
    }




    // find special information in file
    QRegExp rxTitle("<title>(.*)</title>", Qt::CaseInsensitive);
    if (rxTitle.indexIn(result) != -1) fromHTML_replaces.append(qMakePair(QString("title"), rxTitle.cap(1)));

    QRegExp rxMeta("(<meta\\s*content\\s*=\\s*\"([^\"']*)\"\\s*name\\s*=\\s*\"([^\"']*)\"[^>]*>|<meta\\s*name\\s*=\\s*\"([^\"']*)\"\\s*content\\s*=\\s*\"([^\"']*)\"[^>]*>)", Qt::CaseInsensitive);
    rxMeta.setMinimal(false);
    int count = 0;
    int pos = 0;
    while ((pos = rxMeta.indexIn(result, pos)) != -1) {

        QString name=rxMeta.cap(2);
        QString content=rxMeta.cap(1);
        if (name.isEmpty() && content.isEmpty()) {
            name=rxMeta.cap(3);
            content=rxMeta.cap(4);
        }
        if ((!name.isEmpty())&&(!content.isEmpty())) {
            fromHTML_replaces.append(qMakePair(QString("meta_")+name,content));
        }

        ++count;
        pos += rxMeta.matchedLength();
    }

    QRegExp rxLink("<link\\s*rel\\s*=\\s*\"([^\"']*)\"[^\\>]*href\\s*=\\s*\"([^\"']*)\"[^>]*>", Qt::CaseInsensitive);
    rxLink.setMinimal(false);
    count = 0;
    pos = 0;
    while ((pos = rxLink.indexIn(result, pos)) != -1) {

        QString rel=rxLink.cap(1).toLower();
        QString href=rxLink.cap(2);
        if (!href.isEmpty()) {
            if (rel=="contents") fromHTML_replaces.append(qMakePair(QString("rel_contents"), QObject::tr("<a href=\"%1\"><img src=\":/lib/help/help_contents.png\" border=\"0\"><!--Contents--></a> ").arg(href)));
            if (rel=="index") fromHTML_replaces.append(qMakePair(QString("rel_index"), QObject::tr("<a href=\"%1\">Index</a> ").arg(href)));
            if (rel=="copyright") fromHTML_replaces.append(qMakePair(QString("rel_copyright"), QObject::tr("<a href=\"%1\">Copyright</a> ").arg(href)));
            if (rel=="top") fromHTML_replaces.append(qMakePair(QString("rel_top"), QObject::tr("<a href=\"%1\">Top</a> ").arg(href)));
            if (rel=="prev") fromHTML_replaces.append(qMakePair(QString("rel_prev"), QObject::tr("<a href=\"%1\"><img src=\":/lib/help/help_prev.png\" border=\"0\"></a><!--&nbsp;<a href=\"%1\">Previous</a>--> ").arg(href)));
            if (rel=="next") fromHTML_replaces.append(qMakePair(QString("rel_next"), QObject::tr("<a href=\"%1\"><img src=\":/lib/help/help_next.png\" border=\"0\"></a><!--&nbsp;<a href=\"%1\">Next</a>--> ").arg(href)));
            if (rel=="up") fromHTML_replaces.append(qMakePair(QString("rel_up"), QObject::tr("<a href=\"%1\"><img src=\":/lib/help/help_up.png\" border=\"0\"></a><!--&nbsp;<a href=\"%1\">Up</a>--> ").arg(href)));
            //if (rel=="prev") fromHTML_replaces.append(qMakePair(QString("rel_prev"), QObject::tr("<a href=\"%1\"><img src=\":/lib/help/help_prev.png\" border=\"0\"></a>&nbsp;<a href=\"%1\">Previous</a> ").arg(href)));
        }

        ++count;
        pos += rxLink.matchedLength();
    }

    // collectspecial replaces based on current directory
    bool foundPlugin=false;
    if (pluginList) {
        for (int i=0; i<pluginList->size(); i++) {
            if (QDir(pluginList->at(i).directory)==basepath) { // we found the info for this directory
                QString pid=pluginList->at(i).plugin->getID();
                QString pid_sub_deocrated="";
                QString autoplugin_description;
                QString autoplugin_startdescription;
                QString fn=QFileInfo(filename).baseName();
                if (QFFitAlgorithmManager::getInstance()->contains(pid)) {
                    if (QFFitAlgorithmManager::getInstance()->getIDList().contains(fn)) {
                        pid_sub_deocrated=tr("&nbsp;&nbsp;&nbsp;fitAlgorithmID: <b><tt>%1</tt></b>").arg(fn);
                        QFFitAlgorithm* fa=QFFitAlgorithmManager::getInstance()->createAlgorithm(fn);
                        if (fa) {
                            QStringList ffeat;
                            QString fffeatures="";
                            QString params;
                            if (fa->get_supportsBoxConstraints()) {
                                ffeat.append(tr("supports box-constraints"));
                            }
                            if (fa->isThreadSafe()) {
                                ffeat.append(tr("supports multi-threaded fits"));
                            }
                            if (fa->isDeprecated()) {
                                autoplugin_startdescription+=tr("$$note:This fit algorithm is marked as deprecated by the author and may be removed from QuickFit in one of the future versions! Please use an alternative where possible.$$");
                            }
                            if (fa->getParameterIDs().size()>0) {
                                QString p;
                                for (int i=0; i<fa->getParameterIDs().size(); i++) {
                                    p+=tr("<li><tt>%1</tt>, default value: %2 </li>").arg(fa->getParameterIDs().at(i)).arg(fa->getParameter(fa->getParameterIDs().at(i)).toString());
                                }
                                params=tr("This fit algorithm can be configured with these parameters:<ul>%1</ul>").arg(p);
                            }
                            if (!ffeat.isEmpty()) fffeatures=tr("  <li>features: <b>%1</b></li>").arg(ffeat.join(", "));
                            autoplugin_description+=tr("<h2>Fit Algorithm Metadata</h2>"
                                                       "<p>This page describes a QFFitAlgorithm, i.e. a fit algorithm for QuickFit $$version$$:<ul>"
                                                       "  <li>id: <b>%1</b></li>"
                                                       "  <li>name: <b>%2</b></li>"
                                                       "  <li>shortened name: <b>%3</b></li>"
                                                       "%4"
                                                       "</ul>"
                                                       "%5"
                                                       "</p>").arg(fa->id()).arg(fa->name()).arg(fa->shortName()).arg(fffeatures).arg(params);
                            delete fa;
                        }
                    }
                } else  if (QFFitFunctionManager::getInstance()->contains(pid)) {
                    if (QFFitFunctionManager::getInstance()->getIDList().contains(fn)) {
                        pid_sub_deocrated=tr("&nbsp;&nbsp;&nbsp;fitFunctionID: <b><tt>%1</tt></b>").arg(fn);
                        QFFitFunction* ff=QFFitFunctionManager::getInstance()->createFunction(fn);
                        if (ff) {
                            QStringList ffeat;
                            QString fffeatures="";
                            QString params="";
                            if (ff->paramCount()>0) {
                                params=tr("The parameters of this fit function are:<table width=\"95%\" border=\"1\" cellpadding=\"2\" cellspacing=\"0\">"
                                          "<tr bgcolor=\"lightblue\"><th rowspan=\"2\">LABEL [UNIT]</th>"
                                              "<th rowspan=\"2\">ID</th>"
                                              "<th colspan=\"8\">DESCRIPTION</th>"
                                              "</tr><tr bgcolor=\"lightblue\">"
                                              "<th>TYPE</th>"
                                              "<th>FIT</th>"
                                              "<th>EDITABLE</th>"
                                              "<th>ERROR</th>"
                                              "<th>INIT. VALUE</th>"
                                              "<th>INIT. FIX</th>"
                                              "<th>RANGE</th>"
                                              "<th>ABS. RANGE</th>"
                                              "</tr>");
                            }
                            for (int i=0; i<ff->paramCount(); i++) {
                                QFFitFunction::ParameterDescription d=ff->getDescription(i);
                                QString col="";
                                QString type=tr("fit");
                                QString err=tr("editable");
                                if (!d.fit && !d.userEditable) {
                                    col=" bgcolor=\"silver\"";
                                    type=tr("calculated");
                                } else if (!d.fit && d.userEditable) {
                                    type=tr("parmeter");
                                }
                                if (d.displayError==QFFitFunction::DisplayError) {
                                    err=tr("displayed");
                                } else if (d.displayError==QFFitFunction::NoError) {
                                    err=tr("---");
                                }


                                params+=tr("<tr %1><td rowspan=\"2\"><b>%2</b> [%5]</td>"
                                           "<td rowspan=\"2\"><tt><b>%3</b></tt></td>"
                                           "<td colspan=\"8\"><i>%4</i></td>"
                                           "</tr><tr %1>"
                                           "<td>&nbsp;&nbsp;%8</td>"
                                           "<td>%6</td>"
                                           "<td>%7</td>"
                                           "<td>&nbsp;&nbsp;%9</td>"
                                           "<td>&nbsp;&nbsp;%10</td>"
                                           "<td>%11</td>"
                                           "<td>%12 ... %13</td>"
                                           "<td>%14 ... %15%16</td>"
                                           "</tr>").arg(col).arg(d.label).arg(ff->getParameterID(i)).arg(d.name).arg(d.unitLabel)
                                                   .arg(d.fit?QString("<img src=\":/lib/checked.png\">"):QString("<img src=\":/lib/unchecked.png\">"))
                                                   .arg(d.userEditable?QString("<img src=\":/lib/checked.png\">"):QString("<img src=\":/lib/unchecked.png\">"))
                                                   .arg(type).arg(err).arg((d.fit || d.userEditable)?QString::number(d.initialValue):QString(""))
                                                   .arg(d.fit?QString(d.initialFix?QString("<img src=\":/lib/checked.png\">"):QString("<img src=\":/lib/unchecked.png\">")):QString(""))
                                                   .arg(d.minValue).arg(d.maxValue).arg(d.absMinValue).arg(d.absMaxValue).arg((d.comboItems.size()>0)?QString(QString(" (%1)").arg(d.comboItems.join(", "))):QString(""));
                            }
                            if (!params.isEmpty()) {
                                params+=QString("</table><br><br>");
                            }
                            if (ff->get_implementsDerivatives()) {
                                ffeat.append(tr("analytical derivaties"));
                            }
                            if (ff->estimateInitial(NULL, NULL, NULL, 0)) {
                                ffeat.append(tr("estimates initial values"));
                            }
                            if (ff->isDeprecated()) {
                                autoplugin_startdescription+=tr("$$note:This fit function is marked as deprecated by the author and may be removed from QuickFit in one of the future versions! Please use an alternative where possible.$$");
                            }
                            if (!ffeat.isEmpty()) fffeatures=tr("  <li>features: <b>%1</b></li>").arg(ffeat.join(", "));
                            autoplugin_description+=tr("<h2>Fit Function Metadata</h2>"
                                                       "<p>This page describes a QFFitFunction, i.e. a fit function for QuickFit $$version$$:<ul>"
                                                       "  <li>id: <b>%1</b></li>"
                                                       "  <li>name: <b>%2</b></li>"
                                                       "  <li>shortened name: <b>%3</b></li>"
                                                       "%4"
                                                       "</ul>"
                                                       "%5"
                                                       "</p>").arg(ff->id()).arg(ff->name()).arg(ff->shortName()).arg(fffeatures).arg(params);
                            delete ff;
                        }
                    }
                }

                QString pid_decorated=tr("<small><br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[pluginID: <b><tt>%1</tt></b>%2]</small>").arg(pluginList->at(i).plugin->getID()).arg(pid_sub_deocrated);


                fromHTML_replaces.append(qMakePair(QString("local_plugin_icon"), QString("<img src=\"%1\">").arg(pluginList->at(i).plugin->getIconFilename())));
                fromHTML_replaces.append(qMakePair(QString("local_plugin_iconfilename"), pluginList->at(i).plugin->getIconFilename()));
                fromHTML_replaces.append(qMakePair(QString("local_plugin_name"), pluginList->at(i).plugin->getName()));

                fromHTML_replaces.append(qMakePair(QString("local_plugin_id_decorated"), pid_decorated));
                fromHTML_replaces.append(qMakePair(QString("local_plugin_author"), pluginList->at(i).plugin->getAuthor()));
                if (basedir.exists("copyright.html")) {
                    fromHTML_replaces.append(qMakePair(QString("local_plugin_copyright"), QString("<a href=\"copyright.html\">%1</a>").arg(pluginList->at(i).plugin->getCopyright())));
                } else {
                    fromHTML_replaces.append(qMakePair(QString("local_plugin_copyright"), pluginList->at(i).plugin->getCopyright()));
                }

                fromHTML_replaces.append(qMakePair(QString("local_plugin_weblink_url"), pluginList->at(i).plugin->getWeblink()));
                fromHTML_replaces.append(qMakePair(QString("local_plugin_weblink"), QObject::tr("<a href=\"%1\">Plugin Webpage</a>").arg(pluginList->at(i).plugin->getWeblink())));
                fromHTML_replaces.append(qMakePair(QString("local_plugin_id"), pid));
                fromHTML_replaces.append(qMakePair(QString("local_plugin_autodescription"), autoplugin_description));
                fromHTML_replaces.append(qMakePair(QString("local_plugin_autostartdescription"), autoplugin_startdescription));
                fromHTML_replaces.append(qMakePair(QString("local_plugin_dllbasename"), pluginList->at(i).pluginDLLbasename));
                fromHTML_replaces.append(qMakePair(QString("local_plugin_dllsuffix"), pluginList->at(i).pluginDLLSuffix));
                if (QFPluginServices::getInstance()) {
                    fromHTML_replaces.append(qMakePair(QString("local_plugin_assets"), QFPluginServices::getInstance()->getPluginAssetsDirectory(pluginList->at(i).pluginDLLbasename)));
                    fromHTML_replaces.append(qMakePair(QString("local_plugin_help"), QFPluginServices::getInstance()->getPluginHelpDirectory(pluginList->at(i).pluginDLLbasename)));
                    fromHTML_replaces.append(qMakePair(QString("local_plugin_examples"), QFPluginServices::getInstance()->getPluginExamplesDirectory(pluginList->at(i).pluginDLLbasename)));
                    fromHTML_replaces.append(qMakePair(QString("local_plugin_config"), QFPluginServices::getInstance()->getPluginConfigDirectory(pluginList->at(i).pluginDLLbasename)));
                }
                if (!pluginList->at(i).tutorial.isEmpty()) {
                    fromHTML_replaces.append(qMakePair(QString("local_plugin_tutorial_file"), pluginList->at(i).tutorial));
                    fromHTML_replaces.append(qMakePair(QString("local_plugin_tutorial_link"), QObject::tr("$$qf_commondoc_header.separator$$  <a href=\"%1\"><b>Plugin Tutorial</b></a>").arg(pluginList->at(i).tutorial)));
                }
                if (!pluginList->at(i).faq.isEmpty()) {
                    fromHTML_replaces.append(qMakePair(QString("local_plugin_faq_file"), pluginList->at(i).faq));
                    fromHTML_replaces.append(qMakePair(QString("local_plugin_faq_link"), QObject::tr("$$qf_commondoc_header.separator$$  <a href=\"%1\"><b>Plugin FAQ</b></a>").arg(pluginList->at(i).faq)));
                }
                if (!pluginList->at(i).mainhelp.isEmpty()) {
                    fromHTML_replaces.append(qMakePair(QString("local_plugin_mainhelp_file"), pluginList->at(i).mainhelp));
                    fromHTML_replaces.append(qMakePair(QString("local_plugin_mainhelp_link"), QObject::tr("$$qf_commondoc_header.separator$$  <a href=\"%1\"><b>Plugin Help</b></a>").arg(pluginList->at(i).mainhelp)));
                }
                int major=0, minor=0;
                pluginList->at(i).plugin->getVersion(major, minor);
                fromHTML_replaces.append(qMakePair(QString("local_plugin_version"), QString("%1.%2").arg(major).arg(minor)));
                if (!pluginList->at(i).plugintypehelp.isEmpty()) {
                    fromHTML_replaces.append(qMakePair(QString("local_plugin_typehelp_file"), pluginList->at(i).plugintypehelp));
                    fromHTML_replaces.append(qMakePair(QString("local_plugin_typehelp_link"), QObject::tr("$$qf_commondoc_header.separator$$  <a href=\"%1\">%2</a>").arg(pluginList->at(i).plugintypehelp).arg(pluginList->at(i).plugintypename)));
                }
                foundPlugin=true;
                break;
            }
        }

    }

    if (!foundPlugin) {
        fromHTML_replaces.append(qMakePair(QString("local_plugin_icon"), QString("<img src=\":/icon.png\">")));
        fromHTML_replaces.append(qMakePair(QString("local_plugin_iconfilename"), QString(":/icon.png")));
        fromHTML_replaces.append(qMakePair(QString("local_plugin_name"), QObject::tr("QuickFit $$version$$: Online-Help")));
        fromHTML_replaces.append(qMakePair(QString("local_plugin_id_decorated"), QString("")));
        fromHTML_replaces.append(qMakePair(QString("local_plugin_autodescription"), QString("")));
        fromHTML_replaces.append(qMakePair(QString("local_plugin_autostartdescription"), QString("")));
        fromHTML_replaces.append(qMakePair(QString("local_plugin_author"), QString("$$author$$")));
        fromHTML_replaces.append(qMakePair(QString("local_plugin_id"), QString("$$author$$")));
        if (basedir.exists("copyright.html")) {
            fromHTML_replaces.append(qMakePair(QString("local_plugin_copyright"), QString("<a href=\"copyright.html\">$$copyright$$</a>")));
        } else {
            fromHTML_replaces.append(qMakePair(QString("local_plugin_copyright"), QString("$$copyright$$")));
        }
        fromHTML_replaces.append(qMakePair(QString("local_plugin_weblink_url"), QString("$$weblink$$")));
        fromHTML_replaces.append(qMakePair(QString("local_plugin_weblink"), QObject::tr("<a href=\"$$weblink$$\">QuickFit Webpage</a>")));
    }

    // handle replaces, also handles special commands, like $$list:...$$
    if (!result.isEmpty()) {
        QRegExp rxBody("\\<\\s*body([^\\>]*)\\>");
        if (isMainHelp) {
            if (!result.contains("$$qf_commondoc_footer")) {
                result=result.replace(QString("</body>"), QString("<br><br><br><br><br>$$qf_commondoc_footer.start$$ $$qf_commondoc_footer.end$$</body>"));
            }
            /*if (!result.contains("$$qf_commondoc_header")) {
                result=result.replace(QString("<body>"), QString("<body>$$qf_commondoc_header.start$$  $$qf_commondoc_header.end$$"));
            }*/
            if (!result.contains("$$contents")) {
                result=result.replace(QString("$$qf_commondoc_header.end$$"), QString("$$qf_commondoc_header.end$$\n$$contents_full$$"));
            }
        }
        int bidx=-1;
        if ((bidx=rxBody.indexIn(result))>=0) {
            if (isMainHelp && !result.contains("$$qf_commondoc_header")) {
                result=result.replace(bidx, rxBody.matchedLength(), QString("$$qf_css_mainfile_block$$\n%1\n$$qf_commondoc_header.start$$  $$qf_commondoc_header.end$$").arg(rxBody.cap(0)));
                //result=result.replace(QString("<body>"), QString("<body>$$qf_commondoc_header.start$$  $$qf_commondoc_header.end$$"));
            } else {
                result=result.replace(bidx, rxBody.matchedLength(), QString("$$qf_css_mainfile_block$$\n%1").arg(rxBody.cap(0)));
            }
        }


        {
            // insert autolinks
            QStringList ttids=helpdata.autolinks.keys();
            qSort(ttids.begin(), ttids.end(), qfQStringCompareLengthDecreasing);

            for (int ti=0; ti<ttids.size(); ti++){
                const QString key=ttids[ti];
                const QString val=helpdata.autolinks.value(key);
                QRegExp rxTT(QString("\\<\\s*(\\w\\w*)([^\\>]*)\\>[^\\<\\>]*[\\s\\,\\.\\(\\[\\;\\:\\-]+(%1)[\\s\\,\\.\\)\\]\\;\\:\\-]+.*\\<\\s*\\/\\s*([^\\>\\s]*)\\s*\\>").arg(key));
                rxTT.setMinimal(true);
                rxTT.setCaseSensitivity(Qt::CaseInsensitive);
                QRegExp rxTooltip("href\\s*\\=\\s*\\\"tooltip\\:");
                pos = 0;
                //qDebug()<<"-------------------------------- "<<key<<" -------------------------";
                while ((pos = rxTT.indexIn(result, pos)) != -1) {
                    //qDebug()<<rxTT.cap()<<"\n   "<<rxTT.cap(1)<<rxTT.cap(2)<<rxTT.cap(3)<<rxTT.cap(4);
                    QString img=":/lib/help/autolink.png";
                    if (val.toLower().startsWith("http://") || val.toLower().startsWith("https://") || val.toLower().startsWith("ftp://") || val.toLower().startsWith("ftps://")) {
                        img=":/lib/help/autolinkweb.png";
                    }
                    QString tag=rxTT.cap(1).toLower();
                    //qDebug()<<key<<val<<rep<<tag;
                    if (tag!="h1" && tag!="h2" && tag!="h3" && tag!="h4" && tag!="h5" && tag!="title" && tag!="a") {
                        QString rep=QString("%2 <a href=\"%1\"><img src=\"%4\" border=\"0\" alt=\"get more information about %3\"></a>").arg(val).arg(rxTT.cap(3)).arg(key).arg(img);
                        result=result.replace(rxTT.pos(3), rxTT.cap(3).size(), rep);
                        pos += rxTT.matchedLength()-rxTT.cap(3).size()+rep.size();
                        //qDebug()<<"  replaced";
                    } else if (tag=="a" && rxTT.cap(4)=="a" && rxTooltip.indexIn(rxTT.cap(2))>=0) {
                        QString rep=QString("%2 <a href=\"%1\"><img src=\"%4\" border=\"0\" alt=\"get more information about %3\"></a>").arg(val).arg(rxTT.cap()).arg(key).arg(img);
                        result=result.replace(rxTT.pos(), rxTT.matchedLength(), rep);
                        pos += rep.size();
                        //qDebug()<<"  replaced in tooltip";
                    } else {
                        pos += rxTT.matchedLength();
                        //qDebug()<<"  skipped";
                    }
                    //qDebug()<<pos;
                }

            }
        }

        bool replaced=true;
        int cnt=0;
        while (replaced && (cnt<15)) {
            replaced=false;
            if (replaces) for (int i=0; i<replaces->size(); i++) {
                if (result.contains(QString("$$")+(*replaces)[i].first+QString("$$"))) {
                    replaced=true;
                    result=result.replace(QString("$$")+(*replaces)[i].first+QString("$$"), (*replaces)[i].second);
                }
            }
            for (int i=0; i<fromHTML_replaces.size(); i++) {
                if (result.contains(QString("$$")+fromHTML_replaces[i].first+QString("$$"))) {
                    replaced=true;
                    result=result.replace(QString("$$")+fromHTML_replaces[i].first+QString("$$"), fromHTML_replaces[i].second);
                }
            }
            for (int i=0; i<more_replaces.size(); i++) {
                if (result.contains(QString("$$")+more_replaces[i].first+QString("$$"))){
                    replaced=true;
                    result=result.replace(QString("$$")+more_replaces[i].first+QString("$$"), more_replaces[i].second);
                }
            }
            for (int i=0; i<localreplaces.size(); i++) {
                if (result.contains(QString("$$")+localreplaces[i].first+QString("$$"))) {
                    replaced=true;
                    result=result.replace(QString("$$")+localreplaces[i].first+QString("$$"), localreplaces[i].second);
                }
            }


            // interpret $$list:<list_name>:<filter>$$ items
            QRegExp rxList("\\$\\$list\\:([a-z]+)\\:([^\\$\\s]*)\\$\\$", Qt::CaseInsensitive);
            rxList.setMinimal(true);
            int count = 0;
            int pos = 0;
            while ((pos = rxList.indexIn(result, pos)) != -1) {
                QString list=rxList.cap(1).toLower().trimmed();
                QString filter=rxList.cap(2).trimmed();
                //qDebug()<<pos<<list<<filter;
                if (QFPluginServices::getInstance()) {
                    if (list=="fitalg") {
                        QString text="";
                        QString item_template=QString("<li><a href=\"%3\"><img width=\"16\" height=\"16\" src=\"%1\"></a>&nbsp;<a href=\"%3\">%2</a></li>");
                        QString item_template_nolink=QString("<li><img width=\"16\" height=\"16\" src=\"%1\">&nbsp;%2</li>");
                        // gather information about plugins
                        for (int i=0; i<QFPluginServices::getInstance()->getFitAlgorithmManager()->pluginCount(); i++) {
                            int id=i;
                            QString aID="";
                            QStringList fa=QFPluginServices::getInstance()->getFitAlgorithmManager()->getIDList(id);
                            QString dir=QFPluginServices::getInstance()->getFitAlgorithmManager()->getPluginHelp(id);
                            QString name=QFPluginServices::getInstance()->getFitAlgorithmManager()->getName(id);
                            QString icon=QFPluginServices::getInstance()->getFitAlgorithmManager()->getIconFilename(id);
                            for (int j=0; j<fa.size(); j++) {
                                aID=fa[j];
                                QFFitAlgorithm* a=QFPluginServices::getInstance()->getFitAlgorithmManager()->createAlgorithm(aID);
                                if (a) {
                                    name=a->name();
                                    dir=dir=QFPluginServices::getInstance()->getFitAlgorithmManager()->getPluginHelp(id, aID);
                                    delete a;
                                    if (QFile::exists(dir)) text+=item_template.arg(icon).arg(name).arg(dir);
                                    else text+=item_template_nolink.arg(icon).arg(name);
                                }
                            }
                        }
                        if (!text.isEmpty()) {
                            result=result.replace(rxList.cap(0), QString("<ul>")+text+QString("</ul>"));
                        }
                    } else if (list=="fitfunc") {
                        QString text="";
                        QString item_template=QString("<li><a href=\"%3\"><img width=\"16\" height=\"16\" src=\"%1\"></a>&nbsp;<a href=\"%3\">%2</a></li>");
                        QString item_template_nolink=QString("<li><img width=\"16\" height=\"16\" src=\"%1\">&nbsp;%2</li>");
                        // gather information about plugins
                            QMap<QString, QFFitFunction*> models= QFPluginServices::getInstance()->getFitFunctionManager()->getModels(filter);
                            QMapIterator<QString, QFFitFunction*> j(models);
                            while (j.hasNext()) {
                                j.next();
                                int id=QFPluginServices::getInstance()->getFitFunctionManager()->getPluginForID(j.key());
                                QString dir=QFPluginServices::getInstance()->getFitFunctionManager()->getPluginHelp(id);
                                QString name=QFPluginServices::getInstance()->getFitFunctionManager()->getName(id);
                                QString icon=QFPluginServices::getInstance()->getFitFunctionManager()->getIconFilename(id);
                                QFFitFunction* a=j.value();
                                if (a) {
                                    name=a->name();
                                    dir=dir=QFPluginServices::getInstance()->getFitFunctionManager()->getPluginHelp(id, j.key());
                                    delete a;
                                    if (QFile::exists(dir)) text+=item_template.arg(icon).arg(name).arg(dir);
                                    else text+=item_template_nolink.arg(icon).arg(name);
                                }
                            }

                        if (!text.isEmpty()) {
                            result=result.replace(rxList.cap(0), QString("<ul>")+text+QString("</ul>"));
                        }
                    } else if (list=="importers") {
                        QString text="";
                        QString item_template=QString("<li><a href=\"%3\"><img width=\"16\" height=\"16\" src=\"%1\"></a>&nbsp;<a href=\"%3\">%2</a></li>");
                        QString item_template_nolink=QString("<li><img width=\"16\" height=\"16\" src=\"%1\">&nbsp;%2</li>");
                        // gather information about plugins
                            QMap<QString, QFImporter*> models= QFPluginServices::getInstance()->getImporterManager()->createImporters<QFImporter*>(filter);
                            QMapIterator<QString, QFImporter*> j(models);
                            while (j.hasNext()) {
                                j.next();
                                int id=QFPluginServices::getInstance()->getImporterManager()->getPluginForID(j.key());
                                QString dir=QFPluginServices::getInstance()->getImporterManager()->getPluginHelp(id);
                                QString name=QFPluginServices::getInstance()->getImporterManager()->getName(id);
                                QString icon=QFPluginServices::getInstance()->getImporterManager()->getIconFilename(id);
                                QFImporter* a=j.value();
                                if (a) {
                                    name=a->formatName();
                                    dir=dir=QFPluginServices::getInstance()->getImporterManager()->getPluginHelp(id, j.key());
                                    delete a;
                                    if (QFile::exists(dir)) text+=item_template.arg(icon).arg(name).arg(dir);
                                    else text+=item_template_nolink.arg(icon).arg(name);
                                }
                            }

                        if (!text.isEmpty()) {
                            result=result.replace(rxList.cap(0), QString("<ul>")+text+QString("</ul>"));
                        }
                    } else if (list=="imageseriesimporters") {
                        QString text="";
                        QString item_template=QString("<li><a href=\"%3\"><img width=\"16\" height=\"16\" src=\"%1\"></a>&nbsp;<a href=\"%3\">%2</a></li>");
                        QString item_template_nolink=QString("<li><img width=\"16\" height=\"16\" src=\"%1\">&nbsp;%2</li>");
                        // gather information about plugins
                            QMap<QString, QFImporter*> models= QFPluginServices::getInstance()->getImporterManager()->createImporters<QFImporter*>(filter);
                            QMapIterator<QString, QFImporter*> j(models);
                            while (j.hasNext()) {
                                j.next();
                                int id=QFPluginServices::getInstance()->getImporterManager()->getPluginForID(j.key());
                                QString dir=QFPluginServices::getInstance()->getImporterManager()->getPluginHelp(id);
                                QString name=QFPluginServices::getInstance()->getImporterManager()->getName(id);
                                QString icon=QFPluginServices::getInstance()->getImporterManager()->getIconFilename(id);
                                QFImporter* a=j.value();
                                if (a && dynamic_cast<QFImporterImageSeries*>(a)) {
                                    name=a->formatName();
                                    dir=dir=QFPluginServices::getInstance()->getImporterManager()->getPluginHelp(id, j.key());
                                    delete a;
                                    if (QFile::exists(dir)) text+=item_template.arg(icon).arg(name).arg(dir);
                                    else text+=item_template_nolink.arg(icon).arg(name);
                                }
                            }

                        if (!text.isEmpty()) {
                            result=result.replace(rxList.cap(0), QString("<ul>")+text+QString("</ul>"));
                        }
                    } else if (list=="tcspcimporters") {
                        QString text="";
                        QString item_template=QString("<li><a href=\"%3\"><img width=\"16\" height=\"16\" src=\"%1\"></a>&nbsp;<a href=\"%3\">%2</a></li>");
                        QString item_template_nolink=QString("<li><img width=\"16\" height=\"16\" src=\"%1\">&nbsp;%2</li>");
                        // gather information about plugins
                            QMap<QString, QFImporter*> models= QFPluginServices::getInstance()->getImporterManager()->createImporters<QFImporter*>(filter);
                            QMapIterator<QString, QFImporter*> j(models);
                            while (j.hasNext()) {
                                j.next();
                                int id=QFPluginServices::getInstance()->getImporterManager()->getPluginForID(j.key());
                                QString dir=QFPluginServices::getInstance()->getImporterManager()->getPluginHelp(id);
                                QString name=QFPluginServices::getInstance()->getImporterManager()->getName(id);
                                QString icon=QFPluginServices::getInstance()->getImporterManager()->getIconFilename(id);
                                QFImporter* a=j.value();
                                if (a && dynamic_cast<QFTCSPCReader*>(a)) {
                                    name=a->formatName();
                                    dir=dir=QFPluginServices::getInstance()->getImporterManager()->getPluginHelp(id, j.key());
                                    delete a;
                                    if (QFile::exists(dir)) text+=item_template.arg(icon).arg(name).arg(dir);
                                    else text+=item_template_nolink.arg(icon).arg(name);
                                }
                            }

                        if (!text.isEmpty()) {
                            result=result.replace(rxList.cap(0), QString("<ul>")+text+QString("</ul>"));
                        }
                    } else if (list=="extension") {
                        QString text="";
                        QString item_template=QString("<li><a href=\"%3\"><img width=\"16\" height=\"16\" src=\"%1\"></a>&nbsp;<a href=\"%3\">%2</a></li>");
                        QString item_template_nolink=QString("<li><img width=\"16\" height=\"16\" src=\"%1\">&nbsp;%2</li>");
                        // gather information about plugins
                        QStringList ids=QFPluginServices::getInstance()->getExtensionManager()->getIDList();
                        for (int i=0; i<ids.size(); i++) {
                            QString id=ids[i];
                            QString dir=QFPluginServices::getInstance()->getExtensionManager()->getPluginHelp(id);
                            QString name=QFPluginServices::getInstance()->getExtensionManager()->getName(id);
                            QString icon=QFPluginServices::getInstance()->getExtensionManager()->getIconFilename(id);
                            if (filter.isEmpty() || ((!filter.isEmpty()) && (QFPluginServices::getInstance()->getExtensionManager()->getQObjectInstance(id)->inherits(filter.toAscii().data())))) {
                                if (QFile::exists(dir)) text+=item_template.arg(icon).arg(name).arg(dir);
                                else text+=item_template_nolink.arg(icon).arg(name);
                            }
                        }
                        if (!text.isEmpty()) {
                            result=result.replace(rxList.cap(0), QString("<ul>")+text+QString("</ul>"));
                        }
                    } else if (list=="colorpalettes") {
                        QString text="";
                        QString item_template=QString("<tr><td><img alt=\"%1\" src=\"%2\"></td><td><b>%1</b></td></tr></li>\n");
                        QStringList ttids=JKQTPimagePlot_getPredefinedPalettes();
                        for (int ti=0; ti<ttids.size(); ti++){
                            QString id=ttids[ti];
                            QImage pix=JKQTPMathImageGetPaletteImage(ti, 128,16);
                            QString palfilename=QDir::tempPath()+"/qf3help_"+QFileInfo(filename).baseName()+"_pal"+QString::number(ti)+".png";
                            //qDebug()<<"latex-render: "<<latex<<"\n    size = "<<size<<"  output = "<<texfilename;
                            pix.save(palfilename);
                            text+=item_template.arg(id).arg(palfilename);
                        }
                        if (!text.isEmpty()) {
                            result=result.replace(rxList.cap(0), QString("<table border=\"0\" spacing=\"1\" padding=\"1\">\n")+text+QString("</table>"));
                        }
                    } else if (list=="tooltips") {
                        QString text="";
                        QString item_template=QString("<tr><td><b>%1</b></td><td>%2</td></tr></li>\n");
                        QStringList ttids=helpdata.tooltips.keys();
                        qSort(ttids.begin(), ttids.end(), qfQStringCompareLengthDecreasing);
                        for (int ti=0; ti<ttids.size(); ti++){
                            QString id=ttids[ti];
                            QString tt=helpdata.tooltips.value(ttids[ti]).tooltip;
                            text+=item_template.arg(id).arg(tt);
                        }
                        if (!text.isEmpty()) {
                            result=result.replace(rxList.cap(0), QString("<table border=\"1\" spacing=\"1\" padding=\"1\">\n")+text+QString("</table>"));
                        }
                    } else if (list=="autolinks") {
                        QString text="";
                        QString item_template=QString("<tr><td><b>%1</b></td><td><a href=\"%2\">%2</a></td></tr></li>\n");
                        QStringList ttids=helpdata.autolinks.keys();
                        qSort(ttids.begin(), ttids.end(), qfQStringCompareLengthDecreasing);
                        for (int ti=0; ti<ttids.size(); ti++){
                            QString id=ttids[ti];
                            QString lnk=helpdata.autolinks.value(ttids[ti]);
                            text+=item_template.arg(id).arg(lnk);
                        }
                        if (!text.isEmpty()) {
                            result=result.replace(rxList.cap(0), QString("<table border=\"1\" spacing=\"1\" padding=\"1\">\n")+text+QString("</table>"));
                        }
                    }
                }

                ++count;
                pos += rxList.matchedLength();
            }




            // interpret $$insert:<filename>$$ and $$insertglobal:<filename>$$ items
            QRegExp rxInsert("\\$\\$(insert|insertglobal|tooltip|see|note|info|warning|example|codeexample|cexample|tt|code|bqtt|bqcode|startbox|main_fontsize)\\:([^\\$]*)\\$\\$", Qt::CaseInsensitive);
            rxInsert.setMinimal(true);
            count = 0;
            pos = 0;
            while ((pos = rxInsert.indexIn(result, pos)) != -1) {
                QString command=rxInsert.cap(1).toLower().trimmed();
                QString file=rxInsert.cap(2).trimmed();
                //qDebug()<<pos<<list<<filter;

                if (command=="insert") {
                    //qDebug()<<QFileInfo(filename).absoluteDir().absoluteFilePath(file);
                    QFile f(QFileInfo(filename).absoluteDir().absoluteFilePath(file));
                    QString rep="";
                    if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
                        rep=f.readAll();
                    }
                    result=result.replace(rxInsert.cap(0), rep);
                } else if (QFPluginServices::getInstance()&&(command=="insertglobal")) {
                    //qDebug()<<QDir(QFPluginServices::getInstance()->getAssetsDirectory()+"/help/").absoluteFilePath(file);
                    QFile f(QDir(QFPluginServices::getInstance()->getAssetsDirectory()+"/help/").absoluteFilePath(file));
                    QString rep="";
                    if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
                        rep=f.readAll();
                    }
                    result=result.replace(rxInsert.cap(0), rep);
                } else if (QFPluginServices::getInstance()&&(command=="tooltip")) {
                    QString rep=helpdata.tooltips.value(file).tooltip;
                    result=result.replace(rxInsert.cap(0), rep);
                } else if (QFPluginServices::getInstance()&&(command=="see")) {
                    QString rep=tr("<blockquote>"
                                   "<table width=\"100%\" border=\"1\" cellspacing=\"0\" cellpadding=\"2\" style=\"background-color: cornsilk ;  border-color: darkgreen; border-style: solid;\" ><tr><td>"
                                   "<table width=\"100%\" border=\"0\" cellspacing=\"0\" cellpadding=\"2\"><tr><td width=\"35\" align=\"left\"><img src=\":/lib/help/helpboxlogo_see.png\"></td><td align=\"left\"><b>See:</b> %1</td></tr>"
                                   "</table></td></tr></table></blockquote>").arg(file);

                    result=result.replace(rxInsert.cap(0), rep);
                } else if (QFPluginServices::getInstance()&&(command=="note")) {
                    QString rep=tr("<blockquote>"
                                     "<table width=\"100%\" border=\"1\" cellspacing=\"0\" cellpadding=\"2\" style=\"background-color: lightcyan ;  border-color: midnightblue\" ><tr><td>"
                                   "<table width=\"100%\" border=\"0\" cellspacing=\"0\" cellpadding=\"2\"><tr><td width=\"35\" align=\"left\"><img src=\":/lib/help/helpboxlogo_note.png\"></td><td align=\"left\"><b>Note:</b> %1</td></tr>"
                                   "</table></td></tr></table></blockquote>").arg(file);

                    result=result.replace(rxInsert.cap(0), rep);
                } else if (QFPluginServices::getInstance()&&(command=="info")) {
                    QString rep=tr("<blockquote>"
                                     "<table width=\"100%\" border=\"1\" cellspacing=\"0\" cellpadding=\"2\" style=\"background-color: lightcyan ;  border-color: midnightblue\" ><tr><td>"
                                   "<table width=\"100%\" border=\"0\" cellspacing=\"0\" cellpadding=\"2\"><tr><td width=\"35\" align=\"left\"><img src=\":/lib/help/helpboxlogo_info.png\"></td><td align=\"left\"><b>Information:</b> %1</td></tr>"
                                   "</table></td></tr></table></blockquote>").arg(file);

                    result=result.replace(rxInsert.cap(0), rep);
                 } else if (QFPluginServices::getInstance()&&(command=="warning")) {
                    QString rep=tr("<blockquote>"
                                     "<table width=\"100%\" border=\"1\" cellspacing=\"0\" cellpadding=\"2\" style=\"background-color: navajowhite ;  border-color: orangered\" ><tr><td>"
                                   "<table width=\"100%\" border=\"0\" cellspacing=\"0\" cellpadding=\"2\"><tr><td width=\"35\" align=\"left\"><img src=\":/lib/help/helpboxlogo_warning.png\"></td><td align=\"left\"><b>Warning:</b> %1</td></tr>"
                                   "</table></td></tr></table></blockquote>").arg(file);

                    result=result.replace(rxInsert.cap(0), rep);
                } else if (QFPluginServices::getInstance()&&(command=="example")) {
                   QString rep=tr("<blockquote>"
                                    "<table width=\"100%\" border=\"1\" cellspacing=\"0\" cellpadding=\"2\" style=\"background-color: lightgrey ;  border-color: midnightblue\" ><tr><td>"
                                  "<table width=\"100%\" border=\"0\" cellspacing=\"0\" cellpadding=\"2\"><tr><td align=\"left\">%1</td></tr>"
                                  "</table></td></tr></table></blockquote>").arg(file);

                   result=result.replace(rxInsert.cap(0), rep);
                } else if (QFPluginServices::getInstance()&&(command=="cexample"||command=="codeexample")) {
                   QString rep=tr("<blockquote>"
                                    "<table width=\"100%\" border=\"1\" cellspacing=\"0\" cellpadding=\"2\" style=\"background-color: lightgrey ;  border-color: midnightblue\" ><tr><td>"
                                  "<table width=\"100%\" border=\"0\" cellspacing=\"0\" cellpadding=\"2\"><tr><td align=\"left\"><tt>%1</tt></td></tr>"
                                  "</table></td></tr></table></blockquote>").arg(file);

                   result=result.replace(rxInsert.cap(0), rep);
                } else if (QFPluginServices::getInstance()&&(command=="tt"||command=="code")) {
                   QString rep=tr("<tt>%1</tt>").arg(file);

                   result=result.replace(rxInsert.cap(0), rep);
                } else if (QFPluginServices::getInstance()&&(command=="bqtt"||command=="bqcode")) {
                   QString rep=tr("<blockquote><tt>%1</tt></blockquote>").arg(file);

                   result=result.replace(rxInsert.cap(0), rep);
                } else if (QFPluginServices::getInstance()&&(command=="main_fontsize")) {
                   QString rep=QString("%1pt").arg(settings->getConfigValue("quickfit/help_pointsize", 11).toInt()+file.toInt());
                   result=result.replace(rxInsert.cap(0), rep);
                }
                ++count;
                pos += rxInsert.matchedLength();
            }




            // interpret $$math:<latex>$$ items
            QRegExp rxLaTeX("\\$\\$(math|bmath|mathb)\\:(.*)\\$\\$", Qt::CaseInsensitive);
            rxLaTeX.setMinimal(true);
            count = 0;
            pos = 0;
            while ((pos = rxLaTeX.indexIn(result, pos)) != -1) {
                QString command=rxLaTeX.cap(1).toLower().trimmed();
                QString latex="$"+rxLaTeX.cap(2).trimmed()+"$";

                if (command=="math" || command=="bmath" || command=="mathb") {
                    if (dontCreatePics)  {
                        if (command=="bmath" || command=="mathb") {
                            result=result.replace(rxLaTeX.cap(0), QString("<blockquote><font size=\"+2\"><tt><i>%1</i></tt></font></blockquote>").arg(Qt::escape(latex)));
                        } else {
                            result=result.replace(rxLaTeX.cap(0), QString("<tt><i>%1</i></tt>").arg(Qt::escape(latex)));
                        }

                    } else {
                        QImage pix(300,100,QImage::Format_ARGB32_Premultiplied);
                        QPainter p(&pix);
                        p.setRenderHint(QPainter::Antialiasing);
                        p.setRenderHint(QPainter::HighQualityAntialiasing);
                        p.setRenderHint(QPainter::TextAntialiasing);
                        mathParser.parse(latex);
                        bool ok=false;
                        QString ht=mathParser.toHtml(&ok);
                        if (ok) {
                            if (command=="bmath" || command=="mathb") {
                                result=result.replace(rxLaTeX.cap(0), QString("<blockquote><font size=\"+2\" face=\"%2\"><i>%1</i></font></blockquote>").arg(ht).arg(mathParser.get_fontRoman()));
                            } else {
                                result=result.replace(rxLaTeX.cap(0), QString("<font face=\"%2\"><i>%1</i></font>").arg(ht).arg(mathParser.get_fontRoman()));
                            }
                        } else {
                            QSizeF size=mathParser.getSize(p);
                            p.end();
                            pix=QImage(size.width()*1.2, size.height()*1.2, QImage::Format_ARGB32_Premultiplied);
                            pix.fill(Qt::transparent);
                            p.begin(&pix);
                            p.setRenderHint(QPainter::Antialiasing);
                            p.setRenderHint(QPainter::HighQualityAntialiasing);
                            p.setRenderHint(QPainter::TextAntialiasing);
                            mathParser.draw(p,Qt::AlignTop | Qt::AlignLeft, QRectF(QPointF(0,0.1*size.height()), size));
                            p.end();
                            QString texfilename=QDir::tempPath()+"/qf3help_"+QFileInfo(filename).baseName()+"_tex"+QString::number(count)+".png";
                            //qDebug()<<"latex-render: "<<latex<<"\n    size = "<<size<<"  output = "<<texfilename;
                            pix=cropTopBottom(cropLeftRight(pix));
                            pix.save(texfilename);

                            if (command=="bmath" || command=="mathb") {
                                result=result.replace(rxLaTeX.cap(0), QString("<blockquote><img style=\"vertical-align: middle;\" alt=\"%1\" src=\"%2\"></blockquote>").arg(latex).arg(texfilename));
                            } else {
                                result=result.replace(rxLaTeX.cap(0), QString("<img style=\"vertical-align: middle;\" alt=\"%1\" src=\"%2\">").arg(latex).arg(texfilename));
                            }
                        }
                    }

                }

                ++count;
                pos += rxLaTeX.matchedLength();
            }



            // interpret $$plugin_info:<name>:<id>$$, $$fig:file:caption$$,  $$figure:file:caption$$ items, etc.
            QRegExp rxPluginInfo("\\$\\$(plugin_info|fig|figure|startbox|fitfunction|fitalgorithm|importer)\\:([^\\$]*)\\:([^\\$]*)\\$\\$", Qt::CaseInsensitive);
            rxPluginInfo.setMinimal(true);
            count = 0;
            pos = 0;
            while ((pos = rxPluginInfo.indexIn(result, pos)) != -1) {
                QString command=rxPluginInfo.cap(1).toLower().trimmed();
                QString param1=rxPluginInfo.cap(2).toLower().trimmed();
                QString param2=rxPluginInfo.cap(3).trimmed().toLower();

                if (QFPluginServices::getInstance() && command=="plugin_info") {
                    if (param1=="help") result=result.replace(rxPluginInfo.cap(0), QFPluginServices::getInstance()->getPluginHelp(param2));
                    if (param1=="tutorial") result=result.replace(rxPluginInfo.cap(0), QFPluginServices::getInstance()->getPluginTutorial(param2));
                    if (param1=="faq") result=result.replace(rxPluginInfo.cap(0), QFPluginServices::getInstance()->getPluginFAQ(param2));
                    if (param1=="helpdir") result=result.replace(rxPluginInfo.cap(0), QFPluginServices::getInstance()->getPluginHelpDirectory(param2));
                    if (param1=="assetsdir") result=result.replace(rxPluginInfo.cap(0), QFPluginServices::getInstance()->getPluginAssetsDirectory(param2));
                    if (param1=="configdir") result=result.replace(rxPluginInfo.cap(0), QFPluginServices::getInstance()->getPluginConfigDirectory(param2));
                    if (param1=="examplesdir") result=result.replace(rxPluginInfo.cap(0), QFPluginServices::getInstance()->getPluginExamplesDirectory(param2));
                } else if (QFPluginServices::getInstance() && command=="fitfunction") {
                    if (param1=="help") result=result.replace(rxPluginInfo.cap(0), QFPluginServices::getInstance()->getFitFunctionHelp(param2));
                    else if (param1=="name") {
                        QFFitFunction* f=QFPluginServices::getInstance()->getFitFunctionManager()->createFunction(param2);
                        QString name="";
                        if (f) {
                            name=f->name();
                            delete f;
                        }
                        result=result.replace(rxPluginInfo.cap(0), name);
                    }
                    else if (param1=="short_name") {
                        QFFitFunction* f=QFPluginServices::getInstance()->getFitFunctionManager()->createFunction(param2);
                        QString name="";
                        if (f) {
                            name=f->shortName();
                            delete f;
                        }
                        result=result.replace(rxPluginInfo.cap(0), name);
                    }
                } else if (QFPluginServices::getInstance() && command=="fitalgorithm") {
                    if (param1=="help") result=result.replace(rxPluginInfo.cap(0), QFPluginServices::getInstance()->getFitAlgorithmHelp(param2));
                    else if (param1=="name") {
                        QFFitAlgorithm* f=QFPluginServices::getInstance()->getFitAlgorithmManager()->createAlgorithm(param2);
                        QString name="";
                        if (f) {
                            name=f->name();
                            delete f;
                        }
                        result=result.replace(rxPluginInfo.cap(0), name);
                    }
                    else if (param1=="short_name") {
                        QFFitAlgorithm* f=QFPluginServices::getInstance()->getFitAlgorithmManager()->createAlgorithm(param2);
                        QString name="";
                        if (f) {
                            name=f->shortName();
                            delete f;
                        }
                        result=result.replace(rxPluginInfo.cap(0), name);
                    }
                } else if (QFPluginServices::getInstance() && command=="importer") {
                    if (param1=="help") result=result.replace(rxPluginInfo.cap(0), QFPluginServices::getInstance()->getImporterHelp(param2));
                    else if (param1=="name" || param1=="short_name") {
                        QFImporter* f=QFPluginServices::getInstance()->getImporterManager()->createImporter(param2);
                        QString name="";
                        if (f) {
                            name=f->formatName();
                            delete f;
                        }
                        result=result.replace(rxPluginInfo.cap(0), name);
                    }
               } else if (command=="fig" || command=="figure") {
                    QString rep=tr("<center>"
                                     "<img src=\"%1\"><br><i>%2</i><br>"
                                   "</center>").arg(param1).arg(param2);
                    result=result.replace(rxInsert.cap(0), rep);
                } else if (command=="startbox") {
                    QString rep=tr("<blockquote>"
                                     "<table width=\"100%\" border=\"1\" cellspacing=\"0\" cellpadding=\"2\" style=\"background-color: %1 ;  border-color: %2\" >"
                                   "<tr><td align=\"left\">").arg(param1).arg(param2);

                    result=result.replace(rxInsert.cap(0), rep);
                }

                ++count;
                pos += rxPluginInfo.matchedLength();
            }



            cnt++;
        }



        if (isMainHelp) {


            // extract table of contents from header tags
            QRegExp rxHeader("<\\s*h([123456789])([^>]*)>(.*)<\\/\\s*h\\1\\s*>", Qt::CaseInsensitive);
            rxHeader.setMinimal(true);
            count = 0;
            pos = 0;
            int minHeaderLevel=0;
            QList<ContentsEntry> contents;
            bool first=true;
            while ((pos = rxHeader.indexIn(result, pos)) != -1) {

                int level=rxHeader.cap(1).toInt();
                QString text=rxHeader.cap(3);
                QString hres=rxHeader.cap(2);
                //qDebug()<<text<<hres;

                if (!hres.toLower().contains("id=\"title\"") && !text.contains("<!-- title -->")) {
                    ContentsEntry h;
                    if (contents.size()>0) h=contents.value(contents.size()-1);

                    h.header=text;
                    if (first) {
                        for (int i=1; i<=level; i++) {
                            h.num.append(1);
                        }
                    } else {
                        if (level==h.num.size()) {
                            h.num[h.num.size()-1]=h.num[h.num.size()-1]+1;
                        } else if (level>h.num.size()) {
                            for (int i=h.num.size(); i<level; i++) {
                                h.num.append(1);
                            }
                        } else if (level<h.num.size()) {
                            for (int i=h.num.size(); i>=level; i--) {
                                h.num.removeAt(i);
                            }
                            h.num[h.num.size()-1]=h.num[h.num.size()-1]+1;
                        }
                    }

                    QString id="";
                    QString tid="";
                    for (int i=0; i<h.num.size(); i++) {
                        if (!id.isEmpty()) id+="_";
                        if (!tid.isEmpty()) tid+=".";
                        id+=QString::number(h.num[i]);
                        tid+=QString::number(h.num[i]);
                    }
                    if (first) {
                        minHeaderLevel=h.num.size();
                    } else if (h.num.size()<minHeaderLevel) {
                        minHeaderLevel=h.num.size();
                    }
                    //qDebug()<<h.num.size()<<"(min: "<<minHeaderLevel<<")"<<tid<<h.header;
                    first=false;
                    h.id=id;
                    h.prefix=tid;

                    contents.append(h);
                    ++count;
                }
                pos += rxHeader.matchedLength();

            }
            // improve header tags
            QRegExp rxHeader1("(<\\s*h[123456789].*)>", Qt::CaseInsensitive);
            rxHeader1.setMinimal(true);
            count = 0;
            pos = 0;
            while ((pos = rxHeader.indexIn(result, pos)) != -1) {

                int level=rxHeader.cap(1).toInt();
                QString text=rxHeader.cap(3);
                QString hres=rxHeader.cap(2);
                //qDebug()<<text<<hres;

                if (!hres.toLower().contains("id=\"title\"") && !text.contains("<!-- title -->")) {
                    QString prefix="";
                    for (int j=minHeaderLevel-1; j<contents[count].num.size(); j++) {
                        if (!prefix.isEmpty()) prefix+=".";
                        prefix+=QString::number(contents[count].num[j]);
                    }
                    contents[count].prefix=prefix;
                    QString header=contents[count].header;
                    QString newmatch=rxHeader.cap(0);
                    newmatch=newmatch.replace(header, QString("<i>")+contents[count].prefix+QString(".</i>&nbsp;&nbsp;&nbsp;")+header);
                    QString insert=QString("<a name=\"%1\">%2").arg(contents[count].id).arg(newmatch);

                    //qDebug()<<rxHeader.cap(0)<<header<<newmatch;

                    if (count>0 && minHeaderLevel==contents[count].num.size()) {
                        insert=QString("$$qf_commondoc_backtop$$<br><br><br>")+insert;
                    }

                    /*if (rxHeader1.indexIn(insert, 0)!=-1) {
                        insert=insert.replace(rxHeader1.cap(1), rxHeader1.cap(1)+QString(" style=\"background-color: azure;\" "));
                    }*/

                    result=result.replace(rxHeader.cap(0), insert);

                    ++count;
                    pos += insert.size();
                } else {
                    pos += rxHeader.matchedLength();
                }

            }
            // compute table of contents
            QString contentsHTML;
            if (contents.size()>0) {
                contentsHTML="<div style=\"background-color: azure;  border-color: midnightblue; border-style: solid; padding-top:5px; padding-left:5px; padding-right:5px; padding-bottom:5px; margin: 5px;\"><a name=\"table_of_contents\"><b>Table of Contents:</b><br>";
                for (int i=0; i<contents.size(); i++) {
                    QString spaces="";
                    QString prefix="";
                    for (int j=minHeaderLevel-1; j<contents[i].num.size(); j++) {
                        spaces+="&nbsp;&nbsp;&nbsp;";
                        if (!prefix.isEmpty()) prefix+=".";
                        prefix+=QString::number(contents[i].num[j]);
                    }
                    contentsHTML+=QString("%4<a href=\"#%1\">%2 %3</a><br>").arg(contents[i].id).arg(contents[i].prefix).arg(contents[i].header).arg(spaces);
                }
                contentsHTML+="</div>";
                //qDebug()<<contentsHTML;
                //fromHTML_replaces.append(qMakePair(QString("contents"), contentsHTML));
            }


            // extract references tags $$ref:<ID>:Text$$
            QStringList refList;
            QMap<QString, int> refIDMap;
            QRegExp rxRef("\\$\\$(invisibleref|ref)\\:(\\w*)\\:([^\\$]*)\\$\\$", Qt::CaseInsensitive);
            rxRef.setMinimal(true);
            count = 0;
            pos = 0;
            while ((pos = rxRef.indexIn(result, pos)) != -1) {

                QString inst=rxRef.cap(1).toLower();
                QString ID=rxRef.cap(2).toLower();
                QString ref=rxRef.cap(3);
                int refNum=-1;

                if (!ID.isEmpty()) {
                    if (refIDMap.contains(ID)) {
                        refNum=refIDMap[ID];
                        ref=refList.value(refNum, ref);
                    } else {
                        refList.append(ref);
                        refNum=refIDMap[ID]=refList.size()-1;
                    }
                } else {
                    refList.append(ref);
                    refNum=refList.size()-1;
                }

                QString rep=QString("<a href=\"#ref%1\">[%2]</a>").arg(refNum+1).arg(refNum+1);
                if (inst=="invisibleref") {
                    rep="";
                }

                result=result.replace(rxRef.cap(0), rep);

                ++count;
                pos += rep.size();

            }
            QString referencesHTML="";
            for (int i=0; i<refList.size(); i++) {
                referencesHTML=referencesHTML+QString("<li><a name=\"ref%1\">%2</li>\n").arg(i+1).arg(refList[i]);
            }
            if (refList.size()>0) {
                referencesHTML=QString("<ol>\n%1</ol>").arg(referencesHTML);
            }
            //fromHTML_replaces.append(qMakePair(QString("references"), referencesHTML));

            if (!result.contains("$$no_contents$$")) {
                result=result.replace("$$contents$$", contentsHTML);
                QString contentsFull=contentsHTML;
                if (!contentsFull.isEmpty()) contentsFull=tr("<br><hr><br>%1<br><hr><br>").arg(contentsFull);
                result=result.replace("$$contents_full$$", contentsFull);
            }
            if (!result.contains("$$references$$") && referencesHTML.size()>0) {
                result=result.replace("$$DEFAULTREF$$", "<h2>References</h2>\n<p>$$references$$\n</p>");
            }
            result=result.replace("$$references$$", referencesHTML);

        }


        // remove all unreplaces $$name$$ sequences
        if (removeNonReplaced) {
            QRegExp rxSpecials("\\$\\$.+\\$\\$");
            rxSpecials.setMinimal(true);
            result=result.remove(rxSpecials);
        }

        // make all image pathes absolute, WORKAROUND FOR PROBLEM IN Qt4.8.0
        QRegExp rxImages("<img\\s.*src\\s*=\\s*\\\"([^\\\"]*)\\\".*>");
        rxImages.setMinimal(true);
        rxImages.setCaseSensitivity(Qt::CaseInsensitive);
        pos = 0;
        while ((pos = rxImages.indexIn(result, pos)) != -1) {
            QString file=rxImages.cap(1).trimmed();
            QString old=QString("\"%1\"").arg(file);
            QString news=QString("\"%1%2\"").arg(fileDir).arg(file);
            if (QFileInfo(file).isRelative()) {
                result.replace(old, news);
            }
            pos += rxImages.matchedLength()+(news.size()-old.size());
        }


        if (insertTooltips) {
            // insert tooltips: search all occurences of the tooltip keywords that are not inside a tag (i.e. surrounded by a closing tag on
            // the left and an opening tag on the right) and where the tag is not something special (like headers or links).
            QMapIterator<QString, QFToolTipsData> itTT(helpdata.tooltips);
            QStringList ttids=helpdata.tooltips.keys();
            qSort(ttids.begin(), ttids.end(), qfQStringCompareLengthDecreasing);

            for (int ti=0; ti<ttids.size(); ti++){
                const QString key=ttids[ti];
                const QFToolTipsData val=helpdata.tooltips.value(key);
                QRegExp rxTT(QString("\\<\\s*(\\w\\w*)[^\\>]*\\>[^\\<\\>]*[\\s\\,\\.\\(\\[\\;\\:\\-]+(%1)[\\s\\,\\.\\)\\]\\;\\:\\-]+[^\\<\\>]*\\<").arg(key));
                rxTT.setMinimal(true);
                pos = 0;
                //qDebug()<<"----- "<<key<<" ------";
                while ((pos = rxTT.indexIn(result, pos)) != -1) {
                    //qDebug()<<rxTT.cap()<<rxTT.cap(1)<<rxTT.cap(2);
                    QString rep=QString("<a href=\"tooltip:%1\">%2 <img src=\":/lib/help/tooltip.png\" border=\"0\" width=\"12\" height=\"12\" alt=\"get more information about %2\"></a>").arg(key).arg(rxTT.cap(2));
                    QString tag=rxTT.cap(1).toLower();
                    if (tag!="h1" && tag!="h2" && tag!="h3" && tag!="h4" && tag!="h5" && tag!="title" && tag!="a" && tag!="tt" && tag!="code") {
                        result=result.replace(rxTT.pos(2), rxTT.cap(2).size(), rep);
                        pos += rep.size();
                    } else {
                        pos += rxTT.matchedLength();
                    }
                }

            }
        }

        //qDebug()<<result;


    }

    return result;


}
