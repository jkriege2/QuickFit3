/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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

#include "qfmathparsertools.h"
#include "qfmathparser.h"
#include <typeinfo>




qfmpResult::qfmpResult()
{
    isValid=true;
    type=qfmpDouble;
    str="";
    num=0;
    boolean=false;
    numVec.clear();
    boolVec.clear();
    structData.clear();
    strVec.clear();
    m_custom=NULL;    
}

qfmpResult::~qfmpResult()
{
    if (m_custom) delete m_custom;
    m_custom=NULL;
}

qfmpResult::qfmpResult(double value)
{
    isValid=true;
    type=qfmpDouble;
    str="";
    num=value;
    boolean=false;
    numVec.clear();
    boolVec.clear();
    structData.clear();
    strVec.clear();
    m_custom=NULL;
}

qfmpResult::qfmpResult(int value)
{
    isValid=true;
    type=qfmpDouble;
    str="";
    num=value;
    boolean=false;
    numVec.clear();
    boolVec.clear();
    structData.clear();
    strVec.clear();
    m_custom=NULL;
}

qfmpResult::qfmpResult(QString value)
{
    isValid=true;
    type=qfmpString;
    str=value;
    num=0;
    boolean=false;
    numVec.clear();
    boolVec.clear();
    structData.clear();
    strVec.clear();
    m_custom=NULL;
}

qfmpResult::qfmpResult(bool value)
{
    isValid=true;
    type=qfmpBool;
    str="";
    num=0;
    boolean=value;
    numVec.clear();
    boolVec.clear();
    structData.clear();
    strVec.clear();
    m_custom=NULL;
}

qfmpResult::qfmpResult(const QVector<double>& value)
{
    isValid=true;
    type=qfmpDoubleVector;
    str="";
    num=0;
    boolean=false;
    numVec=value;
    boolVec.clear();
    structData.clear();
    strVec.clear();
    m_custom=NULL;
}

qfmpResult::qfmpResult(const QVector<bool> &value)
{
    isValid=true;
    type=qfmpBoolVector;
    str="";
    num=0;
    boolean=false;
    numVec.clear();
    boolVec=value;
    structData.clear();
    strVec.clear();
    m_custom=NULL;
}

qfmpResult::qfmpResult(const QStringList &value)
{
    isValid=true;
    type=qfmpStringVector;
    str="";
    num=0;
    boolean=false;
    numVec.clear();
    boolVec.clear();
    structData.clear();
    strVec=value;
    m_custom=NULL;
}

qfmpResult::qfmpResult(const qfmpResult &value):
    isValid(value.isValid),
    type(value.type),
    num(value.num),
    boolean(value.boolean),
    m_custom(NULL)
{
    if (type==qfmpString) this->str=value.str;
    else if (type==qfmpDoubleVector) this->numVec=value.numVec;
    else if (type==qfmpStringVector) this->strVec=value.strVec;
    else if (type==qfmpBoolVector) this->boolVec=value.boolVec;
    else if (type==qfmpStruct) this->structData=value.structData;
    else if (type==qfmpCustom) this->setCustomCopy(value.custom());
}



qfmpResult &qfmpResult::operator=(const qfmpResult &value)
{
    set(value);
    return *this;
}

void qfmpResult::set(const qfmpResult &value)
{
    isValid=value.isValid;
    type=value.type;
    num=value.num;
    boolean=value.boolean;
    if (type==qfmpString) this->str=value.str;
    else if (type==qfmpDoubleVector) this->numVec=value.numVec;
    else if (type==qfmpStringVector) this->strVec=value.strVec;
    else if (type==qfmpBoolVector) this->boolVec=value.boolVec;
    else if (type==qfmpStruct) this->structData=value.structData;
    else if (type==qfmpCustom) this->setCustomCopy(value.custom());
}

void qfmpResult::setInvalid()
{
    isValid=false;
    numVec.clear();
    boolVec.clear();
    structData.clear();
    strVec.clear();
    str.clear();
    clearCustom();
    type=qfmpVoid;
}

void qfmpResult::setVoid()
{
    isValid=true;
    numVec.clear();
    boolVec.clear();
    structData.clear();
    strVec.clear();
    str.clear();
    clearCustom();
    type=qfmpVoid;
}

QString qfmpResult::toString(int precision) const
{
    switch(type) {
        case qfmpDouble: return doubleToQString(num, precision);
        case qfmpDoubleVector: return QString("[ ")+doubleVecToQString(numVec, precision)+QString(" ]");
        case qfmpStringVector: return QString("[ ")+strVec.join(", ")+QString(" ]");
        case qfmpBoolVector: return QString("[ ")+boolvectorToQString(boolVec)+QString(" ]");
        case qfmpString: return str;
        case qfmpBool: return boolToQString(boolean);
        case qfmpCustom: if (m_custom) return m_custom->toString(precision);
        case qfmpStruct: {
                QStringList sl;
                QMapIterator<QString,qfmpResult> it(structData);
                while (it.hasNext()) {
                    it.next();
                    sl<<QString("%1: %2").arg(it.key()).arg(it.value().toString(precision));
                }
                return QObject::tr("{ %1 }").arg(sl.join(", "));
            }
        case qfmpVoid: break;
    }
    return "";

}

QString qfmpResult::toTypeString(int precision) const
{
    if (!isValid) return QObject::tr("[INVALID]");
    switch(type) {
        case qfmpDouble: return doubleToQString(num, precision)+QObject::tr(" [number]");
        case qfmpDoubleVector: return QString("[ ")+doubleVecToQString(numVec, precision)+QObject::tr(" ] [number vector]");
        case qfmpStringVector: return QString("[ ")+strVec.join(", ")+QObject::tr(" ] [string vector]");
        case qfmpBoolVector: return QString("[ ")+boolvectorToQString(boolVec)+QObject::tr(" ] [boolean vector]");
        case qfmpString: return str+QObject::tr(" [string]");
        case qfmpBool: return boolToQString(boolean)+QObject::tr(" [bool]");
        case qfmpVoid: return QObject::tr(" [void]");
        case qfmpStruct: {
                QStringList sl;
                QMapIterator<QString,qfmpResult> it(structData);
                while (it.hasNext()) {
                    it.next();
                    sl<<QString("%1: %2").arg(it.key()).arg(it.value().toTypeString(precision));
                }
                return QObject::tr("{ %1 } [struct]").arg(sl.join(", "));
            }
        case qfmpCustom: if (m_custom) return m_custom->toTypeString(precision);
    }
    return QFMathParser::resultTypeToString(type);

}

int32_t qfmpResult::toInteger() const
{
    if (type==qfmpDouble) return num;
    if (type==qfmpBool) return (boolean)?1:0;
    return 0;
}

uint32_t qfmpResult::toUInt() const
{
    if (type==qfmpDouble) return num;
    if (type==qfmpBool) return (boolean)?1:0;
    return 0;
}

bool qfmpResult::isInteger() const
{
    return (type==qfmpDouble)&&(fabs(num)<4294967296.0);
}

bool qfmpResult::isUInt() const
{
    return type==qfmpDouble&&(fabs(num)<2147483648.0);
}

int qfmpResult::length() const
{
    if (!isValid) return 0;
    switch(type) {
        case qfmpDouble: return 1;
        case qfmpDoubleVector: return numVec.size();
        case qfmpBoolVector: return boolVec.size();
        case qfmpStringVector: return strVec.size();
        case qfmpString: return str.size();
        case qfmpBool: return 1;
        case qfmpVoid: return 0;
        case qfmpStruct: return structData.size();
        case qfmpCustom: if (m_custom) return m_custom->length();
    }
    return 0;
}

bool qfmpResult::isCustom() const
{
    return type==qfmpCustom;
}

void qfmpResult::setCustomCopy(const qfmpCustomResult *val)
{
    type=qfmpCustom;
    num=0;
    numVec.clear();
    boolVec.clear();
    structData.clear();
    strVec.clear();
    str.clear();
    isValid=true;
    clearCustom();
    m_custom=val->copy();
}

void qfmpResult::setCustom(qfmpCustomResult *val)
{
    type=qfmpCustom;
    num=0;
    numVec.clear();
    boolVec.clear();
    structData.clear();
    strVec.clear();
    str.clear();
    isValid=true;
    clearCustom();
    m_custom=val;
}

void qfmpResult::setDouble(double val)
{
    type=qfmpDouble;
    num=val;
    numVec.clear();
    boolVec.clear();
    structData.clear();
    strVec.clear();
    str.clear();
    isValid=true;
    clearCustom();
}

void qfmpResult::setBoolean(bool val)
{
    type=qfmpBool;
    boolean=val;
    numVec.clear();
    boolVec.clear();
    structData.clear();
    strVec.clear();
    str.clear();
    clearCustom();;
    isValid=true;
}

