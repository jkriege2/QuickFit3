/*
  Name: logfile.cpp
  Copyright: (c) 2007
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/

#include "logfile.h" // class's header file


LogFile::LogFile() {
  log_console=stderr;
  log_file=NULL;
  log_filename="";
  log_to_console=true;
  log_to_file=false;
  last_was_linebreak=true;
  current_indent=0;
  log_file_append=false;
}

LogFile::~LogFile() {
   current_indent=0;
   log_linebreak();
   log_linebreak();
   close_logfile();
}

void LogFile::open_logfile(std::string filename){
  time_t curtime;
  struct tm *loctime;
  /* Get the current time. */
  curtime = time (NULL);
  /* Convert it to local time representation. */
  loctime = localtime (&curtime);

  if (filename!=log_filename) close_logfile();
  if (log_file_append) {
    log_file=fopen(filename.c_str(), "a");
  } else {
    log_file=fopen(filename.c_str(), "w");
  }
  log_to_file=true;
  if (filename!=log_filename) {
      log_line();
      log_text(format("-- starting to log:   %s", asctime (loctime)));
      log_line();
  }
  log_filename=filename;
}

void LogFile::close_logfile(){
  time_t curtime;
  struct tm *loctime;
  /* Get the current time. */
  curtime = time (NULL);
  /* Convert it to local time representation. */
  loctime = localtime (&curtime);
  if (log_file!=NULL) {
    log_line();
    log_text(format("-- stopping to log:   %s", asctime(loctime)));
    log_line();
    fclose(log_file);
  }
  log_file=NULL;
  log_filename="";
  log_to_file=false;
}

void LogFile::do_indent(){
  if (last_was_linebreak) {
    if (current_indent>0) {
      std::string in(current_indent, ' ');
      raw_log_text(in);
    }
    last_was_linebreak=false;
  }
}

std::string LogFile::indentify(std::string text){
  std::string res="";
  if (text.size()==1) return text;
  if (text.size()>0) {
    for (int i=0; i<text.size(); i++) {
      res+=text[i];
      if ((i<text.size()-1)&&(text[i]=='\n')) {
        if (current_indent>0) {
          std::string in(current_indent, ' ');
          res+=in;
        }
      }
    }
  }
  return res;
};

void LogFile::raw_log_text(std::string message){
  if (log_to_file) {
    fprintf(log_file, message.c_str());
  }
  if (log_to_console) {
    fprintf(log_console, message.c_str());
  }
}

void LogFile::log_text(std::string message){
  do_indent();
  raw_log_text(indentify(message));
  if (message.size()>0) {
    last_was_linebreak=false;
    if (message[message.size()-1]=='\n') last_was_linebreak=true;
  }
}

void LogFile::log_warning(std::string message){
  #ifdef __WINDOWS__
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE);
  #endif
  log_text(message);
  #ifdef __WINDOWS__
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
  #endif
}

void LogFile::log_error(std::string message){
  #ifdef __WINDOWS__
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
  #endif
  log_text(message);
  #ifdef __WINDOWS__
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
  #endif
}

void LogFile::log_linebreak(){
  do_indent();
  log_text("\n");
};

void LogFile::log_line(){
  do_indent();
  log_text("------------------------------------------------------------------------------\n");
}

void LogFile::log_dline(){
  do_indent();
  log_text("==============================================================================\n");
}

void LogFile::log_header(std::string caption){
  int l=caption.size();
  int lall=78-2-l-1;
  do_indent();
  log_text("- ");
  log_text(caption);
  log_text(" ");
  if (lall>0) for (int i=0; i<lall; i++) log_text("-");
  log_linebreak();
}

void LogFile::log_dheader(std::string caption){
  int l=caption.size();
  int lall=78-2-l-1;
  do_indent();
  log_text("= ");
  log_text(caption);
  log_text(" ");
  if (lall>0) for (int i=0; i<lall; i++) log_text("=");
  log_linebreak();
}

void LogFile::log_error_line(){
  #ifdef __WINDOWS__
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
  #endif
  log_line();
  #ifdef __WINDOWS__
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
  #endif
}

void LogFile::log_error_dline(){
  #ifdef __WINDOWS__
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
  #endif
  log_dline();
  #ifdef __WINDOWS__
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
  #endif
}

void LogFile::log_error_header(std::string caption){
  #ifdef __WINDOWS__
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
  #endif
  log_header(caption);
  #ifdef __WINDOWS__
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
  #endif
}

void LogFile::log_error_dheader(std::string caption){
  #ifdef __WINDOWS__
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
  #endif
  log_dheader(caption);
  #ifdef __WINDOWS__
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
  #endif
}


void LogFile::log_warning_line(){
  #ifdef __WINDOWS__
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE);
  #endif
  log_line();
  #ifdef __WINDOWS__
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
  #endif
}

void LogFile::log_warning_dline(){
  #ifdef __WINDOWS__
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE);
  #endif
  log_dline();
  #ifdef __WINDOWS__
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
  #endif
}

void LogFile::log_warning_header(std::string caption){
  #ifdef __WINDOWS__
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE);
  #endif
  log_header(caption);
  #ifdef __WINDOWS__
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
  #endif
}

void LogFile::log_warning_dheader(std::string caption){
  #ifdef __WINDOWS__
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE);
  #endif
  log_dheader(caption);
  #ifdef __WINDOWS__
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif
}

