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

/** \file qfmathparser.cpp
 *  \ingroup qfmp
 */

#define COMPILING_THIS_QFMATHPARSER
#include "qfmathparser.h" // class's header file
#include <iostream>
#include <float.h>
#include <QDebug>
#include <stdint.h>
#include <limits>
#include <algorithm>
#include <QLinkedList>
#include "qfmathparserdefaultlib.h"





void QFMathParser::addStandardVariables(){
    if (!QFMathParser_DefaultLib::hasDefaultVariables(this)) {
        QFMathParser_DefaultLib::addDefaultVariables(this);
    }


    for (int i=0; i<externalGlobalVariables.size(); i++) {
        addVariable(externalGlobalVariables[i].first, externalGlobalVariables[i].second);
    }

}

void QFMathParser::addStandardFunctions(){
    if (!QFMathParser_DefaultLib::hasDefaultFunctions(this)) {
        QFMathParser_DefaultLib::addDefaultFunctions(this);
    }

    addVariableString("version", QFMATHPARSER_VERSION);

    for (int i=0; i<externalGlobalFunctions.size(); i++) {
        addFunction(externalGlobalFunctions[i].first, externalGlobalFunctions[i].second);
    }
    for (int i=0; i<externalGlobalFunctionsRR.size(); i++) {
        addFunction(externalGlobalFunctionsRR[i].first, externalGlobalFunctionsRR[i].second);
    }
    for (int i=0; i<externalGlobalFunctionsFN.size(); i++) {
        addFunction(externalGlobalFunctionsFN[i].first, externalGlobalFunctionsFN[i].second);
    }
}
























QString QFMathParser::resultTypeToString(qfmpResultType type)
{
    return qfmpResultTypeToString(type);
}

QString QFMathParser::resultToString(const qfmpResult& r)
{
    return r.toString();
}

QString QFMathParser::resultToTypeString(const qfmpResult& r)
{
    return r.toTypeString();
}

qfmpResult QFMathParser::getInvalidResult()
{
    qfmpResult res;
    res.isValid=false;
    return res;
}

void QFMathParser::qfmpError(QString st)
{
    //qDebug()<<"##ERROR: "<<st;
    lastError.append(st);
    errors++;
}

QString QFMathParser::tokentostring(QFMathParser::qfmpTokenType token) {
    switch(token) {
        case END: return "END";
        case PRINT: return "PRINT (;)";
        case COMMA: return "COMMA (,)";
	    case STRING_DELIM: return "STRING_DELIM (' or \")";
	    case NAME: return "NAME";
	    case NUMBER: return "NUMBER";
	    case PLUS: return "PLUS (+)";
	    case MINUS: return "MINUS (-)";
	    case MUL: return "MUL (*)";
	    case DIV: return "DIV (/)";
	    case MODULO: return "MODULO (%)";
	    case ASSIGN: return "ASSIGN (=)";
        case LPARENTHESE: return "LPARENTHESE '('";
        case RPARENTHESE: return "RPARENTHESE ')'";
        case LBRACKET: return "LBRACKET '['";
        case COLON: return "COLON ':'";
        case RBRACKET: return "RBRACKET ']'";
        case LBRACE: return "LBRACE '{'";
        case RBRACE: return "RBRACE '}'";
        case POWER: return "POWER (^)";
        case DOT: return "DOT (.)";
        case TILDE: return "TILDE (~)";
        case FACTORIAL_LOGIC_NOT: return "FACTORIAL_LOGIC_NOT (!)";
	    case LOGIC_NOT: return "LOGIC_NOT (!/not)";
	    case LOGIC_AND: return "LOGIC_AND (&&/and)";
	    case LOGIC_OR: return "LOGIC_OR (||/or)";
        case BINARY_AND: return "BINARY_AND (&)";
        case BINARY_OR: return "BINARY_OR (|)";
        case LOGIC_XOR: return "LOGIC_XOR (xor)";
	    case LOGIC_NOR: return "LOGIC_NOR (nor)";
	    case LOGIC_NAND: return "LOGIC_NAND (nand)";
	    case LOGIC_TRUE: return "LOGIC_TRUE (true)";
	    case LOGIC_FALSE: return "LOGIC_FALSE (false)";
	    case COMP_EQUALT: return "COMP_EQUALT (==)";
	    case COMP_UNEQUAL: return "COMP_UNEQUAL (!=)";
	    case COMP_GREATER: return "COMP_GREATER (>)";
	    case COMP_SMALLER: return "COMP_SMALLER (<)";
	    case COMP_GEQUAL: return "COMP_GEQUAL (>=)";
	    case COMP_SEQUAL: return "COMP_SEQUAL (<=)";
    }
    return "unknown";
}

QString QFMathParser::currenttokentostring() {
    return tokentostring(CurrentToken);
}



/******************************************************************************************
 * jkMathParser
 ******************************************************************************************/

// class constructor
QFMathParser::QFMathParser() {
    //qDebug()<<"constructing QFMathParser";
    environment.setParent(this);
    //qDebug()<<"constructing QFMathParser: adding functions";
    addStandardFunctions();
    //qDebug()<<"constructing QFMathParser: adding variables";
    addStandardVariables();
    //qDebug()<<"constructing QFMathParser: resetting errors";
    resetErrors();
    //qDebug()<<"done";
}


// class destructor
QFMathParser::~QFMathParser()
{
	clearFunctions();
    clearVariables();
}

void QFMathParser::setGeneralData(const QString &id, const QVariant &data)
{
    m_generalData[id]=data;
}

QVariant QFMathParser::getGeneraldata(const QString &id, const QVariant &defaultData)
{
    return m_generalData.value(id, defaultData);
}

bool QFMathParser::hasErrorOccured() const
{
    return errors>0;
}







QList<QPair<QString, QFMathParser::qfmpEvaluateFunc> > QFMathParser::externalGlobalFunctions= QList<QPair<QString, QFMathParser::qfmpEvaluateFunc> >() ;
QList<QPair<QString, QFMathParser::qfmpEvaluateFuncRefReturn> > QFMathParser::externalGlobalFunctionsRR= QList<QPair<QString, QFMathParser::qfmpEvaluateFuncRefReturn> >() ;
QList<QPair<QString, QFMathParser::qfmpEvaluateFromNodesFuncRefReturn> > QFMathParser::externalGlobalFunctionsFN= QList<QPair<QString, QFMathParser::qfmpEvaluateFromNodesFuncRefReturn> >() ;
QList<QPair<QString, qfmpResult> > QFMathParser::externalGlobalVariables= QList<QPair<QString, qfmpResult> >();


void QFMathParser::addGlobalFunction(const QString &name, QFMathParser::qfmpEvaluateFunc function)
{
    externalGlobalFunctions.append(qMakePair(name, function));
}

void QFMathParser::addGlobalFunction(const QString &name, QFMathParser::qfmpEvaluateFuncRefReturn function)
{
    externalGlobalFunctionsRR.append(qMakePair(name, function));
}

void QFMathParser::addGlobalFunction(const QString &name, QFMathParser::qfmpEvaluateFromNodesFuncRefReturn function)
{
    externalGlobalFunctionsFN.append(qMakePair(name, function));

}

void QFMathParser::addGlobalVariable(const QString &name, double value)
{
    externalGlobalVariables.append(qMakePair(name, qfmpResult(value)));
}

void QFMathParser::addGlobalVariable(const QString &name, const QVector<double> &value)
{
    externalGlobalVariables.append(qMakePair(name, qfmpResult(value)));
}

void QFMathParser::addGlobalVariable(const QString &name, const QVector<bool> &value)
{
    externalGlobalVariables.append(qMakePair(name, qfmpResult(value)));
}

void QFMathParser::addGlobalVariable(const QString &name, const QStringList &value)
{
    externalGlobalVariables.append(qMakePair(name, qfmpResult(value)));
}

void QFMathParser::addGlobalVariable(const QString &name, const QString &value)
{
    externalGlobalVariables.append(qMakePair(name, qfmpResult(value)));
}

void QFMathParser::addGlobalVariable(const QString &name, bool value)
{
    externalGlobalVariables.append(qMakePair(name, qfmpResult(value)));
}










QChar QFMathParser::peekStream(QTextStream *stream)
{
    if (!stream) return QChar(0);
    qint64 p=stream->pos();
    QChar c=QChar(0);
    if (!stream->atEnd()) {
        stream->operator>>(c);
        stream->seek(p);
    }
    //qDebug()<<"peek "<<c;
    return c;
}

void QFMathParser::putbackStream(QTextStream *stream, QChar ch)
{
    if (ch!=0 && stream) stream->seek(stream->pos()-1);
}

bool QFMathParser::getFromStream(QTextStream *stream, QChar &c)
{
    if (!stream) return false;
    c=QChar(0);
    if (stream->atEnd()) {
        //qDebug()<<"read "<<c<<": false";
        return false;
    }
    stream->operator>>(c);
    //qDebug()<<"read "<<c<<": true";
    return true;
}






void QFMathParser::addFunction(const QString &name, QFMathParser::qfmpEvaluateFunc function)
{
    qfmpFunctionDescriptor f;
    f.function=function;
    f.name=name;
    f.type=QFMathParser::functionC;
    environment.setFunction(name, f);
}

void QFMathParser::addFunction(const QString &name, QFMathParser::qfmpEvaluateFuncRefReturn function)
{
    qfmpFunctionDescriptor f;
    f.functionRR=function;
    f.name=name;
    f.type=QFMathParser::functionCRefReturn;
    environment.setFunction(name, f);
}

void QFMathParser::addFunction(const QString &name, QFMathParser::qfmpEvaluateFunc function, QFMathParser::qfmpEvaluateFuncSimple0Param f0, QFMathParser::qfmpEvaluateFuncSimple1Param f1, QFMathParser::qfmpEvaluateFuncSimple2Param f2, QFMathParser::qfmpEvaluateFuncSimple3Param f3)
{
    qfmpFunctionDescriptor f;
    f.function=function;
    f.name=name;
    f.type=QFMathParser::functionC;
    if (f0) f.simpleFuncPointer[0]=(void*)f0;
    if (f1) f.simpleFuncPointer[1]=(void*)f1;
    if (f2) f.simpleFuncPointer[2]=(void*)f2;
    if (f3) f.simpleFuncPointer[3]=(void*)f3;
    environment.setFunction(name, f);
}

void QFMathParser::addFunction(const QString &name, QFMathParser::qfmpEvaluateFunc function, QFMathParser::qfmpEvaluateFuncSimple0ParamMP f0, QFMathParser::qfmpEvaluateFuncSimple1ParamMP f1, QFMathParser::qfmpEvaluateFuncSimple2ParamMP f2, QFMathParser::qfmpEvaluateFuncSimple3ParamMP f3)
{
    qfmpFunctionDescriptor f;
    f.function=function;
    f.name=name;
    f.type=QFMathParser::functionC;
    if (f0) f.simpleFuncPointer[100]=(void*)f0;
    if (f1) f.simpleFuncPointer[101]=(void*)f1;
    if (f2) f.simpleFuncPointer[102]=(void*)f2;
    if (f3) f.simpleFuncPointer[103]=(void*)f3;
    environment.setFunction(name, f);
}

void QFMathParser::addFunction(const QString &name, QFMathParser::qfmpEvaluateFuncRefReturn function, QFMathParser::qfmpEvaluateFuncSimple0Param f0, QFMathParser::qfmpEvaluateFuncSimple1Param f1, QFMathParser::qfmpEvaluateFuncSimple2Param f2, QFMathParser::qfmpEvaluateFuncSimple3Param f3)
{
    qfmpFunctionDescriptor f;
    f.functionRR=function;
    f.name=name;
    f.type=QFMathParser::functionCRefReturn;
    if (f0) f.simpleFuncPointer[0]=(void*)f0;
    if (f1) f.simpleFuncPointer[1]=(void*)f1;
    if (f2) f.simpleFuncPointer[2]=(void*)f2;
    if (f3) f.simpleFuncPointer[3]=(void*)f3;
    environment.setFunction(name, f);
}

void QFMathParser::addFunction(const QString &name, QFMathParser::qfmpEvaluateFuncRefReturn function, QFMathParser::qfmpEvaluateFuncSimple0ParamMP f0, QFMathParser::qfmpEvaluateFuncSimple1ParamMP f1, QFMathParser::qfmpEvaluateFuncSimple2ParamMP f2, QFMathParser::qfmpEvaluateFuncSimple3ParamMP f3)
{
    qfmpFunctionDescriptor f;
    f.functionRR=function;
    f.name=name;
    f.type=QFMathParser::functionCRefReturn;
    if (f0) f.simpleFuncPointer[100]=(void*)f0;
    if (f1) f.simpleFuncPointer[101]=(void*)f1;
    if (f2) f.simpleFuncPointer[102]=(void*)f2;
    if (f3) f.simpleFuncPointer[103]=(void*)f3;
    environment.setFunction(name, f);
}

void QFMathParser::addFunction(const QString &name, QFMathParser::qfmpEvaluateFromNodesFuncRefReturn function)
{
    qfmpFunctionDescriptor f;
    f.functionFN=function;
    f.name=name;
    f.type=QFMathParser::functionFromNode;
    environment.setFunction(name, f);

}


QFMathParser::qfmpTokenType QFMathParser::getToken(){
    QChar ch=0;
    while(getFromStream(program, ch) && ch.isSpace()) {
		;
	}


    switch (ch.toLatin1()) {
		case 0:
		case -1:
			return CurrentToken=END;
        case ';':
            return CurrentToken=PRINT;
        case '.':
            return CurrentToken=DOT;
        case ':':
            return CurrentToken=COLON;
        case '*':
			return CurrentToken=MUL;
		case '/':
			return CurrentToken=DIV;
		case '%':
			return CurrentToken=MODULO;
		case '+':
			return CurrentToken=PLUS;
		case '-':
			return CurrentToken=MINUS;
        case '(':
            return CurrentToken=LPARENTHESE;
        case ')':
            return CurrentToken=RPARENTHESE;
        case '[':
            return CurrentToken=LBRACKET;
        case ']':
            return CurrentToken=RBRACKET;
        case '{':
            return CurrentToken=LBRACE;
        case '}':
            return CurrentToken=RBRACE;
        case ',':
            return CurrentToken=COMMA;
		case '"':
			return CurrentToken=STRING_DELIM;
		case '^':
			return CurrentToken=POWER;
        case '~':
            return CurrentToken=TILDE;
        case '!':{
            QChar ch1=0;
            if (program) getFromStream(program, ch1);
			if (ch1=='=') return CurrentToken=COMP_UNEQUAL;
			// else
            putbackStream(program, ch1);
			return CurrentToken=FACTORIAL_LOGIC_NOT;
		}
		case '&':{
            QChar ch1=0;
            if (program) getFromStream(program, ch1);
			if (ch1=='&') return CurrentToken=LOGIC_AND;
			// else
            putbackStream(program, ch1);
            return CurrentToken=BINARY_AND;
            //qfmpError(QObject::tr("undefined operator '&'; Did you mean LOGICAL_AND ('&&' / 'and')?"));
            break;
		}
		case '|':{
            QChar ch1=0;
            if (program) getFromStream(program, ch1);
			if (ch1=='|') return CurrentToken=LOGIC_OR;
			// else
            putbackStream(program, ch1);
            return CurrentToken=BINARY_OR;
            //qfmpError(QObject::tr("undefined operator '|'; Did you mean LOGICAL_OR ('||' / 'or')?"));
            break;
		}
		case '=':{
            QChar ch1=0;
            if (program) getFromStream(program, ch1);
			if (ch1=='=') return CurrentToken=COMP_EQUALT;
			// else
            putbackStream(program, ch1);
			return CurrentToken=ASSIGN;
		}
		case '>':{
            QChar ch1=0;
            if (program) getFromStream(program, ch1);
			if (ch1=='=') return CurrentToken=COMP_GEQUAL;
			// else
            putbackStream(program, ch1);
			return CurrentToken=COMP_GREATER;
		}
		case '<':{
            QChar ch1=0;
            if (program) getFromStream(program, ch1);
			if (ch1=='=') return CurrentToken=COMP_SEQUAL;
			// else
            putbackStream(program, ch1);
			return CurrentToken=COMP_SMALLER;
		}
		case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9': {
            putbackStream(program, ch);
            //(*program) >> NumberValue;
            NumberValue=readNumber();
			return CurrentToken=NUMBER;
		}
		default:
            if (ch.isLetter() || (ch=='_')) { // try to recognize NAME, LOGIC_TRUE, LOGIC_FALSE, DIFF_LBRACKET
				StringValue=ch;
                while (getFromStream(program, ch) && (ch.isDigit()||ch.isLetter() || (ch=='_'))) {
                    if ((ch.isDigit()||ch.isLetter()) || (ch=='_')) {
                        StringValue.push_back(ch);
                    }
				}

                putbackStream(program, ch); // now put the last thing read back int the stream, as it
									  // could belong to the next token

				if (StringValue=="true") return CurrentToken=LOGIC_TRUE;
				if (StringValue=="false") return CurrentToken=LOGIC_FALSE;
                if (StringValue.toLower()=="nan") {
                    NumberValue=std::numeric_limits<double>::quiet_NaN();
                    return CurrentToken=NUMBER;
                }
                if (StringValue.toLower()=="inf") {
                    NumberValue=std::numeric_limits<double>::infinity();
                    return CurrentToken=NUMBER;
                }
                if (StringValue.toLower()=="ninf") {
                    NumberValue=-std::numeric_limits<double>::infinity();
                    return CurrentToken=NUMBER;
                }
                if (StringValue=="and") return CurrentToken=LOGIC_AND;
                if (StringValue=="or") return CurrentToken=LOGIC_OR;
                if (StringValue=="band") return CurrentToken=BINARY_AND;
                if (StringValue=="bor") return CurrentToken=BINARY_OR;
                if (StringValue=="xor") return CurrentToken=LOGIC_XOR;
				if (StringValue=="not") return CurrentToken=LOGIC_NOT;
				if (StringValue=="nor") return CurrentToken=LOGIC_NOR;
				if (StringValue=="nand") return CurrentToken=LOGIC_NAND;

				return CurrentToken=NAME;
			}
			// the parser has found an unknown token. an exception will be thrown
			//std::cout<<StringValue<<",   "<<ch<<std::endl;
            qfmpError(QString("get_next_token: unknown token currentCharacter='%1', currentString='%2'").arg(qchartostr(ch)).arg(StringValue));
            break;
	}
    return END;
}


QFMathParser::qfmpNode* QFMathParser::parse(QString prog){
    progStr=prog;
    program=new QTextStream(&progStr);
    QFMathParser::qfmpNode* res=NULL;
    QFMathParser::qfmpNodeList* resList=new QFMathParser::qfmpNodeList(this);
	while(true) {
		getToken();
        if (CurrentToken == END) {
            break;
        }
        res= logicalExpression(false);
        resList->add(res);
        //qDebug()<<"parse add nodelist item "<<resList->getCount()<<"  = "<<res->evaluate().toTypeString();
	}
	delete program;
    //qDebug()<<"parsed nodelist with "<<resList->getCount()<<" items";
    if (resList->getCount()==1) {
        //delete resList;
        //qDebug()<<"returning single item";
        res=resList->popLast(false);
        delete resList;
        return res;
    } else {
        //qDebug()<<"returning list";
        return resList;
    }
}

qfmpResult QFMathParser::evaluate(QString prog) {
    QFMathParser::qfmpNode* res=parse(prog);
    qfmpResult r;
    res->evaluate(r);
    delete res;
    return r;
}

QFMathParser::qfmpNode* QFMathParser::logicalExpression(bool get){
    QFMathParser::qfmpNode* left=logicalTerm(get);

	for(;;) // forever, do until you find anything else than an expressions
		switch(CurrentToken) {
			case LOGIC_OR:
                left= (QFMathParser::qfmpNode*)new qfmpBinaryBoolNode(qfmpLOPor, left, logicalTerm(true), this, NULL);
				break;
            case LOGIC_XOR:
                left= (QFMathParser::qfmpNode*)new qfmpBinaryBoolNode(qfmpLOPxor, left, logicalTerm(true), this, NULL);
				break;
			case LOGIC_NOR:
                left= (QFMathParser::qfmpNode*)new qfmpBinaryBoolNode(qfmpLOPnor, left, logicalTerm(true), this, NULL);
				break;
			default:
				return left;
		}
}

QFMathParser::qfmpNode* QFMathParser::logicalTerm(bool get){
    QFMathParser::qfmpNode* left=compExpression(get);

	for(;;) // forever, do until you find anything else than an expressions
		switch(CurrentToken) {
            case LOGIC_AND:
                left= (QFMathParser::qfmpNode*)new qfmpBinaryBoolNode(qfmpLOPand, left, compExpression(true), this, NULL);
                break;
            case LOGIC_NAND:
                left= (QFMathParser::qfmpNode*)new qfmpBinaryBoolNode(qfmpLOPnand, left, compExpression(true), this, NULL);
				break;
			default:
				return left;
		}
}

QFMathParser::qfmpNode* QFMathParser::compExpression(bool get){
    QFMathParser::qfmpNode* left=mathExpression(get);

	for(;;) // forever, do until you find anything else than an expressions
		switch(CurrentToken) {
			case COMP_EQUALT:
                left= (QFMathParser::qfmpNode*)new qfmpCompareNode(qfmpCOMPequal, left, mathExpression(true), this, NULL);
				break;
			case COMP_UNEQUAL:
                left= (QFMathParser::qfmpNode*)new qfmpCompareNode(qfmpCOMPnequal, left, mathExpression(true), this, NULL);
				break;
			case COMP_GREATER:
                left= (QFMathParser::qfmpNode*)new qfmpCompareNode(qfmpCOMPgreater, left, mathExpression(true), this, NULL);
				break;
			case COMP_SMALLER:
                left= (QFMathParser::qfmpNode*)new qfmpCompareNode(qfmpCOMPlesser, left, mathExpression(true), this, NULL);
				break;
			case COMP_GEQUAL:
                left= (QFMathParser::qfmpNode*)new qfmpCompareNode(qfmpCOMPgreaterequal, left, mathExpression(true), this, NULL);
				break;
			case COMP_SEQUAL:
                left= (QFMathParser::qfmpNode*)new qfmpCompareNode(qfmpCOMPlesserequal, left, mathExpression(true), this, NULL);
				break;
			default:
				return left;
		}
}


QFMathParser::qfmpNode* QFMathParser::mathExpression(bool get){
    QFMathParser::qfmpNode* left=mathTerm(get);

	for(;;) // forever, do until you find anything else than an expressions
		switch(CurrentToken) {
			case PLUS:
                left= (QFMathParser::qfmpNode*)new qfmpBinaryArithmeticNode('+', left, mathTerm(true), this, NULL);
				break;
            case BINARY_OR:
                left= (QFMathParser::qfmpNode*)new qfmpBinaryArithmeticNode('|', left, mathTerm(true), this, NULL);
                break;
            case MINUS:
                left= (QFMathParser::qfmpNode*)new qfmpBinaryArithmeticNode('-', left, mathTerm(true), this, NULL);
				break;
			default:
				return left;
		}
}

