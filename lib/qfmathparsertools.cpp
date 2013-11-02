#include "qfmathparsertools.h"
#include "qfmathparser.h"





qfmpResult::qfmpResult()
{
    isValid=true;
    type=qfmpDouble;
    str="";
    num=0;
    boolean=false;
    numVec.clear();
    boolVec.clear();
    strVec.clear();
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
    strVec.clear();
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
    strVec.clear();
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
    strVec.clear();
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
    strVec.clear();
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
    strVec.clear();
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
    strVec.clear();
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
    strVec=value;
}

qfmpResult::qfmpResult(const qfmpResult &value):
    isValid(value.isValid),
    type(value.type),
    num(value.num),
    boolean(value.boolean)
{
    if (type==qfmpString) this->str=value.str;
    else if (type==qfmpDoubleVector) this->numVec=value.numVec;
    else if (type==qfmpStringVector) this->strVec=value.strVec;
    else if (type==qfmpBoolVector) this->boolVec=value.boolVec;
}



qfmpResult &qfmpResult::operator=(const qfmpResult &value)
{
    isValid=value.isValid;
    type=value.type;
    num=value.num;
    boolean=value.boolean;
    if (type==qfmpString) this->str=value.str;
    else if (type==qfmpDoubleVector) this->numVec=value.numVec;
    else if (type==qfmpStringVector) this->strVec=value.strVec;
    else if (type==qfmpBoolVector) this->boolVec=value.boolVec;
    return *this;
}

void qfmpResult::setInvalid()
{
    isValid=false;
    numVec.clear();
    boolVec.clear();
    strVec.clear();
    str.clear();
    type=qfmpVoid;
}

void qfmpResult::setVoid()
{
    isValid=true;
    numVec.clear();
    boolVec.clear();
    strVec.clear();
    str.clear();
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
        case qfmpVoid: return boolToQString(boolean)+QObject::tr(" [void]");
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
    }
    return 0;
}

void qfmpResult::setDouble(double val)
{
    type=qfmpDouble;
    num=val;
    numVec.clear();
    boolVec.clear();
    strVec.clear();
    str.clear();
    isValid=true;
}

void qfmpResult::setBoolean(bool val)
{
    type=qfmpBool;
    boolean=val;
    numVec.clear();
    boolVec.clear();
    strVec.clear();
    str.clear();
    isValid=true;
}

void qfmpResult::setString(const QString &val)
{
    type=qfmpString;
    str=val;
    boolVec.clear();
    strVec.clear();
    numVec.clear();
    isValid=true;
}

void qfmpResult::setDoubleVec(const QVector<double> &val)
{
    isValid=true;
    type=qfmpDoubleVector;
    boolVec.clear();
    strVec.clear();
    numVec=val;
    str.clear();
}

void qfmpResult::setDoubleVec(int size, double defaultVal)
{
    isValid=true;
    type=qfmpDoubleVector;
    boolVec.clear();
    strVec.clear();
    if (size>0) {
        numVec.resize(size);
        for (int i=0; i<size; i++) numVec[i]=defaultVal;
    } else {
        numVec.clear();
    }
    str.clear();
}

void qfmpResult::setBoolVec(const QVector<bool> &val)
{
    isValid=true;
    type=qfmpBoolVector;
    numVec.clear();
    strVec.clear();
    boolVec=val;
    str.clear();

}

void qfmpResult::setBoolVec(int size, bool defaultVal)
{
    isValid=true;
    type=qfmpBoolVector;
    numVec.clear();
    strVec.clear();
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
    str.clear();
}

void qfmpResult::setStringVec(int size, const QString &defaultVal)
{
    isValid=true;
    type=qfmpStringVector;
    numVec.clear();
    boolVec.clear();
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
    }
    return vl;
}

QStringList qfmpResult::asStrVector() const
{
    if (type==qfmpStringVector) return strVec;
    else if (type==qfmpString)  return QStringList(str);
    return QStringList();
}

QVector<bool> qfmpResult::asBoolVector() const
{
    if (type==qfmpBoolVector) return boolVec;
    else if (type==qfmpBool) return QVector<bool>(1, boolean);
    return QVector<bool>();
}

bool qfmpResult::convertsToVector() const
{
    if (type==qfmpDoubleVector || type==qfmpBoolVector || type==qfmpDouble) return true;
    return false;
}

bool qfmpResult::convertsToBoolVector() const
{
    if (type==qfmpBool || type==qfmpBoolVector) return true;
    return false;
}

