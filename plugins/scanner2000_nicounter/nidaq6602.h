#ifndef NIDAQ6602_H
#define NIDAQ6602_H

#include <QObject>
#include "NIDAQmx.h"
#include "qfpluginservices.h"
#include <QDebug>
#include <QFile>
//#include <QtGui>
#include "stdint.h"

#define DAQmxErrWarChk(instruction) \
{ signed long errorcode = (instruction); \
    char errmsg[4096]; \
    if( errorcode > 0 ) { \
    DAQmxGetErrorString(errorcode, errmsg, 4096); \
    qDebug() <<"DAQmx Warning: " << errmsg<< " in  "<< #instruction; \
    log_warning(QString(errmsg)); \
    } \
    else if( errorcode < 0 ) { \
    DAQmxGetErrorString(errorcode, errmsg, 4096); \
    qDebug() <<"DAQmx ERROR: " << errmsg<< " in  "<< #instruction; \
    log_error(QString(errmsg)); \
    } \
}

#define DAQmxExtErrWarChk(instruction) \
{ signed long errorcode = (instruction); \
    char errmsg[8192]; \
    if( errorcode > 0 ) { \
    DAQmxGetExtendedErrorInfo(errmsg, 8192); \
    qDebug() <<"DAQmx Warning: " << errmsg<< "\nin  "<< #instruction; \
    log_warning(QString(errmsg)); \
    } \
    else if( errorcode < 0 ) { \
    DAQmxGetExtendedErrorInfo(errmsg, 8192); \
    qDebug() <<"DAQmx ERROR: " << errmsg<< "\nin  "<< #instruction; \
    log_error(QString(errmsg)); \
    } \
}

class NIDAQ6602 : public QObject
{
    Q_OBJECT
public:
    explicit NIDAQ6602(QObject *parent = 0);
    ~NIDAQ6602();


    void configureLineMode(uint32_t lineLength, uint32_t newLineMeasTimePerPixel);
    void configureStaticMode(uint32_t pointMeasTime);
    bool getLineMode();  // true  => configured for LineMode | false => configured for StaticMode
    void startMeasInLineMode(bool *ok); // start measurement task; actual measurement will be triggered by
    void startMeasStaticMode(bool *ok);
    QList <QList<float> > getLineData(bool *ok); // first list: data form measTask1, second list from measTask2; rates in photons per ms
    QList<float> getStaticCountRate(bool *ok);   //

protected:


    ///////////////////////
    // Properties
    ///////////////////////

    QSettings *settings;
    QString counter1Device, counter2Device;
    QString counter1TriggerCounter, counter2TriggerCounter;
    QString counter1PhotonsCounter, counter2PhotonsCounter;
    QString counter1TriggerInput, counter2TriggerInput;
    QString counter1PhotonsInput, counter2PhotonsInput;

    uint32_t pointMeasTime; // measurement time for countrates (in ticks per 20 MHz)
    uint32_t lineLength; // number of pixels for a single Line
    uint32_t lineMeasTimePerPixel; // measurement time for each pixel (in ticks per 20 MHz)
    uInt64 samplesPerChannel;
    TaskHandle measTask1, measTask2, triggerTask1, triggerTask2; // ^= void pointer

    int measDataSize;
    uInt32 *measData1;
    uInt32 *measData2;
    QTimer *measReadTimer;

    //////////////////
    // status flags //
    //////////////////
    bool lineMode;
    bool measReadable; // If a task had run, the result is readable from the Device ONCE and the flag is true;
    //                 // If the data have been read, it will be reset to false;
    //                 // This flag is necessary as tasks have to be stopped manually before a new measurement can be done.
    //                 // which will be done, if you restart the asks while this flag is true;


    ////////////////////////
    // internal functions //
    ////////////////////////

    void resetDevice();
    void readDataBuffer();
    void clearDataBuffer();

    //////////////////////
    // additional stuff //
    //////////////////////

    void log_text(QString message);
    void log_warning(QString message);
    void log_error(QString message);


};


#endif // NIDAQ6602_H