QFMathParser::qfmpNode* QFMathParser::mathTerm(bool get){
    QFMathParser::qfmpNode* left=primaryLOp(get);

    for(;;) {
        // forever, do until you find anything else than a term
        switch(CurrentToken) {
        case MUL:
            left= (QFMathParser::qfmpNode*)new qfmpBinaryArithmeticNode('*', left, primaryLOp(true), this, NULL);
            break;
        case DIV:
            left= (QFMathParser::qfmpNode*)new qfmpBinaryArithmeticNode('/', left, primaryLOp(true), this, NULL);
            break;
        case MODULO:
            left= (QFMathParser::qfmpNode*)new qfmpBinaryArithmeticNode('%', left, primaryLOp(true), this, NULL);
            break;
        case BINARY_AND:
            left= (QFMathParser::qfmpNode*)new qfmpBinaryArithmeticNode('&', left, primaryLOp(true), this, NULL);
            break;
        default:
            return left;
        }
    }
}



QFMathParser::qfmpNode *QFMathParser::primaryLOp(bool get)
{
    QFMathParser::qfmpNode* res=NULL;

    if (get) getToken();

        switch(CurrentToken) {
            case MINUS:  // found primary:  - primary
                res= (QFMathParser::qfmpNode*)new qfmpUnaryNode('-', vectorPrimary(true), this, NULL);
                break;

            case FACTORIAL_LOGIC_NOT:// found primary:  !/not primary
            case LOGIC_NOT:
                res= (QFMathParser::qfmpNode*)new qfmpUnaryNode('!', vectorPrimary(true), this, NULL);
                break;

            case TILDE:// found primary:  ~ primary
                res= (QFMathParser::qfmpNode*)new qfmpUnaryNode('~', vectorPrimary(true), this, NULL);
                break;

            default:
                res=vectorPrimary(false);
                break;

        }

    if (!res) res=new qfmpInvalidNode(this, NULL);

    //qDebug()<<"primaryLOp";

    return res;

}



QFMathParser::qfmpNode* QFMathParser::vectorPrimary(bool get){

    QFMathParser::qfmpNode* res=NULL;
    //if (get) getToken();

      // vector construct start:delta:end and start:end


    res=primaryOp(get);

    if (CurrentToken==COLON) {
        QFMathParser::qfmpNode* v1=res;
        QFMathParser::qfmpNode* v2=primaryOp(true);
        if (CurrentToken==COLON) {
            //qDebug()<<"vectorPrimary ::";
            res=(QFMathParser::qfmpNode*)new qfmpVectorConstructionNode(v1, primaryOp(true), v2, this, NULL);
            //qDebug()<<"constructed 'a:d:e' current="<<tokentostring(CurrentToken);
        } else {
            //qDebug()<<"vectorPrimary :";
            res=(QFMathParser::qfmpNode*)new qfmpVectorConstructionNode(v1, v2, NULL, this, NULL);
            //qDebug()<<"constructed 'a:e' current="<<tokentostring(CurrentToken);
        }
    }

    return res;
}


QFMathParser::qfmpNode *QFMathParser::primaryOp(bool get)
{
    QFMathParser::qfmpNode* res=primary(get);

    for(;;) {

        switch(CurrentToken) {
            case POWER:  // found primary:  ^ primary
                //qDebug()<<"primaryOp ^";
                res=(QFMathParser::qfmpNode*)new qfmpBinaryArithmeticNode('^', res, primary(true), this, NULL);
                break;

            case LBRACKET: { // found '['
                    //qDebug()<<"primaryOp [";
                    QFMathParser::qfmpNode* parameter=logicalExpression(true);
                    if (CurrentToken == RBRACKET) {
                        res=new qfmpVectorAccessNode(res, parameter, this, NULL);
                        getToken();
                    } else {
                        if (res) delete res;
                        qfmpError(QString("parsing primary_op: ']' expected, but '%1' found").arg(currenttokentostring()));
                        return new qfmpInvalidNode(this, NULL);
                    }

                    //getToken();
                } break;
            case DOT: { // found '.'
                    //qDebug()<<"primaryOp [";
                    if (getToken()==NAME) {
                        res=new qfmpStructAccessNode(res, StringValue, this, NULL);
                        getToken();
                    } else {
                        if (res) delete res;
                        qfmpError(QString("parsing primary_op: 'NAME' expected in EXPR.NAME (struct access) construct, but '%1' found").arg(currenttokentostring()));
                        return new qfmpInvalidNode(this, NULL);
                    }

                    //getToken();
                } break;

            default:
                if (!res) res=new qfmpInvalidNode(this, NULL);
                //qDebug()<<"primaryOp";
                return res;

        }

    }

    if (!res) res=new qfmpInvalidNode(this, NULL);

    return res;

}

QFMathParser::qfmpNode* QFMathParser::primary(bool get){
    QFMathParser::qfmpNode* res=NULL;

	if (get) getToken();

	switch(CurrentToken) {
		case NUMBER: {
            qfmpResult val;
            val.type=qfmpDouble;
			val.num=NumberValue;
			getToken();
            res= (QFMathParser::qfmpNode*)new qfmpConstantNode(val, this, NULL);
			break;
		}

		case NAME: {
                //jkMathParser::qfmpNode* def=NULL;
                QString varname=StringValue;
                getToken();
                if (CurrentToken == ASSIGN) { // assign a variable name
                    res=new qfmpVariableAssignNode(varname, logicalExpression(true), this, NULL);
                } else if (CurrentToken == LBRACKET) { // vector element access found
                    QFMathParser::qfmpNode* parameter=logicalExpression(true);
                    if ( CurrentToken != RBRACKET ) {
                        qfmpError(QObject::tr("parsing primary: ']' expected after '[...', but '%1' found").arg(currenttokentostring()));
                        return NULL;
                    }
                    getToken();
                    if (CurrentToken == ASSIGN) { // assign a variable name
                        res=new qfmpVectorElementAssignNode(varname, parameter, logicalExpression(true), this, NULL);
                    } else {
                        res=new qfmpVariableVectorAccessNode(varname, parameter, this, NULL);
                    }
                } else if (CurrentToken == LPARENTHESE) { // function found
                    //QFMathParser::qfmpNode** params=(QFMathParser::qfmpNode**)qfMalloc(255*sizeof(QFMathParser::qfmpNode*));
                    QVector<QFMathParser::qfmpNode*> params;


                    getToken();
                    while ((CurrentToken != RPARENTHESE)&&(CurrentToken!=END)) {
                        QFMathParser::qfmpNode* parameter=logicalExpression(params.size()>0);
                        params.append(parameter);
                        if ((CurrentToken!=RPARENTHESE)&&(CurrentToken!=COMMA)&&(CurrentToken!=END)) {
                            qfmpError(QObject::tr("parsing primary: ')' or ',' expected after '(...', but '%1' found").arg(currenttokentostring()));
                            return NULL;
                        }

                    }

                    if ( CurrentToken != RPARENTHESE ) {
                        qfmpError(QObject::tr("parsing primary: ')' expected after '(...', but '%1' found").arg(currenttokentostring()));
                        return NULL;
                    }
                    getToken();
                    QString lvarname=varname.toLower();
#ifdef QFMATHPARSER_DEBUGFUNCTIONNAMES
                    if (lvarname=="if" || lvarname=="dbgif") {
#else
                    if (lvarname=="if") {
#endif
                        if (params.size()!=3) {
                            qfmpError(QObject::tr("parsing primary: 'if(decision, trueValue, falseValue)' expects 3 arguments, but '%1' found").arg(params.size()));
                            return NULL;
                        }
                        qfmpCasesNode* cn=new qfmpCasesNode(this, NULL);
                        cn->addCase(params[0], params[1]);
                        cn->setElse(params[2]);
                        res=cn;
#ifdef QFMATHPARSER_DEBUGFUNCTIONNAMES
                    } else if (lvarname=="cases" || lvarname=="dbgcases") {
#else
                    } else if (lvarname=="cases") {
#endif
                        if (params.size()<3 || (params.size()%2==0)) {
                            qfmpError(QObject::tr("parsing primary: 'cases(decision1, trueValue1, decision2, trueValue2, ..., elseValue)' expects 3, 5, 7 ... arguments, but '%1' found").arg(params.size()));
                            return NULL;
                        }
                        qfmpCasesNode* cn=new qfmpCasesNode(this, NULL);
                        for (int i=0; i<params.size()-1; i=i+2) {
                            cn->addCase(params[i], params[i+1]);
                        }
                        cn->setElse(params.last());
                        res=cn;
                    } else if (lvarname=="cumsum" || lvarname=="cumprod" || lvarname=="sum" || lvarname=="prod" || lvarname=="for" || lvarname=="defaultfor" || lvarname=="savefor" || lvarname=="filterfor" || lvarname=="savefilterfor") {
                        if (params.size()==1) {
                            if (lvarname!="for"&&lvarname!="savefor"&&lvarname!="defaultfor") {
                                res=new qfmpFunctionNode(varname, params, this, NULL);
                            } else {
                                qfmpError(QObject::tr("parsing primary: '%2(NAME, start[, delta], end, expression)' expects 3-5 arguments, but '%1' found").arg(params.size()).arg(lvarname));
                                return NULL;
                            }
                        } else if (params.size()==3){
                            QString iName="";
                            qfmpVariableNode* vn=dynamic_cast<qfmpVariableNode*>(params[0]);
                            if (lvarname!="defaultfor"){
                                if (vn) {
                                    iName=vn->getName();
                                } else {
                                    qfmpError(QObject::tr("parsing primary: '%1(NAME, ..., expression)' expects a variable name as first argument").arg(lvarname));
                                    return NULL;
                                }
                                res=new qfmpVectorOperationNode(lvarname, iName, params[1], params[2], this, NULL);
                            } else {
                                qfmpError(QObject::tr("parsing primary: '%1(NAME, ..., expression, defaultValue)' expects at least 4 arguments").arg(lvarname));
                                return NULL;
                            }
                        } else if (params.size()==4){
                            QString iName="";
                            qfmpVariableNode* vn=dynamic_cast<qfmpVariableNode*>(params[0]);
                            qfmpNode* defVal=NULL;
                            qfmpNode* start=params[1];
                            qfmpNode* delta=params[2];
                            qfmpNode* end=params[3];
                            if (vn) {
                                iName=vn->getName();
                            } else {
                                qfmpError(QObject::tr("parsing primary: '%1(NAME, ..., expression)' expects a variable name as first argument").arg(lvarname));
                                return NULL;
                            }
                            if (lvarname=="defaultfor"){
                                //start=params[1];
                                //end=params[2];
                                //defVal=params[3];
                                //delta=NULL;
                                res=new qfmpVectorOperationNode(lvarname, iName, params[1], params[2], this, NULL, defVal);
                            } else {
                                res=new qfmpVectorOperationNode(lvarname, iName, params[1], params[2], NULL, params[3], this, NULL, defVal);
                            }


                        } else if (params.size()==5){
                            QString iName="";
                            qfmpVariableNode* vn=dynamic_cast<qfmpVariableNode*>(params[0]);
                            if (vn) {
                                iName=vn->getName();
                            } else {
                                qfmpError(QObject::tr("parsing primary: '%1(NAME, ..., expression)' expects a variable name as first argument").arg(lvarname));
                                return NULL;
                            }
                            if (lvarname=="defaultfor"){
                                res=new qfmpVectorOperationNode(lvarname, iName, params[1], params[2], NULL, params[3], this, NULL, params[4]);
                            } else {
                                res=new qfmpVectorOperationNode(lvarname, iName, params[1], params[3], params[2], params[4], this, NULL);
                            }
                        } else if (params.size()==6){
                            QString iName="";
                            qfmpVariableNode* vn=dynamic_cast<qfmpVariableNode*>(params[0]);
                            if (vn) {
                                iName=vn->getName();
                            } else {
                                qfmpError(QObject::tr("parsing primary: '%1(NAME, ..., expression)' expects a variable name as first argument").arg(lvarname));
                                return NULL;
                            }
                            if (lvarname=="defaultfor"){
                                res=new qfmpVectorOperationNode(lvarname, iName, params[1], params[3], params[2], params[4], this, NULL, params[5]);
                            } else {
                                qfmpError(QObject::tr("parsing primary: '%1(NAME, ..., expression)' expects at most 5 arguments").arg(lvarname));
                                return NULL;
                            }
                        } else {
                            qfmpError(QObject::tr("parsing primary: '%2(NAME, start[, delta], end, expression)' expects 4 or 5 arguments and 'sum|prod|for(NAME, value_list, expression)' expects 3 arguments, but '%1' found").arg(params.size()).arg(lvarname));
                            return NULL;
                        }
                    } else if (CurrentToken==ASSIGN) { // function assignment
                        bool allParamsAreNames=true;
                        QStringList pnames;
                        for (int i=0; i<params.size(); i++) {
                            qfmpVariableNode* n=NULL;
                            if ((n=dynamic_cast<qfmpVariableNode*>(params[i]))) {
                                pnames<<n->getName();
                            } else {
                                allParamsAreNames=false;
                            }

                        }
                        //qDebug()<<"FASSIGN: "<<varname<<allParamsAreNames<<pnames<<currenttokentostring();
                        if (allParamsAreNames) {
                            res=new qfmpFunctionAssignNode(varname, pnames, logicalExpression(true)/* primary(true)*/, this, NULL);
                        } else {
                            qfmpError(QObject::tr("parsing primary: malformed function assignmentfound, expected this form: FNAME(P1, P2, ...)=expression").arg(currenttokentostring()));
                            return NULL;
                        }
                    } else {
                        //qDebug()<<"FNODE: "<<varname<<currenttokentostring();
                        res=new qfmpFunctionNode(varname, params, this, NULL);
                    }

                } else {
                  res=(QFMathParser::qfmpNode*)new qfmpVariableNode(varname, this, NULL);
                }
                //res= def;
                break;

		}

		case STRING_DELIM: {// found primary: "data" == string constant
            qfmpResult val;
            val.type=qfmpString;
			val.str=readDelim('"');
            res= (QFMathParser::qfmpNode*)new qfmpConstantNode(val, this, NULL);
			getToken();
			break;
		}

        /*case MINUS:  // found primary:  - primary
            res= (QFMathParser::qfmpNode*)new qfmpUnaryNode('-', primary(true), this, NULL);
			break;

        case FACTORIAL_LOGIC_NOT:
        case LOGIC_NOT:
            res= (QFMathParser::qfmpNode*)new qfmpUnaryNode('!', primary(true), this, NULL);
			break;

        case TILDE:
            res= (QFMathParser::qfmpNode*)new qfmpUnaryNode('~', primary(true), this, NULL);
            break;*/

        case LPARENTHESE: { // found primary: ( expression )
            QFMathParser::qfmpNode* expr=logicalExpression(true);
            if (CurrentToken != RPARENTHESE) {
                qfmpError(QString("parsing primary: ')' expected, but '%1' found").arg(currenttokentostring()));
                return NULL;
            }
			getToken(); // swallow ")"
			res= expr;
			break;
		}
        case LOGIC_TRUE: {// found 'true'
            qfmpResult val;
            val.type=qfmpBool;
			val.boolean=true;
            res= (QFMathParser::qfmpNode*)new qfmpConstantNode(val, this, NULL);
			getToken();
			break;
        }
		case LOGIC_FALSE: {// found 'false'
            qfmpResult val;
            val.type=qfmpBool;
			val.boolean=false;
            res= (QFMathParser::qfmpNode*)new qfmpConstantNode(val, this, NULL);
			getToken();
			break;
        }




        case LBRACKET: { // found '['
                qfmpVectorList* vl=new qfmpVectorList(this, NULL);
                res=vl;
                getToken();
                while ((CurrentToken != RBRACKET)) {
                    QFMathParser::qfmpNode* parameter=logicalExpression(vl->getCount()>0);
                    vl->add(parameter);
                    if ((CurrentToken!=RBRACKET)&&(CurrentToken!=COMMA)) {
                        qfmpError(QString("parsing primary: ']' or ',' expected, but '%1' found after %2 elements").arg(currenttokentostring()).arg(vl->getCount()));
                        return new qfmpInvalidNode(this, NULL);
                    }
                    //qDebug()<<"adding to [...]: "<<vl->getCount()<<". element CurrentToke="<<currenttokentostring();
                }


                if ( CurrentToken != RBRACKET ) {
                    qfmpError(QString("parsing primary: ']' expected, but '%1' found").arg(currenttokentostring()));
                    return new qfmpInvalidNode(this, NULL);
                }
                getToken();


            } break;

		default:
            qfmpError(QObject::tr("parsing primary: primary expected, but '%1' found").arg(currenttokentostring()));
            if (res) delete res;
            return new qfmpInvalidNode(this, NULL);

	}

    /*if (CurrentToken==POWER) {
        res=(QFMathParser::qfmpNode*)new qfmpBinaryArithmeticNode('^', res, primary(true), this, NULL);
    }*/


    if (!res) res=new qfmpInvalidNode(this, NULL);

    return res;

}

double QFMathParser::readNumber()
{


  double dfactor=1;
  bool isNumber=true;


  int i=0;
  QChar c;

  if (getFromStream(program, c)) {
   // check sign
    if (c=='-') { dfactor=-1; isNumber=getFromStream(program, c); i++; }
    else if (c=='+') { isNumber=getFromStream(program, c); i++; }

    if (isNumber && c=='0') {
        isNumber=getFromStream(program, c); i++;
        if (c=='x' || c=='X') return dfactor*readHex();
        if (c=='o' || c=='O') return dfactor*readOct();
        if (c=='b' || c=='B') return dfactor*readBin();
    }
    putbackStream(program, c);
    return dfactor*readDec();

  }
  return 0;
}

double QFMathParser::readDec()
{

  double current_double=0;

  bool isMantissa=true;
  double dfactor=1;
  int mantissaPos=0;
  bool isNumber=true;
  bool foundDot=false;


  QString num="";
  int i=0;
  QChar c;

  if (getFromStream(program, c)) {
   // check sign
    if (c=='-') { dfactor=-1; isNumber=getFromStream(program, c); i++; }
    else if (c=='+') { isNumber=getFromStream(program, c); i++; }

    while (isNumber) {
      if (!isMantissa) {
        switch(c.toLatin1()) {
          case '0':
          case '1':
          case '2':
          case '3':
          case '4':
          case '5':
          case '6':
          case '7':
          case '8':
          case '9':
            num+=c;
            break;
          case '-':
            if (i==mantissaPos) {
              num+=c;
            } else {
              isNumber=false;
              putbackStream(program, c);
            }
            break;
          case '+':
            if (i!=mantissaPos) {
              isNumber=false;
              putbackStream(program, c);
            }
            break;
          default:
            putbackStream(program, c);
            isNumber=false;
        }
      }
      if (isMantissa) {
        switch(c.toLatin1()) {
          case '0':
          case '1':
          case '2':
          case '3':
          case '4':
          case '5':
          case '6':
          case '7':
          case '8':
          case '9':
            num+=c;
            break;
          case '.':
            if (foundDot) {
              putbackStream(program, c);
              isNumber=false;
            } else {
              num+=c;
              foundDot=true;
            }
            break;
          case '-':
            if (i==mantissaPos) {
              dfactor=-1;
            } else {
              isNumber=false;
              putbackStream(program, c);
            }
            break;
          case '+':
            if (i==mantissaPos) {
              dfactor=1;
            } else {
              putbackStream(program, c);
              isNumber=false;
            }
            break;
          case 'e':
          case 'E':
            isMantissa=false;
            num+='e';
            mantissaPos=i+1;
            break;

          default:
            putbackStream(program, c);
            isNumber=false;
        }
      }
      i++;
      if (isNumber) isNumber=getFromStream(program, c);
    }
  }

    if (num.length()<=0) num="0";
    current_double = QLocale::c().toDouble(num);
    current_double=(current_double)*dfactor;
    return current_double;
}

double QFMathParser::readHex() {
    double current_double=0;
    double dfactor=1;
    bool isNumber=true;

    QString num="";
    int i=0;
    QChar c;

    if (getFromStream(program, c)) {
        // check sign
        if (c=='-') { dfactor=-1; isNumber=getFromStream(program, c); i++; }
        else if (c=='+') { isNumber=getFromStream(program, c); i++; }

        while (isNumber) {
            switch(c.toLatin1()) {
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                case 'A':
                case 'a':
                case 'B':
                case 'b':
                case 'C':
                case 'c':
                case 'D':
                case 'd':
                case 'E':
                case 'e':
                case 'F':
                case 'f':
                    num+=c;
                    break;
                default:
                    if (c.isLetterOrNumber()) {
                        qfmpError(QObject::tr("read_hex: found unexpected character '%1'").arg(QString(c)));
                        return 0;
                    }
                    putbackStream(program, c);
                    isNumber=false;
            }
            i++;
            if (isNumber) isNumber=getFromStream(program, c);
        }
    }

      if (num.length()<=0) num="0";
      current_double=num.toInt(NULL, 16);
      current_double=(current_double)*dfactor;
      return current_double;
}

double QFMathParser::readOct() {
    double current_double=0;
    double dfactor=1;
    bool isNumber=true;

    QString num="";
    int i=0;
    QChar c;

    if (getFromStream(program, c)) {
        // check sign
        if (c=='-') { dfactor=-1; isNumber=getFromStream(program, c); i++; }
        else if (c=='+') { isNumber=getFromStream(program, c); i++; }

        while (isNumber) {
            switch(c.toLatin1()) {
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                    num+=c;
                    break;
                case '8':
                case '9':
                    qfmpError(QObject::tr("read_oct: found unexpected digit '%1'").arg(QString(c)));
                    return 0;
                    break;

              default:
                  putbackStream(program, c);
                  isNumber=false;
                  break;
            }
            i++;
            if (isNumber) isNumber=getFromStream(program, c);
        }
    }

      if (num.length()<=0) num="0";
      current_double=num.toInt(NULL, 8);
      current_double=(current_double)*dfactor;
      return current_double;
}

double QFMathParser::readBin() {

  double current_double=0;
  double dfactor=1;
  bool isNumber=true;

  QString num="";
  int i=0;
  QChar c;

  if (getFromStream(program, c)) {
      // check sign
      if (c=='-') { dfactor=-1; isNumber=getFromStream(program, c); i++; }
      else if (c=='+') { isNumber=getFromStream(program, c); i++; }

      while (isNumber) {
          switch(c.toLatin1()) {
              case '0':
              case '1':
                  num+=c;
                  break;
              case '2':
              case '3':
              case '4':
              case '5':
              case '6':
              case '7':
              case '8':
              case '9':
                  qfmpError(QObject::tr("read_binary: found unexpected digit '%1'").arg(QString(c)));
                  break;
            default:
              putbackStream(program, c);
              isNumber=false;
              break;
          }
          i++;
          if (isNumber) isNumber=getFromStream(program, c);
      }
  }

    if (num.length()<=0) num="0";
    double fac=1;
    current_double=0;
    for (int i=num.size()-1; i>=0; i--) {
        if (num[i]=='1') current_double=current_double+fac;
        fac=fac*2.0;
    }
    current_double=(current_double)*dfactor;
    return current_double;
}

QString QFMathParser::readDelim(QChar delimiter){
    QString res="";
    QChar ch=0;

    while(getFromStream(program, ch)) {
		if (ch==delimiter ) {
            QChar ch1=peekStream(program);
		    if (ch1==delimiter) {
                    getFromStream(program, ch);
		            res=res+delimiter;
            } else {
                break;
            }
        } else if (ch=='\\')  {
            QChar ch1=peekStream(program);
            if (ch1=='\"') {
                getFromStream(program, ch);
                res=res+"\"";
            } else if (ch1=='\'') {
                getFromStream(program, ch);
                res=res+"\'";
            } else if (ch1=='\t') {
                getFromStream(program, ch);
                res=res+"\t";
            } else if (ch1=='\n') {
                getFromStream(program, ch);
                res=res+"\n";
            } else if (ch1=='\r') {
                getFromStream(program, ch);
                res=res+"\r";
            } else if (ch1=='\\') {
                getFromStream(program, ch);
                res=res+"\\";
            } else if (ch1=='/') {
                getFromStream(program, ch);
                res=res+"/";
            } else {
                //qfmpError(QObject::tr("read_delmited_string: found unexpected escape sequence '\\%1'").arg(QString(ch1)));
                //return res;
                //break;
                res=res+QString(ch)+QString(ch1);
                getFromStream(program, ch);
            }
        } else if ((program) && (ch!=delimiter)) res=res+ch;
	}

	return res;
}









QFMathParser::qfmpUnaryNode::qfmpUnaryNode(char op, QFMathParser::qfmpNode* c, QFMathParser* p, QFMathParser::qfmpNode* par):
    qfmpNode(p, par)
{
  child=c;
  child->setParent(this);
  setParser(p);
  setParent(par);
  operation=op;
}

QFMathParser::qfmpUnaryNode::~qfmpUnaryNode()
{
    if (child) delete child;
}


void QFMathParser::qfmpUnaryNode::evaluate(qfmpResult &res)
{
    //qfmpResult c;
    child->evaluate(res);

    switch(operation) {
      case '!':
            if (res.type==qfmpBool) {
              res.boolean=!res.boolean;
              return ;
            } else if (res.type==qfmpBoolVector) {
                for (int i=0; i<res.boolVec.size(); i++) res.boolVec[i]=!res.boolVec[i];
                return ;
             } else parser->qfmpError(QObject::tr("'!' only defined for bool (argument type was %1)").arg(res.typeName()));
           break;
        case '-':
            if (res.type==qfmpDouble) {
                res.num=-res.num;
                return ;
            } else if (res.type==qfmpDoubleVector) {
                //res.setDoubleVec(res.numVec.size());
                for (int i=0; i<res.numVec.size(); i++) res.numVec[i]=-res.numVec[i];
                return ;
             } else parser->qfmpError(QObject::tr("'-' only defined for numbers (argument type was %1)").arg(res.typeName()));
             break;
        case '~':
             if (res.isInteger()) {
                 res.num=~res.toInteger();
                 return ;
             } else if (res.type==qfmpDoubleVector) {
                 for (int i=0; i<res.numVec.size(); i++) res.numVec[i]=double(~int32_t(res.numVec[i]));
                 return ;
             } else parser->qfmpError(QObject::tr("'~' only defined for integer numbers (argument type was %1)").arg(res.typeName()));
             break;
       default: parser->qfmpError(QObject::tr("unknown unary operation '%1'").arg(operation));
    }
    res.setInvalid();
}

QFMathParser::qfmpNode *QFMathParser::qfmpUnaryNode::copy(QFMathParser::qfmpNode *par)
{
    QFMathParser::qfmpNode *n= new QFMathParser::qfmpUnaryNode(operation, child->copy(), getParser(), par);
    return n;
}

bool QFMathParser::qfmpUnaryNode::createByteCode(QFMathParser::ByteCodeProgram &program, QFMathParser::ByteCodeEnvironment *environment)
{
    bool ok=true;
    if (child) ok=ok&&child->createByteCode(program, environment);

    if (!ok) return false;

    switch(operation) {
        case '!':
            program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcLogicNot));
            break;
        case '-':
            program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcNeg));
            break;
        case '~':
            program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcBitNot));
            break;


        default:
            parser->qfmpError(QObject::tr("unknown unary operation"));
            ok=false;
            break;

    }
    return ok;
}

