#include "scancontroller.h"

#define LOG_PREFIX "[Scanner2000NICounter/scancontroller]: "

ScanController::ScanController(QObject *parent) :
    QObject(parent)
{
    SerConn = NULL;
    // control stuff
}

ScanController::~ScanController()
{
    delete SerConn;
}

int16_t ScanController::get_Xpos()
{
    if(!SerConn->isConnectionOpen()){
        log_error("No connection to Scanner has been established.");
        return 0;
    }
    QTime t;
    uint8_t instruc[3];
    uint8_t feedback[2];
    memset(instruc, 0, 3);
    memset(feedback, 0, 2);
    instruc[0] = 0x2A;
    instruc[2] = 0x01;
    SerConn->write(instruc, 3);
    t.start();
    while (t.elapsed() < 10){
    }
    SerConn->read(feedback, 2);
    int16_t result = (256* ((int16_t) feedback[0]) + ((int16_t) feedback[1]));
    log_text(tr("TEST: XPOS = %1").arg(result));
    return result;
}

int16_t ScanController::get_Ypos()
{
    if(!SerConn->isConnectionOpen()){
        log_error("No connection to Scanner has been established.");
        return 0;
    }
    QTime t;
    uint8_t instruc[3];
    uint8_t feedback[2];
    memset(instruc, 0, 3);
    instruc[0] = 0x2A;
    instruc[2] = 0x02;
    SerConn->write(instruc, 3);
    t.start();
    while (t.elapsed() < 10){
    }
    SerConn->read(feedback, 2);
    int16_t result = (256 * ((int16_t) feedback[0]) + ((int16_t) feedback[1]));
    log_text(tr("TEST: YPOS = %1").arg(result));
    return result;
}

QList<uint16_t> ScanController::get_Status()
{
    bool ok;
    QList<uint16_t> result;
    // initialize the result first
    for (int i=0; i<3; i++)
        result.append(999);
    QTime t;
    // check Connection
    if (!SerConn->isConnectionOpen()){
        log_error("No connection to Scanner has been established.");
        return result;
    }
    // prepare instruction
    uint8_t instruc[9];
    memset(instruc, 0xFF, 9);
    // send instruction
    ok = SerConn->write(instruc, 9);
    if (!ok){
        log_error(tr("Error in ScanController::get_Status(): After Sending the command 0xFFFFFFFFFFFFFFF:"));
        log_error(SerConn->getLastError().c_str());
        return result;
    }
    t.start();
    while(t.elapsed() < 50){
    }
    // get and process readback values
    uint8_t feedback[6];
    memset(feedback, 0, 6);
    ok = SerConn->read(feedback, 6);
    if (!ok){
        log_error("ERROR: Reading from the Serial connection failed in ScanController::get_Status():");
        return result;
    }
    result[0] = feedback[0] * 256 + feedback[1];
    result[1] = feedback[2] * 256 + feedback[3];
    result[2] = feedback[4] * 256 + feedback[5];
    return result;
}

bool ScanController::scConnConfig(uint8_t baudrate_code, uint8_t stopbits, uint8_t parity)
{
    TEST_SERIALCONNECTION_BOOL;
    bool ok;
    QTime t;
    uint8_t instruc[11];
    //check parameter ranges:
    if((baudrate_code < 1) || (baudrate_code > 7)){
        log_error("Parameter baudrate_code in ScanController::scConnConfig(uint8_t baudrate_code, uint8_t stopbits, uint8_t parity) \n is out of range ");
        return false;
    }
    if((stopbits < 1) || (stopbits > 2)){
        log_error("Parameter stopbits in ScanController::scConnConfig(uint8_t baudrate_code, uint8_t stopbits, uint8_t parity) \n is out of range ");
        return false;
    }
    if(parity > 2){
        log_error("Parameter parity in ScanController::scConnConfig(uint8_t baudrate_code, uint8_t stopbits, uint8_t parity) \n is out of range ");
        return false;
    }
    //build instruction:
    memset(instruc, 0x00, 11);
    instruc[0] = 0x23;          // command
    instruc[2] = baudrate_code; // big endian baudrate (second byte)
    instruc[4] = 0x08;          // big endian databits (second byte, fixed value)
    instruc[6] = stopbits;      // big endian stopbits (second byte)
    instruc[8] = parity;        // big endian parity (second byte)
    instruc[10] = 0xE8;         // big endian interface (second byte, 232)
    //send instruction:
    ok = SerConn->write(instruc, 11);
    // log messages:
    if (!ok){
        log_error(tr("Error in after sending the instruction:\n %1").arg(SerConn->getLastError().c_str()));
        return false;
    } else {
        log_text("*************************************************\nScanner connection parameters modified:\n");
        unsigned int baudrate_value=0;
        if (baudrate_code == 1)
            baudrate_value = 2400;
        else if (baudrate_code == 2)
            baudrate_value = 4800;
        else if (baudrate_code == 3)
            baudrate_value = 9600;
        else if (baudrate_code == 4)
            baudrate_value = 19200;
        else if (baudrate_code == 5)
            baudrate_value = 38400;
        else if (baudrate_code == 6)
            baudrate_value = 57600;
        else if (baudrate_code == 7)
            baudrate_value = 115200;
        log_text(tr("****** baudrate set to %1").arg(baudrate_value));
        if (stopbits == 1)
            log_text("****** 1 Stopbit used for transmission");
        else if (stopbits == 2)
            log_text("****** 2 Stopbits used for transmission");
        if(parity == 0)
            log_text("****** No parity used\n*************************************************");
        else if(parity == 1)
            log_text("****** Odd parity used\n*************************************************");
        else if (parity == 2){
            log_text("****** Even parity used\n*************************************************");
        }
        return true;
    }
}

