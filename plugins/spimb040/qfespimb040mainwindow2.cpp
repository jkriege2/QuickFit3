#include "qfespimb040mainwindow2.h"
#include <QtGui>
#include <tiffio.h>
#include "libtiff_tools.h"



#define sqr(a) ((a)*(a))
#define PROCESS_EVENTS_TIMEOUT_MS 20
#define SPIMACQUISITION_ERROR(message, title) \
    log_error(QString("  - ")+(message)+QString("\n")); \
    QMessageBox::critical(this, title, (message));


QFESPIMB040MainWindow2::QFESPIMB040MainWindow2(QFPluginServices* pluginServices, QWidget* parent):
    QWidget(parent, Qt::Dialog|Qt::WindowMaximizeButtonHint|Qt::WindowCloseButtonHint|Qt::WindowSystemMenuHint)
{
    widImageStack=NULL;
    widScriptedAcquisition=NULL;
    widCamParamScan=NULL;
    widDeviceParamScan=NULL;
    widExperimentDescription=NULL;
    widAcquisition=NULL;
    widConfig=NULL;
    optSetup=NULL;
    m_pluginServices=pluginServices;
    // create widgets and actions
    createWidgets(pluginServices->getExtensionManager());
    setWindowTitle("B040 SPIM Control");
    setWindowIcon(QIcon(":/spimb040_logo.png"));
}

QFESPIMB040MainWindow2::~QFESPIMB040MainWindow2()
{
}

void QFESPIMB040MainWindow2::loadSettings(ProgramOptions* settings) {
    jkloadWidgetGeometry((*settings->getQSettings()), this, "plugin_spim_b040/");
    jkloadSplitter((*settings->getQSettings()), splitter, "plugin_spim_b040/");
    if (optSetup) optSetup->loadSettings((*settings->getQSettings()), "plugin_spim_b040/instrument/");
    if (widExperimentDescription) widExperimentDescription->loadSettings((*settings->getQSettings()), "plugin_spim_b040/expdescription/");
    if (widAcquisitionDescription) widAcquisitionDescription->loadSettings((*settings->getQSettings()), "plugin_spim_b040/acqdescription/");
    if (widScriptedAcquisition) widScriptedAcquisition->loadSettings((*settings->getQSettings()), "plugin_spim_b040/acqscripted/");
    if (widImageStack) widImageStack->loadSettings((*settings->getQSettings()), "plugin_spim_b040/image_stack/");
    if (widAcquisition) widAcquisition->loadSettings((*settings->getQSettings()), "plugin_spim_b040/acquisition/");
    if (widCamParamScan) widCamParamScan->loadSettings((*settings->getQSettings()), "plugin_spim_b040/camparamscan/");
    if (widDeviceParamScan) widDeviceParamScan->loadSettings((*settings->getQSettings()), "plugin_spim_b040/deviceparamscan/");
    if (widConfig) widConfig->loadSettings((*settings->getQSettings()), "plugin_spim_b040/config/");
}

void QFESPIMB040MainWindow2::storeSettings(ProgramOptions* settings) {
    jksaveWidgetGeometry((*settings->getQSettings()), this, "plugin_spim_b040/");
    jksaveSplitter((*settings->getQSettings()), splitter, "plugin_spim_b040/");
    if (optSetup) optSetup->storeSettings((*settings->getQSettings()), "plugin_spim_b040/instrument/");
    if (widExperimentDescription) widExperimentDescription->storeSettings((*settings->getQSettings()), "plugin_spim_b040/expdescription/");
    if (widAcquisitionDescription) widAcquisitionDescription->storeSettings((*settings->getQSettings()), "plugin_spim_b040/acqdescription/");
    if (widScriptedAcquisition) widScriptedAcquisition->storeSettings((*settings->getQSettings()), "plugin_spim_b040/acqscripted/");
    if (widImageStack) widImageStack->storeSettings((*settings->getQSettings()), "plugin_spim_b040/image_stack/");
    if (widAcquisition) widAcquisition->storeSettings((*settings->getQSettings()), "plugin_spim_b040/acquisition/");
    if (widCamParamScan) widCamParamScan->storeSettings((*settings->getQSettings()), "plugin_spim_b040/camparamscan/");
    if (widDeviceParamScan) widDeviceParamScan->storeSettings((*settings->getQSettings()), "plugin_spim_b040/deviceparamscan/");
    if (widConfig) widConfig->storeSettings((*settings->getQSettings()), "plugin_spim_b040/config/");

}

QFESPIMB040AcquisitionConfigWidget2 *QFESPIMB040MainWindow2::getWidAcquisition() const
{
    return widAcquisition;
}

void QFESPIMB040MainWindow2::closeEvent ( QCloseEvent * event ) {
    optSetup->close();
}

void QFESPIMB040MainWindow2::showEvent( QShowEvent * event )  {
    optSetup->show();
}

void QFESPIMB040MainWindow2::displayHelp() {
    m_pluginServices->displayHelpWindow(m_pluginServices->getExtensionManager()->getPluginHelp("ext_spimb040"));
}

void QFESPIMB040MainWindow2::createWidgets(QFExtensionManager* extManager) {
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // create main tab and help button as corner widget of the QTabWidget
    ////////////////////////////////////////////////////////////////////////////////////////////////
    QVBoxLayout* mainl=new QVBoxLayout(this);
    mainl->setContentsMargins(0,0,0,0);
    setLayout(mainl);
    splitter=new QVisibleHandleSplitter(Qt::Vertical, this);
    mainl->addWidget(splitter);
    tabMain=new QTabWidget(this);
    QVBoxLayout* topl=new QVBoxLayout(this);
    QWidget* topW=new QWidget(this);
    topW->setLayout(topl);
    topl->addWidget(tabMain,1);
    widAcquisitionDescription=new QFESPIMB040AcquisitionDescription(this);
    topl->addWidget(widAcquisitionDescription);
    splitter->addWidget(topW);
    btnHelp=new QPushButton(QIcon(":/lib/help.png"), tr("Help"), this);
    btnHelp->setToolTip(tr("display the online help window"));
    connect(btnHelp, SIGNAL(clicked()), this,SLOT(displayHelp()));
    tabMain->setCornerWidget(btnHelp, Qt::TopRightCorner);
    tabMain->setCurrentIndex(0);


    ////////////////////////////////////////////////////////////////////////////////////////////////
    // create log pane
    ////////////////////////////////////////////////////////////////////////////////////////////////
    QVBoxLayout* loglayout=new QVBoxLayout(this);
    QWidget* w=new QWidget(this);
    w->setLayout(loglayout);
    splitter->addWidget(w);
    loglayout->setContentsMargins(0,0,0,0);
    logMain=new QtLogFile(this);
    logMain->set_log_date_time(true);
    loglayout->addWidget(new QLabel(tr("<b>Log Messages:</b>")));
    loglayout->addWidget(logMain);
    logMain->set_log_file_append(true);
    logMain->open_logfile(ProgramOptions::getInstance()->getConfigFileDirectory()+"/b040spim_main.log", false);


    ////////////////////////////////////////////////////////////////////////////////////////////////
    // optics setup tab
    ////////////////////////////////////////////////////////////////////////////////////////////////
    optSetup=new QFESPIMB040OpticsSetup(this, this, this, m_pluginServices);
    tabMain->addTab(optSetup, tr("Instrument Setup"));

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // create tab for experiment description input
    ////////////////////////////////////////////////////////////////////////////////////////////////
    widExperimentDescription=new QFESPIMB040ExperimentDescription(this);
    tabMain->addTab(widExperimentDescription, tr("Experiment Description"));



    ////////////////////////////////////////////////////////////////////////////////////////////////
    // create tab which groups different acquisition modes
    ////////////////////////////////////////////////////////////////////////////////////////////////
    //tabAcquisition=new QTabWidget(this);
    //tabMain->addTab(tabAcquisition, tr("Acquisition"));

        //------------------------------------------------------------------------------------------
        // create tab for image series acquisition
        //------------------------------------------------------------------------------------------
        widAcquisition=new QFESPIMB040AcquisitionConfigWidget2(this, this, this, m_pluginServices, optSetup, widAcquisitionDescription, widExperimentDescription, m_pluginServices->getConfigFileDirectory());
        //tabAcquisition->addTab(widAcquisition, tr("Image Series Acquisition"));
        tabMain->addTab(widAcquisition, tr("Acquisition: Image Series"));
        //connect(widAcquisition, SIGNAL(doAcquisition()), this, SLOT(doAcquisition()));
        connect(optSetup, SIGNAL(lightpathesChanged(QFESPIMB040OpticsSetupItems)), widAcquisition, SLOT(lightpathesChanged(QFESPIMB040OpticsSetupItems)));

        //------------------------------------------------------------------------------------------
        // create tab for image stack acquisition
        //------------------------------------------------------------------------------------------
        widImageStack=new QFESPIMB040ImageStackConfigWidget2(this, m_pluginServices, optSetup, widAcquisitionDescription, widExperimentDescription, m_pluginServices->getConfigFileDirectory());
        //tabAcquisition->addTab(widImageStack, tr("Image S&tack"));
        tabMain->addTab(widImageStack, tr("Acquisition: Image S&tack"));
        connect(widImageStack, SIGNAL(doStack()), this, SLOT(doImageStack()));
        connect(optSetup, SIGNAL(lightpathesChanged(QFESPIMB040OpticsSetupItems)), widImageStack, SLOT(lightpathesChanged(QFESPIMB040OpticsSetupItems)));

        //------------------------------------------------------------------------------------------
        // create tab for cam parameter image series acquisition
        //------------------------------------------------------------------------------------------
        widCamParamScan=new QFESPIMB040CamParamStackConfigWidget2(this, m_pluginServices, optSetup, widAcquisitionDescription, widExperimentDescription, m_pluginServices->getConfigFileDirectory());
        tabMain->addTab(widCamParamScan, tr("Acquisition: Camera Parameter Series"));
        connect(widCamParamScan, SIGNAL(doStack()), this, SLOT(doCamParamStack()));
        connect(optSetup, SIGNAL(lightpathesChanged(QFESPIMB040OpticsSetupItems)), widCamParamScan, SLOT(lightpathesChanged(QFESPIMB040OpticsSetupItems)));


        //------------------------------------------------------------------------------------------
        // create tab for device parameter image series acquisition
        //------------------------------------------------------------------------------------------
        widDeviceParamScan=new QFESPIMB040DeviceParamStackConfigWidget(this, this, this, m_pluginServices, optSetup, widAcquisitionDescription, widExperimentDescription, m_pluginServices->getConfigFileDirectory());
        tabMain->addTab(widDeviceParamScan, tr("Acquisition: Device Parameter Series"));
        //connect(widDeviceParamScan, SIGNAL(doStack()), this, SLOT(doDeviceParamStack()));
        connect(optSetup, SIGNAL(lightpathesChanged(QFESPIMB040OpticsSetupItems)), widDeviceParamScan, SLOT(lightpathesChanged(QFESPIMB040OpticsSetupItems)));


        //------------------------------------------------------------------------------------------
        // create tab for scripted acquisition
        //------------------------------------------------------------------------------------------
        widScriptedAcquisition=new QFESPIMB040ScriptedAcquisition(this, this, this, this, m_pluginServices, optSetup, widAcquisitionDescription, widExperimentDescription, m_pluginServices->getConfigFileDirectory());
        tabMain->addTab(widScriptedAcquisition, tr("Scripted Acquisition"));
        //connect(optSetup, SIGNAL(lightpathesChanged(QFESPIMB040OpticsSetupItems)), widDeviceParamScan, SLOT(lightpathesChanged(QFESPIMB040OpticsSetupItems)));


        //------------------------------------------------------------------------------------------
        // create configuration tab
        //------------------------------------------------------------------------------------------
        widConfig=new QFESPIMB040ConfigTabWidget(this);
        tabMain->addTab(widConfig, tr("&Configuration"));
        connect(widConfig, SIGNAL(styleChanged(QString,QString)), this, SLOT(styleChanged(QString,QString)));


        optSetup->emitLighpathesChanged();
}

#define IMAGESTACK_ERROR(message) \
    log_error(QString("  - ")+(message)+QString("\n")); \
    QMessageBox::critical(this, tr("B040SPIM: Image Stack Acquisition Error"), (message));


