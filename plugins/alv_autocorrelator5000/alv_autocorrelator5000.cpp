#include "alv_autocorrelator5000.h"

#define LOG_PREFIX "[ALV5000]: "

AlvAutocorrelator::AlvAutocorrelator(QObject* parent):
    QObject(parent)
{

// SILENCE CONSTRUCTOR UNTIL THE OTHER HARDWARE STUFF IS FINISHED
    /* interest calculator
    int val = 0;
    for(int i=0; i<10; i++){
        val += 200000;
        val *= 1.04;
    }
    qDebug() << val/100;
*/


    logService = NULL;
    //settingsDialog = NULL;
    sm_data_in = NULL;
    sm_data_out = NULL;
    ValCorrLengths = NULL;
    ValCRLengths = NULL;
    Corr0AcquisitionResults = NULL;
    Corr1AcquisitionResults = NULL;
    FastModeAcquisitionResults = NULL;
    CR0AcquisitionResults = NULL;
    CR1AcquisitionResults = NULL;
    ilastProgress = 0;
    flastProgress = 0.0;

    AcquisitionRunCounter = 0;
    AcquisitionRunning = false;

/*
    ////// TEST WIDGET
    alvactions_dialog = new QWidget();
    alvactions_dialog->resize(350, 150);
    alvactions_dialog->move(801,831);
    alvactions_dialog->setWindowTitle("ALV: Testwindow and Settings");
    alvsettings_layout = new QGridLayout(alvactions_dialog);
    starter = new QPushButton("Start Runs");
    stopper = new QPushButton("Stop Runs");
    test_reader = new QPushButton("Read MetaData!");
    settings_caller = new QPushButton("Settings");
    progress = new QProgressBar(alvactions_dialog);
    progress->setMinimum(0);
    progress->setMaximum(100);
    progress->setTextVisible(true);
    alvsettings_layout->addWidget(starter, 1, 1);
    alvsettings_layout->addWidget(stopper, 1, 2);
    alvsettings_layout->addWidget(settings_caller, 1, 0);
    alvsettings_layout->addWidget(test_reader, 1, 3);
    alvsettings_layout->addWidget(progress, 0, 4);

    QObject::connect(settings_caller, SIGNAL(clicked()), this, SLOT(call_show_settings_dialog()));
    QObject::connect(starter, SIGNAL(clicked()), this, SLOT(call_startAcquistion()));
    QObject::connect(stopper, SIGNAL(clicked()), this, SLOT(call_stopAcquistion()));
    QObject::connect(test_reader, SIGNAL(clicked()), this, SLOT(test_action()));
    crshow0 = new QLineEdit(alvactions_dialog);
    crshow1 = new QLineEdit(alvactions_dialog);
    crshow0->setReadOnly(true);
    crshow1->setReadOnly(true);
    monitor = new QTimer(this);
    alvsettings_layout->addWidget(new QLabel("CR0:"), 0, 0);
    alvsettings_layout->addWidget(new QLabel("CR1:"), 0, 2);
    alvsettings_layout->addWidget(crshow0, 0, 1);
    alvsettings_layout->addWidget(crshow1, 0, 3);


    QObject::connect(monitor, SIGNAL(timeout()), this, SLOT(refresh_monitor()));
    monitor->start(100);
    monitor->setSingleShot(false);
    alvactions_dialog->show();
    // END TESTWIDGET
*/
}

AlvAutocorrelator::~AlvAutocorrelator() {
    disconnectAcquisitionDevice(0);
    prepareResultLists();
    delete ValCorrLengths;
    delete ValCRLengths;
    delete Corr0AcquisitionResults;
    delete Corr1AcquisitionResults;
    delete FastModeAcquisitionResults;
    delete CR0AcquisitionResults;
    delete CR1AcquisitionResults;

    //CRAcquisitionResults = (Corr1AcquisitionResults = (Corr0AcquisitionResults = NULL));
}

void AlvAutocorrelator::deinit() {
    /* add code for cleanup here */
}



void AlvAutocorrelator::projectChanged(QFProject* oldProject, QFProject* project) {
    /* usually cameras do not have to react to a change of the project in QuickFit .. so you don't need to do anything here
       But: possibly you could read config information from the project here
     */
}

void AlvAutocorrelator::initExtension() {
    /* do initializations here but do not yet connect to the camera! */

}

