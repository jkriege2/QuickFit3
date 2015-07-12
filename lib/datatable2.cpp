/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/



/*
  Name: datatable2.cpp
  Copyright: (c) 2007
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/


#define COMPILING_THIS_DATATABLE
#include "datatable2.h" // class's header file
#include <cstring>

datatable2_exception::datatable2_exception(int number, std::string message, std::string where)  throw(){
  this->message=message;
  this->number=number;
  this->where=where;
  //std::cout<<"DATATABLE2EXCEPTION:\n  "<<get_message()<<std::endl;
}

std::string datatable2_exception::get_message()  const{
  if (number==-1) return "error "+inttostr(number)+": "+message+ "(in "+where+")";
  return "error: "+message+ "[in "+where+"]";
}

// class constructor
datatable2::datatable2()
{
	resize_factor=1.2;
    set_csv_options();
    data=NULL;
	line_count=line_size=0;
	column_count=column_size=0;
	title_line="";
	resize(2,10);
	clear_titles();
}

datatable2::datatable2(unsigned long columns, unsigned long lines)
{
	resize_factor=1.2;
    set_csv_options();
    data=NULL;
	line_count=line_size=0;
	column_count=column_size=0;
	resize(columns, lines);
    clear_titles();
}

// class destructor
datatable2::~datatable2()
{
	free();
}

void datatable2::set_csv_options(bool include_line_number, std::string separator, char comma) {
    csv_include_line_number=include_line_number;
    csv_separator=separator;
    csv_comma=comma;
}

double datatable2::get(unsigned long x, unsigned long y)
{
	if ((x<column_count)&&(y<line_count)) {
      return data[x][y].value;
    } else {
      DATATABLE2_ERROR(DATATABLE2_ERROR_RANGE_NUM, DATATABLE2_ERROR_RANGE_MESSAGE, "datatable2.get("+inttostr(x)+", "+inttostr(y)+")");
    }
    return 0;
}

bool datatable2::is_empty(unsigned long x, unsigned long y)
{
	if ((x<column_count)&&(y<line_count)) {
      return data[x][y].empty;
    } else {
      DATATABLE2_ERROR(DATATABLE2_ERROR_RANGE_NUM, DATATABLE2_ERROR_RANGE_MESSAGE, "datatable2.is_empty("+inttostr(x)+", "+inttostr(y)+")");
    }
    return true;
}

void datatable2::set(unsigned long x, unsigned long y, double value)
{
	if ((x<column_count)&&(y<line_count)) {
      data[x][y].value=value;
      data[x][y].empty=false;
    } else {
      DATATABLE2_ERROR(DATATABLE2_ERROR_RANGE_NUM, DATATABLE2_ERROR_RANGE_MESSAGE, "datatable2.set("+inttostr(x)+", "+inttostr(y)+")");
    }
}

void datatable2::set_empty(unsigned long x, unsigned long y)
{
	if ((x<column_count)&&(y<line_count)) {
      data[x][y].value=0;
      data[x][y].empty=true;
    } else {
      DATATABLE2_ERROR(DATATABLE2_ERROR_RANGE_NUM, DATATABLE2_ERROR_RANGE_MESSAGE, "datatable2.set_empty("+inttostr(x)+", "+inttostr(y)+")");
    }

}

void datatable2::clear()
{
  // first empty the values
  if (column_count>0) for (unsigned long i=0; i<column_count; i++) {
    if (line_count>0) for (unsigned long j=0; j<line_count; j++) {
      data[i][j].value=0;
      data[i][j].empty=true;
    }
  }

  // hide the reserved memory
  line_count=0;
  column_count=0;

  resize_titles();
}

void datatable2::free()
{
  if (column_size>0) for (unsigned long i=0; i<column_size; i++) {
    std::free(data[i]);
  }
  std::free(data);
  clear_titles();

  // hide the reserved memory
  line_count=0;
  column_count=0;
  line_size=0;
  column_size=0;
  data=NULL;
}


unsigned long datatable2::get_line_count(){
    return line_count;
}

unsigned long datatable2::get_column_count(){
    return column_count;
}

void datatable2::resize(unsigned long newx, unsigned long newy){
    if (newy<=line_size) {
      line_count=newy;
    } else {
      if (column_size>0) for (unsigned long x=0; x<column_size; x++) {
          register void *value = realloc(data[x], (unsigned long)(newy*resize_factor)*sizeof(datatable2_value));
          if (value == NULL)
            DATATABLE2_ERROR(DATATABLE2_ERROR_VMEM_NUM, DATATABLE2_ERROR_VMEM_MESSAGE, "datatable2.resize("+inttostr(newx)+", "+inttostr(newy)+")");
          data[x]=(datatable2_value*)value;
          for (unsigned long y=line_count; y<(unsigned long)(newy*resize_factor); y++) {
            data[x][y].value=0;
            data[x][y].empty=true;
          }
      }
      line_size=(unsigned long)(newy*resize_factor);
      line_count=newy;
    }


    if (newx<=column_size) {
      column_count=newx;
    } else {
      if (column_size==0) {
          register void *value = malloc((unsigned long)(mmax(newx,1)*resize_factor)*sizeof(datatable2_value*)); // get a bit more (+20%) of memory for lines
          if (value == NULL)
            DATATABLE2_ERROR(DATATABLE2_ERROR_VMEM_NUM, DATATABLE2_ERROR_VMEM_MESSAGE, "datatable2.resize("+inttostr(newx)+", "+inttostr(newy)+")");
          data=(datatable2_value**)value;
      } else {
          register void *value = realloc(data, (unsigned long)(newx*resize_factor)*sizeof(datatable2_value*)); // get a bit more (+20%) of memory for lines
          if (value == NULL)
            DATATABLE2_ERROR(DATATABLE2_ERROR_VMEM_NUM, DATATABLE2_ERROR_VMEM_MESSAGE, "datatable2.resize("+inttostr(newx)+", "+inttostr(newy)+")");
          data=(datatable2_value**)value;
      }
      for (unsigned long i=column_size; i<(unsigned long)(newx*resize_factor); i++) {
          register void *value = malloc(line_size*sizeof(datatable2_value));
          if (value == NULL)
            DATATABLE2_ERROR(DATATABLE2_ERROR_VMEM_NUM, DATATABLE2_ERROR_VMEM_MESSAGE, "datatable2.resize("+inttostr(newx)+", "+inttostr(newy)+")");
          data[i]=(datatable2_value*)value;
          for (unsigned long y=0; y<line_size; y++) {
            data[i][y].value=0;
            data[i][y].empty=true;
          }
      }
      column_count=newx;
      column_size=(unsigned long)(newx*resize_factor);
    }
    resize_titles();
}



std::string datatable2::as_string(){
  char line[100];
  std::string fs="";
  //fs+= "accessible: "+inttostr(column_count)+" x "+inttostr(line_count)+"\n\n";
  fs+="titles:  ";
  for (unsigned long long y=0; y<column_count; y++) {
    if (y>0) fs+=", ";
    fs+=titles[y];
  }
  fs+="\n\n";
  for ( long long y=-3; y<=(long long)line_count; y++) {
      for ( long long x=-1; x<(long long)column_count; x++) {
        if (y==-2) {
          sprintf(line, "|%15ld", (long)x);
          fs+=(x==-1)?"+       ":line;
        } else {
            if ((y==-1)||(y==-3)) {
              fs+=(x==-1)?"+-------":"+---------------";
            } else {
              if (y==(long long)line_count) {
                fs+=(x==-1)?"+-------":"+---------------";
              } else {
                if (x==-1) {
                  sprintf(line, "|%7ld", (long)y);
                  fs+=line;
                } else {
                    if (data[x][y].empty) {
                      fs+="|             - ";
                    } else {
                      sprintf(line, "|%15.5lg", data[x][y].value);
                      fs+=line;
                    }
                }
              }
          }
        }
      }
      if ((y>=0)&&(y<(long long)line_count)) fs+="|\n"; else fs+="+\n";
  }
  return fs;
};

std::string datatable2::as_string_with_unaccessible(){
  char line[100];
  std::string fs="";
  //fs+= "accessible: "+inttostr(column_count)+" x "+inttostr(line_count)+"\n";
  //fs+= "all:        "+inttostr(column_size)+" x "+inttostr(line_size)+"\n\n";
  fs+="titles:  ";
  for (unsigned long long y=0; y<column_count; y++) {
    if (y>0) fs+=", ";
    fs+=titles[y];
  }
  fs+="\n\n";
  for ( long long y=-3; y<=(long long)line_size; y++) {
      for ( long long x=-1; x<(long long)column_size; x++) {
        if (y==-2) {
          sprintf(line, "|%15ld", (long)x);
          fs+=(x==-1)?"+       ":line;
        } else {
            if ((y==-1)||(y==-3)) {
              fs+=(x==-1)?"+-------":"+---------------";
            } else {
              if (y==(long long)line_size) {
                fs+=(x==-1)?"+-------":"+---------------";
              } else {
                if (x==-1) {
                  sprintf(line, "|%7ld", (long)y);
                  fs+=line;
                } else {
                    if (data[x][y].empty) {
                      fs+="|             - ";
                    } else {
                      sprintf(line, "|%15.5lg", data[x][y].value);
                      fs+=line;
                    }
                }
              }
            }
         }
      }
      if ((y>=0)&&(y<(long long)line_size)) fs+="|\n"; else fs+="+\n";
  }
  return fs;
};


void datatable2::save_sylk(std::string filename){
  FILE* f;
  f=fopen(filename.c_str(), "w");
  if (f==NULL) DATATABLE2_ERROR(DATATABLE2_ERROR_FILE_NUM, DATATABLE2_ERROR_FILE_MESSAGE, "datatable2.save_sylk("+filename+")");
  fprintf(f, "ID;Pdatatable2.h\n");
  fprintf(f, "B;X%ld;Y%ld\n", column_count, line_count);
  if (column_count>0)
    for (unsigned long x=0; x<column_count; x++) {
      if (line_count>0)
        for (unsigned long y=0; y<line_count; y++) {
          if (! data[x][y].empty) fprintf(f, "C;X%ld;Y%ld;K%lf\n", x+1,y+1,data[x][y].value);
        }
    }
  fprintf(f, "E\n");
  fclose(f);
}

void datatable2::save_formatted_txt(std::string filename){
  FILE* f;
  f=fopen(filename.c_str(), "w");
  if (f==NULL) DATATABLE2_ERROR(DATATABLE2_ERROR_FILE_NUM, DATATABLE2_ERROR_FILE_MESSAGE, "datatable2.save_sylk("+filename+")");
  fprintf(f, "%s\n", as_string().c_str());
  fclose(f);
}

void datatable2::save_csv(std::string filename){
  FILE* f;
  f=fopen(filename.c_str(), "w");
  if (f==NULL) DATATABLE2_ERROR(DATATABLE2_ERROR_FILE_NUM, DATATABLE2_ERROR_FILE_MESSAGE, "datatable2.save_sylk("+filename+")");
  if (column_count>0)
    for (unsigned long y=0; y<line_count; y++) {
      if (line_count>0) {
         for ( long long x=(csv_include_line_number)?-1:0; x<(long long)column_count; x++){
           if (x==-1) {
               fprintf(f, "%15ld", y);
           } else {
               if (!data[x][y].empty) {
                 char dat[128];
                 sprintf(dat, "%25.10lf", data[x][y].value);
                 // change decimal point
                 if (csv_comma!='.') {
                   if (strlen(dat)>0) for (size_t i=0; i<strlen(dat); i++) {
                     if (dat[i]=='.') dat[i]=csv_comma;
                   }
                 }
                 fprintf(f, "%s ", dat);
               } else fprintf(f, "                         ");
           }
           if (x==(long long)column_count-1) fprintf(f, "\n");
           else fprintf(f, csv_separator.c_str());
         }
      }
    }
  fprintf(f, "\n\n");
  fclose(f);
}

void datatable2::load_csv(std::string filename, char separatorchar, char commentchar, std::string startswith, std::string endswith, int firstline){
  //std::cout<<"datatable2::load_csv("<<filename<<")\n";
  //std::cout.flush();

  unsigned long lines=count_lines(filename);
  //std::cout<<"datatable2::load_csv("<<filename<<"): lines="<<lines<<"\n";
  //std::cout.flush();
  unsigned long line=0, col=0;
  // reserve enough lines for all lines in file (count lines in file)
  resize(1,lines-(firstline-1));
  clear(); // set accessible size to (0,0)
  //std::cout<<"lines: "<<inttostr(lines)<<std::endl;

  // now open the csv file
  csv_in_separator=separatorchar;
  csv_in_comment=commentchar;

  title_line="";
  csv_file=fopen(filename.c_str(), "rb");
  if (csv_file==NULL) DATATABLE2_ERROR(DATATABLE2_ERROR_FILE_NUM, DATATABLE2_ERROR_FILE_MESSAGE, "datatable2.load_csv("+filename+")");
  setvbuf(csv_file, NULL, _IOFBF ,1024*1024);
  col=0;
  line=0;
  // now parse the file
  double val;
  bool readthiscol=false;
  bool valueset=false;
  std::string linest="";
  //std::cout<<"startswith: '"<<startswith<<"'"<<std::endl;


  if (firstline>1) {
      int l=1;
      while ((!feof(csv_file))&&(l<firstline)) {
          /*char text[4096];
          fgets(text, 4096 , csv_file);
          //std::cout<<"ignore: '"<<text<<"'"<<std::endl;
          linest=text;*/
          char ch=0;
          while ((!feof(csv_file)) && (ch!='\n') && (ch!='\r')) {
              ch=fgetc(csv_file);
              char ch1=fgetc(csv_file);
              if (!((ch=='\n' && ch1=='\r')||(ch=='\r' && ch1=='\n'))) {
                  fseek(csv_file, -1, SEEK_CUR);
                  // if this isn't a \n\r or \r\n linebreak, we read a valid character and have to step back one character in the file
              }

          }
          if ((ch=='\n') || (ch=='\r')) l++;
      }
  }
  if (startswith.size()>0) {
      while ((!feof(csv_file))&&(linest.find(startswith)==std::string::npos)) {
          /*char text[4096];
          fgets(text, 4096 , csv_file);
          //std::cout<<"ignore: '"<<text<<"'"<<std::endl;
          linest=text;*/
          char ch=0;
          linest="";
          while ((!feof(csv_file)) && (ch!='\n') && (ch!='\r')) {
              ch=fgetc(csv_file);
              linest=linest+ch;
          }
      }
  }

  csv_endswith=endswith;

  while(get_token()!=datatable2_EOF) {
      /*switch (current_token) {
        case datatable2_COMMENT:
          //std::cout<<"datatable2_COMMENT\n";
          break;
        case datatable2_TITLECOMMENT: // the title comment line will be saved in title_line by get_token(), so it can be processed after parsing
          //std::cout<<"datatable2_TITLECOMMENT\n";
          break;
        case datatable2_LINEBREAK:
          //std::cout<<"datatable2_LINEBREAK\n";
          break;
        case datatable2_VALUE:
          //std::cout<<"datatable2_VALUE\n";
          break;
        case datatable2_SEPARATOR:
          //std::cout<<"datatable2_SEPARATOR\n";
          break;
        default: break;
      }*/

    switch (current_token) {
      case datatable2_COMMENT:
      case datatable2_TITLECOMMENT: // the title comment line will be saved in title_line by get_token(), so it can be processed after parsing
      case datatable2_LINEBREAK:
        if (readthiscol) line++;
        col=0;
        readthiscol=false;
        valueset=false;
        break;
      case datatable2_VALUE:
        val=0;
        readthiscol=true;
        // replace ',' by '.' to beeing able to read both decimal separators
        //std::cout<<"  '"<<current_value_string<<"'   ";
        if (current_value_string.size()>0)
          for (size_t i=0; i<current_value_string.size(); i++) {
            if (current_value_string[i]==',') current_value_string[i]='.';
          }
        val=atof(current_value_string.c_str());
        //if (line<100) std::cout<<"(c="<<col<<", r="<<line<<")  <=  '"<<current_value_string<<"'   "<<val<<"\n";
        resize(mmax(column_count,col+1), mmax(line_count,line+1));
        data[col][line].value= val;
        data[col][line].empty= false;
        valueset=true;

        // for space/tab separated files there will be no datatable2_SEPARATOR token!!! so we directly enter the next column
        if (isspace(csv_in_separator) || csv_in_separator=='0') {
            resize(mmax(column_count,col+1), mmax(line_count,line+1));
            valueset=false;
            col++;
        }
        break;
      case datatable2_SEPARATOR:
        readthiscol=true;
        if (!valueset){
          data[col][line].value= 0;
          data[col][line].empty= true;
        }
        col++;
        //resize(mmax(column_count,col+1), mmax(line_count,line+1));
        valueset=false;
        break;
      default: break;
    }
  }
  fclose(csv_file);
  clear_titles();
  // now parse title_line:
  if (title_line.size()>0) {
    int title=0;
    for (size_t i=0; i<title_line.size(); i++) {
      if (title_line[i]==csv_in_separator) {
        titles[title]=strstrip(titles[title]);
        title++;
      } else {
        if (((titles[title].size()==0)&& (title_line[i]>32))||
             ((titles[title].size()>0)&&(title_line[i]!='\n')&&(title_line[i]!='\r')))
        titles[title]+=title_line[i];
      }
      if (title>=(int)column_count) break;
    }
  }
  //std::cout<<"'"<<title_line<<"'"<<std::endl;
}

