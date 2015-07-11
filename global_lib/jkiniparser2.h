/*
  Name: jkINIParser2.h
  Copyright: (c) 2007-2008
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/


/** \file jkINIParser2.h
 *
 * This file contains an enhanced version of the ini parser
 */


#ifndef JKINIPARSER2_H
#define JKINIPARSER2_H

#include <string>
#include <map>
#include <vector>
#include <cstdio>
#include <sstream>
#include <fstream>
#include <exception>
#include <stack>
#include <cstring>


#include "jkmathparser.h"
#include "tools.h"


/*! \brief parser class for INI-style configuration files
    \ingroup jkip

   You may use this class to parse an INI file (\c readFile(<filename>)) and
   then access its contents (\c getAs*, \c jkinifile::getAsParsed*). It is
   also possible to change the contents and to subsequently save the INI file to the
   disk (\c writeFile(<filename>). In the lather case all comments in the
   prevoiusly loaded INI file will be lost.


   This parser supports a specialised INI format which interprets every right hand side as
   a mathematical expression. So you can do simple calculations. It is also possible to use
   all previously defined variables in an expression. In contrast to jkiniparser (v1.0) you
   will have to enclose all string data in quotation marks \c "...". If an unparsable expression
   is encountered the according value is taken to be a string.


   Also the names of the properties are treated in a special way. Basically there are two possibilities
   to mark the current group and property names:

   <b>possibility 1:</b>
   \code
   [group1]
   data1="hallo"
   data2=123

   [group2]
   data1=true
   \endcode
   Here all properties after a group \c [groupname] belong to this group

   <b>possibility 2:</b>
   \code
   group1.data1="hallo"
   group1.data2=123
   group2.data1=true
   \endcode
   Here the grou name is the part of the string befor the first dot/period.


   You may also use any combination of the two possibilities, so also this ini file is the same as the above two:
   \code
   group1.data1="hallo"
   group1.data2=123

   [group2]
   data1=true
   \endcode


   \section jkip2_example Example of Usage

    Here is a simple example of the usage:
    \code
    jkINIParser2 ini;
    try {
       ini.readFile("test.ini"); // read in an INI file
       cout<<endl<<endl<<"printing all contents in the file ..."<<endl;
       ini.print();

       // read some data
       cout<<"gruppe1.intdata = "<<ini.getAsInt("gruppe1.intdata", -10)<<endl;
       cout<<"gruppe1.doubledata1 = "<<ini.getAsDouble("gruppe1.doubledata1", -10.00125)<<endl;
       cout<<"gruppe1.stringdata = "<<ini.getAsString("gruppe1.stringdata")<<endl;

       // change the current root and set there some data
       ini.enterGroup("gruppe1");
       ini.set("intdata", -100.0);
       ini.leaveGroup();

       cout<<"printing all group names on the top level ..."<<endl;
       if (ini.getGroupCount()>0) {
         for (unsigned long i=0; i<ini.getGroupCount(); i++) {
           cout<<ini.getGroupName(i)<<endl;;
         }
       }
       ini.set("gruppe2.data", 15.678);

       cout<<endl<<endl<<"printing all contents in the file ..."<<endl;
       ini.print();


       ini.writeFile("test_out.ini"); // write the changed INI file to disk

       // trying to re-read the currently
       ini.readFile("test_out.ini");
       cout<<endl<<endl<<"printing all contents in the file ..."<<endl;
       ini.print();

       // read another, more complex file
       ini.readFile("diffusion3.ini");
       cout<<endl<<endl<<"printing all contents in the file ..."<<endl;
       ini.print();

    } catch (std::exception& e) { // error handling
       cout<<"error:   "<<e.what()<<endl;
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



    \section jkip_ebnf EBNF Rough Grammar of Ini Files
    <pre> ini_file           -> { ini_file_element }</pre>
    <pre> ini_file_element   ->  EndOfFile
                          | { field_assignment } PRINT
                          | group_definition PRINT
                          | statement PRINT</pre>
    <pre> group_definition   ->  <b>[</b> GROUPNAME <b>]</b></pre>
    <pre> field_assignment   ->  FILEDNAME "=" primary PRINT
                          | FIELDNAME <b>=</b> primary <b>\\</b> PRINT { primary <b>\\</b> PRINT } primary PRINT</pre>
    <pre>  statement         ->  <b>!include</b> primary</pre>
    <pre>  primary           ->  CHARACTERS</pre>
    <pre>  PRINT             ->  #13 / #10</pre>


    Every primary may be broken over several lines if there is a backslash '\\'
    in front of every line end marker '\\n'. This feature is activated only if
    jkiniparser::allowMultiLines is set to true (standard is false).

    The <code>!include filepath\filename.ini</code> statement loads the specified ini file at the specified
    position and parses it before going on with the  current ini file.

    Comments start with a \c # and last until the end of the current line.

  */
