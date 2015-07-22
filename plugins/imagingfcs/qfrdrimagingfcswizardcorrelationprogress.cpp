#include "qfrdrimagingfcswizardcorrelationprogress.h"
#include "ui_qfrdrimagingfcswizardcorrelationprogress.h"
#define UPDATE_TIMEOUT 250
#include "qfpluginservices.h"
#include "qmodernprogresswidget.h"

QFRDRImagingFCSWizardCorrelationProgress::QFRDRImagingFCSWizardCorrelationProgress(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QFRDRImagingFCSWizardCorrelationProgress)
{
    ui->setupUi(this);
    closing=false;
    lastAllDone=true;
    QTimer::singleShot(UPDATE_TIMEOUT, this, SLOT(updateProgress()));
}

QFRDRImagingFCSWizardCorrelationProgress::~QFRDRImagingFCSWizardCorrelationProgress()
{
    delete ui;
}

int QFRDRImagingFCSWizardCorrelationProgress::getIDForProgress(const QFRDRImagingFCSThreadProgress *w) const
{
    for (int i=0; i<jobs.size(); i++) {
        if (w == jobs[i].progress) return i;
    }
    return -1;
}

int QFRDRImagingFCSWizardCorrelationProgress::getLayoutIDForProgress(const QWidget *w) const
{
    for (int i=0; i<ui->layProgress->count(); i++) {
        if (w == ui->layProgress->itemAt(i)->widget()) return i;
    }
    return -1;
}

void QFRDRImagingFCSWizardCorrelationProgress::addJob(const IMFCSJob& jobin)
{
    IMFCSJob job=jobin;
    job.progress=new QFRDRImagingFCSThreadProgress(this);
    job.thread=new QFRDRImagingFCSCorrelationJobThread(QFPluginServices::getInstance(), this);
    connect(job.thread, SIGNAL(messageChanged(QString)), job.progress, SLOT(setMessage(QString)));
    connect(job.thread, SIGNAL(statusChanged(int)), job.progress, SLOT(setStatus(int)));
    connect(job.thread, SIGNAL(rangeChanged(int, int)), job.progress, SLOT(setRange(int, int)));
    connect(job.thread, SIGNAL(progressChanged(int)), job.progress, SLOT(setProgress(int)));
    connect(job.thread, SIGNAL(progressIncrement(int)), job.progress, SLOT(incProgress(int)));
    connect(job.progress, SIGNAL(cancelClicked()), job.thread, SLOT(cancel()));

    emit correlationCompleted(false);
    ui->layProgress->insertWidget(0, job.progress);

    job.progress->setName(tr("correlating '%1'").arg(job.filename));
    job.thread->init(job);
    jobs.append(job);
}

void QFRDRImagingFCSWizardCorrelationProgress::updateProgress() {

    if (!closing) {
        ////////////////////////////////////////////////////////////////////////////////////
        // SET OVERALL PROGRESS
        ////////////////////////////////////////////////////////////////////////////////////
        int max=1;
        int p=0;
        bool allDone=(jobs.size()>0);
        for (int i=0; i<jobs.size(); i++) {
            if (jobs[i].progress) {
                max=max+jobs[i].progress->getRangeMax()-jobs[i].progress->getRangeMin();
                p=p+jobs[i].progress->getProgress();
                allDone=allDone&&jobs[i].progress->isDone();
            }
        }
        ui->progressBar->setRange(0,max);
        ui->progressBar->setValue(p);

        if (allDone) {
            ui->progressBar->setValue(ui->progressBar->maximum());
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // CHECK WHETHER WE HAVE TO START SOME MORE THREADS
        ////////////////////////////////////////////////////////////////////////////////////
        //int started=0;
        while ((waitingThreads()>0)&&(runningThreads()<ui->spinProcesses->value()) /*&& (started<=0)*/) {
            startNextWaitingThread();
            //started++;
        }


    }
    if (waitingThreads()<=0) emit correlationCompleted(true);
    QTimer::singleShot(UPDATE_TIMEOUT, this, SLOT(updateProgress()));
}

void QFRDRImagingFCSWizardCorrelationProgress::cancelThreads()
{
    closing=true;
    bool allOK=allThreadsDone();
    bool someError=false;
    for (int i=0; i<jobs.size(); i++) {
        if (jobs[i].thread->status()<0) someError=true;
    }

    if (allOK) {
    } else {
        if (QMessageBox::warning(this, tr("imFCS Correlator"), tr("Some of the correlation jobs are not done yet.\nClose anyways (unfinished jobs will be canceled)!"), QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::Yes) {
            // cancel all unfinished threads
            for (int i=0; i<jobs.size(); i++) {
                if (!jobs[i].progress->isDone()) {
                    jobs[i].thread->cancel();
                }
            }
            setEnabled(false);

            QModernProgressDialog prg(this);
            prg.setLabelText(tr("Canceling remaining threads ..."));
            prg.open();
            // wait until all are OK
            while (!allThreadsDone()) {
                QApplication::processEvents();
            }
            prg.close();
            allOK=true;

            emit correlationCompleted(true);

        } else {
            closing=false;
        }
    }

    if (closing) {
        // add jobs to project
        QModernProgressDialog prg(this);
        prg.setLabelText(tr("add job results to project ..."));
        prg.open();
        for (int i=0; i<jobs.size(); i++) {
            if (jobs[i].addToProject && (jobs[i].thread->status()==2)) {
                for (int j=0; j<jobs[i].thread->getAddFiles().size(); j++) {
                    filesToAdd.append(jobs[i].thread->getAddFiles().at(j));
                }
            }
            jobs[i].progress->close();
            delete jobs[i].progress;
            delete jobs[i].thread;
        }
        prg.close();
    }
}

bool QFRDRImagingFCSWizardCorrelationProgress::allThreadsDone() const
{
    for (int i=0; i<jobs.size(); i++) {
        if ((jobs[i].thread->isRunning()) || (jobs[i].thread->status()==1)) return false;
    }
    return true;
}

int QFRDRImagingFCSWizardCorrelationProgress::runningThreads() const
{
    int cnt=0;
    for (int i=0; i<jobs.size(); i++) {
        if ((jobs[i].thread->status()==1)||(jobs[i].thread->isRunning())) cnt++;
    }
    return cnt;
}

int QFRDRImagingFCSWizardCorrelationProgress::waitingThreads() const
{
    int cnt=0;
    for (int i=0; i<jobs.size(); i++) {
        if (jobs[i].thread->status()==0) cnt++;
    }
    return cnt;
}

void QFRDRImagingFCSWizardCorrelationProgress::startNextWaitingThread()
{
    for (int i=0; i<jobs.size(); i++) {
        if (jobs[i].thread->status()==0) {
            jobs[i].thread->start();
            break;
        }
    }
}
