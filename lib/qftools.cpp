/*
    Copyright (c) 2008-2015 Jan W. Krieger, German Cancer Research Center

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

#include "qftools.h"
#include <cfloat>
#include <cmath>
#include <QList>
#include <QApplication>
#include <QDesktopWidget>
#include <QLocale>
#include <QDebug>
#include <QElapsedTimer>
#include <QSysInfo>
#include <QProcess>
#include <QPainter>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#ifndef QFMATHPARSER_MATHPARSERTEST
#  include "programoptions.h"
#  include "qfpluginservices.h"
#  include "qfhtmlhelptools.h"
#  include "qfversion.h"
#endif
#include <QMutex>
#include <QPrinter>
#include <QPrinterInfo>
#include <QPrinter>
#if (QT_VERSION > QT_VERSION_CHECK(5, 2, 0))
#include <QPdfWriter>
#endif

#ifndef __LINUX__
# if defined(linux)
#  define __LINUX__
# endif
#endif

#ifdef __LINUX__
#include "malloc.h"
#include "stdlib.h"
#endif

#ifndef QF_ALIGNMENT_BYTES
#define QF_ALIGNMENT_BYTES 32
#endif

#if !defined(QF_DONT_USE_ALIGNED_MALLOC)
#warning("using aligned _aligned_malloc, _aligned_free, _aligned_realloc")
#else
#warning("using non-aligned malloc, free, realloc")
#endif

static QMutex qfallocationMutex(QMutex::Recursive);

void* qfMalloc(size_t size) {
#ifndef QF_DONT_EXPLICITLY_MUTEXLOC_MALLOC
    QMutexLocker locker(&qfallocationMutex);
#endif
#ifdef __LINUX__
    #if  !defined(QF_DONT_USE_ALIGNED_MALLOC)
    return aligned_alloc(QF_ALIGNMENT_BYTES, size);
    #else
    return malloc(size);
    #endif
#else
    #if  !defined(QF_DONT_USE_ALIGNED_MALLOC)
    return _aligned_malloc(size, QF_ALIGNMENT_BYTES);
    #else
    return malloc(size);
    #endif
#endif
}

void* qfCalloc(size_t size) {
    return qfCalloc(size, 1);
}

void* qfCalloc(size_t num, size_t size) {
#ifndef QF_DONT_EXPLICITLY_MUTEXLOC_MALLOC
    QMutexLocker locker(&qfallocationMutex);
#endif
    void* res=qfMalloc(num*size);
    memset(res, 0, num*size);
    return res;
}

void* qfRealloc (void* ptr, size_t size) {
#ifndef QF_DONT_EXPLICITLY_MUTEXLOC_MALLOC
    QMutexLocker locker(&qfallocationMutex);
#endif
#ifdef __LINUX__
    #if !defined(QF_DONT_USE_ALIGNED_MALLOC)
    return realloc(ptr, size);
    #else
    return realloc(ptr, size);
    #endif
#else
    #if !defined(QF_DONT_USE_ALIGNED_MALLOC)
    return _aligned_realloc(ptr, size, QF_ALIGNMENT_BYTES);
    #else
    return realloc(ptr, size);
    #endif
#endif
}

void qfFree(void* data) {
    if (!data) return;
#ifndef QF_DONT_EXPLICITLY_MUTEXLOC_MALLOC
    QMutexLocker locker(&qfallocationMutex);
#endif
#ifdef __LINUX__
    #if !defined(QF_DONT_USE_ALIGNED_MALLOC)
    free(data);
    #else
    free(data);
    #endif
#else
    #if !defined(QF_DONT_USE_ALIGNED_MALLOC)
    _aligned_free(data);
    #else
    free(data);
    #endif

#endif
}






QAction* getSeparatorAction(QObject* parent) {
    QAction* a=new QAction(parent);
    a->setSeparator(true);
    return a;
}




QAction* createActionAndButton(QToolButton*& button, QWidget* parent) {
    return createActionAndButton(button, QIcon(), QString(), parent);
}


QAction* createActionAndButton(QToolButton*& button, const QString& text, QWidget* parent) {
    return createActionAndButton(button, QIcon(), text, parent);
}

QAction* createActionAndButton(QToolButton*& button, const QIcon& icon, const QString& text, QWidget* parent) {
    QAction* act=new QAction(icon, text, parent);
    button=new QToolButton(parent);
    button->setDefaultAction(act);
    return act;
}




QToolButton* createButtonAndAction(QAction*& action, QWidget* parent) {
    return createButtonAndAction(action, QIcon(), QString(), parent);
}


QToolButton* createButtonAndAction(QAction*& action, const QString& text, QWidget* parent) {
    return createButtonAndAction(action, QIcon(), text, parent);
}

QToolButton* createButtonAndAction(QAction*& action, const QIcon& icon, const QString& text, QWidget* parent) {
    action=new QAction(icon, text, parent);
    QToolButton* button=new QToolButton(parent);
    button->setDefaultAction(action);
    return button;
}

QToolButton* createButtonForAction(QAction* action, QWidget* parent) {
    QToolButton* button=new QToolButton(parent);
    button->setDefaultAction(action);
    return button;
}

QToolButton* createButtonAndActionShowText(QAction*& action, QWidget* parent) {
    QToolButton* button= createButtonAndAction(action, QIcon(), QString(), parent);
    button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    QSizePolicy p=button->sizePolicy();
    p.setControlType(QSizePolicy::PushButton);
    p.setHorizontalPolicy(QSizePolicy::Minimum);
    button->setSizePolicy(p);
    button->setFocusPolicy(Qt::StrongFocus);
    return button;
}


QToolButton* createButtonAndActionShowText(QAction*& action, const QString& text, QWidget* parent) {
    QToolButton* button= createButtonAndAction(action, QIcon(), text, parent);
    button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    QSizePolicy p=button->sizePolicy();
    p.setControlType(QSizePolicy::PushButton);
    p.setHorizontalPolicy(QSizePolicy::Minimum);
    button->setSizePolicy(p);
    button->setFocusPolicy(Qt::StrongFocus);
    return button;
}

QToolButton* createButtonAndActionShowText(QAction*& action, const QIcon& icon, const QString& text, QWidget* parent) {
    QToolButton* button= createButtonAndAction(action, icon, text, parent);
    button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    QSizePolicy p=button->sizePolicy();
    p.setControlType(QSizePolicy::PushButton);
    p.setHorizontalPolicy(QSizePolicy::Minimum);
    button->setSizePolicy(p);
    button->setFocusPolicy(Qt::StrongFocus);
    return button;
}

QToolButton* createButtonForActionShowText(QAction* action, QWidget* parent) {
    QToolButton* button= createButtonForAction(action, parent);
    button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    QSizePolicy p=button->sizePolicy();
    p.setControlType(QSizePolicy::PushButton);
    p.setHorizontalPolicy(QSizePolicy::Minimum);
    button->setSizePolicy(p);
    button->setFocusPolicy(Qt::StrongFocus);
    return button;
}


void saveWidgetGeometry(QSettings& settings, QWidget* widget, QString prefix) {
    settings.setValue(prefix+"pos", widget->pos());
    settings.setValue(prefix+"size", widget->size());
}

void loadWidgetGeometry(QSettings& settings, QWidget* widget, QPoint defaultPosition, QSize defaultSize, QString prefix) {
    QPoint pos = settings.value(prefix+"pos", defaultPosition).toPoint();
    QSize size = settings.value(prefix+"size", defaultSize).toSize();

    if (pos.x()<0 || pos.x()>QApplication::desktop()->screenGeometry(widget).width()) pos.setX(0);
    if (pos.y()<0 || pos.y()>QApplication::desktop()->screenGeometry(widget).height()) pos.setY(0);
    widget->move(pos);

    widget->resize(size.boundedTo(QApplication::desktop()->screenGeometry(widget).size()));
}


void saveWidgetGeometry(QSettings* settings, QWidget* widget, QString prefix) {
    if (settings) saveWidgetGeometry(*settings,widget, prefix);
}
void saveWidgetGeometry(ProgramOptions* settings, QWidget* widget, QString prefix) {
#ifndef QFMATHPARSER_MATHPARSERTEST
    if (settings) saveWidgetGeometry(*settings->getQSettings(),widget, prefix);
#endif
}

void loadWidgetGeometry(QSettings* settings, QWidget* widget, QString prefix) {
    if (settings) loadWidgetGeometry(*settings,widget, prefix);
}
void loadWidgetGeometry(ProgramOptions* settings, QWidget* widget, QString prefix) {
#ifndef QFMATHPARSER_MATHPARSERTEST
    if (settings) loadWidgetGeometry(*settings->getQSettings(),widget, prefix);
#endif
}

void loadWidgetGeometry(QSettings& settings, QWidget* widget, QString prefix) {
    loadWidgetGeometry(settings, widget, QPoint(10, 10), QSize(100, 100), prefix);
}

void saveSplitter(QSettings& settings, QSplitter* splitter, QString prefix) {
    /*QList<int> sizes=splitter->sizes();
    QString data="";
    for (int i=0; i<sizes.size(); i++) {
        if (!data.isEmpty()) data=data+",";
        data=data+QString::number(sizes[i]);
    }
    settings.setValue(prefix+"splitter_sizes", data);*/
    settings.setValue(prefix+"splitter_sizes", splitter->saveState());
}

