#include "mainwindow.h"
#include "optionsdialog.h"
#include "ui_about.h"
#include "ui_aboutplugins.h"
#include "qftools.h"
#include "qfrdrreplacedialog.h"
#include "statistics_tools.h"

static QtLogFile* appLogFileQDebugWidget=NULL;


#if defined(Q_OS_WIN)
extern Q_CORE_EXPORT void qWinMsgHandler(QtMsgType t, const char* str);
#endif


void myMessageOutput(QtMsgType type, const char *msg)
 {
    if (appLogFileQDebugWidget) {
         switch (type) {
         case QtDebugMsg:
             appLogFileQDebugWidget->log_text_linebreak(msg);
             break;
         case QtWarningMsg:
             appLogFileQDebugWidget->log_warning_linebreak(msg);
             break;
         case QtCriticalMsg:
             appLogFileQDebugWidget->log_error_linebreak(msg);
             break;
         case QtFatalMsg:
             appLogFileQDebugWidget->log_error_linebreak(QObject::tr("FATAL ERROR: %1\n   APPLICATION WILL BE CLOSED/ABORTED!").arg(msg));
             //abort();
             break;
         }
    }

    #if defined(Q_OS_WIN)
     qWinMsgHandler(type, msg);
    #else
     qt_message_output(type, msg);
    #endif
    if (type==QtFatalMsg) abort();
 }

