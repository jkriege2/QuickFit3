/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

    last modification: $LastChangedDate$  (revision $Rev$)

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/



/** \file tools.cpp
 *  \ingroup tools
 */

#include "cpptools.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>


std::string inttostr(long data){
  return format("%ld", data);
};

std::string inttohex(long data){
  return format("%lX", data);
};

std::string uinttostr(unsigned long data){
  std::ostringstream ost;
  ost<<data;
  return ost.str();
};

std::string floattostr(double data, int past_comma, bool remove_trail0, double belowIsZero){
  if (belowIsZero>0) {
      if (fabs(data)<belowIsZero) return std::string("0");
  }

  std::string form="%."+inttostr(past_comma)+"lf";
  //std::string form="%lf";
  if (past_comma<=0) form="%lf";
  std::string r=format(form,data);
  //std::cout<<r<<std::endl;
  if (remove_trail0 && (tolower(r).find('e')==std::string::npos)) {
      if (data==0) return "0";
      //size_t cp=r.find(".");
      //if (cp<r.size()) return r;
      std::string re;
      size_t dpos=r.find('.');
      if (dpos==std::string::npos) {
          return r;
      } else {
          long i=r.size()-1;
          bool nonz=false;
          while (i>=0) {
              //std::cout<<i<<"\n";
              if (r[i]!='0') {
                  nonz=true;
              }
              if (nonz || (i<long(dpos))) {
                  if (re.size()==0 && r[i]=='.') {
                      // swallow decimal dot, if only 0 folowed
                  } else {
                      re=r[i]+re;
                  }
              }
              i--;
          }
          return re;
      }

  }
  return r;
}

std::string floattounitstr(double data, int past_comma, bool remove_trail0){
  if (data==0) return "0";
  std::string form="%."+inttostr(past_comma)+"lf";
  std::string res=format(form,data);
  if (fabs(data)>=1e3) res=format(form,data/1e3)+"k";
  if (fabs(data)>=1e6) res=format(form,data/1e6)+"M";
  if (fabs(data)>=1e9) res=format(form,data/1e9)+"G";
  if (fabs(data)>=1e12) res=format(form,data/1e12)+"T";
  if (fabs(data)>=1e15) res=format(form,data/1e15)+"P";
  if (fabs(data)<1) res=format(form,data/1e-3)+"m";
  if (fabs(data)<1e-3) res=format(form,data/1e-6)+"u";
  if (fabs(data)<1e-6) res=format(form,data/1e-9)+"n";
  if (fabs(data)<1e-9) res=format(form,data/1e-12)+"f";
  if (fabs(data)==0) res=format(form,data);
  if (remove_trail0) {
      if (data==0) return "0";
      if (res.find('.')==std::string::npos) return res;
      size_t i=res.size()-1;
      while (i>0 && res[i]=='0') {
          i--;
      }
      if (res[i]=='.') i--; // remove decimal divider
      return res.erase(i+1);
  }
  return res;
}

std::string floattolatexstr(double data, int past_comma, bool remove_trail0, double belowIsZero, double minNoExponent, double maxNoExponent){
  if ((belowIsZero>0) && (fabs(data)<belowIsZero)) return "\\rm{0}";
  if (data==0) return "\\rm{0}";
  double adata=fabs(data);
  std::string res=floattostr(data, past_comma, remove_trail0);
  /*std::string form="%."+inttostr(past_comma)+"lf";
  std::string res=format(form,data);
  std::string s="";
  if (data<0) s="-";*/

  long exp=(long)floor(log(adata)/log(10.0));
  //std::cout<<"data="<<data<<"   res="<<res<<"   exp="<<exp<<"   past_comma="<<past_comma<<std::endl;
  //if (exp==0 || exp==-1 || exp==1) return res;
  if ((minNoExponent<=fabs(data)) && (fabs(data)<=maxNoExponent)) return res;
  //if ((-past_comma<exp) && (exp<past_comma)) return res;

  //std::cout<<"adata="<<adata<<"   log(adata)/log(10)="<<log(adata)/log(10.0)<<"   exp="<<exp<<"   adata/pow(10, exp)="<<adata/pow(10.0, (double)exp)<<"\n";
  std::string v=floattostr(data/pow(10.0, (double)exp), past_comma, remove_trail0);
  //std::cout<<"floattolatexstr: v="<<v<<"   exp="<<exp<<std::endl;
  if (v!="1" && v!="10")  return v+std::string("{\\times}10^{")+inttostr(exp)+"}";
  if (v=="10") exp=exp+1;
  return std::string("10^{")+inttostr(exp)+"}";
}

std::string floattohtmlstr(double data, int past_comma, bool remove_trail0, double belowIsZero, double minNoExponent, double maxNoExponent){
  std::string result;
  if ((belowIsZero>0) && (fabs(data)<belowIsZero)) return "0";
  if (data==0) return "0";
  double adata=fabs(data);
  std::string res=floattostr(data, past_comma, remove_trail0);

  long exp=(long)floor(log(adata)/log(10.0));
  if ((minNoExponent<=fabs(data)) && (fabs(data)<maxNoExponent)) return res;
  //if ((-past_comma<exp) && (exp<past_comma)) result= res;
  else {
      std::string v=floattostr(data/pow(10.0, (double)exp), past_comma, remove_trail0);
      if (v!="1") result= v+std::string("&times;10<sup>")+inttostr(exp)+"</sup>";
      else result=std::string("10<sup>")+inttostr(exp)+"</sup>";
  }
  //std::cout<<"floattohtmlstr("<<data<<") = '"<<result<<"'\n";
  return result;
}

std::string bytestostr(double bytes){
    double data=bytes;
  std::string form="%.0lf";
  std::string res=format(form,data);
  form="%.3lf";
  if (fabs(data)>=1024.0) res=format(form,data/1024.0)+" k";
  if (fabs(data)>=1024.0*1024.0) res=format(form,data/(1024.0*1024.0))+" M";
  if (fabs(data)>=1024.0*1024.0*1024.0) res=format(form,data/(1024.0*1024.0*1024.0))+" ";
  if (fabs(data)>=1024.0*1024.0*1024.0*1024.0) res=format(form,data/(1024.0*1024.0*1024.0*1024.0))+" G";
  if (fabs(data)>=1024.0*1024.0*1024.0*1024.0*1024.0) res=format(form,data/(1024.0*1024.0*1024.0*1024.0*1024.0))+" T";
  if (fabs(data)>=1024.0*1024.0*1024.0*1024.0*1024.0*1024.0) res=format(form,data/(1024.0*1024.0*1024.0*1024.0*1024.0*1024.0))+" E";
  if (fabs(data)==0) res="0 ";
  return res+"Bytes";
}

double unitstringtofloat(std::string data){
  double dval=0;
  long long ival=0;
  int newpos;
  int r=read_number_ex(data, &dval, &ival, &newpos);
  if (r==2) dval=ival;
  return dval;
}

std::string floattostr_fmt(double data, std::string format){
  char st[512];
  sprintf(st, format.c_str(), data);
  return st;
}

