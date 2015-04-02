/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

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

#ifndef QFMATHPARSERTOOLS_H
#define QFMATHPARSERTOOLS_H

#include "lib_imexport.h"
#include "qftools.h"
#include <cmath>
#include <QTextStream>

#include <QList>
#include <QMap>
#include <QList>
#include <QString>
#include <QPair>
#include <QStack>
#include "../extlibs/MersenneTwister.h"
#include <stdint.h>
#include <QDebug>

class QFMathParser; // forward



/**
 * \defgroup qfmpultil utilities for QFMathParser function parser class
 * \ingroup qf3lib_mathtools_parser
 */
/*@{*/

#define QFMATHPARSER_VERSION "0.5"


/** possible result types
 *    - \c qfmpDouble: a floating-point number with double precision. This is
 *                     also used to deal with integers
 *    - \c qfmpString: a string of characters
 *    - \c qfmpBool:   a boolean value true|false
 *  .
 */
enum qfmpResultType {qfmpDouble=0x01,  /*!< \brief a floating-point number with double precision. This is also used to deal with integers */
                     qfmpString=0x02,  /*!< \brief a string of characters */
                     qfmpBool=0x04,   /*!< \brief a boolean value true|false */
                     qfmpDoubleVector=0x08,  /*!< \brief a vector of floating point numbers */
                     qfmpStringVector=0x10, /*!< \brief a vector of strings */
                     qfmpBoolVector=0x20, /*!< \brief a vector of booleans */
                     qfmpVoid=0x40,  /*!< \brief a void/non-evaluatable result */
                     qfmpStruct=0x80,  /*!< \brief a struct datatype (qfmpStruct) */
                     qfmpCustom=0x8000,  /*!< \brief a custom datatype (qfmpCustomResult) */
                     };
QFLIB_EXPORT QString qfmpResultTypeToString(qfmpResultType type);
struct  qfmpResult; // forward


/** \brief result of any expression  */
class QFLIB_EXPORT qfmpCustomResult {
    public:
        qfmpCustomResult();
        virtual ~qfmpCustomResult();

        /** \brief convert the value this struct representens into a QString */
        QFLIB_EXPORT virtual QString toString(int precision=10) const=0;
        /** \brief returns the name of the datatype */
        QFLIB_EXPORT virtual QString typeName() const;
        /** \brief convert the value this struct representens into a QString and adds the name of the datatype in \c [...] */
        QFLIB_EXPORT virtual QString toTypeString(int precision=10) const;
        /** \brief convert the value this struct representens into a QString and adds the name of the datatype in \c [...] */
        QFLIB_EXPORT virtual int length() const;
        /** \brief clear all contained data */
        QFLIB_EXPORT virtual void clear();

        QFLIB_EXPORT QVariantList asVariantList() const;


        /** \brief return a new version of this as pointer */
        QFLIB_EXPORT virtual qfmpCustomResult* copy() const=0;

        /** \brief returns \c true, if a conversion to the given datatype is possible */
        QFLIB_EXPORT virtual bool convertsTo(qfmpResultType type) const;

        /** \brief convert the current custom result to the given datatype */
        QFLIB_EXPORT virtual void convertTo(qfmpResult& res, qfmpResultType type) const;


        QFLIB_EXPORT virtual void add(qfmpResult& result, const qfmpResult& l, const qfmpResult& r, QFMathParser* p) const;
        QFLIB_EXPORT virtual void sub(qfmpResult& result, const qfmpResult& l, const qfmpResult& r, QFMathParser* p) const;
        QFLIB_EXPORT virtual void mul(qfmpResult& result, const qfmpResult& l, const qfmpResult& r, QFMathParser* p) const;
        QFLIB_EXPORT virtual void div(qfmpResult& result, const qfmpResult& l, const qfmpResult& r, QFMathParser* p) const;
        QFLIB_EXPORT virtual void mod(qfmpResult& result, const qfmpResult& l, const qfmpResult& r, QFMathParser* p) const;
        QFLIB_EXPORT virtual void power(qfmpResult& result, const qfmpResult& l, const qfmpResult& r, QFMathParser* p) const;
        QFLIB_EXPORT virtual void bitwiseand(qfmpResult& result, const qfmpResult& l, const qfmpResult& r, QFMathParser* p) const;
        QFLIB_EXPORT virtual void bitwiseor(qfmpResult& result, const qfmpResult& l, const qfmpResult& r, QFMathParser* p) const;
        QFLIB_EXPORT virtual void logicand(qfmpResult& result, const qfmpResult& l, const qfmpResult& r, QFMathParser* p) const;
        QFLIB_EXPORT virtual void logicor(qfmpResult& result, const qfmpResult& l, const qfmpResult& r, QFMathParser* p) const;
        QFLIB_EXPORT virtual void logicnot(qfmpResult& result, const qfmpResult& l, QFMathParser *p) const;
        QFLIB_EXPORT virtual void logicnand(qfmpResult& result, const qfmpResult& l, const qfmpResult& r, QFMathParser* p) const;
        QFLIB_EXPORT virtual void logicnor(qfmpResult& result, const qfmpResult& l, const qfmpResult& r, QFMathParser* p) const;
        QFLIB_EXPORT virtual void logicxor(qfmpResult& result, const qfmpResult& l, const qfmpResult& r, QFMathParser* p) const;
        QFLIB_EXPORT virtual void neg(qfmpResult& result, const qfmpResult& l, QFMathParser* p) const;
        QFLIB_EXPORT virtual void bitwisenot(qfmpResult& result, const qfmpResult& l, QFMathParser* p) const;


        QFLIB_EXPORT virtual void compareequal(qfmpResult& result, const qfmpResult& l, const qfmpResult& r, QFMathParser* p) const;
        QFLIB_EXPORT virtual void comparenotequal(qfmpResult& result, const qfmpResult& l, const qfmpResult& r, QFMathParser* p) const;
        QFLIB_EXPORT virtual void comparegreater(qfmpResult& res, const qfmpResult& l, const qfmpResult& r, QFMathParser* p) const;
        QFLIB_EXPORT virtual void comparegreaterequal(qfmpResult& res, const qfmpResult& l, const qfmpResult& r, QFMathParser* p) const;
        QFLIB_EXPORT virtual void comparesmaller(qfmpResult& res, const qfmpResult& l, const qfmpResult& r, QFMathParser* p) const;
        QFLIB_EXPORT virtual void comparesmallerequal(qfmpResult& res, const qfmpResult& l, const qfmpResult& r, QFMathParser* p) const;

    private:
        static int counter;
};




/** \brief result of any expression  */
struct QFLIB_EXPORT qfmpResult {
    public:
        qfmpResult();
        ~qfmpResult();
        qfmpResult(double value);
        qfmpResult(int value);
        qfmpResult(QString value);
        qfmpResult(bool value);
        qfmpResult(const QVector<double> &value);
        qfmpResult(const QVector<bool> &value);
        qfmpResult(const QStringList &value);

