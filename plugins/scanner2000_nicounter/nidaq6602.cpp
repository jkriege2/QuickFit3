#include "nidaq6602.h"
#define LOG_PREFIX "[Scanner2000NICounter/NIDAQ6602]: "
#include "qftools.h"

NIDAQ6602::NIDAQ6602(QObject *parent) :
    QObject(parent)
{
    QFPluginServices *services = QFPluginServices::getInstance();
    QString ini = services->getGlobalConfigFileDirectory()+QString("/nicounterforffm.ini");
    if (!QFile::exists(ini)) ini = services->getConfigFileDirectory()+QString("/nicounterforffm.ini");
    if (!QFile::exists(ini)) ini = services->getAssetsDirectory()+QString("/plugins/scanner2000_nicounter/nicounterforffm.ini");
    settings = new QSettings(ini, QSettings::IniFormat, this);

    counter1Device = settings->value("counter1/device", "Dev3").toString();
    counter1TriggerCounter = settings->value("counter1/triggercounter", "Ctr0").toString();
    counter1PhotonsCounter = settings->value("counter1/photonscounter", "Ctr2").toString();
    counter1TriggerInput = settings->value("counter1/triggerinput", "PFI8").toString();
    counter1PhotonsInput = settings->value("counter1/photonsinput", "PFI0").toString();


    counter2Device = settings->value("counter2/device", "Dev3").toString();
    counter2TriggerCounter = settings->value("counter2/triggercounter", "Ctr0").toString();
    counter2PhotonsCounter = settings->value("counter2/photonscounter", "Ctr2").toString();
    counter2TriggerInput = settings->value("counter2/triggerinput", "PFI10").toString();
    counter2PhotonsInput = settings->value("counter2/photonsinput", "PFI12").toString();


    pointMeasTime = (uint32_t) settings->value("measSettings/pointMeasTime", "10").toInt();

    // for testing environment; value will be set directly from configuration widget;
    // The settings object will be added later;
    pointMeasTime = 5;// (uint32_t) settings->value("measSettings/pointMeasTime", "10").toInt();
    lineLength = 5;//(uint32_t) settings->value("measSettings/lineLength", "400").toInt();
    lineMeasTimePerPixel = 10;//(uint32_t) settings->value("measSettings/lineMeasTimePerPixel", "2000").toInt();
    // create new memory blocks
    samplesPerChannel = lineLength;
    measDataSize = lineLength * sizeof(uInt32);
    measData1 = (uInt32*) qfMalloc(measDataSize);
    measData2 = (uInt32*) qfMalloc(measDataSize);
    clearDataBuffer();

    measTask1 = (measTask2 = (triggerTask1 = (triggerTask2 = NULL )));
    lineMode = false;

    //createTestWidget();
}

NIDAQ6602::~NIDAQ6602()
{
    // clear task objects
    DAQmxExtErrWarChk(DAQmxClearTask(measTask1));
    DAQmxExtErrWarChk(DAQmxClearTask(triggerTask1));
    /* add for 6602
    DAQmxExtErrWarChk(DAQmxClearTask(measTask2));
    DAQmxExtErrWarChk(DAQmxClearTask(triggerTask2));
    */
    qfFree(measData1);
    qfFree(measData2);

    // TESTWIDGET
    //delete testwidget;
    ///
}

void NIDAQ6602::startMeasInLineMode(bool *ok)
{
    if(!lineMode){
        log_error("startMeasInLineMode(bool *ok) can only be used if the device is configured for line mode");
        return;
    }

    if (measReadable){
        log_warning("The data measured before will be discarded!");
        DAQmxExtErrWarChk(DAQmxStopTask(measTask1));
        DAQmxExtErrWarChk(DAQmxStopTask(measTask2));
        DAQmxExtErrWarChk(DAQmxStopTask(triggerTask1));
        DAQmxExtErrWarChk(DAQmxStopTask(triggerTask2));
    }
    DAQmxExtErrWarChk(DAQmxStartTask(measTask1));
    DAQmxExtErrWarChk(DAQmxStartTask(measTask2));
    //DAQmxExtErrWarChk(DAQmxStartTask(triggerTask1));
    //DAQmxExtErrWarChk(DAQmxStartTask(triggerTask2));
    measReadable = true;
}