void qfmpResult::setString(const QString &val)
{
    type=qfmpString;
    str=val;
    boolVec.clear();
    structData.clear();
    strVec.clear();
    numVec.clear();
    clearCustom();;
    isValid=true;
}

void qfmpResult::setString(int size, QChar defaultChar)
{
    type=qfmpString;
    str=QString(size, defaultChar);
    boolVec.clear();
    structData.clear();
    strVec.clear();
    numVec.clear();
    clearCustom();;
    isValid=true;
}

void qfmpResult::setDoubleVec(const QVector<double> &val)
{
    isValid=true;
    type=qfmpDoubleVector;
    boolVec.clear();
    structData.clear();
    strVec.clear();
    numVec=val;
    clearCustom();;
    str.clear();
}

void qfmpResult::setDoubleVec(int size, double defaultVal)
{
    isValid=true;
    type=qfmpDoubleVector;
    boolVec.clear();
    structData.clear();
    strVec.clear();
    clearCustom();;
    if (size>0) {
        numVec.resize(size);
        for (int i=0; i<size; i++) numVec[i]=defaultVal;
    } else {
        numVec.clear();
    }
    str.clear();
}

void qfmpResult::setStruct(const QStringList &items)
{
    isValid=true;
    type=qfmpStruct;
    boolVec.clear();
    strVec.clear();
    clearCustom();;
    numVec.clear();
    str.clear();
    structData.clear();
    for (int i=0; i<items.size(); i++) {
        structData.insert(items[i], qfmpResult::invalidResult());
    }
}

void qfmpResult::setBoolVec(const QVector<bool> &val)
{
    isValid=true;
    type=qfmpBoolVector;
    numVec.clear();
    strVec.clear();
    boolVec=val;
    str.clear();
    structData.clear();
    clearCustom();;
}

void qfmpResult::setBoolVec(int size, bool defaultVal)
{
    isValid=true;
    type=qfmpBoolVector;
    numVec.clear();
    strVec.clear();
    structData.clear();
    clearCustom();;
    if (size>0) {
        boolVec.resize(size);
        for (int i=0; i<size; i++) boolVec[i]=defaultVal;
    } else {
        boolVec.clear();
    }
    str.clear();
}

void qfmpResult::setStringVec(const QStringList &val)
{
    isValid=true;
    type=qfmpStringVector;
    numVec.clear();
    strVec=val;
    boolVec.clear();
    structData.clear();
    clearCustom();;
    str.clear();
}

void qfmpResult::setStringVec(int size, const QString &defaultVal)
{
    isValid=true;
    type=qfmpStringVector;
    numVec.clear();
    boolVec.clear();
    structData.clear();
    if (size>0) {
        strVec.clear();
        for (int i=0; i<size; i++) strVec<<defaultVal;
    } else {
        strVec.clear();
    }
    str.clear();
}

QVector<double> qfmpResult::asVector() const
{
    if (type==qfmpDoubleVector) return numVec;
    else if (type==qfmpBoolVector) return boolvectorToNumVec(boolVec, 1.0, 0.0);
    else if (type==qfmpDouble) return QVector<double>(1, num);
    else if (type==qfmpCustom && custom()) {
        if (custom()->convertsTo(qfmpDoubleVector)) {
            qfmpResult res;
            custom()->convertTo(res,qfmpDoubleVector);
            if (res.isValid && res.type==qfmpDoubleVector) return res.numVec;
        }
    }
    return QVector<double>();
}

QVariantList qfmpResult::asVariantList() const
{
    QVariantList vl;
    if (type==qfmpDouble) {
        vl<<num;
    } else if (type==qfmpString) {
        vl<<str;
    } else if (type==qfmpBool) {
        vl<<boolean;
    } else if (type==qfmpDoubleVector) {
        for (int i=0; i<numVec.size(); i++) {
            vl<<numVec[i];
        }
    } else if (type==qfmpStringVector) {
        for (int i=0; i<strVec.size(); i++) {
            vl<<strVec[i];
        }
    } else if (type==qfmpBoolVector) {
        for (int i=0; i<boolVec.size(); i++) {
            vl<<boolVec[i];
        }
    } else if (type==qfmpCustom && m_custom) {
        return m_custom->asVariantList();
    }
     return vl;
}

bool qfmpResult::setVariant(const QVariant& data, bool convertLists) {
    setInvalid();
    switch (data.type()) {
        case QVariant::Bool: setBoolean(data.toBool()); break;
        case QVariant::Double:
        case QVariant::Int:
        case QVariant::UInt:
        case QVariant::ULongLong:
        case QVariant::LongLong: setDouble(data.toDouble()); break;

        case QVariant::String:
        case QVariant::ByteArray:
        case QVariant::Url: setString(data.toString()); break;
        case QVariant::Char: setString(data.toChar()); break;
        case QVariant::Date:
        case QVariant::DateTime: setDouble(data.toDateTime().toMSecsSinceEpoch()); break;
        case QVariant::StringList: setStringVec(data.toStringList()); break;
        case QVariant::Time: setDouble(QDateTime(QDate::currentDate(), data.toTime()).toMSecsSinceEpoch()); break;
        case QVariant::Point: setDoubleVec(constructQVectorFromItems<double>(data.toPoint().x(), data.toPoint().y())); break;
        case QVariant::PointF: setDoubleVec(constructQVectorFromItems<double>(data.toPointF().x(), data.toPointF().y())); break;
        case QVariant::Size: setDoubleVec(constructQVectorFromItems<double>(data.toSize().width(), data.toSize().height())); break;
        case QVariant::SizeF: setDoubleVec(constructQVectorFromItems<double>(data.toSizeF().width(), data.toSizeF().height())); break;
        case QVariant::Rect: setDoubleVec(constructQVectorFromItems<double>(data.toRect().x(), data.toRect().y(), data.toRect().width(), data.toRect().height())); break;
        case QVariant::RectF: setDoubleVec(constructQVectorFromItems<double>(data.toRectF().x(), data.toRectF().y(), data.toRectF().width(), data.toRectF().height())); break;
        case QVariant::Invalid: setInvalid(); break;
        case QVariant::List: {
                QVariantList vl=data.toList();
                if (vl.size()>0 && convertLists) {
                    switch (vl[0].type()) {
                        case QVariant::Bool: { isValid=true; type=qfmpBoolVector; boolVec.clear(); for (int i=0; i<vl.size(); i++) boolVec<<vl[i].toBool();} break;
                        case QVariant::Double:
                        case QVariant::Int:
                        case QVariant::UInt:
                        case QVariant::ULongLong:
                        case QVariant::LongLong: {isValid=true; type=qfmpDoubleVector; numVec.clear(); for (int i=0; i<vl.size(); i++) numVec<<vl[i].toDouble();} break;
                        case QVariant::Date:
                        case QVariant::DateTime: {isValid=true; type=qfmpDoubleVector; numVec.clear(); for (int i=0; i<vl.size(); i++) numVec<<vl[i].toDateTime().toMSecsSinceEpoch();} break;
                        case QVariant::String:
                        case QVariant::ByteArray:
                        case QVariant::Url:
                        default: {isValid=true; type=qfmpStringVector; strVec.clear(); for (int i=0; i<vl.size(); i++) strVec<<vl[i].toString();} break;
                    }
                } else {
                }
            } break;

        default: setString(data.toString()); break;
    }
    return true;
}

QVariant qfmpResult::asVariant() const
{
    QVariant vl;
    if (type==qfmpDouble) {
        vl=num;
        //if (isInteger()) vl=toInteger();
    } else if (type==qfmpString) {
        vl=str;
    } else if (type==qfmpBool) {
        vl=boolean;
    } else if (type==qfmpDoubleVector) {
        vl=asVariantList();
    } else if (type==qfmpStringVector) {
        vl=asVariantList();
    } else if (type==qfmpBoolVector) {
        vl=asVariantList();
    } else if (type==qfmpCustom && m_custom) {
        QVariantList l=m_custom->asVariantList();
        if (l.size()==1) vl=l[0];
        else vl=l;

    }
     return vl;
}

QStringList qfmpResult::asStrVector() const
{
    if (type==qfmpStringVector) return strVec;
    else if (type==qfmpString)  return QStringList(str);
    else if (type==qfmpCustom && custom())  {
        qfmpResult res;
        custom()->convertTo(res, qfmpStringVector);
        if (res.isValid && res.type==qfmpStringVector) return res.strVec;
    }
    return QStringList();
}

QVector<bool> qfmpResult::asBoolVector() const
{
    if (type==qfmpBoolVector) return boolVec;
    else if (type==qfmpBool) return QVector<bool>(1, boolean);
    else if (type==qfmpCustom && custom())  {
        qfmpResult res;
        custom()->convertTo(res, qfmpBoolVector);
        if (res.isValid && res.type==qfmpBoolVector) return res.boolVec;
    }

    return QVector<bool>();
}