        qfmpResult(const qfmpResult &value);
        QFLIB_EXPORT qfmpResult& operator=(const qfmpResult &value);
        QFLIB_EXPORT void set(const qfmpResult &value);

        QFLIB_EXPORT void setInvalid();
        QFLIB_EXPORT void setVoid();
        /** \brief convert the value this struct representens into a QString */
        QFLIB_EXPORT QString toString(int precision=10) const;

        /** \brief convert the value this struct representens into a QString and adds the name of the datatype in \c [...] */
        QFLIB_EXPORT QString toTypeString(int precision=10) const;

        /** \brief convert the value this struct to an integer */
        QFLIB_EXPORT int32_t toInteger() const;
        /** \brief convert the value this struct to an integer */
        QFLIB_EXPORT uint32_t toUInt() const;
        /** \brief is this result convertible to integer? */
        QFLIB_EXPORT bool isInteger() const;
        /** \brief is this result convertible to unsigned integer? */
        QFLIB_EXPORT bool isUInt() const;
        /** \brief returns the size of the result (number of characters for string, numbers of entries in vectors, 0 for void and 1 else) */
        QFLIB_EXPORT int length() const;
        /** \brief is this a custom result type */
        QFLIB_EXPORT bool isCustom() const;

        QFLIB_EXPORT void setCustomCopy(const qfmpCustomResult *val);
        QFLIB_EXPORT void setCustom( qfmpCustomResult* val);
        QFLIB_EXPORT void setDouble(double val);
        QFLIB_EXPORT void setBoolean(bool val);
        QFLIB_EXPORT void setString(const QString& val);
        QFLIB_EXPORT void setString(int size=0, QChar defaultChar=QLatin1Char(' '));
        QFLIB_EXPORT void setDoubleVec(const QVector<double>& val);
        QFLIB_EXPORT void setDoubleVec(int size=0, double defaultVal=0);
        template <typename T>
        inline void setDoubleVec(T* data, int size) {
            setDoubleVec(size);
            for (int i=0; i<size; i++) {
                numVec[i]=double(data[i]);
            }
        }

        QFLIB_EXPORT void setStruct(const QStringList& items=QStringList());

        QFLIB_EXPORT void setBoolVec(const QVector<bool>& val);
        QFLIB_EXPORT void setBoolVec(int size=0, bool defaultVal=false);

        template <typename T>
        inline void setBoolVec(T* data, int size) {
            setBoolVec(size);
            for (int i=0; i<size; i++) {
                boolVec[i]=bool(data[i]);
            }
        }

        QFLIB_EXPORT void setStringVec(const QStringList& val);
        QFLIB_EXPORT void setStringVec(int size=0, const QString& defaultVal=QString(""));


        template <typename T>
        inline void setStringVec(T* data, int size) {
            setStringVec(size);
            for (int i=0; i<size; i++) {
                strVec[i]=data[i];
            }
        }
        /** \brief converst the result to a vector of number (numbers and number vectors are converted!) */
        QFLIB_EXPORT QVector<double> asVector() const;
        /** \brief converst the result to a list of QVariants */
        QFLIB_EXPORT QVariantList asVariantList() const;
        /** \brief converst the result to a single QVariant */
        QFLIB_EXPORT QVariant asVariant() const;
        /** \brief initializes from a QVariant
         *
         * \return \c true on success
         */
        QFLIB_EXPORT bool setVariant(const QVariant& data, bool convertLists=true);
        /** \brief converst the result to a vector of number (numbers and number vectors are converted!) */
        QFLIB_EXPORT QStringList asStrVector() const;
        /** \brief converst the result to a vector of number (numbers and number vectors are converted!) */
        QFLIB_EXPORT QVector<bool> asBoolVector() const;
        /** \brief returns \c true, if the result may be converted to a vector of number */
        QFLIB_EXPORT bool  convertsToVector() const;
        /** \brief returns \c true, if the result may be converted to a vector of number */
        QFLIB_EXPORT bool  convertsToBoolVector() const;
        /** \brief returns \c true, if the result may be converted to a vector of number */
        QFLIB_EXPORT bool  convertsToStringVector() const;
        /** \brief converst the result to a vector of integers (numbers and number vectors are converted!) */
        QFLIB_EXPORT QVector<int> asIntVector() const;
        /** \brief returns \c true, if the result may be converted to a vector of integers */
        QFLIB_EXPORT bool  convertsToIntVector() const;
        /** \brief returns \c true if the result is valid and not void */
        QFLIB_EXPORT bool isUsableResult() const;
        /** \brief converst the result to a number (numbers are converted!) */
        QFLIB_EXPORT double asNumber() const;
        /** \brief converst the result to a number (numbers are converted and from a number vector the first element is returned!) */
        QFLIB_EXPORT double asNumberAlsoVector() const;
        /** \brief converst the result to a number (numbers are converted and from a number vector the first element is returned!) */
        QFLIB_EXPORT QString asStringAlsoVector() const;
        /** \brief converst the result to a number (numbers are converted and from a number vector the first element is returned!) */
        QFLIB_EXPORT bool asBooleanAlsoVector() const;
        /** \brief converst the result to a string (strings are converted!) */
        QFLIB_EXPORT QString asString() const;
        /** \brief converst the result to a boolean (numbers and booleans are converted!) */
        QFLIB_EXPORT bool asBool() const;
        /** \brief returns the type */
        QFLIB_EXPORT qfmpResultType getType() const;
        /** \brief returns a string, describing the type of this result */
        QFLIB_EXPORT QString typeName() const;

        /** \brief returns an invalid result */
        QFLIB_EXPORT static qfmpResult invalidResult();
        /** \brief returns an void result */
        QFLIB_EXPORT static qfmpResult voidResult();

        /** \brief returns an entry from a struct */
        QFLIB_EXPORT qfmpResult getStructItem(const QString& item);

        /** \brief returns an entry from a struct */
        QFLIB_EXPORT void setStructItem(const QString& item, const qfmpResult& value);
        /** \brief returns an entry from a struct */
        QFLIB_EXPORT void setStruct(const QMap<QString,qfmpResult>& data);


