/** \file QINIParser2.cpp
 *  \ingroup jkip2
 */

#include "qiniparser2.h" // class's header file
#include <iostream>
using namespace std;



//jkip2Property
std::string QINIParser2::jkip2Property::toString() {
    switch(type) {
        case QINIParser2::jkip2Double: return floattostr(doubleData);
        case QINIParser2::jkip2Int: return inttostr(intData);
        case QINIParser2::jkip2Bool: return booltostr(boolData);
        case QINIParser2::jkip2String: return stringData;
    }
    return "";
};

/** \brief returns the value of the current node as a string,
 *         if the node contains string data the string is put into quotation marks (\c " ).
 *         In addition the string is escapified
 */
std::string QINIParser2::jkip2Property::toDelimString() {
    switch(type) {
        case QINIParser2::jkip2Double: return floattostr(doubleData);
        case QINIParser2::jkip2Int: return inttostr(intData);
        case QINIParser2::jkip2Bool: return booltostr(boolData);
        case QINIParser2::jkip2String: return "\""+pascalifyString(stringData, '"')+"\"";
    }
    return "";
};

/** \brief tries to convert the contents of the property to a boolean. Returns \a defaultValue if the conversion is not possible. */
bool QINIParser2::jkip2Property::toBool(bool defaultValue) {
    if (type==QINIParser2::jkip2Bool) return boolData;
    if (type==QINIParser2::jkip2Int) return intData>0;
    if (type==QINIParser2::jkip2Double) return doubleData>0;
    std::string ldata=strstrip(tolower(stringData));
    if (ldata=="true" || ldata=="ja" || ldata=="yes" || ldata=="y" || ldata=="on" || ldata=="j" || ldata=="1") return true;
    if (ldata=="false" || ldata=="nein" || ldata=="no" || ldata=="n" || ldata=="off" || ldata=="0") return false;
    return defaultValue;
};

/** \brief tries to convert the contents of the property to a boolean. Throws an exception if the conversion is not possible. */
bool QINIParser2::jkip2Property::toBool() {
    if (type==QINIParser2::jkip2Bool) return boolData;
    if (type==QINIParser2::jkip2Int) return intData>0;
    if (type==QINIParser2::jkip2Double) return doubleData>0;
    std::string ldata=strstrip(tolower(stringData));
    if (ldata=="true" || ldata=="ja" || ldata=="yes" || ldata=="y" || ldata=="on" || ldata=="j" || ldata=="1") return true;
    if (ldata=="false" || ldata=="nein" || ldata=="no" || ldata=="n" || ldata=="off" || ldata=="0") return false;
    parser->throwException(format("cannot convert '%s' to boolean", stringData.c_str()));
    return false;
};

/** \brief tries to convert the contents of the property to an integer. Returns \a defaultValue if the conversion is not possible. */
int64_t QINIParser2::jkip2Property::toInt(int64_t defaultValue) {
    if (type==QINIParser2::jkip2Bool) return boolData?1:0;
    if (type==QINIParser2::jkip2Int) return intData;
    if (type==QINIParser2::jkip2Double) return (int64_t)round(doubleData);
    char* ti;
    int64_t i=strtol(stringData.c_str(), &ti, 0);
    if (ti!=stringData.c_str()) {
        return i;
    }
    return defaultValue;
};

/** \brief tries to convert the contents of the property to an integer. Throws an exception if the conversion is not possible. */
int64_t QINIParser2::jkip2Property::toInt() {
    if (type==QINIParser2::jkip2Bool) return boolData?1:0;
    if (type==QINIParser2::jkip2Int) return intData;
    if (type==QINIParser2::jkip2Double) return (int64_t)round(doubleData);
    char* ti;
    int64_t i=strtol(stringData.c_str(), &ti, 0);
    if (ti!=stringData.c_str()) {
        return i;
    }
    parser->throwException(format("cannot convert '%s' to integer", stringData.c_str()));
    return 0;
};

/** \brief tries to convert the contents of the property to a double. Returns \a defaultValue if the conversion is not possible. */
double QINIParser2::jkip2Property::toDouble(double defaultValue) {
    if (type==QINIParser2::jkip2Bool) return boolData?1.0:0.0;
    if (type==QINIParser2::jkip2Int) return (double)intData;
    if (type==QINIParser2::jkip2Double) return doubleData;
    char* ti;
    double i=strtod(stringData.c_str(), &ti);
    if (ti!=stringData.c_str()) {
        return i;
    }
    return defaultValue;
};

