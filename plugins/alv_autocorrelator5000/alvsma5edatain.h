#ifndef ALVSMA5EDATAIN_H
#define ALVSMA5EDATAIN_H

#include <QObject>
#include <QSharedMemory>
#include <qftools.h>
#include <qfpluginservices.h>
#include <QTime>
#include <QDebug>
//#include <QtGui>
#include <QApplication>
#include <cmath>

/*
  This data segment is used to set flags, variables and instructions for the
  ALV autocorrelator card. The data are then interpreted by the ALV Software.

  */


class AlvSmA5EDataIn : public QSharedMemory
{
    Q_OBJECT
public:
    explicit AlvSmA5EDataIn(QObject *parent = 0);
    ~AlvSmA5EDataIn();

    /////////////////////////////////////////////////
    // Obligatory methods to be called after construction:
    /////////////////////////////////////////////////
    /*
      Creates a shared memory block and sets pointers to all
      variables defined in this interface by the ALV Software;
    */

public:

    bool connectToAlv();

    void setLogging(QFPluginLogService* logservicepointer);

    //////////// Declaration of Enum Types to control the interface functions
    enum AutoScaling {
        Manual,
        Normal,
        Conservative,
        Secure
    };

    enum RunState {
        Start,
        Stop,
        ParameterTransferOnly
    };

    /*
      ///////////////////////////////////////
      // Public programming interface
      ///////////////////////////////////////////////
    */
    // Methods to write "instruction" variables into the memory segment
public:
    void TriggerALVAction();
    void setDuration(int32_t newDuration);
    void setNumberOfRuns(int32_t runNumber);
    void setAutoScaleMode(AlvSmA5EDataIn::AutoScaling scalingmode);
    void setScaleLevelCh0(int32_t scaleLevel);
    void setScaleLevelCh1(int32_t scaleLevel);
    //void setA5EMode(AlvSmA5EDataIn::SingleOrDualMode flag1, AlvSmA5EDataIn::AutoOrCrossMode flag2, AlvSmA5EDataIn::In0OrIn1Mode flag3);
    void setDualMode(bool dual);
    void setCrossMode(bool cross);
    void setIn1Mode(bool in1);


    void setFastMode(bool setFastMode);
    void setScatteringAngle(float angle);
    void setWavelength(float length);
    void setRIOfSolvent(float RI);
    void setViscosity(float visc);
    void setTemperature(float temp);
    void setRunstate(RunState runstate);

    int32_t getNumberOfRuns();
    int32_t getDuration();
    int32_t getScaleLevelCh0();
    int32_t getScaleLevelCh1();
    bool getDualMode();
    bool getCrossMode();
    bool getIn1Mode();
    int32_t getFastMode();
    float getScatteringAngle();
    float getWavelength();
    float getRIOfSolvent();
    float getViscosity();
    float getTemperature();

    // DEBUG METHODS TO READ BACK THE WRITTEN VALUES
    int32_t getTriggerFlag();
    //int32_t getA5EMode();


    int32_t getRunState();

private:

    //////////////////////////////////////////////////////
    ////   Properties of the Qshared Memory
    /////////////////////////////////////////////////////

    int memorysize;
    QFPluginLogService *logService;
    /////////////////////////////////////////////////////
    ///// content variables of the QSharedMemory block //
    /////////////////////////////////////////////////////


    int32_t *writeAndStartFlag;   // has to be set to -1 to write the
                      // data to the ALV software;
                      //
    int32_t *duration; // duration in seconds
    int32_t *numberOfRuns;
    int32_t *autoScaleMode; // 0=manual, 1=normal, 2=conservative, 3=secure
    int32_t *scaleLevelCh0; // IF (scalelevel < 11) THEN "timeblock size" = 200 ns * (n^scalelevel); =>
    int32_t *scaleLevelCh1; // scalelevel          blocksize
                             //          0          200 ns
                             //          1          400 ns
                             //          2          800 ns
                             //     ......          ......
                             //          10         204.8 µs
                             //        > 10         scaling switched off
    int32_t *A5EMode; // Contains the information flags of the modes for:
                       // Single mode or Dual Mode
                       // Auto- or Crosscorrelation Mode
                       // In0 or In1 Mode:
    int32_t *fastMode; // switches the fastmode on or off;
    float *scatteringAngle;
    float *waveLength; // in nm
    float *RIOfSolvent;
    float *viscosity; // in cp
    float *temperature; // in K
    int32_t *currentRunState; // Start = 1, Stop = 0, -1 Parameter Transfer Only

///////////

    void log_text(QString message);
    void log_warning(QString message);
    void log_error(QString message);

signals:

public slots:

};

#endif // ALVSMA5EDATAIN_H
