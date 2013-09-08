#include "qfmeasurementdevicecombobox.h"

QFMeasurementDeviceComboBox::QFMeasurementDeviceComboBox(QFExtensionManager* extManager, QWidget* parent):
    QEnhancedComboBox(parent)
{
    m_extManager=extManager;
    setSizeAdjustPolicy(QComboBox::AdjustToContents);

    findExtensions();
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(comboIndexChanged(int)));
}

QFMeasurementDeviceComboBox::QFMeasurementDeviceComboBox(QWidget* parent):
    QEnhancedComboBox(parent)
{
    m_extManager=NULL;
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(comboIndexChanged(int)));
}
QFMeasurementDeviceComboBox::~QFMeasurementDeviceComboBox()
{
    //dtor
}

void QFMeasurementDeviceComboBox::init(QFExtensionManager* extManager) {
    m_extManager=extManager;
    findExtensions();
}

void QFMeasurementDeviceComboBox::findExtensions() {
    devices.clear();
    clear();
    if (m_extManager==NULL) return;
    // load available acquisition devices from extManager
    QStringList ids=m_extManager->getIDList();
    for (int i=0; i<ids.size(); i++) {
        QObject* extobj=m_extManager->getQObjectInstance(ids[i]);
        //QFExtension* extension=m_extManager->getInstance(ids[i]);
        QFExtensionMeasurementAndControlDevice* shutter = qobject_cast<QFExtensionMeasurementAndControlDevice*>(extobj);
        if (shutter) devices.append(extobj);
    }
    QPoint sl(-1,-1);
    addItem(QIcon(":/libqf3widgets/measurementdevice_none.png"), tr("--- none ---"), sl);

    for (int i=0; i<devices.size(); i++) {
        QFExtension* extension=qobject_cast<QFExtension*>(devices[i]);
        QFExtensionMeasurementAndControlDevice* device = qobject_cast<QFExtensionMeasurementAndControlDevice*>(devices[i]);
        for (unsigned int j=0; j<device->getMeasurementDeviceCount(); j++) {
            QPoint sl(i, j);
            QString name=extension->getName();
            QString cname=device->getMeasurementDeviceName(j);
            if (device->getMeasurementDeviceCount()>0) name=name+QString(" #%1").arg(j).arg((cname!=name && !cname.isEmpty())?QString(" (%1)").arg(cname):QString(""));
            addItem(QIcon(extension->getIconFilename()), name, sl);
        }
    }

}

QFExtension* QFMeasurementDeviceComboBox::currentExtension() const {
    if (currentIndex()<0) {
        return NULL;
    }

    QPoint p = itemData(currentIndex()).toPoint();
    QFExtension* extension=NULL;
    QFExtensionMeasurementAndControlDevice* device=NULL;
    //int shutterIdx=p.y();
    if ((p.x()>=0)&&(p.x()<devices.size())) {
        extension=qobject_cast<QFExtension*>(devices[p.x()]);
        device=qobject_cast<QFExtensionMeasurementAndControlDevice*>(devices[p.x()]);
    }
    return extension;
}

QObject *QFMeasurementDeviceComboBox::currentExtensionObject() const
{
    if (currentIndex()<0) {
        return NULL;
    }

    QPoint p = itemData(currentIndex()).toPoint();
    if ((p.x()>=0)&&(p.x()<devices.size())) {
        return devices[p.x()];
    }
    return NULL;
}

QFExtensionMeasurementAndControlDevice* QFMeasurementDeviceComboBox::currentExtensionDevice() const {
    if (currentIndex()<0) {
        return NULL;
    }

    QPoint p = itemData(currentIndex()).toPoint();
    QFExtension* extension=NULL;
    QFExtensionMeasurementAndControlDevice* device=NULL;
    //int shutterIdx=p.y();
    if ((p.x()>=0)&&(p.x()<devices.size())) {
        extension=qobject_cast<QFExtension*>(devices[p.x()]);
        device=qobject_cast<QFExtensionMeasurementAndControlDevice*>(devices[p.x()]);
    }
    return device;
}

