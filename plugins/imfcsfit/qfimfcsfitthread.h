#ifndef QFImFCSFitThread_H
#define QFImFCSFitThread_H

#include <QThread>
#include "qfrawdatarecord.h"
#include "qfevaluationitem.h"
#include <QReadWriteLock>
#include <QReadLocker>
#include <QWriteLocker>
#include <QQueue>
#include "qfpluginservices.h"

class QFImFCSFitEvaluation; // forward


/*! \brief QThread used to perform imFCS fits in several parallel threads
    \ingroup qf3evalp_imfcsfit

    Use this class by starting a thread and adding jobs with addJob(). The thread will terminate
    when either cancel() is called (this waits until the current job finishes and the terminates)
    or, when all jobs are done and stopWhenEmpty is \c true. If it is \c false, the thread will
    wait for new jobs.
*/
class QFImFCSFitThread : public QThread, public QFPluginLogService {
        Q_OBJECT
        Q_INTERFACES(QFPluginLogService)
    public:

        explicit QFImFCSFitThread(bool stopWhenEmpty, QObject* parent=NULL);
        virtual ~QFImFCSFitThread();
        
        virtual void run();

        void addJob(QFImFCSFitEvaluation* evaluation, QFRawDataRecord* record, int run, int userMin, int userMax);

        void cancel(bool waitForFinished=true);

        int getJobsDone();
        int getJobCount();

        virtual void log_text(QString message);
        /** \brief log project warning message
         *  \param message the warning message to log
         */
        virtual void log_warning(QString message);
        /** \brief log project error message
         *  \param message the error message to log
         */
        virtual void log_error(QString message);

    signals:
        void sigLogText(QString message);
        void sigLogWarning(QString message);
        void sigLogError(QString message);

    protected:
        struct Job {
            Job();
            QFImFCSFitEvaluation* evaluation;
            QFRawDataRecord* record;
            int run;
            int userMin;
            int userMax;
        };
        QQueue<Job> jobs;

        mutable QReadWriteLock* lock;

        bool stopped;
        bool stopWhenEmpty;

        int jobsDone;
        int jobCount;

        
};

#endif // QFImFCSFitThread_H
