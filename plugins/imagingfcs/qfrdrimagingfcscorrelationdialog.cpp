#include "qfrdrimagingfcscorrelationdialog.h"
#include "ui_qfrdrimagingfcscorrelationdialog.h"
#include "qfrdrimagingfcscorrelationjobthread.h"
#include <QtGui>
#include <QDebug>
#include "qmodernprogresswidget.h"

#define UPDATE_TIMEOUT 50

QFRDRImagingFCSCorrelationDialog::QFRDRImagingFCSCorrelationDialog(ProgramOptions* opt, QWidget *parent) :
    QWidget(parent, Qt::Dialog|Qt::WindowTitleHint|Qt::WindowMinMaxButtonsHint|Qt::WindowCloseButtonHint|Qt::WindowStaysOnTopHint),
    ui(new Ui::QFRDRImagingFCSCorrelationDialog)
{
    closing=false;
    this->options=opt;
    lastImagefileDir="";
    ui->setupUi(this);
    ui->edtFrameTime->setRange(1e-10,1e10);
    ui->edtFrameTime->setValue(10000);
    ui->edtFrameRate->setRange(1e-10,1e10);
    ui->edtFrameRate->setValue(100);
    ui->edtOffset->setRange(-1e10,1e10);
    ui->edtOffset->setValue(0);
    ui->widDetails->setVisible(false);
    ui->labDetails->setVisible(false);

    imageFilters.clear();
    ui->cmbFileformat->clear();

    // add more fileformats here
    imageFilters=QFRDRImagingFCSCorrelationJobThread::getImageFilterList();
    imageFormatNames=QFRDRImagingFCSCorrelationJobThread::getImageFormatNameList();
    ui->cmbFileformat->clear();
    ui->cmbFileformat->addItems(imageFormatNames);

    lastImagefileFilter=imageFilters[0];
    connect(ui->edtFrameRate, SIGNAL(valueChanged(double)), this, SLOT(frameRateChanged(double)));
    connect(ui->edtFrameTime, SIGNAL(valueChanged(double)), this, SLOT(frameTimeChanged(double)));
    if (opt) readSettings();
    jobsToAdd.clear();
    QTimer::singleShot(UPDATE_TIMEOUT, this, SLOT(updateProgress()));
}

QFRDRImagingFCSCorrelationDialog::~QFRDRImagingFCSCorrelationDialog() {
    delete ui;
}

bool QFRDRImagingFCSCorrelationDialog::allThreadsDone() const  {
    for (int i=0; i<jobs.size(); i++) {
        if ((jobs[i].thread->isRunning()) || (jobs[i].thread->status()==1)) return false;
    }
    return true;
}

int QFRDRImagingFCSCorrelationDialog::runningThreads() const  {
    int cnt=0;
    for (int i=0; i<jobs.size(); i++) {
        if ((jobs[i].thread->status()==1)||(jobs[i].thread->isRunning())) cnt++;
    }
    return cnt;
}

int QFRDRImagingFCSCorrelationDialog::waitingThreads() const  {
    int cnt=0;
    for (int i=0; i<jobs.size(); i++) {
        if (jobs[i].thread->status()==0) cnt++;
    }
    return cnt;
}

void QFRDRImagingFCSCorrelationDialog::startNextWaitingThread()   {
    for (int i=0; i<jobs.size(); i++) {
        if (jobs[i].thread->status()==0) {
            jobs[i].thread->start();
        }
    }
}

void QFRDRImagingFCSCorrelationDialog::closeEvent(QCloseEvent * event)  {
    closing=true;
    bool allOK=allThreadsDone();
    bool someError=false;
    for (int i=0; i<jobs.size(); i++) {
        if (jobs[i].thread->status()<0) someError=true;
    }

    if (allOK) {
        event->accept();
    } else {
        event->ignore();
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
            event->accept();
        } else {
            closing=false;
        }
    }

    if (event->isAccepted()) {
        // add jobs to project
        QModernProgressDialog prg(this);
        prg.setLabelText(tr("add job results to project ..."));
        prg.open();
        for (int i=0; i<jobs.size(); i++) {
            if (jobs[i].thread->status()==2) {
                addJobToProject(jobs[i]);
            }
        }
        prg.close();
    } else {
        closing=false;
    }

    writeSettings();
}