void QFESPIMB040MainWindow2::doImageStack() {
    if (!(widImageStack->use1() || widImageStack->use2())) {
        QMessageBox::critical(this, tr("B040SPIM: Image Stack Acquisition"), tr("Cannot start image acquisition: No camera selected!"));
        return;
    }

    QDateTime startDateTime=QDateTime::currentDateTime();
    QList<QFESPIMB040OpticsSetup::measuredValues> measured;

    QProgressListDialog progress(tr("Image Stack Acquisition"), tr("&Cancel"), 0, 100, this);
    progress.setWindowModality(Qt::WindowModal);
    //progress.setMinimumDuration(0);
    progress.setValue(0);
    progress.addItem(tr("initializing stages"));
    progress.addItem(tr("preparing cameras"));
    progress.addItem(tr("performing acquisition"));
    progress.addItem(tr("storing data to disk"));
    progress.addItem(tr("clean up"));
    progress.setHasProgressBar(true);
    progress.show();


    log_text(tr("starting image stack acquisition:\n"));
    log_text(tr("  - locking stages\n"));
    progress.start();
    optSetup->lockStages();
    optSetup->lockLightpath();

    bool ok=true;
    int axisCount=1; // number of axes to use for scan

    //////////////////////////////////////////////////////////////////////////////////////
    // CHECK/CONNECT SELECTED STAGE 1
    //////////////////////////////////////////////////////////////////////////////////////
    QFExtensionLinearStage* stage=widImageStack->stage();
    int stageAxis=widImageStack->currentAxisID();
    double stageInitialPos=0;
    progress.setProgressText(tr("locking stage 1 ..."));
    ok=connectStageForAcquisition(stage, stageAxis, stageInitialPos, tr("B040SPIM: Image Stack Acquisition"),1);


    if (ok) {

        //////////////////////////////////////////////////////////////////////////////////////
        // CHECK/CONNECT SELECTED STAGE 2 (if selected)
        //////////////////////////////////////////////////////////////////////////////////////
        QFExtensionLinearStage* stage2=widImageStack->stage2();
        int stageAxis2=widImageStack->currentAxisID2();
        double stageInitialPos2=0;
        if (widImageStack->useStage2()) {
            progress.setProgressText(tr("locking stage 2 ..."));
            ok=connectStageForAcquisition(stage2, stageAxis2, stageInitialPos2, tr("B040SPIM: Image Stack Acquisition"),2);
            if (ok) axisCount++;

        }

        if (!ok) {
            optSetup->unlockStages();
            optSetup->unlockLightpath();
            return;
        }

        //////////////////////////////////////////////////////////////////////////////////////
        // CHECK/CONNECT SELECTED STAGE 3 (if selected)
        //////////////////////////////////////////////////////////////////////////////////////
        QFExtensionLinearStage* stage3=widImageStack->stage3();
        int stageAxis3=widImageStack->currentAxisID3();
        double stageInitialPos3=0;
        if (widImageStack->useStage3()) {
            progress.setProgressText(tr("locking stage 3 ..."));
            ok=connectStageForAcquisition(stage3, stageAxis3, stageInitialPos3, tr("B040SPIM: Image Stack Acquisition"),3);
            if (ok) axisCount++;

        }

        if (!ok) {
            optSetup->unlockStages();
            optSetup->unlockLightpath();
            return;
        }


        progress.nextItem();

        //////////////////////////////////////////////////////////////////////////////////////
        // LOCK/INIT CAMERA 1
        //////////////////////////////////////////////////////////////////////////////////////
        bool useCam1=false;
        QFExtension* extension1=NULL;
        QFExtensionCamera* ecamera1=NULL;
        int camera1=0;
        QString acquisitionSettingsFilename1="", previewSettingsFilename1="";
        QString acquisitionPrefix1=widImageStack->prefix1();
        QStringList TIFFFIlename1;;
        QList<TIFF*> tiff1;
        if (widImageStack->use1()) {
            progress.setProgressText(tr("locking camera 1 ..."));
            if (!(useCam1=optSetup->lockCamera(0, &extension1, &ecamera1, &camera1, &previewSettingsFilename1))) {
                IMAGESTACK_ERROR(tr("error locking camera 1!\n"));
            }
        }
        if (QFile::exists(widImageStack->currentConfigFilename(0))) acquisitionSettingsFilename1=widImageStack->currentConfigFilename(0);

        //////////////////////////////////////////////////////////////////////////////////////
        // LOCK/INIT CAMERA 2
        //////////////////////////////////////////////////////////////////////////////////////
        bool useCam2=false;
        QFExtension* extension2=NULL;
        QFExtensionCamera* ecamera2=NULL;
        QString acquisitionSettingsFilename2="", previewSettingsFilename2="";
        QString acquisitionPrefix2=widImageStack->prefix2();
        QStringList TIFFFIlename2;
        QList<TIFF*> tiff2;
        int camera2=0;
        if (widImageStack->use2()) {
            progress.setProgressText(tr("locking camera 2 ..."));
            if(!(useCam2=optSetup->lockCamera(1, &extension2, &ecamera2, &camera2, &previewSettingsFilename2))) {
                IMAGESTACK_ERROR(tr("error locking camer 2!\n"));
            }
        }
        if (QFile::exists(widImageStack->currentConfigFilename(1))) acquisitionSettingsFilename2=widImageStack->currentConfigFilename(1);

        if (ok && !useCam1 && !useCam2) {
            IMAGESTACK_ERROR(tr("Cannot start image acquisition: No camera selected, or both cameras not usable!"));
            ok=false;
        }

        if (ok && useCam1) log_text(tr("  - storing files with prefix 1: '%1'\n").arg(acquisitionPrefix1));
        if (ok && useCam2) log_text(tr("  - storing files with prefix 2: '%1'\n").arg(acquisitionPrefix2));

        //////////////////////////////////////////////////////////////////////////////////////
        // PREPARE CAMERA 1
        //////////////////////////////////////////////////////////////////////////////////////
        int width1=0, height1=0;
        uint32_t* buffer1=NULL;
        if (ok && useCam1) {
            progress.setLabelText(tr("preparing camera 1 ..."));
            ok=prepareCamera(1, camera1, ecamera1, acquisitionSettingsFilename1, width1, height1, &buffer1);

        }

        //////////////////////////////////////////////////////////////////////////////////////
        // PREPARE CAMERA 2
        //////////////////////////////////////////////////////////////////////////////////////
        int width2=0, height2=0;
        uint32_t* buffer2=NULL;
        if (ok && useCam2) {
            progress.setLabelText(tr("preparing camera 2 ..."));
            ok=prepareCamera(2, camera2, ecamera2, acquisitionSettingsFilename2, width2, height2, &buffer2);


        }


        progress.setLabelText(tr("preparing lightpathes ..."));


        //////////////////////////////////////////////////////////////////////////////////////
        // COUNT LIGHTPATHS
        //////////////////////////////////////////////////////////////////////////////////////
        QStringList lightpathList;
        QStringList lightpathNames;
        if (widImageStack->lightpath1Activated()) {
            if (QFile::exists(widImageStack->lightpath1Filename())) {
                lightpathList.append(widImageStack->lightpath1Filename());
                lightpathNames.append(widImageStack->lightpath1());
                TIFFFIlename1.append(acquisitionPrefix1+".lightpath1.tif");
                tiff1.append(NULL);
                TIFFFIlename2.append(acquisitionPrefix2+".lightpath1.tif");
                tiff2.append(NULL);
            } else {
                ok=false;
                IMAGESTACK_ERROR(tr("acquisition lightpath 1 '%1' configuration not found!").arg(widImageStack->lightpath1()));
            }
        }
        if (widImageStack->lightpath2Activated()) {
            if (QFile::exists(widImageStack->lightpath2Filename())) {
                lightpathList.append(widImageStack->lightpath2Filename());
                lightpathNames.append(widImageStack->lightpath2());
                TIFFFIlename1.append(acquisitionPrefix1+".lightpath2.tif");
                tiff1.append(NULL);
                TIFFFIlename2.append(acquisitionPrefix2+".lightpath2.tif");
                tiff2.append(NULL);
            } else {
                ok=false;
                IMAGESTACK_ERROR(tr("acquisition lightpath 2 '%1' configuration not found!").arg(widImageStack->lightpath2()));
            }
        }
        if (widImageStack->lightpath3Activated()) {
            if (QFile::exists(widImageStack->lightpath3Filename())) {
                lightpathList.append(widImageStack->lightpath3Filename());
                lightpathNames.append(widImageStack->lightpath3());
                TIFFFIlename1.append(acquisitionPrefix1+".lightpath3.tif");
                tiff1.append(NULL);
                TIFFFIlename2.append(acquisitionPrefix2+".lightpath3.tif");
                tiff2.append(NULL);
            } else {
                ok=false;
                IMAGESTACK_ERROR(tr("acquisition lightpath 3 '%1' configuration not found!").arg(widImageStack->lightpath3()));
            }
        }
        if (ok && lightpathList.isEmpty()) {
            lightpathList.append("");
            lightpathNames.append("default");
            TIFFFIlename1.append(acquisitionPrefix1+".tif");
            tiff1.append(NULL);
            TIFFFIlename2.append(acquisitionPrefix2+".tif");
            tiff2.append(NULL);
        }

        progress.setLabelText(tr("opening/creating output files ..."));

        //////////////////////////////////////////////////////////////////////////////////////
        // OPEN OUTPUT TIFF FILES
        //////////////////////////////////////////////////////////////////////////////////////
        progress.setLabelText(tr("opening output files ..."));
        QApplication::processEvents();
        if (ok && useCam1) {
            for (int i=0; i<TIFFFIlename1.size(); i++) {
                QDir().mkpath(QFileInfo(TIFFFIlename1[i].toAscii().data()).absolutePath());
                tiff1[i]=TIFFOpen(TIFFFIlename1[i].toAscii().data(), "w");
                if (!tiff1[i]) {
                    ok=false;
                    IMAGESTACK_ERROR(tr("error opening TIFF file (camera 1) '%1'!").arg(TIFFFIlename1[i]));
                    break;
                }
            }
        }
        if (ok && useCam2) {
            for (int i=0; i<TIFFFIlename2.size(); i++) {
                QDir().mkpath(QFileInfo(TIFFFIlename2[i].toAscii().data()).absolutePath());
                tiff2[i]=TIFFOpen(TIFFFIlename2[i].toAscii().data(), "w");
                if (!tiff2[i]) {
                    ok=false;
                    IMAGESTACK_ERROR(tr("error opening TIFF file (camera 2) '%1'!").arg(TIFFFIlename2[i]));
                }
            }
        }

        progress.setLabelText(tr("switching main shutter on ..."));


        //////////////////////////////////////////////////////////////////////////////////////
        // switch on light
        //////////////////////////////////////////////////////////////////////////////////////
        bool formerMainShutterState=optSetup->getMainIlluminationShutter();
        if (optSetup->isMainIlluminationShutterAvailable()){
            log_text(tr("  - switch main shutter on!\n"));
            setMainIlluminationShutter(true, true);
        }


        if (progress.wasCanceled()) {
            log_warning(tr("canceled by user!\n"));
            ok=false;
        }


        //////////////////////////////////////////////////////////////////////////////////////
        // CALCULATE A LIST WITH ALL POSITIONS TO MOVE TO
        //////////////////////////////////////////////////////////////////////////////////////
        progress.setLabelText(tr("preparing list of stage positions ..."));
        double stageStart=widImageStack->stackStart();
        double stageDelta=widImageStack->stackDelta();
        int stageCount=widImageStack->stackCount();

        double stageStart2=widImageStack->stackStart2();
        double stageDelta2=widImageStack->stackDelta2();
        int stageCount2=widImageStack->stackCount2();
        bool stageReturn2=widImageStack->stage2Cycling();

        double stageStart3=widImageStack->stackStart3();
        double stageDelta3=widImageStack->stackDelta3();
        int stageCount3=widImageStack->stackCount3();
        bool stageReturn3=widImageStack->stage3Cycling();

        QList<QTriple<double, double, double> > moveTo;

        if (axisCount==1) {
            double pos=stageStart;
            for (int x=0; x<stageCount; x++) {
                moveTo.append(qMakeTriple(pos, 0.0, 0.0));
                pos=pos+stageDelta;
            }
        } else if (axisCount==2) {
            double pos=stageStart;
            double pos2=stageStart2;
            for (int x=0; x<stageCount; x++) {
                if (stageReturn2) pos2=stageStart2;
                for (int y=0; y<stageCount2; y++) {
                    moveTo.append(qMakeTriple(pos, pos2, 0.0));
                    pos2=pos2+stageDelta2;
                }
                pos=pos+stageDelta;
            }
        } else if (axisCount==3) {
            double pos=stageStart;
            double pos2=stageStart2;
            double pos3=stageStart3;
            for (int x=0; x<stageCount; x++) {
                if (stageReturn2) pos2=stageStart2;
                for (int y=0; y<stageCount2; y++) {
                    if (stageReturn3) pos3=stageStart3;
                    for (int z=0; z<stageCount3; z++) {
                        moveTo.append(qMakeTriple(pos, pos2, pos3));
                        pos3=pos3+stageDelta3;
                    }
                    pos2=pos2+stageDelta2;
                }
                pos=pos+stageDelta;
            }
        }


        int images=moveTo.size()*widImageStack->images()*lightpathList.size();


        //////////////////////////////////////////////////////////////////////////////////////
        // ACQUIRE IMAGE, MOVE, ACQUIRE IMAGE, MOVE, ...
        //    images are stored in TIFF files using libtiff and they are (possibly) downscaled to 16-bit
        //////////////////////////////////////////////////////////////////////////////////////
        progress.nextItem();

        QMap<QString, QVariant> acquisitionDescription, acquisitionDescription1, acquisitionDescription2;
        QList<QVariant> positions, positions2, positions3;
        QTime timAcquisition=QTime::currentTime();
        QDateTime timStart;
        QString estimation="";
        QString fps="";
        double duration=0;
        if (ok) {
            progress.setLabelText(tr("acquiring images ..."));
            bool running=ok;
            //double newPos=stageStart;
            int posIdx=0;
            int imageIdx=0;
            measured.append(optSetup->getMeasuredValues());
            while (running && (posIdx<=moveTo.size())) {
                double newPos=stageInitialPos;
                double newPos2=stageInitialPos2;
                double newPos3=stageInitialPos3;
                if (posIdx<moveTo.size()) {
                    newPos=moveTo[posIdx].first;
                    newPos2=moveTo[posIdx].second;
                    newPos3=moveTo[posIdx].third;
                }
                if (axisCount==1) {
                    log_text(tr("  - moving to position %1 micron ...").arg(newPos));
                    stage->move(stageAxis, newPos);
                    QTime t1;
                    t1.start();
                    while (stage->getAxisState(stageAxis)==QFExtensionLinearStage::Moving) {
                        if (t1.elapsed()>PROCESS_EVENTS_TIMEOUT_MS) {
                            progress.setLabelText(tr("moving stage to %1 microns (distance: %2) ...%3%4").arg(newPos).arg(fabs(stage->getPosition(stageAxis)-newPos)).arg(estimation).arg(fps));
                            QApplication::processEvents();
                            if (progress.wasCanceled()) break;
                            t1.start();
                        }
                    }

                    // wait additional time-span after moving stages!
                    QTime t;
                    int DeltaT=qMin(5000,qMax(1,widImageStack->delay()));
                    t.start();
                    t1.start();
                    while (t.elapsed()<DeltaT) {
                        if (t1.elapsed()>PROCESS_EVENTS_TIMEOUT_MS)  {
                            progress.setLabelText(tr("moving stage to %1 microns (distance: %2) ... waiting%3%4").arg(newPos).arg(fabs(stage->getPosition(stageAxis)-newPos)).arg(estimation).arg(fps));
                            QApplication::processEvents();
                            if (progress.wasCanceled()) break;
                            t1.start();
                        }
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

                } else if (axisCount==2) {
                    log_text(tr("  - moving to position (%1, %2) micron ...").arg(newPos).arg(newPos2));
                    stage->move(stageAxis, newPos);
                    stage2->move(stageAxis2, newPos2);
                    QTime t1;
                    t1.start();
                    while (stage->getAxisState(stageAxis)==QFExtensionLinearStage::Moving || stage2->getAxisState(stageAxis2)==QFExtensionLinearStage::Moving) {
                        if (t1.elapsed()>PROCESS_EVENTS_TIMEOUT_MS) {
                            double dist=sqrt(sqr(stage->getPosition(stageAxis)-newPos)+sqr(stage2->getPosition(stageAxis2)-newPos2));
                            progress.setLabelText(tr("moving stage to (%1, %2) microns (distance: %3) ...%4%5").arg(newPos).arg(newPos2).arg(dist).arg(estimation).arg(fps));
                            QApplication::processEvents();
                            if (progress.wasCanceled()) break;
                            t1.start();
                        }
                    }

                    // wait additional time-span after moving stages!
                    QTime t;
                    int DeltaT=qMin(5000,qMax(1,widImageStack->delay()));
                    t.start();
                    t1.start();
                    while (t.elapsed()<DeltaT) {
                        if (t1.elapsed()>PROCESS_EVENTS_TIMEOUT_MS) {
                            double dist=sqrt(sqr(stage->getPosition(stageAxis)-newPos)+sqr(stage2->getPosition(stageAxis2)-newPos2));
                            progress.setLabelText(tr("moving stage to (%1, %2) microns (distance: %3) ... waiting%4%5").arg(newPos).arg(newPos2).arg(dist).arg(estimation).arg(fps));
                            QApplication::processEvents();
                            if (progress.wasCanceled()) break;
                            t1.start();
                        }
                    }
                    if (ok) {
                        if (stage->getAxisState(stageAxis)==QFExtensionLinearStage::Ready && stage2->getAxisState(stageAxis2)==QFExtensionLinearStage::Ready) {
                            log_text(tr(" OK\n"));
                            positions.append(stage->getPosition(stageAxis));
                            positions2.append(stage2->getPosition(stageAxis2));
                        } else {
                            IMAGESTACK_ERROR(tr("error moving to position (%1, %2) micron!\n").arg(newPos).arg(newPos2));
                            ok=false;
                        }
                    }
                } else if (axisCount==3) {
                    log_text(tr("  - moving to position (%1, %2, %3) micron ...").arg(newPos).arg(newPos2).arg(newPos3));
                    stage->move(stageAxis, newPos);
                    stage2->move(stageAxis2, newPos2);
                    stage3->move(stageAxis3, newPos3);
                    QTime t1;
                    t1.start();
                    while (stage->getAxisState(stageAxis)==QFExtensionLinearStage::Moving || stage2->getAxisState(stageAxis2)==QFExtensionLinearStage::Moving || stage3->getAxisState(stageAxis3)==QFExtensionLinearStage::Moving) {
                        if (t1.elapsed()>PROCESS_EVENTS_TIMEOUT_MS) {
                            double dist=sqrt(sqr(stage->getPosition(stageAxis)-newPos) + sqr(stage2->getPosition(stageAxis2)-newPos2) + sqr(stage3->getPosition(stageAxis3)-newPos3));
                            progress.setLabelText(tr("moving stage to (%1, %2, %3) microns (distance: %4) ...%5%6").arg(newPos).arg(newPos2).arg(newPos3).arg(dist).arg(estimation).arg(fps));
                            QApplication::processEvents();
                            if (progress.wasCanceled()) break;
                            t1.start();
                        }
                    }

                    // wait additional time-span after moving stages!
                    QTime t;
                    int DeltaT=qMin(5000,qMax(1,widImageStack->delay()));
                    t.start();
                    t1.start();
                    while (t.elapsed()<DeltaT) {
                        if (t1.elapsed()>PROCESS_EVENTS_TIMEOUT_MS) {
                            double dist=sqrt(sqr(stage->getPosition(stageAxis)-newPos) + sqr(stage2->getPosition(stageAxis2)-newPos2) + sqr(stage3->getPosition(stageAxis3)-newPos3));
                            progress.setLabelText(tr("moving stage to (%1, %2, %3) microns (distance: %4) ... waiting%5%6").arg(newPos).arg(newPos2).arg(newPos3).arg(dist).arg(estimation).arg(fps));
                            QApplication::processEvents();
                            if (progress.wasCanceled()) break;
                            t1.start();
                        }
                    }
                    if (ok) {
                        if (stage->getAxisState(stageAxis)==QFExtensionLinearStage::Ready && stage2->getAxisState(stageAxis2)==QFExtensionLinearStage::Ready && stage3->getAxisState(stageAxis3)==QFExtensionLinearStage::Ready) {
                            log_text(tr(" OK\n"));
                            positions.append(stage->getPosition(stageAxis));
                            positions2.append(stage2->getPosition(stageAxis2));
                            positions3.append(stage3->getPosition(stageAxis3));
                        } else {
                            IMAGESTACK_ERROR(tr("error moving to position (%1, %2, %3) micron!\n").arg(newPos).arg(newPos2).arg(newPos3));
                            ok=false;
                        }
                    }
                }


                QApplication::processEvents();
                if (posIdx<moveTo.size()) {
                    if (progress.wasCanceled()) {
                        running=false;
                        log_warning(tr("  - acquisition canceled by user!\n"));
                    } else {
                        if (widImageStack->saveMeasurements()) measured.append(optSetup->getMeasuredValues());
                        for (int lp=0; lp<lightpathList.size(); lp++) {
                            if (lightpathList.size()>1 || lp==0) {
                                if (!lightpathList[lp].isEmpty() && QFile::exists(lightpathList[lp])) {
                                    log_text(tr("  - setting lightpath '%1' (%2) ...").arg(lightpathNames[lp]).arg(lp));
                                    optSetup->loadLightpathConfig(lightpathList[lp], true);
                                    log_text(tr(" DONE\n"));
                                }
                                if (posIdx<=0) {
                                    optSetup->saveLightpathConfig(acquisitionDescription, lightpathNames[lp], QString("lightpath%1/").arg(lp+1), QList<bool>(), true);
                                }
                            }
                            for (int img=0; img<widImageStack->images(); img++) {
                                log_text(tr("  - acquiring images (%1/%2) ...\n").arg(imageIdx+1).arg(images));
                                if (posIdx>3) {
                                    double duration=double(timAcquisition.elapsed())/1000.0;
                                    double eta=duration/double(posIdx+1.0)*double(moveTo.size());
                                    double etc=eta-duration;
                                    uint mini=floor(etc/60.0);
                                    uint secs=round(etc-double(mini)*60.0);
                                    estimation=tr("\nest. remaining duration (min:secs): %1:%2 ").arg(mini, 2, 10, QChar('0')).arg(secs, 2, 10, QChar('0'));

                                    fps=tr("\nacquisition rate: %1fps").arg(double(imageIdx+1)/duration, 0, 'f', 2);
                                }
                                progress.setLabelText(tr("acquiring images (%1/%2) ...%3%4").arg(imageIdx+1).arg(images).arg(estimation).arg(fps));
                                QApplication::processEvents();
                                if (progress.wasCanceled()) {
                                    running=false;
                                    log_warning(tr("  - acquisition canceled by user!\n"));
                                    break;
                                }
                                if (posIdx==0) {
                                    timAcquisition.start();
                                    timStart=QDateTime::currentDateTime();
                                }
                                uint64_t timestampDummy=0;
                                if (useCam1) {
                                    if (ecamera1->acquireOnCamera(camera1, buffer1, NULL, &acquisitionDescription1)) {
                                        TIFFTWriteUint16from32(tiff1[lp], buffer1, width1, height1, false);
                                        TIFFWriteDirectory(tiff1[lp]);
                                    } else {
                                        ok=false;
                                        IMAGESTACK_ERROR(tr("error acquiring image %1/%2 on camera 1!\n").arg(imageIdx+1).arg(images));
                                    }
                                }
                                //QApplication::processEvents();
                                if (useCam2) {
                                    if (ecamera2->acquireOnCamera(camera2, buffer2, NULL, &acquisitionDescription2)) {
                                        TIFFTWriteUint16from32(tiff2[lp], buffer2, width2, height2, false);
                                        TIFFWriteDirectory(tiff2[lp]);
                                    } else {
                                        ok=false;
                                        IMAGESTACK_ERROR(tr("error acquiring image %1/%2 on camera 2!\n").arg(imageIdx+1).arg(images));
                                    }
                                }
                                imageIdx++;

                            }
                        }

                        //QApplication::processEvents();
                    }
                }
                if (!ok) running=false;

                posIdx++;
                newPos+=stageDelta;
                progress.setValue((int)round((double)posIdx/(double)moveTo.size()*100.0));
                QApplication::processEvents();
                if (progress.wasCanceled()) {
                    break;
                }
            }
            duration=timAcquisition.elapsed()/1000.0;
        }
        progress.setValue(100);
        if (widImageStack->saveMeasurements()) measured.append(optSetup->getMeasuredValues());

        progress.nextItem();
        progress.setProgressText(tr("switching main shutter off ..."));


        //////////////////////////////////////////////////////////////////////////////////////
        // switch on/off light
        //////////////////////////////////////////////////////////////////////////////////////
        if (optSetup->isMainIlluminationShutterAvailable()){
            log_text(tr("  - switch main shutter %1!\n").arg((formerMainShutterState)?tr("on"):tr("off")));
            setMainIlluminationShutter(formerMainShutterState, true);
        }


        progress.setProgressText(tr("closing TIF files ..."));
        //////////////////////////////////////////////////////////////////////////////////////
        // close tiff files and free buffers
        //////////////////////////////////////////////////////////////////////////////////////
        progress.setLabelText(tr("closing output files ..."));
        QApplication::processEvents();
        for (int lp=0; lp<tiff1.size(); lp++) if (tiff1[lp]) TIFFClose(tiff1[lp]);
        for (int lp=0; lp<tiff2.size(); lp++) if (tiff2[lp]) TIFFClose(tiff2[lp]);
        tiff1.clear();
        tiff2.clear();
        if (buffer1) free(buffer1);
        if (buffer2) free(buffer2);
        buffer1=buffer2=NULL;

        progress.setProgressText(tr("collecting acquisition data ..."));
        //////////////////////////////////////////////////////////////////////////////////////
        // collect acquisition data common to all cameras
        //////////////////////////////////////////////////////////////////////////////////////
        QString positionsCSV;
        if (ok) {
            acquisitionDescription["type"]="stack 1 axis";
            acquisitionDescription["axis1/type"]="other";
            if (optSetup->getXStage()==stage && optSetup->getXStageAxis()==stageAxis) acquisitionDescription["axis1/type"]="x";
            if (optSetup->getYStage()==stage && optSetup->getYStageAxis()==stageAxis) acquisitionDescription["axis1/type"]="y";
            if (optSetup->getZStage()==stage && optSetup->getZStageAxis()==stageAxis) acquisitionDescription["axis1/type"]="z";
            acquisitionDescription["axis1/stage_name"]=widImageStack->stageExtension()->getName();
            acquisitionDescription["axis1/stage_axis"]=stageAxis;
            acquisitionDescription["axis1/stack_start"]=stageStart;
            acquisitionDescription["axis1/stack_delta"]=stageDelta;
            acquisitionDescription["axis1/stack_count"]=stageCount;
            acquisitionDescription["sequence_overall_length"]=images;
            acquisitionDescription["images_per_position"]=widImageStack->images();
            acquisitionDescription["lightpath_count"]=lightpathList.count();
            acquisitionDescription["lightpaths"]=lightpathNames;
            acquisitionDescription["start_time"]=timStart;
            acquisitionDescription["duration"]=duration;
            acquisitionDescription["stack_positions"]=positions;
            if (widImageStack->useStage2() && widImageStack->useStage3()) {
                acquisitionDescription["type"]="stack 3 axis";
                acquisitionDescription["axis2/stack_positions"]=positions2;
                acquisitionDescription["axis3/stack_positions"]=positions3;
            } else if (widImageStack->useStage2() && !widImageStack->useStage3()) {
                acquisitionDescription["type"]="stack 2 axis";
                acquisitionDescription["axis1/stack_positions"]=positions2;
            }

            if (widImageStack->useStage2()) {
                acquisitionDescription["axis2/type"]="other";
                if (optSetup->getXStage()==stage2 && optSetup->getXStageAxis()==stageAxis2) acquisitionDescription["axis2/type"]="x";
                if (optSetup->getYStage()==stage2 && optSetup->getYStageAxis()==stageAxis2) acquisitionDescription["axis2/type"]="y";
                if (optSetup->getZStage()==stage2 && optSetup->getZStageAxis()==stageAxis2) acquisitionDescription["axis2/type"]="z";
                acquisitionDescription["axis2/stage_name"]=widImageStack->stageExtension2()->getName();
                acquisitionDescription["axis2/stage_axis"]=stageAxis2;
                acquisitionDescription["axis2/stack_start"]=stageStart2;
                acquisitionDescription["axis2/stack_delta"]=stageDelta2;
                acquisitionDescription["axis2/stack_count"]=stageCount2;
            }

            if (widImageStack->useStage3()) {
                acquisitionDescription["axis3/type"]="other";
                if (optSetup->getXStage()==stage3 && optSetup->getXStageAxis()==stageAxis3) acquisitionDescription["axis3/type"]="x";
                if (optSetup->getYStage()==stage3 && optSetup->getYStageAxis()==stageAxis3) acquisitionDescription["axis3/type"]="y";
                if (optSetup->getZStage()==stage3 && optSetup->getZStageAxis()==stageAxis3) acquisitionDescription["axis3/type"]="z";
                acquisitionDescription["axis3/stage_name"]=widImageStack->stageExtension3()->getName();
                acquisitionDescription["axis3/stage_axis"]=stageAxis3;
                acquisitionDescription["axis3/stack_start"]=stageStart3;
                acquisitionDescription["axis3/stack_delta"]=stageDelta3;
                acquisitionDescription["axis3/stack_count"]=stageCount3;
            }


            QTextStream pf(&positionsCSV);
            if (axisCount==3) {
                pf<<"# number, position 1[micrometer], position 2 [micrometer], position 3 [micrometer], ideal position 1 [micrometer], ideal position 2 [micrometer], ideal position 3 [micrometer] \n";
                for (int i=0; i<qMin(positions.size(), moveTo.size()); i++) {
                    pf<<i<<", "<<CDoubleToQString(positions[i].toDouble())<<", "<<CDoubleToQString(positions2[i].toDouble())<<", "<<CDoubleToQString(positions3[i].toDouble())<<", "<<CDoubleToQString(moveTo[i].first)<<", "<<CDoubleToQString(moveTo[i].second)<<", "<<CDoubleToQString(moveTo[i].third)<<"\n";
                }
            } else if (axisCount==2) {
                pf<<"# number, position 1[micrometer], position 2 [micrometer], ideal position 1 [micrometer], ideal position 2 [micrometer] \n";
                for (int i=0; i<qMin(positions.size(), moveTo.size()); i++) {
                    pf<<i<<", "<<CDoubleToQString(positions[i].toDouble())<<", "<<CDoubleToQString(positions2[i].toDouble())<<", "<<CDoubleToQString(moveTo[i].first)<<", "<<CDoubleToQString(moveTo[i].second)<<"\n";
                }
            } else if (axisCount==1) {
                pf<<"# number, position 1[micrometer], ideal position 1 [micrometer] \n";
                for (int i=0; i<qMin(positions.size(), moveTo.size()); i++) {
                    pf<<i<<", "<<CDoubleToQString(positions[i].toDouble())<<", "<<CDoubleToQString(moveTo[i].first)<<"\n";
                }
            }
        }

        //////////////////////////////////////////////////////////////////////////////////////
        // write image stack properties to files, also collects camera specific information
        //////////////////////////////////////////////////////////////////////////////////////
        if (ok && useCam1) {
            QMap<QString, QVariant> acquisitionDescription11=acquisitionDescription;
            acquisitionDescription11=acquisitionDescription11.unite(acquisitionDescription1);
            QList<QFExtensionCamera::CameraAcquititonFileDescription> files;
            for (int lp=0; lp<TIFFFIlename1.size(); lp++) {
                QFExtensionCamera::CameraAcquititonFileDescription d;
                d.name=TIFFFIlename1[lp];
                d.description="image stack from camera 1, lightpath "+QString(lp+1)+" '"+lightpathNames[lp]+"'";
                d.type="TIFF16";
                files.append(d);
            }


            QString PositionsFilename=acquisitionPrefix1+".positions.dat";
            QFile posFile(PositionsFilename);
            if (posFile.open(QIODevice::WriteOnly)) {
                posFile.write(positionsCSV.toAscii().data());
                posFile.close();
                QFExtensionCamera::CameraAcquititonFileDescription d;
                d.name=PositionsFilename;
                d.description="positions of image stack from camera 1";
                d.type="CSV";
                files.append(d);
            } else {
                log_error(tr("  - could not write positions file '%1' for camera 1: %2 ...").arg(PositionsFilename).arg(posFile.errorString()));
            }

            QString MeasurementsFilename=saveMeasuredData(acquisitionPrefix1, measured);
            if (!MeasurementsFilename.isEmpty() && QFile::exists(MeasurementsFilename)) {
                QFExtensionCamera::CameraAcquititonFileDescription d;
                d.name=MeasurementsFilename;
                d.description="measureable properties of setup";
                d.type="CSV";
                files.append(d);
            }

            log_text(tr("  - writing acquisition description 1 ..."));
            savePreviewDescription(0, extension1, ecamera1, camera1, acquisitionPrefix1, acquisitionDescription11, files, startDateTime);
            log_text(tr(" DONE!\n"));
        }
        if (ok && useCam2) {
            QMap<QString, QVariant> acquisitionDescription22=acquisitionDescription;
            acquisitionDescription22=acquisitionDescription22.unite(acquisitionDescription2);
            QList<QFExtensionCamera::CameraAcquititonFileDescription> files;
            for (int lp=0; lp<TIFFFIlename1.size(); lp++) {
                QFExtensionCamera::CameraAcquititonFileDescription d;
                d.name=TIFFFIlename2[lp];
                d.description="image stack from camera 2, lightpath "+QString(lp+1)+" '"+lightpathNames[lp]+"'";
                d.type="TIFF16";
                files.append(d);
            }


            QString PositionsFilename=acquisitionPrefix2+".positions.dat";
            QFile posFile(PositionsFilename);
            if (posFile.open(QIODevice::WriteOnly)) {
                posFile.write(positionsCSV.toAscii().data());
                posFile.close();
                QFExtensionCamera::CameraAcquititonFileDescription d;
                d.name=PositionsFilename;
                d.description="positions of image stack from camera 1";
                d.type="CSV";
                files.append(d);
            } else {
                log_error(tr("  - could not write positions file '%1' for camera 2: %2 ...").arg(PositionsFilename).arg(posFile.errorString()));
            }

            QString MeasurementsFilename=saveMeasuredData(acquisitionPrefix2, measured);
            if (!MeasurementsFilename.isEmpty() && QFile::exists(MeasurementsFilename)) {
                QFExtensionCamera::CameraAcquititonFileDescription d;
                d.name=MeasurementsFilename;
                d.description="measureable properties of setup";
                d.type="CSV";
                files.append(d);
            }

            log_text(tr("  - writing acquisition description 2 ..."));
            savePreviewDescription(1, extension2, ecamera2, camera2, acquisitionPrefix2, acquisitionDescription22, files, startDateTime);
            log_text(tr(" DONE!\n"));
        }

        progress.nextItem();
        progress.setProgressText(tr("releasing cameras ..."));
        //////////////////////////////////////////////////////////////////////////////////////
        // release cameras
        //////////////////////////////////////////////////////////////////////////////////////
        if (useCam1) {
            optSetup->releaseCamera(0);
            log_text(tr("  - released camera 1!\n"));
        }
        if (useCam2) {
            optSetup->releaseCamera(1);
            log_text(tr("  - released camera 2!\n"));
        }

        if (ok && useCam1) log_text(tr("  - stored files with prefix 1: '%1'\n").arg(acquisitionPrefix1));
        if (ok && useCam2) log_text(tr("  - stored files with prefix 2: '%1'\n").arg(acquisitionPrefix2));

        if (ok) log_text(tr("image stack acquisition DONE!\n"));
    }
    progress.setProgressText(tr("releasing stages and lightpath ..."));
    optSetup->unlockStages();
    optSetup->unlockLightpath();
    optSetup->ensureLightpath();
    progress.close();
}


#define CAMPARAMSTACK_ERROR(message) \
    log_error(QString("  - ")+(message)+QString("\n")); \
    QMessageBox::critical(this, tr("B040SPIM: Parameter Stack Acquisition Error"), (message));


void QFESPIMB040MainWindow2::doCamParamStack() {
    if (!(widCamParamScan->use1() || widCamParamScan->use2())) {
        QMessageBox::critical(this, tr("B040SPIM: Parameter Stack Acquisition"), tr("Cannot start image acquisition: No camera selected!"));
        return;
    }

    QDateTime startDateTime=QDateTime::currentDateTime();
    QList<QFESPIMB040OpticsSetup::measuredValues> measured;


    bool ok=true;

    //////////////////////////////////////////////////////////////////////////////////////
    // get array of values
    //////////////////////////////////////////////////////////////////////////////////////
    QList<double> scanVals=widCamParamScan->stack();
    if (scanVals.isEmpty()) {
        QMessageBox::critical(this, tr("B040SPIM: Parameter Stack Acquisition"), tr("No values to scan selected!"));
        return;
    }
    int images=scanVals.size()*widCamParamScan->images();


    log_text(tr("starting parameter stack acquisition:\n"));
    log_text(tr("  - scan parameter: %1\n").arg(widCamParamScan->stackParameterName()));
    log_text(tr("  - scan mode: %1\n").arg(widCamParamScan->stackModeName()));
    log_text(tr("  - scan start: %1\n").arg(widCamParamScan->stackStart()));
    log_text(tr("  - scan end: %1\n").arg(widCamParamScan->stackEnd()));
    log_text(tr("  - scan delta: %1\n").arg(widCamParamScan->stackDelta()));
    optSetup->lockLightpath();


    //////////////////////////////////////////////////////////////////////////////////////
    // LOCK/INIT CAMERA 1
    //////////////////////////////////////////////////////////////////////////////////////
    bool useCam1=false;
    QFExtension* extension1=NULL;
    QFExtensionCamera* ecamera1=NULL;
    int camera1=0;
    QString acquisitionSettingsFilename1="", previewSettingsFilename1="";
    QString acquisitionPrefix1=widCamParamScan->prefix1();
    QString TIFFFIlename1=acquisitionPrefix1+".tif";
    TIFF* tiff1=NULL;
    if (widCamParamScan->use1()) {
        if (!(useCam1=optSetup->lockCamera(0, &extension1, &ecamera1, &camera1, &previewSettingsFilename1))) {
            CAMPARAMSTACK_ERROR(tr("error locking camer 1!\n"));
        }
    }
    if (QFile::exists(widCamParamScan->currentConfigFilename(0))) acquisitionSettingsFilename1=widCamParamScan->currentConfigFilename(0);

    //////////////////////////////////////////////////////////////////////////////////////
    // LOCK/INIT CAMERA 2
    //////////////////////////////////////////////////////////////////////////////////////
    bool useCam2=false;
    QFExtension* extension2=NULL;
    QFExtensionCamera* ecamera2=NULL;
    QString acquisitionSettingsFilename2="", previewSettingsFilename2="";
    QString acquisitionPrefix2=widCamParamScan->prefix2();
    QString TIFFFIlename2=acquisitionPrefix2+".tif";
    TIFF* tiff2=NULL;
    int camera2=0;
    if (widCamParamScan->use2()) {
        if(!(useCam2=optSetup->lockCamera(1, &extension2, &ecamera2, &camera2, &previewSettingsFilename2))) {
            CAMPARAMSTACK_ERROR(tr("error locking camer 2!\n"));
        }
    }
    if (QFile::exists(widCamParamScan->currentConfigFilename(1))) acquisitionSettingsFilename2=widCamParamScan->currentConfigFilename(1);

    if (ok && !useCam1 && !useCam2) {
        CAMPARAMSTACK_ERROR(tr("Cannot start image acquisition: No camera selected, or both cameras not usable!"));
        optSetup->unlockLightpath();
        ok=false;
    }


    //////////////////////////////////////////////////////////////////////////////////////
    // CHECK WHETHER HARDWRAE SUPPORTS CHANGING THE SELECTED PARAMETER
    //////////////////////////////////////////////////////////////////////////////////////
    int stackParamI=widDeviceParamScan->stackParameter();
    QFExtensionCamera::CameraSetting stackParam=QFExtensionCamera::CameraSetting(stackParamI);
    if (ok) {
        if (stackParam>=0 && stackParam<=QFExtensionCamera::CamSetMaxParam) {
            if (useCam1 && !ecamera1->isCameraSettingChangable(stackParam)) {
                ok=false;
                CAMPARAMSTACK_ERROR(tr("camera 1 does not support changing the %1!\n").arg(widDeviceParamScan->stackParameterName()));
            }
            if (ok && useCam2 && !ecamera2->isCameraSettingChangable(stackParam)) {
                ok=false;
                CAMPARAMSTACK_ERROR(tr("camera 2 does not support changing the %1!\n").arg(widDeviceParamScan->stackParameterName()));
            }
        } else {
            ok=false;
            CAMPARAMSTACK_ERROR(tr("invalid stack parameter %1 '%2'\n").arg(stackParam).arg(widDeviceParamScan->stackParameterName()));
        }
    }



    if (ok) {
        if (ok && useCam1) log_text(tr("  - storing files with prefix 1: '%1'\n").arg(acquisitionPrefix1));
        if (ok && useCam2) log_text(tr("  - storing files with prefix 2: '%1'\n").arg(acquisitionPrefix2));

        QProgressListDialog progress(tr("Parameter Stack Acquisition"), tr("&Cancel"), 0, 100, this);
        progress.addItem(tr("preparation"));
        progress.addItem(tr("acquisition"));
        progress.addItem(tr("cleanup"));
        progress.setWindowModality(Qt::WindowModal);
        progress.setValue(0);
        progress.start();

        //////////////////////////////////////////////////////////////////////////////////////
        // SET LIGHTPATH
        //////////////////////////////////////////////////////////////////////////////////////
        QString oldLightpath=optSetup->getCurrentLightpathFilename();
        QString oldLightpathName=optSetup->getCurrentLightpath();
        QString lightpathName="unknown";
        if (widCamParamScan->lightpathActivated()) {
            if (!QFile::exists(widCamParamScan->lightpathFilename())) {
                CAMPARAMSTACK_ERROR(tr("  - acquisition lighpath configuration '%1' does not exist!\n").arg(widCamParamScan->lightpath()));
                optSetup->unlockLightpath();
                return;

            } else {
                log_text(tr("  - setting acquisition lightpath settings '%1' ...\n").arg(widCamParamScan->lightpath()));
                optSetup->loadLightpathConfig(widCamParamScan->lightpathFilename(), true, &lightpathName);
                log_text(tr("  - setting acquisition lightpath settings '%1' ... DONE\n").arg(widCamParamScan->lightpath()));
            }
        }


        //////////////////////////////////////////////////////////////////////////////////////
        // switch on light
        //////////////////////////////////////////////////////////////////////////////////////
        bool formerMainShutterState=optSetup->getMainIlluminationShutter();
        if (optSetup->isMainIlluminationShutterAvailable()){
            log_text(tr("  - switch main shutter on!\n"));
            setMainIlluminationShutter(true, true);
        }

        QList<QVariant> realValues1, realValues2;
        QTime timAcquisition=QTime::currentTime();
        QDateTime timStart;
        double duration=0;


        progress.nextItem();
        if (widCamParamScan->previewMode()) {
            log_text(tr("acquiring stack in PREVIEW MODE ...\n"));


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
                    CAMPARAMSTACK_ERROR(tr("error opening TIFF file (camera 1) '%1'!").arg(TIFFFIlename1));
                }
            }
            if (ok && useCam2) {
                QDir().mkpath(QFileInfo(TIFFFIlename2.toAscii().data()).absolutePath());
                tiff2=TIFFOpen(TIFFFIlename2.toAscii().data(), "w");
                if (!tiff2) {
                    ok=false;
                    CAMPARAMSTACK_ERROR(tr("error opening TIFF file (camera 2) '%1'!").arg(TIFFFIlename2));
                }
            }



            if (progress.wasCanceled()) {
                log_warning(tr("canceled by user!\n"));
                ok=false;
            }



            //////////////////////////////////////////////////////////////////////////////////////
            // CHANGE PARAM, ACQUIRE IMAGE, CHANGE PARAM, ACQUIRE IMAGE, ...
            //    images are stored in TIFF files using libtiff and they are (possibly) downscaled to 16-bit
            //////////////////////////////////////////////////////////////////////////////////////
            timAcquisition=QTime::currentTime();
            duration=0;
            if (ok) {
                progress.setLabelText(tr("acquiring images ..."));
                bool running=ok;
                //double newPos=stageStart;
                int stackIdx=0;
                int width1=0, height1=0;
                uint32_t* buffer1=NULL;
                int width2=0, height2=0;
                uint32_t* buffer2=NULL;
                int imageCnt=0;

                measured.append(optSetup->getMeasuredValues());

                while (running && (stackIdx<scanVals.size())) {

                    if (ok && useCam1) {

                        //////////////////////////////////////////////////////////////////////////////////////
                        // PREPARE CAMERA 1
                        //////////////////////////////////////////////////////////////////////////////////////
                        if (ok && useCam1) {
                            progress.setLabelText(tr("preparing camera 1 ..."));
                            QApplication::processEvents();

                            QString tmpName1=QDir::temp().absoluteFilePath("qf3spimb040_cam1tmpsettings.ini");

                            QTemporaryFile file1;
                            if (file1.open()) {
                                 tmpName1=file1.fileName();
                            }
                            if (QFile::exists(tmpName1)) QFile::remove(tmpName1);

                            if (useCam1) QFile::copy(acquisitionSettingsFilename1, tmpName1);

                            QSettings settings(tmpName1, QSettings::IniFormat);

                            //QSettings settings(acquisitionSettingsFilename1, QSettings::IniFormat);

                            ecamera1->changeCameraSetting(settings, stackParam, scanVals[stackIdx]);
                            ecamera1->useCameraSettings(camera1, settings);
                            realValues1.append(ecamera1->getCurrentCameraSetting(camera1, stackParam));
                            //realValues1.append(ecamera1->getExposureTime(camera1));
                            log_text(tr("  - prepared camer 1!\n"));
                            width1=ecamera1->getCameraImageWidth(camera1);
                            height1=ecamera1->getImageCameraHeight(camera1);
                            buffer1=(uint32_t*)realloc(buffer1, width1*height1*sizeof(uint32_t));
                            if (!buffer1) {
                                ok=false;
                                CAMPARAMSTACK_ERROR(tr("could not allocate image buffer for camera 1!\n"));
                            }
                            if (QFile::exists(tmpName1)) QFile::remove(tmpName1);
                        }
                    }

                    if (ok && useCam2) {

                        //////////////////////////////////////////////////////////////////////////////////////
                        // PREPARE CAMERA 1
                        //////////////////////////////////////////////////////////////////////////////////////
                        if (ok && useCam2) {
                            progress.setLabelText(tr("preparing camera 2 ..."));
                            QApplication::processEvents();
                            QString tmpName1=QDir::temp().absoluteFilePath("qf3spimb040_cam2tmpsettings.ini");

                            QTemporaryFile file1;
                            if (file1.open()) {
                                 tmpName1=file1.fileName();
                            }
                            if (QFile::exists(tmpName1)) QFile::remove(tmpName1);

                            if (useCam2) QFile::copy(acquisitionSettingsFilename2, tmpName1);

                            QSettings settings(tmpName1, QSettings::IniFormat);

                            ecamera2->changeCameraSetting(settings, stackParam, scanVals[stackIdx]);
                            ecamera2->useCameraSettings(camera2, settings);
                            realValues2.append(ecamera2->getCurrentCameraSetting(camera2, stackParam));
                            //realValues2.append(ecamera2->getExposureTime(camera2));
                            log_text(tr("  - prepared camer 2!\n"));
                            width2=ecamera2->getCameraImageWidth(camera2);
                            height2=ecamera2->getImageCameraHeight(camera2);
                            buffer2=(uint32_t*)realloc(buffer2, width2*height2*sizeof(uint32_t));
                            if (!buffer2) {
                                ok=false;
                                CAMPARAMSTACK_ERROR(tr("could not allocate image buffer for camera 2!\n"));
                            }
                            if (QFile::exists(tmpName1)) QFile::remove(tmpName1);
                        }
                    }

                    QApplication::processEvents();
                    if (progress.wasCanceled()) {
                        running=false;
                        log_warning(tr("  - acquisition canceled by user!\n"));
                    } else {
                        for (int img=0; img<widCamParamScan->images(); img++) {
                            log_text(tr("acquiring images (%1/%2) ...\n").arg(imageCnt+1).arg(images));
                            progress.setLabelText(tr("acquiring images (%1/%2) ...").arg(imageCnt+1).arg(images));
                            QApplication::processEvents();
                            if (progress.wasCanceled()) {
                                running=false;
                                log_warning(tr("  - acquisition canceled by user!\n"));
                                break;
                            }
                            if (stackIdx==0) {
                                timAcquisition.start();
                                timStart=QDateTime::currentDateTime();
                            }
                            if (useCam1) {
                                if (ecamera1->acquireOnCamera(camera1, buffer1)) {
                                    TIFFTWriteUint16from32(tiff1, buffer1, width1, height1, false);
                                    TIFFWriteDirectory(tiff1);
                                } else {
                                    ok=false;
                                    CAMPARAMSTACK_ERROR(tr("error acquiring image %1/%2 on camera 1!\n").arg(imageCnt+1).arg(images));
                                }
                            }
                            //QApplication::processEvents();
                            if (useCam2) {
                                if (ecamera2->acquireOnCamera(camera2, buffer2)) {
                                    TIFFTWriteUint16from32(tiff2, buffer2, width2, height2, false);
                                    TIFFWriteDirectory(tiff2);
                                } else {
                                    ok=false;
                                    CAMPARAMSTACK_ERROR(tr("error acquiring image %1/%2 on camera 2!\n").arg(imageCnt+1).arg(images));
                                }
                            }
                            imageCnt++;
                        }
                        if (widCamParamScan->saveMeasurements()) measured.append(optSetup->getMeasuredValues());
                        //QApplication::processEvents();
                    }
                    if (!ok) running=false;

                    stackIdx++;
                    progress.setValue((int)round((double)stackIdx/(double)scanVals.size()*100.0));
                    QApplication::processEvents();
                }
                duration=timAcquisition.elapsed()/1000.0;
                if (buffer1) free(buffer1);
                if (buffer2) free(buffer2);
                buffer1=buffer2=NULL;

            }
        } else {
            log_text(tr("acquiring stack in ACQUISITION MODE ...\n"));

            if (progress.wasCanceled()) {
                log_warning(tr("canceled by user!\n"));
                ok=false;
            }
            QMap<QString, QVariant> acquisitionDescription1;
            QList<QFExtensionCamera::CameraAcquititonFileDescription> moreFiles1;
            QMap<QString, QVariant> acquisitionDescription2;
            QList<QFExtensionCamera::CameraAcquititonFileDescription> moreFiles2;

            //////////////////////////////////////////////////////////////////////////////////////
            // CHANGE PARAM, ACQUIRE IMAGE, CHANGE PARAM, ACQUIRE IMAGE, ...
            //    images are stored in TIFF files using libtiff and they are (possibly) downscaled to 16-bit
            //////////////////////////////////////////////////////////////////////////////////////
            timAcquisition=QTime::currentTime();
            duration=0;
            if (ok) {
                progress.setLabelText(tr("acquiring images ..."));
                bool running=ok;
                //double newPos=stageStart;
                int stackIdx=0;
                int width1=0, height1=0;
                uint32_t* buffer1=NULL;
                int width2=0, height2=0;
                uint32_t* buffer2=NULL;
                int imageCnt=0;
                int frames=widCamParamScan->images();

                measured.append(optSetup->getMeasuredValues());

                while (running && (stackIdx<scanVals.size())) {
                    QString tmpName1=QDir::temp().absoluteFilePath("qf3spimb040_cam1tmpsettings.ini");

                    QTemporaryFile file1;
                    if (file1.open()) {
                         tmpName1=file1.fileName();
                    }
                    if (QFile::exists(tmpName1)) QFile::remove(tmpName1);
                    if (useCam1) QFile::copy(acquisitionSettingsFilename1, tmpName1);
                    QSettings settings1(tmpName1, QSettings::IniFormat);
                    if (frames>0 && ecamera1->isCameraSettingChangable(QFExtensionCamera::CamSetNumberFrames)) ecamera1->changeCameraSetting(settings1, QFExtensionCamera::CamSetNumberFrames, frames);
                    ecamera1->changeCameraSetting(settings1, stackParam, scanVals[stackIdx]);






                    QString tmpName2=QDir::temp().absoluteFilePath("qf3spimb040_cam2tmpsettings.ini");

                    QTemporaryFile file2;
                    if (file2.open()) {
                         tmpName2=file2.fileName();
                    }
                    if (QFile::exists(tmpName2)) QFile::remove(tmpName2);

                    if (useCam2) QFile::copy(acquisitionSettingsFilename2, tmpName2);

                    QSettings settings2(tmpName2, QSettings::IniFormat);

                    if (frames>0 && ecamera2->isCameraSettingChangable(QFExtensionCamera::CamSetNumberFrames)) ecamera2->changeCameraSetting(settings2, QFExtensionCamera::CamSetNumberFrames, frames);
                    ecamera2->changeCameraSetting(settings2, stackParam, scanVals[stackIdx]);




                    //////////////////////////////////////////////////////////////////////////////////////
                    // acquire image series
                    //////////////////////////////////////////////////////////////////////////////////////
                    ok = acquireSeries(lightpathName, QString("camstack%1").arg(stackIdx,4,10,QLatin1Char('0')), tr("cam parameter stack"), useCam1, extension1, ecamera1, camera1, acquisitionPrefix1+QString("__sidx%1").arg(stackIdx,4,10,QLatin1Char('0')), settings1, acquisitionDescription1, moreFiles1, useCam2, extension2, ecamera2, camera2, acquisitionPrefix2+QString("__sidx%1").arg(stackIdx,4,10,QLatin1Char('0')), settings2, acquisitionDescription2, moreFiles2, &measured, &progress, NULL);
                    if (!ok) {
                        if (frames>1) {
                            CAMPARAMSTACK_ERROR(tr("error acquiring image (%1...%2)/%3 !\n").arg(imageCnt+1).arg(imageCnt+1+frames).arg(images));
                        } else {
                            CAMPARAMSTACK_ERROR(tr("error acquiring image %1/%2 !\n").arg(imageCnt+1).arg(images));
                        }
                    }

                    if (QFile::exists(tmpName1)) QFile::remove(tmpName1);
                    if (QFile::exists(tmpName2)) QFile::remove(tmpName2);

                    if (!ok) running=false;

                    stackIdx++;
                    imageCnt+=frames;
                    progress.setValue((int)round((double)stackIdx/(double)scanVals.size()*100.0));
                    QApplication::processEvents();

                    if (progress.wasCanceled()) {
                        running=false;
                        log_error(tr("CANCELED BY USER!\n"));
                    }
                }
                duration=timAcquisition.elapsed()/1000.0;
                if (buffer1) free(buffer1);
                if (buffer2) free(buffer2);
                buffer1=buffer2=NULL;

            }
        }
        progress.setValue(100);
        measured.append(optSetup->getMeasuredValues());
        progress.nextItem();

        //////////////////////////////////////////////////////////////////////////////////////
        // collect lightpath data common to all cameras
        //////////////////////////////////////////////////////////////////////////////////////
        QMap<QString, QVariant> acquisitionDescription;
        optSetup->saveLightpathConfig(acquisitionDescription, "", "lightpath/");

        //////////////////////////////////////////////////////////////////////////////////////
        // switch on/off light
        //////////////////////////////////////////////////////////////////////////////////////
        if (optSetup->isMainIlluminationShutterAvailable()){
            log_text(tr("  - switch main shutter %1!\n").arg((formerMainShutterState)?tr("on"):tr("off")));
            setMainIlluminationShutter(formerMainShutterState, true);
        }

        //////////////////////////////////////////////////////////////////////////////////////
        // RESET LIGHTPATH
        //////////////////////////////////////////////////////////////////////////////////////
        if (widCamParamScan->lightpathActivated()) {
            optSetup->loadLightpathConfig(oldLightpath, false);
            log_text(tr("  - resetting to old lightpath settings (%1) ...\n").arg(oldLightpath));//Name));
        }

        //////////////////////////////////////////////////////////////////////////////////////
        // close tiff files and free buffers
        //////////////////////////////////////////////////////////////////////////////////////
        progress.setLabelText(tr("closing output files ..."));
        QApplication::processEvents();
        if (tiff1) TIFFClose(tiff1);
        if (tiff2) TIFFClose(tiff2);
        tiff1=tiff2=NULL;

        //////////////////////////////////////////////////////////////////////////////////////
        // collect acquisition data common to all cameras
        //////////////////////////////////////////////////////////////////////////////////////
        QString scanCSV;
        if (ok) {
            acquisitionDescription["type"]="parameter stack";
            acquisitionDescription["stack_parameter"]=widCamParamScan->stackParameterName();
            acquisitionDescription["stack_mode"]=widCamParamScan->stackModeName();
            acquisitionDescription["stack_start"]=widCamParamScan->stackStart();
            acquisitionDescription["stack_delta"]=widCamParamScan->stackDelta();
            acquisitionDescription["stack_end"]=widCamParamScan->stackEnd();
            acquisitionDescription["sequence_overall_length"]=images;
            acquisitionDescription["images_per_step"]=widCamParamScan->images();
            acquisitionDescription["stack_length"]=scanVals.size();
            acquisitionDescription["start_time"]=timStart;
            acquisitionDescription["duration"]=duration;
            acquisitionDescription["stack_value_count"]=scanVals.size();
            acquisitionDescription["stack_values"]=CDoubleListToQString(scanVals);
            if (useCam1) acquisitionDescription["real_stack_values/camera1"]=realValues1;
            if (useCam2) acquisitionDescription["real_stack_values/camera2"]=realValues2;



            QTextStream pf(&scanCSV);
            pf<<"# number,   ideal value";
            if (useCam1) pf<<",   real value camera1";
            if (useCam2) pf<<",   real value camera2";
            pf<<"\n";
            for (int i=0; i<qMin(scanVals.size(), qMax(realValues1.size(), realValues2.size())); i++) {
                pf<<i<<", "<<CDoubleToQString(scanVals[i]);
                if (useCam1 && i<realValues1.size()) pf<<", "<<CDoubleToQString(realValues1[i].toDouble());
                if (useCam2 && i<realValues2.size()) pf<<", "<<CDoubleToQString(realValues2[i].toDouble());
                pf<<"\n";
            }
        }

        //////////////////////////////////////////////////////////////////////////////////////
        // write image stack properties to files, also collects camera specific information
        //////////////////////////////////////////////////////////////////////////////////////
        if (ok && useCam1) {
            QMap<QString, QVariant> acquisitionDescription1=acquisitionDescription;
            QList<QFExtensionCamera::CameraAcquititonFileDescription> files;
            QFExtensionCamera::CameraAcquititonFileDescription d;
            d.name=TIFFFIlename1;
            d.description="image stack from camera 1";
            d.type="TIFF16";
            files.append(d);


            QString ParamValuesFilename=acquisitionPrefix1+".param.dat";
            QFile posFile(ParamValuesFilename);
            if (posFile.open(QIODevice::WriteOnly)) {
                posFile.write(scanCSV.toAscii().data());
                posFile.close();
                d.name=ParamValuesFilename;
                d.description="parameter values";
                d.type="CSV";
                files.append(d);
            } else {
                log_error(tr("  - could not write parameter values file '%1' for camera 1: %2 ...").arg(ParamValuesFilename).arg(posFile.errorString()));
            }

            QString MeasurementsFilename=saveMeasuredData(acquisitionPrefix1, measured);
            if (!MeasurementsFilename.isEmpty() && QFile::exists(MeasurementsFilename)) {
                QFExtensionCamera::CameraAcquititonFileDescription d;
                d.name=MeasurementsFilename;
                d.description="measureable properties of setup";
                d.type="CSV";
                files.append(d);
            }

            log_text(tr("  - writing acquisition description 1 ..."));
            savePreviewDescription(0, extension1, ecamera1, camera1, acquisitionPrefix1, acquisitionDescription1, files, startDateTime);
            log_text(tr(" DONE!\n"));
        }
        if (ok && useCam2) {
            QMap<QString, QVariant> acquisitionDescription2=acquisitionDescription;
            QList<QFExtensionCamera::CameraAcquititonFileDescription> files;
            QFExtensionCamera::CameraAcquititonFileDescription d;
            d.name=TIFFFIlename1;
            d.description="image stack from camera 2";
            d.type="TIFF16";
            files.append(d);


            QString ParamValuesFilename=acquisitionPrefix2+".param.dat";
            QFile posFile(ParamValuesFilename);
            if (posFile.open(QIODevice::WriteOnly)) {
                posFile.write(scanCSV.toAscii().data());
                posFile.close();
                d.name=ParamValuesFilename;
                d.description="parameter values";
                d.type="CSV";
                files.append(d);
            } else {
                log_error(tr("  - could not write parameter values file '%1' for camera 2: %2 ...").arg(ParamValuesFilename).arg(posFile.errorString()));
            }

            QString MeasurementsFilename=saveMeasuredData(acquisitionPrefix2, measured);
            if (!MeasurementsFilename.isEmpty() && QFile::exists(MeasurementsFilename)) {
                QFExtensionCamera::CameraAcquititonFileDescription d;
                d.name=MeasurementsFilename;
                d.description="measureable properties of setup";
                d.type="CSV";
                files.append(d);
            }


            log_text(tr("  - writing acquisition description 2 ..."));
            savePreviewDescription(1, extension2, ecamera2, camera2, acquisitionPrefix2, acquisitionDescription2, files, startDateTime);
            log_text(tr(" DONE!\n"));
        }

        //////////////////////////////////////////////////////////////////////////////////////
        // release cameras
        //////////////////////////////////////////////////////////////////////////////////////
        if (useCam1) {
            optSetup->releaseCamera(0);
            log_text(tr("  - released camera 1!\n"));
        }
        if (useCam2) {
            optSetup->releaseCamera(1);
            log_text(tr("  - released camera 2!\n"));
        }

        if (ok && useCam1) log_text(tr("  - stored files with prefix 1: '%1'\n").arg(acquisitionPrefix1));
        if (ok && useCam2) log_text(tr("  - stored files with prefix 2: '%1'\n").arg(acquisitionPrefix2));

        if (ok) log_text(tr("image stack acquisition DONE!\n"));
    }
    optSetup->unlockLightpath();
    optSetup->ensureLightpath();

}

