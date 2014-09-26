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

#ifndef QF3SIMPLEB040SERIALPROTOCOLHANDLER_H
#define QF3SIMPLEB040SERIALPROTOCOLHANDLER_H
#include "jkserialconnection.h"
#include <QString>
#include "qfpluginservices.h"

/*! \brief tool class to handle a simple serial connection protocoll
    \ingroup qf3extensionplugins

    This protocol is used by most of the hardware devices developed for the SPIM in group B040. It has only few rules:
      -# There are commands without and with return value.
      -# senCommand() transmits the given command (+ a line feed \c 0x0A character). This function  returns immediatly and does not wait for an answer
      -# queryCommand() queries information from the device by first transmitting the query (including a trailing line feed \c 0x0A character) and
         then waiting for an answer which has to be finished by two consecutive line feed \c 0x0A characters (end of message sequence). The function
         also returns after a timeout (this error is reported by hasErrorOccured() afterwards) with possibly a fraction of the query answer, received so far.
*/
class QF3SimpleB040SerialProtocolHandler {
    public:
        /** \brief class constructor
         *
         *  \param com com port to use
         *  \param name name of the device (for error messages)
         */
        QF3SimpleB040SerialProtocolHandler(JKSerialConnection* com, QString name);

        void setLogging(QFPluginLogService* log, QString LOG_PREFIX);

        bool checkComConnected();

        /** \brief send a command to the Mercury controller (this automatically adds a command terminating character (line feed \c 0x0A) */
        void sendCommand(QString command);

        /** \brief send a command to the Mercury controller (this automatically adds a command terminating character (line feed \c 0x0A)
         *         and returns the result (the standard finishing sequence CR LF ETX will be cut from the string) */
        QString queryCommand(QString command);

        void checkComError();

        bool hasErrorOccured();
        QString getLastError();
        void setAddToCommand(const QString& add);
    private:
        JKSerialConnection* com;
        QFPluginLogService* log;
        QString LOG_PREFIX;
        QString name;
        QString addToCommand;
};

#endif // QF3SIMPLEB040SERIALPROTOCOLHANDLER_H