QString QFMathParser::qfmpUnaryNode::print() const
{
    return QString("%1(%2)").arg(QString(operation)).arg(child->print());
}

QString QFMathParser::qfmpUnaryNode::printTree(int level) const
{
    return QString(2*level, QLatin1Char(' '))+QString("UnaryNode %2\n%1").arg(child->printTree(level+1)).arg(QString(operation));
}







QFMathParser::qfmpBinaryArithmeticNode::qfmpBinaryArithmeticNode(char op, QFMathParser::qfmpNode* l, QFMathParser::qfmpNode* r, QFMathParser* p, QFMathParser::qfmpNode* par):
    qfmpNode(p, par)
{
  left=l;
  right=r;
  if (left) left->setParent(this);
  if (right) right->setParent(this);
  setParser(p);
  setParent(par);
  operation=op;
}

QFMathParser::qfmpBinaryArithmeticNode::~qfmpBinaryArithmeticNode()
{
    if (left) delete left;
    if (right) delete right;
}



void QFMathParser::qfmpBinaryArithmeticNode::evaluate(qfmpResult& res){
  qfmpResult l;
  if (left) left->evaluate(l);
  qfmpResult r;
  if (right) right->evaluate(r);
  //qfmpResult res;

  switch(operation) {
      case '+':
            qfmpResult::add(res, l, r, getParser());
          break;
      case '-':
            qfmpResult::sub(res, l, r, getParser());
          break;
      case '*':
            qfmpResult::mul(res, l, r, getParser());
          break;
      case '/':
            qfmpResult::div(res, l, r, getParser());
          break;
      case '%':
            qfmpResult::mod(res, l, r, getParser());
          break;
      case '^':
          qfmpResult::power(res, l, r, getParser());
          break;
      case '&':
          qfmpResult::bitwiseand(res, l, r, getParser());
          break;
      case '|':
          qfmpResult::bitwiseor(res, l, r, getParser());
          break;

      default:
          parser->qfmpError(QObject::tr("unknown arithmetic operation"));
          res.setInvalid();
          break;

  }
}

QFMathParser::qfmpNode *QFMathParser::qfmpBinaryArithmeticNode::copy(QFMathParser::qfmpNode *par)
{
    QFMathParser::qfmpNode *n= new QFMathParser::qfmpBinaryArithmeticNode(operation, left->copy(), right->copy(), getParser(), par);
    return n;
}

bool QFMathParser::qfmpBinaryArithmeticNode::createByteCode(QFMathParser::ByteCodeProgram &program, ByteCodeEnvironment *environment)
{
    bool ok=true;
    if (right) ok=ok&&right->createByteCode(program, environment);
    if (left) ok=ok&&left->createByteCode(program, environment);

    if (!ok) return false;

    switch(operation) {
        case '+':
            program.append(QFMathParser::ByteCodeInstruction(bcAdd));
            break;
        case '-':
            program.append(QFMathParser::ByteCodeInstruction(bcSub));
            break;
        case '*':
            program.append(QFMathParser::ByteCodeInstruction(bcMul));
            break;
        case '/':
            program.append(QFMathParser::ByteCodeInstruction(bcDiv));
            break;
        case '%':
            program.append(QFMathParser::ByteCodeInstruction(bcMod));
            break;
        case '^':
            program.append(QFMathParser::ByteCodeInstruction(bcPow));
            break;
        case '&':
            program.append(QFMathParser::ByteCodeInstruction(bcBitAnd));
            break;
        case '|':
            program.append(QFMathParser::ByteCodeInstruction(bcBitOr));
            break;

        default:
            parser->qfmpError(QObject::tr("unknown arithmetic operation"));
            ok=false;
            break;

    }
    return ok;
}

QString QFMathParser::qfmpBinaryArithmeticNode::print() const
{
    return QString("(%1) %2 (%3)").arg(left->print()).arg(QString(operation)).arg(right->print());
}

QString QFMathParser::qfmpBinaryArithmeticNode::printTree(int level) const
{
    return QString(2*level, QLatin1Char(' '))+QString("BinaryArithmeticNode %2\n%1\n%3").arg(left->printTree(level+1)).arg(QString(operation)).arg(right->printTree(level+1));
}







QFMathParser::qfmpCompareNode::qfmpCompareNode(char op, QFMathParser::qfmpNode* l, QFMathParser::qfmpNode* r, QFMathParser* p, QFMathParser::qfmpNode* par):
    qfmpNode(p, par)
{
  left=l;
  right=r;
  if (left) left->setParent(this);
  if (right) right->setParent(this);
  setParser(p);
  setParent(par);
  operation=op;
}

QFMathParser::qfmpCompareNode::~qfmpCompareNode()
{
    if (left) delete left;
    if (right) delete right;
}


void QFMathParser::qfmpCompareNode::evaluate(qfmpResult &res)
{
    qfmpResult l;
    if (left) left->evaluate(l);
    qfmpResult r;
    if (right) right->evaluate(r);

  /*if (l.type!=r.type) {
      parser->qfmpError(QObject::tr("you can't compare different datatypes"));
      res.setInvalid();
      return;
  }*/

  res.type=qfmpBool;
  switch(operation) {
        case qfmpCOMPequal:
            qfmpResult::compareequal(res, l, r, getParser());
            break;
        case qfmpCOMPnequal:
              qfmpResult::comparenotequal(res, l, r, getParser());
              break;
        case qfmpCOMPgreater:
              qfmpResult::comparegreater(res, l, r, getParser());
              break;

        case qfmpCOMPlesser:
              qfmpResult::comparesmaller(res, l, r, getParser());
              break;

        case qfmpCOMPgreaterequal:
              qfmpResult::comparegreaterequal(res, l, r, getParser());
              break;

        case qfmpCOMPlesserequal:
              qfmpResult::comparesmallerequal(res, l, r, getParser());
              break;


        default:
              parser->qfmpError(QObject::tr("compare operation between %1 and %2 not possible").arg(l.toTypeString()).arg(r.toTypeString()));
              res.setInvalid();
              break;
    }
}

QFMathParser::qfmpNode *QFMathParser::qfmpCompareNode::copy(QFMathParser::qfmpNode *par)
{
    QFMathParser::qfmpNode *n= new QFMathParser::qfmpCompareNode(operation, left->copy(), right->copy(), getParser(), par);
    return n;
}

bool QFMathParser::qfmpCompareNode::createByteCode(QFMathParser::ByteCodeProgram &program, QFMathParser::ByteCodeEnvironment *environment)
{
    bool ok=true;
    if (right) ok=ok&&right->createByteCode(program, environment);
    if (left) ok=ok&&left->createByteCode(program, environment);

    if (!ok) return false;

    switch(operation) {
        case qfmpCOMPequal:
            program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcCmpEqual));
            break;
        case qfmpCOMPnequal:
            program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcCmpEqual));
            program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcLogicNot));
            break;
        case qfmpCOMPlesser:
            program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcCmpLesser));
            break;
        case qfmpCOMPlesserequal:
            program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcCmpLesserEqual));
            break;
        case qfmpCOMPgreater:
            program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcCmpLesserEqual));
            program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcLogicNot));
            break;
        case qfmpCOMPgreaterequal:
            program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcCmpLesser));
            program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcLogicNot));
            break;

        default:
            parser->qfmpError(QObject::tr("unknown compare operation"));
            ok=false;
            break;

    }
    return ok;
}

QString QFMathParser::qfmpCompareNode::print() const
{
    return QString("(%1) %2 (%3)").arg(left->print()).arg(QString(opAsString())).arg(right->print());
}

QString QFMathParser::qfmpCompareNode::printTree(int level) const
{
    return QString(2*level, QLatin1Char(' '))+QString("CompareNode %2\n%1\n%3").arg(left->printTree(level+1)).arg(opAsString()).arg(right->printTree(level+1));
}

QString QFMathParser::qfmpCompareNode::opAsString() const
{
    switch(operation) {
        case qfmpCOMPequal:
            return "==";
            break;
        case qfmpCOMPnequal:
            return "!=";
            break;
        case qfmpCOMPlesser:
            return "<";
            break;
        case qfmpCOMPlesserequal:
            return "<=";
            break;
        case qfmpCOMPgreater:
            return ">";
            break;
        case qfmpCOMPgreaterequal:
            return ">=";
            break;

        default:
            break;

    }
    return QString("???");
}







QFMathParser::qfmpBinaryBoolNode::qfmpBinaryBoolNode(char op, QFMathParser::qfmpNode* l, QFMathParser::qfmpNode* r, QFMathParser* p, QFMathParser::qfmpNode* par):
    qfmpNode(p, par)
{
  left=l;
  right=r;
  if (left) left->setParent(this);
  if (right) right->setParent(this);
  setParser(p);
  setParent(par);
  operation=op;
}

QFMathParser::qfmpBinaryBoolNode::~qfmpBinaryBoolNode()
{
    if (left) delete left;
    if (right) delete right;
}


void QFMathParser::qfmpBinaryBoolNode::evaluate(qfmpResult &res)
{
    qfmpResult l;
    if (left) left->evaluate(l);
    qfmpResult r;
    if (right) right->evaluate(r);

  /*if ((l.type!=qfmpBool)||(r.type!=qfmpBool)) {
      res.setInvalid();
      parser->qfmpError(QObject::tr("logical operations only for bool"));
  } else {*/
      switch(operation) {
          case qfmpLOPand:
              qfmpResult::logicand(res, l, r, getParser());
              break;
          case qfmpLOPor:
              qfmpResult::logicor(res, l, r, getParser());
              break;
          case qfmpLOPnor:
              qfmpResult::logicnor(res, l, r, getParser());
              break;
          case qfmpLOPxor:
              qfmpResult::logicxor(res, l, r, getParser());
              break;
          case qfmpLOPnand:
              qfmpResult::logicnand(res, l, r, getParser());
              break;


          default: parser->qfmpError(QObject::tr("unknown error"));
              res.setInvalid();
              break;
      }
  //}
}

QFMathParser::qfmpNode *QFMathParser::qfmpBinaryBoolNode::copy(QFMathParser::qfmpNode *par)
{
    QFMathParser::qfmpNode *n= new QFMathParser::qfmpBinaryBoolNode(operation, left->copy(), right->copy(), getParser(), par);
    return n;
}

bool QFMathParser::qfmpBinaryBoolNode::createByteCode(QFMathParser::ByteCodeProgram &program, QFMathParser::ByteCodeEnvironment *environment)
{
    bool ok=true;
    if (right) ok=ok&&right->createByteCode(program, environment);
    if (left) ok=ok&&left->createByteCode(program, environment);

    if (!ok) return false;

    switch(operation) {
        case qfmpLOPand:
            program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcLogicAnd));
            break;
        case qfmpLOPor:
            program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcLogicOr));
            break;
        case qfmpLOPxor:
            program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcLogicXor));
            break;
        case qfmpLOPnor:
            program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcLogicOr));
            program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcLogicNot));
            break;
        case qfmpLOPnand:
            program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcLogicAnd));
            program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcLogicNot));
            break;

        default:
            parser->qfmpError(QObject::tr("unknown logic operation"));
            ok=false;
            break;

    }
    return ok;
}

QString QFMathParser::qfmpBinaryBoolNode::print() const
{
    return QString("(%1) %2 (%3)").arg(left->print()).arg(QString(opAsString())).arg(right->print());
}

QString QFMathParser::qfmpBinaryBoolNode::printTree(int level) const
{
    return QString(2*level, QLatin1Char(' '))+QString("BinaryBoolNode %2\n%1\n%3").arg(left->printTree(level+1)).arg(opAsString()).arg(right->printTree(level+1));
}

QString QFMathParser::qfmpBinaryBoolNode::opAsString() const
{
    switch(operation) {
        case qfmpLOPand:
            return "and";
            break;
        case qfmpLOPor:
            return "or";
            break;
        case qfmpLOPxor:
            return "xor";
            break;
        case qfmpLOPnor:
            return "nor";
            break;
        case qfmpLOPnand:
            return "nand";
            break;
        default:
            break;

    }
    return QString("???");
}




QFMathParser::qfmpVariableNode::qfmpVariableNode(QString name, QFMathParser* p, QFMathParser::qfmpNode* par):
    qfmpNode(p, par)
 {
  var=name;
  setParser(p);
  setParent(par);
};

qfmpResult QFMathParser::qfmpVariableNode::evaluate() {
    return getParser()->getVariable(var);
}

void QFMathParser::qfmpVariableNode::evaluate(qfmpResult &result)
{
    result=getParser()->getVariable(var);
}

QFMathParser::qfmpNode *QFMathParser::qfmpVariableNode::copy(QFMathParser::qfmpNode *par)
{
    return new QFMathParser::qfmpVariableNode(var, getParser(), par);
}

bool QFMathParser::qfmpVariableNode::createByteCode(QFMathParser::ByteCodeProgram &program, QFMathParser::ByteCodeEnvironment* environment)
{
    QFMathParser::qfmpVariable def;

    if (environment->heapVariables.contains(var) && environment->heapVariables[var].size()>0) {
        if (environment->heapVariables[var].last()>=0) {
            program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcHeapRead, environment->heapVariables[var].last()));
            return true;
        } else {
            getParser()->qfmpError(QObject::tr("heap-adress-error, tried to access heap item %1").arg(environment->heapVariables[var].last()));
            return false;
        }
    } else if (getParser()->environment.getVariableDef(var, def)) {
        int level=getParser()->environment.getVariableLevel(var);
        if (level>0) {
            getParser()->qfmpError(QObject::tr("only top-level variables allowed in byte-coded expressionen (variable '%1', level %2)").arg(var).arg(level));
            return false;
        }
        if (def.getType()==qfmpDouble) {
            double* varP=def.getNum();
            if (varP) {
                program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcVarRead, varP));
                return true;
            } else {
                getParser()->qfmpError(QObject::tr("variable '%1' points to NULL").arg(var));
                return false;
            }
        } else {
            getParser()->qfmpError(QObject::tr("only number-valued variables allowed in byte-coded expressions (variable '%1')").arg(var));
            return false;
        }
    } else {
        getParser()->qfmpError(QObject::tr("variable '%1' not found").arg(var));
        return false;
    }

}

QString QFMathParser::qfmpVariableNode::print() const
{
    return var;

}

QString QFMathParser::qfmpVariableNode::printTree(int level) const
{
    return QString(2*level, QLatin1Char(' '))+QString("VariableNode '%1'").arg(var);
}




void QFMathParser::qfmpNodeList::add(QFMathParser::qfmpNode* n){
    list.push_back(n);
}

QFMathParser::qfmpNode *QFMathParser::qfmpNodeList::popFirst(bool deleteObject)
{
    if (list.size()>0) {
        if (deleteObject) {
            delete list.first();
            list.removeFirst();
        } else {
            QFMathParser::qfmpNode * n=list.first();
            list.removeFirst();
            return n;
        }

    }
    return NULL;
}

QFMathParser::qfmpNode *QFMathParser::qfmpNodeList::popLast(bool deleteObject)
{
    if (list.size()>0) {
        if (deleteObject) {
            delete list.last();
            list.removeLast();
        } else {
            QFMathParser::qfmpNode * n=list.last();
            list.removeLast();
            return n;
        }

    }
    return NULL;
}



void QFMathParser::qfmpNodeList::evaluate(qfmpResult &res)
{
    int n=getCount();
    //qDebug()<<"evaluating() nodelist with "<<n<<" items";
    if (n>0) {
       for (int i=0; i<n; i++) {
          res.setInvalid();
          if (list[i]) list[i]->evaluate(res);
          //qDebug()<<"eval nodelist(), item"<<i+1<<"/"<<n<<":  "<<res.toTypeString();
       }
       return ;
    } else parser->qfmpError(QObject::tr("NodeList empty"));
    res.setInvalid();
}

QFMathParser::qfmpNode *QFMathParser::qfmpNodeList::copy(QFMathParser::qfmpNode *par)
{

    QFMathParser::qfmpNodeList* n= new QFMathParser::qfmpNodeList(getParser());
    if (list.size()>0) {
        for (int i=0; i<list.size(); i++) {
            n->add(list[i]->copy(n));
        }
    }

    n->setParent(par);
    return n;
}

bool QFMathParser::qfmpNodeList::createByteCode(QFMathParser::ByteCodeProgram &program, QFMathParser::ByteCodeEnvironment *environment)
{
    bool ok=true;
    for (int i=0; (ok&&i<list.size()); i++) {
        if (list[i]) {
            ok=ok&&list[i]->createByteCode(program, environment);
        }
    }

    return ok;
}

QString QFMathParser::qfmpNodeList::print() const
{
    QStringList sl;
    if (list.size()>0) {
        for (int i=0; i<list.size(); i++) {
            sl<<list[i]->print();
        }
    }
    return sl.join("; ");
    //return QString("%1[%3] = %2").arg(variable).arg(child->print()).arg(index->print());
}

QString QFMathParser::qfmpNodeList::printTree(int level) const
{
    QStringList sl;
    if (list.size()>0) {
        for (int i=0; i<list.size(); i++) {
            sl<<list[i]->printTree(level+1);
        }
    }
    return QString(2*level, QLatin1Char(' '))+QString("NodeList\n%1").arg(sl.join("\n"));
}

QFMathParser::qfmpNodeList::qfmpNodeList(QFMathParser *p, QFMathParser::qfmpNode *par):
    qfmpNode(p, par)
{
}

QFMathParser::qfmpNodeList::~qfmpNodeList() {

    if (list.size()>0) for (int i=0; i<list.size(); i++) {
        if (list[i]) delete list[i];
    }
    list.clear();
}


QFMathParser::qfmpVariableAssignNode::~qfmpVariableAssignNode()
{
    if (child) delete child;
}

QFMathParser::qfmpVariableAssignNode::qfmpVariableAssignNode(QString var, QFMathParser::qfmpNode* c, QFMathParser* p, QFMathParser::qfmpNode* par):
    qfmpNode(p, par)
{
  child=c;
  if (child) child->setParent(this);
  setParser(p);
  setParent(par);
  variable=var;
//  std::cout<<"assign: "<<var<<std::endl;
}



void QFMathParser::qfmpVariableAssignNode::evaluate(qfmpResult &result)
{
    if (child) child->evaluate(result);
    getParser()->setVariable(variable, result);
}

QFMathParser::qfmpNode *QFMathParser::qfmpVariableAssignNode::copy(QFMathParser::qfmpNode *par)
{
    if (child) return new QFMathParser::qfmpVariableAssignNode(variable, child->copy(NULL), getParser(), par);
    else return new QFMathParser::qfmpVariableAssignNode(variable, NULL, getParser(), par);
}

bool QFMathParser::qfmpVariableAssignNode::createByteCode(QFMathParser::ByteCodeProgram &program, QFMathParser::ByteCodeEnvironment *environment)
{
    QFMathParser::qfmpVariable def;

    bool ok=child->createByteCode(program, environment);

    if (ok) {
        if (environment->heapVariables.contains(variable) && environment->heapVariables[variable].size()>0) {
            if (environment->heapVariables[variable].last()>=0) {
                program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcHeapWrite, environment->heapVariables[variable].last()));
                program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcHeapRead, environment->heapVariables[variable].last()));
                return true;
            } else {
                getParser()->qfmpError(QObject::tr("heap-adress-error, tried to access heap item %1").arg(environment->heapVariables[variable].last()));
                return false;
            }
        } else if (getParser()->environment.getVariableDef(variable, def)) {
            int level=getParser()->environment.getVariableLevel(variable);
            if (level>0) {
                getParser()->qfmpError(QObject::tr("only top-level variables allowed in byte-coded expressionen (variable '%1', level %2)").arg(variable).arg(level));
                return false;
            }
            double* varP=NULL;
            if (def.getType()==qfmpDouble) {
                varP=def.getNum();
                if (varP) {
                    program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcVarWrite, varP));
                    program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcVarRead, varP));
                    return true;
                } else {
                    getParser()->qfmpError(QObject::tr("variable '%1' points to NULL").arg(variable));
                    return false;
                }
            } else {
                getParser()->qfmpError(QObject::tr("only number-values variables allowed in byte-coded expressions (variable '%1')").arg(variable));
                return false;
            }
        } else {
            /*getParser()->qfmpError(QObject::tr("variable '%1' not found").arg(var));
            return false;*/
            int varIdx=environment->pushVar(variable);
            program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcHeapWrite, varIdx));
            program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcHeapRead, varIdx));
            return true;
        }
    }
    return ok;
}