class jkINIParser2 {
    public:
        /** \brief error handling: exceptions of the type of this class will be thrown if an error occurs */
        class error : public std::exception {
          private:
               std::string errormessage;
            public:
                /** \brief class constructors */
                error() throw() {
                    errormessage="unkown error";
                };

                /** \brief class destructor */
                virtual ~error() throw() {};

                /** \brief constructor with supplied error message */
                error(std::string msg) throw() {
                    errormessage=msg;
                };

                /** \brief  returns the assigned errormessage */
                std::string getMessage() const {
                    return errormessage;
                };

                /** \brief returns the error description as C string */
                virtual const char* what() const throw() { return getMessage().c_str(); };
        };

        /** \brief this function throws an exception or calls an error handler, depending on the state of
         *         jkiniparser_exception_function. The parameter \a st contains the error message */
        inline void throwException(std::string st) {
            if (jkiniparser_exception_function!=NULL) {
                jkiniparser_exception_function(st);
            } else {
                throw error(st);
            }
        };

        /** \brief type for a custom error handler. This an alternative error handling ... may be used together with Matlab in a MEX file! */
        typedef void (*jkipexceptionf)(std::string);

    protected:
        /** \brief if this is NULL then an exception may be thrown otherwise this should point to an error handler that will be called. */
        jkipexceptionf jkiniparser_exception_function;

        /** \brief possible property types */
        enum jkip2PropertyType {jkip2Double,  /*!< \brief a floating-point number with double precision. */
                                jkip2Int,     /*!< \brief an integer number */
                                jkip2String,  /*!< \brief a string of characters */
                                jkip2Bool};   /*!< \brief a boolean value true|false */

        /** \brief this struct represents one property */
        struct jkip2Property {
            /** \brief points to the parser object owning this property */
            jkINIParser2* parser;
            /** \brief type of the property */
            jkip2PropertyType type;
            /** \brief used to store the value of integer properties */
            int64_t intData;
            /** \brief used to store the value of double properties */
            double doubleData;
            /** \brief used to store the value of bool properties */
            bool boolData;
            /** \brief used to store the value of string properties */
            std::string stringData;

            /** \brief returns the value of the current node as a string */
            std::string toString();

            /** \brief returns the value of the current node as a string,
             *         if the node contains string data the string is put into quotation marks (\c " ).
             *         In addition the string is escapified
             */
            std::string toDelimString();

            /** \brief tries to convert the contents of the property to a boolean. Returns \a defaultValue if the conversion is not possible. */
            bool toBool(bool defaultValue);

            /** \brief tries to convert the contents of the property to a boolean. Throws an exception if the conversion is not possible. */
            bool toBool();

            /** \brief tries to convert the contents of the property to an integer. Returns \a defaultValue if the conversion is not possible. */
            int64_t toInt(int64_t defaultValue);

            /** \brief tries to convert the contents of the property to an integer. Throws an exception if the conversion is not possible. */
            int64_t toInt();

            /** \brief tries to convert the contents of the property to a double. Returns \a defaultValue if the conversion is not possible. */
            double toDouble(double defaultValue);

            /** \brief tries to convert the contents of the property to a double. Throws an exception if the conversion is not possible. */
            double toDouble();


