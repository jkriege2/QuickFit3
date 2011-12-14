#include "qfshuttercombobox.h"

QFShutterComboBox::QFShutterComboBox(QFExtensionManager* extManager, QWidget* parent):
    QEnhancedComboBox(parent)
{
    m_extManager=extManager;
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

void QFShutterComboBox::loadSettings(QSettings& settings, QString prefix) {
   QString id=settings.value(prefix+"shutter_plugin_id", "").toString();
   int shutterIdx=settings.value(prefix+"shutter_id", -1).toInt();
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
   if (!ok) setCurrentIndex(-1);

}
