/*
  Name: sdfftools.cpp
  Copyright: (c) 2007
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/

#include "sdfftools.h" // class's header file
#include "sdffparser.h"


namespace sequencer {

/* =======================================================================================================================
 * = tool functions and methods                                                                                          =
 * =======================================================================================================================*/
std::string sdff_vartypetostr(sdff_vartype t){
  switch(t) {
    case sdffVTString: return "string"; break;
    case sdffVTInteger: return "integer"; break;
    case sdffVTDouble: return "double"; break;
    case sdffVTBoolean: return "bool"; break;
    case sdffVTVoid: return "void"; break;
  }
  return "";
}

std::string sdff_resulttypetostr(sdff_resulttype t){
  switch(t) {
    case sdffRTString: return "string"; break;
    case sdffRTInteger: return "integer"; break;
    case sdffRTDouble: return "double"; break;
    case sdffRTBoolean: return "bool"; break;
    case sdffRTVoid: return "void"; break;
  }
  return "";
}

std::string sdff_resulttostr(sdff_result r){
  switch(r.type) {
    case sdffRTString: return "'"+r.stringValue+"' (string)"; break;
    case sdffRTInteger: return inttostr(r.intValue)+" (integer)"; break;
    case sdffRTDouble: return floattostr(r.doubleValue)+" (double)"; break;
    case sdffRTBoolean: return booltostr(r.boolValue)+" (bool)"; break;
    case sdffRTVoid: return "void"; break;
  }
  return "";
}

std::string asdff_resulttostr(sdff_result r){
  switch(r.type) {
    case sdffRTString: return "'"+r.stringValue+"'"; break;
    case sdffRTInteger: return inttostr(r.intValue); break;
    case sdffRTDouble: return floattostr(r.doubleValue); break;
    case sdffRTBoolean: return booltostr(r.boolValue); break;
    case sdffRTVoid: return "void"; break;
  }
  return "";
}

std::string aasdff_resulttostr(sdff_result r){
  switch(r.type) {
    case sdffRTString: return r.stringValue; break;
    case sdffRTInteger: return inttostr(r.intValue); break;
    case sdffRTDouble: return floattostr(r.doubleValue); break;
    case sdffRTBoolean: return booltostr(r.boolValue); break;
    case sdffRTVoid: return "void"; break;
  }
  return "";
}

bool sdff_types_equal(sdff_vartype v, sdff_resulttype r) {
  switch(r) {
    case sdffRTString: return v==sdffVTString;
    case sdffRTInteger: return v==sdffVTInteger;
    case sdffRTDouble: return v==sdffVTDouble;
    case sdffRTBoolean: return v==sdffVTBoolean;
    case sdffRTVoid: return v==sdffVTVoid;
  }
  return false;
}

std::string sdff_paramtypetostr(sdff_paramtype t){
  switch(t) {
    case sdffPTString: return "string"; break;
    case sdffPTInteger: return "integer"; break;
    case sdffPTDouble: return "double"; break;
    case sdffPTBoolean: return "bool"; break;
    case sdffPTUnknown: return "arbitrary"; break;
  }
  return "";
}





/* =======================================================================================================================
 * = SDFFVariableManager                                                                                                 =
 * =======================================================================================================================*/
sdff_variable& SDFFVariableManager::addi(unsigned int block, std::string name, sdff_vartype type, bool constant){
  if (block>=variables.size()) SEQUENCER_ERROR(SEQUENCER_SDFFERROR_LEVELTH_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_LEVELTH_NUM), block), "SDFFVariableManager.addi("+inttostr(block)+"\""+name+"\")");
  sdff_variable v;
  v.name=name;
  v.internal=true;
  v.constant=constant;
  v.type=type;
  v.stringValue=NULL;
  v.intValue=NULL;
  v.doubleValue=NULL;
  v.boolValue=NULL;
  switch(type) {
    case sdffVTString: v.stringValue=new std::string; break;
    case sdffVTInteger: v.intValue=new long long; break;
    case sdffVTDouble: v.doubleValue=new double; break;
    case sdffVTBoolean: v.boolValue=new bool; break;
    case sdffVTVoid:  break;
  }
  (*variables[block])[name]=v;
  return (*variables[block])[name];
}

