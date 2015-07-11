/*
  Name: tcpipinstructionparser.cpp
  Copyright: (c) 2007
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/

#include "tcpipinstructionparser.h" // class's header file

namespace sequencer {


std::string tcpipToken2String(tcpipToken currentToken){
    switch(currentToken) {
      case tcpipEND: return "tcpipEND"; break;
      case tcpipPRINT: return "tcpipPRINT"; break;
      case tcpipPARAMETER_DIV: return "tcpipPARAMETER_DIV"; break;
      case tcpipSTRING: return "tcpipSTRING"; break;
      case tcpipNUMBER:return "tcpipNUMBER"; break;
      case tcpipNAME: return "tcpipNAME"; break;
      case tcpipLBRACKET: return "tcpipLBRACKET"; break;
      case tcpipRBRACKET: return "tcpipRBRACKET"; break;
      case tcpipTRUE: return "tcpipTRUE"; break;
      case tcpipFALSE: return "tcpipFALSE"; break;
      case tcpipALL: return "tcpipALL"; break;
    }
}

TCPIPinstructionParser::TCPIPinstructionParser(BaseServer* s)
{
  server=s;
  timeout=1;
  pushBackChar=0;
  instruction.type=tcpipINone;
  results.clear();
}

TCPIPinstructionParser::~TCPIPinstructionParser()
{
}



tcpipToken TCPIPinstructionParser::getToken(){
	char ch=0;

  // this is a strange workaround, as it does nothing ... it only once accesses timeout.
  // without these lines of coud (BTW std::cout<<timeout<<std::endl; also worked) the
  // program will wiat forever ... this could be some optimization issue ... but it's really
  // strange !!!!!!!!!!!
  long ta=get_timeout();
  std::stringstream s;
  s<<ta<<std::endl;
  // workaround: end


  // read in one non-space character. If a timeout occurs we return a tcpipEND token
  try {
    server->set_timeout(get_timeout());
    if (pushBackChar!=0) { ch=pushBackChar; pushBackChar=0; } else { ch=server->read_char(connection); }
  } catch(exception e) {
    if (e.get_number()==SEQUENCER_NETERROR_TIMEOUT_NUM) {
			return CurrentToken=tcpipEND;
    } else throw e;
  }
	while(isspace(ch)) {
    try {
      if (pushBackChar!=0) { ch=pushBackChar; pushBackChar=0; } else { ch=server->read_char(connection); }
    } catch(exception e) {
      if (e.get_number()==SEQUENCER_NETERROR_TIMEOUT_NUM) {
  			return CurrentToken=tcpipEND;
      } else throw e;
    }
	}

	switch (ch) {
		case 0:
		case -1:
			return CurrentToken=tcpipEND;
		case ';':
			return CurrentToken=tcpipPRINT;
		case '(':
			return CurrentToken=tcpipLBRACKET;
		case ')':
			return CurrentToken=tcpipRBRACKET;
		case ',':
			return CurrentToken=tcpipPARAMETER_DIV;
		case '"':
		case '\'':
      stringValue=readDelim(ch);
			return CurrentToken=tcpipSTRING;
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
		case '.': case '+': case '-':
			readNumber(ch);
			return CurrentToken=tcpipNUMBER;
		default:
			if (isalpha(ch) || (ch=='_')) { // try to recognize NAME, TRUE, FALSE, LBRACKET, ALL
				readName(ch);

				if (stringValue=="true") return CurrentToken=tcpipTRUE;
				if (stringValue=="false") return CurrentToken=tcpipFALSE;
				if (stringValue=="all") return CurrentToken=tcpipALL;

				return CurrentToken=tcpipNAME;
      }
			break;
	}
  // the parser has found an unknown token. an exception will be thrown
  SEQUENCER_ERROR(SEQUENCER_NETERROR_UTOKEN_NUM, format(get_errormessage(SEQUENCER_NETERROR_UTOKEN_NUM), ch), "TCPIPinstructionParser::getToken()");
}



void TCPIPinstructionParser::parse(int con) {
  connection=con;
  instruction.type=tcpipINone;
  instruction.parameters.clear();
  getToken();
  if (CurrentToken != tcpipEND) {
    parse_instruction(false);
  } else  return;
  getToken();
  if ((CurrentToken != tcpipPRINT)&&(CurrentToken != tcpipEND)) {
    SEQUENCER_ERROR(SEQUENCER_NETERROR_TUNEXPECTED_NUM, format(get_errormessage(SEQUENCER_NETERROR_TUNEXPECTED_NUM), tcpipToken2String(CurrentToken).c_str(), tcpipToken2String(tcpipPRINT).c_str()), "TCPIPinstructionParser::parse()");
  }
}


void TCPIPinstructionParser::parse_instruction(bool get){
	if (get) getToken();
  //std::cout<<tcpipToken2String(CurrentToken)<<std::endl;
	switch(CurrentToken) {

		case tcpipNAME:
      instruction.type=tcpipIInstruction;
      instruction.name=stringValue; // set the instruction name and clear the parameters
      instruction.parameters.clear();
    	getToken(); // now we read the next Token ...
    	if (CurrentToken == tcpipLBRACKET) { // ... this has to be an opening bracket '('
         	while ((CurrentToken != tcpipRBRACKET)&&(CurrentToken!=tcpipEND)) {
            tcpip_parameter p;
      	    getToken();
            switch(CurrentToken) {
              case tcpipALL:
                p.type=tcpipPAll;
                instruction.parameters.push_back(p);
                break;
              case tcpipTRUE:
                p.type=tcpipPTrue;
                p.boolValue=true;
                instruction.parameters.push_back(p);
                break;
              case tcpipFALSE:
                p.type=tcpipPFalse;
                p.boolValue=false;
                instruction.parameters.push_back(p);
                break;
              case tcpipNUMBER:
                p.type=tcpipPInteger;
                p.intValue=intValue;
                if (numberType==tcpipDouble) {
                  p.type=tcpipPDouble;
                  p.doubleValue=doubleValue;
                }
                instruction.parameters.push_back(p);
                break;
              case tcpipSTRING:
                p.type=tcpipPString;
                p.stringValue=stringValue;
                instruction.parameters.push_back(p);
                break;
              case tcpipRBRACKET:
                break;
              default:
                SEQUENCER_ERROR(SEQUENCER_NETERROR_TUNEXPECTED_NUM, format(get_errormessage(SEQUENCER_NETERROR_TUNEXPECTED_NUM), tcpipToken2String(CurrentToken).c_str(), "a number, string, true/false, all or a closing bracket ')'"), "TCPIPinstructionParser::parse_instruction()");
            }
            if (CurrentToken != tcpipRBRACKET) getToken(); // now we have to find ',' or ')'
           	if ((CurrentToken!=tcpipRBRACKET)&&(CurrentToken!=tcpipPARAMETER_DIV)) {
           	   std::string err=" '(' ["+tcpipToken2String(tcpipRBRACKET)+"] or ',' ["+tcpipToken2String(tcpipPARAMETER_DIV)+"]";
               SEQUENCER_ERROR(SEQUENCER_NETERROR_TUNEXPECTED_NUM, format(get_errormessage(SEQUENCER_NETERROR_TUNEXPECTED_NUM), tcpipToken2String(CurrentToken).c_str(), err.c_str()), "TCPIPinstructionParser::parse_instruction()");
            }
         	}
  				if ( CurrentToken != tcpipRBRACKET ) SEQUENCER_ERROR(SEQUENCER_NETERROR_TUNEXPECTED_NUM, format(get_errormessage(SEQUENCER_NETERROR_TUNEXPECTED_NUM), tcpipToken2String(CurrentToken).c_str(), tcpipToken2String(tcpipRBRACKET).c_str()), "TCPIPinstructionParser::parse_instruction()");
    	    return; // we found a valid instruction an can return now

  		} else { // no opening bracket ... so we have an instruction without parameters
  		  //getToken();
  		  //if ( CurrentToken != tcpipPRINT ) SEQUENCER_ERROR(SEQUENCER_NETERROR_TUNEXPECTED_NUM, format(get_errormessage(SEQUENCER_NETERROR_TUNEXPECTED_NUM), tcpipToken2String(CurrentToken).c_str(), "a end of instruction ';'"), "TCPIPinstructionParser::parse_instruction()");
  		  return; // we can simply return, as the name is already known and the parameter_count is set to 0!
 			}
			break;


	}
  // the parser has found an unexpected token. an exception will be thrown
  SEQUENCER_ERROR(SEQUENCER_NETERROR_TUNEXPECTED_NUM, format(get_errormessage(SEQUENCER_NETERROR_TUNEXPECTED_NUM), tcpipToken2String(CurrentToken).c_str(), "an instruction name"), "TCPIPinstructionParser::parse_instruction()");
}


std::string TCPIPinstructionParser::readDelim(char delimiter){
	std::string res="";
	char ch;
  bool goon=true;

  server->set_timeout(get_timeout());
 	while(goon) {
    if (pushBackChar!=0) { ch=pushBackChar; pushBackChar=0; } else { ch=server->read_char(connection); }
		if (ch!=delimiter) res=res+ch;
		if (ch==delimiter) {
      if (res.size()>0) {
        if (res[res.size()-1]=='\\') {
          res[res.size()-1]=delimiter;
        } else goon=false;;
      } else goon=false;
    }
	}

	return res;
}

void TCPIPinstructionParser::readNumber(char first){
	std::string res="";
  res+=first;
  if (first=='-') res+="0";
  if (first=='+') res+="0";
	char ch;
  //bool goon=true;

  server->set_timeout(get_timeout());
 	while(!isspace(ch) && isFloat(strstrip(res))) {
    if (pushBackChar!=0) { ch=pushBackChar; pushBackChar=0; } else { ch=server->read_char(connection); }
    res+=ch;
    if (ch=='e' || ch=='E') {
      if (pushBackChar!=0) { ch=pushBackChar; pushBackChar=0; } else { ch=server->read_char(connection); }
      res+=ch;
      if (ch=='+' || ch=='-') {
        if (pushBackChar!=0) { ch=pushBackChar; pushBackChar=0; } else { ch=server->read_char(connection); }
        res+=ch;
      }
    }
    //std::cout<<res<<std::endl;
	}
	pushBackChar=res[res.size()-1];
	res=strstrip(res.erase(res.size()-1));
  //std::cout<<"convert: '"<<res<<"'"<<std::endl;
	if (isInt(res)) {
    intValue=atol(res.c_str());
    numberType=tcpipInteger;
  } else {
    doubleValue=atof(res.c_str());
    numberType=tcpipDouble;
  }
}

void TCPIPinstructionParser::readName(char first){
	std::string res="";
	char ch=first;
  //bool goon=true;

    server->set_timeout(get_timeout());
 	while(isalnum(ch) || ch=='_') {
    res+=ch;
    if (pushBackChar!=0) { ch=pushBackChar; pushBackChar=0; } else { ch=server->read_char(connection); }
	}
	pushBackChar=ch;
	stringValue=res;
}

void TCPIPinstructionParser::send_result(int con){
  /* All data to be sent is first stored in a buffer of growing size. This allows
   * to send all the data at once which makes data transfer much faster, as the
   * overhead per transfered byte is reduced.
   * The size of the buffer is initially 1k and will be doubled each time the procedure
   * runs out of buffer memory. The variable count always points to the next position
   * where to write data into buffer. The variable size stores the current siz of
   * the buffer.
   */
  char data0=4;
  char datan='\n';
  std::string result="";
  unsigned long long size=1024;
  unsigned long long count=0;
  char* buffer=(char*)calloc(size, sizeof(char));
  if (results.size()==0) {
    // there is no return parameter, so we simply have to send a 0x00 character:
    //server->write(con, &data0, 1);
        if (size-count<1) {
          buffer=(char*)realloc(buffer, 2*size);
          size*=2;
        }
        buffer[count]=datan;
        count+=1;
  } else {
    // there are some return parameters, so we have to send them:
    for(size_t i=0; i<results.size(); i++) {
      tcpip_result res=results[i];
      switch(res.type) {
        case tcpipRDouble:
//          server->write(con, floattostr(res.doubleValue));
          result=floattostr(res.doubleValue);
          if (size-count<result.size()) {
            buffer=(char*)realloc(buffer, mmax(2*size, size+result.size()));
            size*=2;
          }
          memcpy(&buffer[count], result.c_str(), result.size());
          count+=result.size();
          break;
        case tcpipRInteger:
//          server->write(con, inttostr(res.intValue));
          result=inttostr(res.intValue);
          if (size-count<result.size()) {
            buffer=(char*)realloc(buffer, mmax(2*size, size+result.size()));
            size*=2;
          }
          memcpy(&buffer[count], result.c_str(), result.size());
          count+=result.size();
          break;
        case tcpipRBoolean:
//          server->write(con, booltostr(res.boolValue));
          result=booltostr(res.boolValue);
          if (size-count<result.size()) {
            buffer=(char*)realloc(buffer, mmax(2*size, size+result.size()));
            size*=2;
          }
          memcpy(&buffer[count], result.c_str(), result.size());
          count+=result.size();
          break;
        case tcpipRString:
//          server->write(con, escapify(res.stringValue));
          result=escapify(res.stringValue);
          if (size-count<result.size()) {
            buffer=(char*)realloc(buffer, mmax(2*size, size+result.size()));
            size*=2;
          }
          memcpy(&buffer[count], result.c_str(), result.size());
          count+=result.size();
          break;
      }
      // if this is the last parameter we have to sen 0x00 otherwise we send a linebreak.
      if (i==results.size()-1) {
//        server->write(con, &datan, 1);
//        server->write(con, &data0, 1);
//        server->write(con, &datan, 1);
        if (size-count<3) {
          buffer=(char*)realloc(buffer, 2*size);
          size*=2;
        }
        buffer[count]=datan;
        buffer[count+1]=data0;
        buffer[count+2]=datan;
        count+=3;

      } else {
//        server->write(con, &datan, 1);
        if (size-count<1) {
          buffer=(char*)realloc(buffer, 2*size);
          size*=2;
        }
        buffer[count]=datan;
        count+=1;
      }
    }
  }
  if (count>0)
    server->write(con, buffer, count);
  free(buffer);
};