bool qfmpResult::convertsToVector() const
{
    if (type==qfmpDoubleVector || type==qfmpBoolVector || type==qfmpDouble) return true;
    if (type==qfmpCustom && custom()) return custom()->convertsTo(qfmpDoubleVector);
    return false;
}

bool qfmpResult::convertsToBoolVector() const
{
    if (type==qfmpBool || type==qfmpBoolVector) return true;
    if (type==qfmpCustom && custom()) return custom()->convertsTo(qfmpBoolVector);
    return false;
}

bool qfmpResult::convertsToStringVector() const
{
    if (type==qfmpStringVector || type==qfmpString) return true;
    if (type==qfmpCustom && custom()) return custom()->convertsTo(qfmpStringVector);
    return false;
}

bool qfmpResult::convertsToIntVector() const
{
    return convertsToVector();
}

bool qfmpResult::isUsableResult() const
{
    return isValid && (type!=qfmpVoid);
}

double qfmpResult::asNumber() const
{
    if (type==qfmpDouble) return num;
    if (type==qfmpBool) return boolean?1:0;
    return NAN;
}

double qfmpResult::asNumberAlsoVector() const
{
    if (type==qfmpDouble) return num;
    if (type==qfmpDoubleVector && numVec.size()>0) return numVec[0];
    return NAN;
}

QString qfmpResult::asStringAlsoVector() const
{
    if (type==qfmpString) return str;
    if (type==qfmpStringVector && strVec.size()>0) return strVec[0];
    return QString();
}

bool qfmpResult::asBooleanAlsoVector() const
{
    if (type==qfmpBool) return boolean;
    if (type==qfmpBoolVector && boolVec.size()>0) return boolVec[0];
    return false;
}

QString qfmpResult::asString() const
{
    if (type==qfmpString) return str;
    return QString();
}

bool qfmpResult::asBool() const
{
    if (type==qfmpBool) return boolean;
    if (type==qfmpDouble) return num!=0;
    return false;
}

qfmpResultType qfmpResult::getType() const
{
    return type;
}

qfmpResult qfmpResult::invalidResult()
{
    qfmpResult r;
    r.isValid=false;
    return r;
}

qfmpResult qfmpResult::voidResult()
{
    qfmpResult r;
    r.isValid=true;
    r.type=qfmpVoid;
    return r;
}

qfmpResult qfmpResult::getStructItem(const QString &item)
{
    qfmpResult res;
    if (type==qfmpStruct) {
        if (structData.contains(item)) res=structData[item];
    }
    return res;
}

void qfmpResult::setStructItem(const QString &item, const qfmpResult &value)
{
    if (type==qfmpStruct) {
        if (structData.contains(item)) structData[item]=value;
        else structData.insert(item, value);
    }
}

void qfmpResult::setStruct(const QMap<QString, qfmpResult> &data)
{
    setStruct();
    structData=data;
}




bool qfmpResult::operator!=(const qfmpResult &other) const
{
    return !operator==(other);
}


bool qfmpResult::operator==(const qfmpResult &r) const
{
    if (!isValid && !r.isValid) return true;
    switch((uint32_t(type)<<16)+uint32_t(r.type)) {
        case (qfmpVoid<<16)+qfmpVoid:
            return  true;
            break;
        case (qfmpDouble<<16)+qfmpDouble:
            return(num==r.num);
            break;
        case (qfmpDoubleVector<<16)+qfmpDoubleVector:
            return(numVec==r.numVec);
            break;
        case (qfmpString<<16)+qfmpString:
            return(str==r.str);
            break;
        case (qfmpBool<<16)+qfmpBool:
            return(boolean==r.boolean);
            break;
        case (qfmpBoolVector<<16)+qfmpBoolVector:
            return(boolVec==r.boolVec);
            break;
        case (qfmpStringVector<<16)+qfmpStringVector:
            return(strVec==r.strVec);
            break;
        case (qfmpStruct<<16)+qfmpStruct:
            return(structData==r.structData);
            break;
        case (qfmpCustom<<16)+qfmpCustom: {
                if (custom() && r.custom()) {
                    qfmpResult res;
                    custom()->compareequal(res, *this, r, NULL);
                    return (res.asBool());
                }
            }
            break;
        default:
            return false;
    }
    return false;
}



//================================================================================================================


void qfmpResult::add(qfmpResult& re, const qfmpResult &l, const qfmpResult &r, QFMathParser* p)
{
    uint32_t lt=l.type;
    uint32_t rt=r.type;
    uint32_t tt=(lt<<16)+rt;
    /*qDebug()<<QString::number(tt, 16);
    //qDebug()<<QString::number((uint32_t(qfmpDouble)<<16)+qfmpDouble, 16);
    //qDebug()<<QString::number((uint32_t(qfmpDouble)<<16)+qfmpDoubleVector, 16);
    //qDebug()<<QString::number((uint32_t(qfmpDoubleVector<<16))+qfmpDoubleVector, 16);
    //qDebug()<<QString::number((uint32_t(qfmpString)<<16)+qfmpString, 16);*/
    switch(tt) {
        case (uint32_t(qfmpDouble)<<16)+qfmpDouble:
            re.setDouble(l.num+r.num);
            break;
        case (qfmpDoubleVector<<16)+qfmpDouble: {
            re.setDoubleVec(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                re.numVec[i]=l.numVec[i]+r.num;
            }
            break;
        }
        case (uint32_t(qfmpDouble)<<16)+qfmpDoubleVector: {
            re.setDoubleVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=l.num+r.numVec[i];
            }
            break;
        }
        case (uint32_t(qfmpDoubleVector<<16))+qfmpDoubleVector: {
            if (l.length()!=r.length()) {
                if (p) p->qfmpError(QObject::tr("arguments have to have the same length, but lengthes were left=%1, right=%2").arg(l.length()).arg(r.length()));
                re.setInvalid();
                return;
            }
            re.setDoubleVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=l.numVec[i]+r.numVec[i];
            }
            break;
        }
        case (uint32_t(qfmpString)<<16)+qfmpString:
            re.setString(l.str+r.str);
            break;
        default:
            if (l.type==qfmpCustom && l.custom()) l.custom()->add(re, l, r, p);
            else if (r.type==qfmpCustom && r.custom()) r.custom()->add(re, l, r, p);
            else {
                if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be added").arg(l.typeName()).arg(r.typeName()));
                re.setInvalid();
            }
            break;
    }
}




void qfmpResult::sub(qfmpResult& re, const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    uint32_t lt=l.type;
    uint32_t rt=r.type;
    uint32_t tt=(lt<<16)+rt;
    //qDebug()<<QString::number(tt, 16);
    switch(tt) {
        case (qfmpDouble<<16)+qfmpDouble:
            re.setDouble(l.num-r.num);
            break;
        case (qfmpDoubleVector<<16)+qfmpDouble: {
            re.setDoubleVec(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                re.numVec[i]=l.numVec[i]-r.num;
            }
            break;
        }
        case (qfmpDouble<<16)+qfmpDoubleVector: {
            re.setDoubleVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=l.num-r.numVec[i];
            }
            break;
        }
        case (qfmpDoubleVector<<16)+qfmpDoubleVector: {
            if (l.length()!=r.length()) {
                if (p) p->qfmpError(QObject::tr("arguments have to have the same length, but lengthes were left=%1, right=%2").arg(l.length()).arg(r.length()));
                re.setInvalid();
                return;
            }
            re.setDoubleVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=l.numVec[i]-r.numVec[i];
            }
            break;
        }
        default:
            if (l.type==qfmpCustom && l.custom()) l.custom()->sub(re, l, r, p);
            else if (r.type==qfmpCustom && r.custom()) r.custom()->sub(re, l, r, p);
            else {
                if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be subtracted").arg(l.typeName()).arg(r.typeName()));
                re.setInvalid();
            }
            break;
    }
}

void qfmpResult::mul(qfmpResult& re, const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    uint32_t lt=l.type;
    uint32_t rt=r.type;
    uint32_t tt=(lt<<16)+rt;
    //qDebug()<<QString::number(tt, 16);
    switch(tt) {
        case (qfmpDouble<<16)+qfmpDouble:
            re.setDouble(l.num*r.num);
            break;
        case (qfmpDoubleVector<<16)+qfmpDouble: {
            re.setDoubleVec(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                re.numVec[i]=l.numVec[i]*r.num;
            }
            break;
        }
        case (qfmpDouble<<16)+qfmpDoubleVector: {
            re.setDoubleVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=l.num*r.numVec[i];
            }
            break;
        }
        case (qfmpDoubleVector<<16)+qfmpDoubleVector: {
            if (l.length()!=r.length()) {
                if (p) p->qfmpError(QObject::tr("arguments have to have the same length, but lengthes were left=%1, right=%2").arg(l.length()).arg(r.length()));
                re.setInvalid();
                return;
            }
            re.setDoubleVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=l.numVec[i]*r.numVec[i];
            }
            break;
        }
        default:
            if (l.type==qfmpCustom && l.custom()) l.custom()->mul(re, l, r, p);
            else if (l.type==qfmpCustom && r.custom()) r.custom()->mul(re, l, r, p);
            else {
                if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be multiplied").arg(l.typeName()).arg(r.typeName()));
                re.setInvalid();
            }
            break;
    }
}

