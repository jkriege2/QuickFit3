#include "qfehelpeditorwidget.h"
#include "ui_qfehelpeditorwidget.h"

#include "qfpluginservices.h"
#include "qfextension.h"
#include <QtGui>
#include <QtCore>
#include "qfcompleterfromfile.h"
#include <QTemporaryFile>
#include "qftools.h"
#include "pasteimagedlg.h"









QFEHelpEditorWidget::QFEHelpEditorWidget(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::QFEHelpEditorWidget)
{
    ui->setupUi(this);
    findDlg=new FindDialog(this);
    replaceDlg=new ReplaceDialog(this);

    highlighter=new QFHTMLHighlighter("", ui->edtScript->getEditor()->document());

    completer = new QCompleter(ui->edtScript->getEditor());
    completermodel=modelFromFile(ProgramOptions::getInstance()->getAssetsDirectory()+"/qtscript/completer.txt");
    completer->setModel(completermodel);
    completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setWrapAround(false);
    ui->edtScript->getEditor()->setCompleter(completer);

    recentHelpFiles=new QRecentFilesMenu(this);
    recentHelpFiles->setUseSystemFileIcons(false);
    recentHelpFiles->setAlwaysEnabled(true);
    connect(recentHelpFiles, SIGNAL(openRecentFile(QString)), this, SLOT(openScriptNoAsk(QString)));
    ui->btnOpen->setMenu(recentHelpFiles);
    connect(ui->edtScript->getEditor(), SIGNAL(cursorPositionChanged()), this, SLOT(edtScript_cursorPositionChanged()));



    cutAct = new QAction(QIcon(":/qfe_helpeditor/script_cut.png"), tr("Cu&t"), this);
    cutAct->setShortcut(tr("Ctrl+X"));
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(cutAct, SIGNAL(triggered()), ui->edtScript->getEditor(), SLOT(cut()));

    copyAct = new QAction(QIcon(":/qfe_helpeditor/script_copy.png"), tr("&Copy"), this);
    copyAct->setShortcut(tr("Ctrl+C"));
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, SIGNAL(triggered()), ui->edtScript->getEditor(), SLOT(copy()));

    pasteAct = new QAction(QIcon(":/qfe_helpeditor/script_paste.png"), tr("&Paste"), this);
    pasteAct->setShortcut(tr("Ctrl+V"));
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, SIGNAL(triggered()), ui->edtScript->getEditor(), SLOT(paste()));

    undoAct = new QAction(QIcon(":/qfe_helpeditor/script_undo.png"), tr("&Undo"), this);
    undoAct->setShortcut(tr("Ctrl+Z"));
    undoAct->setStatusTip(tr("Undo the last change "));
    connect(undoAct, SIGNAL(triggered()), ui->edtScript->getEditor(), SLOT(undo()));

    redoAct = new QAction(QIcon(":/qfe_helpeditor/script_redo.png"), tr("&Redo"), this);
    redoAct->setShortcut(tr("Ctrl+Shift+Z"));
    redoAct->setStatusTip(tr("Redo the last undone change "));
    connect(redoAct, SIGNAL(triggered()), ui->edtScript->getEditor(), SLOT(redo()));

    findAct = new QAction(QIcon(":/qfe_helpeditor/script_find.png"), tr("&Find ..."), this);
    findAct->setShortcut(tr("Ctrl+F"));
    findAct->setStatusTip(tr("Find a string in sequence "));
    connect(findAct, SIGNAL(triggered()), this, SLOT(findFirst()));

    findNextAct = new QAction(QIcon(":/qfe_helpeditor/script_find_next.png"), tr("Find &next"), this);
    findNextAct->setShortcut(tr("F3"));
    findNextAct->setStatusTip(tr("Find the next occurence "));
    connect(findNextAct, SIGNAL(triggered()), this, SLOT(findNext()));
    findNextAct->setEnabled(false);

    replaceAct = new QAction(QIcon(":/qfe_helpeditor/script_find_replace.png"), tr("Find && &replace ..."), this);
    replaceAct->setShortcut(tr("Ctrl+R"));
    replaceAct->setStatusTip(tr("Find a string in sequence and replace it with another string "));
    connect(replaceAct, SIGNAL(triggered()), this, SLOT(replaceFirst()));

    commentAct = new QAction(tr("&Comment text"), this);
    commentAct->setShortcut(tr("Ctrl+B"));
    commentAct->setStatusTip(tr("add (single line) comment at the beginning of each line "));
    connect(commentAct, SIGNAL(triggered()), ui->edtScript->getEditor(), SLOT(comment()));

    unCommentAct = new QAction(tr("&Uncomment text"), this);
    unCommentAct->setShortcut(tr("Ctrl+Shift+B"));
    unCommentAct->setStatusTip(tr("remove (single line) comment at the beginning of each line "));
    connect(unCommentAct, SIGNAL(triggered()), ui->edtScript->getEditor(), SLOT(uncomment()));

    indentAct = new QAction(QIcon(":/qfe_helpeditor/script_indent.png"), tr("&Increase indention"), this);
    commentAct->setShortcut(tr("Ctrl+I"));
    indentAct->setStatusTip(tr("increase indention "));
    connect(indentAct, SIGNAL(triggered()), ui->edtScript->getEditor(), SLOT(indentInc()));

    unindentAct = new QAction(QIcon(":/qfe_helpeditor/script_unindent.png"), tr("&Decrease indention"), this);
    unindentAct->setShortcut(tr("Ctrl+Shift+I"));
    unindentAct->setStatusTip(tr("decrease indention "));
    connect(unindentAct, SIGNAL(triggered()), ui->edtScript->getEditor(), SLOT(indentDec()));

    gotoLineAct = new QAction(tr("&Goto line ..."), this);
    gotoLineAct->setShortcut(tr("Alt+G"));
    gotoLineAct->setStatusTip(tr("goto a line in the opened file "));
    connect(gotoLineAct, SIGNAL(triggered()), this, SLOT(gotoLine()));

    printAct = new QAction(QIcon(":/qfe_helpeditor/script_print.png"), tr("&Print ..."), this);
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



    setScriptFilename(tr("new_help.html"));
    QMenu* menu;
    menu=new QMenu(tr("version/copyright info"), this);
    ui->edtScript->getEditor()->addAction(menu->menuAction());
    addInsertAction(menu, "$$mainhelpdir$$");
    addInsertAction(menu, "$$version.svnrevision$$");
    addInsertAction(menu, "$$version.status$$");
    addInsertAction(menu, "$$version.date$$");
    addInsertAction(menu, "$$version$$");
    addInsertAction(menu, "$$qfInfoVersionFull()$$");
    menu->addSeparator();
    addInsertAction(menu, "$$thanksto$$");
    addInsertAction(menu, "$$copyright$$");
    addInsertAction(menu, "$$author$$");
    addInsertAction(menu, "$$weblink$$");


    menu=new QMenu(tr("page header/footer"), this);
    ui->edtScript->getEditor()->addAction(menu->menuAction());
    addInsertAction(menu, "$$qf_commondoc_backtop$$");
    menu->addSeparator();
    addInsertAction(menu, "$$qf_commondoc_footer.start$$");
    addInsertAction(menu, "$$qf_commondoc_footer.end$$");
    menu->addSeparator();
    addInsertAction(menu, "$$qf_commondoc_header.start$$");
    addInsertAction(menu, "$$qf_commondoc_header.end$$");
    addInsertAction(menu, "$$qf_commondoc_header.end_notitle$$");
    addInsertAction(menu, "$$qf_commondoc_header.rdr$$");
    addInsertAction(menu, "$$qf_commondoc_header.eval$$");
    addInsertAction(menu, "$$qf_commondoc_header.extension$$");
    addInsertAction(menu, "$$qf_commondoc_header.fitfunc$$");
    addInsertAction(menu, "$$qf_commondoc_header.fitalg$$");
    addInsertAction(menu, "$$qf_commondoc_header.separator$$");


    menu=new QMenu(tr("plugin info"), this);
    ui->edtScript->getEditor()->addAction(menu->menuAction());
    addInsertAction(menu, "$$local_plugin_icon$$");
    addInsertAction(menu, "$$local_plugin_iconfilename$$");
    addInsertAction(menu, "$$local_plugin_name$$");
    addInsertAction(menu, "$$local_plugin_author$$");
    addInsertAction(menu, "$$local_plugin_copyright$$");
    addInsertAction(menu, "$$local_plugin_weblink_url$$");
    addInsertAction(menu, "$$local_plugin_weblink$$");
    addInsertAction(menu, "$$local_plugin_id$$");
    addInsertAction(menu, "$$local_plugin_tutorial_file$$");
    addInsertAction(menu, "$$local_plugin_tutorial_link$$");
    addInsertAction(menu, "$$local_plugin_mainhelp_file$$");
    addInsertAction(menu, "$$local_plugin_mainhelp_link$$");
    addInsertAction(menu, "$$local_plugin_version$$");
    addInsertAction(menu, "$$local_plugin_typehelp_link$$");
    addInsertAction(menu, "$$local_plugin_typehelp_file$$");
    menu->addSeparator();
    addInsertAction(menu, "$$plugin_info:help:PLUGINID$$");
    addInsertAction(menu, "$$plugin_info:tutorial:PLUGINID$$");


    menu=new QMenu(tr("plugin lists"), this);
    ui->edtScript->getEditor()->addAction(menu->menuAction());
    addInsertAction(menu, "$$list:extension:INTERFACENAME$$");
    addInsertAction(menu, "$$list:fitfunc:STARTSWITH$$");
    addInsertAction(menu, "$$list:fitalg:$$");


    menu=new QMenu(tr("help lists"), this);
    ui->edtScript->getEditor()->addAction(menu->menuAction());
    addInsertAction(menu, "$$tutorials_contents$$");
    addInsertAction(menu, "$$help_contents$$");


    menu=new QMenu(tr("insert files"), this);
    ui->edtScript->getEditor()->addAction(menu->menuAction());
    addInsertAction(menu, "$$insert:FILENAME$$");
    addInsertAction(menu, "$$insertglobal:FILENAME$$");


    menu=new QMenu(tr("insert Equations (LaTeX)"), this);
    ui->edtScript->getEditor()->addAction(menu->menuAction());
    addInsertAction(menu, "$$math:LATEX$$");
    addInsertAction(menu, "$$bmath:LATEX$$");


    menu=new QMenu(tr("insert Literature References"), this);
    ui->edtScript->getEditor()->addAction(menu->menuAction());
    addInsertAction(menu, "$$ref:<ID>:Text$$");
    addInsertAction(menu, "$$invisibleref:<ID>:Text$$");
    addInsertAction(menu, "$$references$$");


    menu=new QMenu(tr("insert Additional Markup"), this);
    ui->edtScript->getEditor()->addAction(menu->menuAction());
    addInsertAction(menu, "$$fig:FILENAME:CAPTION$$");
    addInsertAction(menu, "$$see:Text$$");
    addInsertAction(menu, "$$note:Text$$");
    addInsertAction(menu, "$$info:Text$$");
    addInsertAction(menu, "$$warning:Text$$");
    //addInsertAction(menu, "$$$$");

}

