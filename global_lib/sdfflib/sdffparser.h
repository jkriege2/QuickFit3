/*
  Name: sdffparser.h
  Copyright: (c) 2007
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/

/** \file sdffparser.h
 * \ingroup hlseqgen_sdff
 *
 */

#ifndef SDFFPARSER_H
#define SDFFPARSER_H

#include <string>
#include <sstream>
#include <fstream>
#include <cmath>
#include <cstdio>
#include <stdint.h>




#include "tools.h"
#include "datatable.h"
#include "sequencererror.h"
#include "channelmanager.h"
#include "sequencegenerator.h"
#include "baseimporter.h"
#include "sdfftools.h"
#include "sdfffunctions.h"
#include "sdffnodes.h"
#include "sdffsequenceelements.h"
#include "sdffhwcommands.h"
#include "logfile.h"
#include "basesequenceplotter.h"

#include "./mersenne/MersenneTwister.h"


namespace sequencer {

/** \brief the possible tokens for the tokenizer in SDFFparser
 * \ingroup hlseqgen_sdff
 */
enum sdff_token {
	sdffTEND,                /*!< \brief end token */
	sdffTPRINT,              /*!< \brief a semicolon ';' */
	sdffTKOMMA,              /*!< \brief a comma ',' between two function parameters */
	sdffTSTRING,             /*!< \brief a string delimiter ' or " has been found ... the string is read in*/
	sdffTNAME,               /*!< \brief a name (consisting of characters) of a variable or function */
	sdffTDOUBLE,             /*!< \brief a number in scientific notation */
	sdffTINTEGER,            /*!< \brief an integer number */
	sdffTPLUS,               /*!< \brief a plus operator '+' */
	sdffTMINUS,              /*!< \brief a minus operator '-' */
	sdffTMUL,                /*!< \brief a multiplication operator '*' */
	sdffTDIV,                /*!< \brief a division operator '/' */
	sdffTMODULO,             /*!< \brief a modulo operator '%' or 'mod' */
	sdffTASSIGN,             /*!< \brief a variable assignment = */
	sdffTLPARANTHESIS,       /*!< \brief left brackets '(' */
	sdffTRPARANTHESIS,       /*!< \brief right brackets ')' */
	sdffTLBRACKET,           /*!< \brief left brackets '[' */
	sdffTRBRACKET,           /*!< \brief right brackets ']' */
	sdffTLBRACE,             /*!< \brief left brackets '{' */
	sdffTRBRACE,             /*!< \brief right brackets '}' */
	sdffTPOWER,              /*!< \brief a power operator '^' */
	sdffTINVERT,             /*!< \brief a invert token '~' */
	sdffTLOGIC_NOT,          /*!< \brief a logical NOT '!' / 'not' */
	sdffTLOGIC_AND,          /*!< \brief a logical AND operator '&' / 'and' */
	sdffTLOGIC_OR,           /*!< \brief a logical OR operator '|' / 'or' */
	sdffTLOGIC_XOR,          /*!< \brief a logical XOR operator 'xor' */
	sdffTLOGIC_NOR,          /*!< \brief a logical NOR operator 'nor' */
	sdffTLOGIC_NAND,         /*!< \brief a logical NAND operator 'nand' */
	sdffTLOGIC_SHL,          /*!< \brief a logical shift left operator 'shl'/'<<' */
	sdffTLOGIC_SHR,          /*!< \brief a logical shift right operator 'shr'/'>>' */
	sdffTLOGIC_TRUE,         /*!< \brief 'true' */
	sdffTLOGIC_FALSE,        /*!< \brief  'false' */
	sdffTCOMP_EQUAL,         /*!< \brief equals operation '==' */
	sdffTCOMP_UNEQUAL,       /*!< \brief unequal operation '!=' */
	sdffTCOMP_GREATER,       /*!< \brief greater than operation '>' */
	sdffTCOMP_SMALLER,       /*!< \brief smaller than operation '<' */
	sdffTCOMP_GEQUAL,        /*!< \brief greater than or equal operation '>=' */
	sdffTCOMP_SEQUAL,        /*!< \brief smaller than or equal operation '<=' */
	sdffTTDOUBLE,            /*!< \brief a type specifier 'double' */
	sdffTTINTEGER,           /*!< \brief a type specifier 'integer' */
	sdffTTSTRING,            /*!< \brief a type specifier 'string' */
	sdffTTBOOLEAN,           /*!< \brief a type specifier 'boolean' */
	sdffTCONST,              /*!< \brief keyword 'const' */
	sdffTIF,                 /*!< \brief keyword 'if' */
	sdffTELSEIF,             /*!< \brief keyword 'elseif' */
	sdffTELSE,               /*!< \brief keyword 'else' */
	sdffTDO,                 /*!< \brief keyword 'do' */
	sdffTREPEAT,             /*!< \brief keyword 'repeat' */
	sdffTWHILE,              /*!< \brief keyword 'while' */
	sdffTUNTIL,              /*!< \brief keyword 'until' */
	sdffTPARAMETERS,         /*!< \brief keyword 'parameters' */
	sdffTSEQUENCE,           /*!< \brief keyword 'sequence' */
	sdffTEVENT,              /*!< \brief keyword 'event' */
	sdffTPREEVENT,           /*!< \brief keyword 'pre_event' */
	sdffTLASTEVENT,          /*!< \brief keyword 'last_event' */
	sdffTCHANNEL,            /*!< \brief keyword 'channel' */
	sdffTCHANNELNAMES,       /*!< \brief keyword 'channel_names' */
	sdffTRELATIVEEVENT,      /*!< \brief keyword 'relative_event' */
	sdffTSAFESETTINGS        /*!< \brief keyword 'safe_settings' */
};

/** \brief convert the given sdff_token into a string
 * \ingroup hlseqgen_sdff
 */
std::string sdff_tokentostr(sdff_token t);

/** \brief convert the given sdff_token into a string with a more user-friendly output than sdff_tokentostr().
 * \ingroup hlseqgen_sdff
 */
std::string asdff_tokentostr(sdff_token t);


/**
 * \brief this class implements a parser for Sequence Definition File Format files. This parser is capable of generating
 * sequences from such files and also implements a debugger for them.
 * \ingroup hlseqgen_sdff
 *
 * This class incorporates two main parts: A parser that can parse SDFF sequences and a sequence generator that can
 * interpret these parsed sequences and generate a sequence from them.
 *
 * \section sdff_parser_section Parser
 * The parser for SDFF files in this class is a <a href="http://en.wikipedia.org/wiki/Recursive_descent_parser">
 * recursive descent parser</a>. Those parsers may parse LL(1) gramatics by representing every production rule as a
 * function or procedure. The grammatic of SDFF is quite complex and so I merged more than one production in one
 * function. This keeps the code a bit more clean. The parser consists of several parts:
 *
 *  - get_token() is a tokenizer that reads one token from the input stream sdff. It will then store the type of the
 *    token and possibly also the data that is represented by the token in the private data members current_token,
 *    current_string (represents the value of string constants and names that appear in the program), current_double,
 *    current_integer and current_boolean. This tokenizer also swallows comments that begin with \c # and end with the
 *    end of line or C-style comments <tt>/*..* /</tt> .
 *  - The main parser routine is sdff_file(). It represents a production like this (<b>bold</b> are terminal characters):\n
 *<pre> sdff_file        ->  { <b>parameters {</b> expression_seq <b>}</b>
 *                      | <b>sequence {</b> event_seq <b>}</b>
 *                      | <b>pre_event {</b> expression_seq <b>}</b>
 *                      | <b>last_event {</b> expression_seq <b>}</b>
 *                      | <b>safe_settings {</b> expression_seq <b>}</b> }*</pre>
 *    It then calls expression_seq() or event_seq() to parse the contents of those blocks. These methods will have to
 *    recognize the last '}' and stop if they find it. sdff_file() will then swallow this brace!
 *  - expression_seq() reads a sequence of instructions. So the production for this is:\n
 *       <pre> expression_seq   ->  { instruction }*</pre>
 *    This method will call instruction() to recognize/parse an instruction.
 *    This method ends when it finds a '}' that belongs to then initial '{'. It will not swallow this '}' as this is done
 *    in the calling method (most commonly sdff_file() )!
 *  - event_seq() reads a sequence of instructions like expression_seq(). But additionally this sequence may contain
 *    \c event, \c relative_event and \c pre_event blocks. This is used to parse a \c sequence block. So the production for this is:\n
 *<pre> event_seq        ->  { <b>event(</b> math_expression <b>){</b> expression_seq <b>}</b>
 *                       | <b>relative_event(</b> math_expression <b>){</b> expression_seq <b>}</b>
 *                       | <b>channel(</b> math_expression <b>){</b> expression_seq <b>}</b>
 *                       | <b>if (</b> logical_expression <b>) {</b> event_seq <b>}</b>
 *                       | <b>if (</b> logical_expression <b>) {</b> event_seq <b>} else {</b> event_seq <b>}</b>
 *                       | <b>if (</b> logical_expression <b>) {</b> event_seq <b>}</b> { <b>elseif (</b> logical_expression <b>) {</b> event_seq <b>}</b> }+ <b>else {</b> event_seq <b>}</b>
 *                       | <b>while (</b> logical_expression <b>) {</b> expression_seq <b>}</b>
 *                       | <b>do {</b> expression_seq <b>} while (</b> logical_expression <b>)</b>
 *                       | <b>do {</b> expression_seq <b>} until (</b> logical_expression <b>)</b>
 *                       | logical_expression <b>;</b>
 *                     }*</pre>
 *    This method will call instruction() to recognize/parse an instruction, math_expression() to parse the time-parameter of
 *    \c event blocks and expression_seq() to parse the contents of \c event blocks.
 *    This method ends when it finds a '}' that belongs to then initial '{'. It will not swallow this '}' as this is done
 *    in the calling method (most commonly sdff_file() )!
 *  - instruction() will parse an instruction (variable assignment, function call, calculatione ...) or a statement
 *    (if, while and other loops). It represents a production like this:
 *<pre> instruction      ->  logical_expression <b>;</b>
 *                     | <b>if (</b> logical_expression <b>) {</b> expression_seq <b>}</b>
 *                     | <b>if (</b> logical_expression <b>) {</b> expression_seq <b>} else {</b> expression_seq <b>}</b>
 *                     | <b>if (</b> logical_expression <b>) {</b> expression_seq <b>}</b> { <b>elseif (</b> logical_expression <b>) {</b> expression_seq <b>}</b> }+ <b>else {</b> expression_seq <b>}</b>
 *                     | <b>while (</b> logical_expression <b>) {</b> expression_seq <b>}</b>
 *                     | <b>do {</b> expression_seq <b>} while (</b> logical_expression <b>)</b>
 *                     | <b>do {</b> expression_seq <b>} until (</b> logical_expression <b>)</b>
 *                     | <b>channel_names {</b>  { NAME<b>,</b> NAME[<b>,</b> string_constant] <b>;</b> }* <b>}</b></pre>
 *    Note that this method swallows the last character of the instruction, i.e. ';' for instructions or '}' for statements.
 *    Note also that the \c channel_names instruction may only be used inside a \c parameters block. This is not implemented
 *    by use of a context-free grammar, but by checking whether this condition applies context-sensitively.
 *  - logical_expression() represents a logic operation or any other expression (represented by
 *    logical_term() ). The representatio of logical operators is split to accomplish the operator precedence of
 *    AND, NAND over OR, XOR, NOR. The represented production is:
 *<pre> logical_expression ->  logical_term
 *                       | logical_expression ( <b>or</b> | <b>|</b> ) logical_term
 *                       | logical_expression <b>xor</b> logical_term
 *                       | logical_expression <b>nor</b> logical_term</pre>
 *  - logical_term() represents a multiplication-like logic operation or any other expression (represented by
 *    comp_expression() ). The representatio of logical operators is split to accomplish the operator precedence of
 *    AND, NAND over OR, XOR, NOR. The represented production is:
 *<pre> logical_term       ->  comp_expression
 *                       | logical_term ( <b>shl</b> | <b>&lt;&lt;</b> ) math_expression
 *                       | logical_term ( <b>shl</b> | <b>&gt;&gt;</b> ) math_expression
 *                       | logical_term ( <b>and</b> | <b>&amp;</b> ) comp_expression
 *                       | logical_term <b>nand</b> comp_expression</pre>
 *  - comp_expression() represents a comparison of two (or more) values or any other expression (represented by
 *    math_expression() ). The represented production is:
 *<pre> comp_expression  ->  math_expression
 *                     | expression <b>==</b> math_expression
 *                     | expression <b>!=</b> math_expression
 *                     | expression <b>&gt;=</b> math_expression
 *                     | expression <b>&lt;=</b> math_expression
 *                     | expression <b>&gt;</b> math_expression
 *                     | expression <b>&lt;</b> math_expression</pre>
 *  - math_expression() represents a mathematical operation or any other expression (represented by
 *    term() ). The mathematical operations are split into two productions (\c math_expression and \c term), as this
 *    allows to represent the precedence of multiplikation and divison over addition and subtraction.
 *    The represented production is:
 *<pre>  math_expression ->  term
 *                     | math_expression <b>+</b> term
 *                     | math_expression <b>-</b> term</pre>
 *  - term() represents a mathematical operation or any other expression (represented by
 *    primary() ). The mathematical operations are split into two productions (\c math_expression and \c term), as this
 *    allows to represent the precedence of multiplikation and divison over addition and subtraction.
 *    The represented production is:
 *<pre>  term            ->  primary
 *                     | term <b>*</b> primary
 *                     | term <b>/</b> primary
 *                     | term ( <b>%</b> | <b>mod</b> ) primary</pre>
 * .
 *  - primary() represents the lowest possible production. It contains variable names, function calls, parantheses
 *    and some unary operations (negation), as well as the power operation, as \c 2^2*3^2 should be calculated as
 *    \f$ 2^3\cdot3^2 \f$ and not \f$ 2^{3\cdot 3^2} \f$, i.e. taking the power has the highest operator precedence!
 *    The represented production is:
 *<pre>  primary         ->  <b>true</b> | <b>false</b>
 *                     | string_constant
 *                     | NUMBER
 *                     | NAME
 *                     | NAME <b>=</b> logical_expression
 *                     | CHANNEL_NAME <b>[</b> logical_expression <b>=</b> logical_expression
 *                     | <b>+</b> primary | <b>-</b> primary | <b>!</b> primary | <b>not</b> primary | <b>~</b> primary
 *                     | <b>(</b> logical_expression <b>)</b>
 *                     | NAME<b>(</b> parameter_list <b>)</b>
 *                     | NAME<b>(</b> name_list <b>)=</b> expression
 *                     | NAME<b>(</b> name_list <b>)= {</b> expression_seq <b>};</b>
 *                     | primary <b>^</b> primary
 * string_constant -> <b>&quot;</b> STRING <b>&quot;</b> | <b>&apos;</b> STRING <b>&apos;</b>
 * parameter_list  ->  \f$ \lambda \f$ | logical_expression | logical_expression <b>,</b> parameter_list
 * name_list       ->  \f$ \lambda \f$ | NAME | NAME <b>,</b> name_list</pre>
 * .
 *
 * From this we can extract an operator precedence table:
 * <center><table border=1 cellspacing=0>
 *   <tr>
 *     <td><b>precedence level</b></td>
 *     <td><b>operators</b></td>
 *   </tr>
 *   <tr>
 *     <td><b>0 (execute first)</b></td>
 *     <td>unary negatio operators <tt>- ~ not</tt>\n
 *         power operator \c ^ \n
 *         variable assignment
 *     </td>
 *   </tr>
 *   <tr>
 *     <td><b>1</b></td>
 *     <td>multiplication like operators <tt>* / % mod</tt>
 *     </td>
 *   </tr>
 *   <tr>
 *     <td><b>2</b></td>
 *     <td>summation like, binary operators <tt>+ -</tt>
 *     </td>
 *   </tr>
 *   <tr>
 *     <td><b>3</b></td>
 *     <td>comparison operators <tt>== != &lt;= &gt;= &lt; &gt; </tt>
 *     </td>
 *   </tr>
 *   <tr>
 *     <td><b>4</b></td>
 *     <td>logical multiplication like operators <tt>and nand shl shr</tt>
 *     </td>
 *   </tr>
 *   <tr>
 *     <td><b>5 (execute last)</b></td>
 *     <td>logical summation like operators <tt>or xor nor</tt>
 *   </tr>
 * </table></center>
 *
 * This results in the following interpretation of expressions:
 *   - first multiply and then sum: <tt>3+4*5</tt> = <tt>3+(4*5)</tt>
 *   - first do and operatiosn \c and then \c or (i.e. \c and is like multiplikation and \c or like summation):
 *     <tt>a and b or c and d</tt> = <tt>(a and b) or (c and d)</tt>
 *   - compare values first and then do the logical operations, so <tt>a==4 and b==5</tt> is equal to
 *     <tt>(a==4) and (b==5)</tt>.
 * .
 *
 * \section sdff_seqgeneration_section Sequence Generation
 *
 * When evaluating the sequence some of the functions in sdfffunctions.h may create lists of SDFFsequenceElement
 * for each channel that represent a basic form of the sequence. There are different flavors of those elements, i.e.
 * child classes, that implement different types of sequence elements. Such a sequence element may be a ramp, or simply
 * the change of a value. Each element acts on exactly one channel and has an exact start time (\c t_start property).
 * There is also a duration to each element that may also be 0, e.g. for set elements. Each element may also contain
 * more data that describes the element (e.g. a \f$ \tau \f$ parameter for exponential ramps).
 *
 * With such lists of elements SDFFparser may do two things:
 *  -# it can test for consistency. This is mainly a collision check, i.e. two sequence elements may not write to the same
 *     point in time.
 *  -# it can generate a sequence. For this the data (meta information) stored in the elements will be written to a
 *     sequence generator object.
 * .
 *
 * The list are stored in a SDFFelementManager object which is a protected member of SDFFparser. This class also implements
 * the methods mentioned above.
 *
 * \section sdff_example An Example Sequence
 *
 * This is an example sequence definition file:
 *   \htmlinclude test2sdff.html
 *
 * And this is the sequence generated by this file:
 *   \image html test2sdff_result.png
 *
 * \section sdff_markers marker system
 * You can use special commands to define markers inside a SDFF sequence. To be able to show them in a plot of the sequence
 * they have to be stored somwhere. This is done in the markers_param and markers_seq properties of this class. You can
 * access them with clear_markers(), get_markers_count() and get_marker().
 *
 * The markers are internally separated between markers generated during the parameters section and markers generated during
 * sequence section (current position is determined by the inside_sequence property). This separation is transparent in the
 * external interface as described above. But this separation is essential to prevent markers that were generated in former
 * evaluations of the sequence. The reset() and import() methods already clear the markers in the appropriate way:
 *  -# import() clears all markers and starts to write markers into markers_param, as only the parameter section is evaluated.
 *  -# reset() also clears all markers an regenerates the ones from the parameters section (this section is evaluated anew)
 *  -# generate() only clears the former sequence markers (in markers_seq) as the sequence is overwritten. It keeps the
 *     markers from the parameters section.
 * .
 *
 *
 *
 */
class SDFFparser: public baseImporter {
  protected:
    /** \brief this is a mersenne twister random number generator that can be used by SDFF functions
     *         (see implementation in sdfffunctions.h). You can access this generator by using get_mersenne().
     *
     *
     * The code is taken from <a href="http://www-personal.umich.edu/~wagnerr/MersenneTwister.html">http://www-personal.umich.edu/~wagnerr/MersenneTwister.html</a>.
     *
     * This service is implemented here to allow for the use of random numbers in SDFF functions. The generator is
     * seeded on class creation with the current system time. Here is the copyright-notice from the source code of the
     * freely available generator:
     * \verbatim
// MersenneTwister.h
// Mersenne Twister random number generator -- a C++ class MTRand
// Based on code by Makoto Matsumoto, Takuji Nishimura, and Shawn Cokus
// Richard J. Wagner  v1.0  15 May 2003  rjwagner@writeme.com

// The Mersenne Twister is an algorithm for generating random numbers.  It
// was designed with consideration of the flaws in various other generators.
// The period, 2^19937-1, and the order of equidistribution, 623 dimensions,
// are far greater.  The generator is also fast; it avoids multiplication and
// division, and it benefits from caches and pipelines.  For more information
// see the inventors' web page at http://www.math.keio.ac.jp/~matumoto/emt.html

// Reference
// M. Matsumoto and T. Nishimura, "Mersenne Twister: A 623-Dimensionally
// Equidistributed Uniform Pseudo-Random Number Generator", ACM Transactions on
// Modeling and Computer Simulation, Vol. 8, No. 1, January 1998, pp 3-30.

// Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
// Copyright (C) 2000 - 2003, Richard J. Wagner
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//   1. Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//
//   2. Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//
//   3. The names of its contributors may not be used to endorse or promote
//      products derived from this software without specific prior written
//      permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// The original code included the following notice:
//
//     When you use this, send an email to: matumoto@math.keio.ac.jp
//     with an appropriate reference to your work.
//
// It would be nice to CC: rjwagner@writeme.com and Cokus@math.washington.edu
// when you write.
\endverbatim
     */
    MTRand* mersenne;