std::string inttostr_fmt(long data, std::string format){
  char st[512];
  sprintf(st, format.c_str(), data);
  return st;
}

std::string uinttostr_fmt(unsigned long data, std::string format){
  char st[512];
  sprintf(st, format.c_str(), data);
  return st;
}

std::string chartostr(char data){
  std::ostringstream ost;
  ost<<data;
  return ost.str();
}

std::string chartoprintablestr(char data){
  std::ostringstream ost;
  switch(data) {
    case 0: ost<<"[NUL]"; break;
    case 1: ost<<"[SOH]"; break;
    case 2: ost<<"[STX]"; break;
    case 3: ost<<"[ETX]"; break;
    case 4: ost<<"[EOT]"; break;
    case 5: ost<<"[ENQ]"; break;
    case 6: ost<<"[ACK]"; break;
    case 7: ost<<"[BEL]"; break;
    case 8: ost<<"[BS]"; break;
    case 9: ost<<"[TAB]"; break;
    case 10: ost<<"[LF]"; break;
    case 11: ost<<"[VT]"; break;
    case 12: ost<<"[FF]"; break;
    case 13: ost<<"[CR]"; break;
    case 14: ost<<"[SO]"; break;
    case 15: ost<<"[SI]"; break;
    case 16: ost<<"[DLE]"; break;
    case 17: ost<<"[DC1]"; break;
    case 18: ost<<"[DC2]"; break;
    case 19: ost<<"[DC3]"; break;
    case 20: ost<<"[DC4]"; break;
    case 21: ost<<"[NAK]"; break;
    case 22: ost<<"[SYN]"; break;
    case 23: ost<<"[ETB]"; break;
    case 24: ost<<"[CAN]"; break;
    case 25: ost<<"[EM]"; break;
    case 26: ost<<"[SUB]"; break;
    case 27: ost<<"[ESC]"; break;
    case 28: ost<<"[FS]"; break;
    case 29: ost<<"[GS]"; break;
    case 30: ost<<"[RS]"; break;
    case 31: ost<<"[US]"; break;
    case 32: ost<<"[SP]"; break;
    case 127: ost<<"[DEL]"; break;
//    case : ost<<""; break;
    default:
      ost<<data;
  }
  return ost.str();
}

std::string toprintablestr(std::string data) {
    std::string ret="";
    if (data.size()<=0) return "";
    for (size_t i=0; i<data.size(); i++) {
        ret+=chartoprintablestr(data[i]);
    }
    return ret;
}

std::string booltostr(bool data){
  if (data) return "true";
  return "false";
}

bool strtobool(std::string data){
    std::string d=tolower(data);
  if (d=="true") return true;
  if (d=="t") return true;
  if (d=="1") return true;
  if (d=="j") return true;
  if (d=="y") return true;
  if (d=="yes") return true;
  if (d=="ja") return true;
  return false;
}

std::string tolower(std::string s){
  std::string d;
  d="";
  if (s.length()>0) {
    for (unsigned long i=0; i<s.length(); i++) {
        d+=tolower(s[i]);
    }
  }
  return d;
};

std::string toupper(std::string s){
  std::string d;
  d="";
  if (s.length()>0) {
    for (unsigned long i=0; i<s.length(); i++) {
        d+=toupper(s[i]);
    }
  }
  return d;
};

std::string doublearraytostr(double* data, long width, long height, bool linenumber) {
    char line[1024];
    std::string fs="";
    int xstart=-1;
    if (!linenumber) xstart=0;
    if ((height>0)&&(width>0)) {
        for (long int y=-1; y<=height; y++) {
          for (long int x=xstart; x<width; x++) {

              if ((y==-1)||(y==height)) {
                fs+=(x==-1)?"+-------":"+---------------";
              } else {
                if (x==-1) {
                  sprintf(line, "|%7ld", y);
                  fs+=line;
                } else {
                  sprintf(line, "|%15.5lg", data[y*width+x]);
                  fs+=line;
                }
            }
          }
          if ((y>=0)&&(y<height)) fs+="|\n"; else fs+="+\n";
        }
    }
    return fs;
}

std::string uint8vectostr(uint8_t* data, unsigned long height, bool showdec, bool showhex, bool showbits, bool linenumber) {
  char line[1024];
  std::string fs="";
  int xstart=-1;
  if (!linenumber) xstart=0;
  if ((height>0))
    for (long int y=-1; y<=int64_t(height); y++) {
      if ((y==-1)||(y==int64_t(height))) {
        if (linenumber) fs+="+-------";
        if (showdec) fs+="+-----";
        if (showhex) fs+="+----";
        if (showbits) fs+="+----------";
      } else {
        if (linenumber) {
          sprintf(line, "|%7ld", y);
          fs+=line;
        }
        if (showdec) {
          sprintf(line, "| %4d", (uint8_t)data[y]);
          fs+=line;
        }
        if (showhex) {
          sprintf(line, "| %3x", data[y]);
          fs+=line;
        }
        if (showbits) {
          fs+="| ";
          // convert to binary string
          for (char i=7; i>=0; i--) {
            fs+=bit_is_set(data[y], i)?"1":"0";
          }
          fs+=" ";
        }
      }
      if ((y>=-1)&&(y<int64_t(height))) fs+="|\n"; else fs+="+\n";
    }
  return fs;
}

std::string uint16vectostr(uint16_t* data, unsigned long height, bool showdec, bool showhex, bool showbits, bool linenumber) {
  char line[1024];
  std::string fs="";
  int xstart=-1;
  if (!linenumber) xstart=0;
  if ((height>0))
    for (long int y=-1; y<=int64_t(height); y++) {
      if ((y==-1)||(y==int64_t(height))) {
        if (linenumber) fs+="+-------";
        if (showdec) fs+="+-------";
        if (showhex) fs+="+------";
        if (showbits) fs+="+------------------";
      } else {
        if (linenumber) {
          sprintf(line, "|%7ld", y);
          fs+=line;
        }
        if (showdec) {
          sprintf(line, "| %6d", (uint16_t)data[y]);
          fs+=line;
        }
        if (showhex) {
          sprintf(line, "| %5x", data[y]);
          fs+=line;
        }
        if (showbits) {
          fs+="| ";
          // convert to binary string
          for (char i=15; i>=0; i--) {
            fs+=bit_is_set(data[y], i)?"1":"0";
          }
          fs+=" ";
        }
      }
      if ((y>=-1)&&(y<int64_t(height))) fs+="|\n"; else fs+="+\n";
    }
  return fs;
}


