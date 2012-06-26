#include "qfimfcsfitthread.h"
#include "qfimfcsfitevaluation.h"

QFImFCSFitThread::QFImFCSFitThread(bool stopWhenEmpty, QObject *parent) :
    QThread(parent)
{
    lock=new QReadWriteLock();
    stopped=false;
    this->stopWhenEmpty=stopWhenEmpty;
    jobCount=0;
    jobsDone=0;

}

QFImFCSFitThread::~QFImFCSFitThread()
{
    delete lock;
}

void QFImFCSFitThread::run() {
    bool jempty=false;
    { QReadLocker locker(lock);
        jempty=jobs.isEmpty();
    }
    while(!stopped  && (!(stopWhenEmpty&&jempty))) {
        QFImFCSFitThread::Job job;
        bool jobIsValid=false;
        // look into input queue, whether there are still tasks to perform
        {   QReadLocker locker(lock);
            if (!jobs.isEmpty()) {
                job=jobs.dequeue();
                jobIsValid= (job.record && job.evaluation);
            }
            jempty=jobs.isEmpty();
        }

        // if we found a valid job: perform it
        if (jobIsValid) {
            job.evaluation->doFitForMultithread(job.record, job.run, job.userMin, job.userMax, this);

            {
                QWriteLocker locker(lock);
                jobsDone++;
            }
        }
    }
}

void QFImFCSFitThread::addJob(QFImFCSFitEvaluation *evaluation, QFRawDataRecord *record, int run, int userMin, int userMax) {
    QFImFCSFitThread::Job j;
    j.evaluation=evaluation;
    j.record=record;
    j.run=run;
    j.userMax=userMax;
    j.userMin=userMin;
    QWriteLocker locker(lock);
    jobs.enqueue(j);
    jobCount++;
}

void QFImFCSFitThread::cancel(bool waitForFinished) {
    stopped=true;
    while (waitForFinished && isRunning()) {
        QApplication::processEvents();
    }
}

int QFImFCSFitThread::getJobsDone()
{
    QReadLocker locker(lock);
    return jobsDone;
}


QFImFCSFitThread::Job::Job() {
    evaluation=NULL;
    record=NULL;
    run=-1;
    userMin=0;
    userMax=0;
}

int QFImFCSFitThread::getJobCount()
{
    return jobCount;
}

void QFImFCSFitThread::log_text(QString message)
{
    emit sigLogText(message);
}

void QFImFCSFitThread::log_error(QString message)
{
    emit sigLogError(message);
}

void QFImFCSFitThread::log_warning(QString message)
{
    emit sigLogWarning(message);
}