void NIDAQ6602::startMeasStaticMode(bool *ok)
{
    if(lineMode){
        log_error("startMeas(bool *ok) can not be useed if the device is configured for line mode");
        return;
    }
    if (measReadable){
        log_warning("The data measured before will be discarded!");
        DAQmxExtErrWarChk(DAQmxStopTask(measTask1));
        DAQmxExtErrWarChk(DAQmxStopTask(measTask2));
        DAQmxExtErrWarChk(DAQmxStopTask(triggerTask1));
        DAQmxExtErrWarChk(DAQmxStopTask(triggerTask2));
    }
    DAQmxExtErrWarChk(DAQmxStartTask(measTask1));
    DAQmxExtErrWarChk(DAQmxStartTask(measTask2));
    DAQmxExtErrWarChk(DAQmxStartTask(triggerTask1));
    DAQmxExtErrWarChk(DAQmxStartTask(triggerTask2));
}

void NIDAQ6602::clearDataBuffer()
{
    memset(measData1, 0, measDataSize);
    memset(measData2, 0, measDataSize);
}

void NIDAQ6602::configureLineMode(uint32_t lineLength, uint32_t newLineMeasTimePerPixel)
{
    //////////////////////////////////////////////////////////////////////////
    // clear eventually existing tasks, reset the device and prepare memory //
    //////////////////////////////////////////////////////////////////////////
    lineMeasTimePerPixel = newLineMeasTimePerPixel;
    samplesPerChannel = lineLength;
    resetDevice();
    // update variables
    lineLength = (uint32_t) settings->value("measSettings/lineLength", "400").toInt();
    lineMeasTimePerPixel = (uint32_t) settings->value("measSettings/lineMeasTimePerPixel", "100").toInt();
    samplesPerChannel = lineLength;
    measDataSize = samplesPerChannel * sizeof(uint32_t);
    // qfReallocate memory
    measData1 = (uInt32*) qfRealloc(measData1, measDataSize);
    measData2 = (uInt32*) qfRealloc(measData2, measDataSize);
    ////////////////////////////////////////
    // Create and configure trigger Tasks //
    ////////////////////////////////////////

    log_text("Create Trigger Channels and Tasks.");
    // recalculate parameters
    int32 triggerdelay = 0;
    int32 lowPulseLength = lineMeasTimePerPixel / 10; // number of ticks: 20 MHz ^= 0.05 microseconds per tick;
    int32 highPulseLength = 9 * lowPulseLength;

    // create first task
    DAQmxExtErrWarChk(DAQmxCreateTask("triggerTask1", &triggerTask1));
    const char *taskName1 = QString(QString("/") + counter1Device + QString("/") + counter1TriggerCounter.toLower()).toStdString().c_str();
    const char *timeBase1 = QString(QString("/") + counter1Device + QString("/20MHzTimebase")).toStdString().c_str();
    DAQmxExtErrWarChk(DAQmxCreateCOPulseChanTicks(triggerTask1, taskName1, "triggerChan0", timeBase1, DAQmx_Val_Low, triggerdelay, lowPulseLength, highPulseLength));
    DAQmxExtErrWarChk(DAQmxCfgImplicitTiming(triggerTask1, DAQmx_Val_FiniteSamps, samplesPerChannel));
    // start trigger later by scanner sync signal: TEST WITH DAQ6602
    const char *triggerInput1 = QString(QString("/") + counter1Device + QString("/") + counter1TriggerInput).toStdString().c_str();
    DAQmxExtErrWarChk(DAQmxCfgDigEdgeStartTrig(triggerTask1, triggerInput1, DAQmx_Val_Rising));
    // create second task
    DAQmxExtErrWarChk(DAQmxCreateTask("triggerTask2", &triggerTask2));
    const char *taskName2 = QString(QString("/") + counter2Device + QString("/") + counter2TriggerCounter.toLower()).toStdString().c_str();
    const char *timeBase2 = QString(QString("/") + counter2Device + QString("/20MHzTimebase")).toStdString().c_str();
    DAQmxExtErrWarChk(DAQmxCreateCOPulseChanTicks(triggerTask2, taskName2, "triggerChan0", timeBase2, DAQmx_Val_Low, triggerdelay, lowPulseLength, highPulseLength));
    DAQmxExtErrWarChk(DAQmxCfgImplicitTiming(triggerTask2, DAQmx_Val_FiniteSamps, samplesPerChannel));
    // start trigger later by scanner sync signal: TEST WITH DAQ6602
    const char *triggerInput2 = QString(QString("/") + counter2Device + QString("/") + counter1TriggerInput).toStdString().c_str();
    DAQmxExtErrWarChk(DAQmxCfgDigEdgeStartTrig(triggerTask2, triggerInput2, DAQmx_Val_Rising));

    ////////////////////////////////////////////
    // Create and configure measurement Tasks //
    ////////////////////////////////////////////

    log_text("Create Measurement Channels and Tasks.");
    // create first task
    DAQmxExtErrWarChk(DAQmxCreateTask("measTask1", &measTask1));
    const char *measTaskName1 = QString(QString("/") + counter1Device + QString("/") + counter1PhotonsCounter.toLower()).toStdString().c_str();
    DAQmxExtErrWarChk(DAQmxCreateCICountEdgesChan(measTask1, measTaskName1, "measChan0", DAQmx_Val_Rising, 0, DAQmx_Val_CountUp));
    const char *measTaskSamplesClock1 = QString(QString("/") + counter1Device + QString("/") + counter1TriggerCounter + QString("InternalOutput")).toStdString().c_str();
    DAQmxExtErrWarChk(DAQmxCfgSampClkTiming(measTask1, measTaskSamplesClock1, 10e6, DAQmx_Val_Rising, DAQmx_Val_FiniteSamps, samplesPerChannel));
    // create second task
    DAQmxExtErrWarChk(DAQmxCreateTask("measTask1", &measTask1));
    const char *measTaskName2 = QString(QString("/") + counter2Device + QString("/") + counter2PhotonsCounter.toLower()).toStdString().c_str();
    DAQmxExtErrWarChk(DAQmxCreateCICountEdgesChan(measTask1, measTaskName2, "measChan0", DAQmx_Val_Rising, 0, DAQmx_Val_CountUp));
    const char *measTaskSamplesClock2 = QString(QString("/") + counter2Device + QString("/") + counter2TriggerCounter + QString("InternalOutput")).toStdString().c_str();
    DAQmxExtErrWarChk(DAQmxCfgSampClkTiming(measTask2, measTaskSamplesClock2, 10e6, DAQmx_Val_Rising, DAQmx_Val_FiniteSamps, samplesPerChannel));

    ///////////////////
    // Connect Tasks //
    ///////////////////

    log_text("Connect Channels.");
    // first task
    const char *measTaskGate1 = QString(QString("/") + counter1Device + QString("/") + counter1PhotonsCounter + QString("InternalOutput")).toStdString().c_str();
    DAQmxExtErrWarChk(DAQmxConnectTerms(measTaskSamplesClock1, measTaskGate1, DAQmx_Val_DoNotInvertPolarity));
    // second task
    const char *measTaskGate2 = QString(QString("/") + counter2Device + QString("/") + counter2PhotonsCounter + QString("InternalOutput")).toStdString().c_str();
    DAQmxExtErrWarChk(DAQmxConnectTerms(measTaskSamplesClock2, measTaskGate2, DAQmx_Val_DoNotInvertPolarity));

    lineMode = true;
    measReadable = false;
    log_text("Configuration finished.");
}

