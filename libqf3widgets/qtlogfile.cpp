/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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



/*
  Name: qtlogfile.cpp
  Copyright: (c) 2008
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/

#include <QApplication>
#include "qtlogfile.h"
#include <QLabel>
#include <QTemporaryFile>
#include <QDebug>

QtLogFile::QtLogFile(QWidget *parent):
    QWidget(parent), textColor(QString("black")),
    errorColor(QString("red")), warningColor(QString("darkorange")),
    infoColor(QString("blue")), currentBold(false),
    fontSize(8), fontName(QString("Courier")), fixedPitch(true), maxFileSizeMBytes(50)
{
    mutex=new QMutex(QMutex::Recursive);
    lay=new QVBoxLayout();
    lay->setContentsMargins(0,0,0,0);
    setLayout(lay);
    browser=new QTextBrowser(this);
    lay->addWidget(browser);
    log_file=NULL;
    log_filename="application.log";
    log_to_console=true;
    log_to_file=false;
    last_was_linebreak=true;
    current_indent=0;
    log_file_append=false;
    log_date_time=false;
    log_date_time_template=QString("yyyy-MM-dd hh:mm:ss");

    actSelectAll=new QAction(tr("select all"), this);
    actSelectAll->setShortcut(QKeySequence::SelectAll);
    connect(actSelectAll, SIGNAL(triggered()), browser, SLOT(selectAll()));

    actClear=new QAction(QIcon(":/qtlogfile/clearall.png"), tr("clear all"), this);
    connect(actClear, SIGNAL(triggered()), browser, SLOT(clear()));

    actCopy=new QAction(QIcon(":/qtlogfile/copy.png"), tr("copy"), this);
    actCopy->setShortcut(QKeySequence::Copy);
    connect(actCopy, SIGNAL(triggered()), browser, SLOT(copy()));

    actShowLogFolder=new QAction(QIcon(":/qtlogfile/openfolder.png"), tr("open logfile folder"), this);
    connect(actShowLogFolder, SIGNAL(triggered()), this, SLOT(showLogFolder()));

    actFind=new QAction(QIcon(":/qtlogfile/find.png"), tr("search log contents ..."), this);
    actFind->setShortcut(QKeySequence::Find);
    actFind->setCheckable(true);
    actFind->setChecked(false);
    connect(actFind, SIGNAL(triggered(bool)), this, SLOT(searchLog(bool)));

    addAction(actFind);
    addAction(actCopy);
    addAction(actSelectAll);
    addAction(actClear);
    addAction(actShowLogFolder);
    browser->addAction(actFind);
    browser->addAction(actCopy);
    browser->addAction(actSelectAll);
    browser->addAction(actClear);
    browser->addAction(actShowLogFolder);

    browser->setReadOnly(true);
    browser->setLineWrapMode(QTextBrowser::NoWrap);
    currentColor=textColor;
    QFont f=font();
    f.setFixedPitch(fixedPitch);
    f.setFamily(fontName);
    f.setPointSize(fontSize);
    setFont(f);
    logStore.clear();
    browser->setCursor(Qt::IBeamCursor);
    setCursor(Qt::IBeamCursor);
    browser->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(browser, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));


    widFind=new QWidget(this);
    QHBoxLayout* layFind=new QHBoxLayout;
    widFind->setLayout(layFind);
    layFind->addWidget(new QLabel(tr("     phrase: ")));
    edtFind=new QFEnhancedLineEdit(this);
    edtFind->addButton(new JKStyledButton(JKStyledButton::ClearLineEdit, edtFind, edtFind));
    edtFind->setMinimumWidth(150);
    edtFind->setToolTip(tr("simply enter a search phrase\n\nthe search begins immediately after typing the first letter. If no match was found, the field will change its background color to a light red."));
    layFind->addWidget(edtFind);
    connect(edtFind, SIGNAL(textEdited(QString)), this, SLOT(findNext()));

    actFindNext=new QAction(QIcon(":/qtlogfile/findnext.png"), tr("find &next"), this);
    actFindNext->setShortcut(QKeySequence::FindNext);
    connect(actFindNext, SIGNAL(triggered()), this, SLOT(findNext()));

    actFindPrev=new QAction(QIcon(":/qtlogfile/findprev.png"), tr("find &previous"), this);
    actFindPrev->setShortcut(QKeySequence::FindPrevious);
    connect(actFindPrev, SIGNAL(triggered()), this, SLOT(findPrev()));

    btnFindNext=new  QToolButton(this);
    btnFindNext->setDefaultAction(actFindNext);
    layFind->addWidget(btnFindNext);
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
    lay->addWidget(widFind);

    addAction(actFindNext);
    addAction(actFindPrev);
    browser->addAction(actFindNext);
    browser->addAction(actFindPrev);
    actFindNext->setEnabled(false);
    actFindPrev->setEnabled(false);

    connect(this, SIGNAL(emit_log_error_linebreak(QString)), this, SLOT(log_error_linebreak(QString)), Qt::QueuedConnection);
    connect(this, SIGNAL(emit_log_warning_linebreak(QString)), this, SLOT(log_warning_linebreak(QString)), Qt::QueuedConnection);
    connect(this, SIGNAL(emit_log_text_linebreak(QString)), this, SLOT(log_text_linebreak(QString)), Qt::QueuedConnection);
}

QtLogFile::~QtLogFile() {
   current_indent=0;
   log_linebreak();
   log_linebreak();
   close_logfile();
   delete mutex;
}

void QtLogFile::clearLog()
{
    QMutexLocker locker(mutex);
    browser->document()->clear();
    browser->setText("");
    logStore.clear();
}

void QtLogFile::clearLogStore() {
    QMutexLocker locker(mutex);
    logStore.clear();
}

void QtLogFile::open_logfile(const QString& filename, bool writeStore) {
  QMutexLocker locker(mutex);

  if (filename!=log_filename) close_logfile();
  log_to_file=false;
  log_file=new QFile(filename);
  if (log_file==NULL) log_error(tr("Could not open LOG file '%1' ...\n").arg(filename));
  else {
    bool r=false;
    if (log_file_append) {
        if (QFileInfo(filename).size()/1024/1024>maxFileSizeMBytes) {
            QByteArray old;
            if (log_file->open(QIODevice::ReadOnly)) {
                old=log_file->readAll();
                log_file->close();
                log_file->remove();
                qDebug()<<"deleted log file";
            }
            if (old.size()>0 && log_file->open(QIODevice::WriteOnly) && log_file->size()/1024/1024<maxFileSizeMBytes) {
                qint64 start=qMax(0,old.size()-1024*1024*maxFileSizeMBytes);
                qint64 end=old.size()-1;
                if (start>=0 && start<old.size()) {
                    while (start>0 && old[(int)start]!='\n') {
                        start--;
                    }
                }
                if (start<old.size()-100) log_file->write(old.data()+start,qMin(old.size()-start, end-start));
                log_file->close();
                qDebug()<<"shrinked log file to "<<(double)start/1024.0/1024.<<"..."<<(double)end/1024.0/1024.0<<" MBytes";
            }

        }
        r=log_file->open(QIODevice::WriteOnly | QIODevice::Append);
    } else {
        r=log_file->open(QIODevice::WriteOnly);
    }
    if (!r) {
        log_error(tr("Could not open LOG file '%1' ...\n   description: %2\n").arg(filename).arg(log_file->errorString()));
        delete log_file;
        log_file=NULL;
    }
  }
  if (log_file!=NULL) {
      log_to_file=true;
      log_file_stream.setDevice(log_file);
      if (filename!=log_filename) {
          log_line();
          log_text(tr("-- starting to log:   %1\n").arg(QDateTime::currentDateTime().toString("yyyy-MMM-d hh:mm:ss")));
          log_line();
      }
      log_filename=filename;

        if (writeStore){
          for (int i=0; i<logStore.size(); i++) {
              log_file_stream<<logStore[i];
          }
        }
    }
}

void QtLogFile::close_logfile(){
  QMutexLocker locker(mutex);
  if (log_file!=NULL) {
    log_line();
    log_text(tr("-- stopping to log:   %1\n").arg(QDateTime::currentDateTime().toString("yyyy-MMM-d hh:mm:ss")));
    log_line();
    log_file_stream.flush();
    log_file_stream.setDevice(NULL);
    log_file->close();
    delete log_file;
    log_file=0;
  }
  log_file=NULL;
  log_filename="";
  log_to_file=false;
}

void QtLogFile::do_indent(){
  QMutexLocker locker(mutex);
  if (last_was_linebreak) {
      if (log_date_time) {
          raw_log_text(QDateTime::currentDateTime().toString(log_date_time_template)+QLatin1String(":  "));
      }
      if (current_indent>0) {
          QString in(current_indent, ' ');
          raw_log_text(in);
      }
      last_was_linebreak=false;
  }
}

QString QtLogFile::indentify(const QString &text){
  QMutexLocker locker(mutex);
  QString res="";
  if (text.size()==1) return text;
  if (text.size()>0) {
    for (int i=0; i<text.size(); i++) {
      res+=text[i];
      if ((i<text.size()-1)&&(text[i]=='\n')) {
        if (log_date_time) {
            res+=QDateTime::currentDateTime().toString(log_date_time_template)+QLatin1String(":  ");
        }
        if (current_indent>0) {
          QString in(current_indent, ' ');
          res+=in;
        }
      }
    }
  }
  return res;
};

void QtLogFile::raw_log_text(const QString &message){
  QMutexLocker locker(mutex);

  //try {
  logStore.push_back(message); // add log to store

  // now: output log to file and/or widget
  if (log_to_file) {
    log_file_stream<<message;
    log_file_stream.flush();
  }
  if (log_to_console) {
    browser->moveCursor(QTextCursor::End);
    QTextCursor tc = browser->textCursor();
    QTextCharFormat f=tc.blockCharFormat();
    QFont qf=font();
    qf.setBold(currentBold);
    qf.setFamily(fontName);
    qf.setPointSizeF(fontSize);
    f.setFont(qf);
    f.setForeground(currentColor);
    tc.setBlockCharFormat(f);
    tc.insertText(message);
    //tc.insertHtml(message);
    browser->setTextCursor(tc);
    QApplication::processEvents();
  }
//  } catch(...) {

//  }
}

void QtLogFile::log_linebreak(){
  QMutexLocker locker(mutex);
  do_indent();
  log_text("\n");
}

void QtLogFile::log_line(){
  QMutexLocker locker(mutex);
  do_indent();
  log_text("------------------------------------------------------------------------------\n");
}

void QtLogFile::log_dline(){
  QMutexLocker locker(mutex);
  do_indent();
  log_text("==============================================================================\n");
}

void QtLogFile::log_header(const QString &caption){
  QMutexLocker locker(mutex);
  int l=caption.size();
  int lall=78-2-l-1;
  do_indent();
  log_text("- ");
  log_text(caption);
  log_text(" ");
  if (lall>0) for (int i=0; i<lall; i++) log_text("-");
  log_linebreak();
}

void QtLogFile::log_dheader(const QString &caption){
  QMutexLocker locker(mutex);
  int l=caption.size();
  int lall=78-2-l-1;
  do_indent();
  log_text("= ");
  log_text(caption);
  log_text(" ");
  if (lall>0) for (int i=0; i<lall; i++) log_text("=");
  log_linebreak();
}

void QtLogFile::saveSettings(QSettings& s, QString namePrefix) {
  s.setValue(namePrefix+"logFontName", fontName);
  s.setValue(namePrefix+"logFontSize", fontSize);
  s.setValue(namePrefix+"logFixedPitch", fixedPitch);
  s.setValue(namePrefix+"logInfoColor", infoColor.name());
  s.setValue(namePrefix+"logErrorColor", errorColor.name());
  s.setValue(namePrefix+"logWarningColor", warningColor.name());
  s.setValue(namePrefix+"logTextColor", textColor.name());

}
void QtLogFile::readSettings(QSettings& s, QString namePrefix) {
  fontName=s.value(namePrefix+"logFontName", fontName).toString();
  fontSize=s.value(namePrefix+"logFontSize", fontSize).toDouble();
  fixedPitch=s.value(namePrefix+"logFixedPitch", fixedPitch).toBool();
  infoColor=QColor(s.value(namePrefix+"logInfoColor", infoColor.name()).toString());
  errorColor=QColor(s.value(namePrefix+"logErrorColor", errorColor.name()).toString());;
  warningColor=QColor(s.value(namePrefix+"logWarningColor", warningColor.name()).toString());;
  textColor=QColor(s.value(namePrefix+"logTextColor", textColor.name()).toString());;
}


void QtLogFile::log_text(const QString& message) {
  QMutexLocker locker(mutex);
  do_indent();
  raw_log_text(indentify(message));
  if (message.size()>0) {
    last_was_linebreak=false;
    if (message[message.size()-1]=='\n') last_was_linebreak=true;
  }
}

void QtLogFile::log_error(const QString &header, const QString &message) {
    QMutexLocker locker(mutex);
    log_error_header(header);
    log_error(message+QString("\n"));
    log_error_line();
}
void QtLogFile::log_warning(const QString &header, const QString &message) {
    QMutexLocker locker(mutex);
    log_warning_header(header);
    log_warning(message+QString("\n"));
    log_warning_line();
}
void QtLogFile::log_text(const QString &header, const QString &message) {
    QMutexLocker locker(mutex);
    log_header(header);
    log_text(message+QString("\n"));
    log_line();
}

void QtLogFile::log_error_linebreak(const QString& message) {
    log_error(message+QString("\n"));
}
void QtLogFile::log_warning_linebreak(const QString& message) {
    log_warning(message+QString("\n"));
}
void QtLogFile::log_text_linebreak(const QString& message) {
    log_text(message+QString("\n"));
}

void QtLogFile::log_error_linebreak_queue(const QString& message) {
    emit emit_log_error_linebreak(message+QString("\n"));
}

void QtLogFile::log_warning_linebreak_queue(const QString& message) {
    emit emit_log_warning_linebreak(message+QString("\n"));
}

void QtLogFile::log_text_linebreak_queue(const QString& message) {
    emit emit_log_text_linebreak(message+QString("\n"));
}

void QtLogFile::showLogFolder()
{
    QDesktopServices::openUrl( QUrl::fromLocalFile( QFileInfo(log_filename).absolutePath() ) );
}

void QtLogFile::showContextMenu(const QPoint& /*p*/ ) {
    QMenu *menu = new QMenu(this);

    QAction* act=new QAction(this);
    if (log_to_file) {
        act->setText(tr("log file: %1").arg(log_filename));
    } else {
        act->setText(tr("does not log to file ..."));
    }
    act->setEnabled(false);
    menu->addAction(act);
    menu->addSeparator();
    menu->addAction(actSelectAll);
    menu->addAction(actCopy);
    menu->addSeparator();
    menu->addAction(actClear);
    menu->addSeparator();
    menu->addAction(actShowLogFolder);
    menu->addAction(actFind);

    menu->exec(QCursor::pos());
    delete menu;
}