void AlvAutocorrelator::loadSettings(ProgramOptions* settingspo) {
    /* here you could read config information from the quickfit.ini file using settings object */
    if (!settingspo) return;
    if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini


    // ALTERNATIVE: read/write Information to/from plugins/<ID>/<ID>.ini file
    // QSettings settings(services->getConfigFileDirectory()+"/plugins/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void AlvAutocorrelator::storeSettings(ProgramOptions* settingspo) {
    /* here you could write config information to the quickfit.ini file using settings object */
    if (!settingspo) return;
    if (settingspo->getQSettings()==NULL) return;
    //QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini

    // ALTERNATIVE: read/write Information to/from plugins/<ID>/<ID>.ini file
    // QSettings settings(services->getConfigFileDirectory()+"/plugins/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}


unsigned int AlvAutocorrelator::getAcquisitionDeviceCount()
{
    return 1;
}

bool AlvAutocorrelator::isAcquisitionDeviceConnected(unsigned int device)
{
    if (!(sm_data_in && sm_data_out))
        return false;
    if(sm_data_in->isAttached() && sm_data_out->isAttached())
        return true;
    else
        return false;
}

bool AlvAutocorrelator::connectAcquisitionDevice(unsigned int device)
{
    if (!isAcquisitionDeviceConnected(0)){
        bool ok = prepare_shared_memory();
        if (ok) log_text("Communication channel with ALV software established.");
        else log_error("Communication channel with ALV software could not be established");
        //loadSettings();

        AcquisitionTimer = new QTimer(this);
        AcquisitionTimer->setSingleShot(true);
        QObject::connect(AcquisitionTimer, SIGNAL(timeout()), this, SLOT(finishAcquisitionStep()));
        return true;
    } else {
        log_warning("Acquisition Device is already connected.");
        return false;
    }
}

void AlvAutocorrelator::disconnectAcquisitionDevice(unsigned int device)
{
    if(isAcquisitionDeviceConnected(0)){
        sm_data_in->detach();
        sm_data_out->detach();
        delete sm_data_in;
        delete sm_data_out;
        delete AcquisitionTimer;
    } else log_error("Acquisition Device was not connected.");
    sm_data_in = NULL;
    sm_data_out = NULL;
    AcquisitionRunCounter = 0;
}

bool AlvAutocorrelator::prepareAcquisition(unsigned int device, const QSettings &settings, QString filenamePrefix)
{
    // get setting parameters information from the QSettings Object
    int32_t qvduration = (int32_t) settings.value("ALV_AUTOCORRELATOR5000_DURATION").toInt();
    int32_t qvRunNumber = (int32_t)settings.value("ALV_AUTOCORRELATOR5000_RUNNUMBER").toInt();
    uint8_t qvAutoScalingMode = (uint8_t) settings.value("ALV_AUTOCORRELATOR5000_AUTOSCALINGMODE").toInt();
    bool qvScalingOnCh0 = settings.value("ALV_AUTOCORRELATOR5000_SCALINGON0").toBool();
    bool qvScalingOnCh1 = settings.value("ALV_AUTOCORRELATOR5000_SCALINGON1").toBool();
    int32_t qvScalingLevel0 = settings.value("ALV_AUTOCORRELATOR5000_SCALINGLEVEL0").toInt();
    int32_t qvScalingLevel1 = settings.value("ALV_AUTOCORRELATOR5000_SCALINGLEVEL1").toInt();
    bool qvDualMode = settings.value("ALV_AUTOCORRELATOR5000_DUALMODE").toBool();           // true = dual mode, false = single mode
    bool qvCrossMode = settings.value("ALV_AUTOCORRELATOR5000_CROSSMODE").toBool();         // true = crosscorrelation mode, false = autocorrelation mode
    bool qvInput1Mode = settings.value("ALV_AUTOCORRELATOR5000_INPUT1MODE").toBool();       // true = Input1 Mode, fast mode
    bool qvFastMode = settings.value("ALV_AUTOCORRELATOR5000_FASTMODE").toBool();           // true = fastmode on, false = fast mode off
    float qvScattAngle = settings.value("ALV_AUTOCORRELATOR5000_SCATTANGLE").toFloat();
    float qvWaveLength = settings.value("ALV_AUTOCORRELATOR5000_WAVELENGTH").toFloat();
    float qvRI = settings.value("ALV_AUTOCORRELATOR5000_RI").toFloat();
    float qvViscosity = settings.value("ALV_AUTOCORRELATOR5000_VISCOSITY").toFloat();
    float qvTemperature = settings.value("ALV_AUTOCORRELATOR5000_TEMPERATURE").toFloat();

    // write setting parameters to the shared memory interface
    log_text("Set initial parameters for the control of the Alv Correlator software:");
    sm_data_in->setDuration(qvduration);
    sm_data_in->setNumberOfRuns(1); // The Number of runs is regulated by quickfit, therefore, ALV does a single run at once only.
    AcquisitionRuns = qvRunNumber;

    if (qvAutoScalingMode == 0)
        sm_data_in->setAutoScaleMode(AlvSmA5EDataIn::Manual);
    else if (qvAutoScalingMode == 1)
        sm_data_in->setAutoScaleMode(AlvSmA5EDataIn::Normal);
    else if (qvAutoScalingMode == 2)
        sm_data_in->setAutoScaleMode(AlvSmA5EDataIn::Conservative);
    else if (qvAutoScalingMode == 3)
        sm_data_in->setAutoScaleMode(AlvSmA5EDataIn::Secure);

    if (qvScalingOnCh0)
        sm_data_in->setScaleLevelCh0(qvScalingLevel0);
    else
        sm_data_in->setScaleLevelCh0(11);
    if (qvScalingOnCh1)
        sm_data_in->setScaleLevelCh1(qvScalingLevel1);
    else
        sm_data_in->setScaleLevelCh1(11);

    // evaluate A5EMode settings
    sm_data_in->setDualMode(qvDualMode);
    sm_data_in->setCrossMode(qvCrossMode);
    sm_data_in->setIn1Mode(qvInput1Mode);

    sm_data_in->setFastMode(qvFastMode);
    sm_data_in->setScatteringAngle(qvScattAngle);
    sm_data_in->setWavelength(qvWaveLength);
    sm_data_in->setRIOfSolvent(qvRI);
    sm_data_in->setViscosity(qvViscosity);
    sm_data_in->setTemperature(qvTemperature);

    return true;
}

bool AlvAutocorrelator::startAcquisition(unsigned int device)
{
     //prepareAcquisition(0, settings, "testacq_");
//  FOR TESTRUNS ALWAYS 5 ACQUISITIONS////////

    AcquisitionRuns = 5;
    sm_data_in->setNumberOfRuns(1);
////////////////////////////////////////////////
    ilastProgress = 0;
    flastProgress = 0.0;
    AcquisitionRunCounter = 0;
    AcquisitionRunning = true;
    /*

*/
    // Prepare container for the acquisition
    prepareResultLists();


    // start acquisition
    startRun();
    // write to file
    return true;
}

void AlvAutocorrelator::cancelAcquisition(unsigned int device)
{
    if (AcquisitionRunning){
        AcquisitionTimer->stop();
        stopRun();
        ilastProgress = 0;
        flastProgress = 0.0;
        AcquisitionRunning = false;
    }
}

bool AlvAutocorrelator::isAcquisitionRunning(unsigned int device, double *percentageDone)
{
    if(AcquisitionRunning){
        float ovruntime = AcquisitionRuns * sm_data_in->getDuration() * 1000;
        float elruntime = sm_data_in->getDuration() * (AcquisitionRunCounter-1) ;
        if (sm_data_out->getStatus() == 1)
           elruntime += sm_data_out->getRunTime() ;
        else
            elruntime += sm_data_in->getDuration() * 1000;
        float ratio = elruntime / ovruntime;
        if (ratio < 0.0)
            *percentageDone = 0.0;
        else if ((0.0 < ratio) && (ratio < 100.0)){
            if(ratio > flastProgress)
                flastProgress = ratio;
        *percentageDone = flastProgress;
        } else
            *percentageDone = 100.0;
        return true;
    } else {
        *percentageDone = 0.0;
        return false;
    }
}

void AlvAutocorrelator::getAcquisitionDescription(unsigned int device, QList<QFExtensionAcquisition::AcquititonFileDescription> *files, QMap<QString, QVariant> *parameters)
{
    QFExtensionAcquisition::AcquititonFileDescription descr;
    QFileInfo fileinfo = QFileInfo(*outfile);
    descr.description = QString(fileinfo.absoluteFilePath());
    descr.name = QString("ALV5000");
    descr.type = QString("AutoCorrelatorOutputALV5000");
    files->append(descr);

    (*parameters)["ALV_AUTOCORRELATOR5000_DURATION"] = sm_data_in->getDuration();
    (*parameters)["ALV_AUTOCORRELATOR5000_RUNNUMBER"] = AcquisitionRuns;
    /*
      // probably not important
    (*parameters)["ALV_AUTOCORRELATOR5000_AUTOSCALINGMODE"] = sm_data_in->;
    (*parameters)["ALV_AUTOCORRELATOR5000_SCALINGON0"] = sm_data_in->;
    (*parameters)["ALV_AUTOCORRELATOR5000_SCALINGON1"] = sm_data_in->
    (*parameters)["ALV_AUTOCORRELATOR5000_SCALINGLEVEL0"] = sm_data_in
    (*parameters)["ALV_AUTOCORRELATOR5000_SCALINGLEVEL1"] = sm_data_in->
    */
    (*parameters)["ALV_AUTOCORRELATOR5000_DUALMODE"] = sm_data_in->getDualMode();
    (*parameters)["ALV_AUTOCORRELATOR5000_CROSSMODE"] = sm_data_in->getCrossMode();
    (*parameters)["ALV_AUTOCORRELATOR5000_INPUT1MODE"] = sm_data_in->getIn1Mode();
    (*parameters)["ALV_AUTOCORRELATOR5000_FASTMODE"] = sm_data_in->getFastMode();
    (*parameters)["ALV_AUTOCORRELATOR5000_SCATTANGLE"] = sm_data_in->getScatteringAngle();
    (*parameters)["ALV_AUTOCORRELATOR5000_WAVELENGTH"] = sm_data_in->getWavelength();
    (*parameters)["ALV_AUTOCORRELATOR5000_RI"] = sm_data_in->getRIOfSolvent();
    (*parameters)["ALV_AUTOCORRELATOR5000_VISCOSITY"] = sm_data_in->getViscosity();
    (*parameters)["ALV_AUTOCORRELATOR5000_TEMPERATURE"] = sm_data_in->getTemperature();
}

int AlvAutocorrelator::getAcquisitionProgress(unsigned int device)
{
    if (AcquisitionRunning){
        float ovruntime = AcquisitionRuns * (sm_data_in->getDuration()) * 1000;
        float elruntime = sm_data_in->getDuration() * (AcquisitionRunCounter-1) * 1000;
        if (sm_data_out->getStatus() == 1){
            elruntime += sm_data_out->getRunTime();
        }
        float ratio = elruntime / ovruntime * 100;
       // qDebug() << ratio;
        if (ratio < 99.99){
            if (ratio > ilastProgress){
                ilastProgress = (int) ratio;
            }
            return ilastProgress;
        } else {
            return 100;
        }
    } else {
        //qDebug() << "sss";
        return 0;
    }
}

void AlvAutocorrelator::showAcquisitionDeviceSettingsDialog(unsigned int device, QSettings &settings, QWidget *parent)
{
    ALVSettingsDialog * settingsDialog = new ALVSettingsDialog(settings, sm_data_in, parent);
    if (settingsDialog->exec()) {
        settingsDialog->storeSettings(settings);
    }
    delete settingsDialog;
}

bool AlvAutocorrelator::prepare_shared_memory()
{
    bool ok;
    // first memory block (data output from alv)
    if(!sm_data_out){
        sm_data_out = new AlvSmA5EDataOut(this);
        //sm_data_out->setLogging(logService);
        ok = sm_data_out->connectToAlv();
        if(ok){
            log_text("Shared memory block AlvSmA5EDataOut connected to ALV Software");
        }
        else {
            log_error(QString("An Error occured while connecting to the shared memory \'A5E32 Data Out\':").arg(sm_data_out->errorString()));
            log_error(QString("Finish Quickfit, start the ALV Autocorrrelator software first."));
            log_error(QString("Then try to start Quickfit againe while running ALV"));
            return false;
        }
    } else {
       log_warning("An allcocated and attached memory block exists already. No new AlvSmA5EDataOut shared memory block created or attached.");
       ok = false;
    }
    // control input for alv
    if(!sm_data_in){
        sm_data_in = new AlvSmA5EDataIn(this);
        //sm_data_in->setLogging(logService);
        ok = sm_data_in->connectToAlv();
        if(ok)
            log_text("Shared memory block AlvSmA5EDataIn connected to ALV Software");
        else {
            log_error(QString("An Error occured while connecting to the shared memory \'A5E32 Data In\':") + sm_data_in->errorString());
            log_error(QString("Finish Quickfit, start the ALV Autocorrrelator software first"));
            log_error(QString("and try to start Quickfit while running ALV"));
            return false;
        }
    } else {
        log_warning("An allcocated and attached memory block exists already. No new AlvSmA5EDataIn shared memory block created or attached.");
    }
    return ok;
}

void AlvAutocorrelator::call_show_settings_dialog()
{
    showAcquisitionDeviceSettingsDialog(0, *(services->getSettings()));
}
///////////
void AlvAutocorrelator::test_action()
{
    log_text("Status: " + QString::number(sm_data_out->getStatus()));
    log_text("RunNumber: " + QString::number(sm_data_out->getRunNumber()));
    log_text("RunTime: " + QString::number(sm_data_out->getRunTime()));
    log_text("OverFlow0: " + QString::number(sm_data_out->getOverFlow0()));
    log_text("OverFlow1: " + QString::number(sm_data_out->getOverFlow1()));
    log_text("ActCh: " + QString::number(sm_data_out->getActCh()));
    log_text("ActChCRCh: " + QString::number(sm_data_out->getActCRCh()));
    log_text("GetCR0: " + QString::number(sm_data_out->getCR0()));
    log_text("GetCR1: " + QString::number(sm_data_out->getCR1()));
    log_text("GetMeanCR0: " + QString::number(sm_data_out->getMeanCR0()));
    log_text("GetMeanCR1: " + QString::number(sm_data_out->getMeanCR1()));
    log_text("Estimated Run Number: " + QString::number(AcquisitionRunCounter));
    //log_text("GetMeanCR0: " + QString::number(sm_data_out->getMeanCR0()));
}



///////////


void AlvAutocorrelator::call_startAcquistion()
{
    startAcquisition(0);
}

void AlvAutocorrelator::call_stopAcquistion()
{
    cancelAcquisition(0);
}




void AlvAutocorrelator::finishAcquisitionStep()
{
    // first, make sure that the application run has really finished;
    // => restart the timer and recall this 300 ms SLOT later, if
    // the ALV acquistion is still running

    if (sm_data_out->getStatus() == 1){
        AcquisitionTimer->start(300);
        return;
    }

    // Now, sacrifice a bull for Odin;
    // Then, store the measurement values;
    storeRunData();
    // Eventually, continue with next measurement
    // by restarting the counter in startrun, this function
    // will be recalled, until AcquisitionRunCounter is equal to
    // the number of runs specified in settings
    if (AcquisitionRunCounter < AcquisitionRuns){
        startRun();
        return;
    }
    else if (AcquisitionRunCounter == AcquisitionRuns){
        // finish the last run of this acquisition
        // Finally, write all file data into this file;
        log_text("Last Run finished.");
        //QApplication::overrideCursor();
        writeALVOutputFile();
        prepareResultLists(); // release the allocated memory after writing data to the file
        //QApplication::restoreOverrideCursor();
        ilastProgress = 0;
        flastProgress = 0.0;
        AcquisitionRunning = false;
        return;
    } else {
        log_error("Wrong number of runs counted, debugging of the aquisition method required.");
        return;
    }
}

void AlvAutocorrelator::startRun()
{
    AcquisitionRunCounter += 1;
    sm_data_in->setRunstate(AlvSmA5EDataIn::Start);
    sm_data_in->TriggerALVAction();
    AcquisitionTimer->start(((sm_data_in->getDuration()) * 1000) + 500); // 500 ms extra, as ALV is always a bit slower than Qtimer
    log_text(tr("Start Acquisition run %1").arg(AcquisitionRunCounter));
}

void AlvAutocorrelator::stopRun()
{
    sm_data_in->setRunstate(AlvSmA5EDataIn::Stop);
    sm_data_in->TriggerALVAction();
    //delete AcquisitionTimer;
    AcquisitionRunCounter = 0;
    prepareResultLists(); // frees the memory from allocated memory for the canceled measurement
    log_text("Acquisition canceled.");
}

void AlvAutocorrelator::storeRunData()
{
    bool fastmode = sm_data_in->getFastMode();
    bool dualmode = sm_data_in->getDualMode();
    ValCorrLengths->append(sm_data_out->getActCh());
    ValCRLengths->append(sm_data_out->getActCRCh());
    Corr0AcquisitionResults->append(sm_data_out->getCorr0());

    Corr1AcquisitionResults->append(sm_data_out->getCorr1());
    FastModeAcquisitionResults->append(sm_data_out->getFastModeData(fastmode,dualmode));
    CR0AcquisitionResults->append(sm_data_out->getCR0Data(fastmode, dualmode));
    CR1AcquisitionResults->append(sm_data_out->getCR1Data(fastmode, dualmode));
}

void AlvAutocorrelator::writeALVOutputFile()  //// <---- BEGIN LONG function :-)  ////
{

    // USE STANDARD FILENAME FOR TESTING the writing routine
    QString filename = QString("testdataALV.asc");
    //
    outfile = new QFile(filename, this);


    outfile->open(QIODevice::ReadWrite | QIODevice::Truncate);

    /////////////////////////////////////////////////////////////////////////////
    // For writing to the file, QStrings have to be converted the following way:
    //
    // QString str1 = "Test";
    // QByteArray ba = str1.toLocal8Bit();
    // const char *c_str2 = ba.data();
    //               or
    // Qstring("...").toLocal8Bit().data();
    //
    // All lines have to be finished with 0x0013 0x0010, "\r\n" (windows style)
    // The method is designed to reproduce the output of the ALV Software as exactly as possible
    /////////////////////////////////////////////////////////////////////////////

    //qDebug() << "TEST 1";
    outfile->write(QString("ALV-5000/E-WIN Data\r\n").toLocal8Bit().data());
    QDateTime datetime = QDateTime::currentDateTime();
    int day = datetime.date().day();
    int month = datetime.date().month();
    QString monthName = QDate::shortMonthName(month, QDate::StandaloneFormat);
    int year = datetime.date().year();
    int second = datetime.time().second();
    int minute = datetime.time().minute();
    int hour = datetime.time().hour();
    outfile->write(QString(tr("Date :\t\"%1-%2-%3\"\r\n").arg(day).arg(monthName).arg(year)).toLocal8Bit());
    outfile->write(QString(tr("Time :\t\"%1:%2:%3\"\r\n").arg(hour).arg(minute).arg(second)).toLocal8Bit());
    outfile->write(QString("Samplename : \t\"\"\r\n").toLocal8Bit().data());
    for(int i=0; i < 10 ; i++){
        outfile->write(QString(tr("SampMemo(%1) : \t\"\"\r\n").arg(i)).toLocal8Bit().data());
    }
    //qDebug() << "TEST 2";
    QLocale conv(QLocale::English, QLocale::AnyCountry);
    conv.setNumberOptions(QLocale::OmitGroupSeparator);
    outfile->write(QString(tr("Temperature [K] :\t     %1\r\n").arg(conv.toString(sm_data_in->getTemperature(), 'f', 5))).toLocal8Bit());
    outfile->write(QString(tr("Viscosity [cp]  :\t       %1\r\n").arg(conv.toString(sm_data_in->getViscosity(), 'f', 5))).toLocal8Bit());
    outfile->write(QString(tr("Refractive Index:\t      %1\r\n").arg(conv.toString(sm_data_in->getRIOfSolvent(), 'f', 5))).toLocal8Bit());
    outfile->write(QString(tr("Wavelength [nm] :\t     %1\r\n").arg(conv.toString(sm_data_in->getWavelength(), 'f', 5))).toLocal8Bit());
    outfile->write(QString(tr("Angle [\xB0]       :\t      %1\r\n").arg(conv.toString(sm_data_in->getScatteringAngle(), 'f', 5))).toLocal8Bit());
    outfile->write(QString(tr("Duration [s]    :\t        %1\r\n").arg(conv.toString(sm_data_in->getDuration()))).toLocal8Bit());
    outfile->write(QString(tr("Runs            :\t        %1\r\n").arg(conv.toString(AcquisitionRuns))).toLocal8Bit());
    // Mode
    QString mode("Mode            :\t\"");
    //qDebug() << "TEST 3";
    //int32_t intmode = sm_data_in->getA5EMode();
    bool dual = sm_data_in->getDualMode();
    bool cross = sm_data_in->getCrossMode();
    bool In1 = sm_data_in->getIn1Mode();
    if (dual) mode.append("DUAL ");
    else mode.append("SINGLE ");
    if (cross) mode.append("CROSS ");
    else mode.append("AUTO ");
    if(In1) mode.append("CH1\"\r\n");
    else mode.append("CH0\"\r\n");
    //qDebug() << "TEST 4";
    outfile->write(mode.toLocal8Bit().data());
    outfile->write(QString(tr("MeanCR0 [kHz]   :\t        %1\r\n").arg(conv.toString(sm_data_out->getMeanCR0(), 'f', 5))).toLocal8Bit());
    outfile->write(QString(tr("MeanCR1 [kHz]   :\t        %1\r\n\r\n").arg(conv.toString(sm_data_out->getMeanCR1(), 'f', 5))).toLocal8Bit());
    outfile->write(QString("\r\n\"Correlation (Multi, Averaged)\"\r\n").toLocal8Bit());

    //////////////////////////////////////////////////////
    //                                                  //
    //    Write Data Matrix of correlation functions    //
    //                                                  //
    //////////////////////////////////////////////////////

    ///////////////
    // A. Header //
    ///////////////
    outfile->write("\"Lag [ms]\"\t");
    if (dual){
        outfile->write(QString("\"Average(0)\"").toLocal8Bit());
        for (int i=1; i<=AcquisitionRuns; i++)
            outfile->write(QString(tr("\t\"Run  %1\"").arg(i)).toLocal8Bit());
        outfile->write(QString("\t\"Average(1)\"").toLocal8Bit());
    } else
        outfile->write("\"Average\"");
    for (int i=1;  i<=AcquisitionRuns; i++)
        outfile->write(QString(tr("\t\" %1,  0.000\"").arg(i)).toLocal8Bit());
    outfile->write(QString("\t\r\n").toLocal8Bit());

    //qDebug() << "TEST 5";
    ///////////////////////////////////////////////////////
    // B. Recalculate lag times of correlation functions //
    ///////////////////////////////////////////////////////

    QList<float> lagtimes;

    float lasttau = 0.0;
    float deltatau;
    if (dual)
        deltatau = 0.0004;
    else
        deltatau = 0.0002;
    // first 16 lag times
    for (int i = 0; i<16; i++){
        lasttau += deltatau;
        lagtimes.append(lasttau);
    }
    // rest, double delta tau each 8 lagtimes;
    for (int i = 1; i < 40; i++){
        deltatau *= 2;
        for(int j = 0; j <8; j++){
            lasttau += deltatau;
            lagtimes.append(lasttau);
        }
    }
    //qDebug() << "TEST 6";
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // C. Find number of values of the "shortest" correlation function (i.e. with the smallest number of values) //
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //int i = 0;
    int32_t corrlengths_min = ValCorrLengths->first();
    for (int i = 0; i < ValCorrLengths->size(); i++){
        if(ValCorrLengths->at(i)){
            corrlengths_min = ValCorrLengths->at(i);
        }
    }
    //qDebug() << "TEST 7: " << corrlengths_min;
    ///////////////////
    // D. Write Data //
    ///////////////////

    float average;
    if (dual){
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////
        //     Format for Dual measurements
        //
        //       + = entry,  ave = average
        //                                                   |
        //              For Mode In1:                        |            For Mode In0:
        //                                                   |
        //     lag, ave1, runs1 ---->, ave0, runs0 ---->     |      lag, ave0, runs0 ---->, ave1, runs1 ---->
        //   i   +    +    +    +  ...  +    +    +   ...    |    i   +    +    +    + ...   +    +    +   ...
        //   |                                               |    |
        //   |   +    +    +    +  ...  +    +    +   ...    |    |   +    +    +    +  ...  +    +    +   ...
        //   |                                               |    |
        //   |   +    +    +    +  ...  +    +    +   ...    |    |   +    +    +    +  ...  +    +    +   ...
        //   |                                               |    |
        //   |   +    +    +    +  ...  +    +    +   ...    |    |   +    +    +    +  ...  +    +    +   ...
        //   v                                               |    v
        //       .    .    .    .       .    .    .          |        .    .    .    .       .    .    .
        //       :    :    :    :       :    :    :          |        :    :    :    :       :    :    :
        //
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // always skip channel 0!
        for(int i = 1; i < corrlengths_min ; i++){
            // 1. write lag times
            //qDebug() << "TEST 8";
            outfile->write(QString(tr("  %1").arg(conv.toString(lagtimes.at(i), 'E', 5))).toLocal8Bit());
            if (In1){
                // 2. calculate and write average of correlation runs of CH1, i.e. at lag time
              //  qDebug() << "TEST 8.11";
                average = 0.0;
                for(int run = 0; run < Corr1AcquisitionResults->length(); run++)
                    average += Corr1AcquisitionResults->at(run)->at(i);
                average /= Corr1AcquisitionResults->length();
                outfile->write(QString(tr("\t  %1").arg(conv.toString(average, 'E', 5))).toLocal8Bit());
              //  qDebug() << "TEST 8.12";
                // 3. write single correlation values of CH1 at this lag time
                for(int run = 0; run < Corr1AcquisitionResults->length(); run++)
                    outfile->write(QString(tr("\t  %1").arg(conv.toString(Corr1AcquisitionResults->at(run)->at(i), 'E', 5))).toLocal8Bit());
                // 4. calculate and write average of correlation runs of CH0, i.e. at lag time
                average = 0.0;
              //  qDebug() << "TEST 8.13";
                for(int run = 0; run < Corr0AcquisitionResults->length(); run++)
                    average += Corr0AcquisitionResults->at(run)->at(i);
                average /= Corr0AcquisitionResults->length();
                outfile->write(QString(tr("\t  %1").arg(conv.toString(average, 'E', 5))).toLocal8Bit());
                // 5. write single correlation values of CH0 at this lag time
                for(int run = 0; run < Corr0AcquisitionResults->length(); run++)
                    outfile->write(QString(tr("\t  %1").arg(conv.toString(Corr0AcquisitionResults->at(run)->at(i), 'E', 5))).toLocal8Bit());
              //  qDebug() << "TEST 8.14";
            } else {
                // 2. calculate and write average of correlation runs of CH0, i.e. at lag time
                average = 0.0;
                for(int run = 0; run < Corr0AcquisitionResults->length(); run++)
                    average += Corr0AcquisitionResults->at(run)->at(i);
                //qDebug() << "TEST 8.21";
                average /= Corr0AcquisitionResults->length();
                outfile->write(QString(tr("\t  %1").arg(conv.toString(average, 'E', 5))).toLocal8Bit());
                // 3. write single correlation values of CH0 at this lag time
                for(int run = 0; run < Corr0AcquisitionResults->length(); run++)
                    outfile->write(QString(tr("\t  %1").arg(conv.toString(Corr0AcquisitionResults->at(run)->at(i), 'E', 5))).toLocal8Bit());
                // 4. calculate and write average of correlation runs of CH1, i.e. at lag time
                average = 0.0;
                for(int run = 0; run < Corr1AcquisitionResults->length(); run++)
                    average += Corr1AcquisitionResults->at(run)->at(i);
                average /= Corr1AcquisitionResults->length();
                outfile->write(QString(tr("\t  %1").arg(conv.toString(average, 'E', 5))).toLocal8Bit());
                // 5. write single correlation values of CH1 at this lag time
                for(int run = 0; run < Corr1AcquisitionResults->length(); run++)
                    outfile->write(QString(tr("\t  %1").arg(conv.toString(Corr1AcquisitionResults->at(run)->at(i), 'E', 5))).toLocal8Bit());
            }
            // 6. finish line
            outfile->write(QString("\t\r\n").toLocal8Bit());
        }

    } else {
        //////////////////////////////////////////////////////
        //     Format for Single measurements
        //
        //     lag, ave, runs ------>
        //   i   +    +    +    +   ...
        //   |                             + = entry,  ave = average
        //   |   +    +    +    +   ...
        //   |
        //   |   +    +    +    +   ...
        //   |
        //   |   +    +    +    +   ...
        //   v
        //       .    .    .    .
        //       :    :    :    :
        //////////////////////////////////////////////////////

        // always skip channel 0!
        for(int i = 1; i < corrlengths_min; i++){
            // 1. write lag times
            outfile->write(QString(tr("  %1").arg(conv.toString(lagtimes.at(i), 'E', 5))).toLocal8Bit());
            if (In1){
                // 2. calculate and write average of correlation runs of one line, i.e. at lag time
               // qDebug() << "TEST 8.31";
                average = 0.0;
                for(int run = 0; run < Corr1AcquisitionResults->length(); run++)
                    average += Corr1AcquisitionResults->at(run)->at(i);
                average /= Corr1AcquisitionResults->length();
               // qDebug() << "TEST 8.32";
                outfile->write(QString(tr("\t  %1").arg(conv.toString(average, 'E', 5))).toLocal8Bit());
                // 3. write single correlation values at this lag time
                for(int run = 0; run < Corr1AcquisitionResults->length(); run++)
                    outfile->write(QString(tr("\t  %1").arg(conv.toString(Corr1AcquisitionResults->at(run)->at(i), 'E', 5))).toLocal8Bit());
               // qDebug() << "TEST 8.33";
            } else {
                // 2. calculate and write average of correlation runs of one line, i.e. at lag time
                average = 0.0;
               // qDebug() << "i: " << i;
               // qDebug() << "max: " << corrlengths_min;
                for(int run = 0; run < Corr0AcquisitionResults->length(); run++){
                    average += Corr0AcquisitionResults->at(run)->at(i);
                   // qDebug() << "List " << run << " : " << Corr0AcquisitionResults->at(run)->length();
                                ;
                }
                average /= Corr0AcquisitionResults->length();
                outfile->write(QString(tr("\t  %1").arg(conv.toString(average, 'E', 5))).toLocal8Bit());
                // 3. write single correlation values at this lag time
                for(int run = 0; run < Corr0AcquisitionResults->length(); run++)
                    outfile->write(QString(tr("\t  %1").arg(conv.toString(Corr0AcquisitionResults->at(run)->at(i), 'E', 5))).toLocal8Bit());

            }
            // 4. finish line
            outfile->write(QString("\t\r\n").toLocal8Bit());
        }
    }

    /////////////////////////////////////////////
    //                                         //
    //    Write Data Matrix for count rates    //
    //                                         //
    /////////////////////////////////////////////

    ///////////////
    // A. Header //
    ///////////////

    outfile->write(QString("\r\n\"Count Rate\"\r\n").toLocal8Bit());
   // qDebug() << "TEST 9";
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // B. Find number of values of the "shortest" Count Rate Data Set (i.e. with the smallest number of values) //
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int32_t crlengths_min = ValCRLengths->first();
    for (int i; i < ValCRLengths->length(); i++){
        if(ValCRLengths->at(i))
            crlengths_min = ValCorrLengths->at(i);
    }

    ///////////////////////////////////////////////////////////////////////
    // C. Recalculate the point of times, when count rates were measured //
    ///////////////////////////////////////////////////////////////////////

    QList<float> realTimes;
    float timespace = (sm_data_in->getDuration()) / ((float) crlengths_min);
    for (int i = 1; i <= crlengths_min; i++){
        realTimes.append(timespace * (i));
        //log_warning(QString("times: ") + QString::number(realTimes.last(),'f', 5));
    }
   // qDebug() << "TEST 9";
    ///////////////////
    // D. Write Data //
    ///////////////////

    if (dual){
        //////////////////////////////////////////////////////////////////////////
        //     Format for Dual measurements
        //                                               |
        //     + = entry
        //
        //     time, CR01, CR02, ...., CR11, CR12, ....
        //   i   +    +     +    ...     +     +   ...
        //   |
        //   |   +    +     +    ...     +     +   ...   |
        //   |                                           |
        //   |   +    +     +    ...     +     +   ...   |
        //   v   .    .     .            .     .         |
        //       :    :     :            :     :         |
        //
        ////////////////////////////////////////////////////

        for(int i = 0; i < crlengths_min - 1; i++){
            // 1. write lag times
            outfile->write(QString(tr("       %1").arg(conv.toString(realTimes.at(i), 'E', 5))).toLocal8Bit());
            // 2. write values of Count Rate of first channel
                for (int run = 0; run < CR0AcquisitionResults->length(); run++)
                    outfile->write(QString(tr("\t      %1").arg(conv.toString(CR0AcquisitionResults->at(run)->at(i), 'E', 5))).toLocal8Bit());
                // 3. write values of Count Rate of second channel;
                for (int run = 0; run < CR1AcquisitionResults->length(); run++)
                    outfile->write(QString(tr("\t      %1").arg(conv.toString(CR1AcquisitionResults->at(run)->at(i), 'E', 5))).toLocal8Bit());
                // 4. finish line
                outfile->write(QString("\r\n").toLocal8Bit());
        }
    } else {
        /////////////////////////////////////////////////////
        //     Format for Dual measurements
        //
        //     + = entry
        //
        //     time, CR01, CR02, ....
        //   i   +     +     +   ...
        //   |
        //   |   +     +     +   ...
        //   |
        //   |   +     +     +   ...
        //   v   .     .     .
        //       :     :     :
        //
        ////////////////////////////////////////////////////

        for(int i = 0; i < crlengths_min - 1; i++){
            // 1. write lag times
            outfile->write(QString(tr("       %1").arg(conv.toString(realTimes.at(i), 'E', 5))).toLocal8Bit());
            // 2. write values of Count Rate
            for (int run = 0; run < CR0AcquisitionResults->length(); run++)
                outfile->write(QString(tr("\t      %1").arg(conv.toString(CR0AcquisitionResults->at(run)->at(i), 'E', 5))).toLocal8Bit());
            // 3. finish line
            outfile->write(QString("\r\n").toLocal8Bit());
        }
    }
    outfile->close();
    log_text(tr("ALV Correlator data file written:").arg(filename));
}                       //// <---- END LONG function :-)  ////









void AlvAutocorrelator::setLogging(QFPluginLogService* logService) {
    this->logService=logService;
}

// TEST SLOT
void AlvAutocorrelator::refresh_monitor()
{
    crshow0->setText(QString::number(sm_data_out->getCR0(), 'f', 5));
    crshow1->setText(QString::number(sm_data_out->getCR0(), 'f', 5));
    progress->setValue(getAcquisitionProgress(0));
    //monitor->start(100);
}
// TEST SLOT

void AlvAutocorrelator::prepareResultLists()
{
    if(!ValCorrLengths)
        ValCorrLengths = new QList<int32_t>();
    else
        ValCorrLengths->clear();
    if(!ValCRLengths)
        ValCRLengths = new QList<int32_t>();
    else
        ValCRLengths->clear();

    if (!Corr0AcquisitionResults){
        Corr0AcquisitionResults = new QList<QList<float>*>();
    } else {
        while(Corr0AcquisitionResults->size() > 0){
            delete (Corr0AcquisitionResults->last());
            Corr0AcquisitionResults->removeLast();
        }
    }

    if (!Corr1AcquisitionResults){
        Corr1AcquisitionResults = new QList<QList<float>*>();
    } else {
        while(Corr1AcquisitionResults->size() > 0){
            delete (Corr1AcquisitionResults->last());
            Corr1AcquisitionResults->removeLast();
        }
    }
    if (!FastModeAcquisitionResults) {
        FastModeAcquisitionResults = new QList<QList<float>*>();
    } else {
        while(FastModeAcquisitionResults->size() > 0){
            delete (FastModeAcquisitionResults->last());
            FastModeAcquisitionResults->removeLast();
        }
    }
    if (!CR0AcquisitionResults) {
        CR0AcquisitionResults = new QList<QList<float>*>();
    } else {
        while(CR0AcquisitionResults->size() > 0){
            delete (CR0AcquisitionResults->last());
            CR0AcquisitionResults->removeLast();
        }
    }
    if (!CR1AcquisitionResults) {
        CR1AcquisitionResults = new QList<QList<float>*>();
    } else {
        while(CR1AcquisitionResults->size() > 0){
            delete (CR1AcquisitionResults->last());
            CR1AcquisitionResults->removeLast();
        }
    }
}

void AlvAutocorrelator::log_text(QString message) {
    QFPluginLogTools::log_text(LOG_PREFIX + message + "\n");
}

void AlvAutocorrelator::log_warning(QString message) {
    QFPluginLogTools::log_warning(LOG_PREFIX + message + "\n");
}

void AlvAutocorrelator::log_error(QString message) {
    QFPluginLogTools::log_error(LOG_PREFIX + message + "\n");
}
Q_EXPORT_PLUGIN2(alv_autocorrelator5000, AlvAutocorrelator)

