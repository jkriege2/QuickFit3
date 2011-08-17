#include "qfespimb040mainwindow.h"
#include <QtGui>





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

void QFESPIMB040MainWindow::doImageStack() {

}

void QFESPIMB040MainWindow::doAcquisition() {
    if (!(chkAcquisitionUseCam1->isChecked() || chkAcquisitionUseCam2->isChecked())) return;

    int cnt=spinAcquisitionCount->value();
    spinAcquisitionCount->setValue(cnt+1);

    bool ok=true;

    bool useCam1=false;
    QFExtension* extension1=NULL;
    QFExtensionCamera* ecamera1=NULL;
    int camera1=0;
    QString acquisitionSettingsFilename1="", previewSettingsFilename1="";
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

    if (ok && useCam1) {
        progress.setLabelText("preparing camera 1 ...");
        QApplication::processEvents();
        QSettings settings(acquisitionSettingsFilename1, QSettings::IniFormat);
        QString filename= edtAcquisitionPrefix1->text();
        filename=filename.replace("%counter%", QString::number(cnt));
        ok=ecamera1->prepareAcquisition(camera1, settings, filename);
        if (ok) {
            log_text("  - prepared camer 1!\n");
        } else {
            log_error("  - error preparing camer 1!\n");
        }
    }
    if (ok && useCam2) {
        progress.setLabelText("preparing camera 2 ...");
        QApplication::processEvents();
        QSettings settings(acquisitionSettingsFilename2, QSettings::IniFormat);
        QString filename= edtAcquisitionPrefix2->text();
        filename=filename.replace("%counter%", QString::number(cnt));
        ok=ecamera2->prepareAcquisition(camera2, settings, filename);
        if (ok) {
            log_text("  - prepared camer 2!\n");
        } else {
            log_error("  - error preparing camer 2!\n");
        }
    }

    if (ok) {
        progress.setLabelText("acquiring images ...");
        log_text("  - acquiring images!\n");
        if (useCam1) {
            ok=ecamera1->startAcquisition(camera1);
            if (!ok) {
                log_error("  - error starting acquisition on camera 1!\n");
            }
        }
        if (ok && useCam2) {
            ok=ecamera2->startAcquisition(camera2);
            if (!ok) {
                log_error("  - error starting acquisition on camera 1!\n");
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
    //QSettings* settings=new QSettings(filename, QSettings::IniFormat);
    //viewData.camera->useCameraSettings(viewData.usedCamera, *settings);
    //delete settings;


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




void QFESPIMB040MainWindow::log_text(QString message) {
    logMain->log_text(message);
};

void QFESPIMB040MainWindow::log_warning(QString message) {
    logMain->log_warning(message);
};

void QFESPIMB040MainWindow::log_error(QString message) {
    logMain->log_error(message);
};





