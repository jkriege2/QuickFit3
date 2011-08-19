#include "qfespimb040mainwindow.h"
#include <QtGui>
#include <tiffio.h>
#include "libtiff_tools.h"




QFESPIMB040MainWindow::QFESPIMB040MainWindow(QFExtensionServices* pluginServices, QWidget* parent):
    QWidget(parent, Qt::Dialog|Qt::WindowMaximizeButtonHint|Qt::WindowCloseButtonHint|Qt::WindowSystemMenuHint)
{
    camConfig1=NULL;
    camConfig2=NULL;
    widImageStack=NULL;
    m_pluginServices=pluginServices;
    // create widgets and actions
    createWidgets(pluginServices->getExtensionManager());
    setWindowTitle("B040 SPIM Control");
    setWindowIcon(QIcon(":/spimb040_logo.png"));
}

QFESPIMB040MainWindow::~QFESPIMB040MainWindow()
{
    if (camConfig1) {
        camConfig1->close();
        delete camConfig1;
        camConfig1=NULL;
    }
    if (camConfig2) {
        camConfig2->close();
        delete camConfig2;
        camConfig2=NULL;
    }
}

void QFESPIMB040MainWindow::loadSettings(ProgramOptions* settings) {
    jkloadWidgetGeometry((*settings->getQSettings()), this, "plugin_spim_b040/");
    if (camConfig1) camConfig1->loadSettings(settings, "plugin_spim_b040/cam_config1/");
    if (camConfig2) camConfig2->loadSettings(settings, "plugin_spim_b040/cam_config2/");
    if (sampleStages) sampleStages->loadSettings(settings, "plugin_spim_b040/sample_stages/");
    if (widImageStack) widImageStack->loadSettings((*settings->getQSettings()), "plugin_spim_b040/image_stack/");

    chkAcquisitionUseCam1->setChecked((*settings->getQSettings()).value("plugin_spim_b040/acquisition/use_cam1", true).toBool());
    chkAcquisitionUseCam2->setChecked((*settings->getQSettings()).value("plugin_spim_b040/acquisition/use_cam2", true).toBool());
    edtAcquisitionPrefix1->setText((*settings->getQSettings()).value("plugin_spim_b040/acquisition/prefix1", "camera1").toString());
    edtAcquisitionPrefix2->setText((*settings->getQSettings()).value("plugin_spim_b040/acquisition/prefix2", "camera2").toString());
}

void QFESPIMB040MainWindow::storeSettings(ProgramOptions* settings) {
    jksaveWidgetGeometry((*settings->getQSettings()), this, "plugin_spim_b040/");
    if (camConfig1) camConfig1->storeSettings(settings, "plugin_spim_b040/cam_config1/");
    if (camConfig2) camConfig2->storeSettings(settings, "plugin_spim_b040/cam_config2/");
    if (sampleStages) sampleStages->storeSettings(settings, "plugin_spim_b040/sample_stages/");
    if (widImageStack) widImageStack->storeSettings((*settings->getQSettings()), "plugin_spim_b040/image_stack/");

    (*settings->getQSettings()).setValue("plugin_spim_b040/acquisition/use_cam1", chkAcquisitionUseCam1->isChecked());
    (*settings->getQSettings()).setValue("plugin_spim_b040/acquisition/prefix1", edtAcquisitionPrefix1->text());
    (*settings->getQSettings()).setValue("plugin_spim_b040/acquisition/use_cam2", chkAcquisitionUseCam2->isChecked());
    (*settings->getQSettings()).setValue("plugin_spim_b040/acquisition/prefix2", edtAcquisitionPrefix2->text());
}

void QFESPIMB040MainWindow::closeEvent ( QCloseEvent * event ) {
    if (camConfig1) {
        camConfig1->close();
    }
    if (camConfig2) {
        camConfig2->close();
    }
}

void QFESPIMB040MainWindow::showEvent( QShowEvent * event )  {
   if (camConfig1) {
        camConfig1->show();
    }
    if (camConfig2) {
        camConfig2->show();
    }
}