bool qfmpResult::convertsToStringVector() const
{
    if (type==qfmpStringVector || type==qfmpString) return true;
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




bool qfmpResult::operator!=(const qfmpResult &other) const
{
    return !operator==(other);
}


bool qfmpResult::operator==(const qfmpResult &r) const
{
    if (!isValid && !r.isValid) return true;
    switch((uint16_t(type)<<8)+uint16_t(r.type)) {
        case (qfmpVoid<<8)+qfmpVoid:
            return  true;
            break;
        case (qfmpDouble<<8)+qfmpDouble:
            return(num==r.num);
            break;
        case (qfmpDoubleVector<<8)+qfmpDoubleVector:
            return(numVec==r.numVec);
            break;
        case (qfmpString<<8)+qfmpString:
            return(str==r.str);
            break;
        case (qfmpBool<<8)+qfmpBool:
            return(boolean==r.boolean);
            break;
        case (qfmpBoolVector<<8)+qfmpBoolVector:
            return(boolVec==r.boolVec);
            break;
        case (qfmpStringVector<<8)+qfmpStringVector:
            return(strVec==r.strVec);
            break;
        default:
            return false;
    }
}



//================================================================================================================


void qfmpResult::add(qfmpResult& re, const qfmpResult &l, const qfmpResult &r, QFMathParser* p)
{
    uint16_t lt=l.type;
    uint16_t rt=r.type;
    uint16_t tt=(lt<<8)+rt;
    /*qDebug()<<QString::number(tt, 16);
    //qDebug()<<QString::number((uint16_t(qfmpDouble)<<8)+qfmpDouble, 16);
    //qDebug()<<QString::number((uint16_t(qfmpDouble)<<8)+qfmpDoubleVector, 16);
    //qDebug()<<QString::number((uint16_t(qfmpDoubleVector<<8))+qfmpDoubleVector, 16);
    //qDebug()<<QString::number((uint16_t(qfmpString)<<8)+qfmpString, 16);*/
    switch(tt) {
        case (uint16_t(qfmpDouble)<<8)+qfmpDouble:
            re.setDouble(l.num+r.num);
            break;
        case (qfmpDoubleVector<<8)+qfmpDouble: {
            re.setDoubleVec(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                re.numVec[i]=l.numVec[i]+r.num;
            }
            break;
        }
        case (uint16_t(qfmpDouble)<<8)+qfmpDoubleVector: {
            re.setDoubleVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=l.num+r.numVec[i];
            }
            break;
        }
        case (uint16_t(qfmpDoubleVector<<8))+qfmpDoubleVector: {
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
        case (uint16_t(qfmpString)<<8)+qfmpString:
            re.setString(l.str+r.str);
            break;
        default:
            if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be added").arg(l.typeName()).arg(r.typeName()));
            re.setInvalid();
            break;
    }
}


void qfmpResult::la_add(qfmpResult &l, const qfmpResult &r, QFMathParser* p)
{
    uint16_t lt=l.type;
    uint16_t rt=r.type;
    uint16_t tt=(lt<<8)+rt;
    switch(tt) {
        case (uint16_t(qfmpDouble)<<8)+qfmpDouble:
            l.setDouble(l.num+r.num);
            break;
        case (qfmpDoubleVector<<8)+qfmpDouble: {
            for (int i=0; i<l.numVec.size(); i++) {
                l.numVec[i]=l.numVec[i]+r.num;
            }
            break;
        }
        case (uint16_t(qfmpDouble)<<8)+qfmpDoubleVector: {
            l.type=qfmpDoubleVector;
            l.numVec.resize(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                l.numVec[i]=l.num+r.numVec[i];
            }
            break;
        }
        case (uint16_t(qfmpDoubleVector<<8))+qfmpDoubleVector: {
            if (l.length()!=r.length()) {
                if (p) p->qfmpError(QObject::tr("arguments have to have the same length, but lengthes were left=%1, right=%2").arg(l.length()).arg(r.length()));
                l.setInvalid();
                return;
            }
            for (int i=0; i<r.numVec.size(); i++) {
                l.numVec[i]=l.numVec[i]+r.numVec[i];
            }
            break;
        }
        case (uint16_t(qfmpString)<<8)+qfmpString:
            l.setString(l.str+r.str);
            break;
        default:
            if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be added").arg(l.typeName()).arg(r.typeName()));
            l.setInvalid();
            break;
    }
}

void qfmpResult::sub(qfmpResult& re, const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    uint16_t lt=l.type;
    uint16_t rt=r.type;
    uint16_t tt=(lt<<8)+rt;
    //qDebug()<<QString::number(tt, 16);
    switch(tt) {
        case (qfmpDouble<<8)+qfmpDouble:
            re.setDouble(l.num-r.num);
            break;
        case (qfmpDoubleVector<<8)+qfmpDouble: {
            re.setDoubleVec(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                re.numVec[i]=l.numVec[i]-r.num;
            }
            break;
        }
        case (qfmpDouble<<8)+qfmpDoubleVector: {
            re.setDoubleVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=l.num-r.numVec[i];
            }
            break;
        }
        case (qfmpDoubleVector<<8)+qfmpDoubleVector: {
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
            if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be subtracted").arg(l.typeName()).arg(r.typeName()));
            re.setInvalid();
            break;
    }
}

void qfmpResult::mul(qfmpResult& re, const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    uint16_t lt=l.type;
    uint16_t rt=r.type;
    uint16_t tt=(lt<<8)+rt;
    //qDebug()<<QString::number(tt, 16);
    switch(tt) {
        case (qfmpDouble<<8)+qfmpDouble:
            re.setDouble(l.num*r.num);
            break;
        case (qfmpDoubleVector<<8)+qfmpDouble: {
            re.setDoubleVec(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                re.numVec[i]=l.numVec[i]*r.num;
            }
            break;
        }
        case (qfmpDouble<<8)+qfmpDoubleVector: {
            re.setDoubleVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=l.num*r.numVec[i];
            }
            break;
        }
        case (qfmpDoubleVector<<8)+qfmpDoubleVector: {
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
            if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be multiplied").arg(l.typeName()).arg(r.typeName()));
            re.setInvalid();
            break;
    }
}

void qfmpResult::div(qfmpResult& re, const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    uint16_t lt=l.type;
    uint16_t rt=r.type;
    uint16_t tt=(lt<<8)+rt;
    //qDebug()<<QString::number(tt, 16);
    switch(tt) {
        case (qfmpDouble<<8)+qfmpDouble:
            re.setDouble(l.num/r.num);
            break;
        case (qfmpDoubleVector<<8)+qfmpDouble: {
            re.setDoubleVec(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                re.numVec[i]=l.numVec[i]/r.num;
            }
            break;
        }
        case (qfmpDouble<<8)+qfmpDoubleVector: {
            re.setDoubleVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=l.num/r.numVec[i];
            }
            break;
        }
        case (qfmpDoubleVector<<8)+qfmpDoubleVector: {
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
            re.setInvalid();
            if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be divided").arg(l.typeName()).arg(r.typeName()));
            break;
    }
}

void qfmpResult::mod(qfmpResult& re, const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    uint16_t lt=l.type;
    uint16_t rt=r.type;
    uint16_t tt=(lt<<8)+rt;
    //qDebug()<<QString::number(tt, 16);
    switch(tt) {
        case (qfmpDouble<<8)+qfmpDouble:
            re.setDouble(double(l.toInteger()%r.toInteger()));
            break;
        case (qfmpDoubleVector<<8)+qfmpDouble: {
            re.setDoubleVec(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                re.numVec[i]=int32_t(l.numVec[i])%r.toInteger();
            }
            break;
        }
        case (qfmpDouble<<8)+qfmpDoubleVector: {
            re.setDoubleVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=l.toInteger()%int32_t(r.numVec[i]);
            }
            break;
        }
        case (qfmpDoubleVector<<8)+qfmpDoubleVector: {
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
            if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be used in mod operation").arg(l.typeName()).arg(r.typeName()));
            re.setInvalid();
            break;
    }
}

void qfmpResult::power(qfmpResult& re, const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    uint16_t lt=l.type;
    uint16_t rt=r.type;
    uint16_t tt=(lt<<8)+rt;
    //qDebug()<<QString::number(tt, 16);
    switch(tt) {
        case (qfmpDouble<<8)+qfmpDouble:
            re.setDouble(pow(l.num,r.num));
            break;
        case (qfmpDoubleVector<<8)+qfmpDouble: {
            re.setDoubleVec(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                re.numVec[i]=pow(l.numVec[i],r.num);
            }
            break;
        }
        case (qfmpDouble<<8)+qfmpDoubleVector: {
            re.setDoubleVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=pow(l.num,r.numVec[i]);
            }
            break;
        }
        case (qfmpDoubleVector<<8)+qfmpDoubleVector: {
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
            re.setInvalid();
            if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be used in power operation").arg(l.typeName()).arg(r.typeName()));
            break;
    }
}

void qfmpResult::bitwiseand(qfmpResult& re, const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    uint16_t lt=l.type;
    uint16_t rt=r.type;
    uint16_t tt=(lt<<8)+rt;
    //qDebug()<<QString::number(tt, 16);
    switch(tt) {
        case (qfmpDouble<<8)+qfmpDouble:
            re.setDouble(double(l.toInteger()&r.toInteger()));
            break;
        case (qfmpDoubleVector<<8)+qfmpDouble: {
            re.setDoubleVec(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                re.numVec[i]=int32_t(l.numVec[i])&r.toInteger();
            }
            break;
        }
        case (qfmpDouble<<8)+qfmpDoubleVector: {
            re.setDoubleVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=l.toInteger()&int32_t(r.numVec[i]);
            }
            break;
        }
        case (qfmpDoubleVector<<8)+qfmpDoubleVector: {
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
            if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be used in bitwise and operation").arg(l.typeName()).arg(r.typeName()));
            re.setInvalid();
            break;
    }
}

void qfmpResult::bitwiseor(qfmpResult& re, const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    uint16_t lt=l.type;
    uint16_t rt=r.type;
    uint16_t tt=(lt<<8)+rt;
    //qDebug()<<QString::number(tt, 16);
    switch(tt) {
        case (qfmpDouble<<8)+qfmpDouble:
            re.setDouble(double(l.toInteger()|r.toInteger()));
            break;
        case (qfmpDoubleVector<<8)+qfmpDouble: {
            re.setDoubleVec(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                re.numVec[i]=int32_t(l.numVec[i])|r.toInteger();
            }
            break;
        }
        case (qfmpDouble<<8)+qfmpDoubleVector: {
            re.setDoubleVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=l.toInteger()|int32_t(r.numVec[i]);
            }
            break;
        }
        case (qfmpDoubleVector<<8)+qfmpDoubleVector: {
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
            re.setInvalid();
            if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be used in bitwise or operation").arg(l.typeName()).arg(r.typeName()));
            break;
    }
}

void qfmpResult::logicand(qfmpResult& re, const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    uint16_t lt=l.type;
    uint16_t rt=r.type;
    uint16_t tt=(lt<<8)+rt;
    //qDebug()<<QString::number(tt, 16);
    switch(tt) {
        case (qfmpBool<<8)+qfmpBool:
            re.setBoolean(l.boolean&&r.boolean);
            break;
        case (qfmpBoolVector<<8)+qfmpBoolVector: {
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
        case (qfmpBoolVector<<8)+qfmpBool: {
            re.setBoolVec(l.boolVec.size());
            for (int i=0; i<l.boolVec.size(); i++) {
                re.boolVec[i]=l.boolVec[i]&&r.boolean;
            }
            break;
        }

        default:
            re.setInvalid();
            if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be used in logic and operation").arg(l.typeName()).arg(r.typeName()));
            break;
    }
}

void qfmpResult::logicor(qfmpResult& re, const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    uint16_t lt=l.type;
    uint16_t rt=r.type;
    uint16_t tt=(lt<<8)+rt;
    //qDebug()<<QString::number(tt, 16);
    switch(tt) {
        case (qfmpBool<<8)+qfmpBool:
            re.setBoolean(l.boolean||r.boolean);
            break;
        case (qfmpBoolVector<<8)+qfmpBoolVector: {
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
        case (qfmpBoolVector<<8)+qfmpBool: {
            re.setBoolVec(l.boolVec.size());
            for (int i=0; i<l.boolVec.size(); i++) {
                re.boolVec[i]=l.boolVec[i]||r.boolean;
            }
            break;
        }
        default:
            re.setInvalid();
            if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be used in logic or operation").arg(l.typeName()).arg(r.typeName()));
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
            re.setInvalid();
            if (p) p->qfmpError(QObject::tr("argument of type %1 may not be used in logic not operation").arg(l.typeName()));
            break;

    }

}

void qfmpResult::logicnand(qfmpResult &re, const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    uint16_t lt=l.type;
    uint16_t rt=r.type;
    uint16_t tt=(lt<<8)+rt;
    //qDebug()<<QString::number(tt, 16);
    switch(tt) {
        case (qfmpBool<<8)+qfmpBool:
            re.setBoolean(!(l.boolean&&r.boolean));
            break;
        case (qfmpBoolVector<<8)+qfmpBoolVector: {
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
        case (qfmpBoolVector<<8)+qfmpBool: {
            re.setBoolVec(l.boolVec.size());
            for (int i=0; i<l.boolVec.size(); i++) {
                re.boolVec[i]=!(l.boolVec[i]&&r.boolean);
            }
            break;
        }
        default:
            re.setInvalid();
            if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be used in logic nand operation").arg(l.typeName()).arg(r.typeName()));
            break;
    }
}

void qfmpResult::logicnor(qfmpResult &re, const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    uint16_t lt=l.type;
    uint16_t rt=r.type;
    uint16_t tt=(lt<<8)+rt;
    //qDebug()<<QString::number(tt, 16);
    switch(tt) {
        case (qfmpBool<<8)+qfmpBool:
            re.setBoolean(!(l.boolean||r.boolean));
            break;
        case (qfmpBoolVector<<8)+qfmpBoolVector: {
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
        case (qfmpBoolVector<<8)+qfmpBool: {
            re.setBoolVec(l.boolVec.size());
            for (int i=0; i<l.boolVec.size(); i++) {
                re.boolVec[i]=!(l.boolVec[i]||r.boolean);
            }
            break;
        }
        default:
            re.setInvalid();
            if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be used in logic nor operation").arg(l.typeName()).arg(r.typeName()));
            break;
    }
}

void qfmpResult::logicxor(qfmpResult &re, const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    uint16_t lt=l.type;
    uint16_t rt=r.type;
    uint16_t tt=(lt<<8)+rt;
    //qDebug()<<QString::number(tt, 16);
    switch(tt) {
        case (qfmpBool<<8)+qfmpBool:
            re.setBoolean((l.boolean&&!r.boolean)||(!l.boolean&&r.boolean));
            break;
        case (qfmpBoolVector<<8)+qfmpBoolVector: {
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
        case (qfmpBoolVector<<8)+qfmpBool: {
            re.setBoolVec(l.boolVec.size());
            for (int i=0; i<l.boolVec.size(); i++) {
                re.boolVec[i]=(!l.boolVec[i]&&r.boolean)||(l.boolVec[i]&&!r.boolean);
            }
            break;
        }
        default:
            re.setInvalid();
            if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be used in logic xor operation").arg(l.typeName()).arg(r.typeName()));
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
            re.setInvalid();
            if (p) p->qfmpError(QObject::tr("argument of type %1 may not be used in negation operation").arg(l.typeName()));
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
            re.setInvalid();
            if (p) p->qfmpError(QObject::tr("argument of type %1 may not be used in bitwise not operation").arg(l.typeName()));
            break;

    }

}

void qfmpResult::compareequal(qfmpResult& res, const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    switch((uint16_t(l.type)<<8)+uint16_t(r.type)) {
        case (qfmpDouble<<8)+qfmpDouble:
            res.setBoolean(l.num==r.num);
            break;
        case (qfmpDoubleVector<<8)+qfmpDoubleVector: {
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
        case (qfmpDoubleVector<<8)+qfmpDouble: {
            res.setBoolVec(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                res.boolVec[i]=(l.numVec[i]==r.num);
            }
            } break;
        case (qfmpDouble<<8)+qfmpDoubleVector: {
            res.setBoolVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                res.boolVec[i]=(r.numVec[i]==l.num);
            }
            } break;

        case (qfmpString<<8)+qfmpString:
            res.setBoolean(l.str==r.str);
            break;
        case (qfmpStringVector<<8)+qfmpStringVector: {
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
        case (qfmpStringVector<<8)+qfmpString: {
            res.setBoolVec(l.strVec.size());
            for (int i=0; i<l.strVec.size(); i++) {
                res.boolVec[i]=(l.strVec[i]==r.str);
            }
            } break;
        case (qfmpString<<8)+qfmpStringVector: {
            res.setBoolVec(r.strVec.size());
            for (int i=0; i<r.strVec.size(); i++) {
                res.boolVec[i]=(r.strVec[i]==l.str);
            }
            } break;


        case (qfmpBool<<8)+qfmpBool:
            res.setBoolean(l.boolean==r.boolean);
            break;
        case (qfmpBoolVector<<8)+qfmpBoolVector: {
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
        case (qfmpBoolVector<<8)+qfmpBool: {
            res.setBoolVec(l.boolVec.size());
            for (int i=0; i<l.boolVec.size(); i++) {
                res.boolVec[i]=(l.boolVec[i]==r.boolean);
            }
            } break;
        case (qfmpBool<<8)+qfmpBoolVector: {
            res.setBoolVec(r.boolVec.size());
            for (int i=0; i<r.boolVec.size(); i++) {
                res.boolVec[i]=(r.boolVec[i]==l.boolean);
            }
            } break;
        default:
            res.setInvalid();
            if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be compared '=='").arg(l.typeName()).arg(r.typeName()));
            break;
    }
}

void qfmpResult::comparenotequal(qfmpResult& res, const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    switch((uint16_t(l.type)<<8)+uint16_t(r.type)) {
        case (qfmpDouble<<8)+qfmpDouble:
            res.setBoolean(l.num!=r.num);
            break;
        case (qfmpDoubleVector<<8)+qfmpDoubleVector: {
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
        case (qfmpDoubleVector<<8)+qfmpDouble: {
            res.setBoolVec(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                res.boolVec[i]=(l.numVec[i]!=r.num);
            }
            } break;
        case (qfmpDouble<<8)+qfmpDoubleVector: {
            res.setBoolVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                res.boolVec[i]=(r.numVec[i]!=l.num);
            }
            } break;

        case (qfmpString<<8)+qfmpString:
            res.setBoolean(l.str!=r.str);
            break;
        case (qfmpStringVector<<8)+qfmpStringVector: {
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
        case (qfmpStringVector<<8)+qfmpString: {
            res.setBoolVec(l.strVec.size());
            for (int i=0; i<l.strVec.size(); i++) {
                res.boolVec[i]=(l.strVec[i]!=r.str);
            }
            } break;
        case (qfmpString<<8)+qfmpStringVector: {
            res.setBoolVec(r.strVec.size());
            for (int i=0; i<r.strVec.size(); i++) {
                res.boolVec[i]=(r.strVec[i]!=l.str);
            }
            } break;


        case (qfmpBool<<8)+qfmpBool:
            res.setBoolean(l.boolean!=r.boolean);
            break;
        case (qfmpBoolVector<<8)+qfmpBoolVector: {
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
        case (qfmpBoolVector<<8)+qfmpBool: {
            res.setBoolVec(l.boolVec.size());
            for (int i=0; i<l.boolVec.size(); i++) {
                res.boolVec[i]=(l.boolVec[i]!=r.boolean);
            }
            } break;
        case (qfmpBool<<8)+qfmpBoolVector: {
            res.setBoolVec(r.boolVec.size());
            for (int i=0; i<r.boolVec.size(); i++) {
                res.boolVec[i]=(r.boolVec[i]!=l.boolean);
            }
            } break;
        default:
            res.setInvalid();
            if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be compared '!='").arg(l.typeName()).arg(r.typeName()));
            break;
    }
}


void qfmpResult::comparegreater(qfmpResult& res, const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    switch((uint16_t(l.type)<<8)+uint16_t(r.type)) {
        case (qfmpDouble<<8)+qfmpDouble:
            res.setBoolean(l.num>r.num);
            break;
        case (qfmpDoubleVector<<8)+qfmpDoubleVector: {
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
        case (qfmpDoubleVector<<8)+qfmpDouble: {
            res.setBoolVec(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                res.boolVec[i]=(l.numVec[i]>r.num);
            }
            } break;
        case (qfmpDouble<<8)+qfmpDoubleVector: {
            res.setBoolVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                res.boolVec[i]=(l.num>r.numVec[i]);
            }
            } break;

        case (qfmpString<<8)+qfmpString:
            res.setBoolean(l.str>r.str);
            break;
        case (qfmpStringVector<<8)+qfmpStringVector: {
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
        case (qfmpStringVector<<8)+qfmpString: {
            res.setBoolVec(l.strVec.size());
            for (int i=0; i<l.strVec.size(); i++) {
                res.boolVec[i]=(l.strVec[i]>r.str);
            }
            } break;
        case (qfmpString<<8)+qfmpStringVector: {
            res.setBoolVec(r.strVec.size());
            for (int i=0; i<r.strVec.size(); i++) {
                res.boolVec[i]=(l.str>r.strVec[i]);
            }
            } break;


        default:
            res.setInvalid();
            if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be compared '>'").arg(l.typeName()).arg(r.typeName()));
            break;
    }
}



void qfmpResult::comparegreaterequal(qfmpResult& res, const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    switch((uint16_t(l.type)<<8)+uint16_t(r.type)) {
        case (qfmpDouble<<8)+qfmpDouble:
            res.setBoolean(l.num>=r.num);
            break;
        case (qfmpDoubleVector<<8)+qfmpDoubleVector: {
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
        case (qfmpDoubleVector<<8)+qfmpDouble: {
            res.setBoolVec(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                res.boolVec[i]=(l.numVec[i]>=r.num);
            }
            } break;
        case (qfmpDouble<<8)+qfmpDoubleVector: {
            res.setBoolVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                res.boolVec[i]=(l.num>=r.numVec[i]);
            }
            } break;

        case (qfmpString<<8)+qfmpString:
            res.setBoolean(l.str>=r.str);
            break;
        case (qfmpStringVector<<8)+qfmpStringVector: {
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
        case (qfmpStringVector<<8)+qfmpString: {
            res.setBoolVec(l.strVec.size());
            for (int i=0; i<l.strVec.size(); i++) {
                res.boolVec[i]=(l.strVec[i]>=r.str);
            }
            } break;
        case (qfmpString<<8)+qfmpStringVector: {
            res.setBoolVec(r.strVec.size());
            for (int i=0; i<r.strVec.size(); i++) {
                res.boolVec[i]=(l.str>=r.strVec[i]);
            }
            } break;


        default:
            res.setInvalid();
            if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be compared '>='").arg(l.typeName()).arg(r.typeName()));
            break;
    }
}





void qfmpResult::comparesmaller(qfmpResult& res, const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    switch((uint16_t(l.type)<<8)+uint16_t(r.type)) {
        case (qfmpDouble<<8)+qfmpDouble:
            res.setBoolean(l.num<r.num);
            break;
        case (qfmpDoubleVector<<8)+qfmpDoubleVector: {
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
        case (qfmpDoubleVector<<8)+qfmpDouble: {
            res.setBoolVec(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                res.boolVec[i]=(l.numVec[i]<r.num);
            }
            } break;
        case (qfmpDouble<<8)+qfmpDoubleVector: {
            res.setBoolVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                res.boolVec[i]=(l.num<r.numVec[i]);
            }
            } break;

        case (qfmpString<<8)+qfmpString:
            res.setBoolean(l.str<r.str);
            break;
        case (qfmpStringVector<<8)+qfmpStringVector: {
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
        case (qfmpStringVector<<8)+qfmpString: {
            res.setBoolVec(l.strVec.size());
            for (int i=0; i<l.strVec.size(); i++) {
                res.boolVec[i]=(l.strVec[i]<r.str);
            }
            } break;
        case (qfmpString<<8)+qfmpStringVector: {
            res.setBoolVec(r.strVec.size());
            for (int i=0; i<r.strVec.size(); i++) {
                res.boolVec[i]=(l.str<r.strVec[i]);
            }
            } break;


        default:
            res.setInvalid();
            if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be compared '<'").arg(l.typeName()).arg(r.typeName()));
            break;
    }
}


void qfmpResult::comparesmallerequal(qfmpResult& res, const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    switch((uint16_t(l.type)<<8)+uint16_t(r.type)) {
        case (qfmpDouble<<8)+qfmpDouble:
            res.setBoolean(l.num<=r.num);
            break;
        case (qfmpDoubleVector<<8)+qfmpDoubleVector: {
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
        case (qfmpDoubleVector<<8)+qfmpDouble: {
            res.setBoolVec(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                res.boolVec[i]=(l.numVec[i]<=r.num);
            }
            } break;
        case (qfmpDouble<<8)+qfmpDoubleVector: {
            res.setBoolVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                res.boolVec[i]=(l.num<=r.numVec[i]);
            }
            } break;

        case (qfmpString<<8)+qfmpString:
            res.setBoolean(l.str<=r.str);
            break;
        case (qfmpStringVector<<8)+qfmpStringVector: {
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
        case (qfmpStringVector<<8)+qfmpString: {
            res.setBoolVec(l.strVec.size());
            for (int i=0; i<l.strVec.size(); i++) {
                res.boolVec[i]=(l.strVec[i]<=r.str);
            }
            } break;
        case (qfmpString<<8)+qfmpStringVector: {
            res.setBoolVec(r.strVec.size());
            for (int i=0; i<r.strVec.size(); i++) {
                res.boolVec[i]=(l.str<=r.strVec[i]);
            }
            } break;


        default:
            res.setInvalid();
            if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be compared '<='").arg(l.typeName()).arg(r.typeName()));
            break;
    }
}














//================================================================================================================

qfmpResult qfmpResult::add(const qfmpResult &l, const qfmpResult &r, QFMathParser* p)
{
    uint16_t lt=l.type;
    uint16_t rt=r.type;
    uint16_t tt=(lt<<8)+rt;
    /*qDebug()<<QString::number(tt, 16);
    //qDebug()<<QString::number((uint16_t(qfmpDouble)<<8)+qfmpDouble, 16);
    //qDebug()<<QString::number((uint16_t(qfmpDouble)<<8)+qfmpDoubleVector, 16);
    //qDebug()<<QString::number((uint16_t(qfmpDoubleVector<<8))+qfmpDoubleVector, 16);
    //qDebug()<<QString::number((uint16_t(qfmpString)<<8)+qfmpString, 16);*/
    switch(tt) {
        case (uint16_t(qfmpDouble)<<8)+qfmpDouble:
            return qfmpResult(l.num+r.num);
            break;
        case (qfmpDoubleVector<<8)+qfmpDouble: {
            qfmpResult re;
            re.setDoubleVec(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                re.numVec[i]=l.numVec[i]+r.num;
            }
            return re;
            break;
        }
        case (uint16_t(qfmpDouble)<<8)+qfmpDoubleVector: {
            qfmpResult re;
            re.setDoubleVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=l.num+r.numVec[i];
            }
            return re;
            break;
        }
        case (uint16_t(qfmpDoubleVector<<8))+qfmpDoubleVector: {
            if (l.length()!=r.length()) {
                if (p) p->qfmpError(QObject::tr("arguments have to have the same length, but lengthes were left=%1, right=%2").arg(l.length()).arg(r.length()));
                return invalidResult();
            }
            qfmpResult re;
            re.setDoubleVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=l.numVec[i]+r.numVec[i];
            }
            return re;
            break;
        }
        case (uint16_t(qfmpString)<<8)+qfmpString:
            return qfmpResult(l.str+r.str);
            break;
        default:
            if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be added").arg(l.typeName()).arg(r.typeName()));
            break;
    }
    return invalidResult();
}

qfmpResult qfmpResult::sub(const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    uint16_t lt=l.type;
    uint16_t rt=r.type;
    uint16_t tt=(lt<<8)+rt;
    //qDebug()<<QString::number(tt, 16);
    switch(tt) {
        case (qfmpDouble<<8)+qfmpDouble:
            return qfmpResult(l.num-r.num);
            break;
        case (qfmpDoubleVector<<8)+qfmpDouble: {
            qfmpResult re;
            re.setDoubleVec(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                re.numVec[i]=l.numVec[i]-r.num;
            }
            return re;
            break;
        }
        case (qfmpDouble<<8)+qfmpDoubleVector: {
            qfmpResult re;
            re.setDoubleVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=l.num-r.numVec[i];
            }
            return re;
            break;
        }
        case (qfmpDoubleVector<<8)+qfmpDoubleVector: {
            if (l.length()!=r.length()) {
                if (p) p->qfmpError(QObject::tr("arguments have to have the same length, but lengthes were left=%1, right=%2").arg(l.length()).arg(r.length()));
                return invalidResult();
            }
            qfmpResult re;
            re.setDoubleVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=l.numVec[i]-r.numVec[i];
            }
            return re;
            break;
        }
        default:
            if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be subtracted").arg(l.typeName()).arg(r.typeName()));
            break;
    }
    return invalidResult();
}

qfmpResult qfmpResult::mul(const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    uint16_t lt=l.type;
    uint16_t rt=r.type;
    uint16_t tt=(lt<<8)+rt;
    //qDebug()<<QString::number(tt, 16);
    switch(tt) {
        case (qfmpDouble<<8)+qfmpDouble:
            return qfmpResult(l.num*r.num);
            break;
        case (qfmpDoubleVector<<8)+qfmpDouble: {
            qfmpResult re;
            re.setDoubleVec(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                re.numVec[i]=l.numVec[i]*r.num;
            }
            return re;
            break;
        }
        case (qfmpDouble<<8)+qfmpDoubleVector: {
            qfmpResult re;
            re.setDoubleVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=l.num*r.numVec[i];
            }
            return re;
            break;
        }
        case (qfmpDoubleVector<<8)+qfmpDoubleVector: {
            if (l.length()!=r.length()) {
                if (p) p->qfmpError(QObject::tr("arguments have to have the same length, but lengthes were left=%1, right=%2").arg(l.length()).arg(r.length()));
                return invalidResult();
            }
            qfmpResult re;
            re.setDoubleVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=l.numVec[i]*r.numVec[i];
            }
            return re;
            break;
        }
        default:
            if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be multiplied").arg(l.typeName()).arg(r.typeName()));
            break;
    }
    return invalidResult();
}

qfmpResult qfmpResult::div(const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    uint16_t lt=l.type;
    uint16_t rt=r.type;
    uint16_t tt=(lt<<8)+rt;
    //qDebug()<<QString::number(tt, 16);
    switch(tt) {
        case (qfmpDouble<<8)+qfmpDouble:
            return qfmpResult(l.num/r.num);
            break;
        case (qfmpDoubleVector<<8)+qfmpDouble: {
            qfmpResult re;
            re.setDoubleVec(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                re.numVec[i]=l.numVec[i]/r.num;
            }
            return re;
            break;
        }
        case (qfmpDouble<<8)+qfmpDoubleVector: {
            qfmpResult re;
            re.setDoubleVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=l.num/r.numVec[i];
            }
            return re;
            break;
        }
        case (qfmpDoubleVector<<8)+qfmpDoubleVector: {
            if (l.length()!=r.length()) {
                if (p) p->qfmpError(QObject::tr("arguments have to have the same length, but lengthes were left=%1, right=%2").arg(l.length()).arg(r.length()));
                return invalidResult();
            }
            qfmpResult re;
            re.setDoubleVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=l.numVec[i]/r.numVec[i];
            }
            return re;
            break;
        }
        default:
            if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be divided").arg(l.typeName()).arg(r.typeName()));
            break;
    }
    return invalidResult();
}

qfmpResult qfmpResult::mod(const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    uint16_t lt=l.type;
    uint16_t rt=r.type;
    uint16_t tt=(lt<<8)+rt;
    //qDebug()<<QString::number(tt, 16);
    switch(tt) {
        case (qfmpDouble<<8)+qfmpDouble:
            return qfmpResult(double(l.toInteger()%r.toInteger()));
            break;
        case (qfmpDoubleVector<<8)+qfmpDouble: {
            qfmpResult re;
            re.setDoubleVec(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                re.numVec[i]=int32_t(l.numVec[i])%r.toInteger();
            }
            return re;
            break;
        }
        case (qfmpDouble<<8)+qfmpDoubleVector: {
            qfmpResult re;
            re.setDoubleVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=l.toInteger()%int32_t(r.numVec[i]);
            }
            return re;
            break;
        }
        case (qfmpDoubleVector<<8)+qfmpDoubleVector: {
            if (l.length()!=r.length()) {
                if (p) p->qfmpError(QObject::tr("arguments have to have the same length, but lengthes were left=%1, right=%2").arg(l.length()).arg(r.length()));
                return invalidResult();
            }
            qfmpResult re;
            re.setDoubleVec(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                re.numVec[i]=int32_t(l.numVec[i])%int32_t(r.numVec[i]);
            }
            return re;
            break;
        }
        default:
            if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be used in mod operation").arg(l.typeName()).arg(r.typeName()));
            break;
    }
    return invalidResult();
}

qfmpResult qfmpResult::power(const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    uint16_t lt=l.type;
    uint16_t rt=r.type;
    uint16_t tt=(lt<<8)+rt;
    //qDebug()<<QString::number(tt, 16);
    switch(tt) {
        case (qfmpDouble<<8)+qfmpDouble:
            return qfmpResult(pow(l.num,r.num));
            break;
        case (qfmpDoubleVector<<8)+qfmpDouble: {
            qfmpResult re;
            re.setDoubleVec(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                re.numVec[i]=pow(l.numVec[i],r.num);
            }
            return re;
            break;
        }
        case (qfmpDouble<<8)+qfmpDoubleVector: {
            qfmpResult re;
            re.setDoubleVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=pow(l.num,r.numVec[i]);
            }
            return re;
            break;
        }
        case (qfmpDoubleVector<<8)+qfmpDoubleVector: {
            if (l.length()!=r.length()) {
                if (p) p->qfmpError(QObject::tr("arguments have to have the same length, but lengthes were left=%1, right=%2").arg(l.length()).arg(r.length()));
                return invalidResult();
            }
            qfmpResult re;
            re.setDoubleVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=pow(l.numVec[i],r.numVec[i]);
            }
            return re;
            break;
        }
        default:
            if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be used in power operation").arg(l.typeName()).arg(r.typeName()));
            break;
    }
    return invalidResult();
}

qfmpResult qfmpResult::bitwiseand(const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    uint16_t lt=l.type;
    uint16_t rt=r.type;
    uint16_t tt=(lt<<8)+rt;
    //qDebug()<<QString::number(tt, 16);
    switch(tt) {
        case (qfmpDouble<<8)+qfmpDouble:
            return qfmpResult(double(l.toInteger()&r.toInteger()));
            break;
        case (qfmpDoubleVector<<8)+qfmpDouble: {
            qfmpResult re;
            re.setDoubleVec(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                re.numVec[i]=int32_t(l.numVec[i])&r.toInteger();
            }
            return re;
            break;
        }
        case (qfmpDouble<<8)+qfmpDoubleVector: {
            qfmpResult re;
            re.setDoubleVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=l.toInteger()&int32_t(r.numVec[i]);
            }
            return re;
            break;
        }
        case (qfmpDoubleVector<<8)+qfmpDoubleVector: {
            if (l.length()!=r.length()) {
                if (p) p->qfmpError(QObject::tr("arguments have to have the same length, but lengthes were left=%1, right=%2").arg(l.length()).arg(r.length()));
                return invalidResult();
            }
            qfmpResult re;
            re.setDoubleVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=int32_t(l.numVec[i])&int32_t(r.numVec[i]);
            }
            return re;
            break;
        }
        default:
            if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be used in bitwise and operation").arg(l.typeName()).arg(r.typeName()));
            break;
    }
    return invalidResult();
}

qfmpResult qfmpResult::bitwiseor(const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    uint16_t lt=l.type;
    uint16_t rt=r.type;
    uint16_t tt=(lt<<8)+rt;
    //qDebug()<<QString::number(tt, 16);
    switch(tt) {
        case (qfmpDouble<<8)+qfmpDouble:
            return qfmpResult(double(l.toInteger()|r.toInteger()));
            break;
        case (qfmpDoubleVector<<8)+qfmpDouble: {
            qfmpResult re;
            re.setDoubleVec(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                re.numVec[i]=int32_t(l.numVec[i])|r.toInteger();
            }
            return re;
            break;
        }
        case (qfmpDouble<<8)+qfmpDoubleVector: {
            qfmpResult re;
            re.setDoubleVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=l.toInteger()|int32_t(r.numVec[i]);
            }
            return re;
            break;
        }
        case (qfmpDoubleVector<<8)+qfmpDoubleVector: {
            if (l.length()!=r.length()) {
                if (p) p->qfmpError(QObject::tr("arguments have to have the same length, but lengthes were left=%1, right=%2").arg(l.length()).arg(r.length()));
                return invalidResult();
            }
            qfmpResult re;
            re.setDoubleVec(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=int32_t(l.numVec[i])|int32_t(r.numVec[i]);
            }
            return re;
            break;
        }
        default:
            if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be used in bitwise or operation").arg(l.typeName()).arg(r.typeName()));
            break;
    }
    return invalidResult();
}

