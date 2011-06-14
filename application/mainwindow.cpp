#include "mainwindow.h"
#include "optionsdialog.h"
#include "ui_about.h"
#include "ui_aboutplugins.h"
#include "qftools.h"
#include "../extlibs/cimg.h"

MainWindow::MainWindow(ProgramOptions* s, QSplashScreen* splash)
{
    settings=s;
    splashPix=splash->pixmap();

    rawDataFactory=new QFRawDataRecordFactory(this);
    evaluationFactory=new QFEvaluationItemFactory(this);
    fitFunctionManager=new QFFitFunctionManager(this);
    fitAlgorithmManager=new QFFitAlgorithmManager(this);
    extensionManager=new QFExtensionManager(this);

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
    setUnifiedTitleAndToolBarOnMac(true);


    // search for plugin
    connect(rawDataFactory, SIGNAL(showMessage(const QString&)), splash, SLOT(showMessage(const QString&)));
    connect(evaluationFactory, SIGNAL(showMessage(const QString&)), splash, SLOT(showMessage(const QString&)));
    connect(fitAlgorithmManager, SIGNAL(showMessage(const QString&)), splash, SLOT(showMessage(const QString&)));
    connect(fitFunctionManager, SIGNAL(showMessage(const QString&)), splash, SLOT(showMessage(const QString&)));
    connect(extensionManager, SIGNAL(showMessage(const QString&)), splash, SLOT(showMessage(const QString&)));

    connect(rawDataFactory, SIGNAL(showLongMessage(const QString&)), logFileMainWidget, SLOT(log_text_linebreak(QString)));
    connect(evaluationFactory, SIGNAL(showLongMessage(const QString&)), logFileMainWidget, SLOT(log_text_linebreak(QString)));
    connect(fitFunctionManager, SIGNAL(showLongMessage(const QString&)), logFileMainWidget, SLOT(log_text_linebreak(QString)));
    connect(fitAlgorithmManager, SIGNAL(showLongMessage(const QString&)), logFileMainWidget, SLOT(log_text_linebreak(QString)));
    connect(extensionManager, SIGNAL(showLongMessage(const QString&)), logFileMainWidget, SLOT(log_text_linebreak(QString)));

    connect(timerAutosave, SIGNAL(timeout()), this, SLOT(autosaveProject()));


    logFileMainWidget->log_header(tr("searching for plugins ..."));
    logFileMainWidget->inc_indent();
    searchAndRegisterPlugins();
    QFile f(settings->getAssetsDirectory()+"/help/plugin_list_autocreate.html");
    if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&f);
        out<<QString("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\"><html><head><title>%1</title></head><body><h2>%1</h2>").arg(tr("Available Plugins"));
        out<<createPluginDoc(true);
        out<<"</body></html>";
        f.close();
    }
    logFileMainWidget->dec_indent();

    splash->showMessage(tr("%1 Plugins loaded successfully").arg(rawDataFactory->getIDList().size()+evaluationFactory->getIDList().size()+fitFunctionManager->pluginCount()+fitAlgorithmManager->pluginCount()+extensionManager->getIDList().size()));

    logFileMainWidget->log_text(tr("QuickFit started succesfully!\n"));


    newProject();

    autoWriteSettings();
    timerAutosave->start();

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
}


MainWindow::~MainWindow() {
    //std::cout<<"deleting MainWindow\n";
    if (project) delete project;
    //std::cout<<"deleting MainWindow ... OK\n";
}

void MainWindow::searchAndRegisterPlugins() {
    // find plugins
    rawDataFactory->searchPlugins(settings->getPluginDirectory());
    evaluationFactory->searchPlugins(settings->getPluginDirectory());
    fitFunctionManager->searchPlugins(settings->getPluginDirectory());
    fitAlgorithmManager->searchPlugins(settings->getPluginDirectory());
    extensionManager->searchPlugins(settings->getPluginDirectory());


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
        evaluationFactory->distribute(NULL, settings, this, this);
        extensionManager->distribute(NULL);
        extensionManager->deinit();
        event->accept();
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
    }
}

void MainWindow::newProject() {
    if (maybeSave()) {
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
    }
}