void QFESPIMB040MainWindow2::doDeviceParameterStack()
{
    widDeviceParamScan->performStack();
}








bool QFESPIMB040MainWindow2::savePreview(QFExtension* extension, QFExtensionCamera* ecamera, int camera, const QString& previewSettingsFilename, const QString& filename, QString* filename32, QMap<QString, QVariant>* acquisitionDescription, const QString& acquisitionDescriptionPrefix) {
    //////////////////////////////////////////////////////////////////////////////////////
    // INIT variables
    //////////////////////////////////////////////////////////////////////////////////////
    bool ok=true;
    QString TIFFFIlename=filename;
    QString TIFFFIlename32=filename.left(filename.size()-(QFileInfo(filename).suffix().size()+1))+".32."+QFileInfo(filename).suffix();
    TIFF* tiff=NULL;

    if (filename32) *filename32="";




    //////////////////////////////////////////////////////////////////////////////////////
    // PREPARE CAMERA
    //////////////////////////////////////////////////////////////////////////////////////
    int width=0, height=0;
    uint32_t* buffer=NULL;
    if (ok) {
        QSettings settings(previewSettingsFilename, QSettings::IniFormat);
        ecamera->useCameraSettings(camera, settings);
        width=ecamera->getCameraImageWidth(camera);
        height=ecamera->getImageCameraHeight(camera);
        buffer=(uint32_t*)calloc(width*height, sizeof(uint32_t));
        if (!buffer) {
            ok=false;
        }
    }


    //////////////////////////////////////////////////////////////////////////////////////
    // OPEN OUTPUT TIFF FILES
    //////////////////////////////////////////////////////////////////////////////////////
    if (ok) {
        QMap<QString, QVariant> acqD;
        QTime time=QTime::currentTime();
        if (ecamera->acquireOnCamera(camera, buffer, NULL, &acqD)) {
            if (acquisitionDescription) {
                QMapIterator<QString, QVariant> it(acqD);
                while (it.hasNext()) {
                    it.next();
                    (*acquisitionDescription)[acquisitionDescriptionPrefix+it.key()]=it.value();
                }
                (*acquisitionDescription)[acquisitionDescriptionPrefix+"/dualview_mode"]=optSetup->dualViewMode(ecamera, camera);
                (*acquisitionDescription)[acquisitionDescriptionPrefix+"/image_width"]=width;
                (*acquisitionDescription)[acquisitionDescriptionPrefix+"/image_height"]=height;
                (*acquisitionDescription)[acquisitionDescriptionPrefix+"/exposure_time"]=ecamera->getCameraExposureTime(camera);
                optSetup->saveLightpathConfig((*acquisitionDescription), optSetup->getCurrentLightpath(), acquisitionDescriptionPrefix+"/lightpath/", QList<bool>(), true);
                (*acquisitionDescription)[acquisitionDescriptionPrefix+"/timestamp"]=time;
                getAdditionalCameraSettings(ecamera, camera, acquisitionDescriptionPrefix, (*acquisitionDescription));

            }
            QDir().mkpath(QFileInfo(TIFFFIlename.toAscii().data()).absolutePath());
            tiff=TIFFOpen(TIFFFIlename.toAscii().data(), "w");
            if (!tiff) {
                ok=false;
            } else {
                TIFFTWriteUint16from32(tiff, buffer, width, height, false);
                TIFFClose(tiff);
            }
            bool is32bit=false;
            for (int i=0; i<width*height; i++) {
                if ((buffer[i]&0xFFFF0000) != 0) {
                    is32bit=true;
                    break;
                }
            }
            if (ok && is32bit) {
                tiff=TIFFOpen(TIFFFIlename32.toAscii().data(), "w");
                if (!tiff) {
                    ok=false;
                } else {
                    if (filename32) {
                        *filename32=TIFFFIlename32;
                    }
                    TIFFTWriteUint32(tiff, buffer, width, height);
                    TIFFClose(tiff);
                }
            }
        } else {
            ok=false;
        }
        if (buffer) free(buffer);
    }

    return ok;
}