MainWindow::MainWindow(ProgramOptions* s, QSplashScreen* splash):
    QMainWindow(NULL)
{
    settings=s;
    splashPix=splash->pixmap();
    project=NULL;
    helpWindow=NULL;

    newProjectTimer.setInterval(2500);
    newProjectTimer.stop();
    connect(&newProjectTimer, SIGNAL(timeout()), this, SLOT(saveProjectFirstTime()));

    rawDataFactory=new QFRawDataRecordFactory(settings, this);
    evaluationFactory=new QFEvaluationItemFactory(settings, this);
    fitFunctionManager=new QFFitFunctionManager(settings, this);
    fitAlgorithmManager=new QFFitAlgorithmManager(settings, this);
    extensionManager=new QFExtensionManager(settings, this);
    importerManager=new QFImporterManager(settings, this);

    //settings=NULL;
    project=NULL;
    currentProjectDir="";

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

    splash->showMessage(tr("%1 Plugins loaded successfully").arg(rawDataFactory->getIDList().size()+evaluationFactory->getIDList().size()+fitFunctionManager->pluginCount()+fitAlgorithmManager->pluginCount()+extensionManager->getIDList().size()+importerManager->pluginCount()));

    htmlReplaceList.append(qMakePair(QString("version.svnrevision"), QString(SVNVERSION).trimmed()));
    htmlReplaceList.append(qMakePair(QString("version.status"), QString(VERSION_STATUS)));
    htmlReplaceList.append(qMakePair(QString("version.date"), QString(COMPILEDATE).trimmed()));
    htmlReplaceList.append(qMakePair(QString("version"), QString(VERSION_FULL)));
    htmlReplaceList.append(qMakePair(QString("version_full"), QApplication::applicationVersion()));
    htmlReplaceList.append(qMakePair(QString("thanksto"), QString(QF_THANKS_TO)));
    htmlReplaceList.append(qMakePair(QString("copyright"), QString(QF_COPYRIGHT)));
    htmlReplaceList.append(qMakePair(QString("author"), QString(QF_AUTHOR)));
    htmlReplaceList.append(qMakePair(QString("weblink"), QString(QF_WEBLINK)));
    htmlReplaceList.append(qMakePair(QString("license"), tr(QF_LICENSE)));
    htmlReplaceList.append(qMakePair(QString("plugin_list"), createPluginDoc(true)));
    htmlReplaceList.append(qMakePair(QString("pluginhelp_list"), createPluginDocHelp()));
    htmlReplaceList.append(qMakePair(QString("plugintutorials_list"), createPluginDocTutorials()));
    htmlReplaceList.append(qMakePair(QString("pluginsettings_list"), createPluginDocSettings()));
    htmlReplaceList.append(qMakePair(QString("plugincopyright_list"), createPluginDocCopyrights()));
    htmlReplaceList.append(qMakePair(QString("mainhelpdir"), settings->getAssetsDirectory()+QString("/help/")));
    htmlReplaceList.append(qMakePair(QString("assetsdir"), settings->getAssetsDirectory()));
    htmlReplaceList.append(qMakePair(QString("tutorials_contents"), QString("<ul>")+createPluginDocTutorials("<li>%1 tutorial:<ul>", "</ul></li>")+QString("/<ul>")));
    htmlReplaceList.append(qMakePair(QString("settings_contents"), QString("<ul>")+createPluginDocSettings("<li>%1 settings:<ul>", "</ul></li>")+QString("/<ul>")));
    htmlReplaceList.append(qMakePair(QString("help_contents"), QString("<ul>")+createPluginDocHelp("<li>%1 help:<ul>", "</ul></li>")+QString("</ul>")));
    htmlReplaceList.append(qMakePair(QString("qf_commondoc_backtop"), tr("<div style=\"background-color: lightsteelblue;  border-color: midnightblue; border-style: solid; padding-top:5px; padding-left:5px; padding-right:5px; padding-bottom:5px; margin: 5px;\"> <a href=\"#top_page\"><img src=\":/lib/help/help_top.png\"></a></div>")));

    htmlReplaceList.append(qMakePair(QString("qf_commondoc_header.start"),
         tr("<table width=\"100%\" border=\"1\" cellspacing=\"0\" cellpadding=\"2\" style=\"background-color: lightsteelblue;  border-color: midnightblue\" ><tr><td align=\"left\">"
            "<table width=\"100%\">"
            "<tr><td align=\"center\" colspan=\"3\">$$local_plugin_icon$$</td><td>&nbsp;&nbsp;&nbsp;&nbsp;</td><td width=\"90%\" align=\"left\"><b>$$local_plugin_name$$</b>  </td></tr> "
            "<tr><td align=\"left\" rowspan=\"2\">$$rel_prev$$</td><td align=\"left\" rowspan=\"2\">$$rel_contents$$</td><td align=\"left\" rowspan=\"2\">$$rel_next$$</td><td>&nbsp;&nbsp;&nbsp;&nbsp;</td><td width=\"90%\" align=\"left\">$$qf_commondoc_header.default_links$$  ")));
    htmlReplaceList.append(qMakePair(QString("qf_commondoc_header.end_notitle"), QString("</td></tr></table></td></tr></table>")));// </div>")));


    htmlReplaceList.append(qMakePair(QString("qf_commondoc_header.end"), tr("$$qf_commondoc_header.end_notitle$$ <h1>$$title$$</h1>")));
    htmlReplaceList.append(qMakePair(QString("qf_commondoc_header.separator"), QString(" | ")));

    htmlReplaceList.append(qMakePair(QString("qf_commondoc_header.default_links"), tr("<a href=\"%1quickfit.html\">QuickFit</a> $$local_plugin_typehelp_link$$ $$local_plugin_mainhelp_link$$ $$local_plugin_tutorial_link$$").arg(settings->getAssetsDirectory()+"/help/")));
    htmlReplaceList.append(qMakePair(QString("qf_commondoc_header.simplest"), tr("back: <a href=\"%1quickfit.html\">QuickFit Basics</a>").arg(settings->getAssetsDirectory()+"/help/")));
    htmlReplaceList.append(qMakePair(QString("qf_commondoc_header.rdr"), tr("$$qf_commondoc_header.separator$$ <a href=\"$$qf_ui_rdr_helpfil$$\">$$qf_ui_rdr_helpfiletitle$$</a>")));
    htmlReplaceList.append(qMakePair(QString("qf_ui_rdr_helpfile"), tr("%1qf3_rdrscreen.html").arg(settings->getAssetsDirectory()+"/help/")));
    htmlReplaceList.append(qMakePair(QString("qf_ui_rdr_helpfiletitle"), tr("Basic Raw Data Record Dialog Help")));
    htmlReplaceList.append(qMakePair(QString("qf_commondoc_header.eval"), tr("$$qf_commondoc_header.separator$$ <a href=\"%1qf3_evalscreen.html\">Basic Evaluation Dialog Help</a>").arg(settings->getAssetsDirectory()+"/help/")));
    htmlReplaceList.append(qMakePair(QString("qf_ui_eval_helpfile"), tr("%1qf3_evalscreen.html").arg(settings->getAssetsDirectory()+"/help/")));
    htmlReplaceList.append(qMakePair(QString("qf_ui_eval_helpfiletitle"), tr("Basic Evaluation Dialog Help")));
    htmlReplaceList.append(qMakePair(QString("qf_commondoc_header.extension"), tr("")));
    htmlReplaceList.append(qMakePair(QString("qf_commondoc_header.fitfunc"), tr("$$qf_commondoc_header.separator$$ <a href=\"%1qf3_evalscreen.html\">Fit Functions Help</a>")));
    htmlReplaceList.append(qMakePair(QString("qf_commondoc_header.fitalg"), tr("$$qf_commondoc_header.separator$$ <a href=\"%1qf3_fitalg.html\">Fit Algorithms Help</a>")));

    htmlReplaceList.append(qMakePair(QString("qf_commondoc_footer.start"),
         tr("<a name=\"#footer\"><table width=\"100%\" border=\"1\" cellspacing=\"0\" cellpadding=\"2\" style=\"background-color: lightsteelblue;  border-color: midnightblue\" ><tr><td align=\"left\">"
            "<table width=\"100%\">"
            "<tr><td align=\"center\" ><a href=\"#top_page\"><img src=\":/lib/help/help_top.png\"></a>&nbsp;&nbsp;&nbsp;</td><td align=\"left\" >$$local_plugin_icon$$&nbsp;&nbsp;&nbsp;</td><td align=\"right\" width=\"90%\">  <b>$$local_plugin_name$$</b> <i>$$local_plugin_copyright$$</i><br>$$local_plugin_weblink$$<br>")));
    htmlReplaceList.append(qMakePair(QString("qf_commondoc_footer.end"), QString("</td></tr></table></td></tr></table>")));// </div>")));

    logFileMainWidget->log_text(tr("QuickFit started succesfully!\n"));


    newProject();

    readSettings();
    rawDataFactory->distribute(project, settings, this, this);
    evaluationFactory->distribute(project, settings, this, this);

    // scan program arguments for a project file (has to be the last argument)
    QStringList args=QApplication::arguments();
    if (args.size()>0) {
        QString lastarg=args[args.size()-1];
        if (QFile::exists(lastarg) && (QFileInfo(lastarg).suffix()=="qfp")) {
            currentProjectDir=QFileInfo(lastarg).dir().absolutePath();
            loadProject(lastarg);
        }
    }

    autoWriteSettings();
    timerAutosave->start();

    setWindowIcon(QIcon(":/icon_large.png"));
}