QString QFMathParser::qfmpVariableAssignNode::print() const
{
    return QString("%1 = %2").arg(variable).arg(child->print());

}

QString QFMathParser::qfmpVariableAssignNode::printTree(int level) const
{
    return QString(2*level, QLatin1Char(' '))+QString("VariableAssigneNode '%2'\n%1").arg(child->printTree(level+1)).arg(variable);
}

QFMathParser::qfmpFunctionNode::qfmpFunctionNode(QString name, QVector<qfmpNode *> params, QFMathParser *p, qfmpNode *par):
    qfmpNode(p, par)
 {
  child=params;
  fun=name;
  setParser(p);
  setParent(par);
  if (child.size()>0) {
    for (int i=0; i<child.size(); i++) {
      if (child[i]) child[i]->setParent(this);
    }
  }
}

void QFMathParser::qfmpFunctionNode::evaluate(qfmpResult &result)
{
    /*QVector<qfmpResult> data(child.size(), QFMathParser::getInvalidResult());
    for (int i=0; i<child.size(); i++) {
        if (child[i]) child[i]->evaluate(data[i]);
    }*/
    result=parser->evaluateFunction(fun, child);
}

QFMathParser::qfmpNode *QFMathParser::qfmpFunctionNode::copy(QFMathParser::qfmpNode *par)
{
    QVector<qfmpNode *> params;
    if (child.size()>0) {
        for (int i=0; i<child.size(); i++) {
            params<<child[i]->copy(NULL);
        }
    }

    return new QFMathParser::qfmpFunctionNode(fun, params, getParser(), par);
}

bool QFMathParser::qfmpFunctionNode::createByteCode(QFMathParser::ByteCodeProgram &program, ByteCodeEnvironment *environment)
{
    bool ok=true;
    int params=0;
    for (int i=child.size()-1; i>=0; i--) {
        if (child[i]) {
            ok=ok&&child[i]->createByteCode(program, environment);
            params++;
        }
    }
    if (ok) {
        QFMathParser::qfmpFunctionDescriptor def;
        if (environment->functionDefs.contains(fun) && environment->functionDefs[fun].second) {
            if (environment->inFunctionCalls.contains(fun)) {
                getParser()->qfmpError(QObject::tr("no recursive function calls allowed in byte-code (function '%1')").arg(fun));
                return false;
            } else if (environment->functionDefs[fun].first.size()==params) {
                for (int i=0; i<params; i++) {
                    int varID=environment->pushVar(environment->functionDefs[fun].first[i]);
                    program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcHeapWrite, varID));
                }

                environment->inFunctionCalls.insert(fun);
                ok=ok&&environment->functionDefs[fun].second->createByteCode(program, environment);
                environment->inFunctionCalls.remove(fun);
                for (int i=0; i<params; i++) {
                    environment->popVar(environment->functionDefs[fun].first[i]);
                }
                return ok;
            } else {
                getParser()->qfmpError(QObject::tr("function '%1' defined with wrong number of parameters (required: %2, in definition: %3)").arg(fun).arg(params).arg(environment->functionDefs[fun].first.size()));
                return false;
            }
        } else if (getParser()->environment.getFunctionDef(fun, def)) {
            int level=getParser()->environment.getFunctionLevel(fun);
            if (level>0) {
                getParser()->qfmpError(QObject::tr("only top-level functions allowed in byte-coded expressionen (function '%1', level %2)").arg(fun).arg(level));
                return false;
            }
            qfmpFunctiontype ft=getParser()->environment.getFunctionType(fun);
            if (ft==functionFromNode || ft==functionInvalid) {
                getParser()->qfmpError(QObject::tr("only simple functions allowed in byte-coded expressionen (function '%1')").arg(fun));
                return false;
            }
            if ((!def.simpleFuncPointer.contains(params) && !def.simpleFuncPointer.contains(100+params)) || (!def.simpleFuncPointer[params] && !def.simpleFuncPointer[100+params])) {
                program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcCallResultFunction, fun, params));
                //getParser()->qfmpError(QObject::tr("no implementation of function '%1(...)' with %2 parameters found").arg(fun).arg(params));
                return true;
            }
            if (def.simpleFuncPointer.contains(params)) {
                void* fp=def.simpleFuncPointer[params];
                program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcCallCFunction, fp, params));
                return true;
            } else {
                void* fp=def.simpleFuncPointer[params+100];
                program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcCallCMPFunction, fp, params));
                return true;
            }
        }
    }
    return false;
}

QString QFMathParser::qfmpFunctionNode::print() const
{
    QStringList sl;
    for (int i=0; i<child.size(); i++) {
        sl<<child[i]->print();
    }
    return QString("%1(%2)").arg(fun).arg(sl.join(", "));
}

QString QFMathParser::qfmpFunctionNode::printTree(int level) const
{
    QStringList sl;
    if (child.size()>0) {
        for (int i=0; i<child.size(); i++) {
            sl<<child[i]->printTree(level+1);
        }
    }
    if (child.size()>0) return QString(2*level, QLatin1Char(' '))+QString("FunctionNode '%1(...)'\n%2").arg(fun).arg(sl.join("\n"));
    else return QString(2*level, QLatin1Char(' '))+QString("FunctionNode '%1()'").arg(fun);
}


QFMathParser::qfmpFunctionNode::~qfmpFunctionNode() {

    if (child.size()>0) {
      for (int i=0; i<child.size(); i++) {
        if (child[i]) delete child[i];
      }
      child.clear();
    }
}



QFMathParser::qfmpVariable::qfmpVariable()
{
    type=qfmpVoid;     /*!< \brief type of the variable */
    internal=false;           /*!< \brief this is an internal variable */
    str=NULL;        /*!< \brief this points to the variable data if \c type==qfmpString */
    num=NULL;             /*!< \brief this points to the variable data if \c type==qfmpDouble */
    boolean=NULL;
    numVec=NULL;
    boolVec=NULL;
    strVec=NULL;
    custom=NULL;
    structData=NULL;
}

QFMathParser::qfmpVariable::qfmpVariable(double *ref)
{
    type=qfmpDouble;     /*!< \brief type of the variable */
    internal=false;           /*!< \brief this is an internal variable */
    str=NULL;        /*!< \brief this points to the variable data if \c type==qfmpString */
    num=ref;             /*!< \brief this points to the variable data if \c type==qfmpDouble */
    boolean=NULL;
    numVec=NULL;
    boolVec=NULL;
    strVec=NULL;
    custom=NULL;
    structData=NULL;
}

QFMathParser::qfmpVariable::qfmpVariable(QString *ref)
{
    type=qfmpString;     /*!< \brief type of the variable */
    internal=false;           /*!< \brief this is an internal variable */
    str=ref;        /*!< \brief this points to the variable data if \c type==qfmpString */
    num=NULL;             /*!< \brief this points to the variable data if \c type==qfmpDouble */
    boolean=NULL;
    numVec=NULL;
    boolVec=NULL;
    strVec=NULL;
    custom=NULL;
    structData=NULL;
}

QFMathParser::qfmpVariable::qfmpVariable(bool *ref)
{
    type=qfmpBool;     /*!< \brief type of the variable */
    internal=false;           /*!< \brief this is an internal variable */
    str=NULL;        /*!< \brief this points to the variable data if \c type==qfmpString */
    num=NULL;             /*!< \brief this points to the variable data if \c type==qfmpDouble */
    boolean=ref;
    numVec=NULL;
    boolVec=NULL;
    strVec=NULL;
    custom=NULL;
    structData=NULL;
}

QFMathParser::qfmpVariable::qfmpVariable(QVector<double> *ref)
{
    type=qfmpDoubleVector;     /*!< \brief type of the variable */
    internal=false;           /*!< \brief this is an internal variable */
    str=NULL;        /*!< \brief this points to the variable data if \c type==qfmpString */
    num=NULL;             /*!< \brief this points to the variable data if \c type==qfmpDouble */
    boolean=NULL;
    numVec=ref;
    boolVec=NULL;
    strVec=NULL;
    custom=NULL;
    structData=NULL;
}

QFMathParser::qfmpVariable::qfmpVariable(QVector<bool> *ref)
{
    type=qfmpBoolVector;     /*!< \brief type of the variable */
    internal=false;           /*!< \brief this is an internal variable */
    str=NULL;        /*!< \brief this points to the variable data if \c type==qfmpString */
    num=NULL;             /*!< \brief this points to the variable data if \c type==qfmpDouble */
    boolean=NULL;
    numVec=NULL;
    boolVec=ref;
    strVec=NULL;
    custom=NULL;
    structData=NULL;
}

QFMathParser::qfmpVariable::qfmpVariable(QStringList *ref)
{
    type=qfmpStringVector;     /*!< \brief type of the variable */
    internal=false;           /*!< \brief this is an internal variable */
    str=NULL;        /*!< \brief this points to the variable data if \c type==qfmpString */
    num=NULL;             /*!< \brief this points to the variable data if \c type==qfmpDouble */
    boolean=NULL;
    numVec=NULL;
    boolVec=NULL;
    strVec=ref;
    custom=NULL;
    custom=NULL;
    structData=NULL;
}

QFMathParser::qfmpVariable::qfmpVariable(QMap<QString, qfmpResult> *ref)
{
    type=qfmpStruct;     /*!< \brief type of the variable */
    internal=false;           /*!< \brief this is an internal variable */
    str=NULL;        /*!< \brief this points to the variable data if \c type==qfmpString */
    num=NULL;             /*!< \brief this points to the variable data if \c type==qfmpDouble */
    boolean=NULL;
    numVec=NULL;
    boolVec=NULL;
    strVec=NULL;
    custom=NULL;
    custom=NULL;
    structData=ref;
}

QFMathParser::qfmpVariable::qfmpVariable(qfmpCustomResult *ref)
{
    type=qfmpCustom;     /*!< \brief type of the variable */
    internal=false;           /*!< \brief this is an internal variable */
    str=NULL;        /*!< \brief this points to the variable data if \c type==qfmpString */
    num=NULL;             /*!< \brief this points to the variable data if \c type==qfmpDouble */
    boolean=NULL;
    numVec=NULL;
    boolVec=NULL;
    strVec=NULL;
    this->custom=ref;
    structData=NULL;
}

void QFMathParser::qfmpVariable::clearMemory()
{
    if (internal) {
        if (type==qfmpDouble && num) delete (num);
        if (type==qfmpBool && boolean) delete (boolean);
        if (type==qfmpString && str) delete (str);
        if (type==qfmpDoubleVector && numVec) delete (numVec);
        if (type==qfmpBoolVector && boolVec) delete (boolVec);
        if (type==qfmpStringVector && strVec) delete (strVec);
        if (type==qfmpStruct && structData) delete (structData);
        if (type==qfmpCustom && custom) delete (custom);
        num=NULL;
        boolean=NULL;
        str=NULL;
        numVec=NULL;
        boolVec=NULL;
        strVec=NULL;
        custom=NULL;
        structData=NULL;
    }
    internal=false;
}

qfmpResult QFMathParser::qfmpVariable::toResult() const
{
    qfmpResult r;
    toResult(r);
    return r;
}

void QFMathParser::qfmpVariable::toResult(qfmpResult &r) const
{
    r.type=type;
    r.isValid=true;
    if (type==qfmpDouble && num) {
        r.num=*(num);
    } else if (type==qfmpString && str) {
        r.str=*(str);
    } else if (type==qfmpBool && boolean) {
        r.boolean=*(boolean);
    } else if (type==qfmpDoubleVector && numVec) {
        r.numVec=*(numVec);
    } else if (type==qfmpBoolVector && boolVec) {
        r.boolVec=*(boolVec);
    } else if (type==qfmpStringVector && strVec) {
        r.strVec=*(strVec);
    } else if (type==qfmpStruct && structData) {
        r.structData=*(structData);
    } else if (type==qfmpCustom && custom) {
        r.setCustomCopy(custom);
    } else {
        r.isValid=false;
    }

}

bool QFMathParser::qfmpVariable::isInternal() const
{
    return internal;
}

void QFMathParser::qfmpVariable::set(const qfmpResult &result)
{
    //qDebug()<<"qfmpVariable::set("<<result.toTypeString()<<")   vartype="<<qfmpResultTypeToString(type)<<"  resultType="<<qfmpResultTypeToString(result.type);
    if (type==result.type) {
        switch (result.type) {
            case qfmpDouble:  if (!num) {num=new double; internal=true; } *num=result.num; break;
            case qfmpDoubleVector:  if (!numVec){ numVec=new QVector<double>; internal=true; } *numVec=result.numVec; break;
            case qfmpStringVector:  if (!strVec){ strVec=new QStringList; internal=true; } *strVec=result.strVec; break;
            case qfmpBoolVector:  if (!boolVec){ boolVec=new QVector<bool>; internal=true; } *boolVec=result.boolVec; break;
            case qfmpString:  if (!str) {str=new QString; internal=true; } *str=result.str; break;
            case qfmpBool:  if (!boolean) {boolean=new bool; internal=true; } *boolean=result.boolean; break;
            case qfmpStruct:  if (!structData) {structData=new QMap<QString, qfmpResult>; internal=true; } *structData=result.structData; break;
            case qfmpCustom:  if (custom && custom->typeName()==result.custom()->typeName()) {
                    *custom=*result.custom();
                } else {
                    if (result.custom()) {
                        if (custom&&internal) delete custom;
                        internal=true;
                        custom=result.custom()->copy();
                    }
                }  break;
            case qfmpVoid: break;
        }
    } else {
        clearMemory();
        internal=true;
        type=result.type;
        switch (result.type) {
            case qfmpDouble: num=new double; *num=result.num; break;
            case qfmpDoubleVector: numVec=new QVector<double>; *numVec=result.numVec; break;
            case qfmpStringVector: strVec=new QStringList; *strVec=result.strVec; break;
            case qfmpBoolVector: boolVec=new QVector<bool>; *boolVec=result.boolVec; break;
            case qfmpString: str=new QString; *str=result.str; break;
            case qfmpBool: boolean=new bool; *boolean=result.boolean; break;
            case qfmpStruct: structData=new QMap<QString, qfmpResult>; *structData=result.structData; break;
            case qfmpCustom:  if (custom && custom->typeName()==result.custom()->typeName()) {
                    *custom=*result.custom();
                } else {
                    if (result.custom()) {
                        if (custom&&internal) delete custom;
                        internal=true;
                        custom=result.custom()->copy();
                    }
                }  break;
            case qfmpVoid: break;
        }
    }
    //qDebug()<<"qfmpVariable::set("<<result.toTypeString()<<")   vartype="<<qfmpResultTypeToString(type)<<"  data="<<toResult().toTypeString();
}




QStringList QFMathParser::getLastErrors() const
{
    return lastError;
}

int QFMathParser::getLastErrorCount()
{
    return errors;
}

void QFMathParser::resetErrors()
{
    lastError.clear();
    errors=0;
}


QString QFMathParser::getLastError() const
{
    if (lastError.isEmpty()) return QString();
    return lastError.last();
}

QString QFMathParser::getFirstError() const
{
    if (lastError.isEmpty()) return QString();
    return lastError.first();
}

QFMathParser::qfmpFunctionDescriptor::qfmpFunctionDescriptor()
{
    name="";
    type=QFMathParser::functionC;
    functionNode=NULL;
    parameterNames.clear();
}

void QFMathParser::qfmpFunctionDescriptor::clearMemory()
{
    if (type==QFMathParser::functionNode && functionNode) {
        delete functionNode;
        functionNode=NULL;
    }
}

void QFMathParser::qfmpFunctionDescriptor::evaluate(qfmpResult &r, const QVector<qfmpResult> &parameters, QFMathParser *parent) const
{
    r.isValid=true;
    if (type==QFMathParser::functionC) {
        r=function(parameters.data(), parameters.size(), parent);
        //qDebug()<<"fd.evalFunction C "<<name<<" = "<<r.toTypeString();
    } else if (type==QFMathParser::functionCRefReturn) {
        functionRR(r, parameters.data(), parameters.size(), parent);
            //qDebug()<<"fd.evalFunction C "<<name<<" = "<<r.toTypeString();
    } else if (type==QFMathParser::functionNode) {
        if (parameterNames.size()!=parameters.size()) {
            r.setInvalid();
            parent->qfmpError(QObject::tr("function '%1' takes %2 parameters, but %3 parameters were given").arg(name).arg(parameterNames.size()).arg(parameters.size()));
        } else {
            if (parent && parameterNames.size()>0) {
                parent->enterBlock();
                //qDebug()<<"enter block "<<parent->getBlockLevel();
                for (int i=0; i<parameterNames.size(); i++) {
                    parent->addVariable(parameterNames[i], parameters[i]);
                    //qDebug()<<"  adding "<<parameterNames[i]<<"="<<parameters[i].toString()<<"  levels="<<parent->getVariableLevels(parameterNames[i]);
                }
                //qDebug()<<"  eval";
                functionNode->evaluate(r);
                //qDebug()<<"leaving block "<<parent->getBlockLevel();
                parent->leaveBlock();
            } else {
                functionNode->evaluate(r);
            }
        }
    } else {
        r.setInvalid();
    }
    //qDebug()<<"fd.evalFunction "<<name<<" = "<<r.toTypeString();

}

void QFMathParser::qfmpFunctionDescriptor::evaluate(qfmpResult &r,  QVector<qfmpNode *> parameters, QFMathParser *parent) const
{

    r.setInvalid();
    if (type==QFMathParser::functionFromNode) {
        functionFN(r, parameters.data(), parameters.size(), parent);
    } else if (type==QFMathParser::functionNode) {
        if (parameterNames.size()!=parameters.size()) {
            r.setInvalid();
            parent->qfmpError(QObject::tr("function '%1' takes %2 parameters, but %3 parameters were given").arg(name).arg(parameterNames.size()).arg(parameters.size()));
        } else {
            if (parent && parameters.size()>0) {
                parent->enterBlock();
                //qDebug()<<"enter block "<<parent->getBlockLevel();
                for (int i=0; i<parameters.size(); i++) {
                    qfmpResult ri;
                    parameters[i]->evaluate(ri);
                    parent->addVariable(parameterNames[i], ri);
                    //qDebug()<<"  adding "<<parameterNames[i]<<"="<<ri.toString()<<"  levels="<<parent->getVariableLevels(parameterNames[i]);
                }
                //qDebug()<<"  eval";
                functionNode->evaluate(r);
                //qDebug()<<"leaving block "<<parent->getBlockLevel();
                parent->leaveBlock();
            } else {
                functionNode->evaluate(r);
            }
        }
    } else if (type==QFMathParser::functionC) {
        QVector<qfmpResult> ps;
        ps.resize(parameters.size());
        for (int i=0; i<parameters.size(); i++) {
            parameters[i]->evaluate(ps[i]);
        }
        r=function(ps.data(), ps.size(), parent);
    } else if (type==QFMathParser::functionCRefReturn) {
        QVector<qfmpResult> ps;
        ps.resize(parameters.size());
        for (int i=0; i<parameters.size(); i++) {
            parameters[i]->evaluate(ps[i]);
        }
        functionRR(r, ps.data(), ps.size(), parent);
    } else {
        r.setInvalid();
    }

}

QString QFMathParser::qfmpFunctionDescriptor::toDefString() const
{
    QString res="";
    if (type==QFMathParser::functionNode) res=QString("%1(%2): node").arg(name).arg(parameterNames.join(", "));
    else if (type==QFMathParser::functionC) res=QString("%1(?): C function (copy-return)").arg(name);
    else if (type==QFMathParser::functionCRefReturn) res=QString("%1(?): C function (return-by-reference)").arg(name);
    else if (type==QFMathParser::functionFromNode) res=QString("%1(?): C function of nodes (return-by-reference)").arg(name);
    else if (type==QFMathParser::functionInvalid) res=QString("%1(?): invalid function").arg(name);
    return res;
}


QFMathParser::executionEnvironment::executionEnvironment(QFMathParser *parent)
{
    //qDebug()<<"executionEnvironment constructed parent="<<parent;
    currentLevel=0;
    this->parent=parent;
}

QFMathParser::executionEnvironment::~executionEnvironment()
{
    clear();
}

void QFMathParser::executionEnvironment::setParent(QFMathParser *parent)
{
    this->parent=parent;
}



void QFMathParser::executionEnvironment::clear()
{
    clearVariables();
    clearFunctions();
    currentLevel=0;
}



void QFMathParser::executionEnvironment::addVariable(const QString &name, const QFMathParser::qfmpVariable &variable)
{
    //qDebug()<<"addVariable("<<name<<")";
    if (variables.contains(name) && variables[name].size()>0) {
        if (variables[name].last().first==currentLevel) {
            variables[name].last().second.clearMemory();
            int l=variables[name].last().first;
            variables[name].removeLast();
            variables[name].append(qMakePair(l, variable));
        } else {
            variables[name].append(qMakePair(currentLevel, variable));
        }
    } else {
        QList<QPair<int, qfmpVariable> > l;
        l.append(qMakePair(currentLevel, variable));
        variables[name]=l;
    }
    //if (name=="x") //qDebug()<<"**ADDED_VARIABLE "<<currentLevel<<": var:"<<name<<"   levels="<<variables[name].size();
}

int QFMathParser::executionEnvironment::getVariableLevels(const QString &name) const
{
    if (variables.contains(name)) {
        return variables[name].size();
    } else {
        return 0;
    }
}

void QFMathParser::executionEnvironment::setVariableDouble(const QString &name, double result)
{
    //qDebug()<<"executionEnvironment::setVariableDouble("<<name<<result<<")";
    qfmpResult r;
    r.setDouble(result);
    setVariable(name, r);
}

void QFMathParser::executionEnvironment::setVariableDoubleVec(const QString &name, const QVector<double> &result)
{
    qfmpResult r;
    r.setDoubleVec(result);
    setVariable(name, r);
}

void QFMathParser::executionEnvironment::setVariableStringVec(const QString &name, const QStringList &result)
{
    qfmpResult r;
    r.setStringVec(result);
    setVariable(name, r);
}

void QFMathParser::executionEnvironment::setVariableBoolVec(const QString &name, const QVector<bool> &result)
{
    qfmpResult r;
    r.setBoolVec(result);
    setVariable(name, r);
}

void QFMathParser::executionEnvironment::setVariableString(const QString &name, const QString &result)
{
    qfmpResult r;
    r.setString(result);
    setVariable(name, r);
}

void QFMathParser::executionEnvironment::setVariableBoolean(const QString &name, bool result)
{
    qfmpResult r;
    r.setBoolean(result);
    setVariable(name, r);

}

void QFMathParser::executionEnvironment::addVariableDouble(const QString &name, double result)
{
    //qDebug()<<"executionEnvironment::addVariableDouble("<<name<<result<<")";
    qfmpResult r;
    r.setDouble(result);
    //qDebug()<<"  adding "<<r.toTypeString();
    addVariable(name, r);
}

