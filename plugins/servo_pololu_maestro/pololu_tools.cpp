#include "pololu_tools.h"


QByteArray pololuMaestroBuildGetPosCommand(int channel)
{
    QByteArray command(2, 0);
    int cnt=0;
    //command[cnt++]=0xAA;
    command[cnt++]=0x90;
    command[cnt++]=channel;
    return command;
}


QByteArray pololuMaestroBuildSetPosCommand(int channel, int pos_microseconds)
{
    int target=pos_microseconds*4;
    int cnt=0;
    QByteArray command(4, 0);
    command[cnt++]=0x84;
    command[cnt++]=channel;
    command[cnt++]=target & 0x7F;
    command[cnt++]=(target >> 7) & 0x7F;

    return command;
}


int pololuMaestroPosToMicroseconds(const QByteArray &pos)
{
    return (pos[0]+pos[1]*0xFF)/4;
}
