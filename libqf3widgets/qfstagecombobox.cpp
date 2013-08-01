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
