/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

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

#ifndef QFFitResultsByIndexEvaluationFitThread_H
#define QFFitResultsByIndexEvaluationFitThread_H

#include <QThread>
#include "qfrawdatarecord.h"
#include "qfevaluationitem.h"
#include <QMutex>
#include <QReadLocker>
#include <QWriteLocker>
#include <QQueue>
#include "qfpluginservices.h"

class QFFitResultsByIndexEvaluation; // forward


/*! \brief QThread used to performFCS fits in several parallel threads
    \ingroup qf3evalp_imfcsfit

    Use this class by starting a thread and adding jobs with addJob(). The thread will terminate
    when either cancel() is called (this waits until the current job finishes and the terminates)
    or, when all jobs are done and stopWhenEmpty is \c true. If it is \c false, the thread will
    wait for new jobs.
*/
class QFFitResultsByIndexEvaluationFitThread : public QThread, public QFPluginLogService {
        Q_OBJECT
        Q_INTERFACES(QFPluginLogService)
    public:

        explicit QFFitResultsByIndexEvaluationFitThread(bool stopWhenEmpty, QObject* parent=NULL);
        virtual ~QFFitResultsByIndexEvaluationFitThread();
        
        virtual void run();

        void addJob(QFFitResultsByIndexEvaluation* evaluation, QFRawDataRecord* record, int run, int userMin, int userMax);
        void addJob(QFFitResultsByIndexEvaluation* evaluation, QList<QFRawDataRecord*> records, int run, int userMin, int userMax);

        void cleanJobs();

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
            QFFitResultsByIndexEvaluation* evaluation;
            QFRawDataRecord* record;
            QList<QFRawDataRecord*> records;
            QFFitAlgorithm* falg;
            QFFitFunction* ffunc;
            int run;
            int userMin;
            int userMax;
        };
        QQueue<Job> jobs;
        QList<QFFitAlgorithm*> falgs;
        QList<QFFitFunction*> ffuncs;

        mutable QMutex* lock;

        bool stopped;
        bool stopWhenEmpty;

        int jobsDone;
        int jobCount;

        
};

class QFFitResultsByIndexEvaluationFitSmartThread; // forward

class QFFitResultsByIndexEvaluationFitSmartThread_Writer  : public QThread, public QFPluginLogService {
        Q_OBJECT
        Q_INTERFACES(QFPluginLogService)
    public:
        explicit QFFitResultsByIndexEvaluationFitSmartThread_Writer(const QFProject* project,  QObject* parent=NULL);
        virtual ~QFFitResultsByIndexEvaluationFitSmartThread_Writer();

        virtual void run();

        void cancel(bool waitForFinished=true);


        virtual void log_text(QString message);
        /** \brief log project warning message
         *  \param message the warning message to log
         */
        virtual void log_warning(QString message);
        /** \brief log project error message
         *  \param message the error message to log
         */
        virtual void log_error(QString message);

        void addFitResult(const QList<QFRawDataRecord::QFFitFitResultsStore>& fitresults);

    signals:
        void sigLogText(QString message);
        void sigLogWarning(QString message);
        void sigLogError(QString message);

    protected:
        QList<QFRawDataRecord::QFFitFitResultsStore> fitresults;
        const QFProject* project;

        mutable QMutex* lock;

        bool stopped;
};

class QFFitResultsByIndexEvaluationFitSmartThread  : public QThread, public QFPluginLogService {
        Q_OBJECT
        Q_INTERFACES(QFPluginLogService)
    public:

        explicit QFFitResultsByIndexEvaluationFitSmartThread(bool stopWhenEmpty, QObject* parent=NULL);
        explicit QFFitResultsByIndexEvaluationFitSmartThread(QFFitResultsByIndexEvaluationFitSmartThread_Writer* writer, bool stopWhenEmpty, QObject* parent=NULL);
        virtual ~QFFitResultsByIndexEvaluationFitSmartThread();

        void setWriter(QFFitResultsByIndexEvaluationFitSmartThread_Writer* writer);

        virtual void run();

        void addJob(QFFitResultsByIndexEvaluation* evaluation, const QFRawDataRecord* record, int run, int userMin, int userMax);
        void addJob(QFFitResultsByIndexEvaluation* evaluation, QList<QFRawDataRecord *> records, int run, int userMin, int userMax);

        void cancel(bool waitForFinished=true);

        int getJobsDone();
        int getJobCount();

        void cleanJobs();

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
            const QFFitResultsByIndexEvaluation* evaluation;
            const QFRawDataRecord* record;
            QList<const QFRawDataRecord*> records;
            QStringList fitfuncIDs;
            QFFitAlgorithm* falg;
            QFFitFunction* ffunc;
            int run;
            int userMin;
            int userMax;
        };
        QQueue<Job> jobs;
        QList<QFFitAlgorithm*> falgs;
        QList<QFFitFunction*> ffuncs;

        mutable QMutex* lock;

        bool stopped;
        bool stopWhenEmpty;

        int jobsDone;
        int jobCount;

        QFFitResultsByIndexEvaluationFitSmartThread_Writer* writer;

};

#endif // QFFitResultsByIndexEvaluationFitThread_H
