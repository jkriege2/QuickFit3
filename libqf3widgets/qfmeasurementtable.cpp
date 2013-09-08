#include "qfmeasurementtable.h"

QFMeasurementTable::QFMeasurementTable(QWidget *parent) :
    QWidget(parent)
{
}

void QFMeasurementTable::init(QFPluginLogService *log, QFPluginServices *pluginServices)
{

}

void QFMeasurementTable::setLog(QFPluginLogService *log)
{

}

void QFMeasurementTable::loadSettings(QSettings &settings, QString prefix)
{

}

void QFMeasurementTable::loadSettings(QFManyFilesSettings &settings, QString prefix)
{

}

void QFMeasurementTable::saveSettings(QSettings &settings, QString prefix)
{

}

void QFMeasurementTable::saveSettings(QFManyFilesSettings &settings, QString prefix)
{

}

void QFMeasurementTable::connectMeasurementDevices()
{

}

void QFMeasurementTable::disconnectMeasurementDevices()
{

}

bool QFMeasurementTable::areMeasurementDevicesConnected() const
{
    return false;
}

void QFMeasurementTable::lockMeasurementDevices()
{

}

void QFMeasurementTable::unlockMeasurementDevices()
{

}

QList<QFExtensionMeasurementAndControlDevice *> QFMeasurementTable::getMeasurementDevices() const
{
    return QList<QFExtensionMeasurementAndControlDevice *>();
}

QList<QFExtension *> QFMeasurementTable::getMeasurementDeviceExtensions() const
{
    return QList<QFExtension *>();
}

QList<QObject *> QFMeasurementTable::getMeasurementDeviceExtensionObjects() const
{
    return QList<QObject *>();
}

QList<int> QFMeasurementTable::getMeasurementDeviceIDs() const
{
    return QList<int>();
}
