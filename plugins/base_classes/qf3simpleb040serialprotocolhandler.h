#ifndef QF3SIMPLEB040SERIALPROTOCOLHANDLER_H
#define QF3SIMPLEB040SERIALPROTOCOLHANDLER_H
#include "jkserialconnection.h"
#include <QString>
#include "qfpluginservices.h"

/** \brief tool class to handle a simple serial connection protocoll */
class QF3SimpleB040SerialProtocolHandler {
    public:
        QF3SimpleB040SerialProtocolHandler(JKSerialConnection* com, QString name);

        void setLogging(QFPluginLogService* log, QString LOG_PREFIX);

        bool checkComConnected();

        /** \brief send a command to the Mercury controller (this automatically adds a command terminating character (carriage return) */
        void sendCommand(std::string command);

        /** \brief send a command to the Mercury controller (this automatically adds a command terminating character (carriage return)
         *         and returns the result (the standard finishing sequence CR LF ETX will be cut from the string) */
        std::string queryCommand(std::string command);

        void checkComError();
    private:
        JKSerialConnection* com;
        QFPluginLogService* log;
        QString LOG_PREFIX;
        QString name;
};

#endif // QF3SIMPLEB040SERIALPROTOCOLHANDLER_H