#define ACQUISITION_ERROR(message) \
    log_error(QString("  - ")+(message)+QString("\n")); \
    QMessageBox::critical(this, tr("B040SPIM: Image Stack Acquisition Error"), (message));

void QFESPIMB040MainWindow2::doAcquisition() {
    widAcquisition->performAcquisition();
    /*if (!(widAcquisition->use1() || widAcquisition->use2())) return;

    QDateTime startDateTime=QDateTime::currentDateTime();
    QList<QFESPIMB040OpticsSetup::measuredValues> measured;

    bool ok=true;
    log_text(tr("starting image series acquisition:\n"));
    optSetup->lockLightpath();

    int repeatCnt=0;
    bool userCanceled=false;

    bool doOverviewA1=!widAcquisition->onlyAcquisition1();
    bool doOverviewA2=!widAcquisition->onlyAcquisition2();


    QProgressListDialog progress(tr("Image Series Acquisition"), tr("&Cancel"), 0, 100, this);
    progress.setWindowModality(Qt::WindowModal);
    //progress.setMinimumDuration(0);
    progress.setValue(0);
    progress.addItem(tr("performing run %1/%2").arg(1).arg(widAcquisition->repeats()));
    progress.addItem(tr("initializing cameras"));
    progress.addItem(tr("preparing cameras"));
    if (widAcquisition->saveBackground()) progress.addItem(tr("acquire background images"));
    progress.addItem(tr("acquire overview images before acquisition"));
    progress.addItem(tr("acquire image series"));
    progress.addItem(tr("acquire overview images after acquisition"));
    progress.addItem(tr("clean up"));
    progress.setHasProgressBar(true);
    progress.show();
    progress.defineIcon(QProgressListWidget::statusUser, QIcon(":/spimb040/clock.png"));
    progress.setItemStatus(0, QProgressListWidget::statusUser);


    while (ok && repeatCnt<widAcquisition->repeats() && !userCanceled) {
        progress.reset();
        progress.start();
        progress.setItemText(0, tr("performing run %1/%2").arg(repeatCnt+1).arg(widAcquisition->repeats()));


        progress.setLabelText(tr("setting up acquisition %1/%2 ...").arg(repeatCnt+1).arg(widAcquisition->repeats()));
        ok=true;
        //////////////////////////////////////////////////////////////////////////////////////
        // collect common acquisition data
        //////////////////////////////////////////////////////////////////////////////////////
        QMap<QString, QVariant> acquisitionDescription1, acquisitionDescription2;
        QMap<QString, QVariant> backgroundDescription1, backgroundDescription2;
        acquisitionDescription1["type"]="acquisition";
        acquisitionDescription2["type"]="acquisition";
        QList<QFExtensionCamera::CameraAcquititonFileDescription> moreFiles1, moreFiles2;
        QList<QFExtensionCamera::CameraAcquititonFileDescription> backgroundFiles1, backgroundFiles2;


        //////////////////////////////////////////////////////////////////////////////////////
        // SET LIGHTPATH
        //////////////////////////////////////////////////////////////////////////////////////
        QString oldLightpath=optSetup->getCurrentLightpathFilename();
        QString lightpathName=widAcquisition->lightpath();
        if (widAcquisition->lightpathActivated()) {
            if (!QFile::exists(widAcquisition->lightpathFilename())) {
                ACQUISITION_ERROR(tr("  - acquisition lighpath configuration '%1' does not exist!\n").arg(widAcquisition->lightpath()));
                optSetup->unlockLightpath();
                return;

            } else {
                log_text(tr("  - setting acquisition lightpath settings '%1' ...\n").arg(widAcquisition->lightpath()));
                optSetup->loadLightpathConfig(widAcquisition->lightpathFilename(), true);
                log_text(tr("  - setting acquisition lightpath settings '%1' ... DONE\n").arg(widAcquisition->lightpath()));
            }
        }

        //////////////////////////////////////////////////////////////////////////////////////
        // switch off light
        //////////////////////////////////////////////////////////////////////////////////////
        if (optSetup->isMainIlluminationShutterAvailable()){
            log_text(tr("  - switch main shutter on!\n"));
            setMainIlluminationShutter(false, true);
        }

        //////////////////////////////////////////////////////////////////////////////////////
        // lock cameras for use by this routine
        //////////////////////////////////////////////////////////////////////////////////////
        bool useCam1=false;
        QFExtension* extension1=NULL;
        QFExtensionCamera* ecamera1=NULL;
        int camera1=0;
        QString acquisitionSettingsFilename1="", previewSettingsFilename1="";
        QString acquisitionPrefix1=widAcquisition->prefix1();
        QString acquisitionPrefix2=widAcquisition->prefix2();
        progress.nextItem();



        //int backgroundFrames1=1;
        if (widAcquisition->use1()) {
            if (!QDir().mkpath(QFileInfo(acquisitionPrefix1+".txt").absolutePath())) {
                ok=false;
                ACQUISITION_ERROR(tr("  - could not create directory '%1'!\n").arg(QFileInfo(acquisitionPrefix1+".txt").absolutePath()));
            }


            useCam1=optSetup->lockCamera(0,&extension1, &ecamera1, &camera1, &previewSettingsFilename1);
            if (QFile::exists(widAcquisition->currentConfigFilename(0))) acquisitionSettingsFilename1=widAcquisition->currentConfigFilename(0);
            if (QFile::exists(widAcquisition->currentPreviewConfigFilename(0))) previewSettingsFilename1=widAcquisition->currentPreviewConfigFilename(0);
            //backgroundFrames1=widAcquisition->currentBackgroundFrames(0);
        }

        bool useCam2=false;
        QFExtension* extension2=NULL;
        QFExtensionCamera* ecamera2=NULL;
        QString acquisitionSettingsFilename2="", previewSettingsFilename2="";
        int camera2=0;
        //int backgroundFrames2=1;
        if (ok && widAcquisition->use2()) {
            if (!QDir().mkpath(QFileInfo(acquisitionPrefix2+".txt").absolutePath())) {
                ok=false;
                ACQUISITION_ERROR(tr("  - could not create directory '%1'!\n").arg(QFileInfo(acquisitionPrefix2+".txt").absolutePath()));
            }
            useCam2=optSetup->lockCamera(1,&extension2, &ecamera2, &camera2, &previewSettingsFilename2);
            if (QFile::exists(widAcquisition->currentConfigFilename(1))) acquisitionSettingsFilename2=widAcquisition->currentConfigFilename(1);
            if (QFile::exists(widAcquisition->currentPreviewConfigFilename(1))) previewSettingsFilename2=widAcquisition->currentPreviewConfigFilename(1);
            //backgroundFrames2=widAcquisition->currentBackgroundFrames(1);
        }

        progress.setValue(0);


        if (ok && useCam1) log_text(tr("  - prefix 1: '%1'\n").arg(acquisitionPrefix1));
        if (ok && useCam2) log_text(tr("  - prefix 2: '%1'\n").arg(acquisitionPrefix2));


        //////////////////////////////////////////////////////////////////////////////////////
        // acquire background images
        //////////////////////////////////////////////////////////////////////////////////////
        if (ok && widAcquisition->saveBackground()) {
            progress.nextItem();
            progress.setLabelText(tr("acquiring background frames from cameras ..."));
            QApplication::processEvents();

            //////////////////////////////////////////////////////////////////////////////////////
            // switch off light
            //////////////////////////////////////////////////////////////////////////////////////
            log_text(tr("  - switch main shutter off!\n"));
            ok=ok&setMainIlluminationShutter(false, true);
            if (!ok) {
                ACQUISITION_ERROR(tr("  - could not switch main shutter off!\n"));
            }
            //////////////////////////////////////////////////////////////////////////////////////
            // pacquire background series
            //////////////////////////////////////////////////////////////////////////////////////
            QMap<QFExtensionCamera::CameraSetting, QVariant> camset1, camset2;
            camset1=widAcquisition->getCameraSettings1();
            camset2=widAcquisition->getCameraSettings2();
            camset1[QFExtensionCamera::CamSetNumberFrames]=widAcquisition->currentBackgroundFrames(0);
            camset2[QFExtensionCamera::CamSetNumberFrames]=widAcquisition->currentBackgroundFrames(1);
            ok = acquireSeries(lightpathName, "", tr("background image series"), useCam1&&doOverviewA1, extension1, ecamera1, camera1, acquisitionPrefix1+"_background", acquisitionSettingsFilename1, backgroundDescription1, backgroundFiles1, useCam2&&doOverviewA2, extension2, ecamera2, camera2, acquisitionPrefix2+"_background", acquisitionSettingsFilename2, backgroundDescription2, backgroundFiles2, camset1, camset2, NULL, &progress, &userCanceled);
            if (!ok) {
                ACQUISITION_ERROR(tr("  - error acquiring background image series!\n"));
            } else {
                log_text(tr("  - acquired background image series!\n"));
            }
        }

        //////////////////////////////////////////////////////////////////////////////////////
        // switch on light
        //////////////////////////////////////////////////////////////////////////////////////
        if (optSetup->isMainIlluminationShutterAvailable()) {
            log_text(tr("  - switch main shutter back on!\n"));
            ok=ok&setMainIlluminationShutter(true, true);
        }



        //////////////////////////////////////////////////////////////////////////////////////
        // acquire overview images
        //////////////////////////////////////////////////////////////////////////////////////
        progress.nextItem();
        QList<int> prevs;
        for (int ovrImg=1; ovrImg<widAcquisition->previewCount(); ovrImg++) {
            prevs<<ovrImg;
        }
        prevs<<0;
        for (int ovrImgI=0; ovrImgI<prevs.size(); ovrImgI++) {
            int ovrImg=prevs[ovrImgI];
            if (ok && widAcquisition->lightpathActivatedPreview(ovrImg)) {
                if (ok && useCam1 && doOverviewA1) {
                    progress.setLabelText(tr("acquiring overview image from camera 1, lightpath %1 ...").arg(ovrImg+1));
                    QApplication::processEvents();
                    QString acquisitionPrefix=acquisitionPrefix1+QString("_overviewlp%1.tif").arg(ovrImg+1);
                    QString prefix=QString("overview_lightpath%1").arg(ovrImg+1);
                    if (ovrImg==0) {
                        acquisitionPrefix=acquisitionPrefix1+QString("_overview.tif");
                        prefix=QString("overview");
                    }
                    ok=acquireImageWithLightpath(widAcquisition->lightpathFilenamePreview(ovrImg), widAcquisition->lightpathPreview(ovrImg), extension1, ecamera1, camera1, widAcquisition->currentPreviewConfigFilename(0,ovrImg), acquisitionPrefix, prefix, tr("overview before acquisition with lightpath %1 for camera 1").arg(ovrImg+1), moreFiles1, acquisitionDescription1);
                    if (!ok) {
                        ACQUISITION_ERROR(tr("  - error acquiring overview image from camera 1, lightpath %1!\n").arg(ovrImg+1));
                    } else {
                        log_text(tr("  - acquired overview image from camer 1, lightpath %1!\n").arg(ovrImg+1));
                    }

                }
                if (ok && useCam2 && doOverviewA2) {
                    progress.setLabelText(tr("acquiring overview image from camera 2, lightpath %1 ...").arg(ovrImg+1));
                    QApplication::processEvents();
                    QString acquisitionPrefix=acquisitionPrefix1+QString("_overviewlp%1.tif").arg(ovrImg+1);
                    QString prefix=QString("overview_lightpath%1").arg(ovrImg+1);
                    if (ovrImg==0) {
                        acquisitionPrefix=acquisitionPrefix1+QString("_overview.tif");
                        prefix=QString("overview");
                    }
                    ok=acquireImageWithLightpath(widAcquisition->lightpathFilenamePreview(ovrImg), widAcquisition->lightpathPreview(ovrImg), extension2, ecamera2, camera2, widAcquisition->currentPreviewConfigFilename(1,ovrImg), acquisitionPrefix2+QString("_overviewlp%1.tif").arg(ovrImg+1), QString("overview_lightpath%1").arg(ovrImg+1), tr("overview before acquisition with lightpath %1 for camera 2").arg(ovrImg+1), moreFiles2, acquisitionDescription2);
                    if (!ok) {
                        ACQUISITION_ERROR(tr("  - error acquiring overview image from camera 2, lightpath %1!\n").arg(ovrImg+1));
                    } else {
                        log_text(tr("  - acquired overview image from camer 2, lightpath %1!\n").arg(ovrImg+1));
                    }
                }
            }
        }

        //////////////////////////////////////////////////////////////////////////////////////
        // SET ACQUISITION LIGHTPATH
        //////////////////////////////////////////////////////////////////////////////////////
        if (widAcquisition->lightpathActivated()) {
            if (!QFile::exists(widAcquisition->lightpathFilename())) {
                ACQUISITION_ERROR(tr("  - acquisition lighpath configuration '%1' does not exist!\n").arg(widAcquisition->lightpath()));
                optSetup->unlockLightpath();
                return;
            } else {
                log_text(tr("  - setting acquisition lightpath settings '%1' ...\n").arg(widAcquisition->lightpath()));
                optSetup->loadLightpathConfig(widAcquisition->lightpathFilename(), true);
                log_text(tr("  - setting acquisition lightpath settings '%1' ... DONE\n").arg(widAcquisition->lightpath()));
            }
        }

        //////////////////////////////////////////////////////////////////////////////////////
        // acquire image series
        //////////////////////////////////////////////////////////////////////////////////////
        progress.nextItem();
        QMap<QFExtensionCamera::CameraSetting, QVariant> camset1, camset2;
        camset1=widAcquisition->getCameraSettings1();
        camset2=widAcquisition->getCameraSettings2();
        ok = acquireSeries(lightpathName, "acquisition", tr("image series"), useCam1, extension1, ecamera1, camera1, acquisitionPrefix1, acquisitionSettingsFilename1, acquisitionDescription1, moreFiles1, useCam2, extension2, ecamera2, camera2, acquisitionPrefix2, acquisitionSettingsFilename2, acquisitionDescription2, moreFiles2, camset1, camset2, &measured, &progress, &userCanceled);
        if (!ok) {
            ACQUISITION_ERROR(tr("  - error acquiring images!\n"));
        } else {
            log_text(tr("  - acquired image series!\n"));
        }





        //////////////////////////////////////////////////////////////////////////////////////
        // acquire overview images
        //////////////////////////////////////////////////////////////////////////////////////
        progress.nextItem();
        prevs.clear();
        for (int ovrImg=0; ovrImg<widAcquisition->previewCount(); ovrImg++) {
            prevs<<ovrImg;
        }
        for (int ovrImgI=0; ovrImgI<prevs.size(); ovrImgI++) {
            int ovrImg=prevs[ovrImgI];
            if (ok && widAcquisition->lightpathActivatedPreview(ovrImg)) {
                if (ok && useCam1 && doOverviewA1) {
                    progress.setLabelText(tr("acquiring overview image from camera 1, lightpath %1 ...").arg(ovrImg+1));
                    QApplication::processEvents();
                    QString acquisitionPrefix=acquisitionPrefix1+QString("_overview_afterlp%1.tif").arg(ovrImg+1);
                    QString prefix=QString("overview_after_lightpath%1").arg(ovrImg+1);
                    if (ovrImg==0) {
                        acquisitionPrefix=acquisitionPrefix1+QString("_overview_after.tif");
                        prefix=QString("overview_after");
                    }
                    ok=acquireImageWithLightpath(widAcquisition->lightpathFilenamePreview(ovrImg), widAcquisition->lightpathPreview(ovrImg), extension1, ecamera1, camera1, widAcquisition->currentPreviewConfigFilename(0,ovrImg), acquisitionPrefix, prefix, tr("overview after acquisition with lightpath %1 for camera 1").arg(ovrImg+1), moreFiles1, acquisitionDescription1);
                    if (!ok) {
                        ACQUISITION_ERROR(tr("  - error acquiring overview image from camera 1, lightpath %1!\n").arg(ovrImg+1));
                    } else {
                        log_text(tr("  - acquired overview image from camer 1, lightpath %1!\n").arg(ovrImg+1));
                    }

                }
                if (ok && useCam2 && doOverviewA2) {
                    progress.setLabelText(tr("acquiring overview image from camera 2, lightpath %1 ...").arg(ovrImg+1));
                    QApplication::processEvents();
                    QString acquisitionPrefix=acquisitionPrefix1+QString("_overview_afterlp%1.tif").arg(ovrImg+1);
                    QString prefix=QString("overview_after_lightpath%1").arg(ovrImg+1);
                    if (ovrImg==0) {
                        acquisitionPrefix=acquisitionPrefix1+QString("_overview_after.tif");
                        prefix=QString("overview_after");
                    }
                    ok=acquireImageWithLightpath(widAcquisition->lightpathFilenamePreview(ovrImg), widAcquisition->lightpathPreview(ovrImg), extension2, ecamera2, camera2, widAcquisition->currentPreviewConfigFilename(1,ovrImg), acquisitionPrefix, prefix, tr("overview after acquisition with lightpath %1 for camera 2").arg(ovrImg+1), moreFiles2, acquisitionDescription2);
                    if (!ok) {
                        ACQUISITION_ERROR(tr("  - error acquiring overview image from camera 2, lightpath %1!\n").arg(ovrImg+1));
                    } else {
                        log_text(tr("  - acquired overview image from camer 2, lightpath %1!\n").arg(ovrImg+1));
                    }
                }
            }
        }


        progress.nextItem();
        //////////////////////////////////////////////////////////////////////////////////////
        // RESET LIGHTPATH
        //////////////////////////////////////////////////////////////////////////////////////
        if (widAcquisition->lightpathActivated()) {
            optSetup->loadLightpathConfig(oldLightpath, false);
            log_text(tr("  - resetting acquisition lightpath ...\n"));
        }

        //////////////////////////////////////////////////////////////////////////////////////
        // write acquisition data
        //////////////////////////////////////////////////////////////////////////////////////
        if (ok && useCam1) {
            log_text(tr("  - writing acquisition description 1 ..."));
            // add the background stuff to the description, files list
            for (int i=0; i<backgroundFiles1.size(); i++) {
                QFExtensionCamera::CameraAcquititonFileDescription d=backgroundFiles1[i];
                d.description="background "+d.description;
                moreFiles1.append(d);
            }
            QMapIterator<QString, QVariant> it(backgroundDescription1);
            while (it.hasNext()) {
                it.next();
                if ( (!acquisitionDescription1.contains(it.key())) || (acquisitionDescription1.value(it.key(), it.value())!=it.value()) ) {
                    acquisitionDescription1["background/"+it.key()]=it.value();
                }
            }

            QString MeasurementsFilename=saveMeasuredData(acquisitionPrefix1, measured);
            if (!MeasurementsFilename.isEmpty() && QFile::exists(MeasurementsFilename)) {
                QFExtensionCamera::CameraAcquititonFileDescription d;
                d.name=MeasurementsFilename;
                d.description="measureable properties of setup";
                d.type="CSV";
                moreFiles1.append(d);
            }

            saveAcquisitionDescription(0, extension1, ecamera1, camera1, acquisitionPrefix1, acquisitionDescription1, moreFiles1, startDateTime, false);
            log_text(tr(" DONE!\n"));
        }
        if (ok && useCam2) {
            log_text(tr("  - writing acquisition description 2 ..."));
            // add the background stuff to the description, files list
            for (int i=0; i<backgroundFiles2.size(); i++) {
                QFExtensionCamera::CameraAcquititonFileDescription d=backgroundFiles2[i];
                d.description="background "+d.description;
                moreFiles2.append(d);
            }
            QMapIterator<QString, QVariant> it(backgroundDescription2);
            while (it.hasNext()) {
                it.next();
                if ( (!acquisitionDescription2.contains(it.key())) || (acquisitionDescription2.value(it.key(), it.value())!=it.value()) ) {
                    acquisitionDescription2["background/"+it.key()]=it.value();
                }
            }
            QString MeasurementsFilename=saveMeasuredData(acquisitionPrefix2, measured);
            if (!MeasurementsFilename.isEmpty() && QFile::exists(MeasurementsFilename)) {
                QFExtensionCamera::CameraAcquititonFileDescription d;
                d.name=MeasurementsFilename;
                d.description="measureable properties of setup";
                d.type="CSV";
                moreFiles2.append(d);
            }
            saveAcquisitionDescription(1, extension2, ecamera2, camera2, acquisitionPrefix2, acquisitionDescription2, moreFiles2, startDateTime, false);
            log_text(tr(" DONE!\n"));
        }

        //////////////////////////////////////////////////////////////////////////////////////
        // release cameras
        //////////////////////////////////////////////////////////////////////////////////////
        if (useCam1) {
            optSetup->releaseCamera(0);
            log_text(tr("  - released camera 1!\n"));
        }
        if (useCam2) {
            optSetup->releaseCamera(1);
            log_text(tr("  - released camera 2!\n"));
        }


        if (ok && useCam1) log_text(tr("  - stored files with prefix 1: '%1'\n").arg(acquisitionPrefix1));
        if (ok && useCam2) log_text(tr("  - stored files with prefix 2: '%1'\n").arg(acquisitionPrefix2));
        repeatCnt++;
    }
    optSetup->unlockLightpath();
    log_text(tr("image series acquisition DONE!\n"));
    optSetup->ensureLightpath();*/

}