sdff_variable& SDFFVariableManager::adde(unsigned int block, std::string name, sdff_vartype type, void* data, bool constant){
  if (block>=variables.size()) SEQUENCER_ERROR(SEQUENCER_SDFFERROR_LEVELTH_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_LEVELTH_NUM), block), "SDFFVariableManager.addi("+inttostr(block)+"\""+name+"\")");
  sdff_variable v;
  v.name=name;
  v.internal=false;
  v.constant=constant;
  v.type=type;
  v.stringValue=NULL;
  v.intValue=NULL;
  v.doubleValue=NULL;
  v.boolValue=NULL;
  switch(type) {
    case sdffVTString: v.stringValue=(std::string*)data; break;
    case sdffVTInteger: v.intValue=(long long*)data; break;
    case sdffVTDouble: v.doubleValue=(double*)data; break;
    case sdffVTBoolean: v.boolValue=(bool*)data; break;
    case sdffVTVoid: break;
  }
  (*variables[block])[name]=v;
  return (*variables[block])[name];
}

SDFFVariableManager::SDFFVariableManager(){
  constant_assign_error=false;
  type_changeable=true;
  write_global=false;
  clear();
}

SDFFVariableManager::~SDFFVariableManager(){
  clear();
  variables.clear();
}

void SDFFVariableManager::clear(){
  variables.clear();
  sdffVariableMap* m=new sdffVariableMap;
  m->clear();
  variables.push_back(m);
};

void SDFFVariableManager::clear_block(int block){
  if (variables[block]->size()>0) {
    for (sdffVariableMap::const_iterator i=variables[block]->begin(); i!=variables[block]->end(); i++) {
      if (i->second.internal) {
        switch(i->second.type) {
          case sdffVTString: delete i->second.stringValue; break;
          case sdffVTInteger: delete i->second.intValue; break;
          case sdffVTDouble: delete i->second.doubleValue; break;
          case sdffVTBoolean: delete i->second.boolValue; break;
          case sdffVTVoid: break;
        }
      }
    }
    variables[block]->clear();
  }
};

void SDFFVariableManager::enter_block(){
  sdffVariableMap* m=new sdffVariableMap;
  m->clear();
  variables.push_back(m);
};

void SDFFVariableManager::leave_block(){
  if (variables.size()>1) {
    clear_block(variables.size()-1);
    delete variables[variables.size()-1];
    variables.pop_back();
  } else SEQUENCER_ERRORN(SEQUENCER_SDFFERROR_NBTL_NUM, "SDFFVariableManager.leave_block()");
};

sdff_result SDFFVariableManager::get(std::string name){
  int level=variables.size()-1;
  while (level>=0) {
    sdffVariableMap::const_iterator i=variables[level]->find(name);
    if (i!=variables[level]->end()) {
      sdff_result r;
      r.type=sdffRTVoid;
      switch(i->second.type) {
        case sdffVTString:
          r.type=sdffRTString;
          r.stringValue=*(i->second.stringValue);
          break;
        case sdffVTInteger:
          r.type=sdffRTInteger;
          r.intValue=*(i->second.intValue);
          break;
        case sdffVTDouble:
          r.type=sdffRTDouble;
          r.doubleValue=*(i->second.doubleValue);
          break;
        case sdffVTBoolean:
          r.type=sdffRTBoolean;
          r.boolValue=*(i->second.boolValue);
          break;
        case sdffVTVoid:
          r.type=sdffRTVoid;
          break;
      }
      return r;
    }
    level--;
  }
  SEQUENCER_ERROR(SEQUENCER_SDFFERROR_VARNOTFOUND_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_VARNOTFOUND_NUM), name.c_str()), "SDFFVariableManager.get(\""+name+"\")");
};

void SDFFVariableManager::set_constant(std::string name, bool value) {
  int level=variables.size()-1;
  while (level>=0) {
    sdffVariableMap::const_iterator i=variables[level]->find(name);
    if (i!=variables[level]->end()) {
      (*variables[level])[name].constant=value;
      return;
    }
    level--;
  }
  SEQUENCER_ERROR(SEQUENCER_SDFFERROR_VARNOTFOUND_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_VARNOTFOUND_NUM), name.c_str()), "SDFFVariableManager.get(\""+name+"\")");
};

sdff_variable& SDFFVariableManager::int_get(std::string name){
  int level=variables.size()-1;
  while (level>=0) {
    sdffVariableMap::const_iterator i=variables[level]->find(name);
    if (i!=variables[level]->end()) {
      return (*variables[level])[name];
    }
    level--;
  }
  SEQUENCER_ERROR(SEQUENCER_SDFFERROR_VARNOTFOUND_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_VARNOTFOUND_NUM), name.c_str()), "SDFFVariableManager.int_get(\""+name+"\")");
};