void QFESPIMB040MainWindow::createWidgets(QFExtensionManager* extManager) {
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // create main layout
    ////////////////////////////////////////////////////////////////////////////////////////////////
    QGridLayout* mainlayout=new QGridLayout(this);
    setLayout(mainlayout);



    ////////////////////////////////////////////////////////////////////////////////////////////////
    // create input widgets for camera devices
    ////////////////////////////////////////////////////////////////////////////////////////////////
    logMain=new QtLogFile(this);
    mainlayout->addWidget(logMain, 3,0,1,2);


    ////////////////////////////////////////////////////////////////////////////////////////////////
    // create camera config widgets
    ////////////////////////////////////////////////////////////////////////////////////////////////
    camConfig1=new QFESPIMB040CameraConfig(this, 0, m_pluginServices);
    mainlayout->addWidget(camConfig1, 0, 0);
    camConfig2=new QFESPIMB040CameraConfig(this, 1, m_pluginServices);
    mainlayout->addWidget(camConfig2, 0, 1);

    connect(camConfig1, SIGNAL(configFilesChanged()), camConfig2, SLOT(rereadConfigCombos()));
    connect(camConfig2, SIGNAL(configFilesChanged()), camConfig1, SLOT(rereadConfigCombos()));

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // create sample stage widget
    ////////////////////////////////////////////////////////////////////////////////////////////////
    sampleStages=new QFESPIMB040SampleStageConfig(this, m_pluginServices);
    mainlayout->addWidget(sampleStages, 1, 0);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // create tab widget for different acquisition modes
    ////////////////////////////////////////////////////////////////////////////////////////////////
    tabAcquisition=new QTabWidget(this);
    mainlayout->addWidget(tabAcquisition, 2,0,1,2);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // create tab for image series acquisition
    ////////////////////////////////////////////////////////////////////////////////////////////////
    QWidget* w=new QWidget(tabAcquisition);
    QGridLayout* grid=new QGridLayout(w);
    w->setLayout(grid);
    grid->addWidget(new QLabel(tr("<b>counter:</b>"), w), 0,0);
    spinAcquisitionCount=new QSpinBox(w);
    spinAcquisitionCount->setRange(0,100000);
    spinAcquisitionCount->setValue(0);
    spinAcquisitionCount->setMaximumWidth(100);
    grid->addWidget(spinAcquisitionCount, 0,1,1,2);
    grid->addWidget(new QLabel(tr("<b>camera 1:</b>"), w), 2,0);
    grid->addWidget(new QLabel(tr("<b>camera 2:</b>"), w), 3,0);
    grid->addWidget(new QLabel(tr("<b>use:</b>"), w), 1,1);
    grid->addWidget(new QLabel(tr("<b>filename prefix:</b>"), w), 1,2);
    chkAcquisitionUseCam1=new QCheckBox(w);
    grid->addWidget(chkAcquisitionUseCam1, 2,1);
    chkAcquisitionUseCam2=new QCheckBox(w);
    grid->addWidget(chkAcquisitionUseCam2, 3,1);
    edtAcquisitionPrefix1=new QLineEdit(w);
    grid->addWidget(edtAcquisitionPrefix1, 2,2);
    connect(chkAcquisitionUseCam1,SIGNAL(clicked(bool)), edtAcquisitionPrefix1, SLOT(setEnabled(bool)));
    edtAcquisitionPrefix2=new QLineEdit(w);
    grid->addWidget(edtAcquisitionPrefix2, 3,2);
    connect(chkAcquisitionUseCam2,SIGNAL(clicked(bool)), edtAcquisitionPrefix2, SLOT(setEnabled(bool)));

    grid->addWidget(new QLabel(tr("<i>%counter% will be replace by the value of the above counter spin box!</i>"), w), 4,2);
    btnAcquire=new QPushButton(tr("&Acquire"), w);
    btnAcquire->setMaximumWidth(100);
    grid->addWidget(btnAcquire, 5, 2);
    connect(btnAcquire, SIGNAL(clicked()), this, SLOT(doAcquisition()));
    QFrame* f=new QFrame(this);
    f->setFrameStyle(QFrame::HLine | QFrame::Raised);
    grid->addWidget(f, 6,0, 1,3);
    QLabel* l=new QLabel(w);
    l->setWordWrap(true);
    l->setText(tr("<img src=\":/lib/help.png\">&nbsp;<b>Help:</b><br>"
                  "This acquisition mode sets one or two cameras into the above selected acquisition mode. "
                  "Then it runs until both acquisitions are complete (they do not have to have the same length). "
                  "The results are stored by the camera plugins into files with the given prefixes. In addition "
                  "a file <i>prefix.settings.ini</i> is create (for every camera separately), which contains the "
                  "acqusition settings and serves to document the acquisition."));
    grid->addWidget(l, 7,0, 1,3);

    tabAcquisition->addTab(w, tr("Image &Series"));
    chkAcquisitionUseCam1->setEnabled(true);
    chkAcquisitionUseCam2->setEnabled(true);



    ////////////////////////////////////////////////////////////////////////////////////////////////
    // create tab for image stack acquisition
    ////////////////////////////////////////////////////////////////////////////////////////////////
    widImageStack=new QFESPIMB040ImageStackConfigWidget(this, m_pluginServices);
    tabAcquisition->addTab(widImageStack, tr("Image S&tack"));
    connect(widImageStack, SIGNAL(doStack()), this, SLOT(doImageStack()));
}