void loadSplitter(QSettings& settings, QSplitter* splitter, QString prefix) {
    /*QString data=settings.value(prefix+"splitter_sizes", "").toString();
    QList<int> sizes, s1;
    QStringList sl=data.split(",");
    for (int i=0; i<sl.size(); i++) {
        sizes.append(sl[i].toInt());
    }
    s1=splitter->sizes();
    for (int i=0; i<s1.count(); i++) {
        if (i<sizes.size()) s1[i]=sizes[i];
    }*/
    if (settings.contains(prefix+"splitter_sizes"))  splitter->restoreState(settings.value(prefix+"splitter_sizes", splitter->saveState()).toByteArray());
}




void writeQVariant(QXmlStreamWriter& w, const QVariant& v){
    w.writeAttribute("type", getQVariantType(v));
    if (v.type()==QVariant::List) {
        QVariantList vl=v.toList();
        for (int j=0; j<vl.size(); j++) {
            w.writeStartElement("list_item");
            const QVariant v=vl[j];
            writeQVariant(w, v);
            w.writeEndElement();
        }
    } else if (v.type()==QVariant::StringList) {
        QStringList vl=v.toStringList();
        for (int j=0; j<vl.size(); j++) {
            w.writeStartElement("list_item");
            w.writeCDATA(vl[j]);
            w.writeEndElement();
        }
    } else {
        w.writeCDATA(getQVariantData(v));
    }
}

QVariant readQVariant(QDomElement& e) {
    QVariant res;
    QString type=e.attribute("type");
    if (type=="list") {
        QDomElement c=e.firstChildElement("list_item");
        QVariantList vl;
        while (!c.isNull()) {
            vl.append(readQVariant(c));
            c=c.nextSiblingElement("list_item");
        }
        res=vl;
    } else if (type=="stringlist") {
        QDomElement c=e.firstChildElement("list_item");
        QStringList vl;
        while (!c.isNull()) {
            vl.append(c.text());
            c=c.nextSiblingElement("list_item");
        }
        res=vl;
    } else {
        res=getQVariantFromString(type, e.text());
    }
    return res;
}

QString getQVariantType(const QVariant& variant) {
    QString t="invalid";
    if (variant.isValid()) {
        switch(variant.type()) {
            case QVariant::Bool: t="bool"; break;
            case QVariant::Char: t="char"; break;
            case QVariant::Date: t="date"; break;
            case QVariant::DateTime: t="datetime"; break;
            case QVariant::Double: t="double"; break;
            case QVariant::Int: t="int"; break;
            case QVariant::LongLong: t="longlong"; break;
            case QVariant::String: t="string"; break;
            case QVariant::StringList: t="stringlist"; break;
            case QVariant::UInt: t="uint"; break;
            case QVariant::ULongLong: t="ulonglong"; break;
            case QVariant::Time: t="time"; break;
            case QVariant::Point: t="point"; break;
            case QVariant::Size: t="size"; break;
            case QVariant::SizeF: t="sizef"; break;
            case QVariant::BitArray: t="bitarray"; break;
            case QVariant::ByteArray: t="bytearray"; break;
            case QVariant::Color: t="color"; break;
            case QVariant::Font: t="font"; break;
            case QVariant::Hash: t="hash"; break;
            case QVariant::List: t="list"; break;
            case QVariant::Map: t="map"; break;
            case QVariant::Rect: t="rect"; break;
            case QVariant::RectF: t="rectf"; break;
            case QVariant::Invalid: t="invalid"; break;
            case QVariant::Url: t="url"; break;
            default: t="unknown"; break;
        }
    }
    return t;
}

QString getQVariantData(const QVariant& variant) {
    if (!variant.isValid()) return QString();
    QString t="";
    QLocale loc=QLocale::c();
    loc.setNumberOptions(QLocale::OmitGroupSeparator);
    switch(variant.type()) {
        case QVariant::Double: t=loc.toString(variant.toDouble(), 'g', 10); break;
        default: t=variant.toString(); break;
    }
    return t;
}

QString getQVariantDataCEscaped(const QVariant& variant, QChar quotations) {
    if (!variant.isValid()) return QString();
    QString t="";
    QLocale loc=QLocale::c();
    loc.setNumberOptions(QLocale::OmitGroupSeparator);
    switch(variant.type()) {
        case QVariant::Double: t=loc.toString(variant.toDouble(), 'g', 10); break;
        case QVariant::Int: t=QString::number(variant.toInt()); break;
        case QVariant::UInt: t=QString::number(variant.toUInt()); break;
        case QVariant::LongLong: t=QString::number(variant.toLongLong()); break;
        case QVariant::ULongLong: t=QString::number(variant.toULongLong()); break;
        default: t=qfCEscaped(variant.toString()); if (quotations!='\0') {t=quotations+t+quotations;} break;
    }
    return t;
}

QVariant getQVariantFromString(const QString& type, const QString& data) {
    QVariant d=data;
    //std::cout<<"  prop "<<n.toStdString()<<" ["+t.toStdString()+"] = "<<d.toString().toStdString()<<"\n";
    QLocale loc=QLocale::c();
    loc.setNumberOptions(QLocale::OmitGroupSeparator);
    bool c=false;
    if (type=="bool") { c=d.convert(QVariant::Bool); }
    else if (type=="char" || type.toLower()=="qchar") { c=d.convert(QVariant::Char); }
    else if (type=="date" || type.toLower()=="qdate") { c=d.convert(QVariant::Date); }
    else if (type=="datetime" || type.toLower()=="qdatetime") { c=d.convert(QVariant::DateTime); }
    else if (type=="double") { d=loc.toDouble(data, &c); }
    else if (type=="int") { c=d.convert(QVariant::Int); }
    else if (type=="longlong") { c=d.convert(QVariant::LongLong); }
    else if (type=="string" || type.toLower()=="qstring") { c=d.convert(QVariant::String); }
    else if (type=="stringlist" || type.toLower()=="qstringlist") { c=d.convert(QVariant::StringList); }
    else if (type=="uint") { c=d.convert(QVariant::UInt); }
    else if (type=="ulonglong") { c=d.convert(QVariant::ULongLong); }
    else if (type=="time" || type.toLower()=="qtime") { c=d.convert(QVariant::Time); }
    else if (type=="bytearray" || type.toLower()=="qbytearray") { c=d.convert(QVariant::ByteArray); }
    else if (type=="color" || type.toLower()=="qcolor") { c=d.convert(QVariant::Color); }
    else {
        d=QVariant();
    }
    if (!c) {
        d=QVariant();
    }
    return d;
}