QFEHelpEditorWidget::~QFEHelpEditorWidget()
{
    delete ui;

}

QString QFEHelpEditorWidget::getScript() const
{
    return ui->edtScript->getEditor()->toPlainText();
}

void QFEHelpEditorWidget::loadSettings(QSettings &settings, QString prefix)
{
    recentHelpFiles->readSettings(settings, prefix+"recentScripts/");
}

void QFEHelpEditorWidget::storeSettings(QSettings &settings, QString prefix) const
{
    recentHelpFiles->storeSettings(settings, prefix+"recentScripts/");
}


void QFEHelpEditorWidget::on_btnExecute_clicked()
{
    QTemporaryFile f;
    QFileInfo fi(currentScript);
    if (QFile::exists(currentScript)) {
        f.setFileTemplate(fi.absolutePath()+"/qfe_helpeditor_temp_XXXXXX.html.tmp");
    }
    if (f.open()) {
        QTextStream txt(&f);
        txt<<ui->edtScript->getEditor()->toPlainText();
        f.close();

        QFPluginServices::getInstance()->displayHelpWindow(f.fileName());
    }
}



void QFEHelpEditorWidget::on_btnNew_clicked()
{
    if (maybeSave()) {
        ui->edtScript->getEditor()->setPlainText("");
        setScriptFilename(tr("new_help.html"));
        lastScript=ui->edtScript->getEditor()->toPlainText();
    }
}

