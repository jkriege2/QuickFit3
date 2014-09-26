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

#ifndef QFBinarySerialProtocolHandler_H
#define QFBinarySerialProtocolHandler_H
#include "jkserialconnection.h"
#include <QString>
#include "qfpluginservices.h"

class QFBinarySerialProtocolHandler {
    public:
        /** \brief class constructor
         *
         *  \param com com port to use
         *  \param name name of the device (for error messages)
         */
        QFBinarySerialProtocolHandler(JKSerialConnection* com, QString name);

        void setLogging(QFPluginLogService* log, QString LOG_PREFIX);

        bool checkComConnected();

        /** \brief send a command to the Mercury controller (this automatically adds a command terminating character (line feed \c 0x0A) */
        void sendCommand(const QByteArray& command);

        /** \brief send a command to the Mercury controller (this automatically adds a command terminating character (line feed \c 0x0A)
         *         and returns the result (the standard finishing sequence CR LF ETX will be cut from the string) */
        QByteArray queryCommand(const QByteArray& command, int resultSize);
        QByteArray queryCommand(const QByteArray& command, const QByteArray& endMessage, int resultMaxSize);

        void checkComError();

        bool hasErrorOccured();
        QString getLastError();
    protected:
        JKSerialConnection* com;
        QFPluginLogService* log;
        QString LOG_PREFIX;
        QString name;

};

#endif // QFBinarySerialProtocolHandler_H