/** \brief tries to convert the contents of the property to a double. Throws an exception if the conversion is not possible. */
double QINIParser2::jkip2Property::toDouble() {
    if (type==QINIParser2::jkip2Bool) return boolData?1.0:0.0;
    if (type==QINIParser2::jkip2Int) return (double)intData;
    if (type==QINIParser2::jkip2Double) return doubleData;
    char* ti;
    double i=strtod(stringData.c_str(), &ti);
    if (ti!=stringData.c_str()) {
        return i;
    }
    parser->throwException(format("cannot convert '%s' to double", stringData.c_str()));
    return 1.0/0.0;
};


/** \brief this function tries to interpret the given string as a floating point or an integer number
 *         and then sets the node accordingly */
void QINIParser2::jkip2Property::setFromNumberString(std::string data) {
    char* td;
    char* ti;
    double d=strtod(data.c_str(), &td);
    int64_t i=strtol(data.c_str(), &ti, 0);
    if ((double)i==d) {
        if (ti!=data.c_str()) {
            type=QINIParser2::jkip2Int;
            intData=i;
        } else {
            parser->throwException(format("'%s' does not contain a valid integer number", data.c_str()));
        }
    } else {
        if (td!=data.c_str()) {
            type=QINIParser2::jkip2Double;
            doubleData=d;
        } else {
            parser->throwException(format("'%s' does not contain a valid floating point number", data.c_str()));
        }
    }
};

/** \brief this function tries to interpret the given string as a boolean, floating point or an integer number
 *         and then sets the node accordingly. If none of the above types is recognized it sets the node as a
 *         string node. */
void QINIParser2::jkip2Property::setFromString(std::string data) {
    std::string ldata=strstrip(tolower(data));
    if (ldata=="true" || ldata=="ja" || ldata=="yes" || ldata=="y" || ldata=="on" || ldata=="j") {
        type=QINIParser2::jkip2Bool;
        boolData=true;
        return;
    } else if (ldata=="false" || ldata=="nein" || ldata=="no" || ldata=="n" || ldata=="off") {
        type=QINIParser2::jkip2Bool;
        boolData=false;
        return;
    }
    char* td;
    char* ti;
    double d=strtod(ldata.c_str(), &td);
    int64_t i=strtol(ldata.c_str(), &ti, 0);
    if ((double)i==d) {
        if (strcmp(ti, "\0")) {
            type=QINIParser2::jkip2Int;
            intData=i;
            return;
        } else {
            type=QINIParser2::jkip2String;
            stringData=data;
            return;
        }
    } else {
        if (strcmp(td,"\0")) {
            type=QINIParser2::jkip2Double;
            doubleData=d;
            return;
        } else {
            type=QINIParser2::jkip2String;
            stringData=data;
            return;
        }
    }
    type=QINIParser2::jkip2String;
    stringData=data;
    if (data.size()>=2) {
        char f=data[0];
        if (f=='\'' || f=='\"') {
            stringData=removeDelimitersPascal(data);
        }
    }
}

/** \brief set the contents of the property according to the data in a jkmpResult struct */
void QINIParser2::jkip2Property::setFromJKMPResult(jkMathParser::jkmpResult res) {
    switch(res.type) {
        case jkMathParser::jkmpDouble:
            doubleData=res.num;
            type=QINIParser2::jkip2Double;
            intData=(int64_t)round(doubleData);
            if (doubleData==(double)intData) {
                type=QINIParser2::jkip2Int;
            }
            return;
        case jkMathParser::jkmpString:
            stringData=res.str;
            type=QINIParser2::jkip2String;
            return;
        case jkMathParser::jkmpBool:
            boolData=res.boolean;
            type=QINIParser2::jkip2Bool;
            return;
    }
}













//QINIParser2::groupNode;

QINIParser2::groupNode::~groupNode() {
     clear();
}

void QINIParser2::groupNode::clear() {
     std::map<std::string, QINIParser2::groupNode*>::const_iterator it=child.begin();
     while (it!=child.end()) {
         delete it->second;
         ++it;
     }
     child.clear();
     property.clear();
}

