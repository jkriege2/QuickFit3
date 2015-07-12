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


#include "qfacquisitioncombobox.h"

QFAcquisitionComboBox::QFAcquisitionComboBox(QFExtensionManager* extManager, QWidget* parent):
    QEnhancedComboBox(parent)
{
    m_extManager=extManager;
    setSizeAdjustPolicy(QComboBox::AdjustToContents);

    findExtensions();
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(deviceIndexChanged(int)));
}

QFAcquisitionComboBox::QFAcquisitionComboBox(QWidget* parent):
    QEnhancedComboBox(parent)
{
    m_extManager=NULL;
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(deviceIndexChanged(int)));
}
QFAcquisitionComboBox::~QFAcquisitionComboBox()
{
    //dtor
}

void QFAcquisitionComboBox::init(QFExtensionManager* extManager) {
    m_extManager=extManager;
    findExtensions();
}

void QFAcquisitionComboBox::findExtensions() {
    devices.clear();
    clear();
    if (m_extManager==NULL) return;
    // load available acquisition devices from extManager
    QStringList ids=m_extManager->getIDList();
    for (int i=0; i<ids.size(); i++) {
        QObject* extobj=m_extManager->getQObjectInstance(ids[i]);
        //QFExtension* extension=m_extManager->getInstance(ids[i]);
        QFExtensionAcquisition* cam = qobject_cast<QFExtensionAcquisition*>(extobj);
        if (cam) devices.append(extobj);
    }

    for (int i=0; i<devices.size(); i++) {
        QFExtension* extension=qobject_cast<QFExtension*>(devices[i]);
        QFExtensionAcquisition* cam = qobject_cast<QFExtensionAcquisition*>(devices[i]);
        for (unsigned int j=0; j<cam->getAcquisitionDeviceCount(); j++) {
            QPoint sl(i, j);
            QString name=extension->getName();
            if (cam->getAcquisitionDeviceCount()>0) name=name+QString(" #%1").arg(j);
            addItem(QIcon(extension->getIconFilename()), name, sl);
        }
    }

}

QFExtension* QFAcquisitionComboBox::currentExtension() const {
    if (currentIndex()<0) {
        return NULL;
    }

    QPoint p = itemData(currentIndex()).toPoint();
    QFExtension* extension=NULL;
    QFExtensionAcquisition* cam=NULL;
    //int camIdx=p.y();
    if ((p.x()>=0)&&(p.x()<devices.size())) {
        extension=qobject_cast<QFExtension*>(devices[p.x()]);
        cam=qobject_cast<QFExtensionAcquisition*>(devices[p.x()]);
    }
    return extension;
}

QFExtensionAcquisition *QFAcquisitionComboBox::currentExtensionAcquisition() const {
    if (currentIndex()<0) {
        return NULL;
    }

    QPoint p = itemData(currentIndex()).toPoint();
    QFExtension* extension=NULL;
    QFExtensionAcquisition* cam=NULL;
    //int camIdx=p.y();
    if ((p.x()>=0)&&(p.x()<devices.size())) {
        extension=qobject_cast<QFExtension*>(devices[p.x()]);
        cam=qobject_cast<QFExtensionAcquisition*>(devices[p.x()]);
    }
    return cam;
}

QObject *QFAcquisitionComboBox::currentAcquisitionQObject() const {
    if (currentIndex()<0) {
        return NULL;
    }

    QPoint p = itemData(currentIndex()).toPoint();
    QFExtension* extension=NULL;
    QFExtensionAcquisition* cam=NULL;
    //int camIdx=p.y();
    if ((p.x()>=0)&&(p.x()<devices.size())) {
        return devices[p.x()];
    }
    return NULL;
}

int QFAcquisitionComboBox::currentAcquisitionID() const {
    if (currentIndex()<0) {
        return -1;
    }

    QPoint p = itemData(currentIndex()).toPoint();
    QFExtension* extension=NULL;
    QFExtensionAcquisition* cam=NULL;
    int camIdx=p.y();
    if ((p.x()>=0)&&(p.x()<devices.size())) {
        extension=qobject_cast<QFExtension*>(devices[p.x()]);
        cam=qobject_cast<QFExtensionAcquisition*>(devices[p.x()]);
    } else {
        camIdx=-1;
    }
    return camIdx;
}

int QFAcquisitionComboBox::currentExtensionID() const {
    if (currentIndex()<0) {
        return -1;
    }

    QPoint p = itemData(currentIndex()).toPoint();
    return p.x();
}

void QFAcquisitionComboBox::deviceIndexChanged(int index) {
    if (index<0) {
        emit deviceChanged(NULL, NULL, -1);
    }

    QPoint p = itemData(index).toPoint();
    QFExtension* extension=NULL;
    QFExtensionAcquisition* cam=NULL;
    int camIdx=p.y();
    if ((p.x()>=0)&&(p.x()<devices.size())) {
        extension=qobject_cast<QFExtension*>(devices[p.x()]);
        cam=qobject_cast<QFExtensionAcquisition*>(devices[p.x()]);
    } else {
        camIdx=-1;
    }
    emit deviceChanged(extension, cam, camIdx);
}

void QFAcquisitionComboBox::storeSettings(QSettings& settings, QString prefix) const {
    if (currentExtension()) {
        settings.setValue(prefix+"device_plugin_id", currentExtension()->getID());
        settings.setValue(prefix+"device_id", currentAcquisitionID());
    } else {
        settings.setValue(prefix+"device_plugin_id", "");
        settings.setValue(prefix+"device_id", -1);
    }
}

void QFAcquisitionComboBox::loadSettings(QSettings& settings, QString prefix) {
   QString id=settings.value(prefix+"device_plugin_id", "").toString();
   int camIdx=settings.value(prefix+"device_id", -1).toInt();
   bool ok=false;
   QFExtension* extension=NULL;
   QFExtensionAcquisition* cam=NULL;
   for (int i=0; i<count(); i++) {
       QPoint p = itemData(i).toPoint();
       int camIdxC=p.y();
       if ((p.x()>=0)&&(p.x()<devices.size())) {
           extension=qobject_cast<QFExtension*>(devices[p.x()]);
           cam=qobject_cast<QFExtensionAcquisition*>(devices[p.x()]);
           if (extension && extension->getID()==id && camIdx==camIdxC) {
               ok=true;
               setCurrentIndex(i);
               break;
           }
       }
   }
   if (!ok) setCurrentIndex(-1);

}
