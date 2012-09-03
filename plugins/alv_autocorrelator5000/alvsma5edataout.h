#ifndef ALVSMA5EDATAOUT_H
#define ALVSMA5EDATAOUT_H

#include <QObject>
#include <QSharedMemory>
#include <qftools.h>
#include <qfpluginservices.h>
#include <QTime>
#include <QDebug>
#include <QApplication>





class AlvSmA5EDataOut : public QSharedMemory
{
    Q_OBJECT
public:
    explicit AlvSmA5EDataOut(QObject *parent = 0);
    ~AlvSmA5EDataOut();

    /////////////////////////////////////////////////
    // Obligatory methods to be called after construction:
    /////////////////////////////////////////////////
    /*
      Creates a shared memory block and sets pointers to all
      variables defined in this interface by the ALV Software;
    */
    bool connectToAlv();

    void setLogging(QFPluginLogService* logservicepointer);
    /*
      ///////////////////////////////////////
      // Public programming interface I
      ///////////////////////////////////////////////
    */
    // Methods to read from the shared Memory interface
    int32_t getRunNumber();
    int32_t getStatus();
    int32_t getRunTime();
    int32_t getOverFlow0();
    int32_t getOverFlow1();
    int32_t getActCh();
    int32_t getActCRCh();
    float getCR0();
    float getCR1();
    float getMeanCR0();
    float getMeanCR1();

    /*
      These reading functions create a new QList, containing the values;
      if such a function is called, make sure to free the memory later;
    */
    QList<float> *getCorr0();
    QList<float> *getCorr1();
    QList<float> *getFastModeData(bool fastmode, bool dual); // list has no elements, if fast mode is void
    QList<float> *getCR0Data(bool fastmode, bool dual);
    QList<float> *getCR1Data(bool fastmode, bool dual);
    QList<float> *getHisto(bool fastmode, bool dual);
    /// ???

private:

    //////////////////////////////////////////////////////
    ////   Properties of the Qshared Memory
    /////////////////////////////////////////////////////

    int memorysize;
    QFPluginLogService *logService;

    /////////////////////////////////////////////////////
    ///// content variables of the QSharedMemory block //
    /////////////////////////////////////////////////////

    int32_t *RunNumber; // decodes the number of the data set, every time the ALV-5000/E
                        // copies data to the shared memory block, the number is increased by one
    int32_t *Status;    // decodes ALV-5000/E status
    int32_t *RunTime;   // so far RunTime of Correlator in ms units
    int32_t *OverFlow0; // first STC block which was overflowing, should not be smaller than 11,
                        // if so, the scaling was not set correctly and should be increased
    int32_t *OverFlow1; // as OverFlow0, but for input channel 1
    int32_t *ActCh;     // actual valid number of correlation channels
    int32_t *ActCRCh;   // actual valid channels in the count rate trace
    float *CR0;         // actual input count rate channel 0 [kHz]
    float *CR1;         // actual input count rate channel 1 [kHz]
    float *MeanCR0;     // actual mean count rate channel 0 [kHz]
    float *MeanCR1;     // actual mean count rate channel 1 [kHz]
    float *Corr0;       // correlation function 0, 288 entries, normalized
          //maxsize:288    // contains data in SINGLE IN0 and DUAL mode, valid channels are given in ActCh
    float *Corr1;       // correlation function 1, 288 entries, normalized
          //maxsize:288    // contains data only in DUAL mode or SINGLE IN1 mode
    float *FastModeData;
          //maxsize:39
    float *CR0Data;          // splitted trace, 1 ... ActCRCH are CR0, ActCRCH+1 ... 2 x ActCRCH are CR1
          //maxsize:512    // all in kHz, valid channels are given by ActCrCh
    float *CR1Data;

    float *Histo;       // histogram data of input channel 0, always 640 valid channels
          //maxsize:640

///////////

    void log_text(QString message);
    void log_warning(QString message);
    void log_error(QString message);

signals:

public slots:

};

#endif // ALVSMA5EDATAOUT_H