void QFEHelpEditorWidget::on_btnOpen_clicked()
{
    openScript("last", true);
}

void QFEHelpEditorWidget::on_btnSave_clicked()
{
    QString dir=ProgramOptions::getInstance()->getQSettings()->value("QFEHelpEditorWidget/lastScriptDir", ProgramOptions::getInstance()->getMainHelpDirectory()).toString();
    QDir d(dir);
    QString filename=qfGetSaveFileName(this, tr("save acquisition script ..."), d.absoluteFilePath(currentScript), tr("help (*.html)"));
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
    ProgramOptions::getInstance()->getQSettings()->setValue("QFEHelpEditorWidget/lasttemplatedir", dir);
}



void QFEHelpEditorWidget::on_btnOpenExample_clicked()
{
    openScript(ProgramOptions::getInstance()->getAssetsDirectory()+"/plugins/qfe_helpeditor/examples/", false);
}

void QFEHelpEditorWidget::on_btnOpenTemplate_clicked()
{
    openScript(ProgramOptions::getInstance()->getAssetsDirectory()+"/plugins/qfe_helpeditor/templates/", false);
}

void QFEHelpEditorWidget::edtScript_cursorPositionChanged()
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




    ui->labCursorPos->setText(tr("Line %1, Column %2").arg(ui->edtScript->getEditor()->getLineNumber()).arg(ui->edtScript->getEditor()->getColumnNumber()));
}

