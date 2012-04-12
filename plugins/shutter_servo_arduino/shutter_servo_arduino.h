#ifndef SHUTTER_SERVO_ARDUINO_H
#define SHUTTER_SERVO_ARDUINO_H

#include <time.h>
#include <QObject>
#include <QVector>
#include <QList>
#include "qfextension.h"
#include "../interfaces/qfextensionshutter.h"
#include "../base_classes/qf3simpleb040serialprotocolhandler.h"
#include "../base_classes/qf3comportmanager.h"
#include "../../../../../LIB/trunk/jkserialconnection.h"

/*!
    \defgroup qf3ext_ShutterArduinoB040 QFExtensionShutter implementation for RC-servos switchable between two states using a custom C program running on an Arduino Nano 3.0 board
    \ingroup qf3extensionplugins
*/

/*! \brief QFExtensionShutter implementation for RC-servos switchable between two states using a custom C program running on an Arduino Nano 3.0 board
    \ingroup qf3ext_ShutterArduinoB040
 */
class QFExtensionShutterServoArduino : public QObject, public QFExtensionBase, public QFExtensionShutter, public QFPluginLogService {
        Q_OBJECT
        Q_INTERFACES(QFExtension QFExtensionShutter )
    public:
        /** Default constructor */
        QFExtensionShutterServoArduino(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFExtensionShutterServoArduino();


    /////////////////////////////////////////////////////////////////////////////
    // QFExtension routines
    /////////////////////////////////////////////////////////////////////////////
        /** \copydoc QFExtension::getID() */
        virtual QString getID() const  { return QString("shutter_servo_arduino"); }
        /** \copydoc QFExtension::getName() */
        virtual QString getName() const  { return tr("Arduino Shutter (B040)"); }
        /** \copydoc QFExtension::getDescription() */
        virtual QString getDescription() const  { return tr("Shutter Driver for Arduino (B040)"); }
        /** \copydoc QFExtension::getAuthor() */
        virtual QString getAuthor() const  { return tr("Jan Krieger"); }
        /** \copydoc QFExtension::getCopyright() */
        virtual QString getCopyright() const  { return tr("(c) 2011 by Jan Krieger"); }
        /** \copydoc QFExtension::getWeblink() */
        virtual QString getWeblink() const  { return tr(""); }
        /** \copydoc QFExtension::getIconFilename() */
        virtual QString getIconFilename() const  { return QString(":/shutter_servo_arduino.png"); }
        /** \copydoc QFExtension::deinit() */
        virtual void deinit();
        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=1;
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
        // QFExtensionShutter routines
        /////////////////////////////////////////////////////////////////////////////
        /** \copydoc QFExtensionShutter::getShutterCount() */
        virtual unsigned int getShutterCount()  ;
        /** \copydoc QFExtensionShutter::shutterConnect() */
        virtual void shutterConnect(unsigned int shutter);
        /** \copydoc QFExtensionShutter::shutterDisonnect() */
        virtual void shutterDisonnect(unsigned int shutter);
        /** \copydoc QFExtensionShutter::isShutterConnected() */
        virtual bool isShutterConnected(unsigned int shutter) ;
        /** \copydoc QFExtensionShutter::isShutterOpen() */
        virtual bool isShutterOpen(unsigned int shutter) ;
        /** \copydoc QFExtensionShutter::setShutterState() */
        virtual void setShutterState(unsigned int shutter, bool opened);
        /** \copydoc QFExtensionShutter::isLastShutterActionFinished() */
        virtual bool isLastShutterActionFinished(unsigned int shutter);
        /** \copydoc QFExtensionShutter::getShutterDescription() */
        virtual QString getShutterDescription(unsigned int shutter) ;
        /** \copydoc QFExtensionShutter::getShutterShortName() */
        virtual QString getShutterShortName(unsigned int shutter) ;
        /** \copydoc QFExtensionShutter::showShutterSettingsDialog() */
        virtual void showShutterSettingsDialog(unsigned int shutter, QWidget* parent=NULL);
        /** \copydoc QFExtensionShutter::setShutterLogging() */
        virtual void setShutterLogging(QFPluginLogService* logService);

	protected:
        QFPluginLogService* logService;
        QF3ComPortManager ports;




        struct SHUTTER {
            QF3SimpleB040SerialProtocolHandler* serial;
            int port;
            int id;
            /** \brief duration of a shutter operation in milliseconds */
            int shutter_operation_duration;
            QString infoMessage;
            QTime lastAction;
        };

        QList<SHUTTER> shutters;

};

#endif // SHUTTER_SERVO_ARDUINO_H