void QFRDRImagingFCSCorrelationDialog::setProject(QFProject* project) {
    this->project=project;
}

void QFRDRImagingFCSCorrelationDialog::on_spinP_valueChanged(int val) {
    updateCorrelator();
}

void QFRDRImagingFCSCorrelationDialog::on_spinS_valueChanged(int val) {
    updateCorrelator();
}

void QFRDRImagingFCSCorrelationDialog::on_spinM_valueChanged(int val) {
    updateCorrelator();
}

void QFRDRImagingFCSCorrelationDialog::on_cmbCorrelator_currentIndexChanged(int idx) {
    updateCorrelator();
}

void QFRDRImagingFCSCorrelationDialog::on_chkFirstFrame_clicked(bool checked) {
    ui->spinFirstFrame->setEnabled(!checked);
}

void QFRDRImagingFCSCorrelationDialog::on_chkLastFrame_clicked(bool checked) {
    ui->spinLastFrame->setEnabled(!checked);
}



void QFRDRImagingFCSCorrelationDialog::on_btnSelectImageFile_clicked() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Image Series File ..."), lastImagefileDir, imageFilters.join(";;"), &lastImagefileFilter);
    if (!fileName.isEmpty()) {
        lastImagefileDir=QFileInfo(fileName).dir().absolutePath();
        ui->cmbFileformat->setCurrentIndex(imageFilters.indexOf(lastImagefileFilter));
        ui->edtImageFile->setText(fileName);
        ui->edtImageFile->setFocus(Qt::MouseFocusReason);
        on_btnLoad_clicked();
        writeSettings();
    }
}

void QFRDRImagingFCSCorrelationDialog::on_btnLoad_clicked() {
    QString filename=ui->edtImageFile->text();
    if (QFile::exists(filename)) {
        updateFromFile();
        ui->widDetails->setVisible(true);
        ui->labDetails->setVisible(true);
    } else {
        ui->widDetails->setVisible(false);
        ui->labDetails->setVisible(false);
        QMessageBox::critical(this, tr("imFCS Correlator"), tr("The file '%1' does not exist.\nPlease select an existing file!").arg(filename));
    }
    writeSettings();
}

void QFRDRImagingFCSCorrelationDialog::writeSettings() {
    if (!options) return;
    options->getQSettings()->setValue("imaging_fcs/dlg_correlate/last_imagefile_dir", lastImagefileDir);
    options->getQSettings()->setValue("imaging_fcs/dlg_correlate/last_imagefile_filter", lastImagefileFilter);
    options->getQSettings()->setValue("imaging_fcs/dlg_correlate/correlator", ui->cmbCorrelator->currentIndex());
    options->getQSettings()->setValue("imaging_fcs/dlg_correlate/background", ui->cmbBackground->currentIndex());
    options->getQSettings()->setValue("imaging_fcs/dlg_correlate/S", ui->spinS->value());
    options->getQSettings()->setValue("imaging_fcs/dlg_correlate/P", ui->spinP->value());
    options->getQSettings()->setValue("imaging_fcs/dlg_correlate/m", ui->spinM->value());
    options->getQSettings()->setValue("imaging_fcs/dlg_correlate/parallel_threads", ui->spinProcesses->value());
    options->getQSettings()->setValue("imaging_fcs/dlg_correlate/add_to_project", ui->chkAddToProject->isChecked());
}