void qfmpResult::div(qfmpResult& re, const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    uint32_t lt=l.type;
    uint32_t rt=r.type;
    uint32_t tt=(lt<<16)+rt;
    //qDebug()<<QString::number(tt, 16);
    switch(tt) {
        case (qfmpDouble<<16)+qfmpDouble:
            re.setDouble(l.num/r.num);
            break;
        case (qfmpDoubleVector<<16)+qfmpDouble: {
            re.setDoubleVec(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                re.numVec[i]=l.numVec[i]/r.num;
            }
            break;
        }
        case (qfmpDouble<<16)+qfmpDoubleVector: {
            re.setDoubleVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=l.num/r.numVec[i];
            }
            break;
        }
        case (qfmpDoubleVector<<16)+qfmpDoubleVector: {
            if (l.length()!=r.length()) {
                if (p) p->qfmpError(QObject::tr("arguments have to have the same length, but lengthes were left=%1, right=%2").arg(l.length()).arg(r.length()));
                re.setInvalid();
                return;
            }
            re.setDoubleVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=l.numVec[i]/r.numVec[i];
            }
            break;
        }
        default:
            if (l.type==qfmpCustom && l.custom()) l.custom()->div(re, l, r, p);
            else if (l.type==qfmpCustom && r.custom()) r.custom()->div(re, l, r, p);
            else {
                re.setInvalid();
                if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be divided").arg(l.typeName()).arg(r.typeName()));
            }
            break;
    }
}

void qfmpResult::mod(qfmpResult& re, const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    uint32_t lt=l.type;
    uint32_t rt=r.type;
    uint32_t tt=(lt<<16)+rt;
    //qDebug()<<QString::number(tt, 16);
    switch(tt) {
        case (qfmpDouble<<16)+qfmpDouble:
            re.setDouble(double(l.toInteger()%r.toInteger()));
            break;
        case (qfmpDoubleVector<<16)+qfmpDouble: {
            re.setDoubleVec(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                re.numVec[i]=int32_t(l.numVec[i])%r.toInteger();
            }
            break;
        }
        case (qfmpDouble<<16)+qfmpDoubleVector: {
            re.setDoubleVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=l.toInteger()%int32_t(r.numVec[i]);
            }
            break;
        }
        case (qfmpDoubleVector<<16)+qfmpDoubleVector: {
            if (l.length()!=r.length()) {
                if (p) p->qfmpError(QObject::tr("arguments have to have the same length, but lengthes were left=%1, right=%2").arg(l.length()).arg(r.length()));
                re.setInvalid();
                return;
            }
            re.setDoubleVec(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                re.numVec[i]=int32_t(l.numVec[i])%int32_t(r.numVec[i]);
            }
            break;
        }
        default:
            if (l.type==qfmpCustom && l.custom()) l.custom()->mod(re, l, r, p);
            else if (l.type==qfmpCustom && r.custom()) r.custom()->mod(re, l, r, p);
            else {
                if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be used in mod operation").arg(l.typeName()).arg(r.typeName()));
                re.setInvalid();
            }
            break;
    }
}

void qfmpResult::power(qfmpResult& re, const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    uint32_t lt=l.type;
    uint32_t rt=r.type;
    uint32_t tt=(lt<<16)+rt;
    //qDebug()<<QString::number(tt, 16);
    switch(tt) {
        case (qfmpDouble<<16)+qfmpDouble:
            re.setDouble(pow(l.num,r.num));
            break;
        case (qfmpDoubleVector<<16)+qfmpDouble: {
            re.setDoubleVec(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                re.numVec[i]=pow(l.numVec[i],r.num);
            }
            break;
        }
        case (qfmpDouble<<16)+qfmpDoubleVector: {
            re.setDoubleVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=pow(l.num,r.numVec[i]);
            }
            break;
        }
        case (qfmpDoubleVector<<16)+qfmpDoubleVector: {
            if (l.length()!=r.length()) {
                if (p) p->qfmpError(QObject::tr("arguments have to have the same length, but lengthes were left=%1, right=%2").arg(l.length()).arg(r.length()));
                re.setInvalid();
                return;
            }
            re.setDoubleVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=pow(l.numVec[i],r.numVec[i]);
            }
            break;
        }
        default:
            if (l.type==qfmpCustom && l.custom()) l.custom()->power(re, l, r, p);
            else if (l.type==qfmpCustom && r.custom()) r.custom()->power(re, l, r, p);
            else {
                re.setInvalid();
                if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be used in power operation").arg(l.typeName()).arg(r.typeName()));
            }
            break;
    }
}

void qfmpResult::bitwiseand(qfmpResult& re, const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    uint32_t lt=l.type;
    uint32_t rt=r.type;
    uint32_t tt=(lt<<16)+rt;
    //qDebug()<<QString::number(tt, 16);
    switch(tt) {
        case (qfmpDouble<<16)+qfmpDouble:
            re.setDouble(double(l.toInteger()&r.toInteger()));
            break;
        case (qfmpDoubleVector<<16)+qfmpDouble: {
            re.setDoubleVec(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                re.numVec[i]=int32_t(l.numVec[i])&r.toInteger();
            }
            break;
        }
        case (qfmpDouble<<16)+qfmpDoubleVector: {
            re.setDoubleVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=l.toInteger()&int32_t(r.numVec[i]);
            }
            break;
        }
        case (qfmpDoubleVector<<16)+qfmpDoubleVector: {
            if (l.length()!=r.length()) {
                if (p) p->qfmpError(QObject::tr("arguments have to have the same length, but lengthes were left=%1, right=%2").arg(l.length()).arg(r.length()));
                re.setInvalid();
                return;
            }
            re.setDoubleVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=int32_t(l.numVec[i])&int32_t(r.numVec[i]);
            }
            break;
        }
        default:
            if (l.type==qfmpCustom && l.custom()) l.custom()->bitwiseand(re, l, r, p);
            else if (l.type==qfmpCustom && r.custom()) r.custom()->bitwiseand(re, l, r, p);
            else {
                if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be used in bitwise and operation").arg(l.typeName()).arg(r.typeName()));
                re.setInvalid();
            }
            break;
    }
}

void qfmpResult::bitwiseor(qfmpResult& re, const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    uint32_t lt=l.type;
    uint32_t rt=r.type;
    uint32_t tt=(lt<<16)+rt;
    //qDebug()<<QString::number(tt, 16);
    switch(tt) {
        case (qfmpDouble<<16)+qfmpDouble:
            re.setDouble(double(l.toInteger()|r.toInteger()));
            break;
        case (qfmpDoubleVector<<16)+qfmpDouble: {
            re.setDoubleVec(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                re.numVec[i]=int32_t(l.numVec[i])|r.toInteger();
            }
            break;
        }
        case (qfmpDouble<<16)+qfmpDoubleVector: {
            re.setDoubleVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=l.toInteger()|int32_t(r.numVec[i]);
            }
            break;
        }
        case (qfmpDoubleVector<<16)+qfmpDoubleVector: {
            if (l.length()!=r.length()) {
                if (p) p->qfmpError(QObject::tr("arguments have to have the same length, but lengthes were left=%1, right=%2").arg(l.length()).arg(r.length()));
                re.setInvalid();
                return;
            }
            re.setDoubleVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=int32_t(l.numVec[i])|int32_t(r.numVec[i]);
            }
            break;
        }
        default:
            if (l.type==qfmpCustom && l.custom()) l.custom()->bitwiseor(re, l, r, p);
            else if (l.type==qfmpCustom && r.custom()) r.custom()->bitwiseor(re, l, r, p);
            else {
                re.setInvalid();
                if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be used in bitwise or operation").arg(l.typeName()).arg(r.typeName()));
            }
            break;
    }
}

