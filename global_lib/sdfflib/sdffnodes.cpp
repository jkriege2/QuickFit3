/*
  Name: sdffnodes.cpp
  Copyright: (c) 2007
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/

#include "sdffnodes.h"

#include "sdffparser.h"

namespace sequencer {


#define RUN_TIME_ERROR(num, message, where) { throw sequencer::exception(num, format("runtime-error at line %d and time %lf:\n  %s", get_line(), parser->get_current_time(), message.c_str()), where, get_line(), parser->get_current_time()); }
#define RUN_TIME_ERRORN(num, where) { throw sequencer::exception(num, format("runtime-error at line %d and time %lf:\n  %s", get_line(), parser->get_current_time(), get_errormessage(num).c_str()), where, get_line(), parser->get_current_time()); }
#define RUN_TIME_ERRORV(num, where, args...) { throw sequencer::exception(num, format("runtime-error at line %d and time %lf:\n  %s", get_line(), parser->get_current_time(), format(get_errormessage(num), args).c_str()), where, get_line(), parser->get_current_time()); }


/* =======================================================================================================================
 * = SDFFnode                                                                                                            =
 * =======================================================================================================================*/
SDFFnode::SDFFnode(SDFFnode* parent) {
  parser=NULL;
  this->parent=parent;
  if (parent!=NULL) {
    parser=parent->get_parser();
  }
  destroy_children=true;
  line=parser->get_current_line();
}

SDFFnode::SDFFnode(SDFFparser* parser) {
  this->parser=parser;
  parent=NULL;
  destroy_children=true;
  line=parser->get_current_line();
}

SDFFnode::~SDFFnode() {
  if (destroy_children) {
    dispose();
  } else {
    deconstruct();
  }
};

void SDFFnode::set_parent(SDFFnode* parent){
  this->parent=parent;
  if (parent!=NULL) {
    parser=parent->get_parser();
  }
}




/* =======================================================================================================================
 * = SDFFunaryNode                                                                                                            =
 * =======================================================================================================================*/
SDFFunaryNode::SDFFunaryNode(SDFFnode* child, SDFFnode* parent):
  SDFFnode(parent)
{
  this->child=child;
  if (child!=NULL) {
    child->set_parent(this);
  }
}

SDFFunaryNode::SDFFunaryNode(SDFFparser* parser, SDFFnode* child):
  SDFFnode(parser)
{
  this->child=child;
  child->set_parent(this);
}

void SDFFunaryNode::set_parent(SDFFnode* parent){
  this->parent=parent;
  this->parser=NULL;
  if (parent!=NULL) {
    parser=parent->get_parser();
  }
  if (child!=NULL) {
    child->set_parent(this);
    child->set_parser(this->parser);
  }
}

void SDFFunaryNode::set_child(SDFFnode* child){
  this->child=child;
  if (child!=NULL) {
    child->set_parent(this);
    child->set_parser(this->parser);
  }
}

void SDFFunaryNode::set_parser(SDFFparser* parser){
  this->parser=parser;
  if (child!=NULL) {
    child->set_parser(parser);
  }
}

void SDFFunaryNode::dispose(){
  delete child;
  child=NULL;
}


void SDFFunaryNode::deconstruct(){
  child->deconstruct();
  child=NULL;
}





/* =======================================================================================================================
 * = SDFFbinaryNode                                                                                                            =
 * =======================================================================================================================*/
SDFFbinaryNode::SDFFbinaryNode(SDFFnode* left, SDFFnode* right, SDFFnode* parent):
  SDFFnode(parent)
{
  this->left=left;
  this->right=right;
  if (left!=NULL) {
    left->set_parent(this);
    left->set_parser(this->parser);
  }
  if (right!=NULL) {
    right->set_parent(this);
    right->set_parser(this->parser);
  }
}

SDFFbinaryNode::SDFFbinaryNode(SDFFparser* parser, SDFFnode* left, SDFFnode* right):
  SDFFnode(parser)
{
  this->left=left;
  this->right=right;
  if (left!=NULL) {
    left->set_parent(this);
    left->set_parser(parser);
  }
  if (right!=NULL) {
    right->set_parent(this);
    right->set_parser(parser);
  }
}

void SDFFbinaryNode::set_parent(SDFFnode* parent){
  this->parent=parent;
  this->parser=NULL;
  if (parent!=NULL) {
    parser=parent->get_parser();
  }
  if (left!=NULL) {
    left->set_parent(this);
    left->set_parser(parser);
  }
  if (right!=NULL) {
    right->set_parent(this);
    right->set_parser(parser);
  }
}

void SDFFbinaryNode::set_left(SDFFnode* child){
  this->left=child;
  if (child!=NULL) {
    child->set_parent(this);
    child->set_parser(parser);
  }
}

void SDFFbinaryNode::set_right(SDFFnode* child){
  this->right=child;
  if (child!=NULL) {
    child->set_parent(this);
    child->set_parser(parser);
  }
}

void SDFFbinaryNode::set_parser(SDFFparser* parser){
  this->parser=parser;
  if (left!=NULL) {
    left->set_parser(parser);
  }
  if (right!=NULL) {
    right->set_parser(parser);
  }
}

void SDFFbinaryNode::dispose(){
  delete left;
  delete right;
  left=NULL;
  right=NULL;
}


void SDFFbinaryNode::deconstruct(){
  left->deconstruct();
  left=NULL;
  right->deconstruct();
  right=NULL;
}







/* =======================================================================================================================
 * = SDFFnaryNode                                                                                                            =
 * =======================================================================================================================*/

void SDFFnaryNode::set_parent(SDFFnode* parent){
  this->parent=parent;
  if (parent!=NULL) {
    parser=parent->get_parser();
  }
  if (children.size()>0) {
    for (size_t i=0; i<children.size(); i++) {
      SDFFnode* c=children[i];
      if (c!=NULL) c->set_parent(this);
    }
  }
}

void SDFFnaryNode::add_child(SDFFnode* child){
  if (child!=NULL) {
    child->set_parent(this);
    child->set_parser(this->parser);
  }
  children.push_back(child);
}

void SDFFnaryNode::add_children(SDFFnaryNode* children){
  if (children->get_child_count()>0)
    for (size_t i=0; i<children->get_child_count(); i++) {
      SDFFnode* child=children->get_child(i);
      if (child!=NULL) {
        child->set_parent(this);
        child->set_parser(this->parser);
      }
      this->children.push_back(child);
    }
}


void SDFFnaryNode::set_parser(SDFFparser* parser){
  this->parser=parser;
  if (children.size()>0) {
    for (size_t i=0; i<children.size(); i++) {
      SDFFnode* c=children[i];
      if (c!=NULL) c->set_parser(parser);
    }
  }
}

void SDFFnaryNode::dispose(){
  if (children.size()>0) {
    for (size_t i=0; i<children.size(); i++) {
      SDFFnode* c=children[i];
      if (c!=NULL) {
        c->dispose();
        delete c;
        children[i]=NULL;
      }
    }
  }
  children.clear();
}

void SDFFnaryNode::deconstruct(){
  if (children.size()>0) {
    for (size_t i=0; i<children.size(); i++) {
      SDFFnode* c=children[i];
      if (c!=NULL) {
        c->deconstruct();
        children[i]=NULL;
      }
    }
  }
  children.clear();
}

SDFFnode* SDFFnaryNode::replace_child(size_t n, SDFFnode* child, bool destroy_old) {
  SDFFnode* old=NULL;
  if (n<children.size()) {
    old=children[n];
    children[n]=child;
  }
  if (destroy_old && (old!=NULL)){
    delete old;
    old=NULL;
  }
  return old;
}

SDFFnode* SDFFnaryNode::delete_child(size_t n, bool destroy_old) {
  SDFFnode* old=NULL;
  if (n<children.size()) {
    old=children[n];
    children.erase(children.begin()+n);
  }
  if (destroy_old && (old!=NULL)){
    delete old;
    old=NULL;
  }
  return old;
}

