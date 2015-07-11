/*
  Name: sdffhwcommands.cpp
  Copyright: (c) 2007
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/

#include "sdffhwcommands.h" // class's header file
#include "sdffparser.h"


namespace sequencer {

std::string sdff_serial_paritymode2str(sdff_serial_paritymodes mode){
    switch(mode) {
        case sdffserNOPARITY: return "NO_PARITY";
        case sdffserEVEN: return "EVEN_PARITY";
        case sdffserODD: return "ODD_PARITY";
    }
    return "UNKNOWN_PARITY";
}

sdff_serial_paritymodes str2sdff_serial_paritymodes(std::string mode){
    std::string s=tolower(mode);
    if (s=="none" || s=="no_parity" || s=="off") return sdffserNOPARITY;
    if (s=="even" || s=="even_parity") return sdffserEVEN;
    if (s=="odd" || s=="odd_parity") return sdffserODD;
    return sdffserNOPARITY;
}

std::string sdff_serial_handshaking2str(sdff_serial_handshaking mode){
    switch(mode) {
        case sdffserNOHANDSHAKING: return "NO_HANDSHAKING";
        case sdffserXONXOFF: return "XONXOFF_HANDSHAKING";
        case sdffserHARDWARE: return "HARDWARE_HANDSHAKING";
    }
    return "UNKNOWN_HANDSHAKING";
}

sdff_serial_handshaking str2sdff_serial_handshaking(std::string mode){
    std::string s=tolower(mode);
    if (s=="none" || s=="no_handshaking" || s=="off") return sdffserNOHANDSHAKING;
    if (s=="software" || s=="xonxoff") return sdffserXONXOFF;
    if (s=="hardware") return sdffserHARDWARE;
    return sdffserNOHANDSHAKING;
}


std::string command_classtostring(unsigned int command_class) {
  switch(command_class) {
    case SDFF_CMDclass_SERIAL:
      return "serial interface";
    case SDFF_CMDclass_SERGPIB:
      return "serial-to-GPIB interface";
    default:
      return "unknown";
  }
};


void SDFFHWcommandManager::clear(){
  std::map<int, std::vector<SDFFcommand*> >::const_iterator i;
  for (i=commands.begin(); i!=commands.end(); ++i) {
    if (i->second.size()>0) for (size_t j=0; j<i->second.size(); j++) {
      delete i->second[j];
    }
  }
  commands.clear();
};

std::string SDFFHWcommandManager::report(){
  std::string res="";
  std::map<int, std::vector<SDFFcommand*> >::const_iterator i;
  for (i=commands.begin(); i!=commands.end(); ++i) {
    res+=command_classtostring(i->first)+ " commands:\n";
    if (i->second.size()>0) for (size_t j=0; j<i->second.size(); j++) {
      res+="  "+i->second[j]->tostring()+"\n";
    }
  }
  return res;
};

std::string SDFFHWcommandManager::report_results(){
  std::string res="";
  std::map<int, std::vector<SDFFcommand*> >::const_iterator i;
  for (i=commands.begin(); i!=commands.end(); ++i) {
    res+=command_classtostring(i->first)+ " command resultss:\n";
    if (i->second.size()>0) for (size_t j=0; j<i->second.size(); j++) {
      std::string r=i->second[j]->report_result();
      if (r!="") res+="  "+r+"\n";
    }
  }
  return res;
};

void SDFFHWcommandManager::clear_results(){
  std::map<int, std::vector<SDFFcommand*> >::const_iterator i;
  for (i=commands.begin(); i!=commands.end(); ++i) {
    if (i->second.size()>0) for (size_t j=0; j<i->second.size(); j++) {
      i->second[j]->clear_result();
    }
  }
}


std::vector<sdff_hwcmd_result> SDFFHWcommandManager::get_result_list() {
  std::vector<sdff_hwcmd_result> res;
  std::map<int, std::vector<SDFFcommand*> >::const_iterator i;
  for (i=commands.begin(); i!=commands.end(); ++i) {
    if (i->second.size()>0) for (size_t j=0; j<i->second.size(); j++) {
      sdff_hwcmd_result r;
      r.command_class=i->first;
      r.result=i->second[j]->get_result();
      res.push_back(r);
    }
  }
  return res;
}

};

