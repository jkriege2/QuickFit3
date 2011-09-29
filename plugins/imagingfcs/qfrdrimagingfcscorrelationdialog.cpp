#include "qfrdrimagingfcscorrelationdialog.h"
#include "ui_qfrdrimagingfcscorrelationdialog.h"
#include "qfrdrimagingfcscorrelationjobthread.h"
#include <QtGui>
#include <QDebug>
#include "qmodernprogresswidget.h"

#define UPDATE_TIMEOUT 50

QFRDRImagingFCSCorrelationDialog::QFRDRImagingFCSCorrelationDialog(QFPluginServices* pluginservices, ProgramOptions* opt, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFRDRImagingFCSCorrelationDialog)
{
    this->pluginServices=pluginservices;
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
    connect(ui->spinFirstFrame, SIGNAL(valueChanged(int)), this, SLOT(updateFrameCount()));
    connect(ui->spinLastFrame, SIGNAL(valueChanged(int)), this, SLOT(updateFrameCount()));
    connect(ui->spinSegments, SIGNAL(valueChanged(int)), this, SLOT(updateFrameCount()));
    connect(ui->spinStatistics, SIGNAL(valueChanged(int)), this, SLOT(updateFrameCount()));
    connect(ui->spinVideoFrames, SIGNAL(valueChanged(int)), this, SLOT(updateFrameCount()));
    if (opt) readSettings();
    filesToAdd.clear();
    setEditControlsEnabled(false);
    QTimer::singleShot(UPDATE_TIMEOUT, this, SLOT(updateProgress()));
}

QFRDRImagingFCSCorrelationDialog::~QFRDRImagingFCSCorrelationDialog() {
    delete ui;
}

