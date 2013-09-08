#include "pololu_tools.h"


QByteArray pololuMaestroBuildGetPosCommand(int channel)
{
    QByteArray command(2, 0);
    command[0]=0x90;
    command[1]=channel;
    return command;
}


QByteArray pololuMaestroBuildSetPosCommand(int channel, int pos_microseconds)
{
    int target=pos_microseconds*4;
    QByteArray command(4, 0);
    command[0]=0x84;
    command[1]=channel;
    command[2]=target & 0x7F;
    command[3]=target >> 7 & 0x7F;
    return command;
}


int pololuMaestroPosToMicroseconds(const QByteArray &pos)
{
    return (pos[0]+pos[1]*0xFF)/4;
}
