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
#include <QStack>
#include "../extlibs/MersenneTwister.h"
#include <stdint.h>


#ifndef QFMATHPARSER_H
#define QFMATHPARSER_H



/**
 * \defgroup qfmpmain main function parser class
 *  \ingroup qf3lib_mathtools_parser
 */
/*@{*/

/*! \brief This macro allows to easily define functions for QFMathParser from a C-function that
           has one numeric argument, e.g. sin(x)

    The resulting function will:
      - check the number of arguments
      - apply the C-function to any number parameter
      - apply the C-function item-wise to any number vector parameter
    .

    \param FName name of the function to declare
    \param NAME_IN_PARSER name the function should have in the parser (used for error messages only)
    \param CFUNC name of the C function to call
*/
#define QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC(FName, NAME_IN_PARSER, CFUNC) \
QFMathParser::qfmpResult FName(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){\
    QFMathParser::qfmpResult r;\
    r.type=QFMathParser::qfmpDouble;\
    if (n!=1) {\
        p->qfmpError(QObject::tr("%1(...) needs exacptly 1 argument").arg(#NAME_IN_PARSER));\
        r.isValid=false;\
        return r;\
    }\
    if (params[0].type==QFMathParser::qfmpDouble) {\
        r.num=CFUNC(params[0].num);\
    } else if(params[0].type==QFMathParser::qfmpDoubleVector) {\
        r.type=QFMathParser::qfmpDoubleVector;\
        r.numVec.resize(params[0].numVec.size());\
        for (int i=0; i<params[0].numVec.size(); i++) {\
            r.numVec[i]=CFUNC(params[0].numVec[i]);\
        }\
    } else {\
        p->qfmpError(QObject::tr("%1(...) argument has to be a number or vector of numbers").arg(#NAME_IN_PARSER));\
        r.isValid=false;\
    }\
    return r;\
}

/** \brief same as QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC(FName, NAME_IN_PARSER, CFUNC), but NAME_IN_PARSER==CFUNC */
#define QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC_SIMPLE(FName, CFUNC) QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC(FName, CFUNC, CFUNC)



/*! \brief This macro allows to easily define functions for QFMathParser from a C-function that
           has one numeric vector argument and returns a vector

    The resulting function will:
      - check the number of arguments and their type
      - apply the C-function to any number parameter
    .

    \param FName name of the function to declare
    \param NAME_IN_PARSER name the function should have in the parser (used for error messages only)
    \param CFUNC name of the C function to call
*/
#define QFMATHPARSER_DEFINE_1PARAM_NUMERICVEC_FUNC(FName, NAME_IN_PARSER, CFUNC) \
QFMathParser::qfmpResult FName(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){\
    QFMathParser::qfmpResult r;\
    r.type=QFMathParser::qfmpDouble;\
    if (n!=1) {\
        p->qfmpError(QObject::tr("%1(x) needs exacptly 1 argument").arg(#NAME_IN_PARSER));\
        r.isValid=false;\
        return r;\
    }\
    if(params[0].type==QFMathParser::qfmpDoubleVector) {\
        r.type=QFMathParser::qfmpDoubleVector;\
        r.numVec=CFUNC(params[0].numVec);\
    } else {\
        p->qfmpError(QObject::tr("%1(x) argument has to be vector of numbers").arg(#NAME_IN_PARSER));\
        r.isValid=false;\
    }\
    return r;\
}

/*! \brief This macro allows to easily define functions for QFMathParser from a C-function that
           has one numeric vector argument and returns a number, e.g. qfstatisticsMedian()

    The resulting function will:
      - check the number of arguments and their type
      - apply the C-function to the argument
    .

    \param FName name of the function to declare
    \param NAME_IN_PARSER name the function should have in the parser (used for error messages only)
    \param CFUNC name of the C function to call
*/
#define QFMATHPARSER_DEFINE_1PARAM_VECTONUM_FUNC(FName, NAME_IN_PARSER, CFUNC) \
QFMathParser::qfmpResult FName(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){\
    QFMathParser::qfmpResult r;\
    r.type=QFMathParser::qfmpDouble;\
    if (n!=1) {\
        p->qfmpError(QObject::tr("%1(x) needs exacptly 1 argument").arg(#NAME_IN_PARSER));\
        r.isValid=false;\
        return r;\
    }\
    if(params[0].type==QFMathParser::qfmpDoubleVector) {\
        r.type=QFMathParser::qfmpDouble;\
        r.num=CFUNC(params[0].numVec);\
    } else {\
        p->qfmpError(QObject::tr("%1(x) argument has to be a vector of numbers").arg(#NAME_IN_PARSER));\
        r.isValid=false;\
    }\
    return r;\
}

/*! \brief This macro allows to easily define functions for QFMathParser from a C-function that
           has one numeric vector argument and one numeric argument and returns a number, e.g. qfstatisticsQuantile()

    The resulting function will:
      - check the number of arguments and their type
      - apply the C-function to the argument
    .

    \param FName name of the function to declare
    \param NAME_IN_PARSER name the function should have in the parser (used for error messages only)
    \param CFUNC name of the C function to call
*/
#define QFMATHPARSER_DEFINE_2PARAM1VEC_VECTONUM_FUNC(FName, NAME_IN_PARSER, CFUNC) \
QFMathParser::qfmpResult FName(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){\
    QFMathParser::qfmpResult r;\
    r.type=QFMathParser::qfmpDouble;\
    if (n!=2) {\
        p->qfmpError(QObject::tr("%1(x, p) needs exacptly 2 argument").arg(#NAME_IN_PARSER));\
        r.isValid=false;\
        return r;\
    }\
    if(params[0].type==QFMathParser::qfmpDoubleVector && params[1].type==QFMathParser::qfmpDouble) {\
        r.type=QFMathParser::qfmpDouble;\
        r.num=CFUNC(params[0].numVec, params[1].num);\
    } else {\
        p->qfmpError(QObject::tr("%1(x, p) argument x has to be a vector of numbers and p a number").arg(#NAME_IN_PARSER));\
        r.isValid=false;\
    }\
    return r;\
}


/*! \brief This macro allows to easily define functions for QFMathParser from a C-function that
           has one string (QString!!!) argument, e.g. sin(x)

    The resulting function will:
      - check the number of arguments
      - apply the C-function to any string parameter
      - result is also string
    .

    \param FName name of the function to declare
    \param NAME_IN_PARSER name the function should have in the parser (used for error messages only)
    \param CFUNC name of the C function to call
*/
#define QFMATHPARSER_DEFINE_1PARAM_STRING_FUNC(FName, NAME_IN_PARSER, CFUNC) \
QFMathParser::qfmpResult FName(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){\
    QFMathParser::qfmpResult r;\
    r.type=QFMathParser::qfmpString;\
    if (n!=1) {\
        p->qfmpError(QObject::tr("%1(...) needs exacptly 1 argument").arg(#NAME_IN_PARSER));\
        r.isValid=false;\
        return r;\
    }\
    if (params[0].type==QFMathParser::qfmpString) {\
        r.str=CFUNC(params[0].str);\
    } else {\
        p->qfmpError(QObject::tr("%1(...) argument has to be a string").arg(#NAME_IN_PARSER));\
        r.isValid=false;\
    }\
    return r;\
}

/** \brief same as QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC(FName, NAME_IN_PARSER, CFUNC), but NAME_IN_PARSER==CFUNC */
#define QFMATHPARSER_DEFINE_1PARAM_STRING_FUNC_SIMPLE(FName, CFUNC) QFMATHPARSER_DEFINE_1PARAM_STRING_FUNC(FName, CFUNC, CFUNC)


/*! \brief This macro allows to easily define functions for QFMathParser from a C-function that
           has two numeric argument, e.g. yn(p, x), p is a number, x may be a vector

    The resulting function will:
      - check the number of arguments
      - apply the C-function to any number parameter
      - apply the C-function item-wise to any number vector parameter
      - result is also a number or number vector
    .
    \param FName name of the function to declare
    \param NAME_IN_PARSER name the function should have in the parser (used for error messages only)
    \param CFUNC name of the C function to call
*/
#define QFMATHPARSER_DEFINE_2PARAM2VEC_NUMERIC_FUNC(FName, NAME_IN_PARSER, CFUNC) \
QFMathParser::qfmpResult FName(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){\
    QFMathParser::qfmpResult r;\
    r.type=QFMathParser::qfmpDouble;\
    if (n!=2) {\
        p->qfmpError(QObject::tr("%1(...) needs exactly 2 argument").arg(#NAME_IN_PARSER));\
        r.isValid=false;\
        return r;\
    }\
    double pa=0; \
    if (params[0].type==QFMathParser::qfmpDouble) {\
        pa=params[0].num;\
    } else {\
        p->qfmpError(QObject::tr("%1(...) first argument has to be a number").arg(#NAME_IN_PARSER));\
        r.isValid=false;\
        return r;\
    }\
    if (params[1].type==QFMathParser::qfmpDouble) {\
        r.num=CFUNC(pa, params[1].num);\
    } else if(params[1].type==QFMathParser::qfmpDoubleVector) {\
        r.type=QFMathParser::qfmpDoubleVector;\
        r.numVec.resize(params[1].numVec.size());\
        for (int i=0; i<params[1].numVec.size(); i++) {\
            r.numVec[i]=CFUNC(pa, params[1].numVec[i]);\
        }\
    } else {\
        p->qfmpError(QObject::tr("%1(...) second argument has to be a number or vector of numbers").arg(#NAME_IN_PARSER));\
        r.isValid=false;\
    }\
    return r;\
}

/*! \brief This macro allows to easily define functions for QFMathParser from a C-function that
           has two numeric argument, e.g. yn(x, p), p is a number, x may be a vector

    The resulting function will:
      - check the number of arguments
      - apply the C-function to any number parameter
      - apply the C-function item-wise to any number vector parameter
      - result is also a number or number vector
    .
    \param FName name of the function to declare
    \param NAME_IN_PARSER name the function should have in the parser (used for error messages only)
    \param CFUNC name of the C function to call
*/
#define QFMATHPARSER_DEFINE_2PARAM1VEC_NUMERIC_FUNC(FName, NAME_IN_PARSER, CFUNC) \
QFMathParser::qfmpResult FName(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){\
    QFMathParser::qfmpResult r;\
    r.type=QFMathParser::qfmpDouble;\
    if (n!=2) {\
        p->qfmpError(QObject::tr("%1(...) needs exactly 2 argument").arg(#NAME_IN_PARSER));\
        r.isValid=false;\
        return r;\
    }\
    double pa=0; \
    if (params[1].type==QFMathParser::qfmpDouble) {\
        pa=params[1].num;\
    } else {\
        p->qfmpError(QObject::tr("%1(...) second argument has to be a number").arg(#NAME_IN_PARSER));\
        r.isValid=false;\
        return r;\
    }\
    if (params[0].type==QFMathParser::qfmpDouble) {\
        r.num=CFUNC(params[0].num, pa);\
    } else if(params[0].type==QFMathParser::qfmpDoubleVector) {\
        r.type=QFMathParser::qfmpDoubleVector;\
        r.numVec.resize(params[0].numVec.size());\
        for (int i=0; i<params[0].numVec.size(); i++) {\
            r.numVec[i]=CFUNC(params[0].numVec[i], pa);\
        }\
    } else {\
        p->qfmpError(QObject::tr("%1(...) first argument has to be a number or vector of numbers").arg(#NAME_IN_PARSER));\
        r.isValid=false;\
    }\
    return r;\
}


/*! \brief This macro allows to easily define functions for QFMathParser from a C-function that
           has two numeric argument, e.g. fmod(x, y),  x and y may be a vector (both, same length) or a number (both)

    The resulting function will:
      - check the number of arguments
      - apply the C-function to any number parameter
      - apply the C-function item-wise to any number vector parameter, i.e. [ fmod(x1,y1), fmod(x2,y2), ... ]
      - result is also a number or number vector
    .
    \param FName name of the function to declare
    \param NAME_IN_PARSER name the function should have in the parser (used for error messages only)
    \param CFUNC name of the C function to call
*/
#define QFMATHPARSER_DEFINE_2PARAM12VEC_NUMERIC_FUNC(FName, NAME_IN_PARSER, CFUNC) \
QFMathParser::qfmpResult FName(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){\
    QFMathParser::qfmpResult r;\
    r.type=QFMathParser::qfmpDouble;\
    if (n!=2) {\
        p->qfmpError(QObject::tr("%1(...) needs exactly 2 argument").arg(#NAME_IN_PARSER));\
        r.isValid=false;\
        return r;\
    }\
    if (params[0].type==QFMathParser::qfmpDouble && params[1].type==QFMathParser::qfmpDouble) {\
    r.num=CFUNC(params[0].num, params[1].num);\
    } else if(params[0].type==QFMathParser::qfmpDoubleVector && params[1].type==QFMathParser::qfmpDoubleVector) {\
        if (params[0].numVec.size()!=params[1].numVec.size()) {\
            p->qfmpError(QObject::tr("%1(x,y) both arguments have to have same length").arg(#NAME_IN_PARSER));\
            r.isValid=false;\
            return r;\
        }\
        r.type=QFMathParser::qfmpDoubleVector;\
        r.numVec.resize(params[0].numVec.size());\
        for (int i=0; i<params[0].numVec.size(); i++) {\
            r.numVec[i]=CFUNC(params[0].numVec[i], params[1].numVec[i]);\
        }\
    } else {\
        p->qfmpError(QObject::tr("%1(...) first argument has to be a number or vector of numbers").arg(#NAME_IN_PARSER));\
        r.isValid=false;\
    }\
    return r;\
}

/** \brief same as QFMATHPARSER_DEFINE_2PARAM1VEC_NUMERIC_FUNC(FName, NAME_IN_PARSER, CFUNC), but NAME_IN_PARSER==CFUNC */
#define QFMATHPARSER_DEFINE_2PARAM1VEC_NUMERIC_FUNC_SIMPLE(FName, CFUNC) QFMATHPARSER_DEFINE_2PARAM1VEC_NUMERIC_FUNC(FName, CFUNC, CFUNC)

/** \brief same as QFMATHPARSER_DEFINE_2PARAM2VEC_NUMERIC_FUNC(FName, NAME_IN_PARSER, CFUNC), but NAME_IN_PARSER==CFUNC */
#define QFMATHPARSER_DEFINE_2PARAM2VEC_NUMERIC_FUNC_SIMPLE(FName, CFUNC) QFMATHPARSER_DEFINE_2PARAM2VEC_NUMERIC_FUNC(FName, CFUNC, CFUNC)

/** \brief same as QFMATHPARSER_DEFINE_2PARAM12VEC_NUMERIC_FUNC(FName, NAME_IN_PARSER, CFUNC), but NAME_IN_PARSER==CFUNC */
#define QFMATHPARSER_DEFINE_2PARAM12VEC_NUMERIC_FUNC_SIMPLE(FName, CFUNC) QFMATHPARSER_DEFINE_2PARAM12VEC_NUMERIC_FUNC(FName, CFUNC, CFUNC)


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
   - ...
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
                      | math_expression <b>-</b> math_term
                      | math_expression <b>|</b> math_term</pre>
<pre>  math_term       ->  vec_primary
                      | term <b>*</b> vec_primary
                      | term <b>/</b> vec_primary
                      | term <b>&amp;</b> vec_primary
                      | term ( <b>%</b> | <b>mod</b> ) vec_primary</pre>
<pre>  vec_primary     ->  primary
                      | primary <b>:</b> primary
                      | primary <b>:</b> primary <b>:</b> primary
                      | <b>[</b> vector_list <b>]</b>
<pre>  primary         ->  <b>true</b> | <b>false</b>
                      | string_constant
                      | NUMBER
                      | NAME
                      | <b>sum</b> | <b>prod</b> <b>(</b> logical_expression <b>)</b>
                      | vectorops_name <b>(</b> NAME, logical_expression, logical_expression, logical_expression, logical_expression <b>)</b>
                      | vectorops_name <b>(</b> NAME, logical_expression, logical_expression, logical_expression <b>)</b>
                      | vectorops_name <b>(</b> NAME, logical_expression, logical_expression <b>)</b>
                      | NAME <b>=</b> logical_expression
                      | NAME<b>[</b> logical_expression <b>] =</b> logical_expression
                      | NAME<b>[</b> logical_expression <b>]</b>
                      | NAME<b>(</b> parameter_list <b>)</b>
                      | NAME(parametername_list) <b>=</b> logical_expression
                      | <b>+</b> primary | <b>-</b> primary | <b>!</b> primary | <b>not</b> primary | <b>~</b> primary
                      | <b>(</b> logical_expression <b>)</b>
                      | primary <b>^</b> primary</pre>

<pre>  string_constant -> <b>&quot;</b> STRING <b>&quot;</b> | <b>&apos;</b> STRING <b>&apos;</b></pre>
<pre>  parameter_list  ->  \f$ \lambda \f$ | logical_expression | logical_expression <b>,</b> parameter_list</pre>
<pre>  parametername_list  ->  \f$ \lambda \f$ | NAME | NAME <b>,</b> parametername_list</pre>
<pre>  vector_list  ->  logical_expression | logical_expression <b>,</b> vector_list</pre>
<pre>  vectorops_name  ->  <b>sum</b> | <b>prod</b> | <b>for</b> </pre>




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
        }
        if (mp.hasErrorOccured()) {
            qDebug()<<"ERROR: "<<mp.getLastError();
        }
        if (n) delete n;

        // parse some string expression
        n=mp.parse("var1='false'; var1+'true'");
        if (!mp.hasErrorOccured()) {
            r=n->evaluate();
            if (r.type==qfmpString) cout<<r.str<<endl;
        }
        if (mp.hasErrorOccured()) {
            qDebug()<<"ERROR: "<<mp.getLastError();
        }
        if (n) delete n;
     \endcode



 */
class QFLIB_EXPORT QFMathParser
{
    protected:

        /** \brief the possible tokens that can be recognized by the tokenizer in jkMathParser::getToken() */
        enum qfmpTokenType {
            END,                /*!< \brief end token */
            PRINT,              /*!< \brief a semicolon ';' */
            COMMA,      /*!< \brief a comma ',' between two function parameters */
            STRING_DELIM,       /*!< \brief a string delimiter ' or " */
            NAME,               /*!< \brief a name (consisting of characters) of a variable or function */
            NUMBER,             /*!< \brief a number in scientific notation */
            PLUS,               /*!< \brief a plus operator '+' */
            MINUS,              /*!< \brief a minus operator '-' */
            MUL,                /*!< \brief a multiplication operator '*' */
            DIV,                /*!< \brief a division operator '/' */
            MODULO,             /*!< \brief a modulo operator '%' */
            ASSIGN,             /*!< \brief a variable assignment = */
            LPARENTHESE,        /*!< \brief left parentheses '(' */
            RPARENTHESE,        /*!< \brief right parentheses ')' */
            LBRACKET,           /*!< \brief left bracket '[' */
            RBRACKET,           /*!< \brief right bracket ']' */
            LBRACE,             /*!< \brief left brace '{' */
            RBRACE,             /*!< \brief right brace '}' */
            POWER,              /*!< \brief a power operator '^' */
            COLON,              /*!< \brief a colon ':' */
            TILDE,              /*!< \brief a tilde symbol '~' */
            FACTORIAL_LOGIC_NOT, /*!< \brief a factorial operator or a logical NOT '!' */
            LOGIC_NOT,          /*!< \brief a logical NOT '!' / 'not' */
            LOGIC_AND,          /*!< \brief a logical AND operator '&&' / 'and' */
            LOGIC_OR,           /*!< \brief a logical OR operator '||' / 'or' */
            LOGIC_XOR,          /*!< \brief a logical XOR operator 'xor' */
            LOGIC_NOR,          /*!< \brief a logical NOR operator 'nor' */
            LOGIC_NAND,         /*!< \brief a logical NAND operator 'nand' */
            LOGIC_TRUE,         /*!< \brief 'true' */
            LOGIC_FALSE,        /*!< \brief  'false' */
            BINARY_AND,          /*!< \brief a binary AND operator '&' */
            BINARY_OR,           /*!< \brief a binary OR operator '|' */
            COMP_EQUALT,        /*!< \brief equals operation '==' */
            COMP_UNEQUAL,       /*!< \brief unequal operation '!=' */
            COMP_GREATER,       /*!< \brief greater than operation '>' */
            COMP_SMALLER,       /*!< \brief smaller than operation '<' */
            COMP_GEQUAL,        /*!< \brief greater than or equal operation '>=' */
            COMP_SEQUAL        /*!< \brief smaller than or equal operation '<=' */
        };



        /** \brief internal names for logic/binary operations */
        enum {
            qfmpLOPand='a',
            qfmpLOPor='o',
            qfmpLOPxor='x',
            qfmpLOPnor='r',
            qfmpLOPnand='d',
            qfmpLOPnot='n',

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
                             qfmpBool,   /*!< \brief a boolean value true|false */
                             qfmpDoubleVector  /*!< \brief a vector of floating point numbers */
                             };

        static QString resultTypeToString(qfmpResultType type);

        /** \brief result of any expression  */
        struct QFLIB_EXPORT qfmpResult {
            public:
                qfmpResult();
                qfmpResult(double value);
                qfmpResult(QString value);
                qfmpResult(bool value);
                qfmpResult(const QVector<double> &value);


                /** \brief convert the value this struct representens into a QString */
                QFLIB_EXPORT QString toString() const;

                /** \brief convert the value this struct representens into a QString and adds the name of the datatype in \c [...] */
                QFLIB_EXPORT QString toTypeString() const;

                /** \brief convert the value this struct to an integer */
                QFLIB_EXPORT int32_t toInteger() const;
                /** \brief convert the value this struct to an integer */
                QFLIB_EXPORT uint32_t toUInt() const;
                /** \brief is this result convertible to integer? */
                QFLIB_EXPORT bool isInteger() const;
                /** \brief is this result convertible to unsigned integer? */
                QFLIB_EXPORT bool isUInt() const;

                QFLIB_EXPORT void setDouble(double val);
                QFLIB_EXPORT void setBoolean(bool val);
                QFLIB_EXPORT void setString(const QString& val);
                QFLIB_EXPORT void setDoubleVec(const QVector<double>& val);
                QFLIB_EXPORT QVector<double> asVector() const;
                QFLIB_EXPORT bool  convertsToVector() const;
                QFLIB_EXPORT QVector<int> asIntVector() const;
                QFLIB_EXPORT bool  convertsToIntVector() const;


                bool isValid;
                qfmpResultType type;   /*!< \brief type of the result */
                QString str;       /*!< \brief contains result if \c type==qfmpString */
                double num;            /*!< \brief contains result if \c type==qfmpDouble */
                bool boolean;          /*!< \brief contains result if \c type==qfmpBool */
                QVector<double> numVec; /*!< \brief contains result if \c type==qfmpDoubleVector */

        };

        static QString resultToString(const qfmpResult &r);
        static QString resultToTypeString(const qfmpResult &r);

        static qfmpResult getInvalidResult();


        /** \brief This struct is for managing variables. Unlike qfmpResult this struct
          * only contains pointers to the data
          */
        struct QFLIB_EXPORT qfmpVariable {
            public:
                qfmpVariable();
                //~qfmpVariable();
                QFLIB_EXPORT void clearMemory();

                QFLIB_EXPORT QFMathParser::qfmpResult toResult() const;
                QFLIB_EXPORT bool isInternal() const;
                QFLIB_EXPORT void set(const qfmpResult& result);
                inline  qfmpResultType getType() const { return type; }


            protected:
                qfmpResultType type;     /*!< \brief type of the variable */
                bool internal;           /*!< \brief this is an internal variable */
                QString *str;        /*!< \brief this points to the variable data if \c type==qfmpString */
                double *num;             /*!< \brief this points to the variable data if \c type==qfmpDouble */
                bool *boolean;           /*!< \brief this points to the variable data if \c type==qfmpBool */
                QVector<double>* numVec; /*!< \brief this points to the variable data if \c type==qfmpDoubleVector */
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
            explicit qfmpNode(QFMathParser* parser, qfmpNode* parent=NULL) { this->parser=parser;  this->parent=parent; }
            /** \brief virtual class destructor */
            virtual ~qfmpNode() {}

            /** \brief evaluate this node */
            virtual qfmpResult evaluate()=0;

            /** \brief return a pointer to the jkMathParser  */
            inline QFMathParser* getParser(){ return parser; }

            /** \brief set the jkMathParser  */
            inline void setParser(QFMathParser* mp){ parser=mp; }

            /** \brief returns a pointer to the parent node */
            inline qfmpNode* getParent(){ return parent; }

            /** \brief sets the parent node  */
            inline void setParent(qfmpNode* par) { parent=par; }

            /** \brief returns a copy of the current node (and the subtree). The parent is set to \a par */
            virtual qfmpNode* copy(qfmpNode* par=NULL) =0;
        };


        /**
         * \brief This class represents a binary arithmetic operation:
         *  add (+), subtract (-), multiply (*), divide (/), a to the power of b (a^b),
         *  binary and (&), binary or (|)
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
            explicit qfmpBinaryArithmeticNode(char op, qfmpNode* l, qfmpNode* r, QFMathParser* p, qfmpNode* par);

            /** \brief standard destructor, also destroy the children (recursively)  */
            ~qfmpBinaryArithmeticNode();

            /** \brief evaluate this node */
            virtual qfmpResult evaluate();

            /** \brief returns a copy of the current node (and the subtree). The parent is set to \a par */
            virtual qfmpNode* copy(qfmpNode* par=NULL) ;
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
            explicit qfmpBinaryBoolNode(char op, qfmpNode* l, qfmpNode* r, QFMathParser* p, qfmpNode* par);

            /** \brief standard destructor, also destroy the children (recursively)  */
            ~qfmpBinaryBoolNode();

            /** \brief evaluate this node */
            virtual qfmpResult evaluate();

            /** \brief returns a copy of the current node (and the subtree). The parent is set to \a par */
            virtual qfmpNode* copy(qfmpNode* par=NULL) ;

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
            explicit qfmpCompareNode(char op, qfmpNode* l, qfmpNode* r, QFMathParser* p, qfmpNode* par);

            /** \brief standard destructor, also destroy the children (recursively)  */
            ~qfmpCompareNode ();

            /** \brief evaluate this node */
            virtual qfmpResult evaluate();

            /** \brief returns a copy of the current node (and the subtree). The parent is set to \a par */
            virtual qfmpNode* copy(qfmpNode* par=NULL) ;

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
            explicit qfmpUnaryNode(char op, qfmpNode* c, QFMathParser* p, qfmpNode* par);

            /** \brief standard destructor, also destroy the children (recursively)  */
            ~qfmpUnaryNode();

            /** \brief evaluate this node */
            virtual qfmpResult evaluate();

            /** \brief returns a copy of the current node (and the subtree). The parent is set to \a par */
            virtual qfmpNode* copy(qfmpNode* par=NULL);

        };

        /**
         * \brief This class represents a vector element access
         */
        class QFLIB_EXPORT qfmpVectorAccessNode: public qfmpNode {
          protected:
            qfmpNode* index;
            QString variable;
          public:
            /** \brief standard destructor, also destroy the children (recursively)  */
            ~qfmpVectorAccessNode() ;

            /** \brief constructor for a qfmpVariableAssignNode
             *  \param var name of the variable to assign to
             *  \param index index child node
             *  \param p a pointer to a jkMathParser object
             *  \param par a pointer to the parent node
             */
            explicit qfmpVectorAccessNode(QString var, qfmpNode* index, QFMathParser* p, qfmpNode* par);

            /** \brief evaluate this node */
            virtual qfmpResult evaluate();

            /** \brief returns a copy of the current node (and the subtree). The parent is set to \a par */
            virtual qfmpNode* copy(qfmpNode* par=NULL) ;

        };

        /**
         * \brief This class represents a variable assignment (a = expression)
         */
        class QFLIB_EXPORT qfmpVariableAssignNode: public qfmpNode {
          protected:
            qfmpNode* child;
            QString variable;
          public:
            /** \brief standard destructor, also destroy the children (recursively)  */
            ~qfmpVariableAssignNode() ;

            /** \brief constructor for a qfmpVariableAssignNode
             *  \param var name of the variable to assign to
             *  \param c child node/right-hand-side expression
             *  \param p a pointer to a jkMathParser object
             *  \param par a pointer to the parent node
             */
            explicit qfmpVariableAssignNode(QString var, qfmpNode* c, QFMathParser* p, qfmpNode* par);

            /** \brief evaluate this node */
            virtual qfmpResult evaluate();

            /** \brief returns a copy of the current node (and the subtree). The parent is set to \a par */
            virtual qfmpNode* copy(qfmpNode* par=NULL) ;

        };

        /**
         * \brief This class represents a vector element assignment (a[index] = expression)
         */
        class QFLIB_EXPORT qfmpVectorElementAssignNode: public qfmpVariableAssignNode {
          protected:
            qfmpNode* index;
          public:
            /** \brief standard destructor, also destroy the children (recursively)  */
            ~qfmpVectorElementAssignNode() ;

            /** \brief constructor for a qfmpVectorElementAssignNode
             *  \param var name of the variable to assign to
             *  \param index index node
             *  \param expression child node/right-hand-side expression
             *  \param p a pointer to a jkMathParser object
             *  \param par a pointer to the parent node
             */
            explicit qfmpVectorElementAssignNode(QString var, qfmpNode* index, qfmpNode* expression, QFMathParser* p, qfmpNode* par);

            /** \brief evaluate this node */
            virtual qfmpResult evaluate();

            /** \brief returns a copy of the current node (and the subtree). The parent is set to \a par */
            virtual qfmpNode* copy(qfmpNode* par=NULL) ;

        };


        /**
         * \brief This class represents a variable assignment (a = expression)
         */
        class QFLIB_EXPORT qfmpFunctionAssignNode: public qfmpNode {
          private:
            qfmpNode* child;
            QString function;
            QStringList parameterNames;
          public:
            /** \brief standard destructor, also destroy the children (recursively)  */
            ~qfmpFunctionAssignNode() ;

            /** \brief constructor for a qfmpVariableAssignNode
             *  \param function name of the function to assign to
             *  \param parameterNames list of parameters of the new function
             *  \param c child node/right-hand-side expression
             *  \param p a pointer to a jkMathParser object
             *  \param par a pointer to the parent node
             */
            explicit qfmpFunctionAssignNode(QString function, QStringList parameterNames, qfmpNode* c, QFMathParser* p, qfmpNode* par);

            /** \brief evaluate this node */
            virtual qfmpResult evaluate();

            /** \brief returns a copy of the current node (and the subtree). The parent is set to \a par */
            virtual qfmpNode* copy(qfmpNode* par=NULL) ;

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
            explicit qfmpConstantNode(qfmpResult d, QFMathParser* p, qfmpNode* par):qfmpNode(p, par) { data=d; };

            /** \brief evaluate this node */
            virtual qfmpResult evaluate() { return data; };

            /** \brief returns a copy of the current node (and the subtree). The parent is set to \a par */
            virtual qfmpNode* copy(qfmpNode* par=NULL) ;

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
            explicit qfmpVariableNode(QString name, QFMathParser* p, qfmpNode* par);

            /** \brief evaluate this node */
            virtual qfmpResult evaluate();

            inline QString getName() const { return var; }

            /** \brief returns a copy of the current node (and the subtree). The parent is set to \a par */
            virtual qfmpNode* copy(qfmpNode* par=NULL) ;

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
            explicit qfmpInvalidNode(QFMathParser* p, qfmpNode* par):qfmpNode(p, par) { };

            /** \brief evaluate this node */
            virtual qfmpResult evaluate() { return parser->getInvalidResult(); };

            /** \brief returns a copy of the current node (and the subtree). The parent is set to \a par */
            virtual qfmpNode* copy(qfmpNode* par=NULL) ;

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
        typedef qfmpResult (*qfmpEvaluateFunc)(const qfmpResult*, unsigned int, QFMathParser*);

        /** \brief types of functions */
        enum qfmpFunctiontype {
            functionC,
            functionNode
        };

        /** \brief description of a user registered function */
        struct QFLIB_EXPORT qfmpFunctionDescriptor {
            explicit qfmpFunctionDescriptor();
            //~qfmpFunctionDescriptor();
            QFLIB_EXPORT void clearMemory();
            qfmpEvaluateFunc function;    /*!< \brief a pointer to the function implementation */
            QString name;             /*!< \brief name of the function */
            qfmpFunctiontype type;  /*!< \brief type of the function */
            qfmpNode* functionNode;   /*!< \brief points to the node definig the function */
            QStringList parameterNames;  /*!< \brief a list of the function parameters, if the function is defined by a node */

            QFLIB_EXPORT qfmpResult evaluate(const QVector<qfmpResult> &parameters, QFMathParser *parent) const;
            QFLIB_EXPORT QString toDefString() const;
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
            QVector<QFMathParser::qfmpNode*> child;
          public:
            /** \brief constructor for a qfmpFunctionNode
             *  \param name name of the function
             *  \param c a pointer to an array of qfmpNode objects that represent the parameter expressions
             *  \param num number of children in c
             *  \param p a pointer to a jkMathParser object
             *  \param par a pointer to the parent node
             */
            explicit qfmpFunctionNode(QString name, QVector<QFMathParser::qfmpNode*> params, QFMathParser* p, qfmpNode* par);

            /** \brief standard destructor, also destroy the children (recursively) */
            ~qfmpFunctionNode();

            /** \brief evaluate this node */
            virtual qfmpResult evaluate();

            /** \brief returns a copy of the current node (and the subtree). The parent is set to \a par */
            virtual qfmpNode* copy(qfmpNode* par=NULL) ;

        };

        /**
         * \brief This class represents a list of qfmpNode.
         *
         * when evaluating the result will be the result of the last node in the list.
         */
        class QFLIB_EXPORT qfmpNodeList: public qfmpNode {
          protected:
            QList<qfmpNode*> list;
          public:
            /** \brief constructor for a qfmpNodeList
             *  \param p a pointer to a jkMathParser object
             */
            explicit qfmpNodeList(QFMathParser* p, qfmpNode* par=NULL):qfmpNode(p, par) { };

            /** \brief standard destructor, also destroy the children (recursively) */
            ~qfmpNodeList();

            /** \brief add a qfmpNode n to the list */
            void add(qfmpNode* n);
            /** \brief remove the first node from the list and only delet it if \a deleteObject is \c true, returns a pointer to the first node, if \a deleteObject is \c false otherwise returns \c NULL */
            qfmpNode* popFirst(bool deleteObject=false);

            /** \brief evaluate the node */
            virtual qfmpResult evaluate();

            /** \brief get the number of nodes in the list */
            int getCount() {return list.size();};

            /** \brief returns a copy of the current node (and the subtree). The parent is set to \a par */
            virtual qfmpNode* copy(qfmpNode* par=NULL) ;

        };

        /**
         * \brief This class represents a fector of qfmpNode.
         *
         * when evaluating the result will be the result of the last node in the list.
         */
        class QFLIB_EXPORT qfmpVectorList: public qfmpNodeList {
          public:
            /** \brief constructor for a qfmpNodeList
             *  \param p a pointer to a jkMathParser object
             */
            explicit qfmpVectorList(QFMathParser* p, qfmpNode* par=NULL): qfmpNodeList(p, par) { setParser(p); setParent(par); };

            /** \brief standard destructor, also destroy the children (recursively) */
            ~qfmpVectorList() {};


            /** \brief evaluate the node */
            virtual qfmpResult evaluate();

            /** \brief returns a copy of the current node (and the subtree). The parent is set to \a par */
            virtual qfmpNode* copy(qfmpNode* par=NULL) ;

        };


        /**
         * \brief This class represents a vector construction in the form start:end or start:step:end
         */
        class QFLIB_EXPORT qfmpVectorConstructionNode: public qfmpNode {
          private:
            qfmpNode* start, *end, *step;
          public:
            /** \brief constructor for a qfmpCompareNode
             *  \param start start child node/operand
             *  \param end end child node/operand
             *  \param step step child node/operand
             *  \param p a pointer to a jkMathParser object
             *  \param par a pointer to the parent node
             */
            explicit qfmpVectorConstructionNode(qfmpNode* start, qfmpNode* end, qfmpNode* step, QFMathParser* p, qfmpNode* par);

            /** \brief standard destructor, also destroy the children (recursively)  */
            ~qfmpVectorConstructionNode ();

            /** \brief evaluate this node */
            virtual qfmpResult evaluate();

            /** \brief returns a copy of the current node (and the subtree). The parent is set to \a par */
            virtual qfmpNode* copy(qfmpNode* par=NULL) ;

        };

        /**
         * \brief This class represents a cases or if construction
         */
        class QFLIB_EXPORT qfmpCasesNode: public qfmpNode {
          private:
            qfmpNode* elseNode;
            QList<QPair<qfmpNode*, qfmpNode*> > casesNodes;
          public:
            /** \brief constructor for a qfmpCompareNode
             *  \param start start child node/operand
             *  \param end end child node/operand
             *  \param step step child node/operand
             *  \param p a pointer to a jkMathParser object
             *  \param par a pointer to the parent node
             */
            explicit qfmpCasesNode(QFMathParser* p, qfmpNode* par);

            /** \brief standard destructor, also destroy the children (recursively)  */
            ~qfmpCasesNode ();

            void setElse(qfmpNode* elseNode);
            void addCase(qfmpNode* decision, qfmpNode* value);

            /** \brief evaluate this node */
            virtual qfmpResult evaluate();

            /** \brief returns a copy of the current node (and the subtree). The parent is set to \a par */
            virtual qfmpNode* copy(qfmpNode* par=NULL) ;

        };

        /**
         * \brief This class represents a cases or if construction
         */
        class QFLIB_EXPORT qfmpVectorOperationNode: public qfmpNode {
          private:
            QString operationName;
            QString variableName;
            qfmpNode* items;
            qfmpNode* start;
            qfmpNode* end;
            qfmpNode* delta;
            qfmpNode* expression;
            QList<QPair<qfmpNode*, qfmpNode*> > casesNodes;
          public:
            /** \brief constructor for a qfmpVectorOperationNode
             */
            explicit qfmpVectorOperationNode(const QString& operationName, const QString& variableName, qfmpNode* items, qfmpNode* expression, QFMathParser* p, qfmpNode* par);
            explicit qfmpVectorOperationNode(const QString& operationName, const QString& variableName, qfmpNode* start, qfmpNode* end, qfmpNode* delta, qfmpNode* expression, QFMathParser* p, qfmpNode* par);

            /** \brief standard destructor, also destroy the children (recursively)  */
            ~qfmpVectorOperationNode ();

            /** \brief evaluate this node */
            virtual qfmpResult evaluate();

            /** \brief returns a copy of the current node (and the subtree). The parent is set to \a par */
            virtual qfmpNode* copy(qfmpNode* par=NULL) ;

        };

        /*@}*/



        class executionEnvironment {
            protected:
                /** \brief map to manage all currently defined variables */
                QMap<QString, QList<QPair<int, qfmpVariable> > > variables;

                /** \brief map to manage all currently rtegistered functions */
                QMap<QString, QList<QPair<int, qfmpFunctionDescriptor> > > functions;

                int currentLevel;

                QFMathParser* parent;
            public:
                executionEnvironment(QFMathParser* parent=NULL);
                ~executionEnvironment();

                void setParent(QFMathParser* parent);

                void enterBlock();
                void leaveBlock();
                int getBlocklevel() const;
                void clearVariables();
                void clearFunctions();
                void clear();

                void addVariable(const QString& name, const qfmpVariable& variable);
                void setFunction(const QString& name, const qfmpFunctionDescriptor& function);
                void addFunction(const QString& name, QStringList parameterNames, qfmpNode* function);

                /** \brief  tests whether a variable exists */
                bool variableExists(const QString& name){ return (variables.find(name)!=variables.end()); }

                /** \brief  tests whether a function exists */
                bool functionExists(const QString& name){ return !(functions.find(name)==functions.end()); }

                qfmpResult getVariable(const QString& name) const;
                qfmpResult evaluateFunction(const QString& name, const QVector<qfmpResult> &parameters) const;

                void addVariable(const QString& name, const qfmpResult& result);
                void setVariable(const QString& name, const qfmpResult& result);
                void setVariableDouble(const QString& name, double result);
                void setVariableDoubleVec(const QString& name, const QVector<double>& result);
                void setVariableString(const QString& name, const QString& result);
                void setVariableBoolean(const QString& name, bool result);
                void addVariableDouble(const QString& name, double result);
                void addVariableDoubleVec(const QString& name, const QVector<double>& result);
                void addVariableString(const QString& name, const QString& result);
                void addVariableBoolean(const QString& name, bool result);
                void deleteVariable(const QString& name);

                QString printVariables() const;
                QString printFunctions() const;
                QList<QPair<QString, qfmpVariable> > getVariables() const;
                QList<QPair<QString, qfmpFunctionDescriptor> > getFunctions() const;


        };

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

        /** \brief recognizes a vector_primary while parsing. If \a get ist \c true, this function first retrieves a new token by calling getToken() */
        qfmpNode* vectorPrimary(bool get);

        /** \brief recognizes a primary while parsing. If \a get ist \c true, this function first retrieves a new token by calling getToken() */
        qfmpNode* primary(bool get);

		/** \brief this stream is used to read in the program. An object is created and assigned
  		 * (and destroyed) by the parse()-function */
        QTextStream * program;
        QString progStr;

        MTRand rng;

        double readNumber();
        double readBin();
        double readHex();
        double readOct();
        double readDec();



        executionEnvironment environment;

        /** \brief the current token while parsing a string */
        qfmpTokenType CurrentToken;

        /** \brief the string value of the current token (when applicable) during the parsing step */
        QString StringValue;

        /** \brief the string value of the current token (when applicable) during the parsing step */
        double NumberValue;


        void* data;

        QStringList lastError;
        int errors;

        QChar peekStream(QTextStream* stream);
        void putbackStream(QTextStream* stream, QChar ch);
        bool getFromStream(QTextStream* stream, QChar& ch);

	public:
        /** \brief class constructor */
        QFMathParser();

        /** \brief class destructor */
        virtual ~QFMathParser();

        /** \brief returns whether an error has occured since the last call to resetError() */
        bool hasErrorOccured() const;
        /** \brief retuns the last error */
        QString getLastError() const;
        /** \brief retuns the last error */
        QString getFirstError() const;
        /** \brief return a list of the last errors */
        QStringList getLastErrors() const;
        /** \brief returns the number of errors since the last call to resetErrors() */
        int getLastErrorCount();
        /** \brief resets the error state to no errors */
        void resetErrors();

        void* get_data() const { return data; }
        void set_data(void* _data) { data=_data; }

        MTRand* get_rng() { return &rng; }


        /** \brief  register a new function
		 * \param name name of the new function
		 * \param function a pointer to the implementation
		 */
        void addFunction(const QString &name, qfmpEvaluateFunc function);

        void addFunction(const QString& name, QStringList parameterNames, qfmpNode* function);

        /** \brief set the defining struct of the given variable */
        void addVariable(const QString& name, const qfmpVariable &value);


        /** \brief  register a new internal variable of type double
		 * \param name name of the new variable
		 * \param v initial value of this variable
		 */
        void addVariableDouble(const QString& name, double v);

        /** \brief  register a new internal variable of type string
		 * \param name name of the new variable
		 * \param v initial value of this variable
		 */
        void addVariableString(const QString &name, const QString& v);

        /** \brief  register a new internal variable of type boolean
         * \param name name of the new variable
         * \param v initial value of this variable
         */
        void addVariableBoolean(const QString& name, bool v);

        /** \brief  register a new internal variable of type boolean
         * \param name name of the new variable
         * \param v initial value of this variable
         */
        void addVariable(const QString &name, const qfmpResult &result);
        /** \brief  register a new internal variable if the given variable does not exist, otherwise set the axisting variable to the given value
         * \param name name of the new variable
         * \param v initial value of this variable
         */
        void setVariable(const QString &name, const qfmpResult &result);

        void deleteVariable(const QString& name);




        /** \brief  returns the value of the given variable */
        qfmpResult getVariable(QString name);
        /** \brief  returns the value of the given variable */
        qfmpResult getVariableOrInvalid(QString name);



        /** \brief  evaluates a registered function
         * \param name name of the (registered function) to be evaluated
         * \param params array of the input parameters
         */
        qfmpResult evaluateFunction(const QString &name, const QVector<qfmpResult>& params);



        /** \brief  tests whether a variable exists */
        inline bool variableExists(const QString& name){ return environment.variableExists(name); }

        /** \brief  tests whether a function exists */
        inline bool functionExists(const QString& name){ return environment.functionExists(name); }

        void enterBlock();
        void leaveBlock();

        /** \brief  deletes all defined variables. the memory of internal variables
         * will be released. the external memory will not be released.
         */
        void clearVariables();

        /** \brief  clears the list of internal functions*/
        void clearFunctions();

        /** \brief  registers standard variables*/
        void addStandardVariables();

        /** \brief  registers standard functions*/
        void addStandardFunctions();

        /** \brief  parses the given expression*/
        qfmpNode* parse(QString prog);

        /** \brief evaluate the given expression */
        qfmpResult evaluate(QString prog);

        /** \brief  prints a list of all registered variables */
        QString printVariables();

        QList<QPair<QString, qfmpVariable> > getVariables();

        /** \brief  prints a list of all registered variables */
        QString printFunctions();

        QList<QPair<QString, qfmpFunctionDescriptor> > getFunctions();

};

#endif // QFMATHPARSER_H
/*@}*/
