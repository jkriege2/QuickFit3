#include "qfcoboltlaserprotocolhandler.h"

#include <QObject>
#include <QTime>
#include <QDebug>

QFCoboltLaserProtocolHandler::QFCoboltLaserProtocolHandler(JKSerialConnection *com, QString name):
    QFSimpleSerialProtocolHandler(com, name, QString("\r"), QString("\n"))
{
}