bool ScanController::calibrate()
{
    /*
      Waiting times are crucial in this method as the scanner does not execute comands,
      if they're sent while another scanner program is still running;
    */
    uint8_t meas_side = 10; // measurements on ONE side of the axis=>
    uint8_t meas_total = meas_side * 2 + 1; // Total amount of measurements
    int16_t upper_meas_limit = 16000;
    int16_t lower_meas_limit = -16000;
    int16_t meas_range = upper_meas_limit - lower_meas_limit;
    int16_t meas_dist = (meas_range / (meas_total - 1));
    bool ok;
    QTime t;
    log_text("-------------------------------\nStart Calibration.....\n");
    QApplication::overrideCursor();
    // move to origin;
    double waittime = 0.005 * meas_range;
    ok =  h_moveToXY(0, 0, waittime);
    waittime *= 1.1;
    if (ok){
        log_text(tr("Moving scanner to X: %1, Y: %2").arg(0).arg(0));
        t.start();
        while(t.elapsed() < waittime){
        }
    } else {
        log_error(tr("Error: Scanner could not be moved!!\nError Messgae: %1").arg(tr(SerConn->getLastError().c_str())));;
        QApplication::restoreOverrideCursor();
        return false;
    }
    // set all factors back to 0, 1 respectively;
    ok = reset_readback_factors();
    if(!ok){
        QApplication::restoreOverrideCursor();
        return false;
    }
    // drive to the positions and get measurement values:
    int16_t newpos;
    int16_t x_meas[meas_total];
    int16_t x_driven[meas_total];
    int16_t y_meas[meas_total];
    int16_t y_driven[meas_total];
    QList<double> xcoeffs;
    QList<double> ycoeffs;
    //// PERHAPS several iterations
    //int iterations = 4;
    ////
    //for( int iter = 0; iter < iterations; iter ++){
        for(uint8_t i = 0; i < meas_total; i++){
            newpos = lower_meas_limit + (i * meas_dist);
            MoveToXY(newpos, newpos);
            t.restart();
            waittime = 0.005 * meas_dist;
            while(t.elapsed() < waittime){
            }
            wait(5.0);
            t.restart();
            while(t.elapsed() < 8){
            }
            y_driven[i] = (x_driven[i] = newpos);
            x_meas[i] = get_Xpos();
            y_meas[i] = get_Ypos();
    //  }
        // calculate readback factors as linear regression coefficients:
        xcoeffs = calc_linear_regression(x_meas, x_driven, meas_total);
        ycoeffs = calc_linear_regression(y_meas, y_driven, meas_total);
        //set_readback_factors();
        ok = set_readback_factors(xcoeffs.at(0), xcoeffs.at(1), ycoeffs.at(0), ycoeffs.at(1));
        //////// PERHAPS
    }
    /////////
    log_text("\nCalibration finished!\n--------------------------------------");
    QApplication::restoreOverrideCursor();
    return ok;
}
// creates an JKSerialConnection instance and (hopefully) connects to the Scanner;
bool ScanController::connectToScanner()
{
    bool ok;
    if (!SerConn){
        SerConn = new JKSerialConnection(std::string("COM1"), 2400, JKSC8databits, JKSCnoParity, JKSConeStopbit, JKSChardwareHandshakingRTS);
        SerConn->set_logToFile(true);
    }
    //SerConn->set_baudrate(baudrate);
    if(!SerConn->isConnectionOpen()){
        // at the moment
        ok = SerConn->open();
        if (ok){
            log_text("-------------------------------------\nConnection to Scanner established succesfully.\n");

        } else {
            log_text(tr("Connection to Scanner could not be established.\nError Messgae: %1").arg(tr(SerConn->getLastError().c_str())));
        }
    } else {
        log_warning("Already connected to the Scanner. No new connection established!");
        ok = false;
    }
    return ok;
}

