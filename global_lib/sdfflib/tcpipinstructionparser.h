/*
  Name: tcpipinstructionparser.h
  Copyright: (c) 2007
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/
/**
 * \ingroup network
 */
/*@{*/

/** \file tcpipinstructionparser.h
 *
 */

#ifndef TCPIPINSTRUCTIONPARSER_H
#define TCPIPINSTRUCTIONPARSER_H

#include "tools.h"
#include "sequencererror.h"
#include "baseserver.h"
#include "logfile.h"

#include <cstdlib>
#include <iostream>
#include <string>
#include <cstdio>
#include <vector>

namespace sequencer {

/** \brief the possible tokens that can be recognized by the tokenizer in TCPIPinstructionParser
 * \ingroup network
 */
enum tcpipToken {
  tcpipEND,                /*!< \brief end token */
  tcpipPRINT,              /*!< \brief a semicolon ';' */
  tcpipPARAMETER_DIV,      /*!< \brief a comma ',' between two function parameters */
  tcpipSTRING,             /*!< \brief a string delimited by \c ' or \c " */
  tcpipNAME,               /*!< \brief a name (consisting of characters) of a variable or function */
  tcpipNUMBER,             /*!< \brief a number in scientific notation */
  tcpipLBRACKET,           /*!< \brief left brackets '(' */
  tcpipRBRACKET,           /*!< \brief right brackets ')' */
  tcpipTRUE,               /*!< \brief 'true' */
  tcpipFALSE,              /*!< \brief  'false' */
  tcpipALL,                /*!< \brief  'all' */
};

/** \brief possible number types to read by TCPIPinstructionParser:readNumber()
 * \ingroup network
 */
enum tcpipNumberType {
  tcpipDouble,                /*!< \brief we read a double number */
  tcpipInteger                /*!< \brief we read an integer number */
};

/** \brief possible parameter types of an instruction
 * \ingroup network
 */
enum tcpipParameterType {
  tcpipPDouble,                /*!< \brief a double parameter */
  tcpipPInteger,               /*!< \brief an integer parameter */
  tcpipPString,                /*!< \brief a string parameter */
  tcpipPTrue,                  /*!< \brief parameter true */
  tcpipPFalse,                 /*!< \brief parameter false */
  tcpipPAll                    /*!< \brief parameter all */
};

/** \brief possible instruction types
 * \ingroup network
 */
enum tcpipInstructionType {
  tcpipINone,                /*!< \brief not an instruction */
  tcpipIInstruction          /*!< \brief a standard instruction */
};

/** \brief one parameter of an instruction parsed by TCPIPinstructionParser
 * \ingroup network
 */
typedef struct {
  tcpipParameterType type;                /** \brief type of the parameter */
  long long intValue;                     /** \brief value of an integer parameter */
  double doubleValue;                     /** \brief value of a floating point parameter */
  bool boolValue;                         /** \brief value of a boolean parameter (true|false) */
  std::string stringValue;                /** \brief value of a string parameter */
} tcpip_parameter;

/** \brief one instruction parsed by TCPIPinstructionParser
 * \ingroup network
 */
typedef struct {
  tcpipInstructionType type;               /** \brief type of the instruction */
  std::string name;                        /** \brief name of the instruction */
  std::vector<tcpip_parameter> parameters; /** \brief a vector containing all parameters of the instruction */
} tcpip_instruction;

/** \brief possible result types of an instruction
 * \ingroup network
 */
enum tcpipResultType {
  tcpipRDouble,                /*!< \brief a double parameter */
  tcpipRInteger,               /*!< \brief an integer parameter */
  tcpipRString,                /*!< \brief a string parameter */
  tcpipRBoolean                /*!< \brief a boolean parameter */
};

/** \brief a single result parameter
 * \ingroup network
 */
typedef struct {
  tcpipResultType type;               /** \brief type of the result */
  long long intValue;                     /** \brief value of an integer parameter */
  double doubleValue;                     /** \brief value of a floating point parameter */
  bool boolValue;                         /** \brief value of a boolean parameter (true|false) */
  std::string stringValue;                /** \brief value of a string parameter */
} tcpip_result;

std::string tcpipToken2String(tcpipToken currentToken);

/**
 * \brief This class forms a parser that reads instructions characterwise and
 *     interprets them as simple inctructions (definition see below). This is
 *     just a parser, so it generates a simple list of the instructions. This
 *     class reads from a BaseServer object supplied to the constructor. It can
 *     also send data back to the client
 *
 * \ingroup network
 *
 * This class implements a simple <a href="http://en.wikipedia.org/wiki/Recursive_descent_parser">
 * recursive descent parser</a> for a very simple instruction language that is defined below in EBNF.
 * Both the tokenizer ( getToken() ) and the parser ( parse() and parse_instruction() )are hand-written
 * and should be fairly easy to understand. For more information about recursive descent parser have
 * a look into a book about compiler construction or into the introductory examples (math parser) in
 * this reference (very easy to understand if you already talk C++ or C):
 *
 *  <i>Bjarne Stroustrup</i> (2000): <b>The C++ Programming Language</b>, Addison-Wesley, 3rd
 *     edition, ISBN 0-201-70073-5
 *
 * <b>EBNF for instructions:</b>
 * \verbatim
instruction      ->  NAME ( parameter_list );
parameter_list   -> empty | primary | primary , parameter_list
primary          -> true | false | all
                  | NUMBER | " STRING " | ' STRING '
                  | NAME
NUMBER           -> number in scientific notation -1.234E-56
TEXT_WITHOUT_LB  -> all ascii characters except linebreak (LB) and carriage return (CR)
STRING           -> all ascii characters except linebreak (LB), carriage return (CR) and string delimiters (" or ')
NAME             -> ( A .. Z | a .. z | _ ) { A .. Z | a .. z | _ | 0 .. 9 | . }
WHITESPACE       -> LB | CR | SPACE | TAB \endverbatim
 *
 *
 * <b>Parameter and Instruction checking and handling capabilities:</b>
 * You can access the instruction received with these methods:
 *   - read_instruction_name()
 *   - read_instruction_argcount()
 * .
 * There are also some methods to read the parameters:
 *   - read_instruction_arg_double()
 *   - read_instruction_arg_int()
 *   - read_instruction_arg_string()
 *   - read_instruction_arg_bool()
 * .
 * This class also provides a set of functions to check the parameters sent in an instruction by the client.
 * To do so use one of these methods:
 *   - check_instruction_argcount()
 *   - check_instruction_argtype_bool()
 *   - check_instruction_argtype_string()
 *   - check_instruction_argtype_int()
 *   - check_instruction_argtype_double()
 * .
 *
 * <b>Sending function results back:</b>
 * This class also implements capabilities to send the result of a function back to the client. For this
 * it uses a simple format: It sends
 * \verbatim <parameter 1>\n<parameter 2>\n...<parameter last>\n\0x04\n \endverbatim
 * So after each single parameter (transmitted as ASCII) it sends a linebreak and after the last linebreak
 * a 0x04 and a 0x0A character sequence to signal the end of the transmission. This may cause problems with
 * strings that contain a linebreak character. So these character will have to be escaped using escapify()
 * from tools.h (see documentation there).
 * There is a very simple method to send a list of values. You simply supply a vector containing all the
 * values and they will be formatted in the right way and sent using the BaseServer object. The method is
 * called send_result() and the data to be sent will be stored in the private member variable results.
 * You can access this variable using clear_results(), add_result_double(), add_result_bool(), add_result_int()
 * and add_result_string().
 *
 *
 * <b>Standard Use of this Class:</b> Within the sequencer project this class is used for communication with MatLab.
 * For more details about the protocol, see \ref main_com_matlab .
 *
 */
class TCPIPinstructionParser
{
  protected:
    /** \brief the server object used for I/O */
    BaseServer* server;

