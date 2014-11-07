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


QFFitResultsByIndexEvaluationFitSmartThread::QFFitResultsByIndexEvaluationFitSmartThread(bool stopWhenEmpty, QObject *parent) :
    QThread(parent)
{
    lock=new QReadWriteLock();
    stopped=false;
    this->stopWhenEmpty=stopWhenEmpty;
    jobCount=0;
    jobsDone=0;

}

QFFitResultsByIndexEvaluationFitSmartThread::~QFFitResultsByIndexEvaluationFitSmartThread()
{
    delete lock;
}

void QFFitResultsByIndexEvaluationFitSmartThread::addJob(QFFitResultsByIndexEvaluation *evaluation, const QFRawDataRecord *record, int run, int userMin, int userMax) {
    QFFitResultsByIndexEvaluationFitSmartThread::Job j;
    j.evaluation=evaluation;
    j.record=record;
    j.run=run;
    j.userMax=userMax;
    j.userMin=userMin;
    QWriteLocker locker(lock);
    jobs.enqueue(j);
    jobCount++;
}

void QFFitResultsByIndexEvaluationFitSmartThread::addJob(QFFitResultsByIndexEvaluation *evaluation, QList<QFRawDataRecord *> records, int run, int userMin, int userMax)
{
    QFFitResultsByIndexEvaluationFitSmartThread::Job j;
    j.evaluation=evaluation;
    for (int i=0; i<records.size(); i++) {
        j.records<<records[i];
        j.fitfuncIDs.append(evaluation->getFitFunctionID(records[i]));
    }
    j.record=NULL;
    j.run=run;
    j.userMax=userMax;
    j.userMin=userMin;
    QWriteLocker locker(lock);
    jobs.enqueue(j);
    jobCount++;
}

void QFFitResultsByIndexEvaluationFitSmartThread::cancel(bool waitForFinished) {
    stopped=true;
    while (waitForFinished && isRunning()) {
        QApplication::processEvents();
    }
}

int QFFitResultsByIndexEvaluationFitSmartThread::getJobsDone()
{
    QReadLocker locker(lock);
    return jobsDone;
}


QFFitResultsByIndexEvaluationFitSmartThread::Job::Job() {
    evaluation=NULL;
    record=NULL;
    run=-1;
    userMin=0;
    userMax=0;
}

int QFFitResultsByIndexEvaluationFitSmartThread::getJobCount()
{
    return jobCount;
}

void QFFitResultsByIndexEvaluationFitSmartThread::log_text(QString message)
{
    emit sigLogText(message);
}

void QFFitResultsByIndexEvaluationFitSmartThread::log_error(QString message)
{
    emit sigLogError(message);
}

void QFFitResultsByIndexEvaluationFitSmartThread::log_warning(QString message)
{
    emit sigLogWarning(message);
}