//testfunction
/*
QWidget *ScanController::get_testwidget()
{
    return testwidget;
}
*/
bool ScanController::reset_readback_factors()
{
    return set_readback_factors(1.0, 0.0, 1.0, 0.0);
}
//*testfunction

/*0x29
  uint16_t *r;
  returns: Boot Segment Division              at r[0];
           Firmware Revision (default 1.0)    at r[1];
           Hardware Default (default0)        at r[2];
           Device ID (default 0)              at r[3];
*/
QList<QString> ScanController::getHardwareInformation()
{

    QList<QString> result;
    for(int i=0; i<4; i++)
        result.append(QString("NO INFORMATIOM AVAILABLE!"));
    bool ok;
    QTime t;
    uint8_t instruc = 0x29;
    uint8_t feedback[6];
    memset(feedback, 0, 6);
    ok = SerConn->write(&instruc,1);
    if (!ok){
        log_text(tr("<b>ERROR</b> in ScanController::getHardwareInformation() after sending the instruction").arg(SerConn->getLastError().c_str()));
        return result;
    }
    t.start();
    while(t.elapsed() < 10){
    }

    ok = SerConn->read(feedback,6);
    if(ok && !(feedback[0]==0 && feedback[1]==0 && feedback[2]==0 && feedback[3]==0 && feedback[4]==0 && feedback[5]==0)){
        result[0] = (QString::number(feedback[0], 10) + QString(".") + QString::number(feedback[1], 10));
        result[1] = (QString::number(feedback[2], 10) + QString(".") + QString::number(feedback[3], 10));
        result[2] = (QString::number(feedback[4]));
        result[3] = (QString::number(feedback[5]));
    }
    return result;
}

/*0x1A
  enables Vector Mode of the scanner;
*/
bool ScanController::enableVectorMode()
{
    TEST_SERIALCONNECTION_BOOL;
    uint8_t instruc =0x1A;
    bool ok = SerConn->write(&instruc,1);
    log_text(tr("TEST: Vector Mode: ").arg((int) ok));
    return ok;
}

/*
  enables one or both of the SAXes
*/
bool ScanController::enableSAXes(bool first, bool second)
{
    TEST_SERIALCONNECTION_BOOL;
    uint8_t instruc[3];
    uint8_t device_id = 1;
    if (first)
        device_id += 1;
    if (second)
        device_id += 1;
    memset(instruc, 0, 3);
    instruc[0]= 0x14;
    instruc[2] = device_id;
    bool ok = SerConn->write(instruc,1);
    return ok;
}

/*
  Move focus to a position during a certain time, given in milliseconds;
  Time Maximum ~ 1507 milliseconds
*/
bool ScanController::h_moveToXY(int16_t xpos, int16_t ypos, float time)
{
    TEST_SERIALCONNECTION_BOOL;
    uint8_t instruc[7];
    memset(instruc, 0, 7);
    instruc[0] = 0x06;
    // split int_16 xpos and ypos into to uint_8 with equivalent bit patterns;
    instruc[2] = xpos & 0xFF;
    instruc[1] = (xpos>>8)&0xFF;
    instruc[4] = ypos & 0xFF;
    instruc[3] = (ypos>>8) & 0xFF;
    // check if time Maximum is exceeded
    if ((time > 23 * 0xFFFF / 1000)){
        log_text(QString("Given time in h_moveToXY exceeds the maximum value!"));
        return false;
    }
    uint16_t ticks = (uint16_t) time*1000/23; // returns number of ticks of 23 microseconds each;
    instruc[6] = ticks & 0xFF;
    instruc[5] = (ticks - instruc[6]) / 256;
    bool ok = SerConn->write(instruc,7);
    return ok;
}



