/*
  Name: qfmathparser.h
  Copyright: (c) 2007-2013
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/

/*! \defgroup qf3lib_mathtools_parser parser for mathematical expressions
    \ingroup qf3lib_mathtools

 In this group there are classes that form a parser and evaluator for mathematical expressions.
 In the context of the sequencer program this is a tool class that can be used by the classes
 in the project. E.g. by jkiniparser which allows for mathematical expressions in the configuration
 files.

 */

/** \file jkmathparser.h
 *  \ingroup qf3lib_mathtools_parser
 */
#include "lib_imexport.h"
#include "qftools.h"
#include <cmath>
#include <QTextIStream>
#include <QTextStream>

#include <QList>
#include <QMap>
#include <QList>
#include <QString>
#include <QPair>



#ifndef QFMATHPARSER_H
#define QFMATHPARSER_H



/**
 * \defgroup qfmpmain main function parser class
 *  \ingroup qf3lib_mathtools_parser
 */
/*@{*/


/*! \brief A simple function parser to parse (build memory tree representation) and
           evaluate simple mathematical expressions
    \ingroup qf3lib_mathtools_parser
 This class implements a simple function parser which can parse
 mathematical expressions like <code> z=a*3+2.34^2*sin(pi*sqrt(x))</code> .
 More than one expression can be separated by semicolon ';'. The result of
 a parse operation will be a memory structure (tree) representing the given
 expression.
 The parser can cope with constants and (user defined) variables and supports
 the data types number (double precision floating point), boolean (true/false)
 and string. It already contains a lot of fundamental mathematical functions
 (i.e. nearly all the functions from C StdLib).

 \section qfmp_constantsAndVars constants and variables:
 This class provides management utilities for constants and variables. Variables
 can also be defined as external variables, when a pointer to corresponding
 memory is provided by the calling program. The parser supports a variable
 assign operation \code a=<expression>\endcode  which allows to define new
 variables during evaluation. There are some mathematical standard constants
 registered by calling jkMathParser::addStandardVariables():
   - \c pi = \f$ \pi \f$
   - \c e = \f$ \exp(1) \f$
   - \c sqrt2 = \f$ \sqrt{2} \f$
   - \c version = the parser version
   - \c log2e = \f$ \log_2(e) \f$
   - \c log10e = \f$ \log_{10}(e) \f$
   - \c ln2 = \f$ \ln(2)=\log_{e}(2) \f$
   - \c ln10 = \f$ \ln(10)=\log_{e}(10) \f$
   - \c h = \f$ 6.6260689633\cdot 10^{-34}\;\mathrm{J\cdot s} \f$ (planck constant)
   - \c hbar = \f$ \hbar=\frac{h}{2\pi}= 1.05457162853\cdot 10^{-34}\;\mathrm{J\cdot s} \f$ (planck constant)
   - \c epsilon0 = \f$ \epsilon_0= 8.854187817\cdot 10^{-12}\;\mathrm{\frac{F}{m}} \f$ (electric constant)
   - \c mu0 = \f$ \mu_0=2\pi\cdot 10^{-7}= 12.566370614\cdot 10^{-7}\;\mathrm{\frac{N}{A^2}} \f$ (magnetic constant)
   - \c c = \f$ 299792458\;\mathrm{\frac{m}{s}} \f$ (speed of light in vacuum)
   - \c ce = \f$ 1.60217648740\cdot 10^{-19}\;\mathrm{C}\f$ (elementary charge)
   - \c muB = \f$ \mu_B= 927.40091523\cdot 10^{-26}\;\mathrm{\frac{J}{T}} \f$ (Bohr magneton)
   - \c muB_eV = \f$ \mu_B=5.788381755579\cdot 10^{-5}\;\mathrm{\frac{eV}{T}}  \f$ (Bohr magneton)
   - \c muN = \f$ \mu_N=5.0507832413\cdot 10^{-27}\;\mathrm{\frac{J}{T}}  \f$ (nuclear magneton)
   - \c muN_eV = \f$ \mu_N=3.152451232645\cdot 10^{-8}\;\mathrm{\frac{eV}{T}}  \f$ (nuclear magneton)
   - \c me = \f$ m_e= 9.1093821545\cdot 10^{-31}\;\mathrm{kg} \f$ (mass of electron)
   - \c mp = \f$ m_p= 1.67262163783\cdot 10^{-27}\;\mathrm{kg} \f$ (mass of proton)
   - \c mn = \f$ m_n= 1.67492721184\cdot 10^{-27}\;\mathrm{kg} \f$ (mass of neutron)
   - \c NA = \f$ N_A= 6.0221417930\cdot 10^{23} \f$ (Avogadro constant = particles in 1 mol)
   - \c kB = \f$ k_B=1.380650424\cdot 10^{-23}\;\mathrm{\frac{J}{K}}  \f$ (Boltzman constant)
   - \c kB_eV = \f$ k_B=8.61734315\cdot 10^{-5}\;\mathrm{\frac{eV}{K}}  \f$ (Boltzman constant)
.
 You can add user-defined contants by calling jkMathParser::addVariableDouble()
 jkMathParser::addVariableBoolean() or jkMathParser::addVariableString()


 \section qfmp_functions functions:
 this class provides a wide range of (mathematical) functions:
   - sinc, gauss, slit, theta, tanc, sigmoid
   - asin, acos, atan, atan2,
   - sin, cos, tan,
   - sinh, cosh, tanh
   - log, log2, log10,
   - exp, sqr, sqrt
   - abs, sign
   - if
   - erf, erfc, lgamma, tgamma, j0, j1, jn, y0, y1, yn
   - rand, srand
   - ceil, floor, trunc, round,
   - fmod, min, max
   - floattostr, boolToQString
 .

 these functions are registere by calling jkMathParser::addStandardFunctions().
 you can add new functions by calling jkMathParser::addFunction();

 \section qfmp_resultsofparsing result of parsing and evaluation:
 The result of calling jkMathParser::parse()
 will be a tree-like structure in memory. The parse() function will return
 a pointer to the root node of this structure. All nodes inherit from
 qfmpNode class. To evaluate such a structure simply call qfmpNode::evaluate()
 of the root node. This will then return a qfmpResult structure which contains
 the result. This scheme allows for once parsing and multiply evaluating an expression.
 So if you want to define a function by an expression you can provide an
 external variable x as the argument and then evaluate the function for
 each x.

 \section qfmp_ebnf EBNF definition of the parsed expressions

<pre> logical_expression ->  logical_term
                      | logical_expression <b>or</b> logical_term
                      | logical_expression <b>||</b> logical_term</pre>
<pre> logical_term    ->  comp_expression
                      | logical_term <b>and</b> comp_expression
                      | logical_term <b>&amp;&amp;</b> comp_expression</pre>
<pre> comp_expression  ->  math_expression
                      | expression <b>==</b> math_expression
                      | expression <b>!=</b> math_expression
                      | expression <b>&gt;=</b> math_expression
                      | expression <b>&lt;=</b> math_expression
                      | expression <b>&gt;</b> math_expression
                      | expression <b>&lt;</b> math_expression</pre>
<pre>  math_expression ->  term
                      | math_expression <b>+</b> math_term
                      | math_expression <b>-</b> math_term</pre>
<pre>  math_term       ->  primary
                      | term <b>*</b> primary
                      | term <b>/</b> primary
                      | term ( <b>%</b> | <b>mod</b> ) primary</pre>
<pre>  primary         ->  <b>true</b> | <b>false</b>
                      | string_constant
                      | NUMBER
                      | NAME
                      | NAME <b>=</b> logical_expression
                      | <b>+</b> primary | <b>-</b> primary | <b>!</b> primary | <b>not</b> primary
                      | <b>(</b> logical_expression <b>)</b>
                      | NAME<b>(</b> parameter_list <b>)</b>
                      | primary <b>^</b> primary</pre>
<pre>  string_constant -> <b>&quot;</b> STRING <b>&quot;</b> | <b>&apos;</b> STRING <b>&apos;</b></pre>
<pre>  parameter_list  ->  \f$ \lambda \f$ | logical_expression | logical_expression <b>,</b> parameter_list</pre>




  \section qfmp_example Simple Example of Usage
     \code
        QFMathParser mp; // instanciate
        qfmpNode* n;
        qfmpResult r;
        // parse some numeric expression
        n=mp.parse("pi^2+4*sin(65*pi/exp(3.45))");
        r=n->evaluate();
        cout<<r.num<<endl;
        //delete n;

        // parse some boolean expression
        n=mp.parse("true==false");
        if (!mp.hasErrorOccured()) {
            r=n->evaluate();
            if (r.type==qfmpBool) {
                if (r.boolean) cout<<"true";
                else cout<<"false";
            }
            if (r.type==qfmpDouble) cout<<r.num<<endl;
            if (r.type==qfmpString) cout<<r.str<<endl;
            delete n;
        }
        if (mp.hasErrorOccured()) {
            qDebug()<<"ERROR: "<<mp.getLastError();
        }

        // parse some string expression
        n=mp.parse("var1='false'; var1+'true'");
        if (!mp.hasErrorOccured()) {
            r=n->evaluate();
            if (r.type==qfmpString) cout<<r.str<<endl;
            delete n;
        }
        if (mp.hasErrorOccured()) {
            qDebug()<<"ERROR: "<<mp.getLastError();
        }
     \endcode



 */
