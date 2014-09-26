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
