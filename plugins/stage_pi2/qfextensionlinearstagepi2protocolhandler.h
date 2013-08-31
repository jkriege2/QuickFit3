#ifndef QFEXTENSIONLINEARSTAGEPI2PROTOCOLHANDLER_H
#define QFEXTENSIONLINEARSTAGEPI2PROTOCOLHANDLER_H

#include "jkserialconnection.h"
#include <QString>
#include "qfpluginservices.h"
#include <QMutex>
#include <QMutexLocker>
#include "qfextensionlinearstage.h"

/*! \brief tool class to handle the PI Mercury serial connection protocoll
    \ingroup qf3ext_StagePI2

    This protocol is used by most of the hardware devices developed for the SPIM in group B040. It has only few rules:
      -# There are commands without and with return value.
      -# senCommand() transmits the given command (+ a line feed \c 0x0A character). This function  returns immediatly and does not wait for an answer
      -# queryCommand() queries information from the device by first transmitting the query (including a trailing line feed \c 0x0A character) and
         then waiting for an answer which has to be finished by two consecutive line feed \c 0x0A characters (end of message sequence). The function
         also returns after a timeout (this error is reported by hasErrorOccured() afterwards) with possibly a fraction of the query answer, received so far.
*/
class QFExtensionLinearStagePI2ProtocolHandler {
    public:
        QFExtensionLinearStagePI2ProtocolHandler(JKSerialConnection* com, QMutex* mutexSerial, QString name);

        bool hasErrorOccured();
        QString getLastError();
        void setLogging(QFPluginLogService *log, QString LOG_PREFIX);


        bool checkComConnected();

        void selectAxis(int i);
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
        QMutex* mutexSerial;
};

#endif // QFEXTENSIONLINEARSTAGEPI2PROTOCOLHANDLER_H
