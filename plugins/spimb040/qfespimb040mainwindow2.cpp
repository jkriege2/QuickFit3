#include "qfespimb040mainwindow2.h"
#include <QtGui>
#include <tiffio.h>
#include "libtiff_tools.h"



#define sqr(a) qfSqr(a)
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
    optSetup2=NULL;
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
    setUpdatesEnabled(false);
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
    setUpdatesEnabled(true);
    if (optSetup) optSetup->setUpdatesEnabled(true);
    if (optSetup2) optSetup2->setUpdatesEnabled(true);
    if (widExperimentDescription) widExperimentDescription->setUpdatesEnabled(true);
    if (widAcquisitionDescription) widAcquisitionDescription->setUpdatesEnabled(true);
    if (widScriptedAcquisition) widScriptedAcquisition->setUpdatesEnabled(true);
    if (widImageStack) widImageStack->setUpdatesEnabled(true);
    if (widAcquisition) widAcquisition->setUpdatesEnabled(true);
    if (widCamParamScan) widCamParamScan->setUpdatesEnabled(true);
    if (widDeviceParamScan) widDeviceParamScan->setUpdatesEnabled(true);
    if (widConfig) widConfig->setUpdatesEnabled(true);
}

void QFESPIMB040MainWindow2::storeSettings(ProgramOptions* settings) {
    jksaveWidgetGeometry((*settings->getQSettings()), this, "plugin_spim_b040/");
    jksaveSplitter((*settings->getQSettings()), splitter, "plugin_spim_b040/");
    if (optSetup) optSetup->storeSettings((*settings->getQSettings()), "plugin_spim_b040/instrument/");

    if (optSetup2) {
        QString optSetupFile=ProgramOptions::getConfigValue("plugin_spim_b040/config/optsetup_filename", m_pluginServices->getAssetsDirectory()+"plugins/spimb040/spim_at_b040.optSetup").toString();

        if (QFile::exists(optSetup2->getLastOptSetup()) && QFileInfo(optSetupFile)==QFileInfo(optSetup2->getLastOptSetup())) {

            QString optSetupGlobalConfigFile=ProgramOptions::getConfigValue("plugin_spim_b040/config/optsetup_config_filename_readonly", m_pluginServices->getGlobalConfigFileDirectory()+"/spim_at_b040.optSetup.ini").toString();
            QString optSetupUserConfigFile=ProgramOptions::getConfigValue("plugin_spim_b040/config/optsetup_config_filename", m_pluginServices->getConfigFileDirectory()+"plugins/spimb040/spim_at_b040.optSetup.ini").toString();
            QStringList optSetupFiles;
            QDir().mkpath(QFileInfo(optSetupUserConfigFile).absolutePath());
            QDir().mkpath(QFileInfo(optSetupGlobalConfigFile).absolutePath());
            optSetupFiles<<optSetupGlobalConfigFile;
            optSetupFiles<<optSetupUserConfigFile;
            optSetup2->storeSettings(optSetupFiles, "", true);
        }
    }


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
    QFPluginServices::getInstance()->log_global_text("\n\n=========================================================\n");
    QFPluginServices::getInstance()->log_global_text("== CLOSING SPIM CONTROL PLUGIN!                        ==\n");
    QFPluginServices::getInstance()->log_global_text("=========================================================\n\n\n");
    QFPluginServices::getInstance()->setProjectMode(true);
    QWidget::closeEvent(event);
    //deleteLater();
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
    optSetup->setAcquisitionTools(this);
    tabMain->addTab(optSetup, tr("Instrument Setup"));
    topl->insertWidget(0, optSetup->takeLightpathWidget());

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // create tab for experiment description input
    ////////////////////////////////////////////////////////////////////////////////////////////////
    widExperimentDescription=new QFESPIMB040ExperimentDescription(this);
    tabMain->addTab(widExperimentDescription, tr("Experiment Description"));



    widAcquisitionDescription->setOtherSettingWidgets(optSetup, widExperimentDescription);


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
        widImageStack=new QFESPIMB040ImageStackConfigWidget2(this, this, this, m_pluginServices, optSetup, widAcquisitionDescription, widExperimentDescription, m_pluginServices->getConfigFileDirectory());
        //tabAcquisition->addTab(widImageStack, tr("Image S&tack"));
        tabMain->addTab(widImageStack, tr("Acquisition: Image S&tack"));
        connect(widImageStack, SIGNAL(doStack()), this, SLOT(doImageStack()));
        connect(optSetup, SIGNAL(lightpathesChanged(QFESPIMB040OpticsSetupItems)), widImageStack, SLOT(lightpathesChanged(QFESPIMB040OpticsSetupItems)));

        //------------------------------------------------------------------------------------------
        // create tab for cam parameter image series acquisition
        //------------------------------------------------------------------------------------------
        widCamParamScan=new QFESPIMB040CamParamStackConfigWidget2(this, this, this, m_pluginServices, optSetup, widAcquisitionDescription, widExperimentDescription, m_pluginServices->getConfigFileDirectory());
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

        ////////////////////////////////////////////////////////////////////////////////////////////////
        // optics setup tab
        ////////////////////////////////////////////////////////////////////////////////////////////////
        optSetup2=new QFESPIMB040OpticsSetup2(this, this, this, m_pluginServices);
        optSetup2->setAcquisitionTools(this);
        QString optSetupFile=ProgramOptions::getConfigValue("plugin_spim_b040/config/optsetup_filename", m_pluginServices->getAssetsDirectory()+"plugins/spimb040/spim_at_b040.optSetup").toString();
        QString optSetupGlobalConfigFile=ProgramOptions::getConfigValue("plugin_spim_b040/config/optsetup_config_filename_readonly", m_pluginServices->getAssetsDirectory()+"plugins/spimb040/spim_at_b040.optSetup.ini").toString();
        QString optSetupUserConfigFile=ProgramOptions::getConfigValue("plugin_spim_b040/config/optsetup_config_filename", m_pluginServices->getConfigFileDirectory()+"plugins/spimb040/spim_at_b040.optSetup.ini").toString();
        optSetup2->loadOptSetup(optSetupFile);
        //optSetup2->loadSettings(ProgramOptions::getConfigValue("spimb040/optsetup_config_filename", m_pluginServices->getAssetsDirectory()+"plugins/spimb040/spim_at_b040.optSetup.ini").toString());
        QStringList optSetupFiles;
        QDir().mkpath(QFileInfo(optSetupUserConfigFile).absolutePath());
        QDir().mkpath(QFileInfo(optSetupGlobalConfigFile).absolutePath());
        optSetupFiles<<optSetupGlobalConfigFile;
        optSetupFiles<<optSetupUserConfigFile;
        optSetup2->loadSettings(optSetupFiles, "", true);

        tabMain->addTab(optSetup2, tr("TESTING: Instrument Setup 2"));

        optSetup->emitLighpathesChanged();
}

