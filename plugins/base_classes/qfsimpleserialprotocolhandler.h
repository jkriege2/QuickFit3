#ifndef QFSIMPLESERIALPROTOCOLHANDLER_H
#define QFSIMPLESERIALPROTOCOLHANDLER_H
#include "jkserialconnection.h"
#include <QString>
#include "qfpluginservices.h"

class QFSimpleSerialProtocolHandler {
    public:
        /** \brief class constructor
         *
         *  \param com com port to use
         *  \param name name of the device (for error messages)
         */
        QFSimpleSerialProtocolHandler(JKSerialConnection* com, QString name, const QString& sendTerminationString=QString("\n"), const QString& receiveReadUntilString=QString("\n"));

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
        void setSendTerminationString(const QString& str);
        void setReceiveUntilString(const QString& str);
    protected:
        JKSerialConnection* com;
        QFPluginLogService* log;
        QString LOG_PREFIX;
        QString name;
        QString sendTerminationString;
        QString receiveReadUntilString;

};

#endif // QFSIMPLESERIALPROTOCOLHANDLER_H