QString QFESPIMB040MainWindow2::saveAcquisitionDescription(int use_cam, QFExtension* extension, QFExtensionCamera* ecamera, int camera, const QString& filenamePrefix, const QMap<QString, QVariant>& acquisitionDescription, const QList<QFExtensionCamera::CameraAcquititonFileDescription>& moreFiles, QDateTime startDateTime, bool getAcquisitionSettings) {
    QString iniFilename=filenamePrefix+".configuration.ini";
    QSettings settings(iniFilename, QSettings::IniFormat);

    double magnification=optSetup->getCameraMagnification(use_cam);

    QList<QFExtensionCamera::CameraAcquititonFileDescription> files;
    QMap<QString, QVariant> parameters;
    if (getAcquisitionSettings) ecamera->getCameraAcquisitionDescription(camera, &files, &parameters);

    // WRITE ACQUISITION SETTINGS

    {
        QMapIterator <QString, QVariant> it1(acquisitionDescription);
        while (it1.hasNext()) {
            it1.next();
            if (!it1.value().toString().isEmpty()) {
                if (it1.value().type()==QVariant::List) {
                    settings.setValue("acquisition/"+it1.key(), jkVariantListToString(it1.value().toList(), "; "));
                } else {
                    settings.setValue("acquisition/"+it1.key(), it1.value().toString());
                }

            }
        }
    }
    {
        QMapIterator <QString, QVariant> it(parameters);
        while (it.hasNext()) {
            it.next();
            if (!it.value().toString().isEmpty()) {
                if (it.value().type()==QVariant::List) {
                    settings.setValue("acquisition/"+it.key(), jkVariantListToString(it.value().toList(), "; "));
                } else {
                    settings.setValue("acquisition/"+it.key(), it.value().toString());
                }

            }
        }
    }
    settings.setValue("acquisition/pixel_width", ecamera->getCameraPixelWidth(camera)/magnification);
    settings.setValue("acquisition/pixel_height", ecamera->getCameraPixelHeight(camera)/magnification);
    settings.setValue("acquisition/camera_pixel_width", ecamera->getCameraPixelWidth(camera));
    settings.setValue("acquisition/camera_pixel_height", ecamera->getCameraPixelHeight(camera));
    settings.setValue("acquisition/camera_model", ecamera->getCameraName(camera));
    settings.setValue("acquisition/sensor_model", ecamera->getCameraSensorName(camera));
    settings.setValue("acquisition/magnification", magnification);


    // OPTICS SETUP
    QMap<QString, QVariant> setup=optSetup->getSetup(use_cam);
    {
        QMapIterator <QString, QVariant> it(setup);
        while (it.hasNext()) {
            it.next();
            if (!it.value().toString().isEmpty()) {
                if (it.value().type()==QVariant::List) {
                    settings.setValue("setup/"+it.key(), jkVariantListToString(it.value().toList(), "; "));
                } else {
                    settings.setValue("setup/"+it.key(), it.value().toString());
                }

            }
        }
    }

    // Experiment Description
    QMap<QString, QVariant> experiment=widExperimentDescription->getDescription();
    experiment=experiment.unite(widAcquisitionDescription->getDescription());
    experiment["start_time"]= startDateTime;
    {
        QMapIterator <QString, QVariant> it(experiment);
        while (it.hasNext()) {
            it.next();
            if (!it.value().toString().isEmpty()) {
                if (it.value().type()==QVariant::List) {
                    settings.setValue("experiment/"+it.key(), jkVariantListToString(it.value().toList(), "; "));
                } else {
                    settings.setValue("experiment/"+it.key(), it.value().toString());
                }

            }
        }
    }

    // WRITE FILES LIST
    settings.setValue("files/count", files.size()+moreFiles.size());
    for (int i=0; i<moreFiles.size(); i++) {
        settings.setValue("files/name"+QString::number(i), QFileInfo(settings.fileName()).absoluteDir().relativeFilePath(moreFiles[i].name));
        settings.setValue("files/type"+QString::number(i), moreFiles[i].type);
        settings.setValue("files/description"+QString::number(i), moreFiles[i].description);
    }
    for (int i=0; i<files.size(); i++) {
        settings.setValue("files/name"+QString::number(moreFiles.size()+i), QFileInfo(settings.fileName()).absoluteDir().relativeFilePath(files[i].name));
        settings.setValue("files/type"+QString::number(moreFiles.size()+i), files[i].type);
        settings.setValue("files/description"+QString::number(moreFiles.size()+i), files[i].description);
    }
    settings.sync();
    return iniFilename;
}