qfmpResult qfmpResult::logicand(const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    uint16_t lt=l.type;
    uint16_t rt=r.type;
    uint16_t tt=(lt<<8)+rt;
    //qDebug()<<QString::number(tt, 16);
    switch(tt) {
        case (qfmpBool<<8)+qfmpBool:
            return qfmpResult(l.boolean&&r.boolean);
            break;

        default:
            if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be used in logic and operation").arg(l.typeName()).arg(r.typeName()));
            break;
    }
    return invalidResult();
}

qfmpResult qfmpResult::logicor(const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    uint16_t lt=l.type;
    uint16_t rt=r.type;
    uint16_t tt=(lt<<8)+rt;
    //qDebug()<<QString::number(tt, 16);
    switch(tt) {
        case (qfmpBool<<8)+qfmpBool:
            return qfmpResult(l.boolean||r.boolean);
            break;

        default:
            if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be used in logic or operation").arg(l.typeName()).arg(r.typeName()));
            break;
    }
    return invalidResult();
}

qfmpResult qfmpResult::logicnot(const qfmpResult &l, QFMathParser *p)
{
    switch(l.type) {
        case qfmpBool: return qfmpResult(!l.boolean);
        default:
            if (p) p->qfmpError(QObject::tr("argument of type %1 may not be used in logic not operation").arg(l.typeName()));
            break;

    }

    return invalidResult();
}