    /** \brief holds all the variables in this object */
    SDFFVariableManager* vmanager;

    /** \brief holds all usable functions in this object */
    SDFFFunctionManager* fmanager;

    /** \brief a LogFile object that can be used for output instructions in the SDFF seuqnece */
    LogFile* log;

    /** \brief a list of markers, generated by the sequence during parameters section */
    std::vector<sp_marker> markers_param;

    /** \brief a list of markers, generated by the sequence during sequence section */
    std::vector<sp_marker> markers_seq;

    /** \brief current filename */
    std::string current_filename;

    /** \brief urrent line number */
    long current_line;

    /** \brief the current time when executing */
    double current_time;

    /** \brief the current channel when inside a channel-block */
    std::string current_channel;

    /** \brief duration of one sample in microseconds */
    double sample_timestep;

    /** \brief the contents of the \c parameters block */
    SDFFlistNode* parameters;

    /** \brief the contents of the \c sequence block */
    SDFFlistNode* sequence;

    /** \brief the contents of the \c safe_settings block */
    SDFFlistNode* safe_settings;

    /** \brief all \c last_event blocks are combined into one \c last_event block.
      *        This points to this block. */
    SDFFlastEventNode* last_event;

    /** \brief all \c pre_event blocks are combined into one \c pre_event block.
      *        This points to this block. */
    SDFFpreEventNode* pre_event;