void QFRDRImagingFCSCorrelationDialog::readSettings() {
    if (!options) return;
    lastImagefileDir=options->getQSettings()->value("imaging_fcs/dlg_correlate/last_imagefile_dir", lastImagefileDir).toString();
    lastImagefileFilter=options->getQSettings()->value("imaging_fcs/dlg_correlate/last_imagefile_filter", lastImagefileFilter).toString();

    ui->cmbCorrelator->setCurrentIndex(options->getQSettings()->value("imaging_fcs/dlg_correlate/correlator", ui->cmbCorrelator->currentIndex()).toInt());
    ui->cmbCorrelator->setCurrentIndex(options->getQSettings()->value("imaging_fcs/dlg_correlate/background", ui->cmbBackground->currentIndex()).toInt());
    ui->spinS->setValue(options->getQSettings()->value("imaging_fcs/dlg_correlate/S", ui->spinS->value()).toInt());
    ui->spinP->setValue(options->getQSettings()->value("imaging_fcs/dlg_correlate/P", ui->spinP->value()).toInt());
    ui->spinM->setValue(options->getQSettings()->value("imaging_fcs/dlg_correlate/m", ui->spinM->value()).toInt());
    ui->spinProcesses->setValue(options->getQSettings()->value("imaging_fcs/dlg_correlate/parallel_threads", ui->spinProcesses->value()).toInt());
    ui->chkAddToProject->setChecked(options->getQSettings()->value("imaging_fcs/dlg_correlate/add_to_project", ui->chkAddToProject->isChecked()).toBool());

}

int QFRDRImagingFCSCorrelationDialog::getIDForProgress(const QFRDRImagingFCSThreadProgress* w) const {
    for (int i=0; i<jobs.size(); i++) {
        if (w == jobs[i].progress) return i;
    }
    return -1;
}

int QFRDRImagingFCSCorrelationDialog::getLayoutIDForProgress(const QWidget* w) const {
    for (int i=0; i<ui->layProgress->count(); i++) {
        if (w == ui->layProgress->itemAt(i)->widget()) return i;
    }
    return -1;
}

void QFRDRImagingFCSCorrelationDialog::updateProgress() {
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
        while ((waitingThreads()>0)&&(runningThreads()<ui->spinProcesses->value())) {
            startNextWaitingThread();
        }


    }
    QTimer::singleShot(UPDATE_TIMEOUT, this, SLOT(updateProgress()));
}

void QFRDRImagingFCSCorrelationDialog::on_btnAddJob_clicked() {
    updateFromFile();

    Job job;
    job.progress=new QFRDRImagingFCSThreadProgress(this);
    job.thread=new QFRDRImagingFCSCorrelationJobThread(this);
    connect(job.thread, SIGNAL(messageChanged(QString)), job.progress, SLOT(setMessage(QString)));
    connect(job.thread, SIGNAL(statusChanged(int)), job.progress, SLOT(setStatus(int)));
    connect(job.thread, SIGNAL(rangeChanged(int, int)), job.progress, SLOT(setRange(int, int)));
    connect(job.thread, SIGNAL(progressChanged(int)), job.progress, SLOT(setProgress(int)));
    connect(job.thread, SIGNAL(progressIncrement(int)), job.progress, SLOT(incProgress(int)));
    connect(job.progress, SIGNAL(cancelClicked()), job.thread, SLOT(cancel()));
    job.filename=ui->edtImageFile->text();
    job.correlator=ui->cmbCorrelator->currentIndex();
    job.fileFormat=ui->cmbFileformat->currentIndex();
    job.backgroundCorrection=ui->cmbBackground->currentIndex();
    job.S=ui->spinS->value();
    job.P=ui->spinP->value();
    job.m=ui->spinM->value();
    job.frameTime=ui->edtFrameTime->value()*1e-6;
    job.addToProject=ui->chkAddToProject->isChecked();
    job.prefix=ui->edtPrefix->text();
    job.backgroundOffset=ui->edtOffset->value();
    job.range_min=-1;
    if (!ui->chkFirstFrame->isChecked()) {
        job.range_min=ui->spinFirstFrame->value();
    }
    job.range_max=-1;
    if (!ui->chkLastFrame->isChecked()) {
        job.range_max=ui->spinLastFrame->value();
    }
    job.acf=true;
    job.ccf=true;
    job.video=true;
    job.video_frames=100;
    job.statistics=true;
    job.statistics_frames=100;
    writeSettings();

    ui->widDetails->setVisible(false);
    ui->labDetails->setVisible(false);
    ui->layProgress->insertWidget(0, job.progress);

    job.progress->setName(tr("correlating '%1'").arg(job.filename));
    job.thread->init(job);
    jobs.append(job);
}

