/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

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


#include "qfstagecombobox.h"

QFStageComboBox::QFStageComboBox(QFExtensionManager* extManager, QWidget* parent):
    QEnhancedComboBox(parent)
{
    init(extManager);
}

QFStageComboBox::QFStageComboBox(QWidget* parent):
    QEnhancedComboBox(parent)
{
    m_extManager=NULL;
    setSizeAdjustPolicy(QComboBox::AdjustToContents);

}

QFStageComboBox::~QFStageComboBox()
{
    //dtor
}

void QFStageComboBox::init(QFExtensionManager* extManager) {
    m_extManager=extManager;
    findExtensions();
}

void QFStageComboBox::findExtensions() {
    stages.clear();
    clear();
    if (!m_extManager) return;
    // load available acquisition devices from extManager
    QStringList ids=m_extManager->getIDList();
    for (int i=0; i<ids.size(); i++) {
        QObject* extobj=m_extManager->getQObjectInstance(ids[i]);
        //QFExtension* extension=m_extManager->getInstance(ids[i]);
        QFExtensionLinearStage* stage = qobject_cast<QFExtensionLinearStage*>(extobj);
        if (stage) stages.append(extobj);
    }

    addItem(QIcon(":/libqf3widgets/stage_none.png"), tr("--- none ---"), QPoint(-1, -1));
    for (int i=0; i<stages.size(); i++) {
        QFExtension* extension=qobject_cast<QFExtension*>(stages[i]);
        QFExtensionLinearStage* stage = qobject_cast<QFExtensionLinearStage*>(stages[i]);
        for (unsigned int j=0; j<stage->getAxisCount(); j++) {
            QPoint sl(i, j);
            QString name=extension->getName();
            if (stage->getAxisCount()>0) name=name+QString(" #%1").arg(j);
            addItem(QIcon(extension->getIconFilename()), name, sl);
        }
    }

}

QObject* QFStageComboBox::currentExtensionObject() const {
    if (currentIndex()<1) {
        return NULL;
    }

    QPoint p = itemData(currentIndex()).toPoint();
    if ((p.x()>=0)&&(p.x()<stages.size())) {
        return stages[p.x()];
    }
    return NULL;
}

QFExtension* QFStageComboBox::currentExtension() const {
    if (currentIndex()<1) {
        return NULL;
    }

    QPoint p = itemData(currentIndex()).toPoint();
    QFExtension* extension=NULL;
    QFExtensionLinearStage* stage=NULL;
    //int stageIdx=p.y();
    if ((p.x()>=0)&&(p.x()<stages.size())) {
        extension=qobject_cast<QFExtension*>(stages[p.x()]);
        stage=qobject_cast<QFExtensionLinearStage*>(stages[p.x()]);
    }
    return extension;
}

QFExtensionLinearStage* QFStageComboBox::currentExtensionLinearStage() const {
    if (currentIndex()<1) {
        return NULL;
    }

    QPoint p = itemData(currentIndex()).toPoint();
    QFExtension* extension=NULL;
    QFExtensionLinearStage* stage=NULL;
    //int stageIdx=p.y();
    if ((p.x()>=0)&&(p.x()<stages.size())) {
        extension=qobject_cast<QFExtension*>(stages[p.x()]);
        stage=qobject_cast<QFExtensionLinearStage*>(stages[p.x()]);
    }
    return stage;
}

int QFStageComboBox::currentAxisID() const {
    if (currentIndex()<1) {
        return -1;
    }

    QPoint p = itemData(currentIndex()).toPoint();
    QFExtension* extension=NULL;
    QFExtensionLinearStage* stage=NULL;
    int stageIdx=p.y();
    if ((p.x()>=0)&&(p.x()<stages.size())) {
        extension=qobject_cast<QFExtension*>(stages[p.x()]);
        stage=qobject_cast<QFExtensionLinearStage*>(stages[p.x()]);
    } else {
        stageIdx=-1;
    }
    return stageIdx;
}


int QFStageComboBox::currentExtensionID() const {
    if (currentIndex()<1) {
        return -1;
    }

    QPoint p = itemData(currentIndex()).toPoint();
    return p.x();
}

void QFStageComboBox::storeSettings(QSettings& settings, QString prefix) const {
    if (currentExtension()) {
        settings.setValue(prefix+"stage_plugin_id", currentExtension()->getID());
        settings.setValue(prefix+"stage_axis_id", currentAxisID());
    } else {
        settings.setValue(prefix+"stage_plugin_id", "");
        settings.setValue(prefix+"stage_axis_id", -1);
    }
}

void QFStageComboBox::storeSettings(QFManyFilesSettings &settings, QString prefix) const
{
    if (currentExtension()) {
        settings.setValue(prefix+"stage_plugin_id", currentExtension()->getID());
        settings.setValue(prefix+"stage_axis_id", currentAxisID());
    } else {
        settings.setValue(prefix+"stage_plugin_id", "");
        settings.setValue(prefix+"stage_axis_id", -1);
    }
}

void QFStageComboBox::loadSettings(QSettings& settings, QString prefix) {
   QString id=settings.value(prefix+"stage_plugin_id", "").toString();
   int axisIdx=settings.value(prefix+"stage_axis_id", -1).toInt();
   bool ok=false;
   QFExtension* extension=NULL;
   for (int i=0; i<count(); i++) {
       QPoint p = itemData(i).toPoint();
       int axisIdxC=p.y();
       if ((p.x()>=0)&&(p.x()<stages.size())) {
           extension=qobject_cast<QFExtension*>(stages[p.x()]);
           if (extension && extension->getID()==id && axisIdx==axisIdxC) {
               ok=true;
               setCurrentIndex(i);
               break;
           }
       }
   }
   if (!ok) setCurrentIndex(0);

}

void QFStageComboBox::loadSettings(QFManyFilesSettings &settings, QString prefix)
{
    QString id=settings.value(prefix+"stage_plugin_id", "").toString();
    int axisIdx=settings.value(prefix+"stage_axis_id", -1).toInt();
    bool ok=false;
    QFExtension* extension=NULL;
    for (int i=0; i<count(); i++) {
        QPoint p = itemData(i).toPoint();
        int axisIdxC=p.y();
        if ((p.x()>=0)&&(p.x()<stages.size())) {
            extension=qobject_cast<QFExtension*>(stages[p.x()]);
            if (extension && extension->getID()==id && axisIdx==axisIdxC) {
                ok=true;
                setCurrentIndex(i);
                break;
            }
        }
    }
    if (!ok) setCurrentIndex(0);
}
