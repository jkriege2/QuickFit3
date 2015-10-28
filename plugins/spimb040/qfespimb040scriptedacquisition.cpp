/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    

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

#include "qfespimb040scriptedacquisition.h"
#include "ui_qfespimb040scriptedacquisition.h"

#include "qfpluginservices.h"
#include "qfstagecombobox.h"
#include "qfextensionmanager.h"
#include "qfextensionlinearstage.h"
#include "qfextension.h"
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtGlobal>
#include <QtWidgets>
#else
#include <QtGui>
#endif


#include <QtCore>
#include "qfcompleterfromfile.h"
#include "qfespimb040mainwindow2.h"
#include "qfespimb040scriptedacquisitiontools.h"





QFESPIMB040ScriptedAcquisitionDocSearchThread::QFESPIMB040ScriptedAcquisitionDocSearchThread(QStringList files, QObject *parent):
    QThread(parent)
{
    this->files=files;
    stopped=false;
}

void QFESPIMB040ScriptedAcquisitionDocSearchThread::stopThread()
{
    stopped=true;
}

void QFESPIMB040ScriptedAcquisitionDocSearchThread::run()
{
    for (int i=0; i<files.size(); i++) {
        QFile f(files[i]);
        if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
            QString contents=f.readAll();
            f.close();
            int pos=0;
            QRegExp rx("<!--\\s*func:([\\w\\.\\_]*)\\s*-->(.*)<!--\\s*/func:\\1\\s*-->");
            while ((pos = rx.indexIn(contents, pos)) != -1) {
                QString name=rx.cap(1).trimmed();
                QString help=rx.cap(2).trimmed();
                QRegExp rx2("<!--\\s*template\\s*-->(.*)<!--\\s*/template\\s*-->");
                QString templ="";
                if (rx2.indexIn(help,0)!=-1) {
                    templ=rx2.cap(1);
                }
                emit foundFunction(name, templ, help);
                pos += rx.matchedLength();
                if (stopped) break;
             }
        }
        if (stopped) break;
    }
}