    /** \brief the current Token */
    tcpipToken CurrentToken;

    /** \brief the currently readin string value */
    std::string stringValue;

    /** \brief the currently readin floating point value */
    double doubleValue;

    /** \brief the currently readin integer value */
    long long intValue;

    /** \brief type of the currently readin number (double or ineteger) */
    tcpipNumberType numberType;

    /** \brief the connection we are reading from */
    int connection;

    /** \brief Tokenizer: extract the next token from the input */
    tcpipToken getToken();

    /** \brief read characters until the given delimiter appears (usefull for reading in strings)
      *        if the given delimiter shall be used in the read string it has to be escaped with
      *        a preceding backslash \c '\'. */
    std::string readDelim(char delimiter);

    /** \brief read a number (integer or floating point). The methods assigns the value to
      *        doubleValue or intValue and sets numberType accordingly.
      */
    void readNumber(char first);

    /** \brief read a name. The methods assigns the value to stringValue
      */
    void readName(char first);

    /** \brief parse an instruction expression */
    void parse_instruction(bool get);

    /** \brief read timeout in seconds, standard is 1 second */
    int timeout;

    /** \brief a character that has been pushed back by some read function */
    char pushBackChar;

    /** \brief a vector containing all the results that will be sent by send_result(). */
    std::vector<tcpip_result> results;

    /** \brief this represents the instruction that was parsed. */
    tcpip_instruction instruction;
  public:
    /** \brief class constructor. Supply a pointer to a BaseServer object that will be used to send and receiva data. */
    TCPIPinstructionParser(BaseServer* s);

    /** \brief class destructor */
    virtual ~TCPIPinstructionParser();

    /** \brief return the timeout for reading and writing operations in seconds */
    GetMacro(int, timeout);
    /** \brief set the timeout for reading and writing operations in seconds */
    SetMacro(int, timeout);

