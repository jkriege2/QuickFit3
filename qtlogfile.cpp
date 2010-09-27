/*
  Name: qtlogfile.cpp
  Copyright: (c) 2008
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/


#include "qtlogfile.h"

QtLogFile::QtLogFile(QWidget *parent):
    QTextEdit(parent), textColor(QString("black")),
    errorColor(QString("red")), warningColor(QString("darkgreen")),
    infoColor(QString("blue")), LogFile(), currentBold(false),
    fontSize(8), fontName(QString("Courier")), fixedPitch(true)
{
    set_log_to_console(true);
    setReadOnly(true);
    setLineWrapMode(QTextEdit::NoWrap);
    currentColor=textColor;
    QFont f=font();
    f.setFixedPitch(fixedPitch);
    f.setFamily(fontName);
    f.setPointSize(fontSize);
    setFont(f);
    logStore.clear();
}

void QtLogFile::open_logfile(std::string filename, bool writeStore) {
    LogFile::open_logfile(filename);
    if (writeStore){
      for (size_t i=0; i<logStore.size(); i++) {
          fprintf(log_file, logStore[i].c_str());
      }
    }
}


void QtLogFile::raw_log_text(std::string message){
  logStore.push_back(message); // add log to store

  // now: output log to file and/or widget
  if (log_to_file) {
    fprintf(log_file, message.c_str());
  }
  if (log_to_console) {
    moveCursor(QTextCursor::End);
    QTextCursor tc = textCursor();
    QTextCharFormat f=tc.blockCharFormat();
    QFont qf=font();
    qf.setBold(currentBold);
    qf.setFamily(fontName);
    qf.setPointSizeF(fontSize);
    f.setFont(qf);
    f.setForeground(currentColor);
    tc.setBlockCharFormat(f);
    tc.insertText(QString(message.c_str()));
    setTextCursor(tc);
  }
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


void QtLogFile::log_warning(std::string message){
  currentColor=warningColor;
  log_text(message);
  currentColor=textColor;
}

void QtLogFile::log_error(std::string message){
  currentColor=errorColor;
  currentBold=true;
  log_text(message);
  currentBold=false;
  currentColor=textColor;
}

void QtLogFile::log_error_line(){
  currentColor=errorColor;
  currentBold=true;
  log_line();
  currentBold=false;
  currentColor=textColor;
}

void QtLogFile::log_error_dline(){
  currentColor=errorColor;
  currentBold=true;
  log_dline();
  currentBold=false;
  currentColor=textColor;
}

void QtLogFile::log_error_header(std::string caption){
  currentColor=errorColor;
  currentBold=true;
  log_header(caption);
  currentBold=false;
  currentColor=textColor;
}

void QtLogFile::log_error_dheader(std::string caption){
  currentColor=errorColor;
  currentBold=true;
  log_dheader(caption);
  currentBold=false;
  currentColor=textColor;
}


void QtLogFile::log_warning_line(){
  currentColor=warningColor;
  log_line();
  currentColor=textColor;
}

void QtLogFile::log_warning_dline(){
  currentColor=warningColor;
  log_dline();
  currentColor=textColor;
}

void QtLogFile::log_warning_header(std::string caption){
  currentColor=warningColor;
  log_header(caption);
  currentColor=textColor;
}

void QtLogFile::log_warning_dheader(std::string caption){
  currentColor=warningColor;
  log_dheader(caption);
  currentColor=textColor;
}