#define IMAGESTACK_ERROR(message) \
    log_error(QString("  - ")+(message)+QString("\n")); \
    QMessageBox::critical(this, tr("B040SPIM: Image Stack Acquisition Error"), (message));


void QFESPIMB040MainWindow2::doImageStack() {
    widImageStack->performStack();

}


#define CAMPARAMSTACK_ERROR(message) \
    log_error(QString("  - ")+(message)+QString("\n")); \
    QMessageBox::critical(this, tr("B040SPIM: Parameter Stack Acquisition Error"), (message));


void QFESPIMB040MainWindow2::doCamParamStack() {
    widCamParamScan->performStack();

}

void QFESPIMB040MainWindow2::doDeviceParameterStack()
{
    widDeviceParamScan->performStack();
}








bool QFESPIMB040MainWindow2::savePreview(QFExtension* extension, QFExtensionCamera* ecamera, int camera, const QString& previewSettingsFilename, const QString& filename, QString* filename32, QMap<QString, QVariant>* acquisitionDescription, const QString& acquisitionDescriptionPrefix, bool mainShutterOpenOnlyForAcquisition) {
    //////////////////////////////////////////////////////////////////////////////////////
    // INIT variables
    //////////////////////////////////////////////////////////////////////////////////////
    bool ok=true;
    QString TIFFFIlename=filename;
    QString TIFFFIlename32=filename.left(filename.size()-(QFileInfo(filename).suffix().size()+1))+".32."+QFileInfo(filename).suffix();
    TIFF* tiff=NULL;

    if (filename32) *filename32="";


    bool oldShutterState=false;
    if (!optSetup->isMainIlluminationShutterAvailable()) oldShutterState=optSetup->getMainIlluminationShutter();
    //////////////////////////////////////////////////////////////////////////////////////
    // Close Main shutter
    //////////////////////////////////////////////////////////////////////////////////////
    if (mainShutterOpenOnlyForAcquisition && oldShutterState && optSetup->isMainIlluminationShutterAvailable()) {
        optSetup->setMainIlluminationShutter(false, true);
    }


    //////////////////////////////////////////////////////////////////////////////////////
    // PREPARE CAMERA
    //////////////////////////////////////////////////////////////////////////////////////
    int width=0, height=0;
    uint32_t* buffer=NULL;
    if (ok) {
        QSettings settings(previewSettingsFilename, QSettings::IniFormat);
        ecamera->useCameraSettings(camera, settings);
        width=ecamera->getCameraImageWidth(camera);
        height=ecamera->getCameraImageHeight(camera);
        buffer=(uint32_t*)calloc(width*height, sizeof(uint32_t));
        if (!buffer) {
            ok=false;
        }
    }


    //////////////////////////////////////////////////////////////////////////////////////
    // Open Main shutter
    //////////////////////////////////////////////////////////////////////////////////////
    if (mainShutterOpenOnlyForAcquisition && optSetup->isMainIlluminationShutterAvailable()) {
        optSetup->setMainIlluminationShutter(true, true);
    }

    //////////////////////////////////////////////////////////////////////////////////////
    // OPEN OUTPUT TIFF FILES
    //////////////////////////////////////////////////////////////////////////////////////
    if (ok) {
        QMap<QString, QVariant> acqD;
        QTime time=QTime::currentTime();
        if (ecamera->acquireOnCamera(camera, buffer, NULL, &acqD)) {
            //////////////////////////////////////////////////////////////////////////////////////
            // Close Main shutter
            //////////////////////////////////////////////////////////////////////////////////////
            if (mainShutterOpenOnlyForAcquisition && optSetup->isMainIlluminationShutterAvailable()) {
                optSetup->setMainIlluminationShutter(false, true);
            }
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

    //////////////////////////////////////////////////////////////////////////////////////
    // Reset Main shutter
    //////////////////////////////////////////////////////////////////////////////////////
    if (mainShutterOpenOnlyForAcquisition && optSetup->isMainIlluminationShutterAvailable()) {
        optSetup->setMainIlluminationShutter(oldShutterState, true);
    }


    return ok;
}






#define ACQUISITION_ERROR(message) \
    log_error(QString("  - ")+(message)+QString("\n")); \
    QMessageBox::critical(this, tr("B040SPIM: Image Stack Acquisition Error"), (message));

void QFESPIMB040MainWindow2::doAcquisition() {
    widAcquisition->performAcquisition();


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
    settings.setValue("acquisition/image_height", ecamera->getCameraImageHeight(camera));
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


QString QFESPIMB040MainWindow2::saveMeasuredData(const QString& filenamePrefix, const QList<QFESPIMB040OpticsSetupBase::measuredValues>& data) {
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
    height=ecamera->getCameraImageHeight(camera);
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

        SPIMACQUISITION_ERROR(tr("no stage %1 selected").arg(stage->getStageName(stageAxis)), acquisitionTitle);
        optSetup->unlockStages();
        optSetup->unlockLightpath();
        return false;
    }
    if (ok && (!stage->isConnected(stageAxis))) {
        log_text(tr("  - connecting to stage %3: '%1', axis %2!\n").arg(stage->getStageName(stageAxis)).arg(stageAxis).arg(stageNum));
        stage->connectDevice(stageAxis);
    }
    if (ok && (!stage->isConnected(stageAxis)))  {
        ok=false;
        SPIMACQUISITION_ERROR(tr("error connecting to stage %3: '%1', axis %2").arg(stage->getStageName(stageAxis)).arg(stageAxis).arg(stageNum), acquisitionTitle);
    }
    if (ok) {
        stage->setJoystickActive(stageAxis, false);
        stageInitialPos=stage->getPosition(stageAxis);
    }
    return ok;
}

bool QFESPIMB040MainWindow2::acquireImageWithLightpath(const QString& lightpathFilename, const QString& lightpathName, QFExtension* extension1, QFExtensionCamera* ecamera1, int camera1, const QString& previewSettingsFilename1, const QString& outputFilename, const QString& imageID, const QString& imageDescription, QList<QFExtensionCamera::CameraAcquititonFileDescription>& moreFiles1, QMap<QString, QVariant>& acquisitionDescription1, bool mainShutterOpenOnlyForAcquisition) {


    bool oldShutterState=false;
    if (!optSetup->isMainIlluminationShutterAvailable()) oldShutterState=optSetup->getMainIlluminationShutter();
    //////////////////////////////////////////////////////////////////////////////////////
    // Close Main shutter
    //////////////////////////////////////////////////////////////////////////////////////
    if (mainShutterOpenOnlyForAcquisition && oldShutterState && optSetup->isMainIlluminationShutterAvailable()) {
        optSetup->setMainIlluminationShutter(false, true);
    }

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
    bool ok=savePreview(extension1, ecamera1, camera1, previewSettingsFilename1, outputFilename, &filename32, &acquisitionDescription1, imageID+"/", mainShutterOpenOnlyForAcquisition);
    if (ok) {
        log_text(tr("  - acquired %1!\n").arg(imageDescription));
        acquisitionDescription1[imageID+"/image_width"]=ecamera1->getCameraImageWidth(camera1);
        acquisitionDescription1[imageID+"/image_height"]=ecamera1->getCameraImageHeight(camera1);
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

    //////////////////////////////////////////////////////////////////////////////////////
    // Reset Main shutter
    //////////////////////////////////////////////////////////////////////////////////////
    if (mainShutterOpenOnlyForAcquisition  && optSetup->isMainIlluminationShutterAvailable()) {
        optSetup->setMainIlluminationShutter(oldShutterState, true);
    }

    return ok;
}

bool QFESPIMB040MainWindow2::acquireSeries(const QString& lightpathName, const QString& imageID, const QString& imageDescription, bool useCam1, QFExtension* extension1, QFExtensionCamera* ecamera1, int camera1, const QString& acquisitionPrefix1, const QString& acquisitionSettingsFilename1, QMap<QString, QVariant>& acquisitionDescription1, QList<QFExtensionCamera::CameraAcquititonFileDescription>& moreFiles1, bool useCam2, QFExtension* extension2, QFExtensionCamera* ecamera2, int camera2, const QString& acquisitionPrefix2, const QString& acquisitionSettingsFilename2, QMap<QString, QVariant>& acquisitionDescription2, QList<QFExtensionCamera::CameraAcquititonFileDescription>& moreFiles2, const QMap<QFExtensionCamera::CameraSetting, QVariant>& camsettings1, const QMap<QFExtensionCamera::CameraSetting, QVariant>& camsettings2, QList<QFESPIMB040OpticsSetupBase::measuredValues>* measured, QProgressListDialog* progress, bool* userCanceled,bool  measureDuringAcquisitions )
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




    bool result=acquireSeries(lightpathName, imageID, imageDescription, useCam1, extension1, ecamera1, camera1, acquisitionPrefix1, settings1, acquisitionDescription1, moreFiles1, useCam2, extension2, ecamera2, camera2, acquisitionPrefix2, settings2, acquisitionDescription2, moreFiles2, measured, progress, userCanceled, measureDuringAcquisitions);


    if (QFile::exists(tmpName1)) QFile::remove(tmpName1);
    if (QFile::exists(tmpName2)) QFile::remove(tmpName2);

    return result;

}

bool QFESPIMB040MainWindow2::acquireSeries(const QString &lightpathName, const QString &imageID, const QString &imageDescription, bool useCam1, QFExtension *extension1, QFExtensionCamera *ecamera1, int camera1, const QString &acquisitionPrefix1, QSettings &acquisitionSettings1, QMap<QString, QVariant> &acquisitionDescription1, QList<QFExtensionCamera::CameraAcquititonFileDescription> &moreFiles1, bool useCam2, QFExtension *extension2, QFExtensionCamera *ecamera2, int camera2, const QString &acquisitionPrefix2, QSettings &acquisitionSettings2, QMap<QString, QVariant> &acquisitionDescription2, QList<QFExtensionCamera::CameraAcquititonFileDescription> &moreFiles2, QList<QFESPIMB040OpticsSetupBase::measuredValues> *measured, QProgressListDialog *progress, bool *userCanceled, bool measureDuringAcquisitions)
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
             if (measureDuringAcquisitions && measured && time1.elapsed()>200) {
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
    acquisitionDescription[prefix+"/image_height"]=ecamera->getCameraImageHeight(camera);
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




