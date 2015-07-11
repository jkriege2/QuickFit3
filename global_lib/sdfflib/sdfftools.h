/*
  Name: sdfftools.h
  Copyright: (c) 2007
  Author: Jan Krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/

/** \file sdfftools.h
 * \ingroup hlseqgen_sdff_tools
 *
 * In this file you will find some tool methods and classes for sequencer::SDFFparser.
 */
/*@{*/

#ifndef SDFFTOOLS_H
#define SDFFTOOLS_H

#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <cstdio>
#include <stdint.h>
#include <cmath>

#include "tools.h"
#include "datatable.h"
#include "sequencererror.h"
#include "channelmanager.h"
#include "sequencegenerator.h"

namespace sequencer {

class SDFFparser; // forward declaration
class SDFFnode;   // forward declaration


/** \brief this struct is used to manage a list of channel aliases
 * \ingroup hlseqgen_sdff_tools
 */
typedef struct {
    std::string channel;  /*!< \brief real name of the channel */
    std::string alias;    /*!< \brief the alias */
    std::string info;     /*!< \brief additional channel info */
} sdff_channel_alias_data;


/** \brief types of variables in a SDFF sequence definition
 * \ingroup hlseqgen_sdff_tools
 */
enum sdff_vartype {
  sdffVTString,      /*!< \brief a string variable */
  sdffVTDouble,      /*!< \brief a double variable */
  sdffVTInteger,     /*!< \brief an integer variable */
  sdffVTBoolean,     /*!< \brief a boolean variable */
  sdffVTVoid         /*!< \brief the variable contains the value void */
};

/** \brief convert a sdff_vartype to a string
 * \ingroup hlseqgen_sdff_tools
 */
std::string sdff_vartypetostr(sdff_vartype t);

/** \brief represents a variable in a SDFF sequence definition
 * \ingroup hlseqgen_sdff_tools
 */
typedef struct {
  std::string name;          /*!< \brief name of the variable */
  sdff_vartype type;         /*!< \brief type of the variable */
  bool internal;             /*!< \brief specifies whether this variable is internally managed. If this is true the variable will be created (malloc) and disposed (free) by SDFFVariableManager */
  bool constant;             /*!< \brief this variable is a constant */
  std::string* stringValue;  /*!< \brief pointer to the value of a string variable */
  long long* intValue;       /*!< \brief pointer to the value of an integer variable */
  bool* boolValue;           /*!< \brief pointer to the value of a boolean variable */
  double* doubleValue;       /*!< \brief pointer to the value of a double variable */
} sdff_variable;

/** \brief types of results in a SDFF sequence definition
 * \ingroup hlseqgen_sdff_tools
 */
enum sdff_resulttype {
  sdffRTString,      /*!< \brief a string result */
  sdffRTDouble,      /*!< \brief a double result */
  sdffRTInteger,     /*!< \brief an integer result */
  sdffRTBoolean,     /*!< \brief a boolean result */
  sdffRTVoid         /*!< \brief a void result */
};

/** \brief convert a sdff_resulttype to a string
 * \ingroup hlseqgen_sdff_tools
 */
std::string sdff_resulttypetostr(sdff_resulttype t);

/** \brief compare a sdff_vartype and a sdff_resulttype
 * \ingroup hlseqgen_sdff_tools
 */
bool sdff_types_equal(sdff_vartype v, sdff_resulttype r);

/** \brief represents the result of a function
 * \ingroup hlseqgen_sdff_tools
 */
typedef struct {
  sdff_resulttype type;        /*!< \brief type of the result */
  std::string stringValue;  /*!< \brief value of a string result */
  long long intValue;       /*!< \brief value of an integer result */
  bool boolValue;           /*!< \brief value of a boolean result */
  double doubleValue;       /*!< \brief value of a double result */
} sdff_result;

/** \brief convert a sdff_result to a string
 * \ingroup hlseqgen_sdff_tools
 */
std::string sdff_resulttostr(sdff_result r);

/** \brief convert a sdff_result to a string like sdff_resulttostr(), but without giving the type
 * \ingroup hlseqgen_sdff_tools
 */
std::string asdff_resulttostr(sdff_result r);

/** \brief convert a sdff_result to a string like sdff_resulttostr(), but without giving the type and even without quotation marks for strings
 * \ingroup hlseqgen_sdff_tools
 */
std::string aasdff_resulttostr(sdff_result r);


/** \brief just for convenience: a map that represents the variables of one block
 * \ingroup hlseqgen_sdff_tools
 */
typedef std::map<std::string, sdff_variable> sdffVariableMap;

/** \brief this class can be used to manage variables
 * \ingroup hlseqgen_sdff_tools
 *
 * A variable is represented by a sdff_variable struct. Each variable may have one of the types specified
 * in sdff_vartype. There are two types of variables:
 *    - internal variables are managed by SDFFVariableManager completely, i.e. they are allocated and freed
 *    - external variables are not allocated and freed by SDFFVariableManager. You have to supply a pointer to the
 *      according memory structure. the type hast to be <code>long long*</code> for integer variables, \c double*
 *      for double variables \c bool* for boolean variables and \c std::string* for string variables.
 * .
 *
 * Each variable belongs to a block. Variables in higher block overwrite variables in lower blocks. You can enter
 * and leave blocks by using the enter_block() and leave_block() methods.
 *
 * You can get the variable definition for a name by using \c get(name) or the <code>[]</code>-operator. They both
 * start to search the variable in the highest block and then descend to lower blocks.
 *
 * Internally this class works with a std::vector that conatins std::map objects that then contain the variables.
 *
 * Here is some <b>test code</b>:
 * \code
SDFFVariableManager vman;
vman.add_variable_string("test", "level0.t");
vman.add_variable_string("test1", "level0.t1");
vman.add_variable_int("testint");
vman.add_variable_double("testdouble", 3.14156);
vman.add_variable_bool("testbool", true, true);
double data=4.5;
vman.add_ext_variable_double("Edouble", &data);
cout<<vman.report()<<endl;
vman.enter_block();
vman.add_variable_double("testdouble", 2.0*3.14156);
vman.add_variable_bool("testbool1", false);
vman.add_variable_string("test", "level1.t");
cout<<"test='"<<vman.get("test").stringValue<<"'"<<endl;
cout<<"test1='"<<vman.get("test1").stringValue<<"'"<<endl;
cout<<vman.report()<<endl;
sdff_result v;
v.type=sdffVTInteger;
v.intValue=-1000000000;
vman.set("test1", v);
v.type=sdffVTBoolean;
v.intValue=false;
vman.set("testbool", v);
vman.leave_block();
cout<<"test='"<<vman.get("test").stringValue<<"'"<<endl;
cout<<"test1='"<<inttostr(vman.get("test1").intValue)<<"'"<<endl;
cout<<vman.report()<<endl;
\endcode
 * This should output something like:
 * \verbatim
-- 0 --------------------------------------------------------
  external double Edouble = 4.5
  string test = 'level0.t'
  string test1 = 'level0.t1'
  const bool testbool = true
  double testdouble = 3.14156
  int testint = 0
-------------------------------------------------------------

test='level1.t'
test1='level0.t1'
-- 1 --------------------------------------------------------
  string test = 'level1.t'
  bool testbool1 = false
  double testdouble = 6.28312
-- 0 --------------------------------------------------------
  external double Edouble = 4.5
  string test = 'level0.t'
  string test1 = 'level0.t1'
  const bool testbool = true
  double testdouble = 3.14156
  int testint = 0
-------------------------------------------------------------

test='level0.t'
test1='-1000000000'
-- 0 --------------------------------------------------------
  external double Edouble = 4.5
  string test = 'level0.t'
  int test1 = -1000000000
  const bool testbool = true
  double testdouble = 3.14156
  int testint = 0
-------------------------------------------------------------
\endverbatim
 */
class SDFFVariableManager {
  protected:
    /** \brief this vector contains maps that contain the variables */
    std::vector<sdffVariableMap*> variables;