            /** \brief this function tries to interpret the given string as a floating point or an integer number
             *         and then sets the node accordingly */
            void setFromNumberString(std::string data);

            /** \brief this function tries to interpret the given string as a boolean, floating point or an integer number
             *         and then sets the node accordingly. If none of the above types is recognized it sets the node as a
             *         string node. */
            void setFromString(std::string data);

            /** \brief set the contents of the property according to the data in a jkmpResult struct */
            void setFromJKMPResult(jkMathParser::jkmpResult res);
        };

        /** \brief class to manage a group of configuration properties
         *
         * This class may be used to build a tree of configuration properties. Each node represents one
         * group and may contain a number of properties.
         */
         class groupNode {
             protected:
                /** \brief points to the parser object owning this groupNode. */
                jkINIParser2* parser;
                /** \brief points to the parent node, or NULL for the root node */
                groupNode* parent;
                /** \brief a list of all child nodes. Each child node is accessed using its name. */
                std::map<std::string, groupNode*> child;
                std::vector<std::string> child_order;
                /** \brief a list of all properties in this node */
                std::map<std::string, jkip2Property> property;

             public:
                /** \brief class constructor */
                groupNode(groupNode* parent, jkINIParser2* parser) {
                    this->parent=parent;
                    this->parser=parser;
                };

                /** \brief class destructor, deletes all child objects! */
                ~groupNode();

                /** \brief returns a pointer to the parent node */
                inline groupNode* getParent() { return parent; };

                /** \brief deletes all children and properties */
                void clear();

                /** \brief returns the number of properties in this node (i.e. directly connected to this node, not in the sub-tree rooted by it!) */
                inline size_t getPropertyCount() { return property.size(); }

                /** \brief returns the number of children in this node*/
                inline size_t getGroupCount() { return child.size(); }

                /** \brief adds a property to the current node.
                 *
                 * If the property name contains dot '.' then the name of the property is the part after the last dot.
                 * The other parts name groups, so if you ask to insert \c "group1.group2.propnam" this function will first
                 * create a child group \c group1" and then insert "group2.propname" there. This process will iterate until
                 * only a property name stays. Then a property with the accoording name and value will be inserted.
                 *
                 * If the property already exists, its value will be set to the new data by this method.
                 */
                void setProperty(std::string propertyName, jkip2Property value);

                /** \brief adds a boolean property to the current node.
                 *
                 * If the property name contains dot '.' then the name of the property is the part after the last dot.
                 * The other parts name groups, so if you ask to insert \c "group1.group2.propnam" this function will first
                 * create a child group \c group1" and then insert "group2.propname" there. This process will iterate until
                 * only a property name stays. Then a property with the accoording name and value will be inserted.
                 *
                 * If the property already exists, its value will be set to the new data by this method.
                 */
                inline void setProperty(std::string propertyName, bool value) {
                    jkip2Property p;
                    p.type=jkip2Bool;
                    p.boolData=value;
                    setProperty(propertyName, p);
                };

                /** \brief adds a double property to the current node.
                 *
                 * If the property name contains dot '.' then the name of the property is the part after the last dot.
                 * The other parts name groups, so if you ask to insert \c "group1.group2.propnam" this function will first
                 * create a child group \c group1" and then insert "group2.propname" there. This process will iterate until
                 * only a property name stays. Then a property with the accoording name and value will be inserted.
                 *
                 * If the property already exists, its value will be set to the new data by this method.
                 */
                inline void setProperty(std::string propertyName, double value) {
                    jkip2Property p;
                    p.type=jkip2Double;
                    p.doubleData=value;
                    setProperty(propertyName, p);
                };

                /** \brief adds a integer property to the current node.
                 *
                 * If the property name contains dot '.' then the name of the property is the part after the last dot.
                 * The other parts name groups, so if you ask to insert \c "group1.group2.propnam" this function will first
                 * create a child group \c group1" and then insert "group2.propname" there. This process will iterate until
                 * only a property name stays. Then a property with the accoording name and value will be inserted.
                 *
                 * If the property already exists, its value will be set to the new data by this method.
                 */
                inline void setProperty(std::string propertyName, int64_t value) {
                    jkip2Property p;
                    p.type=jkip2Int;
                    p.intData=value;
                    setProperty(propertyName, p);
                };

