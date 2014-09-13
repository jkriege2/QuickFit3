/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

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

#include "qfhtmlhelpwindow.h"
#include <QHBoxLayout>
#include <QTextDocument>
#include <QFile>
#include <QDir>
#include <QtGui>
#include <iostream>
#include <QDir>
#include <QDesktopServices>
#include "qfpluginservices.h"
#include "qffitalgorithmmanager.h"
#include "qffitfunctionmanager.h"
#include "qfextensionmanager.h"
#include "qffitalgorithm.h"
#include "qffitfunction.h"
#include "jkqtmathtext.h"
#include "programoptions.h"
#include "qmodernprogresswidget.h"
#include "qfhtmlhelptools.h"
#include <QRegExp>
#include <QtGlobal>


QFHTMLHelpWindow::QFHTMLHelpWindow(QWidget* parent, Qt::WindowFlags flags):
    QWidget(parent, flags)
{
    m_pluginServices=NULL;
    searchPath="";
    replaces=NULL;
    m_baseFont=font();
    m_titleFont=font();
    m_titleFont.setPointSizeF(m_titleFont.pointSizeF()*2);
    m_titleFont.setBold(true);
    m_home=ProgramOptions::getInstance()->getAssetsDirectory()+"/help/quickfit.html";
    history_idx=-1;

    labelTitle=new QLabel(this);
    descriptionBrowser=new QTextBrowser(this);
    QPalette p=descriptionBrowser->palette();
    p.setColor(QPalette::Inactive, QPalette::Highlight, p.color(QPalette::Active,  QPalette::Highlight));
    p.setColor(QPalette::Inactive, QPalette::HighlightedText, p.color(QPalette::Active,  QPalette::HighlightedText));
    descriptionBrowser->setPalette(p);
    descriptionBrowser->setOpenLinks(false);
    descriptionBrowser->setOpenExternalLinks(false);
    connect(descriptionBrowser, SIGNAL(anchorClicked(const QUrl&)), this, SLOT(anchorClicked(const QUrl&)));


    menuBar=new QMenuBar(this);
    menuPage=menuBar->addMenu(tr("&Page"));
    menuContents=menuBar->addMenu(tr("&Help Contents"));

    actHelpHelp=menuContents->addAction(QIcon(":/lib/help_help.png"), tr("Help for &Online-Help Browser"));
    connect(actHelpHelp, SIGNAL(triggered()), this, SLOT(helpOnHelp()));
    actHelpHelp->setParent(this);


    QHBoxLayout* layButtons=new QHBoxLayout;//(this);


    actPrevious=menuPage->addAction(tr("&Previous"), this, SLOT(previous()));
    actPrevious->setIcon(QIcon(":/lib/help_previous.png"));
    actPrevious->setShortcut(QKeySequence::Back);
    btnPrevious=new QToolButton(this);
    btnPrevious->setDefaultAction(actPrevious);
    layButtons->addWidget(btnPrevious);


    btnHome=new QToolButton(this);
    layButtons->addWidget(btnHome);

    actNext=menuPage->addAction(tr("&Next"), this, SLOT(next()));
    actNext->setIcon(QIcon(":/lib/help_next.png"));
    actNext->setShortcut(QKeySequence::NextChild);
    btnNext=new QToolButton(this);
    btnNext->setDefaultAction(actNext);
    layButtons->addWidget(btnNext);

    actHome=menuPage->addAction(tr("&Home"), this, SLOT(home()));
    actHome->setIcon(QIcon(":/lib/help_home.png"));
    btnHome->setDefaultAction(actHome);
    menuPage->addSeparator();

    actPrint=menuPage->addAction(tr("&Print"), this, SLOT(print()));
    actPrint->setIcon(QIcon(":/lib/help_print.png"));
    actPrint->setShortcut(QKeySequence::Print);
    actPrint->setToolTip(tr("Print this help page"));
    menuPage->addSeparator();
    btnPrint=new QToolButton(this);
    btnPrint->setDefaultAction(actPrint);
    layButtons->addWidget(btnPrint);

    actFind=menuPage->addAction(tr("&Search in current page"));
    actFind->setCheckable(true);
    actFind->setShortcut(QKeySequence::Find);
    actFind->setChecked(false);
    connect(actFind, SIGNAL(triggered(bool)), this, SLOT(find(bool)));
    actFind->setIcon(QIcon(":/lib/help_find.png"));
    btnFind=new QToolButton(this);
    btnFind->setDefaultAction(actFind);
    layButtons->addWidget(btnFind);

    actFindInAll=menuPage->addAction(tr("&Search in all help pages"));
    actFindInAll->setCheckable(true);
    actFindInAll->setShortcut(QKeySequence("Ctrl+Shift+F"));
    actFindInAll->setChecked(false);
    connect(actFindInAll, SIGNAL(triggered(bool)), this, SLOT(findInAll(bool)));
    actFindInAll->setIcon(QIcon(":/lib/help_findinall.png"));
    btnFindInAll=new QToolButton(this);
    btnFindInAll->setDefaultAction(actFindInAll);
    layButtons->addWidget(btnFindInAll);

    menuPage->addSeparator();
    actClose=menuPage->addAction(tr("&Close"));
    actClose->setShortcut(QKeySequence::Close);
    actClose->setIcon(QIcon(":/lib/exit.png"));
    connect(actClose, SIGNAL(triggered()), this, SLOT(close()));



    widFind=new QWidget(this);
    QHBoxLayout* layFind=new QHBoxLayout;
    widFind->setLayout(layFind);
    layFind->addWidget(new QLabel(tr("     phrase: ")));
    edtFind=new QFEnhancedLineEdit(this);
    edtFind->addButton(new QFStyledButton(QFStyledButton::ClearLineEdit, edtFind, edtFind));
    edtFind->setMinimumWidth(150);
    edtFind->setToolTip(tr("simply enter a search phrase\n\nthe search in the currently displayed help page begins immediately after typing the first letter. If no match was found, the field will change its background color to a light red."));
    layFind->addWidget(edtFind);
    connect(edtFind, SIGNAL(textEdited(QString)), this, SLOT(findNext()));

    actFindNext=new QAction(QIcon(":/lib/help_findnext.png"), tr("&find next"), this);
    actFindNext->setShortcut(QKeySequence::FindNext);
    connect(actFindNext, SIGNAL(triggered()), this, SLOT(findNext()));

    btnFindNext=new  QToolButton(this);
    btnFindNext->setDefaultAction(actFindNext);
    layFind->addWidget(btnFindNext);

    actFindPrev=new QAction(QIcon(":/lib/help_findprev.png"), tr("find &previous"), this);
    actFindPrev->setShortcut(QKeySequence::FindPrevious);
    connect(actFindPrev, SIGNAL(triggered()), this, SLOT(findPrev()));

    btnFindPrev=new  QToolButton(this);
    btnFindPrev->setDefaultAction(actFindPrev);
    layFind->addWidget(btnFindPrev);

    chkCaseSensitive=new QCheckBox(tr("case-sensitive"), this);
    chkCaseSensitive->setChecked(false);
    connect(chkCaseSensitive, SIGNAL(toggled(bool)), this, SLOT(findNext()));
    layFind->addWidget(chkCaseSensitive);
    chkFindWholeWord=new QCheckBox(tr("whole word"), this);
    chkFindWholeWord->setChecked(false);
    connect(chkFindWholeWord, SIGNAL(toggled(bool)), this, SLOT(findNext()));
    layFind->addWidget(chkFindWholeWord);

    widFind->setVisible(false);
    layButtons->addWidget(widFind);


    layButtons->addStretch();



    widFindInAll=new QWidget(this);
    QGridLayout* layFindAll=new QGridLayout();
    layFindAll->setContentsMargins(0,0,0,0);
    widFindInAll->setLayout(layFindAll);
    layFindAll->addWidget(new QLabel(tr("phrase: ")),0,0);
    edtFindInAll=new QFEnhancedLineEdit(widFindInAll);
    edtFindInAll->addButton(new QFStyledButton(QFStyledButton::ClearLineEdit, edtFindInAll, edtFindInAll));
    edtFindInAll->setMinimumWidth(150);
    edtFindInAll->setToolTip(tr("<b>find text in all help files:</b><br><br>enter a search phrase and click on <b><img src=\":/lib/help_findinall.png\"> Search ...</b> afterwards to start the search. the results will be displayed in the list below"));
    scFindInAllFind=new QShortcut(QKeySequence("ENTER"), edtFindInAll);
    connect(scFindInAllFind, SIGNAL(activated()), this, SLOT(searchInAll()));
    connect(edtFindInAll, SIGNAL(returnPressed()), this, SLOT(searchInAll()));
    layFindAll->addWidget(edtFindInAll,0,1);

    QHBoxLayout* layTemp=new QHBoxLayout();
    layFindAll->addLayout(layTemp,1,1,1,2);
    chkCaseSensitiveInAll=new QCheckBox(tr("case-sensitive"), widFindInAll);
    chkCaseSensitiveInAll->setChecked(false);
    layTemp->addWidget(chkCaseSensitiveInAll);
    chkFindWholeWordInAll=new QCheckBox(tr("whole word"), widFindInAll);
    chkFindWholeWordInAll->setChecked(false);
    layTemp->addWidget(chkFindWholeWordInAll,1);

    btnFindInAllFind=new QToolButton(widFindInAll);
    btnFindInAllFind->setText("&Search ...");
    btnFindInAllFind->setIcon(QIcon(":/lib/help_findinall.png"));
    btnFindInAllFind->setToolTip(tr("search all help files available"));
    btnFindInAllFind->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    connect(btnFindInAllFind, SIGNAL(clicked()), this, SLOT(searchInAll()));
    QSizePolicy sp=btnFindInAllFind->sizePolicy();
    sp.setControlType(QSizePolicy::PushButton);
    sp.setHorizontalPolicy(QSizePolicy::Minimum);
    btnFindInAllFind->setSizePolicy(sp);
    btnFindInAllFind->setFocusPolicy(Qt::StrongFocus);
    layFindAll->addWidget(btnFindInAllFind,0,2);

    listFindInAll=new QListWidget(widFindInAll);
    listFindInAll->setToolTip(tr("<b>Reult of the last global help search.</b><br>double-click an entry to display the according help page"));
    connect(listFindInAll, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(searchAllItemDoubleClicked(QListWidgetItem*)));
    layFindAll->addWidget(listFindInAll, 2,0,1,3);

    layFindAll->setColumnStretch(1,1);
    layFindAll->setRowStretch(2,1);
    widFindInAll->setVisible(false);
    widFindInAll->setMinimumWidth(100);


    QVBoxLayout* layout=new QVBoxLayout;//(this);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(menuBar);
    layout->addLayout(layButtons);
    setLayout(layout);


    QVBoxLayout* layoutContents=new QVBoxLayout;//(this);
    layoutContents->addWidget(labelTitle);
    layoutContents->setStretchFactor(labelTitle, 0);
    layoutContents->addWidget(descriptionBrowser);
    layoutContents->setStretchFactor(descriptionBrowser, 6);

    splitterSearch=new QVisibleHandleSplitter(Qt::Horizontal, this);

    QWidget* w=new QWidget(this);
    w->setLayout(layoutContents);
    splitterSearch->addWidget(widFindInAll);
    splitterSearch->addWidget(w);

    layout->addWidget(splitterSearch,1);
    splitterSearch->setCollapsible(0,false);
    splitterSearch->setCollapsible(1,false);
    QList<int> sizes;
    sizes<<200<<400;
    splitterSearch->setSizes(sizes);

    //setWindowFlags(flags);
    setWindowTitle(tr("QuickFit Online-Help"));
    setWindowIcon(QIcon(":/lib/help.png"));

    //if (ProgramOptions::getInstance()->getHelpWindowsStayOnTop()) setWindowFlags(Qt::Tool/*|Qt::WindowStaysOnTopHint|Qt::X11BypassWindowManagerHint*/ );
    //else setWindowFlags(Qt::Tool);
    setParent(NULL);
    addAction(actFindNext);
    addAction(actFindPrev);
    descriptionBrowser->addAction(actFindNext);
    descriptionBrowser->addAction(actFindPrev);
    actFindNext->setEnabled(false);
    actFindPrev->setEnabled(false);

}

