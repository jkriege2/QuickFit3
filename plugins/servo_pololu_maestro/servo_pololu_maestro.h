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

#ifndef servo_pololu_maestro_H
#define servo_pololu_maestro_H

#include <time.h>
#include <QObject>
#include <QVector>
#include <QList>
#include "qfextension.h"
#include "qfextensionshutter.h"
#include "../base_classes/qfbinaryserialprotocolhandler.h"
#include "../base_classes/qf3comportmanager.h"
#include "../../../../../LIB/trunk/jkserialconnection.h"
#include "pololu_tools.h"
#include "qfextensionfilterchanger.h"
/*!
    \defgroup qf3ext_ShutterArduinoB040 QFExtensionShutter implementation for RC-servos switchable between two states using a custom C program running on an Arduino Nano 3.0 board
    \ingroup qf3extensionplugins
*/

/*! \brief QFExtensionShutter implementation for RC-servos switchable between two states using a custom C program running on an Arduino Nano 3.0 board
    \ingroup qf3ext_ShutterArduinoB040
 */
class QFExtensionServoPololuMaestro : public QObject, public QFExtensionBase, public QFExtensionShutter, public QFPluginLogService, public QFExtensionFilterChanger {
        Q_OBJECT
        Q_INTERFACES(QFExtension QFExtensionShutter QFPluginLogService QFExtensionFilterChanger)
    public:
        /** Default constructor */
        QFExtensionServoPololuMaestro(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFExtensionServoPololuMaestro();


    /////////////////////////////////////////////////////////////////////////////
    // QFExtension routines
    /////////////////////////////////////////////////////////////////////////////
        /** \copydoc QFExtension::getID() */
        virtual QString getID() const  { return QString("servo_pololu_maestro"); }
        /** \copydoc QFExtension::getName() */
        virtual QString getName() const  { return tr("Pololu Maestro Servo Controller"); }
        /** \copydoc QFExtension::getDescription() */
        virtual QString getDescription() const  { return tr("control shutters and filterchangers attatched to servos controlled by Pololu Maestro Servo Controllers"); }
        /** \copydoc QFExtension::getAuthor() */
        virtual QString getAuthor() const  { return tr("Jan W. Krieger"); }
        /** \copydoc QFExtension::getCopyright() */
        virtual QString getCopyright() const  { return tr("(c) 2013 by Jan W. Krieger"); }
        /** \copydoc QFExtension::getWeblink() */
        virtual QString getWeblink() const  { return tr(""); }
        /** \copydoc QFExtension::getIconFilename() */
        virtual QString getIconFilename() const  { return QString(":/servo_pololu_maestro.png"); }
        /** \copydoc QFExtension::deinit() */
        virtual void deinit();
        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=2;
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













        /** \copydoc QFExtensionFilterChanger::getFilterChangerCount() */
        virtual unsigned int getFilterChangerCount()  ;
        /** \copydoc QFExtensionFilterChanger::filterChangerConnect() */
        virtual void filterChangerConnect(unsigned int filterChanger);
        /** \copydoc QFExtensionFilterChanger::filterChangerDisonnect() */
        virtual void filterChangerDisonnect(unsigned int filterChanger);
        /** \copydoc QFExtensionFilterChanger::setFilterChangerLogging() */
        virtual void setFilterChangerLogging(QFPluginLogService* logService);

        /** \copydoc QFExtensionFilterChanger::isFilterChangerConnected() */
        virtual bool isFilterChangerConnected(unsigned int filterChanger) ;

        /** \copydoc QFExtensionFilterChanger::getFilterChangerFilterCount() */
        virtual unsigned int getFilterChangerFilterCount(unsigned int filterChanger) ;

        /** \copydoc QFExtensionFilterChanger::setFilterChangerFilter() */
        virtual void setFilterChangerFilter(unsigned int filterChanger, unsigned int filter) ;

        /** \copydoc QFExtensionFilterChanger::getFilterChangerCurrentFilter() */
        virtual unsigned int getFilterChangerCurrentFilter(unsigned int filterChanger) ;

        /** \copydoc QFExtensionFilterChanger::isLastFilterChangerActionFinished() */
        virtual bool isLastFilterChangerActionFinished(unsigned int filterChanger);
        /** \copydoc QFExtensionFilterChanger::getFilterChangerDescription() */
        virtual QString getFilterChangerDescription(unsigned int filterChanger) ;
        /** \copydoc QFExtensionFilterChanger::getFilterChangerShortName() */
        virtual QString getFilterChangerShortName(unsigned int filterChanger) ;
        /** \copydoc QFExtensionFilterChanger::showFilterChangerSettingsDialog() */
        virtual void showFilterChangerSettingsDialog(unsigned int filterChanger, QWidget* parent=NULL);


	protected:
        QFPluginLogService* logService;
        QF3ComPortManager ports;




        struct SHUTTER {
            QFBinarySerialProtocolHandler* serial;
            int port;
            int id;
            /** \brief duration of a shutter operation in milliseconds */
            int operation_duration;
            QString infoMessage;
            QTime lastAction;
            QString label;
            QString description;
            bool currentState;
            int on_target;
            int off_target;
        };

        QList<SHUTTER> shutters;

        struct FILTERWHEEL {
            QFBinarySerialProtocolHandler* serial;
            int port;
            int id;
            /** \brief duration of a shutter operation in milliseconds */
            int operation_duration;
            QString infoMessage;
            QTime lastAction;
            QString label;
            QString description;
            QList<int> targets;
            int currentFilter;
        };

        QList<FILTERWHEEL> wheels;

        QMap<int, QPair<int, int> > deviceIndex;

    protected slots:
        void showTester();

};

#endif // servo_pololu_maestro_H