QString QtLogFile::htmlify(const QString &message) const
{
    QString res;
    res.reserve(message.size());
    for (int i=0; i<message.size(); i++) {
        QChar c=message[i];
        if (c.unicode()=='\"') res+="&quot;";
        else if (c.unicode()=='<') res+="&lt;";
        else if (c.unicode()=='>') res+="&gt;";
        else if (c.unicode()=='&') res+="&amp;";
        else if (c.unicode()=='\n') res+="<br>";
        else if (c.unicode()=='\r') res+="&#13;";
        else if (c.unicode()=='\t') res+="&#9;";
        else if (c.unicode()==' ') res+="&nbsp;";
        else res+=c;
    }
    return res;
}

QString QtLogFile::dehtmlify(const QString &message) const
{
    QString res=message;
    res=res.remove("<small>");
    res=res.remove("</small>");
    res=res.replace("<br>", "\n");
    res=res.replace("<br/>", "\n");
    res=res.replace("&nbsp;", " ");
    res=res.replace("&gt;", ">");
    res=res.replace("&lt;", "<");
    res=res.replace("&#10;", "\n");
    res=res.replace("&#13;", "\r");
    res=res.replace("&#9;", "\t");
    res=res.replace("&amp;", "&");
    return res;
}

void QtLogFile::log_warning(const QString& message){
    QMutexLocker locker(mutex);
  currentColor=warningColor;
  log_text(message);
  currentColor=textColor;
}

