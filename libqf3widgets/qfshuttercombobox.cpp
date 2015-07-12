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


#include "qfshuttercombobox.h"

QFShutterComboBox::QFShutterComboBox(QFExtensionManager* extManager, QWidget* parent):
    QEnhancedComboBox(parent)
{
    m_extManager=extManager;
    setSizeAdjustPolicy(QComboBox::AdjustToContents);

    findExtensions();
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(comboIndexChanged(int)));
}

QFShutterComboBox::QFShutterComboBox(QWidget* parent):
    QEnhancedComboBox(parent)
{
    m_extManager=NULL;
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(comboIndexChanged(int)));
}
QFShutterComboBox::~QFShutterComboBox()
{
    //dtor
}

void QFShutterComboBox::init(QFExtensionManager* extManager) {
    m_extManager=extManager;
    findExtensions();
}

void QFShutterComboBox::findExtensions() {
    shutters.clear();
    clear();
    if (m_extManager==NULL) return;
    // load available acquisition devices from extManager
    QStringList ids=m_extManager->getIDList();
    for (int i=0; i<ids.size(); i++) {
        QObject* extobj=m_extManager->getQObjectInstance(ids[i]);
        //QFExtension* extension=m_extManager->getInstance(ids[i]);
        QFExtensionShutter* shutter = qobject_cast<QFExtensionShutter*>(extobj);
        if (shutter) shutters.append(extobj);
    }
    QPoint sl(-1,-1);
    addItem(QIcon(":/libqf3widgets/shutter_none.png"), tr("--- none ---"), sl);

    for (int i=0; i<shutters.size(); i++) {
        QFExtension* extension=qobject_cast<QFExtension*>(shutters[i]);
        QFExtensionShutter* shutter = qobject_cast<QFExtensionShutter*>(shutters[i]);
        for (unsigned int j=0; j<shutter->getShutterCount(); j++) {
            QPoint sl(i, j);
            QString name=extension->getName();
            QString cname=shutter->getShutterShortName(j);
            if (shutter->getShutterCount()>0) name=name+QString(" #%1").arg(j).arg((cname!=name && !cname.isEmpty())?QString(" (%1)").arg(cname):QString(""));
            addItem(QIcon(extension->getIconFilename()), name, sl);
        }
    }

}

QFExtension* QFShutterComboBox::currentExtension() const {
    if (currentIndex()<0) {
        return NULL;
    }

    QPoint p = itemData(currentIndex()).toPoint();
    QFExtension* extension=NULL;
    QFExtensionShutter* shutter=NULL;
    //int shutterIdx=p.y();
    if ((p.x()>=0)&&(p.x()<shutters.size())) {
        extension=qobject_cast<QFExtension*>(shutters[p.x()]);
        shutter=qobject_cast<QFExtensionShutter*>(shutters[p.x()]);
    }
    return extension;
}

QObject *QFShutterComboBox::currentExtensionObject() const
{
    if (currentIndex()<0) {
        return NULL;
    }

    QPoint p = itemData(currentIndex()).toPoint();
    if ((p.x()>=0)&&(p.x()<shutters.size())) {
        return shutters[p.x()];
    }
    return NULL;
}

QFExtensionShutter* QFShutterComboBox::currentExtensionShutter() const {
    if (currentIndex()<0) {
        return NULL;
    }

    QPoint p = itemData(currentIndex()).toPoint();
    QFExtension* extension=NULL;
    QFExtensionShutter* shutter=NULL;
    //int shutterIdx=p.y();
    if ((p.x()>=0)&&(p.x()<shutters.size())) {
        extension=qobject_cast<QFExtension*>(shutters[p.x()]);
        shutter=qobject_cast<QFExtensionShutter*>(shutters[p.x()]);
    }
    return shutter;
}