void QFHTMLHelpWindow::readSettings(QSettings& settings, QString prefix) {
     resize(settings.value(prefix+"htmlhelpwin.size", QSize(400, 400)).toSize());
     move(settings.value(prefix+"htmlhelpwin.pos", QPoint(200, 200)).toPoint());
     loadSplitter(settings, splitterSearch, prefix);
}

void QFHTMLHelpWindow::writeSettings(QSettings& settings, QString prefix) {
     settings.setValue(prefix+"htmlhelpwin.size", size());
     settings.setValue(prefix+"htmlhelpwin.pos", pos());
     saveSplitter(settings, splitterSearch, prefix);
}



void QFHTMLHelpWindow::updateHelp(QString filename1) {
    anchorClicked(filename1);

}

void QFHTMLHelpWindow::clear() {
    labelTitle->setText("");
    actHome->setText(tr("&Home"));
    descriptionBrowser->clear();
    updateButtons();
}

void QFHTMLHelpWindow::showAndSearchInAll() {
    clear();
    show();
    actFindInAll->setChecked(false);
    actFindInAll->setChecked(true);
}

void QFHTMLHelpWindow::helpOnHelp()
{
    updateHelp(ProgramOptions::getInstance()->getAssetsDirectory()+"/help/qf3_help.html");
}

