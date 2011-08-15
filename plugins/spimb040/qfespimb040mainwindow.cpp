#include "qfespimb040mainwindow.h"
#include <QtGui>





QFESPIMB040MainWindow::QFESPIMB040MainWindow(QFExtensionServices* pluginServices, QWidget* parent):
    QWidget(parent, Qt::Dialog|Qt::WindowMaximizeButtonHint|Qt::WindowCloseButtonHint|Qt::WindowSystemMenuHint)
{
    camConfig1=NULL;
    camConfig2=NULL;
    m_pluginServices=pluginServices;
    // create widgets and actions
    createWidgets(pluginServices->getExtensionManager());
    createActions();
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
    grid->addWidget(new QLabel(tr("<b>camera 1:</b>"), w), 1,0);
    grid->addWidget(new QLabel(tr("<b>camera 2:</b>"), w), 2,0);
    grid->addWidget(new QLabel(tr("<b>use:</b>"), w), 0,1);
    grid->addWidget(new QLabel(tr("<b>filename prefix:</b>"), w), 0,2);
    chkAcquisitionUseCam1=new QCheckBox(w);
    grid->addWidget(chkAcquisitionUseCam1, 1,1);
    chkAcquisitionUseCam2=new QCheckBox(w);
    grid->addWidget(chkAcquisitionUseCam2, 2,1);
    edtAcquisitionPrefix1=new QLineEdit(w);
    grid->addWidget(edtAcquisitionPrefix1, 1,2);
    connect(chkAcquisitionUseCam1,SIGNAL(clicked(bool)), edtAcquisitionPrefix1, SLOT(setEnabled(bool)));
    edtAcquisitionPrefix2=new QLineEdit(w);
    grid->addWidget(edtAcquisitionPrefix2, 2,2);
    connect(chkAcquisitionUseCam2,SIGNAL(clicked(bool)), edtAcquisitionPrefix2, SLOT(setEnabled(bool)));
    btnAcquire=new QPushButton(tr("&Acquire"), w);
    grid->addWidget(btnAcquire, 3, 2);
    connect(btnAcquire, SIGNAL(clicked()), this, SLOT(doAcquisition()));

    tabAcquisition->addTab(w, tr("Image &Series"));
    chkAcquisitionUseCam1->setEnabled(true);
    chkAcquisitionUseCam2->setEnabled(true);


}

void QFESPIMB040MainWindow::createActions() {

}

void QFESPIMB040MainWindow::doAcquisition() {
    if (!(chkAcquisitionUseCam1->isChecked() || chkAcquisitionUseCam2->isChecked())) return;

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

    QProgressDialog progress(tr("Imager Series Acquisition"), tr("&Cancel"), 0, 100, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(0);
    progress.setValue(0);
    log_text(tr("starting image series acquisition:\n"));

    if (ok && useCam1) {
        progress.setLabelText("preparing camera 1 ...");
        QApplication::processEvents();
        QSettings settings(acquisitionSettingsFilename1, QSettings::IniFormat);
        ok=ecamera1->prepareAcquisition(camera1, settings, edtAcquisitionPrefix1->text());
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
        ok=ecamera2->prepareAcquisition(camera2, settings, edtAcquisitionPrefix2->text());
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