class QFLIB_EXPORT QFMathParser
{
    protected:

        /** \brief the possible tokens that can be recognized by the tokenizer in jkMathParser::getToken() */
        enum qfmpTokenType {
            END,                /*!< \brief end token */
            PRINT,              /*!< \brief a semicolon ';' */
            PARAMETER_DIV,      /*!< \brief a comma ',' between two function parameters */
            STRING_DELIM,       /*!< \brief a string delimiter ' or " */
            NAME,               /*!< \brief a name (consisting of characters) of a variable or function */
            NUMBER,             /*!< \brief a number in scientific notation */
            PLUS,               /*!< \brief a plus operator '+' */
            MINUS,              /*!< \brief a minus operator '-' */
            MUL,                /*!< \brief a multiplication operator '*' */
            DIV,                /*!< \brief a division operator '/' */
            MODULO,             /*!< \brief a modulo operator '%' */
            ASSIGN,             /*!< \brief a variable assignment = */
            LBRACKET,           /*!< \brief left brackets '(' */
            RBRACKET,           /*!< \brief right brackets ')' */
            POWER,              /*!< \brief a power operator '^' */
            FACTORIAL_LOGIC_NOT, /*!< \brief a factorial operator or a logical NOT '!' */
            LOGIC_NOT,          /*!< \brief a logical NOT '!' / 'not' */
            LOGIC_AND,          /*!< \brief a logical AND operator '&&' / 'and' */
            LOGIC_OR,           /*!< \brief a logical OR operator '||' / 'or' */
            LOGIC_XOR,          /*!< \brief a logical XOR operator 'xor' */
            LOGIC_NOR,          /*!< \brief a logical NOR operator 'nor' */
            LOGIC_NAND,         /*!< \brief a logical NAND operator 'nand' */
            LOGIC_TRUE,         /*!< \brief 'true' */
            LOGIC_FALSE,        /*!< \brief  'false' */
            COMP_EQUALT,        /*!< \brief equals operation '==' */
            COMP_UNEQUAL,       /*!< \brief unequal operation '!=' */
            COMP_GREATER,       /*!< \brief greater than operation '>' */
            COMP_SMALLER,       /*!< \brief smaller than operation '<' */
            COMP_GEQUAL,        /*!< \brief greater than or equal operation '>=' */
            COMP_SEQUAL,        /*!< \brief smaller than or equal operation '<=' */
        };