void QFEHelpEditorWidget::on_btnHelp_clicked()
{
    QFPluginServices::getInstance()->displayHelpWindow(QFPluginServices::getInstance()->getMainHelpDirectory()+"/qf3_helpref.html");
}

bool QFEHelpEditorWidget::maybeSave() {
    if (ui->edtScript->getEditor()->toPlainText().isEmpty()) return true;
    if (ui->edtScript->getEditor()->toPlainText()==lastScript) return true;
    int r=QMessageBox::question(this, tr("save help file ..."), tr("The current script has not been saved.\n  Delete?\n    Yes: Any changes will be lost.\n    No: You will be asked for a filename for the script.\n    Cancel: return to editing the script."), QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel, QMessageBox::No);
    if (r==QMessageBox::Yes) {
        return true;
    } else if (r==QMessageBox::No) {
        on_btnSave_clicked();
        return true;
    }

    return false;
}

void QFEHelpEditorWidget::setScriptFilename(QString filename)
{
    currentScript=filename;
    recentHelpFiles->addRecentFile(filename);
    ui->labScriptFilename->setText(tr("current file: <tt><i>%1</i></tt>").arg(QFileInfo(filename).fileName()));
}


QStringListModel *QFEHelpEditorWidget::modelFromFile(const QString &fileName)
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

void QFEHelpEditorWidget::addInsertAction(const QString &insert)
{
    addInsertAction(tr("insert \"%1\"").arg(insert), insert);
}

void QFEHelpEditorWidget::addInsertAction(const QString &label, const QString &insert)
{
    QAction* act=new QAction(label, this);
    connect(act, SIGNAL(triggered()), this, SLOT(insertActionClicked()));
    insertmap[act]=insert;
    ui->edtScript->getEditor()->addAction(act);
    ui->edtScript->getEditor()->setContextMenuPolicy(Qt::ActionsContextMenu);
}

void QFEHelpEditorWidget::addInsertAction(QMenu *menu, const QString &insert)
{
    addInsertAction(menu, tr("insert \"%1\"").arg(insert), insert);
}

void QFEHelpEditorWidget::addInsertAction(QMenu *menu, const QString &label, const QString &insert)
{
    QAction* act=new QAction(label, this);
    connect(act, SIGNAL(triggered()), this, SLOT(insertActionClicked()));
    insertmap[act]=insert;
    //ui->edtScript->getEditor()->addAction(act);
    menu->addAction(act);
    ui->edtScript->getEditor()->setContextMenuPolicy(Qt::ActionsContextMenu);
}


void QFEHelpEditorWidget::openScript(QString dir, bool saveDir) {
    if (maybeSave()) {
        if (dir=="last") {
            dir=ProgramOptions::getInstance()->getQSettings()->value("QFEHelpEditorWidget/lastScriptDir", ProgramOptions::getInstance()->getMainHelpDirectory()).toString();
        }
        QString filename=qfGetOpenFileName(this, tr("Open help file ..."), dir, tr("help (*.html)"))    ;
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
        if (saveDir) ProgramOptions::getInstance()->getQSettings()->setValue("QFEHelpEditorWidget/lastScriptDir", dir);
    }

}