QString doubleToQString(double value, int prec, char f, QChar decimalSeparator) {
    QLocale loc=QLocale::c();
    loc.setNumberOptions(QLocale::OmitGroupSeparator);
    QString res=loc.toString(value, f, prec);
    if (loc.decimalPoint()!=decimalSeparator) {
        res=res.replace(loc.decimalPoint(), decimalSeparator);
    }
    return res;
}

QString doubleToQStringNoTrail(double value, int prec, QChar decimalSeparator ) {
    QString res=doubleToQString(value, prec, 'f', decimalSeparator).trimmed();
    int i=res.size()-1;
    while (i>=0 && res[i]=='0') {
        i--;
    }
    res=res.left(i);
    if (res.right(1)==QString(decimalSeparator)) res=res.left(res.size()-1);
    return res;
}

QString doubleVecToQString(const QVector<double>& value, int prec, char f, QChar decimalSeparator, const QString itemSeparator) {
    QLocale loc=QLocale::c();
    loc.setNumberOptions(QLocale::OmitGroupSeparator);
    QString out="";
    for (int i=0; i<value.size(); i++) {
        if (i>0) out+=itemSeparator;
        QString res=loc.toString(value[i], f, prec);
        if (loc.decimalPoint()!=decimalSeparator) {
            res=res.replace(loc.decimalPoint(), decimalSeparator);
        }
        out+=res;
    }
    return out;
}

QString bitarrayToQString(const QBitArray& value, const QString itemSeparator, const QString& trueName, const QString& falseName ) {
    QString out="";
    for (int i=0; i<value.size(); i++) {
        if (i>0) out+=itemSeparator;
        if (value[i]) out+=trueName;
        else out+=falseName;
    }
    return out;

}

QVector<double> bitarrayToNumVec(const QBitArray& value, double trueValue, double falseValue) {
    QVector<double> v;
    for (int i=0; i<value.size(); i++) {
        v<<(value[i]?trueValue:falseValue);
    }
    return v;
}

QString boolvectorToQString(const QVector<bool>& value, const QString itemSeparator, const QString& trueName, const QString& falseName ) {
    QString out="";
    for (int i=0; i<value.size(); i++) {
        if (i>0) out+=itemSeparator;
        if (value[i]) out+=trueName;
        else out+=falseName;
    }
    return out;

}

QVector<double> boolvectorToNumVec(const QVector<bool>& value, double trueValue, double falseValue) {
    QVector<double> v;
    for (int i=0; i<value.size(); i++) {
        v<<(value[i]?trueValue:falseValue);
    }
    return v;
}

QString CDoubleToQString(double value) {
    QLocale loc=QLocale::c();
    loc.setNumberOptions(QLocale::OmitGroupSeparator);
    return loc.toString(value, 'g', 18);
}

QString CDoubleListToQString(const QList<double> values, const QString& separator) {
    QString result="";
    foreach(double value, values) {
        if (!result.isEmpty()) result+=separator;
        result+=CDoubleToQString(value);
    }
    return result;
}


double CQStringToDouble(QString value) {
    QLocale loc=QLocale::c();
    loc.setNumberOptions(QLocale::OmitGroupSeparator);
    return loc.toDouble(value) ;
}

double QStringToDouble(QString value) {
    QString v=value;
    if (value.contains(',')) {
        v=value.replace(',', '.');
    }
    QLocale loc=QLocale::c();
    loc.setNumberOptions(QLocale::OmitGroupSeparator);
    return loc.toDouble(v) ;
}

QString boolToQString(bool data) {
    if (data) return QString("true");
    return QString("false");
}

bool QStringToBool(const QString& data){
    QString d=data.toLower();
    if (d=="true") return true;
    if (d=="t") return true;
    if (d=="1") return true;
    if (d=="j") return true;
    if (d=="y") return true;
    if (d=="yes") return true;
    if (d=="ja") return true;
    bool ok=false;
    int i=d.toInt(&ok);
    if (ok) return i!=0;
    return false;
}

QColor QStringToQColor(const QString& data) {
    QString d=data;
    if (d.startsWith("#")) d=d.replace(' ', '0');
    return QColor(d);
}

bool touchFile(const QString& filename) {
    if (QFile::exists(filename)) return true;
    QFile f(filename);
    if (f.open(QIODevice::WriteOnly)) {
        f.close();
        return true;
    }
    return false;
}

QString replaceFileExt(const QString& filename, const QString& extension) {
    QFileInfo fi(filename);
    QString ext=extension;
    if (!ext.startsWith('.')) ext=QString(".")+ext;
    QString f=fi.absolutePath()+"/"+fi.baseName()+ext;
    return f;
}

QString readFile(const QString& filename) {
    if (!QFile::exists(filename)) return QString();
    QFile f(filename);
    if (f.open(QIODevice::ReadOnly)) {
        return f.readAll();
        f.close();
    }
    return QString();
}



QString escapify(const QString& text){
  //QElapsedTimer time;
  //time.start();
  QString res="";
  res.reserve(text.size());
  for (int i=0; i<text.size(); i++) {
      QChar c=text.at(i);
      if (c.unicode()=='\0') res.append(QLatin1String("\\0"));
      else if (c.unicode()=='\n') res.append(QLatin1String("\\n"));
      else if (c.unicode()=='\r') res.append(QLatin1String("\\r"));
      else if (c.unicode()=='\t') res.append(QLatin1String("\\t"));
      else if (c.unicode()=='\\') res.append(QLatin1String("\\\\"));
      else if (c.unicode()=='"') res.append(QLatin1String("\\\""));
      else if (c.unicode()=='\'') res.append(QLatin1String("\\'"));
      else if (c.unicode()=='\a') res.append(QLatin1String("\\a"));
      else if (c.unicode()=='\b') res.append(QLatin1String("\\b"));
      else if (c.unicode()=='\v') res.append(QLatin1String("\\v"));
      else if (c.unicode()=='\f') res.append(QLatin1String("\\f"));
      else if (c.unicode()=='\e') res.append(QLatin1String("\\e"));
      else if (c.unicode()=='\?') res.append(QLatin1String("\\?"));
      else if (c.unicode()<32) {
          QString n=QString::number((unsigned char)c.unicode(), 16);
          if (n.size()<3) n.prepend(QString(3-n.size(), QChar('0')));
          res.append(QLatin1String("\\x"))+n;
      } else res+=c;
  }
  //qDebug()<<"escapify()    "<<time.elapsed()<<" ms";
  return res;
}


QString deescapify(const QString& text){
  QString res="";
  res.reserve(text.size());
  if (text.size()>0) {
    int i=0;
    while (i<text.size()) {
      QChar c=text.at(i);
      if (c!='\\') {
        res+=c;
      } else {
        if (i+1<text.size()) {
          char next=text[i+1].toLatin1();
          switch(next) {
            case '0': res+='\0'; i++; break;
            case 'n': res+='\n'; i++; break;
            case 'r': res+='\r'; i++; break;
            case 't': res+='\t'; i++; break;
            case 'a': res+='\a'; i++; break;
            case 'b': res+='\b'; i++; break;
            case 'v': res+='\v'; i++; break;
            case 'f': res+='\f'; i++; break;
            case 'e': res+='\e'; i++; break;
            case '\\': res+='\\'; i++; break;
            case '"': res+='\"'; i++; break;
            case '?': res+='\?'; i++; break;
            case '\'': res+='\''; i++; break;
            case 'x':
            case 'X':
              if (i+3<text.size()) {
                QString num=text.mid(i+2,2);
                i+=3;
                res+=QChar(num.toUInt(0, 16));
              } else i++;
              break;
          }
        }
      }
      i++;
    }
  }
  return res;
}

QString escapify(const QStringList& text) {
    QString s;
    QElapsedTimer time;
    time.start();
    s.reserve(text.size()*100);
    for (int si=0; si<text.size(); si++) {
        if (si>0) s+=";";
        s+=QLatin1String("\'")+escapify(text[si])+QLatin1String("\'");
    }
    //qDebug()<<"escapify(QStringList, N="<<text.size()<<" )    "<<time.elapsed()<<" ms";
    return s;
}

