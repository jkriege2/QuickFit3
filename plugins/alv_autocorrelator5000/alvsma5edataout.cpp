#include "alvsma5edataout.h"
#define LOG_PREFIX "[ALV5000/smA5EDataOut]: "
AlvSmA5EDataOut::AlvSmA5EDataOut(QObject *parent)
    // : QObject(parent)
{
    setNativeKey(QString("A5E32 Data Out"));
    memorysize = 8192;
}

AlvSmA5EDataOut::~AlvSmA5EDataOut()
{
    detach();
    //setNativeKey(QString());
}

bool AlvSmA5EDataOut::connectToAlv()
{
    bool ok;
    ok = attach(ReadOnly);
    if(!ok){
        log_error(tr("Error while attaching AlvSmA5EDataOut to the shared memory segment: ").arg(this->errorString()));
                return false;
    }
    // Set all declared content variable pointers to
    // their corresponding places of the qshared memory data field;
    // Use funny pointer arithemtics :-)
    RunNumber = (int32_t*) (this->data());
    Status = RunNumber + 1;
    RunTime = Status + 1;
    OverFlow0 = RunTime + 1;
    OverFlow1 = OverFlow0 + 1;
    ActCh = OverFlow1 + 1;
    ActCRCh = ActCh + 1;
    CR0 = (float*) (ActCRCh + 1);
    CR1 = CR0 + 1;
    MeanCR0 = CR1 + 1;
    MeanCR1 = MeanCR0 + 1;
    Corr0 = MeanCR1 + 1;
    /* other pointers have to be allocated dynamically before reading,
       as their blocks seem to have a dynamic size
    Corr1 = Corr0 + 288;
    CR = Corr1 + 288;
    Histo = CR + 512;
    */
    return ok;
}

void AlvSmA5EDataOut::setLogging(QFPluginLogService* logService) {
    this->logService=logService;
}

int32_t AlvSmA5EDataOut::getRunNumber()
{
    return *RunNumber;
}

int32_t AlvSmA5EDataOut::getStatus()
{
    return *Status;
}

int32_t AlvSmA5EDataOut::getRunTime()
{
    return *RunTime;
}

int32_t AlvSmA5EDataOut::getOverFlow0()
{
    return *OverFlow0;
}

int32_t AlvSmA5EDataOut::getOverFlow1()
{
    return *OverFlow1;
}

int32_t AlvSmA5EDataOut::getActCh()
{
    return *ActCh;
}

int32_t AlvSmA5EDataOut::getActCRCh()
{
    return *ActCRCh;
}

float AlvSmA5EDataOut::getCR0()
{
    return *CR0;
}

float AlvSmA5EDataOut::getCR1()
{

    return *CR1;
}

float AlvSmA5EDataOut::getMeanCR0()
{
    return *MeanCR0;
}

float AlvSmA5EDataOut::getMeanCR1()
{
    return *MeanCR1;
}

QList<float> *AlvSmA5EDataOut::getCorr0()
{
    int maxlength = getActCh();
    QList<float> *result = new QList<float>();
    for(int i=0; i<maxlength; i++){
        result->append(Corr0[i]);
    }
    return result;
}

QList<float> *AlvSmA5EDataOut::getCorr1()
{
    int maxlength = getActCh();
    Corr1 = Corr0 + maxlength;
    QList<float> *result = new QList<float>();
    for(int i=0; i<maxlength; i++){
        result->append(Corr1[i]);
    }
    return result;
}

QList<float> *AlvSmA5EDataOut::getFastModeData(bool fastmode, bool dual)
{
    QList<float> *result = new QList<float>();
    if(fastmode){
        FastModeData = Corr0 + (2 * getActCh());
        // According to the ALV example software:
        //
        // if(!dual)
        //    FastModeData--;
        //
        // decrementation is probably a bug in the alv example software => skip it here

        for(int i=0; i<39; i++)
            result->append(FastModeData[i]);
    }
    return result;
}

QList<float> *AlvSmA5EDataOut::getCR0Data(bool fastmode, bool dual)
{
    QList<float> *result = new QList<float>();
    CR0Data = Corr0 + (2 * getActCh()); // skip Corr function(s)
    // According to the ALV example software:
    //
    // if(!dual)
    //    CR0Data--;
    //
    // decrementation is probably a bug in the alv example software => skip it here

    if (fastmode) // if fastmode on, skip 39 floats
        CR0Data += 39;
    CR0Data++;
    int maxlength = getActCRCh();
    for(int i=0; i<maxlength; i++){
        result->append(CR0Data[i]);
    }
    return result;

}
QList<float> *AlvSmA5EDataOut::getCR1Data(bool fastmode, bool dual)
{
    QList<float> *result = new QList<float>();
    CR1Data = Corr0 + (2 * getActCh()); // skip Corr function(s)
    // According to the ALV example software:
    //
    // if(!dual)
    //    CR1Data--;
    //
    // decrementation is probably a bug in the alv example software => skip it here

    if (fastmode) // if fastmode on, skip 39 floats
        CR1Data += 39;
    CR1Data++;
    int maxlength = getActCRCh(); // maxlength of CR0Data and CR1Data
    CR1Data += maxlength;
    for(int i=0; i<maxlength; i++){
        result->append(CR1[i]);
    }
    return result;
}

QList<float> *AlvSmA5EDataOut::getHisto(bool fastmode, bool dual)
{


    QList<float> *result = new QList<float>();

    Histo = Corr0 + (2 * getActCh()); // skip Corr function(s)
    // According to the ALV example software:
    //
    // if(!dual)
    //    Histo--;
    //
    // decrementation is probably a bug in the alv example software => skip it here

    if (fastmode) // if fastmode on, skip 39 floats
        Histo += 39;
    Histo++;
    //int maxlength = getActCRCh(); // maxlength of CR0Data and CR1Data
    Histo += (2 * getActCRCh());
    for(int i=0; i<640; i++){
        result->append(Histo[i]);
    }
    return result;
}

void AlvSmA5EDataOut::log_text(QString message) {
    QFPluginLogTools::log_text(LOG_PREFIX + message + "\n");
    //else if (services) services->log_text(LOG_PREFIX + message + "\n");
}

void AlvSmA5EDataOut::log_warning(QString message) {
    QFPluginLogTools::log_warning(LOG_PREFIX + message + "\n");
    //else if (services) services->log_warning(LOG_PREFIX + message + "\n");
}

void AlvSmA5EDataOut::log_error(QString message) {
    QFPluginLogTools::log_error(LOG_PREFIX + message + "\n");
    //else if (services) services->log_error(LOG_PREFIX + message + "\n");
}