void SDFFVariableManager::set(std::string name, sdff_result value){
  int level=variables.size()-1;
  while (level>=0) {
    sdffVariableMap::const_iterator i=variables[level]->find(name);
    if (i!=variables[level]->end()) {
      if (i->second.constant) {
        if (!constant_assign_error) return;
        else SEQUENCER_ERROR(SEQUENCER_SDFFERROR_VARCONSTANT_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_VARCONSTANT_NUM), name.c_str()), "SDFFVariableManager.set(\""+name+"\", "+sdff_resulttostr(value)+")");
      } else {
        if (sdff_types_equal(i->second.type, value.type)) {
          switch(i->second.type) {
            case sdffVTString: *(i->second.stringValue)=value.stringValue; return; break;
            case sdffVTInteger: *(i->second.intValue)=value.intValue; return; break;
            case sdffVTDouble: *(i->second.doubleValue)=value.doubleValue; return; break;
            case sdffVTBoolean: *(i->second.boolValue)=value.boolValue; return; break;
            case sdffVTVoid: return; break; // this may never appear!!
          }
        } else {
          if (! i->second.internal) {
            if (type_changeable) {
              SEQUENCER_ERROR(SEQUENCER_SDFFERROR_EXTNOTC_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_EXTNOTC_NUM), name.c_str()), "SDFFVariableManager.set(\""+name+"\", "+sdff_resulttostr(value)+")");
            } else {
              SEQUENCER_ERROR(SEQUENCER_SDFFERROR_SETTYPEMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_SETTYPEMISS_NUM), sdff_resulttypetostr(value.type).c_str(), sdff_vartypetostr(i->second.type).c_str(), name.c_str()), "SDFFVariableManager.set(\""+name+"\", "+sdff_resulttostr(value)+")");
            }
          } else {
            if (type_changeable) {
              // change the ytpe of the variable
              sdff_variable& v=(*variables[level])[name];
              switch(i->second.type) {
                case sdffVTString: delete v.stringValue; break;
                case sdffVTInteger: delete v.intValue; break;
                case sdffVTDouble: delete v.doubleValue; break;
                case sdffVTBoolean: delete v.boolValue; break;
                case sdffVTVoid: break; // this may never appear!!
              }
              //v.type=value.type;
              switch(value.type) {
                case sdffRTString:
                  v.type=sdffVTString;
                  v.stringValue=new std::string;
                  break;
                case sdffRTInteger:
                  v.type=sdffVTInteger;
                  v.intValue=new long long;
                  break;
                case sdffRTDouble:
                  v.type=sdffVTDouble;
                  v.doubleValue=new double;
                  break;
                case sdffRTBoolean:
                  v.type=sdffVTBoolean;
                  v.boolValue=new bool;
                  break;
                case sdffRTVoid:
                  SEQUENCER_ERRORN(SEQUENCER_SDFFERROR_VOIDUSED_NUM, "SDFFVariableManager.set(\""+name+"\", "+sdff_resulttostr(value)+")");
                  break;
              }
            } else {
              SEQUENCER_ERROR(SEQUENCER_SDFFERROR_SETTYPEMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_SETTYPEMISS_NUM), sdff_resulttypetostr(value.type).c_str(), sdff_vartypetostr(i->second.type).c_str(), name.c_str()), "SDFFVariableManager.set(\""+name+"\", "+sdff_resulttostr(value)+")");
            }
          }
          switch(i->second.type) {
            case sdffVTString: *(i->second.stringValue)=value.stringValue; return; break;
            case sdffVTInteger: *(i->second.intValue)=value.intValue; return; break;
            case sdffVTDouble: *(i->second.doubleValue)=value.doubleValue; return; break;
            case sdffVTBoolean: *(i->second.boolValue)=value.boolValue; return; break;
            case sdffVTVoid: return; break; // this may never appear!!
          }
        }
      }
    }
    level--;
  }
  SEQUENCER_ERROR(SEQUENCER_SDFFERROR_VARNOTFOUND_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_VARNOTFOUND_NUM), name.c_str()), "SDFFVariableManager.set(\""+name+"\", "+sdff_resulttostr(value)+")");
};

