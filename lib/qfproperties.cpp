#include "qfproperties.h"
#include <iostream>
#include <QtXml>
#include "qftools.h"


QFProperties::QFProperties() {
    //ctor
}

QFProperties::~QFProperties() {
    props.clear();
}



unsigned int QFProperties::getVisiblePropertyCount() {
    unsigned int c=0;
    //for (int i=0; i<props.keys().size(); i++) {
        //QString p=props.keys().at(i);
    QMapIterator<QString, propertyItem> i(props);
    while (i.hasNext()) {
        i.next();
        if (i.value().visible) c++;
    }
    return c;
}

QString QFProperties::getVisibleProperty(unsigned int j) {
    unsigned int c=0;
    //for (int i=0; i<props.keys().size(); i++) {
        //QString p=props.keys().at(i);
    QMapIterator<QString, propertyItem> i(props);
    while (i.hasNext()) {
        i.next();
        if (i.value().visible) {
            if (c==j) return i.key();
            c++;
        }
    }
    return QString("");
}

void QFProperties::storeProperties(QXmlStreamWriter& w) {
    QMapIterator<QString, propertyItem> i(props);
    while (i.hasNext()) {
        i.next();
        w.writeStartElement("property");
        QString n=i.key();
        propertyItem it=i.value();
        w.writeAttribute("name", n);
        /*QString t="invalid";
        switch(it.data.type()) {
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
        w.writeAttribute("type", t);
        w.writeAttribute("data", it.data.toString());*/
        w.writeAttribute("type", getQVariantType(it.data));
        w.writeAttribute("data", getQVariantData(it.data));
        w.writeAttribute("usereditable", (it.usereditable)?QString("true"):QString("false"));
        w.writeAttribute("visible", (it.visible)?QString("true"):QString("false"));
        w.writeEndElement();
    }
}

void QFProperties::readProperties(QDomElement& e) {
    QDomElement te=e.firstChildElement("property");
    props.clear();
    while (!te.isNull()) {
        QString n=te.attribute("name", "");
        /*QString t=te.attribute("type", "string").toLower();
        QVariant d=te.attribute("data", "");
        //std::cout<<"  prop "<<n.toStdString()<<" ["+t.toStdString()+"] = "<<d.toString().toStdString()<<"\n";
        bool c=false;
        if (t=="bool") { c=d.convert(QVariant::Bool); }
        else if (t=="char") { c=d.convert(QVariant::Char); }
        else if (t=="date") { c=d.convert(QVariant::Date); }
        else if (t=="datetime") { c=d.convert(QVariant::DateTime); }
        else if (t=="double") { c=d.convert(QVariant::Double); }
        else if (t=="int") { c=d.convert(QVariant::Int); }
        else if (t=="longlong") { c=d.convert(QVariant::LongLong); }
        else if (t=="string") { c=d.convert(QVariant::String); }
        else if (t=="stringlist") { c=d.convert(QVariant::StringList); }
        else if (t=="uint") { c=d.convert(QVariant::UInt); }
        else if (t=="ulonglong") { c=d.convert(QVariant::ULongLong); }
        else if (t=="time") { c=d.convert(QVariant::Time); }
        else if (t=="bytearray") { c=d.convert(QVariant::ByteArray); }
        else if (t=="color") { c=d.convert(QVariant::Color); }
        else {
            setPropertiesError(QString("Property '%1' has an unsupported type (%2)!\n Value is \"%3\".").arg(n).arg(t).arg(te.attribute("data", "")));
            //return;
        }
        if (!c) {
            setPropertiesError(QString("The value of property '%1' (%2) could not be converted to type %3!").arg(n).arg(te.attribute("data", "")).arg(t));
            //return;
        }*/
        QString t=te.attribute("type", "string").toLower();
        QString pd=te.attribute("data", "");
        QVariant d=getQVariantFromString(t, pd);
        if (!d.isValid()) {
            setPropertiesError(QString("Property '%1' has an unsupported type (%2) or a conversion error occured (data invalid)!\n Value is \"%3\".").arg(n).arg(t).arg(pd));
        }

        propertyItem pi;
        pi.data=d;
        pi.usereditable=QVariant(te.attribute("usereditable", "true")).toBool();
        pi.visible=QVariant(te.attribute("visible", "true")).toBool();
        props[n]=pi;
        te = te.nextSiblingElement("property");
    }
    emitPropertiesChanged();

}