QString QFESPIMB040MainWindow2::savePreviewDescription(int use_cam, QFExtension* extension, QFExtensionCamera* ecamera, int camera, const QString& filenamePrefix, const QMap<QString, QVariant>& acquisitionDescription, const QList<QFExtensionCamera::CameraAcquititonFileDescription>& files, QDateTime startDateTime, const QString& lightpathPrefix, const QString& prefix) {
    QString iniFilename=filenamePrefix+".configuration.ini";
    QSettings settings(iniFilename, QSettings::IniFormat);

    settings.beginGroup(prefix);

    double magnification=optSetup->getCameraMagnification(use_cam);

    QMap<QString, QVariant> parameters;

    // WRITE ACQUISITION SETTINGS

    QMapIterator <QString, QVariant> it1(acquisitionDescription);
    while (it1.hasNext()) {
        it1.next();
        if (!it1.value().toString().isEmpty()) {
            if (it1.value().type()==QVariant::List) {
                settings.setValue("acquisition/"+it1.key(), jkVariantListToString(it1.value().toList(), "; "));
            } else {
                settings.setValue("acquisition/"+it1.key(), it1.value().toString());
            }

        }
    }
    settings.setValue("acquisition/pixel_width", ecamera->getCameraPixelWidth(camera)*magnification);
    settings.setValue("acquisition/pixel_height", ecamera->getCameraPixelHeight(camera)*magnification);
    settings.setValue("acquisition/camera_pixel_width", ecamera->getCameraPixelWidth(camera));
    settings.setValue("acquisition/camera_pixel_height", ecamera->getCameraPixelHeight(camera));
    settings.setValue("acquisition/camera_model", ecamera->getCameraName(camera));
    settings.setValue("acquisition/sensor_model", ecamera->getCameraSensorName(camera));
    settings.setValue("acquisition/exposure", ecamera->getCameraExposureTime(camera));
    settings.setValue("acquisition/image_width", ecamera->getCameraImageWidth(camera));
    settings.setValue("acquisition/image_height", ecamera->getImageCameraHeight(camera));
    settings.setValue("acquisition/magnification", magnification);
    // OPTICS SETUP
    QMap<QString, QVariant> setup=optSetup->getSetup(use_cam);
    {
        QMapIterator <QString, QVariant> it(setup);
        while (it.hasNext()) {
            it.next();
            if (!it.value().toString().isEmpty()) {
                if (it.value().type()==QVariant::List) {
                    settings.setValue(lightpathPrefix+"setup/"+it.key(), jkVariantListToString(it.value().toList(), "; "));
                } else {
                    settings.setValue(lightpathPrefix+"setup/"+it.key(), it.value().toString());
                }

            }
        }
    }

    // Experiment Description
    QMap<QString, QVariant> experiment=widExperimentDescription->getDescription();
    experiment=experiment.unite(widAcquisitionDescription->getDescription());
    experiment["start_time"]= startDateTime;
    {
        QMapIterator <QString, QVariant> it(experiment);
        while (it.hasNext()) {
            it.next();
            if (!it.value().toString().isEmpty()) {
                if (it.value().type()==QVariant::List) {
                    settings.setValue("experiment/"+it.key(), jkVariantListToString(it.value().toList(), "; "));
                } else {
                    settings.setValue("experiment/"+it.key(), it.value().toString());
                }

            }
        }
    }
    // WRITE FILES LIST
    settings.setValue("files/count", files.size());
    for (int i=0; i<files.size(); i++) {
        settings.setValue("files/name"+QString::number(i), QFileInfo(settings.fileName()).absoluteDir().relativeFilePath(files[i].name));
        settings.setValue("files/type"+QString::number(i), files[i].type);
        settings.setValue("files/description"+QString::number(i), files[i].description);
    }

    settings.sync();
    return iniFilename;
}