void QFHTMLHelpWindow::displayTitle() {
    QString title=descriptionBrowser->documentTitle();
    labelTitle->setFont(m_titleFont);
    labelTitle->setText(title);
    labelTitle->setVisible(!title.isEmpty());
    updateButtons();
}

void QFHTMLHelpWindow::anchorClicked(const QUrl& link) {

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    QString linkstr=link.toString();
    QString scheme=link.scheme().toLower();

    if (linkstr.startsWith("#")) {
        descriptionBrowser->scrollToAnchor(linkstr.right(linkstr.size()-1));
        return;
    }

    if ((scheme!="http") && (scheme!="https") && (scheme!="ftp") && (scheme!="ftps") && (scheme!="mailto") && (scheme!="sftp") && (scheme!="svn") && (scheme!="ssh") /*&& QFile::exists(QFileInfo(s).absoluteFilePath())*/) {

        if ((scheme=="pop") || (scheme=="popup") || (scheme=="tooltip") || (scheme=="tool")||(scheme=="tip")){
            QString tooltip=link.toString(QUrl::RemoveScheme);
            QString tooltipfn="";
            QString tooltipstr=tr("<i>no tooltip available</i>");
            if (tooltips.contains(tooltip)) {
                tooltipstr=tooltips[tooltip].tooltip;
                tooltipfn=tooltips[tooltip].tooltipfile;
            }
            //qDebug()<<"show tooltip: "<<tooltip<<tooltips.value(tooltip);
            QToolTip::showText(descriptionBrowser->mapFromGlobal(QCursor::pos()), transformQF3HelpHTML(tooltipstr, tooltipfn), descriptionBrowser, QRect());
        } else if ((scheme=="open") || (scheme=="opendir") || (scheme=="openfile") || (scheme=="file")){
            //QDir spd(searchPath);
            //QString filenamen=link.toString(QUrl::RemoveScheme|QUrl::StripTrailingSlash);
            //QString filename=QFileInfo(QUrl("file:"+filenamen).toLocalFile()).absoluteFilePath();
            //QString cl=spd.cleanPath(spd.absoluteFilePath(filename));
            //qDebug()<<scheme<<link<<filenamen<<QUrl("file:"+filenamen)<<filename<<cl<<spd;
            QUrl url=link;
            url.setScheme("file");
#ifdef Q_WS_WIN
            //qDebug()<<url.toString();
            QRegExp rx("file://([a-zA-Z])/");
            if (rx.indexIn(url.toString())>=0) {
                //qDebug()<<rx.cap(1)<<QString("file:///%1:/").arg(rx.cap(1));
                //qDebug()<<url.toString()<<url.toString().replace(rx, QString("file:///%1:/").arg(rx.cap(1)));
                url=QUrl(url.toString().replace(rx, QString("file:///%1:/").arg(rx.cap(1))), QUrl::TolerantMode);
            }
#endif
            //qDebug()<<"open linked file: "<<url;
            //qDebug()<<
            QDesktopServices::openUrl(url);
        } else {
            QDir spd(searchPath);
            QString filename=link.toString(QUrl::RemoveFragment);
            QString fragment=link.fragment();
            QString cl=spd.cleanPath(spd.absoluteFilePath(filename));
            QString s=spd.absoluteFilePath(cl); //absoluteFilePath
            searchPath=QFileInfo(s).absolutePath();
            descriptionBrowser->setSearchPaths(QStringList(searchPath)<<"./");
            descriptionBrowser->setHtml(loadHTML(QFileInfo(s).absoluteFilePath()));
            for (int i=history.size()-1; i>=qMax(0,history_idx)+1; i--) {
                history.pop();
            }
            history.push(HistoryEntry(s, descriptionBrowser->documentTitle()));
            history_idx=history.size()-1;
            updateButtons();
            if (link.hasFragment()) {
                descriptionBrowser->scrollToAnchor(QString("#")+fragment);
                descriptionBrowser->scrollToAnchor(fragment);
            }
        }
    } else {
        QDesktopServices::openUrl(link);
    }
    QApplication::restoreOverrideCursor();
}