void QINIParser2::groupNode::setProperty(std::string propertyName, QINIParser2::jkip2Property value) {
  std::string p=tolower(strstrip(propertyName));
  size_t found=p.find(".");
  if (found==std::string::npos) { // this is a property name without groups
      property[p]=value;
  } else {
      std::string group=p.substr(0, found);
      std::string rest=p.substr(found+1);
      if (child.find(group)==child.end()) {
          child[group]=new QINIParser2::groupNode(this, parser);
      }
      child[group]->setProperty(rest, value);
  }
}

bool QINIParser2::groupNode::exists(std::string propertyName) {
  std::string p=tolower(strstrip(propertyName));
  size_t found=p.find(".");
  if (found==std::string::npos) { // this is a property name without groups
      return (property.find(p)!=property.end());
  } else {
      std::string group=p.substr(0, found);
      std::string rest=p.substr(found+1);
      if (child.find(group)==child.end()) {
          return false;
      }
      return child[group]->exists(rest);
  }
}

bool QINIParser2::groupNode::groupExists(std::string name) {
  std::string p=tolower(strstrip(name));
  size_t found=p.find(".");
  if (found==std::string::npos) { // this is a property name without groups
      return (child.find(p)!=child.end());
  } else {
      std::string group=p.substr(0, found);
      std::string rest=p.substr(found+1);
      if (child.find(group)==child.end()) {
          return false;
      }
      return child[group]->groupExists(rest);
  }
}

QINIParser2::groupNode* QINIParser2::groupNode::getGroup(std::string name) {
  std::map<std::string, QINIParser2::groupNode*>::const_iterator it;
  std::string p=tolower(strstrip(name));
  size_t found=p.find(".");
  if (found==std::string::npos) { // this is a property name without groups
      it=child.find(p);
      if (it==child.end()) return NULL;
      else return it->second;
  } else {
      std::string group=p.substr(0, found);
      std::string rest=p.substr(found+1);
      if (child.find(group)==child.end()) {
          return NULL;
      }
      return child[group]->getGroup(rest);
  }
}

QINIParser2::groupNode* QINIParser2::groupNode::ensureGroup(std::string name) {
  std::map<std::string, QINIParser2::groupNode*>::const_iterator it;
  std::string p=tolower(strstrip(name));
  size_t found=p.find(".");
  if (found==std::string::npos) { // this is a property name without groups
      it=child.find(p);
      if (it==child.end()) {
          child[p]=new QINIParser2::groupNode(this, parser);
      }
      return child[p];
  } else {
      std::string group=p.substr(0, found);
      std::string rest=p.substr(found+1);
      if (child.find(group)==child.end()) {
          child[group]=new QINIParser2::groupNode(this, parser);
          return child[group]->ensureGroup(rest);
      }
      return child[group]->ensureGroup(rest);
  }
}

QINIParser2::groupNode* QINIParser2::groupNode::getGroup(size_t n) {
    if (n>=child.size()) return NULL;
    std::map<std::string, QINIParser2::groupNode*>::const_iterator it=child.begin();
    for (size_t i=0; i<n; i++) {
        it++;
    };
    return it->second;
}

QINIParser2::jkip2Property QINIParser2::groupNode::getProperty(size_t n) {
    if (n>=property.size()) parser->throwException("there are only "+inttostr(property.size())+" properties in this group node. You tried to access property no."+inttostr(n)+"!");
    std::map<std::string, QINIParser2::jkip2Property>::const_iterator it=property.begin();
    for (size_t i=0; i<n; i++) {
        it++;
    };
    return it->second;
}

std::string QINIParser2::groupNode::getGroupName(size_t n) {
    if (n>=child.size()) parser->throwException("there are only "+inttostr(property.size())+" properties in this group node. You tried to access property no."+inttostr(n)+"!");;
    std::map<std::string, QINIParser2::groupNode*>::const_iterator it=child.begin();
    for (size_t i=0; i<n; i++) {
        it++;
    };
    return tolower(it->first);
}

std::string QINIParser2::groupNode::getGroupName(QINIParser2::groupNode* group) {
    std::map<std::string, QINIParser2::groupNode*>::const_iterator it=child.begin();
    for (it=child.begin(); it!=child.end(); ++it) {
        if (it->second==group) return it->first;
    }
    parser->throwException("didn't find the spüecified group");
    return "";
}