    /** \brief type of the current token */
    sdff_token current_token;

    /** \brief string value of the current token (if meaningful) */
    std::string current_string;

    /** \brief double value of the current token (if meaningful) */
    double current_double;

    /** \brief integer value of the current token (if meaningful) */
    long long current_integer;

    /** \brief tokenizer: reads one token from sdff and returns the token type.
     *         This also sets current_token, current_string, surrent_double and current_integer.
     *         For more information, see \link SDFFparser documentation \endlink
     *
     * \note
     *    This method will ignore all comments, i.e. it will cut them out of the input stream by treating
     *    them as white space.
     */
		sdff_token get_token();

    /** \brief read from sdff until you reach the given delimiter character. The delimiter character is NOT returned.
     *
     * \note
     *         This method will not cut comments out! So we assume that we are in something like a string constant
     *         Actually string constants will be the main use case for this function.
     */
		std::string read_delim(char delimiter);

    /** \brief read a number (double or integer) from sdff. This function returns the token type that was read (sdffTDOUBLE
     *         or sdffTINTEGER). It reads number specified in the format for read_number_ex() (i.e. with an optional unit
     *         character).
     */
		sdff_token read_number();

	/** \brief this stream is used to read in the SDFF file (or string). It is created and deleted by the
     *         parse()-function and the import()-function.
     */
	std::istream* sdff;