/*0x30
  Set configuration variables:
  var specifies the variable to be set
  varID-------Variable name------------------Value space---------Default value
  1         GlobalSampleSize               1 to 100            10
  2         X readback gain correction     0.5 to 1.5          -
  3         X readback offset correction   -32768 to 32767     -
  4         Y readback gain correction     0.5 to 1.5          -
  5         Y readback offset correction   -32768 to 32767     -
  6         SetSync delay                  0 to 32767          -
  7         UnSetSync delay                0 to 32767          -
*/
bool ScanController::setConfigVar(uint8_t var, double value)
{
    uint8_t instruc[5];
    memset(instruc, 0, 5);
    instruc[0] = 0x30;
    instruc[2] = var;
    uint16_t intval;

    // TEST_SERIALCONNECTION();
    // Transform the value, operation depends on the variable ID
    switch (var){
    case 1: // Value Space: 1 to 100
        intval = (uint16_t) (value + 0.5);
        if ((intval >= 1) && (intval <= 100)){
            instruc[4] = intval & 0xFF;
            instruc[3] = (intval>>8) & 0xFF;
        } else {
            log_error(QString("Value %1 in setConfigValue for specified variable %2 is out of range!").arg(value).arg(var));
            return false;
        }
        break;
    case 2: // Value Space: 0.5 to 1.5, has to be transformed manually into an integer with intvalue = truncate(32768*floatvalue)
        if((value >= 0.5) && (value <= 1.5)){
            intval = (uint16_t) (0x8000 * value);
            instruc[3] = (intval>>8) & 0xFF;
            instruc[4] = intval & 0xFF;
        } else {
            log_error(QString("Value %1 in setConfigValue for specified variable %2 is out of range!").arg(value).arg(var));
            return false;
        }
        break;
    case 3: // Value Space: -32768 to 32767
        if(((int32_t) value >= -32768) && ((int32_t) value <= 32768)){
            int16_t sintval = (int16_t) value;
            instruc[3] = (sintval>>8) & 0xFF;
            instruc[4] = sintval & 0xFF;;
        } else {
            log_error(QString("Value %1 in setConfigValue for specified variable %2 is out of range!").arg(value).arg(var));
            return false;
        }
        break;
    case 4: // Value Space: 0.5 to 1.5, has to be transformed manually into an integer with intvalue = truncate(32768*floatvalue)
        if((value >= 0.5) && (value <= 1.5)){
            intval = (uint16_t) (0x8000 * value);
            instruc[3] = (intval>>8) & 0xFF;
            instruc[4] = intval & 0xFF;
        } else {
            log_error(QString("Value %1 in setConfigValue for specified variable %2 is out of range!").arg(value).arg(var));
            return false;
        }
        break;
    case 5: // Value Space: -32768 to 32767
        if(((int32_t) value >= -32768) && ((int32_t) value <= 32768)){
            int16_t sintval = (int16_t) value;
            instruc[3] = (sintval>>8) & 0xFF;
            instruc[4] = sintval & 0xFF;;
        } else {
            log_error(QString("Value %1 in setConfigValue for specified variable %2 is out of range!").arg(value).arg(var));
            return false;
        }
        break;
    case 6: // Value Space: 0 to 32767
        if(((uint32_t) value >= 0) && ((uint32_t) value <= 32768)){
            intval = (uint16_t) value;
            instruc[3] = (intval>>8) & 0xFF;
            instruc[4] = intval & 0xFF;;
        } else {
            log_error(QString("Value %1 in setConfigValue for specified variable %2 is out of range!").arg(value).arg(var));
            return false;
        }
        break;
    case 7: // Value Space: 0 to 32767
        if(((uint32_t) value >= 0) && ((uint32_t) value <= 32768)){
            intval = (uint16_t) value;
            instruc[3] = (intval>>8) & 0xFF;
            instruc[4] = intval & 0xFF;;
        } else {
            log_error(QString("Value %1 in setConfigValue for specified variable %2 is out of range!").arg(value).arg(var));
            return false;
        }
        break;
    default: // If an unknowm variable ID is received
        log_error(QString("Variable %1 in setConfigValue does not exist!").arg(var));
        return false;
    }
    // send instructions to SerConn
    bool ok = SerConn->write(instruc, 5);

    return ok;
}

/* 0x12        0x13
   setSync and unSetSync
   channel_mask specifies which one of the writable sync
   channels is to be set.
   channel------------Description
   1                  Sync Channel 1 (J4, pin 1 pulls low)
   2                  Sync Channel 2 (J4, pin 2 pulls low)
   3                  Sync Channel 3 (J4, pin 3 pulls low)
   4                  Sync Channel 4 (J4, pin 4 pulls low)
   13                 turns INTCNTL on                           //Is it 13 or 0x13?
   14                 drives PCLKOUT output with the clock on    //Is it 14 or 0x14?
                      CMOS_PCLK input pin
  */