std::string QINIParser2::groupNode::getPropertyName(size_t n) {
    if (n>=property.size()) parser->throwException("there are only "+inttostr(property.size())+" properties in this group node. You tried to access property no."+inttostr(n)+"!");
    std::map<std::string, QINIParser2::jkip2Property>::const_iterator it=property.begin();
    for (size_t i=0; i<n; i++) {
        it++;
    };
    return tolower(it->first);
}

QINIParser2::jkip2Property QINIParser2::groupNode::get(std::string propertyName) {
  std::string p=tolower(strstrip(propertyName));
  size_t found=p.find(".");
  if (found==std::string::npos) { // this is a property name without groups
      if (property.find(p)!=property.end()) {
          return property[p];
      } else {
          parser->throwException("didn't find property named '"+p+"'");
      }

  } else {
      std::string group=p.substr(0, found);
      std::string rest=p.substr(found+1);
      if (child.find(group)==child.end()) {
          parser->throwException("didn't find property named '"+p+"'");
      }
      return child[group]->get(rest);
  }
  QINIParser2::jkip2Property pr;
  return pr;
}

bool QINIParser2::groupNode::getAsBool(std::string propertyName, bool defaultValue) {
    try {
        return get(propertyName).toBool(defaultValue);
    } catch(std::exception& E) {
        return defaultValue;
    }
}

int64_t QINIParser2::groupNode::getAsInt(std::string propertyName, int64_t defaultValue) {
    try {
        return get(propertyName).toInt(defaultValue);
    } catch(std::exception& E) {
        return defaultValue;
    }
}

double QINIParser2::groupNode::getAsDouble(std::string propertyName, double defaultValue) {
    try {
        return get(propertyName).toDouble(defaultValue);
    } catch(std::exception& E) {
        return defaultValue;
    }
}

std::string QINIParser2::groupNode::getAsString(std::string propertyName, std::string defaultValue) {
    try {
        return get(propertyName).toString();
    } catch(std::exception& E) {
        return defaultValue;
    }
}




bool QINIParser2::groupNode::getSetAsBool(std::string propertyName, bool defaultValue) {
    bool r;
    try {
        r=get(propertyName).toBool(defaultValue);
    } catch(std::exception& E) {
        r=defaultValue;
    }
    setProperty(propertyName, r);
    return r;
}

int64_t QINIParser2::groupNode::getSetAsInt(std::string propertyName, int64_t defaultValue) {
    int64_t r;
    try {
        r= get(propertyName).toInt(defaultValue);
    } catch(std::exception& E) {
        r= defaultValue;
    }
    setProperty(propertyName, r);
    return r;
}

double QINIParser2::groupNode::getSetAsDouble(std::string propertyName, double defaultValue) {
    double r;
    try {
        r= get(propertyName).toDouble(defaultValue);
    } catch(std::exception& E) {
        r= defaultValue;
    }
    setProperty(propertyName, r);
    return r;
}

std::string QINIParser2::groupNode::getSetAsString(std::string propertyName, std::string defaultValue) {
    std::string r;
    try {
        r= get(propertyName).toString();
    } catch(std::exception& E) {
        r= defaultValue;
    }
    setProperty(propertyName, r);
    return r;
}
















//QINIParser2

// class constructor
QINIParser2::QINIParser2()
{
  jkiniparser_exception_function=NULL;
  jkiCommentMarker='#';
  jkiMultilineMarker='\\';
  allowMultiLine=false;
  root=new QINIParser2::groupNode(NULL, this);
  currentRoot=root;
  clear();
}

// class constructor that automatically loads an ini-file
QINIParser2::QINIParser2(std::string filename, bool allowML)
{
  jkiniparser_exception_function=NULL;
  jkiCommentMarker='#';
  jkiMultilineMarker='\\';
  allowMultiLine = allowML;
  root=new QINIParser2::groupNode(NULL, this);
  currentRoot=root;
  readFile(filename);
}

// class destructor
QINIParser2::~QINIParser2()
{
	clear();
}

// deletes all saved configuration data
void QINIParser2::clear()
{
	root->clear();
	currentRoot=root;
	clearParserStack();
}


