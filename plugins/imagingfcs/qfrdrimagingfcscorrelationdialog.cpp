#include "qfrdrimagingfcscorrelationdialog.h"
#include "ui_qfrdrimagingfcscorrelationdialog.h"
#include "qfrdrimagingfcscorrelationjobthread.h"
#include <QtGui>
#include <QDebug>
#include "qmodernprogresswidget.h"
#include "qfrdrimagingfcsdataexplorer.h"
#include "qfrdrimagingfcstools.h"

#define UPDATE_TIMEOUT 250

QFRDRImagingFCSCorrelationDialog::QFRDRImagingFCSCorrelationDialog(QFPluginServices* pluginservices, ProgramOptions* opt, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFRDRImagingFCSCorrelationDialog)
{
    this->pluginServices=pluginservices;
    closing=false;
    image_width=image_height=1;
    this->options=opt;
    lastImagefileDir="";
    ui->setupUi(this);
    ui->edtFrameTime->setRange(1e-10,1e10);
    ui->edtFrameTime->setValue(10000);
    ui->edtFrameRate->setRange(1e-10,1e10);
    ui->edtFrameRate->setValue(100);
    ui->edtOffset->setRange(-1e10,1e10);
    ui->edtOffset->setValue(0);
    ui->edtDecayA->setCheckBounds(false, false);
    ui->edtDecayB->setCheckBounds(false, false);
    ui->spinDecay->setCheckBounds(false, false);
    ui->edtDecayA->setValue(1000);
    ui->edtDecayB->setValue(1000);
    ui->spinDecay->setValue(10000);
    ui->edtPostfix->addInsertContextMenuEntry(tr("insert %counter%"), QString("%counter%"));
    ui->edtPostfix->addInsertContextMenuEntry(tr("insert %s%"), QString("%s%"));
    ui->edtPostfix->addInsertContextMenuEntry(tr("insert %p%"), QString("%p%"));
    ui->edtPostfix->addInsertContextMenuEntry(tr("insert %m%"), QString("%m%"));
    ui->edtPostfix->addInsertContextMenuEntry(tr("insert %framerate%"), QString("%framerate%"));
    ui->edtPostfix->addInsertContextMenuEntry(tr("insert %frametime%"), QString("%frametime%"));
    ui->edtPostfix->addInsertContextMenuEntry(tr("insert %first%"), QString("%first%"));
    ui->edtPostfix->addInsertContextMenuEntry(tr("insert %last%"), QString("%last%"));
    ui->edtPostfix->addInsertContextMenuEntry(tr("insert %backoffset%"), QString("%backoffset%"));
    ui->edtPostfix->addInsertContextMenuEntry(tr("insert %segments%"), QString("%segments%"));
    ui->edtPostfix->addInsertContextMenuEntry(tr("insert %backcorrectionid%"), QString("%backcorrectionid%"));
    ui->edtPostfix->addInsertContextMenuEntry(tr("insert %correlatorid%"), QString("%correlatorid%"));
    ui->edtPostfix->addInsertContextMenuEntry(tr("insert %binning%"), QString("%binning%"));
    ui->edtPostfix->addInsertContextMenuEntry(tr("insert %backcorrection%"), QString("%backcorrection%"));
    ui->edtPostfix->addInsertContextMenuEntry(tr("insert %correlator%"), QString("%correlator%"));
    ui->edtPostfix->addInsertContextMenuEntry(tr("insert %bleach%"), QString("%bleach%"));
    //ui->edtPostfix->addInsertContextMenuEntry(tr("insert %%"), QString("%%"));

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

void QFRDRImagingFCSCorrelationDialog::on_btnDataExplorer_clicked() {
    QFRDRImagingFCSDataExplorer* explorer=new QFRDRImagingFCSDataExplorer(this);
    explorer->readSettings(*(pluginServices->getOptions()->getQSettings()), "imFCS/dataExplorer/");
    QFRDRImageReader* reader=NULL;
    QFRDRImageReader* readerRaw=NULL;
    if (ui->cmbFileformat->currentIndex()>=0 && ui->cmbFileformat->currentIndex()<QFRDRImagingFCSCorrelationJobThread::getImageReaderCount()) {
        reader=QFRDRImagingFCSCorrelationJobThread::getImageReader(ui->cmbFileformat->currentIndex());
        readerRaw=QFRDRImagingFCSCorrelationJobThread::getImageReader(ui->cmbFileformat->currentIndex());
    }
    if (reader) {
        explorer->setBleachDecay(ui->spinDecay->value());
        explorer->setBleachA(ui->edtDecayA->value());
        explorer->setBleachB(ui->edtDecayB->value());
        explorer->init(reader, readerRaw, ui->edtImageFile->text(), ui->chkFirstFrame->isChecked(), ui->spinFirstFrame->value(), ui->chkLastFrame->isChecked(), ui->spinLastFrame->value(), ui->chkCrop->isChecked(), ui->spinXFirst->value(), ui->spinXLast->value(), ui->spinYFirst->value(), ui->spinYLast->value(), ui->spinBinning->value());
        if (explorer->exec()==QDialog::Accepted) {
            ui->spinDecay->setValue(explorer->getBleachDecay());
            ui->chkCrop->setChecked(explorer->getUseCropping());
            ui->spinXFirst->setValue(explorer->getCropX0());
            ui->spinXLast->setValue(explorer->getCropX1());
            ui->spinYFirst->setValue(explorer->getCropY0());
            ui->spinYLast->setValue(explorer->getCropY1());
            ui->spinBinning->setValue(explorer->getBinning());
            ui->edtDecayA->setValue(explorer->getBleachA());
            ui->edtDecayB->setValue(explorer->getBleachB());
            ui->chkFirstFrame->setChecked(explorer->getUseFirst());
            ui->chkLastFrame->setChecked(explorer->getUseLast());
            ui->spinFirstFrame->setValue(explorer->getFirst());
            ui->spinLastFrame->setValue(explorer->getLast());
        }
        explorer->writeSettings(*(pluginServices->getOptions()->getQSettings()), "imFCS/dataExplorer/");
        reader->close();
        readerRaw->close();
        delete reader;
        delete readerRaw;
    } else {
        QMessageBox::critical(this, explorer->windowTitle(), tr("Could not read input file!"));
    }
    delete explorer;
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
            break;
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

void QFRDRImagingFCSCorrelationDialog::on_spinDecay_valueChanged(double val) {
    updateBleach();
}

void QFRDRImagingFCSCorrelationDialog::on_cmbCorrelator_currentIndexChanged(int idx) {
    updateCorrelator();
}

void QFRDRImagingFCSCorrelationDialog::on_cmbBackground_currentIndexChanged(int idx) {
    ui->label_16->setEnabled(idx>0);
    ui->edtOffset->setEnabled(idx>0);
    ui->edtBackgroundFile->setEnabled(idx>2);
    ui->label_31->setEnabled(idx>2);
    ui->btnSelectBackgroundFile->setEnabled(idx>2);
}

void QFRDRImagingFCSCorrelationDialog::on_cmbBleachType_currentIndexChanged(int idx) {
    ui->widBleach->setEnabled(ui->cmbBleachType->currentIndex()==2);
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

void QFRDRImagingFCSCorrelationDialog::on_btnSelectBackgroundFile_clicked() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Background Image Series File ..."), lastImagefileDir, imageFilters.join(";;"), &lastImagefileFilter);
    if (!fileName.isEmpty()) {
        lastImagefileDir=QFileInfo(fileName).dir().absolutePath();
        ui->edtBackgroundFile->setText(fileName);
        ui->edtBackgroundFile->setFocus(Qt::MouseFocusReason);
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
    options->getQSettings()->setValue("imaging_fcs/dlg_correlate/binning", ui->spinBinning->value());
    options->getQSettings()->setValue("imaging_fcs/dlg_correlate/crop", ui->chkCrop->isChecked());
    options->getQSettings()->setValue("imaging_fcs/dlg_correlate/crop_x0", ui->spinXFirst->value());
    options->getQSettings()->setValue("imaging_fcs/dlg_correlate/crop_x1", ui->spinXLast->value());
    options->getQSettings()->setValue("imaging_fcs/dlg_correlate/crop_y0", ui->spinYFirst->value());
    options->getQSettings()->setValue("imaging_fcs/dlg_correlate/crop_y1", ui->spinYLast->value());
    options->getQSettings()->setValue("imaging_fcs/dlg_correlate/dccf_deltax", ui->spinDistanceCCFDeltaX->value());
    options->getQSettings()->setValue("imaging_fcs/dlg_correlate/dccf_deltay", ui->spinDistanceCCFDeltaY->value());
    options->getQSettings()->setValue("imaging_fcs/dlg_correlate/dccf", ui->chkDistanceCCD->isChecked());
    options->getQSettings()->setValue("imaging_fcs/dlg_correlate/bleach", ui->cmbBleachType->currentIndex());
    options->getQSettings()->setValue("imaging_fcs/dlg_correlate/bleachConst", ui->spinDecay->value());
    options->getQSettings()->setValue("imaging_fcs/dlg_correlate/bleachA", ui->edtDecayA->value());
    options->getQSettings()->setValue("imaging_fcs/dlg_correlate/bleachB", ui->edtDecayB->value());
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
    ui->spinBinning->setValue(options->getQSettings()->value("imaging_fcs/dlg_correlate/binning", ui->spinBinning->value()).toInt());
    ui->chkCrop->setChecked(options->getQSettings()->value("imaging_fcs/dlg_correlate/crop", ui->chkCrop->isChecked()).toBool());
    ui->spinXFirst->setValue(options->getQSettings()->value("imaging_fcs/dlg_correlate/crop_x0", ui->spinXFirst->value()).toInt());
    ui->spinXLast->setValue(options->getQSettings()->value("imaging_fcs/dlg_correlate/crop_x1", ui->spinXLast->value()).toInt());
    ui->spinYFirst->setValue(options->getQSettings()->value("imaging_fcs/dlg_correlate/crop_y0", ui->spinYFirst->value()).toInt());
    ui->spinYLast->setValue(options->getQSettings()->value("imaging_fcs/dlg_correlate/crop_y1", ui->spinYLast->value()).toInt());
    ui->spinDistanceCCFDeltaX->setValue(options->getQSettings()->value("imaging_fcs/dlg_correlate/dccf_deltax", ui->spinDistanceCCFDeltaX->value()).toInt());
    ui->spinDistanceCCFDeltaY->setValue(options->getQSettings()->value("imaging_fcs/dlg_correlate/dccf_deltay", ui->spinDistanceCCFDeltaY->value()).toInt());
    ui->chkDistanceCCD->setChecked(options->getQSettings()->value("imaging_fcs/dlg_correlate/dccf", ui->chkDistanceCCD->isChecked()).toBool());
    ui->cmbBleachType->setCurrentIndex(options->getQSettings()->value("imaging_fcs/dlg_correlate/bleach", ui->cmbBleachType->currentIndex()).toInt());
    ui->spinDecay->setValue(options->getQSettings()->value("imaging_fcs/dlg_correlate/bleachConst", ui->spinDecay->value()).toDouble());
    ui->edtDecayA->setValue(options->getQSettings()->value("imaging_fcs/dlg_correlate/bleachA", ui->edtDecayA->value()).toDouble());
    ui->edtDecayB->setValue(options->getQSettings()->value("imaging_fcs/dlg_correlate/bleachB", ui->edtDecayB->value()).toDouble());

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
        //int started=0;
        while ((waitingThreads()>0)&&(runningThreads()<ui->spinProcesses->value()) /*&& (started<=0)*/) {
            startNextWaitingThread();
            //started++;
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
    //qDebug()<<"job.fileFormat="<<job.fileFormat<<"  "<<ui->cmbFileformat->count();
    job.backgroundCorrection=ui->cmbBackground->currentIndex();
    job.S=ui->spinS->value();
    job.P=ui->spinP->value();
    job.m=ui->spinM->value();
    job.frameTime=ui->edtFrameTime->value()*1e-6;
    job.addToProject=ui->chkAddToProject->isChecked();
    job.prefix=ui->edtPrefix->text();
    job.postfix=ui->edtPostfix->text();
    job.backgroundOffset=ui->edtOffset->value();
    job.filenameBackground=ui->edtBackgroundFile->text();
    //qDebug()<<job.filenameBackground;
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
    job.binning=ui->spinBinning->value();
    job.use_cropping=ui->chkCrop->isChecked();
    job.crop_x0=ui->spinXFirst->value();
    job.crop_x1=ui->spinXLast->value();
    job.crop_y0=ui->spinYFirst->value();
    job.crop_y1=ui->spinYLast->value();
    job.distanceCCF=ui->chkDistanceCCD->isChecked();
    job.DCCFDeltaX=ui->spinDistanceCCFDeltaX->value();
    job.DCCFDeltaY=ui->spinDistanceCCFDeltaY->value();
    job.bleach=ui->cmbBleachType->currentIndex();
    job.bleachDecay=ui->spinDecay->value();
    job.bleachA=ui->edtDecayA->value();
    job.bleachB=ui->edtDecayB->value();
    job.bleachAvgFrames=100;
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
    updateBleach();
}

void QFRDRImagingFCSCorrelationDialog::frameRateChanged(double value) {
    if (value!=0) {
        disconnect(ui->edtFrameTime, SIGNAL(valueChanged(double)), this, SLOT(frameTimeChanged(double)));
        ui->edtFrameTime->setValue((1.0/value)*1e6);
        connect(ui->edtFrameTime, SIGNAL(valueChanged(double)), this, SLOT(frameTimeChanged(double)));
    }
    updateCorrelator();
    updateFrameCount();
    updateBleach();
}

void QFRDRImagingFCSCorrelationDialog::updateImageSize() {
    int w=image_width;
    int h=image_height;

    if (image_width-1>0) ui->spinXFirst->setMaximum(image_width-1);
    if (image_width-1>0) ui->spinXLast->setMaximum(image_width-1);
    if (image_height-1>0) ui->spinYFirst->setMaximum(image_height-1);
    if (image_height-1>0) ui->spinYLast->setMaximum(image_height-1);
    if (image_width>0) ui->spinDistanceCCFDeltaX->setRange(-1*image_width, image_width);
    if (image_height>0) ui->spinDistanceCCFDeltaY->setRange(-1*image_height, image_height);

    if (ui->chkCrop->isChecked()) {
        w=fabs(ui->spinXLast->value()-ui->spinXFirst->value())+1;
        h=fabs(ui->spinYLast->value()-ui->spinYFirst->value())+1;
    }

    w=w/ui->spinBinning->value();
    h=h/ui->spinBinning->value();
    ui->labSize->setText(tr("input: %1&times;%2   output: %3&times;%4").arg(image_width).arg(image_height).arg(w).arg(h));
}

void QFRDRImagingFCSCorrelationDialog::updateFrameCount() {
    double taumin=ui->edtFrameTime->value()*1e-6;
    double frames=frame_count;
    int64_t first=0;
    int64_t last=frame_count-1;
    if (!ui->chkFirstFrame->isChecked()) first= ui->spinFirstFrame->value();
    if (!ui->chkLastFrame->isChecked()) last= ui->spinLastFrame->value();
    frames=last-first+1;

    if (frames>0) {
        ui->labInputLength->setText(tr("length: %1 = %2 s").arg((int64_t)round(frames)).arg(frames*taumin));
        ui->labRange->setText(tr("= %1 frames").arg(round(frames)));
        if (ui->spinSegments->value()!=0) ui->labSegments->setText(tr("segments of length: %1 (à %2 s)").arg(frames/ui->spinSegments->value()).arg(taumin*(double)(frames/ui->spinSegments->value())));
        else ui->labSegments->setText("");
        if (ui->spinStatistics->value()!=0) ui->labStatistics->setText(tr("&Delta;<sub>Statistics</sub>= %1 &mu;s  => %2 values").arg(1e6*taumin*(double)ui->spinStatistics->value()).arg(frames/ui->spinStatistics->value()));
        else ui->labStatistics->setText("");
        if (ui->spinVideoFrames->value()!=0) ui->labVideo->setText(tr("&Delta;<sub>Video</sub>= %1 &mu;s  => %2 frames").arg(1e6*taumin*(double)ui->spinVideoFrames->value()).arg(frames/ui->spinVideoFrames->value()));
        else ui->labVideo->setText(QString(""));
    } else {
        ui->labInputLength->setText(QString(""));
        ui->labRange->setText(QString(""));
        ui->labSegments->setText(QString(""));
        ui->labStatistics->setText(QString(""));
        ui->labVideo->setText(QString(""));
    }

}

void QFRDRImagingFCSCorrelationDialog::updateBleach() {
    ui->labDecay->setTextFormat(Qt::RichText);
    ui->labDecay->setText(tr("&tau;<sub>Bleach</sub> = %1 s").arg((double)ui->spinDecay->value()*ui->edtFrameTime->value()/1e6));
}

void QFRDRImagingFCSCorrelationDialog::updateCorrelator() {
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
    QString backgroundF=ui->edtBackgroundFile->text();

    //////////////////////////////////////////////////////////////////////////////////
    // now we search for a .configuration.ini file describing the selected file
    //////////////////////////////////////////////////////////////////////////////////
    QDir d=QFileInfo(filename).absoluteDir();
    QStringList nameFilters;
    nameFilters<<"*.ini";
    nameFilters<<"*.cfg";
    nameFilters<<"*.txt";
    inputconfigfile="";
    d.setNameFilters(nameFilters);
    // could be that the absolute pathes are wrong. In this case we try to get a second guess by finding a file which
    // equals in the name, but not the directory ... the user can correct the data anyways. This second guess is stored
    // in these variables:
    double sframetime=frametime;
    double sbaseline_offset=baseline_offset;
    QString sbackgroundF=backgroundF;
    bool hasSecond=false;
    bool hasFirst=false;
    bool sIsTiff=false;
    foreach (QString iniFile, d.entryList(QDir::Files)) {
        //QApplication::processEvents();
        inputconfigfile=d.absoluteFilePath(iniFile);
        QSettings set(d.absoluteFilePath(iniFile), QSettings::IniFormat);
        int fcnt=set.value("files/count", 0).toInt();
        if (fcnt>0) {
            bool foundSecond=false;
            for (int f=0; f<fcnt; f++) {
                QString fn=set.value("files/name"+QString::number(f), "").toString();
                QString ft=set.value("files/type"+QString::number(f), "").toString();
                QString fd=set.value("files/description"+QString::number(f), "").toString();
                if (!fn.isEmpty()) {
                    QString fnAbs=d.absoluteFilePath(fn);
                    //qDebug()<<"fn="<<fn<<"  fnAbs="<<fnAbs<<"  fn(filename)="<<QFileInfo(filename).fileName()<<"  fn(fnAbs)="<<QFileInfo(fnAbs).fileName();
                    if (fnAbs==filename) {
                        hasFirst=true;
                        if (ft.toLower().simplified().startsWith("tiff")) ui->cmbFileformat->setCurrentIndex(0);
                    } else if (QFileInfo(fnAbs).fileName()==QFileInfo(filename).fileName()) {
                        foundSecond=true;
                        if (ft.toLower().simplified().startsWith("tiff")) sIsTiff=true;
                    }
                }
            }

            if (hasFirst) {
                readB040SPIMExperimentConfigFile(set, frametime, baseline_offset, backgroundF, image_width, image_height);
                //qDebug()<<"read first  "<<frametime<<baseline_offset<<backgroundF<<image_width<<image_height;
            } else if (foundSecond) {
                readB040SPIMExperimentConfigFile(set, sframetime, sbaseline_offset, sbackgroundF, image_width, image_height);
                //qDebug()<<"read second  "<<sframetime<<sbaseline_offset<<sbackgroundF<<image_width<<image_height;
                hasSecond=true;
            }
        }
        if (hasFirst) break;
    }

    // if we didn't find a second guess, we try to find a config file with the same
    // basename + one of a set of extensions (newsuffix list) and try to read info from
    // that.
    if (!hasSecond) {
        QString suffix=QFileInfo(filename).suffix();
        QString cfgname;

        QStringList newsuffix;
        newsuffix<<"ini"
                 <<"configuration.ini"
                 <<"settings.ini"
                 <<"settings.txt"
                 <<"cfg";
        for (int i=0; i<newsuffix.size(); i++) {
            cfgname=filename.left(filename.size()-suffix.size())+newsuffix[i];
            if (QFile::exists(cfgname)) {
                QSettings set(cfgname, QSettings::IniFormat);
                readB040SPIMExperimentConfigFile(set, sframetime, sbaseline_offset, sbackgroundF, image_width, image_height);
                //qDebug()<<"read third  "<<sframetime<<sbaseline_offset<<sbackgroundF<<image_width<<image_height;
                break;
            }
        }


    }

    // if there is no direct match, we take the second best, if there is one
    if (!hasFirst && hasSecond)  {
        frametime=sframetime;
        baseline_offset=sbaseline_offset;
        backgroundF=sbackgroundF;
        if (sIsTiff) ui->cmbFileformat->setCurrentIndex(0);
    }


    // SET THE FRAMETIME/RATE
    ui->edtFrameTime->setValue(frametime);
    frameTimeChanged(frametime);
    ui->edtOffset->setValue(baseline_offset);
    ui->edtBackgroundFile->setText(backgroundF);



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
                image_width=reader->frameWidth();
                image_height=reader->frameHeight();
                frame_count=reader->countFrames();
                if (frame_count>0) {
                    ui->spinLastFrame->setMaximum(frame_count-1);
                    ui->spinFirstFrame->setMaximum(frame_count-1);
                    if (ui->chkLastFrame->isChecked()) ui->spinLastFrame->setValue(frame_count-1);
                    ui->spinVideoFrames->setMaximum(frame_count-1);
                    ui->spinVideoFrames->setValue(qMax(2,frame_count/1000));
                    ui->spinStatistics->setMaximum(frame_count-1);
                } else {
                    ui->spinLastFrame->setMaximum(10000000);
                    ui->spinFirstFrame->setMaximum(10000000);
                    if (ui->chkLastFrame->isChecked()) ui->spinLastFrame->setValue(10000000);
                    ui->spinVideoFrames->setMaximum(10000000);
                    ui->spinVideoFrames->setValue(100);
                    ui->spinStatistics->setMaximum(10000000);
                }
            }
            reader->close();
            delete reader;
        }
    }

    prg.close();
    QApplication::restoreOverrideCursor();

    if (image_width<=1) image_width=1;
    if (image_height<=1) image_height=1;

    updateFrameCount();
    updateImageSize();
}


QStringList QFRDRImagingFCSCorrelationDialog::getFilesToAdd() const {
    return filesToAdd;
}