QString QFESPIMB040MainWindow2::saveMeasuredData(const QString& filenamePrefix, const QList<QFESPIMB040OpticsSetup::measuredValues>& data) {
    QString csvFilename=filenamePrefix+".measured.dat";

    QFile file(csvFilename);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out.setCodec(QTextCodec::codecForName("ISO-8859-1"));
        out.setLocale(QLocale::c());
        QStringList columns;
        columns<<"time [s]";
        for (int i=0; i<data.size(); i++) {
            QMapIterator <QString, QVariant> it1(data[i].data);
            while (it1.hasNext()) {
                it1.next();
                if (!columns.contains(it1.key())) columns<<it1.key();
            }
        }

        out<<"#! ";
        for (int c=0; c<columns.size(); c++) {
            if (c>0) out<<",\t";
            out<<columns[c];
        }
        out<<"\n";
        for (int i=0; i<data.size(); i++) {
            out<<double(data[0].time.msecsTo(data[i].time))/1000.0;
            //out<<",\t\""<<data[0].time.toString("yyyy.MM.dd-hh:mm:ss.zzz")<<"\"";
            for (int j=1; j<columns.size(); j++) {
                QVariant v=data[i].data.value(columns[j], QVariant());
                out<<",\t";
                if (   v.type()==QVariant::Bool
                       || v.type()==QVariant::Char
                       || v.type()==QVariant::Int
                       || v.type()==QVariant::LongLong ) {
                    out<<v.toLongLong();

                } else if ( v.type()==QVariant::UInt || v.type()==QVariant::ULongLong ) {
                    out<<v.toULongLong();
                } else if ( v.type()==QVariant::Double ) {
                    out<<v.toDouble();
                } else if (v.canConvert(QVariant::String)) {
                    out<<"\""<<v.toString()<<"\"";
                }
            }
            out<<"\n";
        }
        file.close();

    } else {
        csvFilename="";
    }


    return csvFilename;
}

void QFESPIMB040MainWindow2::log_text(QString message) {
    logMain->log_text(message);
    //optSetup->log_text(message);
};

void QFESPIMB040MainWindow2::log_warning(QString message) {
    logMain->log_warning(message);
    //optSetup->log_warning(message);
};

void QFESPIMB040MainWindow2::log_error(QString message) {
    logMain->log_error(message);
    //optSetup->log_error(message);
}

void QFESPIMB040MainWindow2::setStyleAndStylesheet(QWidget* widget, const QString &style, const QString &stylesheet) {
    widget->setStyle(QStyleFactory::create(style));
    widget->setPalette(widget->style()->standardPalette());

    QFile f(stylesheet);
    f.open(QFile::ReadOnly);
    QTextStream s(&f);
    QString qss=s.readAll();
    f.close();
    widget->setStyleSheet(qss);

}

void QFESPIMB040MainWindow2::styleChanged(const QString &style, const QString &stylesheet)
{
    setStyleAndStylesheet(this, style, stylesheet);
}

bool QFESPIMB040MainWindow2::setMainIlluminationShutter(bool on_off, bool blocking) {
    /*if (optSetup->isMainIlluminationShutterAvailable()) {
        optSetup->setMainIlluminationShutter(on_off);
        if (blocking) {
            QTime t;
            t.start();
            while (optSetup->getMainIlluminationShutter()!=on_off && t.elapsed()<10000) {
                QApplication::processEvents();
            }

            if (t.elapsed()>=10000) {
                log_error("main shutter timed out after 10s!\n");
                return false;
            }
        }
        return true;
    }
    log_error("main shutter not available!\n");
    return false;*/
    return optSetup->setMainIlluminationShutter(on_off, blocking);
}

bool QFESPIMB040MainWindow2::prepareCamera(int num, int camera, QFExtensionCamera *ecamera, const QString& acquisitionSettingsFilename, int &width, int &height, uint32_t **buffer, const QString& acquisitionTitle) {
    width=0;
    height=0;
    *buffer=NULL;
    QSettings settings(acquisitionSettingsFilename, QSettings::IniFormat);
    ecamera->useCameraSettings(camera, settings);
    width=ecamera->getCameraImageWidth(camera);
    height=ecamera->getImageCameraHeight(camera);
    *buffer=(uint32_t*)calloc(width*height, sizeof(uint32_t));
    if (!(*buffer)) {
        SPIMACQUISITION_ERROR(tr("could not allocate image buffer for camera 1!\n"), acquisitionTitle);
        return false;
    }
    log_text(tr("  - prepared camera %1!\n").arg(num));
    return true;
}