void QFRDRImagingFCSCorrelationDialog::frameTimeChanged(double value) {
    if (value!=0) {
        disconnect(ui->edtFrameRate, SIGNAL(valueChanged(double)), this, SLOT(frameRateChanged(double)));
        ui->edtFrameRate->setValue(1.0/(value/1e6));
        connect(ui->edtFrameRate, SIGNAL(valueChanged(double)), this, SLOT(frameRateChanged(double)));
    }
    updateCorrelator();
}

void QFRDRImagingFCSCorrelationDialog::frameRateChanged(double value) {
    if (value!=0) {
        disconnect(ui->edtFrameTime, SIGNAL(valueChanged(double)), this, SLOT(frameTimeChanged(double)));
        ui->edtFrameTime->setValue((1.0/value)*1e6);
        connect(ui->edtFrameTime, SIGNAL(valueChanged(double)), this, SLOT(frameTimeChanged(double)));
    }
    updateCorrelator();
}

void QFRDRImagingFCSCorrelationDialog::updateCorrelator() {
    int S=ui->spinS->value();
    double P=ui->spinP->value();
    double m=ui->spinM->value();
    double taumin=ui->edtFrameTime->value();
    int corrType=ui->cmbCorrelator->currentIndex();
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
    ui->labCorrelator->setText(tr("<i>spanned correlator lags:</i> &tau;<sub>min</sub> = %1&mu;s ...</i><sub>max</sub><i> = %2s</i>").arg(taumin).arg(taumax/1e6));
}

void QFRDRImagingFCSCorrelationDialog::updateFromFile() {
    QString filename=QFileInfo(ui->edtImageFile->text()).absoluteFilePath();
    double frametime=ui->edtFrameTime->value();
    double baseline_offset=ui->edtOffset->value();

    //////////////////////////////////////////////////////////////////////////////////
    // now we search for a .configuration.ini file describing the selected file
    //////////////////////////////////////////////////////////////////////////////////
    QSettings settings;
    QDir d=QFileInfo(filename).absoluteDir();
    QFileInfo finfo(filename);
    QStringList nameFilters;
    nameFilters<<"*.ini";
    d.setNameFilters(nameFilters);
    foreach (QString iniFile, d.entryList(QDir::Files)) {
        QSettings set(d.absoluteFilePath(iniFile), QSettings::IniFormat);
        int fcnt=set.value("files/count", 0).toInt();
        if (fcnt>0) {
            for (int f=0; f<fcnt; f++) {
                QString fn=set.value("files/name"+QString::number(f), "").toString();
                QString ft=set.value("files/type"+QString::number(f), "").toString();
                if (!fn.isEmpty()) {
                    QString fnAbs=d.absoluteFilePath(fn);
                    if (fnAbs==filename) {
                        if (ft.toLower().simplified().startsWith("tiff")) ui->cmbFileformat->setCurrentIndex(0);
                        else ui->cmbFileformat->setCurrentIndex(-1);

                        if (set.contains("acquisition/frame_time")) frametime=set.value("acquisition/frame_time", frametime).toDouble()*1e6;
                        else if (set.contains("acquisition/frame_rate")) frametime=1.0/set.value("acquisition/frame_rate", frametime).toDouble()*1e6;

                        baseline_offset=set.value("acquisition/baseline_offset", baseline_offset).toDouble();
                    }
                }
            }
        }
    }


    // SET THE FRAMETIME/RATE
    ui->edtFrameTime->setValue(frametime);
    frameTimeChanged(frametime);
    ui->edtOffset->setValue(baseline_offset);
}

void QFRDRImagingFCSCorrelationDialog::addJobToProject(Job job) {
    jobsToAdd.append(job);
}
