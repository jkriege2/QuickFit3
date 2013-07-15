#include "qffitresultsbyindexevaluationfitthread.h"
#include "qffitresultsbyindexevaluation.h"
#include "qffitresultsbyindexevaluationfittools.h"

QFFitResultsByIndexEvaluationFitThread::QFFitResultsByIndexEvaluationFitThread(bool stopWhenEmpty, QObject *parent) :
    QThread(parent)
{
    lock=new QReadWriteLock();
    stopped=false;
    this->stopWhenEmpty=stopWhenEmpty;
    jobCount=0;
    jobsDone=0;

}

QFFitResultsByIndexEvaluationFitThread::~QFFitResultsByIndexEvaluationFitThread()
{
    delete lock;
}

void QFFitResultsByIndexEvaluationFitThread::run() {
    bool jempty=false;
    { QReadLocker locker(lock);
        jempty=jobs.isEmpty();
    }
    while(!stopped  && (!(stopWhenEmpty&&jempty))) {
        QFFitResultsByIndexEvaluationFitThread::Job job;
        bool jobIsValid=false;
        // look into input queue, whether there are still tasks to perform
        {   QReadLocker locker(lock);
            if (!jobs.isEmpty()) {
                job=jobs.dequeue();
                jobIsValid= ((job.record||(!job.record&&job.records.size()>0)) && job.evaluation);
            }
            jempty=jobs.isEmpty();
        }

        // if we found a valid job: perform it
        if (job.record) {
            // if the record property is used, we have a single-record fit and run the according function
            QFFitResultsByIndexEvaluationFitTools* feval=dynamic_cast<QFFitResultsByIndexEvaluationFitTools*>(job.evaluation);
            if (jobIsValid&&feval) {
                feval->doFitForMultithread(job.record, job.run, job.userMin, job.userMax, this);

                {
                    QWriteLocker locker(lock);
                    jobsDone++;
                }
            }
        } else {
            // otherwise thefit depends on several files (stored in records) and we call the according function.
            QFFitResultsByIndexMultiRDREvaluationFitTools* feval=dynamic_cast<QFFitResultsByIndexMultiRDREvaluationFitTools*>(job.evaluation);
            //qDebug()<<jobIsValid<<feval;
            if (jobIsValid&&feval) {
                feval->doFitForMultithread(job.records, job.run, job.userMin, job.userMax, this);

                {
                    QWriteLocker locker(lock);
                    jobsDone++;
                }
            }

        }
    }
}

void QFFitResultsByIndexEvaluationFitThread::addJob(QFFitResultsByIndexEvaluation *evaluation, QFRawDataRecord *record, int run, int userMin, int userMax) {
    QFFitResultsByIndexEvaluationFitThread::Job j;
    j.evaluation=evaluation;
    j.record=record;
    j.run=run;
    j.userMax=userMax;
    j.userMin=userMin;
    QWriteLocker locker(lock);
    jobs.enqueue(j);
    jobCount++;
}

void QFFitResultsByIndexEvaluationFitThread::addJob(QFFitResultsByIndexEvaluation *evaluation, QList<QFRawDataRecord *> records, int run, int userMin, int userMax)
{
    QFFitResultsByIndexEvaluationFitThread::Job j;
    j.evaluation=evaluation;
    j.records=records;
    j.record=NULL;
    j.run=run;
    j.userMax=userMax;
    j.userMin=userMin;
    QWriteLocker locker(lock);
    jobs.enqueue(j);
    jobCount++;
}

void QFFitResultsByIndexEvaluationFitThread::cancel(bool waitForFinished) {
    stopped=true;
    while (waitForFinished && isRunning()) {
        QApplication::processEvents();
    }
}

int QFFitResultsByIndexEvaluationFitThread::getJobsDone()
{
    QReadLocker locker(lock);
    return jobsDone;
}


QFFitResultsByIndexEvaluationFitThread::Job::Job() {
    evaluation=NULL;
    record=NULL;
    run=-1;
    userMin=0;
    userMax=0;
}

int QFFitResultsByIndexEvaluationFitThread::getJobCount()
{
    return jobCount;
}

void QFFitResultsByIndexEvaluationFitThread::log_text(QString message)
{
    emit sigLogText(message);
}

void QFFitResultsByIndexEvaluationFitThread::log_error(QString message)
{
    emit sigLogError(message);
}

void QFFitResultsByIndexEvaluationFitThread::log_warning(QString message)
{
    emit sigLogWarning(message);
}
