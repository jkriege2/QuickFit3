/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

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


#include "qfcameracombobox.h"

QFCameraComboBox::QFCameraComboBox(QFExtensionManager* extManager, QWidget* parent):
    QEnhancedComboBox(parent)
{
    m_extManager=extManager;
    setSizeAdjustPolicy(QComboBox::AdjustToContents);

    findExtensions();
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(camIndexChanged(int)));
}

QFCameraComboBox::QFCameraComboBox(QWidget* parent):
    QEnhancedComboBox(parent)
{
    m_extManager=NULL;
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(camIndexChanged(int)));
}
QFCameraComboBox::~QFCameraComboBox()
{
    //dtor
}

void QFCameraComboBox::init(QFExtensionManager* extManager) {
    m_extManager=extManager;
    findExtensions();
}

void QFCameraComboBox::findExtensions() {
    cameras.clear();
    clear();
    if (m_extManager==NULL) return;
    // load available acquisition devices from extManager
    QStringList ids=m_extManager->getIDList();
    for (int i=0; i<ids.size(); i++) {
        QObject* extobj=m_extManager->getQObjectInstance(ids[i]);
        //QFExtension* extension=m_extManager->getInstance(ids[i]);
        QFExtensionCamera* cam = qobject_cast<QFExtensionCamera*>(extobj);
        if (cam) cameras.append(extobj);
    }

    for (int i=0; i<cameras.size(); i++) {
        QFExtension* extension=qobject_cast<QFExtension*>(cameras[i]);
        QFExtensionCamera* cam = qobject_cast<QFExtensionCamera*>(cameras[i]);
        for (unsigned int j=0; j<cam->getCameraCount(); j++) {
            QPoint sl(i, j);
            QString name=extension->getName();
            QString cname=cam->getCameraName(j);
            if (!cam->getCameraSensorName(j).isEmpty()) cname+=QString(" (%1)").arg(cam->getCameraSensorName(j));
            if (cam->getCameraCount()>0) name=name+QString(" #%1: %2").arg(j).arg(cname);
            addItem(QIcon(extension->getIconFilename()), name, sl);
        }
    }

}

QFExtension* QFCameraComboBox::currentExtension() const {
    if (currentIndex()<0) {
        return NULL;
    }

    QPoint p = itemData(currentIndex()).toPoint();
    QFExtension* extension=NULL;
    QFExtensionCamera* cam=NULL;
    //int camIdx=p.y();
    if ((p.x()>=0)&&(p.x()<cameras.size())) {
        extension=qobject_cast<QFExtension*>(cameras[p.x()]);
        cam=qobject_cast<QFExtensionCamera*>(cameras[p.x()]);
    }
    return extension;
}

QFExtensionCamera* QFCameraComboBox::currentExtensionCamera() const {
    if (currentIndex()<0) {
        return NULL;
    }

    QPoint p = itemData(currentIndex()).toPoint();
    QFExtension* extension=NULL;
    QFExtensionCamera* cam=NULL;
    //int camIdx=p.y();
    if ((p.x()>=0)&&(p.x()<cameras.size())) {
        extension=qobject_cast<QFExtension*>(cameras[p.x()]);
        cam=qobject_cast<QFExtensionCamera*>(cameras[p.x()]);
    }
    return cam;
}

QObject *QFCameraComboBox::currentCameraQObject() const {
    if (currentIndex()<0) {
        return NULL;
    }

    QPoint p = itemData(currentIndex()).toPoint();
    QFExtension* extension=NULL;
    QFExtensionCamera* cam=NULL;
    //int camIdx=p.y();
    if ((p.x()>=0)&&(p.x()<cameras.size())) {
        return cameras[p.x()];
    }
    return NULL;
}

