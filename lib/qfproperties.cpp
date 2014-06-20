/*
    Copyright (c) 2008-2014 Jan W. Krieger, German Cancer Research Center

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "qfproperties.h"
#include <iostream>
#include <QtXml>
#include "qftools.h"
#include <QDebug>


QFProperties::QFProperties()
{
    propertyLocker=new QReadWriteLock();
}

QFProperties::~QFProperties() {
    props.clear();
    delete propertyLocker;
}

void QFProperties::clearProperties()
{
    {
        QWriteLocker lock();
        props.clear();
    }
    emitPropertiesChanged();

}

QVariant QFProperties::getProperty(const QString &p) const
{
    QReadLocker lock(propertyLocker);
    if (props.contains(p)) return props[p].data; else return QVariant();
}

QVariant QFProperties::getProperty(const QString &p, const QVariant &defaultValue) const
{
    QReadLocker lock(propertyLocker);
    if (props.contains(p)) return props[p].data;
    return defaultValue;

}

unsigned int QFProperties::getPropertyCount() const
{
    QReadLocker lock(propertyLocker); return props.size();
}



unsigned int QFProperties::getVisiblePropertyCount() const {
    QReadLocker lock(propertyLocker);
    unsigned int c=0;
    //for (int i=0; i<props.keys().size(); i++) {
        //QString p=props.keys().at(i);
    QHashIterator<QString, propertyItem> i(props);
    while (i.hasNext()) {
        i.next();
        if (i.value().visible) c++;
    }
    return c;
}

QStringList QFProperties::getVisibleProperties() const
{
    QReadLocker lock(propertyLocker);
    QStringList c;
    //for (int i=0; i<props.keys().size(); i++) {
        //QString p=props.keys().at(i);
    QHashIterator<QString, propertyItem> i(props);
    while (i.hasNext()) {
        i.next();
        if (i.value().visible) c.append(i.key());
    }
    return c;
}

QString QFProperties::getVisibleProperty(unsigned int j) const {
    QReadLocker lock(propertyLocker);
    unsigned int c=0;
    //for (int i=0; i<props.keys().size(); i++) {
        //QString p=props.keys().at(i);
    QHashIterator<QString, propertyItem> i(props);
    while (i.hasNext()) {
        i.next();
        if (i.value().visible) {
            if (c==j) return i.key();
            c++;
        }
    }
    return QString("");
}

QStringList QFProperties::getPropertyNames() const
{
    QReadLocker lock(propertyLocker);
    return props.keys();
}

QString QFProperties::getPropertyName(int i) const
{
    QReadLocker lock(propertyLocker);
    return props.keys().at(i);
}

bool QFProperties::isPropertyVisible(QString property) const {
    QReadLocker lock(propertyLocker);
    if (!props.contains(property)) return false;
    return props[property].visible;
}

bool QFProperties::isPropertyUserEditable(QString property) const{
    QReadLocker lock(propertyLocker);
    if (!props.contains(property)) return false;
    return props[property].usereditable;
}

void QFProperties::deleteProperty(const QString &n) {
    {
        QWriteLocker lock(propertyLocker);
        props.remove(n);
    }
    emitPropertiesChanged();
}

bool QFProperties::propertyExists(const QString &p) const {
    QReadLocker lock(propertyLocker);
    return props.contains(p);
}

void QFProperties::setQFProperty(const QString &p, QVariant value, bool usereditable, bool visible) {
    {
        QWriteLocker lock(propertyLocker);
        propertyItem i;
        i.data=value;
        i.usereditable=usereditable;
        i.visible=visible;
        props[p]=i;
    }
    emitPropertiesChanged(p,visible);
}

void QFProperties::setQFPropertyIfNotUserEditable(const QString &p, QVariant value, bool usereditable, bool visible)
{
    bool ok=true;
    {
        QWriteLocker lock(propertyLocker);
        if (props.contains(p)) {
            ok = !props[p].usereditable;
        }
        if (ok) {
            propertyItem i;
            i.data=value;
            i.usereditable=usereditable;
            i.visible=visible;
            props[p]=i;
        }
    }
    if (ok) emitPropertiesChanged(p,visible);
}

void QFProperties::addProperties(const QFProperties &other)
{
    QReadLocker lockOther(other.propertyLocker);
    QWriteLocker lock(propertyLocker);
    QHashIterator<QString, propertyItem> i(other.props);
    while (i.hasNext()) {
        i.next();
        QFProperties::propertyItem it=i.value();
        props[i.key()]=it;
    }
}

void QFProperties::storeProperties(QXmlStreamWriter& w) const {
    QReadLocker lock(propertyLocker);
    QHashIterator<QString, propertyItem> i(props);
    while (i.hasNext()) {
        i.next();
        w.writeStartElement("property");
        QString n=i.key();
        propertyItem it=i.value();
        w.writeAttribute("name", n);

        w.writeAttribute("type", getQVariantType(it.data));
        w.writeAttribute("data", getQVariantData(it.data));
        w.writeAttribute("usereditable", (it.usereditable)?QString("true"):QString("false"));
        w.writeAttribute("visible", (it.visible)?QString("true"):QString("false"));
        w.writeEndElement();
    }
}

void QFProperties::readProperties(QDomElement& e) {
    {
        QWriteLocker lock(propertyLocker);
        QDomElement te=e.firstChildElement("property");
        props.clear();
        while (!te.isNull()) {
            QString n=te.attribute("name", "");

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
    }
    emitPropertiesChanged();

}
