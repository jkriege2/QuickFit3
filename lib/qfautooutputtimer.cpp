#include "qfautooutputtimer.h"
#include <QDebug>

QFAutoOutputTimer::QFAutoOutputTimer(const QString& message) :
    QElapsedTimer()
{
    this->message=message;
    start();
}

QFAutoOutputTimer::~QFAutoOutputTimer()
{
    qDebug()<<"TIMERMESSAGE: "<<message<<"    DUR: "<<double(nsecsElapsed())/1.0e6<<"ms";
}