void TCPIPinstructionParser::clear_results(){
  results.clear();
};

void TCPIPinstructionParser::add_result_int(long long data){
   tcpip_result res;
   res.type=tcpipRInteger;
   res.intValue=data;
   results.push_back(res);
};

void TCPIPinstructionParser::add_result_double(double data){
   tcpip_result res;
   res.type=tcpipRDouble;
   res.doubleValue=data;
   results.push_back(res);
};

void TCPIPinstructionParser::add_result_bool(bool data){
   tcpip_result res;
   res.type=tcpipRBoolean;
   res.boolValue=data;
   results.push_back(res);
};

void TCPIPinstructionParser::add_result_string(std::string data){
   tcpip_result res;
   res.type=tcpipRString;
   res.stringValue=data;
   results.push_back(res);
};

void TCPIPinstructionParser::write_result_error(std::string message, int code){
  clear_results();
  add_result_int(code);
  add_result_string(message);
}

bool TCPIPinstructionParser::check_instruction_argcount(int count, LogFile* lf){
  if (read_instruction_argcount()!=count) {
    write_result_error(read_instruction_name()+": this function requires "+inttostr(count)+" arguments, but you provided "+inttostr(read_instruction_argcount()));
    if (lf!=NULL) {
      lf->inc_indent();
      lf->log_error("\nerror: function requires "+inttostr(count)+" arguments");
      lf->dec_indent();
    }
    return false;
  }
  return true;
}