QFESPIMB040ScriptedAcquisition::QFESPIMB040ScriptedAcquisition(QFESPIMB040MainWindow2* mainWindow, QFESPIMB040AcquisitionTools* acqTools, QFPluginLogService* log, QWidget* parent, QFPluginServices* pluginServices, QFESPIMB040OpticsSetupBase* opticsSetup, QFESPIMB040AcquisitionDescription* acqDescription, QFESPIMB040ExperimentDescription* expDescription, QString /*configDirectory*/) :
    QWidget(parent),
    ui(new Ui::QFESPIMB040ScriptedAcquisition)
{

    this->m_pluginServices=pluginServices;
    this->opticsSetup=opticsSetup;
    this->acqDescription=acqDescription;
    this->expDescription=expDescription;
    this->log=log;
    this->acqTools=acqTools;
    this->mainWindow=mainWindow;

    acquisitionTools=new QFESPIMB040ScriptedAcquisitionTools(this, mainWindow, acqTools, log, this, pluginServices, opticsSetup, acqDescription, expDescription);
    instrumentControl=new QFESPIMB040ScriptedAcquisitionInstrumentControl(this, mainWindow, acqTools, log, this, pluginServices, opticsSetup, acqDescription, expDescription);
    acquisitionControl=new QFESPIMB040ScriptedAcquisitionAcquisitionControl(this, mainWindow, acqTools, log, this, pluginServices, opticsSetup, acqDescription, expDescription);


    engine=new QScriptEngine();


    ui->setupUi(this);
    findDlg=new QFESPIMB040FindDialog(this);
    replaceDlg=new QFESPIMB040ReplaceDialog(this);

    highlighter=new QFQtScriptHighlighter("", ui->edtScript->getEditor()->document());

    completer = new QCompleter(ui->edtScript->getEditor());
    completermodel=modelFromFile(ProgramOptions::getInstance()->getAssetsDirectory()+"/qtscript/completer.txt");
    completer->setModel(completermodel);
    completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setWrapAround(false);
    ui->edtScript->getEditor()->setCompleter(completer);

    recentMaskFiles=new QRecentFilesMenu(this);
    recentMaskFiles->setUseSystemFileIcons(false);
    recentMaskFiles->setAlwaysEnabled(true);
    connect(recentMaskFiles, SIGNAL(openRecentFile(QString)), this, SLOT(openScriptNoAsk(QString)));
    ui->btnOpen->setMenu(recentMaskFiles);
    connect(ui->edtScript->getEditor(), SIGNAL(cursorPositionChanged()), this, SLOT(edtScript_cursorPositionChanged()));



    cutAct = new QFActionWithNoMenuRole(QIcon(":/spimb040/script_cut.png"), tr("Cu&t"), this);
    cutAct->setShortcut(tr("Ctrl+X"));
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(cutAct, SIGNAL(triggered()), ui->edtScript->getEditor(), SLOT(cut()));

    copyAct = new QFActionWithNoMenuRole(QIcon(":/spimb040/script_copy.png"), tr("&Copy"), this);
    copyAct->setShortcut(tr("Ctrl+C"));
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, SIGNAL(triggered()), ui->edtScript->getEditor(), SLOT(copy()));

    pasteAct = new QFActionWithNoMenuRole(QIcon(":/spimb040/script_paste.png"), tr("&Paste"), this);
    pasteAct->setShortcut(tr("Ctrl+V"));
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, SIGNAL(triggered()), ui->edtScript->getEditor(), SLOT(paste()));

    undoAct = new QFActionWithNoMenuRole(QIcon(":/spimb040/script_undo.png"), tr("&Undo"), this);
    undoAct->setShortcut(tr("Ctrl+Z"));
    undoAct->setStatusTip(tr("Undo the last change "));
    connect(undoAct, SIGNAL(triggered()), ui->edtScript->getEditor(), SLOT(undo()));

    redoAct = new QFActionWithNoMenuRole(QIcon(":/spimb040/script_redo.png"), tr("&Redo"), this);
    redoAct->setShortcut(tr("Ctrl+Shift+Z"));
    redoAct->setStatusTip(tr("Redo the last undone change "));
    connect(redoAct, SIGNAL(triggered()), ui->edtScript->getEditor(), SLOT(redo()));

    findAct = new QFActionWithNoMenuRole(QIcon(":/spimb040/script_find.png"), tr("&Find ..."), this);
    findAct->setShortcut(tr("Ctrl+F"));
    findAct->setStatusTip(tr("Find a string in sequence "));
    connect(findAct, SIGNAL(triggered()), this, SLOT(findFirst()));

    findNextAct = new QFActionWithNoMenuRole(QIcon(":/spimb040/script_find_next.png"), tr("Find &next"), this);
    findNextAct->setShortcut(tr("F3"));
    findNextAct->setStatusTip(tr("Find the next occurence "));
    connect(findNextAct, SIGNAL(triggered()), this, SLOT(findNext()));
    findNextAct->setEnabled(false);

    replaceAct = new QFActionWithNoMenuRole(QIcon(":/spimb040/script_find_replace.png"), tr("Find && &replace ..."), this);
    replaceAct->setShortcut(tr("Ctrl+R"));
    replaceAct->setStatusTip(tr("Find a string in sequence and replace it with another string "));
    connect(replaceAct, SIGNAL(triggered()), this, SLOT(replaceFirst()));

    commentAct = new QFActionWithNoMenuRole(tr("&Comment text"), this);
    commentAct->setShortcut(tr("Ctrl+B"));
    commentAct->setStatusTip(tr("add (single line) comment at the beginning of each line "));
    connect(commentAct, SIGNAL(triggered()), ui->edtScript->getEditor(), SLOT(comment()));

    unCommentAct = new QFActionWithNoMenuRole(tr("&Uncomment text"), this);
    unCommentAct->setShortcut(tr("Ctrl+Shift+B"));
    unCommentAct->setStatusTip(tr("remove (single line) comment at the beginning of each line "));
    connect(unCommentAct, SIGNAL(triggered()), ui->edtScript->getEditor(), SLOT(uncomment()));

    indentAct = new QFActionWithNoMenuRole(QIcon(":/spimb040/script_indent.png"), tr("&Increase indention"), this);
    commentAct->setShortcut(tr("Ctrl+I"));
    indentAct->setStatusTip(tr("increase indention "));
    connect(indentAct, SIGNAL(triggered()), ui->edtScript->getEditor(), SLOT(indentInc()));

    unindentAct = new QFActionWithNoMenuRole(QIcon(":/spimb040/script_unindent.png"), tr("&Decrease indention"), this);
    unindentAct->setShortcut(tr("Ctrl+Shift+I"));
    unindentAct->setStatusTip(tr("decrease indention "));
    connect(unindentAct, SIGNAL(triggered()), ui->edtScript->getEditor(), SLOT(indentDec()));

    gotoLineAct = new QFActionWithNoMenuRole(tr("&Goto line ..."), this);
    gotoLineAct->setShortcut(tr("Alt+G"));
    gotoLineAct->setStatusTip(tr("goto a line in the opened file "));
    connect(gotoLineAct, SIGNAL(triggered()), this, SLOT(gotoLine()));

    printAct = new QFActionWithNoMenuRole(QIcon(":/spimb040/script_print.png"), tr("&Print ..."), this);
    printAct->setStatusTip(tr("print the current SDFF file "));
    connect(printAct, SIGNAL(triggered()), this, SLOT(print()));

    cutAct->setEnabled(false);
    copyAct->setEnabled(false);
    undoAct->setEnabled(false);
    redoAct->setEnabled(false);
    connect(ui->edtScript->getEditor(), SIGNAL(copyAvailable(bool)), cutAct, SLOT(setEnabled(bool)));
    connect(ui->edtScript->getEditor(), SIGNAL(copyAvailable(bool)), copyAct, SLOT(setEnabled(bool)));
    connect(ui->edtScript->getEditor(), SIGNAL(undoAvailable(bool)), undoAct, SLOT(setEnabled(bool)));
    connect(ui->edtScript->getEditor(), SIGNAL(redoAvailable(bool)), redoAct, SLOT(setEnabled(bool)));
    connect(ui->edtScript->getEditor(), SIGNAL(findNextAvailable(bool)), findNextAct, SLOT(setEnabled(bool)));


    QMenu* menuMore=new QMenu(ui->tbMoreOptions);
    menuMore->addAction(indentAct);
    menuMore->addAction(unindentAct);
    menuMore->addAction(commentAct);
    menuMore->addAction(unCommentAct);
    menuMore->addSeparator();
    menuMore->addAction(gotoLineAct);
    menuMore->addAction(findAct);
    menuMore->addAction(replaceAct);
    menuMore->addAction(findNextAct);
    ui->tbMoreOptions->setMenu(menuMore);
    ui->tbFind->setDefaultAction(findAct);
    ui->tbFindNext->setDefaultAction(findNextAct);
    ui->tbReplace->setDefaultAction(replaceAct);
    ui->tbPrint->setDefaultAction(printAct);
    ui->tbCopy->setDefaultAction(copyAct);
    ui->tbCut->setDefaultAction(cutAct);
    ui->tbPaste->setDefaultAction(pasteAct);
    ui->tbRedo->setDefaultAction(redoAct);
    ui->tbUndo->setDefaultAction(undoAct);



    updateReplaces();
    //bindLineEdit(ui->edtPrefix1);
    ui->btnCancel->setVisible(false);
    ui->widProgress->setVisible(false);
    ui->labStatus->setVisible(false);
    setScriptFilename(tr("new_acquisition_script.js"));


    QDir d(QFPluginServices::getInstance()->getPluginHelpDirectory("ext_spimb040")+"acquisition_script/");
    QStringList filter;
    filter<<"*.html"<<"*.htm"<<"*.txt";
    QStringList files=d.entryList(filter, QDir::Files);
    threadsFinished=0;
    maxThreads=2;
    QList<QStringList> absFiles;

    for (int i=0; i<maxThreads; i++) {
        QStringList sl;
        absFiles.push_back(sl);
    }
    for (int i=0; i<files.size(); i++) {
        QString file=d.absoluteFilePath(files[i]);
        absFiles[i%maxThreads].append(file);
    }
    for (int i=0; i<maxThreads; i++) {
        threads.append(new QFESPIMB040ScriptedAcquisitionDocSearchThread(absFiles[i], this));
        connect(threads[i], SIGNAL(finished()), this, SLOT(threadFinished()));
        connect(threads[i], SIGNAL(foundFunction(QString,QString,QString)), this, SLOT(addFunction(QString,QString,QString)));
    }
    QTimer::singleShot(10, this, SLOT(delayedStartSearchThreads()));
}