QStringList deescapifyList(const QString& text, char stringDelim, char separator) {
    QStringList result;
    QString res="";
    bool inString=false;
    if (text.size()>0) {
        int i=0;
        while (i<text.size()) {
            QChar c=text.at(i);
            if (c==stringDelim) {
                inString=!inString;
            } else if ((!inString) && (c==separator)) {
                if (!res.isEmpty()) {
                    result.append(res);
                    res="";
                }
            } else if (c!='\\') {
                res+=c;
            } else if (inString) {
              if (i+1<text.size()) {
                char next=text[i+1].toLatin1();
                switch(next) {
                  case '0': res+='\0'; i++; break;
                  case 'n': res+='\n'; i++; break;
                  case 'r': res+='\r'; i++; break;
                  case 't': res+='\t'; i++; break;
                  case 'a': res+='\a'; i++; break;
                  case 'b': res+='\b'; i++; break;
                  case 'v': res+='\v'; i++; break;
                  case 'f': res+='\f'; i++; break;
                  case 'e': res+='\e'; i++; break;
                  case '\\': res+='\\'; i++; break;
                  case '"': res+='\"'; i++; break;
                  case '?': res+='\?'; i++; break;
                  case '\'': res+='\''; i++; break;
                  case 'x':
                  case 'X':
                    if (i+3<text.size()) {
                      QString num=text.mid(i+2,2);
                      i+=3;
                      res+=QChar(num.toUInt(0, 16));
                    } else i++;
                    break;
                }
              }
        }
        i++;
      }
    }
    if (!res.isEmpty()) result.append(res);
    return result;
}

QStringList deescapifyList(const QString& text) {
    QStringList result=deescapifyList(text, '\'', ';');
    if (result.size()<=0) result=deescapifyList(text, '\"', ';');
    return result;
}

QString	qfGetExistingDirectory ( QWidget * parent, const QString & caption, const QString & dir, QFileDialog::Options options ) {
    QFileDialog::Options opt=options;

    if (ProgramOptions::getConfigValue("quickfit/native_file_dialog", true).toBool()) {
        opt=options;
    } else {
        opt=options|QFileDialog::DontUseNativeDialog;
    }
    return QFileDialog::getExistingDirectory(parent, caption, dir, opt);
}



QString qfGetOpenFileNameSet (const QString& setPrefix,  QWidget * parent , const QString & caption , const QString & dir , const QString & filter , QString * selectedFilter , QFileDialog::Options options  ) {
    QString d=dir;
    QString f=filter.split(";;").value(0);
#ifndef QFMATHPARSER_MATHPARSERTEST
    d=ProgramOptions::getConfigValue(setPrefix+"lastDir",  dir).toString();
    f=ProgramOptions::getConfigValue(setPrefix+"lastFilter",  filter.split(";;").value(0)).toString();;
#endif
    if (selectedFilter && (!(*selectedFilter).isEmpty())) {
        f=*selectedFilter;
    }

    QString fn=qfGetOpenFileName(parent, caption, d, filter, &f, options);
    if (!fn.isEmpty()) {
#ifndef QFMATHPARSER_MATHPARSERTEST
        ProgramOptions::setConfigValue(setPrefix+"lastDir",  QFileInfo(fn).absoluteDir().absolutePath());
        ProgramOptions::setConfigValue(setPrefix+"lastFilter",  f);
#endif
    }
    if (selectedFilter) *selectedFilter=f;
    return fn;
}

QStringList qfGetOpenFileNamesSet (const QString& setPrefix,  QWidget * parent , const QString & caption , const QString & dir , const QString & filter , QString * selectedFilter , QFileDialog::Options options  ) {
    QString d=dir;
    QString f=filter.split(";;").value(0);
#ifndef QFMATHPARSER_MATHPARSERTEST
    d=ProgramOptions::getConfigValue(setPrefix+"lastDir",  dir).toString();
    f=ProgramOptions::getConfigValue(setPrefix+"lastFilter",  filter.split(";;").value(0)).toString();;
#endif
    if (selectedFilter && (!(*selectedFilter).isEmpty())) {
        f=*selectedFilter;
    }

    QStringList fn=qfGetOpenFileNames(parent, caption, d, filter, &f, options);
    if (fn.size()>0) {
#ifndef QFMATHPARSER_MATHPARSERTEST
        ProgramOptions::setConfigValue(setPrefix+"lastDir",  QFileInfo(fn.first()).absoluteDir().absolutePath());
        ProgramOptions::setConfigValue(setPrefix+"lastFilter",  f);
#endif
    }
    if (selectedFilter) *selectedFilter=f;
    return fn;
}

QString qfGetSaveFileNameSet (const QString& setPrefix,  QWidget * parent , const QString & caption , const QString & dir , const QString & filter , QString * selectedFilter , QFileDialog::Options options  ) {
    QString d=dir;
    QString f=filter.split(";;").value(0);
#ifndef QFMATHPARSER_MATHPARSERTEST
    d=ProgramOptions::getConfigValue(setPrefix+"lastDir",  dir).toString();
    f=ProgramOptions::getConfigValue(setPrefix+"lastFilter",  filter.split(";;").value(0)).toString();;
#endif
    if (selectedFilter && (!(*selectedFilter).isEmpty())) {
        f=*selectedFilter;
    }
    QString fn=qfGetSaveFileName(parent, caption, d, filter, &f, options);
    if (!fn.isEmpty()) {
#ifndef QFMATHPARSER_MATHPARSERTEST
        ProgramOptions::setConfigValue(setPrefix+"lastDir",  QFileInfo(fn).absoluteDir().absolutePath());
        ProgramOptions::setConfigValue(setPrefix+"lastFilter",  f);
#endif
    }
    if (selectedFilter) *selectedFilter=f;
    return fn;

}





QString	qfGetOpenFileName ( QWidget * parent, const QString & caption, const QString & dir, const QString & filter, QString * selectedFilter, QFileDialog::Options options ) {
    QFileDialog::Options opt=options;

    if (ProgramOptions::getConfigValue("quickfit/native_file_dialog", true).toBool()) {
        opt=options;
    } else {
        opt=options|QFileDialog::DontUseNativeDialog;
    }


    return QFileDialog::getOpenFileName(parent, caption, dir, filter, selectedFilter, opt);
}

QStringList	qfGetOpenFileNames ( QWidget * parent, const QString & caption, const QString & dir, const QString & filter, QString * selectedFilter, QFileDialog::Options options ) {
    QFileDialog::Options opt=options;

    if (ProgramOptions::getConfigValue("quickfit/native_file_dialog", true).toBool()) {
        opt=options;
    } else {
        opt=options|QFileDialog::DontUseNativeDialog;
    }

    return QFileDialog::getOpenFileNames(parent, caption, dir, filter, selectedFilter, opt);
}

QString	qfGetSaveFileName ( QWidget * parent, const QString & caption, const QString & dir, const QString & filter, QString * selectedFilter, QFileDialog::Options options )  {
    QString s="";
    QString selF="";
    if (selectedFilter) selF=*selectedFilter;

    QFileDialog::Options opt=options;

    if (ProgramOptions::getConfigValue("quickfit/native_file_dialog", true).toBool()) {
        opt=options;
    } else {
        opt=options|QFileDialog::DontUseNativeDialog;
    }

    s= QFileDialog::getSaveFileName(parent, caption, dir, filter, &selF, opt);

    if (!ProgramOptions::getConfigValue("quickfit/native_file_dialog", true).toBool()) {
        // this regexp recognizes the first file extension in filters like "Name File (*.cpw *.abc);; Filename 2 (*.txt)" then the first
        // extension is appended to the filename, if it does not already contain a suffix
        QRegExp rx(".*\\(\\*\\.(\\S+).*\\)\\w*[;;.]*", Qt::CaseInsensitive);
        rx.setMinimal(true);
        if (rx.indexIn(selF, 0)!=-1) {
            QString ext=rx.cap(1);
            if (QFileInfo(s).suffix().isEmpty()) {
                s=s+"."+ext;
            }
        }
    }


    if (selectedFilter) *selectedFilter=selF;
    return s;
}

