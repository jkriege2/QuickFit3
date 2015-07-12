/*
  Name: qtlogfile2.cpp
  Copyright: (c) 2012
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/
#include "qtlogfile2.h"
#include <QApplication>
#include <QFileInfo>


QtLogFile2Model::logMessage::logMessage()
{
    isLine=false;
    message="";
    header="";
    indent=0;
    timestamp=QDateTime::currentDateTime();
    type=QtLogFile2Model::logText;
}

QtLogFile2Model::QtLogFile2Model(QObject *parent):
    QAbstractTableModel(parent), textColor(QString("black")),
    errorColor(QString("red")), warningColor(QString("darkorange")),
    infoColor(QString("blue")),
    fontSize(8), fontName(QString("Courier")), fixedPitch(true)
{
    log_file=NULL;
    log_filename=QApplication::applicationFilePath();
    if (QFileInfo(log_filename).suffix().isEmpty()) {
        log_filename.append(".log");
    } else {
        log_filename=log_filename.left(QFileInfo(log_filename).completeSuffix().size())+"log";
    }
    log_to_file=false;
    last_was_linebreak=true;
    current_indent=0;
    log_file_append=false;
    currentType=QtLogFile2Model::logText;
    logStore.clear();
}

QtLogFile2Model::~QtLogFile2Model() {
    current_indent=0;
    log_linebreak();
    log_linebreak();
    close_logfile();
}

int QtLogFile2Model::rowCount(const QModelIndex &parent) const
{
    return logStore.size();
}

int QtLogFile2Model::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant QtLogFile2Model::data(const QModelIndex &index, int role) const
{
    int row=index.row();
    int col=index.column();
    if (role==Qt::DisplayRole) {
        if (row>=0 && row<logStore.size()) {
            if (col==0) return logStore[row].message;
        }
    } else if (role==Qt::DecorationRole) {
        if (row>=0 && row<logStore.size()) {
            if (col==0) {
                switch (logStore[row].type) {
                    case QtLogFile2Model::logError: return QIcon(":/QtLogFile2Model/logo_error.png");
                    case QtLogFile2Model::logWarning: return QIcon(":/QtLogFile2Model/logo_warning.png");
                    case QtLogFile2Model::logText: return QVariant();
                }
            }
        }
    } else if (role==Qt::FontRole) {
        QFont f=QApplication::font();
        f.setFixedPitch(fixedPitch);
        f.setFamily(fontName);
        f.setPointSize(fontSize);
        return f;
    } else if (role==Qt::TextColorRole) {
        if (row>=0 && row<logStore.size()) {
            if (col==0) {
                switch (logStore[row].type) {
                    case QtLogFile2Model::logError: return errorColor;
                    case QtLogFile2Model::logWarning: return warningColor;
                    case QtLogFile2Model::logText:
                    default:
                        return textColor;
                }
            }
        }
    }
    return QVariant();
}

void QtLogFile2Model::clearLog()
{
    logStore.clear();
    reset();
}

void QtLogFile2Model::inc_indent()
{
    current_indent+=LOGFILE2_INDENT_INC;
}

void QtLogFile2Model::dec_indent()
{
    current_indent-=LOGFILE2_INDENT_INC;
    if (current_indent<0) current_indent=0;
}

void QtLogFile2Model::open_logfile(QString filename) {
  if (filename!=log_filename) close_logfile();
  log_to_file=false;
  log_file=new QFile(filename);
  if (log_file==NULL) log_error(tr("Could not open LOG file '%1' ...").arg(filename));
  else {
    bool r=false;
    if (log_file_append) r=log_file->open(QIODevice::WriteOnly | QIODevice::Append);
    else r=log_file->open(QIODevice::WriteOnly);
    if (!r) {
        delete log_file;
        log_file=NULL;
        log_error(tr("Could not open LOG file '%1' ...").arg(filename));
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

    }
}

void QtLogFile2Model::close_logfile(){
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

void QtLogFile2Model::do_indent(){
  if (last_was_linebreak) {
    if (current_indent>0) {
      QString in(current_indent, ' ');
      raw_log_text(in);
    }
    last_was_linebreak=false;
  }
}

QString QtLogFile2Model::indentify(QString text){
  QString res="";
  if (text.size()==1) return text;
  if (text.size()>0) {
    for (int i=0; i<text.size(); i++) {
      res+=text[i];
      if ((i<text.size()-1)&&(text[i]=='\n')) {
        if (current_indent>0) {
          QString in(current_indent, ' ');
          res+=in;
        }
      }
    }
  }
  return res;
};

void QtLogFile2Model::raw_log_text(QString message){

  // now: output log to file and/or widget
  if (log_to_file) {
    log_file_stream<<message;
    log_file_stream.flush();
  }
  QStringList sl=message.split('\n');
  QtLogFile2Model::logMessage m;
  bool append=true;
  if (logStore.size()>0) {
      m=logStore.last();
      append=false;
  }
  if (last_was_linebreak) sl.append("");
  QModelIndex appended;
  for (int i=0; i<sl.size(); i++) {
      m.message.append(sl[i]);
      m.type=currentType;
      if (append) {
          beginInsertRows(QModelIndex(), logStore.size(), logStore.size());
          logStore.append(m);
          endInsertRows();
          appended=index(logStore.size()-1, 0);
      } else {
          logStore.last()=m;
          emit dataChanged(index(logStore.size()-1, 0), index(logStore.size()-1, columnCount()));
      }
      append=true;
      m=QtLogFile2Model::logMessage();
  }
  if (appended.isValid())  emit messageAppended(appended);
}

void QtLogFile2Model::log_linebreak(){
  do_indent();
  log_text("\n");
}

void QtLogFile2Model::log_line(){
  do_indent();
  log_text("------------------------------------------------------------------------------\n");
}

void QtLogFile2Model::log_dline(){
  do_indent();
  log_text("==============================================================================\n");
}

void QtLogFile2Model::log_header(QString caption){
  int l=caption.size();
  int lall=78-2-l-1;
  do_indent();
  log_text("- ");
  log_text(caption);
  log_text(" ");
  if (lall>0) for (int i=0; i<lall; i++) log_text("-");
  log_linebreak();
}

void QtLogFile2Model::log_dheader(QString caption){
  int l=caption.size();
  int lall=78-2-l-1;
  do_indent();
  log_text("= ");
  log_text(caption);
  log_text(" ");
  if (lall>0) for (int i=0; i<lall; i++) log_text("=");
  log_linebreak();
}

void QtLogFile2Model::saveSettings(QSettings& s, QString namePrefix) {
  s.setValue(namePrefix+"logFontName", fontName);
  s.setValue(namePrefix+"logFontSize", fontSize);
  s.setValue(namePrefix+"logFixedPitch", fixedPitch);
  s.setValue(namePrefix+"logInfoColor", infoColor.name());
  s.setValue(namePrefix+"logErrorColor", errorColor.name());
  s.setValue(namePrefix+"logWarningColor", warningColor.name());
  s.setValue(namePrefix+"logTextColor", textColor.name());

}
void QtLogFile2Model::readSettings(QSettings& s, QString namePrefix) {
  fontName=s.value(namePrefix+"logFontName", fontName).toString();
  fontSize=s.value(namePrefix+"logFontSize", fontSize).toDouble();
  fixedPitch=s.value(namePrefix+"logFixedPitch", fixedPitch).toBool();
  infoColor=QColor(s.value(namePrefix+"logInfoColor", infoColor.name()).toString());
  errorColor=QColor(s.value(namePrefix+"logErrorColor", errorColor.name()).toString());;
  warningColor=QColor(s.value(namePrefix+"logWarningColor", warningColor.name()).toString());;
  textColor=QColor(s.value(namePrefix+"logTextColor", textColor.name()).toString());;
}


void QtLogFile2Model::log_text(QString message) {
  do_indent();
  raw_log_text(indentify(message));
  if (message.size()>0) {
    last_was_linebreak=false;
    if (message[message.size()-1]=='\n') last_was_linebreak=true;
  }
}

void QtLogFile2Model::log_error(QString header, QString message) {
    log_error_header(header);
    log_error(message+QString("\n"));
    log_error_line();
}
void QtLogFile2Model::log_warning(QString header, QString message) {
    log_warning_header(header);
    log_warning(message+QString("\n"));
    log_warning_line();
}
void QtLogFile2Model::log_text(QString header, QString message) {
    log_header(header);
    log_text(message+QString("\n"));
    log_line();
}

void QtLogFile2Model::log_error_linebreak(QString message) {
    log_error(message+QString("\n"));
}
void QtLogFile2Model::log_warning_linebreak(QString message) {
    log_warning(message+QString("\n"));
}
void QtLogFile2Model::log_text_linebreak(QString message) {
    log_text(message+QString("\n"));
}

void QtLogFile2Model::log_warning(QString message){

    currentType=QtLogFile2Model::logWarning;
    log_text(message);
    currentType=QtLogFile2Model::logText;
}

void QtLogFile2Model::log_error(QString message){

    currentType=QtLogFile2Model::logError;
    log_text(message);
    currentType=QtLogFile2Model::logText;
}

void QtLogFile2Model::log_error_line(){

    currentType=QtLogFile2Model::logError;
    log_line();
    currentType=QtLogFile2Model::logText;
}

void QtLogFile2Model::log_error_dline(){

    currentType=QtLogFile2Model::logError;
    log_dline();
    currentType=QtLogFile2Model::logText;
}

void QtLogFile2Model::log_error_header(QString caption){

    currentType=QtLogFile2Model::logError;
    log_header(caption);
    currentType=QtLogFile2Model::logText;
}

void QtLogFile2Model::log_error_dheader(QString caption){

    currentType=QtLogFile2Model::logError;
    log_dheader(caption);
    currentType=QtLogFile2Model::logText;
}


void QtLogFile2Model::log_warning_line(){

    currentType=QtLogFile2Model::logWarning;
    log_line();
    currentType=QtLogFile2Model::logText;
}

void QtLogFile2Model::log_warning_dline(){

    currentType=QtLogFile2Model::logWarning;
    log_dline();
    currentType=QtLogFile2Model::logText;
}

void QtLogFile2Model::log_warning_header(QString caption){

    currentType=QtLogFile2Model::logWarning;
    log_header(caption);
    currentType=QtLogFile2Model::logText;
}

void QtLogFile2Model::log_warning_dheader(QString caption){

    currentType=QtLogFile2Model::logWarning;
    log_dheader(caption);
    currentType=QtLogFile2Model::logText;
}

























QtLogFile2::QtLogFile2(QWidget *parent):
    QWidget(parent)
{
    logStore=new QtLogFile2Model(NULL);
    lay=new QVBoxLayout(this);
    lay->setContentsMargins(0,0,0,0);
    setLayout(lay);

    list=new QListView(this);
    list->setFlow(QListView::TopToBottom);
    list->setViewMode(QListView::ListMode);
    list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    list->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    list->setFrameStyle(QFrame::NoFrame);
    list->setWindowTitle(tr("Logging"));
    list->setSelectionMode(QAbstractItemView::SingleSelection);
    list->setResizeMode(QListView::Fixed);
    lay->addWidget(list);

    list->setModel(logStore);
    connect(logStore, SIGNAL(messageAppended(QModelIndex)), this, SLOT(messageAppended(QModelIndex)));
}

QtLogFile2::~QtLogFile2() {
    delete logStore;
}

void QtLogFile2::clearLog()
{
    logStore->clearLog();
}

void QtLogFile2::open_logfile(QString filename)
{
    logStore->open_logfile(filename);
}

void QtLogFile2::saveSettings(QSettings &s, QString namePrefix)
{
    logStore->saveSettings(s, namePrefix);
}

void QtLogFile2::readSettings(QSettings &s, QString namePrefix)
{
    logStore->readSettings(s, namePrefix);
}

QString QtLogFile2::get_log_filename() const
{
    return logStore->get_log_filename();
}


bool QtLogFile2::get_log_to_file() const
{
    return logStore->get_log_to_file();
}

void QtLogFile2::set_log_file_append(bool append)
{
    logStore->set_log_file_append(append);
}

bool QtLogFile2::get_log_file_append() const
{
    return logStore->get_log_file_append();
}

void QtLogFile2::close_logfile()
{
    logStore->close_logfile();
}

void QtLogFile2::log_linebreak()
{
    logStore->log_linebreak();
}

void QtLogFile2::log_line()
{
    logStore->log_line();
}

void QtLogFile2::log_dline()
{
    logStore->log_dline();
}

void QtLogFile2::log_header(QString caption)
{
    logStore->log_header(caption);
}

void QtLogFile2::log_dheader(QString caption)
{
    logStore->log_dheader(caption);
}

void QtLogFile2::log_error_line()
{
    logStore->log_error_line();
}

void QtLogFile2::log_error_dline()
{
    logStore->log_error_dline();
}

void QtLogFile2::log_error_header(QString caption)
{
    logStore->log_error_header(caption);
}

void QtLogFile2::log_error_dheader(QString caption)
{
    logStore->log_error_dheader(caption);
}

void QtLogFile2::log_warning_line()
{
    logStore->log_warning_line();
}

void QtLogFile2::log_warning_dline()
{
    logStore->log_warning_dline();
}

void QtLogFile2::log_warning_header(QString caption)
{
    logStore->log_warning_header(caption);
}

void QtLogFile2::log_warning_dheader(QString caption)
{
    logStore->log_warning_dheader(caption);
}

void QtLogFile2::inc_indent()
{
    logStore->inc_indent();
}

void QtLogFile2::dec_indent()
{
    logStore->dec_indent();
}

void QtLogFile2::log_text(QString header, QString message)
{
    logStore->log_text(header, message);
}

void QtLogFile2::log_warning(QString header, QString message)
{
    logStore->log_warning(header, message);
}

void QtLogFile2::log_error(QString header, QString message)
{
    logStore->log_error(header, message);
}

void QtLogFile2::log_text(QString message)
{
    logStore->log_text(message);
}

void QtLogFile2::log_warning(QString message)
{
    logStore->log_warning(message);
}

void QtLogFile2::log_error(QString message)
{
    logStore->log_error(message);
}

void QtLogFile2::log_error_linebreak(QString message)
{
    logStore->log_error_linebreak(message);
}

void QtLogFile2::log_warning_linebreak(QString message)
{
    logStore->log_warning_linebreak(message);
}

void QtLogFile2::log_text_linebreak(QString message)
{
    logStore->log_text_linebreak(message);
}

void QtLogFile2::messageAppended(const QModelIndex &message) {
    list->scrollTo(message);
}