bool ScanController::setSync(uint8_t channel_mask)
{
    TEST_SERIALCONNECTION_BOOL;
    uint8_t instruc[3];
    memset(instruc, 0, 3);
    instruc[0] = 0x12;
    if ( (channel_mask != 1) &&
         (channel_mask != 2) &&
         (channel_mask != 3) &&
         (channel_mask != 4) &&
         (channel_mask != 13) &&
         (channel_mask != 14) ){
        log_text(QString("channel_mask %1 in setConfigValue does not exist!").arg(channel_mask));
        return false;
    } else {
        instruc[2] = channel_mask;
        bool ok = SerConn->write(instruc, 3);
        return ok;
    }
}

bool ScanController::unSetSync(uint8_t channel_mask)
{
    TEST_SERIALCONNECTION_BOOL;
    uint8_t instruc[3];
    memset(instruc, 0, 3);
    instruc[0] = 0x13;
    if ( (channel_mask != 1) &&
         (channel_mask != 2) &&
         (channel_mask != 3) &&
         (channel_mask != 4) &&
         (channel_mask != 13) &&
         (channel_mask != 14) ){
        log_text(QString("channel_mask %1 in setConfigValue does not exist!").arg(channel_mask));
        return false;
    } else {
        instruc[2] = channel_mask;
        bool ok = SerConn->write(instruc, 3);
        return ok;
    }
}

/*
  Scanner waits for the given time in milliseconds
          Time Maximum ~ 2.722e06 milliseconds
*/
bool ScanController::wait(float time)
{
    TEST_SERIALCONNECTION_BOOL;
    uint8_t instruc[5];
    memset(instruc, 0, 5);
    instruc[0]=0x10;
    // check, if time maximum is exceeded
    if (time > (23 * (0xFFFFFFFF / 1000))){
        log_error(QString("Given time in ScanController::wait(float time) exceeds the maximum value!"));
        return false;
    }
    uint32_t intval = (time * 1000) / 23;
    // transform time into a (sic!) middle endian double word,
    // í.e.  byte order a | b | c | d  has to be sent as c | d | a | b
    instruc[3] = (intval & 0xFF000000) / 0x1000000;
    instruc[4] = (intval & 0x00FF0000) / 0x10000;
    instruc[1] = (intval & 0x0000FF00) / 0x100;
    instruc[2] = intval & 0x000000FF;
    // send instructions to SerConn
    bool ok = SerConn->write(instruc, 5);
    return ok;
}

// Move the scanner with standardized speed
void ScanController::MoveToX(int16_t newxpos)
{
    TEST_SERIALCONNECTION_VOID;
    int16_t oldypos = get_Ypos();
    int16_t oldxpos = get_Xpos();
    float time = abs(newxpos-oldxpos) / 230;

    if (newxpos!=oldxpos){
        bool ok = h_moveToXY(newxpos, oldypos, time);
        if (ok){
            log_text(tr("Scanner set to X = %1").arg(newxpos));
        } else {
            log_text(tr("Setting the scanner position to X = %1 failed: %2\n").arg(newxpos).arg(QString((SerConn->getLastError()).c_str())));
        }
    } else {
        log_warning(tr("No new X-Coordinate specified."));
    }
}

void ScanController::MoveToX(int16_t newxpos, float moveTime)
{
    TEST_SERIALCONNECTION_VOID;
    int16_t oldypos = get_Ypos();
    int16_t oldxpos = get_Xpos();
    if (newxpos!=oldxpos){
        bool ok = h_moveToXY(newxpos, oldypos, moveTime);
        if (ok){
            log_text(tr("Scanner set to X = %1").arg(newxpos));
        } else {
            log_text(tr("Setting the scanner position to X = %1 failed: %2\n").arg(newxpos).arg(QString((SerConn->getLastError()).c_str())));
        }
    } else {
        log_warning(tr("No new X-Coordinate specified."));
    }

}