        /** \brief internal names for logic operations */
        enum {
          qfmpLOPand='a',
          qfmpLOPor='o',
          qfmpLOPxor='x',
          qfmpLOPnor='n',
          qfmpLOPnand='A',
          qfmpLOPnot='n'
        };


        /** \brief qfmpCOMPdefs internal names for compare operations */
        enum {
            qfmpCOMPequal='=',
            qfmpCOMPnequal='!',
            qfmpCOMPlesser='<',
            qfmpCOMPgreater='>',
            qfmpCOMPlesserequal='a',
            qfmpCOMPgreaterequal='b'
        };
        /*@}*/

    public:
        /**
         * \defgroup qfmpultil utilities for jkMathParser function parser class
         * \ingroup qf3lib_mathtools_parser
         */
        /*@{*/

        /** possible result types
         *    - \c qfmpDouble: a floating-point number with double precision. This is
         *                     also used to deal with integers
         *    - \c qfmpString: a string of characters
         *    - \c qfmpBool:   a boolean value true|false
         *  .
         */
        enum qfmpResultType {qfmpDouble,  /*!< \brief a floating-point number with double precision. This is also used to deal with integers */
                             qfmpString,  /*!< \brief a string of characters */
                             qfmpBool};   /*!< \brief a boolean value true|false */