bool SDFFVariableManager::variable_exists(std::string name){
  int level=variables.size()-1;
  while (level>=0) {
    sdffVariableMap::const_iterator i=variables[level]->find(name);
    if (i!=variables[level]->end())
      return true;
    level--;
  }
  return false;
};

std::string SDFFVariableManager::report(){
  int level=variables.size()-1;
  std::string res="";
  while (level>=0) {
    res+="-- "+inttostr(level)+" --------------------------------------------------------\n";
    if (variables[level]->size()>0) {
      for (sdffVariableMap::const_iterator i=variables[level]->begin(); i!=variables[level]->end(); i++) {
        if (i->second.internal) res+="  ";
        else res+="  external ";
        if (i->second.constant) res+="const ";
        switch(i->second.type) {
          case sdffVTString: res+="string "+i->second.name+" = '"+*(i->second.stringValue)+"'\n"; break;
          case sdffVTInteger: res+="int "+i->second.name+" = "+inttostr(*(i->second.intValue))+"\n"; break;
          case sdffVTDouble: res+="double "+i->second.name+" = "+floattostr(*(i->second.doubleValue))+"\n"; break;
          case sdffVTBoolean: res+="bool "+i->second.name+" = "+booltostr(*(i->second.boolValue))+"\n"; break;
          case sdffVTVoid: res+=i->second.name+" = (void)\n"; break;
        }
      }
    }
    level--;
  }
  res+="-------------------------------------------------------------\n";
  return res;
};

unsigned int SDFFVariableManager::get_count(){
  int level=variables.size()-1;
  unsigned int res=0;
  while (level>=0) {
    res+=variables[level]->size();
    level--;
  }
  return res;
};

unsigned int SDFFVariableManager::get_globalcount(){
  unsigned int res=variables[0]->size();
  return res;
};

sdff_variable SDFFVariableManager::get_vardef(unsigned int name){
  int level=variables.size()-1;
  size_t cnt=0;
  sdff_variable res;
  while (level>=0) {
    if (variables[level]->size()>0) {
      for (sdffVariableMap::const_iterator i=variables[level]->begin(); i!=variables[level]->end(); i++) {
        if (cnt==name) return i->second;
        else cnt++;
      }
    }
    level--;
  }
  SEQUENCER_ERRORN(SEQUENCER_ERROR_RANGE_NUM, "SDFFVariableManager.get_vardef("+inttostr(name)+")");
};

sdff_variable SDFFVariableManager::get_globalvardef(std::string name){
  int level=0;
  sdffVariableMap::const_iterator i=variables[level]->find(name);
  if (i!=variables[level]->end()) {
    return (*variables[level])[name];
  }
  SEQUENCER_ERROR(SEQUENCER_SDFFERROR_VARNOTFOUND_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_VARNOTFOUND_NUM), name.c_str()), "SDFFVariableManager.get_globalvardef(\""+name+"\")");
};



sdff_variable SDFFVariableManager::get_globalvardef(unsigned int name){
  int level=0;
  size_t cnt=0;
  sdff_variable res;
  if (variables[level]->size()>0) {
    for (sdffVariableMap::const_iterator i=variables[level]->begin(); i!=variables[level]->end(); i++) {
      if (cnt==name) return i->second;
      else cnt++;
    }
  }
  SEQUENCER_ERRORN(SEQUENCER_ERROR_RANGE_NUM, "SDFFVariableManager.get_globalvardef("+inttostr(name)+")");
};





/* =======================================================================================================================
 * = SDFFFunctionManager                                                                                                 =
 * =======================================================================================================================*/
SDFFFunctionManager::SDFFFunctionManager(SDFFparser* p, SDFFVariableManager* vm) {
  parser=p;
  vmanager=vm;
  overwrite_functions=true;
};

SDFFFunctionManager::~SDFFFunctionManager() {
  clear();
}