                /** \brief adds a string property to the current node.
                 *
                 * If the property name contains dot '.' then the name of the property is the part after the last dot.
                 * The other parts name groups, so if you ask to insert \c "group1.group2.propnam" this function will first
                 * create a child group \c group1" and then insert "group2.propname" there. This process will iterate until
                 * only a property name stays. Then a property with the accoording name and value will be inserted.
                 *
                 * If the property already exists, its value will be set to the new data by this method.
                 */
                inline void setProperty(std::string propertyName, std::string value) {
                    jkip2Property p;
                    p.type=jkip2String;
                    p.stringData=value;
                    setProperty(propertyName, p);
                };


                /** \brief returns \c true if the given property exists.
                 *
                 * If the property name consists of multiple parts with dots in bewteen, this method will follow the property
                 * tree downwards until it finds a property with the right name, or return \c false. */
                bool exists(std::string propertyName);

                /** \brief returns \c true if the given group exists as child of this node.
                 *
                 * If the property name consists of multiple parts with dots in bewteen, this method will follow the property
                 * tree downwards until it finds a property with the right name, or return \c false. */
                bool groupExists(std::string name);

                /** \brief returns a pointer to the group with the given name or \c NULL if the group does not exist
                 *
                 * If the property name consists of multiple parts with dots in bewteen, this method will follow the property
                 * tree downwards until it finds a property with the right name, or return \c false. */
                groupNode* getGroup(std::string name);

                /** \brief returns a pointer to the group with the given name. If the group does not exist it is created.
                 *
                 * If the property name consists of multiple parts with dots in bewteen, this method will follow the property
                 * tree downwards until it finds a property with the right name, or return \c false. */
                groupNode* ensureGroup(std::string name);

                /** \brief returns the n-th group which is a direct child of this node. If an error occurs this function returns \c NULL. */
                groupNode* getGroup(size_t n);

                /** \brief returns the name of the n-th group which is a direct child of this node. If an error occurs this function returns \c NULL. */
                std::string getGroupName(size_t n);

                /** \brief returns the name of the given group if it is a direct child of this node. If an error occurs this function throws an exception. */
                std::string getGroupName(groupNode* group);

                /** \brief returns the n-th property which is a direct child of this node. If an error occurs this functions throws an exception. */
                jkip2Property getProperty(size_t n);

                /** \brief returns the name of the n-th property which is a direct child of this node. If an error occurs this functions throws an exception. */
                std::string getPropertyName(size_t n);

                /** \brief returns the jkip2Property struct if the given property exists.
                 *
                 * If the property name consists of multiple parts with dots in bewteen, this method will follow the property
                 * tree downwards until it finds a property with the right name, or throw an exception. */
                jkip2Property get(std::string propertyName);

                /** \brief returns the boolean value of the given property or (if it does not exist) the supplied default value */
                bool getAsBool(std::string propertyName, bool defaultValue=false);

                /** \brief returns the integer value of the given property or (if it does not exist) the supplied default value */
                int64_t getAsInt(std::string propertyName, int64_t defaultValue=0);

                /** \brief returns the double value of the given property or (if it does not exist) the supplied default value */
                double getAsDouble(std::string propertyName, double defaultValue=0.0);

                /** \brief returns the string value of the given property or (if it does not exist) the supplied default value */
                std::string getAsString(std::string propertyName, std::string defaultValue="");

                /** \brief returns the boolean value of the given property or (if it does not exist) the supplied default value.
                 *         Afterwards the read value is written back into the internal datastructure.
                 *
                 * Use this method to ensure that the read data is afterwards stored in the object. This can be usefull, if
                 * you want to read configuration data that has probably not been part of the file, but which you want to save
                 * using writeFile().
                 */
                bool getSetAsBool(std::string propertyName, bool defaultValue=false);