// reads a new token from Config-File the token will be saved in current_token and returned from the function
datatable2::datatable2_TOKEN datatable2::get_token()
{
   unsigned char ch;
   ch=fgetc(csv_file);
        while((!feof(csv_file)) && ((ch<=32) && (ch!='\n') && (ch!='\r'))) {
		ch=fgetc(csv_file);
	}
	if (feof(csv_file)||(ch==255)) return current_token=datatable2_EOF;
        if ((ch=='\n')||(ch=='\r')||(ch==0x0D)) return current_token=datatable2_LINEBREAK;
        if (ch==csv_in_separator) return current_token=datatable2_SEPARATOR;
	if (ch==csv_in_comment){
            if ((!feof(csv_file)) && (ch!='\n')) ch=fgetc(csv_file);
            if (ch=='$') { // title comment
              title_line="";
              // eat up all characters until end of line
              while((!feof(csv_file)) && (ch!='\n') ) {
                ch=fgetc(csv_file);
                if (((title_line.size()==0) && (ch>32))||
                    ((title_line.size()>0)&&(ch!='\n')&&(ch!='\r'))) title_line+=ch;
              }
              return current_token=datatable2_TITLECOMMENT;
            } else { // standard comment
              current_value_string+=ch;
              // eat up all characters until end of line
              while((!feof(csv_file)) && (ch!='\n') ) {
                ch=fgetc(csv_file);
                current_value_string+=ch;
              }
			  return current_token=datatable2_COMMENT;
            }
    }
	if (isdigit(ch)||(ch=='-')||(ch=='+')||((ch=='.')&&(csv_in_separator!='.'))||((ch==',')&&(csv_in_separator!=','))||(ch=='e')||(ch=='E')) {
      current_value_string="";
      // read in the complete number in scientific notation
      while((!feof(csv_file)) && (isdigit(ch)||(ch=='-')||(ch=='+')||((ch=='.')&&(csv_in_separator!='.'))||((ch==',')&&(csv_in_separator!=','))||(ch=='e')||(ch=='E'))) {
        current_value_string+=ch;
        ch=fgetc(csv_file);
      }
      ungetc(ch, csv_file);
      return current_token=datatable2_VALUE;
    }

	// the parser has found an unknown token. an exception will be thrown
	std::string token;
	token+=ch;
    size_t i=0;
    if (csv_endswith.size()>0) {
        while ((!feof(csv_file)) && i<csv_endswith.size() && ch==csv_endswith[i]) {
            ch=fgetc(csv_file);
            token+=ch;
            i++;
        }

        if (feof(csv_file)) return current_token=datatable2_EOF;

        if (i==csv_endswith.size()) {
            ungetc(ch, csv_file);
             return current_token=datatable2_EOF;
        }
    }
	DATATABLE2_ERROR(DATATABLE2_ERROR_UTOKEN_NUM, DATATABLE2_ERROR_UTOKEN_MESSAGE, "datatable2.getToken(), token was '"+token+"' ="+inttostr(ch));
	return current_token=datatable2_EOF;
}