// SLOT method for moving the scanner with standardardized speed
void ScanController::MoveToY(int16_t newypos)
{
    TEST_SERIALCONNECTION_VOID;
    int16_t oldxpos = get_Xpos();
    int16_t oldypos = get_Ypos();
    float time = abs(newypos-oldypos) / 230;
    if (newypos!=oldypos){
        bool ok = h_moveToXY(oldxpos, newypos, time);
        if (ok){
            log_text(tr("Scanner set to Y = %1").arg(newypos));
        } else {
            log_text(tr("Setting the scanner position to Y = %1 failed: %2\n").arg(newypos).arg(QString((SerConn->getLastError()).c_str())));
        }
    } else {
        log_warning(tr("No new Y-Coordinate specified."));
    }
}

void ScanController::MoveToY(int16_t newypos, float moveTime)
{
    TEST_SERIALCONNECTION_VOID;
    int16_t oldxpos = get_Xpos();
    int16_t oldypos = get_Ypos();
    if (newypos!=oldypos){
        bool ok = h_moveToXY(oldxpos, newypos, moveTime);
        if (ok){
            log_text(tr("Scanner set to Y = %1").arg(newypos));
        } else {
            log_text(tr("Setting the scanner position to Y = %1 failed: %2\n").arg(newypos).arg(QString((SerConn->getLastError()).c_str())));
        }
    } else {
        log_warning(tr("No new Y-Coordinate specified."));
    }
}

// SLOT method for moving the scanner with standardized speed
void ScanController::MoveToXY(int16_t newxpos, int16_t newypos)
{
    TEST_SERIALCONNECTION_VOID;
    int16_t oldxpos = get_Xpos();
    int16_t oldypos = get_Ypos();
    float time = ((float) std::max(abs(newxpos-oldxpos), abs(newypos-oldxpos))) / 230;
    if ((newxpos != oldxpos) || (newypos != oldypos)){
        bool ok = h_moveToXY(newxpos, newypos, time);
        if (ok){
            log_text(tr("Scanner set to X = %1").arg(newxpos));
            log_text(tr("Scanner set to Y = %1").arg(newypos));
        } else {
            log_text(tr("Setting the scanner position to X = %1 failed: %2\n").arg(newxpos).arg(QString((SerConn->getLastError()).c_str())));
            log_text(tr("Setting the scanner position to Y = %1 failed: %2\n").arg(newypos).arg(QString((SerConn->getLastError()).c_str())));
        }
    } else {
        log_warning(tr("No new Coordinate specified. Scanner was not moved"));
    }
}

void ScanController::MoveToXY(int16_t newxpos, int16_t newypos, float moveTime)
{
    TEST_SERIALCONNECTION_VOID;
    int16_t oldxpos = get_Xpos();
    int16_t oldypos = get_Ypos();
    if ((newxpos!= oldxpos) || (newypos!= oldypos)){
        bool ok = h_moveToXY(newxpos, newypos, moveTime);
        if (ok){
            log_text(tr("Scanner set to X = %1").arg(newxpos));
            log_text(tr("Scanner set to Y = %1").arg(newypos));
        } else {
            log_text(tr("Setting the scanner position to X = %1 failed: %2\n").arg(newxpos).arg(QString((SerConn->getLastError()).c_str())));
            log_text(tr("Setting the scanner position to Y = %1 failed: %2\n").arg(newypos).arg(QString((SerConn->getLastError()).c_str())));
        }
    } else {
        log_warning(tr("No new Coordinate specified. Scanner was not moved"));
    }
}