        QFLIB_EXPORT static void add(qfmpResult& result, const qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void sub(qfmpResult& result, const qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void mul(qfmpResult& result, const qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void div(qfmpResult& result, const qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void mod(qfmpResult& result, const qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void power(qfmpResult& result, const qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void bitwiseand(qfmpResult& result, const qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void bitwiseor(qfmpResult& result, const qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void logicand(qfmpResult& result, const qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void logicor(qfmpResult& result, const qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void logicnot(qfmpResult& result, const qfmpResult& l, QFMathParser *p);
        QFLIB_EXPORT static void logicnand(qfmpResult& result, const qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void logicnor(qfmpResult& result, const qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void logicxor(qfmpResult& result, const qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void neg(qfmpResult& result, const qfmpResult& l, QFMathParser* p);
        QFLIB_EXPORT static void bitwisenot(qfmpResult& result, const qfmpResult& l, QFMathParser* p);



        QFLIB_EXPORT bool operator==(const qfmpResult& other) const;
        QFLIB_EXPORT bool operator!=(const qfmpResult& other) const;

        QFLIB_EXPORT static void compareequal(qfmpResult& res, const qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void comparenotequal(qfmpResult& res, const qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void comparegreater(qfmpResult& res, const qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void comparegreaterequal(qfmpResult& res, const qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void comparesmaller(qfmpResult& res, const qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void comparesmallerequal(qfmpResult& res, const qfmpResult& l, const qfmpResult& r, QFMathParser* p);

        bool isValid;
        qfmpResultType type;   /*!< \brief type of the result */
        QString str;       /*!< \brief contains result if \c type==qfmpString */
        double num;            /*!< \brief contains result if \c type==qfmpDouble */
        bool boolean;          /*!< \brief contains result if \c type==qfmpBool */
        QVector<double> numVec; /*!< \brief contains result if \c type==qfmpDoubleVector */
        QStringList strVec;
        QVector<bool> boolVec;
        QMap<QString,qfmpResult> structData;


        /*!< \brief clear the current qfmpCustomResult object */
        QFLIB_EXPORT void clearCustom();
        /*!< \brief returns a pointer to the current qfmpCustomResult object */
        inline qfmpCustomResult* custom() const { return m_custom; }
    private:
        qfmpCustomResult* m_custom;
};













/*! \brief This macro allows to easily define functions for QFMathParser from a C-function that
           is numeric->numeric, e.g. sin(x). The result is numeric->numeric or num_vector->num_vector

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
static inline void FName(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){\
    if (n!=1) {\
        p->qfmpError(QObject::tr("%1(...) needs exacptly 1 argument").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return ;\
    }\
    if (params[0].type==qfmpDouble) {\
        r.setDouble(CFUNC(params[0].num));\
    } else if(params[0].type==qfmpDoubleVector) {\
        r.setDoubleVec(params[0].numVec.size());\
        for (int i=0; i<params[0].numVec.size(); i++) {\
            r.numVec[i]=CFUNC(params[0].numVec[i]);\
        }\
    } else {\
        p->qfmpError(QObject::tr("%1(...) argument has to be a number or vector of numbers").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
    }\
}

/** \brief same as QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC(FName, NAME_IN_PARSER, CFUNC), but NAME_IN_PARSER==CFUNC */
#define QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC_SIMPLE(FName, CFUNC) QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC(FName, CFUNC, CFUNC)



/*! \brief This macro allows to easily define functions for QFMathParser from a C-function that
           is numeric_vector->numeric_vector (QVector<double> -> QVector<double>)

    The resulting function will:
      - check the number of arguments and their type
      - apply the C-function to any number parameter
    .

    \param FName name of the function to declare
    \param NAME_IN_PARSER name the function should have in the parser (used for error messages only)
    \param CFUNC name of the C function to call
*/
#define QFMATHPARSER_DEFINE_1PARAM_NUMERICVEC_FUNC(FName, NAME_IN_PARSER, CFUNC) \
static inline void FName(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){\
    if (n!=1) {\
        p->qfmpError(QObject::tr("%1(x) needs exacptly 1 argument").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    if(params[0].type==qfmpDoubleVector) {\
        r.setDoubleVec(CFUNC(params[0].numVec));\
    } else {\
        p->qfmpError(QObject::tr("%1(x) argument has to be vector of numbers").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
    }\
    return; \
}


/*! \brief This macro allows to easily define functions for QFMathParser from a C template-function that
           is vector->vector it has to accept QList and QVector as arguments! The result of the parser
           function will have the same type as the input!

    The resulting function will:
      - check the number of arguments and their type
      - apply the C-function to any number parameter
    .

    \param FName name of the function to declare
    \param NAME_IN_PARSER name the function should have in the parser (used for error messages only)
    \param CFUNC name of the C function to call
*/
#define QFMATHPARSER_DEFINE_1PARAM_VECTOR_FUNC(FName, NAME_IN_PARSER, CFUNC) \
static inline void FName(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){\
    if (n!=1) {\
        p->qfmpError(QObject::tr("%1(x) needs exacptly 1 argument").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    if(params[0].type==qfmpDoubleVector) {\
        r.setDoubleVec(CFUNC(params[0].numVec));\
    } else if(params[0].type==qfmpStringVector) {\
        r.setStringVec(CFUNC(params[0].strVec));\
    } else if(params[0].type==qfmpBoolVector) {\
        r.setBoolVec(CFUNC(params[0].boolVec));\
    } else {\
        p->qfmpError(QObject::tr("%1(x) argument has to be a vector of numbers/booleans/strings").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
    }\
    return; \
}



/*! \brief This macro allows to easily define functions for QFMathParser from a C-function that
           is numeric_vector->numeric (QVector<double> -> double), e.g. qfstatisticsMedian()

    The resulting function will:
      - check the number of arguments and their type
      - apply the C-function to the argument
    .

    \param FName name of the function to declare
    \param NAME_IN_PARSER name the function should have in the parser (used for error messages only)
    \param CFUNC name of the C function to call
*/
#define QFMATHPARSER_DEFINE_1PARAM_VECTONUM_FUNC(FName, NAME_IN_PARSER, CFUNC) \
static inline void FName(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){\
    if (n!=1) {\
        p->qfmpError(QObject::tr("%1(x) needs exacptly 1 argument").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    if(params[0].type==qfmpDoubleVector) {\
        r.setDouble(CFUNC(params[0].numVec));\
    } else {\
        p->qfmpError(QObject::tr("%1(x) argument has to be a vector of numbers").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
    }\
    return; \
}



/*! \brief This macro allows to easily define functions for QFMathParser from a C-function that
           is numeric_vector->numeric (QVector<double> -> double), e.g. qfstatisticsMedian()

    This variant also accepts a single double-number and converts it to a QVector<double> before evaluating the function.

    The resulting function will:
      - check the number of arguments and their type
      - apply the C-function to the argument
    .

    \param FName name of the function to declare
    \param NAME_IN_PARSER name the function should have in the parser (used for error messages only)
    \param CFUNC name of the C function to call
*/
#define QFMATHPARSER_DEFINE_1PARAM_VECORNUMTONUM_FUNC(FName, NAME_IN_PARSER, CFUNC) \
static inline void FName(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){\
    if (n!=1) {\
        p->qfmpError(QObject::tr("%1(x) needs exacptly 1 argument").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    if(params[0].type==qfmpDoubleVector) {\
        r.setDouble(CFUNC(params[0].numVec));\
    } if(params[0].type==qfmpDouble) {\
        r.setDouble(CFUNC(constructQVectorFromItems<double>(params[0].num)));\
    } else {\
        p->qfmpError(QObject::tr("%1(x) argument has to be a vector of numbers or a number").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
    }\
    return; \
}



/*! \brief This macro allows to easily define functions for QFMathParser from a C-function that
           is numeric_vector->numeric (QVector<double> -> double), e.g. qfstatisticsMedian()

    This variant also accepts a single double-number, or a list of numbers and converts it to a QVector<double> before evaluating the function.

    The resulting function will:
      - check the number of arguments and their type
      - apply the C-function to the argument
    .

    \param FName name of the function to declare
    \param NAME_IN_PARSER name the function should have in the parser (used for error messages only)
    \param CFUNC name of the C function to call
*/
#define QFMATHPARSER_DEFINE_1PARAM_VECORNUMSTONUM_FUNC(FName, NAME_IN_PARSER, CFUNC) \
static inline void FName(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){\
    if (n<1) {\
        p->qfmpError(QObject::tr("%1(x1, x2, ...) needs at least 1 argument").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    if(n==1 && params[0].type==qfmpDoubleVector) {\
        r.setDouble(CFUNC(params[0].numVec));\
    } if( n>=1) {\
        QVector<double> d; \
        for (unsigned int i=0; i<n; i++) {\
            if (params[i].type==qfmpDouble) { \
                d<<params[i].num;     \
            } else if (params[i].type==qfmpDoubleVector) {\
                d<<params[i].numVec;     \
            } else {\
                p->qfmpError(QObject::tr("%1(x1, x2, ...) argument %2 has to be a vector of numbers or a number").arg(#NAME_IN_PARSER).arg(i+1));\
                r.setInvalid();\
                return; \
            }\
        } \
        r.setDouble(CFUNC(d));\
    } else {\
        p->qfmpError(QObject::tr("%1(x1, x2, ...) argument 1 has to be a vector of numbers or a number").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
    }\
    return; \
}

/*! \brief This macro allows to easily define functions for QFMathParser from a C-function that
           is (numeric_vector, numeric)->number , e.g. qfstatisticsQuantile()

    The resulting function will:
      - check the number of arguments and their type
      - apply the C-function to the argument
    .

    \param FName name of the function to declare
    \param NAME_IN_PARSER name the function should have in the parser (used for error messages only)
    \param CFUNC name of the C function to call
*/
#define QFMATHPARSER_DEFINE_2PARAM1VEC_VECTONUM_FUNC(FName, NAME_IN_PARSER, CFUNC) \
static inline void FName(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){\
    if (n!=2) {\
        p->qfmpError(QObject::tr("%1(x, p) needs exacptly 2 argument").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    if(params[0].type==qfmpDoubleVector && params[1].type==qfmpDouble) {\
        r.setDouble(CFUNC(params[0].numVec, params[1].num));\
    } else {\
        p->qfmpError(QObject::tr("%1(x, p) argument x has to be a vector of numbers and p a number").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
    }\
    return; \
}

/*! \brief This macro allows to easily define functions for QFMathParser from a C-function that
           is (numeric_vector, numeric)->number. If the second argument is also a vector, the function
           will be applied element-wise to this vector to build an output vector!

    The resulting function will:
      - check the number of arguments and their type
      - apply the C-function to the argument
    .

    \param FName name of the function to declare
    \param NAME_IN_PARSER name the function should have in the parser (used for error messages only)
    \param CFUNC name of the C function to call
*/
#define QFMATHPARSER_DEFINE_2PARAM1VEC2POSVEC_VECTONUM_FUNC(FName, NAME_IN_PARSER, CFUNC) \
static inline void FName(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){\
    if (n!=2) {\
        p->qfmpError(QObject::tr("%1(x, p) needs exacptly 2 argument").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    if(params[0].type==qfmpDoubleVector && params[1].type==qfmpDouble) {\
        r.setDouble(CFUNC(params[0].numVec, params[1].num));\
    } else if(params[0].type==qfmpDoubleVector && params[1].type==qfmpDoubleVector) {\
        QVector<double> res=params[1].numVec;\
        for (int i=0; i<params[1].numvec.size(); i++) {\
            res[i]=CFUNC(params[0].numVec, res[i]);\
        }\
        r.setDoubleVec(res);\
    } else {\
        p->qfmpError(QObject::tr("%1(x, p) argument x has to be a vector of numbers and p a number").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
    }\
    return; \
}

/*! \brief This macro allows to easily define functions for QFMathParser from a C-function that
           is (numeric_vector, numeric)->number. If the second argument is also a vector, the function
           will be applied element-wise to this vector to build an output vector!
           The C-function expects (double* data, int length) and not a QVector<double> as parameter!)

    The resulting function will:
      - check the number of arguments and their type
      - apply the C-function to the argument
    .

    \param FName name of the function to declare
    \param NAME_IN_PARSER name the function should have in the parser (used for error messages only)
    \param CFUNC name of the C function to call
*/
#define QFMATHPARSER_DEFINE_2PARAM1VEC2POSVEC_VECTONUM_CFUNC(FName, NAME_IN_PARSER, CFUNC) \
static inline void FName(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){\
    if (n!=2) {\
        p->qfmpError(QObject::tr("%1(x, p) needs exacptly 2 argument").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    if(params[0].type==qfmpDoubleVector && params[1].type==qfmpDouble) {\
        r.setDouble(CFUNC(params[0].numVec, params[1].num));\
    } else if(params[0].type==qfmpDoubleVector && params[1].type==qfmpDoubleVector) {\
        QVector<double> res=params[1].numVec;\
        for (int i=0; i<params[1].numvec.size(); i++) {\
            res[i]=CFUNC(params[0].numVec.data(), params[0].numVec.size(), res[i]);\
        }\
    r.setDoubleVec(res);\
    } else {\
        p->qfmpError(QObject::tr("%1(x, p) argument x has to be a vector of numbers and p a number").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
    }\
    return; \
}


/*! \brief This macro allows to easily define functions for QFMathParser from a C-function that
           is (numeric_vector, numeric)->number. If the second argument is also a vector, the function
           will be applied element-wise to this vector to build an output vector!

    The resulting function will:
      - check the number of arguments and their type
      - apply the C-function to the argument
    .

    \param FName name of the function to declare
    \param NAME_IN_PARSER name the function should have in the parser (used for error messages only)
    \param CFUNC name of the C function to call
*/
#define QFMATHPARSER_DEFINE_2PARAM1NUM2VEC_VECTONUM_FUNC(FName, NAME_IN_PARSER, CFUNC) \
static inline void FName(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){\
    if (n!=2) {\
        p->qfmpError(QObject::tr("%1(x, p) needs exacptly 2 argument").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    if(params[0].type==qfmpDouble && params[1].type==qfmpDoubleVector) {\
        r.setDouble(CFUNC(params[0].num, params[1].numVec));\
    } else if(params[0].type==qfmpDoubleVector && params[1].type==qfmpDoubleVector) {\
        QVector<double> res=params[0].numVec;\
        for (int i=0; i<res.size(); i++) {\
            res[i]=CFUNC(res[i], params[1].numVec);\
        }\
        r.setDoubleVec(res);\
    } else {\
        p->qfmpError(QObject::tr("%1(x, p) argument x has to be a number or a vector of numbers and p a vector of number").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
    }\
    return; \
}

/*! \brief This macro allows to easily define functions for QFMathParser from a C-function that
           is (numeric, numeric_vector)->number. If the second argument is also a vector, the function
           will be applied element-wise to this vector to build an output vector!
           The C-function expects (double, double* data, int length) and not a QVector<double> as parameter!)

    The resulting function will:
      - check the number of arguments and their type
      - apply the C-function to the argument
    .

    \param FName name of the function to declare
    \param NAME_IN_PARSER name the function should have in the parser (used for error messages only)
    \param CFUNC name of the C function to call
*/
#define QFMATHPARSER_DEFINE_2PARAM1NUM2VEC_VECTONUM_CFUNC(FName, NAME_IN_PARSER, CFUNC) \
static inline void FName(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){\
    if (n!=2) {\
        p->qfmpError(QObject::tr("%1(x, p) needs exacptly 2 argument").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    if(params[0].type==qfmpDouble && params[1].type==qfmpDoubleVector) {\
        r.setDouble(CFUNC(params[0].num, params[1].numVec.data(), params[1].numVec.size()));\
    } else if(params[0].type==qfmpDoubleVector && params[1].type==qfmpDoubleVector) {\
        QVector<double> res=params[0].numVec;\
        for (int i=0; i<res.size(); i++) {\
            res[i]=CFUNC(res[i], params[1].numVec.data(), params[1].numVec.size());\
        }\
        r.setDoubleVec(res);\
    } else {\
        p->qfmpError(QObject::tr("%1(x, p) argument x has to be a number or a vector of numbers and p a vector of number").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
    }\
    return; \
}



/*! \brief This macro allows to easily define functions for QFMathParser from a C-function that
           is (numeric_vector, numeric, numeric)->number

    The resulting function will:
      - check the number of arguments and their type
      - apply the C-function to the argument
    .

    \param FName name of the function to declare
    \param NAME_IN_PARSER name the function should have in the parser (used for error messages only)
    \param CFUNC name of the C function to call
*/
#define QFMATHPARSER_DEFINE_3PARAM1VEC_VECTONUM_FUNC(FName, NAME_IN_PARSER, CFUNC) \
static inline void FName(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){\
    if (n!=3) {\
        p->qfmpError(QObject::tr("%1(x, p1, p2) needs exacptly 3 argument").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    if(params[0].type==qfmpDoubleVector && params[1].type==qfmpDouble && params[2].type==qfmpDouble) {\
        r.setDouble(CFUNC(params[0].numVec, params[1].num, params[2].num));\
    } else {\
        p->qfmpError(QObject::tr("%1(x, p) argument x has to be a vector of numbers and p a number").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
    }\
    return; \
}

/*! \brief This macro allows to easily define functions for QFMathParser from a C-function that
           is (numeric_vector, numeric_vector)->numeric, e.g. qfstatisticsCorrelationCoefficient()

    The resulting function will:
      - check the number of arguments and their type
      - apply the C-function to the argument
    .

    \param FName name of the function to declare
    \param NAME_IN_PARSER name the function should have in the parser (used for error messages only)
    \param CFUNC name of the C function to call
*/
#define QFMATHPARSER_DEFINE_2PARAM2VEC_VECTONUM_FUNC(FName, NAME_IN_PARSER, CFUNC) \
static inline void FName(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){\
    if (n!=2) {\
        p->qfmpError(QObject::tr("%1(x, y) needs exactly 2 argument").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    if(params[0].type==qfmpDoubleVector && params[1].type==qfmpDoubleVector) {\
        r.setDouble(CFUNC(params[0].asVector(), params[1].asVector()));\
    } else {\
        p->qfmpError(QObject::tr("%1(x, y) arguments x and y have to be a vector of numbers").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
    }\
    return; \
}


/*! \brief This macro allows to easily define functions for QFMathParser from a C-function that
           is string->string.  The result is string->string or str_vector->str_vector.

    The resulting function will:
      - check the number of arguments
      - apply the C-function to any string parameter
      - apply the C-function item-wise to any string vector parameter item
      - result is also string
    .

    \param FName name of the function to declare
    \param NAME_IN_PARSER name the function should have in the parser (used for error messages only)
    \param CFUNC name of the C function to call
*/
#define QFMATHPARSER_DEFINE_1PARAM_STRING_FUNC(FName, NAME_IN_PARSER, CFUNC) \
static inline void FName(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){\
    r.type=qfmpString;\
    r.isValid=true;\
    if (n!=1) {\
        p->qfmpError(QObject::tr("%1(...) needs exacptly 1 argument").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    if (params[0].type==qfmpString) {\
        r.str=CFUNC(params[0].str);\
    } else if (params[0].type==qfmpStringVector) {\
        r.setStringVec(params[0].strVec.size());\
        for (int i=0; i<params[0].strVec.size(); i++) {\
            r.strVec[i]=CFUNC(params[0].strVec[i]);\
        }\
    } else {\
        p->qfmpError(QObject::tr("%1(...) argument has to be a string or string-vector").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
    }\
    return; \
}

/** \brief same as QFMATHPARSER_DEFINE_1PARAM_STRING_FUNC_SIMPLE(FName, NAME_IN_PARSER, CFUNC), but NAME_IN_PARSER==CFUNC */
#define QFMATHPARSER_DEFINE_1PARAM_STRING_FUNC_SIMPLE(FName, CFUNC) QFMATHPARSER_DEFINE_1PARAM_STRING_FUNC(FName, CFUNC, CFUNC)


/*! \brief This macro allows to easily define functions for QFMathParser from a C-function that
           is string->str_vector.  The result is string->str_vector.

    The resulting function will:
      - check the number of arguments
      - apply the C-function to any string parameter
      - apply the C-function item-wise to any string vector parameter item
      - result is also string
    .

    \param FName name of the function to declare
    \param NAME_IN_PARSER name the function should have in the parser (used for error messages only)
    \param CFUNC name of the C function to call
*/
#define QFMATHPARSER_DEFINE_1PARAM_STRING2STRINGVEC_FUNC(FName, NAME_IN_PARSER, CFUNC) \
static inline void FName(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){\
    r.type=qfmpStringVector;\
    r.strVec.clear();\
    r.isValid=true;\
    if (n!=1) {\
        p->qfmpError(QObject::tr("%1(...) needs exacptly 1 argument").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    if (params[0].type==qfmpString) {\
        r.strVec=CFUNC(params[0].str);\
    } else {\
        p->qfmpError(QObject::tr("%1(...) argument has to be a string").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
    }\
    return; \
}



/*! \brief This macro allows to easily define functions for QFMathParser from a C-function that
           is numeric->bool.  The result is numeric->bool or num_vector->bool_vector.

    The resulting function will:
      - check the number of arguments
      - apply the C-function to any string parameter
      - apply the C-function item-wise to any string vector parameter item
      - result is also string
    .

    \param FName name of the function to declare
    \param NAME_IN_PARSER name the function should have in the parser (used for error messages only)
    \param CFUNC name of the C function to call
*/
#define QFMATHPARSER_DEFINE_1PARAM_NUM2BOOL_FUNC(FName, NAME_IN_PARSER, CFUNC) \
static inline void FName(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){\
    r.type=qfmpBool;\
    r.isValid=false;\
    if (n!=1) {\
        p->qfmpError(QObject::tr("%1(...) needs exacptly 1 argument").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    if (params[0].type==qfmpDouble) {\
        r.setBoolean(CFUNC(params[0].num));\
    } else if (params[0].type==qfmpDoubleVector) {\
        r.setBoolVec(params[0].numVec.size());\
        for (int i=0; i<params[0].numVec.size(); i++) {\
            r.boolVec[i]=CFUNC(params[0].numVec[i]);\
        }\
    } else {\
        p->qfmpError(QObject::tr("%1(...) argument has to be a number or number-vector").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
    }\
    return; \
}

/** \brief same as QFMATHPARSER_DEFINE_1PARAM_NUM2BOOL_FUNC_SIMPLE(FName, NAME_IN_PARSER, CFUNC), but NAME_IN_PARSER==CFUNC */
#define QFMATHPARSER_DEFINE_1PARAM_NUM2BOOL_FUNC_SIMPLE(FName, CFUNC) QFMATHPARSER_DEFINE_1PARAM_NUM2STRING_FUNC(FName, CFUNC, CFUNC)



/*! \brief This macro allows to easily define functions for QFMathParser from a C-function that
           is numeric->string.  The result is numeric->string or num_vector->str_vector.

    The resulting function will:
      - check the number of arguments
      - apply the C-function to any string parameter
      - apply the C-function item-wise to any string vector parameter item
      - result is also string
    .

    \param FName name of the function to declare
    \param NAME_IN_PARSER name the function should have in the parser (used for error messages only)
    \param CFUNC name of the C function to call
*/
#define QFMATHPARSER_DEFINE_1PARAM_NUM2STRING_FUNC(FName, NAME_IN_PARSER, CFUNC) \
static inline void FName(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){\
    r.type=qfmpString;\
    r.isValid=true;\
    if (n!=1) {\
        p->qfmpError(QObject::tr("%1(...) needs exacptly 1 argument").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    if (params[0].type==qfmpDouble) {\
        r.str=CFUNC(params[0].num);\
    } else if (params[0].type==qfmpDoubleVector) {\
        r.setStringVec(params[0].numVec.size());\
        for (int i=0; i<params[0].numVec.size(); i++) {\
            r.strVec[i]=CFUNC(params[0].numVec[i]);\
        }\
    } else {\
        p->qfmpError(QObject::tr("%1(...) argument has to be a number or number-vector").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
    }\
    return; \
}




/*! \brief This macro allows to easily define functions for QFMathParser from a C-function that
           is string->numeric.  The result is string->numeric or str_vector->num_vector.

    The resulting function will:
      - check the number of arguments
      - apply the C-function to any string parameter
      - apply the C-function item-wise to any string vector parameter item
      - result is also string
    .

    \param FName name of the function to declare
    \param NAME_IN_PARSER name the function should have in the parser (used for error messages only)
    \param CFUNC name of the C function to call
*/
#define QFMATHPARSER_DEFINE_1PARAM_STRING2NUM_FUNC(FName, NAME_IN_PARSER, CFUNC) \
static inline void FName(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){\
    r.setInvalid();\
    if (n!=1) {\
        p->qfmpError(QObject::tr("%1(...) needs exacptly 1 argument").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    if (params[0].type==qfmpString) {\
        r.setDouble(CFUNC(params[0].str));\
    } else if (params[0].type==qfmpStringVector) {\
        r.setDoubleVec(params[0].strVec.size());\
        for (int i=0; i<params[0].strVec.size(); i++) {\
            r.numVec[i]=CFUNC(params[0].strVec[i]);\
        }\
    } else {\
        p->qfmpError(QObject::tr("%1(...) argument has to be a string or string-vector").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
    }\
    return; \
}



/*! \brief This macro allows to easily define functions for QFMathParser from a C-function that
           is string->boolean.  The result is string->boolean or str_vector->boolean_vector.

    The resulting function will:
      - check the number of arguments
      - apply the C-function to any string parameter
      - apply the C-function item-wise to any string vector parameter item
      - result is also string
    .

    \param FName name of the function to declare
    \param NAME_IN_PARSER name the function should have in the parser (used for error messages only)
    \param CFUNC name of the C function to call
*/
#define QFMATHPARSER_DEFINE_1PARAM_STRING2BOOL_FUNC(FName, NAME_IN_PARSER, CFUNC) \
static inline void FName(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){\
    r.type=qfmpBool;\
    r.isValid=true;\
    if (n!=1) {\
        p->qfmpError(QObject::tr("%1(...) needs exacptly 1 argument").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    if (params[0].type==qfmpString) {\
        r.boolean=CFUNC(params[0].str);\
    } else if (params[0].type==qfmpStringVector) {\
        r.setBoolVec(params[0].strVec.size());\
        for (int i=0; i<params[0].strVec.size(); i++) {\
            r.boolVec[i]=CFUNC(params[0].strVec[i]);\
        }\
    } else {\
        p->qfmpError(QObject::tr("%1(...) argument has to be a string or string-vector").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
    }\
    return; \
}

/** \brief same as QFMATHPARSER_DEFINE_1PARAM_NUM2STRING_FUNC_SIMPLE(FName, NAME_IN_PARSER, CFUNC), but NAME_IN_PARSER==CFUNC */
#define QFMATHPARSER_DEFINE_1PARAM_NUM2STRING_FUNC_SIMPLE(FName, CFUNC) QFMATHPARSER_DEFINE_1PARAM_NUM2STRING_FUNC(FName, CFUNC, CFUNC)


/*! \brief This macro allows to easily define functions for QFMathParser from a C-function that
           is bool->string.  The result is bool->string or bool_vector->str_vector.

    The resulting function will:
      - check the number of arguments
      - apply the C-function to any bool parameter
      - apply the C-function item-wise to any bool vector parameter item
      - result is also string
    .

    \param FName name of the function to declare
    \param NAME_IN_PARSER name the function should have in the parser (used for error messages only)
    \param CFUNC name of the C function to call
*/
#define QFMATHPARSER_DEFINE_1PARAM_BOOL2STRING_FUNC(FName, NAME_IN_PARSER, CFUNC) \
static inline void FName(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){\
    r.type=qfmpString;\
    r.isValid=true;\
    if (n!=1) {\
        p->qfmpError(QObject::tr("%1(...) needs exacptly 1 argument").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    if (params[0].type==qfmpBool) {\
        r.str=CFUNC(params[0].boolean);\
    } else if (params[0].type==qfmpBoolVector) {\
        r.setStringVec(params[0].boolVec.size());\
        for (int i=0; i<params[0].boolVec.size(); i++) {\
            r.strVec[i]=CFUNC(params[0].boolVec[i]);\
        }\
    } else {\
        p->qfmpError(QObject::tr("%1(...) argument has to be a boolean or boolean-vector").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
    }\
    return; \
}

/** \brief same as QFMATHPARSER_DEFINE_1PARAM_BOOL2STRING_FUNC_SIMPLE(FName, NAME_IN_PARSER, CFUNC), but NAME_IN_PARSER==CFUNC */
#define QFMATHPARSER_DEFINE_1PARAM_BOOL2STRING_FUNC_SIMPLE(FName, CFUNC) QFMATHPARSER_DEFINE_1PARAM_NUM2STRING_FUNC(FName, CFUNC, CFUNC)



/*! \brief This macro allows to easily define functions for QFMathParser from a C-function that
           is (numeric, numeric_vector)->numeric.  The result is (numeric, numeric)->numeric or (numeric, num_vector)->num_vector.

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
static inline void FName(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){\
    r.type=qfmpDouble;\
    r.isValid=true;\
    if (n!=2) {\
        p->qfmpError(QObject::tr("%1(...) needs exactly 2 argument").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    double pa=0; \
    if (params[0].type==qfmpDouble) {\
        pa=params[0].num;\
    } else {\
        p->qfmpError(QObject::tr("%1(...) first argument has to be a number").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    if (params[1].type==qfmpDouble) {\
        r.num=CFUNC(pa, params[1].num);\
    } else if(params[1].type==qfmpDoubleVector) {\
        r.type=qfmpDoubleVector;\
        r.numVec.resize(params[1].numVec.size());\
        for (int i=0; i<params[1].numVec.size(); i++) {\
            r.numVec[i]=CFUNC(pa, params[1].numVec[i]);\
        }\
    } else {\
        p->qfmpError(QObject::tr("%1(...) second argument has to be a number or vector of numbers").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
    }\
    return; \
}

/*! \brief This macro allows to easily define functions for QFMathParser from a C-function that
           is (numeric, numeric)->numeric, e.g. yn(x, p), p is a number, x may be a vector
             The result is (numeric, numeric)->numeric or (num_vector, numeric)->num_vector.

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
static inline void FName(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){\
    r.isValid=true;\
    r.type=qfmpDouble;\
    if (n!=2) {\
        p->qfmpError(QObject::tr("%1(...) needs exactly 2 argument").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    double pa=0; \
    if (params[1].type==qfmpDouble) {\
        pa=params[1].num;\
    } else {\
        p->qfmpError(QObject::tr("%1(...) second argument has to be a number").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    if (params[0].type==qfmpDouble) {\
        r.num=CFUNC(params[0].num, pa);\
    } else if(params[0].type==qfmpDoubleVector) {\
        r.type=qfmpDoubleVector;\
        r.numVec.resize(params[0].numVec.size());\
        for (int i=0; i<params[0].numVec.size(); i++) {\
            r.numVec[i]=CFUNC(params[0].numVec[i], pa);\
        }\
    } else {\
        p->qfmpError(QObject::tr("%1(...) first argument has to be a number or vector of numbers").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
    }\
    return; \
}








/*! \brief This macro allows to easily define functions for QFMathParser from a C-function that
           is (numeric, numeric, numeric)->numeric
             The result is (numeric, numeric, numeric)->numeric or (num_vector, numeric, numeric)->num_vector.

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
#define QFMATHPARSER_DEFINE_3PARAM1VEC_NUMERIC_FUNC(FName, NAME_IN_PARSER, CFUNC) \
static inline void FName(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){\
    r.isValid=true;\
    r.type=qfmpDouble;\
    if (n!=3) {\
        p->qfmpError(QObject::tr("%1(...) needs exactly 3 argument").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    double pa=0; \
    double pb=0; \
    if (params[1].type==qfmpDouble) {\
        pa=params[1].num;\
    } else {\
        p->qfmpError(QObject::tr("%1(...) second argument has to be a number").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    if (params[2].type==qfmpDouble) {\
        pb=params[2].num;\
    } else {\
        p->qfmpError(QObject::tr("%1(...) third argument has to be a number").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    if (params[0].type==qfmpDouble) {\
        r.num=CFUNC(params[0].num, pa, pb);\
    } else if(params[0].type==qfmpDoubleVector) {\
        r.type=qfmpDoubleVector;\
        r.numVec.resize(params[0].numVec.size());\
        for (int i=0; i<params[0].numVec.size(); i++) {\
            r.numVec[i]=CFUNC(params[0].numVec[i], pa, pb);\
        }\
    } else {\
        p->qfmpError(QObject::tr("%1(...) first argument has to be a number or vector of numbers").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
    }\
    return; \
}




/*! \brief This macro allows to easily define functions for QFMathParser from a C-function that
           is (numeric, numeric, numeric, numeric)->numeric
             The result is (numeric, numeric, numeric, numeric)->numeric or (num_vector, numeric, numeric, numeric)->num_vector.

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
#define QFMATHPARSER_DEFINE_4PARAM1VEC_NUMERIC_FUNC(FName, NAME_IN_PARSER, CFUNC) \
static inline void FName(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){\
    r.isValid=true;\
    r.type=qfmpDouble;\
    if (n!=4) {\
        p->qfmpError(QObject::tr("%1(...) needs exactly 4 argument").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    double pa=0; \
    double pb=0; \
    double pc=0; \
    if (params[1].type==qfmpDouble) {\
        pa=params[1].num;\
    } else {\
        p->qfmpError(QObject::tr("%1(...) second argument has to be a number").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    if (params[2].type==qfmpDouble) {\
        pb=params[2].num;\
    } else {\
        p->qfmpError(QObject::tr("%1(...) third argument has to be a number").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    if (params[3].type==qfmpDouble) {\
        pc=params[3].num;\
    } else {\
        p->qfmpError(QObject::tr("%1(...) fourth argument has to be a number").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    if (params[0].type==qfmpDouble) {\
        r.num=CFUNC(params[0].num, pa, pb, pc);\
    } else if(params[0].type==qfmpDoubleVector) {\
        r.type=qfmpDoubleVector;\
        r.numVec.resize(params[0].numVec.size());\
        for (int i=0; i<params[0].numVec.size(); i++) {\
            r.numVec[i]=CFUNC(params[0].numVec[i], pa, pb, pc);\
        }\
    } else {\
        p->qfmpError(QObject::tr("%1(...) first argument has to be a number or vector of numbers").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
    }\
    return; \
}

/*! \brief This macro allows to easily define functions for QFMathParser from a C-function that
           is (numeric, numeric)->numeric, e.g. yn(p, x), p is a number, x may be a vector
             The result is (numeric, numeric)->numeric or (num_vector, numeric)->num_vector.
           The c-function takes the vector-argument as LAST argument!

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
#define QFMATHPARSER_DEFINE_2PARAMLVEC_NUMERIC_FUNC(FName, NAME_IN_PARSER, CFUNC) \
static inline void FName(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){\
    r.isValid=true;\
    r.type=qfmpDouble;\
    if (n!=2) {\
        p->qfmpError(QObject::tr("%1(...) needs exactly 2 argument").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    double pa=0; \
    if (params[1].type==qfmpDouble) {\
        pa=params[1].num;\
    } else {\
        p->qfmpError(QObject::tr("%1(...) second argument has to be a number").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    if (params[0].type==qfmpDouble) {\
        r.num=CFUNC(params[0].num, pa);\
    } else if(params[0].type==qfmpDoubleVector) {\
        r.type=qfmpDoubleVector;\
        r.numVec.resize(params[0].numVec.size());\
        for (int i=0; i<params[0].numVec.size(); i++) {\
            r.numVec[i]=CFUNC(params[0].numVec[i], pa);\
        }\
    } else {\
        p->qfmpError(QObject::tr("%1(...) first argument has to be a number or vector of numbers").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
    }\
    return; \
}

/*! \brief This macro allows to easily define functions for QFMathParser from a C-function that
           is (numeric, numeric, numeric)->numeric
             The result is (numeric, numeric, numeric)->numeric or (num_vector, numeric, numeric)->num_vector.
             The c-function takes the vector-argument as LAST argument!

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
#define QFMATHPARSER_DEFINE_3PARAMLVEC_NUMERIC_FUNC(FName, NAME_IN_PARSER, CFUNC) \
static inline void FName(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){\
    r.isValid=true;\
    r.type=qfmpDouble;\
    if (n!=3) {\
        p->qfmpError(QObject::tr("%1(...) needs exactly 3 argument").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    double pa=0; \
    double pb=0; \
    if (params[1].type==qfmpDouble) {\
        pa=params[1].num;\
    } else {\
        p->qfmpError(QObject::tr("%1(...) second argument has to be a number").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    if (params[2].type==qfmpDouble) {\
        pb=params[2].num;\
    } else {\
        p->qfmpError(QObject::tr("%1(...) third argument has to be a number").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    if (params[0].type==qfmpDouble) {\
        r.num=CFUNC(params[0].num, pa, pb);\
    } else if(params[0].type==qfmpDoubleVector) {\
        r.type=qfmpDoubleVector;\
        r.numVec.resize(params[0].numVec.size());\
        for (int i=0; i<params[0].numVec.size(); i++) {\
            r.numVec[i]=CFUNC(params[0].numVec[i], pa, pb);\
        }\
    } else {\
        p->qfmpError(QObject::tr("%1(...) first argument has to be a number or vector of numbers").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
    }\
    return; \
}




/*! \brief This macro allows to easily define functions for QFMathParser from a C-function that
           is (numeric, numeric, numeric, numeric)->numeric
             The result is (numeric, numeric, numeric, numeric)->numeric or (num_vector, numeric, numeric, numeric)->num_vector.
             The c-function takes the vector-argument as LAST argument!

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
#define QFMATHPARSER_DEFINE_4PARAMLVEC_NUMERIC_FUNC(FName, NAME_IN_PARSER, CFUNC) \
static inline void FName(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){\
    r.isValid=true;\
    r.type=qfmpDouble;\
    if (n!=4) {\
        p->qfmpError(QObject::tr("%1(...) needs exactly 4 argument").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    double pa=0; \
    double pb=0; \
    double pc=0; \
    if (params[1].type==qfmpDouble) {\
        pa=params[1].num;\
    } else {\
        p->qfmpError(QObject::tr("%1(...) second argument has to be a number").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    if (params[2].type==qfmpDouble) {\
        pb=params[2].num;\
    } else {\
        p->qfmpError(QObject::tr("%1(...) third argument has to be a number").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    if (params[3].type==qfmpDouble) {\
        pc=params[3].num;\
    } else {\
        p->qfmpError(QObject::tr("%1(...) fourth argument has to be a number").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    if (params[0].type==qfmpDouble) {\
        r.num=CFUNC(params[0].num, pa, pb, pc);\
    } else if(params[0].type==qfmpDoubleVector) {\
        r.type=qfmpDoubleVector;\
        r.numVec.resize(params[0].numVec.size());\
        for (int i=0; i<params[0].numVec.size(); i++) {\
            r.numVec[i]=CFUNC(params[0].numVec[i], pa, pb, pc);\
        }\
    } else {\
        p->qfmpError(QObject::tr("%1(...) first argument has to be a number or vector of numbers").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
    }\
    return; \
}



/*! \brief This macro allows to easily define functions for QFMathParser from a C-function that
           is (numeric,numeric)->numeric, e.g. fmod(x, y).  The result is (numeric, numeric)->numeric or (num_vector, num_vector)->num_vector.

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
static inline void FName(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){\
    r.isValid=true;\
    r.type=qfmpDouble;\
    if (n!=2) {\
        p->qfmpError(QObject::tr("%1(...) needs exactly 2 argument").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    if (params[0].type==qfmpDouble && params[1].type==qfmpDouble) {\
    r.num=CFUNC(params[0].num, params[1].num);\
    } else if(params[0].type==qfmpDoubleVector && params[1].type==qfmpDoubleVector) {\
        if (params[0].numVec.size()!=params[1].numVec.size()) {\
            p->qfmpError(QObject::tr("%1(x,y) both arguments have to have same length").arg(#NAME_IN_PARSER));\
            r.setInvalid();\
            return; \
        }\
        r.type=qfmpDoubleVector;\
        r.numVec.resize(params[0].numVec.size());\
        for (int i=0; i<params[0].numVec.size(); i++) {\
            r.numVec[i]=CFUNC(params[0].numVec[i], params[1].numVec[i]);\
        }\
    } else {\
        p->qfmpError(QObject::tr("%1(...) first argument has to be a number or vector of numbers").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
    }\
    return; \
}

/** \brief same as QFMATHPARSER_DEFINE_2PARAM1VEC_NUMERIC_FUNC(FName, NAME_IN_PARSER, CFUNC), but NAME_IN_PARSER==CFUNC */
#define QFMATHPARSER_DEFINE_2PARAM1VEC_NUMERIC_FUNC_SIMPLE(FName, CFUNC) QFMATHPARSER_DEFINE_2PARAM1VEC_NUMERIC_FUNC(FName, CFUNC, CFUNC)

/** \brief same as QFMATHPARSER_DEFINE_2PARAM2VEC_NUMERIC_FUNC(FName, NAME_IN_PARSER, CFUNC), but NAME_IN_PARSER==CFUNC */
#define QFMATHPARSER_DEFINE_2PARAM2VEC_NUMERIC_FUNC_SIMPLE(FName, CFUNC) QFMATHPARSER_DEFINE_2PARAM2VEC_NUMERIC_FUNC(FName, CFUNC, CFUNC)

/** \brief same as QFMATHPARSER_DEFINE_2PARAM12VEC_NUMERIC_FUNC(FName, NAME_IN_PARSER, CFUNC), but NAME_IN_PARSER==CFUNC */
#define QFMATHPARSER_DEFINE_2PARAM12VEC_NUMERIC_FUNC_SIMPLE(FName, CFUNC) QFMATHPARSER_DEFINE_2PARAM12VEC_NUMERIC_FUNC(FName, CFUNC, CFUNC)


/*@}*/

#endif // QFMATHPARSERTOOLS_H