void QFMathParser::executionEnvironment::addVariableDoubleVec(const QString &name, const QVector<double> &result)
{
    qfmpResult r;
    r.setDoubleVec(result);
    addVariable(name, r);
}

void QFMathParser::executionEnvironment::addVariableStringVec(const QString &name, const QStringList &result)
{
    qfmpResult r;
    r.setStringVec(result);
    addVariable(name, r);
}

void QFMathParser::executionEnvironment::addVariableBoolVec(const QString &name, const QVector<bool> &result)
{
    qfmpResult r;
    r.setBoolVec(result);
    addVariable(name, r);
}

void QFMathParser::executionEnvironment::addVariableString(const QString &name, const QString &result)
{
    qfmpResult r;
    r.setString(result);
    addVariable(name, r);
}

void QFMathParser::executionEnvironment::addVariableBoolean(const QString &name, bool result)
{
    qfmpResult r;
    r.setBoolean(result);
    addVariable(name, r);
}

void QFMathParser::executionEnvironment::deleteVariable(const QString &name)
{
    if (variables.contains(name)) {
        for (int i=0; i<variables[name].size(); i++) {
            qfmpVariable v=variables[name].at(i).second;
            v.clearMemory();
        }
    }
    variables.remove(name);
}

QString QFMathParser::executionEnvironment::printVariables() const
{
    QString res="";

    if (variables.size()>0) {

        QMapIterator<QString, QList<QPair<int, QFMathParser::qfmpVariable> > > itV(variables);
        while (itV.hasNext()) {
            itV.next();

            qfmpVariable v=itV.value().last().second;
            res+="'"+itV.key()+"'"+"\t\t";
            if (v.isInternal()) res+="intern"; else res+="extern";
            res+="\t"+v.toResult().toTypeString();
            res+="\n";

        }
    }
    return res;
}

QString QFMathParser::executionEnvironment::printFunctions() const
{
    QString res="";

    if (functions.size()>0) {

        QMapIterator<QString, QList<QPair<int, QFMathParser::qfmpFunctionDescriptor> > > itV(functions);
        while (itV.hasNext()) {
            itV.next();
            if (res.size()>0) res+="\n";

            res+=itV.value().last().second.toDefString();

            /*qfmpFunctionDescriptor v=itV.value().last().second;
            if (v.type==QFMathParser::functionNode) res+=QString("%1(%2): node\n").arg(itV.key()).arg(v.parameterNames.join(", "));
            if (v.type==QFMathParser::functionC) res+=QString("%1(?): C function").arg(itV.key());*/
        }
    }
    return res;
}

QList<QPair<QString, QFMathParser::qfmpVariable> > QFMathParser::executionEnvironment::getVariables() const
{
    QList<QPair<QString, QFMathParser::qfmpVariable> > res;

    if (variables.size()>0) {
        QMapIterator<QString, QList<QPair<int, QFMathParser::qfmpVariable> > > itV(variables);
        while (itV.hasNext()) {
            itV.next();
            res.append(qMakePair(itV.key(), itV.value().last().second));
        }
    }
    return res;
}

QList<QPair<QString, QFMathParser::qfmpFunctionDescriptor> > QFMathParser::executionEnvironment::getFunctions() const
{
    QList<QPair<QString, QFMathParser::qfmpFunctionDescriptor> > res;

    if (functions.size()>0) {
        QMapIterator<QString, QList<QPair<int, QFMathParser::qfmpFunctionDescriptor> > > itV(functions);
        while (itV.hasNext()) {
            itV.next();
            res.append(qMakePair(itV.key(), itV.value().last().second));
        }
    }
    return res;
}

void QFMathParser::executionEnvironment::setFunction(const QString &name, const QFMathParser::qfmpFunctionDescriptor &function)
{
    if (functions.contains(name) && functions[name].size()>0) {
        if (functions[name].last().first==currentLevel) {
            functions[name].last().second.clearMemory();
            int l=functions[name].last().first;
            functions[name].removeLast();
            functions[name].append(qMakePair(l, function));
        } else {
            functions[name].append(qMakePair(currentLevel, function));
        }
    } else {
        QList<QPair<int, qfmpFunctionDescriptor> > l;
        l.append(qMakePair(currentLevel, function));
        functions.insert(name, l);
    }
}

void QFMathParser::executionEnvironment::addFunction(const QString &name, const QStringList &parameterNames, QFMathParser::qfmpNode *function)
{
    QFMathParser::qfmpFunctionDescriptor fd;
    fd.type=functionNode;
    fd.parameterNames=parameterNames;
    fd.functionNode=function;
    fd.name=name;
    setFunction(name, fd);
}






void QFMathParser::executionEnvironment::clearVariables()
{
    QStringList keys=variables.keys();
    for (int j=0; j<keys.size(); j++) {
        for (int i=0; i<variables[keys[j]].size(); i++) {
            variables[keys[j]].operator[](i).second.clearMemory();
        }
    }

    variables.clear();
}

void QFMathParser::executionEnvironment::clearFunctions()
{
    functions.clear();
}

QFMathParser::qfmpFunctionAssignNode::~qfmpFunctionAssignNode()
{
    if (child) delete child;
}

QFMathParser::qfmpFunctionAssignNode::qfmpFunctionAssignNode(QString function, QStringList parameterNames, QFMathParser::qfmpNode *c, QFMathParser *p, QFMathParser::qfmpNode *par):
    qfmpNode(p, par)

{
    child=c;
    if (child) child->setParent(this);
    setParser(p);
    setParent(par);
    this->function=function;
    this->parameterNames=parameterNames;
}

void QFMathParser::qfmpFunctionAssignNode::evaluate(qfmpResult &res)
{
    getParser()->addFunction(function, parameterNames, child->copy(NULL));
    res.setVoid();
}

QFMathParser::qfmpNode *QFMathParser::qfmpFunctionAssignNode::copy(QFMathParser::qfmpNode *par)
{
    if (child) return new QFMathParser::qfmpFunctionAssignNode(function, parameterNames, child->copy(NULL), getParser(), par);
    else return new QFMathParser::qfmpFunctionAssignNode(function, parameterNames, NULL, getParser(), par);
}

bool QFMathParser::qfmpFunctionAssignNode::createByteCode(QFMathParser::ByteCodeProgram &program, QFMathParser::ByteCodeEnvironment *environment)
{
    Q_UNUSED(program);
    if (child) {
        environment->functionDefs[function]=qMakePair(parameterNames, child);
        return true;
    }
    return false;
}

QString QFMathParser::qfmpFunctionAssignNode::print() const
{
    if (child) return QString("%1( %2 ) = %3").arg(function).arg(parameterNames.join(", ")).arg(child->print());
    else return QString("%1( %2 ) = %3").arg(function).arg(parameterNames.join(", ")).arg(QString());
}

QString QFMathParser::qfmpFunctionAssignNode::printTree(int level) const
{
    if (child) return QString(2*level, QLatin1Char(' '))+QString("FunctionAssigneNode '%2(%3)'\n%1").arg(child->printTree(level+1)).arg(function).arg(parameterNames.join(", "));
    else return QString(2*level, QLatin1Char(' '))+QString("FunctionAssigneNode '%1(%2)'").arg(function).arg(parameterNames.join(", "));
}

qfmpResult QFMathParser::qfmpConstantNode::evaluate()
{
    return data;
}

void QFMathParser::qfmpConstantNode::evaluate(qfmpResult &result)
{
    result=data;
}

QFMathParser::qfmpNode *QFMathParser::qfmpConstantNode::copy(QFMathParser::qfmpNode *par)
{
    return new QFMathParser::qfmpConstantNode(data, getParser(), par);
}

bool QFMathParser::qfmpConstantNode::createByteCode(QFMathParser::ByteCodeProgram &program, ByteCodeEnvironment *environment)
{
    Q_UNUSED(environment);
    if (data.type==qfmpDouble) {
        program.append(QFMathParser::ByteCodeInstruction(bcPush, data.num));
    } else if (data.type==qfmpBool) {
        program.append(QFMathParser::ByteCodeInstruction(bcPush, (data.boolean)?1.0:0.0));
    } else {
        getParser()->qfmpError(QObject::tr("only numbers and booleans allowed in bytecoded expressions!"));
        return false;
    }
    return true;
}

QString QFMathParser::qfmpConstantNode::print() const
{
    return data.toString();
}

QString QFMathParser::qfmpConstantNode::printTree(int level) const
{
    return QString(2*level, QLatin1Char(' '))+QString("ConstantNode %1").arg(data.toTypeString());
}

qfmpResult QFMathParser::qfmpInvalidNode::evaluate()
{
    return qfmpResult::invalidResult();
}

void QFMathParser::qfmpInvalidNode::evaluate(qfmpResult &result)
{
    result.setInvalid();
}

QFMathParser::qfmpNode *QFMathParser::qfmpInvalidNode::copy(QFMathParser::qfmpNode *par)
{
    return new QFMathParser::qfmpInvalidNode(getParser(), par);
}

QString QFMathParser::qfmpInvalidNode::print() const
{
    return QString("INVALID");
}

QString QFMathParser::qfmpInvalidNode::printTree(int level) const
{
    return QString(2*level, QLatin1Char(' '))+QString("InvalidNode");
}

void QFMathParser::qfmpVectorList::evaluate(qfmpResult &res)
{
    qfmpResult r;
    res.setDoubleVec(0,0);
    if (list.size()>0) {
        for (int i=0; i<list.size(); i++) {
            r.setInvalid();
            list[i]->evaluate(r);
            if (i==0) {
                if (r.type==qfmpString||(r.type==qfmpStringVector)) {
                    res.setStringVec(0, QString());
                } else if (r.type==qfmpBool||(r.type==qfmpBoolVector)) {
                    res.setBoolVec(0, false);
                }
            }
            if (res.type==qfmpDoubleVector) {
                if (r.isValid && r.type==qfmpDouble) {
                    res.numVec.append(r.num);
                } else if (r.isValid && r.type==qfmpDoubleVector) {
                    res.numVec+=r.numVec;
                } else {
                    res.setInvalid();
                    if (getParser()) getParser()->qfmpError(QObject::tr("error in vector construct [Val1, Val2, ...]: item %1 has the wrong type (not number or number vector, but %2!)").arg(i).arg(r.typeName()));
                    break;
                }
            } else if (res.type==qfmpStringVector) {
                if (r.isValid && r.type==qfmpString) {
                    res.strVec.append(r.str);
                } else if (r.isValid && r.type==qfmpStringVector) {
                    res.strVec.append(r.strVec);
                } else {
                    res.setInvalid();
                    if (getParser()) getParser()->qfmpError(QObject::tr("error in vector construct [Val1, Val2, ...]: item %1 has the wrong type (not string or string vector, but %2!)").arg(i).arg(r.typeName()));
                    break;
                }
            } else if (res.type==qfmpBoolVector) {
                if (r.isValid && r.type==qfmpBool) {
                    res.boolVec.append(r.boolean);
                } else if (r.isValid && r.type==qfmpBoolVector) {
                    res.boolVec+=r.boolVec;
                } else {
                    res.setInvalid();
                    if (getParser()) getParser()->qfmpError(QObject::tr("error in vector construct [Val1, Val2, ...]: item %1 has the wrong type (not string or string vector, but %2!)").arg(i).arg(r.typeName()));
                    break;
                }
            }
        }
    }
}

QFMathParser::qfmpNode *QFMathParser::qfmpVectorList::copy(QFMathParser::qfmpNode *par)
{
    QFMathParser::qfmpNodeList* n= new QFMathParser::qfmpVectorList(getParser(), par);
    if (list.size()>0) {
        for (int i=0; i<list.size(); i++) {
            n->add(list[i]->copy(n));
        }
    }

    n->setParent(par);
    return n;

}

bool QFMathParser::qfmpVectorList::createByteCode(QFMathParser::ByteCodeProgram &program, QFMathParser::ByteCodeEnvironment *environment)
{
    Q_UNUSED(program);
    Q_UNUSED(environment);
    if (getParser()) getParser()->qfmpError(QObject::tr("no vector constructs in byte-code allowed"));
    return false;
}

QString QFMathParser::qfmpVectorList::print() const
{
    QStringList sl;
    for (int i=0; i<list.size(); i++) {
        sl<<list[i]->print();
    }
    return QString("[ %1 ]").arg(sl.join(", "));
}

QString QFMathParser::qfmpVectorList::printTree(int level) const
{
    QStringList sl;
    if (list.size()>0) {
        for (int i=0; i<list.size(); i++) {
            sl<<list[i]->printTree(level+1);
        }
    }
    return QString(2*level, QLatin1Char(' '))+QString("VectorListNode\n%1").arg(sl.join("\n"));

}

QFMathParser::qfmpNode *QFMathParser::qfmpVectorConstructionNode::copy(QFMathParser::qfmpNode *par)
{
    if (step) {
        return new qfmpVectorConstructionNode(start->copy(NULL), end->copy(NULL), step->copy(NULL), getParser(), par);
    } else {
        return new qfmpVectorConstructionNode(start->copy(NULL), end->copy(NULL), NULL, getParser(), par);
    }
}

QString QFMathParser::qfmpVectorConstructionNode::print() const
{
    if (step)  return QString("(%1):(%3):(%2)").arg(start->print()).arg(end->print()).arg(step->print());
    else  return QString("(%1):(%2)").arg(start->print()).arg(end->print());
}

QString QFMathParser::qfmpVectorConstructionNode::printTree(int level) const
{
    if (step)  return QString(2*level, QLatin1Char(' '))+QString("VectorConstructionNode:\n%1\n%3\n%2").arg(start->printTree(level+1)).arg(end->printTree(level+1)).arg(step->printTree(level+1));
    else  return QString(2*level, QLatin1Char(' '))+QString("VectorConstructionNode:\n%1\n%2").arg(start->printTree(level+1)).arg(end->printTree(level+1));

}

QFMathParser::qfmpVectorConstructionNode::qfmpVectorConstructionNode(QFMathParser::qfmpNode *start, QFMathParser::qfmpNode *end, QFMathParser::qfmpNode *step, QFMathParser *p, QFMathParser::qfmpNode *par):
    qfmpNode(p, par)

{
    this->start=start;
    this->end=end;
    this->step=step;
    setParser(p);
    setParent(par);
}

QFMathParser::qfmpVectorConstructionNode::~qfmpVectorConstructionNode()
{
    if (start) delete start;
    if (step) delete step;
    if (end) delete end;
}



void QFMathParser::qfmpVectorConstructionNode::evaluate(qfmpResult &res)
{
    if (!start) {
        if (getParser()) getParser()->qfmpError(QObject::tr("error in vector construct 'start[:delta]:end' INTERNAL ERROR: start does not exist"));
        res.setInvalid();
        return;
    }
    if (!end) {
        if (getParser()) getParser()->qfmpError(QObject::tr("error in vector construct 'start[:delta]:end' INTERNAL ERROR: end does not exist"));
        res.setInvalid();
        return;
    }
     res.setDoubleVec();
     qfmpResult rstart;
     start->evaluate(rstart);
     qfmpResult rend;
     end->evaluate(rend);
     if (rstart.type!=qfmpDouble || rend.type!=qfmpDouble) {
         if (getParser()) getParser()->qfmpError(QObject::tr("error in vector construct 'start[:delta]:end'' start, delta and end have to be numbers (start=%1, end=%2)").arg(rstart.toTypeString()).arg(rend.toTypeString()));
         res.setInvalid();
         return;
     }
     double s=rstart.num;
     double e=rend.num;
     double d=1;
     if (step) {
         qfmpResult rstep;
         step->evaluate(rstep);
         if (rstep.type!=qfmpDouble) {
             if (getParser()) getParser()->qfmpError(QObject::tr("error in vector construct 'start:delta:end'' delta has to be numbers (start=%1, step=%2, end=%3)").arg(rstart.toTypeString()).arg(rstep.toTypeString()).arg(rend.toTypeString()));
             res.setInvalid();
             return;
         }
         d=rstep.num;
     }
     res.isValid=true;
     if (d>0) {
         for (double t=s; t<=e; t=t+d) {
             res.numVec<<t;
         }
     } else if (d<0) {
         for (double t=s; t>=e; t=t+d) {
             res.numVec<<t;
         }
     }
}

QFMathParser::qfmpCasesNode::qfmpCasesNode(QFMathParser *p, QFMathParser::qfmpNode *par):
    qfmpNode(p, par)

{
    setParent(par);
    setParser(p);
    elseNode=NULL;
}

QFMathParser::qfmpCasesNode::~qfmpCasesNode()
{
    if (elseNode) delete elseNode;
    for (int i=0; i<casesNodes.size(); i++) {
        if (casesNodes[i].first) delete casesNodes[i].first;
        if (casesNodes[i].second) delete casesNodes[i].second;
    }
    casesNodes.clear();
}

void QFMathParser::qfmpCasesNode::setElse(QFMathParser::qfmpNode *elseNode)
{
    this->elseNode=elseNode;
}

void QFMathParser::qfmpCasesNode::addCase(QFMathParser::qfmpNode *decision, QFMathParser::qfmpNode *value)
{
    casesNodes.append(qMakePair(decision, value));
}



void QFMathParser::qfmpCasesNode::evaluate(qfmpResult &r)
{
    r.isValid=false;
    qfmpResult d;
    for (int i=0; i<casesNodes.size(); i++) {
        casesNodes[i].first->evaluate(d);
        if (d.type==qfmpBool) {
            if (d.boolean) {
                casesNodes[i].second->evaluate(r);
                return;
            }
        } else {
            if (getParser()) getParser()->qfmpError(QObject::tr("decision statement %1 does not have type boolean").arg(i+1));
            r.setInvalid();
            return;
        }
    }
    if (elseNode) elseNode->evaluate(r);
}

QFMathParser::qfmpNode *QFMathParser::qfmpCasesNode::copy(QFMathParser::qfmpNode *par)
{
    QFMathParser::qfmpCasesNode* res=new QFMathParser::qfmpCasesNode(getParser(), par);
    for (int i=0; i<casesNodes.size(); i++) {
        QFMathParser::qfmpNode* n1=NULL;
        QFMathParser::qfmpNode* n2=NULL;
        if (casesNodes[i].first) n1=casesNodes[i].first->copy();
        if (casesNodes[i].second) n2=casesNodes[i].second->copy();
        res->addCase(n1, n2);
    }
    if (elseNode) res->setElse(elseNode->copy(res));
    return res;
}

bool QFMathParser::qfmpCasesNode::createByteCode(QFMathParser::ByteCodeProgram &program, QFMathParser::ByteCodeEnvironment *environment)
{
    /*
        if (a1) return va1;
        else if (a2) return va2;
        else if (a3) return va3;
        else return vae;


          EVAL_A1
          LOGICNOT
          JMPCOND label1    # jump if (!a1)
          EVAL VA1
          JMP END
        label1:     # label 1
          EVAL_A2
          LOGICNOT
          JMPCOND label2    # jump if (!a2)
          EVAL VA2
          JMP END
        label2:     # label 2
          EVAL_A3
          LOGICNOT
          JMPCOND labelelse    # jump if (!a2)
          EVAL VA3
          JMP END
        labelelse:  # label 0
          EVAL VAE
        end:        # label -1
          NOP

      */
    bool ok=true;
    QList<int> caseadresses;
    QMap<int, int> jumpadresses;
    for (int i=0; (ok && i<casesNodes.size()); i++) {
        ok=ok&&casesNodes[i].first->createByteCode(program, environment);
        program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcLogicNot));
        if (ok) {
            caseadresses<<program.size();
            program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcJumpCondRel, i+1));
            ok=ok&&casesNodes[i].second->createByteCode(program, environment);
            caseadresses<<program.size();
            program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcJumpRel, -1));
            jumpadresses[i+1]=program.size();
        }
    }
    if (elseNode && ok) {
        jumpadresses[0]=program.size();
        ok=ok&&elseNode->createByteCode(program, environment);
    }
    jumpadresses[-1]=program.size();
    program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcNOP));
    for (int i=0; i<caseadresses.size(); i++) {
        int oldjmp=program[caseadresses[i]].intpar;
       // //qDebug()<<"   convert JMP "<<oldjmp<<"  ==>  JMP "<<jumpadresses[oldjmp]-caseadresses[i]<<" (new abs: "<<jumpadresses[oldjmp]<<")    @ "<<caseadresses[i];
        program[caseadresses[i]].intpar=jumpadresses[oldjmp]-caseadresses[i];
    }
    return ok;
}

QString QFMathParser::qfmpCasesNode::print() const
{
    QStringList sl;
    for (int i=0; i<casesNodes.size(); i++) {
        sl<<casesNodes[i].first->print();
        sl<<casesNodes[i].second->print();
    }
    if (elseNode) sl<<elseNode->print();
    return QString("cases(%1)").arg(sl.join(", "));
}

QString QFMathParser::qfmpCasesNode::printTree(int level) const
{
    QStringList sl;
    for (int i=0; i<casesNodes.size(); i++) {
        sl<<casesNodes[i].first->printTree(level+1);
        sl<<casesNodes[i].second->printTree(level+1);
    }
    if (elseNode) sl<<elseNode->printTree(level+1);
    return QString(2*level, QLatin1Char(' '))+QString("CasesNode\n%1").arg(sl.join("\n"));

}

QFMathParser::qfmpVectorOperationNode::qfmpVectorOperationNode(const QString &operationName, const QString &variableName, QFMathParser::qfmpNode *items, QFMathParser::qfmpNode *expression, QFMathParser *p, QFMathParser::qfmpNode *par, qfmpNode *defaultValue):
    qfmpNode(p, par)

{
    this->operationName=operationName.toLower();
    this->variableName=variableName;
    this->expression=expression;
    this->items=items;
    this->start=NULL;
    this->end=NULL;
    this->delta=NULL;
    this->defaultValue=defaultValue;
}

QFMathParser::qfmpVectorOperationNode::qfmpVectorOperationNode(const QString &operationName, const QString &variableName, QFMathParser::qfmpNode *start, QFMathParser::qfmpNode *end, QFMathParser::qfmpNode *delta, QFMathParser::qfmpNode *expression, QFMathParser *p, QFMathParser::qfmpNode *par, QFMathParser::qfmpNode *defaultValue):
    qfmpNode(p, par)
{
    this->operationName=operationName.toLower();
    this->variableName=variableName;
    this->expression=expression;
    this->defaultValue=defaultValue;
    this->items=NULL;
    this->start=start;
    this->end=end;
    this->delta=delta;
}

QFMathParser::qfmpVectorOperationNode::~qfmpVectorOperationNode()
{
    if (defaultValue) delete defaultValue;
    if (expression) delete expression;
    if (items) delete items;
    if (start) delete start;
    if (end) delete end;
    if (delta) delete delta;
}