void QFHTMLHelpWindow::showFile(QString filename1) {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QString filename=filename1;
    QString fragment="";
    int hashpos=filename1.indexOf("#");
    if (hashpos>=0) {
        filename=filename1.left(hashpos);
        fragment=filename1.mid(hashpos);
    }
    //qDebug()<<filename<<fragment;

    QDir spd(filename);
    QString cl=spd.cleanPath(spd.absoluteFilePath(filename));
    QString s=spd.absoluteFilePath(cl); //absoluteFilePath
    searchPath=QFileInfo(s).absolutePath();
    //qDebug()<<QFileInfo(filename).absoluteFilePath()<<searchPath;

    //std::cout<<"showFile("<<filename.toStdString()<<")   spd="<<spd.canonicalPath().toStdString()<<"   cl="<<cl.toStdString()<<"   s="<<s.toStdString()<<"   searchPath="<<searchPath.toStdString()<<"  src="<<QFileInfo(s).fileName().toStdString()<<"\n";
    descriptionBrowser->setSearchPaths(QStringList(searchPath)<<"./");
    //descriptionBrowser->setSource(QFileInfo(s).fileName());
    //descriptionBrowser->reload();
    descriptionBrowser->setHtml(loadHTML(QFileInfo(filename).absoluteFilePath()));
    if (!fragment.isEmpty()) {
        descriptionBrowser->scrollToAnchor(QString("#")+fragment);
    }
    QApplication::restoreOverrideCursor();
}