void MainWindow::openProject() {
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open Project ..."), currentProjectDir, tr("QuickFit Project (*.qfp)"));
        if (!fileName.isEmpty()) {
            currentProjectDir=QFileInfo(fileName).dir().absolutePath();
            loadProject(fileName);
        }
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
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Project As ..."), currentProjectDir, tr("QuickFit Project (*.qfp)"));
    if (fileName.isEmpty())
        return false;
    currentProjectDir=QFileInfo(fileName).dir().absolutePath();

    return saveProject(fileName);
}


void MainWindow::about() {
    QDialog *widget = new QDialog(this);
    Ui::About ui;
    ui.setupUi(widget);
    QTextEdit* ui_textEdit = qFindChild<QTextEdit*>(widget, "edtInfo");
    QLabel* ui_label = qFindChild<QLabel*>(widget, "labSplash");
    ui_label->setPixmap(splashPix);
    ui_textEdit->setText(tr("<b>Copyright:</b><blockquote>%3</blockquote><b>libraries, used by QuickFit:</b><ul><li>Qt %1 (<a href=\"http://qt.nokia.com/\">http://qt.nokia.com/</a>)</li><li>CImg %4 (<a href=\"http://cimg.sourceforge.net/\">http://cimg.sourceforge.net/</a>)</li></ul><b>many thanks to:</b><blockquote>%2</blockquote>").arg(QT_VERSION_STR).arg(QF_THANKS_TO).arg(QF_COPYRIGHT).arg(cimg_version));
    widget->exec();
    delete widget;
}