void NIDAQ6602::configureStaticMode(uint32_t pointMeasTime)
{
    //////////////////////////////////////////////////////////////////////////
    // clear eventually existing tasks, reset the device and prepare memory //
    //////////////////////////////////////////////////////////////////////////

    resetDevice();
    // update variables
   // lineLength = (uint32_t) settings->value("measSettings/lineLength", "400").toInt();
   // lineMeasTimePerPixel = (uint32_t) settings->value("measSettings/lineMeasTimePerPixel", "100").toInt();
    samplesPerChannel = 5;
    measDataSize = samplesPerChannel * sizeof(uint32_t);
    // qfReallocate memory
    measData1 = (uInt32*) qfRealloc(measData1, measDataSize);
    measData2 = (uInt32*) qfRealloc(measData2, measDataSize);
    ////////////////////////////////////////
    // Create and configure trigger Tasks //
    ////////////////////////////////////////

    log_text("Create Trigger Channels and Tasks.");
    // recalculate parameters
    int32 triggerdelay = 0;
    int32 lowPulseLength = pointMeasTime / 50; // number of ticks: 20 MHz ^= 0.05 5s per tick;
    int32 highPulseLength = 9 * lowPulseLength;

    // create first task
    DAQmxExtErrWarChk(DAQmxCreateTask("triggerTask1", &triggerTask1));
    const char *taskName1 = QString(QString("/") + counter1Device + QString("/") + counter1TriggerCounter.toLower()).toStdString().c_str();
    const char *timeBase1 = QString(QString("/") + counter1Device + QString("/20MHzTimebase")).toStdString().c_str();
    DAQmxExtErrWarChk(DAQmxCreateCOPulseChanTicks(triggerTask1, taskName1, "triggerChan0", timeBase1, DAQmx_Val_Low, triggerdelay, lowPulseLength, highPulseLength));
    DAQmxExtErrWarChk(DAQmxCfgImplicitTiming(triggerTask1, DAQmx_Val_FiniteSamps, 5));

    // create second task
    DAQmxExtErrWarChk(DAQmxCreateTask("triggerTask2", &triggerTask2));
    const char *taskName2 = QString(QString("/") + counter2Device + QString("/") + counter2TriggerCounter.toLower()).toStdString().c_str();
    const char *timeBase2 = QString(QString("/") + counter2Device + QString("/20MHzTimebase")).toStdString().c_str();
    DAQmxExtErrWarChk(DAQmxCreateCOPulseChanTicks(triggerTask2, taskName2, "triggerChan0", timeBase2, DAQmx_Val_Low, triggerdelay, lowPulseLength, highPulseLength));
    DAQmxExtErrWarChk(DAQmxCfgImplicitTiming(triggerTask2, DAQmx_Val_FiniteSamps, 5));

    ////////////////////////////////////////////
    // Create and configure measurement Tasks //
    ////////////////////////////////////////////

    log_text("Create Measurement Channels and Tasks.");
    // create first task
    DAQmxExtErrWarChk(DAQmxCreateTask("measTask1", &measTask1));
    const char *measTaskName1 = QString(QString("/") + counter1Device + QString("/") + counter1PhotonsCounter.toLower()).toStdString().c_str();
    DAQmxExtErrWarChk(DAQmxCreateCICountEdgesChan(measTask1, measTaskName1, "measChan0", DAQmx_Val_Rising, 0, DAQmx_Val_CountUp));
    const char *measTaskSamplesClock1 = QString(QString("/") + counter1Device + QString("/") + counter1TriggerCounter + QString("InternalOutput")).toStdString().c_str();
    DAQmxExtErrWarChk(DAQmxCfgSampClkTiming(measTask1, measTaskSamplesClock1, 10e6, DAQmx_Val_Rising, DAQmx_Val_FiniteSamps, 5));
    // create second task
    DAQmxExtErrWarChk(DAQmxCreateTask("measTask1", &measTask1));
    const char *measTaskName2 = QString(QString("/") + counter2Device + QString("/") + counter2PhotonsCounter.toLower()).toStdString().c_str();
    DAQmxExtErrWarChk(DAQmxCreateCICountEdgesChan(measTask1, measTaskName2, "measChan0", DAQmx_Val_Rising, 0, DAQmx_Val_CountUp));
    const char *measTaskSamplesClock2 = QString(QString("/") + counter2Device + QString("/") + counter2TriggerCounter + QString("InternalOutput")).toStdString().c_str();
    DAQmxExtErrWarChk(DAQmxCfgSampClkTiming(measTask2, measTaskSamplesClock2, 10e6, DAQmx_Val_Rising, DAQmx_Val_FiniteSamps, 5));

    ///////////////////
    // Connect Tasks //
    ///////////////////

    log_text("Connect Channels.");
    // first task
    const char *measTaskGate1 = QString(QString("/") + counter1Device + QString("/") + counter1PhotonsCounter + QString("InternalOutput")).toStdString().c_str();
    DAQmxExtErrWarChk(DAQmxConnectTerms(measTaskSamplesClock1, measTaskGate1, DAQmx_Val_DoNotInvertPolarity));
    // second task
    const char *measTaskGate2 = QString(QString("/") + counter2Device + QString("/") + counter2PhotonsCounter + QString("InternalOutput")).toStdString().c_str();
    DAQmxExtErrWarChk(DAQmxConnectTerms(measTaskSamplesClock2, measTaskGate2, DAQmx_Val_DoNotInvertPolarity));

    lineMode = false;
    measReadable = false;

    log_text("Configuration finished.");
}