                /** \brief returns the integer value of the given property or (if it does not exist) the supplied default value.
                 *         Afterwards the read value is written back into the internal datastructure.
                 *
                 * Use this method to ensure that the read data is afterwards stored in the object. This can be usefull, if
                 * you want to read configuration data that has probably not been part of the file, but which you want to save
                 * using writeFile().
                 */
                int64_t getSetAsInt(std::string propertyName, int64_t defaultValue=0);

                /** \brief returns the double value of the given property or (if it does not exist) the supplied default value.
                 *         Afterwards the read value is written back into the internal datastructure.
                 *
                 * Use this method to ensure that the read data is afterwards stored in the object. This can be usefull, if
                 * you want to read configuration data that has probably not been part of the file, but which you want to save
                 * using writeFile().
                 */
                double getSetAsDouble(std::string propertyName, double defaultValue=0.0);

                /** \brief returns the strng value of the given property or (if it does not exist) the supplied default value.
                 *         Afterwards the read value is written back into the internal datastructure.
                 *
                 * Use this method to ensure that the read data is afterwards stored in the object. This can be usefull, if
                 * you want to read configuration data that has probably not been part of the file, but which you want to save
                 * using writeFile().
                 */
                std::string getSetAsString(std::string propertyName, std::string defaultValue="");

         };

    public:
        /** \brief activate error handling by use of an exception function */
        inline void set_exception_function(jkipexceptionf exception_function) {
            jkiniparser_exception_function=exception_function;
        };

        /** \brief deactivate error handling by use of an exception function */
        inline void reset_exception_function() {
            jkiniparser_exception_function=NULL;
        };

	protected:
		int argc;
		char **argv;
    private:
        /** \brief comment character */
        char jkiCommentMarker;
        /** \brief character to indicate an argument broken over several lines*/
        char jkiMultilineMarker;

        /** allowed tokens in the INI file */
        enum jkiTOKEN {
            jkiEOFile,    /*!< \brief end of file token */
            jkiEND,       /*!< \brief end of input token */
            jkiPRINT,     /*!< \brief linebrak (#13 or #10) token */
            jkiLBRACKET,  /*!< \brief '[' token */
            jkiCOMMENT,   /*!< \brief comment token */
            jkiNAME,      /*!< \brief name token */
            jkiINCLUDE,   /*!< \brief \c !include token */
            jkiUNKNOWN    /*!< \brief unkown token */
        };

        /** \brief after parsing this will contain the ini file */
        groupNode* root;

        /** \brief points to the node currentlytreated as root node */
        groupNode* currentRoot;

        /** \brief this struct is used to store the current parser state */
        struct ParserState {
            /** \brief name of the currently processed group */
            std::string currentGroup;
            /** \brief name of the currently processed field */
            std::string currentField;
            /** \brief the value of the currently processed field */
            //std::string currentValue;
            /** \brief a FILE struct used when parsing the INI file */
            FILE* configf;
            /** \brief the token currently to be processed */
            jkiTOKEN currentToken;
            /** \brief a math parser object used for evaluated fields */
            jkMathParser* mp;
            /** \brief indicates whether the jkMathParser is owned by this object or not
             *
             * If set \c true the parser will be deleted when popping the parser state,
             * otherwise it is not deleted, as it is owned by another state */
            bool owns_mp;

        };

        /** \brief a stack that is used to push and pop the current parser state when reading additional files specified by \c !include */
        std::stack<ParserState> pstateStack;

        /** \brief clears the current parser stack */
        void clearParserStack();

        /** \brief when \c true field contents may be spread over multiple lines
         *         with one backslash at the end of every line */
        bool allowMultiLine;
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
        /** \brief parse a \c !include statement in the ini file
         *
         * This function determines the filename to be read. Then it pushes the current parser state
         * onto the stack, generates a new parser state and parses the new file
         */
         void parse_include(bool get);

        /** \brief  prints the contents of the specified node and prepends the given prefix to each property and group name */
        void groupPrint(groupNode* n, std::string nameBase);