void qfmpResult::logicand(qfmpResult& re, const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    uint32_t lt=l.type;
    uint32_t rt=r.type;
    uint32_t tt=(lt<<16)+rt;
    //qDebug()<<QString::number(tt, 16);
    switch(tt) {
        case (qfmpBool<<16)+qfmpBool:
            re.setBoolean(l.boolean&&r.boolean);
            break;
        case (qfmpBoolVector<<16)+qfmpBoolVector: {
            if (l.length()!=r.length()) {
                if (p) p->qfmpError(QObject::tr("arguments have to have the same length, but lengthes were left=%1, right=%2").arg(l.length()).arg(r.length()));
                re.setInvalid();
                return;
            }
            re.setBoolVec(r.boolVec.size());
            for (int i=0; i<r.boolVec.size(); i++) {
                re.boolVec[i]=l.boolVec[i]&&r.boolVec[i];
            }
            break;
        }
        case (qfmpBoolVector<<16)+qfmpBool: {
            re.setBoolVec(l.boolVec.size());
            for (int i=0; i<l.boolVec.size(); i++) {
                re.boolVec[i]=l.boolVec[i]&&r.boolean;
            }
            break;
        }

        default:
            if (l.type==qfmpCustom && l.custom()) l.custom()->logicand(re, l, r, p);
            else if (l.type==qfmpCustom && r.custom()) r.custom()->logicand(re, l, r, p);
            else {
                re.setInvalid();
                if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be used in logic and operation").arg(l.typeName()).arg(r.typeName()));
            }
            break;
    }
}

void qfmpResult::logicor(qfmpResult& re, const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    uint32_t lt=l.type;
    uint32_t rt=r.type;
    uint32_t tt=(lt<<16)+rt;
    //qDebug()<<QString::number(tt, 16);
    switch(tt) {
        case (qfmpBool<<16)+qfmpBool:
            re.setBoolean(l.boolean||r.boolean);
            break;
        case (qfmpBoolVector<<16)+qfmpBoolVector: {
            if (l.length()!=r.length()) {
                if (p) p->qfmpError(QObject::tr("arguments have to have the same length, but lengthes were left=%1, right=%2").arg(l.length()).arg(r.length()));
                re.setInvalid();
                return;
            }
            re.setBoolVec(r.boolVec.size());
            for (int i=0; i<r.boolVec.size(); i++) {
                re.boolVec[i]=l.boolVec[i]||r.boolVec[i];
            }
            break;
        }
        case (qfmpBoolVector<<16)+qfmpBool: {
            re.setBoolVec(l.boolVec.size());
            for (int i=0; i<l.boolVec.size(); i++) {
                re.boolVec[i]=l.boolVec[i]||r.boolean;
            }
            break;
        }
        default:
            if (l.type==qfmpCustom && l.custom()) l.custom()->logicor(re, l, r, p);
            else if (l.type==qfmpCustom && r.custom()) r.custom()->logicor(re, l, r, p);
            else {
                re.setInvalid();
                if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be used in logic or operation").arg(l.typeName()).arg(r.typeName()));
            }
            break;
    }
}

void qfmpResult::logicnot(qfmpResult& re, const qfmpResult &l, QFMathParser *p)
{
    switch(l.type) {
        case qfmpBool: re.setBoolean(!l.boolean); break;
        case qfmpBoolVector: {
            re.setBoolVec(l.boolVec.size());
            for (int i=0; i<l.boolVec.size(); i++) {
                re.boolVec[i]=!l.boolVec[i];
            }
            break;
        }
        default:
            if (l.type==qfmpCustom && l.custom()) l.custom()->logicnot(re, l, p);
            else {
                re.setInvalid();
                if (p) p->qfmpError(QObject::tr("argument of type %1 may not be used in logic not operation").arg(l.typeName()));
            }
            break;

    }

}

void qfmpResult::logicnand(qfmpResult &re, const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    uint32_t lt=l.type;
    uint32_t rt=r.type;
    uint32_t tt=(lt<<16)+rt;
    //qDebug()<<QString::number(tt, 16);
    switch(tt) {
        case (qfmpBool<<16)+qfmpBool:
            re.setBoolean(!(l.boolean&&r.boolean));
            break;
        case (qfmpBoolVector<<16)+qfmpBoolVector: {
            if (l.length()!=r.length()) {
                if (p) p->qfmpError(QObject::tr("arguments have to have the same length, but lengthes were left=%1, right=%2").arg(l.length()).arg(r.length()));
                re.setInvalid();
                return;
            }
            re.setBoolVec(r.boolVec.size());
            for (int i=0; i<r.boolVec.size(); i++) {
                re.boolVec[i]=!(l.boolVec[i]&&r.boolVec[i]);
            }
            break;
        }
        case (qfmpBoolVector<<16)+qfmpBool: {
            re.setBoolVec(l.boolVec.size());
            for (int i=0; i<l.boolVec.size(); i++) {
                re.boolVec[i]=!(l.boolVec[i]&&r.boolean);
            }
            break;
        }
        default:
            if (l.type==qfmpCustom && l.custom()) l.custom()->logicand(re, l, r, p);
            else if (l.type==qfmpCustom && r.custom()) r.custom()->logicand(re, l, r, p);
            else {
                re.setInvalid();
                if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be used in logic nand operation").arg(l.typeName()).arg(r.typeName()));
            }
            break;
    }
}

void qfmpResult::logicnor(qfmpResult &re, const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    uint32_t lt=l.type;
    uint32_t rt=r.type;
    uint32_t tt=(lt<<16)+rt;
    //qDebug()<<QString::number(tt, 16);
    switch(tt) {
        case (qfmpBool<<16)+qfmpBool:
            re.setBoolean(!(l.boolean||r.boolean));
            break;
        case (qfmpBoolVector<<16)+qfmpBoolVector: {
            if (l.length()!=r.length()) {
                if (p) p->qfmpError(QObject::tr("arguments have to have the same length, but lengthes were left=%1, right=%2").arg(l.length()).arg(r.length()));
                re.setInvalid();
                return;
            }
            re.setBoolVec(r.boolVec.size());
            for (int i=0; i<r.boolVec.size(); i++) {
                re.boolVec[i]=!(l.boolVec[i]||r.boolVec[i]);
            }
            break;
        }
        case (qfmpBoolVector<<16)+qfmpBool: {
            re.setBoolVec(l.boolVec.size());
            for (int i=0; i<l.boolVec.size(); i++) {
                re.boolVec[i]=!(l.boolVec[i]||r.boolean);
            }
            break;
        }
        default:
            if (l.type==qfmpCustom && l.custom()) l.custom()->logicnor(re, l, r, p);
            else if (l.type==qfmpCustom && r.custom()) r.custom()->logicnor(re, l, r, p);
            else {
                re.setInvalid();
                if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be used in logic nor operation").arg(l.typeName()).arg(r.typeName()));
            }
            break;
    }
}

void qfmpResult::logicxor(qfmpResult &re, const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    uint32_t lt=l.type;
    uint32_t rt=r.type;
    uint32_t tt=(lt<<16)+rt;
    //qDebug()<<QString::number(tt, 16);
    switch(tt) {
        case (qfmpBool<<16)+qfmpBool:
            re.setBoolean((l.boolean&&!r.boolean)||(!l.boolean&&r.boolean));
            break;
        case (qfmpBoolVector<<16)+qfmpBoolVector: {
            if (l.length()!=r.length()) {
                if (p) p->qfmpError(QObject::tr("arguments have to have the same length, but lengthes were left=%1, right=%2").arg(l.length()).arg(r.length()));
                re.setInvalid();
                return;
            }
            re.setBoolVec(r.boolVec.size());
            for (int i=0; i<r.boolVec.size(); i++) {
                re.boolVec[i]=(!l.boolVec[i]&&r.boolVec[i])||(l.boolVec[i]&&!r.boolVec[i]);
            }
            break;
        }
        case (qfmpBoolVector<<16)+qfmpBool: {
            re.setBoolVec(l.boolVec.size());
            for (int i=0; i<l.boolVec.size(); i++) {
                re.boolVec[i]=(!l.boolVec[i]&&r.boolean)||(l.boolVec[i]&&!r.boolean);
            }
            break;
        }
        default:
            if (l.type==qfmpCustom && l.custom()) l.custom()->logicxor(re, l, r, p);
            else if (l.type==qfmpCustom && r.custom()) r.custom()->logicxor(re, l, r, p);
            else {
                re.setInvalid();
                if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be used in logic xor operation").arg(l.typeName()).arg(r.typeName()));
            }
            break;
    }
}

void qfmpResult::neg(qfmpResult& re, const qfmpResult &l, QFMathParser *p)
{
    switch(l.type) {
        case qfmpDouble: re.setDouble(-l.num); break;
        case qfmpDoubleVector: {
            re.setDoubleVec(l.numVec);
            for (int i=0; i<l.numVec.size(); i++) {
                re.numVec[i]=l.numVec[i];
            }
            break;
        }
        default:
            if (l.type==qfmpCustom && l.custom()) l.custom()->neg(re, l, p);
            else {
                re.setInvalid();
                if (p) p->qfmpError(QObject::tr("argument of type %1 may not be used in negation operation").arg(l.typeName()));
            }
            break;

    }
}

