/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

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

#ifndef QFEXTENSIONSHUTTER_H
#define QFEXTENSIONSHUTTER_H


#include <QtPlugin>
#include <QWidget>
#include <QSettings>
#include "qfpluginservices.h"
#include <stdint.h>



/*! \brief QuickFit QFExtension to control shutter devices
    \ingroup qf3extensionplugins
    
    \note <b>Your implementation has to be thread-safe!!! So it can be called simultaneously from multiple threads, even when the invocations use shared data, because all references to the shared data are serialized</b>


 */
class QFExtensionShutter {
    public:
        virtual ~QFExtensionShutter() {}

        /** \brief returns the number of shutters controlable by this plugin */
        virtual unsigned int  getShutterCount()  =0;
        /** \brief connect to a shutter controller/driver/.... */
        virtual void shutterConnect(unsigned int shutter)=0;
        /** \brief disconnect from a shutter controller/driver/.... */
        virtual void shutterDisonnect(unsigned int shutter)=0;
        /** \brief set QFPluginLogServices to use (or \c NULL) for message/error logging */
        virtual void setShutterLogging(QFPluginLogService* logService)=0;

        /** \brief returns \c true if the device is connected */
        virtual bool isShutterConnected(unsigned int shutter) =0;
        /** \brief read the shutter state and return it.
         *
         *  \note that this might return wrong values during shutter movement, see: isLastShutterActionFinished()
         *  \see isLastShutterActionFinished()
         */
        virtual bool isShutterOpen(unsigned int shutter) =0;
        /** \brief open or close the given shutter, use isLastShutterActionFinished() to check whether the instruction has been executed */
        virtual void setShutterState(unsigned int shutter, bool opened)=0;
        /** \brief return \c true if the last command, sent to the given shutter was executet, i.e. the shutter has settled to its new position
         *
         *  \note Use this to check, whether the shutter has been closed/opened after a call to setShutterState(), not isShutterOpen(), as the second
         *        one might not be defined (and return a wrong value) during the shutter movement!
         */
        virtual bool isLastShutterActionFinished(unsigned int shutter)=0;
        /** \brief return a human-readable description for the given shutter */
        virtual QString getShutterDescription(unsigned int shutter) =0;
        /** \brief return a human-readable short name for the given shutter */
        virtual QString getShutterShortName(unsigned int shutter) =0;
        /*! \brief displays a modal dialog which allows the user to set the configuration options
                   of the controled shutter.

            The options are stored internally and written/read using QFExtension::readSettings() and
            QFExtension::writeSettings().

            \param[in] shutter the shutter for which to display the dialog
            \param[in] parent parent widget for the returned QWidget
         */
         virtual void showShutterSettingsDialog(unsigned int shutter, QWidget* parent=NULL)=0;
};



Q_DECLARE_INTERFACE( QFExtensionShutter,
                     "www.dkfz.de.b040.quickfit3.extensions.QFExtensionShutter/1.0")

#endif // QFEXTENSIONSHUTTER_H