    /** \brief internal use only: add an internal variable to the specified block.
     *         This methods returns a reference to the variable struct.
     */
    sdff_variable& addi(unsigned int block, std::string name, sdff_vartype type, bool constant=false);

    /** \brief internal use only: add an external variable to the specified block.
     *         This methods returns a reference to the variable struct.
     */
    sdff_variable& adde(unsigned int block, std::string name, sdff_vartype type, void* data, bool constant=false);

    /** \brief internal use only: clear all variables in the specified \a block. If there are internal
     *         variables: dispose their memory.
     */
    void clear_block(int block);

    /** \brief return the sdff_variable struct that describes the variable with the given \a name
     *
     * This methods starts to search for the variable in the current block. If it doesn't find
     * a variable with the right name, it descends to the next block ... and so on. If it does not
     * find a variable this methods throws an exception (SEQUENCER_SDFFERROR_VARNOTFOUND_NUM).
     */
     sdff_variable& int_get(std::string name);

     /** \brief determines whether there should be an exception (\c true ) or just no action (\c false, default)
      *         if you try to set the value of a constant variable
      */
     bool constant_assign_error;

     /** \brief determines whether the type of an internal variable is changeable (\c true, default) or not (false) */
     bool type_changeable;

     /** \brief while set \c true then all variables that are added to this object will be added in block 0!  (default:false) */
     bool write_global;
  public:
    /** get the value of constant_assign_error */
    GetMacro(bool, constant_assign_error);
    /** set the value of constant_assign_error */
    SetMacro(bool, constant_assign_error);
    /** get the value of type_changeable */
    GetMacro(bool, type_changeable);
    /** set the value of type_changeable */
    SetMacro(bool, type_changeable);

