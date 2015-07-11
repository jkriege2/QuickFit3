/** \file jkiniparser.cpp
 *  \ingroup jkip
 */

#define COMPILING_THIS_JKINIPARSER
#include "jkiniparser.h" // class's header file
#include <iostream>
#include <stdlib.h>
using namespace std;

// class constructor
jkipError::jkipError() throw() {
  errormessage="unknown error";
}

jkipError::jkipError(std::string msg) throw() {
  errormessage=msg;
}

// returns the assigned errormessage
std::string jkipError::getMessage() const {
	return errormessage;
}







// class constructor
jkiniparser::jkiniparser()
{
  jkiCommentMarker='#';
  jkiMultilineMarker='\\';
  allowMultiLine=false;
  clear();
}

// class constructor that automatically loads an ini-file
jkiniparser::jkiniparser(std::string filename, bool allowML)
{
  jkiCommentMarker='#';
  jkiMultilineMarker='\\';
  allowMultiLine = allowML;
  readFile(filename);
}

// class destructor
jkiniparser::~jkiniparser()
{
	clear();
}


int jkiniparser::getGroupCount(){
    return dictionary.size();
};

std::string jkiniparser::getGroupName(int num){
  typedef std::map<std::string, std::map<std::string,std::string> >::const_iterator ITER;
  int j=0;

  if (dictionary.size()>0) {
  	for (ITER i=dictionary.begin(); i!=dictionary.end(); ++i) {
        if (j==num) return i->first;
        j++;
    }
  }
  jkipException("index out of range");
  return "";
};



// deletes all saved configuration data
void jkiniparser::clear()
{
	dictionary.clear();
}

// reads in an ini-file
void jkiniparser::readFile(std::string filename)
{
  clear();
  if ((configf=fopen(filename.c_str(), "r"))!=NULL) {
     parse_file(true);
     fclose(configf);
  }
}

// writes configuration data to an ini-file
void jkiniparser::writeFile(std::string filename)
{
	typedef std::map<std::string, std::map<std::string,std::string> >::const_iterator ITER;
	typedef std::map<std::string,std::string>::const_iterator IITER;

//	cout<<"opening ... "<<filename<<endl;
  configf=fopen(filename.c_str(), "w");
//	cout<<"opened ... "<<filename<<endl;
  if ((!ferror(configf))&&(dictionary.size()>0)) {
  	for (ITER i=dictionary.begin(); i!=dictionary.end(); ++i) {
        if (i->first!="")
          fprintf(configf, "\n[%s]\n", i->first.c_str());
        for (IITER j=i->second.begin(); j!=i->second.end(); ++j) {
          fprintf(configf, "%s = %s\n", j->first.c_str(), j->second.c_str());
        }
    }

  	fclose(configf);
  }
}

// sets group/field with data
void jkiniparser::set(std::string group, std::string field, int data)
{
  std::ostringstream ost;
  ost<<data;
  set(group, field, ost.str());
}

void jkiniparser::set(std::string group, std::string field, std::string data)
{
	dictionary[group][field]=data;
}

void jkiniparser::set(std::string group, std::string field, double data)
{
  std::ostringstream ost;
  ost<<data;
  set(group, field, ost.str());
}

void jkiniparser::set(std::string group, std::string field, bool data)
{
  std::ostringstream ost;
  ost<<data;
  set(group, field, ost.str());
}

// returns the contents of group/field
std::string jkiniparser::getAsString(std::string group, std::string field, std::string stdresult)
{
	std::string result;
  if (exists(group, field))
    result=dictionary[group][field];
  else
    result=stdresult;
  return result;
}

bool jkiniparser::getAsBool(std::string group, std::string field, bool stdresult)
{
	bool result=false;
  if (exists(group, field)){
  	std::string data=dictionary[group][field];
  	if (data=="true") result=true;
  	else if (data=="1") result=true;
  	else if (data=="yes") result=true;
  	else if (data=="on") result=true;
  	else if (data=="o") result=true;
  	else if (data=="wahr") result=true;
  	else if (data=="ja") result=true;
  	else if (data=="y") result=true;
  	else if (data=="j") result=true;
  	else if (data=="t") result=true;
  } else
    result=stdresult;
  return result;
}

int jkiniparser::getAsInt(std::string group, std::string field, int stdresult)
{
	int result;
  if (exists(group, field))
    result=atoi(getAsString(group,field).c_str());
  else
    result=stdresult;
  return result;
}

double jkiniparser::getAsDouble(std::string group, std::string field, double stdresult)
{
	double result;
  if (exists(group, field))
    result=atof(getAsString(group,field).c_str());
  else
    result=stdresult;
  return result;
}