	/** \brief <b>parser:</b> parse a \c sdff_file element
	 *         \a get specifies whether to read a new token at the begin of the procedure.
     *         For more information, see \link SDFFparser documentation \endlink
	 */
	void sdff_file(bool get);

	/** \brief <b>parser:</b> parse a \c block element that contains an expression and event sequence: \c sequence.
     *         Add the found nodes to \a list.
	 *         \a get specifies whether to read a new token at the begin of the procedure.
     *         For more information, see \link SDFFparser documentation \endlink
     */
	void event_seq(bool get, SDFFnaryNode* list);

	/** \brief <b>parser:</b> identify one of the control structure (if and loops)
     *         For more information, see \link SDFFparser documentation \endlink
     */
	void control_stmt(bool get, SDFFnaryNode* list);

	/** \brief <b>parser:</b> parse a \c block element that contains an expression sequence: \c parameters, \c safe_settings.
     *         This is also used by event_seq(). Add the found nodes to \a list.
	 *         \a get specifies whether to read a new token at the begin of the procedure.
     *         For more information, see \link SDFFparser documentation \endlink
     */
	void expression_seq(bool get, SDFFnaryNode* list);

    /** \brief <b>parser:</b> parse an instruction (if, loops, expression)
	 *         \a get specifies whether to read a new token at the begin of the procedure.
     *         For more information, see \link SDFFparser documentation \endlink
     */
	SDFFnode* instruction(bool get);