unsigned long datatable2::count_lines(std::string filename) {
  unsigned long  l=0;
  FILE* f;
  char ch;
  f=fopen(filename.c_str(), "rb");
  if (f==NULL) DATATABLE2_ERROR(DATATABLE2_ERROR_FILE_NUM, DATATABLE2_ERROR_FILE_MESSAGE, "datatable2.count_lines("+filename+")");
  while(!feof(f)) {
    ch=fgetc(f);
    if (ch=='\n') l++;
  }
  if (l==0) {
      fseek(f, 0, SEEK_SET);
      while(!feof(f)) {
        ch=fgetc(f);
        if (ch=='\r') l++;
      }
  }
  fclose(f);
  return l;
}

std::string datatable2::get_title(unsigned long x){
  if (x>=column_count) DATATABLE2_ERROR(DATATABLE2_ERROR_TITLEINDEXRANGE_NUM, DATATABLE2_ERROR_TITLEINDEXRANGE_MESSAGE, "datatable2.get_title("+inttostr(x)+")");
  return titles[x];
};

void datatable2::set_title(unsigned long x, std::string name){
  if (x>=column_count) DATATABLE2_ERROR(DATATABLE2_ERROR_TITLEINDEXRANGE_NUM, DATATABLE2_ERROR_TITLEINDEXRANGE_MESSAGE, "datatable2.set_title("+inttostr(x)+", '"+name+"')");
  titles[x]=name;
};

