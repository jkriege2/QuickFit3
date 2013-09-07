#ifndef meas_b040resheater_H
#define meas_b040resheater_H

#include <time.h>
#include <QObject>
#include <QVector>
#include <QList>
#include "qfextension.h"
#include "../interfaces/qfextensionmeasurementdevice.h"
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
        virtual unsigned int getMeasurementDeviceCount();

        /** \copydoc QFExtensionMeasurementAndControlDevice::showMeasurementDeviceSettingsDialog() */
        virtual void showMeasurementDeviceSettingsDialog(unsigned int measuremenDevice, QWidget* parent=NULL);
        /** \copydoc QFExtensionMeasurementAndControlDevice::isMeasurementDeviceConnected() */
         virtual bool isMeasurementDeviceConnected(unsigned int measuremenDevice);
        /** \copydoc QFExtensionMeasurementAndControlDevice::connectMeasurementDevice() */
         virtual void connectMeasurementDevice(unsigned int measuremenDevice);
        /** \copydoc QFExtensionMeasurementAndControlDevice::disconnectMeasurementDevice() */
         virtual void disconnectMeasurementDevice(unsigned int measuremenDevice);
        /** \copydoc QFExtensionMeasurementAndControlDevice::setMeasurementDeviceLogging() */
         virtual void setMeasurementDeviceLogging(QFPluginLogService* logService);
        /** \copydoc QFExtensionMeasurementAndControlDevice::getMeasurementDeviceValueCount() */
        virtual unsigned int getMeasurementDeviceValueCount(unsigned int measuremenDevice);
        /** \copydoc QFExtensionMeasurementAndControlDevice::getMeasurementDeviceValue() */
        virtual QVariant getMeasurementDeviceValue(unsigned int measuremenDevice, unsigned int value);
        /** \copydoc QFExtensionMeasurementAndControlDevice::getMeasurementDeviceValueName() */
        virtual QString getMeasurementDeviceValueName(unsigned int measuremenDevice, unsigned int value);
        /** \copydoc QFExtensionMeasurementAndControlDevice::getMeasurementDeviceValueShortName() */
        virtual QString getMeasurementDeviceValueShortName(unsigned int measuremenDevice, unsigned int value);
        /** \copydoc QFExtensionMeasurementAndControlDevice::isMeasurementDeviceValueEditable() */
        virtual bool isMeasurementDeviceValueEditable(unsigned int measuremenDevice, unsigned int value);
        /** \copydoc QFExtensionMeasurementAndControlDevice::setMeasurementDeviceValue() */
        virtual void setMeasurementDeviceValue(unsigned int measuremenDevice, unsigned int value, const QVariant& data);
        /** \copydoc QFExtensionMeasurementAndControlDevice::getMeasurementDeviceEditableValueType() */
        virtual QVariant::Type getMeasurementDeviceEditableValueType(unsigned int measuremenDevice, unsigned int value);

	protected:
        QFPluginLogService* logService;
        QF3ComPortManager ports;




        struct DEVICEINFO {
            QF3SimpleB040SerialProtocolHandler* serial;
            int port;
            QString infoMessage;
            QTime lastAction;
        };

        QList<DEVICEINFO> devices;

};

#endif // meas_b040resheater_H
