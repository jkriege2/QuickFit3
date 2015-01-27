#include "alvsma5edatain.h"

#define LOG_PREFIX "[ALV5000/alvsmA5EDataIn]: "
AlvSmA5EDataIn::AlvSmA5EDataIn(QObject *parent)
{
    setNativeKey(QString("A5E32 Data In"));
    memorysize = 128;
}

AlvSmA5EDataIn::~AlvSmA5EDataIn()
{
}

bool AlvSmA5EDataIn::connectToAlv()
{
    bool ok;
    ok = attach(ReadWrite);
    if(!ok){
        log_error(tr("Error while attaching AlvSmA5EDataIn to the shared memory segment: ").arg(this->errorString()));
        return false;
    }
    // Set all declared content variable pointers to
    // their corresponding places of the qshared memory data field;
    // Use funny pointer arithemtics :-)
    writeAndStartFlag = (int32_t*) this->data();
    duration = writeAndStartFlag + 1;
    numberOfRuns = duration + 1;
    autoScaleMode = numberOfRuns + 1;
    scaleLevelCh0 = autoScaleMode + 1;
    scaleLevelCh1 = scaleLevelCh0 + 1;
    A5EMode = scaleLevelCh1 + 1;
    fastMode = A5EMode + 1;
    scatteringAngle = (float*) (fastMode + 1);
    waveLength = scatteringAngle +1;
    RIOfSolvent = waveLength + 1;
    viscosity = RIOfSolvent + 1;
    temperature = viscosity + 1;
    currentRunState = (int32_t*) (temperature + 1);


    return ok;
}

void AlvSmA5EDataIn::setLogging(QFPluginLogService* logService) {
    this->logService=logService;
}

void AlvSmA5EDataIn::TriggerALVAction()
{
    *writeAndStartFlag = -1;
// wait until writing has finished
    while(*writeAndStartFlag == -1){
        //QApplication::processEvents();
    }
}

void AlvSmA5EDataIn::setDuration(int32_t newDuration)
{
    if (newDuration > 0){
        *duration = newDuration;
        log_text(tr("Duration set to %1").arg(QLocale::system().toString(*duration)));

    } else
        log_error(tr("In AlvSmA5EDataIn::setDuration(...) : Parameter out of range: newDuration == %1").arg(newDuration));
}

void AlvSmA5EDataIn::setNumberOfRuns(int32_t runNumber)
{
    if (runNumber > 0){
        *numberOfRuns = runNumber;
    }
    else
        log_error(tr("In AlvSmA5EDataIn::setNumberOfRuns(...) : Parameter out of range: runNumber == ").arg(runNumber));
}

void AlvSmA5EDataIn::setAutoScaleMode(AlvSmA5EDataIn::AutoScaling scalingmode)
{
    switch(scalingmode){
    case Manual:
        *autoScaleMode = 0;
        log_text("AutoScaleMode set to \'Manual\'.");
        break;
    case Normal:
        *autoScaleMode = 1;
        log_text("AutoScaleMode set to \'Normal\'.");
        break;
    case Conservative:
        *autoScaleMode = 2;
        log_text("AutoScaleMode set to \'Conservative\'.");
        break;
    case Secure:
        *autoScaleMode = 3;
        log_text("AutoScaleMode set to \'Secure\'.");
        break;
    }
}

void AlvSmA5EDataIn::setScaleLevelCh0(int32_t scaleLevel)
{
    *scaleLevelCh0 = scaleLevel;
    if (scaleLevel < 11)
        log_text(tr("Scale Level set to: ").arg(scaleLevel));
    else
        log_text(tr("Scaling switched off"));
//    QFPluginLogService::log_text(tr("ScaleLevel of Ch0 set to %1").arg(scaleLevel));
}

void AlvSmA5EDataIn::setScaleLevelCh1(int32_t scaleLevel)
{
    *scaleLevelCh1 = scaleLevel;
    if (scaleLevel < 11)
        log_text(tr("Scale Level set to: ").arg(scaleLevel));
    else
        log_text(tr("Scaling switched off"));
    //    QFPluginLogService::log_text(tr("ScaleLevel of Ch1 set to %1").arg(scaleLevel));
}

void AlvSmA5EDataIn::setDualMode(bool dual)
{
    if (!dual) // toggle singlemode bit on:
        *A5EMode = *A5EMode | 0x00000004;
    else // toggle singlemode bit off
        *A5EMode = *A5EMode & (~(0x00000004));
}

void AlvSmA5EDataIn::setCrossMode(bool cross)
{
    if (cross) // toggle crossmode bit on:
        *A5EMode = *A5EMode | 0x00000001;
    else // toggle crossmode bit off
        *A5EMode = *A5EMode & (~0x00000001);
}

void AlvSmA5EDataIn::setIn1Mode(bool in1)
{
    if (in1) // toggle in1mode bit on:
        *A5EMode = *A5EMode | 0x00000002;
    else // toggle in0mode bit off
        *A5EMode = *A5EMode & (~0x00000002);
}