    /** \brief returns the instruction with all its parameters as srting */
    std::string instruction_as_string();

    /** \brief parses instructions from the given connection. */
    void parse(int con);

    /** \brief send the return values, stored in results using the method outlined in the class documentation */
    void send_result(int con);

    /** \brief clear the list of return values */
    void clear_results();

    /** \brief add an integer return value */
    void add_result_int(long long data);

    /** \brief add a double return value */
    void add_result_double(double data);

    /** \brief add a boolean return value */
    void add_result_bool(bool data);

    /** \brief add a string return value */
    void add_result_string(std::string data);

    /** \brief set the return value to represent an error with the given message and error code
     *
     * An error message contains two return values:
     *  -# a negative integer number (default is -1) indicating an error
     *  -# an error message string
     * .
     * Note that all results already stored in \c results, i.e. in this object since the last call
     * of send_result() will be erased!
     */
    void write_result_error(std::string message, int code=-1);

    /** \brief check for the proper number of arguments (== \a count ) in the property \c instruction
     *
     * This methods checks whether the proper number of arguments has been supplied. If not, it generates an
     * error message with write_result_error() and returns \c false.
     */
    bool check_instruction_argcount(int count, LogFile* lf=NULL);

    /** \brief check for the proper number of arguments (>= \a count ) in the property \c instruction
     *
     * This methods checks whether the proper number of arguments has been supplied. If not, it generates an
     * error message with write_result_error() and returns \c false.
     */
    bool check_instruction_argcount_ormore(int count, LogFile* lf=NULL);

    /** \brief check for a boolean argument \c arg in the property \c instruction
     *
     * This methods checks whether the proper number of arguments has been supplied. If not, it generates an
     * error message with write_result_error() and returns \c false.
     *
     * This method returns true in one of these cases:
     *   - The argument is \c true or \c false
     *   - The argument is an integer number
     *   - The argument is a string which is \c "true" or \c "false"
     *  .
     *
     * If \c lf is provided an error message will be written to it.
     */
    bool check_instruction_argtype_bool(int arg, LogFile* lf=NULL);

    /** \brief check for a string argument \c arg in the property \c instruction
     *
     * If \c lf is provided an error message will be written to it.
     *
     */
    bool check_instruction_argtype_string(int arg, LogFile* lf=NULL);

    /** \brief check for an integer argument \c arg in the property \c instruction
     *
     * If \c lf is provided an error message will be written to it.
     *
     */
    bool check_instruction_argtype_int(int arg, LogFile* lf=NULL);

    /** \brief check for an integer or string argument \c arg in the property \c instruction
     *
     * This method is usefull e.g. if there are two possibilities to name something: by a number
     * or by a real name (string).
     *
     * If \c lf is provided an error message will be written to it.
     */
    bool check_instruction_argtype_intstring(int arg, LogFile* lf=NULL);

    /** \brief check for a floating-point argument \c arg in the property \c instruction
     *
     * This methods checks whether the proper number of arguments has been supplied. If not, it generates an
     * error message with write_result_error() and returns \c false.
     *
     * This method returns true in one of these cases:
     *   - The argument is an integer number
     *   - The argument is a double number
     *  .
     *
     * If \c lf is provided an error message will be written to it.
     */
    bool check_instruction_argtype_double(int arg, LogFile* lf=NULL);

    /** \brief read a floating-point number from the \c arg -th argument. This will return the value of a
     * floating-point or integer parameter. */
    double read_instruction_arg_double(int arg);

    /** \brief read an integer number from the \c arg -th argument */
    long long read_instruction_arg_int(int arg);

    /** \brief read a string number from the \c arg -th argument */
    std::string read_instruction_arg_string(int arg);

    /** \brief read an integer number from the \c arg -th argument
     *
     * This method will return \c true for:
     *    - an argument \c true
     *    - a non-zero integer number
     *    - a non-zero floating-point number
     *    - a string \c "true"
     * .
     * This method will return \c false for:
     *    - an argument \c false
     *    - a 0 integer number
     *    - a 0 floating-point number
     *    - a string \c "false"
     * .
     */
    bool read_instruction_arg_bool(int arg);

    /** \brief return the name of the received instruction */
    inline std::string read_instruction_name(){
      return instruction.name;
    };

    /** \brief return the type of the n-th argument */
    inline tcpipParameterType read_instruction_paramtype(int n){
      return instruction.parameters[n].type;
    };

    /** \brief return the number of arguments of the current instruction */
    inline int read_instruction_argcount() {
      return instruction.parameters.size();
    };

    /** \brief return the number of arguments of the current instruction */
    inline tcpipInstructionType read_instruction_type() {
      return instruction.type;
    };
};

}

#endif // TCPIPINSTRUCTIONPARSER_H