void QFHTMLHelpWindow::previous() {
    if (history_idx>=1) {
        history_idx--;
        showFile(history[history_idx].url);
        updateButtons();
    }
}

void QFHTMLHelpWindow::next() {
    if (history_idx<history.size()-1) {
        history_idx++;
        showFile(history[history_idx].url);
        updateButtons();
    }
}

void QFHTMLHelpWindow::home() {
    showFile(m_home);
    history.push(HistoryEntry(m_home, descriptionBrowser->documentTitle()));
    history_idx=history.size()-1;
    updateButtons();
}

void QFHTMLHelpWindow::print() {
    QPrinter* p=new QPrinter;

    // select a printer

    QPrintDialog *dialog = new QPrintDialog(p, this);
    dialog->setWindowTitle(tr("Print Help Page"));
    if (dialog->exec() != QDialog::Accepted) {
        delete p;
        return;
    }
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    descriptionBrowser->print(p);
    delete p;
    QApplication::restoreOverrideCursor();
}



QString QFHTMLHelpWindow::loadHTML(QString filename, bool noPics) {

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    const QString defaultTxt=tr("<html><header><title>Error: Help page does not exist</title></header><body>$$qf_commondoc_header.start$$ $$qf_commondoc_header.simplest$$ $$qf_commondoc_header.end$$<center>The Quickfit online-help page you are trying to access does not exist!<br><br>File was: <i>%1</i></center></body></html>").arg(filename);

    QString s= transformQF3HelpHTMLFile(filename, defaultTxt, true, QFHelpReplacesList(), true, noPics, true);
    QApplication::restoreOverrideCursor();

    return s;

}