    /** \brief class constructor */
    SDFFVariableManager();

    /** \brief class destructor */
    virtual ~SDFFVariableManager();

    /** \brief return the \c write_global property */
    GetMacro(bool, write_global);
    /** \brief set the \c write_global property */
    SetMacro(bool, write_global);

    /** \brief clear all variables in the object and free their memory */
    void clear();

    /** \brief begin a new block  */
    void enter_block();

    /** \brief begin a new block  */
    void leave_block();

    /** \brief return the the value of variable \a name as a sdff_result
     *
     * This methods starts to search for the variable in the current block. If it doesn't find
     * a variable with the right name, it descends to the next block ... and so on. If it does not
     * find a variable this methods throws an exception (SEQUENCER_SDFFERROR_VARNOTFOUND_NUM).
     */
     sdff_result get(std::string name);

    /** \brief return the the definition of variable \a name */
     inline sdff_variable get_vardef(std::string name) { return int_get(name); };

    /** \brief return the the definition of num-th variable */
     sdff_variable get_vardef(unsigned int num);

    /** \brief return the the definition of global variable \a name */
     sdff_variable get_globalvardef(std::string name);

    /** \brief return the the definition of num-th global variable */
     sdff_variable get_globalvardef(unsigned int num);

    /** \brief overwrite the the value of variable \a name with \a value
     *
     * This method first searches for the variable (exactly like get() ). If it found the variable
     * there are several possibilities:
     *   - the variable is a constant: Depending on constant_assign_error there will be an exception
     *     (SEQUENCER_SDFFERROR_VARCONSTANT_NUM) or the method will end without setting the value.
     *   - if the types of the non-constant variable and \a value match, the value is assigned to the
     *     variable
     *   - if those types mismatch and we access an internal variable: the type is beeing changed (depending on
     *     type_changable). For an external variable this is not possible and an exception
     *     (SEQUENCER_SDFFERROR_EXTNOTC_NUM) is thrown.
     */
     void set(std::string name, sdff_result value);

    /** \brief set the \c constant property of the given variable
     */
     void set_constant(std::string name, bool value);

     /** \brief return the the value of variable \a name as a sdff_result
      *
      * this just calls get() (also see the documentation there).
      */
     inline sdff_result operator[](std::string name) { return get(name); };

     /** \brief returns \c true if a variable with the given name exists.
      *
      * This works just like get(), but it does not return the variable definition.
      */
     bool variable_exists(std::string name);

     /** \brief return the current block level
      *
      *   - this returns -1 if there was an error!
      *   - this returns 0 if we are not in a block (global variables)
      *   - this returns 1 if we are one block level below the global block
      *   - ...
      * .
      */
      inline int get_block_level() {
        return variables.size()-1;
      };

      /** \brief lists all variables in all levels */
      std::string report();

      /** \brief return the number of variables stored in this class */
      unsigned int get_count();

      /** \brief return the number of variables stored in this class */
      unsigned int get_globalcount();