std::string intvectostr(int* data, unsigned long height, bool showdec, bool showhex, bool showbits, bool linenumber) {
  char line[1024];
  std::string fs="";
  int xstart=-1;
  if (!linenumber) xstart=0;
  if ((height>0))
    for (long int y=-1; y<=int64_t(height); y++) {
      if ((y==-1)||(y==int64_t(height))) {
        if (linenumber) fs+="+-------";
        if (showdec) fs+="+-------";
        if (showhex) fs+="+------";
        if (showbits) fs+="+------------------";
      } else {
        if (linenumber) {
          sprintf(line, "|%7ld", y);
          fs+=line;
        }
        if (showdec) {
          sprintf(line, "| %6d", data[y]);
          fs+=line;
        }
        if (showhex) {
          sprintf(line, "| %5x", data[y]);
          fs+=line;
        }
        if (showbits) {
          fs+="| ";
          // convert to binary string
          for (char i=15; i>=0; i--) {
            fs+=bit_is_set(data[y], i)?"1":"0";
          }
          fs+=" ";
        }
      }
      if ((y>=-1)&&(y<int64_t(height))) fs+="|\n"; else fs+="+\n";
    }
  return fs;
}

std::string uint32vectostr(uint32_t* data, unsigned long height, bool showdec, bool showhex, bool showbits, bool linenumber) {
  char line[1024];
  std::string fs="";
  int xstart=-1;
  if (!linenumber) xstart=0;
  if ((height>0))
    for (long int y=-1; y<=int64_t(height); y++) {
      if ((y==-1)||(y==int64_t(height))) {
        if (linenumber) fs+="+-------";
        if (showdec) fs+="+-------------";
        if (showhex) fs+="+----------";
        if (showbits) fs+="+----------------------------------";
      } else {
        if (linenumber) {
          sprintf(line, "|%7ld", y);
          fs+=line;
        }
        if (showdec) {
          sprintf(line, "| %12ld", (long int)(uint32_t)data[y]);
          fs+=line;
        }
        if (showhex) {
          sprintf(line, "| %9x", data[y]);
          fs+=line;
        }
        if (showbits) {
          fs+="| ";
          // convert to binary string
          for (char i=31; i>=0; i--) {
            fs+=bit_is_set(data[y], i)?"1":"0";
          }
          fs+=" ";
        }
      }
      if ((y>=-1)&&(y<int64_t(height))) fs+="|\n"; else fs+="+\n";
    }
  return fs;
}

bool isInt(std::string s){
  if (s.length()<=0) return false;
  for (size_t i=0; i<s.length(); i++) {
    switch((char)s[i]) {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9': break;
      case '+':
      case '-':
        if (i==0) break;
      default:
        return false;

    };
  }
  return true;
}

bool isFloat(std::string s){
  bool mantissa=true;
  bool dot=false;
  size_t mpos=0;
  if (s.length()<=0) return false;
  for (size_t i=0; i<s.length(); i++) {
    if (!mantissa) switch((char)s[i]) {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9': break;
      case '+':
      case '-':
        if (i==mpos) break;
      default:
        return false;

    };
    if (mantissa) switch((char)s[i]) {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9': break;
      case '.':
        if (dot) return false;
        dot=true;
        break;
      case '+':
      case '-':
        if (i==0) break;
      case 'e':
      case 'E':
        mantissa=false;
        mpos=i+1;
        break;
      default:
        return false;

    };
  }
  return true;
}

std::string format(std::string templ, ...){
  va_list ap;
  char buffer[4096];
  va_start (ap, templ);
  vsprintf (buffer, templ.c_str(), ap);
  va_end (ap);
  std::string ret(buffer);
  return ret;
};

bool toBool(double val) {
  if (val==0) return false;
  return true;
}

void toStringVector(std::vector<std::string>& lst, std::string text, char separator, bool strip){
  lst.clear();
  std::string line="";
  if (text.size()>0) {
    for(size_t i=0; i<text.size(); i++) {
      if (text[i]==separator) {
        if (strip) line=strstrip(line);
        lst.push_back(line);
        line="";
      } else line+=text[i];
    }
    if (strip) line=strstrip(line);
    //std::cout<<(strip?"true":"false")<<"   "<<line<<std::endl;
    lst.push_back(line);
  }
}

std::string fromStringVector(std::vector<std::string>& lst, std::string separator) {
  std::string r="";
  if (lst.size()>0) {
    for (size_t i=0; i<lst.size(); i++) {
      if (i>0) r+=separator;
      r+=lst[i];
    }
  }
  return r;
};

bool onlySpace(std::string text) {
  if (text.size()>0) {
    for (size_t i=0; i<text.size(); i++)
      if (text[i]>32) return false;
  }
  return true;
}


//´`°§^2³µ€ÄÖÜäöüßáéíóúÁÉÍÓÚàèìòùÀÈÌÒÙâêîôûÂÊÎÔÛ)
std::string toUTF8(std::string text){
  std::string res="";
  if (text.size()>0) {
    for (size_t i=0; i<text.size(); i++)
      switch((char)text[i]) {
        case 'À': res+=0xC3; res+=0x80; break;
        case 'ä': res+=0xC3; res+=0xA4; break;
        case '€': res+=0xE2; res+=0x82; res+=0xAC; break;
/*        case 'ä': res+="ae"; break; // workaround so far!
        case 'ö': res+="oe"; break;
        case 'ü': res+="ue"; break;
        case 'ß': res+="ss"; break;
        case 'Ä': res+="AE"; break;
        case 'Ö': res+="OE"; break;
        case 'Ü': res+="UE"; break;
        case '€': res+="EURO"; break;
        case '\xB5': res+="u"; break;*/
        default:
          res+=text[i];
          break;
      };
  }
  return res;
}

std::string strstrip(std::string text) {
  std::string ret=text;
  if (ret.size()>0) {
    unsigned long i=0;
    while(ret[i]<=32 ) {
      i++;
    }
    unsigned long j=ret.size()-1;
    while(ret[j]<=32) {
      j--;
    }
    return ret.substr(i, j-i+1);
  }
  return ret;
}

std::string removeDelimiters(std::string data) {
  std::string ret=data;
  if (ret.size()>0) {
    unsigned long i=0;
    char delim='\0';
    while(data[i]<=32 ) {
      i++;
    }
    delim=data[i];
    unsigned long j=i+1;
    bool delimFound=false;
    int bcount=0;
    while(!delimFound && i<data.size()) {
      i++;
      if (data[i]==delim) {
          delimFound=true;
          if (bcount%2==1) {
              delimFound=false;
          }
          bcount=0;
      } else if (data[i]=='\\') {
          bcount++;
      } else {
          bcount=0;
      }

    }
    return data.substr(j, i-j);
  }
  return ret;
}


std::string removeDelimitersPascal(std::string data) {
  std::string ret=data;
  if (ret.size()>0) {
    unsigned long i=0;
    char delim='\0';
    while(data[i]<=32 ) {
      i++;
    }
    delim=data[i];
    //unsigned long j=i+1;
    bool delimFound=false;
    std::string result;
    while(!delimFound && i<data.size()) {
      i++;
      if (data[i]==delim) {
          delimFound=true;
          if (i<data.size()-1) {
              if (data[i+1]==delim) {
                  delimFound=false;
                  result=result+delim;
              }
          }
      } else {
          result=result+data[i];
      }
    }
    return result;//data.substr(j, i-j);
  }
  return ret;
}

