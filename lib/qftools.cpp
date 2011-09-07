#include "qftools.h"
#include <cfloat>
#include <cmath>
#include <QList>
#include <QApplication>
#include <QDesktopWidget>
#include <QLocale>

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


double roundError(double error, int addSignifcant) {
    if (fabs(error)<DBL_MIN*10.0) return error;
    int sbits_error=ceil(log(fabs(error))/log(10.0));
    double f=pow(10.0, sbits_error-1-addSignifcant);
    return round(error/f)*f;
}

double roundWithError(double value, double error, int addSignifcant)  {
    if ((fabs(error)<DBL_MIN*10)||(!std::isfinite(error))) return value;

    int sbits_error=ceil(log(fabs(error))/log(10.0));
    if (sbits_error>=0) sbits_error=-1;
    double f=pow(10.0, sbits_error-1-addSignifcant);

    return round(value/f)*f;
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
        case QVariant::Double: t=loc.toString(variant.toDouble()); break;
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

bool QStringToBool(QString& data){
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
