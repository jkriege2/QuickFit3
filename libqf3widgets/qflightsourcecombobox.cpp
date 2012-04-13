#include "qflightsourcecombobox.h"

QFLightSourceComboBox::QFLightSourceComboBox(QFExtensionManager* extManager, QWidget* parent):
    QEnhancedComboBox(parent)
{
    m_extManager=extManager;
    setSizeAdjustPolicy(QComboBox::AdjustToContents);

    findExtensions();
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(comboIndexChanged(int)));
}

QFLightSourceComboBox::QFLightSourceComboBox(QWidget* parent):
    QEnhancedComboBox(parent)
{
    m_extManager=NULL;
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(comboIndexChanged(int)));
}
QFLightSourceComboBox::~QFLightSourceComboBox()
{
    //dtor
}

void QFLightSourceComboBox::init(QFExtensionManager* extManager) {
    m_extManager=extManager;
    findExtensions();
}

void QFLightSourceComboBox::findExtensions() {
    LightSources.clear();
    clear();
    if (m_extManager==NULL) return;
    // load available acquisition devices from extManager
    QStringList ids=m_extManager->getIDList();
    for (int i=0; i<ids.size(); i++) {
        QObject* extobj=m_extManager->getQObjectInstance(ids[i]);
        //QFExtension* extension=m_extManager->getInstance(ids[i]);
        QFExtensionLightSource* LightSource = qobject_cast<QFExtensionLightSource*>(extobj);
        if (LightSource) LightSources.append(extobj);
    }
    QPoint sl(-1,-1);
    addItem(QIcon(":/libqf3widgets/lightsource_none.png"), tr("--- none ---"), sl);

    for (int i=0; i<LightSources.size(); i++) {
        QFExtension* extension=qobject_cast<QFExtension*>(LightSources[i]);
        QFExtensionLightSource* LightSource = qobject_cast<QFExtensionLightSource*>(LightSources[i]);
        for (unsigned int j=0; j<LightSource->getLightSourceCount(); j++) {
            QPoint sl(i, j);
            QString name=extension->getName();
            QString cname=LightSource->getLightSourceShortName(j);
            if (LightSource->getLightSourceCount()>0) name=name+QString(" #%1").arg(j).arg((cname!=name && !cname.isEmpty())?QString(" (%1)").arg(cname):QString(""));
            addItem(QIcon(extension->getIconFilename()), name, sl);
        }
    }

}

QFExtension* QFLightSourceComboBox::currentExtension() const {
    if (currentIndex()<0) {
        return NULL;
    }

    QPoint p = itemData(currentIndex()).toPoint();
    QFExtension* extension=NULL;
    QFExtensionLightSource* LightSource=NULL;
    //int LightSourceIdx=p.y();
    if ((p.x()>=0)&&(p.x()<LightSources.size())) {
        extension=qobject_cast<QFExtension*>(LightSources[p.x()]);
        LightSource=qobject_cast<QFExtensionLightSource*>(LightSources[p.x()]);
    }
    return extension;
}

QFExtensionLightSource* QFLightSourceComboBox::currentExtensionLightSource() const {
    if (currentIndex()<0) {
        return NULL;
    }

    QPoint p = itemData(currentIndex()).toPoint();
    QFExtension* extension=NULL;
    QFExtensionLightSource* LightSource=NULL;
    //int LightSourceIdx=p.y();
    if ((p.x()>=0)&&(p.x()<LightSources.size())) {
        extension=qobject_cast<QFExtension*>(LightSources[p.x()]);
        LightSource=qobject_cast<QFExtensionLightSource*>(LightSources[p.x()]);
    }
    return LightSource;
}

int QFLightSourceComboBox::currentLightSourceID() const {
    if (currentIndex()<0) {
        return -1;
    }

    QPoint p = itemData(currentIndex()).toPoint();
    QFExtension* extension=NULL;
    QFExtensionLightSource* LightSource=NULL;
    int LightSourceIdx=p.y();
    if ((p.x()>=0)&&(p.x()<LightSources.size())) {
        extension=qobject_cast<QFExtension*>(LightSources[p.x()]);
        LightSource=qobject_cast<QFExtensionLightSource*>(LightSources[p.x()]);
    } else {
        LightSourceIdx=-1;
    }
    return LightSourceIdx;
}

int QFLightSourceComboBox::currentExtensionID() const {
    if (currentIndex()<0) {
        return -1;
    }

    QPoint p = itemData(currentIndex()).toPoint();
    return p.x();
}

void QFLightSourceComboBox::comboIndexChanged(int index) {

    QPoint p = itemData(index).toPoint();
    QFExtension* extension=NULL;
    QFExtensionLightSource* LightSource=NULL;
    int LightSourceIdx=p.y();
    if ((p.x()>=0)&&(p.x()<LightSources.size())) {
        extension=qobject_cast<QFExtension*>(LightSources[p.x()]);
        LightSource=qobject_cast<QFExtensionLightSource*>(LightSources[p.x()]);
    } else {
        LightSourceIdx=-1;
    }
}

void QFLightSourceComboBox::storeSettings(QSettings& settings, QString prefix) const {
    if (currentExtension()) {
        settings.setValue(prefix+"lightsource_plugin_id", currentExtension()->getID());
        settings.setValue(prefix+"lightsource_id", currentLightSourceID());
    } else {
        settings.setValue(prefix+"lightsource_plugin_id", "");
        settings.setValue(prefix+"lightsource_id", -1);
    }
}

void QFLightSourceComboBox::loadSettings(QSettings& settings, QString prefix) {
   QString id=settings.value(prefix+"lightsource_plugin_id", "").toString();
   int LightSourceIdx=settings.value(prefix+"lightsource_id", -1).toInt();
   setCurrentIndex(0);
   bool ok=false;
   QFExtension* extension=NULL;
   QFExtensionLightSource* LightSource=NULL;
   for (int i=0; i<count(); i++) {
       QPoint p = itemData(i).toPoint();
       int LightSourceIdxC=p.y();
       if ((p.x()>=0)&&(p.x()<LightSources.size())) {
           extension=qobject_cast<QFExtension*>(LightSources[p.x()]);
           LightSource=qobject_cast<QFExtensionLightSource*>(LightSources[p.x()]);
           if (extension && extension->getID()==id && LightSourceIdx==LightSourceIdxC) {
               ok=true;
               setCurrentIndex(i);
               break;
           }
       }
   }
   if (!ok) setCurrentIndex(0);

}