void datatable2::clear_titles(){
  titles.clear();
  if (column_count>0) {
    for (unsigned long i=0; i<column_count; i++) {
      titles.push_back("");
    }
  }
};

unsigned long datatable2::get_column_by_title(std::string name){
  if (column_count>0) {
    for (unsigned long i=0; i<column_count; i++) {
      if (titles[i]==name) return i;
    }
  }
  DATATABLE2_ERROR(DATATABLE2_ERROR_UNKNOWNTITLE_NUM, DATATABLE2_ERROR_UNKNOWNTITLE_MESSAGE, "datatable2.get_column_by_title('"+name+"')");
  return 0;
};

bool datatable2::title_exists(std::string name){
  if (column_count>0) {
    for (unsigned long i=0; i<column_count; i++) {
      if (titles[i]==name) return true;
    }
  }
  return false;
};

void datatable2::resize_titles(){
  std::vector<std::string> t=titles;
  clear_titles();
  if (column_count>0) {
    for (unsigned long i=0; i<column_count; i++) {
      if (i<t.size()) titles[i]=t[i];
    }
  }
};

void datatable2::qswap(long long i, long long j) {
  //std::cout<<"swap("<<i<<", "<<j<<")\n";
  if (column_count>0)
    for (unsigned long x=0; x<column_count; x++) {
      datatable2_value t=data[x][i];
      data[x][i]=data[x][j];
      data[x][j]=t;
    }
}

