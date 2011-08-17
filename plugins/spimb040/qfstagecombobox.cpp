#include "qfstagecombobox.h"

QFStageComboBox::QFStageComboBox(QFExtensionManager* extManager, QWidget* parent):
    QComboBox(parent)
{
    init(extManager);
}

QFStageComboBox::QFStageComboBox(QWidget* parent):
    QComboBox(parent)
{
    m_extManager=NULL;
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

QFExtension* QFStageComboBox::currentExtension() const {
    if (currentIndex()<0) {
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
    if (currentIndex()<0) {
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
    if (currentIndex()<0) {
        return NULL;
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
    if (currentIndex()<0) {
        return -1;
    }

    QPoint p = itemData(currentIndex()).toPoint();
    return p.x();
}
