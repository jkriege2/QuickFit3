/*
  Name: jkiniparser.h
  Copyright: (c) 2007-2008
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/
/*!
    \defgroup jkip INI File Parser
    \ingroup tools_classes

    This group contains classes that form a (stand-alone) INI file parser. This can be used to read in
    a variety of configuration files.
 */
/*@{*/

/** \file jkiniparser.h
 */


#ifndef JKINIPARSER_H
#define JKINIPARSER_H

#include <string>
#include <map>
#include <vector>
#include <cstdio>
#include <sstream>
#include <fstream>
#include <exception>


#ifdef JKIP_USE_MATHPARSER
#include "jkmathparser.h"
#endif




/** \brief error handling: exceptions of the type of this class will be thrown if an error occurs */
class jkipError : public std::exception {
  private:
       std::string errormessage;
	public:
		/** \brief class constructors */
		jkipError() throw();

		/** \brief class destructor */
		virtual ~jkipError() throw() {};

		/** \brief constructor with supplied error message */
		jkipError(std::string msg) throw();

		/** \brief  returns the assigned errormessage */
		std::string getMessage() const;

		/** \brief returns the error description as C string */
		virtual const char* what() const throw() { return getMessage().c_str(); };
};



/** \brief macro that throws an exception or calls an error handler */
#define jkipException(st) { if (jkiniparser_exception_function!=NULL) { jkiniparser_exception_function(st); } else { throw jkipError(st); } };
//#define jkmpError(st) {printf("error while parsing:  %s\n", st); system("PAUSE"); abort();}

/** \brief type for a custom error handler. This an alternative error handling ... may be used together with Matlab in a MEX file! */
typedef void (*jkipexceptionf)(std::string);
#ifdef COMPILING_THIS_JKINIPARSER
/** \brief if this is NULL then an exception may be thrown otherwise this should point to an error handler that will be called. */
jkipexceptionf jkiniparser_exception_function=NULL;
#else
extern jkipexceptionf jkiniparser_exception_function;
#endif







/*! \brief parser class for INI-style configuration files

   You may use this class to parse an INI file (\c readFile(<filename>)) and
   then access its contents (\c getAs*, \c jkinifile::getAsParsed*). It is
   also possible to change the contents and to subsequently save the INI file to the
   disk (\c writeFile(<filename>). In the lather case all comments in the
   prevoiusly loaded INI file will be lost.

   It is possible to interpret the data in the INI file as a mathematical formula. Then by
   using the \c getAsParsed* methods you can evaluate this formula and get the result
   back. To parse these mathematical expressions the class jkMathParser will be used.


    Here is a simple example of the usage:
    \code
    jkiniparser ini;
    try {
       ini.readFile("test.ini"); // read in an INI file

       // rwad some data
       int data1=ini.getAsInt("gruppe1", "int data", -10);
       double data2=ini.getAsDouble("gruppe1", "doubledata", -10.00125);
       string data3=ini.getAsString("gruppe1", "stringdata");

       cout<<"printing all group names in the file ..."<<endl;
       if (ini.getGroupCount()>0) {
         for (unsigned long i=0; i<ini.getGroupCount(); i++) {
           cout<<ini.getGroupName(i)<<endl;;
         }
       }

       if (ini.groupExists("parsed")) { // check whether group exists
         // parse some property in the group
         double data4=ini.getAsParsedDouble("parsed", "data", -10.00125); // now [parsed] ... data=pi^2+5 will return the evaluated data
       }

       ini.set("gruppe2", "data", 15.678);
       ini.writeFile("test1.ini"); // write the changed INI file to disk
    } catch (jkipError e) { // error handling
       cout<<"error:   "<<e.getMessage()<<endl;
    }
    \endcode

    <b>example of an INI file:</b>
    \code
     [Group]
     param1 = value_part1\
              value_part2  # comment
     param2 = value
     # comment
    \endcode


    \section jkip_error Error Handling
    This class implements two modes of error handling that may be controlled by the global variable
    jkiniparser_exception_function. By default this variable is \c NULL and the jkiniparser
    throws jkipError exceptions on any error condition. If this behaviour is not wished for, because
    maybe your programming environment does not support std::exception (this is one of the problems
    when writing C++ code for Matlab MEX-files), you can let jkiniparser_exception_function point to
    an error handling function that should also stop the program due to the error and output the
    error to the user. a simple implementation could look like this:

    \code
    void errorHandler(std::string errorMessage) {
        std::cout<<"there was an error while parsing an ini file:\n    "<<errorMessage<<std::endl;
        abort();
    }
    \endcode



    \section jkip_ebnf EBNF grammar of ini files
    \code
    file:
      jkiEND                 // jkiEND is the end of input
      fieldassignments PRINT
      group PRINT
      group fieldassignments PRINT

    group:
      "["groupname"]"PRINT

    fieldassignments:
      fieldassignment fieldassignments
      fieldassignment PRINT

    fieldassignment:
      fieldname "=" primary PRINT
      fieldname "=" primary "\" PRINT { primary "\" PRINT } primary PRINT

    primary:
      <characters>           // contents of field

    PRINT:                   // linebreak
      #13 | #10
    \endcode

    Every primary may be broken over several lines if there is a backslash '\'
    in front of every line end marker '\\n'. This feature is activated only if
    jkiniparser::allowMultiLines is set to true (standard is false).

  */

