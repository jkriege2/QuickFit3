/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2014-11-14 18:30:07 +0100 (Fr, 14 Nov 2014) $  (revision $Rev: 3610 $)

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

#ifndef QFTHREADINGTOOLS_H
#define QFTHREADINGTOOLS_H
#include "qftools.h"
#include <QDebug>
#include <QReadLocker>
#include <QWriteLocker>
#include <QReadWriteLock>
#include "lib_imexport.h"
#include <QObject>
#include <QString>
#include <QHash>
#include <QVariant>
#include <QStringList>
#include <QThread>


class QFLIB_EXPORT QFDebugReadLocker: public QReadLocker {
    public:
        inline QFDebugReadLocker(QReadWriteLock *readWriteLock):
            QReadLocker(readWriteLock)
        {

        }

        inline ~QFDebugReadLocker() {
        }

        inline void unlock()
        {
            qDebug()<<"QFDebugReadLocker::unlock() (waiting)    "<<QThread::currentThread();
            QReadLocker::unlock();
            qDebug()<<"QFDebugReadLocker::unlock() (success)    "<<QThread::currentThread();
        }

        inline void relock()
        {
            qDebug()<<"QFDebugReadLocker::unlock() (waiting)    "<<QThread::currentThread();
            QReadLocker::relock();
            qDebug()<<"QFDebugReadLocker::unlock() (success)    "<<QThread::currentThread();
        }


    private:
        Q_DISABLE_COPY(QFDebugReadLocker)
};




class QFLIB_EXPORT QFDebugWriteLocker: public QWriteLocker {
    public:
        inline QFDebugWriteLocker(QReadWriteLock *readWriteLock):
            QWriteLocker(readWriteLock)
        {

        }

        inline ~QFDebugWriteLocker() {
        }

        inline void unlock()
        {
            qDebug()<<"QFDebugWriteLocker::unlock() (waiting)    "<<QThread::currentThread();
            QWriteLocker::unlock();
            qDebug()<<"QFDebugWriteLocker::unlock() (success)    "<<QThread::currentThread();
        }

        inline void relock()
        {
            qDebug()<<"QFDebugWriteLocker::unlock() (waiting)    "<<QThread::currentThread();
            QWriteLocker::relock();
            qDebug()<<"QFDebugWriteLocker::unlock() (success)    "<<QThread::currentThread();
        }


    private:
        Q_DISABLE_COPY(QFDebugWriteLocker)
};

#endif // QFTHREADINGTOOLS_H