void NIDAQ6602::resetDevice()
{
    log_text("Reset Device...");
    if (measTask1 || measTask2 || triggerTask1 || triggerTask2){
        DAQmxExtErrWarChk(DAQmxClearTask(measTask1));
        DAQmxExtErrWarChk(DAQmxClearTask(triggerTask2));
        DAQmxExtErrWarChk(DAQmxClearTask(measTask1));
        DAQmxExtErrWarChk(DAQmxClearTask(triggerTask2));
        qfFree(measTask1);
        qfFree(triggerTask1);
        qfFree(measTask2);
        qfFree(triggerTask2);
    }
    DAQmxExtErrWarChk(DAQmxResetDevice(counter1Device.toStdString().c_str()));
    DAQmxExtErrWarChk(DAQmxSelfTestDevice(counter1Device.toStdString().c_str()));

    clearDataBuffer();
}


void NIDAQ6602::log_text(QString message) {
    QFPluginLogTools::log_text(LOG_PREFIX + message + "\n");
    /// debugging
    qDebug() << message;
    /// end debugging
}

void NIDAQ6602::log_warning(QString message) {
    QFPluginLogTools::log_warning(LOG_PREFIX + message + "\n");
}

void NIDAQ6602::log_error(QString message) {
    QFPluginLogTools::log_error(LOG_PREFIX + message + "\n");
}

