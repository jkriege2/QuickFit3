/*
Copyright (c) 2012-2015 by Sebastian Isbaner

    last modification: $LastChangedDate$  (revision $Rev$)

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

#include "alexevallog.h"
#ifdef QT_DEBUG
    #include <QDebug>
#endif
AlexEvalLog* AlexEvalLog::instance=NULL;

AlexEvalLog::AlexEvalLog()
{
//    ts.setString(&buffer,QIODevice::ReadWrite);
    if (!instance) instance=this;
}

void AlexEvalLog::error(const QString &message)
{
#ifdef QT_DEBUG
    qDebug()<<message;
#endif
    if (instance) instance->intLogError(message);
}

void AlexEvalLog::warning(const QString &message)
{
#ifdef QT_DEBUG
    qDebug()<<message;
#endif
    if (instance) instance->intLogWarning(message);
}

void AlexEvalLog::text(const QString &message)
{
#ifdef QT_DEBUG
    qDebug()<<message;
#endif
    if (instance) instance->intLogText(message);
}


AlexEvalLogStream::AlexEvalLogStream(AlexEvalLog *instance)
{
    this->instance=instance;
}

AlexEvalLogStream::~AlexEvalLogStream()
{
    instance->text(buffer);
}


