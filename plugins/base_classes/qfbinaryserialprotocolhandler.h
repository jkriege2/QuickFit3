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