void QFMathParser::qfmpVectorOperationNode::evaluate(qfmpResult &r)
{
     r.isValid=false;
     QStringList strVec, itemValsS;
     QVector<bool> boolVec, itemValsB;
     QVector<double> numVec, itemVals;
     qfmpResultType resType=qfmpDouble;
     bool evalItems=true;
     bool ok=false;
     bool isString=false;
     bool isBool=false;

     int cnt=0;
     if (!ok) {
         if (items) {
             qfmpResult ri;
             if (evalItems) {
                 items->evaluate(ri);
                 if (ri.type==qfmpDouble) {itemVals<<ri.num; cnt=itemVals.size(); }
                 else if (ri.type==qfmpDoubleVector) {itemVals=ri.numVec; cnt=itemVals.size(); }
                 else if (ri.type==qfmpString) {itemValsS<<ri.str; isString=true; cnt=itemValsS.size(); }
                 else if (ri.type==qfmpStringVector) {itemValsS=ri.strVec; isString=true; cnt=itemValsS.size();}
                 else if (ri.type==qfmpBool) {itemValsB<<ri.boolean; isBool=true; cnt=itemValsB.size();}
                 else if (ri.type==qfmpBoolVector) {itemValsB=ri.boolVec; isBool=true; cnt=itemValsB.size();}
                 else {
                     if (getParser()) getParser()->qfmpError(QObject::tr("%1(NAME, ITEMS, EXPRESSION) expects a list of numbers, booleans or strings as ITEMS").arg(operationName));
                     r.setInvalid();
                     return;
                 }
             }
         } else {
             qfmpResult rs;
             start->evaluate(rs);
             qfmpResult re;
             end->evaluate(re);
             double s=rs.num;
             double e=re.num;
             if (rs.type!=qfmpDouble) {
                 if (getParser()) getParser()->qfmpError(QObject::tr("%1(NAME, START, DELTA, END, EXPRESSION) expects a number for START").arg(operationName));
                 r.setInvalid();
                 return;
             }
             if (re.type!=qfmpDouble) {
                 if (getParser()) getParser()->qfmpError(QObject::tr("%1(NAME, START, DELTA, END, EXPRESSION) expects a number for END").arg(operationName));
                 r.setInvalid();
                 return;
             }
             double d=1;
             if (delta) {
                 qfmpResult rd;
                 delta->evaluate(rd);
                 if (rd.type==qfmpDouble) {
                     d=rd.num;
                 } else {
                     if (getParser()) getParser()->qfmpError(QObject::tr("%1(NAME, START, DELTA, END, EXPRESSION) expects a number for DELTA").arg(operationName));
                     r.setInvalid();
                     return;
                 }

             }
             //qDebug()<<"for: t="<<s<<"; t<="<<e<<"; t=t+"<<d;
             if (d>0) {
                 for (double t=s; t<=e; t=t+d) {
                     itemVals<<t;
                 }
             } else if (d<0) {
                 for (double t=s; t>=e; t=t+d) {
                     itemVals<<t;
                 }
             }
             cnt=itemVals.size();
         }
         if (cnt<=0) {
             if (operationName=="for"||operationName=="savefor") {
                 r.setDoubleVec(QVector<double>());
                 return;
             } else if (operationName=="defaultfor") {
                 if (defaultValue) {
                     defaultValue->evaluate(r);
                 } else {
                     r.setDoubleVec(QVector<double>());
                 }
                 return;
             } else {
                 r.setDouble(0.0);
                 return;
             }
         }
         qfmpResult defr;

         getParser()->enterBlock();
         qfmpResult thisr;
         bool isFilterFor=((operationName=="filterfor") || (operationName=="savefilterfor"));
         for (int i=0; i<cnt; i++) {
             if (isBool) {
                 getParser()->addVariable(variableName, qfmpResult(itemValsB[i]));
             } else if (isString) {
                 getParser()->addVariable(variableName, qfmpResult(itemValsS[i]));
             } else {
                 getParser()->addVariable(variableName, qfmpResult(itemVals[i]));
             }
             expression->evaluate(thisr);
             if (isFilterFor) {
                 if (i==0) {
                     if (isBool) {
                         resType=qfmpBoolVector;
                         boolVec.clear();
                     } else if (isString) {
                         resType=qfmpStringVector;
                         strVec.clear();
                     } else {
                         resType=qfmpDoubleVector;
                         numVec.clear();
                     }
                 }
                 if (thisr.type==qfmpBool) {
                     if (thisr.boolean) {
                         if (isBool) {
                             boolVec.append(itemValsB[i]);
                         } else if (isString) {
                             strVec.append(itemValsS[i]);
                         } else {
                             numVec.append(itemVals[i]);
                         }
                     }
                 } else {
                     if (operationName!="savefilterfor") {
                         if (getParser()) getParser()->qfmpError(QObject::tr("EXPRESSION in %1(NAME, ..., EXPRESSION) has to evaluate to a boolean, but found %2 in iteration %3").arg(operationName).arg(resultTypeToString(thisr.type)).arg(i+1));
                         r.setInvalid();
                     }
                     return;
                 }
             } else {
                 if (i==0) {
                     switch(thisr.type) {
                         case qfmpBool:
                         case qfmpBoolVector:
                             resType=qfmpBoolVector;
                             break;
                         case qfmpString:
                         case qfmpStringVector:
                             resType=qfmpStringVector;
                             break;
                         case qfmpDouble:
                         case qfmpDoubleVector:
                             resType=qfmpDoubleVector;
                             break;
                         default:
                             resType=qfmpDoubleVector;
                             if (operationName=="defaultfor") {
                                 defr.setInvalid();
                                 if (defaultValue) defaultValue->evaluate(defr);
                                 resType=defr.type;
                             }
                             break;
                     }

                     //resType=thisr.type;
                 } else if ((resType==qfmpBoolVector && thisr.type!=qfmpBool && thisr.type!=qfmpBoolVector)
                            || (resType==qfmpStringVector && thisr.type!=qfmpString && thisr.type!=qfmpStringVector)
                            || (resType==qfmpDoubleVector && thisr.type!=qfmpDouble && thisr.type!=qfmpDoubleVector)) {
                     if (operationName=="defaultfor") {
                         defr.setInvalid();
                         if (defaultValue) defaultValue->evaluate(defr);
                         if (!defr.isValid) {
                             if (getParser()) getParser()->qfmpError(QObject::tr("DEFAULT in %1(NAME, ..., EXPRESSION, DEFAULT)evaluated to invalid!").arg(operationName));
                             r.setInvalid();
                             return;
                         }
                         if ((resType==qfmpBoolVector && (defr.type==qfmpBool || defr.type==qfmpBoolVector))
                                                     || (resType==qfmpStringVector && (defr.type==qfmpString || defr.type==qfmpStringVector))
                                                     || (resType==qfmpDoubleVector && (defr.type==qfmpDouble || defr.type==qfmpDoubleVector))) {
                             thisr=defr;
                         } else {
                             if (getParser()) getParser()->qfmpError(QObject::tr("EXPRESSION in %1(NAME, ..., EXPRESSION, DEFAULT): DEFAULT has to evaluate to a compatible type in every iteration: expected compaitble with %3 (single/vector), but found %2 in iteration %4").arg(operationName).arg(defr.toTypeString()).arg(resultTypeToString(resType)).arg(i+1));
                             r.setInvalid();
                             return;
                         }
                     } else {
                         if (getParser()) getParser()->qfmpError(QObject::tr("EXPRESSION in %1(NAME, ..., EXPRESSION) has to evaluate to a compatible type in every iteration: expected compaitble with %3 (single/vector), but found %2 in iteration %4").arg(operationName).arg(defr.toTypeString()).arg(resultTypeToString(resType)).arg(i+1));
                         r.setInvalid();
                         return;
                     }
                 }
                 switch(resType) {
                     case qfmpDoubleVector:
                         if (thisr.type==qfmpDouble) numVec<<thisr.num;
                         else if (thisr.type==qfmpDoubleVector) numVec<<thisr.numVec;
                         break;
                     case qfmpStringVector:
                         if (operationName=="sum" || operationName=="for" || operationName=="savefor" || operationName=="defaultfor") {
                             if (thisr.type==qfmpString) strVec<<thisr.str;
                             else if (thisr.type==qfmpStringVector) strVec<<thisr.strVec;
                         } else  if (operationName!="savefor")  {
                             if (getParser()) getParser()->qfmpError(QObject::tr("EXPRESSION in %1(NAME, ..., EXPRESSION) has to evaluate to number: but found %2 in iteration %3").arg(operationName).arg(resultTypeToString(resType)).arg(i+1));
                             r.setInvalid();
                             return;
                         }
                         break;
                     case qfmpBoolVector:
                         if (operationName=="for" || operationName=="savefor" || operationName=="defaultfor") {
                             if (thisr.type==qfmpBool) boolVec<<thisr.boolean;
                             else if (thisr.type==qfmpBoolVector) boolVec<<thisr.boolVec;
                         } else  if (operationName!="savefor")  {
                             if (getParser()) getParser()->qfmpError(QObject::tr("EXPRESSION in %1(NAME, ..., EXPRESSION) has to evaluate to number: but found %2 in iteration %3").arg(operationName).arg(resultTypeToString(resType)).arg(i+1));
                             r.setInvalid();
                             return;
                         }
                         break;
                     default:
                         if (operationName!="savefor") {
                             if (getParser()) getParser()->qfmpError(QObject::tr("EXPRESSION in %1(NAME, ..., EXPRESSION) has to evaluate to string or number: but found %2 in iteration %3").arg(operationName).arg(resultTypeToString(resType)).arg(i+1));
                             r.setInvalid();
                         }
                         return;
                 }
             }
         }
         getParser()->leaveBlock();

         r.isValid=true;
         if (operationName=="for"||operationName=="savefor"||operationName=="filterfor"||operationName=="savefilterfor"||operationName=="defaultfor") {
             if (resType==qfmpDoubleVector) r.setDoubleVec(numVec);
             else if (resType==qfmpStringVector) r.setStringVec(strVec);
             else if (resType==qfmpBoolVector) r.setBoolVec(boolVec);
             else if (operationName=="savefor" || operationName=="savefilterfor") {
                 r.setDoubleVec(QVector<double>());
             } else r.setInvalid();

         } else if (operationName=="sum") {
             if (resType==qfmpDoubleVector) r.setDouble(qfstatisticsSum(numVec));
             else if (resType==qfmpStringVector) r.setString(strVec.join(""));
             else r.setInvalid();
         } else if (operationName=="prod") {
             if (resType==qfmpDoubleVector) r.setDouble(qfstatisticsProd(numVec));
             else r.setInvalid();
         }
     }
}

bool QFMathParser::qfmpVectorOperationNode::createByteCode(QFMathParser::ByteCodeProgram &program, QFMathParser::ByteCodeEnvironment *environment)
{
    /*

  # reserve variable __operationName__stop
  # reserve variable __operationName__delta
  # reserve variable __operationName__runvar
      PUSH 0
      EVAL END
      HEAPWRITE __operationName__end           # __operationName__stop = END
  #if (delta exists)
      EVAL DELTA
  #else
      PUSH 1
  #endif
      HEAPWRITE __operationName__delta         # __operationName__delta = DELTA
      EVAL START
      HEAPWRITE __operationName__runvar        # __operationName__runvar = START
  start:
      HEAPREAD __operationName__stop           # if (__operationName__runvar>__operationName__stop) jump end
      HEAPREAD __operationName__runvar
      CMPLESSEREQUAL
      LOGICNOT
      JUMPCONDREL end

      EVAL EXPRESSION

      HEAPREAD __operationName__runvar         # __operationName__runvar=__operationName__delta+DELTA
      HEAPREAD __operationName__delta
      ADD
      HEAPWRITE __operationName__runvar
      JUMPREL start
  end:
      NOP
      */
    bool ok=expression&&start&&end;
    if (operationName=="sum" || operationName=="prod") {
        if (ok) {
            if (operationName=="prod") {
                program.append(QFMathParser::ByteCodeInstruction( QFMathParser::bcPush, 1.0));
            } else {
                program.append(QFMathParser::ByteCodeInstruction( QFMathParser::bcPush, 0.0));
            }
            QString vnStop=QString("__%1__stop").arg(operationName);
            QString vnDelta=QString("__%1__delta").arg(operationName);
            QString vnRun=variableName;//QString("__%1__runvar").arg(operationName);
            int stopvar=environment->pushVar(vnStop);
            int deltavar=environment->pushVar(vnDelta);
            int runvar=environment->pushVar(vnRun);

            ok=ok&&end->createByteCode(program, environment);
            program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcHeapWrite, stopvar));
            if (delta) {
                ok=ok&&delta->createByteCode(program, environment);
            } else {
                program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcPush, 1.0));
            }
            program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcHeapWrite, deltavar));
            ok=ok&&start->createByteCode(program, environment);
            program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcHeapWrite, runvar));
            int expr_adress=program.size();

            program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcHeapRead, stopvar));
            program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcHeapRead, runvar));
            program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcCmpLesserEqual));
            program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcLogicNot));
            int jmpAdress=program.size();
            program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcJumpCondRel, 0));

            ok=ok&&expression->createByteCode(program, environment);
            if (operationName=="prod") {
                program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcMul));
            } else {
                program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcAdd));
            }

            program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcHeapRead, deltavar));
            program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcHeapRead, runvar));
            program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcAdd));
            program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcHeapWrite, runvar));
            program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcBJumpRel, program.size()-expr_adress));
            environment->popVar(vnRun);
            environment->popVar(vnDelta);
            environment->popVar(vnStop);
            int end_adress=program.size();
            program[jmpAdress].intpar=end_adress-jmpAdress;
            program.append(QFMathParser::ByteCodeInstruction(QFMathParser::bcNOP));
            return ok;
        } else {
            if (getParser()) getParser()->qfmpError(QObject::tr("%1(VAR, ITEMS, EXPRESSION) not usable in byte-code").arg(operationName));
            return false;
        }
    } else {
        if (getParser()) getParser()->qfmpError(QObject::tr("%1(...) not usable in byte-code").arg(operationName));
        return false;
    }

}

QFMathParser::qfmpNode *QFMathParser::qfmpVectorOperationNode::copy(QFMathParser::qfmpNode *par)
{
    if (items) {
        return new qfmpVectorOperationNode(operationName, variableName, items->copy(), expression->copy(), getParser(), par);
    } else {
        qfmpNode* def=NULL;
        if (defaultValue) def=defaultValue->copy();
        if (delta) return new qfmpVectorOperationNode(operationName, variableName, start->copy(), end->copy(), delta->copy(), expression->copy(), getParser(), par, def);
        else return new qfmpVectorOperationNode(operationName, variableName, start->copy(), end->copy(), NULL, expression->copy(), getParser(), par, def);
    }
}

QString QFMathParser::qfmpVectorOperationNode::print() const
{
    QStringList sl;
    QString def="";
    if (items) sl<<items->print();
    else {
        if (delta) {
            sl<<start->print();
            sl<<delta->print();
            sl<<end->print();
        } else {
            sl<<start->print();
            sl<<end->print();
        }
    }
    if (defaultValue) {
        def=QString(", ")+defaultValue->print();
    }
    return QString("%2(%3, %1, %4%5)").arg(sl.join(", ")).arg(operationName).arg(variableName).arg(expression->print()).arg(def);
}

QString QFMathParser::qfmpVectorOperationNode::printTree(int level) const
{
    QStringList sl;
    if (items) sl<<items->printTree(level+1);
    else {
        if (delta) {
            sl<<start->printTree(level+1);
            sl<<delta->printTree(level+1);
            sl<<end->printTree(level+1);
        } else {
            sl<<start->printTree(level+1);
            sl<<end->printTree(level+1);
        }
        if (defaultValue) {
            sl<<defaultValue->printTree(level+1);
        }
    }
    return QString(2*level, QLatin1Char(' '))+QString("VectorOperationNode %2, %3\n%1\n%4)").arg(sl.join("\n")).arg(operationName).arg(variableName).arg(expression->printTree(level+1));


}

QFMathParser::qfmpVectorElementAssignNode::~qfmpVectorElementAssignNode()
{
    if (index) delete index;
}

QFMathParser::qfmpVectorElementAssignNode::qfmpVectorElementAssignNode(QString var, QFMathParser::qfmpNode *index, QFMathParser::qfmpNode *expression, QFMathParser *p, QFMathParser::qfmpNode *par):
    qfmpVariableAssignNode(var, expression, p, par)
{
    this->index=index;
}

void QFMathParser::qfmpVectorElementAssignNode::evaluate(qfmpResult &res)
{

     qfmpResult exp, idx, var;
     res.isValid=true;
     if (child) child->evaluate(exp);
     if (index) index->evaluate(idx);

     getParser()->getVariable(var, variable);
     //qDebug()<<"got variable "<<variable<<var.toTypeString();
     if (idx.convertsToIntVector()) {
         QVector<int> ii=idx.asIntVector();
         if (ii.size()==0) {
             getParser()->qfmpError(QObject::tr("vector element assignment needs non-empty number vector as index, but index is %1").arg(idx.toTypeString()));
             res.setInvalid();
             return;
         }
         if (var.type==qfmpDoubleVector && exp.convertsToVector()) {
             QVector<double> dat=exp.asVector();
             if (ii.size()==1 && dat.size()==1) {
                 if (ii[0]>=0 && ii[0]<var.numVec.size()) {
                     var.numVec[ii[0]]=dat[0];
                     getParser()->setVariable(variable, var);
                     res.setDouble(dat[0]);
                 } else {
                     getParser()->qfmpError(QObject::tr("OUT OF RANGE: trying to assign to element %1, but vector %2 has only %3 elements").arg(ii[0]).arg(variable).arg(var.numVec.size()));
                     res.setInvalid();
                     return;
                 }
                 getParser()->setVariable(variable, var);
             } else if (ii.size()>1 && dat.size()==1) {
                 res.type=qfmpDoubleVector;
                 res.numVec.clear();
                 const double t=dat[0];
                 for (int i=0; i<ii.size(); i++) {
                     if (ii[i]>=0 && ii[i]<var.numVec.size()) {
                         var.numVec[ii[i]]=t;
                         res.numVec<<t;
                     } else {
                         getParser()->qfmpError(QObject::tr("OUT OF RANGE: trying to assign to element %1, but vector %2 has only %3 elements").arg(ii[i]).arg(variable).arg(var.numVec.size()));
                         res.setInvalid();
                         return;
                     }
                 }
                 getParser()->setVariable(variable, var);
             } else if (ii.size()>1 && dat.size()==ii.size()) {
                 res.type=qfmpDoubleVector;
                 res.numVec.clear();
                 for (int i=0; i<ii.size(); i++) {
                     if (ii[i]>=0 && ii[i]<var.numVec.size()) {
                         const double t=dat[i];
                         var.numVec[ii[i]]=t;
                         res.numVec<<t;
                     } else {
                         getParser()->qfmpError(QObject::tr("OUT OF RANGE: trying to assign to element %1, but vector %2 has only %3 elements").arg(ii[i]).arg(variable).arg(var.numVec.size()));
                         res.setInvalid();
                         return;
                     }
                 }
                 getParser()->setVariable(variable, var);
             } else {
                 getParser()->qfmpError(QObject::tr("can only assign x[N elements]=(1 element), x[N elements]=(N elements), x[1 elements]=(1 elements)"));
                 res.setInvalid();
                 return;
             }

         } else if ((exp.type==qfmpString||exp.type==qfmpStringVector) && var.type==qfmpStringVector) {
             QStringList dat=exp.asStrVector();
             if (ii.size()==1 && dat.size()==1) {
                 if (ii[0]>=0 && ii[0]<var.strVec.size()) {
                     var.strVec[ii[0]]=dat[0];
                     getParser()->setVariable(variable, var);
                     res=qfmpResult(dat[0]);
                 } else {
                     getParser()->qfmpError(QObject::tr("OUT OF RANGE: trying to assign to element %1, but vector %2 has only %3 elements").arg(ii[0]).arg(variable).arg(var.strVec.size()));
                     res.setInvalid();
                     return;
                 }
                 getParser()->setVariable(variable, var);
             } else if (ii.size()>1 && dat.size()==1) {
                 res.type=qfmpStringVector;
                 res.strVec.clear();
                 const QString t=dat[0];
                 for (int i=0; i<ii.size(); i++) {
                     if (ii[i]>=0 && ii[i]<var.strVec.size()) {
                         var.strVec[ii[i]]=t;
                         res.strVec<<t;
                     } else {
                         getParser()->qfmpError(QObject::tr("OUT OF RANGE: trying to assign to element %1, but vector %2 has only %3 elements").arg(ii[i]).arg(variable).arg(var.strVec.size()));
                         res.setInvalid();
                         return;
                     }
                 }
                 getParser()->setVariable(variable, var);
             } else if (ii.size()>1 && dat.size()==ii.size()) {
                 res.type=qfmpStringVector;
                 res.strVec.clear();
                 for (int i=0; i<ii.size(); i++) {
                     if (ii[i]>=0 && ii[i]<var.strVec.size()) {
                         const QString t=dat[i];
                         var.strVec[ii[i]]=t;
                         res.strVec<<t;
                     } else {
                         getParser()->qfmpError(QObject::tr("OUT OF RANGE: trying to assign to element %1, but vector %2 has only %3 elements").arg(ii[i]).arg(variable).arg(var.strVec.size()));
                         res.setInvalid();
                         return;
                     }
                 }
                 getParser()->setVariable(variable, var);
             } else {
                 getParser()->qfmpError(QObject::tr("can only assign x[N elements]=(1 element), x[N elements]=(N elements), x[1 elements]=(1 elements)"));
                 res.setInvalid();
                 return;
             }
         } else if ((exp.type==qfmpBool||exp.type==qfmpBoolVector) && var.type==qfmpBoolVector) {
             QVector<bool> dat=exp.asBoolVector();
             if (ii.size()==1 && dat.size()==1) {
                 if (ii[0]>=0 && ii[0]<var.boolVec.size()) {
                     var.boolVec[ii[0]]=dat[0];
                     getParser()->setVariable(variable, var);
                     res=qfmpResult(dat[0]);
                 } else {
                     getParser()->qfmpError(QObject::tr("OUT OF RANGE: trying to assign to element %1, but vector %2 has only %3 elements").arg(ii[0]).arg(variable).arg(var.boolVec.size()));
                     res.setInvalid();
                     return;
                 }
                 getParser()->setVariable(variable, var);
             } else if (ii.size()>1 && dat.size()==1) {
                 res.type=qfmpBoolVector;
                 res.boolVec.clear();
                 const bool t=dat[0];
                 for (int i=0; i<ii.size(); i++) {
                     if (ii[i]>=0 && ii[i]<var.boolVec.size()) {
                         var.boolVec[ii[i]]=t;
                         res.boolVec<<t;
                     } else {
                         getParser()->qfmpError(QObject::tr("OUT OF RANGE: trying to assign to element %1, but vector %2 has only %3 elements").arg(ii[i]).arg(variable).arg(var.boolVec.size()));
                         res.setInvalid();
                         return;
                     }
                 }
                 getParser()->setVariable(variable, var);
             } else if (ii.size()>1 && dat.size()==ii.size()) {
                 res.type=qfmpBoolVector;
                 res.boolVec.clear();
                 for (int i=0; i<ii.size(); i++) {
                     if (ii[i]>=0 && ii[i]<var.boolVec.size()) {
                         const bool t=dat[i];
                         var.boolVec[ii[i]]=t;
                         res.boolVec<<t;
                     } else {
                         getParser()->qfmpError(QObject::tr("OUT OF RANGE: trying to assign to element %1, but vector %2 has only %3 elements").arg(ii[i]).arg(variable).arg(var.boolVec.size()));
                         res.setInvalid();
                         return;
                     }
                 }
                 getParser()->setVariable(variable, var);
             } else {
                 getParser()->qfmpError(QObject::tr("can only assign x[N elements]=(1 element), x[N elements]=(N elements), x[1 elements]=(1 elements)"));
                 res.setInvalid();
                 return;
             }



         } else if (exp.type==qfmpString && var.type==qfmpString){
             QString dat=exp.asString();
             if (ii.size()==1 && dat.size()==1) {
                 if (ii[0]>=0 && ii[0]<var.str.size()) {
                     var.str[ii[0]]=dat[0];
                     getParser()->setVariable(variable, var);
                     res.setString(QString(dat[0]));
                 } else {
                     getParser()->qfmpError(QObject::tr("OUT OF RANGE: trying to assign to element %1, but string %2 has only %3 elements").arg(ii[0]).arg(variable).arg(var.str.size()));
                     res.setInvalid();
                     return;
                 }
                 getParser()->setVariable(variable, var);
             } else if (ii.size()>1 && dat.size()==1) {
                 res.type=qfmpString;
                 res.str.clear();
                 QChar t=dat[0];
                 for (int i=0; i<ii.size(); i++) {
                     if (ii[i]>=0 && ii[i]<var.str.size()) {
                         var.str[ii[i]]=t;
                         res.str+=t;
                     } else {
                         getParser()->qfmpError(QObject::tr("OUT OF RANGE: trying to assign to element %1, but string %2 has only %3 elements").arg(ii[i]).arg(variable).arg(var.str.size()));
                         res.setInvalid();
                         return;
                     }
                 }
                 getParser()->setVariable(variable, var);
             } else if (ii.size()>1 && dat.size()==ii.size()) {
                 res.type=qfmpString;
                 res.str.clear();
                 for (int i=0; i<ii.size(); i++) {
                     if (ii[i]>=0 && ii[i]<var.str.size()) {
                         QChar t=dat[i];
                         var.str[ii[i]]=t;
                         res.str+=t;
                     } else {
                         getParser()->qfmpError(QObject::tr("OUT OF RANGE: trying to assign to element %1, but string %2 has only %3 elements").arg(ii[i]).arg(variable).arg(var.str.size()));
                         res.setInvalid();
                         return;
                     }
                 }
                 getParser()->setVariable(variable, var);
             } else {
                 getParser()->qfmpError(QObject::tr("can only assign x[N elements]=(1 element), x[N elements]=(N elements), x[1 elements]=(1 elements)"));
                 res.setInvalid();
                 return;
             }
         } else {
             getParser()->qfmpError(QObject::tr("vector element assignment needs an expression which evaluates to the same type as the variable (var: %1, expression: %2)").arg(resultTypeToString(var.type)).arg(exp.toTypeString()));
             res.setInvalid();
             return;
         }
     } else {
         getParser()->qfmpError(QObject::tr("vector element assignment needs number or number vector as index, but index is %1").arg(resultTypeToString(idx.type)));
         res.setInvalid();
         return;
     }

}