void QFEHelpEditorWidget::openScriptNoAsk(QString filename)
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

void QFEHelpEditorWidget::on_btnBold_clicked()
{
    insertAroundOld("<b>%1°</b>");
}

void QFEHelpEditorWidget::on_btnItalic_clicked()
{
    insertAroundOld("<i>%1°</i>");
}

void QFEHelpEditorWidget::on_btnUnderline_clicked()
{
    insertAroundOld("<u>%1°</u>");
}

void QFEHelpEditorWidget::on_btnImage_clicked()
{
    insertAroundOld("<img src=\"image.png\">%1°");

}

void QFEHelpEditorWidget::on_btnAnchor_clicked()
{
    insertAroundOld("<a name=\"\">%1");
}

void QFEHelpEditorWidget::on_btnLink_clicked()
{
    bool ok=false;
    QStringList links;
    links<<"http://";
    QString txt=ui->edtScript->getEditor()->toPlainText();
    QRegExp rxLink("<a\\s*[^>]*name\\s*=\\s*\"\#([^\"']*)\"[^>]*>", Qt::CaseInsensitive);
    rxLink.setMinimal(false);
    int count = 0;
    int pos = 0;
    while ((pos = rxLink.indexIn(txt, pos)) != -1) {

        QString name=rxLink.cap(1).toLower();
        links<<QString("#"+name);
        ++count;
        pos += rxLink.matchedLength();
    }

    links<<"$$mainhelpdir$$/";
    links<<"$$assetsdir$$/";
    links<<"$$configdir$$/";
    //links<<"$$$$/";
    links<<"mailto:$$email$$";
    links<<"mailto:$$maillist$$";
    links<<"$$weblink$$";
    //links<<"";



    QString link=QInputDialog::getItem(this, tr("insert link"), tr("link target:"), links, 0, true,&ok);
    if (ok) {
        insertAroundOld(QString("<a href=\"%1\">").arg(link)+QString("%1°</a>"));
    }
}

void QFEHelpEditorWidget::on_btnBlockquote_clicked()
{
    insertAroundOld("\n<blockquote>\n    %1\n</blockquote>\n");
}

void QFEHelpEditorWidget::on_btnNumberedList_clicked()
{
    insertAroundOld("\n<ol>\n    <li>%1</li>\n</ol>");

}

void QFEHelpEditorWidget::on_btnBulletList_clicked()
{
    insertAroundOld("\n<ul>\n    <li>%1</li>\n</ul>");
}

void QFEHelpEditorWidget::on_btnInsertH1_clicked()
{
    insertAroundOld("\n\n<h1>%1°</h1>\n<p></p>\n");
}

void QFEHelpEditorWidget::on_btnInsertH2_clicked()
{
    insertAroundOld("\n\n<h2>%1°</h2>\n<p></p>\n");
}

void QFEHelpEditorWidget::on_btnInsertH3_clicked()
{
    insertAroundOld("\n\n<h3>%1°</h3>\n<p></p>\n");
}

void QFEHelpEditorWidget::on_btnInsertH4_clicked()
{
    insertAroundOld("\n\n<h4>%1°</h4>\n<p></p>\n");
}

void QFEHelpEditorWidget::on_btnInsertParagraph_clicked()
{
    insertAroundOld("\n<p>\n    %1\n</p>\n");
}

void QFEHelpEditorWidget::on_btnInsertListItem_clicked()
{
    insertAroundOld("\n<li>%1°</li>");
}

void QFEHelpEditorWidget::on_btnPasteImage_clicked()
{
    PasteImageDlg* dlg=new PasteImageDlg(QFileInfo(currentScript).absolutePath(), this, QString("./pic/%1").arg(QFileInfo(currentScript).baseName())+QString("_pic%1.png") );
    if (dlg->exec()) {
        insertAroundOld(dlg->saveImage()+"%1");
    }
    delete dlg;
}

void QFEHelpEditorWidget::on_btnInsertCode_clicked()
{
    insertAroundOld("<code>%1°</code>");
}