void qfmpResult::bitwisenot(qfmpResult& re, const qfmpResult &l, QFMathParser *p)
{
    switch(l.type) {
        case qfmpDouble: re.setDouble(double(~l.toInteger())); break;
        case qfmpDoubleVector: {
            re.setDoubleVec(l.numVec);
            for (int i=0; i<l.numVec.size(); i++) {
                re.numVec[i]=~int32_t(l.numVec[i]);
            }
            break;
        }
        default:
            if (l.type==qfmpCustom && l.custom()) l.custom()->bitwisenot(re, l, p);
            else {
                re.setInvalid();
                if (p) p->qfmpError(QObject::tr("argument of type %1 may not be used in bitwise not operation").arg(l.typeName()));
            }
            break;

    }

}

void qfmpResult::compareequal(qfmpResult& res, const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    switch((uint32_t(l.type)<<16)+uint32_t(r.type)) {
        case (qfmpDouble<<16)+qfmpDouble:
            res.setBoolean(l.num==r.num);
            break;
        case (qfmpDoubleVector<<16)+qfmpDoubleVector: {
            if (l.numVec.size()!=r.numVec.size()) {
                res.setInvalid();
                if (p) p->qfmpError(QObject::tr("vectors in comparison '==' have different length"));
                return;
            }
            res.setBoolVec(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                res.boolVec[i]=(l.numVec[i]==r.numVec[i]);
            }
            } break;
        case (qfmpDoubleVector<<16)+qfmpDouble: {
            res.setBoolVec(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                res.boolVec[i]=(l.numVec[i]==r.num);
            }
            } break;
        case (qfmpDouble<<16)+qfmpDoubleVector: {
            res.setBoolVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                res.boolVec[i]=(r.numVec[i]==l.num);
            }
            } break;

        case (qfmpString<<16)+qfmpString:
            res.setBoolean(l.str==r.str);
            break;
        case (qfmpStringVector<<16)+qfmpStringVector: {
            if (l.strVec.size()!=r.strVec.size()) {
                res.setInvalid();
                if (p) p->qfmpError(QObject::tr("vectors in comparison '==' have different length"));
                return;
            }
            res.setBoolVec(l.strVec.size());
            for (int i=0; i<l.strVec.size(); i++) {
                res.boolVec[i]=(l.strVec[i]==r.strVec[i]);
            }
            } break;
        case (qfmpStringVector<<16)+qfmpString: {
            res.setBoolVec(l.strVec.size());
            for (int i=0; i<l.strVec.size(); i++) {
                res.boolVec[i]=(l.strVec[i]==r.str);
            }
            } break;
        case (qfmpString<<16)+qfmpStringVector: {
            res.setBoolVec(r.strVec.size());
            for (int i=0; i<r.strVec.size(); i++) {
                res.boolVec[i]=(r.strVec[i]==l.str);
            }
            } break;


        case (qfmpBool<<16)+qfmpBool:
            res.setBoolean(l.boolean==r.boolean);
            break;
        case (qfmpBoolVector<<16)+qfmpBoolVector: {
            if (l.boolVec.size()!=r.boolVec.size()) {
                res.setInvalid();
                if (p) p->qfmpError(QObject::tr("vectors in comparison '==' have different length"));
                return;
            }
            res.setBoolVec(l.boolVec.size());
            for (int i=0; i<l.boolVec.size(); i++) {
                res.boolVec[i]=(l.boolVec[i]==r.boolVec[i]);
            }
            } break;
        case (qfmpBoolVector<<16)+qfmpBool: {
            res.setBoolVec(l.boolVec.size());
            for (int i=0; i<l.boolVec.size(); i++) {
                res.boolVec[i]=(l.boolVec[i]==r.boolean);
            }
            } break;
        case (qfmpBool<<16)+qfmpBoolVector: {
            res.setBoolVec(r.boolVec.size());
            for (int i=0; i<r.boolVec.size(); i++) {
                res.boolVec[i]=(r.boolVec[i]==l.boolean);
            }
            } break;
        default:
            if (l.type==qfmpCustom && l.custom()) l.custom()->compareequal(res, l, r, p);
            else if (l.type==qfmpCustom && r.custom()) r.custom()->compareequal(res, l, r, p);
            else {
                res.setInvalid();
                if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be compared '=='").arg(l.typeName()).arg(r.typeName()));
            }
            break;
    }
}

void qfmpResult::comparenotequal(qfmpResult& res, const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    switch((uint32_t(l.type)<<16)+uint32_t(r.type)) {
        case (qfmpDouble<<16)+qfmpDouble:
            res.setBoolean(l.num!=r.num);
            break;
        case (qfmpDoubleVector<<16)+qfmpDoubleVector: {
            if (l.numVec.size()!=r.numVec.size()) {
                res.setInvalid();
                if (p) p->qfmpError(QObject::tr("vectors in comparison '!=' have different length"));
                return;
            }
            res.setBoolVec(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                res.boolVec[i]=(l.numVec[i]!=r.numVec[i]);
            }
            } break;
        case (qfmpDoubleVector<<16)+qfmpDouble: {
            res.setBoolVec(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                res.boolVec[i]=(l.numVec[i]!=r.num);
            }
            } break;
        case (qfmpDouble<<16)+qfmpDoubleVector: {
            res.setBoolVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                res.boolVec[i]=(r.numVec[i]!=l.num);
            }
            } break;

        case (qfmpString<<16)+qfmpString:
            res.setBoolean(l.str!=r.str);
            break;
        case (qfmpStringVector<<16)+qfmpStringVector: {
            if (l.strVec.size()!=r.strVec.size()) {
                res.setInvalid();
                if (p) p->qfmpError(QObject::tr("vectors in comparison '!=' have different length"));
                return;
            }
            res.setBoolVec(l.strVec.size());
            for (int i=0; i<l.strVec.size(); i++) {
                res.boolVec[i]=(l.strVec[i]!=r.strVec[i]);
            }
            } break;
        case (qfmpStringVector<<16)+qfmpString: {
            res.setBoolVec(l.strVec.size());
            for (int i=0; i<l.strVec.size(); i++) {
                res.boolVec[i]=(l.strVec[i]!=r.str);
            }
            } break;
        case (qfmpString<<16)+qfmpStringVector: {
            res.setBoolVec(r.strVec.size());
            for (int i=0; i<r.strVec.size(); i++) {
                res.boolVec[i]=(r.strVec[i]!=l.str);
            }
            } break;


        case (qfmpBool<<16)+qfmpBool:
            res.setBoolean(l.boolean!=r.boolean);
            break;
        case (qfmpBoolVector<<16)+qfmpBoolVector: {
            if (l.boolVec.size()!=r.boolVec.size()) {
                res.setInvalid();
                if (p) p->qfmpError(QObject::tr("vectors in comparison '!=' have different length"));
                return;
            }
            res.setBoolVec(l.boolVec.size());
            for (int i=0; i<l.boolVec.size(); i++) {
                res.boolVec[i]=(l.boolVec[i]!=r.boolVec[i]);
            }
            } break;
        case (qfmpBoolVector<<16)+qfmpBool: {
            res.setBoolVec(l.boolVec.size());
            for (int i=0; i<l.boolVec.size(); i++) {
                res.boolVec[i]=(l.boolVec[i]!=r.boolean);
            }
            } break;
        case (qfmpBool<<16)+qfmpBoolVector: {
            res.setBoolVec(r.boolVec.size());
            for (int i=0; i<r.boolVec.size(); i++) {
                res.boolVec[i]=(r.boolVec[i]!=l.boolean);
            }
            } break;
        default:
            if (l.type==qfmpCustom && l.custom()) l.custom()->comparenotequal(res, l, r, p);
            else if (l.type==qfmpCustom && r.custom()) r.custom()->comparenotequal(res, l, r, p);
            else {
                res.setInvalid();
                if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be compared '!='").arg(l.typeName()).arg(r.typeName()));
            }
            break;
    }
}


void qfmpResult::comparegreater(qfmpResult& res, const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    switch((uint32_t(l.type)<<16)+uint32_t(r.type)) {
        case (qfmpDouble<<16)+qfmpDouble:
            res.setBoolean(l.num>r.num);
            break;
        case (qfmpDoubleVector<<16)+qfmpDoubleVector: {
            if (l.numVec.size()!=r.numVec.size()) {
                res.setInvalid();
                if (p) p->qfmpError(QObject::tr("vectors in comparison '>' have different length"));
                return;
            }
            res.setBoolVec(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                res.boolVec[i]=(l.numVec[i]>r.numVec[i]);
            }
            } break;
        case (qfmpDoubleVector<<16)+qfmpDouble: {
            res.setBoolVec(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                res.boolVec[i]=(l.numVec[i]>r.num);
            }
            } break;
        case (qfmpDouble<<16)+qfmpDoubleVector: {
            res.setBoolVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                res.boolVec[i]=(l.num>r.numVec[i]);
            }
            } break;

        case (qfmpString<<16)+qfmpString:
            res.setBoolean(l.str>r.str);
            break;
        case (qfmpStringVector<<16)+qfmpStringVector: {
            if (l.strVec.size()!=r.strVec.size()) {
                res.setInvalid();
                if (p) p->qfmpError(QObject::tr("vectors in comparison '>' have different length"));
                return;
            }
            res.setBoolVec(l.strVec.size());
            for (int i=0; i<l.strVec.size(); i++) {
                res.boolVec[i]=(l.strVec[i]>r.strVec[i]);
            }
            } break;
        case (qfmpStringVector<<16)+qfmpString: {
            res.setBoolVec(l.strVec.size());
            for (int i=0; i<l.strVec.size(); i++) {
                res.boolVec[i]=(l.strVec[i]>r.str);
            }
            } break;
        case (qfmpString<<16)+qfmpStringVector: {
            res.setBoolVec(r.strVec.size());
            for (int i=0; i<r.strVec.size(); i++) {
                res.boolVec[i]=(l.str>r.strVec[i]);
            }
            } break;


        default:
            if (l.type==qfmpCustom && l.custom()) l.custom()->comparegreater(res, l, r, p);
            else if (l.type==qfmpCustom && r.custom()) r.custom()->comparegreater(res, l, r, p);
            else {
                res.setInvalid();
                if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be compared '>'").arg(l.typeName()).arg(r.typeName()));
            }
            break;
    }
}