void QtLogFile::log_error(const QString& message){
    QMutexLocker locker(mutex);
  currentColor=errorColor;
  currentBold=true;
  log_text(message);
  currentBold=false;
  currentColor=textColor;
}

void QtLogFile::log_error_line(){
    QMutexLocker locker(mutex);
  currentColor=errorColor;
  currentBold=true;
  log_line();
  currentBold=false;
  currentColor=textColor;
}

void QtLogFile::log_error_dline(){
    QMutexLocker locker(mutex);
  currentColor=errorColor;
  currentBold=true;
  log_dline();
  currentBold=false;
  currentColor=textColor;
}

void QtLogFile::log_error_header(const QString &caption){
    QMutexLocker locker(mutex);
  currentColor=errorColor;
  currentBold=true;
  log_header(caption);
  currentBold=false;
  currentColor=textColor;
}

void QtLogFile::log_error_dheader(const QString &caption){
    QMutexLocker locker(mutex);
  currentColor=errorColor;
  currentBold=true;
  log_dheader(caption);
  currentBold=false;
  currentColor=textColor;
}


void QtLogFile::log_warning_line(){
    QMutexLocker locker(mutex);
  currentColor=warningColor;
  log_line();
  currentColor=textColor;
}

void QtLogFile::log_warning_dline(){
    QMutexLocker locker(mutex);
  currentColor=warningColor;
  log_dline();
  currentColor=textColor;
}