qfmpResult qfmpResult::neg(const qfmpResult &l, QFMathParser *p)
{
    switch(l.type) {
        case qfmpDouble: return qfmpResult(-l.num);
        case qfmpDoubleVector: {
            qfmpResult re=l;
            for (int i=0; i<l.numVec.size(); i++) {
                re.numVec[i]=l.numVec[i];
            }
            return re;
            break;
        }
        default:
            if (p) p->qfmpError(QObject::tr("argument of type %1 may not be used in negation operation").arg(l.typeName()));
            break;

    }

    return invalidResult();
}

qfmpResult qfmpResult::bitwisenot(const qfmpResult &l, QFMathParser *p)
{
    switch(l.type) {
        case qfmpDouble: return qfmpResult(double(~l.toInteger()));
        case qfmpDoubleVector: {
            qfmpResult re=l;
            for (int i=0; i<l.numVec.size(); i++) {
                re.numVec[i]=~int32_t(l.numVec[i]);
            }
            return re;
            break;
        }
        default:
            if (p) p->qfmpError(QObject::tr("argument of type %1 may not be used in bitwise not operation").arg(l.typeName()));
            break;

    }

    return invalidResult();
}

QString qfmpResult::typeName() const
{
    return QFMathParser::resultTypeToString(type);
}


QVector<int> qfmpResult::asIntVector() const
{
    if (type==qfmpDouble) return QVector<int>(1, num);
    else if (type==qfmpDoubleVector) {
        QVector<int> ii(numVec.size(), 0);
        for (int i=0; i<numVec.size(); i++) {
            ii[i]=numVec[i];
        }
        return ii;
    }
    return QVector<int>();
}



