QFESPIMB040ScriptedAcquisition::~QFESPIMB040ScriptedAcquisition()
{
    for (int i=0; i<threads.size(); i++) {
        if (threads[i]->isRunning()) {
            threads[i]->stopThread();
            threads[i]->wait();
        }
    }
    delete ui;
    delete engine;
}

QString QFESPIMB040ScriptedAcquisition::getScript() const
{
    return ui->edtScript->getEditor()->toPlainText();
}

void QFESPIMB040ScriptedAcquisition::loadSettings(QSettings &settings, QString prefix)
{
    lastScript=settings.value(prefix+"script", tr("tools.logText(\"Hello World!\\n\");")).toString();
    ui->edtScript->getEditor()->setPlainText(lastScript);
    recentMaskFiles->readSettings(settings, prefix+"recentScripts/");
    loadSplitter(settings, ui->splitter, prefix+"splitter");
}

void QFESPIMB040ScriptedAcquisition::storeSettings(QSettings &settings, QString prefix) const
{
    settings.setValue(prefix+"script", ui->edtScript->getEditor()->toPlainText());
    recentMaskFiles->storeSettings(settings, prefix+"recentScripts/");
    saveSplitter(settings, ui->splitter, prefix+"splitter");
}


void QFESPIMB040ScriptedAcquisition::on_btnExecute_clicked()
{
    emit doAcquisition();
    QApplication::processEvents();
    performAcquisition();
}

