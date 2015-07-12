/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