std::string pascalifyString(std::string data, char delim) {
    std::string result="";
    for (size_t i=0; i<data.size(); i++) {
        if (data[i]==delim) {
            result=result+delim+delim;
        } else {
            result=result+data[i];
        }
    }
    return result;
}

long extract_right_int(std::string text){
  int cnt=0;
  int pos=text.size()-1;
  if (text.size()>0) {
    while (isdigit(text[pos]) && pos>=0){
      pos--;
      cnt++;
    }
  }
  return atol(text.substr(text.size()-cnt, cnt).c_str());
}


bool isRightInt(std::string text){
  int cnt=0;
  int pos=text.size()-1;
  if (text.size()>0) {
    while (isdigit(text[pos]) && pos>=0){
      pos--;
      cnt++;
    }
  }
  if (cnt>0) return true;;
  return false;
}

std::string escapify(std::string text){
  std::string res="";
  if (text.size()>0) {
    for (size_t i=0; i<text.size(); i++)
      switch((char)text[i]) {
        case '\0': res+="\\0"; break;
        case '\n': res+="\\n"; break;
        case '\r': res+="\\r"; break;
        case '\t': res+="\\t"; break;
        case '\\': res+="\\\\"; break;
        case '"': res+="\\\""; break;
        case '\'': res+="\\'"; break;
        case '\a': res+="\\a"; break;
        case '\b': res+="\\b"; break;
        case '\v': res+="\\v"; break;
        case '\f': res+="\\f"; break;
        case '\e': res+="\\e"; break;
        case '\?': res+="\\?"; break;
        default:
          if ((unsigned char)text[i]<32) {
            res+="\\x"+uinttostr_fmt((unsigned char)text[i], "%02lX");
          } else res+=text[i];
          break;
      };
  }
  return res;
}

std::string escapify(const char c){
  switch(c) {
    case '\0': return "\\0"; break;
    case '\n': return "\\n"; break;
    case '\r': return "\\r"; break;
    case '\t': return "\\t"; break;
    case '\\': return "\\\\"; break;
    case '"': return "\\\""; break;
    case '\'': return "\\'"; break;
    case '\a': return "\\a"; break;
    case '\b': return "\\b"; break;
    case '\v': return "\\v"; break;
    case '\f': return "\\f"; break;
    case '\e': return "\\e"; break;
    case '?': return "\\?"; break;
  };
  if (c<32) {
    return "\\x"+uinttostr_fmt(c, "%02lX");
  } else {
    return chartostr(c);
  }
}

std::string deescapify(std::string text){
  std::string res="";
  if (text.size()>0) {
    unsigned int i=0;
    while (i<text.size()) {
      if (text[i]!='\\') {
        res+=text[i];
      } else {
        if (i+1<text.size()) {
          char next=text[i+1];
          switch(next) {
            case '0': res+='\0'; i++; break;
            case 'n': res+='\n'; i++; break;
            case 'r': res+='\r'; i++; break;
            case 't': res+='\t'; i++; break;
            case 'a': res+='\a'; i++; break;
            case 'b': res+='\b'; i++; break;
            case 'v': res+='\v'; i++; break;
            case 'f': res+='\f'; i++; break;
            case 'e': res+='\e'; i++; break;
            case '\\': res+='\\'; i++; break;
            case '"': res+='\"'; i++; break;
            case '?': res+='\?'; i++; break;
            case '\'': res+='\''; i++; break;
            case 'x':
            case 'X':
              if (i+3<text.size()) {
                std::string num=text.substr(i+2,2);
                i+=3;
                res+=(char)strtol(num.c_str(), NULL, 16);
              } else i++;
              break;
          }
        }
      }
      i++;
    }
  }
  return res;
}

std::string datetimetostr(time_t t){
  char nowstr[100];
  const struct tm *nowstruct;
  nowstruct = localtime(&t);

  if (strftime(nowstr, 100, "%x %X", nowstruct) == (size_t) 0)
      return "";
  return std::string(nowstr);

}

std::string extract_file_path(std::string filename) {
  size_t p=filename.find_last_of(PATHSEPARATOR_STRING);
  if (p==std::string::npos) { // if there is not separator: return nothing. The filname
                              // only consists a filename, no path
    return "";
  } else {
    return include_trailing_backslash(filename.substr(0, p+1));
  }
}


std::string extract_file_name(std::string filename) {
  size_t p=filename.find_last_of(PATHSEPARATOR_STRING);
  if (p==std::string::npos) { // if there is not separator: return the complete string,
                              // <filename> only consists of a filename
    return filename;
  } else {
    return filename.substr(p+1);
  }
}


std::string extract_file_ext(std::string filename) {
  size_t p=filename.find_last_of(".");
  if (p==std::string::npos) { // if there is not separator: there is no file extension
    return "";
  } else {
    return filename.substr(p+1);
  }
}


std::string change_file_ext(std::string filename, std::string ext) {
  size_t p=filename.find_last_of(".");
  std::string ext1=ext;
  if (ext.size()>0)
    if (ext[0]!='.') ext1="."+ext;
  if (p==std::string::npos) { // if there is not separator: there is no file extension
    return filename+ext1;
  } else {
    return filename.substr(0,p)+ext1;
  }
}


std::string include_trailing_backslash(std::string filename) {
  std::string ret=filename;
  if (filename.size()>0) {
    if (filename[filename.size()-1]!=PATHSEPARATOR_CHAR) ret+=PATHSEPARATOR_STRING;
  }
  return ret;
}


std::string exclude_trailing_backslash(std::string filename) {
  std::string ret=filename;
  if (filename.size()>0) {
    if (filename[filename.size()-1]==PATHSEPARATOR_CHAR) ret.erase(filename.size()-1,1);
  }
  return ret;
}

