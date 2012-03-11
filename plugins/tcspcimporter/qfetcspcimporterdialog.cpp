#include "qfetcspcimporterdialog.h"
#include "ui_qfetcspcimporterdialog.h"
#include "qfetcspcimporterjobthread.h"
#include <QtGui>
#include <QDebug>
#include "qmodernprogresswidget.h"

#define UPDATE_TIMEOUT 250

QFETCSPCImporterDialog::QFETCSPCImporterDialog(QFPluginServices* pluginservices, ProgramOptions* opt, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFETCSPCImporterDialog)
{
    this->pluginServices=pluginservices;
    closing=false;
    this->options=opt;
    lastTCSPCFileDir="";
    duration=0;
    ui->setupUi(this);
    ui->spinRangeStart->setRange(0,1e10);
    ui->spinRangeStart->setValue(0);
    ui->spinRangeEnd->setRange(0,1e10);
    ui->spinRangeEnd->setValue(100);
    ui->spinCountrateBinning->setRange(1e-6,1e10);
    ui->spinCountrateBinning->setValue(10);
    ui->spinFCSTauMin->setRange(1e-6,1e10);
    ui->spinFCSTauMin->setValue(1);
/*
%counter% by the value of \a counter, if this value is >0, or by an empty string
          - \c %S% S parameter of the correlator
          - \c %P% P parameter of the correlator
          - \c %M% m parameter of the correlator
          - \c %start% first used frame
          - \c %end% last used frame
          - \c %fcs_segments% segments the image series is cut into
          - \c %fcs_taumin% minimum lagtime of FCS
          - \c %cr_binning% countrate binning
          - \c %correlator% correlator type used
          - \c %correlatorid% correlator type used as number  */
    ui->edtPostfix->addInsertContextMenuEntry(tr("insert %counter%"), QString("%counter%"));
    ui->edtPostfix->addInsertContextMenuEntry(tr("insert %s%"), QString("%s%"));
    ui->edtPostfix->addInsertContextMenuEntry(tr("insert %p%"), QString("%p%"));
    ui->edtPostfix->addInsertContextMenuEntry(tr("insert %m%"), QString("%m%"));
    ui->edtPostfix->addInsertContextMenuEntry(tr("insert %start%"), QString("%start%"));
    ui->edtPostfix->addInsertContextMenuEntry(tr("insert %end%"), QString("%end%"));
    ui->edtPostfix->addInsertContextMenuEntry(tr("insert %fcs_segments%"), QString("%fcs_segments%"));
    ui->edtPostfix->addInsertContextMenuEntry(tr("insert %fcs_taumin%"), QString("%fcs_taumin%"));
    ui->edtPostfix->addInsertContextMenuEntry(tr("insert %correlator%"), QString("%correlator%"));
    ui->edtPostfix->addInsertContextMenuEntry(tr("insert %correlatorid%"), QString("%correlatorid%"));
    ui->edtPostfix->addInsertContextMenuEntry(tr("insert %cr_binning%"), QString("%cr_binning%"));
    //ui->edtPostfix->addInsertContextMenuEntry(tr("insert %%"), QString("%%"));

    tcspcFilters.clear();
    ui->cmbFileformat->clear();

    // add more fileformats here
    tcspcFilters=QFETCSPCImporterJobThread::getImageFilterList();
    tcspcFormatNames=QFETCSPCImporterJobThread::getImageFormatNameList();
    ui->cmbFileformat->clear();
    ui->cmbFileformat->addItems(tcspcFormatNames);

    lastTCSPCFileFilter=tcspcFilters[0];

    if (opt) readSettings();
    filesToAdd.clear();
    setEditControlsEnabled(false);
    QTimer::singleShot(UPDATE_TIMEOUT, this, SLOT(updateProgress()));


}

QFETCSPCImporterDialog::~QFETCSPCImporterDialog() {
    delete ui;
}

void  QFETCSPCImporterDialog::setEditControlsEnabled(bool enabled) {
    ui->widDetails->setEnabled(enabled);
    ui->labDetails->setEnabled(enabled);
    ui->spinRangeStart->setEnabled(enabled && !ui->chkFirstFrame->isChecked());
    ui->spinRangeEnd->setEnabled(enabled && !ui->chkLastFrame->isChecked());
}

bool QFETCSPCImporterDialog::allThreadsDone() const  {
    for (int i=0; i<jobs.size(); i++) {
        if ((jobs[i].thread->isRunning()) || (jobs[i].thread->status()==1)) return false;
    }
    return true;
}