void QFHTMLHelpWindow::setHtmlReplacementList(QList<QPair<QString, QString> >* list) {
    replaces=list;
}

void QFHTMLHelpWindow::setTooltips(const QMap<QString, QFToolTipsData> &list)
{
    tooltips=list;
}

void QFHTMLHelpWindow::setPluginDirList(QList<QFHelpDirectoryInfo>* pluginList) {
    this->pluginList=pluginList;
}

void QFHTMLHelpWindow::initFromPluginServices(QFPluginServices* services) {
    m_pluginServices=services;
    setHtmlReplacementList(services->getHTMLReplacementList());
    setPluginDirList(services->getPluginHelpList());
    setTooltips(services->getTooltips());
}

void QFHTMLHelpWindow::setContentsMenuActions(const QList<QAction *> &items)
{
    menuContents->clear();
    menuContents->addAction(actHome);
    menuContents->addSeparator();
    menuContents->addActions(items);
    menuContents->addSeparator();
    menuContents->addAction(actHelpHelp);
}

void QFHTMLHelpWindow::updateButtons() {
    //std::cout<<"updateButtons():  history_idx="<<history_idx<<"   history.size()="<<history.size()<<std::endl;
    if(history_idx<history.size()-1) {
        actNext->setEnabled(true);
        actNext->setWhatsThis("");
        actNext->setToolTip("");
        if (history_idx>=0) {
            actNext->setWhatsThis(tr("move on to help page: \"%1\"").arg(history[history_idx+1].name));
            actNext->setToolTip(tr("move on to help page: \"%1\"").arg(history[history_idx+1].name));
        }
    } else {
        actNext->setEnabled(false);
        actNext->setWhatsThis("");
        actNext->setToolTip("");
    }

    if(history_idx>0) {
        actPrevious->setEnabled(true);
        actPrevious->setWhatsThis("");
        actPrevious->setToolTip("");
        if (history_idx<history.size()) {
            actPrevious->setWhatsThis(tr("move back to help page: \"%1\"").arg(history[history_idx-1].name));
            actPrevious->setToolTip(tr("move back to help page: \"%1\"").arg(history[history_idx-1].name));
        }
    } else {
        actPrevious->setEnabled(false);
        actPrevious->setWhatsThis("");
        actPrevious->setToolTip("");
    }
}

void QFHTMLHelpWindow::find(bool checked) {
    widFind->setVisible(checked);
    if (checked) {
        edtFind->setFocus();
        edtFind->selectAll();
    }
    actFindNext->setEnabled(checked);
    actFindPrev->setEnabled(checked);
}