        /** \brief  saves the contents of the specified node into file \a f and prepends the given prefix to each property and group name */
        void groupSave(FILE* f, groupNode* n, std::string nameBase);

    public:
        /** \brief  class constructor */
        jkINIParser2();
        /** \brief  class constructor */
        jkINIParser2(int argc, char **argv);
        
        /** \brief  class destructor */
        ~jkINIParser2();


 
        /** \brief  class constructor that automatically loads an ini-file */
        jkINIParser2(std::string filename, bool allowML=false, int argc=0, char **argv=NULL);

        /** \brief  writes configuration data to an ini-file */
        void writeFile(std::string filename);

        /** \brief  reads in an ini-file */
        void readFile(std::string filename, const std::map<std::string, std::string>& presets=std::map<std::string, std::string>());
        
  
        /** \brief  returns true, if the property \ propertyName exists on thecurrent level
         *
         *
         * If the property name consists of multiple parts with dots in bewteen, this method will follow the property
         * tree downwards until it finds a property with the right name, or return \c false.
         */
        inline bool exists(std::string propertyName) { return currentRoot->exists(propertyName); };

        /** \brief  returns true, if the group \a group exist on the current level
         *
         *
         * If the property name consists of multiple parts with dots in bewteen, this method will follow the property
         * tree downwards until it finds a property with the right name, or return \c false.
         */
        inline bool groupExists(std::string group) { return currentRoot->groupExists(group); };

        /** \brief  sets the given group as current root/level (relative to the current level). If the group does not exist it is created */
        inline void enterGroup(std::string groupName) {
            currentRoot=currentRoot->ensureGroup(groupName);
        }

        /** \brief  sets the given group as current root/level (relative to the file/absolute root). If the group does not exist it is created */
        inline void enterGroupAbsolut(std::string groupName) {
            currentRoot=root->ensureGroup(groupName);
        }

        /** \brief set the parent node of the current root/level as root/level. If we are already in the absolute root node this function does nothing */
        inline void leaveGroup() {
            if (currentRoot->getParent()!=NULL) {
                currentRoot=currentRoot->getParent();
            }
        }

        /** \brief set the file/absolute root node as the current root/level. If we are already in the absolute root node this function does nothing */
        inline void resetGroup() { currentRoot=root; };

        /** \brief  returns the number of groups on the current level */
        inline int getGroupCount() {return currentRoot->getGroupCount(); };

        /** \brief  returns the name of the i-th group on the current level */
        inline std::string getGroupName(int num) { return currentRoot->getGroupName(num); };

        /** \brief  returns the absolute name of the current group/level */
        std::string getGroupName();

        /** \brief  returns the contents of the given property as a string */
        inline std::string getAsString(std::string propertyName, std::string stdresult="") { return currentRoot->getAsString(propertyName, stdresult); };

        /** \brief  returns the contents of the given property as an integer */
        inline int getAsInt(std::string propertyName, int stdresult=0) { return currentRoot->getAsInt(propertyName, stdresult); };

        /** \brief  returns the contents of the given property as a boolean*/
        inline bool getAsBool(std::string propertyName, bool stdresult=false) { return currentRoot->getAsBool(propertyName, stdresult); };

        /** \brief  returns the contents of the given property as a double*/
        inline double getAsDouble(std::string propertyName, double stdresult=0.0) { return currentRoot->getAsDouble(propertyName, stdresult); };


        /** \brief  returns the contents of the given property as a string and subsequently writes the returned value back into the internal data structure */
        inline std::string getSetAsString(std::string propertyName, std::string stdresult="") { return currentRoot->getSetAsString(propertyName, stdresult); };

        /** \brief  returns the contents of the given property as an integer and subsequently writes the returned value back into the internal data structure  */
        inline int getSetAsInt(std::string propertyName, int stdresult=0) { return currentRoot->getSetAsInt(propertyName, stdresult); };

