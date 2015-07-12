/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    

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

#ifndef SHUTTER_RELAIS_ARDUINO_H
#define SHUTTER_RELAIS_ARDUINO_H

#include <time.h>
#include <QObject>
#include <QVector>
#include <QList>
#include "qfextension.h"
#include "qfextensionshutter.h"
#include "../base_classes/qf3simpleb040serialprotocolhandler.h"
#include "qf3comportmanager.h"
#include "qfserialconnection.h"

/*!
    \defgroup qf3ext_ShutterArduinoB040 QFExtensionShutter implementation for Relais converted to shutters and controlled by an Arduino UNO
    \ingroup qf3extensionplugins
*/

/*! \brief QFExtensionShutter implementation for Relais converted to shutters and controlled by an Arduino UNO
    \ingroup qf3ext_ShutterArduinoB040
 */
class QFExtensionShutterShuterArduino : public QObject, public QFExtensionBase, public QFExtensionShutter, public QFPluginLogService {
        Q_OBJECT
        Q_INTERFACES(QFExtension QFExtensionShutter QFPluginLogService)
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        Q_PLUGIN_METADATA(IID "www.dkfz.de.QuickFit3.Plugins.QFExtensionShutterShuterArduino")
#endif

    public:
        /** Default constructor */
        QFExtensionShutterShuterArduino(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFExtensionShutterShuterArduino();


    /////////////////////////////////////////////////////////////////////////////
    // QFExtension routines
    /////////////////////////////////////////////////////////////////////////////
        /** \copydoc QFExtension::getID() */
        virtual QString getID() const  { return QString("shutter_relais_arduino"); }
        /** \copydoc QFExtension::getName() */
        virtual QString getName() const  { return tr("Arduino Relais Shutter (B040)"); }
        /** \copydoc QFExtension::getDescription() */
        virtual QString getDescription() const  { return tr("Relais Shutter Driver for Arduino (B040)"); }
        /** \copydoc QFExtension::getAuthor() */
        virtual QString getAuthor() const  { return tr("Jan W. Krieger"); }
        /** \copydoc QFExtension::getCopyright() */
        virtual QString getCopyright() const  { return tr("(c) 2013 by Jan W. Krieger"); }
        /** \copydoc QFExtension::getWeblink() */
        virtual QString getWeblink() const  { return tr(""); }
        /** \copydoc QFExtension::getIconFilename() */
        virtual QString getIconFilename() const  { return QString(":/shutter_relais_arduino.png"); }
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
            QString label;
            QString description;
            QTime lastAction;
        };

        QList<SHUTTER> shutters;

};

#endif // SHUTTER_RELAIS_ARDUINO_H