int QFMeasurementDeviceComboBox::currentDeviceID() const {
    if (currentIndex()<0) {
        return -1;
    }

    QPoint p = itemData(currentIndex()).toPoint();
    QFExtension* extension=NULL;
    QFExtensionMeasurementAndControlDevice* device=NULL;
    int shutterIdx=p.y();
    if ((p.x()>=0)&&(p.x()<devices.size())) {
        extension=qobject_cast<QFExtension*>(devices[p.x()]);
        device=qobject_cast<QFExtensionMeasurementAndControlDevice*>(devices[p.x()]);
    } else {
        shutterIdx=-1;
    }
    return shutterIdx;
}

int QFMeasurementDeviceComboBox::currentExtensionID() const {
    if (currentIndex()<0) {
        return -1;
    }

    QPoint p = itemData(currentIndex()).toPoint();
    return p.x();
}

void QFMeasurementDeviceComboBox::comboIndexChanged(int index) {

    QPoint p = itemData(index).toPoint();
    QFExtension* extension=NULL;
    QFExtensionMeasurementAndControlDevice* device=NULL;
    int shutterIdx=p.y();
    if ((p.x()>=0)&&(p.x()<devices.size())) {
        extension=qobject_cast<QFExtension*>(devices[p.x()]);
        device=qobject_cast<QFExtensionMeasurementAndControlDevice*>(devices[p.x()]);
    } else {
        shutterIdx=-1;
    }
}

void QFMeasurementDeviceComboBox::storeSettings(QSettings& settings, QString prefix) const {
    if (currentExtension()) {
        settings.setValue(prefix+"device_plugin_id", currentExtension()->getID());
        settings.setValue(prefix+"device_id", currentDeviceID());
    } else {
        settings.setValue(prefix+"device_plugin_id", "");
        settings.setValue(prefix+"device_id", -1);
    }
}

void QFMeasurementDeviceComboBox::storeSettings(QFManyFilesSettings &settings, QString prefix) const
{
    if (currentExtension()) {
        settings.setValue(prefix+"device_plugin_id", currentExtension()->getID());
        settings.setValue(prefix+"device_id", currentDeviceID());
    } else {
        settings.setValue(prefix+"device_plugin_id", "");
        settings.setValue(prefix+"device_id", -1);
    }
}

void QFMeasurementDeviceComboBox::loadSettings(QSettings& settings, QString prefix) {
   QString id=settings.value(prefix+"device_plugin_id", "").toString();
   int shutterIdx=settings.value(prefix+"device_id", -1).toInt();
   setCurrentIndex(0);
   bool ok=false;
   QFExtension* extension=NULL;
   QFExtensionMeasurementAndControlDevice* device=NULL;
   for (int i=0; i<count(); i++) {
       QPoint p = itemData(i).toPoint();
       int shutterIdxC=p.y();
       if ((p.x()>=0)&&(p.x()<devices.size())) {
           extension=qobject_cast<QFExtension*>(devices[p.x()]);
           device=qobject_cast<QFExtensionMeasurementAndControlDevice*>(devices[p.x()]);
           if (extension && extension->getID()==id && shutterIdx==shutterIdxC) {
               ok=true;
               setCurrentIndex(i);
               break;
           }
       }
   }
   if (!ok) setCurrentIndex(0);

}

void QFMeasurementDeviceComboBox::loadSettings(QFManyFilesSettings &settings, QString prefix)
{
    QString id=settings.value(prefix+"device_plugin_id", "").toString();
    int shutterIdx=settings.value(prefix+"device_id", -1).toInt();
    setCurrentIndex(0);
    bool ok=false;
    QFExtension* extension=NULL;
    QFExtensionMeasurementAndControlDevice* device=NULL;
    for (int i=0; i<count(); i++) {
        QPoint p = itemData(i).toPoint();
        int shutterIdxC=p.y();
        if ((p.x()>=0)&&(p.x()<devices.size())) {
            extension=qobject_cast<QFExtension*>(devices[p.x()]);
            device=qobject_cast<QFExtensionMeasurementAndControlDevice*>(devices[p.x()]);
            if (extension && extension->getID()==id && shutterIdx==shutterIdxC) {
                ok=true;
                setCurrentIndex(i);
                break;
            }
        }
    }
    if (!ok) setCurrentIndex(0);
}