// sets group/field with data
void QINIParser2::set(std::string propertyName, int64_t data)
{
  QINIParser2::jkip2Property p;
  p.type=QINIParser2::jkip2Int;
  p.intData=data;
  currentRoot->setProperty(propertyName, p);
}

void QINIParser2::set(std::string propertyName, double data)
{
  QINIParser2::jkip2Property p;
  p.type=QINIParser2::jkip2Double;
  p.doubleData=data;
  currentRoot->setProperty(propertyName, p);
}

void QINIParser2::set(std::string propertyName, bool data)
{
  QINIParser2::jkip2Property p;
  p.type=QINIParser2::jkip2Bool;
  p.boolData=data;
  currentRoot->setProperty(propertyName, p);
}

void QINIParser2::set(std::string propertyName, std::string data)
{
  QINIParser2::jkip2Property p;
  p.type=QINIParser2::jkip2String;
  p.stringData=data;
  currentRoot->setProperty(propertyName, p);
}

std::string QINIParser2::getGroupName() {
    if (currentRoot==root) return "";

    std::string name="";
    QINIParser2::groupNode* np=currentRoot->getParent();
    QINIParser2::groupNode* nc=currentRoot;
    while (np!=NULL) {
        name=np->getGroupName(nc)+"."+name;
        nc=np;
        np=nc->getParent();
    }
    return name;
};

void QINIParser2::clearParserStack() {
    while (!pstateStack.empty()) {
        if (pstateStack.top().owns_mp) delete pstateStack.top().mp;
        if (pstateStack.top().configf) fclose(pstateStack.top().configf);
				pstateStack.top().configf=NULL;
        pstateStack.pop();
    }
}

// reads in an ini-file
void QINIParser2::readFile(std::string filename)
{
    clear();
    QINIParser2::ParserState s;
    s.configf=fopen(filename.c_str(), "r");
    s.mp=new jkMathParser();
    s.owns_mp=true;
    s.currentGroup="";
    pstateStack.push(s);
    if (s.configf!=NULL) {
        parse_file(true);
        //if (s.configf) fclose(s.configf);
				//s.configf=NULL;
    } else {
        throwException("could not open file '"+filename+"'");
    }
    clearParserStack();
}


// reads a new token from Config-File
QINIParser2::jkiTOKEN QINIParser2::getToken()
{
    FILE* configf=pstateStack.top().configf;
    std::string name="";

	char ch=0;
    ch=fgetc(configf); // read one character
	while((!feof(configf)) && isspace(ch)) { // swallow whitespaces
		ch=fgetc(configf);
	}
	if (feof(configf)) return jkiEOFile; // recognize end-of-file
	switch (ch) {
		case 0:
		case -1:
			return pstateStack.top().currentToken=jkiEND; // recognize end-of-input
		case 10:
		case 13:
			return pstateStack.top().currentToken=jkiPRINT; // linebreak
		case '[':
			return pstateStack.top().currentToken=jkiLBRACKET; // left group specifier bracket
		default:
            if (ch=='!') {
                ch=fgetc(configf);
                while((!feof(configf)) && !isspace(ch) && (ch!=10 && ch!=13)) { // read until the next linebreak or space
                    name=name+ch;
                    ch=fgetc(configf);
                }
                if (tolower(name)=="include") return pstateStack.top().currentToken=jkiINCLUDE;
                throwException("unknown token '!"+name+"'");
            }
            if (ch==jkiCommentMarker) return pstateStack.top().currentToken=jkiCOMMENT; // recognize comment
			if (isalpha(ch)) { // we found a name
                ungetc(ch,configf);
				pstateStack.top().currentField=parse_name('='); // read name until '=' is encountered

				return pstateStack.top().currentToken=jkiNAME;
			}
			// the parser has found an unknown token. an exception will be thrown
			throwException("unknown token '"+chartoprintablestr(ch)+"'");
	}
	return pstateStack.top().currentToken=jkiUNKNOWN;
}

// parses a group declaration of the form [groupname]
void QINIParser2::parse_groupdecl(bool get)
{
    pstateStack.top().currentGroup=parse_name(']');
    //cout<<"  + parsed groupdecl ["<<currentGroup<<"]"<<endl;
    currentRoot->ensureGroup(pstateStack.top().currentGroup);
    if (get) pstateStack.top().currentToken=getToken();
    //cout<<"  + return groupdecl ["<<currentGroup<<"]"<<endl;
}