sdff_result SDFFFunctionManager::evaluate(std::string name, sdff_result* parameters, unsigned char parameter_count, SDFFnode** nparameters){
  if (!function_exists(name)) {
    SEQUENCER_ERROR(SEQUENCER_SDFFERROR_FUNCNOTFOUND_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCNOTFOUND_NUM), name.c_str()), "SDFFFunctionManager::evaluate(\""+name+"\")");
  }
  sdff_function_description d=functions[name];
  if (d.type==sdffFTExternal) {
    sdff_result r= d.function(parser, name, parameters, parameter_count);
    return r;
  }
  if (d.type==sdffFTNodeExternal) {
    sdff_result r= d.nfunction(parser, name, nparameters, parameter_count);
    return r;
  }
  if (d.type==sdffFTNode) {

    // check parameter count
    if (parameter_count!=d.parameters.size()) {
      SEQUENCER_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name.c_str(), parameter_count, d.parameters.size()), "SDFFFunctionManager.evaluate(\""+name+"\")");
    }

    // now add temporary variables and check parameter types
    if (parameter_count>0) {
      vmanager->enter_block();
      for (int i=0; i<parameter_count; i++) {
        switch(d.parameters[i].type) {
          case sdffPTDouble:
            if (parameters[i].type==sdffRTDouble) {
              vmanager->add_variable_double(d.parameters[i].name, parameters[i].doubleValue);break;
            }
            if (parameters[i].type==sdffRTInteger) { // implicit cast int->double
              vmanager->add_variable_double(d.parameters[i].name, parameters[i].intValue);
              break;
            }
            SEQUENCER_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str(), sdff_resulttypetostr(parameters[i].type).c_str(), i, sdff_paramtypetostr(d.parameters[i].type).c_str()), "SDFFFunctionManager.evaluate(\""+name+"\")");
            break;
          case sdffPTInteger:
            if (parameters[i].type==sdffRTInteger) {
              vmanager->add_variable_int(d.parameters[i].name, parameters[i].intValue);
              break;
            }
            SEQUENCER_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str(), sdff_resulttypetostr(parameters[i].type).c_str(), i, sdff_paramtypetostr(d.parameters[i].type).c_str()), "SDFFFunctionManager.evaluate(\""+name+"\")");
            break;
          case sdffPTString:
            if (parameters[i].type==sdffRTString) {
              vmanager->add_variable_string(d.parameters[i].name, parameters[i].stringValue);
              break;
            }
            SEQUENCER_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str(), sdff_resulttypetostr(parameters[i].type).c_str(), i, sdff_paramtypetostr(d.parameters[i].type).c_str()), "SDFFFunctionManager.evaluate(\""+name+"\")");
            break;
          case sdffPTBoolean:
            if (parameters[i].type==sdffRTBoolean) {
              vmanager->add_variable_bool(d.parameters[i].name, parameters[i].boolValue);
              break;
            }
            SEQUENCER_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str(), sdff_resulttypetostr(parameters[i].type).c_str(), i, sdff_paramtypetostr(d.parameters[i].type).c_str()), "SDFFFunctionManager.evaluate(\""+name+"\")");
            break;
          case sdffPTUnknown:
            if (parameters[i].type==sdffRTBoolean) {
              vmanager->add_variable_bool(d.parameters[i].name, parameters[i].boolValue);
              break;
            }
            if (parameters[i].type==sdffRTInteger) {
              vmanager->add_variable_int(d.parameters[i].name, parameters[i].intValue);
              break;
            }
            if (parameters[i].type==sdffRTDouble) {
              vmanager->add_variable_double(d.parameters[i].name, parameters[i].doubleValue);
              break;
            }
            if (parameters[i].type==sdffRTString) {
              vmanager->add_variable_string(d.parameters[i].name, parameters[i].stringValue);
              break;
            }
            break;

        }
      }
    }

    // evaluate function definition with the given parameters
    sdff_result r;
    try {
      if (dynamic_cast<SDFFlistNode*>(d.node)) {
        vmanager->enter_block();
        vmanager->add_variable_void("result");
      }
      r=d.node->evaluate();
      if (dynamic_cast<SDFFlistNode*>(d.node)) {
        r=vmanager->get("result");
        vmanager->leave_block();
      }
    } catch (exception E) {
      // delete temporary variables ... even if there was an exception when executing
      if (parameter_count>0) vmanager->leave_block();
      throw E;
    }

    if (parameter_count>0) vmanager->leave_block();
    return r;
  }
}

void SDFFFunctionManager::add_node(std::string name, std::vector<sdff_function_parameter> parameters, SDFFnode* node, long line){
  if (function_exists(name) && (!overwrite_functions)) {
    SEQUENCER_ERROR(SEQUENCER_SDFFERROR_FUNCEXISTS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCEXISTS_NUM), name.c_str()), "SDFFFunctionManager::evaluate(\""+name+"\")");