bool ScanController::TriggerMove(uint8_t triggerChannel, int16_t newxpos, int16_t newypos, float moveTime)
{
    // functional splitted version:/
    /*
    TEST_SERIALCONNECTION_BOOL;
    bool ok;
    QTime t;
    ok = wait(0.575);
    if(!ok)
        return false;
    t.start();
    while(t.elapsed() < 1){
    }
    setSync(triggerChannel);
    if(!ok)
        return false;
    ok = wait(0.575);
    if(!ok)
        return false;
    t.restart();
    while(t.elapsed() < 1){
    }
    ok = unSetSync(triggerChannel);
    if(!ok)
        return false;
    ok = h_moveToXY(newxpos, newypos, moveTime);
    return ok;
*/
      //Alternative method: sends a single bit pattern to the scanner;
     TEST_SERIALCONNECTION_BOOL;
    bool ok;
    QTime t;

    //// check parameter ranges
    if ( (triggerChannel != 1) &&
         (triggerChannel != 2) &&
         (triggerChannel != 3) &&
         (triggerChannel != 4) &&
         (triggerChannel != 13) &&
         (triggerChannel != 14) ){
        log_error(QString("triggerChannel is %1 in TriggerMove is not a valid channel!").arg(triggerChannel));
        return false;
    }

    // check if time Maximum is exceeded
    if ((moveTime > 23 * 0xFFFF / 1000)){
        log_error(QString("Given time in h_moveToXY exceeds the maximum value!"));
        return false;
    }
    // build instructions:
    uint8_t instruc[23];
    memset(instruc, 0x00, 23);
    instruc[0] = 0x10;  // wait, 5 bytes
    instruc[2] = 0x19;  // 25 ticks a 23 microseconds
    instruc[5] = 0x12;  // set Sync trigger high, 3 bytes
    instruc[7] = triggerChannel;
    instruc[8] = 0x10;  // wait, 5 bytes
    instruc[10] = 0x19;  // 25 ticks a 23 microseconds
    instruc[13] = 0x13;  // set Sync trigger low, 3 bytes
    instruc[15] = triggerChannel;
    instruc[16] = 0x06; // move
    instruc[18] = newxpos & 0xFF; //
    instruc[17] = (newxpos>>8)&0xFF;
    instruc[20] = newypos & 0xFF;
    instruc[19] = (newypos>>8) & 0xFF;
    uint16_t ticks = (uint16_t) moveTime*1000/23; // returns number of ticks of 23 microseconds length each;
    instruc[22] = ticks & 0xFF; // add moveTime
    instruc[21] = (ticks>>8) & 0xFF;
    ok = SerConn->write(instruc, 23);
    return ok;

}

void ScanController::setLog(QtLogFile *logfile)
{
    log = logfile;
}

///////test SLOT method
/*
void ScanController::move()// test SLOT function for moveButton
{
    if (SerConn){
        MoveToXY((int16_t) x_axis->value(), (int16_t) y_axis->value());
        log_text(tr("TEST MOVE TARGET X = %1 ").arg(get_Xpos()));
        log_text(tr("TEST MOVE TARGET Y = %2 ").arg(get_Ypos()));
    } else {
        log_error("No connection to Scanner has been established.");
    }
}
*/
/////// END TEST SLOT METHOD






/*
  Initiates the scanner and calibrates the readback values for controller;
  can be called as SLOT additionally;
*/
bool ScanController::initScanner()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    bool ok;
    QTime t;
    if(!SerConn){
        QApplication::restoreOverrideCursor();
        log_warning("No connection to the Scanner has been established. Initialization failed.");
        return false;
    }
    ok = SerConn->isConnectionOpen();
    if(!ok){
        log_error("No connection to the Scanner has been established. Initialization failed.");
        QApplication::restoreOverrideCursor();
        return false;
    }
    log_text(tr("---------------------------------\nInitialize Scanner...\n"));
    // Read Error Messages from the scanner
    QList<uint16_t> status = get_Status();
    if (status.length() > 0){
        log_text(tr("*************************************************\n****** get_Status(): "));
        log_text(tr("****** Status Message 1: Error Source: %1").arg(status.at(0)));
        log_text(tr("****** Status Message 2: Command in error: %1").arg(status.at(1)));
        log_text(tr("****** Status Message 3: System Error: %1\n*************************************************").arg(status.at(2)));
    } else {
        log_error("Error: No error messages could be read");
        QApplication::restoreOverrideCursor();
        return false;
    }
    // return version number etc.
    QList<QString> hwinfo = getHardwareInformation();
    log_text(tr("Boot Segment Division: %1").arg(hwinfo.at(0)));
    log_text(tr("Firmware Revision (default 1.0): %1").arg(hwinfo.at(1)));
    log_text(tr("Hardware Default (default 0): %1").arg(hwinfo.at(2)));
    log_text(tr("Device ID (default 0): %1").arg(hwinfo.at(3)));
// Enable SAXes
    ok = enableSAXes(true, true);
    if (ok){
        log_text("SAXes enabled. Lang lebe König Stanislaus I.");
        t.start();
        while(t.elapsed() < 2){
        }
    } else {
        log_error(tr("Error: SAXes could not be enabled...\nScanner initiation aborted...\nError Message: %1").arg(tr(SerConn->getLastError().c_str())));
        QApplication::restoreOverrideCursor();
        return false;
    }
    // calibrate scanner
    ok = calibrate();
    // check for errors
    status = get_Status();
    if (status.length() > 0){
        log_text(tr("*************************************************\n******get_Status(): "));
        log_text(tr("****** Status Message 1: Error Source: %1").arg(status.at(0)));
        log_text(tr("****** Status Message 2: Command in error: %1").arg(status.at(1)));
        log_text(tr("****** Status Message 3: System Error: %1\n*************************************************").arg(status.at(2)));
    } else {
        log_error("Error: No error messages could be read");
        QApplication::restoreOverrideCursor();
        return false;
    }
    QApplication::restoreOverrideCursor();
    if(status.at(2) == 0)
        return ok;
    else
        return false;
}

