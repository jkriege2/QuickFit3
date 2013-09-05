#ifndef QFCOBOLTLASERPROTOCOLHANDLER_H
#define QFCOBOLTLASERPROTOCOLHANDLER_H
#include "jkserialconnection.h"
#include <QString>
#include "qfpluginservices.h"
#include "qfsimpleserialprotocolhandler.h"

class QFCoboltLaserProtocolHandler: public QFSimpleSerialProtocolHandler
{
public:
    /** \brief class constructor
     *
     *  \param com com port to use
     *  \param name name of the device (for error messages)
     */
    QFCoboltLaserProtocolHandler(JKSerialConnection* com, QString name);
};

#endif // QFCOBOLTLASERPROTOCOLHANDLER_H