void QFESPIMB040ScriptedAcquisition::on_btnCancel_clicked()
{
    engine->abortEvaluation();;
}

void QFESPIMB040ScriptedAcquisition::updateReplaces()
{
    setGlobalReplaces(opticsSetup, expDescription, acqDescription);
    //setReplaceValue("acquisition1_name",  cleanStringForFilename(ui->cmbAcquisitionSettings1->currentConfigName()));
    //setReplaceValue("acquisition2_name",  cleanStringForFilename(ui->cmbAcquisitionSettings2->currentConfigName()));
}

void QFESPIMB040ScriptedAcquisition::on_btnNew_clicked()
{
    if (maybeSave()) {
        ui->edtScript->getEditor()->setPlainText("");
        setScriptFilename(tr("new_acquisition_script.js"));
        lastScript=ui->edtScript->getEditor()->toPlainText();
    }
}

void QFESPIMB040ScriptedAcquisition::on_btnOpen_clicked()
{
    if (opticsSetup->getStopRelease(0)) opticsSetup->getStopRelease(0)->stop();
    if (opticsSetup->getStopRelease(1)) opticsSetup->getStopRelease(1)->stop();
    openScript("last", true);
    if (opticsSetup->getStopRelease(0)) opticsSetup->getStopRelease(0)->resume();
    if (opticsSetup->getStopRelease(1)) opticsSetup->getStopRelease(1)->resume();
}

void QFESPIMB040ScriptedAcquisition::on_btnSave_clicked()
{
    if (opticsSetup->getStopRelease(0)) opticsSetup->getStopRelease(0)->stop();
    if (opticsSetup->getStopRelease(1)) opticsSetup->getStopRelease(1)->stop();
    QString tdir=ProgramOptions::getInstance()->getConfigValue("spimb040/script_directory", ProgramOptions::getInstance()->getHomeQFDirectory()+"/acquisitionScripts/").toString();
    QDir().mkpath(tdir);
    QString dir=ProgramOptions::getInstance()->getQSettings()->value("QFESPIMB040ScriptedAcquisition/lastScriptDir", tdir).toString();
    QDir d(dir);
    QString filename=qfGetSaveFileName(this, tr("save acquisition script ..."), d.absoluteFilePath(currentScript), tr("acquisition script (*.js)"));
    if (!filename.isEmpty()) {
        bool ok=true;
        /*if (QFile::exists(filename)) {
            ok=false;
            if (QMessageBox::question(this, tr("save acquisition script ..."), tr("The file\n  '%1'\nalready exists. Overwrite?").arg(filename), QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::Yes) {
                ok=true;
            }
        }*/
        if (ok) {
            QFile f(filename);
            if (f.open(QIODevice::WriteOnly|QIODevice::Text)) {
                QTextStream s(&f);
                s<<ui->edtScript->getEditor()->toPlainText().toUtf8();
                lastScript=ui->edtScript->getEditor()->toPlainText();
                f.close();
                setScriptFilename(filename);
                dir=QFileInfo(filename).absolutePath();

            }

        }
    }
    ProgramOptions::getInstance()->getQSettings()->setValue("QFESPIMB040ExperimentDescription/lasttemplatedir", dir);
    if (opticsSetup->getStopRelease(0)) opticsSetup->getStopRelease(0)->resume();
    if (opticsSetup->getStopRelease(1)) opticsSetup->getStopRelease(1)->resume();
}


#define ADD_SCRIPT_FUNCTION(engine, function, name) { \
    QScriptValue fun = engine->newFunction(function); \
    engine->globalObject().setProperty(name, fun); \
  }
#define ADD_SCRIPT_QOBJECT(engine, someObject, name) { \
     QScriptValue objectValue = engine->newQObject(someObject);\
     engine->globalObject().setProperty(name, objectValue);\
   }
#define ADD_SCRIPT_QFOBJECT(engine, someObject, name) { \
    someObject->setEngine(engine); \
     QScriptValue objectValue = engine->newQObject(someObject);\
     engine->globalObject().setProperty(name, objectValue);\
   }

