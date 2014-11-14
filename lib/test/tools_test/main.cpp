#include "../../qftools.h"
#include <cmath>
#include <QTextStream>
#include <QElapsedTimer>
#include <iostream>
#include <QStringList>
#include "../../qfpreprocessormagic.h"
#include <QDebug>
#include <QMutex>
#include <QReadWriteLock>
#include <QThread>

#include "threadtest.h"

int main(int argc, char *argv[])
{

    QReadWriteLock* lock=new QReadWriteLock(QReadWriteLock::Recursive);
    QReadLockThread* readThread=new QReadLockThread(lock, NULL);
    qDebug()<<"thread "<<QThread::currentThread()<<": before";
    lock->lockForRead();
    readThread->start();

    QElapsedTimer time;
    time.start();
    while (time.elapsed()<1000) ;

    qDebug()<<"thread "<<QThread::currentThread()<<": locked read 1";
    lock->lockForRead();
    readThread->start();

    qDebug()<<"thread "<<QThread::currentThread()<<": locked read 2";
    lock->unlock();
    qDebug()<<"thread "<<QThread::currentThread()<<": unlocked read 2";
    lock->unlock();
    qDebug()<<"thread "<<QThread::currentThread()<<": unlocked read 1";

    qDebug()<<"thread "<<QThread::currentThread()<<": done";

    time.start();
    while (time.elapsed()<5000) ;
    QWriteLockThread* writeThread=new QWriteLockThread(lock, NULL);
    qDebug()<<"\n\n\n\nthread "<<QThread::currentThread()<<": before";
    lock->lockForWrite();
    writeThread->start();

    time.start();
    while (time.elapsed()<1000) ;

    qDebug()<<"thread "<<QThread::currentThread()<<": locked write 1";
    lock->lockForWrite();
    writeThread->start();

    qDebug()<<"thread "<<QThread::currentThread()<<": locked write 2";
    lock->unlock();
    qDebug()<<"thread "<<QThread::currentThread()<<": unlocked write 2";
    lock->unlock();
    qDebug()<<"thread "<<QThread::currentThread()<<": unlocked write 1";

    qDebug()<<"thread "<<QThread::currentThread()<<": done";


    time.start();
    while (time.elapsed()<5000) ;

    QStringList test1, test1res;
    test1<<"123"<<"abc"<<"xyz"<<"XyZ";
    qDebug()<<"escapify: "<<escapify(test1)<<"\n";
    test1res=deescapifyList(escapify(test1));
    qDebug()<<"deescapifyList(escapify): "<<((test1==test1res)?"OK":"COMPARE ERROR")<<" : "<<test1res.join(";;")<<"\n";



    test1.clear();
    test1<<"1\n2\n3"<<"a\rb\tc"<<"x\"yz"<<"X\'yZ"<<"X\'y\'Z"<<"X\"yZ\"";
    qDebug()<<"escapify: "<<escapify(test1)<<"\n";
    test1res=deescapifyList(escapify(test1));
    qDebug()<<"deescapifyList(escapify): "<<((test1==test1res)?"OK":"COMPARE ERROR")<<" : "<<test1res.join(";;")<<"\n";



    qDebug()<<"PREPROCESSOR MAGIC:\n";
    #define PARAMCNT 0
    qDebug()<<PARAMCNT;
    #define PARAMCNT1 QFPPM_INC(PARAMCNT)
    qDebug()<<PARAMCNT1;
    #define PARAMCNT2 QFPPM_INC(PARAMCNT1)
    qDebug()<<PARAMCNT2;


    //QDir d("../../");
    //qDebug()<<qfDirListFilesRecursive(d).join("\n");

    return 0;
}