      /** \brief add an internal variable \a name on the current block level and initialize it with \c void */
      inline void add_variable_void(std::string name, bool constant=false) {
        sdff_variable v=addi(write_global?0:get_block_level(), name, sdffVTVoid,constant);
      };
      /** \brief add an internal string variable \a name on the current block level and initialize it with \a init */
      inline void add_variable_string(std::string name, std::string init="", bool constant=false) {
        sdff_variable v=addi(write_global?0:get_block_level(), name, sdffVTString,constant);
        *(v.stringValue) = init;
      };
      /** \brief add an external string variable \a name on the current block level and let it point to \a var */
      inline void add_ext_variable_string(std::string name, std::string* var, bool constant=false) {
        sdff_variable v=adde(write_global?0:get_block_level(), name, sdffVTString, var,constant);
      };
      /** \brief add an internal integer variable \a name on the current block level and initialize it with \a init */
      inline void add_variable_int(std::string name, long long init=0, bool constant=false) {
        sdff_variable v=addi(write_global?0:get_block_level(), name, sdffVTInteger,constant);
        *(v.intValue) = init;
      };
      /** \brief add an external integer variable \a name on the current block level and let it point to \a var */
      inline void add_ext_variable_int(std::string name, long long* var, bool constant=false) {
        sdff_variable v=adde(write_global?0:get_block_level(), name, sdffVTInteger, var,constant);
      };
      /** \brief add an internal boolean variable \a name on the current block level and initialize it with \a init */
      inline void add_variable_bool(std::string name, bool init=false, bool constant=false) {
        sdff_variable v=addi(write_global?0:get_block_level(), name, sdffVTBoolean,constant);
        *(v.boolValue) = init;
      };
      /** \brief add an external boolean variable \a name on the current block level and let it point to \a var */
      inline void add_ext_variable_bool(std::string name, bool* var, bool constant=false) {
        sdff_variable v=adde(write_global?0:get_block_level(), name, sdffVTBoolean, var,constant);
      };
      /** \brief add an internal double variable \a name on the current block level and initialize it with \a init */
      inline void add_variable_double(std::string name, double init=0, bool constant=false) {
        sdff_variable v=addi(write_global?0:get_block_level(), name, sdffVTDouble,constant);
        *(v.doubleValue) = init;
      };
      /** \brief add an external string variable \a name on the current block level and let it point to \a var */
      inline void add_ext_variable_double(std::string name, double* var, bool constant=false) {
        sdff_variable v=adde(write_global?0:get_block_level(), name, sdffVTDouble, var,constant);
      };
};



/** \brief types of parameters that are supplied to user-define functions
 * \ingroup hlseqgen_sdff_tools
 */
enum sdff_paramtype {
  sdffPTUnknown,     /*!< \brief the parameter type is yet unknown */
  sdffPTString,      /*!< \brief a string parameter */
  sdffPTDouble,      /*!< \brief a double parameter */
  sdffPTInteger,     /*!< \brief an integer parameter */
  sdffPTBoolean      /*!< \brief a boolean parameter */
};
/** \brief convert a sdff_result to a string
 * \ingroup hlseqgen_sdff_tools
 */
std::string sdff_paramtypetostr(sdff_paramtype t);



/** \brief This is a function prototype for adding new mathematical functions to SDFFparser
 * \ingroup hlseqgen_sdff_tools
 *
 * The methods you can implement with this look like:
 * \code sdff_result function(SDFFparser* parser, std::string fname, sdff_result* arguments, unsigned char argument_count) \endcode
 * The first parameter is a pointer to a SDFFparser object. You can use this pointer in issue
 * error or warning messages. The second argument contains all the arguments supplied to the function
 * (as an array) and the third parameter gives the number of arguments that are supplied. The function
 * shell return the result. If you return a result of type sdff_RTVoid this is not a function, but a
 * procedure (like \c void function in C/C++). The parameter \a fname contains the function's name
 */
typedef sdff_result (*sdff_function)(SDFFparser*, std::string, sdff_result*, unsigned char);

/** \brief This is a prototype for a test function that checks for errors during parsing. Return \c true if this function
 *         may work, i.e. you did not find any apparent errors and \c false only if there were errors for sure.
 * \ingroup hlseqgen_sdff_tools
 *
 * The methods you can implement with this look like:
 * \code bool test(SDFFparser* parser, std::string fname, sdff_paramtype* arguments, unsigned char argument_count, bool return_used) \endcode
 * The first parameter \c parser is a pointer to a SDFFparser object. You can use this pointer in issue
 * error or warning messages. The second argument \c arguments contains all information already known
 * about the arguments supplied to the function (as an array) and the third parameter \c argument_count
 * gives the number of arguments that are  supplied. The last parameter \c return_used specifies whether
 * the return value of such a function is used. The function shell return \c true when the function call
 * looks OK and \c false if you find an error. The parameter \a fname contains the function's name
 *
 * It is important to notice that there may be pointers to the actual nodes in the second parameters, but they do
 * not have to be defined.
 */
typedef bool (*sdff_function_check)(SDFFparser*, std::string, sdff_paramtype*, unsigned char, bool);


/** \brief This is a function prototype for adding new functions to SDFFparser that do not take sdff_result as parameter, but just a link to the nodes
 * \ingroup hlseqgen_sdff_tools
 *
 * The methods you can implement with this look like:
 * \code sdff_result function(SDFFparser* parser, std::string fname, SDFFnode** arguments, unsigned char argument_count) \endcode
 * The first parameter is a pointer to a SDFFparser object. You can use this pointer in issue
 * error or warning messages. The second argument contains all the arguments supplied to the function
 * (as an array) and the third parameter gives the number of arguments that are supplied. The function
 * shell return the result. If you return a result of type sdff_RTVoid this is not a function, but a
 * procedure (like \c void function in C/C++). The parameter \a fname contains the function's name
 */
typedef sdff_result (*sdff_nodefunction)(SDFFparser*, std::string, SDFFnode**, unsigned char);



/** \brief types of functions
 * \ingroup hlseqgen_sdff_tools
 */
enum sdff_function_type {
  sdffFTNode,         /*!< \brief function is defined by a SDFFnode tree */
  sdffFTExternal,     /*!< \brief function is defined by external C routines */
  sdffFTNodeExternal  /*!< \brief function is defined by external C routines that gets SDFFnode* as parameters and not sdff_result */
};

/** \brief information about a user-defined function or procedure
 * \ingroup hlseqgen_sdff_tools
 */
typedef struct {
  std::string name;      /*!< \brief name of the parameter in the definition */
  sdff_paramtype type;   /*!< \brief type of the parameter in the definition */
  SDFFnode* node;        /*!< \brief a pointer to the parameter node ... this may be NULL */
} sdff_function_parameter;


/** \brief information about a user-defined function or procedure
 * \ingroup hlseqgen_sdff_tools
 */
typedef struct {
  sdff_function_type type;     /*!< \brief type of the function */
  std::string name;            /*!< \brief name of the variable */
  sdff_function_check check;   /*!< \brief a function that checks the parameters during parsing */
  sdff_function function;      /*!< \brief pointer to a implementation */
  SDFFnode* node;              /*!< \brief points to a node that represents the function of type sdffFTNode */
  sdff_nodefunction nfunction;      /*!< \brief pointer to a implementation and gets SDFFnode* as parameter */
  std::vector<sdff_function_parameter> parameters; /*!< \brief a vector that contains informations about the parameters
                                                      of the function of type sdffFTNode */
  long line;                   /*!< \brief line number where the user-defined function is defined */
} sdff_function_description;


/** \brief this class manages all fcuntions in a SDFFparser
 * \ingroup hlseqgen_sdff_tools
 *
 * Basically this provides a list (internally a map for faster find) of the functions that are available.
 * there are two types of functions:
 *   -# user-defined (\c sdffFTNode ) functions that are defined within a sequence
 *   -# C/C++-functions  (\c sdffFTExternal ) that are defined by two C/C++ fucntions
 * .
 *
 * This class provides possibilities to add, redefine and delete such functions. It may also
 * be used to evaluate them. Evalutaion is quite simple:
 *   -# C/C++ functions are simply called with the parameters. the result is then beeing returned.
 *      all error checking has to happen inside the function.
 *   -# for user-defined functions this class first generates a new block in the given SDFFVariableManager
 *      object and then adds the parameters as variables to this block. Then it evaluates the given node
 *      and returns the evaluation result.
 * .
 */
class SDFFFunctionManager {
  private:
    /** \brief hide the standard constructor with no parameters */
    SDFFFunctionManager() {};
  protected:
    /** \brief a pointer to a SDFFparser object that calls methods from this class */
    SDFFparser* parser;
    /** \brief a pointer to a SDFFVariableManager object that is used by the parser */
    SDFFVariableManager* vmanager;