bool jkiniparser::groupExists(std::string group){
	typedef std::map<std::string, std::map<std::string,std::string> >::const_iterator ITER;

  ITER i=dictionary.find(group);
	if (i!=dictionary.end()){
    return true;
  }
  return false;
};

// returns true, if [group] and field exist
bool jkiniparser::exists(std::string group, std::string field)
{
	typedef std::map<std::string, std::map<std::string,std::string> >::const_iterator ITER;
	typedef std::map<std::string,std::string>::const_iterator IITER;

  ITER i=dictionary.find(group);
	if (i!=dictionary.end()){
    IITER j=i->second.find(field);
	  if (j!=i->second.end())
	    return true;
  }
  return false;
}

// reads a new token from Config-File
jkiniparser::jkiTOKEN jkiniparser::getToken()
{
	unsigned char ch=0;
  ch=fgetc(configf);
	while((!feof(configf)) && isspace(ch)) {
		ch=fgetc(configf);
	}
	if (feof(configf)) return jkiEOFile;
	switch (ch) {
		case 0:
		case -1:
			return currentToken=jkiEND;
		case 10:
		case 13:
		//case ';':
			return currentToken=jkiPRINT;
		case '[':
			return currentToken=jkiLBRACKET;
		default:
            if (ch==jkiCommentMarker) return currentToken=jkiCOMMENT;
			if (isalpha(ch)) {
                ungetc(ch,configf);
				currentField=parse_name('=');

				return currentToken=jkiNAME;
			}
			// the parser has found an unknown token. an exception will be thrown
			jkipException("unknown token ");
	}
	return jkiUNKNOWN;
}

// registeres a group name
void jkiniparser::registerGroup(std::string group)
{
	if (!groupexists(group)) {
    dictionary[group];
  }
	//cout<<"  + registering group "<<group<<endl;
}

// return true if the specified group is already registered
bool jkiniparser::groupexists(std::string group)
{
	return dictionary.find(group)!=dictionary.end();
}

// parses a group declaration of the form [groupname]
void jkiniparser::parse_groupdecl(bool get)
{
  currentGroup=parse_name(']');
	//cout<<"  + parsed groupdecl ["<<currentGroup<<"]"<<endl;
  registerGroup(currentGroup);
	if (get) currentToken=getToken();
	//cout<<"  + return groupdecl ["<<currentGroup<<"]"<<endl;
}

// parses a fieldassignment of the form field = anyvalue
void jkiniparser::parse_fieldassignment(bool get)
{
  //parse_name('=');
  if (allowMultiLine) {  currentValue=parse_data_multiline(jkiCommentMarker);  }
  else { currentValue=parse_data(jkiCommentMarker); }
  set(currentGroup, currentField, currentValue);
	//cout<<"  + parsed assign: "<<currentGroup<<" /  "<<currentField<<" /  "<<currentValue<<endl;
	if (get && (currentToken!=jkiCOMMENT)) currentToken=getToken();
}

// reads in all characters until end of line or file
std::string jkiniparser::parse_data(unsigned char commentchar)
{
  std::string result="";
	unsigned char ch=0;

  ch=fgetc(configf);
	while((!feof(configf)) ) {
    if (ch==commentchar) {
      currentToken=jkiCOMMENT;
      return result;
    }
    if ((ch==13)||(ch==10)) {
      currentToken=jkiPRINT;
      return result;
    }
    if ((result!="")|| ((result=="")&&(!isspace(ch))))result+=ch;
		ch=fgetc(configf);
	}
	//cout<<"  + parsed data: "<<result<<endl;
  return result;
}

std::string jkiniparser::parse_data_multiline(unsigned char commentchar)
{
  std::string result="";
	unsigned char ch=0;

  ch=fgetc(configf);
	while((!feof(configf)) ) {
    if (ch==commentchar) {
      currentToken=jkiCOMMENT;
      return result;
    }
    if (ch=='\n') {
      if (!allowMultiLine) {
        currentToken=jkiPRINT;
        return result;
      } else {
        if (result[result.size()-1] != jkiMultilineMarker) {
          currentToken=jkiPRINT;
          return result;
        } else {
          result.erase(result.size()-1);
          ch=fgetc(configf);
        }
      }
    }
    if ((result!="")|| ((result=="")&&(!isspace(ch))))result+=ch;
		ch=fgetc(configf);
	}
	//cout<<"  + parsed data: "<<result<<endl;
  return result;
}

