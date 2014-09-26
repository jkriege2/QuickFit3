/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

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


#include "qffilterchangercombobox.h"

QFFilterChangerComboBox::QFFilterChangerComboBox(QFExtensionManager* extManager, QWidget* parent):
    QEnhancedComboBox(parent)
{
    m_extManager=extManager;
    setSizeAdjustPolicy(QComboBox::AdjustToContents);

    findExtensions();
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(comboIndexChanged(int)));
}

QFFilterChangerComboBox::QFFilterChangerComboBox(QWidget* parent):
    QEnhancedComboBox(parent)
{
    m_extManager=NULL;
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(comboIndexChanged(int)));
}
QFFilterChangerComboBox::~QFFilterChangerComboBox()
{
    //dtor
}

void QFFilterChangerComboBox::init(QFExtensionManager* extManager) {
    m_extManager=extManager;
    findExtensions();
}

void QFFilterChangerComboBox::findExtensions() {
    FilterChangers.clear();
    clear();
    if (m_extManager==NULL) return;
    // load available acquisition devices from extManager
    QStringList ids=m_extManager->getIDList();
    for (int i=0; i<ids.size(); i++) {
        QObject* extobj=m_extManager->getQObjectInstance(ids[i]);
        //QFExtension* extension=m_extManager->getInstance(ids[i]);
        QFExtensionFilterChanger* FilterChanger = qobject_cast<QFExtensionFilterChanger*>(extobj);
        if (FilterChanger) FilterChangers.append(extobj);
    }
    QPoint sl(-1,-1);
    addItem(QIcon(":/libqf3widgets/filterchanger_none.png"), tr("--- none ---"), sl);

    for (int i=0; i<FilterChangers.size(); i++) {
        QFExtension* extension=qobject_cast<QFExtension*>(FilterChangers[i]);
        QFExtensionFilterChanger* FilterChanger = qobject_cast<QFExtensionFilterChanger*>(FilterChangers[i]);
        for (unsigned int j=0; j<FilterChanger->getFilterChangerCount(); j++) {
            QPoint sl(i, j);
            QString name=extension->getName();
            QString cname=FilterChanger->getFilterChangerShortName(j);
            if (FilterChanger->getFilterChangerCount()>0) name=name+QString(" #%1").arg(j).arg((cname!=name && !cname.isEmpty())?QString(" (%1)").arg(cname):QString(""));
            addItem(QIcon(extension->getIconFilename()), name, sl);
        }
    }

}

QFExtension* QFFilterChangerComboBox::currentExtension() const {
    if (currentIndex()<0) {
        return NULL;
    }

    QPoint p = itemData(currentIndex()).toPoint();
    QFExtension* extension=NULL;
    QFExtensionFilterChanger* FilterChanger=NULL;
    //int FilterChangerIdx=p.y();
    if ((p.x()>=0)&&(p.x()<FilterChangers.size())) {
        extension=qobject_cast<QFExtension*>(FilterChangers[p.x()]);
        FilterChanger=qobject_cast<QFExtensionFilterChanger*>(FilterChangers[p.x()]);
    }
    return extension;
}

QObject* QFFilterChangerComboBox::currentExtensionObject() const {
    if (currentIndex()<0) {
        return NULL;
    }

    QPoint p = itemData(currentIndex()).toPoint();
    if ((p.x()>=0)&&(p.x()<FilterChangers.size())) {
        return FilterChangers[p.x()];
    }
    return NULL;
}
QFExtensionFilterChanger* QFFilterChangerComboBox::currentExtensionFilterChanger() const {
    if (currentIndex()<0) {
        return NULL;
    }

    QPoint p = itemData(currentIndex()).toPoint();
    QFExtension* extension=NULL;
    QFExtensionFilterChanger* FilterChanger=NULL;
    //int FilterChangerIdx=p.y();
    if ((p.x()>=0)&&(p.x()<FilterChangers.size())) {
        extension=qobject_cast<QFExtension*>(FilterChangers[p.x()]);
        FilterChanger=qobject_cast<QFExtensionFilterChanger*>(FilterChangers[p.x()]);
    }
    return FilterChanger;
}

