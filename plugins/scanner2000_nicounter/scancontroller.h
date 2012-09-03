#ifndef SCANCONTROLLER_H
#define SCANCONTROLLER_H

#include <QObject>
#include "jkserialconnection.h"
#include <stdint.h>
#include <QtGui>
#include <QGridLayout>
#include <QSpinBox>
#include <string>
#include <cstring>
#include <cmath>
#include <limits>
#include <algorithm>
#include <qtlogfile.h>
#include "qfpluginservices.h"

#define TEST_SERIALCONNECTION_BOOL \
{ if(!SerConn->isConnectionOpen()){ \
    log_error("No connection to Scanner has been established."); \
    return false; \
    } \
}
#define TEST_SERIALCONNECTION_VOID \
{ \
if(!SerConn->isConnectionOpen()){ \
    log_error("No connection to Scanner has been established."); \
    return; \
    } \
}
/*
*   interface class for the control of the sc2000 thing; connected to COM1 at the moment...
*/

class ScanController : public QObject
{
    Q_OBJECT
public:
    explicit ScanController(QObject *parent = 0);
    ~ScanController();



    // test
    QWidget *get_testwidget(); //test





    /*
      |--------------------------------|
      | public programming interface   |
      |--------------------------------|
     */
public slots:
    /*
      calibrate() sets the readback_gain_correction factors to 1 and the offset correction factor to 0;
      Then, the scanner drives to xxx positions and measures the corresponding positions.
      The correction factors are calculated as the regression coefficients g and o of the regression line
               regx(posx) = gx * posx + ox    and    regy(posy) = gy * posy + oy
      with meas(pos) as the measured position posx;
      gx, ox, gy, oy are finally sent to the scanner;



    initScanner initializes the Scanner and calls the calibrate()-function



    If the waittime of a move instruction is not specified,
    the scanner will move with a standardized speed and waittime = abs(newxpos-oldxpos) / 230
     */

    // TEST SLOT METHOD
    //void move(); // TEST

public:
    void MoveToX(int16_t newxpos);
    void MoveToX(int16_t newxpos, float moveTime);
    void MoveToY(int16_t newypos);
    void MoveToY(int16_t newypos, float moveTime);
    void MoveToXY(int16_t newxpos, int16_t newypos);
    void MoveToXY(int16_t newxpos, int16_t newypos, float moveTime);

    /*
      Moves the scanner to a and sets a "sync" signal before moving.
    */
    bool TriggerMove(uint8_t triggerChannel, int16_t newxpos, int16_t newypos, float moveTime);
    void setLog(QtLogFile* logfile);
    bool calibrate();
    bool connectToScanner();
    bool initScanner();
    bool disconnect();

private:
    /* Properties
    ---------------
      */
    JKSerialConnection *SerConn;
  //  int16_t xpos;
    // int16_t ypos;
    //float x_readback_gain_correction;
    //float y_readback_gain_correction;
    //float x_readback_offset_correction;
    //float y_readback_offset_correction;
    //uint32_t baudrate;





    QtLogFile *log;


    /*
      |-----------------------------------|
      | internal utitlity functions       |
      |-----------------------------------|
      */

    /*
      sets readback gain correction factors of both axes to 1.0;
      sets readback gain correction factors of both axes to 0.0;
     */
    bool reset_readback_factors();

    /*
      sets readback gain & offset correction factors of both axes
      */
    bool set_readback_factors(double gain_x, double offset_x, double gain_y, double offset_y);

     /*
       calculates regression coefficients for a given set of measurement values:
       Yvals and Xvals dont't relate to scanner positions, but to the variable names
       known from statistics.
       popsize is just the length of the arrays Yvals and Xvals.
       output:
       QList<double> at [0] = slope;
       QList<double> at [1] = offset;
     */
    QList<double> calc_linear_regression(int16_t *Yvals, int16_t *Xvals, uint8_t popsize);

    /*
      |-----------------------------------|
      | hidden hardware control functions |
      |-----------------------------------|
     */