QList <QList<float> > NIDAQ6602::getLineData(bool *ok)
{
    // check if the function was called in the right context;
    QList<QList <float> > data;
    if(!lineMode){
        log_error("getLineData(bool *ok) can only be executed if the counter card is configured for lineMode!");
        *ok = false;
        return data;
    }
    if(!measReadable){
        log_error("getLineData(bool *ok) can only be executed ONCE after a task has been started!");
        *ok = false;
        return data;
    }
    // check if tasks are finished
    bool32 task1Done = false;
    bool32 task2Done = false;
    DAQmxExtErrWarChk(DAQmxIsTaskDone(measTask1, &task1Done));
    DAQmxExtErrWarChk(DAQmxIsTaskDone(measTask2, &task2Done));
    if(!(task1Done && task2Done)){
        *ok = false;
        log_warning("Task has not finished, data could not be read with getLineData(bool *ok)");
        return data;
    }
    // read from counter buffers
    int32 actSamplesRead1 = 0;
    int32 actSamplesRead2 = 0;
    DAQmxExtErrWarChk(DAQmxReadCounterU32(measTask1, -1, -1, measData1, samplesPerChannel, &actSamplesRead1, NULL));
    DAQmxExtErrWarChk(DAQmxReadCounterU32(measTask2, -1, -1, measData2, samplesPerChannel, &actSamplesRead2, NULL));
    DAQmxExtErrWarChk(DAQmxStopTask(measTask1));
    DAQmxExtErrWarChk(DAQmxStopTask(measTask2));
    DAQmxExtErrWarChk(DAQmxStopTask(triggerTask1));
    DAQmxExtErrWarChk(DAQmxStopTask(triggerTask2));
    // check if all samples could be read
    if (!((actSamplesRead1 == samplesPerChannel) && (actSamplesRead2 == samplesPerChannel))){
        log_error("Not all samples could be read in getLineData(bool *ok)");
        *ok = false;
        measReadable = false;
        return data;
    }
    // make QLists by calculating the differences between the ith and (i+1)th measurement point (=> list size is samplesPerChannel - 1)
    // divide each by the measurement time per Pixel;
    QList<float> list1;
    QList<float> list2;
    data.append(list1);
    data.append(list2);
    float measTimeInms = lineMeasTimePerPixel / 20000;
    for (int i = 1; i < samplesPerChannel; i++){
        (data[0]).append((measData1[i] - measData1[i-1]) / measTimeInms);
        (data[1]).append((measData2[i] - measData2[i-1]) / measTimeInms);
    }
    measReadable = false;
    *ok = true;
    clearDataBuffer();
    return data;
}

