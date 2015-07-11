/*
  Name: baseimporter.cpp
  Copyright: (c) 2007
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/

#include "baseimporter.h" // class's header file


namespace sequencer {


baseImporter::baseImporter(sequenceGenerator* sg, channelManager* cm) {
  generator=sg;
  cmanager=cm;
  position_mode=biLine;
}


void baseImporter::add_error(int number, std::string message, std::string smessage, std::string filename, double time, long position) {
  bi_event e;
  e.type=biError;
  e.position=position;
  e.time=time;
  e.position=position;
  e.text=message;
  e.stext=smessage;
  e.num=number;
  e.filename=filename;
  events.push_back(e);
  errornum++;
}

void baseImporter::add_warning(int number, std::string message, std::string smessage, std::string filename, double time, long position) {
  bi_event e;
  e.type=biWarning;
  e.time=time;
  e.position=position;
  e.text=message;
  e.stext=smessage;
  e.num=number;
  e.filename=filename;
  events.push_back(e);
  warningnum++;
}

std::string baseImporter::get_position(bi_event e) {
  if (position_mode==biTime)
    return "(t="+floattostr(e.time)+")";
  if (position_mode==biLine)
    return "(l."+inttostr(e.position)+")";
  return  "(l."+inttostr(e.position)+", t="+floattostr(e.time)+")"; 
}

std::string baseImporter::get_report(){
  std::string report=""; //"  errors:    "+inttostr(errornum)+"\n  warnings:  "+inttostr(warningnum)+"\n\n";
  if (events.size()>0) {
    for (unsigned int i=0; i<events.size(); i++) {
      bi_event e=events[i];
      switch(e.type) {
        case biError:
          report+="error "+inttostr(e.num)+" in "+e.filename+get_position(e)+":\n"+e.text+ "\n\n";
          break;
        case biWarning:
          report+="warning "+inttostr(e.num)+" in "+e.filename+get_position(e)+":\n"+e.text+ "\n\n";
          break;
        case biInfo:
          report+="info "+inttostr(e.num)+" in "+e.filename+get_position(e)+":\n"+e.text+ "\n\n";
          break;
      }
    }
  }
  return report;
}

void baseImporter::print_report(FILE* conn){
  FILE* con=conn;
  if (con==NULL) con=stderr;
  if (events.size()>0) {
    for (unsigned int i=0; i<events.size(); i++) {
      bi_event e=events[i];
      std::string t="";
      switch(e.type) {
        case biError:
          #ifdef __WINDOWS__
          SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
          #endif
          t="error "+inttostr(e.num)+" in "+e.filename+e.filename+get_position(e)+": "+e.text+ "\n\n";
          fprintf(con, "%s", t.c_str());;
          break;
        case biWarning:
          #ifdef __WINDOWS__
          SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE);
          #endif
          t="warning "+inttostr(e.num)+" in "+e.filename+e.filename+get_position(e)+": "+e.text+ "\n\n";
          fprintf(con, "%s", t.c_str());;
          break;
        case biInfo:
          t="info "+inttostr(e.num)+" in "+e.filename+get_position(e)+": "+e.text+ "\n\n";
          fprintf(con, "%s", t.c_str());;
          break;
      }
      #ifdef __WINDOWS__
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
      #endif
    }
  }
}




};