        /** \brief result of any expression  */
        struct QFLIB_EXPORT qfmpResult {
          qfmpResult();

          bool isValid;
          qfmpResultType type;   /*!< \brief type of the result */
          QString str;       /*!< \brief contains result if \c type==qfmpString */
          double num;            /*!< \brief contains result if \c type==qfmpDouble */
          bool boolean;          /*!< \brief contains result if \c type==qfmpBool */

          /** \brief convert the value this struct representens into a QString */
          inline QString toString();

          /** \brief convert the value this struct representens into a QString and adds the name of the datatype in \c [...] */
          inline QString toTypeString();
        };

        qfmpResult getInvalidResult();


        /** \brief This struct is for managing variables. Unlike qfmpResult this struct
          * only contains pointers to the data
          */
        struct QFLIB_EXPORT qfmpVariable {
          qfmpVariable();
          qfmpResultType type;     /*!< \brief type of the variable */
          bool internal;           /*!< \brief this is an internal variable */
          QString *str;        /*!< \brief this points to the variable data if \c type==qfmpString */
          double *num;             /*!< \brief this points to the variable data if \c type==qfmpDouble */
          bool *boolean;           /*!< \brief this points to the variable data if \c type==qfmpBool */
        };

        /** \brief This struct is for managing temporary variables. It is generally like qfmpVariable.
          */
        struct QFLIB_EXPORT qfmpTempVariable {
          QString name;       /*!< \brief name of the variable */
          qfmpResultType type;    /*!< \brief type of the variable */
          bool internal;          /*!< \brief this is an internal variable */
          QString *str;       /*!< \brief this points to the variable data if \c type==qfmpString */
          double *num;            /*!< \brief this points to the variable data if \c type==qfmpDouble */
          bool *boolean;          /*!< \brief this points to the variable data if \c type==qfmpBool */
        };



        /** \brief This is a function prototype for adding new mathematical functions
         *         to the parser
         *
         * If you want to add more math functions (like sin, cos , abs ...) to the
         * parser, you will have to implement it with this prototype and then register
         * it with jkMathParser::addFunction(). The first parameter points to an array
         * containing the input parameters while the second one specifies the number
         * of supplied parameters. The result has to be of type qfmpResult.
         *
         * All error handling has to be done inside the function definition. Here is a
         * simple example:
         * \code
         * qfmpResult Abs(qfmpResult* params, unsigned char n){
         *   qfmpResult r;
         *   r.type=qfmpDouble;
         *   if (n!=1) qfmpError("abs accepts 1 argument");
         *   if (params[0].type!=qfmpDouble) qfmpError("abs needs double argument");
         *   r.num=fabs(params[0].num);
         *   return r;
         * }
         * \endcode
         */
        typedef qfmpResult (*qfmpEvaluateFunc)(qfmpResult*, unsigned char, QFMathParser*);


        /** \brief description of a user registered function */
        struct QFLIB_EXPORT qfmpFunctionDescriptor {
          qfmpEvaluateFunc function;    /*!< \brief a pointer to the function implementation */
          QString name;             /*!< \brief name of the function */
        };


        /*@}*/


        /**
         * \defgroup qfmpNodes memory representation of expressions
         * \ingroup qf3lib_mathtools_parser
         */
        /*@{*/