void SDFFnaryNode::clear_children(bool destroy_old) {
  if (destroy_old) dispose(); else children.clear();
}



/* =======================================================================================================================
 * = SDFFlistNode                                                                                                            =
 * =======================================================================================================================*/
sdff_result SDFFlistNode::evaluate() {
  sdff_result r;
  r.type=sdffRTVoid;
  SDFFVariableManager* vm=parser->get_vmanager();
  vm->enter_block();
  if (children.size()>0) {
    for (size_t i=0; i<children.size(); i++) {
      SDFFnode* n=children[i];
      if (n!=NULL) r=n->evaluate();
    }
  }
  vm->leave_block();
  return r;
}

std::string SDFFlistNode::print(int indent) {
  std::string in(indent, ' ');
  std::string s;
  if (children.size()>0) {
    for (size_t i=0; i<children.size(); i++) {
      s+=children[i]->print()+";\n"+in;
    }
  }
  return s;
};





/* =======================================================================================================================
 * = SDFFvariableNode                                                                                                            =
 * =======================================================================================================================*/
sdff_result SDFFvariableNode::evaluate() {
  SDFFparser* p=get_parser();
  SDFFVariableManager* v=p->get_vmanager();
  if (p->is_valid_channel_name(varname)) {
      sdff_result r;
      r.type=sdffRTString;
      r.stringValue=p->resolve_alias(varname);
      return r;
  }
  return v->get(varname);
}

std::string SDFFvariableNode::print(int indent) {
  return varname;
};





/* =======================================================================================================================
 * = SDFFchannelnamesNode                                                                                                            =
 * =======================================================================================================================*/
sdff_result SDFFchannelnamesNode::evaluate() {
  SDFFparser* p=get_parser();
  if (items.size()>0) for (size_t i=0; i<items.size(); i++) {
    p->add_alias(items[i].channel, items[i].alias, items[i].info);
  }
  sdff_result r;
  r.type=sdffRTVoid;
  return r;
}

std::string SDFFchannelnamesNode::print(int indent) {
  std::string in(indent, ' ');
  std::string s="channel_names {\n"+in;
  if (items.size()>0) {
    for (size_t i=0; i<items.size(); i++) {
      s+="  "+items[i].channel+", "+items[i].alias;
      if (items[i].info.size()>0) s+=", '"+items[i].info+"'";
      s+=";\n"+in;
    }
  }
  return s+"\n"+in+"}";
};





/* =======================================================================================================================
 * = SDFFfunctionNode                                                                                                            =
 * =======================================================================================================================*/
sdff_result SDFFfunctionNode::evaluate() {
  SDFFFunctionManager* fm=parser->get_fmanager();
  if (!fm->function_exists(functionname)) {
    RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCNOTFOUND_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCNOTFOUND_NUM), functionname.c_str()), "SDFFfunctionNode::evaluate()");
  }
  long l=parser->get_current_line();
  parser->set_current_line(line);

  sdff_result* r=NULL;
  SDFFnode** rn=NULL;
  unsigned int n=get_child_count();
  bool eval=fm->get_function_type(functionname)!=sdffFTNodeExternal;
  if (n>0) {
    r=new sdff_result[n];
    rn=(SDFFnode**)malloc(n*sizeof(SDFFnode*));
    for (size_t i=0; i<n; i++) {
      r[i].type=sdffRTVoid;
      SDFFnode* no=children[i];
      rn[i]=no;
      if ((eval)&&(no!=NULL)) {
        r[i]=no->evaluate();
      }
    }
  }
  sdff_result res=fm->evaluate(functionname, r, n, rn);
  if (n>0) {
    delete[] r;
    free(rn);
  }
  parser->set_current_line(l);
  return res;
}


std::string SDFFfunctionNode::print(int indent) {
  std::string s=functionname+"(";
  if (children.size()>0) {
    for (size_t i=0; i<children.size(); i++) {
      if (i>0) s+=", ";
      s+=children[i]->print();
    }
  }
  return s+")";
};




/* =======================================================================================================================
 * = SDFFnegateNode                                                                                                            =
 * =======================================================================================================================*/
sdff_result SDFFnegateNode::evaluate() {
  sdff_result res, c;
  c=child->evaluate();
  res.type=c.type;
  switch (c.type) {
    case sdffRTString:
      if (negationtype==sdffNTBitwise) {
        char cc;
        res.stringValue="";
        if (c.stringValue.size()>0) for (size_t i=0; i<c.stringValue.size(); i++) {
          cc=c.stringValue[i];
          res.stringValue=res.stringValue+(char)(~cc);
        }
      } else RUN_TIME_ERRORN(SEQUENCER_SDFFERROR_NEGSTRING_NUM, "SDFFnegateNode.evaluate()");
      break;
    case sdffRTVoid:
      RUN_TIME_ERRORN(SEQUENCER_SDFFERROR_NEGVOID_NUM, "SDFFnegateNode.evaluate()");
      break;
    case sdffRTDouble:
      if (negationtype==sdffNTMinus) {
        res.doubleValue=-c.doubleValue;
      } else RUN_TIME_ERRORN(SEQUENCER_SDFFERROR_NEGDOUBLE_NUM, "SDFFnegateNode.evaluate()");
      break;
    case sdffRTInteger:
      if (negationtype==sdffNTMinus) {
        res.intValue=-c.intValue;
      } else if (negationtype==sdffNTBitwise) {
        res.intValue=~c.intValue;
      } else RUN_TIME_ERRORN(SEQUENCER_SDFFERROR_NEGINT_NUM, "SDFFnegateNode.evaluate()");
      break;
    case sdffRTBoolean:
      if (negationtype==sdffNTLogic) {
        res.boolValue=!c.boolValue;
      } else if (negationtype==sdffNTBitwise) {
        res.boolValue=!c.boolValue;
      } else RUN_TIME_ERRORN(SEQUENCER_SDFFERROR_NEGBOOL_NUM, "SDFFnegateNode.evaluate()");
      break;
  }
  return res;
}


std::string SDFFnegateNode::print(int indent) {
  std::string s;
  if (negationtype==sdffNTMinus) s+="-";
  if (negationtype==sdffNTLogic) s+="not ";
  if (negationtype==sdffNTBitwise) s+="~";
  if (dynamic_cast<SDFFconstantNode*>(child)
   || dynamic_cast<SDFFvariableNode*>(child)
   || dynamic_cast<SDFFfunctionNode*>(child)) {
    return s+child->print();
  }
  return s+"("+child->print()+")";
};





/* =======================================================================================================================
 * = SDFFarithmeticNode                                                                                                            =
 * =======================================================================================================================*/