bool TCPIPinstructionParser::check_instruction_argcount_ormore(int count, LogFile* lf){
  if (read_instruction_argcount()<count) {
    write_result_error(read_instruction_name()+": this function requires at least "+inttostr(count)+" arguments, but you provided "+inttostr(read_instruction_argcount()));
    if (lf!=NULL) {
      lf->inc_indent();
      lf->log_error("\nerror: function requires at least "+inttostr(count)+" arguments");
      lf->dec_indent();
    }
    return false;
  }
  return true;
}

bool TCPIPinstructionParser::check_instruction_argtype_bool(int arg, LogFile* lf){
  switch(instruction.parameters[arg].type) {
    case tcpipPTrue:
    case tcpipPFalse:
      return true;
    case tcpipPInteger: // an ineteger can alwas be converted to a boolean
      if ((instruction.parameters[arg].intValue==0) ||
          (instruction.parameters[arg].intValue==1))
        return true;
      break;
    case tcpipPString:
      if ((instruction.parameters[arg].stringValue=="true") ||
          (instruction.parameters[arg].stringValue=="false"))
        return true;
      break;
  }
  write_result_error(read_instruction_name()+": this function requires a boolean type argument "+inttostr(arg+1));
  if (lf!=NULL) {
    lf->inc_indent();
    lf->log_error("\nerror: function requires a boolean type argument "+inttostr(arg+1));
    lf->dec_indent();
  }
  return false;
}