/*
void AlvSmA5EDataIn::setA5EMode(AlvSmA5EDataIn::SingleOrDualMode flag1, AlvSmA5EDataIn::AutoOrCrossMode flag2, AlvSmA5EDataIn::In0OrIn1Mode flag3)
{
    int32_t instruc = 0;
    if (flag1 == Single)
        instruc += 4;     // => 0000...100
    if (flag2 == Cross)
        instruc += 1;     // => 0000...x10
    if (flag3 == In1)
        instruc += 2;     // => 0000...xx1
    *A5EMode = instruc;
    //log messages:
    if (flag1 == Single)
        log_text("Configuration switched to single acquisition (?) mode.");
    else
        log_text("Configuration switched to dual acquisition (?) mode.");
    if (flag2 == Cross)
        log_text("Configuration switched to auto correlation mode.");
    else
        log_text("Configuration switched to cross correlation mode.");
    if (flag3 == In1)
        log_text("Configuration switched to Input 0 mode.");
    else
        log_text("Configuration switched to Input 1 mode.");
}
*/

void AlvSmA5EDataIn::setFastMode(bool setOn)
{
    if (setOn){
        *fastMode = 0x00000001;
        log_text("Fast mode switched on.");
    } else {
        *fastMode = 0x00000000;
        log_text("Fast mode switched off.");
    }
}

void AlvSmA5EDataIn::setScatteringAngle(float angle)
{
    *scatteringAngle = angle;
    log_text(tr("Scattering angle set to %1").arg(angle));
}

void AlvSmA5EDataIn::setWavelength(float length)
{
    *waveLength = length;
    log_text(tr("Wavelength set to %1").arg(length));
}

void AlvSmA5EDataIn::setRIOfSolvent(float RI)
{
    *RIOfSolvent = RI;
    log_text(tr("RI of solvent set to %1").arg(RI));
}

void AlvSmA5EDataIn::setViscosity(float visc)
{
    *viscosity = visc;
    log_text(tr("Viscosity set to %1").arg(visc));
}

void AlvSmA5EDataIn::setTemperature(float temp)
{
    *temperature = temp;
    log_text(tr("Temperature set to %2").arg(temp));
}

void AlvSmA5EDataIn::setRunstate(AlvSmA5EDataIn::RunState runstate)
{
    switch (runstate){
    case Start:
        *currentRunState = 1;
        log_text("Runstate set to \'Start\'");
        break;
    case Stop:
        *currentRunState = 0;
        log_text("Runstate set to \'Stop\'");
        break;
    case ParameterTransferOnly:
        *currentRunState = -1;
        log_text("Runstate set to \'Parameter Transfer Only\'");
        break;
    }
}

int32_t AlvSmA5EDataIn::getNumberOfRuns()
{
    return *numberOfRuns;
}

void AlvSmA5EDataIn::log_text(QString message) {
 //     QFPluginLogService::log_text(LOG_PREFIX + message + "\n");
    //else if (services) services->log_text(LOG_PREFIX + message + "\n");
    QFPluginLogTools::log_text(LOG_PREFIX + message + "\n");
}

void AlvSmA5EDataIn::log_warning(QString message) {
 //     QFPluginLogService::log_warning(LOG_PREFIX + message + "\n");
    //else if (services) services->log_warning(LOG_PREFIX + message + "\n");
    QFPluginLogTools::log_warning(LOG_PREFIX + message + "\n");
}

void AlvSmA5EDataIn::log_error(QString message) {

    //QFPluginLogService::log_error(LOG_PREFIX + message + "\n");
    //else if (services) services->log_error(LOG_PREFIX + message + "\n");
    QFPluginLogTools::log_error(LOG_PREFIX + message + "\n");
}

/////// DEBUG METHODS
int32_t AlvSmA5EDataIn::getDuration()
{
    return *duration;
}

int32_t AlvSmA5EDataIn::getScaleLevelCh0()
{
    return *scaleLevelCh0;
}

int32_t AlvSmA5EDataIn::getScaleLevelCh1()
{
    return *scaleLevelCh1;

}






/*
int32_t AlvSmA5EDataIn::getA5EMode()
{
    return *A5EMode;
}
*/

int32_t AlvSmA5EDataIn::getFastMode()
{
    return *fastMode;
}

bool AlvSmA5EDataIn::getDualMode()
{
    if ((*A5EMode) & 0x00000004)
        return false;
    else
        return true;
}

bool AlvSmA5EDataIn::getCrossMode()
{
    if ((*A5EMode) & 0x00000001)
        return true;
    else
        return false;
}

bool AlvSmA5EDataIn::getIn1Mode()
{
    if ((*A5EMode) & 0x00000002)
        return true;
    else
        return false;
}

float AlvSmA5EDataIn::getScatteringAngle()
{
    return *scatteringAngle;
}

float AlvSmA5EDataIn::getWavelength()
{
    return *waveLength;
}

float AlvSmA5EDataIn::getRIOfSolvent()
{
    return *RIOfSolvent;
}

float AlvSmA5EDataIn::getViscosity()
{
    return *viscosity;
}

float AlvSmA5EDataIn::getTemperature()
{
    return *temperature;
}

int32_t AlvSmA5EDataIn::getRunState()
{
    return *currentRunState;
}