    /** \brief this map contains all function definitions */
    std::map<std::string, sdff_function_description> functions;

    /** \brief specify whether it is allowed to overwrite an already defined function (default: \c true ) */
    bool overwrite_functions;
  public:
    /** \brief class constructor */
    SDFFFunctionManager(SDFFparser* p, SDFFVariableManager* vm);
    /** \brief class destructor */
    virtual ~SDFFFunctionManager();

    /** \brief enable/disable the overwriting of functions */
    SetMacro(bool, overwrite_functions);
    /** \brief return whether it is allowed to overwrite functions or not */
    GetMacro(bool, overwrite_functions);

    /** \brief get the SDFFparser object this class is using internally */
    GetMacro(SDFFparser*, parser);
    /** \brief set the SDFFparser object this class is using internally */
    SetMacro(SDFFparser*, parser);

    /** \brief get the SDFVariableManager object this class is using internally */
    GetMacro(SDFFVariableManager*, vmanager);
    /** \brief set the SDFVariableManager object this class is using internally */
    SetMacro(SDFFVariableManager*, vmanager);

    /** \brief evaluate the given function with name \a name.
     *
     *   -# C/C++ functions are simply called with the parameters. the result is then beeing returned.
     *      all error checking has to happen inside the function.
     *   -# C/C++ fucntions that get SDFFnode s as input parameters use a list of SDFFnode in nparameters.
     *      You can in this case set parameters=NULL.
     *   -# for user-defined functions this class first generates a new block in the given SDFFVariableManager
     *      object and then adds the parameters as variables to this block. Then it evaluates the given node
     *      and returns the evaluation result.
     * .
     */
     sdff_result evaluate(std::string name, sdff_result* parameters, unsigned char parameter_count, SDFFnode** nparameters=NULL);

