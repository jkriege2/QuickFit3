#ifndef POLOLU_TOOLS_H
#define POLOLU_TOOLS_H

#include <QByteArray>

QByteArray pololuMaestroBuildGetPosCommand(int channel);
QByteArray pololuMaestroBuildSetPosCommand(int channel, int pos_microseconds);
QByteArray pololuMaestroBuildSetSpeedCommand(int channel, int speed);
QByteArray pololuMaestroBuildGetSpeedCommand(int channel);
int pololuMaestroPosToMicroseconds(const QByteArray& pos);

#endif // POLOLU_TOOLS_H