bool TCPIPinstructionParser::check_instruction_argtype_string(int arg, LogFile* lf){
  switch(instruction.parameters[arg].type) {
    case tcpipPString:
      return true;
  }
  write_result_error(read_instruction_name()+": this function requires a string argument "+inttostr(arg+1));
  if (lf!=NULL) {
    lf->inc_indent();
    lf->log_error("\nerror: function requires a string argument "+inttostr(arg+1));
    lf->dec_indent();
  }
  return false;
}

bool TCPIPinstructionParser::check_instruction_argtype_int(int arg, LogFile* lf){
  switch(instruction.parameters[arg].type) {
    case tcpipPInteger:
      return true;
  }
  write_result_error(read_instruction_name()+": this function requires an integer argument "+inttostr(arg+1));
  if (lf!=NULL) {
    lf->inc_indent();
    lf->log_error("\nerror: function requires an integer argument "+inttostr(arg+1));
    lf->dec_indent();
  }
  return false;
}

bool TCPIPinstructionParser::check_instruction_argtype_intstring(int arg, LogFile* lf){
  switch(instruction.parameters[arg].type) {
    case tcpipPString:
    case tcpipPInteger:
      return true;
  }
  write_result_error(read_instruction_name()+": this function requires an integer or string argument "+inttostr(arg+1));
  if (lf!=NULL) {
    lf->inc_indent();
    lf->log_error("\nerror: function requires an integer or string argument "+inttostr(arg+1));
    lf->dec_indent();
  }
  return false;
}