        /**
         * \brief This class is the abstract base class for nodes.
         * All allowed node types must inherit from qfmpNode
         */
        class QFLIB_EXPORT qfmpNode {
          protected:
            QFMathParser* parser;  /*!< \brief points to the parser object that is used to evaluate this node */
            qfmpNode* parent;      /*!< \brief points to the parent node */
          public:
            /** \brief virtual class destructor */
            virtual ~qfmpNode() {};

            /** \brief evaluate this node */
            virtual qfmpResult evaluate()=0;

            /** \brief return a pointer to the jkMathParser  */
            inline QFMathParser* getParser(){ return parser; };

            /** \brief set the jkMathParser  */
            inline void setParser(QFMathParser* mp){ parser=mp; };

            /** \brief returns a pointer to the parent node */
            inline qfmpNode* getParent(){ return parent; };

            /** \brief sets the parent node  */
            inline void setParent(qfmpNode* par) { parent=par; };
        };


        /**
         * \brief This class represents a binary arithmetic operation:
         *  add (+), subtract (-), multiply (*), divide (/), a to the power of b (a^b)
         */
        class QFLIB_EXPORT qfmpBinaryArithmeticNode: public qfmpNode {
          private:
            qfmpNode* left, *right;
            char operation;
          public:
            /** \brief constructor for a qfmpBinaryArithmeticNode
             *  \param op the operation to be performed: add (+), subtract (-), multiply (*), divide (/), a to the power of b (a^b)
             *  \param l left child node/operand
             *  \param r right child node/operand
             *  \param p a pointer to a jkMathParser object
             *  \param par a pointer to the parent node
             */
            qfmpBinaryArithmeticNode(char op, qfmpNode* l, qfmpNode* r, QFMathParser* p, qfmpNode* par);

            /** \brief standard destructor, also destroy the children (recursively)  */
            ~qfmpBinaryArithmeticNode() { delete left; delete right;};

            /** \brief evaluate this node */
            virtual qfmpResult evaluate();
        };

        /**
         * \brief This class represents a binary boolean operation: and, or, xor, nor, nand
         */
        class QFLIB_EXPORT qfmpBinaryBoolNode: public qfmpNode {
          private:
            qfmpNode* left, *right;
            char operation;
          public:
            /** \brief constructor for a qfmpBinaryBoolNode
             *  \param op the operation to be performed: (a)nd, (o)r, (x)or, (n)or, nand (A)
             *  \param l left child node/operand
             *  \param r right child node/operand
             *  \param p a pointer to a jkMathParser object
             *  \param par a pointer to the parent node
             */
            qfmpBinaryBoolNode(char op, qfmpNode* l, qfmpNode* r, QFMathParser* p, qfmpNode* par);

            /** \brief standard destructor, also destroy the children (recursively)  */
            ~qfmpBinaryBoolNode() { delete left; delete right;};

            /** \brief evaluate this node */
            virtual qfmpResult evaluate();
        };

        /**
         * \brief This class represents a binary compare operation: !=, ==, >=, <=, >, <
         */
        class QFLIB_EXPORT qfmpCompareNode: public qfmpNode {
          private:
            qfmpNode* left, *right;
            char operation;
          public:
            /** \brief constructor for a qfmpCompareNode
             *  \param op the operation to be performed: != (!), == (=), >= (b), <= (a), (>), (<)
             *  \param l left child node/operand
             *  \param r right child node/operand
             *  \param p a pointer to a jkMathParser object
             *  \param par a pointer to the parent node
             */
            qfmpCompareNode(char op, qfmpNode* l, qfmpNode* r, QFMathParser* p, qfmpNode* par);

            /** \brief standard destructor, also destroy the children (recursively)  */
            ~qfmpCompareNode () { delete left; delete right;};

            /** \brief evaluate this node */
            virtual qfmpResult evaluate();
        };