void QFESPIMB040ScriptedAcquisition::performAcquisition()
{
    ui->btnCancel->setVisible(true);
    ui->widProgress->setVisible(true);
    ui->btnExecute->setEnabled(false);
    ui->widProgress->setSpin(true);
    ui->edtScript->getEditor()->setEnabled(false);
    ui->btnNew->setEnabled(false);
    ui->btnOpen->setEnabled(false);
    ui->btnOpenExample->setEnabled(false);
    ui->btnOpenTemplate->setEnabled(false);
    ui->labStatus->setVisible(true);
    ui->widProgress->setMode(QModernProgressWidget::GradientRing);
    QString script=ui->edtScript->getEditor()->toPlainText();
    log->log_text(tr("\n\n====================================================================================\n"));
    log->log_text(tr("== SCRIPTED ACQUISITION                                                           ==\n"));
    log->log_text(tr("====================================================================================\n"));
    log->log_text(tr("Script:\n%1\n").arg(script));
    log->log_text(tr("====================================================================================\n"));
    QScriptSyntaxCheckResult checkResult=QScriptEngine::checkSyntax(script);
    if (checkResult.state()==QScriptSyntaxCheckResult::Valid) {
        log->log_text(tr("  - script syntax check OK\n"));
        ui->labStatus->setText(tr("script syntax check OK ..."));
        log->log_text(tr("  - initializing script engine\n"));
        ui->labStatus->setText(tr("initializing script engine ..."));
        delete engine;
        engine=new QScriptEngine();
        engine->setProcessEventsInterval(50);
        engine->clearExceptions();
        engine->collectGarbage();
        log->log_text(tr("  - registering script objects\n"));
        ui->labStatus->setText(tr("registering script objects ..."));

        ADD_SCRIPT_QFOBJECT(engine, acquisitionTools, "tools");
        ADD_SCRIPT_QFOBJECT(engine, instrumentControl, "instrument");
        ADD_SCRIPT_QFOBJECT(engine, acquisitionControl, "acquisition");

        ui->labStatus->setText(tr("running script ..."));
        log->log_text(tr("  - running script\n"));
        QScriptValue result = engine->evaluate(script);
        if (engine->hasUncaughtException()) {
            int line = engine->uncaughtExceptionLineNumber();
            log->log_error(tr("\n\n  - SCRIPT ERROR: uncaught exception at line %1: %2").arg(line).arg(result.toString()));
        } else {
            log->log_text(tr("\n\n  - SCRIPT FINISHED SUCCESSFULLY: result %1\n").arg(result.toString()));
        }
    } else {
        if (checkResult.state()==QScriptSyntaxCheckResult::Error) {
            log->log_error(tr("Error in script (l. %1, col. %2): %3").arg(checkResult.errorLineNumber()).arg(checkResult.errorColumnNumber()).arg(checkResult.errorMessage()));
        } else  {
            log->log_error(tr("Incomplete script (l. %1, col. %2): %3").arg(checkResult.errorLineNumber()).arg(checkResult.errorColumnNumber()).arg(checkResult.errorMessage()));
        }
    }
    log->log_text(tr("====================================================================================\n"));
    log->log_text(tr("== SCRIPTED ACQUISITION ... DONE!!!                                               ==\n"));
    log->log_text(tr("====================================================================================\n\n\n"));
    ui->btnCancel->setVisible(false);
    ui->widProgress->setVisible(false);
    ui->btnExecute->setEnabled(true);
    ui->edtScript->getEditor()->setEnabled(true);
    ui->btnNew->setEnabled(true);
    ui->btnOpen->setEnabled(true);
    ui->btnOpenExample->setEnabled(true);
    ui->btnOpenTemplate->setEnabled(true);
    ui->labStatus->setVisible(false);
}

void QFESPIMB040ScriptedAcquisition::setStatus(const QString &text)
{
    ui->labStatus->setText(text);
}

void QFESPIMB040ScriptedAcquisition::setStatusProgressRange(double minimum, double maximum)
{
    ui->progress->setRange(minimum, maximum);
}

void QFESPIMB040ScriptedAcquisition::setStatusProgress(double value)
{
    ui->progress->setPercentageMode(QModernProgressWidget::Percent);
    ui->progress->setDisplayPercent(true);
    ui->progress->setValue(value);
}

void QFESPIMB040ScriptedAcquisition::incStatusProgress()
{
    ui->progress->setPercentageMode(QModernProgressWidget::Percent);
    ui->progress->setDisplayPercent(true);
    ui->progress->setValue(ui->progress->value()+1.0);
}

void QFESPIMB040ScriptedAcquisition::incStatusProgress(double value)
{
    ui->progress->setPercentageMode(QModernProgressWidget::Percent);
    ui->progress->setDisplayPercent(true);
    ui->progress->setValue(ui->progress->value()+value);
}


void QFESPIMB040ScriptedAcquisition::on_btnSyntaxCheck_clicked()
{
    QString script=ui->edtScript->getEditor()->toPlainText();
    QScriptSyntaxCheckResult checkResult=QScriptEngine::checkSyntax(script);
    if (checkResult.state()==QScriptSyntaxCheckResult::Valid) {
        log->log_text(tr("\n\nScript syntax check OK\n"));
    } else {
        if (checkResult.state()==QScriptSyntaxCheckResult::Error) log->log_error(tr("Error in script (l. %1, col. %2): %3").arg(checkResult.errorLineNumber()).arg(checkResult.errorColumnNumber()).arg(checkResult.errorMessage()));
        else  log->log_error(tr("Incomplete script (l. %1, col. %2): %3").arg(checkResult.errorLineNumber()).arg(checkResult.errorColumnNumber()).arg(checkResult.errorMessage()));
    }
}