        /** \brief  returns the contents of the given property as a boolean and subsequently writes the returned value back into the internal data structure */
        inline bool getSetAsBool(std::string propertyName, bool stdresult=false) { return currentRoot->getSetAsBool(propertyName, stdresult); };

        /** \brief  returns the contents of the given property as a double and subsequently writes the returned value back into the internal data structure */
        inline double getSetAsDouble(std::string propertyName, double stdresult=0.0) { return currentRoot->getSetAsDouble(propertyName, stdresult); };

        /** \brief adds a property to the current node.
         *
         * If the property name contains dot '.' then the name of the property is the part after the last dot.
         * The other parts name groups, so if you ask to insert \c "group1.group2.propnam" this function will first
         * create a child group \c group1" and then insert "group2.propname" there. This process will iterate until
         * only a property name stays. Then a property with the accoording name and value will be inserted.
         *
         * If the property already exists, its value will be set to the new data by this method.
         */
        void setProperty(std::string propertyName, jkip2Property value) {
            currentRoot->setProperty(propertyName, value);
        }

        /** \brief adds a boolean property to the current node.
         *
         * If the property name contains dot '.' then the name of the property is the part after the last dot.
         * The other parts name groups, so if you ask to insert \c "group1.group2.propnam" this function will first
         * create a child group \c group1" and then insert "group2.propname" there. This process will iterate until
         * only a property name stays. Then a property with the accoording name and value will be inserted.
         *
         * If the property already exists, its value will be set to the new data by this method.
         */
        inline void setProperty(std::string propertyName, bool value) {
            currentRoot->setProperty(propertyName, value);
        };

        /** \brief adds a double property to the current node.
         *
         * If the property name contains dot '.' then the name of the property is the part after the last dot.
         * The other parts name groups, so if you ask to insert \c "group1.group2.propnam" this function will first
         * create a child group \c group1" and then insert "group2.propname" there. This process will iterate until
         * only a property name stays. Then a property with the accoording name and value will be inserted.
         *
         * If the property already exists, its value will be set to the new data by this method.
         */
        inline void setProperty(std::string propertyName, double value) {
            currentRoot->setProperty(propertyName, value);
        };

        /** \brief adds a integer property to the current node.
         *
         * If the property name contains dot '.' then the name of the property is the part after the last dot.
         * The other parts name groups, so if you ask to insert \c "group1.group2.propnam" this function will first
         * create a child group \c group1" and then insert "group2.propname" there. This process will iterate until
         * only a property name stays. Then a property with the accoording name and value will be inserted.
         *
         * If the property already exists, its value will be set to the new data by this method.
         */
        inline void setProperty(std::string propertyName, int64_t value) {
            currentRoot->setProperty(propertyName, value);
        };

        /** \brief adds a string property to the current node.
         *
         * If the property name contains dot '.' then the name of the property is the part after the last dot.
         * The other parts name groups, so if you ask to insert \c "group1.group2.propnam" this function will first
         * create a child group \c group1" and then insert "group2.propname" there. This process will iterate until
         * only a property name stays. Then a property with the accoording name and value will be inserted.
         *
         * If the property already exists, its value will be set to the new data by this method.
         */
        inline void setProperty(std::string propertyName, std::string value) {
            currentRoot->setProperty(propertyName, value);
        };

        /** \brief  sets the given property with data (integer) */
        void set(std::string propertyName, int64_t data);

        /** \brief  sets the given property with data (string) */
        void set(std::string propertyName, std::string data);

        /** \brief  sets the given property with data (double) */
        void set(std::string propertyName, double data);

        /** \brief  sets the given property with data (data) */
        void set(std::string propertyName, bool data);

        /** \brief  prints the config-data to screen*/
        void print();

        /** \brief  deletes all saved configuration data*/
        void clear();

        /** \brief gets the status of the feature of values stretched over several lines */
        inline bool getAllowMultiline() {return allowMultiLine;};
        /** \brief enables/disables the feature of values stretched over several lines */
        inline void setAllowMultiline(bool val) {allowMultiLine=val;};
		
};

#endif // JKINIPARSER2_H
/*@}*/