QList<float> NIDAQ6602::getStaticCountRate(bool *ok)
{
    QList<float> data;
     // check if the function was called in the right context;
    if(lineMode){
        log_error("getStaticCountRate(bool *ok) can only be executed if the counter card is configured for lineMode!");
        *ok = false;
        return data;
    }
    if(!measReadable){
        log_error("getStaticCountRate(bool *ok) can only be executed ONCE after a task has been started!");
        *ok = false;
        return data;
    }
    // check if tasks are finished
    bool32 task1Done = false;
    bool32 task2Done = false;
    DAQmxExtErrWarChk(DAQmxIsTaskDone(measTask1, &task1Done));
    DAQmxExtErrWarChk(DAQmxIsTaskDone(measTask2, &task2Done));
    if(!(task1Done && task2Done)){
        *ok = false;
        log_warning("Task has not finished, data could not be read with getStaticCountRate(bool *ok)");
        return data;
    }
    // read from counter buffers;
    int32 actSamplesRead1;
    int32 actSamplesRead2;

    DAQmxExtErrWarChk(DAQmxReadCounterU32(measTask1, -1, -1, measData1, samplesPerChannel, &actSamplesRead1, NULL));
    DAQmxExtErrWarChk(DAQmxReadCounterU32(measTask2, -1, -1, measData2, samplesPerChannel, &actSamplesRead2, NULL));
    DAQmxExtErrWarChk(DAQmxStopTask(measTask1));
    DAQmxExtErrWarChk(DAQmxStopTask(measTask2));
    DAQmxExtErrWarChk(DAQmxStopTask(triggerTask1));
    DAQmxExtErrWarChk(DAQmxStopTask(triggerTask2));
    // check if all data read
    if (!((actSamplesRead1 == 5) && (actSamplesRead2 == 5))){
        log_error("Not all samples could be read in getLineData(bool *ok)");
        *ok = false;
        measReadable = false;
        return data;
    }
    data.append((measData1[4] - measData1[0]) / 4);
    data.append((measData2[4] - measData2[0]) / 2);
    measReadable = false;
    *ok = true;
    clearDataBuffer();
    return data;
}