void QFEHelpEditorWidget::on_btnInsertMath_clicked()
{
    insertAroundOld("$$math:°$$%1");
}

void QFEHelpEditorWidget::on_btnInsertBMath_clicked()
{
    insertAroundOld("$$bmath:°$$%1");
}



void QFEHelpEditorWidget::findFirst()
{
    if (ui->edtScript->getEditor()->hasSelection()) findDlg->setPhrase(ui->edtScript->getEditor()->getSelection());
    if (findDlg->exec()==QDialog::Accepted) {
        // enable "Find next" action
        findNextAct->setEnabled(true);
        findNextAct->setIcon(QIcon(":/qfe_helpeditor/script_find_next.png"));
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

void QFEHelpEditorWidget::findNext()
{
    if (!ui->edtScript->getEditor()->findNext()) {
        QMessageBox::information(this, tr("Find ..."),
                         tr("Did not find '%1' ...")
                         .arg(ui->edtScript->getEditor()->getPhrase()));
    }
}

void QFEHelpEditorWidget::replaceFirst()
{
    if (ui->edtScript->getEditor()->hasSelection()) replaceDlg->setPhrase(ui->edtScript->getEditor()->getSelection());
    if (replaceDlg->exec()==QDialog::Accepted) {
        // enable "Find next" action
        findNextAct->setEnabled(true);
        findNextAct->setIcon(QIcon(":/qfe_helpeditor/script_find_replace_next.png"));
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

void QFEHelpEditorWidget::replaceNext()
{
    if (! ui->edtScript->getEditor()->replaceNext()) {
        QMessageBox::information(this, tr("Find & Replace ..."),
                                 tr("Did not find '%1' ...")
                                 .arg(ui->edtScript->getEditor()->getPhrase()));
    }

}

void QFEHelpEditorWidget::gotoLine()
{
    int maxLine=ui->edtScript->getEditor()->document()->blockCount();
    bool ok;
    unsigned long line = QInputDialog::getInteger(this, tr("Goto Line ..."),
                              tr("Enter a line number (1 - %2):").arg(maxLine), 1, 1, maxLine, 1, &ok);
    if (ok) {
        ui->edtScript->getEditor()->gotoLine(line);
        ui->edtScript->getEditor()->setFocus();
    }

}

void QFEHelpEditorWidget::print()
{
#ifndef QT_NO_PRINTER
   QPrinter printer;

   QPrintDialog *dialog = new QPrintDialog(&printer, this);
   dialog->setWindowTitle(tr("Print Document"));
   if (ui->edtScript->getEditor()->textCursor().hasSelection())
       dialog->addEnabledOption(QAbstractPrintDialog::PrintSelection);
   if (dialog->exec() != QDialog::Accepted)
       return;

   ui->edtScript->getEditor()->print(&printer);
#endif

}

void QFEHelpEditorWidget::printPreviewClick()
{
#ifndef QT_NO_PRINTER
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, this);
    connect(&preview, SIGNAL(paintRequested(QPrinter *)), SLOT(printPreview(QPrinter *)));
    preview.exec();
#endif
}

void QFEHelpEditorWidget::printPreview(QPrinter *printer)
{
#ifndef QT_NO_PRINTER
    ui->edtScript->getEditor()->print(printer);
#endif

}

void QFEHelpEditorWidget::clearFindActions()
{
    findNextAct->setEnabled(false);
}

void QFEHelpEditorWidget::insertActionClicked()
{
    QAction* act=qobject_cast<QAction*>(sender());
    if (act) {
        QString txt=insertmap.value(act, "");
        if (!txt.isEmpty()) {
            ui->edtScript->getEditor()->insertPlainText(txt);
        }
    }
}

void QFEHelpEditorWidget::insertAroundOld(const QString &newText)
{
    QString txt=newText.arg(ui->edtScript->getEditor()->getSelection());
    if (!txt.isEmpty()) {
        int pos=txt.indexOf('°');
        if (pos>=0) txt=txt.remove('°');
        int textpos=ui->edtScript->getEditor()->textCursor().position();
        ui->edtScript->getEditor()->insertPlainText(txt);
        if (pos>=0) {
            for (int i=0; i<txt.length()-pos; i++) {
                ui->edtScript->getEditor()->moveCursor(QTextCursor::Left, QTextCursor::MoveAnchor);
            }
        }

    }

}