int QFShutterComboBox::currentShutterID() const {
    if (currentIndex()<0) {
        return -1;
    }

    QPoint p = itemData(currentIndex()).toPoint();
    QFExtension* extension=NULL;
    QFExtensionShutter* shutter=NULL;
    int shutterIdx=p.y();
    if ((p.x()>=0)&&(p.x()<shutters.size())) {
        extension=qobject_cast<QFExtension*>(shutters[p.x()]);
        shutter=qobject_cast<QFExtensionShutter*>(shutters[p.x()]);
    } else {
        shutterIdx=-1;
    }
    return shutterIdx;
}

int QFShutterComboBox::currentExtensionID() const {
    if (currentIndex()<0) {
        return -1;
    }

    QPoint p = itemData(currentIndex()).toPoint();
    return p.x();
}

void QFShutterComboBox::comboIndexChanged(int index) {

    QPoint p = itemData(index).toPoint();
    QFExtension* extension=NULL;
    QFExtensionShutter* shutter=NULL;
    int shutterIdx=p.y();
    if ((p.x()>=0)&&(p.x()<shutters.size())) {
        extension=qobject_cast<QFExtension*>(shutters[p.x()]);
        shutter=qobject_cast<QFExtensionShutter*>(shutters[p.x()]);
    } else {
        shutterIdx=-1;
    }
}

void QFShutterComboBox::storeSettings(QSettings& settings, QString prefix) const {
    if (currentExtension()) {
        settings.setValue(prefix+"shutter_plugin_id", currentExtension()->getID());
        settings.setValue(prefix+"shutter_id", currentShutterID());
    } else {
        settings.setValue(prefix+"shutter_plugin_id", "");
        settings.setValue(prefix+"shutter_id", -1);
    }
}

void QFShutterComboBox::storeSettings(QFManyFilesSettings &settings, QString prefix) const
{
    if (currentExtension()) {
        settings.setValue(prefix+"shutter_plugin_id", currentExtension()->getID());
        settings.setValue(prefix+"shutter_id", currentShutterID());
    } else {
        settings.setValue(prefix+"shutter_plugin_id", "");
        settings.setValue(prefix+"shutter_id", -1);
    }
}

void QFShutterComboBox::loadSettings(QSettings& settings, QString prefix) {
   QString id=settings.value(prefix+"shutter_plugin_id", "").toString();
   int shutterIdx=settings.value(prefix+"shutter_id", -1).toInt();
   setCurrentIndex(0);
   bool ok=false;
   QFExtension* extension=NULL;
   QFExtensionShutter* shutter=NULL;
   for (int i=0; i<count(); i++) {
       QPoint p = itemData(i).toPoint();
       int shutterIdxC=p.y();
       if ((p.x()>=0)&&(p.x()<shutters.size())) {
           extension=qobject_cast<QFExtension*>(shutters[p.x()]);
           shutter=qobject_cast<QFExtensionShutter*>(shutters[p.x()]);
           if (extension && extension->getID()==id && shutterIdx==shutterIdxC) {
               ok=true;
               setCurrentIndex(i);
               break;
           }
       }
   }
   if (!ok) setCurrentIndex(0);

}

void QFShutterComboBox::loadSettings(QFManyFilesSettings &settings, QString prefix)
{
    QString id=settings.value(prefix+"shutter_plugin_id", "").toString();
    int shutterIdx=settings.value(prefix+"shutter_id", -1).toInt();
    setCurrentIndex(0);
    bool ok=false;
    QFExtension* extension=NULL;
    QFExtensionShutter* shutter=NULL;
    for (int i=0; i<count(); i++) {
        QPoint p = itemData(i).toPoint();
        int shutterIdxC=p.y();
        if ((p.x()>=0)&&(p.x()<shutters.size())) {
            extension=qobject_cast<QFExtension*>(shutters[p.x()]);
            shutter=qobject_cast<QFExtensionShutter*>(shutters[p.x()]);
            if (extension && extension->getID()==id && shutterIdx==shutterIdxC) {
                ok=true;
                setCurrentIndex(i);
                break;
            }
        }
    }
    if (!ok) setCurrentIndex(0);
}