void QtLogFile::log_warning_header(const QString &caption){
    QMutexLocker locker(mutex);
  currentColor=warningColor;
  log_header(caption);
  currentColor=textColor;
}

void QtLogFile::log_warning_dheader(const QString &caption){
    QMutexLocker locker(mutex);
  currentColor=warningColor;
  log_dheader(caption);
  currentColor=textColor;
}

void QtLogFile::inc_indent()
{
    QMutexLocker locker(mutex);
    current_indent+=LOGFILE_INDENT_INC;
}

void QtLogFile::dec_indent()
{
    QMutexLocker locker(mutex);
    current_indent-=LOGFILE_INDENT_INC;
    if (current_indent<0) current_indent=0;
}

void QtLogFile::searchLog(bool checked) {
    widFind->setVisible(checked);
    if (checked) {
        edtFind->setFocus();
        edtFind->selectAll();
    }
    actFindNext->setEnabled(checked);
    actFindPrev->setEnabled(checked);
}


void QtLogFile::findNext()
{
    QString phrase=edtFind->text();
    QTextDocument::FindFlags options;
    if (chkCaseSensitive->isChecked()) options=options|QTextDocument::FindCaseSensitively;
    if (chkFindWholeWord->isChecked()) options=options|QTextDocument::FindWholeWords;
    if (phrase.isEmpty()) {
        browser->moveCursor(QTextCursor::Start);
    } else {
        bool found=browser->find(phrase, options);
        if (!found) {
            browser->moveCursor(QTextCursor::Start);
            found=browser->find(phrase, options);
        }
        QPalette p=edtFind->palette();
        if (!found) p.setColor(QPalette::Base, QColor("mistyrose"));
        else p.setColor(QPalette::Base, palette().color(QPalette::Base));
        edtFind->setPalette(p);
    }
}

void QtLogFile::findPrev()
{
    QString phrase=edtFind->text();
    QTextDocument::FindFlags options=QTextDocument::FindBackward;
    if (chkCaseSensitive->isChecked()) options=options|QTextDocument::FindCaseSensitively;
    if (chkFindWholeWord->isChecked()) options=options|QTextDocument::FindWholeWords;
    if (phrase.isEmpty()) {
        browser->moveCursor(QTextCursor::End);
    } else {
        bool found=browser->find(phrase, options);
        if (!found) {
            browser->moveCursor(QTextCursor::End);
            found=browser->find(phrase, options);
        }
        QPalette p=edtFind->palette();
        if (!found) p.setColor(QPalette::Base, QColor("mistyrose"));
        else p.setColor(QPalette::Base, palette().color(QPalette::Base));
        edtFind->setPalette(p);
    }
}