	/** \brief <b>parser:</b> parse a comp_expression
	 *         \a get specifies whether to read a new token at the begin of the procedure.
     *         For more information, see \link SDFFparser documentation \endlink
     */
	SDFFnode* comp_expression(bool get);

	/** \brief <b>parser:</b> parse a math_expression
	 *         \a get specifies whether to read a new token at the begin of the procedure.
     *         For more information, see \link SDFFparser documentation \endlink
     */
	SDFFnode* math_expression(bool get);

	/** \brief <b>parser:</b> parse a logical_expression
	 *         \a get specifies whether to read a new token at the begin of the procedure.
     *         For more information, see \link SDFFparser documentation \endlink
     */
	SDFFnode* logical_expression(bool get);

	/** \brief <b>parser:</b> parse a logical_term
	 *         \a get specifies whether to read a new token at the begin of the procedure.
     *         For more information, see \link SDFFparser documentation \endlink
     */
	SDFFnode* logical_term(bool get);

	/** \brief <b>parser:</b> parse a term
	 *         \a get specifies whether to read a new token at the begin of the procedure.
     *         For more information, see \link SDFFparser documentation \endlink
     */
	SDFFnode* term(bool get);

	/** \brief <b>parser:</b> parse a primary
	 *         \a get specifies whether to read a new token at the begin of the procedure.
     *         For more information, see \link SDFFparser documentation \endlink
     *
     * \todo add function checking, as implemented in the check functions in sdfffunctions.h and sdfffunctions.cpp
     */
	SDFFnode* primary(bool get);