// reads in a name
std::string jkiniparser::parse_name(unsigned char endchar)
{
  std::string result="";
	unsigned char ch=0;

  ch=fgetc(configf);
	while((!feof(configf)) &&(ch!=endchar)) {
    if (!isspace(ch)) result+=ch;
		ch=fgetc(configf);
	}
	//cout<<"  + parsed name: "<<result<<endl;
  return result;
}

// parses a config file
void jkiniparser::parse_file(bool get)
{
	if (get) currentToken=getToken();
	for(;;) {
    //cout<<"  + Token:  "<<currentToken<<endl;
    switch (currentToken) {
      case jkiEOFile: return;
      case jkiCOMMENT:
           //cout<<"  + found comment"<<endl;
           parse_data(0);
           break;
      case jkiLBRACKET:
           //cout<<"  + found lbracket"<<endl;
           parse_groupdecl(true);
           break;
      case jkiNAME:
           //cout<<"  + found name"<<endl;
           parse_fieldassignment(true);
           break;
      default:
           currentToken=getToken();
    }
  }
}

// prints the config-data to screen
void jkiniparser::print()
{
	typedef std::map<std::string, std::map<std::string,std::string> >::const_iterator ITER;
	typedef std::map<std::string,std::string>::const_iterator IITER;

  printf("* config **********************************************************\n");
	if (dictionary.size()>0) {
    for (ITER i=dictionary.begin(); i!=dictionary.end(); ++i) {
        if (i->first!="")
          printf("\n[{%s}]\n", i->first.c_str());
        for (IITER j=i->second.begin(); j!=i->second.end(); ++j) {
          printf("{%s} = {%s}\n", j->first.c_str(), j->second.c_str());
        }
    }
  }
  printf("*******************************************************************\n");

}

#ifdef JKIP_USE_MATHPARSER
bool jkiniparser::getAsParsedBool(std::string group, std::string field, bool stdresult)
{
  bool result=false;
  if (exists(group, field)){
  	std::string data=dictionary[group][field];
  	jkMathParser::jkmpNode* n=NULL;
  	jkMathParser::jkmpResult r;
  	try {
      n=mp.parse(data);
  	  r=n->evaluate();
    } catch(jkMathParser::jkmpException& E) {
      jkipException("error in math parser while parsing "+field+" in ["+group+"]: \n"+E.getMessage());
    }
  	delete n;
  	if (r.type==jkMathParser::jkmpBool) result=r.boolean;
  	if (r.type==jkMathParser::jkmpDouble) result=r.num>0;
  	if (r.type==jkMathParser::jkmpString) {
        data=r.str;
      	if (data=="true") result=true;
      	else if (data=="1") result=true;
      	else if (data=="yes") result=true;
      	else if (data=="on") result=true;
      	else if (data=="o") result=true;
      	else if (data=="wahr") result=true;
      	else if (data=="ja") result=true;
      	else if (data=="y") result=true;
      	else if (data=="j") result=true;
      	else if (data=="t") result=true;
    }
  }else
    result=stdresult;
  return result;
}

int jkiniparser::getAsParsedInt(std::string group, std::string field, int stdresult)
{
	int result;
    if (exists(group, field))  result=atoi(getAsString(group,field).c_str());
  	std::string data=dictionary[group][field];
  	jkMathParser::jkmpNode* n;
  	jkMathParser::jkmpResult r;
  	try {
    	n=mp.parse(data);
    	r=n->evaluate();
    } catch(jkMathParser::jkmpException E) {
        jkipException("error in math parser while parsing "+field+" in ["+group+"]: \n"+E.getMessage());
    }
  	delete n;
  	if (r.type==jkMathParser::jkmpBool) result=r.boolean?1:0;
  	if (r.type==jkMathParser::jkmpDouble) result=(int)r.num;
  	if (r.type==jkMathParser::jkmpString) {
        result=stdresult;
    } else result=stdresult;
    return result;
}

double jkiniparser::getAsParsedDouble(std::string group, std::string field, double stdresult)
{
	double result;
  if (exists(group, field))
    result=atoi(getAsString(group,field).c_str());
  	std::string data=dictionary[group][field];
  	jkMathParser::jkmpNode* n=NULL;
  	jkMathParser::jkmpResult r;
  	try {
    	n=mp.parse(data);
  	    r=n->evaluate();
    } catch(jkMathParser::jkmpException E) {
      jkipException("error in math parser while parsing "+field+" in ["+group+"]: \n"+E.getMessage());
    }
  	delete n;
  	if (r.type==jkMathParser::jkmpBool) result=r.boolean?1:0;
  	if (r.type==jkMathParser::jkmpDouble) result=r.num;
  	if (r.type==jkMathParser::jkmpString) {
        result=stdresult;
    }
  else
    result=stdresult;
  return result;
}

#endif