void QFESPIMB040ScriptedAcquisition::on_btnOpenExample_clicked()
{
    if (opticsSetup->getStopRelease(0)) opticsSetup->getStopRelease(0)->stop();
    if (opticsSetup->getStopRelease(1)) opticsSetup->getStopRelease(1)->stop();
    openScript(ProgramOptions::getInstance()->getAssetsDirectory()+"/plugins/spimb040/acquisitionScriptExamples/", false);
    if (opticsSetup->getStopRelease(0)) opticsSetup->getStopRelease(0)->resume();
    if (opticsSetup->getStopRelease(1)) opticsSetup->getStopRelease(1)->resume();
}

void QFESPIMB040ScriptedAcquisition::on_btnOpenTemplate_clicked()
{
    if (opticsSetup->getStopRelease(0)) opticsSetup->getStopRelease(0)->stop();
    if (opticsSetup->getStopRelease(1)) opticsSetup->getStopRelease(1)->stop();
    openScript(ProgramOptions::getInstance()->getAssetsDirectory()+"/plugins/spimb040/acquisitionScriptTemplates/", false);
    if (opticsSetup->getStopRelease(0)) opticsSetup->getStopRelease(0)->resume();
    if (opticsSetup->getStopRelease(1)) opticsSetup->getStopRelease(1)->resume();
}

void QFESPIMB040ScriptedAcquisition::edtScript_cursorPositionChanged()
{
    QTextCursor tc = ui->edtScript->getEditor()->textCursor();
    /*
    tc.select(QTextCursor::WordUnderCursor);
    QString text=tc.selectedText();

    QString word=text.toLower();*/


    QString text=ui->edtScript->getEditor()->toPlainText();
    QString word;
    int newPos=tc.position();
    if (newPos>=0 && newPos<text.size()) {
        word+=text[newPos];
        int p=newPos-1;
        while (p>=0 && (text[p].isLetterOrNumber()||text[p]=='_'||text[p]=='.')) {
            word=text[p]+word;
            p--;
        }
        p=newPos+1;
        while (p<text.size() && (text[p].isLetterOrNumber()||text[p]=='_'||text[p]=='.')) {
            word=word+text[p];
            p++;
        }
        word=word.toLower();
    }



    if (functionhelp.contains(word)) {
        ui->labHelp->setText(getFunctionHelp(word));
        //ui->labTemplate->setText(tr("<tt>%1</tt>").arg(getFunctionTemplate(word)));
    } else {
        ui->labHelp->setText(tr("no help for '%1'' available ...").arg(word));
    }

    ui->labCursorPos->setText(tr("Line %1, Column %2").arg(ui->edtScript->getEditor()->getLineNumber()).arg(ui->edtScript->getEditor()->getColumnNumber()));
}

void QFESPIMB040ScriptedAcquisition::on_btnHelp_clicked()
{
    m_pluginServices->displayHelpWindow(m_pluginServices->getPluginHelpDirectory("ext_spimb040")+"/acquisition_script.html");
}

bool QFESPIMB040ScriptedAcquisition::maybeSave() {
    if (ui->edtScript->getEditor()->toPlainText().isEmpty()) return true;
    if (ui->edtScript->getEditor()->toPlainText()==lastScript) return true;
    int r=QMessageBox::question(this, tr("save acquisition script ..."), tr("The current script has not been saved.\n  Delete?\n    Yes: Any changes will be lost.\n    No: You will be asked for a filename for the script.\n    Cancel: return to editing the script."), QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel, QMessageBox::No);
    if (r==QMessageBox::Yes) {
        return true;
    } else if (r==QMessageBox::No) {
        on_btnSave_clicked();
        return true;
    }

    return false;
}

void QFESPIMB040ScriptedAcquisition::setScriptFilename(QString filename)
{
    currentScript=filename;
    recentMaskFiles->addRecentFile(filename);
    ui->labScriptFilename->setText(tr("current script: <tt><i>%1</i></tt>").arg(QFileInfo(filename).fileName()));
}

QString QFESPIMB040ScriptedAcquisition::getFunctionTemplate(QString name)
{
    return functionhelp.value(name.toLower(), qMakePair(name+"()", QString())).first;
}

QString QFESPIMB040ScriptedAcquisition::getFunctionHelp(QString name)
{
    return functionhelp.value(name.toLower(), qMakePair(QString(""), tr("<b><tt>%1(...)</tt></b> - <i>function </i>:<br>no help available").arg(name))).second;
}

