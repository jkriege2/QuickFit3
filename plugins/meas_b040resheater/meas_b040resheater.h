/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

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

#ifndef meas_b040resheater_H
#define meas_b040resheater_H

#include <time.h>
#include <QObject>
#include <QVector>
#include <QList>
#include "qfextension.h"
#include "qfextensionmeasurementdevice.h"
#include "../base_classes/qf3simpleb040serialprotocolhandler.h"
#include "../base_classes/qf3comportmanager.h"
#include "../../../../../LIB/trunk/jkserialconnection.h"

/*!
    \defgroup qf3ext_ShutterArduinoB040 QFExtensionShutter implementation for Relais converted to shutters and controlled by an Arduino UNO
    \ingroup qf3extensionplugins
*/

/*! \brief QFExtensionShutter implementation for Relais converted to shutters and controlled by an Arduino UNO
    \ingroup qf3ext_ShutterArduinoB040
 */
class QFExtensionB040ResistorHeater : public QObject, public QFExtensionBase, public QFExtensionMeasurementAndControlDevice, public QFPluginLogService {
        Q_OBJECT
        Q_INTERFACES(QFExtension QFExtensionMeasurementAndControlDevice QFPluginLogService)
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        Q_PLUGIN_METADATA(IID "www.dkfz.de.QuickFit3.Plugins.QFExtensionB040ResistorHeater")
#endif

    public:
        /** Default constructor */
        QFExtensionB040ResistorHeater(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFExtensionB040ResistorHeater();


    /////////////////////////////////////////////////////////////////////////////
    // QFExtension routines
    /////////////////////////////////////////////////////////////////////////////
        /** \copydoc QFExtension::getID() */
        virtual QString getID() const  { return QString("meas_b040resheater"); }
        /** \copydoc QFExtension::getName() */
        virtual QString getName() const  { return tr("B040 Temperature Controller"); }
        /** \copydoc QFExtension::getDescription() */
        virtual QString getDescription() const  { return tr("Temperature controller (PI loop) built in B040"); }
        /** \copydoc QFExtension::getAuthor() */
        virtual QString getAuthor() const  { return tr("Jan W. Krieger"); }
        /** \copydoc QFExtension::getCopyright() */
        virtual QString getCopyright() const  { return tr("(c) 2013 by Jan W. Krieger"); }
        /** \copydoc QFExtension::getWeblink() */
        virtual QString getWeblink() const  { return tr(""); }
        /** \copydoc QFExtension::getIconFilename() */
        virtual QString getIconFilename() const  { return QString(":/meas_b040resheater.png"); }
        /** \copydoc QFExtension::deinit() */
        virtual void deinit();
        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=0;
        };


        /** \brief log project text message
         *  \param message the message to log
         */
        virtual void log_text(QString message);
        /** \brief log project warning message
         *  \param message the warning message to log
         */
        virtual void log_warning(QString message);
        /** \brief log project error message
         *  \param message the error message to log
         */
        virtual void log_error(QString message);

    protected:
        /** \copydoc QFExtensionBase::projectChanged() */
        virtual void projectChanged(QFProject* oldProject, QFProject* project);
        /** \copydoc QFExtensionBase::initExtension() */
        virtual void initExtension();
        /** \copydoc QFExtensionBase::loadSettings() */
        virtual void loadSettings(ProgramOptions* settings);
        /** \copydoc QFExtensionBase::storeSettings() */
        virtual void storeSettings(ProgramOptions* settings);


        virtual void setLogging(QFPluginLogService* logService);


        /////////////////////////////////////////////////////////////////////////////
        // QFExtensionMeasurementAndControlDevice routines
        /////////////////////////////////////////////////////////////////////////////

        /** \copydoc QFExtensionMeasurementAndControlDevice::getMeasurementDeviceCount() */
        virtual unsigned int getMeasurementDeviceCount() ;
        /** \copydoc QFExtensionMeasurementDevice::getMeasurementDeviceName() */
        virtual QString getMeasurementDeviceName(unsigned int measuremenDevice) ;

        /** \copydoc QFExtensionMeasurementAndControlDevice::showMeasurementDeviceSettingsDialog() */
        virtual void showMeasurementDeviceSettingsDialog(unsigned int measuremenDevice, QWidget* parent=NULL);
        /** \copydoc QFExtensionMeasurementAndControlDevice::isMeasurementDeviceConnected() */
         virtual bool isMeasurementDeviceConnected(unsigned int measuremenDevice) ;
        /** \copydoc QFExtensionMeasurementAndControlDevice::connectMeasurementDevice() */
         virtual void connectMeasurementDevice(unsigned int measuremenDevice);
        /** \copydoc QFExtensionMeasurementAndControlDevice::disconnectMeasurementDevice() */
         virtual void disconnectMeasurementDevice(unsigned int measuremenDevice);
        /** \copydoc QFExtensionMeasurementAndControlDevice::setMeasurementDeviceLogging() */
         virtual void setMeasurementDeviceLogging(QFPluginLogService* logService);
        /** \copydoc QFExtensionMeasurementAndControlDevice::getMeasurementDeviceValueCount() */
        virtual unsigned int getMeasurementDeviceValueCount(unsigned int measuremenDevice);
        /** \copydoc QFExtensionMeasurementAndControlDevice::getMeasurementDeviceValue() */
        virtual QVariant getMeasurementDeviceValue(unsigned int measuremenDevice, unsigned int value) ;
        /** \copydoc QFExtensionMeasurementAndControlDevice::getMeasurementDeviceValueName() */
        virtual QString getMeasurementDeviceValueName(unsigned int measuremenDevice, unsigned int value) ;
        /** \copydoc QFExtensionMeasurementAndControlDevice::getMeasurementDeviceValueShortName() */
        virtual QString getMeasurementDeviceValueShortName(unsigned int measuremenDevice, unsigned int value) ;
        /** \copydoc QFExtensionMeasurementAndControlDevice::isMeasurementDeviceValueEditable() */
        virtual bool isMeasurementDeviceValueEditable(unsigned int measuremenDevice, unsigned int value) ;
        /** \copydoc QFExtensionMeasurementAndControlDevice::setMeasurementDeviceValue() */
        virtual void setMeasurementDeviceValue(unsigned int measuremenDevice, unsigned int value, const QVariant& data);
        /** \copydoc QFExtensionMeasurementAndControlDevice::getMeasurementDeviceEditableValueType() */
        virtual QVariant::Type getMeasurementDeviceEditableValueType(unsigned int measuremenDevice, unsigned int value) ;
        /** \copydoc QFExtensionMeasurementDevice::getMeasurementDeviceValueWidget() */
        virtual WidgetTypes getMeasurementDeviceValueWidget(unsigned int measuremenDevice, unsigned int value, QStringList* comboboxEntries=NULL) ;
        /** \copydoc QFExtensionMeasurementDevice::getMeasurementDeviceEditableValueRange() */
        virtual void getMeasurementDeviceEditableValueRange(unsigned int measuremenDevice, unsigned int value, double& minimum, double& maximum) ;

	protected:
        QFPluginLogService* logService;
        QF3ComPortManager ports;




        struct DEVICEINFO {
            QF3SimpleB040SerialProtocolHandler* serial;
            int port;
            QString infoMessage;
            QTime lastAction;
            QString label;
        };

        QList<DEVICEINFO> devices;

};

#endif // meas_b040resheater_H