sdff_result SDFFarithmeticNode::evaluate() {
  sdff_result res, l,r;
  l=left->evaluate();
  r=right->evaluate();
  res.type=l.type; // left operand determines result type ... but change is possible,
                   // see below (e.g. when adding)
  switch (type) {
    case sdffAOAdd:
      switch(l.type) { // we first look at the type of the left arghument ...
        case sdffRTString:
          if (r.type==l.type) {
            res.stringValue=l.stringValue+r.stringValue;
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFarithmeticNode.evaluate()", "concatenate", "string", sdff_resulttypetostr(r.type).c_str());
          break;
        case sdffRTDouble:
          if (r.type==sdffRTDouble) {
            res.doubleValue=l.doubleValue+r.doubleValue;
          } else if (r.type==sdffRTInteger) {
            res.doubleValue=l.doubleValue+(double)r.intValue;
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFarithmeticNode.evaluate()", "add", "double", sdff_resulttypetostr(r.type).c_str());
          break;
        case sdffRTInteger:
          if (r.type==sdffRTInteger) {
            res.intValue=l.intValue+r.intValue;
          } else if (r.type==sdffRTDouble) { // here we have to convert the result to double
            res.doubleValue=(double)l.intValue+r.doubleValue;
            res.type=sdffRTDouble;
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFarithmeticNode.evaluate()", "add", "integer", sdff_resulttypetostr(r.type).c_str());
          break;
        default:
          RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFarithmeticNode.evaluate()", "add", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
      }
      break;
    case sdffAOSubtract:
      switch(l.type) { // we first look at the type of the left arghument ...
        case sdffRTDouble:
          if (r.type==sdffRTDouble) {
            res.doubleValue=l.doubleValue-r.doubleValue;
          } else if (r.type==sdffRTInteger) {
            res.doubleValue=l.doubleValue-(double)r.intValue;
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFarithmeticNode.evaluate()", "subtract", "double", sdff_resulttypetostr(r.type).c_str());
          break;
        case sdffRTInteger:
          if (r.type==sdffRTInteger) {
            res.intValue=l.intValue-r.intValue;
          } else if (r.type==sdffRTDouble) { // here we have to convert the result to double
            res.doubleValue=(double)l.intValue-r.doubleValue;
            res.type=sdffRTDouble;
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFarithmeticNode.evaluate()", "subtract", "integer", sdff_resulttypetostr(r.type).c_str());
          break;
        default:
          RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFarithmeticNode.evaluate()", "subtract", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
      }
      break;
    case sdffAOMultiply:
      switch(l.type) { // we first look at the type of the left arghument ...
        case sdffRTDouble:
          if (r.type==sdffRTDouble) {
            res.doubleValue=l.doubleValue*r.doubleValue;
          } else if (r.type==sdffRTInteger) {
            res.doubleValue=l.doubleValue*(double)r.intValue;
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFarithmeticNode.evaluate()", "multiply", "double", sdff_resulttypetostr(r.type).c_str());
          break;
        case sdffRTInteger:
          if (r.type==sdffRTInteger) {
            res.intValue=l.intValue*r.intValue;
          } else if (r.type==sdffRTDouble) { // here we have to convert the result to double
            res.doubleValue=(double)l.intValue*r.doubleValue;
            res.type=sdffRTDouble;
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFarithmeticNode.evaluate()", "multiply", "integer", sdff_resulttypetostr(r.type).c_str());
          break;
        default:
          RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFarithmeticNode.evaluate()", "multiply", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
      }
      break;
    case sdffAODivide:
      switch(l.type) { // we first look at the type of the left arghument ...
        case sdffRTDouble:
          if (r.type==sdffRTDouble) {
            res.doubleValue=l.doubleValue/r.doubleValue;
          } else if (r.type==sdffRTInteger) {
            res.doubleValue=l.doubleValue/(double)r.intValue;
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFarithmeticNode.evaluate()", "divide", "double", sdff_resulttypetostr(r.type).c_str());
          break;
        case sdffRTInteger:
          if (r.type==sdffRTInteger) {
            res.intValue=l.intValue/r.intValue;
          } else if (r.type==sdffRTDouble) { // here we have to convert the result to double
            res.doubleValue=(double)l.intValue/r.doubleValue;
            res.type=sdffRTDouble;
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFarithmeticNode.evaluate()", "divide", "integer", sdff_resulttypetostr(r.type).c_str());
          break;
        default:
          RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFarithmeticNode.evaluate()", "divide", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
      }
      break;
    case sdffAOPower:
      switch(l.type) { // we first look at the type of the left arghument ...
        case sdffRTDouble:
          res.type=sdffRTDouble;
          if (r.type==sdffRTDouble) {
            res.doubleValue=pow(l.doubleValue, r.doubleValue);
          } else if (r.type==sdffRTInteger) {
            if (r.intValue==0) res.doubleValue=1;
            else if (r.intValue==1) res.doubleValue=l.doubleValue; // speed up for squares
            else if (r.intValue==-1) res.doubleValue=1/l.doubleValue; // speed up for squares
            else if (r.intValue==2) res.doubleValue=l.doubleValue*l.doubleValue; // speed up for squares
            else if (r.intValue==-2) res.doubleValue=1/l.doubleValue/l.doubleValue; // speed up for squares
            else if (r.intValue==3) res.doubleValue=l.doubleValue*l.doubleValue*l.doubleValue; // ... and cubes
            else if (r.intValue==-3) res.doubleValue=1/l.doubleValue/l.doubleValue/l.doubleValue; // ... and cubes
            else res.doubleValue=pow(l.doubleValue, (double)r.intValue);
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFarithmeticNode.evaluate()", "exponentiate", "double", sdff_resulttypetostr(r.type).c_str());
          break;
        case sdffRTInteger:
          if (r.type==sdffRTInteger) {
            if (r.intValue==0) res.intValue=1;
            else if (r.intValue==1) res.intValue=l.intValue; // speed up for squares
            else if (r.intValue==2) res.intValue=l.intValue*l.intValue; // speed up for squares
            else if (r.intValue==3) res.intValue=l.intValue*l.intValue*l.intValue; // ... and cubes
            else if (r.intValue<0) { // ... x^-e is always double
              res.doubleValue=pow((double)l.intValue, (double)r.intValue);
              res.type=sdffRTDouble;
            } else {
              res.intValue=l.intValue;
              for (long long i=2; i<=r.intValue; i++) {
                res.intValue*=l.intValue;
              }
            }
          } else if (r.type==sdffRTDouble) { // here we have to convert the result to double
            res.doubleValue=pow((double)l.intValue, r.doubleValue);
            res.type=sdffRTDouble;
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFarithmeticNode.evaluate()", "exponentiate", "integer", sdff_resulttypetostr(r.type).c_str());
          break;
        default:
          RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFarithmeticNode.evaluate()", "exponentiate", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
      }
      break;
    case sdffAOModulo:
      switch(l.type) { // we first look at the type of the left arghument ...
        case sdffRTInteger:
          if (r.type==sdffRTInteger) {
            res.intValue=l.intValue%r.intValue;
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFarithmeticNode.evaluate()", "modulo divide", "integer", sdff_resulttypetostr(r.type).c_str());
          break;
        default:
          RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFarithmeticNode.evaluate()", "modulo divide", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
      }
      break;
  }
  return res;
}


std::string SDFFarithmeticNode::print(int indent) {
  std::string s="";
  /* here we first check the type of the arguments if they are simple, we can simply
   * print them. If they are not so simple we wil have to put them in parantheses.
   * To determine the type of the childre we use a dynamic_cast operator
   */
  SDFFnode* child=left;
  if (dynamic_cast<SDFFconstantNode*>(child)
   || dynamic_cast<SDFFvariableNode*>(child)
   || dynamic_cast<SDFFfunctionNode*>(child)) {
    s=left->print();
  } else if (dynamic_cast<SDFFarithmeticNode*>(child))  {
    sdff_arithmetic_ops oo=(dynamic_cast<SDFFarithmeticNode*>(child))->get_type();
    if (type==oo && oo!=sdffAOPower) s=left->print(); else s="("+left->print()+")";
  } else {
    s="("+left->print()+")";
  }
  switch (type) {
    case sdffAOAdd: s+=" + "; break;
    case sdffAOSubtract: s+=" - "; break;
    case sdffAOMultiply: s+=" * "; break;
    case sdffAODivide: s+=" / "; break;
    case sdffAOPower: s+=" ^ "; break;
    case sdffAOModulo:s+=" mod "; break;
  }
  child=right;
  if (dynamic_cast<SDFFconstantNode*>(child)
   || dynamic_cast<SDFFvariableNode*>(child)
   || dynamic_cast<SDFFfunctionNode*>(child)) {
    s+=right->print();
  } else if (dynamic_cast<SDFFarithmeticNode*>(child))  {
    sdff_arithmetic_ops oo=(dynamic_cast<SDFFarithmeticNode*>(child))->get_type();
    if (type==oo && oo!=sdffAOPower) s+=right->print(); else s+="("+right->print()+")";
  } else {
    s+="("+right->print()+")";
  }
  return s;
};





/* =======================================================================================================================
 * = SDFFcompareNode                                                                                                            =
 * =======================================================================================================================*/
sdff_result SDFFcompareNode::evaluate() {
  sdff_result res, l,r;
  l=left->evaluate();
  r=right->evaluate();
  res.type=sdffRTBoolean;
  switch (type) {
    case sdffCOEqual:
      switch(l.type) { // we first look at the type of the left arghument ...
        case sdffRTString:
          if (r.type==l.type) {
            res.boolValue=(l.stringValue==r.stringValue);
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFcompareNode.evaluate()", "compare (==)", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
        case sdffRTBoolean:
          if (r.type==l.type) {
            res.boolValue=(l.boolValue==r.boolValue);
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFcompareNode.evaluate()", "compare (==)", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
        case sdffRTDouble:
          if (r.type==sdffRTDouble) {
            res.boolValue=(l.doubleValue==r.doubleValue);
          } else if (r.type==sdffRTInteger) {
            res.boolValue=(l.doubleValue==(double)r.intValue);
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFcompareNode.evaluate()", "compare (==)", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
        case sdffRTInteger:
          if (r.type==sdffRTInteger) {
            res.boolValue=(l.intValue==r.intValue);
          } else if (r.type==sdffRTDouble) {
            res.boolValue=((double)l.intValue==r.doubleValue);
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFcompareNode.evaluate()", "compare (==)", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
        default:
          RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFcompareNode.evaluate()", "compare (==)", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
      }
      break;
    case sdffCOUnequal:
      switch(l.type) { // we first look at the type of the left arghument ...
        case sdffRTString:
          if (r.type==l.type) {
            res.boolValue=(l.stringValue!=r.stringValue);
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFcompareNode.evaluate()", "compare (!=)", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
        case sdffRTBoolean:
          if (r.type==l.type) {
            res.boolValue=(l.boolValue!=r.boolValue);
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFcompareNode.evaluate()", "compare (!=)", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
        case sdffRTDouble:
          if (r.type==sdffRTDouble) {
            res.boolValue=(l.doubleValue!=r.doubleValue);
          } else if (r.type==sdffRTInteger) {
            res.boolValue=(l.doubleValue!=(double)r.intValue);
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFcompareNode.evaluate()", "compare (!=)", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
        case sdffRTInteger:
          if (r.type==sdffRTInteger) {
            res.boolValue=(l.intValue!=r.intValue);
          } else if (r.type==sdffRTDouble) {
            res.boolValue=((double)l.intValue!=r.doubleValue);
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFcompareNode.evaluate()", "compare (!=)", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
        default:
          RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFcompareNode.evaluate()", "compare (!=)", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
      }
      break;
    case sdffCOLess:
      switch(l.type) { // we first look at the type of the left arghument ...
        case sdffRTString:
          if (r.type==l.type) {
            res.boolValue=(l.stringValue<r.stringValue);
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFcompareNode.evaluate()", "compare (<)", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
        case sdffRTDouble:
          if (r.type==sdffRTDouble) {
            res.boolValue=(l.doubleValue<r.doubleValue);
          } else if (r.type==sdffRTInteger) {
            res.boolValue=(l.doubleValue<(double)r.intValue);
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFcompareNode.evaluate()", "compare (<)", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
        case sdffRTInteger:
          if (r.type==sdffRTInteger) {
            res.boolValue=(l.intValue<r.intValue);
          } else if (r.type==sdffRTDouble) {
            res.boolValue=((double)l.intValue<r.doubleValue);
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFcompareNode.evaluate()", "compare (<)", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
        default:
          RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFcompareNode.evaluate()", "compare (<)", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
      }
      break;
    case sdffCOBigger:
      switch(l.type) { // we first look at the type of the left arghument ...
        case sdffRTString:
          if (r.type==l.type) {
            res.boolValue=(l.stringValue>r.stringValue);
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFcompareNode.evaluate()", "compare (>)", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
        case sdffRTDouble:
          if (r.type==sdffRTDouble) {
            res.boolValue=(l.doubleValue>r.doubleValue);
          } else if (r.type==sdffRTInteger) {
            res.boolValue=(l.doubleValue>(double)r.intValue);
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFcompareNode.evaluate()", "compare (>)", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
        case sdffRTInteger:
          if (r.type==sdffRTInteger) {
            res.boolValue=(l.intValue>r.intValue);
          } else if (r.type==sdffRTDouble) {
            res.boolValue=((double)l.intValue>r.doubleValue);
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFcompareNode.evaluate()", "compare (>)", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
        default:
          RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFcompareNode.evaluate()", "compare (>)", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
      }
      break;
    case sdffCOLessEqual:
      switch(l.type) { // we first look at the type of the left arghument ...
        case sdffRTString:
          if (r.type==l.type) {
            res.boolValue=(l.stringValue<=r.stringValue);
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFcompareNode.evaluate()", "compare (<=)", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
        case sdffRTDouble:
          if (r.type==sdffRTDouble) {
            res.boolValue=(l.doubleValue<=r.doubleValue);
          } else if (r.type==sdffRTInteger) {
            res.boolValue=(l.doubleValue<=(double)r.intValue);
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFcompareNode.evaluate()", "compare (<=)", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
        case sdffRTInteger:
          if (r.type==sdffRTInteger) {
            res.boolValue=(l.intValue<=r.intValue);
          } else if (r.type==sdffRTDouble) {
            res.boolValue=((double)l.intValue<=r.doubleValue);
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFcompareNode.evaluate()", "compare (<=)", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
        default:
          RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFcompareNode.evaluate()", "compare (<=)", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
      }
      break;
    case sdffCOBiggerEqual:
      switch(l.type) { // we first look at the type of the left arghument ...
        case sdffRTString:
          if (r.type==l.type) {
            res.boolValue=(l.stringValue>=r.stringValue);
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFcompareNode.evaluate()", "compare (>=)", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
        case sdffRTDouble:
          if (r.type==sdffRTDouble) {
            res.boolValue=(l.doubleValue>=r.doubleValue);
          } else if (r.type==sdffRTInteger) {
            res.boolValue=(l.doubleValue>=(double)r.intValue);
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFcompareNode.evaluate()", "compare (>=)", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
        case sdffRTInteger:
          if (r.type==sdffRTInteger) {
            res.boolValue=(l.intValue>=r.intValue);
          } else if (r.type==sdffRTDouble) {
            res.boolValue=((double)l.intValue>=r.doubleValue);
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFcompareNode.evaluate()", "compare (>=)", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
        default:
          RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFcompareNode.evaluate()", "compare (>=)", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
      }
      break;
  }
  return res;
}

std::string SDFFcompareNode::print(int indent) {
  std::string s;
  /* here we first check the type of the arguments if they are simple, we can simply
   * print them. If they are not so simple we wil have to put them in parantheses.
   * To determine the type of the childre we use a dynamic_cast operator
   */
  SDFFnode* child=left;
  if (dynamic_cast<SDFFconstantNode*>(child)
   || dynamic_cast<SDFFvariableNode*>(child)
   || dynamic_cast<SDFFfunctionNode*>(child)) {
    s=left->print();
  } else if (dynamic_cast<SDFFcompareNode*>(child))  {
    sdff_compare_ops oo=(dynamic_cast<SDFFcompareNode*>(child))->get_type();
    if (type==oo) s=left->print(); else s="("+left->print()+")";
  } else {
    s="("+left->print()+")";
  }
  switch (type) {
    case sdffCOEqual: s+=" == "; break;
    case sdffCOUnequal: s+=" != "; break;
    case sdffCOLessEqual: s+=" <= "; break;
    case sdffCOBiggerEqual: s+=" >= "; break;
    case sdffCOBigger: s+=" > "; break;
    case sdffCOLess: s+=" < "; break;
  }
 child=right;
  if (dynamic_cast<SDFFconstantNode*>(child)
   || dynamic_cast<SDFFvariableNode*>(child)
   || dynamic_cast<SDFFfunctionNode*>(child)) {
    s+=right->print();
  } else if (dynamic_cast<SDFFcompareNode*>(child))  {
    sdff_compare_ops oo=(dynamic_cast<SDFFcompareNode*>(child))->get_type();
    if (type==oo) s+=right->print(); else s+="("+right->print()+")";
  } else {
    s+="("+right->print()+")";
  }
  return s;
};










/* =======================================================================================================================
 * = SDFFlogicNode                                                                                                            =
 * =======================================================================================================================*/
sdff_result SDFFlogicNode::evaluate() {
  sdff_result res, l,r;
  l=left->evaluate();
  r=right->evaluate();
  res.type=l.type; // left operand determines result type ... but change is possible,
                   // see below (e.g. when adding)

  switch (type) {
    case sdffLOAND:
      switch(l.type) { // we first look at the type of the left arghument ...
        case sdffRTString:
          if (r.type==l.type) {
            if (l.stringValue.size()==r.stringValue.size()) {
              res.stringValue="";
              for (size_t i=0; i<l.stringValue.size(); i++) {
                res.stringValue+=l.stringValue[i] & r.stringValue[i];
              }
            } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_STRUNEQUALLENGTH_NUM, "SDFFlogicNode.evaluate()", "AND");
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFlogicNode.evaluate()", "AND", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
        case sdffRTInteger:
          if (r.type==sdffRTInteger) {
            res.intValue=l.intValue & r.intValue;
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFlogicNode.evaluate()", "AND", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
        case sdffRTBoolean:
          if (r.type==sdffRTBoolean) {
            res.boolValue=l.boolValue && r.boolValue;
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFlogicNode.evaluate()", "AND", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
        default:
          RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFlogicNode.evaluate()", "AND", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
      }
      break;
    case sdffLOOR:
      switch(l.type) { // we first look at the type of the left arghument ...
        case sdffRTString:
          if (r.type==l.type) {
            if (l.stringValue.size()==r.stringValue.size()) {
              res.stringValue="";
              for (size_t i=0; i<l.stringValue.size(); i++) {
                res.stringValue+=l.stringValue[i] | r.stringValue[i];
              }
            } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_STRUNEQUALLENGTH_NUM, "SDFFlogicNode.evaluate()", "OR");
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFlogicNode.evaluate()", "OR", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
        case sdffRTInteger:
          if (r.type==sdffRTInteger) {
            res.intValue=l.intValue | r.intValue;
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFlogicNode.evaluate()", "OR", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
        case sdffRTBoolean:
          if (r.type==sdffRTBoolean) {
            res.boolValue=l.boolValue || r.boolValue;
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFlogicNode.evaluate()", "OR", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
        default:
          RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFlogicNode.evaluate()", "OR", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
      }
      break;
    case sdffLOXOR:
      switch(l.type) { // we first look at the type of the left arghument ...
        case sdffRTString:
          if (r.type==l.type) {
            if (l.stringValue.size()==r.stringValue.size()) {
              res.stringValue="";
              for (size_t i=0; i<l.stringValue.size(); i++) {
                res.stringValue+=l.stringValue[i] ^ r.stringValue[i];
              }
            } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_STRUNEQUALLENGTH_NUM, "SDFFlogicNode.evaluate()", "XOR");
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFlogicNode.evaluate()", "XOR", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
        case sdffRTInteger:
          if (r.type==sdffRTInteger) {
            res.intValue=l.intValue ^ r.intValue;
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFlogicNode.evaluate()", "XOR", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
        case sdffRTBoolean:
          if (r.type==sdffRTBoolean) {
            res.boolValue=(l.boolValue && !r.boolValue)||(!l.boolValue && r.boolValue); // there is no logical XOR operator in C/C++
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFlogicNode.evaluate()", "XOR", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
        default:
          RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFlogicNode.evaluate()", "XOR", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
      }
      break;
    case sdffLONAND:
      switch(l.type) { // we first look at the type of the left arghument ...
        case sdffRTString:
          if (r.type==l.type) {
            if (l.stringValue.size()==r.stringValue.size()) {
              res.stringValue="";
              for (size_t i=0; i<l.stringValue.size(); i++) {
                res.stringValue+=~(l.stringValue[i] & r.stringValue[i]);
              }
            } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_STRUNEQUALLENGTH_NUM, "SDFFlogicNode.evaluate()", "NAND");
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFlogicNode.evaluate()", "NAND", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
        case sdffRTInteger:
          if (r.type==sdffRTInteger) {
            res.intValue=~(l.intValue & r.intValue);
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFlogicNode.evaluate()", "NAND", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
        case sdffRTBoolean:
          if (r.type==sdffRTBoolean) {
            res.boolValue=!(l.boolValue && r.boolValue);
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFlogicNode.evaluate()", "NAND", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
        default:
          RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFlogicNode.evaluate()", "NAND", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
      }
      break;
    case sdffLONOR:
      switch(l.type) { // we first look at the type of the left arghument ...
        case sdffRTString:
          if (r.type==l.type) {
            if (l.stringValue.size()==r.stringValue.size()) {
              res.stringValue="";
              for (size_t i=0; i<l.stringValue.size(); i++) {
                res.stringValue+=~(l.stringValue[i] | r.stringValue[i]);
              }
            } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_STRUNEQUALLENGTH_NUM, "SDFFlogicNode.evaluate()", "NOR");
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFlogicNode.evaluate()", "NOR", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
        case sdffRTInteger:
          if (r.type==sdffRTInteger) {
            res.intValue=~(l.intValue | r.intValue);
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFlogicNode.evaluate()", "NOR", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
        case sdffRTBoolean:
          if (r.type==sdffRTBoolean) {
            res.boolValue=!(l.boolValue || r.boolValue);
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFlogicNode.evaluate()", "NOR", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
        default:
          RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFlogicNode.evaluate()", "NOR", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
      }
      break;
    case sdffLOSHL:
      switch(l.type) { // we first look at the type of the left arghument ...
        case sdffRTInteger:
          if (r.type==sdffRTInteger) {
            res.intValue=l.intValue << r.intValue;
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFlogicNode.evaluate()", "SHL", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
        default:
          RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFlogicNode.evaluate()", "SHL", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
      }
      break;
    case sdffLOSHR:
      switch(l.type) { // we first look at the type of the left arghument ...
        case sdffRTInteger:
          if (r.type==sdffRTInteger) {
            res.intValue=l.intValue >> r.intValue;
          } else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFlogicNode.evaluate()", "SHR", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
        default:
          RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM, "SDFFlogicNode.evaluate()", "SHR", sdff_resulttypetostr(l.type).c_str(), sdff_resulttypetostr(r.type).c_str());
          break;
      }
      break;
  }
  return res;
}

std::string SDFFlogicNode::print(int indent) {
  std::string s;
  /* here we first check the type of the arguments if they are simple, we can simply
   * print them. If they are not so simple we wil have to put them in parantheses.
   * To determine the type of the childre we use a dynamic_cast operator
   */
  SDFFnode* child=left;
  if (dynamic_cast<SDFFconstantNode*>(child)
   || dynamic_cast<SDFFvariableNode*>(child)
   || dynamic_cast<SDFFfunctionNode*>(child)) {
    s=left->print();
  } else if (dynamic_cast<SDFFlogicNode*>(child))  {
    sdff_logic_ops oo=(dynamic_cast<SDFFlogicNode*>(child))->get_type();
    if (type==oo) s=left->print(); else s="("+left->print()+")";
  } else {
    s="("+left->print()+")";
  }
  switch (type) {
    case sdffLOSHR: s+=" >> "; break;
    case sdffLOSHL: s+=" << "; break;
    case sdffLOAND: s+=" and "; break;
    case sdffLOOR: s+=" or "; break;
    case sdffLOXOR: s+=" xor "; break;
    case sdffLONOR: s+=" nor "; break;
    case sdffLONAND: s+=" nand "; break;
  }
  child=right;
  if (dynamic_cast<SDFFconstantNode*>(child)
   || dynamic_cast<SDFFvariableNode*>(child)
   || dynamic_cast<SDFFfunctionNode*>(child)) {
    s+=right->print();
  } else if (dynamic_cast<SDFFlogicNode*>(child))  {
    sdff_logic_ops oo=(dynamic_cast<SDFFlogicNode*>(child))->get_type();
    if (type==oo) s+=right->print(); else s+="("+right->print()+")";
  } else {
    s+="("+right->print()+")";
  }
  return s;
};






/* =======================================================================================================================
 * = SDFFvariableassignNode                                                                                                            =
 * =======================================================================================================================*/
sdff_result SDFFvariableassignNode::evaluate() {
  SDFFparser* parser=get_parser();
  sdff_result c;
  SDFFVariableManager* vman=parser->get_vmanager();
  //channelManager* cmanager=parser->get_cmanager();
  SDFFelementManager* emanager=parser->get_emanager();

  // evaluate right-hand-side
  c=child->evaluate();

  // if the variable name is a channel we can issue a set command on this channel
  if (parser->is_valid_channel_name(variable_name)) {
    std::string channel=parser->resolve_alias(variable_name);
    bool resolved=(channel!=variable_name);
    if (parser->get_inside_event() || parser->get_inside_preevent() || parser->get_inside_lastevent()) { // channel assign inside an event or relativ_event-block => relative time parameter
      double time=0;
      if (parameter!=NULL) {
        sdff_result p=parameter->evaluate();
        if (p.type==sdffRTDouble) { time=p.doubleValue; }
        else if (p.type==sdffRTInteger) { time=p.intValue; }
        else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_CHASSIGNWRONGTIMETYPE_NUM, "SDFFvariableassignNode.evaluate()", (variable_name+(resolved?(" (="+channel+")"):(""))).c_str(), sdff_resulttypetostr(p.type).c_str());
        if (time!=0 && (parser->get_inside_preevent() || parser->get_inside_lastevent()) ) RUN_TIME_ERRORN(SEQUENCER_SDFFERROR_DELAYEDOUTPUTINPRE_NUM, "SDFFvariableassignNode.evaluate()");
      }
      double value=0;
      if (c.type==sdffRTDouble) { value=c.doubleValue; }
      else if (c.type==sdffRTInteger) { value=c.intValue; }
      else if (c.type==sdffRTBoolean) { value=c.boolValue?1:0; }
      else  RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_CHASSIGNWRONGTYPE_NUM, "SDFFvariableassignNode.evaluate()", (variable_name+(resolved?(" (="+channel+")"):(""))).c_str(), sdff_resulttypetostr(c.type).c_str());
      SDFFSEset* e=new SDFFSEset(emanager, channel, parser->get_current_time()+time*1e6, value);
      e->set_line(parser->get_current_line());
      emanager->add(channel, e);
    } else { // channel assignment inside a channel-block or outside any block!  => absolute time
      if (parameter==NULL)
        RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_CHASSIGNNOTIME_NUM, "SDFFvariableassignNode.evaluate()", (variable_name+(resolved?(" (="+channel+")"):(""))).c_str());
      sdff_result p=parameter->evaluate();
      double time=0;
      if (p.type==sdffRTDouble) { time=p.doubleValue; }
      else if (p.type==sdffRTInteger) { time=p.intValue; }
      else RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_CHASSIGNWRONGTIMETYPE_NUM, "SDFFvariableassignNode.evaluate()", (variable_name+(resolved?(" (="+channel+")"):(""))).c_str(), sdff_resulttypetostr(p.type).c_str());
      double value=0;
      if (c.type==sdffRTDouble) { value=c.doubleValue; }
      else if (c.type==sdffRTInteger) { value=c.intValue; }
      else if (c.type==sdffRTBoolean) { value=c.boolValue?1:0; }
      else  RUN_TIME_ERRORV(SEQUENCER_SDFFERROR_CHASSIGNWRONGTYPE_NUM, "SDFFvariableassignNode.evaluate()", (variable_name+(resolved?(" (="+channel+")"):(""))).c_str(), sdff_resulttypetostr(c.type).c_str());
      SDFFSEset* e=new SDFFSEset(emanager, channel, time*1e6, value);
      e->set_line(parser->get_current_line());
      emanager->add(channel, e);
    }
  }
  // create a new variable
  else if (!vman->variable_exists(variable_name) || create) {
    switch (c.type) {
      case sdffRTDouble:
        vman->add_variable_double(variable_name, c.doubleValue, constant);
        break;
      case sdffRTInteger:
        vman->add_variable_int(variable_name, c.intValue, constant);
        break;
      case sdffRTBoolean:
        vman->add_variable_bool(variable_name, c.boolValue, constant);
        break;
      case sdffRTString:
        vman->add_variable_string(variable_name, c.stringValue, constant);
        break;
      case sdffRTVoid:
        RUN_TIME_ERRORN(SEQUENCER_SDFFERROR_VOIDUSED_NUM, "SDFFvariableassignNode.evaluate()");
      }
  } else { // or simply set the variable value
    vman->set(variable_name, c);
    if (constant) vman->set_constant(variable_name, true);
  }
  return c;
}

std::string SDFFvariableassignNode::print(int indent) {
  std::string in(indent, ' ');
  std::string res=variable_name;
  if (parameter!=NULL) res+="["+parameter->print()+"]";
  res+=" = "+child->print();
  return res;
};





/* =======================================================================================================================
 * = SDFFfunctionassignNode                                                                                                            =
 * =======================================================================================================================*/
sdff_result SDFFfunctionassignNode::evaluate() {
  SDFFFunctionManager* fman=parser->get_fmanager();
  std::vector<sdff_function_parameter> param;
  if (parameters.size()>0) {
    for (size_t i=0; i<parameters.size(); i++) {
      sdff_function_parameter p;
      p.name=parameters[i];
      p.type=sdffPTUnknown;
      param.push_back(p);
    }
  }

  fman->add_node(function_name, param, child, parser->get_current_line());

  // return void
  sdff_result r;
  r.type=sdffRTVoid;
  return r;
}

std::string SDFFfunctionassignNode::print(int indent) {
  std::string in(indent, ' ');
  std::string s=function_name+"(";
  if (parameters.size()>0) {
    for (size_t i=0; i<parameters.size(); i++) {
      if (i>0) s+=", ";
      s+=parameters[i];
    }
  }
  return s+") = "+child->print();
};








/* =======================================================================================================================
 * = SDFFwhileNode                                                                                                            =
 * =======================================================================================================================*/
sdff_result SDFFwhileNode::evaluate() {
  sdff_result r, t;
  r.type=sdffRTVoid;
  parser->get_vmanager()->enter_block();
  t=test->evaluate();
  if (t.type!=sdffRTBoolean) RUN_TIME_ERRORN(SEQUENCER_SDFFERROR_LOOPTESTNOTBOOL_NUM, "SDFFwhileNode.evaluate()");
  while (t.boolValue) {
    if (children.size()>0) {
      for (size_t i=0; i<children.size(); i++) {
        SDFFnode* n=children[i];
        if (n!=NULL) n->evaluate();
      }
    }
    t=test->evaluate();
    if (t.type!=sdffRTBoolean) RUN_TIME_ERRORN(SEQUENCER_SDFFERROR_LOOPTESTNOTBOOL_NUM, "SDFFwhileNode.evaluate()");
  }
  parser->get_vmanager()->leave_block();
  return r;
}

std::string SDFFwhileNode::print(int indent) {
  std::string in(indent, ' ');
  std::string s="while ("+test->print()+") {\n"+in;
  if (children.size()>0) {
    for (size_t i=0; i<children.size(); i++) {
      if (children[i]!= NULL)
        s+="  "+children[i]->print(indent+2)+";\n"+in;
    }
  }
  return s+"\n"+in+"}";
};








/* =======================================================================================================================
 * = SDFFdowhileNode                                                                                                            =
 * =======================================================================================================================*/
sdff_result SDFFdowhileNode::evaluate() {
  sdff_result r, t;
  r.type=sdffRTVoid;
  parser->get_vmanager()->enter_block();
  do {
    if (children.size()>0) {
      for (size_t i=0; i<children.size(); i++) {
        SDFFnode* n=children[i];
        if (n!=NULL) n->evaluate();
      }
    }
    t=test->evaluate();
    if (t.type!=sdffRTBoolean) RUN_TIME_ERRORN(SEQUENCER_SDFFERROR_LOOPTESTNOTBOOL_NUM, "SDFFdowhileNode.evaluate()");
  } while (t.boolValue);
  parser->get_vmanager()->leave_block();
  return r;
}

std::string SDFFdowhileNode::print(int indent) {
  std::string in(indent, ' ');
  std::string s="do {\n"+in;
  if (children.size()>0) {
    for (size_t i=0; i<children.size(); i++) {
      if (children[i]!= NULL)
        s+="  "+children[i]->print(indent+2)+";\n"+in;
    }
  }
  return s+"\n"+in+"} while ("+test->print()+")";
};







/* =======================================================================================================================
 * = SDFFrepeatuntilNode                                                                                                            =
 * =======================================================================================================================*/
sdff_result SDFFrepeatuntilNode::evaluate() {
  sdff_result r, t;
  r.type=sdffRTVoid;
  parser->get_vmanager()->enter_block();
  do {
    if (children.size()>0) {
      for (size_t i=0; i<children.size(); i++) {
        SDFFnode* n=children[i];
        if (n!=NULL) n->evaluate();
      }
    }
    t=test->evaluate();
    if (t.type!=sdffRTBoolean) RUN_TIME_ERRORN(SEQUENCER_SDFFERROR_LOOPTESTNOTBOOL_NUM, "SDFFrepeatuntilNode.evaluate()");
  } while (!t.boolValue);
  parser->get_vmanager()->leave_block();
  return r;
}

std::string SDFFrepeatuntilNode::print(int indent) {
  std::string in(indent, ' ');
  std::string s="repeat {\n"+in;
  if (children.size()>0) {
    for (size_t i=0; i<children.size(); i++) {
      if (children[i]!= NULL)
        s+="  "+children[i]->print(indent+2)+";\n"+in;
    }
  }
  return s+"\n"+in+"} until ("+test->print()+")";
};








/* =======================================================================================================================
 * = SDFFifNode                                                                                                            =
 * =======================================================================================================================*/

void SDFFifNode::set_parent(SDFFnode* parent){
  this->parent=parent;
  if (parent!=NULL) {
    parser=parent->get_parser();
  }
  if (truechildren.size()>0) {
    for (size_t i=0; i<truechildren.size(); i++) {
      SDFFnode* c=truechildren[i];
      if (c!=NULL) c->set_parent(this);
    }
  }
  if (falsechildren.size()>0) {
    for (size_t i=0; i<falsechildren.size(); i++) {
      SDFFnode* c=falsechildren[i];
      if (c!=NULL) c->set_parent(this);
    }
  }
}

void SDFFifNode::add_true_child(SDFFnode* child){
  if (child!=NULL) {
    child->set_parent(this);
    child->set_parser(this->parser);
  }
  truechildren.push_back(child);
}

void SDFFifNode::add_false_child(SDFFnode* child){
  if (child!=NULL) {
    child->set_parent(this);
    child->set_parser(this->parser);
  }
  falsechildren.push_back(child);
}

void SDFFifNode::add_true_children(SDFFnaryNode* children){
  if (children->get_child_count()>0)
    for (size_t i=0; i<children->get_child_count(); i++) {
      SDFFnode* child=children->get_child(i);
      if (child!=NULL) {
        child->set_parent(this);
        child->set_parser(this->parser);
      }
      truechildren.push_back(child);
    }
}

void SDFFifNode::add_false_children(SDFFnaryNode* children){
  if (children->get_child_count()>0)
    for (size_t i=0; i<children->get_child_count(); i++) {
      SDFFnode* child=children->get_child(i);
      if (child!=NULL) {
        child->set_parent(this);
        child->set_parser(this->parser);
      }
      falsechildren.push_back(child);
    }
}

void SDFFifNode::set_parser(SDFFparser* parser){
  this->parser=parser;
  if (truechildren.size()>0) {
    for (size_t i=0; i<truechildren.size(); i++) {
      SDFFnode* c=truechildren[i];
      if (c!=NULL) c->set_parser(parser);
    }
  }
  if (falsechildren.size()>0) {
    for (size_t i=0; i<falsechildren.size(); i++) {
      SDFFnode* c=falsechildren[i];
      if (c!=NULL) c->set_parser(parser);
    }
  }
}

void SDFFifNode::dispose(){
  if (truechildren.size()>0) {
    for (size_t i=0; i<truechildren.size(); i++) {
      SDFFnode* c=truechildren[i];
      if (c!=NULL) {
        delete c;
        truechildren[i]=NULL;
      }
    }
  }
  truechildren.clear();
  if (falsechildren.size()>0) {
    for (size_t i=0; i<falsechildren.size(); i++) {
      SDFFnode* c=falsechildren[i];
      if (c!=NULL) {
        delete c;
        falsechildren[i]=NULL;
      }
    }
  }
  falsechildren.clear();
}

void SDFFifNode::deconstruct(){
  if (truechildren.size()>0) {
    for (size_t i=0; i<truechildren.size(); i++) {
      SDFFnode* c=truechildren[i];
      if (c!=NULL) {
        c->deconstruct();
        truechildren[i]=NULL;
      }
    }
  }
  truechildren.clear();
  if (falsechildren.size()>0) {
    for (size_t i=0; i<falsechildren.size(); i++) {
      SDFFnode* c=falsechildren[i];
      if (c!=NULL) {
        c->deconstruct();
        falsechildren[i]=NULL;
      }
    }
  }
  falsechildren.clear();
}

SDFFnode* SDFFifNode::replace_true_child(size_t n, SDFFnode* child, bool destroy_old) {
  SDFFnode* old=NULL;
  if (n<truechildren.size()) {
    old=truechildren[n];
    truechildren[n]=child;
  }
  if (destroy_old && (old!=NULL)){
    delete old;
    old=NULL;
  }
  return old;
}

SDFFnode* SDFFifNode::replace_false_child(size_t n, SDFFnode* child, bool destroy_old) {
  SDFFnode* old=NULL;
  if (n<falsechildren.size()) {
    old=falsechildren[n];
    falsechildren[n]=child;
  }
  if (destroy_old && (old!=NULL)){
    delete old;
    old=NULL;
  }
  return old;
}

SDFFnode* SDFFifNode::delete_true_child(size_t n, bool destroy_old) {
  SDFFnode* old=NULL;
  if (n<truechildren.size()) {
    old=truechildren[n];
    truechildren.erase(truechildren.begin()+n);
  }
  if (destroy_old && (old!=NULL)){
    delete old;
    old=NULL;
  }
  return old;
}

SDFFnode* SDFFifNode::delete_false_child(size_t n, bool destroy_old) {
  SDFFnode* old=NULL;
  if (n<falsechildren.size()) {
    old=falsechildren[n];
    falsechildren.erase(falsechildren.begin()+n);
  }
  if (destroy_old && (old!=NULL)){
    delete old;
    old=NULL;
  }
  return old;
}

sdff_result SDFFifNode::evaluate() {
  sdff_result r, t;
  r.type=sdffRTVoid;
  SDFFVariableManager*vm=parser->get_vmanager();
  t=test->evaluate();
  if (t.type!=sdffRTBoolean) RUN_TIME_ERRORN(SEQUENCER_SDFFERROR_IFTESTNOTBOOL_NUM, "SDFFifNode.evaluate()");
  if (t.boolValue) {
    vm->enter_block();
    if (truechildren.size()>0) {
      for (size_t i=0; i<truechildren.size(); i++) {
        SDFFnode* n=truechildren[i];
        if (n!=NULL) n->evaluate();
      }
    }
    vm->leave_block();
  } else {
    vm->enter_block();
    if (falsechildren.size()>0) {
      for (size_t i=0; i<falsechildren.size(); i++) {
        SDFFnode* n=falsechildren[i];
        if (n!=NULL) n->evaluate();
      }
    }
    vm->leave_block();
  }
  return r;
}

std::string SDFFifNode::print(int indent) {
  std::string in(indent, ' ');
  std::string s="if ("+test->print()+") {\n"+in;
  if (truechildren.size()>0) {
    for (size_t i=0; i<truechildren.size(); i++) {
      if (truechildren[i]!= NULL)
        s+="  "+truechildren[i]->print(indent+2)+";\n"+in;
    }
  }
  bool falsePrinted=false;
  if (falsechildren.size()==1) {
      if (SDFFifNode* nf=dynamic_cast<SDFFifNode*>(falsechildren[0])) {
          s+="} else"+nf->print(indent);
          falsePrinted=true;
      }
  }
  if (!falsePrinted && falsechildren.size()>0) {
    s+="} else {\n"+in;
    for (size_t i=0; i<falsechildren.size(); i++) {
      if (falsechildren[i]!= NULL)
        s+="  "+falsechildren[i]->print(indent+2)+";\n"+in;
    }
    s+="}";
  }
  return s;
};









/* =======================================================================================================================
 * = SDFFeventNode                                                                                                            =
 * =======================================================================================================================*/
sdff_result SDFFeventNode::evaluate() {
  sdff_result r, t;
  r.type=sdffRTVoid;
  SDFFVariableManager* vm=parser->get_vmanager();
  t=time->evaluate();
  if (t.type==sdffRTDouble) {
    if (relative) {
      parser->set_current_time(parser->get_current_time()+t.doubleValue*1e6);
      //std::cout<<"rel t="<<parser->get_current_time()<<"   inc="<<t.doubleValue*1e6<<std::endl;
    } else {
      parser->set_current_time(t.doubleValue*1e6);
      //std::cout<<"abs t="<<parser->get_current_time()<<"   set="<<t.doubleValue*1000000<<std::endl;
    }
  } else if (t.type==sdffRTInteger) {
    if (relative) {
      parser->set_current_time(parser->get_current_time()+t.intValue*1000000);
      //std::cout<<"rel t="<<parser->get_current_time()<<"   inc="<<t.intValue*1000000<<std::endl;
    } else {
      parser->set_current_time(t.intValue*1000000);
      //std::cout<<"abs t="<<parser->get_current_time()<<"   set="<<t.intValue*1000000<<std::endl;
    }
  } else RUN_TIME_ERRORN(SEQUENCER_SDFFERROR_TIMENOTNUMERIC_NUM, "SDFFeventNode.evaluate()");
  vm->enter_block();
  parser->set_inside_event(true);
  if (children.size()>0) {
    for (size_t i=0; i<children.size(); i++) {
      SDFFnode* n=children[i];
      if (n!=NULL) n->evaluate();
    }
  }
  parser->set_inside_event(false);
  vm->leave_block();
  return r;
}


std::string SDFFeventNode::print(int indent) {
  std::string in(indent, ' ');
  std::string s="";
  if (!relative) {
    s+="event ("+time->print()+") {\n"+in;
  } else {
    s+="relative_event ("+time->print()+") {\n"+in;
  }
  if (children.size()>0) {
    for (size_t i=0; i<children.size(); i++) {
      if (children[i]!= NULL)
        s+="  "+children[i]->print(indent+2)+";\n"+in;
    }
  }
  return s+"\n"+in+"}";
};









/* =======================================================================================================================
 * = SDFFpreEventNode                                                                                                            =
 * =======================================================================================================================*/
sdff_result SDFFpreEventNode::evaluate() {
  sdff_result r, t;
  r.type=sdffRTVoid;
  SDFFVariableManager* vm=parser->get_vmanager();
  parser->set_current_time(0);

  vm->enter_block();
  parser->set_inside_preevent(true);
  if (children.size()>0) {
    for (size_t i=0; i<children.size(); i++) {
      SDFFnode* n=children[i];
      if (n!=NULL) n->evaluate();
    }
  }
  parser->set_inside_preevent(false);
  vm->leave_block();
  return r;
}


std::string SDFFpreEventNode::print(int indent) {
  std::string in(indent, ' ');
  std::string s="";
  s+="pre_event {\n"+in;
  if (children.size()>0) {
    for (size_t i=0; i<children.size(); i++) {
      if (children[i]!= NULL)
        s+="  "+children[i]->print(indent+2)+";\n"+in;
    }
  }
  return s+"\n"+in+"}";
};




/* =======================================================================================================================
 * = SDFFlastEventNode                                                                                                            =
 * =======================================================================================================================*/
sdff_result SDFFlastEventNode::evaluate() {
  sdff_result r, t;
  r.type=sdffRTVoid;
  SDFFVariableManager* vm=parser->get_vmanager();

  vm->enter_block();
  parser->set_inside_lastevent(true);
  if (children.size()>0) {
    for (size_t i=0; i<children.size(); i++) {
      SDFFnode* n=children[i];
      if (n!=NULL) n->evaluate();
    }
  }
  parser->set_inside_lastevent(false);
  vm->leave_block();
  return r;
}


std::string SDFFlastEventNode::print(int indent) {
  std::string in(indent, ' ');
  std::string s="";
  s+="last_event {\n"+in;
  if (children.size()>0) {
    for (size_t i=0; i<children.size(); i++) {
      if (children[i]!= NULL)
        s+="  "+children[i]->print(indent+2)+";\n"+in;
    }
  }
  return s+"\n"+in+"}";
};








/* =======================================================================================================================
 * = SDFFchannelNode                                                                                                            =
 * =======================================================================================================================*/
sdff_result SDFFchannelNode::evaluate() {
  sdff_result r, t;
  r.type=sdffRTVoid;
  SDFFVariableManager* vm=parser->get_vmanager();
  t=channel->evaluate();
  if (t.type==sdffRTString) {
    parser->set_current_channel(t.stringValue);
  } else RUN_TIME_ERRORN(SEQUENCER_SDFFERROR_CHANNELNOTSTRING_NUM, "SDFFchannelNode.evaluate()");
  vm->enter_block();
  if (children.size()>0) {
  parser->set_inside_channel(true);
    for (size_t i=0; i<children.size(); i++) {
      SDFFnode* n=children[i];
      if (n!=NULL) n->evaluate();
    }
  }
  parser->set_inside_channel(false);
  vm->leave_block();
  parser->set_current_channel("");
  return r;
}


std::string SDFFchannelNode::print(int indent) {
  std::string in(indent, ' ');
  std::string s="";
  s+="channel ("+channel->print()+") {\n"+in;
  if (children.size()>0) {
    for (size_t i=0; i<children.size(); i++) {
      if (children[i]!= NULL)
        s+="  "+children[i]->print(indent+2)+";\n"+in;
    }
  }
  return s+"\n"+in+"}";
};



};