        /**
         * \brief This class represents a unary operations: ! (bool negation), - (arithmetic negation)
         */
        class QFLIB_EXPORT qfmpUnaryNode: public qfmpNode {
          private:
            qfmpNode* child;
            char operation;
          public:
            /** \brief constructor for a qfmpUnaryNode
             *  \param op the operation to be performed: (!), (-)
             *  \param c child node/operand
             *  \param p a pointer to a jkMathParser object
             *  \param par a pointer to the parent node
             */
            qfmpUnaryNode(char op, qfmpNode* c, QFMathParser* p, qfmpNode* par);

            /** \brief standard destructor, also destroy the children (recursively)  */
            ~qfmpUnaryNode() {delete child;};

            /** \brief evaluate this node */
            virtual qfmpResult evaluate();
        };

        /**
         * \brief This class represents a variable assignment (a = expression)
         */
        class QFLIB_EXPORT qfmpVariableAssignNode: public qfmpNode {
          private:
            qfmpNode* child;
            QString variable;
            char operation;
          public:
            /** \brief standard destructor, also destroy the children (recursively)  */
            ~qfmpVariableAssignNode() {delete child;};

            /** \brief constructor for a qfmpVariableAssignNode
             *  \param var name of the variable to assign to
             *  \param c child node/right-hand-side expression
             *  \param p a pointer to a jkMathParser object
             *  \param par a pointer to the parent node
             */
            qfmpVariableAssignNode(QString var, qfmpNode* c, QFMathParser* p, qfmpNode* par);

            /** \brief evaluate this node */
            virtual qfmpResult evaluate();
        };

        /**
         * \brief This class represents a number, a string contant or a boolean contant (true/false)
         */
        class QFLIB_EXPORT qfmpConstantNode: public qfmpNode {
          private:
            qfmpResult data;
          public:
            /** \brief constructor for a qfmpConstantNode
             *  \param d the value of the constant
             *  \param p a pointer to a jkMathParser object
             *  \param par a pointer to the parent node
             */
            qfmpConstantNode(qfmpResult d, QFMathParser* p, qfmpNode* par) { data=d; setParser(p); setParent(par); };

            /** \brief evaluate this node */
            virtual qfmpResult evaluate() { return data; };
        };

        /**
         * \brief This class represents a variable.
         */
        class QFLIB_EXPORT qfmpVariableNode: public qfmpNode {
          private:
            QString var;
          public:
            /** \brief constructor for a qfmpVariableNode
             *  \param name name of the variable
             *  \param p a pointer to a jkMathParser object
             *  \param par a pointer to the parent node
             */
            qfmpVariableNode(QString name, QFMathParser* p, qfmpNode* par);

            /** \brief evaluate this node */
            virtual qfmpResult evaluate();
        };


        /**
         * \brief This class represents a an invalid node that always resturns an invalid value
         */
        class QFLIB_EXPORT qfmpInvalidNode: public qfmpNode {
          public:
            /** \brief constructor for a qfmpConstantNode
             *  \param d the value of the constant
             *  \param p a pointer to a jkMathParser object
             *  \param par a pointer to the parent node
             */
            qfmpInvalidNode(QFMathParser* p, qfmpNode* par) { setParser(p); setParent(par); };

            /** \brief evaluate this node */
            virtual qfmpResult evaluate() { return parser->getInvalidResult(); };
        };

        /**
         * \brief This class represents an arbitrary function.
         *
         * When initialized this class will get the function description that is
         * linked to the supplied function name from jkMathParser object. This
         * information is saved locally and won't be changed when evaluating!
         *
         * Functions may have 8 parameters at the most.
         */
        class QFLIB_EXPORT qfmpFunctionNode: public qfmpNode {
          private:
            QString fun;
            qfmpNode** child;
            unsigned char n;
            qfmpEvaluateFunc function;
          public:
            /** \brief constructor for a qfmpFunctionNode
             *  \param name name of the function
             *  \param c a pointer to an array of qfmpNode objects that represent the parameter expressions
             *  \param num number of children in c
             *  \param p a pointer to a jkMathParser object
             *  \param par a pointer to the parent node
             */
            qfmpFunctionNode(QString name, qfmpNode** c, unsigned char num, QFMathParser* p, qfmpNode* par);

            /** \brief standard destructor, also destroy the children (recursively) */
            ~qfmpFunctionNode();