void QFHTMLHelpWindow::findInAll(bool checked) {
    widFindInAll->setVisible(checked);
    btnFindInAllFind->setEnabled(checked);
    scFindInAllFind->setEnabled(checked);
    if (checked) {
        edtFindInAll->setFocus(Qt::TabFocusReason);
        edtFindInAll->selectAll();
    }
}

void QFHTMLHelpWindow::searchInAll() {

    QModernProgressDialog progress(this);
    progress.setHasCancel(true);
    progress.setLabelText(tr("building list of help files ..."));
    progress.setMode(true, false);
    progress.show();
    QApplication::processEvents();
    QApplication::processEvents();

    QString phrase=edtFindInAll->text();
    Qt::CaseSensitivity flagstxt=Qt::CaseInsensitive;
    int flags=0;
    bool findWhole=chkFindWholeWordInAll->isChecked();
    if (chkCaseSensitiveInAll->isChecked()) {
        flags|=QTextDocument::FindCaseSensitively;
        flagstxt=Qt::CaseSensitive;
    }
    if (findWhole) flags|=QTextDocument::FindWholeWords;

    QDir dir(ProgramOptions::getInstance()->getAssetsDirectory());
    dir.cd("help");
    QStringList files;
    searchHelpDir(dir, files);
    dir.cdUp();

    dir.cd("plugins");
    dir.cd("help");
    searchHelpDir(dir, files);
    dir.cdUp();
    dir.cdUp();

    if (!progress.wasCanceled()) {
        listFindInAll->clear();
        progress.setLabelText(tr("searching in help files ...\n"));
        progress.setRange(0, files.size());
        progress.setMode(true, true);
        progress.setValue(0);
        QApplication::processEvents();
        QApplication::processEvents();
        for (int i=0; i<files.size(); i++) {
            progress.setLabelText(tr("searching in help files ...\n   current: %1").arg(dir.relativeFilePath(files[i])));
            progress.setValue(i);
            QApplication::processEvents();
            if (progress.wasCanceled()) break;
            if (QFile::exists(files[i])) {

                /*QTextDocument doc;
                doc.setHtml(loadHTML(files[i], true));
                QTextCursor pos=doc.find(phrase, 0, QTextDocument::FindFlags(flags));
                if (!pos.isNull()) {
                    QListWidgetItem* it=new QListWidgetItem(listFindInAll);
                    it->setText(doc.metaInformation(QTextDocument::DocumentTitle));
                    it->setData(Qt::UserRole, files[i]);
                    it->setData(Qt::UserRole+1, pos.selectionStart());
                    it->setData(Qt::UserRole+2, pos.selectionEnd());
                    it->setData(Qt::UserRole+3, phrase);
                    it->setData(Qt::UserRole+4, flags);
                    listFindInAll->addItem(it);
                }*/
                QString html=readFile(files[i]);
                QString txt=removeHTML(html);
                //qDebug()<<files[i]<<"\n  "<<txt.size();
                bool found=false;
                int start=0;
                int idx=txt.indexOf(phrase, start, flagstxt);
                while (idx>=start && start<txt.size()) {
                    //qDebug()<<"   "<<idx<<start;
                    if (idx>=0) {
                        QChar l='\0';
                        QChar r='\0';
                        if (idx>0) l=txt[idx-1];
                        if (idx+phrase.size()<txt.size()) r=txt[idx+phrase.size()];
                        found=true;
                        if (findWhole) {
                            //qDebug()<<"   "<<l<<l.isLetterOrNumber()<<r<<r.isLetterOrNumber();
                            if (l!='\0' && l.isLetterOrNumber()) found=false;
                            if (r!='\0' && r.isLetterOrNumber()) found=false;
                        }
                        start=idx+1;
                    }
                    if (found) break;
                    idx=txt.indexOf(phrase, start, flagstxt);
                }

                //qDebug()<<"  "<<found;
                if (found) {
                    QListWidgetItem* it=new QListWidgetItem(listFindInAll);
                    it->setText(HTMLGetTitle(html));
                    it->setData(Qt::UserRole, files[i]);
                    it->setData(Qt::UserRole+1, -1);
                    it->setData(Qt::UserRole+2, -1);
                    it->setData(Qt::UserRole+3, phrase);
                    it->setData(Qt::UserRole+4, flags);
                    listFindInAll->addItem(it);
                }
            }

        }
    }
}