	/** \brief <b>parser:</b> This is a special version of expression_seq() which ends without an
	 *         error if the end of the input is reached. This is used by evaluate() and evaluate_parameters().
     */
	void expression_seq_e(bool get, SDFFnaryNode* list);

    /** \brief this object manages the sequence elements generated by a SDFF file. It is mainly used in generate() */
    SDFFelementManager* emanager;

    /** \brief this object manages all special output commands, like serial interface or GPIB outputs */
    SDFFHWcommandManager* cmd_manager;

    /** \brief determines whether we are inside an event or relative_event block */
    bool inside_event;

    /** \brief determines whether we are inside a pre_event block */
    bool inside_preevent;

    /** \brief determines whether we are inside a last_event block */
    bool inside_lastevent;

    /** \brief determines whether we are inside a channel block */
    bool inside_channel;

    /** \brief determines whether we execute the parameter section (false) or the sequence section (true) of a SDFF file */
    bool inside_sequence;

    /** \brief determines whether we are currently inside a parameters{..} block */
    bool inside_parameters;

    bool debug;

    /** \brief this map is used to store channel aliases */
    std::map<std::string, sdff_channel_alias_data> channel_aliases;

  public:
    /** \brief class constructor. \c sg is a pointer to a sequencer::sequenceGenerator object that will be used by the class */
    SDFFparser(sequenceGenerator* sg, channelManager* cm);
    /** \brief class destructor */
    virtual ~SDFFparser();

    /** \brief get the number of channel aliases */
    inline size_t get_alias_count() { return channel_aliases.size(); };

    /** \brief returns \c true if an alias for the given channel name exists */
    inline bool alias_exists(std::string alias) { return channel_aliases.find(alias)!=channel_aliases.end(); };

    /** \brief returns \c true if the supplied \a name is a valid channel name or channel alias */
    inline bool is_valid_channel_name(std::string name) {
        if (cmanager->exists(name)) return true;
        std::map<std::string, sdff_channel_alias_data>::iterator it=channel_aliases.find(name);
        if (it==channel_aliases.end())
          return false;
        return cmanager->exists(it->second.channel);
    };

    /** \brief returns the real channel name for an alias name or the argument \a alias if you already supplied a channel name */
    inline std::string resolve_alias(std::string alias) {
        if (cmanager->exists(alias)) return alias;
        std::map<std::string, sdff_channel_alias_data>::iterator it=channel_aliases.find(alias);
        if (it==channel_aliases.end())
          SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_ALIASNOTEXISTS_NUM, "SDFFparser::resolve_alias('"+alias+"')", alias.c_str());
        return it->second.channel;
    };