QStringListModel *QFESPIMB040ScriptedAcquisition::modelFromFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly))
     return new QStringListModel(completer);

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QStringList words;

    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        if (!line.isEmpty()) {
            //QMessageBox::information(this, "", line.trimmed());
            words << line.trimmed();
        }
    }
    words.sort();
    //QMessageBox::information(this, "", words.join(", "));
    QApplication::restoreOverrideCursor();
    return new QStringListModel(words, completer);
}


void QFESPIMB040ScriptedAcquisition::openScript(QString dir, bool saveDir) {
    if (maybeSave()) {
        if (dir=="last") {
            QString tdir=ProgramOptions::getInstance()->getConfigValue("spimb040/script_directory", ProgramOptions::getInstance()->getHomeQFDirectory()+"/acquisitionScripts/").toString();
            QDir().mkpath(tdir);
            dir=ProgramOptions::getInstance()->getQSettings()->value("QFESPIMB040ScriptedAcquisition/lastScriptDir", tdir).toString();
        }
        QString filename=qfGetOpenFileName(this, tr("open script ..."), dir, tr("acquisition script (*.js)"))    ;
        if (QFile::exists(filename)) {
            QFile f(filename);
            if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
                ui->edtScript->getEditor()->setPlainText(QString::fromUtf8(f.readAll()));
                setScriptFilename(filename);
                f.close();
                lastScript=ui->edtScript->getEditor()->toPlainText();
                dir=QFileInfo(filename).absolutePath();

            }
        }
        if (saveDir) ProgramOptions::getInstance()->getQSettings()->setValue("QFESPIMB040ScriptedAcquisition/lastScriptDir", dir);
    }

}

void QFESPIMB040ScriptedAcquisition::openScriptNoAsk(QString filename)
{
    if (maybeSave()) {
        if (QFile::exists(filename)) {
            QFile f(filename);
            if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
                ui->edtScript->getEditor()->setPlainText(QString::fromUtf8(f.readAll()));
                setScriptFilename(filename);
                f.close();
                lastScript=ui->edtScript->getEditor()->toPlainText();
            }
        }
    }
}

void QFESPIMB040ScriptedAcquisition::threadFinished()
{
    threadsFinished++;
    if (threadsFinished>=maxThreads) {
        ui->progress->setSpin(false);
        ui->progress->setVisible(false);
        ui->labProgress->setVisible(false);
    }
    highlighter->setSpecialFunctions(specialFunctions);
}

void QFESPIMB040ScriptedAcquisition::delayedStartSearchThreads()
{
    helpModel.setStringList(defaultWords);
    ui->progress->setVisible(true);
    ui->progress->setSpin(true);
    ui->labProgress->setVisible(true);
    threadsFinished=0;
    for (int i=0; i<threads.size(); i++) {
        threads[i]->start();
    }
}

void QFESPIMB040ScriptedAcquisition::addFunction(QString name, QString templ, QString help)
{
    QString templ1=templ;
    templ1=templ1.replace(QLatin1String("<!--\xB0-->"), "");
    functionhelp[name.toLower()]=qMakePair(templ1, help);
    specialFunctions<<name;
    QStringList csl=completermodel->stringList();

    if (templ.size()>0) {
        QString t=templ;
        t=t.replace(QLatin1String("<!--\xB0-->"), QLatin1String("\xB0"));
        csl<<t;
    } else {
        csl<<QString(name+QLatin1String("(\xB0)"));
    }

    QStringList sl;
    /*sl=compExpression->stringlistModel()->stringList();
    if (!sl.contains(name)) sl.append(name);
    if (!sl.contains(templ)) sl.append(templ);
    sl.sort();
    compExpression->stringlistModel()->setStringList(sl);*/

    completermodel->setStringList(csl);
    sl=helpModel.stringList();
    sl.removeAll(name);
    sl.removeAll(templ1);
    sl.append(templ1);
    sl.sort();
    helpModel.setStringList(sl);
    //qDebug()<<csl;
}

void QFESPIMB040ScriptedAcquisition::findFirst()
{
    if (ui->edtScript->getEditor()->hasSelection()) findDlg->setPhrase(ui->edtScript->getEditor()->getSelection());
    if (findDlg->exec()==QDialog::Accepted) {
        // enable "Find next" action
        findNextAct->setEnabled(true);
        findNextAct->setIcon(QIcon(":/spimb040/script_find_next.png"));
        findNextAct->setText(tr("Find &next"));
        findNextAct->setStatusTip(tr("Find the next occurence "));
        disconnect(findNextAct, SIGNAL(triggered()), this, 0);
        connect(findNextAct, SIGNAL(triggered()), this, SLOT(findNext()));

        if (!ui->edtScript->getEditor()->findFirst(findDlg->getPhrase(), findDlg->getSearchFromStart(), findDlg->getMatchCase(), findDlg->getWholeWords())) {
            QMessageBox::information(this, tr("Find ..."),
                             tr("Did not find '%1' ...")
                             .arg(ui->edtScript->getEditor()->getPhrase()));
            findNextAct->setEnabled(false);
        }
    }
}