int QFCameraComboBox::currentCameraID() const {
    if (currentIndex()<0) {
        return -1;
    }

    QPoint p = itemData(currentIndex()).toPoint();
    QFExtension* extension=NULL;
    QFExtensionCamera* cam=NULL;
    int camIdx=p.y();
    if ((p.x()>=0)&&(p.x()<cameras.size())) {
        extension=qobject_cast<QFExtension*>(cameras[p.x()]);
        cam=qobject_cast<QFExtensionCamera*>(cameras[p.x()]);
    } else {
        camIdx=-1;
    }
    return camIdx;
}

int QFCameraComboBox::currentExtensionID() const {
    if (currentIndex()<0) {
        return -1;
    }

    QPoint p = itemData(currentIndex()).toPoint();
    return p.x();
}

void QFCameraComboBox::camIndexChanged(int index) {
    if (index<0) {
        emit cameraChanged(NULL, NULL, -1);
    }

    QPoint p = itemData(index).toPoint();
    QFExtension* extension=NULL;
    QFExtensionCamera* cam=NULL;
    int camIdx=p.y();
    if ((p.x()>=0)&&(p.x()<cameras.size())) {
        extension=qobject_cast<QFExtension*>(cameras[p.x()]);
        cam=qobject_cast<QFExtensionCamera*>(cameras[p.x()]);
    } else {
        camIdx=-1;
    }
    emit cameraChanged(extension, cam, camIdx);
}

void QFCameraComboBox::storeSettings(QSettings& settings, QString prefix) const {
    if (currentExtension()) {
        settings.setValue(prefix+"camera_plugin_id", currentExtension()->getID());
        settings.setValue(prefix+"camera_id", currentCameraID());
    } else {
        settings.setValue(prefix+"camera_plugin_id", "");
        settings.setValue(prefix+"camera_id", -1);
    }
}

void QFCameraComboBox::storeSettings(QFManyFilesSettings &settings, QString prefix) const
{
    if (currentExtension()) {
        settings.setValue(prefix+"camera_plugin_id", currentExtension()->getID());
        settings.setValue(prefix+"camera_id", currentCameraID());
    } else {
        settings.setValue(prefix+"camera_plugin_id", "");
        settings.setValue(prefix+"camera_id", -1);
    }
}

void QFCameraComboBox::loadSettings(QSettings& settings, QString prefix) {
   QString id=settings.value(prefix+"camera_plugin_id", "").toString();
   int camIdx=settings.value(prefix+"camera_id", -1).toInt();
   bool ok=false;
   QFExtension* extension=NULL;
   QFExtensionCamera* cam=NULL;
   for (int i=0; i<count(); i++) {
       QPoint p = itemData(i).toPoint();
       int camIdxC=p.y();
       if ((p.x()>=0)&&(p.x()<cameras.size())) {
           extension=qobject_cast<QFExtension*>(cameras[p.x()]);
           cam=qobject_cast<QFExtensionCamera*>(cameras[p.x()]);
           if (extension && extension->getID()==id && camIdx==camIdxC) {
               ok=true;
               setCurrentIndex(i);
               break;
           }
       }
   }
   if (!ok) setCurrentIndex(-1);

}

void QFCameraComboBox::loadSettings(QFManyFilesSettings &settings, QString prefix)
{
    QString id=settings.value(prefix+"camera_plugin_id", "").toString();
    int camIdx=settings.value(prefix+"camera_id", -1).toInt();
    bool ok=false;
    QFExtension* extension=NULL;
    QFExtensionCamera* cam=NULL;
    for (int i=0; i<count(); i++) {
        QPoint p = itemData(i).toPoint();
        int camIdxC=p.y();
        if ((p.x()>=0)&&(p.x()<cameras.size())) {
            extension=qobject_cast<QFExtension*>(cameras[p.x()]);
            cam=qobject_cast<QFExtensionCamera*>(cameras[p.x()]);
            if (extension && extension->getID()==id && camIdx==camIdxC) {
                ok=true;
                setCurrentIndex(i);
                break;
            }
        }
    }
    if (!ok) setCurrentIndex(-1);
}