MainWindow::~MainWindow() {
    //std::cout<<"deleting MainWindow\n";
    if (project) delete project;
    //std::cout<<"deleting MainWindow ... OK\n";
}

void MainWindow::searchAndRegisterPlugins() {
    // find plugins
    rawDataFactory->searchPlugins(settings->getPluginDirectory(), &pluginHelpList);
    evaluationFactory->searchPlugins(settings->getPluginDirectory(), &pluginHelpList);
    fitFunctionManager->searchPlugins(settings->getPluginDirectory(), &pluginHelpList);
    fitAlgorithmManager->searchPlugins(settings->getPluginDirectory(), &pluginHelpList);
    importerManager->searchPlugins(settings->getPluginDirectory(), &pluginHelpList);
    extensionManager->searchPlugins(settings->getPluginDirectory(), &pluginHelpList);


    // distribute application hooks
    rawDataFactory->distribute(project, settings, this, this);
    evaluationFactory->distribute(project, settings, this, this);
    extensionManager->distribute(project);

    // register plugins to menus
    QStringList sl=getRawDataRecordFactory()->getIDList();
    for (int i=0; i<sl.size(); i++) {
        getRawDataRecordFactory()->registerMenu(sl[i], insertItemMenu);
    }

    sl=getEvaluationItemFactory()->getIDList();
    for (int i=0; i<sl.size(); i++) {
        getEvaluationItemFactory()->registerMenu(sl[i], evaluationMenu);
    }

    // init extensions
    extensionManager->init(this, this);
}