int read_number_ex(std::string s, double* dv, long long* iv, int *newpos) {
  *dv=0;
  *iv=0;

  bool isInt=true;
  bool isMantissa=true;
  long long factor=1;
  double dfactor=1;
  int mantissaPos=0;
 //bool isUnit=false;
  bool isNumber=true;
  bool foundDot=false;

  // strip of whitespace
  *newpos=0;
  if (s.length()<=0) return -1;
  while (isspace(s[0])) {
    s.erase(0,1);
    (*newpos)++;
  }

  std::string num="";

  int i=0;

  // check sign
  if (s[i]=='-') { factor=-1; dfactor=-1; i++; (*newpos)++; }
  else if (s[i]=='+') { i++; (*newpos)++; }

  while (isNumber) {
    if (!isMantissa) {
      switch(s[i]) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
          num+=s[i];
          break;
        case '-':
          if (i==mantissaPos) {
            num+=s[i];
          } else isNumber=false;
          break;
        case '+':
          if (i!=mantissaPos) isNumber=false;
          break;
        case 'a':
          dfactor*=1e-15;
          isInt=false;
          isNumber=false;
          (*newpos)++;
          break;
        case 'f':
          dfactor*=1e-12;
          isInt=false;
          isNumber=false;
          (*newpos)++;
          break;
        case 'n':
          dfactor*=1e-9;
          isInt=false;
          isNumber=false;
          (*newpos)++;
          break;
        case 'u':
          dfactor*=1e-6;
          isInt=false;
          isNumber=false;
          (*newpos)++;
          break;
        case 'm':
          dfactor*=1e-3;
          isInt=false;
          (*newpos)++;
          isNumber=false;
          break;
        case 'c':
          dfactor*=1e-2;
          isInt=false;
          (*newpos)++;
          isNumber=false;
          break;
        case 'd':
          dfactor*=1e-1;
          isInt=false;
          (*newpos)++;
          isNumber=false;
          break;
        case 'k':
          factor*=1000;
          dfactor*=1e3;
          (*newpos)++;
          isNumber=false;
          break;
        case 'M':
          factor*=1000000;
          dfactor*=1e6;
          (*newpos)++;
          isNumber=false;
          break;
        case 'G':
          factor*=1000000000;
          dfactor*=1e9;
          (*newpos)++;
          isNumber=false;
          break;
        case 'T':
          factor*=1000000000;
          factor*=1000;
          dfactor*=1e12;
          (*newpos)++;
          isNumber=false;
          break;
        case 'P':
          factor*=1000000000;
          factor*=1000;
          factor*=1000;
          dfactor*=1e15;
          (*newpos)++;
          isNumber=false;
          break;
        default:
          isNumber=false;
      }
    }
    if (isMantissa) {
      switch(s[i]) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
          num+=s[i];
          break;
        case '.':
          if (foundDot) {
            isNumber=false;
          } else {
            num+=s[i];
            isInt=false;
            foundDot=true;
          }
          break;
        case '-':
          if (i==mantissaPos) {
            factor=-1;
          } else isNumber=false;
          break;
        case '+':
          if (i==mantissaPos) {
            factor=1;
          } else isNumber=false;
          break;
        case 'e':
        case 'E':
          isMantissa=false;
          isInt=false;
          num+='e';
          mantissaPos=i+1;
          break;
        case 'a':
          dfactor*=1e-15;
          isInt=false;
          isNumber=false;
          (*newpos)++;
          break;
        case 'f':
          dfactor*=1e-12;
          isInt=false;
          isNumber=false;
          (*newpos)++;
          break;
        case 'n':
          dfactor*=1e-9;
          isInt=false;
          isNumber=false;
          (*newpos)++;
          break;
        case 'u':
          dfactor*=1e-6;
          isInt=false;
          isNumber=false;
          (*newpos)++;
          break;
        case 'm':
          dfactor*=1e-3;
          isInt=false;
          isNumber=false;
          (*newpos)++;
          break;
        case 'c':
          dfactor*=1e-2;
          isInt=false;
          isNumber=false;
          (*newpos)++;
          break;
        case 'd':
          dfactor*=1e-1;
          isInt=false;
          isNumber=false;
          (*newpos)++;
          break;
        case 'k':
          factor*=1000;
          dfactor*=1e3;
          isNumber=false;
          (*newpos)++;
          break;
        case 'M':
          factor*=1000000;
          dfactor*=1e6;
          isNumber=false;
          (*newpos)++;
          break;
        case 'G':
          factor*=1000000000;
          dfactor*=1e9;
          isNumber=false;
          (*newpos)++;
          break;
        case 'T':
          factor*=1000000000;
          factor*=1000;
          dfactor*=1e12;
          isNumber=false;
          (*newpos)++;
          break;
        case 'P':
          factor*=1000000000;
          factor*=1000;
          factor*=1000;
          dfactor*=1e15;
          isNumber=false;
          (*newpos)++;
          break;
        default:
          isNumber=false;
      }
    }
    i++;
    if (isNumber) (*newpos)++;
  }

  if (num.length()<=0) return -1; // this is no number
  if (isInt) {
    *iv=atoll(num.c_str());
    *iv = factor*(*iv);
    return 2; // this is an integer
  } else {
    *dv = atof(num.c_str());
    *dv=(*dv)*dfactor;
    return 1;
  }
}

bool file_exists (std::string fileName) {
   struct stat buf;
   int i = stat ( fileName.c_str(), &buf );
   /* File found */
   if ( i == 0 )
   {
     return true;
   }
   return false;
};

std::string floattounitstr(double dataa, std::string unitname){
  if (dataa==0) return floattostr(dataa)+unitname;
  std::string u="";
  double factor=1;
  double data=fabs(dataa);
  if (data>=1e3) { u="k"; factor=1e3; }
  if (data>=1e6) { u="M"; factor=1e6; }
  if (data>=1e9) { u="G"; factor=1e9; }
  if (data>=1e12) { u="T"; factor=1e12; }
  if (data>=1e15) { u="P"; factor=1e15; }
  if (data>=1e18) { u="E"; factor=1e18; }
  if (data<1) {u="m"; factor=1e-3; }
  if (data<1e-3) {u="u"; factor=1e-6; }
  if (data<1e-6) {u="n"; factor=1e-9; }
  if (data<1e-9) {u="p"; factor=1e-12; }
  if (data<1e-12) {u="f"; factor=1e-15; }
  if (data<1e-15) {u="a"; factor=1e-18; }

  return floattostr(dataa/factor)+u+unitname;
};

std::string get_full_filename(std::string filename){
  #ifndef __WINDOWS__
    #warning("get_full_filename(std::string filename) was not implemented for non-MS Windows OS!!!");
    return filename;
  #else
  char buffer[8192];
  long retval = GetFullPathNameA(filename.c_str(), 8192, buffer, NULL);
  if (retval>0)
    return buffer;
  else
    return "";
  #endif
}




#ifdef rgb
#  undef rgb
#endif
#define rgb(r,g,b) (0xff000000 | (r << 16) |  (g << 8) | b)