    /** \brief returns the complete alias information for the given alias name */
    inline sdff_channel_alias_data get_alias_info(std::string alias) {
        std::map<std::string, sdff_channel_alias_data>::iterator it=channel_aliases.find(alias);
        if (it==channel_aliases.end())
          SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_ALIASNOTEXISTS_NUM, "SDFFparser::get_alias_info('"+alias+"')", alias.c_str());
        return it->second;
    };

    /** \brief returns the complete alias information for the given channel name */
    sdff_channel_alias_data get_alias_info_for_channel(std::string channel);

    /** \brief returns the complete alias information for the given item (specified by number) */
    sdff_channel_alias_data get_alias_info(size_t n);

    /** \brief clear all channel aliases */
    inline void clear_aliases() { channel_aliases.clear(); };

    /** \brief add a new channel alias */
    inline void add_alias(std::string channel, std::string alias, std::string info=std::string("")) {
        sdff_channel_alias_data d;
        d.channel=channel;
        d.alias=alias;
        d.info=info;
        channel_aliases[alias]=d;
    };

    /** \brief add a set of standard functions to the function manager. */
    void add_standard_functions();

    /** \brief add a set of standard constants
     *
     * the constants are:
     *   - \c pi = \f$ \pi= 3.14159265358979323846 \f$
     *   - \c sqrt2 = \f$ \sqrt{2}= 1.41421356237309504880 \f$
     *   - \c log2e = \f$ \log_2(e)= 1.4426950408889634074 \f$
     *   - \c log10e = \f$ \log_{10}(e)= 0.43429448190325182765 \f$
     *   - \c ln2 = \f$ \ln(2)= 0.69314718055994530942 \f$
     *   - \c ln10 = \f$ \ln(10)= 2.30258509299404568402 \f$
     * .
     */
    void add_standard_constants();

    /** \brief add a set of standard physics constants. The data is taken from <a href="http://physics.nist.gov/cuu/Constants/">
     *         NIST</a>
     *
     * the constants are:
     *   - \c h = \f$ 6.6260689633\cdot 10^{-34}\;\mathrm{J\cdot s} \f$ (planck constant)
     *   - \c hbar = \f$ \hbar=\frac{h}{2\pi}= 1.05457162853\cdot 10^{-34}\;\mathrm{J\cdot s} \f$ (planck constant)
     *   - \c epsilon0 = \f$ \epsilon_0= 8.854187817\cdot 10^{-12}\;\mathrm{\frac{F}{m}} \f$ (electric constant)
     *   - \c mu0 = \f$ \mu_0=2\pi\cdot 10^{-7}= 12.566370614\cdot 10^{-7}\;\mathrm{\frac{N}{A^2}} \f$ (magnetic constant)
     *   - \c c = \f$ 299792458\;\mathrm{\frac{m}{s}} \f$ (speed of light in vacuum)
     *   - \c ce = \f$ 1.60217648740\cdot 10^{-19}\;\mathrm{C}\f$ (elementary charge)
     *   - \c muB = \f$ \mu_B= 927.40091523\cdot 10^{-26}\;\mathrm{\frac{J}{T}} \f$ (Bohr magneton)
     *   - \c muB_eV = \f$ \mu_B=5.788381755579\cdot 10^{-5}\;\mathrm{\frac{eV}{T}}  \f$ (Bohr magneton)
     *   - \c muN = \f$ \mu_N=5.0507832413\cdot 10^{-27}\;\mathrm{\frac{J}{T}}  \f$ (nuclear magneton)
     *   - \c muN_eV = \f$ \mu_N=3.152451232645\cdot 10^{-8}\;\mathrm{\frac{eV}{T}}  \f$ (nuclear magneton)
     *   - \c me = \f$ m_e= 9.1093821545\cdot 10^{-31}\;\mathrm{kg} \f$ (mass of electron)
     *   - \c mp = \f$ m_p= 1.67262163783\cdot 10^{-27}\;\mathrm{kg} \f$ (mass of proton)
     *   - \c mn = \f$ m_n= 1.67492721184\cdot 10^{-27}\;\mathrm{kg} \f$ (mass of neutron)
     *   - \c NA = \f$ N_A= 6.0221417930\cdot 10^{23} \f$ (Avogadro constant = particles in 1 mol)
     *   - \c kB = \f$ k_B=1.380650424\cdot 10^{-23}\;\mathrm{\frac{J}{K}}  \f$ (Boltzman constant)
     *   - \c kB_eV = \f$ k_B=8.61734315\cdot 10^{-5}\;\mathrm{\frac{eV}{K}}  \f$ (Boltzman constant)
     * .
     */
    void add_physics_constants();

    /** \brief clear the function manager */
    inline void clear_functions() { fmanager->clear(); };

    /** \brief clear the variable manager */
    inline void clear_variables() {vmanager->clear(); };

    /** \brief return the current filename the class is working on */
    GetMacro(std::string, current_filename);

    /** \brief return the current line number */
    GetMacro(long, current_line);
    /** \brief set the current line number */
    SetMacro(long, current_line);

    /** \brief return the \c sample_timestep */
    GetMacro(double, sample_timestep);


    /** \brief return the current time */
    GetMacro(double, current_time);
    /** \brief set the current time */
    SetMacro(double, current_time);

    /** \brief return the current channel */
    GetMacro(std::string, current_channel);
    /** \brief set the current channel */
    SetMacro(std::string, current_channel);

    /** \brief set \c inside_event property */
    SetMacro(bool, inside_event);
    /** \brief return \c inside_event property */
    GetMacro(bool, inside_event);

    /** \brief set \c inside_parameters property */
    SetMacro(bool, inside_parameters);
    /** \brief return \c inside_event property */
    GetMacro(bool, inside_parameters);

    /** \brief set inside_preevent property */
    SetMacro(bool, inside_preevent);
    /** \brief return inside_preevent property */
    GetMacro(bool, inside_preevent);

    /** \brief set inside_preevent property */
    SetMacro(bool, inside_lastevent);
    /** \brief return inside_preevent property */
    GetMacro(bool, inside_lastevent);

    /** \brief set \c inside_channel property */
    SetMacro(bool, inside_channel);
    /** \brief return \c inside_channel property */
    GetMacro(bool, inside_channel);

    /** \brief return \c inside_sequence property */
    GetMacro(bool, inside_sequence);

    /** \brief return a pointer to the mersenne twister random number generator mersenne */
    GetMacro(MTRand*, mersenne);

    /** \brief return a pointer to the sequence element manager */
    GetMacro(SDFFelementManager*, emanager);

    /** \brief return a pointer to the SDFF hardware command manager */
    GetMacro(SDFFHWcommandManager*, cmd_manager);

    /** \brief return a pointer to the function manager */
    GetMacro(SDFFFunctionManager*, fmanager);

    /** \brief return a pointer to the variable manager */
    GetMacro(SDFFVariableManager*, vmanager);

    /** \brief set a LogFile object for output in SDFF sequences */
    SetMacro(LogFile*, log);
    /** \brief get the LogFile object for output in SDFF sequences */
    GetMacro(LogFile*, log);

    GetSetMacro(bool, debug);

    /** \brief returns the number of markers generated by the sequence */
    inline int get_marker_count() { return markers_param.size()+markers_seq.size(); };

    /** \brief clear all markers */
    inline void clear_markers() { markers_param.clear(); markers_seq.clear(); }

    /** \brief return the <i>n</i>-th marker */
    inline sp_marker get_marker(unsigned int n) {
      if (n>=markers_param.size()) {
        return markers_seq[n-markers_param.size()];
      } else {
        return markers_param[n];
      }
    };

    /** \brief add a new marker at the appropriate position */
    inline void add_marker(sp_marker m) {
      if (inside_sequence) {
        markers_seq.push_back(m);
      } else {
        markers_param.push_back(m);
      }
    };

    /** \brief parses a sdff file and generstes a memory tree from it.
     *
     * \return 0 when there were no warnings and errors, -1 when there were errors and +1 when there were
     *         warning but no errors
     *
     * This method will parse a SDFF file and generate an internal tree from the file. This will then evaluate
     * the parameters section from the file only! So after calling import() you can change parameters and so on.
     * This class will not create a sequence until a call if generate().
     *
     * This method will generate entries in the events vector, so you can read this afterwards to check
     * the errors that occured during parsing
     *
     *
     */
    int import(std::string filename);

    /** \brief this will generate a temporary memory tree from the given \a program and then execute it.
     *         The result is returned in \a result. You can use this to read some parameters.
     *
     * \note This method assumes that \a program can be parsed as a \c expression_seq. So there may not be
     *         any sections (event, paremeters ...) in here, just plain instructions. The result will always
     *         be the result of the last instruction!
     * \note This method executes the instructions in the given \a program in a new variables scope. So you can
     *       not use this to add global variables. All new variables defined in here will be deleted after
     *       execution!
     */
    int evaluate(std::string program, sdff_result* result=NULL);

    /** \brief this will generate a temporary memory tree from the given \a program and then execute it.
     *         The result is returned in \a result. After evaluating the temporary tree, this tree will be
     *         added to the parameters section if \a add \c =true. You can use this to set parameters after
     *         a call of import().
     *
     * \note This method assumes that \a program can be parsed as a \c expression_seq. So there may not be
     *         any sections (event, paremeters ...) in here, just plain instructions. The result will always
     *         be the result of the last instruction!
     * \note This method executes the instructions in the given \a program in the global variables scope. So
     *       you can use this to add global variables. All new variables defined in here will be saved as
     *       global variables and won't be deleted after execution.
     */
    int evaluate_parameters(std::string program, sdff_result* result=NULL, bool add=true);

    /** \brief generate a memory sequence from the data in this object. \a sample_timestep gives the period between two
     *         samples. This value will be set in the sequenceGenerator object accessed by this method/class. If
     *         \c timestep=-1 then the timestep from the sequenceGenerator is used (default).
     *
     * \return 0 when there were no warnings and errors, -1 when there were errors and +1 when there were
     *         warning but no errors
     *
     * This method will generate entries in the events vector, so you can read this afterwards to check
     * the errors that occured during parsing
     *
     */
    int generate(double sample_timestep=-1);

    /** \brief clears the variable and function manager objects and then executes the parameters section (with additions
     *         meanwhile made by evaluate_parameters() instructions). This can be used if you don't want to reload the
     *         sequence, but want to regenerate it with different parameters.
     */
    int reset();

    /** \brief print out the stored memory trees */
    std::string report_contents();

    /** \brief generates a string that contains the currently loaded SDFF code (regenerated from the memory tree)
     *
     * This method can be used to document a sequence. If you call it after some evaluate() calls the newly set global
     * variables will be included into the parameters section (if you set \a params_with_current_setting \c = \c true ).
     */
    std::string get_current_sdff_code(bool params_with_current_setting=true);

};

}
#endif // SDFFPARSER_H


/*@}*/