void MainWindow::showLogMessage(const QString& message) {
    logFileProjectWidget->log_text(message);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (maybeSave()) {
        writeSettings();
        closeProject();
        rawDataFactory->distribute(NULL, settings, this, this);
        rawDataFactory->deinit();
        evaluationFactory->distribute(NULL, settings, this, this);
        evaluationFactory->deinit();
        extensionManager->distribute(NULL);
        extensionManager->deinit();
        newProjectTimer.stop();
        helpWindow->close();
        event->accept();
        QApplication::exit();
    } else {
        event->ignore();
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

        rawDataFactory->distribute(project, settings, this, this);
        evaluationFactory->distribute(project, settings, this, this);
        extensionManager->distribute(project);

        newProjectTimer.start();
        QApplication::restoreOverrideCursor();
    }
}

void MainWindow::openProject() {
    if (maybeSave()) {
        QString fileName = qfGetOpenFileName(this, tr("Open Project ..."), currentProjectDir, tr("QuickFit Project (*.qfp);;QuickFit Project Autosave (*.qfp.autosave *.qfp.autosave.backup);;QuickFit Project backup (*.qfp.backup)"));
        if (!fileName.isEmpty()) {
            currentProjectDir=QFileInfo(fileName).dir().absolutePath();
            QApplication::setOverrideCursor(Qt::WaitCursor);
            loadProject(fileName);
            QApplication::restoreOverrideCursor();
        }
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
    QString fileName = qfGetSaveFileName(this, tr("Save Project As ..."), currentProjectDir, tr("QuickFit Project (*.qfp)"));
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
    QTextEdit* ui_textEdit = qFindChild<QTextEdit*>(widget, "edtInfo");
    QLabel* ui_label = qFindChild<QLabel*>(widget, "labSplash");
    QLabel* ui_labelLic = qFindChild<QLabel*>(widget, "labLicense");
    ui_label->setPixmap(splashPix);
    ui_textEdit->setText(tr("<b>Copyright:</b><blockquote>%3</blockquote><b>libraries, used by QuickFit:</b><ul><li>QuickFit library v%4.%5</li><li>Qt %1 (<a href=\"http://qt.nokia.com/\">http://qt.nokia.com/</a>)</li></ul><b>many thanks to:</b><blockquote>%2</blockquote>").arg(QT_VERSION_STR).arg(QF_THANKS_TO).arg(QF_COPYRIGHT).arg(QF3LIB_APIVERSION_MAJOR).arg(QF3LIB_APIVERSION_MINOR));
    ui_labelLic->setText(tr(QF_LICENSE));
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

    text+=mainitem_before.arg(tr("Importers"));
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
    QString item_template=QString("<li><a href=\"%3\"><img width=\"16\" height=\"16\" src=\"%1\"></a>&nbsp;<a href=\"%3\">%2</a></li>");
    QString text=mainitem_before.arg(tr("Raw Data Record"));
    // gather information about plugins
    for (int i=0; i<getRawDataRecordFactory()->getIDList().size(); i++) {
        QString id=getRawDataRecordFactory()->getIDList().at(i);
        QString dir=getRawDataRecordFactory()->getPluginTutorial(id);
        if (QFile::exists(dir)) text+=item_template.arg(getRawDataRecordFactory()->getIconFilename(id)).arg(getRawDataRecordFactory()->getName(id)).arg(dir);
    }
    text+=mainitem_after;

    text+=mainitem_before.arg(tr("Data Evaluation"));
    // gather information about plugins
    for (int i=0; i<getEvaluationItemFactory()->getIDList().size(); i++) {
        QString id=getEvaluationItemFactory()->getIDList().at(i);
        QString dir=getEvaluationItemFactory()->getPluginTutorial(id);
        if (QFile::exists(dir)) text+=item_template.arg(getEvaluationItemFactory()->getIconFilename(id)).arg(getEvaluationItemFactory()->getName(id)).arg(dir);
    }
    text+=mainitem_after;

    text+=mainitem_before.arg(tr("<a href=\"$$mainhelpdir$$qf3_fitalg.html\">Fit Algorithm</a>"));
    // gather information about plugins
    for (int i=0; i<fitAlgorithmManager->pluginCount(); i++) {
        int id=i;
        QString dir=fitAlgorithmManager->getPluginTutorial(id);
        if (QFile::exists(dir)) text+=item_template.arg(fitAlgorithmManager->getIconFilename(id)).arg(fitAlgorithmManager->getName(id)).arg(dir);
    }
    text+=mainitem_after;
    text+=mainitem_before.arg(tr("<a href=\"$$mainhelpdir$$qf3_fitfunc.html\">Fit Function</a>"));
    // gather information about plugins
    for (int i=0; i<fitFunctionManager->pluginCount(); i++) {
        int id=i;
        QString dir=fitFunctionManager->getPluginTutorial(id);
        if (QFile::exists(dir)) text+=item_template.arg(fitFunctionManager->getIconFilename(id)).arg(fitFunctionManager->getName(id)).arg(dir);
    }
    text+=mainitem_after;

    text+=mainitem_before.arg(tr("Importers"));
    // gather information about plugins
    for (int i=0; i<importerManager->pluginCount(); i++) {
        int id=i;
        QString dir=importerManager->getPluginTutorial(id);
        if (QFile::exists(dir)) text+=item_template.arg(importerManager->getIconFilename(id)).arg(importerManager->getName(id)).arg(dir);
    }
    text+=mainitem_after;

    text+=mainitem_before.arg(tr("<a href=\"$$mainhelpdir$$qf3_extension.html\">Extension</a>"));
    // gather information about plugins
    for (int i=0; i<getExtensionManager()->getIDList().size(); i++) {
        QString id=getExtensionManager()->getIDList().at(i);
        QString dir=getExtensionManager()->getPluginTutorial(id);
        if (QFile::exists(dir)) text+=item_template.arg(getExtensionManager()->getIconFilename(id)).arg(getExtensionManager()->getName(id)).arg(dir);
    }
    text+=mainitem_after;

    return text;
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

    text+=mainitem_before.arg(tr("Importers"));
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

    text+=mainitem_before.arg(tr("Importers"));
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
    spCenter=new QVisibleHandleSplitter(Qt::Horizontal, this);
    spCenter->setOrientation(Qt::Horizontal);
    spMain=new QVisibleHandleSplitter(Qt::Vertical, this);
    spMain->setOrientation(Qt::Vertical);
    tvMain=new QTreeView(this);
    tvMain->setHeaderHidden(true);
    connect(tvMain, SIGNAL(doubleClicked ( const QModelIndex &)), this, SLOT(projectElementDoubleClicked(const QModelIndex&)));
    spCenter->addWidget(tvMain);
    QShortcut* shortcut = new QShortcut(QKeySequence(tr("Del")), tvMain);
    connect(shortcut, SIGNAL(activated()), this, SLOT(deleteItem()));
    setCentralWidget(spMain);

    QWidget* w=new QWidget(spCenter);
    QFormLayout* fl=new QFormLayout(w);
    w->setLayout(fl);
    spCenter->addWidget(w);
    spMain->addWidget(spCenter);

    edtName=new QLineEdit(w);
    fl->addRow(tr("Project &Name:"), edtName);
    connect(edtName, SIGNAL(textChanged(const QString&)), this, SLOT(projectNameChanged(const QString&)));
    labFile=new QLabel(w);
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
    logFileQDebugWidget->open_logfile(QString(settings->getConfigFileDirectory()+"/"+fi.completeBaseName()+"_qdebug.log"), false);
    appLogFileQDebugWidget=logFileQDebugWidget;

    logFileMainWidget=new QtLogFile(tabLogs);
    logFileMainWidget->set_log_file_append(true);
    logFileMainWidget->set_log_date_time(true);
    logFileProjectWidget->set_log_file_append(true);
    tabLogs->addTab(logFileMainWidget, tr("QuickFit Log"));
    logFileMainWidget->open_logfile(QString(settings->getConfigFileDirectory()+"/"+fi.completeBaseName()+".log"), false);
    logFileMainWidget->log_text(tr("starting up QuickFit %1 (SVN: %2 COMILEDATE: %3), %4-bit ...\n").arg(VERSION_FULL).arg(SVNVERSION).arg(COMPILEDATE).arg(getApplicationBitDepth()));
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

    /*for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        connect(recentFileActs[i], SIGNAL(triggered()), this, SLOT(openRecentProject()));
    }*/


    saveProjectAct = new QAction(QIcon(":/project_save.png"), tr("&Save Project"), this);
    saveProjectAct->setShortcuts(QKeySequence::Save);
    saveProjectAct->setStatusTip(tr("Save the project to disk"));
    connect(saveProjectAct, SIGNAL(triggered()), this, SLOT(saveProject()));

    saveProjectAsAct = new QAction(QIcon(":/project_saveas.png"), tr("Save Project &As..."), this);
    saveProjectAsAct->setShortcuts(QKeySequence::SaveAs);
    saveProjectAsAct->setStatusTip(tr("Save the project under a new name"));
    connect(saveProjectAsAct, SIGNAL(triggered()), this, SLOT(saveProjectAs()));

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
    helpPluginCopyrightAct=new QAction(QIcon(":/lib/help/help_copyright.png"), tr("Plugin C&opyright"), this);
    connect(helpPluginCopyrightAct, SIGNAL(triggered()), this, SLOT(displayHelpPluginCopyright()));
    helpTutorialsAct=new QAction(QIcon(":/lib/help/help_tutorial.png"), tr("Plugin &Tutorials"), this);
    connect(helpTutorialsAct, SIGNAL(triggered()), this, SLOT(displayHelpTutorials()));
    helpPluginAct=new QAction(QIcon(":/lib/help/help_contents.png"), tr("&Plugin Help"), this);
    connect(helpPluginAct, SIGNAL(triggered()), this, SLOT(displayPluginHelp()));

    helpActList.append(helpAct);
    helpActList.append(helpCopyrightAct);
    helpActList.append(helpPluginAct);
    helpActList.append(helpTutorialsAct);
    helpActList.append(helpPluginCopyrightAct);



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




    delItemAct = new QAction(QIcon(":/lib/item_delete.png"), tr("&Delete Current Item"), this);
    delItemAct->setStatusTip(tr("delete the currently selected item (if deletable) ..."));
    connect(delItemAct, SIGNAL(triggered()), this, SLOT(deleteItem()));

    tvMain->addAction(delItemAct);

    actRDRReplace=new QAction(tr("find/replace in raw data record names/folders"), this);
    connect(actRDRReplace, SIGNAL(triggered()), this, SLOT(rdrReplace()));
    actRDRUndoReplace=new QAction(tr("undo last find/replace"), this);
    connect(actRDRUndoReplace, SIGNAL(triggered()), this, SLOT(rdrUndoReplace()));

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
    recentMenu->setMaxRecentFilesCount(10);
    QSettings* s=settings->getQSettings();
    recentMenu->readSettings(*s, "mainwindow/recentfilelist");
    connect(recentMenu, SIGNAL(openRecentFile(QString)), this, SLOT(openRecentProject(QString)));
    fileMenu->addMenu(recentMenu);

    fileMenu->addAction(saveProjectAct);
    fileMenu->addAction(saveProjectAsAct);
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
    dataMenu->addAction(delItemAct);

    extensionMenu=menuBar()->addMenu(tr("&Extensions"));
    toolsMenu=menuBar()->addMenu(tr("&Tools"));
    projectToolsMenu=toolsMenu->addMenu(tr("project tools"));
    projectToolsMenu->addAction(actRDRReplace);
    projectToolsMenu->addAction(actRDRUndoReplace);
    debugToolsMenu=toolsMenu->addMenu(tr("debug tools"));
    debugToolsMenu->addAction(actPerformanceTest);
    toolsMenu->addSeparator();

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(helpAct);
    helpMenu->addAction(helpCopyrightAct);
    helpMenu->addSeparator();
    helpMenu->addAction(helpPluginAct);
    helpMenu->addAction(helpTutorialsAct);
    helpMenu->addAction(helpPluginCopyrightAct);

    helpMenu->addSeparator();
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutPluginsAct);
    helpMenu->addAction(aboutQtAct);

    menus["file"]=fileMenu;
    menus["data"]=dataMenu;
    menus["data/rdr"]=insertItemMenu;
    menus["data/eval"]=evaluationMenu;
    menus["extensions"]=extensionMenu;
    menus["help"]=helpMenu;
    menus["tools"]=toolsMenu;
    menus["tools/project"]=projectToolsMenu;
    menus["tools/debug"]=debugToolsMenu;

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
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->setObjectName("toolbar_file");
    fileToolBar->addAction(newProjectAct);
    fileToolBar->addAction(openProjectAct);
    fileToolBar->addAction(saveProjectAct);
    fileToolBar->addSeparator();
    fileToolBar->addAction(helpAct);
    dataToolBar = addToolBar(tr("Data"));
    QToolButton* tb=new QToolButton(dataToolBar);
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
    dataToolBar->addAction(delItemAct);
    dataToolBar->setObjectName("toolbar_data");
    extensionToolBar=addToolBar(tr("Extensions"));
    extensionToolBar->setObjectName("toolbar_extensions");

    toolbars["file"]=fileToolBar;
    toolbars["data"]=dataToolBar;
    toolbars["extensions"]=extensionToolBar;

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

    qInstallMsgHandler(myMessageOutput);

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
        ret = QMessageBox::warning(this, tr("QuickFit %1").arg(VERSION_FULL),
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

void MainWindow::loadProject(const QString &fileName) {
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
        QMessageBox::critical(this, tr("QuickFit %1").arg(VERSION_FULL),
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
    project->readXML(fn);
    prgMainProgress->reset();
    prgMainProgress->setRange(0,1);
    prgMainProgress->setValue(0);
    connect(project, SIGNAL(propertiesChanged()), this, SLOT(readProjectProperties()));
    readProjectProperties();
    connect(project, SIGNAL(wasChanged(bool)), this, SLOT(setWindowModified(bool)));
    connect(project, SIGNAL(structureChanged()), this, SLOT(modelReset()));
    if (project->error()) {
        QMessageBox::critical(this, tr("QuickFit %1").arg(VERSION_FULL), project->errorDescription());
        logFileProjectWidget->log_error(project->errorDescription()+"\n");
    }
    tvMain->setModel(project->getTreeModel());
    connect(project->getTreeModel(), SIGNAL(modelReset()), tvMain, SLOT(expandAll()));
    tvMain->expandToDepth(2);
    rawDataFactory->distribute(project, settings, this, this);
    evaluationFactory->distribute(project, settings, this, this);
    extensionManager->distribute(project);
    QApplication::restoreOverrideCursor();

    setCurrentProject(fileName);
    statusBar()->showMessage(tr("Project file '%1' loaded!").arg(fileName), 2000);
    logFileProjectWidget->log_text(tr("loading project file '%1' ... DONE!\n").arg(fileName));
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
        time.start();
        project->writeXML(fileName);
#if QT_VERSION >= 0x040800
   elapsed=double(time.nsecsElapsed())/1.0e9;
#else
    elapsed=double(time.elapsed())/1000.0;
#endif
        elapsed=double(time.elapsed())/1000.0;
        readProjectProperties();
        if (project->error()) {
            QMessageBox::critical(this, tr("QuickFit %1").arg(VERSION_FULL), project->errorDescription());
            logFileProjectWidget->log_error(project->errorDescription()+"\n");
        }
        tvMain->setModel(project->getTreeModel());
        connect(project->getTreeModel(), SIGNAL(modelReset()), tvMain, SLOT(expandAll()));
        tvMain->expandAll();
    }
    QApplication::restoreOverrideCursor();

    setCurrentProject(fileName);
    statusBar()->showMessage(tr("Project file '%1' saved!").arg(fileName), 2000);
    logFileProjectWidget->log_text(tr("Project file '%1' saved after %2 secs!\n").arg(fileName).arg(elapsed));
    return true;
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

    setWindowTitle(tr("%1[*] - %2").arg(shownName).arg(tr("QuickFit %1").arg(VERSION_FULL)));

    // update recent files list in ini file
    recentMenu->addRecentFile(fileName);
    QSettings* s=settings->getQSettings();
    recentMenu->storeSettings(*s, "mainwindow/recentfilelist");
    /*QStringList files = s->value("mainwindow/recentfilelist").toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > MaxRecentFiles)
        files.removeLast();
    files.removeDuplicates();
    if (files.size()>0) for (int j = files.size()-1; j >=0 ; j--) {
        if (files[j].trimmed().isEmpty()) {
            files.removeAt(j);
        } else {
            recentFileActs[j]->setVisible(false);
        }
    }
    s->setValue("mainwindow/recentfilelist", files);
    updateRecentFileActions();*/
}

/*void MainWindow::updateRecentFileActions(){
    QSettings* s=settings->getQSettings();
    QStringList files = s->value("mainwindow/recentfilelist").toStringList();
    files.removeDuplicates();
    if (files.size()>0) for (int j = files.size()-1; j >=0 ; j--) {
        if (files[j].trimmed().isEmpty()) {
            files.removeAt(j);
        } else {
            recentFileActs[j]->setVisible(false);
        }
    }
    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);
    for (int i = 0; i < numRecentFiles; ++i) {
        QString text = tr("&%1 %2").arg(i + 1).arg(strippedName(files[i]));
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData(files[i]);
        recentFileActs[i]->setVisible(true);
    }
    if (recentMenu) recentMenu->setEnabled(numRecentFiles > 0);
}*/

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

QFFitFunctionManager* MainWindow::getFitFunctionManager() {
    return fitFunctionManager;
}

QFFitAlgorithmManager* MainWindow::getFitAlgorithmManager() {
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

QString MainWindow::getPluginHelp(const QString& pluginID) {
    if (evaluationFactory->contains(pluginID)) return evaluationFactory->getPluginHelp(pluginID);
    if (rawDataFactory->contains(pluginID)) return rawDataFactory->getPluginHelp(pluginID);
    if (extensionManager->contains(pluginID)) return extensionManager->getPluginHelp(pluginID);
    return "";
}

QString MainWindow::getPluginTutorial(const QString& pluginID) {
    if (evaluationFactory->contains(pluginID)) return evaluationFactory->getPluginTutorial(pluginID);
    if (rawDataFactory->contains(pluginID)) return rawDataFactory->getPluginTutorial(pluginID);
    if (extensionManager->contains(pluginID)) return extensionManager->getPluginTutorial(pluginID);
    if (fitFunctionManager->contains(pluginID)) return fitFunctionManager->getPluginTutorial( fitFunctionManager->getPluginForID(pluginID));
    if (fitAlgorithmManager->contains(pluginID)) return fitAlgorithmManager->getPluginTutorial(fitAlgorithmManager->getPluginForID(pluginID));
    if (importerManager->contains(pluginID)) return importerManager->getPluginTutorial(importerManager->getPluginForID(pluginID));
    return "";
}

QString MainWindow::getPluginSettings(const QString& pluginID) {
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

QFExtensionManager* MainWindow::getExtensionManager() {
    return extensionManager;
}

QFImporterManager *MainWindow::getImporterManager() {
    return importerManager;
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

QString MainWindow::getPluginsDirectory() {
    return settings->getPluginDirectory();
}

void MainWindow::saveProjectFirstTime() {
    if (!settings->getUserSaveAfterFirstEdit()) return;
    if (!project) return;
    if (project->getRawDataCount()+project->getEvaluationCount()>0) {
        newProjectTimer.stop();
        int ret = QMessageBox::question(this, tr("QuickFit %1").arg(VERSION_FULL),
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

QList<QPair<QString, QString> >* MainWindow::getHTMLReplacementList() {
    return &htmlReplaceList;
}

QList<QFPluginServices::HelpDirectoryInfo>* MainWindow::getPluginHelpList() {
    return &pluginHelpList;

}