void  QFRDRImagingFCSCorrelationDialog::setEditControlsEnabled(bool enabled) {
    ui->widDetails->setEnabled(enabled);
    ui->labDetails->setEnabled(enabled);
    ui->spinFirstFrame->setEnabled(enabled && !ui->chkFirstFrame->isChecked());
    ui->spinLastFrame->setEnabled(enabled && !ui->chkLastFrame->isChecked());
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

void QFRDRImagingFCSCorrelationDialog::done(int status)  {
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
    updateFrameCount();
}

void QFRDRImagingFCSCorrelationDialog::on_chkLastFrame_clicked(bool checked) {
    ui->spinLastFrame->setEnabled(!checked);
    updateFrameCount();
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

void QFRDRImagingFCSCorrelationDialog::on_btnHelp_clicked() {
    pluginServices->displayHelpWindow(pluginServices->getPluginHelpDirectory("imaging_fcs")+"imfcs_correlator.html");
}

void QFRDRImagingFCSCorrelationDialog::on_btnLoad_clicked() {
    QString filename=ui->edtImageFile->text();
    if (QFile::exists(filename)) {
        updateFromFile();
        setEditControlsEnabled(true);
    } else {
        setEditControlsEnabled(false);
        QMessageBox::critical(this, tr("imFCS Correlator"), tr("The file '%1' does not exist.\nPlease select an existing file!").arg(filename));
    }
    writeSettings();
}

void QFRDRImagingFCSCorrelationDialog::writeSettings() {
    if (!options) return;
    options->getQSettings()->setValue("imaging_fcs/dlg_correlate/last_imagefile_dir", lastImagefileDir);
    options->getQSettings()->setValue("imaging_fcs/dlg_correlate/last_imagefile_filter", lastImagefileFilter);
    options->getQSettings()->setValue("imaging_fcs/dlg_correlate/correlator", ui->cmbCorrelator->currentIndex());
    options->getQSettings()->setValue("imaging_fcs/dlg_correlate/prefix", ui->edtPrefix->text());
    options->getQSettings()->setValue("imaging_fcs/dlg_correlate/postfix", ui->edtPostfix->text());
    options->getQSettings()->setValue("imaging_fcs/dlg_correlate/background", ui->cmbBackground->currentIndex());
    options->getQSettings()->setValue("imaging_fcs/dlg_correlate/S", ui->spinS->value());
    options->getQSettings()->setValue("imaging_fcs/dlg_correlate/P", ui->spinP->value());
    options->getQSettings()->setValue("imaging_fcs/dlg_correlate/m", ui->spinM->value());
    options->getQSettings()->setValue("imaging_fcs/dlg_correlate/parallel_threads", ui->spinProcesses->value());
    options->getQSettings()->setValue("imaging_fcs/dlg_correlate/add_to_project", ui->chkAddToProject->isChecked());
    options->getQSettings()->setValue("imaging_fcs/dlg_correlate/statistics", ui->spinStatistics->value());
    options->getQSettings()->setValue("imaging_fcs/dlg_correlate/video", ui->chkVideo->isChecked());
    options->getQSettings()->setValue("imaging_fcs/dlg_correlate/video_frames", ui->spinVideoFrames->value());
    options->getQSettings()->setValue("imaging_fcs/dlg_correlate/acf", ui->chkACF->isChecked());
    options->getQSettings()->setValue("imaging_fcs/dlg_correlate/ccf", ui->chkCCF->isChecked());
    options->getQSettings()->setValue("imaging_fcs/dlg_correlate/segments", ui->spinSegments->value());
}

void QFRDRImagingFCSCorrelationDialog::readSettings() {
    if (!options) return;
    lastImagefileDir=options->getQSettings()->value("imaging_fcs/dlg_correlate/last_imagefile_dir", lastImagefileDir).toString();
    lastImagefileFilter=options->getQSettings()->value("imaging_fcs/dlg_correlate/last_imagefile_filter", lastImagefileFilter).toString();

    ui->cmbCorrelator->setCurrentIndex(options->getQSettings()->value("imaging_fcs/dlg_correlate/correlator", ui->cmbCorrelator->currentIndex()).toInt());
    ui->cmbBackground->setCurrentIndex(options->getQSettings()->value("imaging_fcs/dlg_correlate/background", ui->cmbBackground->currentIndex()).toInt());
    ui->spinS->setValue(options->getQSettings()->value("imaging_fcs/dlg_correlate/S", ui->spinS->value()).toInt());
    ui->spinP->setValue(options->getQSettings()->value("imaging_fcs/dlg_correlate/P", ui->spinP->value()).toInt());
    ui->spinM->setValue(options->getQSettings()->value("imaging_fcs/dlg_correlate/m", ui->spinM->value()).toInt());
    ui->spinProcesses->setValue(options->getQSettings()->value("imaging_fcs/dlg_correlate/parallel_threads", ui->spinProcesses->value()).toInt());
    ui->chkAddToProject->setChecked(options->getQSettings()->value("imaging_fcs/dlg_correlate/add_to_project", ui->chkAddToProject->isChecked()).toBool());
    ui->edtPrefix->setText(options->getQSettings()->value("imaging_fcs/dlg_correlate/prefix", ui->edtPrefix->text()).toString());
    ui->edtPostfix->setText(options->getQSettings()->value("imaging_fcs/dlg_correlate/postfix", ui->edtPostfix->text()).toString());
    ui->chkVideo->setChecked(options->getQSettings()->value("imaging_fcs/dlg_correlate/video", ui->chkVideo->isChecked()).toBool());
    ui->spinVideoFrames->setValue(options->getQSettings()->value("imaging_fcs/dlg_correlate/video_frames", ui->spinVideoFrames->value()).toInt());
    ui->spinStatistics->setValue(options->getQSettings()->value("imaging_fcs/dlg_correlate/statistics", ui->spinStatistics->value()).toInt());
    ui->chkACF->setChecked(options->getQSettings()->value("imaging_fcs/dlg_correlate/acf", ui->chkACF->isChecked()).toBool());
    ui->chkCCF->setChecked(options->getQSettings()->value("imaging_fcs/dlg_correlate/ccf", ui->chkCCF->isChecked()).toBool());
    ui->spinSegments->setValue(options->getQSettings()->value("imaging_fcs/dlg_correlate/segments", ui->spinSegments->value()).toInt());

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
    updateFromFile(false); // make sure that inputconfigfile cintains the settings file for the input (if it exists)
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
    job.descriptionFilename=inputconfigfile;
    job.correlator=ui->cmbCorrelator->currentIndex();
    job.fileFormat=ui->cmbFileformat->currentIndex();
    qDebug()<<"job.fileFormat="<<job.fileFormat<<"  "<<ui->cmbFileformat->count();
    job.backgroundCorrection=ui->cmbBackground->currentIndex();
    job.S=ui->spinS->value();
    job.P=ui->spinP->value();
    job.m=ui->spinM->value();
    job.frameTime=ui->edtFrameTime->value()*1e-6;
    job.addToProject=ui->chkAddToProject->isChecked();
    job.prefix=ui->edtPrefix->text();
    job.postfix=ui->edtPostfix->text();
    job.backgroundOffset=ui->edtOffset->value();
    job.range_min=-1;
    if (!ui->chkFirstFrame->isChecked()) {
        job.range_min=ui->spinFirstFrame->value();
    }
    job.range_max=-1;
    if (!ui->chkLastFrame->isChecked()) {
        job.range_max=ui->spinLastFrame->value();
    }
    job.acf=ui->chkACF->isChecked();
    job.ccf=ui->chkCCF->isChecked();
    job.video=ui->chkVideo->isChecked();
    job.video_frames=qMax(2,ui->spinVideoFrames->value());
    job.statistics=true;
    job.statistics_frames=qMax(2, ui->spinStatistics->value());
    job.segments=ui->spinSegments->value();
    writeSettings();

    setEditControlsEnabled(false);
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
    updateFrameCount();
}

void QFRDRImagingFCSCorrelationDialog::frameRateChanged(double value) {
    if (value!=0) {
        disconnect(ui->edtFrameTime, SIGNAL(valueChanged(double)), this, SLOT(frameTimeChanged(double)));
        ui->edtFrameTime->setValue((1.0/value)*1e6);
        connect(ui->edtFrameTime, SIGNAL(valueChanged(double)), this, SLOT(frameTimeChanged(double)));
    }
    updateCorrelator();
    updateFrameCount();
}

void QFRDRImagingFCSCorrelationDialog::updateFrameCount() {
    double taumin=ui->edtFrameTime->value()*1e-6;
    double frames=frame_count;
    int64_t first=0;
    int64_t last=frame_count-1;
    if (!ui->chkFirstFrame->isChecked()) first= ui->spinFirstFrame->value();
    if (!ui->chkLastFrame->isChecked()) last= ui->spinLastFrame->value();
    frames=last-first+1;

    if (frame_count>0) {
        ui->labInputLength->setText(tr("length: %1 = %2 s").arg((int64_t)round(frames)).arg(frames*taumin));
        ui->labRange->setText(tr("= %1 frames").arg(round(frames)));
        ui->labSegments->setText(tr("segments of length: %1 (à %2 s)").arg(frame_count/ui->spinSegments->value()).arg(taumin*(double)(frame_count/ui->spinSegments->value())));
        ui->labStatistics->setText(tr("&Delta;<sub>Statistics</sub>= %1 &mu;s  => %2 values").arg(1e6*taumin*(double)ui->spinStatistics->value()).arg(frame_count/ui->spinStatistics->value()));
        ui->labVideo->setText(tr("&Delta;<sub>Video</sub>= %1 &mu;s  => %2 frames").arg(taumin*(double)ui->spinVideoFrames->value()).arg(frame_count/ui->spinVideoFrames->value()));
    } else {
        ui->labInputLength->setText(QString(""));
        ui->labRange->setText(QString(""));
        ui->labSegments->setText(QString(""));
        ui->labStatistics->setText(QString(""));
        ui->labVideo->setText(QString(""));
    }

}

void QFRDRImagingFCSCorrelationDialog::updateCorrelator() {
    int corrType=ui->cmbCorrelator->currentIndex();
    if (corrType==2) {
        ui->spinM->setEnabled(false);
        ui->spinM->setValue(2);
    } else {
        ui->spinM->setEnabled(true);
    }

    int S=ui->spinS->value();
    double P=ui->spinP->value();
    double m=ui->spinM->value();
    double taumin=ui->edtFrameTime->value();
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

void QFRDRImagingFCSCorrelationDialog::updateFromFile(bool readFrameCount) {
    QModernProgressDialog prg(this);
    prg.setWindowTitle(tr("imFCS: Correlator"));
    prg.setLabelText(tr("Reading image series information ... reading config file ..."));
    prg.open();
    QApplication::processEvents();
    QApplication::processEvents();
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    QString filename=QFileInfo(ui->edtImageFile->text()).absoluteFilePath();
    double frametime=ui->edtFrameTime->value();
    double baseline_offset=ui->edtOffset->value();

    //////////////////////////////////////////////////////////////////////////////////
    // now we search for a .configuration.ini file describing the selected file
    //////////////////////////////////////////////////////////////////////////////////
    QDir d=QFileInfo(filename).absoluteDir();
    QStringList nameFilters;
    nameFilters<<"*.ini";
    inputconfigfile="";
    d.setNameFilters(nameFilters);
    foreach (QString iniFile, d.entryList(QDir::Files)) {
        QApplication::processEvents();
        inputconfigfile=d.absoluteFilePath(iniFile);
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
                        //else ui->cmbFileformat->setCurrentIndex(-1);

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



    if (readFrameCount) {

        //////////////////////////////////////////////////////////////////////////////////
        // we also try to open the file with an appropriate reader and read th number of frames in it
        //////////////////////////////////////////////////////////////////////////////////
        prg.setLabelText(tr("Reading image series information ... counting frames ..."));
        QApplication::processEvents();
        QApplication::processEvents();
        QFRDRImageReader* reader=NULL;
        bool OK=false;
        if (ui->cmbFileformat->currentIndex()>=0 && ui->cmbFileformat->currentIndex()<QFRDRImagingFCSCorrelationJobThread::getImageReaderCount()) {
            reader=QFRDRImagingFCSCorrelationJobThread::getImageReader(ui->cmbFileformat->currentIndex());
        }
        if (reader) {
            OK=reader->open(filename);
            if (OK)  {
                QApplication::processEvents();
                frame_count=reader->countFrames();
                if (frame_count>0) {
                    ui->spinLastFrame->setMaximum(frame_count-1);
                    ui->spinFirstFrame->setMaximum(frame_count-1);
                    ui->spinLastFrame->setValue(frame_count-1);
                    ui->spinVideoFrames->setMaximum(frame_count-1);
                    ui->spinVideoFrames->setValue(qMax(2,frame_count/1000));
                    ui->spinStatistics->setMaximum(frame_count-1);
                } else {
                    ui->spinLastFrame->setMaximum(10000000);
                    ui->spinFirstFrame->setMaximum(10000000);
                    ui->spinLastFrame->setValue(10000000);
                    ui->spinVideoFrames->setMaximum(10000000);
                    ui->spinVideoFrames->setValue(100);
                    ui->spinStatistics->setMaximum(10000000);
                }
            }
            delete reader;
        }
    }

    prg.close();
    QApplication::restoreOverrideCursor();
    updateFrameCount();
}


QStringList QFRDRImagingFCSCorrelationDialog::getFilesToAdd() const {
    return filesToAdd;
}