static const struct RGBData {
    const char *name;
    unsigned int  value;
} rgbTbl[] = {
    { "aliceblue", rgb(240, 248, 255) },
    { "antiquewhite", rgb(250, 235, 215) },
    { "aqua", rgb( 0, 255, 255) },
    { "aquamarine", rgb(127, 255, 212) },
    { "azure", rgb(240, 255, 255) },
    { "beige", rgb(245, 245, 220) },
    { "bisque", rgb(255, 228, 196) },
    { "black", rgb( 0, 0, 0) },
    { "blanchedalmond", rgb(255, 235, 205) },
    { "blue", rgb( 0, 0, 255) },
    { "blueviolet", rgb(138, 43, 226) },
    { "brown", rgb(165, 42, 42) },
    { "burlywood", rgb(222, 184, 135) },
    { "cadetblue", rgb( 95, 158, 160) },
    { "chartreuse", rgb(127, 255, 0) },
    { "chocolate", rgb(210, 105, 30) },
    { "coral", rgb(255, 127, 80) },
    { "cornflowerblue", rgb(100, 149, 237) },
    { "cornsilk", rgb(255, 248, 220) },
    { "crimson", rgb(220, 20, 60) },
    { "cyan", rgb( 0, 255, 255) },
    { "darkblue", rgb( 0, 0, 139) },
    { "darkcyan", rgb( 0, 139, 139) },
    { "darkgoldenrod", rgb(184, 134, 11) },
    { "darkgray", rgb(169, 169, 169) },
    { "darkgreen", rgb( 0, 100, 0) },
    { "darkgrey", rgb(169, 169, 169) },
    { "darkkhaki", rgb(189, 183, 107) },
    { "darkmagenta", rgb(139, 0, 139) },
    { "darkolivegreen", rgb( 85, 107, 47) },
    { "darkorange", rgb(255, 140, 0) },
    { "darkorchid", rgb(153, 50, 204) },
    { "darkred", rgb(139, 0, 0) },
    { "darksalmon", rgb(233, 150, 122) },
    { "darkseagreen", rgb(143, 188, 143) },
    { "darkslateblue", rgb( 72, 61, 139) },
    { "darkslategray", rgb( 47, 79, 79) },
    { "darkslategrey", rgb( 47, 79, 79) },
    { "darkturquoise", rgb( 0, 206, 209) },
    { "darkviolet", rgb(148, 0, 211) },
    { "deeppink", rgb(255, 20, 147) },
    { "deepskyblue", rgb( 0, 191, 255) },
    { "dimgray", rgb(105, 105, 105) },
    { "dimgrey", rgb(105, 105, 105) },
    { "dodgerblue", rgb( 30, 144, 255) },
    { "firebrick", rgb(178, 34, 34) },
    { "floralwhite", rgb(255, 250, 240) },
    { "forestgreen", rgb( 34, 139, 34) },
    { "fuchsia", rgb(255, 0, 255) },
    { "gainsboro", rgb(220, 220, 220) },
    { "ghostwhite", rgb(248, 248, 255) },
    { "gold", rgb(255, 215, 0) },
    { "goldenrod", rgb(218, 165, 32) },
    { "gray", rgb(128, 128, 128) },
    { "green", rgb( 0, 128, 0) },
    { "greenyellow", rgb(173, 255, 47) },
    { "grey", rgb(128, 128, 128) },
    { "honeydew", rgb(240, 255, 240) },
    { "hotpink", rgb(255, 105, 180) },
    { "indianred", rgb(205, 92, 92) },
    { "indigo", rgb( 75, 0, 130) },
    { "ivory", rgb(255, 255, 240) },
    { "khaki", rgb(240, 230, 140) },
    { "lavender", rgb(230, 230, 250) },
    { "lavenderblush", rgb(255, 240, 245) },
    { "lawngreen", rgb(124, 252, 0) },
    { "lemonchiffon", rgb(255, 250, 205) },
    { "lightblue", rgb(173, 216, 230) },
    { "lightcoral", rgb(240, 128, 128) },
    { "lightcyan", rgb(224, 255, 255) },
    { "lightgoldenrodyellow", rgb(250, 250, 210) },
    { "lightgray", rgb(211, 211, 211) },
    { "lightgreen", rgb(144, 238, 144) },
    { "lightgrey", rgb(211, 211, 211) },
    { "lightpink", rgb(255, 182, 193) },
    { "lightsalmon", rgb(255, 160, 122) },
    { "lightseagreen", rgb( 32, 178, 170) },
    { "lightskyblue", rgb(135, 206, 250) },
    { "lightslategray", rgb(119, 136, 153) },
    { "lightslategrey", rgb(119, 136, 153) },
    { "lightsteelblue", rgb(176, 196, 222) },
    { "lightyellow", rgb(255, 255, 224) },
    { "lime", rgb( 0, 255, 0) },
    { "limegreen", rgb( 50, 205, 50) },
    { "linen", rgb(250, 240, 230) },
    { "magenta", rgb(255, 0, 255) },
    { "maroon", rgb(128, 0, 0) },
    { "mediumaquamarine", rgb(102, 205, 170) },
    { "mediumblue", rgb( 0, 0, 205) },
    { "mediumorchid", rgb(186, 85, 211) },
    { "mediumpurple", rgb(147, 112, 219) },
    { "mediumseagreen", rgb( 60, 179, 113) },
    { "mediumslateblue", rgb(123, 104, 238) },
    { "mediumspringgreen", rgb( 0, 250, 154) },
    { "mediumturquoise", rgb( 72, 209, 204) },
    { "mediumvioletred", rgb(199, 21, 133) },
    { "midnightblue", rgb( 25, 25, 112) },
    { "mintcream", rgb(245, 255, 250) },
    { "mistyrose", rgb(255, 228, 225) },
    { "moccasin", rgb(255, 228, 181) },
    { "navajowhite", rgb(255, 222, 173) },
    { "navy", rgb( 0, 0, 128) },
    { "oldlace", rgb(253, 245, 230) },
    { "olive", rgb(128, 128, 0) },
    { "olivedrab", rgb(107, 142, 35) },
    { "orange", rgb(255, 165, 0) },
    { "orangered", rgb(255, 69, 0) },
    { "orchid", rgb(218, 112, 214) },
    { "palegoldenrod", rgb(238, 232, 170) },
    { "palegreen", rgb(152, 251, 152) },
    { "paleturquoise", rgb(175, 238, 238) },
    { "palevioletred", rgb(219, 112, 147) },
    { "papayawhip", rgb(255, 239, 213) },
    { "peachpuff", rgb(255, 218, 185) },
    { "peru", rgb(205, 133, 63) },
    { "pink", rgb(255, 192, 203) },
    { "plum", rgb(221, 160, 221) },
    { "powderblue", rgb(176, 224, 230) },
    { "purple", rgb(128, 0, 128) },
    { "red", rgb(255, 0, 0) },
    { "rosybrown", rgb(188, 143, 143) },
    { "royalblue", rgb( 65, 105, 225) },
    { "saddlebrown", rgb(139, 69, 19) },
    { "salmon", rgb(250, 128, 114) },
    { "sandybrown", rgb(244, 164, 96) },
    { "seagreen", rgb( 46, 139, 87) },
    { "seashell", rgb(255, 245, 238) },
    { "sienna", rgb(160, 82, 45) },
    { "silver", rgb(192, 192, 192) },
    { "skyblue", rgb(135, 206, 235) },
    { "slateblue", rgb(106, 90, 205) },
    { "slategray", rgb(112, 128, 144) },
    { "slategrey", rgb(112, 128, 144) },
    { "snow", rgb(255, 250, 250) },
    { "springgreen", rgb( 0, 255, 127) },
    { "steelblue", rgb( 70, 130, 180) },
    { "tan", rgb(210, 180, 140) },
    { "teal", rgb( 0, 128, 128) },
    { "thistle", rgb(216, 191, 216) },
    { "tomato", rgb(255, 99, 71) },
    { "transparent", 0 },
    { "turquoise", rgb( 64, 224, 208) },
    { "violet", rgb(238, 130, 238) },
    { "wheat", rgb(245, 222, 179) },
    { "white", rgb(255, 255, 255) },
    { "whitesmoke", rgb(245, 245, 245) },
    { "yellow", rgb(255, 255, 0) },
    { "yellowgreen", rgb(154, 205, 50) }
};