            /** \brief evaluate this node */
            virtual qfmpResult evaluate();
        };

        /**
         * \brief This class represents a list of qfmpNode.
         *
         * when evaluating the result will be the result of the last node in the list.
         */
        class QFLIB_EXPORT qfmpNodeList: public qfmpNode {
          private:
            QList<qfmpNode*> list;
          public:
            /** \brief constructor for a qfmpNodeList
             *  \param p a pointer to a jkMathParser object
             */
            qfmpNodeList(QFMathParser* p) { setParser(p); setParent(NULL); };

            /** \brief standard destructor, also destroy the children (recursively) */
            ~qfmpNodeList();

            /** \brief add a qfmpNode n to the list */
            void add(qfmpNode* n);

            /** \brief evaluate the node */
            virtual qfmpResult evaluate();

            /** \brief get the number of nodes in the list */
            int getCount() {return list.size();};
        };

        /*@}*/


    public:
        /**
         * \defgroup qfmpErrorhandling error handling
         * \ingroup qf3lib_mathtools_parser
         */
        /*@{*/






        /** \brief macro that throws an exception or calls an error handler */
        void qfmpError(QString st);


        /*@}*/

    protected:
        /** \brief return the given token as human-readable string */
        QString tokentostring(qfmpTokenType token);

        /** \brief return the current token as human-readable string */
        QString currenttokentostring();

		/** \brief Tokenizer: extract the next token from the input */
        qfmpTokenType getToken();

		/** \brief return a delimited text, i.e. extract the texte between the delimiters <code>"</code> in: of <code>"Hallo!"</code>, i.e. returns <code> Hallo!</code>
         *         This is used to parse string constants.
         *
         * This functions actually reads pascal style delimited string constants. So if you want to use the delimiter as part of the string you will have to
         * write it as doubled character. So <code>'Jan''s Test'</code> stands for <code>Jan's Test</code>.
         */
        QString readDelim(QChar delimiter);

		/** \brief recognizes an compExpression while parsing. If \a get ist \c true, this function first retrieves a new token by calling getToken() */
        qfmpNode* compExpression(bool get);

		/** \brief recognizes a logicalExpression while parsing. If \a get ist \c true, this function first retrieves a new token by calling getToken() */
        qfmpNode* logicalExpression(bool get);

		/** \brief recognizes a logicalTerm while parsing. If \a get ist \c true, this function first retrieves a new token by calling getToken() */
        qfmpNode* logicalTerm(bool get);

		/** \brief recognizes a mathExpression while parsing. If \a get ist \c true, this function first retrieves a new token by calling getToken() */
        qfmpNode* mathExpression(bool get);

		/** \brief recognizes a term while parsing. If \a get ist \c true, this function first retrieves a new token by calling getToken() */
        qfmpNode* mathTerm(bool get);

		/** \brief recognizes a primary while parsing. If \a get ist \c true, this function first retrieves a new token by calling getToken() */
        qfmpNode* primary(bool get);

		/** \brief this stream is used to read in the program. An object is created and assigned
  		 * (and destroyed) by the parse()-function */
        QTextStream * program;
        QString progStr;

        /** \brief vector containing all temporary variables */
        QList<qfmpTempVariable> tempvariables;

        /** \brief map to manage all currently defined variables */
        QMap<QString, qfmpVariable> variables;

        /** \brief map to manage all currently rtegistered functions */
        QMap<QString, qfmpFunctionDescriptor> functions;

        /** \brief the current token while parsing a string */
        qfmpTokenType CurrentToken;

        /** \brief the string value of the current token (when applicable) during the parsing step */
        QString StringValue;

        /** \brief the string value of the current token (when applicable) during the parsing step */
        double NumberValue;

        /** \brief set the defining struct of the given variable */
        void setVariable(QString name, qfmpResult value);
        /** \brief set the defining struct of the given variable */
        void setVariableDouble(QString name, double value);


        /**\brief  adds a temporary variable */
        void addTempVariable(QString name, qfmpResult value);

        QStringList lastError;
        int errors;