void qfmpResult::comparegreaterequal(qfmpResult& res, const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    switch((uint32_t(l.type)<<16)+uint32_t(r.type)) {
        case (qfmpDouble<<16)+qfmpDouble:
            res.setBoolean(l.num>=r.num);
            break;
        case (qfmpDoubleVector<<16)+qfmpDoubleVector: {
            if (l.numVec.size()!=r.numVec.size()) {
                res.setInvalid();
                if (p) p->qfmpError(QObject::tr("vectors in comparison '>=' have different length"));
                return;
            }
            res.setBoolVec(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                res.boolVec[i]=(l.numVec[i]>=r.numVec[i]);
            }
            } break;
        case (qfmpDoubleVector<<16)+qfmpDouble: {
            res.setBoolVec(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                res.boolVec[i]=(l.numVec[i]>=r.num);
            }
            } break;
        case (qfmpDouble<<16)+qfmpDoubleVector: {
            res.setBoolVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                res.boolVec[i]=(l.num>=r.numVec[i]);
            }
            } break;

        case (qfmpString<<16)+qfmpString:
            res.setBoolean(l.str>=r.str);
            break;
        case (qfmpStringVector<<16)+qfmpStringVector: {
            if (l.strVec.size()!=r.strVec.size()) {
                res.setInvalid();
                if (p) p->qfmpError(QObject::tr("vectors in comparison '>=' have different length"));
                return;
            }
            res.setBoolVec(l.strVec.size());
            for (int i=0; i<l.strVec.size(); i++) {
                res.boolVec[i]=(l.strVec[i]>=r.strVec[i]);
            }
            } break;
        case (qfmpStringVector<<16)+qfmpString: {
            res.setBoolVec(l.strVec.size());
            for (int i=0; i<l.strVec.size(); i++) {
                res.boolVec[i]=(l.strVec[i]>=r.str);
            }
            } break;
        case (qfmpString<<16)+qfmpStringVector: {
            res.setBoolVec(r.strVec.size());
            for (int i=0; i<r.strVec.size(); i++) {
                res.boolVec[i]=(l.str>=r.strVec[i]);
            }
            } break;


        default:
            if (l.type==qfmpCustom && l.custom()) l.custom()->comparegreaterequal(res, l, r, p);
            else if (l.type==qfmpCustom && r.custom()) r.custom()->comparegreaterequal(res, l, r, p);
            else {
                res.setInvalid();
                if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be compared '>='").arg(l.typeName()).arg(r.typeName()));
            }
            break;
    }
}





void qfmpResult::comparesmaller(qfmpResult& res, const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    switch((uint32_t(l.type)<<16)+uint32_t(r.type)) {
        case (qfmpDouble<<16)+qfmpDouble:
            res.setBoolean(l.num<r.num);
            break;
        case (qfmpDoubleVector<<16)+qfmpDoubleVector: {
            if (l.numVec.size()!=r.numVec.size()) {
                res.setInvalid();
                if (p) p->qfmpError(QObject::tr("vectors in comparison '<' have different length"));
                return;
            }
            res.setBoolVec(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                res.boolVec[i]=(l.numVec[i]<r.numVec[i]);
            }
            } break;
        case (qfmpDoubleVector<<16)+qfmpDouble: {
            res.setBoolVec(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                res.boolVec[i]=(l.numVec[i]<r.num);
            }
            } break;
        case (qfmpDouble<<16)+qfmpDoubleVector: {
            res.setBoolVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                res.boolVec[i]=(l.num<r.numVec[i]);
            }
            } break;

        case (qfmpString<<16)+qfmpString:
            res.setBoolean(l.str<r.str);
            break;
        case (qfmpStringVector<<16)+qfmpStringVector: {
            if (l.strVec.size()!=r.strVec.size()) {
                res.setInvalid();
                if (p) p->qfmpError(QObject::tr("vectors in comparison '<' have different length"));
                return;
            }
            res.setBoolVec(l.strVec.size());
            for (int i=0; i<l.strVec.size(); i++) {
                res.boolVec[i]=(l.strVec[i]<r.strVec[i]);
            }
            } break;
        case (qfmpStringVector<<16)+qfmpString: {
            res.setBoolVec(l.strVec.size());
            for (int i=0; i<l.strVec.size(); i++) {
                res.boolVec[i]=(l.strVec[i]<r.str);
            }
            } break;
        case (qfmpString<<16)+qfmpStringVector: {
            res.setBoolVec(r.strVec.size());
            for (int i=0; i<r.strVec.size(); i++) {
                res.boolVec[i]=(l.str<r.strVec[i]);
            }
            } break;


        default:
            if (l.type==qfmpCustom && l.custom()) l.custom()->comparesmaller(res, l, r, p);
            else if (l.type==qfmpCustom && r.custom()) r.custom()->comparesmaller(res, l, r, p);
            else {
                res.setInvalid();
                if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be compared '<'").arg(l.typeName()).arg(r.typeName()));
                break;            }

    }
}


void qfmpResult::comparesmallerequal(qfmpResult& res, const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    switch((uint32_t(l.type)<<16)+uint32_t(r.type)) {
        case (qfmpDouble<<16)+qfmpDouble:
            res.setBoolean(l.num<=r.num);
            break;
        case (qfmpDoubleVector<<16)+qfmpDoubleVector: {
            if (l.numVec.size()!=r.numVec.size()) {
                res.setInvalid();
                if (p) p->qfmpError(QObject::tr("vectors in comparison '<=' have different length"));
                return;
            }
            res.setBoolVec(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                res.boolVec[i]=(l.numVec[i]<=r.numVec[i]);
            }
            } break;
        case (qfmpDoubleVector<<16)+qfmpDouble: {
            res.setBoolVec(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                res.boolVec[i]=(l.numVec[i]<=r.num);
            }
            } break;
        case (qfmpDouble<<16)+qfmpDoubleVector: {
            res.setBoolVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                res.boolVec[i]=(l.num<=r.numVec[i]);
            }
            } break;

        case (qfmpString<<16)+qfmpString:
            res.setBoolean(l.str<=r.str);
            break;
        case (qfmpStringVector<<16)+qfmpStringVector: {
            if (l.strVec.size()!=r.strVec.size()) {
                res.setInvalid();
                if (p) p->qfmpError(QObject::tr("vectors in comparison '<=' have different length"));
                return;
            }
            res.setBoolVec(l.strVec.size());
            for (int i=0; i<l.strVec.size(); i++) {
                res.boolVec[i]=(l.strVec[i]<=r.strVec[i]);
            }
            } break;
        case (qfmpStringVector<<16)+qfmpString: {
            res.setBoolVec(l.strVec.size());
            for (int i=0; i<l.strVec.size(); i++) {
                res.boolVec[i]=(l.strVec[i]<=r.str);
            }
            } break;
        case (qfmpString<<16)+qfmpStringVector: {
            res.setBoolVec(r.strVec.size());
            for (int i=0; i<r.strVec.size(); i++) {
                res.boolVec[i]=(l.str<=r.strVec[i]);
            }
            } break;


        default:
            if (l.type==qfmpCustom && l.custom()) l.custom()->comparesmallerequal(res, l, r, p);
            else if (l.type==qfmpCustom && r.custom()) r.custom()->comparesmallerequal(res, l, r, p);
            else {
                res.setInvalid();
                if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be compared '<='").arg(l.typeName()).arg(r.typeName()));
            }
            break;
    }
}

void qfmpResult::clearCustom()
{
    if (m_custom) delete m_custom;
    m_custom=NULL;
}















//================================================================================================================



QString qfmpResult::typeName() const
{
    if (!isValid) return QString("invalid");
    if (type==qfmpCustom && custom()) return QString("custom (%1)").arg(custom()->typeName());
    return QFMathParser::resultTypeToString(type);
}