void QFESPIMB040ScriptedAcquisition::findNext()
{
    if (!ui->edtScript->getEditor()->findNext()) {
        QMessageBox::information(this, tr("Find ..."),
                         tr("Did not find '%1' ...")
                         .arg(ui->edtScript->getEditor()->getPhrase()));
    }
}

void QFESPIMB040ScriptedAcquisition::replaceFirst()
{
    if (ui->edtScript->getEditor()->hasSelection()) replaceDlg->setPhrase(ui->edtScript->getEditor()->getSelection());
    if (replaceDlg->exec()==QDialog::Accepted) {
        // enable "Find next" action
        findNextAct->setEnabled(true);
        findNextAct->setIcon(QIcon(":/spimb040/script_find_replace_next.png"));
        findNextAct->setText(tr("Replace &next"));
        findNextAct->setStatusTip(tr("Replace the next occurence "));
        disconnect(findNextAct, SIGNAL(triggered()), this, 0);
        connect(findNextAct, SIGNAL(triggered()), this, SLOT(replaceNext()));

        if (!ui->edtScript->getEditor()->replaceFirst(replaceDlg->getPhrase(), replaceDlg->getReplace(), replaceDlg->getSearchFromStart(), replaceDlg->getMatchCase(), replaceDlg->getWholeWords(), replaceDlg->getReplaceAll(), replaceDlg->getAskBeforeReplace())) {
            QMessageBox::information(this, tr("Find & Replace..."),
                             tr("Did not find '%1' ...")
                             .arg(ui->edtScript->getEditor()->getPhrase()));
        }
    }
}

void QFESPIMB040ScriptedAcquisition::replaceNext()
{
    if (! ui->edtScript->getEditor()->replaceNext()) {
        QMessageBox::information(this, tr("Find & Replace ..."),
                                 tr("Did not find '%1' ...")
                                 .arg(ui->edtScript->getEditor()->getPhrase()));
    }

}

void QFESPIMB040ScriptedAcquisition::gotoLine()
{
    int maxLine=ui->edtScript->getEditor()->document()->blockCount();
    bool ok;
    unsigned long line = QInputDialog::getInt(this, tr("Goto Line ..."),
                              tr("Enter a line number (1 - %2):").arg(maxLine), 1, 1, maxLine, 1, &ok);
    if (ok) {
        ui->edtScript->getEditor()->gotoLine(line);
        ui->edtScript->getEditor()->setFocus();
    }

}

void QFESPIMB040ScriptedAcquisition::print()
{
#ifndef QT_NO_PRINTER
    if (opticsSetup->getStopRelease(0)) opticsSetup->getStopRelease(0)->stop();
    if (opticsSetup->getStopRelease(1)) opticsSetup->getStopRelease(1)->stop();
   QPrinter printer;

   QPrintDialog *dialog = new QPrintDialog(&printer, this);
   dialog->setWindowTitle(tr("Print Document"));
   if (ui->edtScript->getEditor()->textCursor().hasSelection())
       dialog->addEnabledOption(QAbstractPrintDialog::PrintSelection);
   if (dialog->exec() != QDialog::Accepted)
       return;

   ui->edtScript->getEditor()->print(&printer);
   if (opticsSetup->getStopRelease(0)) opticsSetup->getStopRelease(0)->resume();
   if (opticsSetup->getStopRelease(1)) opticsSetup->getStopRelease(1)->resume();
#endif

}

void QFESPIMB040ScriptedAcquisition::printPreviewClick()
{
#ifndef QT_NO_PRINTER
    if (opticsSetup->getStopRelease(0)) opticsSetup->getStopRelease(0)->stop();
    if (opticsSetup->getStopRelease(1)) opticsSetup->getStopRelease(1)->stop();
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, this);
    connect(&preview, SIGNAL(paintRequested(QPrinter *)), SLOT(printPreview(QPrinter *)));
    preview.exec();
    if (opticsSetup->getStopRelease(0)) opticsSetup->getStopRelease(0)->resume();
    if (opticsSetup->getStopRelease(1)) opticsSetup->getStopRelease(1)->resume();
#endif
}

void QFESPIMB040ScriptedAcquisition::printPreview(QPrinter *printer)
{
#ifndef QT_NO_PRINTER
    ui->edtScript->getEditor()->print(printer);
#endif

}

void QFESPIMB040ScriptedAcquisition::clearFindActions()
{
    findNextAct->setEnabled(false);
}