        QChar peekStream(QTextStream* stream);
        void putbackStream(QTextStream* stream, QChar ch);
        bool getFromStream(QTextStream* stream, QChar& ch);

	public:
		/**\brief class constructor */
        QFMathParser();

		/**\brief class destructor */
        virtual ~QFMathParser();

        /** \brief returns whether an error has occured since the last call to resetError() */
        bool hasErrorOccured() const;
        /** \brief retuns the last error */
        QString getLastError() const;
        /** \brief return a list of the last errors */
        QStringList getLastErrors() const;
        /** \brief returns the number of errors since the last call to resetErrors() */
        int getLastErrorCount();
        /** \brief resets the error state to no errors */
        void resetErrors();


		/**\brief  register a new function
		 * \param name name of the new function
		 * \param function a pointer to the implementation
		 */
        void addFunction(QString name, qfmpEvaluateFunc function);

		/**\brief  register a new external variable of type double
		 * \param name name of the new variable
		 * \param v pointer to the variable memory
		 */
        void addVariableDouble(QString name, double* v);

		/**\brief  register a new external variable of type string
		 * \param name name of the new variable
		 * \param v pointer to the variable memory
		 */
        void addVariableString(QString name, QString* v);

		/**\brief  register a new external variable of type boolean
		 * \param name name of the new variable
		 * \param v pointer to the variable memory
		 */
        void addVariableBoolean(QString name, bool* v);


		/**\brief  register a new internal variable of type double
		 * \param name name of the new variable
		 * \param v initial value of this variable
		 */
        void addVariableDouble(QString name, double v);

		/**\brief  register a new internal variable of type string
		 * \param name name of the new variable
		 * \param v initial value of this variable
		 */
        void addVariableString(QString name, QString v);

        /**\brief  register a new internal variable of type boolean
         * \param name name of the new variable
         * \param v initial value of this variable
         */
        void addVariableBoolean(QString name, bool v);

        /**\brief  register a new internal variable of type boolean
         * \param name name of the new variable
         * \param v initial value of this variable
         */
        void addVariable(QString name, qfmpResult result);



        /**\brief  returns the value of the given variable */
        qfmpResult getVariable(QString name);
        /**\brief  returns the value of the given variable */
        qfmpResult getVariableOrInvalid(QString name);

        /**\brief  returns the defining structure of the given variable */
        qfmpVariable getVariableDef(QString name);


        /**\brief  evaluates a registered function
         * \param name name of the (registered function) to be evaluated
         * \param params array of the input parameters
         * \param n number of input parameters (<=8)
         */
        qfmpResult evaluateFunction(QString name, qfmpResult* params, unsigned char n);

        /**\brief  returns the defining structure of the given function */
        qfmpEvaluateFunc getFunctionDef(QString name);

        /**\brief  tests whether a temporary variable exists */
        inline bool tempvariableExists(QString name){
          if (tempvariables.size()<=0)  return false;
          for (int i=tempvariables.size()-1; i>=0; i--) {
            if (tempvariables[i].name==name) return true;
          }
          return false;
        };

        /**\brief  tests whether a variable exists */
        inline bool variableExists(QString name){ return tempvariableExists(name)||(variables.find(name)!=variables.end()); };

        /**\brief  tests whether a function exists */
        inline bool functionExists(QString name){ return !(functions.find(name)==functions.end()); };

        /**\brief  deletes all defined variables. the memory of internal variables
         * will be released. the external memory will not be released.
         */
        void clearVariables();

        /**\brief  delete the specified variabale and releases its internal memory.*/
        void deleteVariable(QString name);

        /**\brief  clears the list of internal functions*/
        inline void clearFunctions() {functions.clear();};

        /**\brief  registers standard variables*/
        void addStandardVariables();

        /**\brief  registers standard functions*/
        void addStandardFunctions();

        /**\brief  parses the given expression*/
        qfmpNode* parse(QString prog);

        /** \brief evaluate the given expression */
        qfmpResult evaluate(QString prog);

        /**\brief  prints a list of all registered variables */
        QString printVariables();

        QList<QPair<QString, qfmpVariable> > getVariables();

};

#endif // QFMATHPARSER_H
/*@}*/