QVector<int> qfmpResult::asIntVector() const
{
    QVector<double> dbl=asVector();
    QVector<int> ii(dbl.size(), 0);
    for (int i=0; i<dbl.size(); i++) {
        ii[i]=dbl[i];
    }
    return ii;
}





















qfmpCustomResult::qfmpCustomResult()
{
    counter++;
    //qDebug()<<"qfmpCustomResult::counter="<<counter;
}

qfmpCustomResult::~qfmpCustomResult()
{
    counter--;
    //qDebug()<<"qfmpCustomResult::counter="<<counter;
}


QString qfmpCustomResult::typeName() const
{
    return typeid(*this).name();
}

QString qfmpCustomResult::toTypeString(int precision) const
{
    return this->toString(precision)+QString(" [")+typeName()+QString("]");
}

int qfmpCustomResult::length() const
{
    return 1;
}

void qfmpCustomResult::clear()
{

}

QVariantList qfmpCustomResult::asVariantList() const
{
    return QVariantList();
}



bool qfmpCustomResult::convertsTo(qfmpResultType /*type*/) const
{
    return false;
}

void qfmpCustomResult::convertTo(qfmpResult &res, qfmpResultType /*type*/) const
{
    res.setInvalid();
}

void qfmpCustomResult::add(qfmpResult &result, const qfmpResult &l, const qfmpResult &r, QFMathParser *p) const
{
    result.setInvalid();
    if (p) p->qfmpError(QObject::tr("operation '+' not defined for arguments of type %1 (l) and %2 (r)").arg(l.typeName()).arg(r.typeName()));

}

void qfmpCustomResult::sub(qfmpResult &result, const qfmpResult &l, const qfmpResult &r, QFMathParser *p) const
{
    result.setInvalid();
    if (p) p->qfmpError(QObject::tr("operation '-' not defined for arguments of type %1 (l) and %2 (r)").arg(l.typeName()).arg(r.typeName()));

}

void qfmpCustomResult::mul(qfmpResult &result, const qfmpResult &l, const qfmpResult &r, QFMathParser *p) const
{
    result.setInvalid();
    if (p) p->qfmpError(QObject::tr("operation '*' not defined for arguments of type %1 (l) and %2 (r)").arg(l.typeName()).arg(r.typeName()));

}

void qfmpCustomResult::div(qfmpResult &result, const qfmpResult &l, const qfmpResult &r, QFMathParser *p) const
{
    result.setInvalid();
    if (p) p->qfmpError(QObject::tr("operation '/' not defined for arguments of type %1 (l) and %2 (r)").arg(l.typeName()).arg(r.typeName()));

}

void qfmpCustomResult::mod(qfmpResult &result, const qfmpResult &l, const qfmpResult &r, QFMathParser *p) const
{
    result.setInvalid();
    if (p) p->qfmpError(QObject::tr("operation 'mod' not defined for arguments of type %1 (l) and %2 (r)").arg(l.typeName()).arg(r.typeName()));

}

void qfmpCustomResult::power(qfmpResult &result, const qfmpResult &l, const qfmpResult &r, QFMathParser *p) const
{
    result.setInvalid();
    if (p) p->qfmpError(QObject::tr("operation 'power' not defined for arguments of type %1 (l) and %2 (r)").arg(l.typeName()).arg(r.typeName()));

}

void qfmpCustomResult::bitwiseand(qfmpResult &result, const qfmpResult &l, const qfmpResult &r, QFMathParser *p) const
{
    result.setInvalid();
    if (p) p->qfmpError(QObject::tr("operation '&' not defined for arguments of type %1 (l) and %2 (r)").arg(l.typeName()).arg(r.typeName()));

}

void qfmpCustomResult::bitwiseor(qfmpResult &result, const qfmpResult &l, const qfmpResult &r, QFMathParser *p) const
{
    result.setInvalid();
    if (p) p->qfmpError(QObject::tr("operation '|' not defined for arguments of type %1 (l) and %2 (r)").arg(l.typeName()).arg(r.typeName()));

}

void qfmpCustomResult::logicand(qfmpResult &result, const qfmpResult &l, const qfmpResult &r, QFMathParser *p) const
{
    result.setInvalid();
    if (p) p->qfmpError(QObject::tr("operation '&&' not defined for arguments of type %1 (l) and %2 (r)").arg(l.typeName()).arg(r.typeName()));

}

void qfmpCustomResult::logicor(qfmpResult &result, const qfmpResult &l, const qfmpResult &r, QFMathParser *p) const
{
    result.setInvalid();
    if (p) p->qfmpError(QObject::tr("operation '||' not defined for arguments of type %1 (l) and %2 (r)").arg(l.typeName()).arg(r.typeName()));

}

void qfmpCustomResult::logicnot(qfmpResult &result, const qfmpResult &l, QFMathParser *p) const
{
    result.setInvalid();
    if (p) p->qfmpError(QObject::tr("operation '!' not defined for arguments of type %1").arg(l.typeName()));

}

void qfmpCustomResult::logicnand(qfmpResult &result, const qfmpResult &l, const qfmpResult &r, QFMathParser *p) const
{
    result.setInvalid();
    if (p) p->qfmpError(QObject::tr("operation 'NAND' not defined for arguments of type %1 (l) and %2 (r)").arg(l.typeName()).arg(r.typeName()));

}

void qfmpCustomResult::logicnor(qfmpResult &result, const qfmpResult &l, const qfmpResult &r, QFMathParser *p) const
{
    result.setInvalid();
    if (p) p->qfmpError(QObject::tr("operation 'NOR' not defined for arguments of type %1 (l) and %2 (r)").arg(l.typeName()).arg(r.typeName()));

}

void qfmpCustomResult::logicxor(qfmpResult &result, const qfmpResult &l, const qfmpResult &r, QFMathParser *p) const
{
    result.setInvalid();
    if (p) p->qfmpError(QObject::tr("operation 'XOR' not defined for arguments of type %1 (l) and %2 (r)").arg(l.typeName()).arg(r.typeName()));

}

void qfmpCustomResult::neg(qfmpResult &result, const qfmpResult &l, QFMathParser *p) const
{
    result.setInvalid();
    if (p) p->qfmpError(QObject::tr("operation 'neg' not defined for arguments of type %1").arg(l.typeName()));

}

void qfmpCustomResult::bitwisenot(qfmpResult &result, const qfmpResult &l, QFMathParser *p) const
{
    result.setInvalid();
    if (p) p->qfmpError(QObject::tr("operation '~' not defined for arguments of type %1").arg(l.typeName()));

}


void qfmpCustomResult::compareequal(qfmpResult &result, const qfmpResult &l, const qfmpResult &r, QFMathParser *p) const
{
    result.setInvalid();
    if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be compared '=='").arg(l.typeName()).arg(r.typeName()));
}

void qfmpCustomResult::comparenotequal(qfmpResult &result, const qfmpResult &l, const qfmpResult &r, QFMathParser *p) const
{
    result.setInvalid();
    if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be compared '!='").arg(l.typeName()).arg(r.typeName()));
}

void qfmpCustomResult::comparegreater(qfmpResult &res, const qfmpResult &l, const qfmpResult &r, QFMathParser *p) const
{
    res.setInvalid();
    if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be compared '>'").arg(l.typeName()).arg(r.typeName()));
}

void qfmpCustomResult::comparegreaterequal(qfmpResult &res, const qfmpResult &l, const qfmpResult &r, QFMathParser *p) const
{
    res.setInvalid();
    if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be compared '>='").arg(l.typeName()).arg(r.typeName()));

}

void qfmpCustomResult::comparesmaller(qfmpResult &res, const qfmpResult &l, const qfmpResult &r, QFMathParser *p) const
{
    res.setInvalid();
    if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be compared '<'").arg(l.typeName()).arg(r.typeName()));

}

void qfmpCustomResult::comparesmallerequal(qfmpResult &res, const qfmpResult &l, const qfmpResult &r, QFMathParser *p) const
{
    res.setInvalid();
    if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be compared '<='").arg(l.typeName()).arg(r.typeName()));

}

int qfmpCustomResult::counter=0;


QString qfmpResultTypeToString(qfmpResultType type)
{
    switch(type) {
        case qfmpDouble: return QObject::tr("number");
        case qfmpString: return QObject::tr("string");
        case qfmpBool: return QObject::tr("bool");
        case qfmpDoubleVector: return QObject::tr("number vector");
        case qfmpStringVector: return QObject::tr("string vector");
        case qfmpBoolVector: return QObject::tr("bool vector");
        case qfmpVoid: return QObject::tr("void");
        case qfmpStruct: return QObject::tr("struct");
        case qfmpCustom: return QObject::tr("custom");
    }
    return QObject::tr("invalid");
}
