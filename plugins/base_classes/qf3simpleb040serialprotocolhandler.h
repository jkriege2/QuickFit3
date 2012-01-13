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
    private:
        JKSerialConnection* com;
        QFPluginLogService* log;
        QString LOG_PREFIX;
        QString name;
};

#endif // QF3SIMPLEB040SERIALPROTOCOLHANDLER_H