// parses a fieldassignment of the form field = anyvalue
void QINIParser2::parse_fieldassignment(bool get)
{
    jkMathParser* mp=pstateStack.top().mp;
    std::string currentValue;
    if (allowMultiLine) {
        currentValue=parse_data_multiline(jkiCommentMarker);
    } else {
        currentValue=parse_data(jkiCommentMarker);
    }
    std::string name=pstateStack.top().currentField;
    std::string field_var=name;
    if (pstateStack.top().currentGroup!="") {
        name=pstateStack.top().currentGroup+"."+pstateStack.top().currentField;
    } else {
        size_t dpos=field_var.find('.');
        if (dpos!=string::npos) {
            field_var=field_var.substr(dpos+1, string::npos);
        }
    }
    field_var=std::string("_this.")+field_var;
    //std::cout<<"###   adding "<<name<<" & " <<field_var<<std::endl;
    jkMathParser::jkmpResult res;
    try {
        //std::cout<<"!!! evaluating <"<<currentValue<<">\n";
        res=mp->evaluate(currentValue);
        switch(res.type) {
            case jkMathParser::jkmpString:
                //std::cout<<"   !!! string\n";
                set(name, res.str);
                mp->addVariableString(name, res.str);
                mp->addVariableString(field_var, res.str);
                break;
            case jkMathParser::jkmpBool:
                //std::cout<<"   !!! bool\n";
                set(name, res.boolean);
                mp->addVariableBoolean(name, res.boolean);
                mp->addVariableBoolean(field_var, res.boolean);
                break;
            case jkMathParser::jkmpDouble:
                int64_t i=res.num;
                if ((double)i==res.num) {
                    //std::cout<<"   !!! int\n";
                    set(name, i);
                } else {
                    //std::cout<<"   !!! double\n";
                    set(name, res.num);
                }
                mp->addVariableDouble(name, res.num);
                mp->addVariableDouble(field_var, res.num);
                break;
        }
    } catch(jkMathParser::jkmpException E) {
       // std::cout<<"   !!! error: string '"<<currentValue<<"'\n             "<<E.what()<<"\n\n";
        set(name, currentValue);
        mp->addVariableString(name, currentValue);
        mp->addVariableString(field_var, currentValue);
    }


    if (get && (pstateStack.top().currentToken!=jkiCOMMENT)) pstateStack.top().currentToken=getToken();
}

// reads in all characters until end of line or file
std::string QINIParser2::parse_data(unsigned char commentchar)
{
    std::string result="";
    unsigned char ch=0;
    FILE* configf=pstateStack.top().configf;

    ch=fgetc(configf);
	while((!feof(configf)) ) {
        if (ch==commentchar) {
            pstateStack.top().currentToken=QINIParser2::jkiCOMMENT;
            return strstrip(result);
        }
        if ((ch==13)||(ch==10)) {
          pstateStack.top().currentToken=QINIParser2::jkiPRINT;
          return strstrip(result);
        }
        if ((result!="")|| ((result=="")&&(!isspace(ch)))) {
            result+=ch;
        }
        ch=fgetc(configf);
	}
	//cout<<"  + parsed data: "<<result<<endl;
  return strstrip(result);
}

std::string QINIParser2::parse_data_multiline(unsigned char commentchar) {
    FILE* configf=pstateStack.top().configf;
    std::string result="";
	unsigned char ch=0;

    ch=fgetc(configf);
	while((!feof(configf)) ) {
        if (ch==commentchar) {
            pstateStack.top().currentToken=QINIParser2::jkiCOMMENT;
            return strstrip(result);
        }
        if (ch=='\n') {
            if (!allowMultiLine) {
                pstateStack.top().currentToken=QINIParser2::jkiPRINT;
                return strstrip(result);
            } else {
                if (result[result.size()-1] != jkiMultilineMarker) {
                  pstateStack.top().currentToken=QINIParser2::jkiPRINT;
                  return strstrip(result);
                } else {
                  result.erase(result.size()-1);
                  ch=fgetc(configf);
                }
            }
        }
        if ((result!="")|| ((result=="")&&(!isspace(ch)))) {
            result+=ch;
        }
        ch=fgetc(configf);
	}
  //cout<<"  + parsed data: "<<result<<endl;
  return strstrip(result);
}

