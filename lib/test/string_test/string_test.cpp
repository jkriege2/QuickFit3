#include <iostream>
#include "..\..\tools.h"

using namespace std;

int main()
{
    std::cout<<"inttostr-test:\n  "<<inttostr(-10)<<"   "<<inttostr(10)<<"   "<<inttostr(0)<<"   "<<inttostr(65535)<<"   "<<inttostr(-65535)<<"\n";
    std::cout<<"inttohex-test:\n  "<<inttohex(-10)<<"   "<<inttohex(10)<<"   "<<inttohex(0)<<"   "<<inttohex(65535)<<"   "<<inttohex(-65535)<<"\n";
    std::cout<<"floattostr-test:\n  "<<floattostr(M_PI)<<"   "<<floattostr(M_PI,2)<<"   "<<floattostr(M_PI,2,true)<<"   "<<floattostr(1.25000)<<"   "<<floattostr(1.25000,5,false)<<"   "<<floattostr(1.25000,5,true)<<"\n";
    std::cout<<"floattounitstr-test:\n  "<<floattounitstr(M_PI,2)<<"   "<<floattounitstr(M_PI*10,2)<<"   "<<floattounitstr(M_PI*100,2)<<"   "<<floattounitstr(M_PI,2)<<"   "<<floattounitstr(M_PI*1000,2)<<"   "<<floattounitstr(M_PI*10000,2)<<"   "<<floattounitstr(M_PI*100000,2)<<"   "<<floattounitstr(M_PI*1000,2)<<"   "<<floattounitstr(M_PI*1000000,2)<<"   "<<floattounitstr(M_PI*1000000000,2)<<"   "<<floattounitstr(M_PI/1000.0,2)<<"   "<<floattounitstr(M_PI/1000000.0,2)<<"   "<<floattounitstr(M_PI/1000000000.0,2)<<"\n";
    std::cout<<"floattounitstr-test:\n  "<<floattounitstr(M_PI,"m")<<"   "<<floattounitstr(M_PI*10,"m")<<"   "<<floattounitstr(M_PI*100,"m")<<"   "<<floattounitstr(M_PI,"m")<<"   "<<floattounitstr(M_PI*1000,"m")<<"   "<<floattounitstr(M_PI*10000,"m")<<"   "<<floattounitstr(M_PI*100000,"m")<<"   "<<floattounitstr(M_PI*1000,"m")<<"   "<<floattounitstr(M_PI*1000000,"m")<<"   "<<floattounitstr(M_PI*1000000000,"m")<<"   "<<floattounitstr(M_PI/1000.0,"m")<<"   "<<floattounitstr(M_PI/1000000.0,"m")<<"   "<<floattounitstr(M_PI/1000000000.0,"m")<<"\n";
    std::cout<<"floattolatexstr-test:\n  "<<floattolatexstr(M_PI,2)<<"   "<<floattolatexstr(M_PI*10,2)<<"   "<<floattolatexstr(M_PI*100,2)<<"   "<<floattolatexstr(M_PI,2)<<"   "<<floattolatexstr(M_PI*1000,2)<<"   "<<floattolatexstr(M_PI*10000,2)<<"   "<<floattolatexstr(M_PI*100000,2)<<"   "<<floattolatexstr(M_PI*1000,2)<<"   "<<floattolatexstr(M_PI*1000000,2)<<"   "<<floattolatexstr(M_PI*1000000000,2)<<"   "<<floattolatexstr(M_PI/1000.0,2)<<"   "<<floattolatexstr(M_PI/1000000.0,2)<<"   "<<floattolatexstr(M_PI/1000000000.0,2)<<"\n";
    std::cout<<"floattohtmlstr-test:\n  "<<floattohtmlstr(M_PI,2)<<"   "<<floattohtmlstr(M_PI*10,2)<<"   "<<floattohtmlstr(M_PI*100,2)<<"   "<<floattohtmlstr(M_PI,2)<<"   "<<floattohtmlstr(M_PI*1000,2)<<"   "<<floattohtmlstr(M_PI*10000,2)<<"   "<<floattohtmlstr(M_PI*100000,2)<<"   "<<floattohtmlstr(M_PI*1000,2)<<"   "<<floattohtmlstr(M_PI*1000000,2)<<"   "<<floattohtmlstr(M_PI*1000000000,2)<<"   "<<floattohtmlstr(M_PI/1000.0,2)<<"   "<<floattohtmlstr(M_PI/1000000.0,2)<<"   "<<floattohtmlstr(M_PI/1000000000.0,2)<<"\n";
    std::string test="ABCDE    \n\r\a"; test[6]=3; test[7]=5; test[8]=15;
    std::cout<<"toprintablestr-test:\n  "<<toprintablestr(test)<<"\n";
    std::cout<<"escapify-test:\n  "<<escapify(test)<<"\n";
    std::cout<<"deescapify-test:\n  "<<toprintablestr(deescapify(escapify(test)))<<"\n";
    std::cout<<"extract_right_int-test:\n  "<<extract_right_int("test123")<<" "<<extract_right_int("test123a")<<" "<<extract_right_int("123")<<" "<<extract_right_int("test")<<"\n";
    std::cout<<"datetimetostr-test:\n  "<<datetimetostr(time(0))<<"\n";

    std::cout<<"removeDelimiters-test:\n";
    std::string s;
    s="   'blabla\\''"; cout<<"   <"<<s<<">   <"<<removeDelimiters(s)<<">\n";
    s="\"blabla\\\"'   "; cout<<"   <"<<s<<">   <"<<removeDelimiters(s)<<">\n";
    s="\"blabla\\\"'   \""; cout<<"   <"<<s<<">   <"<<removeDelimiters(s)<<">\n";
    s=""; cout<<"   <"<<s<<">   <"<<removeDelimiters(s)<<">\n";
    s="\"\""; cout<<"   <"<<s<<">   <"<<removeDelimiters(s)<<">\n";
    s="\"a\""; cout<<"   <"<<s<<">   <"<<removeDelimiters(s)<<">\n";
    s="\"\\\"\""; cout<<"   <"<<s<<">   <"<<removeDelimiters(s)<<">\n";

    std::string str="- This is, a sample string. With man!words in it.";
    std::vector<std::string> t=tokenize_string(str, " ,-.");
    std::cout<<"tokenize '"<<str<<"':\n";
    for (size_t i=0; i<t.size(); i++) {
        std::cout<<t[i]<<std::endl;
    }
    std::cout<<"\n\n";


    std::cout<<"list directory '..\\..\\*':\n";
    t=listfiles_wildcard("..\\..\\*");
    for (size_t i=0; i<t.size(); i++) {
        std::cout<<t[i]<<std::endl;
    }
    std::cout<<"\n\n";

    std::cout<<"list directory '..\\..\\*.cpp':\n";
    t=listfiles_wildcard("..\\..\\*.cpp");
    for (size_t i=0; i<t.size(); i++) {
        std::cout<<t[i]<<std::endl;
    }
    std::cout<<"\n\n";

    std::cout<<"list directory '..\\..\\tools*.*':\n";
    t=listfiles_wildcard("..\\..\\tools*.*");
    for (size_t i=0; i<t.size(); i++) {
        std::cout<<t[i]<<std::endl;
    }
    std::cout<<"\n\n";

    std::cout<<"list directory '..\\..\\ools.h':\n";
    t=listfiles_wildcard("..\\..\\ools.h");
    for (size_t i=0; i<t.size(); i++) {
        std::cout<<t[i]<<std::endl;
    }
    std::cout<<"\n\n";

    std::cout<<"list directory 'c:\\windows\\*.exe':\n";
    t=listfiles_wildcard("c:\\windows\\*.exe");
    for (size_t i=0; i<t.size(); i++) {
        std::cout<<t[i]<<std::endl;
    }
    std::cout<<"\n\n";

    std::cout<<"linecount in 'test.csv' (no comment_char):"<< count_lines("test.csv") <<"\n";
    std::cout<<"linecount in 'test.csv' (comment_char='#'):"<< count_lines("test.csv", '#') <<"\n";
    std::cout<<"read in 'test.csv' line-by-line:\n";
    try {
        FILE* f=fopen("test.csv", "r");
        std::vector<double> r=csv_readline(f);
        while (r.size()>0) {
            for (size_t i=0; i<r.size(); i++) {
                if (i>0) cout<<",\t";
                cout<<r[i];
            }
            cout<<"\n";
            r=csv_readline(f);
        }
        fclose(f);
    } catch (std::exception& E) {
        cout<<"ERROR: "<<E.what()<<std::endl;
    }
    std::cout<<"\n\n";

    std::cout<<"linecount in 'test.ssf' (no comment_char):"<< count_lines("test.ssf") <<"\n";
    std::cout<<"linecount in 'test.ssf' (comment_char='#'):"<< count_lines("test.ssf", '#') <<"\n";
    std::cout<<"read in 'test.ssf' line-by-line:\n";
    try {
        FILE* f=fopen("test.ssf", "r");
        std::vector<double> r=csv_readline(f, ' ');
        while (r.size()>0) {
            for (size_t i=0; i<r.size(); i++) {
                if (i>0) cout<<",\t";
                cout<<r[i];
            }
            cout<<"\n";
            r=csv_readline(f, ' ');
        }
        fclose(f);
    } catch (std::exception& E) {
        cout<<"ERROR: "<<E.what()<<std::endl;
    }
    std::cout<<"\n\n";

    std::cout<<"linecount in 'test.error' (no comment_char):"<< count_lines("test.error") <<"\n";
    std::cout<<"linecount in 'test.error' (comment_char='#'):"<< count_lines("test.error", '#') <<"\n";
    std::cout<<"read in 'test.error' line-by-line:\n";
    try {
        FILE* f=fopen("test.error", "r");
        std::vector<double> r=csv_readline(f);
        while (r.size()>0) {
            for (size_t i=0; i<r.size(); i++) {
                if (i>0) cout<<",\t";
                cout<<r[i];
            }
            cout<<"\n";
            r=csv_readline(f);
        }
        fclose(f);
    } catch (std::exception& E) {
        cout<<"ERROR: "<<E.what()<<std::endl;
    }
    std::cout<<"\n\n";

    std::cout<<"test 'Test12Test21Test':\n";
    std::cout<<"  matches '*12*21*' :"<<booltostr(match_wildcard("Test12Test21Test", "*12*21*"))<<std::endl;
    std::cout<<"  contains '*12*21*' :"<<booltostr(contains_wildcard("Test12Test21Test", "*12*21*"))<<std::endl;
    std::cout<<"  matches '*12*21' :"<<booltostr(match_wildcard("Test12Test21Test", "*12*21"))<<std::endl;
    std::cout<<"  contains '*12*21' :"<<booltostr(contains_wildcard("Test12Test21Test", "*12*21"))<<std::endl;
    std::cout<<"  matches '*' :"<<booltostr(match_wildcard("Test12Test21Test", "*"))<<std::endl;
    std::cout<<"  contains '*' :"<<booltostr(contains_wildcard("Test12Test21Test", "*"))<<std::endl;
    std::cout<<"  matches 'Test1?Test' :"<<booltostr(match_wildcard("Test12Test21Test", "Test1?Test"))<<std::endl;
    std::cout<<"  contains 'Test1?Test' :"<<booltostr(contains_wildcard("Test12Test21Test", "Test1?Test"))<<std::endl;
    std::cout<<"  contains 'Test?Test' :"<<booltostr(contains_wildcard("Test12Test21Test", "Test?Test"))<<std::endl;
    std::cout<<"  contains 'Test??Test' :"<<booltostr(contains_wildcard("Test12Test21Test", "Test??Test"))<<std::endl;
    std::cout<<"\n\n";

    std::string filename="c:"+std::string(PATHSEPARATOR_STRING)+"temp"+std::string(PATHSEPARATOR_STRING)+"test"+std::string(PATHSEPARATOR_STRING)+"test.filename.dat";
    std::cout<<"filename = '"<<filename<<"'\n";
    std::cout<<"  extract_file_path(filename) = '"<<extract_file_path(filename)<<"'\n";
    std::cout<<"  extract_file_name(filename) = '"<<extract_file_name(filename)<<"'\n";
    std::cout<<"  extract_file_ext(filename) = '"<<extract_file_ext(filename)<<"'\n";
    std::cout<<"  change_file_ext(filename, \".txt\") = '"<<change_file_ext(filename, ".txt")<<"'\n";
    std::cout<<"  change_file_ext(filename, \"txt\") = '"<<change_file_ext(filename, "txt")<<"'\n";
    std::cout<<"  change_file_ext(filename, \"ttr.txt\") = '"<<change_file_ext(filename, "ttr.txt")<<"'\n";
    std::cout<<"  change_file_ext(filename, \".ttr.txt\") = '"<<change_file_ext(filename, ".ttr.txt")<<"'\n";
    std::cout<<"  get_full_filename(filename) = '"<<get_full_filename(filename)<<"'\n";
    std::cout<<"  extend_file_path(filename, \"results\") = '"<<extend_file_path(filename, "results")<<"'\n";
    std::cout<<"  extend_file_path(filename, \"results"+std::string(PATHSEPARATOR_STRING)+"\") = '"<<extend_file_path(filename, "results")<<"'\n";
    std::cout<<"  extend_file_path(filename, \"\") = '"<<extend_file_path(filename, "")<<"'\n";
    std::cout<<"  extend_file_path(filename, \""+std::string(PATHSEPARATOR_STRING)+"\") = '"<<extend_file_path(filename, std::string(PATHSEPARATOR_STRING))<<"'\n";

    filename=".."+std::string(PATHSEPARATOR_STRING)+".."+std::string(PATHSEPARATOR_STRING)+"temp"+std::string(PATHSEPARATOR_STRING)+"test"+std::string(PATHSEPARATOR_STRING)+"test.filename.dat";
    std::cout<<"filename = '"<<filename<<"'\n";
    std::cout<<"  extract_file_path(filename) = '"<<extract_file_path(filename)<<"'\n";
    std::cout<<"  extract_file_name(filename) = '"<<extract_file_name(filename)<<"'\n";
    std::cout<<"  extract_file_ext(filename) = '"<<extract_file_ext(filename)<<"'\n";
    std::cout<<"  change_file_ext(filename, \".txt\") = '"<<change_file_ext(filename, ".txt")<<"'\n";
    std::cout<<"  change_file_ext(filename, \"txt\") = '"<<change_file_ext(filename, "txt")<<"'\n";
    std::cout<<"  change_file_ext(filename, \"ttr.txt\") = '"<<change_file_ext(filename, "ttr.txt")<<"'\n";
    std::cout<<"  change_file_ext(filename, \".ttr.txt\") = '"<<change_file_ext(filename, ".ttr.txt")<<"'\n";
    std::cout<<"  get_full_filename(filename) = '"<<get_full_filename(filename)<<"'\n";
    std::cout<<"  extend_file_path(filename, \"results\") = '"<<extend_file_path(filename, "results")<<"'\n";
    std::cout<<"  extend_file_path(filename, \"results"+std::string(PATHSEPARATOR_STRING)+"\") = '"<<extend_file_path(filename, std::string(PATHSEPARATOR_STRING))<<"'\n";
    std::cout<<"  extend_file_path(filename, \"\") = '"<<extend_file_path(filename, "")<<"'\n";
    std::cout<<"  extend_file_path(filename, \""+std::string(PATHSEPARATOR_STRING)+"\") = '"<<extend_file_path(filename, std::string(PATHSEPARATOR_STRING))<<"'\n";

    std::cout<<"\n\n\n";
    std::string testvarname="HalloWelt";
    std::cout<<"  to_valid_variable_name(\""<<testvarname<<"\") = '"<<to_valid_variable_name(testvarname)<<"'\n";
    testvarname="Hallo Welt";
    std::cout<<"  to_valid_variable_name(\""<<testvarname<<"\") = '"<<to_valid_variable_name(testvarname)<<"'\n";
    testvarname="Hallo Welt 123[]";
    std::cout<<"  to_valid_variable_name(\""<<testvarname<<"\") = '"<<to_valid_variable_name(testvarname)<<"'\n";
    testvarname="12 Hallo Welt?";
    std::cout<<"  to_valid_variable_name(\""<<testvarname<<"\") = '"<<to_valid_variable_name(testvarname)<<"'\n";
    return 0;
}