int QFFilterChangerComboBox::currentFilterChangerID() const {
    if (currentIndex()<0) {
        return -1;
    }

    QPoint p = itemData(currentIndex()).toPoint();
    QFExtension* extension=NULL;
    QFExtensionFilterChanger* FilterChanger=NULL;
    int FilterChangerIdx=p.y();
    if ((p.x()>=0)&&(p.x()<FilterChangers.size())) {
        extension=qobject_cast<QFExtension*>(FilterChangers[p.x()]);
        FilterChanger=qobject_cast<QFExtensionFilterChanger*>(FilterChangers[p.x()]);
    } else {
        FilterChangerIdx=-1;
    }
    return FilterChangerIdx;
}

int QFFilterChangerComboBox::currentExtensionID() const {
    if (currentIndex()<0) {
        return -1;
    }

    QPoint p = itemData(currentIndex()).toPoint();
    return p.x();
}

void QFFilterChangerComboBox::comboIndexChanged(int index) {

    QPoint p = itemData(index).toPoint();
    QFExtension* extension=NULL;
    QFExtensionFilterChanger* FilterChanger=NULL;
    int FilterChangerIdx=p.y();
    if ((p.x()>=0)&&(p.x()<FilterChangers.size())) {
        extension=qobject_cast<QFExtension*>(FilterChangers[p.x()]);
        FilterChanger=qobject_cast<QFExtensionFilterChanger*>(FilterChangers[p.x()]);
    } else {
        FilterChangerIdx=-1;
    }
}

void QFFilterChangerComboBox::storeSettings(QSettings& settings, QString prefix) const {
    if (currentExtension()) {
        settings.setValue(prefix+"filterchanger_plugin_id", currentExtension()->getID());
        settings.setValue(prefix+"filterchanger_id", currentFilterChangerID());
    } else {
        settings.setValue(prefix+"filterchanger_plugin_id", "");
        settings.setValue(prefix+"filterchanger_id", -1);
    }
}

void QFFilterChangerComboBox::storeSettings(QFManyFilesSettings &settings, QString prefix) const
{
    if (currentExtension()) {
        settings.setValue(prefix+"filterchanger_plugin_id", currentExtension()->getID());
        settings.setValue(prefix+"filterchanger_id", currentFilterChangerID());
    } else {
        settings.setValue(prefix+"filterchanger_plugin_id", "");
        settings.setValue(prefix+"filterchanger_id", -1);
    }
}

void QFFilterChangerComboBox::loadSettings(QSettings& settings, QString prefix) {
   QString id=settings.value(prefix+"filterchanger_plugin_id", "").toString();
   int FilterChangerIdx=settings.value(prefix+"filterchanger_id", -1).toInt();
   setCurrentIndex(0);
   bool ok=false;
   QFExtension* extension=NULL;
   QFExtensionFilterChanger* FilterChanger=NULL;
   for (int i=0; i<count(); i++) {
       QPoint p = itemData(i).toPoint();
       int FilterChangerIdxC=p.y();
       if ((p.x()>=0)&&(p.x()<FilterChangers.size())) {
           extension=qobject_cast<QFExtension*>(FilterChangers[p.x()]);
           FilterChanger=qobject_cast<QFExtensionFilterChanger*>(FilterChangers[p.x()]);
           if (extension && extension->getID()==id && FilterChangerIdx==FilterChangerIdxC) {
               ok=true;
               setCurrentIndex(i);
               break;
           }
       }
   }
   if (!ok) setCurrentIndex(0);

}

void QFFilterChangerComboBox::loadSettings(QFManyFilesSettings &settings, QString prefix)
{
    QString id=settings.value(prefix+"filterchanger_plugin_id", "").toString();
    int FilterChangerIdx=settings.value(prefix+"filterchanger_id", -1).toInt();
    setCurrentIndex(0);
    bool ok=false;
    QFExtension* extension=NULL;
    QFExtensionFilterChanger* FilterChanger=NULL;
    for (int i=0; i<count(); i++) {
        QPoint p = itemData(i).toPoint();
        int FilterChangerIdxC=p.y();
        if ((p.x()>=0)&&(p.x()<FilterChangers.size())) {
            extension=qobject_cast<QFExtension*>(FilterChangers[p.x()]);
            FilterChanger=qobject_cast<QFExtensionFilterChanger*>(FilterChangers[p.x()]);
            if (extension && extension->getID()==id && FilterChangerIdx==FilterChangerIdxC) {
                ok=true;
                setCurrentIndex(i);
                break;
            }
        }
    }
    if (!ok) setCurrentIndex(0);
}