int QFETCSPCImporterDialog::runningThreads() const  {
    int cnt=0;
    for (int i=0; i<jobs.size(); i++) {
        if ((jobs[i].thread->status()==1)||(jobs[i].thread->isRunning())) cnt++;
    }
    return cnt;
}

int QFETCSPCImporterDialog::waitingThreads() const  {
    int cnt=0;
    for (int i=0; i<jobs.size(); i++) {
        if (jobs[i].thread->status()==0) cnt++;
    }
    return cnt;
}

void QFETCSPCImporterDialog::startNextWaitingThread()   {
    for (int i=0; i<jobs.size(); i++) {
        if (jobs[i].thread->status()==0) {
            jobs[i].thread->start();
            break;
        }
    }
}

void QFETCSPCImporterDialog::done(int status)  {
    closing=true;
    bool allOK=allThreadsDone();
    bool someError=false;
    for (int i=0; i<jobs.size(); i++) {
        if (jobs[i].thread->status()<0) someError=true;
    }

    if (allOK) {
    } else {
        if (QMessageBox::warning(this, tr("TCSPC Importer"), tr("Some of the correlation jobs are not done yet.\nClose anyways (unfinished jobs will be canceled)!"), QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::Yes) {
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
        QDialog::done(status);
        close();
    }

    writeSettings();
}

void QFETCSPCImporterDialog::setProject(QFProject* project) {
    this->project=project;
}

void QFETCSPCImporterDialog::on_spinP_valueChanged(int val) {
    updateCorrelator();
}

void QFETCSPCImporterDialog::on_spinS_valueChanged(int val) {
    updateCorrelator();
}

void QFETCSPCImporterDialog::on_spinM_valueChanged(int val) {
    updateCorrelator();
}

void QFETCSPCImporterDialog::on_cmbCorrelator_currentIndexChanged(int idx) {
    updateCorrelator();
}

void QFETCSPCImporterDialog::on_chkFirstFrame_clicked(bool checked) {
    ui->spinRangeStart->setEnabled(!checked);
    updateDuration();
}

void QFETCSPCImporterDialog::on_chkLastFrame_clicked(bool checked) {
    ui->spinRangeEnd->setEnabled(!checked);
    updateDuration();
}



void QFETCSPCImporterDialog::on_btnSelectFile_clicked() {
    QString fileName = qfGetOpenFileName(this, tr("Select TCSPC File ..."), lastTCSPCFileDir, tcspcFilters.join(";;"), &lastTCSPCFileFilter);
    if (!fileName.isEmpty()) {
        lastTCSPCFileDir=QFileInfo(fileName).dir().absolutePath();
        ui->cmbFileformat->setCurrentIndex(tcspcFilters.indexOf(lastTCSPCFileFilter));
        ui->edtTCSPCFile->setText(fileName);
        ui->edtTCSPCFile->setFocus(Qt::MouseFocusReason);
        on_btnLoad_clicked();
        writeSettings();
    }
}

void QFETCSPCImporterDialog::on_btnHelp_clicked() {
    pluginServices->displayHelpWindow(pluginServices->getPluginHelpDirectory("tcspcimporter")+"tcspcimporter.html");
}

void QFETCSPCImporterDialog::on_btnLoad_clicked() {
    QString filename=ui->edtTCSPCFile->text();
    if (QFile::exists(filename)) {
        updateFromFile();
        setEditControlsEnabled(true);
    } else {
        setEditControlsEnabled(false);
        QMessageBox::critical(this, tr("TCSPC Importer"), tr("The file '%1' does not exist.\nPlease select an existing file!").arg(filename));
    }
    writeSettings();
}

void QFETCSPCImporterDialog::writeSettings() {
    if (!options) return;
    options->getQSettings()->setValue("tcspcimporter/dlg_correlate/last_imagefile_dir", lastTCSPCFileDir);
    options->getQSettings()->setValue("tcspcimporter/dlg_correlate/last_imagefile_filter", lastTCSPCFileFilter);
    options->getQSettings()->setValue("tcspcimporter/dlg_correlate/correlator", ui->cmbCorrelator->currentIndex());
    options->getQSettings()->setValue("tcspcimporter/dlg_correlate/prefix", ui->edtPrefix->text());
    options->getQSettings()->setValue("tcspcimporter/dlg_correlate/postfix", ui->edtPostfix->text());
    options->getQSettings()->setValue("tcspcimporter/dlg_correlate/S", ui->spinS->value());
    options->getQSettings()->setValue("tcspcimporter/dlg_correlate/P", ui->spinP->value());
    options->getQSettings()->setValue("tcspcimporter/dlg_correlate/m", ui->spinM->value());
    options->getQSettings()->setValue("tcspcimporter/dlg_correlate/parallel_threads", ui->spinProcesses->value());
    options->getQSettings()->setValue("tcspcimporter/dlg_correlate/add_to_project", ui->chkAddToProject->isChecked());
    options->getQSettings()->setValue("tcspcimporter/dlg_correlate/segments", ui->spinSegments->value());
    options->getQSettings()->setValue("tcspcimporter/dlg_correlate/cr_binning", ui->spinCountrateBinning->value());
    options->getQSettings()->setValue("tcspcimporter/dlg_correlate/fcs_taumin", ui->spinFCSTauMin->value());
    options->getQSettings()->setValue("tcspcimporter/dlg_correlate/range_start", ui->spinRangeStart->value());
    options->getQSettings()->setValue("tcspcimporter/dlg_correlate/range_end", ui->spinRangeEnd->value());
}

void QFETCSPCImporterDialog::readSettings() {
    if (!options) return;
    lastTCSPCFileDir=options->getQSettings()->value("tcspcimporter/dlg_correlate/last_imagefile_dir", lastTCSPCFileDir).toString();
    lastTCSPCFileFilter=options->getQSettings()->value("tcspcimporter/dlg_correlate/last_imagefile_filter", lastTCSPCFileFilter).toString();

    ui->cmbCorrelator->setCurrentIndex(options->getQSettings()->value("tcspcimporter/dlg_correlate/correlator", ui->cmbCorrelator->currentIndex()).toInt());
    ui->spinS->setValue(options->getQSettings()->value("tcspcimporter/dlg_correlate/S", ui->spinS->value()).toInt());
    ui->spinP->setValue(options->getQSettings()->value("tcspcimporter/dlg_correlate/P", ui->spinP->value()).toInt());
    ui->spinM->setValue(options->getQSettings()->value("tcspcimporter/dlg_correlate/m", ui->spinM->value()).toInt());
    ui->spinProcesses->setValue(options->getQSettings()->value("tcspcimporter/dlg_correlate/parallel_threads", ui->spinProcesses->value()).toInt());
    ui->chkAddToProject->setChecked(options->getQSettings()->value("tcspcimporter/dlg_correlate/add_to_project", ui->chkAddToProject->isChecked()).toBool());
    ui->edtPrefix->setText(options->getQSettings()->value("tcspcimporter/dlg_correlate/prefix", ui->edtPrefix->text()).toString());
    ui->edtPostfix->setText(options->getQSettings()->value("tcspcimporter/dlg_correlate/postfix", ui->edtPostfix->text()).toString());
    ui->spinSegments->setValue(options->getQSettings()->value("tcspcimporter/dlg_correlate/segments", ui->spinSegments->value()).toInt());
    ui->spinSegments->setValue(options->getQSettings()->value("tcspcimporter/dlg_correlate/cr_binning", ui->spinCountrateBinning->value()).toDouble());
    ui->spinSegments->setValue(options->getQSettings()->value("tcspcimporter/dlg_correlate/fcs_taumin", ui->spinFCSTauMin->value()).toDouble());
    ui->spinSegments->setValue(options->getQSettings()->value("tcspcimporter/dlg_correlate/range_end", ui->spinRangeEnd->value()).toDouble());
    ui->spinSegments->setValue(options->getQSettings()->value("tcspcimporter/dlg_correlate/range_start", ui->spinRangeStart->value()).toDouble());

}

int QFETCSPCImporterDialog::getIDForProgress(const QFETCSPCImporterThreadProgress *w) const {
    for (int i=0; i<jobs.size(); i++) {
        if (w == jobs[i].progress) return i;
    }
    return -1;
}

int QFETCSPCImporterDialog::getLayoutIDForProgress(const QWidget* w) const {
    for (int i=0; i<ui->layProgress->count(); i++) {
        if (w == ui->layProgress->itemAt(i)->widget()) return i;
    }
    return -1;
}

void QFETCSPCImporterDialog::updateProgress() {
    if (!closing) {
        ////////////////////////////////////////////////////////////////////////////////////
        // SET OVERALL PROGRESS
        ////////////////////////////////////////////////////////////////////////////////////
        int max=1;
        int p=0;
        for (int i=0; i<jobs.size(); i++) {
            if (jobs[i].progress) {
                max=max+jobs[i].progress->getRangeMax()-jobs[i].progress->getRangeMin();
                p=p+jobs[i].progress->getProgress();
            }
        }
        ui->progressBar->setRange(0,max);
        ui->progressBar->setValue(p);


        ////////////////////////////////////////////////////////////////////////////////////
        // CHECK WHETHER WE HAVE TO START SOME MORE THREADS
        ////////////////////////////////////////////////////////////////////////////////////
        //int started=0;
        while ((waitingThreads()>0)&&(runningThreads()<ui->spinProcesses->value()) /*&& (started<=0)*/) {
            startNextWaitingThread();
            //started++;
        }


    }
    QTimer::singleShot(UPDATE_TIMEOUT, this, SLOT(updateProgress()));
}

void QFETCSPCImporterDialog::on_btnAddJob_clicked() {
    updateFromFile(false); // make sure that inputconfigfile cintains the settings file for the input (if it exists)
    TCSPCImporterJob job;
    job.progress=new QFETCSPCImporterThreadProgress(this);
    job.thread=new QFETCSPCImporterJobThread(this);
    connect(job.thread, SIGNAL(messageChanged(QString)), job.progress, SLOT(setMessage(QString)));
    connect(job.thread, SIGNAL(statusChanged(int)), job.progress, SLOT(setStatus(int)));
    connect(job.thread, SIGNAL(rangeChanged(int, int)), job.progress, SLOT(setRange(int, int)));
    connect(job.thread, SIGNAL(progressChanged(int)), job.progress, SLOT(setProgress(int)));
    connect(job.thread, SIGNAL(progressIncrement(int)), job.progress, SLOT(incProgress(int)));
    connect(job.progress, SIGNAL(cancelClicked()), job.thread, SLOT(cancel()));
    job.filename=ui->edtTCSPCFile->text();
    job.correlator=ui->cmbCorrelator->currentIndex();
    job.fileFormat=ui->cmbFileformat->currentIndex();
    //qDebug()<<"job.fileFormat="<<job.fileFormat<<"  "<<ui->cmbFileformat->count();
    job.S=ui->spinS->value();
    job.P=ui->spinP->value();
    job.m=ui->spinM->value();
    job.countrate_binning=ui->spinCountrateBinning->value()*1e-6;
    job.addToProject=ui->chkAddToProject->isChecked();
    job.prefix=ui->edtPrefix->text();
    job.postfix=ui->edtPostfix->text();
    job.fcs_taumin=ui->spinFCSTauMin->value()*1e-6;
    job.fcs_segments=ui->spinSegments->value();

    //qDebug()<<job.filenameBackground;
    job.range_min=-1;
    if (!ui->chkFirstFrame->isChecked()) {
        job.range_min=ui->spinRangeStart->value();
    }
    job.range_max=-1;
    if (!ui->chkLastFrame->isChecked()) {
        job.range_max=ui->spinRangeEnd->value();
    }
    writeSettings();

    setEditControlsEnabled(false);
    ui->layProgress->insertWidget(0, job.progress);

    job.progress->setName(tr("correlating '%1'").arg(job.filename));
    job.thread->init(job);
    jobs.append(job);
}


void QFETCSPCImporterDialog::updateCorrelator() {
    int corrType=ui->cmbCorrelator->currentIndex();
    if (corrType==2) {
        ui->spinM->setEnabled(false);
        ui->labM->setEnabled(false);
        ui->spinM->setValue(2);
    } else {
        ui->spinM->setEnabled(true);
        ui->labM->setEnabled(true);
    }

    int S=ui->spinS->value();
    double P=ui->spinP->value();
    double m=ui->spinM->value();
    double taumin=ui->spinFCSTauMin->value();
    double taumax=taumin;

    if (corrType==1) {
        taumax=0;
        for (int s=0; s<S; s++) {
            if (s==0) {
                taumax+=pow(m, s)*taumin*P;
            } else {
                taumax+=pow(m, s)*taumin*P*(m-1.0)/m;
            }
        }
    } else {
        taumax=0;
        for (int s=0; s<S; s++) {
            taumax+=pow(m, s)*taumin*P;
        }
    }
    ui->labCorrelator->setText(tr("<i>spanned correlator lags:</i> &tau;<sub>min</sub> = %1&mu;s ...&tau;<sub>max</sub><i> = %2s</i>").arg(taumin).arg(taumax/1e6));
}

void QFETCSPCImporterDialog::updateDuration() {
    double start=0;
    if (!ui->chkFirstFrame->isChecked()) {
        start=qMin(duration, ui->spinRangeStart->value());
    }
    double stop=0;
    if (!ui->chkLastFrame->isChecked()) {
        stop=qMin(duration, ui->spinRangeEnd->value());
    }
    ui->labRange->setText(tr("duration: %1 s").arg(fabs(stop-start)));
}



void QFETCSPCImporterDialog::updateFromFile(bool readFrameCount) {
    updateDuration();
}


QStringList QFETCSPCImporterDialog::getFilesToAdd() const {
    return filesToAdd;
}