    /*  PERHAPS STILL TO IMPLEMENT
       Eventually add an instruction accumulator and executor function (?)
       which merges a set of instructions to a little program and sends them together
       'well-packed' to the serial connection
     */

    /*0x29

      returns: Boot Segment Division              at QString[0];
               Firmware Revision (default 1.0)    at QString[1];
               Hardware Default (default0)        at QString[2];
               Device ID (default 0)              at Qstring[3];
    */
    QList<QString> getHardwareInformation();

    /*0x1A
      enables Vector Mode of the scanner;
    */
    bool enableVectorMode();

    /*0x14
      enables one or both of the SAXes
    */
    bool enableSAXes(bool first, bool second);

    /*0x06
      Move focus to a position during a certain time, given in milliseconds;
      Time Maximum ~ 1507 milliseconds
    */
    bool h_moveToXY(int16_t xpos, int16_t ypos, float time);

    /*0x30
      Set configuration variables:
      var specifies the variable to be set
      var-------Variable name------------------Value space---------Default value
      1         GlobalSampleSize               1 to 100            10
      2         X readback gain correction     0.5 to 1.5          -
      3         X readback offset correction   -32768 to 32767     -
      4         Y readback gain correction     0.5 to 1.5          -
      5         Y readback offset correction   -32768 to 32767     -
      6         SetSync delay                  0 to 32767          -
      7         UnSetSync delay                0 to 32767          -
    */
    bool setConfigVar(uint8_t var, double value);

    /* 0x12        0x13
       setSync and unSetSync
       The channel_mask specifies which one of the writable sync
       channels is to be set.
       channel------------Description
       1                  Sync Channel 1 (J4, pin 1 pulls low)
       2                  Sync Channel 2 (J4, pin 2 pulls low)
       3                  Sync Channel 3 (J4, pin 3 pulls low)
       4                  Sync Channel 4 (J4, pin 4 pulls low)
       13                 turns INTCNTL on
       14                 drives PCLKOUT output with the clock on
                          CMOS_PCLK input pin
      */
    bool setSync(uint8_t channel_mask);
    bool unSetSync(uint8_t channel_mask);

    /*
      waits for the given time in milliseconds
    */
    bool wait(float time);

    /*
      get X Postion of scanner
    */
    int16_t get_Xpos();

    /*
      get Y Postion of scanner
    */
    int16_t get_Ypos();
    /*
    The get_Status command returns the error information described in the Error Processing section of this document. When an
    error is discovered while a stored program is executing or while commands are being received over the communication
    link, all subsequent commands are ignored until a get_Status command is issued or the controller is power cycled. If the
    error occurs while a program is running, the program will continue to run until the ?status command is issued. If the
    SC2000 is in the error state (LED ON) ?status will return the error code of the command in error, halt any running
    programs, set the LED to OFF and unlock command processing. If there is no error, get_status returns the success
    code of the last command executed.

    QList.at(0) =                 Error Source
    QList.at(1) =                 Command in error
    QList.at(2) =                 System Error

    If 999 is the return value in all three elements, the method was not succesful
  */
    QList<uint16_t> get_Status();

    /* Configure the Scanner for parameters concerning the RS232 interface;

     baudrte_code:
     code              baudrate
     1                 2,400
     2                 4,800
     3                 9,600
     4                19,200
     5                38,400
     6                57,600
     7                115,200
     stopbits:
     code              stopbits
     1                 1 stopbit
     2                 2 stopbit
     parity:
     code              parity
     0                 no parity
     1                 odd parity
     2                 even parity

     Defaults: 2400 baud, 8 data_bits, 1 stopbit, no parity, 232
               (code 1)    (fixed)      (code 1)  (code 1)  (fixed)
     */
    bool scConnConfig(uint8_t baudrate_code, uint8_t stopbits, uint8_t parity);

    ///////////
    // additional stuff
    ////////
    void log_text(QString message);
    void log_warning(QString message);
    void log_error(QString message);






};

#endif // SCANCONTROLLER_H