QFMathParser::qfmpNode *QFMathParser::qfmpVectorElementAssignNode::copy(QFMathParser::qfmpNode *par)
{
    return new QFMathParser::qfmpVectorElementAssignNode(variable, index->copy(NULL), child->copy(NULL), getParser(), par);
}

QString QFMathParser::qfmpVectorElementAssignNode::print() const
{
    return QString("%1[%3] = %2").arg(variable).arg(child->print()).arg(index->print());
}

QString QFMathParser::qfmpVectorElementAssignNode::printTree(int level) const
{
    return QString(2*level, QLatin1Char(' '))+QString("VectorElementAssigneNode '%2'\n%1\n%3").arg(index->printTree(level+1)).arg(variable).arg(child->printTree(level+1));
}

QFMathParser::qfmpVariableVectorAccessNode::~qfmpVariableVectorAccessNode()
{
    if (index) delete index;
}

QFMathParser::qfmpVariableVectorAccessNode::qfmpVariableVectorAccessNode(QString var, QFMathParser::qfmpNode *index, QFMathParser *p, QFMathParser::qfmpNode *par):
    qfmpNode(p, par)
{
    variable=var;
    this->index=index;
    if(index) index->setParent(this);
}


void QFMathParser::qfmpVariableVectorAccessNode::evaluate(qfmpResult &res)
{
    qfmpResult  idx, var;
    if (index) index->evaluate(idx);
    res.isValid=true;

    var=getParser()->getVariable(variable);
    if (var.type==qfmpDoubleVector) {
        if (idx.convertsToBoolVector()) {
            QVector<bool> ii=idx.asBoolVector();
            if (ii.size()!=var.numVec.size()) {
                getParser()->qfmpError(QObject::tr("vector element access by boolean-vectors needs a boolean vector of the same size, as the data vector, but: index: %1, data: %2 elements").arg(ii.size()).arg(var.numVec.size()));
                res.setInvalid();
                return;
            }
            int trues=0;
            for (int i=0; i<ii.size(); i++) {
                if (ii[i]) trues++;
            }
            res.setDoubleVec(trues);
            int j=0;
            for (int i=0; i<ii.size(); i++) {
                if (ii[i]) {
                    res.numVec[j]=var.numVec[i];
                    j++;
                }
            }
        } else if (idx.convertsToIntVector()) {
            QVector<int> ii=idx.asIntVector();
            if (ii.size()==0) {
                getParser()->qfmpError(QObject::tr("vector element access needs non-empty number vector as index, but index is %1").arg(idx.toTypeString()));
                res.setInvalid();
                return;
            }
            res.type=qfmpDoubleVector;
            res.numVec=QVector<double>(ii.size(), 0);
            for (int i=0; i<ii.size(); i++) {
                if (ii[i]>=0 && ii[i]<var.numVec.size()) {
                    res.numVec[i]=var.numVec[ii[i]];
                } else {
                    getParser()->qfmpError(QObject::tr("OUT OF RANGE: trying to access element %1, but vector %2 has only %3 elements").arg(ii[i]).arg(variable).arg(var.numVec.size()));
                    res.setInvalid();
                    return;
                }
            }
            if (res.numVec.size()==1) res.setDouble(res.numVec[0]);

        } else {
            getParser()->qfmpError(QObject::tr("vector element access needsnumber/number vector or boolean/boolean vector as index, but index is %1").arg(resultTypeToString(idx.type)));
            res.setInvalid();
            return;
        }
    } else if (var.type==qfmpStringVector) {
        if (idx.convertsToBoolVector()) {
            QVector<bool> ii=idx.asBoolVector();
            if (ii.size()!=var.strVec.size()) {
                getParser()->qfmpError(QObject::tr("vector element access by boolean-vectors needs a boolean vector of the same size, as the data vector, but: index: %1, data: %2 elements").arg(ii.size()).arg(var.numVec.size()));
                res.setInvalid();
                return;
            }
            int trues=0;
            for (int i=0; i<ii.size(); i++) {
                if (ii[i]) trues++;
            }
            res.setStringVec(trues);
            int j=0;
            for (int i=0; i<ii.size(); i++) {
                if (ii[i]) {
                    res.strVec[j]=var.strVec[i];
                    j++;
                }
            }
        } else if (idx.convertsToIntVector()) {
            QVector<int> ii=idx.asIntVector();
            if (ii.size()==0) {
                getParser()->qfmpError(QObject::tr("vector element access needs non-empty number vector as index, but index is %1").arg(idx.toTypeString()));
                res.setInvalid();
                return;
            }
            res.type=qfmpStringVector;
            res.strVec.clear();
            //res.strVec.resize(ii.size(), QString());
            for (int i=0; i<ii.size(); i++) {
                if (ii[i]>=0 && ii[i]<var.strVec.size()) {
                    res.strVec<<var.strVec[ii[i]];
                } else {
                    getParser()->qfmpError(QObject::tr("OUT OF RANGE: trying to access element %1, but vector %2 has only %3 elements").arg(ii[i]).arg(variable).arg(var.strVec.size()));
                    res.setInvalid();
                    return;
                }
            }
            if (res.strVec.size()==1) res.setString(res.strVec[0]);

        } else {
            getParser()->qfmpError(QObject::tr("vector element access needsnumber/number vector or boolean/boolean vector as index, but index is %1").arg(resultTypeToString(idx.type)));
            res.setInvalid();
            return;
        }
    } else if (var.type==qfmpBoolVector) {
        if (idx.convertsToBoolVector()) {
            QVector<bool> ii=idx.asBoolVector();
            if (ii.size()!=var.boolVec.size()) {
                getParser()->qfmpError(QObject::tr("vector element access by boolean-vectors needs a boolean vector of the same size, as the data vector, but: index: %1, data: %2 elements").arg(ii.size()).arg(var.numVec.size()));
                res.setInvalid();
                return;
            }
            int trues=0;
            for (int i=0; i<ii.size(); i++) {
                if (ii[i]) trues++;
            }
            res.setBoolVec(trues);
            int j=0;
            for (int i=0; i<ii.size(); i++) {
                if (ii[i]) {
                    res.boolVec[j]=var.boolVec[i];
                    j++;
                }
            }
        } else if (idx.convertsToIntVector()) {
            QVector<int> ii=idx.asIntVector();
            if (ii.size()==0) {
                getParser()->qfmpError(QObject::tr("vector element access needs non-empty number vector as index, but index is %1").arg(idx.toTypeString()));
                res.setInvalid();
                return;
            }
            res.type=qfmpBoolVector;
            res.boolVec=QVector<bool>(ii.size(), 0);
            for (int i=0; i<ii.size(); i++) {
                if (ii[i]>=0 && ii[i]<var.boolVec.size()) {
                    res.boolVec[i]=var.boolVec[ii[i]];
                } else {
                    getParser()->qfmpError(QObject::tr("OUT OF RANGE: trying to access element %1, but vector %2 has only %3 elements").arg(ii[i]).arg(variable).arg(var.boolVec.size()));
                    res.setInvalid();
                    return;
                }
            }
            if (res.boolVec.size()==1) res.setBoolean(res.boolVec[0]);

        } else {
            getParser()->qfmpError(QObject::tr("vector element access needsnumber/number vector or boolean/boolean vector as index, but index is %1").arg(resultTypeToString(idx.type)));
            res.setInvalid();
            return;
        }
    } else if (var.type==qfmpString) {
        if (idx.convertsToBoolVector()) {
            QVector<bool> ii=idx.asBoolVector();
            if (ii.size()!=var.str.size()) {
                getParser()->qfmpError(QObject::tr("vector element access by boolean-vectors needs a boolean vector of the same size, as the data vector, but: index: %1, data: %2 elements").arg(ii.size()).arg(var.numVec.size()));
                res.setInvalid();
                return;
            }
            int trues=0;
            for (int i=0; i<ii.size(); i++) {
                if (ii[i]) trues++;
            }
            res.setString(trues);
            int j=0;
            for (int i=0; i<ii.size(); i++) {
                if (ii[i]) {
                    res.str[j]=var.str[i];
                    j++;
                }
            }
        } else if (idx.convertsToIntVector()) {
            QVector<int> ii=idx.asIntVector();
            if (ii.size()==0) {
                getParser()->qfmpError(QObject::tr("vector element access needs non-empty number vector as index, but index is %1").arg(idx.toTypeString()));
                res.setInvalid();
                return;
            }
            res.type=qfmpString;
            res.str="";
            for (int i=0; i<ii.size(); i++) {
                if (ii[i]>=0 && ii[i]<var.str.size()) {
                    res.str+=var.str[ii[i]];
                } else {
                    getParser()->qfmpError(QObject::tr("OUT OF RANGE: trying to access element %1, but string %2 has only %3 elements").arg(ii[i]).arg(variable).arg(var.str.size()));
                    res.setInvalid();
                    return;
                }
            }

        } else {
            getParser()->qfmpError(QObject::tr("string character access needsnumber/number vector or boolean/boolean vector as index, but index is %1").arg(resultTypeToString(idx.type)));
            res.setInvalid();
            return;
        }
    } else {
        getParser()->qfmpError(QObject::tr("vector element/string character access is only possible if the variable is a number/string/bool vector, or a string, but variable '%1' is of type %2").arg(variable).arg(resultTypeToString(var.type)));
        res.setInvalid();
        return;
    }
}

QFMathParser::qfmpNode *QFMathParser::qfmpVariableVectorAccessNode::copy(QFMathParser::qfmpNode *par)
{
    return new qfmpVariableVectorAccessNode(variable, index->copy(NULL), getParser(), par);
}

QString QFMathParser::qfmpVariableVectorAccessNode::print() const
{
    return QString("%1[%2]").arg(variable).arg(index->print());
}

QString QFMathParser::qfmpVariableVectorAccessNode::printTree(int level) const
{
return QString(2*level, QLatin1Char(' '))+QString("VariableVectorAccessNode: '%2'\n%1").arg(index->printTree(level+1)).arg(variable);
}

void QFMathParser::qfmpNode::evaluate(qfmpResult &result)
{
    result=evaluate();
}












//#define QFMATHPARSER_BYTECODESTACK_ERRORCHECKING





double QFMathParser::evaluateBytecode(const QFMathParser::ByteCodeProgram &program)
{
    /*QLinkedList<qfmpResult> resultStack;
    QLinkedList<QString> stringStack;
    QLinkedList<int> intStack;*/
    QStack<double> resultStack;
    QVector<double> heap;
    //int heappointer=0;
    heap.resize(ByteCodeInitialHeapSize);
    resultStack.reserve(128);
    bool ok=true;
    QFMathParser::ByteCodeProgram::const_iterator itp=program.begin();
    while (ok && itp<program.end()) {
#ifdef QFMATHPARSER_BYTECODESTACK_DEBUGMESSAGES
        QFMathParser::ByteCodeProgram::const_iterator itp_old=itp;
#endif
        switch (itp->opcode) {
            case bcNOP:
                break;
            case bcPush:
                resultStack.push(itp->numpar);
                break;
            case bcPop:
#ifdef QFMATHPARSER_BYTECODESTACK_ERRORCHECKING
                if (resultStack.isEmpty()) {
                    qfmpError(QObject::tr("QFMathParser Bytecode Interpreter: error in POP opcode: stack is empty"));
                    ok=false;
                } else
#endif
                    resultStack.pop();

                break;

            case bcVarRead:
                resultStack.push(*((double*)itp->pntpar));
                break;
            case bcVarWrite:
#ifdef QFMATHPARSER_BYTECODESTACK_ERRORCHECKING
                if (resultStack.size()<1) {
                    qfmpError(QObject::tr("QFMathParser Bytecode Interpreter: error in VARWRITE opcode: stack is too small"));
                    ok=false;
                } else
#endif
                *((double*)itp->pntpar)=resultStack.pop();
                break;


            case bcHeapRead:
                resultStack.push(heap.value(itp->intpar, NAN));
#ifdef QFMATHPARSER_BYTECODESTACK_DEBUGMESSAGES
                //qDebug()<<"HEAPREAD  #"<<itp->intpar+heapoffset<<" == "<<heap.value(itp->intpar+heapoffset, NAN);
#endif
                break;
            case bcHeapWrite:
#ifdef QFMATHPARSER_BYTECODESTACK_ERRORCHECKING
                if (resultStack.size()<1) {
                    qfmpError(QObject::tr("QFMathParser Bytecode Interpreter: error in HEAPWRITE opcode: stack is too small"));
                    ok=false;
                } else
#endif
                {
                    if (itp->intpar>heap.size()) heap.resize(itp->intpar+1);
                    heap[itp->intpar]=resultStack.pop();
#ifdef QFMATHPARSER_BYTECODESTACK_DEBUGMESSAGES
                    //qDebug()<<"HEAPWRITE  #"<<itp->intpar+heapoffset<<" <= "<<heap[itp->intpar+heapoffset];
#endif
                }
                break;



            case bcCallCFunction:
#ifdef QFMATHPARSER_BYTECODESTACK_ERRORCHECKING
                if (resultStack.size()<itp->intpar) {
                    qfmpError(QObject::tr("QFMathParser Bytecode Interpreter: error in CALLCFUNC opcode: stack is too small"));
                    ok=false;
                } else
#endif
                {
                    const QStack<double>::iterator it=resultStack.end();
                    switch (itp->intpar) {
                        case 0: *(it-1)=((qfmpEvaluateFuncSimple0Param)itp->pntpar)(); break;
                        case 1: *(it-1)=((qfmpEvaluateFuncSimple1Param)itp->pntpar)(*(it-1)); break;
                        case 2: *(it-2)=((qfmpEvaluateFuncSimple2Param)itp->pntpar)(*(it-1),*(it-2)); resultStack.pop(); break;
                        case 3: *(it-3)=((qfmpEvaluateFuncSimple3Param)itp->pntpar)(*(it-1),*(it-2),*(it-3)); resultStack.pop(); resultStack.pop(); break;
                        default: break;
                    }
                } break;
            case bcCallCMPFunction:
#ifdef QFMATHPARSER_BYTECODESTACK_ERRORCHECKING
                if (resultStack.size()<itp->intpar) {
                    qfmpError(QObject::tr("QFMathParser Bytecode Interpreter: error in CALLCMPFUNC opcode: stack is too small"));
                    ok=false;
                } else
#endif
                {
                    const QStack<double>::iterator it=resultStack.end();
                    switch (itp->intpar) {
                        case 0: *(it-1)=((qfmpEvaluateFuncSimple0ParamMP)itp->pntpar)(this); break;
                        case 1: *(it-1)=((qfmpEvaluateFuncSimple1ParamMP)itp->pntpar)(*(it-1), this); break;
                        case 2: *(it-2)=((qfmpEvaluateFuncSimple2ParamMP)itp->pntpar)(*(it-1),*(it-2), this); resultStack.pop(); break;
                        case 3: *(it-3)=((qfmpEvaluateFuncSimple3ParamMP)itp->pntpar)(*(it-1),*(it-2),*(it-3), this); resultStack.pop(); resultStack.pop(); break;
                        default: break;
                    }
                } break;
        case bcCallResultFunction:
#ifdef QFMATHPARSER_BYTECODESTACK_ERRORCHECKING
            if (resultStack.size()<itp->intpar) {
                qfmpError(QObject::tr("QFMathParser Bytecode Interpreter: error in CALLRESULTFUNC opcode: stack is too small"));
                ok=false;
            } else
#endif
            {
                const QStack<double>::iterator it=resultStack.end();
                QVector<qfmpResult> parameters;
                for (int i=0; i<itp->intpar; i++) {
                    parameters<<qfmpResult(resultStack.pop());
                }
                qfmpResult r;
                evaluateFunction(r, itp->strpar, parameters);
                if (r.type==qfmpDouble) resultStack.push(r.asNumber());
                else if (r.type==qfmpBool) resultStack.push((r.asBool())?1.0:0.0);
                else {
                    qfmpError(QObject::tr("QFMathParser Bytecode Interpreter: result of function call ('%1'') was not a number!").arg(itp->strpar));
                    ok=false;
                }
            } break;

            case bcAdd:
#ifdef QFMATHPARSER_BYTECODESTACK_ERRORCHECKING
                if (resultStack.size()<2) {
                    qfmpError(QObject::tr("QFMathParser Bytecode Interpreter: error in ADD opcode: stack is too small"));
                    ok=false;
                } else
#endif
                {
                    const QStack<double>::iterator it=resultStack.end();
                    *(it-2) = (*(it-1))+(*(it-2));
                    resultStack.pop();

                }
                break;
            case bcSub:
#ifdef QFMATHPARSER_BYTECODESTACK_ERRORCHECKING
                if (resultStack.size()<2) {
                    qfmpError(QObject::tr("QFMathParser Bytecode Interpreter: error in SUB opcode: stack is too small"));
                    ok=false;
                } else
#endif
                {
                    const QStack<double>::iterator it=resultStack.end();
                    *(it-2) = (*(it-1))-(*(it-2));
                    resultStack.pop();

                }
                break;
            case bcMul:
#ifdef QFMATHPARSER_BYTECODESTACK_ERRORCHECKING
                if (resultStack.size()<2) {
                    qfmpError(QObject::tr("QFMathParser Bytecode Interpreter: error in MUL opcode: stack is too small"));
                    ok=false;
                } else
#endif
                {
                    const QStack<double>::iterator it=resultStack.end();
                    *(it-2) = (*(it-1))*(*(it-2));
                    resultStack.pop();

                }
                break;

            case bcDiv:
#ifdef QFMATHPARSER_BYTECODESTACK_ERRORCHECKING
                if (resultStack.size()<2) {
                    qfmpError(QObject::tr("QFMathParser Bytecode Interpreter: error in DIV opcode: stack is too small"));
                    ok=false;
                } else
#endif
                {
                    const QStack<double>::iterator it=resultStack.end();
                    *(it-2) = (*(it-1))/(*(it-2));
                    resultStack.pop();

                }
                break;
            case bcMod:
#ifdef QFMATHPARSER_BYTECODESTACK_ERRORCHECKING
                if (resultStack.size()<2) {
                    qfmpError(QObject::tr("QFMathParser Bytecode Interpreter: error in MOD opcode: stack is too small"));
                    ok=false;
                } else
#endif
                {
                    const QStack<double>::iterator it=resultStack.end();
                    *(it-2) = int32_t(*(it-1))%int32_t(*(it-2));
                    resultStack.pop();

                }
                break;
            case bcPow:
#ifdef QFMATHPARSER_BYTECODESTACK_ERRORCHECKING
                if (resultStack.size()<2) {
                    qfmpError(QObject::tr("QFMathParser Bytecode Interpreter: error in POW opcode: stack is too small"));
                    ok=false;
                } else
#endif
                {
                    const QStack<double>::iterator it=resultStack.end();
                    *(it-2) = pow((*(it-1)),(*(it-2)));
                    resultStack.pop();

                }

                break;
            case bcNeg:
#ifdef QFMATHPARSER_BYTECODESTACK_ERRORCHECKING
                if (resultStack.size()<1) {
                    qfmpError(QObject::tr("QFMathParser Bytecode Interpreter: error in NEG opcode: stack is too small"));
                    ok=false;
                } else
#endif
                {
                    const QStack<double>::iterator it=resultStack.end();
                    *(it-1) = -(*(it-1));

                }
                break;



            case bcBitAnd:
#ifdef QFMATHPARSER_BYTECODESTACK_ERRORCHECKING
                if (resultStack.size()<2) {
                    qfmpError(QObject::tr("QFMathParser Bytecode Interpreter: error in BITAND opcode: stack is too small"));
                    ok=false;
                } else
#endif
                {
                    const QStack<double>::iterator it=resultStack.end();
                    *(it-2) = int32_t(*(it-1))&int32_t(*(it-2));
                    resultStack.pop();

                } break;
            case bcBitOr:
#ifdef QFMATHPARSER_BYTECODESTACK_ERRORCHECKING
                if (resultStack.size()<2) {
                    qfmpError(QObject::tr("QFMathParser Bytecode Interpreter: error in BITOR opcode: stack is too small"));
                    ok=false;
                } else
#endif
                {
                    const QStack<double>::iterator it=resultStack.end();
                    *(it-2) = int32_t(*(it-1))|int32_t(*(it-2));
                    resultStack.pop();

                }

                break;
            case bcBitNot:
#ifdef QFMATHPARSER_BYTECODESTACK_ERRORCHECKING
                if (resultStack.size()<1) {
                    qfmpError(QObject::tr("QFMathParser Bytecode Interpreter: error in BITNOT opcode: stack is too small"));
                    ok=false;
                } else
#endif
                {
                    const QStack<double>::iterator it=resultStack.end();
                    *(it-1) = ~int32_t(*(it-1));

                }
                break;



            case bcLogicAnd:
#ifdef QFMATHPARSER_BYTECODESTACK_ERRORCHECKING
                if (resultStack.size()<2) {
                    qfmpError(QObject::tr("QFMathParser Bytecode Interpreter: error in LOGICAND opcode: stack is too small"));
                    ok=false;
                } else
#endif
                {
                    const QStack<double>::iterator it=resultStack.end();
                    *(it-2) = ((*(it-1)!=0.0)&&(*(it-2)!=0.0))?1:0;
                    resultStack.pop();

                } break;
            case bcLogicOr:
#ifdef QFMATHPARSER_BYTECODESTACK_ERRORCHECKING
                if (resultStack.size()<2) {
                    qfmpError(QObject::tr("QFMathParser Bytecode Interpreter: error in LOGICOR opcode: stack is too small"));
                    ok=false;
                } else
#endif
                {
                    const QStack<double>::iterator it=resultStack.end();
                    *(it-2) = ((*(it-1)!=0.0)||(*(it-2)!=0.0))?1:0;
                    resultStack.pop();

                }

                break;
            case bcLogicXor:
#ifdef QFMATHPARSER_BYTECODESTACK_ERRORCHECKING
                if (resultStack.size()<2) {
                    qfmpError(QObject::tr("QFMathParser Bytecode Interpreter: error in LOGICOR opcode: stack is too small"));
                    ok=false;
                } else
#endif
                {
                    const QStack<double>::iterator it=resultStack.end();
                    *(it-2) = (((*(it-1)!=0.0)&&(*(it-2)==0.0))||((*(it-1)==0.0)&&(*(it-2)!=0.0)))?1:0;
                    resultStack.pop();

                }

                break;

            case bcLogicNot:
#ifdef QFMATHPARSER_BYTECODESTACK_ERRORCHECKING
                if (resultStack.size()<1) {
                    qfmpError(QObject::tr("QFMathParser Bytecode Interpreter: error in LOGICNOT opcode: stack is too small"));
                    ok=false;
                } else
#endif
                {
                    const QStack<double>::iterator it=resultStack.end();
                    *(it-1) = (!(*(it-1)!=0.0))?1:0;

                }
                break;

            case bcCmpEqual:
#ifdef QFMATHPARSER_BYTECODESTACK_ERRORCHECKING
                if (resultStack.size()<2) {
                    qfmpError(QObject::tr("QFMathParser Bytecode Interpreter: error in CMPEQUAL opcode: stack is too small"));
                    ok=false;
                } else
#endif
                {
                    const QStack<double>::iterator it=resultStack.end();
                    *(it-2) = ((*(it-1))==(*(it-2)))?1:0;
                    resultStack.pop();

                }
                break;



            case bcCmpLesser:
#ifdef QFMATHPARSER_BYTECODESTACK_ERRORCHECKING
                if (resultStack.size()<2) {
                    qfmpError(QObject::tr("QFMathParser Bytecode Interpreter: error in CMPLESSER opcode: stack is too small"));
                    ok=false;
                } else
#endif
                {
                    const QStack<double>::iterator it=resultStack.end();
                    *(it-2) = ((*(it-1))<(*(it-2)))?1:0;
                    resultStack.pop();

                }
                break;

            case bcCmpLesserEqual:
#ifdef QFMATHPARSER_BYTECODESTACK_ERRORCHECKING
                if (resultStack.size()<2) {
                    qfmpError(QObject::tr("QFMathParser Bytecode Interpreter: error in CMPLESSEREQUAL opcode: stack is too small"));
                    ok=false;
                } else
#endif
                {
                    const QStack<double>::iterator it=resultStack.end();
                    *(it-2) = ((*(it-1))<=(*(it-2)))?1:0;
                    resultStack.pop();

                }
                break;

            case bcJumpRel:
                itp+=(itp->intpar-1);
#ifdef QFMATHPARSER_BYTECODESTACK_DEBUGMESSAGES
                //qDebug()<<"                                                    @"<<((int)itp_old-(int)program.begin())/int(sizeof(*itp_old));
                //qDebug()<<"                                                       jump to "<<1+((int)itp-(int)program.begin())/int(sizeof(*itp_old));
#endif
                break;
            case bcBJumpRel:
                itp-=(itp->intpar+1);
#ifdef QFMATHPARSER_BYTECODESTACK_DEBUGMESSAGES
                //qDebug()<<"                                                    @"<<((int)itp_old-(int)program.begin())/int(sizeof(*itp_old));
                //qDebug()<<"                                                       bjump to "<<1+((int)itp-(int)program.begin())/int(sizeof(*itp_old));
#endif
                break;

            case bcJumpCondRel:
#ifdef QFMATHPARSER_BYTECODESTACK_ERRORCHECKING
                if (resultStack.size()<1) {
                    qfmpError(QObject::tr("QFMathParser Bytecode Interpreter: error in JMPCONDREL opcode: stack is too small"));
                    ok=false;
                } else
#endif
                {
                    if (resultStack.pop()!=0.0) {
#ifdef QFMATHPARSER_BYTECODESTACK_DEBUGMESSAGES
                        //qDebug()<<"                                                    @"<<((int)itp_old-(int)program.begin())/int(sizeof(*itp_old));
#endif
                        itp+=(itp->intpar-1);
#ifdef QFMATHPARSER_BYTECODESTACK_DEBUGMESSAGES
                        //qDebug()<<"                                                       jumpcond to "<<1+((int)itp-(int)program.begin())/int(sizeof(*itp_old));
#endif
                    }
                }
                break;
            case bcBJumpCondRel:
#ifdef QFMATHPARSER_BYTECODESTACK_ERRORCHECKING
                if (resultStack.size()<1) {
                    qfmpError(QObject::tr("QFMathParser Bytecode Interpreter: error in BJMPCONDREL opcode: stack is too small"));
                    ok=false;
                } else
#endif
                {
                    if (resultStack.pop()!=0.0) {
#ifdef QFMATHPARSER_BYTECODESTACK_DEBUGMESSAGES
                        //qDebug()<<"                                                    @"<<((int)itp_old-(int)program.begin())/int(sizeof(*itp_old));
#endif
                        itp-=(itp->intpar+1);
#ifdef QFMATHPARSER_BYTECODESTACK_DEBUGMESSAGES
                        //qDebug()<<"                                                       bjumpcond to "<<1+((int)itp-(int)program.begin())/int(sizeof(*itp_old));
#endif
                    }
                }
                break;

            default:
#ifdef QFMATHPARSER_BYTECODESTACK_ERRORCHECKING
                qfmpError(QObject::tr("QFMathParser Bytecode Interpreter: unknown opcode %1 encountered").arg(itp->opcode));
                ok=false;
#endif
                break;
        }

#ifdef QFMATHPARSER_BYTECODESTACK_DEBUGMESSAGES
        //qDebug()<<((int)itp_old-(int)program.begin())/int(sizeof(*itp_old))<<": "<<printBytecode(*itp_old)<<"   stack_after = "<<arrayToString(resultStack.data(), resultStack.size());
#endif

        ++itp;
    }
    if (ok) {
        if (resultStack.size()>0) {
            return resultStack.last();
        } else {
            qfmpError(QObject::tr("QFMathParser Bytecode Interpreter: no result returned"));
            return NAN;
        }
    } else {
        qfmpError(QObject::tr("QFMathParser Bytecode Interpreter: error running program"));
        return NAN;
    }
}