/*
void NIDAQ6602::testReadfunction()
{

    // check if task has finished
    bool32 taskDone = false;
    //log_warning("Instruc1");
    QTime t;
    DAQmxExtErrWarChk(DAQmxIsTaskDone(triggerTask0, &taskDone));
    if(taskDone){
        log_text("Trigger Task finished");
    } else {
         log_text("Trigger Task not finished");
         log_text("Wait...");
         t.start();
         while(!taskDone){
             while(t.elapsed() < 500){
             }
             t.restart();
             DAQmxExtErrWarChk(DAQmxIsTaskDone(triggerTask0, &taskDone));
             log_text("...");
         }
    }
    int32 actSamplesRead = 0;
    DAQmxExtErrWarChk(DAQmxIsTaskDone(measTask0, &taskDone));
    if (taskDone) log_text("meastask done!");
    else log_text("measTask not Done");
    DAQmxExtErrWarChk(DAQmxReadCounterU32(measTask0, -1, 4.0, measData0, samplesPerChannel, &actSamplesRead, NULL));
    log_text(tr("%1 samples read.").arg((int) actSamplesRead));
    if (actSamplesRead > 0 ){
        for(int i = 0; i<10; i++){
            log_text(tr("Sample %1: %2").arg(i).arg(measData0[i]));
        }
        log_text("Sample x: .......");
    }


}

void NIDAQ6602::callConfigureCard()
{
    configureCard(measnumBox->value());
}

void NIDAQ6602::TaskLoop()
{
    if (!triggerTask0)
        return;
    if (autoStart->isChecked()){
        DAQmxExtErrWarChk(DAQmxStopTask(triggerTask0));
        DAQmxExtErrWarChk(DAQmxStartTask(triggerTask0));
    }
}

void NIDAQ6602::startTasks()
{
    log_text("Start measTask0");
    DAQmxExtErrWarChk(DAQmxStartTask(measTask0));
    log_text("Start triggerTask0");
    DAQmxExtErrWarChk(DAQmxStartTask(triggerTask0));

}

void NIDAQ6602::stopTasks()
{
    log_text("Stop measTask0");
    DAQmxExtErrWarChk(DAQmxStopTask(measTask0));

    log_text("Stop triggerTask0");
    DAQmxExtErrWarChk(DAQmxStopTask(triggerTask0));


    log_text("----------------------------");
}

void NIDAQ6602::startMeasTask()
{
    log_text("Start measTask0");
    DAQmxExtErrWarChk(DAQmxStartTask(measTask0));
}

void NIDAQ6602::stopMeasTask()
{
    log_text("Stop measTask0");
    DAQmxExtErrWarChk(DAQmxStopTask(measTask0));
}

void NIDAQ6602::startTriggerTask()
{
    log_text("Start triggerTask0");
    DAQmxExtErrWarChk(DAQmxStartTask(triggerTask0));

}

void NIDAQ6602::stopTriggerTask()
{
    log_text("Stop triggerTask0");
    DAQmxExtErrWarChk(DAQmxStopTask(triggerTask0));
}
*/