static const int rgbTblSize = sizeof(rgbTbl) / sizeof(RGBData);

std::string rgbtostring(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    if (a==255) {// only for non-transparent colors
        for (int i=0; i<rgbTblSize; i++) {
            if (rgb(r,g,b)==rgbTbl[i].value) {
                return rgbTbl[i].name;
            }
        }
        return format("#%02X%02X%02X", (int)r, (int)g, (int)b);
    }
    // if we reach this, we have an unnamed transparent color
    return format("#%02X%02X%02X%02X", (int)r, (int)g, (int)b, (int)a);
}

std::string doublevectortostr(double* data, unsigned int N) {
    std::string s="( ";
    for (unsigned int i=0; i<N; i++) {
        if (i>0) s+=", ";
        s+=floattostr(data[i]);
    }
    return s+" )";
}

std::string intvectortostr(int* data, unsigned int N) {
    std::string s="( ";
    for (unsigned int i=0; i<N; i++) {
        if (i>0) s+=", ";
        s+=inttostr(data[i]);
    }
    return s+" )";
}

void copy_file(std::string in, std::string out) {
    FILE* fin=fopen(in.c_str(), "r");
    FILE* fout=fopen(out.c_str(), "w");
    register char ch=fgetc(fin);
    while(!feof(fin)) {
        fputc(ch, fout);
        ch=fgetc(fin);
    }

    fclose(fin);
    fclose(fout);
}