    /** \brief check a function during parsing. If this returns false the there is an error and the error
     *         description has been added to the parser object. The parameter \a line gives the line number
     *         where the function is beeing used.
     *
     *   -# C/C++ check function is called with the parameters (if supplied). the result is then beeing returned.
     *      all error checking has to happen inside the function.
     *   -# for user-defined functions this class checks whether the given information complied with the
     *      stored description
     * .
     */
     bool check(std::string name, sdff_paramtype* parameters, unsigned char parameter_count, bool result_used=false);

     /** \brief add a user-defined function to this manager. If the function already exists,
      *         its definition is overwritten. The parameter \a line gives the source code line where this
      *         function is defined (-1 = don't know).
      */
     void add_node(std::string name, std::vector<sdff_function_parameter> parameters, SDFFnode* node, long line=-1);


     /** \brief add a C/C++ function to this manager. If the function already exists,
      *         its definition is overwritten
      */
     void add_ext(std::string name, sdff_function function, sdff_function_check check=NULL);

     /** \brief add a C/C++ function to this manager. If the function already exists,
      *         its definition is overwritten
      */
     void add_next(std::string name, sdff_nodefunction function, sdff_function_check check=NULL);

     /** \brief delete all function definitions in this object */
     inline void clear() { functions.clear(); };

     /** \brief check whether a function is registered */
     inline bool function_exists(std::string name) { return functions.find(name)!=functions.end(); };

     /** \brief return the type of a given function */
     inline sdff_function_type get_function_type(std::string name) {
        if (!function_exists(name)) {
          SEQUENCER_ERROR(SEQUENCER_SDFFERROR_FUNCNOTFOUND_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCNOTFOUND_NUM), name.c_str()), "SDFFFunctionManager::get_function_type(\""+name+"\")");
        }
        return functions[name].type;
     };

     /** \brief lists all functions */
     std::string report();

     /** \brief return the number of defined functions in this object */
     inline unsigned int get_count() { return functions.size();};
};


}

#endif // SDFFTOOLS_H


/*@}*/