QString MainWindow::createPluginDoc(bool docLinks) {
    QString text=tr("<h2>Raw Data Record Plugins:</h2><center><table border=\"0\" bgcolor=\"darkgray\" width=\"90%\">");
    // gather information about plugins
    for (int i=0; i<getRawDataRecordFactory()->getIDList().size(); i++) {
        QString id=getRawDataRecordFactory()->getIDList().at(i);
        if (docLinks) text+=QString("<tr><td>&nbsp;</td><td></td></tr><tr><td colspan=\"2\" bgcolor=\"silver\"><a href=\"../plugins/help/%3/%3.html\"><img src=\"%2\">&nbsp;<b>%1</b></a></td></tr>").arg(getRawDataRecordFactory()->getName(id)).arg(getRawDataRecordFactory()->getIconFilename(id)).arg(id);
        else text+=QString("<tr><td>&nbsp;</td><td></td></tr><tr><td colspan=\"2\" bgcolor=\"silver\"><img src=\"%2\">&nbsp;<b>%1</b></td></tr>").arg(getRawDataRecordFactory()->getName(id)).arg(getRawDataRecordFactory()->getIconFilename(id));
        text+=QString("<tr><td><i>%1</i></td><td bgcolor=\"silver\">%2</td></tr>").arg(tr("description:")).arg(getRawDataRecordFactory()->getDescription(id));
        text+=QString("<tr><td><i>%1</i></td><td bgcolor=\"silver\">%2</td></tr>").arg(tr("author:")).arg(getRawDataRecordFactory()->getAuthor(id));
        text+=QString("<tr><td><i>%1</i></td><td bgcolor=\"silver\">%2</td></tr>").arg(tr("copyright:")).arg(getRawDataRecordFactory()->getCopyright(id));
        text+=QString("<tr><td><i>%1</i></td><td bgcolor=\"silver\">%2</td></tr>").arg(tr("weblink:")).arg(getRawDataRecordFactory()->getWeblink(id));
        text+=QString("<tr><td><i>%1</i></td><td bgcolor=\"silver\">%2</td></tr>").arg(tr("file:")).arg(getRawDataRecordFactory()->getPluginFilename(id));
        text+=QString("<tr><td><i>%1</i></td><td bgcolor=\"silver\">%2</td></tr>").arg(tr("id:")).arg(id);
    }
    text+="</table></center>";
    text+=tr("<br><br><h2>Data Evaluation Plugins:</h2><center><table border=\"0\" bgcolor=\"darkgray\" width=\"90%\">");
    // gather information about plugins
    for (int i=0; i<getEvaluationItemFactory()->getIDList().size(); i++) {
        QString id=getEvaluationItemFactory()->getIDList().at(i);
        if (docLinks) text+=QString("<tr><td>&nbsp;</td><td></td></tr><tr><td colspan=\"2\" bgcolor=\"silver\"><a href=\"../plugins/help/%3/%3.html\"><img src=\"%2\">&nbsp;<b>%1</b></a></td></tr>").arg(getEvaluationItemFactory()->getName(id)).arg(getEvaluationItemFactory()->getIconFilename(id)).arg(id);
        else text+=QString("<tr><td>&nbsp;</td><td></td></tr><tr><td colspan=\"2\" bgcolor=\"silver\"><img src=\"%2\">&nbsp;<b>%1</b></td></tr>").arg(getEvaluationItemFactory()->getName(id)).arg(getEvaluationItemFactory()->getIconFilename(id));
        text+=QString("<tr><td><i>%1</i></td><td bgcolor=\"silver\">%2</td></tr>").arg(tr("description:")).arg(getEvaluationItemFactory()->getDescription(id));
        text+=QString("<tr><td><i>%1</i></td><td bgcolor=\"silver\">%2</td></tr>").arg(tr("author:")).arg(getEvaluationItemFactory()->getAuthor(id));
        text+=QString("<tr><td><i>%1</i></td><td bgcolor=\"silver\">%2</td></tr>").arg(tr("copyright:")).arg(getEvaluationItemFactory()->getCopyright(id));
        text+=QString("<tr><td><i>%1</i></td><td bgcolor=\"silver\">%2</td></tr>").arg(tr("weblink:")).arg(getEvaluationItemFactory()->getWeblink(id));
        text+=QString("<tr><td><i>%1</i></td><td bgcolor=\"silver\">%2</td></tr>").arg(tr("file:")).arg(getEvaluationItemFactory()->getPluginFilename(id));
        text+=QString("<tr><td><i>%1</i></td><td bgcolor=\"silver\">%2</td></tr>").arg(tr("id:")).arg(id);
    }
    text+="</table></center>";

    text+=tr("<br><br><h2>Fit Algorithm Plugins:</h2><center><table border=\"0\" bgcolor=\"darkgray\" width=\"90%\">");
    // gather information about plugins
    for (int i=0; i<fitAlgorithmManager->pluginCount(); i++) {
        int id=i;
        if (docLinks) text+=QString("<tr><td>&nbsp;</td><td></td></tr><tr><td colspan=\"2\" bgcolor=\"silver\"><a href=\"../plugins/help/%2/%2.html\"><b>%1</b></a></td></tr>").arg(fitAlgorithmManager->getName(id)).arg(QFileInfo(fitAlgorithmManager->getFilename(id)).completeBaseName());
        else text+=QString("<tr><td>&nbsp;</td><td></td></tr><tr><td colspan=\"2\" bgcolor=\"silver\"><b>%1</b></td></tr>").arg(fitAlgorithmManager->getName(id));
        text+=QString("<tr><td><i>%1</i></td><td bgcolor=\"silver\">%2</td></tr>").arg(tr("description:")).arg(fitAlgorithmManager->getDescription(id));
        text+=QString("<tr><td><i>%1</i></td><td bgcolor=\"silver\">%2</td></tr>").arg(tr("author:")).arg(fitAlgorithmManager->getAuthor(id));
        text+=QString("<tr><td><i>%1</i></td><td bgcolor=\"silver\">%2</td></tr>").arg(tr("copyright:")).arg(fitAlgorithmManager->getCopyright(id));
        text+=QString("<tr><td><i>%1</i></td><td bgcolor=\"silver\">%2</td></tr>").arg(tr("weblink:")).arg(fitAlgorithmManager->getWeblink(id));
        text+=QString("<tr><td><i>%1</i></td><td bgcolor=\"silver\">%2</td></tr>").arg(tr("file:")).arg(fitAlgorithmManager->getFilename(id));
        text+=QString("<tr><td><i>%1</i></td><td bgcolor=\"silver\">%2</td></tr>").arg(tr("implemented ids:")).arg(fitAlgorithmManager->getIDList(id).join(", "));
    }
    text+="</table></center>";
    text+=tr("<br><br><h2>Fit Function Plugins:</h2><center><table border=\"0\" bgcolor=\"darkgray\" width=\"90%\">");
    // gather information about plugins
    for (int i=0; i<fitFunctionManager->pluginCount(); i++) {
        int id=i;
        if (docLinks) text+=QString("<tr><td>&nbsp;</td><td></td></tr><tr><td colspan=\"2\" bgcolor=\"silver\"><a href=\"../plugins/help/%2/%2.html\"><b>%1</b></a></td></tr>").arg(fitFunctionManager->getName(id)).arg(QFileInfo(fitFunctionManager->getFilename(id)).completeBaseName());
        else text+=QString("<tr><td>&nbsp;</td><td></td></tr><tr><td colspan=\"2\" bgcolor=\"silver\"><b>%1</b></td></tr>").arg(fitFunctionManager->getName(id));
        text+=QString("<tr><td><i>%1</i></td><td bgcolor=\"silver\">%2</td></tr>").arg(tr("description:")).arg(fitFunctionManager->getDescription(id));
        text+=QString("<tr><td><i>%1</i></td><td bgcolor=\"silver\">%2</td></tr>").arg(tr("author:")).arg(fitFunctionManager->getAuthor(id));
        text+=QString("<tr><td><i>%1</i></td><td bgcolor=\"silver\">%2</td></tr>").arg(tr("copyright:")).arg(fitFunctionManager->getCopyright(id));
        text+=QString("<tr><td><i>%1</i></td><td bgcolor=\"silver\">%2</td></tr>").arg(tr("weblink:")).arg(fitFunctionManager->getWeblink(id));
        text+=QString("<tr><td><i>%1</i></td><td bgcolor=\"silver\">%2</td></tr>").arg(tr("file:")).arg(fitFunctionManager->getFilename(id));
        text+=QString("<tr><td><i>%1</i></td><td bgcolor=\"silver\">%2</td></tr>").arg(tr("implemented ids:")).arg(fitFunctionManager->getIDList(id).join(", "));
    }
    text+="</table></center>";

    text+=tr("<br><br><h2>Extension Plugins:</h2><center><table border=\"0\" bgcolor=\"darkgray\" width=\"90%\">");
    // gather information about plugins
    for (int i=0; i<getExtensionManager()->getIDList().size(); i++) {
        QString id=getExtensionManager()->getIDList().at(i);
        if (docLinks) text+=QString("<tr><td>&nbsp;</td><td></td></tr><tr><td colspan=\"2\" bgcolor=\"silver\"><a href=\"../plugins/help/%3/%3.html\"><img src=\"%2\">&nbsp;<b>%1</b></a></td></tr>").arg(getExtensionManager()->getName(id)).arg(getExtensionManager()->getIconFilename(id)).arg(id);
        else text+=QString("<tr><td>&nbsp;</td><td></td></tr><tr><td colspan=\"2\" bgcolor=\"silver\"><img src=\"%2\">&nbsp;<b>%1</b></td></tr>").arg(getExtensionManager()->getName(id)).arg(getExtensionManager()->getIconFilename(id));
        text+=QString("<tr><td><i>%1</i></td><td bgcolor=\"silver\">%2</td></tr>").arg(tr("description:")).arg(getExtensionManager()->getDescription(id));
        text+=QString("<tr><td><i>%1</i></td><td bgcolor=\"silver\">%2</td></tr>").arg(tr("author:")).arg(getExtensionManager()->getAuthor(id));
        text+=QString("<tr><td><i>%1</i></td><td bgcolor=\"silver\">%2</td></tr>").arg(tr("copyright:")).arg(getExtensionManager()->getCopyright(id));
        text+=QString("<tr><td><i>%1</i></td><td bgcolor=\"silver\">%2</td></tr>").arg(tr("weblink:")).arg(getExtensionManager()->getWeblink(id));
        text+=QString("<tr><td><i>%1</i></td><td bgcolor=\"silver\">%2</td></tr>").arg(tr("file:")).arg(getExtensionManager()->getPluginFilename(id));
        text+=QString("<tr><td><i>%1</i></td><td bgcolor=\"silver\">%2</td></tr>").arg(tr("id:")).arg(id);
    }
    text+="</table></center>";

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
    spCenter=new QSplitter(Qt::Horizontal, this);
    spCenter->setOrientation(Qt::Horizontal);
    spMain=new QSplitter(Qt::Vertical, this);
    spMain->setOrientation(Qt::Vertical);
    tvMain=new QTreeView(this);
    connect(tvMain, SIGNAL(doubleClicked ( const QModelIndex &)), this, SLOT(projectElementDoubleClicked(const QModelIndex&)));
    spCenter->addWidget(tvMain);
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
    fl->addRow(tr("Project File:"), labFile);
    edtCreator=new QLineEdit(w);
    fl->addRow(tr("Project &Creator:"), edtCreator);
    connect(edtCreator, SIGNAL(textChanged(const QString&)), this, SLOT(projectCreatorChanged(const QString&)));
    pteDescription=new QPlainTextEdit(w);
    fl->addRow(tr("Project &Description:"), pteDescription);
    connect(pteDescription, SIGNAL(textChanged()), this, SLOT(projectDescriptionChanged()));

    tabLogs=new QTabWidget(this);
    logFileProjectWidget=new QtLogFile(tabLogs);
    logFileMainWidget=new QtLogFile(tabLogs);
    logFileMainWidget->set_log_file_append(true);
    logFileProjectWidget->set_log_file_append(true);
    tabLogs->addTab(logFileMainWidget, tr("QuickFit Log"));
    QFileInfo fi(QApplication::applicationFilePath());
    logFileMainWidget->open_logfile(QString(settings->getConfigFileDirectory()+"/"+fi.completeBaseName()+".log"), false);
    logFileMainWidget->log_text(tr("starting up QuickFit 3.0 ...\n"));
    logFileMainWidget->log_text(tr("logging to '%1' ...\n").arg(settings->getConfigFileDirectory()+"/"+fi.completeBaseName()+".log"));
    tabLogs->addTab(logFileProjectWidget, tr("Project Log"));
    tabLogs->setCurrentWidget(logFileMainWidget);
    spMain->addWidget(tabLogs);

    prgMainProgress=new QProgressBar(this);
    statusBar()->addPermanentWidget(prgMainProgress);

    helpWindow=new QFHTMLHelpWindow(0);
    helpWindow->close();

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


    saveProjectAct = new QAction(QIcon(":/project_save.png"), tr("&Save Project"), this);
    saveProjectAct->setShortcuts(QKeySequence::Save);
    saveProjectAct->setStatusTip(tr("Save the project to disk"));
    connect(saveProjectAct, SIGNAL(triggered()), this, SLOT(saveProject()));

    saveProjectAsAct = new QAction(QIcon(":/project_saveas.png"), tr("Save Project &As..."), this);
    saveProjectAsAct->setShortcuts(QKeySequence::SaveAs);
    saveProjectAsAct->setStatusTip(tr("Save the project under a new name"));
    connect(saveProjectAsAct, SIGNAL(triggered()), this, SLOT(saveProjectAs()));

    optionsAct = new QAction(QIcon(":/configure.png"), tr("&Settings"), this);
    optionsAct->setStatusTip(tr("Application settings dialog"));
    connect(optionsAct, SIGNAL(triggered()), this, SLOT(openSettingsDialog()));

    exitAct = new QAction(QIcon(":/exit.png"), tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));


    helpAct=new QAction(QIcon(":/help.png"), tr("&Help"), this);
    connect(helpAct, SIGNAL(triggered()), this, SLOT(displayHelp()));


    aboutAct = new QAction(QIcon(":/about.png"), tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutPluginsAct = new QAction(QIcon(":/about.png"), tr("About &Plugins"), this);
    aboutPluginsAct->setStatusTip(tr("Show a list with all registered plugins"));
    connect(aboutPluginsAct, SIGNAL(triggered()), this, SLOT(aboutPlugins()));


    aboutQtAct = new QAction(QIcon(":/aboutqt.png"), tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));




    delItemAct = new QAction(QIcon(":/item_delete.png"), tr("&Delete Current Item"), this);
    delItemAct->setStatusTip(tr("delete the currently selected item (if deletable) ..."));
    connect(delItemAct, SIGNAL(triggered()), this, SLOT(deleteItem()));


}