int getApplicationBitDepth() {
    return 8*sizeof(void*);
}

QString getOSShortName() {
    QString result="???";


#ifdef Q_WS_X11
    result="linux";
#endif

#ifdef Q_OS_WIN32
    result="win";
#endif

#ifdef Q_WS_MAC
    result="mac";
#endif

    return result+QString::number(getApplicationBitDepth());
}


QString getOSName() {
    QString result="unknown OS";


#ifdef Q_WS_X11
    result=QObject::tr("Linux operating system");
    QProcess uname;
    uname.start("uname", QStringList() << "-r");
    if (uname.waitForStarted(500)) {
        if (uname.waitForFinished(500)) {
            QByteArray ba=uname.readAllStandardOutput();
            if (!ba.isEmpty()) result+=QString(" (%1)").arg(QString::fromLocal8Bit(ba));
        }
    }
#endif

#ifdef Q_OS_WIN32
    switch (QSysInfo::windowsVersion()) {
        case QSysInfo::WV_32s: result=QObject::tr("Windows 3.1 with Win 32s"); break;
        case QSysInfo::WV_95: result=QObject::tr("Windows 95"); break;
        case QSysInfo::WV_98: result=QObject::tr("Windows 98"); break;
        case QSysInfo::WV_Me: result=QObject::tr("Windows Me"); break;
        case QSysInfo::WV_NT: result=QObject::tr("Windows NT 4.0"); break;
        case QSysInfo::WV_2000: result=QObject::tr("Windows 2000"); break;
        case QSysInfo::WV_XP: result=QObject::tr("Windows XP"); break;
        case QSysInfo::WV_2003: result=QObject::tr("Windows Server 2003 or Windows Xp Professional 64-bit"); break;
        case QSysInfo::WV_VISTA: result=QObject::tr("Windows Vista or Server 2008"); break;
        case QSysInfo::WV_WINDOWS7: result=QObject::tr("Windows 7 or Server 2008 R2"); break;

        default: result=QObject::tr("unknown Windows version"); break;
    }
#endif

#ifdef Q_WS_MAC
    switch (QSysInfo::MacintoshVersion) {
        case QSysInfo::MV_10_3: result=QObject::tr("Mac OS X 10.3 (Panther)"); break;
        case QSysInfo::MV_10_4: result=QObject::tr("Mac OS X 10.4 (Tiger)"); break;
        case QSysInfo::MV_10_5: result=QObject::tr("Mac OS X 10.5 (Leopard)"); break;
        case QSysInfo::MV_10_6: result=QObject::tr("Mac OS X 10.6 (Snowleopard)"); break;
        case QSysInfo::MV_10_7: result=QObject::tr("Mac OS X 10.7 (Lion)"); break;
        case QSysInfo::MV_Unknown: result=QObject::tr("unknown MacOS version"); break;
        case QSysInfo::MV_9: result=QObject::tr("Mac OS 9"); break;
        case QSysInfo::MV_10_0: result=QObject::tr("Mac OS X 10.0 (Cheetah)"); break;
        case QSysInfo::MV_10_1: result=QObject::tr("Mac OS X 10.1 (Puma)"); break;
        case QSysInfo::MV_10_2: result=QObject::tr("Mac OS X 10.2 (Jaguar)"); break;

        default: result=QObject::tr("unknown Windows version"); break;
    }
#endif

    return result;
}

QString cleanStringForFilename(const QString& text, int maxLen, bool removeDot, bool removeSlash) {
    QString t=text.simplified();
    t=t.replace(" ", "_");
    if (removeDot) t=t=t.remove('.');
    if (removeSlash) {
        t=t.remove('\\');
        t=t.remove('/');
    }
    QString t1=t;
    t="";
    //qDebug()<<"cleanStringForFilename("<<text<<")   t1 = "<<t1;
    for (int i=0; i<t1.size(); i++) {
        if (t1[i]>='0' && t1[i]<='9') t=t+t1[i];
        else if (t1[i]>='A' && t1[i]<='z') t=t+t1[i];
        else {
            switch(t1[i].toLatin1()) {
                case '\xE4': t=t+"ae"; break;
                case '\xC4': t=t+"Ae"; break;
                case '\xF6': t=t+"oe"; break;
                case '\xD4': t=t+"Oe"; break;
                case '\xFC': t=t+"ue"; break;
                case '\xDC': t=t+"Ue"; break;
                case '\xDF': t=t+"ss"; break;
                case '@': t=t+"_at_"; break;
                case '\xB5': t=t+"mu"; break;
                case ';':
                case ',':
                case '?':
                case '!':
                case ':': t=t+'_'; break;
                case '_':
                case '.':
                case '(':
                case ')':
                case '[':
                case ']':
                    t=t+t1[i].toLatin1(); break;
            }
        }
    }
    if (maxLen>0 && t.size()>maxLen) {
        t=t.left(maxLen);
    }
    //qDebug()<<"cleanStringForFilename("<<text<<") = "<<t;
    return t;
}




QListWidgetItem* addCheckableQListWidgetItem(QListWidget* listWidget, const QString& text, Qt::CheckState checked, const QVariant& userData) {
    QListWidgetItem* item=new QListWidgetItem(text, listWidget);
    item->setCheckState(checked);
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
    item->setData(Qt::UserRole, userData);
    listWidget->addItem(item);
    return item;
}



QString doubleToUnitString(double value, int past_comma, bool remove_trail0, QChar decimalSeparator, bool HTMLout ) {
  if (value==0) return "0";
  QString res=doubleToQString(value, past_comma, 'f', decimalSeparator);
  if (fabs(value)>=1e3) res=doubleToQString(value/1e3, past_comma, 'f', decimalSeparator)+"k";
  if (fabs(value)>=1e6) res=doubleToQString(value/1e6, past_comma, 'f', decimalSeparator)+"M";
  if (fabs(value)>=1e9) res=doubleToQString(value/1e9, past_comma, 'f', decimalSeparator)+"G";
  if (fabs(value)>=1e12) res=doubleToQString(value/1e12, past_comma, 'f', decimalSeparator)+"T";
  if (fabs(value)>=1e15) res=doubleToQString(value/1e15, past_comma, 'f', decimalSeparator)+"P";
  if (fabs(value)<1) res=doubleToQString(value/1e-3, past_comma, 'f', decimalSeparator)+"m";
  if (fabs(value)<1e-3) res=doubleToQString(value/1e-6, past_comma, 'f', decimalSeparator)+QString((HTMLout)?QString("&mu;"):QString("u"));
  if (fabs(value)<1e-6) res=doubleToQString(value/1e-9, past_comma, 'f', decimalSeparator)+"n";
  if (fabs(value)<1e-9) res=doubleToQString(value/1e-12, past_comma, 'f', decimalSeparator)+"p";
  if (fabs(value)<1e-12) res=doubleToQString(value/1e-15, past_comma, 'f', decimalSeparator)+"f";
  if (fabs(value)<1e-15) res=doubleToQString(value/1e-18, past_comma, 'f', decimalSeparator)+"a";
  if (fabs(value)==0) res=doubleToQString(value, past_comma, 'f', decimalSeparator);
  if (remove_trail0) {
      if (value==0) return "0";
      if (res.indexOf(decimalSeparator)<0) return res;
      int i=res.lastIndexOf(QRegExp("\\d"));
      int ilast=i+1;
      while (i>0 && res[i]=='0') {
          i--;
      }
      if (res[i]==decimalSeparator) i--; // remove decimal divider
      return res.left(i+1)+res.right(res.size()-ilast);
  }
  return res;
}

void removeAllActions(QWidget* widget) {
    QList<QAction*> act=widget->actions();
    for (int i=0; i<act.size(); i++) {
        if (act[i]) widget->removeAction(act[i]);
    }
}