// reads in a name
std::string QINIParser2::parse_name(unsigned char endchar) {
    FILE* configf=pstateStack.top().configf;
    std::string result="";
    unsigned char ch=0;
    ch=fgetc(configf);
    while((!feof(configf)) &&(ch!=endchar)) {
        if (!isspace(ch)) {
            result+=ch;
        }
        ch=fgetc(configf);
    }
    //cout<<"  + parsed name: "<<result<<endl;
    return result;
}

void QINIParser2::parse_include(bool get) {
    if (pstateStack.size()>20) throwException("!include depth is restricted to 20 !include's.");

    std::string filename=parse_data(jkiCommentMarker);

    QINIParser2::ParserState s;
    s.configf=fopen(filename.c_str(), "r");
    s.mp=pstateStack.top().mp;// new jkMathParser();
    s.owns_mp=false;
    s.currentGroup="";
    pstateStack.push(s);
    if (s.configf!=NULL) {
        parse_file(true);
        if (s.configf) fclose(s.configf);
        s.configf=NULL;
    } else {
        throwException("could not open !include'd file '"+filename+"'");
    }
    if (pstateStack.top().owns_mp) delete pstateStack.top().mp;
    pstateStack.pop();

    if (get && (pstateStack.top().currentToken!=jkiCOMMENT)) pstateStack.top().currentToken=getToken();
}


// parses a config file
void QINIParser2::parse_file(bool get)
{
	if (get) pstateStack.top().currentToken=getToken();
	for(;;) {
        //cout<<"  + Token:  "<<currentToken<<endl;
        switch (pstateStack.top().currentToken) {
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
            case jkiINCLUDE:
                parse_include(true);
                break;
            default:
                pstateStack.top().currentToken=getToken();
        }
    }
}

// prints the config-data to screen
void QINIParser2::print()
{
    std::cout<<"* config **********************************************************\n";
    QINIParser2::groupNode* cn=root;
    for (size_t i=0; i<cn->getPropertyCount(); i++) {
        std::cout<<cn->getPropertyName(i)<<" = "<<cn->getProperty(i).toDelimString()<<"\n";
    }
    for (size_t i=0; i<cn->getGroupCount(); i++) {
        std::cout<<"\n";
        groupPrint(cn->getGroup(i), cn->getGroupName(i)+".");
    }

    std::cout<<"*******************************************************************\n";

}

void QINIParser2::groupPrint(QINIParser2::groupNode* n, std::string nameBase) {
    for (size_t i=0; i<n->getPropertyCount(); i++) {
       std::cout<<nameBase<<n->getPropertyName(i)<<" = "<<n->getProperty(i).toDelimString()<<"\n";
    }
    for (size_t i=0; i<n->getGroupCount(); i++) {
        std::cout<<"\n";
        groupPrint(n->getGroup(i), nameBase+n->getGroupName(i)+".");
    }

}


void QINIParser2::groupSave(FILE* f, QINIParser2::groupNode* n, std::string nameBase) {
    for (size_t i=0; i<n->getPropertyCount(); i++) {
        fprintf(f, "%s%s = %s\n", nameBase.c_str(), n->getPropertyName(i).c_str(), n->getProperty(i).toDelimString().c_str());
    }
    for (size_t i=0; i<n->getGroupCount(); i++) {
        fprintf(f, "\n");
        groupSave(f, n->getGroup(i), nameBase+n->getGroupName(i)+".");
    }

}












// writes configuration data to an ini-file
void QINIParser2::writeFile(std::string filename)
{
    FILE* f=fopen(filename.c_str(), "w");
    if (!ferror(f)) {

        QINIParser2::groupNode* cn=root;
        for (size_t i=0; i<cn->getPropertyCount(); i++) {
            fprintf(f, "%s = %s\n", cn->getPropertyName(i).c_str(), cn->getProperty(i).toDelimString().c_str());
        }
        for (size_t i=0; i<cn->getGroupCount(); i++) {
            fprintf(f, "\n");
            groupSave(f, cn->getGroup(i), cn->getGroupName(i)+".");
        }

        fclose(f);
    }
}













