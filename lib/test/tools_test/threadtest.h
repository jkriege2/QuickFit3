#ifndef THREADTEST_H
#define THREADTEST_H
#include <QMutex>
#include <QReadWriteLock>
#include <QThread>
#include <QDebug>
#include <QElapsedTimer>

class QReadLockThread: public QThread {
        Q_OBJECT
    protected:
        QReadWriteLock* lock;
    public:
        inline QReadLockThread(QReadWriteLock* lock, QObject* parent): QThread(parent) {
            this->lock=lock;
        }
    protected:
        inline virtual void run() {
            qDebug()<<"thread "<<QThread::currentThread()<<": started";
            lock->lockForRead();
            qDebug()<<"thread "<<QThread::currentThread()<<": locked read 1";
            lock->lockForRead();
            qDebug()<<"thread "<<QThread::currentThread()<<": locked read 2";
            QElapsedTimer time;
            time.start();
            while (time.elapsed()<500);
            lock->unlock();
            qDebug()<<"thread "<<QThread::currentThread()<<": unlocked read 2";
            lock->unlock();
            qDebug()<<"thread "<<QThread::currentThread()<<": unlocked read 1";

            qDebug()<<"thread "<<QThread::currentThread()<<": done";
        }

};

class QWriteLockThread: public QThread {
        Q_OBJECT
    protected:
        QReadWriteLock* lock;
    public:
        inline QWriteLockThread(QReadWriteLock* lock, QObject* parent): QThread(parent) {
            this->lock=lock;
        }
    protected:
        inline virtual void run() {
            qDebug()<<"thread "<<QThread::currentThread()<<": started";
            lock->lockForWrite();
            qDebug()<<"thread "<<QThread::currentThread()<<": locked write 1";
            lock->lockForWrite();
            qDebug()<<"thread "<<QThread::currentThread()<<": locked write 2";
            QElapsedTimer time;
            time.start();
            while (time.elapsed()<500);
            lock->unlock();
            qDebug()<<"thread "<<QThread::currentThread()<<": unlocked write 2";
            lock->unlock();
            qDebug()<<"thread "<<QThread::currentThread()<<": unlocked write 1";

            qDebug()<<"thread "<<QThread::currentThread()<<": done";
        }

};

#endif // THREADTEST_H
