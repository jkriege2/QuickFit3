#include "qftools.h"
#include <cfloat>
#include <cmath>
#include <QList>
#include <QApplication>
#include <QDesktopWidget>
#include <QLocale>
#include <QDebug>

void saveWidgetGeometry(QSettings& settings, QWidget* widget, QString prefix) {
    settings.setValue(prefix+"pos", widget->pos());
    settings.setValue(prefix+"size", widget->size());
}

void loadWidgetGeometry(QSettings& settings, QWidget* widget, QPoint defaultPosition, QSize defaultSize, QString prefix) {
    QPoint pos = settings.value(prefix+"pos", defaultPosition).toPoint();
    QSize size = settings.value(prefix+"size", defaultSize).toSize();

    widget->resize(size.boundedTo(QApplication::desktop()->screenGeometry(widget).size()));
    if (pos.x()<0 || pos.x()>QApplication::desktop()->screenGeometry(widget).width()) pos.setX(0);
    if (pos.y()<0 || pos.y()>QApplication::desktop()->screenGeometry(widget).height()) pos.setY(0);
    widget->move(pos);
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
    splitter->restoreState(settings.value(prefix+"splitter_sizes").toByteArray());
}






QString getQVariantType(const QVariant& variant) {
    QString t="invalid";
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
    return t;
}

QString getQVariantData(const QVariant& variant) {
    QString t="";
    QLocale loc=QLocale::c();
    loc.setNumberOptions(QLocale::OmitGroupSeparator);
    switch(variant.type()) {
        case QVariant::Double: t=loc.toString(variant.toDouble(), 'g', 10); break;
        default: t=variant.toString(); break;
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
    else if (type=="char") { c=d.convert(QVariant::Char); }
    else if (type=="date") { c=d.convert(QVariant::Date); }
    else if (type=="datetime") { c=d.convert(QVariant::DateTime); }
    else if (type=="double") { d=loc.toDouble(data, &c); }
    else if (type=="int") { c=d.convert(QVariant::Int); }
    else if (type=="longlong") { c=d.convert(QVariant::LongLong); }
    else if (type=="string") { c=d.convert(QVariant::String); }
    else if (type=="stringlist") { c=d.convert(QVariant::StringList); }
    else if (type=="uint") { c=d.convert(QVariant::UInt); }
    else if (type=="ulonglong") { c=d.convert(QVariant::ULongLong); }
    else if (type=="time") { c=d.convert(QVariant::Time); }
    else if (type=="bytearray") { c=d.convert(QVariant::ByteArray); }
    else if (type=="color") { c=d.convert(QVariant::Color); }
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

QString CDoubleToQString(double value) {
    QLocale loc=QLocale::c();
    loc.setNumberOptions(QLocale::OmitGroupSeparator);
    return loc.toString(value, 'g', 10);
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

bool touchFile(const QString& filename) {
    if (QFile::exists(filename)) return true;
    QFile f(filename);
    if (f.open(QIODevice::WriteOnly)) {
        f.close();
        return true;
    }
    return false;
}





QString escapify(const QString& text){
  QString res="";
  if (text.size()>0) {
    for (int i=0; i<text.size(); i++)
      switch((char)text[i].toLatin1()) {
        case '\0': res+="\\0"; break;
        case '\n': res+="\\n"; break;
        case '\r': res+="\\r"; break;
        case '\t': res+="\\t"; break;
        case '\\': res+="\\\\"; break;
        case '"': res+="\\\""; break;
        case '\'': res+="\\'"; break;
        case '\a': res+="\\a"; break;
        case '\b': res+="\\b"; break;
        case '\v': res+="\\v"; break;
        case '\f': res+="\\f"; break;
        case '\e': res+="\\e"; break;
        case '\?': res+="\\?"; break;
        default:
          if ((unsigned char)text[i].toLatin1()<32) {
              QString n=QString::number((unsigned char)text[i].toLatin1(), 16);
              if (n.size()<3) n.prepend(QString(3-n.size(), QChar('0')));
              res+="\\x"+n;
          } else res+=text[i];
          break;
      };
  }
  return res;
}


QString deescapify(const QString& text){
  QString res="";
  if (text.size()>0) {
    int i=0;
    while (i<text.size()) {
      if (text[i]!='\\') {
        res+=text[i];
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
    for (int si=0; si<text.size(); si++) {
        if (si>0) s=s+";";
        s=s+QString("\'%1\'").arg(escapify(text[si]));
    }
    return s;
}

QStringList deescapifyList(const QString& text, char stringDelim, char separator) {
    QStringList result;
    QString res="";
    bool inString=false;
    if (text.size()>0) {
      int i=0;
      while (i<text.size()) {
        if (text[i]==stringDelim) {
            inString=!inString;
        } else if ((!inString) && (text[i]==separator)) {
            if (!res.isEmpty()) {
                result.append(res);
                res="";
            }
        } else if (text[i]!='\\') {
            res+=text[i];
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


QString	qfGetOpenFileName ( QWidget * parent, const QString & caption, const QString & dir, const QString & filter, QString * selectedFilter, QFileDialog::Options options ) {
#ifdef Q_OS_UNIX
    return QFileDialog::getOpenFileName(parent, caption, dir, filter, selectedFilter, QFileDialog::DontUseNativeDialog|options);
#else
    return QFileDialog::getOpenFileName(parent, caption, dir, filter, selectedFilter, options);
#endif
}

QStringList	qfGetOpenFileNames ( QWidget * parent, const QString & caption, const QString & dir, const QString & filter, QString * selectedFilter, QFileDialog::Options options ) {
#ifdef Q_OS_UNIX
    return QFileDialog::getOpenFileNames(parent, caption, dir, filter, selectedFilter, QFileDialog::DontUseNativeDialog|options);
#else
    return QFileDialog::getOpenFileNames(parent, caption, dir, filter, selectedFilter, options);
#endif
}

QString	qfGetSaveFileName ( QWidget * parent, const QString & caption, const QString & dir, const QString & filter, QString * selectedFilter, QFileDialog::Options options )  {
    QString s="";
    QString selF="";
    if (selectedFilter) selF=*selectedFilter;
#ifdef Q_OS_UNIX
    s= QFileDialog::getSaveFileName(parent, caption, dir, filter, &selF, QFileDialog::DontUseNativeDialog|options);
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
#else
    s= QFileDialog::getSaveFileName(parent, caption, dir, filter, &selF, options);
#endif
    if (selectedFilter) *selectedFilter=selF;
    return s;
}

int getApplicationBitDepth() {
    return 8*sizeof(void*);
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
        if (t1[i]>'0' && t1[i]<'9') t=t+t1[i];
        else if (t1[i]>'A' && t1[i]<'z') t=t+t1[i];
        else {
            switch(t1[i].toAscii()) {
                case 'ä': t=t+"ae"; break;
                case 'Ä': t=t+"Ae"; break;
                case 'ö': t=t+"oe"; break;
                case 'Ö': t=t+"Oe"; break;
                case 'ü': t=t+"ue"; break;
                case 'Ü': t=t+"Ue"; break;
                case 'ß': t=t+"ss"; break;
                case '@': t=t+"_at_"; break;
                case 'µ': t=t+"mu"; break;
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
                    t=t+t1[i].toAscii(); break;
            }
        }
    }
    if (maxLen>0 && t.size()>maxLen) {
        t=t.left(maxLen);
    }
    //qDebug()<<"cleanStringForFilename("<<text<<") = "<<t;
    return t;
}