bool datatable2::qless(datatable2_value a, datatable2_value b) {
  if (a.empty && b.empty) return true;
  if (a.empty && !b.empty) return true;
  if (!a.empty && b.empty) return false;
  return a.value<b.value;
}

bool datatable2::qlarger(datatable2_value a, datatable2_value b) {
  if (a.empty && b.empty) return true;
  if (a.empty && !b.empty) return false;
  if (!a.empty && b.empty) return true;
  return a.value>b.value;
}

void datatable2::sort(unsigned long bycolumn) {
  if ((column_count>0)&&(line_count>0)) {
    //qsort(0, get_line_count()-1, bycolumn);
    unsigned long n = line_count-1;
    bool swapped;
    do {
      swapped=false;
      for (unsigned long i=0; i<n; i++) {
        if (qless(data[bycolumn][i+1], data[bycolumn][i])) {
          qswap(i, i+1);
          swapped=true;
        }
      }
      n--;
    } while (swapped);

  }
}

double datatable2::calc_column_min(unsigned long column) {
  if ((column<column_count)) {
    if ((column_count>0)&&(line_count>0)) {
      double m=data[column][0].value;
      if (line_count>1) {
        for (unsigned long i=1; i<line_count; i++) {
          if (data[column][i].value<m) m=data[column][i].value;
        }
      }
      return m;
    } else return 0;
  } else DATATABLE2_ERROR(DATATABLE2_ERROR_RANGE_NUM, DATATABLE2_ERROR_RANGE_MESSAGE, "datatable2.column_min("+inttostr(column)+")");
  return 0;
}

double datatable2::calc_column_max(unsigned long column) {
  if ((column<column_count)) {
    if ((column_count>0)&&(line_count>0)) {
      double m=data[column][0].value;
      if (line_count>1) {
        for (unsigned long i=1; i<line_count; i++) {
          if (data[column][i].value>m) m=data[column][i].value;
        }
      }
      return m;
    }
  } else {
    DATATABLE2_ERROR(DATATABLE2_ERROR_RANGE_NUM, DATATABLE2_ERROR_RANGE_MESSAGE, "datatable2.column_max("+inttostr(column)+")");
  }
  return 0;
}

double datatable2::calc_column_average(unsigned long column) {
  int count=0;
  double result=0;
  if ((column<column_count)) {
    if ((column_count>0)&&(line_count>0)) {
      if (line_count>1) {
        for (unsigned long i=0; i<line_count; i++) {
          result=result+data[column][i].value;
          count++;
        }
      }
      return result/(double)count;
    }
  } else {
    DATATABLE2_ERROR(DATATABLE2_ERROR_RANGE_NUM, DATATABLE2_ERROR_RANGE_MESSAGE, "datatable2.column_average("+inttostr(column)+")");
  }
  return 0;
}