bool ScanController::disconnect()
{
    bool ok;
    if(!SerConn){
        log_error("No Connection that could have been closed had been established before!");
        return false;
    }
    if(SerConn->isConnectionOpen()){
        ok = SerConn->close();
        if (ok){
            log_text(tr("Scanner has been disconnected...\n*------------------------"));
        } else {
            log_error(tr("Error: Disconnection failed.\nError Messgae: %1").arg(tr(SerConn->getLastError().c_str())));
            return ok;
        }
        return ok;
    } else {
        log_text("Scanner was not connected and could not be disconected thereby!");
        return false;
    }
}

bool ScanController::set_readback_factors(double gain_x, double offset_x, double gain_y, double offset_y)
{
    bool ok;
    QTime t;
    int waittime = 10;
    log_text(tr("++++++++++++++++++++++++++++++++++++++++++++\n+++ Set readback correction factors of the scanner:"));
    // Set X gain correction factor:
    ok = setConfigVar(2, gain_x);
    if (ok){
        log_text(tr("+++ X readback gain correction set to %1").arg(gain_x, 0, 'g', 4));
        t.start();
        while(t.elapsed() < waittime){
        }
    } else {
        log_error(tr("Error: X readback gain correction could not be set!!\nError Messgae: %1").arg(tr(SerConn->getLastError().c_str())));
        return false;
    }
    // Set X offset correction factor:
    ok = setConfigVar(3, offset_x);
    if (ok){
        log_text(tr("+++ X readback offset correction set to %1").arg(offset_x, 0, 'g', 4));
        t.restart();
        while(t.elapsed() < waittime){
        }
    } else {
        log_error(tr("Error: X readback offset correction could not be set!!\nError Messgae: %1").arg(tr(SerConn->getLastError().c_str())));
        return false;
    }
    // Set Y gain correction factor:
    ok = setConfigVar(4, gain_y);
    if (ok){
        log_text(tr("+++ Y readback gain correction set to %1").arg(gain_y, 0, 'g', 4));
        t.restart();
        while(t.elapsed() < waittime){
        }
    } else {
        log_error(tr("Error: Y readback gain correction could not be set!!\nError Messgae: %1").arg(tr(SerConn->getLastError().c_str())));
        return false;
    }
    // Set Y offset correction factor:
    ok = setConfigVar(5, offset_y);
    if (ok){
        log_text(tr("+++ Y readback offset correction set to %1").arg(offset_y, 0, 'g', 4));
        t.restart();
        while(t.elapsed() < waittime){
        }
    } else {
        log_error(tr("Error: Y readback offset correction could not be set!!\nError Messgae: %1").arg(tr(SerConn->getLastError().c_str())));
        return false;
    }
    log_text(tr("++++++++++++++++++++++++++++++++++++++++++++"));
    return ok;
}

QList<double> ScanController::calc_linear_regression(int16_t *Yvals, int16_t *Xvals, uint8_t popsize)
{
    QList<double> result;
    double x_i;
    double y_i;
    double sumx=0;
    double sumy=0;
    double sumxsquared=0;
    double sumxy=0;
    double meanx=0;
    double meany=0;

    // calculate the "sum components":
    for (int i=0; i<popsize; i++){
        x_i = Xvals[i];
        y_i = Yvals[i];
        sumx += x_i;
        sumy += y_i;
        sumxsquared += x_i * x_i;
        sumxy += x_i * y_i;
    }
    meanx = sumx / popsize;
    meany = sumy / popsize;
    // calculate slope;
    double slope = (popsize * sumxy - sumx*sumy) / (popsize * sumxsquared - sumx*sumx);
    result.append(slope);
    // calculate offset
    double offset = meany - slope*meanx;
    result.append(offset);
    return result;
}
void ScanController::log_text(QString message) {
    QFPluginLogTools::log_text(LOG_PREFIX + message + "\n");
}

void ScanController::log_warning(QString message) {
    QFPluginLogTools::log_warning(LOG_PREFIX + message + "\n");
}

void ScanController::log_error(QString message) {
    QFPluginLogTools::log_error(LOG_PREFIX + message + "\n");
}