class jkiniparser {
    private:
        /** \brief comment character */
        char jkiCommentMarker;
        /** \brief character to indicate an argument broken over several lines*/
        char jkiMultilineMarker;

        /** allowed tokens in the INI file */
        enum jkiTOKEN {
            jkiEOFile,jkiEND,jkiPRINT,jkiLBRACKET,jkiCOMMENT,jkiNAME,jkiUNKNOWN
        };
        /** \brief after parsing this will contain the ini file
         *
         * The outer map contains one entry per group and the inner map contains one entry per
         * field in the group, so you can access the fiel \c fieldname from group \c groupname
         * by writing \c dictionary[groupname][fieldname].
         */
        std::map<std::string, std::map<std::string,std::string> > dictionary;
        /** \brief name of the currently processed group */
        std::string currentGroup;
        /** \brief name of the currently processed field */
        std::string currentField;
        /** \brief the value of the currently processed field */
        std::string currentValue;
        /** \brief a FILE struct used when parsing the INI file */
        FILE* configf;
        /** \brief the token currently to be processed */
        jkiTOKEN currentToken;

    #ifdef JKIP_USE_MATHPARSER
        /** \brief a math parser object used for evaluated fields */
        jkMathParser mp;
    #endif
        /** \brief when \c true field contents may be spread over multiple lines
         *         with one backslash at the end of every line */
        bool allowMultiLine;
        /** \brief return true if the specified group is already registered */
        bool groupexists(std::string group);
        /** \brief  registeres a group name*/
        void registerGroup(std::string group);
        /** \brief  reads a new token from Config-File */
        jkiTOKEN getToken();
        /** \brief  parses a group declaration of the form [groupname] */
        void parse_groupdecl(bool get);
        /** \brief  reads in all characters until jkiEND of line or file */
        std::string parse_data(unsigned char commentchar);
        /** \brief  reads in all characters until jkiEND of line or file, includes multilines */
        std::string parse_data_multiline(unsigned char commentchar);
        /** \brief  reads in all characters until jkiEND of line or file,
         *          datasections may contain more than one line
         *          std::string parse_data_multiline(unsigned char commentchar);
         *          parses a config file
         */
        void parse_file(bool get);
        /** \brief  reads in a name */
        std::string parse_name(unsigned char jkiENDchar);
        /** \brief  parses a fieldassignment of the form field = anyvalue */
        void parse_fieldassignment(bool get);
    public:
        /** \brief  class constructor */
        jkiniparser();

        /** \brief  class destructor */
        ~jkiniparser();

        /** \brief  class constructor that automatically loads an ini-file */
        jkiniparser(std::string filename, bool allowML=false);

        /** \brief  writes configuration data to an ini-file */
        void writeFile(std::string filename);

        /** \brief  reads in an ini-file */
        void readFile(std::string filename);

        /** \brief  returns true, if [group] and field exist  */
        bool exists(std::string group, std::string field);

        /** \brief  returns true, if [group] exist */
        bool groupExists(std::string group);

        /** \brief  returns the current number of groups */
        int getGroupCount();

        /** \brief  returns the name of the ith group*/
        std::string getGroupName(int num);

        /** \brief  returns the contents of group/field as a string */
        std::string getAsString(std::string group, std::string field, std::string stdresult="");
        /** \brief  returns the contents of group/field as an integer */
        int getAsInt(std::string group, std::string field, int stdresult=0);
        /** \brief  returns the contents of group/field as a boolean*/
        bool getAsBool(std::string group, std::string field, bool stdresult=false);
        /** \brief  returns the contents of group/field as a double*/
        double getAsDouble(std::string group, std::string field, double stdresult=0);

    #ifdef JKIP_USE_MATHPARSER
        /** \brief  returns the parsed contents of group/field as an integer. The contents is a formula that will be parsed. */
        int getAsParsedInt(std::string group, std::string field, int stdresult=0);
        /** \brief  returns the parsed contents of group/field as a boolean. The contents is a formula that will be parsed.*/
        bool getAsParsedBool(std::string group, std::string field, bool stdresult=false);
        /** \brief  returns the parsed contents of group/field as a double. The contents is a formula that will be parsed.*/
        double getAsParsedDouble(std::string group, std::string field, double stdresult=0);
    #endif

        /** \brief  sets group/field with data (integer) */
        void set(std::string group, std::string field, int data);

        /** \brief  sets group/field with data (string) */
        void set(std::string group, std::string field, std::string data);

        /** \brief  sets group/field with data (double) */
        void set(std::string group, std::string field, double data);

        /** \brief  sets group/field with data (data) */
        void set(std::string group, std::string field, bool data);

        /** \brief  prints the config-data to screen*/
        void print();
        /** \brief  deletes all saved configuration data*/
        void clear();

        /** \brief gets the status of the feature of values stretched over several lines */
        inline bool getAllowMultiline() {return allowMultiLine;};
        /** \brief enables/disables the feature of values stretched over several lines */
        inline void setAllowMultiline(bool val) {allowMultiLine=val;};
};

#endif // JKINIPARSER_H
/*@}*/