QString qchartostr(QChar data) {
    QString ost;
    switch(data.toLatin1()) {
      case 0: ost+="[NUL]"; break;
      case 1: ost+="[SOH]"; break;
      case 2: ost+="[STX]"; break;
      case 3: ost+="[ETX]"; break;
      case 4: ost+="[EOT]"; break;
      case 5: ost+="[ENQ]"; break;
      case 6: ost+="[ACK]"; break;
      case 7: ost+="[BEL]"; break;
      case 8: ost+="[BS]"; break;
      case 9: ost+="[TAB]"; break;
      case 10: ost+="[LF]"; break;
      case 11: ost+="[VT]"; break;
      case 12: ost+="[FF]"; break;
      case 13: ost+="[CR]"; break;
      case 14: ost+="[SO]"; break;
      case 15: ost+="[SI]"; break;
      case 16: ost+="[DLE]"; break;
      case 17: ost+="[DC1]"; break;
      case 18: ost+="[DC2]"; break;
      case 19: ost+="[DC3]"; break;
      case 20: ost+="[DC4]"; break;
      case 21: ost+="[NAK]"; break;
      case 22: ost+="[SYN]"; break;
      case 23: ost+="[ETB]"; break;
      case 24: ost+="[CAN]"; break;
      case 25: ost+="[EM]"; break;
      case 26: ost+="[SUB]"; break;
      case 27: ost+="[ESC]"; break;
      case 28: ost+="[FS]"; break;
      case 29: ost+="[GS]"; break;
      case 30: ost+="[RS]"; break;
      case 31: ost+="[US]"; break;
      case 32: ost+="[SP]"; break;
      case 127: ost+="[DEL]"; break;
  //    case : ost+=""; break;
      default:
        ost+=data;
    }
    return ost;
}


QColor getCycleColor(int value, unsigned int max, double S, double V) {
    while (value<0) value=value+max;
    QColor col;
    col.setHsvF(double(value%max)/double(max-1)*0.8, S, V);
    return col;
}


int wavelengthToColorAdjust(double color, double factor, double gamma=0.8, int IntensityMax=255) {
    if (color==0) return 0;
    else return qBound(0,(int)round(IntensityMax * pow(color * factor, gamma)), IntensityMax);
}

QColor wavelengthToColor(double wavelength) {
    int IntensityMax=255;
    double gamma=0.8;
    double factor=1;

    double r=0, g=0, b=0;

    if (wavelength<380) {
        r=1;
        g=0;
        b=1;
    } else if (wavelength>=380 && wavelength<=439) {
        r=-(wavelength-440.0)/(440.0-380.0);
        g=0;
        b=1;
    } else if (wavelength>439 && wavelength<=489) {
        r=0;
        g=(wavelength-439.0)/(490.0-439.0);
        b=1;
    } else if (wavelength>489 && wavelength<=509) {
        r=0;
        g=1;
        b=-(wavelength-510.0)/(510.0-490.0);
    } else if (wavelength>509 && wavelength<=579) {
        r= (wavelength - 510.0) / (580.0 - 510.0);
        g= 1.0;
        b= 0.0;
    } else if (wavelength>579 && wavelength<=644) {
        r= 1.0;
        g= -(wavelength - 645.0) / (645.0 - 580.0);
        b= 0.0;
    } else if (wavelength>644 ) {
        r= 1.0;
        g= 0.0;
        b= 0.0;
    }

    if (wavelength<380) {
        factor=0.3;
    } else if (wavelength>=380 && wavelength<=419) {
        factor = 0.3 + 0.7*(wavelength - 380.0) / (420.0 - 380.0);
    } else if (wavelength>700 && wavelength<=780.0) {
        factor = 0.3 + 0.7*(780.0 - wavelength) / (780.0 - 700.0);
    } else if (wavelength>780.0) {
        factor=0.3;
    }

    return QColor(wavelengthToColorAdjust(r, factor, gamma, IntensityMax),
                  wavelengthToColorAdjust(g, factor, gamma, IntensityMax),
                  wavelengthToColorAdjust(b, factor, gamma, IntensityMax));
}

QIcon qfGetColorIcon(QColor color, QSize size) {
    QPixmap pm(size);
    pm.fill(color);
    return QIcon(pm);
}

QString getNewFilename(const QString& filename, const QDir& dir) {
    if (!QFile::exists(dir.absoluteFilePath(filename))) return filename;
    QFileInfo info(dir.absoluteFilePath(filename));
    QDir d=info.absoluteDir();
    QString base=info.baseName();
    QString suffix=info.completeSuffix();
    QString newFilename=d.absoluteFilePath(base+"."+suffix);
    int i=1;
    while (QFile::exists(newFilename)) {
        newFilename=d.absoluteFilePath(base+QString("%1.").arg(i, 4,10,QLatin1Char('0'))+suffix);
        i++;
    }
    return newFilename;
}

int qfReadFirstInt(const QString& text, bool* ok) {
    QString num="";
    for (int i=0; i<text.size(); i++) {
        if (text[i].isDigit()) num+=text[i];
        else if (!num.isEmpty()) {
            break;
        }
    }
    return num.toInt(ok);
}


QString intToQString(int64_t value)
{
    return QString::number(value);
}


QString intToBinaryQString(int64_t value)
{
    return QString::number(value, 2);
}


QString intToHexQString(int64_t value)
{
    return QString::number(value, 16);
}

QString pntToHexQString(const void* value)
{
    return QString::number((int64_t)value, 16);
}

QString intToOctQString(int64_t value)
{
    return QString::number(value, 8);
}


QString doubleToLatexQStringAlwaysSign(double data, int precision, bool remove_trail0, double belowIsZero, double minNoExponent, double maxNoExponent, const QString& signSeparator){
    QString sign="+"+signSeparator;
    double d=data;
    if (data<0) {
        d=-data;
        sign="-"+signSeparator;
    }
    return sign+doubleToLatexQString(data, precision, remove_trail0, belowIsZero, minNoExponent, maxNoExponent);
}

QString doubleToLatexQString(double data, int precision, bool remove_trail0, double belowIsZero, double minNoExponent, double maxNoExponent){
  if ((belowIsZero>0) && (fabs(data)<belowIsZero)) return "\\rm{0}";
  if (data==0) return "\\rm{0}";
  double adata=fabs(data);
  QString res;
  if (remove_trail0) res=doubleToQStringNoTrail(data, precision);
  else res=doubleToQString(data, precision, 'f');

  long exp=(long)floor(log(adata)/log(10.0));
  if ((minNoExponent<=fabs(data)) && (fabs(data)<=maxNoExponent)) return res;

  QString v;
  if (remove_trail0) v=doubleToQStringNoTrail(data/pow(10.0, (double)exp), precision);
  else v=doubleToQString(data/pow(10.0, (double)exp), precision, 'f');

  if (v!="1" && v!="10")  return v+QString("{\\times}10^{")+intToQString(exp)+"}";
  if (v=="10") exp=exp+1;
  return QString("10^{")+intToQString(exp)+"}";
}


QString doubleToHTMLQStringAlwaysSign(double data, int precision, bool remove_trail0, double belowIsZero, double minNoExponent, double maxNoExponent, const QString& signSeparator){
    QString sign="+"+signSeparator;
    double d=data;
    if (data<0) {
        d=-data;
        sign="-"+signSeparator;
    }
    return sign+doubleToHTMLQString(data, precision, remove_trail0, belowIsZero, minNoExponent, maxNoExponent);

}

QString doubleToHTMLQString(double data, int precision, bool remove_trail0, double belowIsZero, double minNoExponent, double maxNoExponent){
  if ((belowIsZero>0) && (fabs(data)<belowIsZero)) return "0";
  if (data==0) return "0";
  double adata=fabs(data);
  QString res;
  if (remove_trail0) res=doubleToQStringNoTrail(data, precision);
  else res=doubleToQString(data, precision, 'f');

  long exp=(long)floor(log(adata)/log(10.0));
  if ((minNoExponent<=fabs(data)) && (fabs(data)<=maxNoExponent)) return res;

  QString v;
  if (remove_trail0) v=doubleToQStringNoTrail(data/pow(10.0, (double)exp), precision);
  else v=doubleToQString(data/pow(10.0, (double)exp), precision, 'f');

  if (v!="1" && v!="10")  return v+QString("&times;10<sup>")+intToQString(exp)+"</sup>";
  if (v=="10") exp=exp+1;
  return QString("10<sup>")+intToQString(exp)+"</sup>";
}