void mk_all_dir(std::string directories) {
    std::string current="";
    size_t i=0;
    while (i<directories.size()) {
        current=current+directories[i];
        if (directories[i]==PATHSEPARATOR_CHAR) {
	#ifdef __WINDOWS__
            mkdir(current.c_str());
	#else
            mkdir(current.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	#endif
            //std::cout<<"mkdir('"<<current<<"')\n";
        }
        i++;
    }
    #ifdef __WINDOWS__
        mkdir(directories.c_str());
    #else
        mkdir(directories.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    #endif
    //std::cout<<"mkdir('"<<directories<<"')\n";
}

bool match_wildcard(std::string data, std::string pattern) {
    if (pattern.size()<=0) return true;
    size_t j=0;
    bool done=false;
    size_t pi=0;
    //printf("i=%ld\n",i);
    while (j<data.size() && (!done) && pi<pattern.size()) {
        //printf("  j=%ld  pi=%ld\n",j,pi);
        switch(pattern[pi]) {
            case '*':
                if (pi+1==pattern.size()) {
                    // match, because we found * at the end of the pattern
                    return true;
                } else {
                    pi++;
                    //printf("    new pattern: '%s'\n", pattern.substr(pi, pattern.size()).c_str());

                    while (j<data.size()) {
                        //printf("      new search_string: '%s'\n", data.substr(j, data.size()).c_str());
                        if (match_wildcard(data.substr(j, data.size()), pattern.substr(pi, pattern.size()))) return true;
                        j++;
                    }
                }
                break;
            case '?':
                pi++;
                break;
            default:
                //printf("   comparing data[%ld]='%c' with pattern[%ld]='%c'\n", j, data[j], pi, pattern[pi]);
                if (data[j]!=pattern[pi]) {
                    done=true;
                } else {
                    pi++;
                }
                break;
        }
        j++;
    }
    if ((!done) && (j+1>=data.size()) && (pi+1>=pattern.size())) return true;
    return false;
};

bool contains_wildcard(std::string data, std::string pattern) {
    if (pattern.size()<=0) return true;
    for (size_t i=0; i<data.size(); i++) {
        size_t j=i;
        bool done=false;
        size_t pi=0;
        //printf("i=%ld\n",i);
        while (j<data.size() && (!done) && pi<pattern.size()) {
            //printf("  j=%ld  pi=%ld\n",j,pi);
            switch(pattern[pi]) {
                case '*':
                    if (pi+1==pattern.size()) {
                        // match, because we found * at the end of the pattern
                        return true;
                    } else {
                        pi++;
                        char next=pattern[pi]; // get next character in pattern
                        //printf("    next=%s\n", pattern.substr(pi,1).c_str());
                        // find occurence of next character in pattern
                        while (j<data.size() && data[j]!=next) {
                            j++;
                        }
                        // if we reached the end, the pattern didn't match, because we never
                        // found a character which matches <next>
                        if (j==data.size()) {
                            done=true;
                        } else {
                            pi++;
                        }
                    }
                    break;
                case '?':
                    pi++;
                    break;
                default:
                    if (data[j]!=pattern[pi]) {
                        done=true;
                    } else {
                        pi++;
                    }
                    break;
            }
            j++;
        }
        if ((!done) && (pi+1>=pattern.size())) return true;
    }
    return false;
};

std::vector<std::string> tokenize_string(std::string data, std::string separators) {
    std::vector<std::string> r;
    r.clear();
    if (separators.size()<=0) {
        r.push_back(data);
        return r;
    }
    std::string substr="";
    for (size_t i=0; i<data.size(); i++) {
        bool match=false;
        size_t j=0;
        while ((!match) && j<separators.size()) {
            if (data[i]==separators[j]) {
                match=true;
            }
            j++;
        }
        if (match) {
            if (substr.size()>0) r.push_back(substr);
            substr="";
        } else {
            substr=substr+data[i];
        }
    }
    if (substr.size()>0) r.push_back(substr);
    return r;
}

std::vector<std::string> listfiles_wildcard(std::string pattern) {
    std::vector<std::string> dirs=tokenize_string(pattern, PATHSEPARATOR_STRING);
    std::vector<std::string> r;
    r.clear();
    std::string dir=std::string(".")+PATHSEPARATOR_STRING;
#ifndef __WINDOWS__
    if (pattern.size()>0) { // the path is not relative, so it starts with a '/' to indicate the file system root
        if (pattern[0]=='/') dir="/";
        if (pattern[0]=='~') dir="~";
    }
#endif
#ifdef __WINDOWS__
    if (pattern.size()>1) // the path contains a drive letter
        if (pattern[1]==':') dir="";
#endif

    std::string file_pattern="*";
    if (dirs.size()>0) { // extract the part after the last / or \ in <pattern> as this is the file matching pattern
        file_pattern=dirs[dirs.size()-1];
    }
    int dircount=(int)dirs.size()-1; // number of directories specified in pattern
    if (dircount>0) { // put together the path
        for (size_t i=0; i<(size_t)dircount; i++) {
            if (dirs[i]!=".") dir=dir+dirs[i]+PATHSEPARATOR_STRING;
        }
    }

    //std::cout<<"dircount="<<dircount<<"   pattern='"<<pattern<<"'   dir='"<<dir<<"'   file_pattern='"<<file_pattern<<"'\n";

    DIR *dp;
    struct dirent *ep;
    struct stat buf;

    dp = opendir (dir.c_str()); // list the specified path
    if (dp != NULL) {
        while ((ep = readdir (dp))) {
          //printf("fn='%s'  wc='%s'\n", ep->d_name, file_pattern.c_str());
          stat(ep->d_name, &buf);
          //if (S_ISREG(buf.st_mode))  {
              if (match_wildcard(ep->d_name, file_pattern)) {
                  r.push_back(dir+ep->d_name);
                  //printf("  added '%s'\n", ep->d_name);
              }
          //}
          //if (S_ISDIR(buf.st_mode))  { puts("DIR: "); puts (ep->d_name); }
        }
        closedir(dp);
    }

    return r;
};

std::vector<double> csv_readline(FILE* f, char separator_char, char comment_char) {
    if (f==NULL) throw std::runtime_error("no valid file specified");
    std::vector<double> result;
    result.clear();
    bool done=false;
    std::string num="";
    bool isComment=false;
    //std::cout<<"SEP "<<chartoprintablestr(separator_char)<<"  COM "<<chartoprintablestr(comment_char)<<"\n";
    while ((!feof(f)) && (!done)) {
        char ch=fgetc(f);
        //std::cout<<"read "<<chartoprintablestr(ch)<<"  isc="<<booltostr(isComment)<<"\n";

        if (!isComment) {
            switch(ch) {
                case '\n':
                case '\r':
                    //std::cout<<"LF: num="<<num<<"\n";
                    if (result.size()>0) done=true;
                    if (num.size()>0) {
                        result.push_back(strtofloat(num.c_str()));
                    }
                    num="";
                    break;
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                case '.':
                case '-':
                case 'e': case 'E':
                    num=num+ch;
                    break;
                default:
                    if ((ch==separator_char)||(separator_char==' ' && isspace(ch))) {
                        //std::cout<<"SEP: num="<<num<<"\n";
                        if (num.size()>0) {
                            result.push_back(strtofloat(num.c_str()));
                        }
                        num="";
                    } else if (ch==comment_char) {
                        //std::cout<<"COM: num="<<num<<"\n";
                        isComment=true;
                        if (num.size()>0) {
                            result.push_back(strtofloat(num.c_str()));
                        }
                        num="";
                    } else {
                        //std::cout<<"NON: num="<<num<<"\n";
                    }
                    break;
            }
        } else {
            if ((ch=='\n') || (ch=='\r')) {
                if (result.size()>0) {
                    done=true;
                } else {
                    isComment=false;
                }
            }
        }
    }
    if ((!done)&& num.size()>0) {
        result.push_back(strtofloat(num.c_str()));
    }
    return result;
}

double strtofloat(std::string data) {
    char* p;
    double result=strtod(data.c_str(), &p);
    if (result==0 && p==data.c_str())  {
        throw std::runtime_error(format("could not convert '%s' to a double", data.c_str()));
    }
    std::string tail=p;
    if (tail.size()>0 && !onlySpace(tail)) {
        throw std::runtime_error(format("could not convert '%s' to a double", data.c_str()));
    }
    return result;
}


long get_filesize(char *FileName) {
    struct stat file;
    if(!stat(FileName,&file)) {
         return file.st_size;
    }
    return 0;
}


unsigned long long count_lines(std::string filename, char comment_char) {
    unsigned long long cnt=0;

    std::ifstream f;
    f.open(filename.c_str());
    if (!f.is_open()) {
        return 0;
    }

    std::string s;
    while(!f.eof()) {
        getline(f,s);
        if (comment_char==0) {
            cnt++;
        } else {
            if (s.size()>0) {
                for(size_t i=0; i<s.size(); i++) {
                    if (s[i]==comment_char) break;
                    else if (!isspace(s[i])) {
                        cnt++;
                        break;
                    }
                }
            }
        }
    }
    f.close();

    return cnt;
}

std::string string_replace(std::string input, std::string match, std::string replace) {
    std::string res=input;
    size_t p=std::string::npos;
    size_t l=match.size();
    size_t sp=0;
    while ((p=res.find(match, sp))!=std::string::npos) {
        res=res.replace(p, l, replace);
        sp=p+replace.size();
    }
    return res;
}

std::string extend_file_path(std::string filename, std::string addpath) {
    std::string path=extract_file_path(filename);
    std::string file=extract_file_name(filename);
    return include_trailing_backslash(include_trailing_backslash(path)+exclude_trailing_backslash(addpath))+file;
}

std::string get_currentworkingdirectory() {
    size_t size = 1024;
    std::string res="";
    bool ok=true;
    while (ok) {
        char *buffer = (char *)calloc(size, sizeof(char));
        if (getcwd(buffer, size) == buffer) {
            res=buffer;
            ok=false;
        }
        //printf("\n--%s--\n", buffer);
        free (buffer);
        if (errno != ERANGE) ok=false;
        size *= 2;
    }
    return res;
}

int execute_in(std::string command, std::string directory) {
    std::string cwd=get_currentworkingdirectory();
    chdir(directory.c_str());
    int r=system(command.c_str());
    chdir(cwd.c_str());
    return r;
}

std::string to_valid_variable_name(std::string input) {
    std::string out="";
    for (size_t i=0; i<input.size(); i++) {
        if (isalpha(input[i])) {
            out=out+input[i];
        }
        if ((isdigit(input[i]))&&(out.size()>0)) out=out+input[i];
        if ((input[i]=='_')&&(out.size()>0)) out=out+input[i];
    }
    return out;
}

std::string repeated_string(std::string input, int n) {
    if (n<=0) return "";
    std::string out="";
    for (int i=0; i<n; i++) out=out+input;
    return out;
}

std::string replace_to_system_pathseparator(std::string filename) {
    std::string fn;
    for (size_t i=0; i<filename.size(); i++) {
        if (filename[i]=='/'||filename[i]=='\\') fn=fn+PATHSEPARATOR_STRING;
        else fn=fn+filename[i];
    }
    return fn;
}

double getHighResolutionTime() {
#ifdef JKTOOLS_TIMER_USE_TIME
    #warning(getHighResolutionTime() uses time(0)*1e6))
    return time(0)*1e6;
#else
    #ifdef __LINUX__
        #warning(getHighResolutionTime() uses gettimeofday()))
        struct timeval tv;
        gettimeofday(&tv,0);

        long long t2;
        t2 = tv.tv_sec * 1000000 + tv.tv_usec;
        return t2;
    #else
        #ifdef __WINDOWS__
            #warning(getHighResolutionTime() uses QueryPerformanceFrequency()))
            LARGE_INTEGER fr;
            QueryPerformanceFrequency(&fr);
            double freq=(double)(fr.QuadPart);
            LARGE_INTEGER now;
            QueryPerformanceCounter(&now);
            return ((double)(now.QuadPart)/freq)*1e6;
        #else
            #warning(getHighResolutionTime() uses fallback time(0)*1e6))
            return time(0)*1e6;
        #endif
    #endif
#endif
}