bool TCPIPinstructionParser::check_instruction_argtype_double(int arg, LogFile* lf){
  switch(instruction.parameters[arg].type) {
    case tcpipPInteger:
    case tcpipPDouble:
      return true;
  }
  write_result_error(read_instruction_name()+": this function requires an integer argument "+inttostr(arg+1));
  if (lf!=NULL) {
    lf->inc_indent();
    lf->log_error("\nerror: function requires an integer argument "+inttostr(arg+1));
    lf->dec_indent();
  }
  return false;
}

double TCPIPinstructionParser::read_instruction_arg_double(int arg){
  switch(instruction.parameters[arg].type) {
    case tcpipPInteger:
      return instruction.parameters[arg].intValue;
    case tcpipPDouble:
      return instruction.parameters[arg].doubleValue;
  }
  return 0;
}

long long TCPIPinstructionParser::read_instruction_arg_int(int arg){
  switch(instruction.parameters[arg].type) {
    case tcpipPInteger:
      return instruction.parameters[arg].intValue;
  }
  return 0;
}

std::string TCPIPinstructionParser::read_instruction_arg_string(int arg){
  switch(instruction.parameters[arg].type) {
    case tcpipPString:
      return instruction.parameters[arg].stringValue;
  }
  return "";
}

bool TCPIPinstructionParser::read_instruction_arg_bool(int arg){
  switch(instruction.parameters[arg].type) {
    case tcpipPTrue:
      return true;
    case tcpipPFalse:
      return false;
    case tcpipPString:
      return (instruction.parameters[arg].stringValue=="true");
    case tcpipPInteger:
      return (instruction.parameters[arg].intValue!=0);
    case tcpipPDouble:
      return (instruction.parameters[arg].intValue!=0);
  }
  return false;
}

std::string TCPIPinstructionParser::instruction_as_string(){
    std::string res=instruction.name+"(";
    if (instruction.parameters.size()>0) for (size_t i=0; i<instruction.parameters.size(); i++) {
        if (i>0) res+=", ";
        switch (instruction.parameters[i].type) {
            case tcpipPDouble: res+=floattostr(instruction.parameters[i].doubleValue); break;
            case tcpipPInteger: res+=inttostr(instruction.parameters[i].intValue); break;
            case tcpipPString: res+="'"+toprintablestr(instruction.parameters[i].stringValue)+"'"; break;
            case tcpipPTrue: res+="true"; break;
            case tcpipPFalse: res+="false"; break;
            case tcpipPAll: res+="all"; break;
        }
    }
    return res+")";
}


}