QString qfShortenString(const QString& input, int maxLen, int keepend, const QString& ellipsis) {
    if (input.size()<=maxLen) return input;
    QString res;
    int len=qMax(0, maxLen-ellipsis.size()-keepend);
    res=input.left(len)+ellipsis;
    if (keepend>0) res+=input.right(keepend);
    return res;
}

void saveStringToFile(const QString& filename, const QString& text, const QString& codec) {
    QFile data(filename);
    if (data.open(QFile::WriteOnly | QFile::Text | QIODevice::Truncate)) {
        QTextStream out(&data);
        QTextCodec* c=QTextCodec::codecForName(codec.toLatin1());
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
        if (c==NULL) c=QTextCodec::codecForCStrings();
#endif
        if (c==NULL) c=QTextCodec::codecForLocale();
        out.setCodec(c);
        out << text;
    }
}

static QStringList qfDirListFilesRecursive_private(QDir& base, const QDir& dir, const QStringList& filters) {
    QStringList sl;

    QStringList d=dir.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
    for (int i=0; i<d.size(); i++) {
        QDir ddir=dir;
        ddir.cd(d[i]);
        sl<<qfDirListFilesRecursive_private(base, ddir, filters);
    }

    QStringList l;
    if (!filters.isEmpty()) l=dir.entryList(filters, QDir::Files);
    else l=dir.entryList(QDir::Files);
    if (l.size()>0) {
        for (int i=0; i<l.size(); i++) {
            sl<<base.relativeFilePath(dir.absoluteFilePath(l[i]));
        }
    }
    return sl;
}

QStringList qfDirListFilesRecursive(QDir& dir, const QStringList& filters) {
    QStringList sl;
    sl=qfDirListFilesRecursive_private(dir, dir, filters);
    return sl;
}


QList<int> stringToIntList(const QString& data) {
    QList<int> res;
    QStringList sl=data.trimmed().split(',');
    for (int i=0; i<sl.size(); i++) {
        bool ok=false;
        int val=sl[i].toInt(&ok);
        if (ok) res<<val;
    }
    return res;
}


bool qfQStringCompareLengthDecreasing(const QString &s1, const QString &s2) {
    return s1.size() > s2.size();
}

bool qfQStringCompareLengthIncreasing(const QString &s1, const QString &s2) {
    return s1.size() < s2.size();
}


QString qfCEscaped(const QString& data) {
    QString res;
    for (int i=0; i<data.size(); i++) {
        if (data[i]=='"') {
            res+="\\\"";
        } else if (data[i]=='\'') {
            res+="\\\'";
        } else if (data[i]=='\\') {
            res+="\\\\";
        } else if (data[i]=='\n') {
            res+="\\n";
        } else if (data[i]=='\r') {
            res+="\\r";
        } else if (data[i]=='\t') {
            res+="\\t";
        } else {
            res+=data[i];
        }
    }
    return res;
}

QString qfGetTempFilename(const QString& templateName) {
    QTemporaryFile f(templateName);
    f.setAutoRemove(true);
    f.open();
    QString fn=f.fileName();
    f.close();
    return fn;
}

#define QFCOPYDATA_CHUNKSIZEHUNKSIZE 1024*1024*32


bool qfFileEqualsString( const QString& filename, const QByteArray& data) {
    QFile f(filename);
    if (f.open(QFile::ReadOnly|QFile::Text)) {
        bool res= (f.readAll()==data);
        f.close();
        return res;
    }
    return false;
}

bool qfCopyData( QIODevice* in, QIODevice* out) {
    size_t chunksize=QFCOPYDATA_CHUNKSIZEHUNKSIZE;
    char* chunk=qfMallocT<char>(QFCOPYDATA_CHUNKSIZEHUNKSIZE);
    while (!chunk && chunksize>1024) {
        chunksize=chunksize/2;
        chunk=qfMallocT<char>(QFCOPYDATA_CHUNKSIZEHUNKSIZE);
    }
    if (chunk) {
        while (!in->atEnd()) {
            qint64 bytesRead=in->read(chunk, chunksize);
            out->write(chunk, bytesRead);
        }
        qfFree(chunk);
    } else {
        return false;
    }
    return true;
}

bool qfCopyData( QIODevice* in, QIODevice* out, QFProgressMinorProgress* pdlg) {
    size_t chunksize=QFCOPYDATA_CHUNKSIZEHUNKSIZE;
    char* chunk=qfMallocT<char>(QFCOPYDATA_CHUNKSIZEHUNKSIZE);
    while (!chunk && chunksize>1024) {
        chunksize=chunksize/2;
        chunk=qfMallocT<char>(QFCOPYDATA_CHUNKSIZEHUNKSIZE);
    }
    if (chunk) {
        qint64 maxs=in->size();
        int maxx=maxs/chunksize;
        if (pdlg) pdlg->setMinorProgressRange(0, maxx);
        int cnt=0;
        while (!in->atEnd() && (!pdlg || !pdlg->wasMinorProgressCanceled())) {
            qint64 bytesRead=in->read(chunk, chunksize);
            out->write(chunk, bytesRead);
            cnt++;
            if (pdlg) pdlg->setMinorProgress(cnt);
            QApplication::processEvents();
        }
        qfFree(chunk);
        if (pdlg) pdlg->setMinorProgress(0);
    } else {
        if (pdlg) pdlg->setMinorProgress(0);
        return false;
    }
    if (pdlg) return !pdlg->wasMinorProgressCanceled();
    return true;
}


QString qfGetLargestCommonStart(const QStringList& data, const QString& defaultResult) {
    QString res;
    int maxSize=data.value(0, "").size();
    for (int i=0; i<data.size(); i++) {
        if (data[i].size()<maxSize) maxSize=data[i].size();
    }

    int len=0;
    for (int l=1; l<maxSize; l++) {
        bool ok=true;
        QString start=data.value(0, "").left(l);
        for (int i=0; i<data.size(); i++) {
            if (!data[i].startsWith(start)) {
                ok=false;
                break;
            }
        }
        if (!ok) {
            len=l-1;
            break;
        }
    }

    if (len>0) {
        res=data.value(0, "").left(len);
    }

    if (res.size()<=0) res=defaultResult;
    return res;
}


QString qfGetLargestCommonEnd(const QStringList& data, const QString& defaultResult) {
    QString res;
    int maxSize=data.value(0, "").size();
    for (int i=0; i<data.size(); i++) {
        if (data[i].size()<maxSize) maxSize=data[i].size();
    }

    int len=0;
    for (int l=1; l<maxSize; l++) {
        bool ok=true;
        QString start=data.value(0, "").right(l);
        for (int i=0; i<data.size(); i++) {
            if (!data[i].endsWith(start)) {
                ok=false;
                break;
            }
        }
        if (!ok) {
            len=l-1;
            break;
        }
    }

    if (len>0) {
        res=data.value(0, "").right(len);
    }

    if (res.size()<=0) res=defaultResult;
    return res;
}


QPixmap cropLeftRight(const QPixmap& pix, QRgb cropcolor) {
    return QPixmap::fromImage(cropLeftRight(pix.toImage(), cropcolor));
}

QImage cropLeftRight(const QImage& img, QRgb col) {

    int left=0;
    int right=0;
    for (int x=0; x<img.width(); x++) {
        bool allc=true;
        for (int y=0; y<img.height(); y++) {
            allc=allc&&(img.pixel(x,y)==col);
            if (!allc) break;
        }
        if (allc) left++;
        else break;
    }
    for (int x=img.width()-1; x>left; x--) {
        bool allc=true;
        for (int y=0; y<img.height(); y++) {
            allc=allc&&(img.pixel(x,y)==col);
            if (!allc) break;
        }
        if (allc) right++;
        else break;
    }
    //qDebug()<<"cropLeftRight("<<img.width()<<img.height()<<col<<img.pixel(0,0)<<"): "<<left<<right;
    if (left+right<img.width()) {
        return img.copy(left,0,img.width()-left-right,img.height());
    } else {
        return QImage();
    }
}