void QFFitResultsByIndexEvaluationFitSmartThread::run()
{
    bool jempty=false;
    { QReadLocker locker(lock);
        jempty=jobs.isEmpty();
    }

    QList<QFFitResultsByIndexEvaluationFitTools::MultiFitFitResult> fitresults;
    int cnt=0;
    bool done=false;
    while(!done) {
        QFFitResultsByIndexEvaluationFitSmartThread::Job job;
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
            const QFFitResultsByIndexEvaluationFitTools* feval=dynamic_cast<const QFFitResultsByIndexEvaluationFitTools*>(job.evaluation);
            if (jobIsValid&&feval) {
                QFFitResultsByIndexEvaluationFitTools::MultiFitFitResult result;
                feval->doFitForMultithreadReturn(result, job.record, job.run, job.userMin, job.userMax, this);

                fitresults.append(result);

                {
                    QWriteLocker locker(lock);
                    jobsDone++;
                }
            }
        } else {
            // otherwise thefit depends on several files (stored in records) and we call the according function.
            const QFFitResultsByIndexMultiRDREvaluationFitTools* feval=dynamic_cast<const QFFitResultsByIndexMultiRDREvaluationFitTools*>(job.evaluation);
            //qDebug()<<jobIsValid<<feval;
            if (jobIsValid&&feval) {
                QList<QFFitResultsByIndexEvaluationFitTools::MultiFitFitResult> result;
                feval->doFitForMultithreadReturn(result, job.records, job.fitfuncIDs, job.run, job.userMin, job.userMax, this);
                fitresults<<result;
                {
                    QWriteLocker locker(lock);
                    jobsDone++;
                }
            }

        }
        cnt++;
        done=!(!stopped  && (!(stopWhenEmpty&&jempty)));
        //qDebug()<<"thread, run="<<job.run<<"  done="<<done<<"  stopped="<<stopped<<"  jempty="<<jempty;
        if (cnt>500 || done) {
            QWriteLocker locker(lock);
            QFProject* project=QFPluginServices::getInstance()->getCurrentProject();
            for (int i=0; i<fitresults.size(); i++) {
                const QString evalID=fitresults[i].evalID;
                fitresults[i].getRDR(project)->resultsSetEvaluationGroup(evalID, fitresults[i].evalgroup);
                fitresults[i].getRDR(project)->resultsSetEvaluationGroupLabel(fitresults[i].evalgroup, fitresults[i].egrouplabel);
                fitresults[i].getRDR(project)->resultsSetEvaluationGroupIndex(evalID, fitresults[i].egroupindex);
                fitresults[i].getRDR(project)->resultsSetEvaluationDescription(evalID, fitresults[i].egroupdescription);
            }
            while (!fitresults.isEmpty()) {
                QFRawDataRecord* rdr=fitresults.first().getRDR(project);

                bool emitchange=rdr->isEmitResultsChangedEnabled();
                rdr->disableEmitResultsChanged();

                const QString evalID=fitresults.first().evalID;
                int index=fitresults.first().index;

                if (index<0) {
                    QMapIterator<QString, QFFitResultsByIndexEvaluationFitTools::MultiFitFitParameterResult> mit(fitresults.first().fitresults);
                    while (mit.hasNext()) {
                        mit.next();
                        rdr->resultsSet(evalID, mit.key(), mit.value());
                    }
                    fitresults.first().fitresults.clear();
                } else if (index>=0) {
                    QMapIterator<QString, QFFitResultsByIndexEvaluationFitTools::MultiFitFitParameterResult> mit(fitresults.first().fitresults);
                    while (mit.hasNext()) {
                        mit.next();
                        const QString pid=mit.key();
                        QFFitResultsByIndexEvaluationFitTools::MultiFitFitParameterResult newres=mit.value();
                        switch(mit.value().type) {
                            case QFRawDataRecord::qfrdreBoolean: {
                                    QVector<bool> value;
                                    if (rdr->resultsExists(evalID, pid)) {
                                        QFFitResultsByIndexEvaluationFitTools::MultiFitFitParameterResult ores=rdr->resultsGet(evalID, pid);
                                        if (ores.type==QFRawDataRecord::qfrdreBooleanVector) {
                                            value=ores.bvec;
                                        }
                                    }
                                    while (value.size()<=index) value<<false;
                                    value[index]=mit.value().bvalue;
                                    for (int si=1; si<fitresults.size(); si++) {
                                        int oidx=fitresults[si].index;
                                        if (fitresults[si].rdr==rdr && fitresults[si].evalID==evalID && oidx>=0 && fitresults[si].fitresults.contains(pid)) {
                                            QFFitResultsByIndexEvaluationFitTools::MultiFitFitParameterResult ores=fitresults[si].fitresults[pid];
                                            fitresults[si].fitresults.remove(pid);
                                            while (value.size()<=oidx) value<<false;
                                            value[oidx]=ores.bvalue;
                                        }
                                    }
                                    newres.type=QFRawDataRecord::qfrdreBooleanVector;
                                    newres.bvec=value;
                                    rdr->resultsSet(evalID, pid, newres);
                                } break;
                            case QFRawDataRecord::qfrdreNumber: {
                                    QVector<double> value;
                                    if (rdr->resultsExists(evalID, pid)) {
                                        QFFitResultsByIndexEvaluationFitTools::MultiFitFitParameterResult ores=rdr->resultsGet(evalID, pid);
                                        if (ores.type==QFRawDataRecord::qfrdreNumberVector) {
                                            value=ores.dvec;
                                        }
                                    }
                                    while (value.size()<=index) value<<0.0;
                                    value[index]=mit.value().dvalue;
                                    for (int si=1; si<fitresults.size(); si++) {
                                        int oidx=fitresults[si].index;
                                        if (fitresults[si].rdr==rdr && fitresults[si].evalID==evalID && oidx>=0 && fitresults[si].fitresults.contains(pid)) {
                                            QFFitResultsByIndexEvaluationFitTools::MultiFitFitParameterResult ores=fitresults[si].fitresults[pid];
                                            fitresults[si].fitresults.remove(pid);
                                            while (value.size()<=oidx) value<<0.0;
                                            value[oidx]=ores.dvalue;
                                        }
                                    }
                                    newres.type=QFRawDataRecord::qfrdreNumberVector;
                                    newres.dvec=value;
                                    rdr->resultsSet(evalID, pid, newres);
                                } break;
                            case QFRawDataRecord::qfrdreNumberError: {
                                    QVector<double> value, valuee;
                                    if (rdr->resultsExists(evalID, pid)) {
                                        QFFitResultsByIndexEvaluationFitTools::MultiFitFitParameterResult ores=rdr->resultsGet(evalID, pid);
                                        if (ores.type==QFRawDataRecord::qfrdreNumberErrorVector) {
                                            value=ores.dvec;
                                            valuee=ores.evec;
                                        }
                                    }
                                    while (value.size()<=index) {value<<0.0;}
                                    while (valuee.size()<=index) { valuee<<0.0;}
                                    value[index]=mit.value().dvalue;
                                    valuee[index]=mit.value().derror;
                                    for (int si=1; si<fitresults.size(); si++) {
                                        int oidx=fitresults[si].index;
                                        if (fitresults[si].rdr==rdr && fitresults[si].evalID==evalID && oidx>=0 && fitresults[si].fitresults.contains(pid)) {
                                            QFFitResultsByIndexEvaluationFitTools::MultiFitFitParameterResult ores=fitresults[si].fitresults[pid];
                                            fitresults[si].fitresults.remove(pid);
                                            while (value.size()<=oidx) {value<<0.0;}
                                            while (valuee.size()<=oidx) { valuee<<0.0;}
                                            value[oidx]=ores.dvalue;
                                            valuee[oidx]=ores.derror;
                                        }
                                    }
                                    newres.type=QFRawDataRecord::qfrdreNumberErrorVector;
                                    newres.dvec=value;
                                    newres.evec=valuee;
                                    rdr->resultsSet(evalID, pid, newres);
                                } break;
                            case QFRawDataRecord::qfrdreInteger: {
                                    QVector<qlonglong> value;
                                    if (rdr->resultsExists(evalID, pid)) {
                                        QFFitResultsByIndexEvaluationFitTools::MultiFitFitParameterResult ores=rdr->resultsGet(evalID, pid);
                                        if (ores.type==QFRawDataRecord::qfrdreIntegerVector) {
                                            value=ores.ivec;
                                        }
                                    }
                                    while (value.size()<=index) value<<0;
                                    value[index]=mit.value().ivalue;
                                    for (int si=1; si<fitresults.size(); si++) {
                                        int oidx=fitresults[si].index;
                                        if (fitresults[si].rdr==rdr && fitresults[si].evalID==evalID && oidx>=0 && fitresults[si].fitresults.contains(pid)) {
                                            QFFitResultsByIndexEvaluationFitTools::MultiFitFitParameterResult ores=fitresults[si].fitresults[pid];
                                            fitresults[si].fitresults.remove(pid);
                                            while (value.size()<=oidx) value<<0;
                                            value[oidx]=ores.ivalue;
                                        }
                                    }
                                    newres.type=QFRawDataRecord::qfrdreIntegerVector;
                                    newres.ivec=value;
                                    rdr->resultsSet(evalID, pid, newres);
                                } break;
                            case QFRawDataRecord::qfrdreString: {
                                    QStringList value;
                                    if (rdr->resultsExists(evalID, pid)) {
                                        QFFitResultsByIndexEvaluationFitTools::MultiFitFitParameterResult ores=rdr->resultsGet(evalID, pid);
                                        if (ores.type==QFRawDataRecord::qfrdreStringVector) {
                                            value=ores.svec;
                                        }
                                    }
                                    while (value.size()<=index) value<<"";
                                    value[index]=mit.value().ivalue;
                                    for (int si=1; si<fitresults.size(); si++) {
                                        int oidx=fitresults[si].index;
                                        if (fitresults[si].rdr==rdr && fitresults[si].evalID==evalID && oidx>=0 && fitresults[si].fitresults.contains(pid)) {
                                            QFFitResultsByIndexEvaluationFitTools::MultiFitFitParameterResult ores=fitresults[si].fitresults[pid];
                                            fitresults[si].fitresults.remove(pid);
                                            while (value.size()<=oidx) value<<"";
                                            value[oidx]=ores.svalue;
                                        }
                                    }
                                    newres.type=QFRawDataRecord::qfrdreStringVector;
                                    newres.svec=value;
                                    rdr->resultsSet(evalID, pid, newres);
                                } break;
                        }



                    }
                    fitresults.first().fitresults.clear();
                }

                if (emitchange) rdr->enableEmitResultsChanged();


                // remove all empty fit result sets
                for (int i=fitresults.size()-1; i>=0; i--) {
                    if (fitresults[i].fitresults.isEmpty()) {
                        fitresults.removeAt(i);
                    }
                }
            }


            cnt=0;
        }
    }

}