QString QFMathParser::printBytecode(const QFMathParser::ByteCodeInstruction &inst)
{
    QString res="";

    switch (inst.opcode) {
        case bcNOP:
            res+=QString("NOP");
            break;
        case bcPush:
            res+=QString("PUSH %1").arg(inst.numpar);
            break;
        case bcCallCFunction:
            res+=QString("CALLCFUNCTION 0x%1, %2").arg((uint64_t)inst.pntpar,0,16).arg(inst.intpar);
            break;
        case bcCallCMPFunction:
            res+=QString("CALLCMPFUNCTION 0x%1, %2").arg((uint64_t)inst.pntpar,0,16).arg(inst.intpar);
            break;
        case bcCallResultFunction:
            res+=QString("CALLRESULTFUNCTION %1, %2").arg(inst.strpar).arg(inst.intpar);
            break;
        case bcJumpRel:
            res+=QString("JMPREL %1").arg(inst.intpar);
            break;
        case bcJumpCondRel:
            res+=QString("JMPCONDREL %1").arg(inst.intpar);
            break;
        case bcBJumpRel:
            res+=QString("BJMPREL %1").arg(inst.intpar);
            break;
        case bcBJumpCondRel:
            res+=QString("BJMPCONDREL %1").arg(inst.intpar);
            break;
        case bcPop:
            res+=QString("POP");
            break;
        case bcVarRead:
            res+=QString("VARREAD 0x%1").arg((uint64_t)inst.pntpar,0,16);
            break;
        case bcVarWrite:
            res+=QString("VARWRITE 0x%1").arg((uint64_t)inst.pntpar,0,16);
            break;
        case bcAdd:
            res+=QString("ADD");
            break;
        case bcSub:
            res+=QString("SUB");
            break;
        case bcMul:
            res+=QString("MUL");
            break;
        case bcDiv:
            res+=QString("DIV");
            break;
        case bcMod:
            res+=QString("MOD");
            break;
        case bcPow:
            res+=QString("POW");
            break;
        case bcBitAnd:
            res+=QString("BITAND");
            break;
        case bcBitOr:
            res+=QString("BITOR");
            break;
        case bcBitNot:
            res+=QString("BITNOT");
            break;
        case bcNeg:
            res+=QString("NEG");
            break;
        case bcLogicAnd: res+=QString("LOGICAND"); break;
        case bcLogicOr: res+=QString("LOGICOR"); break;
        case bcLogicNot: res+=QString("LOGICNOT"); break;
        case bcLogicXor: res+=QString("LOGICXOR"); break;
        case bcCmpEqual: res+=QString("CMPEQUAL"); break;
        case bcCmpLesser: res+=QString("CMPLESSER"); break;
        case bcCmpLesserEqual: res+=QString("CMPLESSEREQUAL"); break;
        case bcHeapRead: res+=QString("HEAPREAD %1").arg(inst.intpar); break;
        case bcHeapWrite: res+=QString("HEAPWRITE %1").arg(inst.intpar); break;
        default:
            res+=QString("*** UNKNOWN *** %1").arg(inst.opcode);
            break;
    }
    return res;
}

QString QFMathParser::printBytecode(const QFMathParser::ByteCodeProgram &program)
{
    QString res="";
    for (int i = 0; i < program.size(); ++i) {
        const ByteCodeInstruction& inst=program[i];
        res+=QString("%1: %2\n").arg(i, 5, 10, QLatin1Char(' ')).arg(printBytecode(inst));
    }
    return res;
}


QFMathParser::ByteCodeInstruction::ByteCodeInstruction(QFMathParser::ByteCodes opcode)
{
    this->opcode=opcode;
}

QFMathParser::ByteCodeInstruction::ByteCodeInstruction(QFMathParser::ByteCodes opcode, double respar)
{
    this->opcode=opcode;
    this->numpar=respar;
}


QFMathParser::ByteCodeInstruction::ByteCodeInstruction(QFMathParser::ByteCodes opcode, int intpar)
{
    this->opcode=opcode;
    this->intpar=intpar;
}

QFMathParser::ByteCodeInstruction::ByteCodeInstruction(QFMathParser::ByteCodes opcode, void *pntpar)
{
    this->opcode=opcode;
    this->pntpar=pntpar;
}

QFMathParser::ByteCodeInstruction::ByteCodeInstruction(QFMathParser::ByteCodes opcode, void *pntpar, int intpar)
{
    this->opcode=opcode;
    this->pntpar=pntpar;
    this->intpar=intpar;
}

QFMathParser::ByteCodeInstruction::ByteCodeInstruction(QFMathParser::ByteCodes opcode, QString strpar, int intpar)
{
    this->opcode=opcode;
    this->strpar=strpar;
    this->intpar=intpar;
}

QFMathParser::ByteCodeEnvironment::ByteCodeEnvironment(QFMathParser *parser)
{
    this->parser=parser;
    heapItemPointer=0;
}

void QFMathParser::ByteCodeEnvironment::init(QFMathParser *parser)
{
    heapItemPointer=0;
}

int QFMathParser::ByteCodeEnvironment::pushVar(const QString &name)
{
    heapVariables[name].append(heapItemPointer);
    heapItemPointer++;
    return heapItemPointer-1;
}

void QFMathParser::ByteCodeEnvironment::popVar(const QString &name)
{
    if (heapVariables[name].size()<=1) {
        heapVariables.remove(name);
    } else {
        heapVariables[name].pop_back();
    }
    heapItemPointer--;
}

qfmpResult QFMathParser::qfmpNode::evaluate()
{
    qfmpResult res;
    evaluate(res);
    return res;
}

QString QFMathParser::qfmpNode::print() const
{
    return QString();
}

QString QFMathParser::qfmpNode::printTree(int level) const
{
    return QString();
}


QFMathParser::qfmpVectorAccessNode::~qfmpVectorAccessNode()
{
    if (index) delete index;
    if (left) delete left;
}

QFMathParser::qfmpVectorAccessNode::qfmpVectorAccessNode(QFMathParser::qfmpNode *left, QFMathParser::qfmpNode *index, QFMathParser *p, QFMathParser::qfmpNode *par):
    qfmpNode(p, par)
{
    this->left=left;
    this->index=index;
    if (index) index->setParent(this);
    if (left) left->setParent(this);
}

void QFMathParser::qfmpVectorAccessNode::evaluate(qfmpResult &res)
{
    qfmpResult  idx, var;
    if (index) index->evaluate(idx);
    res.isValid=true;

    if (left) left->evaluate(var);
    if (var.type==qfmpDoubleVector) {
        if (idx.convertsToBoolVector()) {
            QVector<bool> ii=idx.asBoolVector();
            if (ii.size()!=var.numVec.size()) {
                getParser()->qfmpError(QObject::tr("vector element access by boolean-vectors needs a boolean vector of the same size, as the data vector, but: index: %1, data: %2 elements").arg(ii.size()).arg(var.numVec.size()));
                res.setInvalid();
                return;
            }
            int trues=0;
            for (int i=0; i<ii.size(); i++) {
                if (ii[i]) trues++;
            }
            res.setDoubleVec(trues);
            int j=0;
            for (int i=0; i<ii.size(); i++) {
                if (ii[i]) {
                    res.numVec[j]=var.numVec[i];
                    j++;
                }
            }
        } else if (idx.convertsToIntVector()) {
            QVector<int> ii=idx.asIntVector();
            if (ii.size()==0) {
                getParser()->qfmpError(QObject::tr("vector element access needs non-empty number vector as index, but index is %1").arg(idx.toTypeString()));
                res.setInvalid();
                return;
            }
            res.type=qfmpDoubleVector;
            res.numVec=QVector<double>(ii.size(), 0);
            for (int i=0; i<ii.size(); i++) {
                if (ii[i]>=0 && ii[i]<var.numVec.size()) {
                    res.numVec[i]=var.numVec[ii[i]];
                } else {
                    getParser()->qfmpError(QObject::tr("OUT OF RANGE: trying to access element %1, but vector has only %2 elements").arg(ii[i]).arg(var.numVec.size()));
                    res.setInvalid();
                    return;
                }
            }
            if (res.numVec.size()==1) res.setDouble(res.numVec[0]);

        } else {
            getParser()->qfmpError(QObject::tr("vector element access needs number/number vector or boolean/boolean vector as index, but index is %1").arg(resultTypeToString(idx.type)));
            res.setInvalid();
            return;
        }
    } else if (var.type==qfmpStringVector) {
        if (idx.convertsToBoolVector()) {
            QVector<bool> ii=idx.asBoolVector();
            if (ii.size()!=var.strVec.size()) {
                getParser()->qfmpError(QObject::tr("vector element access by boolean-vectors needs a boolean vector of the same size, as the data vector, but: index: %1, data: %2 elements").arg(ii.size()).arg(var.numVec.size()));
                res.setInvalid();
                return;
            }
            int trues=0;
            for (int i=0; i<ii.size(); i++) {
                if (ii[i]) trues++;
            }
            res.setStringVec(trues);
            int j=0;
            for (int i=0; i<ii.size(); i++) {
                if (ii[i]) {
                    res.strVec[j]=var.strVec[i];
                    j++;
                }
            }
        } else if (idx.convertsToIntVector()) {
            QVector<int> ii=idx.asIntVector();
            if (ii.size()==0) {
                getParser()->qfmpError(QObject::tr("vector element access needs non-empty number vector as index, but index is %1").arg(idx.toTypeString()));
                res.setInvalid();
                return;
            }
            res.type=qfmpStringVector;
            res.strVec.clear();
            //res.strVec.resize(ii.size(), QString());
            for (int i=0; i<ii.size(); i++) {
                if (ii[i]>=0 && ii[i]<var.strVec.size()) {
                    res.strVec<<var.strVec[ii[i]];
                } else {
                    getParser()->qfmpError(QObject::tr("OUT OF RANGE: trying to access element %1, but vector has only %2 elements").arg(ii[i]).arg(var.strVec.size()));
                    res.setInvalid();
                    return;
                }
            }
            if (res.strVec.size()==1) res.setString(res.strVec[0]);

        } else {
            getParser()->qfmpError(QObject::tr("vector element access needs number/number vector or boolean/boolean vector as index, but index is %1").arg(resultTypeToString(idx.type)));
            res.setInvalid();
            return;
        }
    } else if (var.type==qfmpBoolVector) {
        if (idx.convertsToBoolVector()) {
            QVector<bool> ii=idx.asBoolVector();
            if (ii.size()!=var.boolVec.size()) {
                getParser()->qfmpError(QObject::tr("vector element access by boolean-vectors needs a boolean vector of the same size, as the data vector, but: index: %1, data: %2 elements").arg(ii.size()).arg(var.numVec.size()));
                res.setInvalid();
                return;
            }
            int trues=0;
            for (int i=0; i<ii.size(); i++) {
                if (ii[i]) trues++;
            }
            res.setBoolVec(trues);
            int j=0;
            for (int i=0; i<ii.size(); i++) {
                if (ii[i]) {
                    res.boolVec[j]=var.boolVec[i];
                    j++;
                }
            }
        } else if (idx.convertsToIntVector()) {
            QVector<int> ii=idx.asIntVector();
            if (ii.size()==0) {
                getParser()->qfmpError(QObject::tr("vector element access needs non-empty number vector as index, but index is %1").arg(idx.toTypeString()));
                res.setInvalid();
                return;
            }
            res.type=qfmpBoolVector;
            res.boolVec=QVector<bool>(ii.size(), 0);
            for (int i=0; i<ii.size(); i++) {
                if (ii[i]>=0 && ii[i]<var.boolVec.size()) {
                    res.boolVec[i]=var.boolVec[ii[i]];
                } else {
                    getParser()->qfmpError(QObject::tr("OUT OF RANGE: trying to access element %1, but vector has only %2 elements").arg(ii[i]).arg(var.boolVec.size()));
                    res.setInvalid();
                    return;
                }
            }
            if (res.boolVec.size()==1) res.setBoolean(res.boolVec[0]);

        } else {
            getParser()->qfmpError(QObject::tr("vector element access needs number/number vector or boolean/boolean vector as index, but index is %1").arg(resultTypeToString(idx.type)));
            res.setInvalid();
            return;
        }
    } else if (var.type==qfmpString) {
        if (idx.convertsToBoolVector()) {
            QVector<bool> ii=idx.asBoolVector();
            if (ii.size()!=var.str.size()) {
                getParser()->qfmpError(QObject::tr("vector element access by boolean-vectors needs a boolean vector of the same size, as the data vector, but: index: %1, data: %2 elements").arg(ii.size()).arg(var.numVec.size()));
                res.setInvalid();
                return;
            }
            int trues=0;
            for (int i=0; i<ii.size(); i++) {
                if (ii[i]) trues++;
            }
            res.setString(trues);
            int j=0;
            for (int i=0; i<ii.size(); i++) {
                if (ii[i]) {
                    res.str[j]=var.str[i];
                    j++;
                }
            }
        } else if (idx.convertsToIntVector()) {
            QVector<int> ii=idx.asIntVector();
            if (ii.size()==0) {
                getParser()->qfmpError(QObject::tr("vector element access needs non-empty number vector as index, but index is %1").arg(idx.toTypeString()));
                res.setInvalid();
                return;
            }
            res.type=qfmpString;
            res.str="";
            for (int i=0; i<ii.size(); i++) {
                if (ii[i]>=0 && ii[i]<var.str.size()) {
                    res.str+=var.str[ii[i]];
                } else {
                    getParser()->qfmpError(QObject::tr("OUT OF RANGE: trying to access element %1, but string has only %2 elements").arg(ii[i]).arg(var.str.size()));
                    res.setInvalid();
                    return;
                }
            }

        } else {
            getParser()->qfmpError(QObject::tr("string character access needs number/number vector or boolean/boolean vector as index, but index is %1").arg(resultTypeToString(idx.type)));
            res.setInvalid();
            return;
        }
    } else {
        getParser()->qfmpError(QObject::tr("vector element/string character access is only possible if the expression is a number/string/bool vector, or a string, but expression is of type %1").arg(resultTypeToString(var.type)));
        res.setInvalid();
        return;
    }
}

QFMathParser::qfmpNode *QFMathParser::qfmpVectorAccessNode::copy(QFMathParser::qfmpNode *par)
{
    return new qfmpVectorAccessNode(left->copy(NULL), index->copy(NULL), getParser(), par);
}

QString QFMathParser::qfmpVectorAccessNode::print() const
{
    return QString("(%1)[%2]").arg(left->print()).arg(index->print());
}

QString QFMathParser::qfmpVectorAccessNode::printTree(int level) const
{
    return QString(2*level, QLatin1Char(' '))+QString("VectorAccessNode:\n%1\n%2").arg(left->printTree(level+1)).arg(index->printTree(level+1));
}


QFMathParser::qfmpStructAccessNode::~qfmpStructAccessNode()
{
    if (left) delete left;
}

QFMathParser::qfmpStructAccessNode::qfmpStructAccessNode(QFMathParser::qfmpNode *left, const QString &index, QFMathParser *p, QFMathParser::qfmpNode *par):
    qfmpNode(p, par)
{
    this->left=left;
    this->index=index;
    if (left) left->setParent(this);
}

void QFMathParser::qfmpStructAccessNode::evaluate(qfmpResult &result)
{
    qfmpResult var;

    if (left) left->evaluate(var);
    if (var.type==qfmpStruct) {
        if (var.structData.contains(index)) {
            result=var.structData[index];
            return;
        } else {
            getParser()->qfmpError(QObject::tr("struct element access: item '%1' does not exist (available elements: %2).").arg(index).arg(QStringList(var.structData.keys()).join(", ")));
            result.setInvalid();
            return;
        }

    } else {
        getParser()->qfmpError(QObject::tr("struct element access: left-hand-side expression hast to be a struct, but expression is of type %1").arg(resultTypeToString(var.type)));
        result.setInvalid();
        return;
    }
}

QFMathParser::qfmpNode *QFMathParser::qfmpStructAccessNode::copy(QFMathParser::qfmpNode *par)
{
    return new qfmpStructAccessNode(left->copy(NULL), index, getParser(), par);
}

QString QFMathParser::qfmpStructAccessNode::print() const
{
    return QString("(%1).%2").arg(left->print()).arg(index);
}

QString QFMathParser::qfmpStructAccessNode::printTree(int level) const
{
return QString(2*level, QLatin1Char(' '))+QString("StructAccessNode '%2':\n%1").arg(left->printTree(level+1)).arg(index);}
