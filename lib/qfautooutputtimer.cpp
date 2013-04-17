#include "qfautooutputtimer.h"
#include <QDebug>
#include <QtGlobal>

QFAutoOutputTimer::QFAutoOutputTimer(const QString& message) :
    QElapsedTimer()
{
    this->message=message;
    start();
}

QFAutoOutputTimer::~QFAutoOutputTimer()
{
    #if QT_VERSION >= 0x040800
        qDebug()<<"TIMERMESSAGE: "<<message<<"    DUR: "<<double(nsecsElapsed())/1.0e6<<"ms";
    #else
        qDebug()<<"TIMERMESSAGE: "<<message<<"    DUR: "<<double(elapsed())/1.0e3<<"ms";
    #endif
}