QPixmap cropLeftRight(const QPixmap& pix) {
    return QPixmap::fromImage(cropLeftRight(pix.toImage()));
}

QImage cropLeftRight(const QImage& pix) {
    return cropLeftRight(pix, pix.pixel(0,0));
}






QPixmap cropBottom(const QPixmap& pix, QRgb cropcolor) {
    return QPixmap::fromImage(cropBottom(pix.toImage(), cropcolor));
}

QImage cropBottom(const QImage& img, QRgb col) {

    int bot=0;
    for (int y=img.height()-1; y>=0; y--) {
        bool allc=true;
        for (int x=0; x<img.width(); x++) {
            allc=allc&&(img.pixel(x,y)==col);
            if (!allc) break;
        }
        if (allc) bot++;
        else break;
    }

    //qDebug()<<"cropLeftRight("<<img.width()<<img.height()<<col<<img.pixel(0,0)<<"): "<<left<<right;
    if (bot<img.height()) {
        return img.copy(0,0,img.width(),img.height()-bot);
    } else {
        return QImage();
    }
}

QPixmap cropBottom(const QPixmap& pix) {
    return QPixmap::fromImage(cropBottom(pix.toImage()));
}

QImage cropBottom(const QImage& pix) {
    return cropBottom(pix, pix.pixel(0,pix.height()-1));
}




QPixmap cropTopBottom(const QPixmap& pix, QRgb cropcolor) {
    return QPixmap::fromImage(cropTopBottom(pix.toImage(), cropcolor));
}

QImage cropTopBottom(const QImage& img, QRgb col) {

    int bot=0;
    int top=0;
    for (int y=0; y<img.height(); y++) {
        bool allc=true;
        for (int x=0; x<img.width(); x++) {
            allc=allc&&(img.pixel(x,y)==col);
            if (!allc) break;
        }
        if (allc) top++;
        else break;
    }
    for (int y=img.height()-1; y>=0; y--) {
        bool allc=true;
        for (int x=0; x<img.width(); x++) {
            allc=allc&&(img.pixel(x,y)==col);
            if (!allc) break;
        }
        if (allc) bot++;
        else break;
    }

    //qDebug()<<"cropLeftRight("<<img.width()<<img.height()<<col<<img.pixel(0,0)<<"): "<<left<<right;
    if (bot+top<img.height()) {
        return img.copy(0,top,img.width(),img.height()-bot-top);
    } else {
        return QImage();
    }
}

QPixmap cropTopBottom(const QPixmap& pix) {
    return QPixmap::fromImage(cropTopBottom(pix.toImage()));
}

QImage cropTopBottom(const QImage& pix) {
    return cropTopBottom(pix, pix.pixel(0,0));
}



QString qfCanonicalOrAbsoluteFilePath(const QString& file) {
    QString fn=QFileInfo(file).canonicalFilePath();
    if (fn.isEmpty()) return QFileInfo(file).absoluteFilePath();
    return fn;
}

QString qfHTMLEscape(const QString& input) {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    return input.toHtmlEscaped();
#else
    return Qt::escape(input);
#endif
}


void qfSaveReport(QTextDocument* doc, const QString& title, const QString& prefix, QWidget* parent){
    QString currentSaveDirectory="";

    qreal oldMargin=doc->documentMargin();
    doc->setDocumentMargin(0);

    QStringList filters;
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        filters<<QObject::tr("A4 PDF File (*.pdf)");
        filters<<QObject::tr("A3 PDF File (*.pdf)");
    #else
        filters<<QObject::tr("A4 PDF File (*.pdf)");
        filters<<QObject::tr("A3 PDF File (*.pdf)");
        filters<<QObject::tr("PostScript File (*.ps)");
    #endif
    int firstDForm=filters.size();
    QList<QByteArray> dforms=QTextDocumentWriter::supportedDocumentFormats();
    for (int i=0; i<dforms.size(); i++) {
        filters<<QString("%1 File (*.%2)").arg(QString::fromLatin1(dforms[i])).arg(QString::fromLatin1(dforms[i].toLower()));
    }
    QString selFilter="";
    QString fn = qfGetSaveFileNameSet(prefix, parent, QObject::tr("Save Report"),
                                currentSaveDirectory,
                                filters.join(";;"), &selFilter);
    int filterID=filters.indexOf(selFilter);

    if (!fn.isEmpty()) {
        currentSaveDirectory=QFileInfo(fn).absolutePath();
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        QProgressDialog progress(QObject::tr("Saving Report ..."), "", 0, 100, NULL);
        progress.setWindowModality(Qt::WindowModal);
        //progress.setHasCancel(false);
        progress.setLabelText(QObject::tr("saving report <br> to '%1' ...").arg(fn));
        progress.setValue(50);
        progress.show();

        QFileInfo fi(fn);

        QPrinter* printer=new QPrinter();//QPrinter::HighResolution);
        printer->setOutputFormat(QPrinter::PdfFormat);
        printer->setPaperSize(QPrinter::A4);
        printer->setPageMargins(10,10,10,10,QPrinter::Millimeter);
        printer->setOrientation(QPrinter::Portrait);
        printer->setColorMode(QPrinter::Color);


        if (filterID==0||filterID==1
        #if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
            ||filterID==2
        #endif
            ) {
    #if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
            if (filterID==1) printer->setPaperSize(QPrinter::A3);
            if (filterID==2) printer->setOutputFormat(QPrinter::PostScriptFormat);
    #endif
    #if (QT_VERSION > QT_VERSION_CHECK(5, 2, 0))
            QPdfWriter pdf(fn);
            pdf.setPageMargins(QMarginsF(10,10,10,10),QPageLayout::Millimeter);
            pdf.setPageOrientation(QPageLayout::Portrait);
#ifndef QFMATHPARSER_MATHPARSERTEST
            pdf.setCreator(QString("QuickFit %1").arg(qfInfoVersionFull()));
#endif
            pdf.setPageSize(QPageSize(QPageSize::A4));
            if (filterID==1) pdf.setPageSize(QPageSize(QPageSize::A3));
            pdf.setTitle(title);
            pdf.setResolution(300);
            doc->setTextWidth(pdf.pageLayout().paintRect().width());
            doc->print(&pdf);
    #else
            printer->setColorMode(QPrinter::Color);
            printer->setOutputFileName(fn);
            doc->setTextWidth(printer->pageRect().width());
            doc->print(printer);
    #endif
        } else if (filterID>firstDForm) {
            QTextDocumentWriter writer(fn, dforms.value(filterID-firstDForm, "HTML"));
            writer.write(doc);
        }
        //qDebug()<<doc->toHtml();
        delete printer;
        progress.accept();
        QApplication::restoreOverrideCursor();
    }
    doc->setDocumentMargin(oldMargin);
}

QString qfSecondsDurationToHMSString(double seconds) {
    int64_t runHours=floor(seconds/3600.0);
    int64_t runMins=floor((seconds-(double)runHours*3600.0)/60.0);
    int64_t runSecs=ceil((seconds-(double)runHours*3600.0-(double)runMins*60.0));

    return QString("%1:%2:%3").arg(runHours,2,10,QLatin1Char('0')).arg(runMins,2,10,QLatin1Char('0')).arg(runSecs,2,10,QLatin1Char('0'));
}

QString qfSecondsDurationToMSString(double seconds) {
    int64_t runMins=floor((seconds)/60.0);
    int64_t runSecs=ceil((seconds-(double)runMins*60.0));

    return QString("%1:%2").arg(runMins,2,10,QLatin1Char('0')).arg(runSecs,2,10,QLatin1Char('0'));
}