void MainWindow::createMenus() {
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newProjectAct);

    fileMenu->addAction(openProjectAct);
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

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(helpAct);
    helpMenu->addSeparator();
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutPluginsAct);
    helpMenu->addAction(aboutQtAct);

    menus["file"]=fileMenu;
    menus["data"]=dataMenu;
    menus["extensions"]=extensionMenu;
    menus["help"]=helpMenu;
}

void MainWindow::insertRawData() {
    QVariant id=sender()->property("call_id");
    if (id.isValid() && project) {
         getRawDataRecordFactory()->createRecord(id.toString(), project);
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
        getEvaluationItemFactory()->createRecord(id.toString(), this, project);
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

    currentProjectDir=settings->getQSettings()->value("mainwindow/currentProjectDir", currentProjectDir).toString();
    currentRawDataDir=settings->getQSettings()->value("mainwindow/currentRawDataDir", currentRawDataDir).toString();
    //column_separator=settings->getQSettings()->value("csvimport/column_separator", column_separator).toString();
    //decimal_separator=settings->getQSettings()->value("csvimport/decimal_separator", decimal_separator).toString();
    //comment_start=settings->getQSettings()->value("csvimport/comment_start", comment_start).toString();
    //header_start=settings->getQSettings()->value("csvimport/header_start", header_start).toString();
    //currentFCSFileFormatFilter=settings->getQSettings()->value("mainwindow/currentFCSFileFormatFilter", currentFCSFileFormatFilter).toString();

    logFileMainWidget->readSettings(*(settings->getQSettings()), "mainwindow/logMain");
    logFileProjectWidget->readSettings(*(settings->getQSettings()), "mainwindow/logProject");

    helpWindow->readSettings(*settings->getQSettings(), "mainwindow/help_");

    extensionManager->readPluginSettings(settings);

    //std::cout<<"autosave="<<settings->getAutosave()<<"\n";
    if (settings->getAutosave()<=0) {
        timerAutosave->stop();
    } else {
        timerAutosave->setInterval(settings->getAutosave()*60000);
        timerAutosave->start();
    }

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

    logFileMainWidget->saveSettings(*(settings->getQSettings()), "mainwindow/logMain");
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
        ret = QMessageBox::warning(this, tr("QuickFit 3.0"),
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
    tabLogs->setCurrentWidget(logFileProjectWidget);
    logFileProjectWidget->close_logfile();
    logFileProjectWidget->clearLog();
    logFileProjectWidget->open_logfile(fileName+".log", false);
    logFileProjectWidget->log_line();
    logFileProjectWidget->log_text(tr("loading project file '%1' ...\n").arg(fileName));
    statusBar()->showMessage(tr("loading project file '%1' ...").arg(fileName), 2000);
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::critical(this, tr("QuickFit 3.0"),
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
    connect(project, SIGNAL(modelReset()), this, SLOT(modelReset()));
    if (project->error()) {
        QMessageBox::critical(this, tr("QuickFit 3.0"), project->errorDescription());
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
    logFileMainWidget->log_text(tr("saving project to file '%1' ...\n").arg(fileName));
    tabLogs->setCurrentWidget(logFileProjectWidget);
    statusBar()->showMessage(tr("saving project file '%1' ...").arg(fileName), 2000);
    logFileProjectWidget->log_text(tr("saving project file '%1' ...\n").arg(fileName));
    QApplication::setOverrideCursor(Qt::WaitCursor);
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
        project->writeXML(fileName);
        readProjectProperties();
        if (project->error()) {
            QMessageBox::critical(this, tr("QuickFit 3.0"), project->errorDescription());
            logFileProjectWidget->log_error(project->errorDescription()+"\n");
        }
        tvMain->setModel(project->getTreeModel());
        connect(project->getTreeModel(), SIGNAL(modelReset()), tvMain, SLOT(expandAll()));
        tvMain->expandAll();
    }
    QApplication::restoreOverrideCursor();

    setCurrentProject(fileName);
    statusBar()->showMessage(tr("Project file '%1' saved!").arg(fileName), 2000);
    logFileProjectWidget->log_text(tr("Project file '%1' saved!\n").arg(fileName));
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

    setWindowTitle(tr("%1[*] - %2").arg(shownName).arg(tr("QuickFit 3.0")));
}

QString MainWindow::strippedName(const QString &fullFileName) {
    return QFileInfo(fullFileName).fileName();
}

void MainWindow::projectElementDoubleClicked ( const QModelIndex & index ) {
    if (project) {
        QFProjectTreeModel::nodeType nt=project->getTreeModel()->classifyIndex(tvMain->selectionModel()->currentIndex());
        if (nt==QFProjectTreeModel::qfpntRawDataRecord) {
            QFRawDataRecord* rec=project->getTreeModel()->getRawDataByIndex(index);
            if (rec) {
                QFRawDataPropertyEditor* edt=new QFRawDataPropertyEditor(this, settings, rec, rawDataPropEditors.size(), this, Qt::Dialog|Qt::WindowMaximizeButtonHint|Qt::WindowCloseButtonHint|Qt::WindowSystemMenuHint);
                edt->setAttribute(Qt::WA_DeleteOnClose);
                rawDataPropEditors.append(edt);
                edt->show();
            }
        } else if (nt==QFProjectTreeModel::qfpntEvaluationRecord) {
            QFEvaluationItem* rec=project->getTreeModel()->getEvaluationByIndex(tvMain->selectionModel()->currentIndex());
            if (rec) {
                QFEvaluationPropertyEditor* edt=new QFEvaluationPropertyEditor(this, settings, rec, evaluationPropEditors.size(), this, Qt::Dialog|Qt::WindowMaximizeButtonHint|Qt::WindowCloseButtonHint|Qt::WindowSystemMenuHint);
                //QFEvaluationPropertyEditor* edt=rec->createPropertyEditor(settings, evaluationPropEditors.size(), this, Qt::Dialog|Qt::WindowMaximizeButtonHint);
                edt->setAttribute(Qt::WA_DeleteOnClose);
                evaluationPropEditors.append(edt);
                edt->show();
            }
        }
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
        QFProjectTreeModel::nodeType nt=project->getTreeModel()->classifyIndex(tvMain->selectionModel()->currentIndex());
        if (nt==QFProjectTreeModel::qfpntRawDataRecord) {
            QFRawDataRecord* rec=project->getTreeModel()->getRawDataByIndex(tvMain->selectionModel()->currentIndex());
            if (rec) {
                project->deleteRawData(rec->getID());
                tvMain->expandToDepth(2);
            }
        } else if (nt==QFProjectTreeModel::qfpntEvaluationRecord) {
            QFEvaluationItem* rec=project->getTreeModel()->getEvaluationByIndex(tvMain->selectionModel()->currentIndex());
            if (rec) {
                project->deleteEvaluation(rec->getID());
                tvMain->expandToDepth(2);
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
    //std::cout<<"opening settings\n";
    opt->open(settings);
    //std::cout<<"autosave="<<settings->getAutosave()<<"\n";
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
    QString autosaveFilename=curFile;
    if (autosaveFilename.isEmpty()) autosaveFilename="untitled.qfp";
    autosaveFilename=autosaveFilename+".autosave";

    statusBar()->showMessage(tr("autosaving project file '%1' ...").arg(autosaveFilename), 500);
    logFileProjectWidget->log_text(tr("autosaving project file '%1' ...\n").arg(autosaveFilename));
    project->writeXML(autosaveFilename, false);

}

void MainWindow::displayHelp() {
    helpWindow->clear();
    helpWindow->updateHelp(QApplication::applicationDirPath()+QString("/help/quickfit.html"));
    helpWindow->show();
}

QMenu* MainWindow::getMenu(QString menu) {
    if (menus.contains(menu)) return menus[menu];
    return NULL;
}

QToolBar* MainWindow::getToolbar(QString toolbar) {
    if (toolbars.contains(toolbar)) return toolbars[toolbar];
    return NULL;
}

void MainWindow::insertMenu(QString menuname, QMenu* newMenu, QMenu* before) {
    menus[menuname]=newMenu;
    menuBar()->insertMenu(before->menuAction(), newMenu);
}

void MainWindow::insertToolBar(QString toolbarname, QToolBar* newToolbar) {
    toolbars[toolbarname]=newToolbar;
    addToolBar(newToolbar);
}

QFExtensionManager* MainWindow::getExtensionManager() {
    return extensionManager;
}

QString MainWindow::getConfigFileDirectory() {
    return settings->getConfigFileDirectory();
}

QString MainWindow::getAssetsDirectory() {
    return settings->getAssetsDirectory();
}

QString MainWindow::getPluginsDirectory() {
    return settings->getPluginDirectory();
}