//    parser->add_error_p(SEQUENCER_SDFFERROR_FUNCEXISTS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCEXISTS_NUM), name.c_str()), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCEXISTS_NUM), name.c_str()), parser->get_current_filename(), line);
  }
  sdff_function_description d;
  d.node=node;
  d.name=name;
  d.type=sdffFTNode;
  d.line=line;
  d.parameters.assign(parameters.begin(), parameters.end());
  functions[name]=d;
}

void SDFFFunctionManager::add_ext(std::string name, sdff_function function, sdff_function_check check){
  if (function_exists(name) && (!overwrite_functions)) {
    SEQUENCER_ERROR(SEQUENCER_SDFFERROR_FUNCEXISTS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCEXISTS_NUM), name.c_str()), "SDFFFunctionManager::evaluate(\""+name+"\")");
//    parser->add_error_p(SEQUENCER_SDFFERROR_FUNCEXISTS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCEXISTS_NUM), name.c_str()), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCEXISTS_NUM), name.c_str()), parser->get_current_filename(), -1);
  }
  sdff_function_description d;
  d.name=name;
  d.type=sdffFTExternal;
  d.function=function;
  d.check=check;
  functions[name]=d;
}

void SDFFFunctionManager::add_next(std::string name, sdff_nodefunction function, sdff_function_check check){
  if (function_exists(name) && (!overwrite_functions)) {
    SEQUENCER_ERROR(SEQUENCER_SDFFERROR_FUNCEXISTS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCEXISTS_NUM), name.c_str()), "SDFFFunctionManager::evaluate(\""+name+"\")");
//    parser->add_error_p(SEQUENCER_SDFFERROR_FUNCEXISTS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCEXISTS_NUM), name.c_str()), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCEXISTS_NUM), name.c_str()), parser->get_current_filename(), -1);
  }
  sdff_function_description d;
  d.name=name;
  d.type=sdffFTNodeExternal;
  d.nfunction=function;
  d.check=check;
  functions[name]=d;
}

bool SDFFFunctionManager::check(std::string name, sdff_paramtype* parameters, unsigned char parameter_count, bool result_used){
  if (!function_exists(name)) {
    parser->add_error_p(SEQUENCER_SDFFERROR_FUNCNOTFOUND_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCNOTFOUND_NUM), name.c_str()), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCNOTFOUND_NUM), name.c_str()), parser->get_current_filename(), parser->get_current_line());
    return false;
  } else {
    sdff_function_description d=functions[name];
    if (d.type==sdffFTNode) {
      // check parameter count
      if (parameter_count!=d.parameters.size()) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name.c_str(), parameter_count, d.parameters.size()), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name.c_str()), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if (parameter_count>0) {
        for (int i=0; i<parameter_count; i++) { // go through all parameters
          if (parameters[i]!=sdffPTUnknown) { // we can not check if given parameter id sdffPTUnknown, as this represents no information
            if (d.parameters[i].type!=sdffPTUnknown) { // we do not have information about the needed data type
              if (d.parameters[i].type!=parameters[i]) { // type missmatch!
                parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str(), sdff_paramtypetostr(parameters[i]).c_str(), i, sdff_paramtypetostr(d.parameters[i].type).c_str()), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str()), parser->get_current_filename(), parser->get_current_line());
                return false;
              }
            }
          }
        }
      }
      return true;
    } else if ((d.type==sdffFTExternal)||(d.type==sdffFTNodeExternal)) {
      // call check function if supplied
      if (d.check!=NULL) {
        return (*d.check)(parser, name, parameters, parameter_count, result_used);
      } else return true;
    }
    return true;
  }
}

std::string SDFFFunctionManager::report() {
  std::string res="";
  std::map<std::string, sdff_function_description>::const_iterator i;
  for (i=functions.begin(); i!=functions.end(); ++i) {
    res+=i->first+": ";
    switch(i->second.type) {
      case sdffFTExternal: res+="external function, sdff_result arguments"; break;
      case sdffFTNodeExternal: res+="external function, SDFFnode* arguments"; break;
      case sdffFTNode:
        res+="defined in l."+inttostr(i->second.line)+" by SDFFnode*:\n";
        res+="  "+i->second.name+"(";
        if (i->second.parameters.size()>0)
          for (size_t j=0; j<i->second.parameters.size(); j++) {
            if (j>0) res+=", ";
            res+=i->second.parameters[j].name;
          }
        res+=") = "+i->second.node->print();
        break;
    }
    res+="\n";
  }
  return res;
};


};