#define IMAGESTACK_ERROR(message) \
    log_error(QString("  - ")+(message)+QString("\n")); \
    QMessageBox::critical(this, tr("B040SPIM: Image Stack Acquisition Error"), (message));


void QFESPIMB040MainWindow::doImageStack() {
    if (!(widImageStack->use1() || widImageStack->use2())) {
        QMessageBox::critical(this, tr("B040SPIM: Image Stack Acquisition"), tr("Cannot start image acquisition: No camera selected!"));
        return;
    }


    log_text(tr("starting image stack acquisition:\n"));

    bool ok=true;

    //////////////////////////////////////////////////////////////////////////////////////
    // CHECK/CONNECT SELECTED STAGE
    //////////////////////////////////////////////////////////////////////////////////////
    QFExtensionLinearStage* stage=widImageStack->stage();
    int stageAxis=widImageStack->currentAxisID();
    if (!stage) {
        IMAGESTACK_ERROR(tr("no stage selected"));
        QMessageBox::critical(this, tr("B040SPIM: Image Stack Acquisition"), tr("Cannot start image acquisition: No stage selected!"));
        return;
    }
    if (ok && (!stage->isConnected(stageAxis))) {
        log_text(tr("  - connecting to stage '%1', axis %2!\n").arg(widImageStack->stageExtension()->getName()).arg(stageAxis));
        stage->connectDevice(stageAxis);
    }
    if (ok && (!stage->isConnected(stageAxis)))  {
        ok=false;
        IMAGESTACK_ERROR(tr("error connecting to stage '%1', axis %2").arg(widImageStack->stageExtension()->getName()).arg(stageAxis));
    }
    if (ok) stage->setJoystickActive(stageAxis, false);

    if (!ok) return;

    //////////////////////////////////////////////////////////////////////////////////////
    // LOCK/INIT CAMERA 1
    //////////////////////////////////////////////////////////////////////////////////////
    bool useCam1=false;
    QFExtension* extension1=NULL;
    QFExtensionCamera* ecamera1=NULL;
    int camera1=0;
    QString acquisitionSettingsFilename1="", previewSettingsFilename1="";
    QString acquisitionPrefix1=widImageStack->prefix1();
    QString TIFFFIlename1=acquisitionPrefix1+".tif";
    TIFF* tiff1=NULL;
    if (widImageStack->use1()) {
        if (!(useCam1=camConfig1->lockCamera(&extension1, &ecamera1, &camera1, &acquisitionSettingsFilename1, &previewSettingsFilename1))) {
            IMAGESTACK_ERROR(tr("error locking camer 1!\n"));
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////
    // LOCK/INIT CAMERA 2
    //////////////////////////////////////////////////////////////////////////////////////
    bool useCam2=false;
    QFExtension* extension2=NULL;
    QFExtensionCamera* ecamera2=NULL;
    QString acquisitionSettingsFilename2="", previewSettingsFilename2="";
    QString acquisitionPrefix2=widImageStack->prefix2();
    QString TIFFFIlename2=acquisitionPrefix2+".tif";
    TIFF* tiff2=NULL;
    int camera2=0;
    if (widImageStack->use2()) {
        if(!(useCam2=camConfig2->lockCamera(&extension2, &ecamera2, &camera2, &acquisitionSettingsFilename2, &previewSettingsFilename2))) {
            IMAGESTACK_ERROR(tr("error locking camer 2!\n"));
        }
    }

    if (ok && !useCam1 && !useCam2) {
        IMAGESTACK_ERROR(tr("Cannot start image acquisition: No camera selected, or both cameras not usable!"));
        ok=false;
    }

    QProgressDialog progress(tr("Image Stack Acquisition"), tr("&Cancel"), 0, 100, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(0);
    progress.setValue(0);

    //////////////////////////////////////////////////////////////////////////////////////
    // PREPARE CAMERA 1
    //////////////////////////////////////////////////////////////////////////////////////
    int width1=0, height1=0;
    uint32_t* buffer1=NULL;
    if (ok && useCam1) {
        progress.setLabelText(tr("preparing camera 1 ..."));
        QApplication::processEvents();
        QSettings settings(acquisitionSettingsFilename1, QSettings::IniFormat);
        ecamera1->useCameraSettings(camera1, settings);
        log_text(tr("  - prepared camer 1!\n"));
        width1=ecamera1->getImageWidth(camera1);
        height1=ecamera1->getImageHeight(camera1);
        buffer1=(uint32_t*)calloc(width1*height1, sizeof(uint32_t));
        if (!buffer1) {
            ok=false;
            IMAGESTACK_ERROR(tr("could not allocate image buffer for camera 1!\n"));
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////
    // PREPARE CAMERA 2
    //////////////////////////////////////////////////////////////////////////////////////
    int width2=0, height2=0;
    uint32_t* buffer2=NULL;
    if (ok && useCam2) {
        progress.setLabelText(tr("preparing camera 2 ..."));
        QApplication::processEvents();
        QSettings settings(acquisitionSettingsFilename2, QSettings::IniFormat);
        ecamera2->useCameraSettings(camera2, settings);
        log_text(tr("  - prepared camera 2!\n"));
        width2=ecamera2->getImageWidth(camera2);
        height2=ecamera2->getImageHeight(camera2);
        buffer2=(uint32_t*)calloc(width2*height2, sizeof(uint32_t));
        if (!buffer2) {
            ok=false;
            IMAGESTACK_ERROR(tr("could not allocate image buffer for camera 2!\n"));
        }
    }


    //////////////////////////////////////////////////////////////////////////////////////
    // OPEN OUTPUT TIFF FILES
    //////////////////////////////////////////////////////////////////////////////////////
    progress.setLabelText(tr("opening output files ..."));
    QApplication::processEvents();
    if (ok && useCam1) {
        QDir().mkpath(QFileInfo(TIFFFIlename1.toAscii().data()).absolutePath());
        tiff1=TIFFOpen(TIFFFIlename1.toAscii().data(), "w");
        if (!tiff1) {
            ok=false;
            IMAGESTACK_ERROR(tr("error opening TIFF file (camera 1) '%1'!").arg(TIFFFIlename1));
        }
    }
    if (ok && useCam2) {
        QDir().mkpath(QFileInfo(TIFFFIlename2.toAscii().data()).absolutePath());
        tiff2=TIFFOpen(TIFFFIlename2.toAscii().data(), "w");
        if (!tiff2) {
            ok=false;
            IMAGESTACK_ERROR(tr("error opening TIFF file (camera 2) '%1'!").arg(TIFFFIlename2));
        }
    }


    double stageStart=widImageStack->stackStart();
    double stageDelta=widImageStack->stackDelta();
    int images=widImageStack->stackCount();


    if (progress.wasCanceled()) {
        log_warning(tr("canceled by user!\n"));
        ok=false;
    }


    //////////////////////////////////////////////////////////////////////////////////////
    // ACQUIRE IMAGE, MOVE, ACQUIRE IMAGE, MOVE, ...
    //    images are stored in TIFF files using libtiff and they are (possibly) downscaled to 16-bit
    //////////////////////////////////////////////////////////////////////////////////////
    QList<QVariant> positions;
    QTime timAcquisition=QTime::currentTime();
    QDateTime timStart;
    double duration=0;
    if (ok) {
        progress.setLabelText(tr("acquiring images ..."));
        bool running=ok;
        int prog=0;
        double newPos=stageStart;
        while (running && (prog<images)) {

            log_text(tr("  - moving to position %1 micron ...").arg(newPos));
            stage->move(stageAxis, newPos);
            while (stage->getAxisState(stageAxis)==QFExtensionLinearStage::Moving) {
                progress.setLabelText(tr("moving stage to %1 microns (distance: %2) ...").arg(newPos).arg(fabs(stage->getPosition(stageAxis)-newPos)));
                QApplication::processEvents();
                if (progress.wasCanceled()) break;
                QApplication::processEvents();
            }
            if (ok) {
                if (stage->getAxisState(stageAxis)==QFExtensionLinearStage::Ready) {
                    log_text(tr(" OK\n"));
                    positions.append(stage->getPosition(stageAxis));
                } else {
                    IMAGESTACK_ERROR(tr("error moving to position %1 micron!\n").arg(newPos));
                    ok=false;
                }
            }


            QApplication::processEvents();
            QApplication::processEvents();

            if (progress.wasCanceled()) {
                running=false;
                log_warning(tr("  - acquisition canceled by user!\n"));
            } else {
                log_text(tr("acquiring images (%2/%3) @ %1 micron ...").arg(fabs(stage->getPosition(stageAxis))).arg(prog+1).arg(images));
                progress.setLabelText(tr("acquiring images (%2/%3) @ %1 micron ...").arg(fabs(stage->getPosition(stageAxis))).arg(prog+1).arg(images));
                QApplication::processEvents();
                if (prog==0) {
                    timAcquisition.start();
                    timStart=QDateTime::currentDateTime();
                }
                if (useCam1) {
                    if (ecamera1->acquire(camera1, buffer1)) {
                        TIFFTWriteUint16from32(tiff1, buffer1, width1, height1);
                        TIFFWriteDirectory(tiff1);
                    } else {
                        ok=false;
                        IMAGESTACK_ERROR(tr("error acquiring image %1/%2 on camera 1!\n").arg(prog+1).arg(images));
                    }
                }
                QApplication::processEvents();
                if (useCam2) {
                    if (ecamera2->acquire(camera2, buffer2)) {
                        TIFFTWriteUint16from32(tiff2, buffer2, width2, height2);
                        TIFFWriteDirectory(tiff2);
                    } else {
                        ok=false;
                        IMAGESTACK_ERROR(tr("error acquiring image %1/%2 on camera 2!\n").arg(prog+1).arg(images));
                    }
                }

                QApplication::processEvents();
            }
            if (!ok) running=false;

            prog++;
            newPos+=stageDelta;
            progress.setValue((int)round((double)prog/(double)images*100.0));
            QApplication::processEvents();
        }
        duration=timAcquisition.elapsed()/1000.0;
    }
    progress.setValue(100);


    //////////////////////////////////////////////////////////////////////////////////////
    // close tiff files and free buffers
    //////////////////////////////////////////////////////////////////////////////////////
    progress.setLabelText(tr("closing output files ..."));
    QApplication::processEvents();
    if (tiff1) TIFFClose(tiff1);
    if (tiff2) TIFFClose(tiff2);
    tiff1=tiff2=NULL;
    if (buffer1) free(buffer1);
    if (buffer2) free(buffer2);
    buffer1=buffer2=NULL;

    //////////////////////////////////////////////////////////////////////////////////////
    // collect acquisition data common to all cameras
    //////////////////////////////////////////////////////////////////////////////////////
    QMap<QString, QVariant> acquisitionDescription;
    if (ok) {
        acquisitionDescription["type"]="stack";
        acquisitionDescription["axis"]="other";
        if (sampleStages->getXStage()==stage && sampleStages->getXStageAxis()==stageAxis) acquisitionDescription["axis"]="x";
        if (sampleStages->getYStage()==stage && sampleStages->getYStageAxis()==stageAxis) acquisitionDescription["axis"]="y";
        if (sampleStages->getZStage()==stage && sampleStages->getZStageAxis()==stageAxis) acquisitionDescription["axis"]="z";
        acquisitionDescription["stage_name"]=widImageStack->stageExtension()->getName();
        acquisitionDescription["stage_axis"]=stageAxis;
        acquisitionDescription["stack_start"]=stageStart;
        acquisitionDescription["stack_delta"]=stageDelta;
        acquisitionDescription["sequence_length"]=images;
        acquisitionDescription["start_time"]=timStart;
        acquisitionDescription["duration"]=duration;
        acquisitionDescription["stack_positions"]=positions;
    }


    //////////////////////////////////////////////////////////////////////////////////////
    // write image stack properties to files, also collects camera specific information
    //////////////////////////////////////////////////////////////////////////////////////
    if (ok && useCam1) {
        QMap<QString, QVariant> acquisitionDescription1=acquisitionDescription;
        QList<QFExtensionCamera::AcquititonFileDescription> files;
        QFExtensionCamera::AcquititonFileDescription d;
        d.name=TIFFFIlename1;
        d.description="image stack from camera 1";
        d.type="TIFF16";
        files.append(d);

        acquisitionDescription1["image_width"]=width1;
        acquisitionDescription1["image_height"]=height1;
        acquisitionDescription1["pixel_width"]=ecamera1->getPixelWidth(camera1);
        acquisitionDescription1["pixel_height"]=ecamera1->getPixelHeight(camera1);
        acquisitionDescription1["exposure_time"]=ecamera1->getExposureTime(camera1);
        acquisitionDescription1["camera_model"]=ecamera1->getCameraName(camera1);
        acquisitionDescription1["sensor_model"]=ecamera1->getCameraSensorName(camera1);

        log_text(tr("  - writing acquisition description 1 ..."));
        savePreviewDescription(extension1, ecamera1, camera1, acquisitionPrefix1, acquisitionDescription1, files);
        log_text(tr(" DONE!\n"));
    }
    if (ok && useCam2) {
        QMap<QString, QVariant> acquisitionDescription2=acquisitionDescription;
        QList<QFExtensionCamera::AcquititonFileDescription> files;
        QFExtensionCamera::AcquititonFileDescription d;
        d.name=TIFFFIlename1;
        d.description="image stack from camera 2";
        d.type="TIFF16";
        files.append(d);

        acquisitionDescription2["image_width"]=width2;
        acquisitionDescription2["image_height"]=height2;
        acquisitionDescription2["pixel_width"]=ecamera2->getPixelWidth(camera2);
        acquisitionDescription2["pixel_height"]=ecamera2->getPixelHeight(camera2);
        acquisitionDescription2["camera_model"]=ecamera2->getCameraName(camera2);
        acquisitionDescription2["sensor_model"]=ecamera2->getCameraSensorName(camera2);
        acquisitionDescription2["exposure_time"]=ecamera2->getExposureTime(camera2);

        log_text(tr("  - writing acquisition description 2 ..."));
        savePreviewDescription(extension2, ecamera2, camera2, acquisitionPrefix2, acquisitionDescription2, files);
        log_text(tr(" DONE!\n"));
    }

    //////////////////////////////////////////////////////////////////////////////////////
    // release cameras
    //////////////////////////////////////////////////////////////////////////////////////
    if (useCam1) {
        camConfig1->releaseCamera();
        log_text(tr("  - released camera 1!\n"));
    }
    if (useCam2) {
        camConfig2->releaseCamera();
        log_text(tr("  - released camera 2!\n"));
    }


    if (ok) log_text(tr("image stack acquisition DONE!\n"));
    if (ok) widImageStack->incCounter();
}


#define ACQUISITION_ERROR(message) \
    log_error(QString("  - ")+(message)+QString("\n")); \
    QMessageBox::critical(this, tr("B040SPIM: Image Stack Acquisition Error"), (message));

void QFESPIMB040MainWindow::doAcquisition() {
    if (!(chkAcquisitionUseCam1->isChecked() || chkAcquisitionUseCam2->isChecked())) return;

    int cnt=spinAcquisitionCount->value();
    spinAcquisitionCount->setValue(cnt+1);

    bool ok=true;

    //////////////////////////////////////////////////////////////////////////////////////
    // lock cameras for use by this routine
    //////////////////////////////////////////////////////////////////////////////////////
    bool useCam1=false;
    QFExtension* extension1=NULL;
    QFExtensionCamera* ecamera1=NULL;
    int camera1=0;
    QString acquisitionSettingsFilename1="", previewSettingsFilename1="";
    QString acquisitionPrefix1=edtAcquisitionPrefix1->text();
    acquisitionPrefix1=acquisitionPrefix1.replace("%counter%", QString::number(cnt));
    QString acquisitionPrefix2=edtAcquisitionPrefix2->text();
    acquisitionPrefix2=acquisitionPrefix2.replace("%counter%", QString::number(cnt));
    if (chkAcquisitionUseCam1->isChecked()) {
        useCam1=camConfig1->lockCamera(&extension1, &ecamera1, &camera1, &acquisitionSettingsFilename1, &previewSettingsFilename1);
    }
    bool useCam2=false;
    QFExtension* extension2=NULL;
    QFExtensionCamera* ecamera2=NULL;
    QString acquisitionSettingsFilename2="", previewSettingsFilename2="";
    int camera2=0;
    if (chkAcquisitionUseCam2->isChecked()) {
        useCam2=camConfig2->lockCamera(&extension2, &ecamera2, &camera2, &acquisitionSettingsFilename2, &previewSettingsFilename2);
    }

    QProgressDialog progress(tr("Image Series Acquisition"), tr("&Cancel"), 0, 100, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(0);
    progress.setValue(0);
    log_text(tr("starting image series acquisition:\n"));

    //////////////////////////////////////////////////////////////////////////////////////
    // prepare cameras  (set camera settings)
    //////////////////////////////////////////////////////////////////////////////////////
    if (ok && useCam1) {
        progress.setLabelText(tr("preparing camera 1 ..."));
        QApplication::processEvents();
        QSettings settings(acquisitionSettingsFilename1, QSettings::IniFormat);
        ok=ecamera1->prepareAcquisition(camera1, settings, acquisitionPrefix1);
        if (ok) {
            log_text(tr("  - prepared camer 1!\n"));
        } else {
            ACQUISITION_ERROR(tr("  - error preparing camer 1!\n"));
        }
    }
    if (ok && useCam2) {
        progress.setLabelText("preparing camera 2 ...");
        QApplication::processEvents();
        QSettings settings(acquisitionSettingsFilename2, QSettings::IniFormat);
        ok=ecamera2->prepareAcquisition(camera2, settings, acquisitionPrefix2);
        if (ok) {
            log_text(tr("  - prepared camer 2!\n"));
        } else {
            ACQUISITION_ERROR(tr("  - error preparing camer 2!\n"));
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////
    // start acquisition and wait until finished
    //////////////////////////////////////////////////////////////////////////////////////
    if (ok) {
        progress.setLabelText(tr("acquiring images ..."));
        log_text(tr("  - acquiring images!\n"));
        if (useCam1) {
            ok=ecamera1->startAcquisition(camera1);
            if (!ok) {
                ACQUISITION_ERROR(tr("  - error starting acquisition on camera 1!\n"));
            }
        }
        if (ok && useCam2) {
            ok=ecamera2->startAcquisition(camera2);
            if (!ok) {
                ACQUISITION_ERROR(tr("  - error starting acquisition on camera 1!\n"));
            }
        }
        bool running=ok;
        while (running) {
            int prog1=99, prog2=99;
            if (useCam1) prog1=ecamera1->getAcquisitionProgress(camera1);
            if (useCam2) prog2=ecamera2->getAcquisitionProgress(camera2);
            progress.setValue(qMin(prog1,prog2));

            QApplication::processEvents();

            if (progress.wasCanceled()) {
                running=false;
                if (useCam1) ecamera1->cancelAcquisition(camera1);
                if (useCam2) ecamera2->cancelAcquisition(camera2);
                log_warning(tr("  - acquisition canceled by user!\n"));
            } else {
                bool run1=false;
                bool run2=false;
                if (useCam1) run1=ecamera1->isAcquisitionRunning(camera1);
                if (useCam2) run2=ecamera2->isAcquisitionRunning(camera2);
                running=run1||run2;
            }
        }
    }
    progress.setValue(100);

    //////////////////////////////////////////////////////////////////////////////////////
    // collect common acquisition data
    //////////////////////////////////////////////////////////////////////////////////////
    QMap<QString, QVariant> acquisitionDescription;
    acquisitionDescription["type"]="acquisition";



    //////////////////////////////////////////////////////////////////////////////////////
    // write acquisition data
    //////////////////////////////////////////////////////////////////////////////////////
    if (ok && useCam1) {
        log_text(tr("  - writing acquisition description 1 ..."));
        saveAcquisitionDescription(extension1, ecamera1, camera1, acquisitionPrefix1, acquisitionDescription);
        log_text(tr(" DONE!\n"));
    }
    if (ok && useCam2) {
        log_text(tr("  - writing acquisition description 2 ..."));
        saveAcquisitionDescription(extension2, ecamera2, camera2, acquisitionPrefix2, acquisitionDescription);
        log_text(tr(" DONE!\n"));
    }

    //////////////////////////////////////////////////////////////////////////////////////
    // release cameras
    //////////////////////////////////////////////////////////////////////////////////////
    if (useCam1) {
        camConfig1->releaseCamera();
        log_text(tr("  - released camera 1!\n"));
    }
    if (useCam2) {
        camConfig2->releaseCamera();
        log_text(tr("  - released camera 2!\n"));
    }

    log_text(tr("image series acquisition DONE!\n"));
}


QString QFESPIMB040MainWindow::saveAcquisitionDescription(QFExtension* extension, QFExtensionCamera* ecamera, int camera, const QString& filenamePrefix, const QMap<QString, QVariant>& acquisitionDescription) {
    QString iniFilename=filenamePrefix+"_configuration.ini";
    QSettings settings(iniFilename, QSettings::IniFormat);

    QFESPIMB040CameraConfig* cam=camConfig1;
    if (camera==1) cam=camConfig2;

    QList<QFExtensionCamera::AcquititonFileDescription> files;
    QMap<QString, QVariant> parameters;
    ecamera->getAcquisitionDescription(camera, &files, &parameters);

    // WRITE ACQUISITION SETTINGS
    settings.setValue("acquisition/pixel_width", ecamera->getPixelWidth(camera));
    settings.setValue("acquisition/pixel_height", ecamera->getPixelHeight(camera));
    settings.setValue("acquisition/camera_model", ecamera->getCameraName(camera));
    settings.setValue("acquisition/sensor_model", ecamera->getCameraSensorName(camera));

    QMapIterator <QString, QVariant> it1(acquisitionDescription);
    while (it1.hasNext()) {
        it1.next();
        it1.next();
        if (it1.value().type()==QVariant::List) {
            settings.setValue("acquisition/"+it1.key(), jkVariantListToString(it1.value().toList(), "; "));
        } else {
            settings.setValue("acquisition/"+it1.key(), it1.value().toString());
        }
    }
    QMapIterator <QString, QVariant> it(parameters);
    while (it.hasNext()) {
        it.next();
        it1.next();
        if (it.value().type()==QVariant::List) {
            settings.setValue("acquisition/"+it.key(), jkVariantListToString(it.value().toList(), "; "));
        } else {
            settings.setValue("acquisition/"+it.key(), it.value().toString());
        }
    }

    // WRITE OBJECTIVE SETTINGS
    settings.setValue("objective_detection/name", cam->objective().name);
    settings.setValue("objective_detection/manufacturer", cam->objective().manufacturer);
    settings.setValue("objective_detection/magnification", cam->objective().magnification);
    settings.setValue("objective_detection/numerical_aperature", cam->objective().NA);

    settings.setValue("objectives_projection/name", cam->objectiveProjection().name);
    settings.setValue("objectives_projection/manufacturer", cam->objectiveProjection().manufacturer);
    settings.setValue("objectives_projection/magnification", cam->objectiveProjection().magnification);
    settings.setValue("objectives_projection/numerical_aperature", cam->objectiveProjection().NA);

    // WRITE FILES LIST
    settings.setValue("files/count", files.size());
    for (int i=0; i<files.size(); i++) {
        settings.setValue("files/name"+QString::number(i), files[i].name);
        settings.setValue("files/type"+QString::number(i), files[i].type);
        settings.setValue("files/description"+QString::number(i), files[i].description);
    }

    settings.sync();
    return iniFilename;
}


QString QFESPIMB040MainWindow::savePreviewDescription(QFExtension* extension, QFExtensionCamera* ecamera, int camera, const QString& filenamePrefix, const QMap<QString, QVariant>& acquisitionDescription, const QList<QFExtensionCamera::AcquititonFileDescription>& files) {
    QString iniFilename=filenamePrefix+"_configuration.ini";
    QSettings settings(iniFilename, QSettings::IniFormat);

    QFESPIMB040CameraConfig* cam=camConfig1;
    if (camera==1) cam=camConfig2;

    QMap<QString, QVariant> parameters;

    // WRITE ACQUISITION SETTINGS
    settings.setValue("acquisition/pixel_width", ecamera->getPixelWidth(camera));
    settings.setValue("acquisition/pixel_height", ecamera->getPixelHeight(camera));
    settings.setValue("acquisition/camera_model", ecamera->getCameraName(camera));
    settings.setValue("acquisition/sensor_model", ecamera->getCameraSensorName(camera));
    settings.setValue("acquisition/exposure", ecamera->getExposureTime(camera));
    settings.setValue("acquisition/image_width", ecamera->getImageWidth(camera));
    settings.setValue("acquisition/image_height", ecamera->getImageHeight(camera));
    QMapIterator <QString, QVariant> it1(acquisitionDescription);
    while (it1.hasNext()) {
        it1.next();
        if (it1.value().type()==QVariant::List) {
            settings.setValue("acquisition/"+it1.key(), jkVariantListToString(it1.value().toList(), "; "));
        } else {
            settings.setValue("acquisition/"+it1.key(), it1.value().toString());
        }
    }

    // WRITE OBJECTIVE SETTINGS
    settings.setValue("objective_detection/name", cam->objective().name);
    settings.setValue("objective_detection/manufacturer", cam->objective().manufacturer);
    settings.setValue("objective_detection/magnification", cam->objective().magnification);
    settings.setValue("objective_detection/numerical_aperature", cam->objective().NA);

    settings.setValue("objectives_projection/name", cam->objectiveProjection().name);
    settings.setValue("objectives_projection/manufacturer", cam->objectiveProjection().manufacturer);
    settings.setValue("objectives_projection/magnification", cam->objectiveProjection().magnification);
    settings.setValue("objectives_projection/numerical_aperature", cam->objectiveProjection().NA);

    // WRITE FILES LIST
    settings.setValue("files/count", files.size());
    for (int i=0; i<files.size(); i++) {
        settings.setValue("files/name"+QString::number(i), files[i].name);
        settings.setValue("files/type"+QString::number(i), files[i].type);
        settings.setValue("files/description"+QString::number(i), files[i].description);
    }

    settings.sync();
    return iniFilename;
}

void QFESPIMB040MainWindow::log_text(QString message) {
    logMain->log_text(message);
};

void QFESPIMB040MainWindow::log_warning(QString message) {
    logMain->log_warning(message);
};

void QFESPIMB040MainWindow::log_error(QString message) {
    logMain->log_error(message);
};