/*
void NIDAQ6602::configureCard(int sampleNumber)
{
    samplesPerChannel = sampleNumber;


    /////////////////////////////////
    // RESET AND SELF-TEST
    ///////////////////////////////
    log_text("Reset Device...");
    DAQmxExtErrWarChk(DAQmxResetDevice("Dev3"));
    DAQmxExtErrWarChk(DAQmxSelfTestDevice("Dev3"));

    /////////////////////////////////////////
    // Trigger Channels (ctr1, ctr3) and task configuration //
    ///////////////////////////////////

    log_text("Create Trigger Channels and Tasks.");
    DAQmxExtErrWarChk(DAQmxCreateTask("triggerTask1", &triggerTask1));
    int32 triggerdelay = 0;
    int32 lowPulseLength = 5000; // number of ticks: 20 MHz ^= 0.05 5s per tick;
    int32 highPulseLength = 15000;
    DAQmxExtErrWarChk(DAQmxCreateCOPulseChanTicks(triggerTask1, "/Dev3/ctr0", "triggerChan0", "/Dev3/20MHzTimebase", DAQmx_Val_Low, triggerdelay, lowPulseLength, highPulseLength));
    DAQmxExtErrWarChk(DAQmxCfgImplicitTiming(triggerTask1, DAQmx_Val_FiniteSamps, samplesPerChannel));

    //// start trigger by scanner sync signal
    // DAQmxExtErrWarChk(DAQmxCfgDigEdgeStartTrig(triggerTask0, "/Dev2/PFI0", DAQmx_Val_Rising));
    //DAQmxExtErrWarChk(DAQmxCfgDigEdgeRefTrig(triggerTask0, "/Dev2/PFI0", DAQmx_Val_Falling, 0));

    ////////////////////////////////////////////////////////////////
    // Measurement Channnels (ctr0, ctr2) and task configurations //
    ////////////////////////////////////////////////////////////////

    log_text("Create Measurement Channels and Tasks.");
    DAQmxExtErrWarChk(DAQmxCreateTask("measTask1", &measTask1));
    DAQmxExtErrWarChk(DAQmxCreateCICountEdgesChan(measTask1, "/Dev3/ctr2", "measChan0", DAQmx_Val_Rising, 0, DAQmx_Val_CountUp));
    DAQmxExtErrWarChk(DAQmxCfgSampClkTiming(measTask1, "/Dev3/Ctr0InternalOutput", 10e6, DAQmx_Val_Rising, DAQmx_Val_FiniteSamps, samplesPerChannel));
    //DAQmxExtErrWarChk(DAQmxCfgChangeDetectionTiming(measTask1, "/Dev2/Ctr0InternalOutput", "", DAQmx_Val_FiniteSamps, samplesPerChannel) );
    //DAQmxExtErrWarChk(DAQmxConnectTerms("/Dev2/PFI8", "/Dev2/Ctr1Source", DAQmx_Val_DoNotInvertPolarity)); //PFI 8 specific for 6023E


    // SPÄTER LESEN MIT: DAQmxReadCounterU32() !!! ... wenn task beendet (DAQmxIsTaskDone()), dann sollten genau samplesPerChannel samples auszulesen sein!
    // PUFFER WERDEN INTERN ALLOZIERT



    //////////////////////
    // Connect Channels //
    //////////////////////

    log_text("Connect Channels.");
    DAQmxExtErrWarChk(DAQmxConnectTerms("/Dev3/Ctr0InternalOutput", "/Dev3/Ctr2Gate", DAQmx_Val_DoNotInvertPolarity));

    //DAQmxExtErrWarChk(DAQmxConnectTerms("/Dev2/Ctr1Gate", "/Dev2/PFI4", DAQmx_Val_DoNotInvertPolarity));
    //DAQmxExtErrWarChk(DAQmxConnectTerms("/Dev2/Ctr0InternalOutput", "/Dev2/RTSI0", DAQmx_Val_DoNotInvertPolarity)); // Subsystem routing specific for 6023E
    //DAQmxExtErrWarChk(DAQmxConnectTerms("/Dev2/RTSI0", "/Dev2/Ctr1Gate", DAQmx_Val_DoNotInvertPolarity)); // Subsystem routing specific for 6023E
    ///////////////////////(/////
    // Allocate reading memory //
    /////////////////////////////
    log_text("qfReallocate reading memory:");
    measDataSize = sampleNumber * sizeof(uInt32);
    measData1 = (uInt32*) qfRealloc(measData1, sampleNumber * sizeof(uInt32));
    measData2 = (uInt32*) qfRealloc(measData2, sampleNumber * sizeof(uInt32));
    clearDataBuffer();

    log_text("Configuration finished.");
}
*/