bool QFESPIMB040MainWindow2::connectStageForAcquisition(QFExtensionLinearStage* stage, int stageAxis, double& stageInitialPos, const QString& acquisitionTitle, int stageNum) {
    bool ok=true;
    stageInitialPos=0;
    if (!stage) {

        SPIMACQUISITION_ERROR(tr("no stage %1 selected").arg(stageNum), acquisitionTitle);
        optSetup->unlockStages();
        optSetup->unlockLightpath();
        return false;
    }
    if (ok && (!stage->isConnected(stageAxis))) {
        log_text(tr("  - connecting to stage %3: '%1', axis %2!\n").arg(widImageStack->stageExtension()->getName()).arg(stageAxis).arg(stageNum));
        stage->connectDevice(stageAxis);
    }
    if (ok && (!stage->isConnected(stageAxis)))  {
        ok=false;
        SPIMACQUISITION_ERROR(tr("error connecting to stage %3: '%1', axis %2").arg(widImageStack->stageExtension()->getName()).arg(stageAxis).arg(stageNum), acquisitionTitle);
    }
    if (ok) {
        stage->setJoystickActive(stageAxis, false);
        stageInitialPos=stage->getPosition(stageAxis);
    }
    return ok;
}

bool QFESPIMB040MainWindow2::acquireImageWithLightpath(const QString& lightpathFilename, const QString& lightpathName, QFExtension* extension1, QFExtensionCamera* ecamera1, int camera1, const QString& previewSettingsFilename1, const QString& outputFilename, const QString& imageID, const QString& imageDescription, QList<QFExtensionCamera::CameraAcquititonFileDescription>& moreFiles1, QMap<QString, QVariant>& acquisitionDescription1) {

    if (!lightpathFilename.isEmpty()) {
        if (!QFile::exists(lightpathFilename)) {
            log_error(tr("  - acquisition lighpath configuration '%1' does not exist!\n").arg(lightpathName));
            //optSetup->unlockLightpath();
            return false;

        } else {
            log_text(tr("  - setting acquisition lightpath settings '%1' ...\n").arg(lightpathName));
            optSetup->loadLightpathConfig(lightpathFilename, true);
            log_text(tr("  - setting acquisition lightpath settings '%1' ... DONE\n").arg(lightpathName));
        }
    }


    QDateTime time=QDateTime::currentDateTime();
    QString filename32="";
    bool ok=savePreview(extension1, ecamera1, camera1, previewSettingsFilename1, outputFilename, &filename32, &acquisitionDescription1, imageID+"/");
    if (ok) {
        log_text(tr("  - acquired %1!\n").arg(imageDescription));
        acquisitionDescription1[imageID+"/image_width"]=ecamera1->getCameraImageWidth(camera1);
        acquisitionDescription1[imageID+"/image_height"]=ecamera1->getImageCameraHeight(camera1);
        acquisitionDescription1[imageID+"/exposure_time"]=ecamera1->getCameraExposureTime(camera1);
        optSetup->saveLightpathConfig(acquisitionDescription1, lightpathName, imageID+"/lightpath/", QList<bool>(), true);
        acquisitionDescription1[imageID+"/dualview_mode"]=optSetup->dualViewMode(ecamera1, camera1);
        acquisitionDescription1[imageID+"/timestamp"]=time;
        getAdditionalCameraSettings(ecamera1, camera1, imageID, acquisitionDescription1);

        QFExtensionCamera::CameraAcquititonFileDescription d;
        d.description=imageDescription;
        d.name=outputFilename;
        d.type="TIFF16";
        moreFiles1.append(d);
        if (!filename32.isEmpty()) {
            d.description=imageDescription;
            d.name=filename32;
            d.type="TIFF32";
            moreFiles1.append(d);
        }
    }
    return ok;
}

bool QFESPIMB040MainWindow2::acquireSeries(const QString& lightpathName, const QString& imageID, const QString& imageDescription, bool useCam1, QFExtension* extension1, QFExtensionCamera* ecamera1, int camera1, const QString& acquisitionPrefix1, const QString& acquisitionSettingsFilename1, QMap<QString, QVariant>& acquisitionDescription1, QList<QFExtensionCamera::CameraAcquititonFileDescription>& moreFiles1, bool useCam2, QFExtension* extension2, QFExtensionCamera* ecamera2, int camera2, const QString& acquisitionPrefix2, const QString& acquisitionSettingsFilename2, QMap<QString, QVariant>& acquisitionDescription2, QList<QFExtensionCamera::CameraAcquititonFileDescription>& moreFiles2, const QMap<QFExtensionCamera::CameraSetting, QVariant>& camsettings1, const QMap<QFExtensionCamera::CameraSetting, QVariant>& camsettings2, QList<QFESPIMB040OpticsSetup::measuredValues>* measured, QProgressListDialog* progress, bool* userCanceled )
{
    QString tmpName1=QDir::temp().absoluteFilePath("qf3spimb040_cam1tmpsettings.ini");

    QTemporaryFile file1;
    if (file1.open()) {
         tmpName1=file1.fileName();
    }
    if (QFile::exists(tmpName1)) QFile::remove(tmpName1);

    if (useCam1) QFile::copy(acquisitionSettingsFilename1, tmpName1);

    QSettings settings1(tmpName1, QSettings::IniFormat);

    //if (useCam1 && frames1>0 && ecamera1->isCameraSettingChangable(QFExtensionCamera::CamSetNumberFrames)) ecamera1->changeCameraSetting(settings1, QFExtensionCamera::CamSetNumberFrames, frames1);
    if (useCam1) {
        QMapIterator<QFExtensionCamera::CameraSetting, QVariant> it(camsettings1);
        while (it.hasNext()) {
            it.next();
            if (it.value().isValid() && ecamera1->isCameraSettingChangable(it.key())) {
                ecamera1->changeCameraSetting(settings1, it.key(), it.value());
            }
        }
    }







    QString tmpName2=QDir::temp().absoluteFilePath("qf3spimb040_cam2tmpsettings.ini");

    QTemporaryFile file2;
    if (file2.open()) {
         tmpName2=file2.fileName();
    }
    if (QFile::exists(tmpName2)) QFile::remove(tmpName2);

    if (useCam2) QFile::copy(acquisitionSettingsFilename2, tmpName2);

    QSettings settings2(tmpName2, QSettings::IniFormat);

    //if (useCam2 && frames2>0 && ecamera2->isCameraSettingChangable(QFExtensionCamera::CamSetNumberFrames)) ecamera2->changeCameraSetting(settings2, QFExtensionCamera::CamSetNumberFrames, frames2);
    if (useCam2) {
        QMapIterator<QFExtensionCamera::CameraSetting, QVariant> it(camsettings2);
        while (it.hasNext()) {
            it.next();
            if (it.value().isValid() && ecamera2->isCameraSettingChangable(it.key())) {
                ecamera2->changeCameraSetting(settings2, it.key(), it.value());
            }
        }
    }




    bool result=acquireSeries(lightpathName, imageID, imageDescription, useCam1, extension1, ecamera1, camera1, acquisitionPrefix1, settings1, acquisitionDescription1, moreFiles1, useCam2, extension2, ecamera2, camera2, acquisitionPrefix2, settings2, acquisitionDescription2, moreFiles2, measured, progress, userCanceled);


    if (QFile::exists(tmpName1)) QFile::remove(tmpName1);
    if (QFile::exists(tmpName2)) QFile::remove(tmpName2);

    return result;

}

bool QFESPIMB040MainWindow2::acquireSeries(const QString &lightpathName, const QString &imageID, const QString &imageDescription, bool useCam1, QFExtension *extension1, QFExtensionCamera *ecamera1, int camera1, const QString &acquisitionPrefix1, QSettings &acquisitionSettings1, QMap<QString, QVariant> &acquisitionDescription1, QList<QFExtensionCamera::CameraAcquititonFileDescription> &moreFiles1, bool useCam2, QFExtension *extension2, QFExtensionCamera *ecamera2, int camera2, const QString &acquisitionPrefix2, QSettings &acquisitionSettings2, QMap<QString, QVariant> &acquisitionDescription2, QList<QFExtensionCamera::CameraAcquititonFileDescription> &moreFiles2, QList<QFESPIMB040OpticsSetup::measuredValues> *measured, QProgressListDialog *progress, bool *userCanceled)
{

    bool ok=true;
    QDateTime time;

     //////////////////////////////////////////////////////////////////////////////////////
     // prepare cameras  (set camera settings)
     //////////////////////////////////////////////////////////////////////////////////////
     if (ok && useCam1) {
         if (progress) progress->setLabelText(tr("preparing camera 1 for %1 ...").arg(imageDescription));
         QApplication::processEvents();


         ok=ecamera1->prepareCameraAcquisition(camera1, acquisitionSettings1, acquisitionPrefix1);

         if (ok) {
             log_text(tr("  - prepared camer 1 for %1!\n").arg(imageDescription));
         } else {
             log_error(tr("  - error preparing camera 1 for %1!\n").arg(imageDescription));
             ok=false;
         }

     }
     if (ok && useCam2) {
         if (progress) progress->setLabelText(tr("preparing camera 2 for %1 ...").arg(imageDescription));
         QApplication::processEvents();

         ok=ecamera2->prepareCameraAcquisition(camera2, acquisitionSettings2, acquisitionPrefix2);

         if (ok) {
             log_text(tr("  - prepared camer 2 for %1!\n").arg(imageDescription));
         } else {
             log_error(tr("  - error preparing camera 2 for %1!\n").arg(imageDescription));
             ok=false;
         }

     }

     //////////////////////////////////////////////////////////////////////////////////////
     // start background acquisition and wait until finished
     //////////////////////////////////////////////////////////////////////////////////////
     if (ok) {
         if (progress) progress->setLabelText(tr("acquiring %1 images ...").arg(imageDescription));
         log_text(tr("  - acquiring %1 frames!\n").arg(imageDescription));
         time=QDateTime::currentDateTime();
         if (useCam1) {
             ok=ecamera1->startCameraAcquisition(camera1);
             if (!ok) {
                 log_error(tr("  - error starting acquisition on camera 1 for %1!\n").arg(imageDescription));
                 ok=false;
             }
         }
         if (ok && useCam2) {
             ok=ecamera2->startCameraAcquisition(camera2);
             if (!ok) {
                 log_error(tr("  - error starting acquisition on camera 2 for %1!\n").arg(imageDescription));
                 ok=false;
             }
         }
         bool running=ok;

         QTime time1;
         time1.start();
         QTime timA;
         timA.start();
         while (running) {
             int prog1=99, prog2=99;
             if (useCam1) prog1=ecamera1->getCameraAcquisitionProgress(camera1);
             if (useCam2) prog2=ecamera2->getCameraAcquisitionProgress(camera2);
             progress->setValue(qMin(prog1,prog2));

             QString estimation;
             if (qMin(prog1,prog2)>0) {
                 double duration=double(timA.elapsed())/1000.0;
                 double eta=duration/double(qMin(prog1,prog2))*100.0;
                 double etc=eta-duration;
                 uint mini=floor(etc/60.0);
                 uint secs=round(etc-double(mini)*60.0);
                 estimation=tr("\nest. remaining duration (min:secs): %1:%2 ").arg(mini, 2, 10, QChar('0')).arg(secs, 2, 10, QChar('0'));
                 if (progress) progress->setLabelText(tr("acquiring images ...\n")+estimation);
             }
             if (measured && time1.elapsed()>200) {
                 measured->append(optSetup->getMeasuredValues());
                 time1.start();
             }
             QApplication::processEvents();

             if (progress && progress->wasCanceled()) {
                 if (userCanceled) *userCanceled=true;
                 running=false;
                 if (useCam1) ecamera1->cancelCameraAcquisition(camera1);
                 if (useCam2) ecamera2->cancelCameraAcquisition(camera2);
                 log_warning(tr("  - acquisition canceled by user!\n"));
             } else {
                 bool run1=false;
                 bool run2=false;
                 if (useCam1) run1=ecamera1->isCameraAcquisitionRunning(camera1);
                 if (useCam2) run2=ecamera2->isCameraAcquisitionRunning(camera2);
                 running=run1||run2;
             }
         }
     }
     if (progress) progress->setValue(100);

     //////////////////////////////////////////////////////////////////////////////////////
     // retrieve background acquisition description
     //////////////////////////////////////////////////////////////////////////////////////
     if (ok && useCam1) {
         QMap<QString, QVariant> acquisitionDescription;
         ecamera1->getCameraAcquisitionDescription(camera1, &moreFiles1, &acquisitionDescription);
         QMapIterator<QString, QVariant> it(acquisitionDescription);
         while(it.hasNext()) {
             it.next();
             acquisitionDescription1[imageID+"/"+it.key()]=it.value();
         }

         log_text(tr("  - acquired %1 image from camera 1!\n").arg(imageDescription));
         acquisitionDescription1[imageID+"/timestamp"]=time;
         optSetup->saveLightpathConfig(acquisitionDescription1, lightpathName, imageID+"/lightpath/", QList<bool>(), true);
         acquisitionDescription1[imageID+"/dualview_mode"]=optSetup->dualViewMode(0);
         getAdditionalCameraSettings(ecamera1, camera1, imageID, acquisitionDescription1);
     }
     if (ok && useCam2) {
         QMap<QString, QVariant> acquisitionDescription;
         ecamera2->getCameraAcquisitionDescription(camera2, &moreFiles2, &acquisitionDescription);
         QMapIterator<QString, QVariant> it(acquisitionDescription);
         while(it.hasNext()) {
             it.next();
             acquisitionDescription2[imageID+"/"+it.key()]=it.value();
         }
         log_text(tr("  - acquired %1 image from camera 2!\n").arg(imageDescription));
         acquisitionDescription2[imageID+"/timestamp"]=time;
         optSetup->saveLightpathConfig(acquisitionDescription2, lightpathName, imageID+"/lightpath/", QList<bool>(), true);
         acquisitionDescription2[imageID+"/dualview_mode"]=optSetup->dualViewMode(1);
         getAdditionalCameraSettings(ecamera2, camera2, imageID, acquisitionDescription2);
     }

     return ok;
}

void QFESPIMB040MainWindow2::getAdditionalCameraSettings(QFExtensionCamera *ecamera, int camera, const QString &prefix, QMap<QString, QVariant> &acquisitionDescription) {
    acquisitionDescription[prefix+"/camera_pixel_width"]=ecamera->getCameraPixelWidth(camera);
    acquisitionDescription[prefix+"/camera_pixel_height"]=ecamera->getCameraPixelHeight(camera);
    acquisitionDescription[prefix+"/camera_model"]=ecamera->getCameraName(camera);
    acquisitionDescription[prefix+"/sensor_model"]=ecamera->getCameraSensorName(camera);
    acquisitionDescription[prefix+"/image_width"]=ecamera->getCameraImageWidth(camera);
    acquisitionDescription[prefix+"/image_height"]=ecamera->getImageCameraHeight(camera);
    acquisitionDescription[prefix+"/dualview_mode"]=optSetup->dualViewMode(ecamera, camera);
    optSetup->saveLightpathConfig(acquisitionDescription, "", prefix+"/lightpath/", QList<bool>(), true);
    QMap<QString, QVariant> setup=optSetup->getSetup(optSetup->camNumFromExtension(ecamera, camera));
    QMapIterator <QString, QVariant> it(setup);
    while (it.hasNext()) {
        it.next();
        if (!it.value().toString().isEmpty()) {
            if (it.value().type()==QVariant::List) {
                acquisitionDescription[prefix+"/setup/"+it.key()]=jkVariantListToString(it.value().toList(), "; ");
            } else {
                acquisitionDescription[prefix+"/setup/"+it.key()]=it.value().toString();
            }

        }
    }

    //acquisitionDescription[prefix+"/setting"]=ecamera->getCameraSetting();

}