void QFHTMLHelpWindow::searchHelpDir(const QDir &dir, QStringList &files) {
    //qDebug()<<"searchHelpDir("<<dir<<")";
    QStringList f=dir.entryList(QStringList("*.html"), QDir::Files|QDir::Readable|QDir::NoDotAndDotDot);
    for (int i=0; i<f.size(); i++) {
        files.append(dir.absoluteFilePath(f[i]));
    }
    QStringList dirs=dir.entryList(QStringList("*"), QDir::AllDirs|QDir::NoDotAndDotDot);
    for (int i=0; i<dirs.size(); i++) {
        //qDebug()<<"   -> "<<dirs[i];
        QDir d=dir;
        d.cd(dirs[i]);
        if (d!=dir) searchHelpDir(d, files);
        d.cdUp();
    }

}

void QFHTMLHelpWindow::findNext()
{
    QString phrase=edtFind->text();
    QTextDocument::FindFlags options;
    if (chkCaseSensitive->isChecked()) options=options|QTextDocument::FindCaseSensitively;
    if (chkFindWholeWord->isChecked()) options=options|QTextDocument::FindWholeWords;
    if (phrase.isEmpty()) {
        descriptionBrowser->moveCursor(QTextCursor::Start);
    } else {
        bool found=descriptionBrowser->find(phrase, options);
        if (!found) {
            descriptionBrowser->moveCursor(QTextCursor::Start);
            found=descriptionBrowser->find(phrase, options);
        }
        QPalette p=edtFind->palette();
        if (!found) p.setColor(QPalette::Base, QColor("mistyrose"));
        else p.setColor(QPalette::Base, palette().color(QPalette::Base));
        edtFind->setPalette(p);
    }
}

void QFHTMLHelpWindow::findPrev()
{
    QString phrase=edtFind->text();
    QTextDocument::FindFlags options=QTextDocument::FindBackward;
    if (chkCaseSensitive->isChecked()) options=options|QTextDocument::FindCaseSensitively;
    if (chkFindWholeWord->isChecked()) options=options|QTextDocument::FindWholeWords;
    if (phrase.isEmpty()) {
        descriptionBrowser->moveCursor(QTextCursor::End);
    } else {
        bool found=descriptionBrowser->find(phrase, options);
        if (!found) {
            descriptionBrowser->moveCursor(QTextCursor::End);
            found=descriptionBrowser->find(phrase, options);
        }
        QPalette p=edtFind->palette();
        if (!found) p.setColor(QPalette::Base, QColor("mistyrose"));
        else p.setColor(QPalette::Base, palette().color(QPalette::Base));
        edtFind->setPalette(p);
    }
}

void QFHTMLHelpWindow::searchAllItemDoubleClicked(QListWidgetItem *item) {
    QString file=item->data(Qt::UserRole).toString();
    int selStart=item->data(Qt::UserRole+1).toInt();
    int selEnd=item->data(Qt::UserRole+2).toInt();
    int flags=item->data(Qt::UserRole+4).toInt();
    QString phrase=item->data(Qt::UserRole+3).toString();
    anchorClicked(QUrl(file));
    if (selStart>=0 && selEnd>selStart) {
        QTextCursor cur=QTextCursor(descriptionBrowser->document());
        cur.setPosition(selStart);
        cur.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, selEnd-selStart);
        descriptionBrowser->setTextCursor(cur);
    } else {
        descriptionBrowser->find(phrase, (QTextDocument::FindFlags)flags);
    }
}

QFHTMLHelpWindow::ContentsEntry::ContentsEntry()
{
    num.clear();
    header="";
    id="";
    prefix="";
}
