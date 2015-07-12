#include "mcstagecontroller.h"
#define LOG_PREFIX "[mcstagecontroller]: "
MCStageController::MCStageController(std::string Port, float accel, float vel, float joyspeed, QObject *parent) :
    QObject(parent)
{
    // Objects and status flags
    SerConn = NULL;
    dims = NULL;
    joystick_active = false;
    // configurable values:
    comPort = Port;
    max_accel = accel;
    max_vel = vel;
    max_joyspeed = joyspeed;
    // fixed values
    comPortSpeed = 9600;


    /* DEFAULTS
    comPort = std::string("COM2")
    max_accel = 10.0
    max_vel = 10.0
    max_joyspeed = 10.0
      */
}

MCStageController::~MCStageController()
{
    disconnect();
    delete SerConn;
}


bool MCStageController::connectToStage()
{
    SerConn = new QFSerialConnection(comPort, comPortSpeed);
    SerConn->set_logToFile(true);
    bool ok;
    if(!SerConn->isConnectionOpen()){
        ok = SerConn->open(comPort, comPortSpeed);
        if (ok){
            log_text("-------------------------------------\nConnection to Stage established succesfully.\n");
            //ok = initStage();
        } else {
            log_warning(tr("Connection to the Stage could not be established.\nError Messgae: %1").arg(SerConn->getLastError().c_str()));
        }
        return ok;
    } else {
        log_warning("Already connected to the Stage! No new connection established!");
        return false;
    }
}

bool MCStageController::move_lim(float xpos, float ypos, float zpos, float wait)
{
    bool ok;
    ok = move_To(xpos, ypos, zpos);
    if(wait > 0){
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        QTime t;
        t.start();
        while (t.elapsed() < wait){
        }
        QApplication::restoreOverrideCursor();
    }
    return ok;
}
/*
bool MCStageController::move_remove(float xpos, float ypos, float zpos, float wait1, float wait2)
{
    bool ok;
    float oldxpos = get_position(1, true);
    float oldypos = get_position(2, true);
    float oldzpos = get_position(3, true);

    ok = move_To(xpos, ypos, zpos);
    if(wait1 > 0){
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        QTime t;
        t.start();
        while (t.elapsed() < wait1){
        }
        QApplication::restoreOverrideCursor();
    }
    int errorcode = getLastErrorCode();
    if (errorcode  == 1004 )
    {
        log_warning("Stage was instructed to drive to a position which exceeds the limit; it will return to its old position");
        move_To(oldxpos, oldypos, oldzpos);
        if(wait2 > 0){
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
            QTime t;
            t.start();
            while (t.elapsed() < wait2){
            }
            QApplication::restoreOverrideCursor();
        }
        ok = false;
    }
    return ok;
}
*/
bool MCStageController::connected()
{
    if (SerConn)
        return SerConn->isConnectionOpen();
    else
        return false;
}

bool MCStageController::hardware_error()
{
    if(getLastErrorCode() == 0)
        return false;
    else
        return true;
}

bool MCStageController::config_joystick(bool enable, float speed)
{
    bool ok;
    if (enable){
        ok = set_joystick(true, true);
        if (ok)
           ok = set_max_joyspeed(speed);
    } else {
       ok = set_joystick(false, true);
    }
    return ok;
}

float MCStageController::get_currentPosition(uint8_t axis)
{
    return get_position(axis, true);
}

uint8_t MCStageController::getDimensions()
{
    if (dims)
        return dims;
    else
        return NULL;
}

uint16_t MCStageController::get_VersionNumber()
{
    bool ok;
    QTime t;
    uint16_t version;
    std::string response;
    if (SerConn->isConnectionOpen())
        ok = true;
    else {
        log_error("Error in MCStageController::get_VersionNumber(): Stage has not been connected.");
        ok = false;
    }
    if (ok){
        t.start();
        while(t.elapsed() < 15){
        }
        SerConn->write(std::string("version\r\0"));
        response = SerConn->readUntil("\r\n",&ok);
    } else {
        log_error(tr("Error occured in MCStageController::get_VersionNumber(): Version Number could not be read."));
        log_error(tr("Error Message: %1").arg(SerConn->getLastError().c_str()));
    }
    if (ok){
        chop_end_recog_seq(response);
        version = (uint16_t) atoi(response.c_str());
        log_text(tr("MultiControl2000 Version Number = %1").arg(version));
    }
    if (ok) return version;
    else return -1;
}

float MCStageController::get_max_accel()
{
    std::string response;
    float accel;
    QTime t;
    if (!SerConn->isConnectionOpen()){
        log_error(" in MCStageController::get_max_accel(): Stage has not been connected.");
        return -1.0;
    }
    t.start();
    log_text("Check maximal acceleration...");
    while(t.elapsed() < 15){
    }
    bool ok;
    ok = SerConn->write(std::string("getaccel\r\0"));
    if (!ok){
        log_error("Error in MCStageController::get_max_accel(): Error after sending the instruction.");
        log_error(tr("Error Message: %1").arg(SerConn->getLastError().c_str()));
        return -1.0;
    }
    response = SerConn->readUntil("\r\n",&ok);
    if(ok){
        chop_end_recog_seq(response);
        accel = atof(response.c_str());
        return accel;
        //log_text(tr("Acceleration is %1").arg(version));
    } else {
        log_error("Error in MCStageController::get_max_accel(): Error occured while reading the response of the serial connection.");
        log_error(tr("Error Messgae from Serial Connection: %1").arg(SerConn->getLastError().c_str()));
        return -1.0;
    }
}

float MCStageController::get_max_vel()
{
    std::string response;
    float vel;
    QTime t;
    if (!SerConn->isConnectionOpen()){
        log_error("Error in MCStageController::get_max_accel(): Stage has not been connected.");
        return -1.0;
    }
    t.start();
    log_text("Check maximal velocity...");
    while(t.elapsed() < 15){
    }
    bool ok;
    ok = SerConn->write(std::string("getvel\r\0"));
    if (!ok){
        log_error("Error in MCStageController::get_max_vel(): Error after sending the instruction.");
        log_error(tr("Error Messgae: %1").arg(SerConn->getLastError().c_str()));
        return -1.0;
    }
    response = SerConn->readUntil("\r\n",&ok);
    if(ok){
        chop_end_recog_seq(response);
        vel = atof(response.c_str());
        return vel;
        //log_text(tr("Acceleration is %1").arg(version));
    } else {
        log_text("Error in MCStageController::get_max_vel(): Error occured while reading the response of the serial connection.");
        log_text(tr("Error Messgae: %1").arg(SerConn->getLastError().c_str()));
        return -1.0;
    }
}

float MCStageController::get_max_joyspeed()
{
    std::string response;
    float speed;
    QTime t;
    if (!SerConn->isConnectionOpen()){
        log_error("Error in MCStageController::get_max_joyspeed(): Stage has not been connected.");
        return -1.0;
    }
    t.start();
    log_text("Check maximal joyspeed...");
    while(t.elapsed() < 15){
    }
    bool ok;
    ok = SerConn->write(std::string("getjoyspeed\r\0"));
    if (!ok){
        log_error("Error in MCStageController::get_max_joyspeed(): Error after sending the instruction.");
        log_error(tr("Error Messgae: %1").arg(SerConn->getLastError().c_str()));
        return -1.0;
    }
    response = SerConn->readUntil("\r\n",&ok);
    if(ok){
        chop_end_recog_seq(response);
        speed = atof(response.c_str());
        return speed;
        //log_text(tr("Acceleration is %1").arg(version));
    } else {
        log_error("Error in MCStageController::get_max_speed(): Error occured while reading the response of the serial connection.");
        log_error(tr("Error Messgae: %1").arg(SerConn->getLastError().c_str()));
        return -1.0;
    }
}

QList<bool> MCStageController::get_status()
{
    //check connection and send instruction
    QList<bool> statusflags;
    std::string response;
    uint8_t  stateCode;
    QTime t;
    if (!SerConn->isConnectionOpen()){
        log_error("Error in MCStageController::get_status(): Stage has not been connected.");
        statusflags.prepend(false);
        return statusflags;
    }
    log_text("Check moving state...");
    while(t.elapsed() < 15){
    }
    bool ok;
    ok = SerConn->write(std::string("status\r\0"));
    if (!ok){
        log_error("Error in MCStageController::get_stauts(): Error after sending the instruction.");
        log_error(tr("Error Messgae: %1").arg(SerConn->getLastError().c_str()));
        statusflags.prepend(false);
        return statusflags;
    }
    t.restart();
    while(t.elapsed() < 15){
    }
    // read value back and parse response string;#
    response = SerConn->readUntil("\r\n",&ok);
    if(!ok){
        log_error("Error in MCStageController::get_status(): Error occured while reading the response of the serial connection.");
        log_error(tr("Error Messgae: %1").arg(SerConn->getLastError().c_str()));
        statusflags.prepend(false);
        return statusflags;
    }
    chop_end_recog_seq(response);
    stateCode = (uint8_t) atoi(response.c_str());
    // check response bits and translate the answer to a handy Qlist<bool> :-)
    if (stateCode >= 8){
        statusflags.prepend(true);
        stateCode -= 8;
    } else
        statusflags.prepend(false);
    if (stateCode >= 4){
        statusflags.prepend(true);
        stateCode -= 4;
    } else
        statusflags.prepend(false);
    if (stateCode >= 2){
        statusflags.prepend(true);
        stateCode -= 2;
    } else
        statusflags.prepend(false);
    if (stateCode == 1){
        statusflags.prepend(true);
        stateCode -= 1;
    } else
        statusflags.prepend(false);
    // check, if the translation worked and return answer;
    if (!(stateCode == 0)){
        log_error("Error in QList<bool> MCStageController::get_moveState(): Could not parse the response String properly!");
        statusflags.prepend(false);
        return statusflags;
    } else {
        statusflags.prepend(true);
        return statusflags;
    }
}

float MCStageController::get_pitch(uint8_t axis)
{
    std::string response;
    float pitch;
    QTime t;
    //check argument range
    if (axis > 3){
        log_error("Error occured inMCStageController::get_pitch(uint8_t axis): Argument out of range!");
        return -1.0;
    }
    if (!SerConn->isConnectionOpen()){
        log_error("Error in MCStageController::get_pitch(uint8_t axis): Stage has not been connected.");
        return -1.0;
    }
    t.start();
    while(t.elapsed() < 15){
    }
    bool ok;
    switch(axis){
    case 0:
        log_text("Check pitch of the speed axis...");
        ok = SerConn->write(std::string("0 getpitch\r\0"));
        break;
    case 1:
        log_text("Check pitch of the x-axis...");
        ok = SerConn->write(std::string("1 getpitch\r\0"));
        break;
    case 2:
        log_text("Check pitch of the y-axis...\nGlory to the holy empire of steel and steam!");
        ok = SerConn->write(std::string("2 getpitch\r\0"));
        break;
    case 3:
        log_text("Check pitch of the z-axis...");
        ok = SerConn->write(std::string("3 getpitch\r\0"));
        break;
    }
    if (!ok){
        log_error("Error in MCStageController::get_pitch(): Error after sending the instruction.");
        log_error(tr("Error Messgae: %1").arg(SerConn->getLastError().c_str()));
        return -1.0;
    }
    response = SerConn->readUntil("\r\n",&ok);
    if(ok){
        chop_end_recog_seq(response);
        pitch = atof(response.c_str());
        return pitch;
        //log_text(tr("Acceleration is %1").arg(version));
    } else {
        log_error("Error in MCStageController::get_pitch(): Error occured while reading the response of the serial connection.");
        log_error(tr("Error Messgae: %1").arg(SerConn->getLastError().c_str()));
        return -1.0;
    }
}

float MCStageController::get_position(uint8_t axis, bool silent)
{

    std::string response;
    float pos;
    QTime t;
    //check argument range
    if ((axis > 3) || (axis < 1)){
        log_error("<b>Error</> occured inMCStageController::get_position(uint8_t axis): Argument out of range!");
        return -1.0;
    }
    if (!SerConn->isConnectionOpen()){
        log_error("Error in MCStageController::get_position(uint8_t axis): Stage has not been connected.");
        return -1.0;
    }
    t.start();
    while(t.elapsed() < 15){
    }
    bool ok;
        if(!silent) log_text("Check current positions...");
        ok = SerConn->write(std::string("pos\r\0"));
    if (!ok){
        log_error("Error in MCStageController::get_pitch(): Error after sending the instruction.");
        log_error(tr("Error Messgae: %1").arg(SerConn->getLastError().c_str()));
        return -1.0;
    }
    t.restart();
    while(t.elapsed() < 10){
    }
    response = SerConn->readUntil("\r\n",&ok);
    if(!ok){
        log_error("Error in MCStageController::get_pitch(): Error occured while reading the response of the serial connection.");
        log_error(tr("Error Messgae: %1").arg(SerConn->getLastError().c_str()));
        return -1.0;
    }
    // process the response string, to find out the specified value
    //int i = 0;
    //int n = response.length();
    QString qresponse;
    QRegExp sep = QRegExp(" ");
    QStringList responselist;
    chop_end_recog_seq(response);
    switch(dims){
    case 1:
        pos = atof(response.c_str());
    case 2:

        //log_text(tr("DEBUG: returnstring in get_position(): %1").arg(qresponse));
        qresponse = QString(response.c_str());
        responselist = qresponse.split(sep);
        //log_text(tr("DEBUG: splitted returnstring in get_position(): %1").arg(responselist.at(axis-1)));
        pos = responselist.at(axis-1).toFloat();
    case 3:
        //log_text(tr("DEBUG: returnstring in get_position(): %1").arg(qresponse));
        qresponse = QString(response.c_str());
        responselist = qresponse.split(sep);
        log_text(tr("*****DEBUG: splitted returnstring in get_position(): %1 , axis: %2").arg(responselist.at(axis-1)).arg(axis));
        pos = responselist.at(axis-1).toFloat();
    }
    return pos;
}

int8_t MCStageController::get_unit(uint8_t axis)
{
    std::string response;
    int8_t unit;
    QTime t;
    //check argument range
    if (axis > 3){
        log_error("Error occured inMCStageController::get_axis_mode(uint8_t axis): Argument out of range!");
        return -1;
    }
    if (!SerConn->isConnectionOpen()){
        log_error("Error in MCStageController::get_axis_mode(): Stage has not been connected.");
        return -1;
    }
    t.start();
    while(t.elapsed() < 15){
    }
    bool ok;
    switch(axis){
    case 0:
        log_text("Check unit of the speed axis...");
        ok = SerConn->write(std::string("0 getunit\r\0"));
    case 1:
        log_text("Check unit of the x-axis...");
        ok = SerConn->write(std::string("1 getunit\r\0"));
    case 2:
        log_text("Check unit of the y-axis...\n");
        ok = SerConn->write(std::string("2 getunit\r\0"));
    case 3:
        log_text("Check unit of the z-axis...");
        ok = SerConn->write(std::string("3 getunit\r\0"));
    }
    if (!ok){
        log_error("Error in MCStageController::get_unit(): Error after sending the instruction.");
        log_error(tr("Error Messgae: %1").arg(SerConn->getLastError().c_str()));
        return -1;
    }
    response = SerConn->readUntil("\r\n",&ok);
    if(ok){
        chop_end_recog_seq(response);
        unit = atoi(response.c_str());
        return unit;
        //log_text(tr("Acceleration is %1").arg(version));
    } else {
        log_error(tr("Error in MCStageController::get_unit(): Error occured while reading the response of the serial connection."));
        log_error(tr("Error Messgae: %1").arg(SerConn->getLastError().c_str()));
        return -1;
    }
}

bool MCStageController::move_To(float xpos, float ypos, float zpos)
{
    // print warnings, if more position parameters were specified with non-0, if the
    // multicontrol stage has been prepared for working in a lower dimension mode (i.e. uint 8_t dims <3);
//    set_joystick(false); // joystick mode has to be inactivated, before instructions are given;
    switch(dims){
    case NULL:
        log_error(tr("Error occured in MCStageController::move_To(float xpos, float ypos, float zpos, uint8_t handle_if_outOfRange) "));
        log_error(tr("!! uint8_t has to be set via set MCStageController::set_dim(uint8_t), before the stage can be moved"));
        return false;
    case 1:
        if ((ypos > 0.0) || (zpos > 0.0)){
            log_warning("WARNING in MCStageController::(float xpos, float ypos, float zpos, uint8_t handle_if_outOfRange)");
            log_warning("uint8_t dims has been set to 1 => wont move to y and z positions");
        }
        break;
    case 2:
        if (zpos > 0.0){
            log_warning("WARNING in MCStageController::move_To(float xpos, float ypos, float zpos, uint8_t handle_if_outOfRange)");
            log_warning("uint8_t dims has been set to 1 => stage won't move to z positions");
        }
        break;
    }
    // build instruction from given data
    QString qstr_xpos;
    QString qstr_ypos;
    QString qstr_zpos;
    QByteArray ba_scale;
    std::string instruction = "";
    bool ok;
    QTime t;
    QString message;
    qstr_xpos = QLocale::system().toString(xpos, 'f', 5);          // float xscale to std::string   |
    ba_scale = qstr_xpos.toLatin1();                     //                               |
    instruction.append(ba_scale.data());  // const char* QByteArray::data() const        V   (result is '\0'-terminated and can be appended)
    instruction.append(" ");
    if ((dims == 2) || (dims == 3)){
        qstr_ypos = QLocale::system().toString(ypos, 'f', 5);  // float yscale to std::string   |
        ba_scale = qstr_ypos.toLatin1();            //                                |
        instruction.append(ba_scale.data());        //                               V
        instruction.append(" ");
    }
    if (dims == 3){
        qstr_zpos = QLocale::system().toString(zpos, 'f', 5);  // float zscale to std::string   |
        ba_scale = qstr_zpos.toLatin1();            //                                |
        instruction.append(ba_scale.data());        //                               V
        instruction.append(" ");
    }
    instruction.append("move \r\0");

    if(!SerConn->isConnectionOpen()){
        log_error("Error in move_TO(...): Stage has not been connected.");
        return false;
    }
    ok = SerConn->write(instruction);
    if(ok){
            //build log message
        message = QString("Stage moved to x: %1").arg(qstr_xpos);
        if ((dims == 2) || (dims == 3))
            message.append(tr(", y: %1").arg(qstr_ypos));
        if (dims == 3)
            message.append(tr(", z: %1").arg(qstr_zpos));
        log_text(message);
        return true;
    } else {
        log_error(tr("Serial Connection returned an error message in MCStageController::move_to():"));
        log_error(tr("Error Messgae: %1").arg(SerConn->getLastError().c_str()));
        return false;
    }
}

bool MCStageController::set_max_accel(float accel)
{
    // check range
    if (!(accel > 0)){
        log_text(tr("Error in set_max_accel(float accel) : Argument out of range!"));
        return false;
    }
    // convert float accel to std::string and build instruction
    //std::string instruction = "";
    QString qstr_accel = QLocale::system().toString(accel, 'f', 5);  // float to std::string
    QString qinstr = qstr_accel;
    //QByteArray ba_accel = qstr_accel.toLatin1();
    //qinstr.append(qstr_accel); // const char* QByteArray::data() const (result is '\0'-terminated and can be appended)
    qinstr.append(" setaccel ");
    // send instruction to serial connection
    if (!SerConn->isConnectionOpen()){
        log_text("Error in MCStageController::get_max_accel(): Stage has not been connected.");
        return false;
    }
    bool ok;
    // log_text(tr("DEBUG in set_max_accel(), instruction after building the instruction is: \"%1\"").arg(qinstr));//check the instruction for correct syntax
    ok = SerConn->write(qinstr.toStdString());
    if (!ok){
        log_text("Error in MCStageController::set_max_accel(): Error occured while writing to the serial connection.");
        log_text(tr("Error Messgae: %1").arg(SerConn->getLastError().c_str()));
        return false;
    } else {
        log_text(tr("Trying to set maximal acceleration to %1; this value might be rounded off.").arg(qstr_accel));
    }
    // check the value and return;
    float check = get_max_accel();
    QString qstr_check = QLocale::system().toString(check, 'f', 5);  // float to std::string
    QByteArray ba_check = qstr_check.toLatin1();
    if (!(check < 0)){
        log_text(tr("Maximal acceleration set to %1").arg(ba_check.data()));
        return true;
    } else {
        log_text(tr("Acceleration check value is %1; The maximal acceleration could probably not be set!").arg(ba_check.data()));
        return false;
    }
}

bool MCStageController::set_dim(uint8_t dimensions)
{
    bool ok;
    if(!SerConn->isConnectionOpen()){
        log_error(tr("Error in MCStageController::set_dim(uint8_t dimensions). Stage is not connected"));
        return false;
    }
    switch(dimensions){
    case 1:
        ok = SerConn->write("1 setdim ");
        log_text("Try to set the dimension of position inputs to 1...");
            break;
    case 2:
            ok = SerConn->write("2 setdim ");
            log_text("Try to set the dimension of position inputs to 2...");
        break;
    case 3:
        ok = SerConn->write("3 setdim ");
        log_text("Try to set the dimension of position inputs to 3...");
        break;
    default:
        log_text(tr("<b>FAIL:</b> Wrong input parameter \"%1\" in MCStageController::set_dim(uint8_t dimensions).\ndims could not be set.").arg(dimensions));
            return false;
    }
    if(ok){
        dims = dimensions;
        if (dims == 1) log_text("Dimension of position input set to 1: x-values will be accepted.");
        else if (dims == 2) log_text("Dimension of position input set to 2: x- and y-values will be accepted.");
        else if (dims ==3) log_text("Dimensions of position input set to 3: x-, y- and z-values will be accepted.");
    }
    return ok;
}

bool MCStageController::set_axes_modes(uint8_t mode_x, uint8_t mode_y, uint8_t mode_z)
{
    QTime t;
    bool ok;
/*
    t.start();
    while(t.elapsed() < 15){
    }

*/
    // check input parameter range;
    if ((0 > mode_x) || (0 > mode_y) || (0 > mode_z) || (mode_x > 2) || (mode_y > 2) || (mode_z > 2)){
        log_text("Error occured in MCStageController::set_axis_modes: At least one input parameter is out of range!");
        return false;
    }
    if(!SerConn->open()){
        log_text(tr("<b>FAIL </b> in MCStageController::set_axis_modes(uint8_t code_x, uint8_t code_y, uint8_t code_z): Stage is not connected!"));
        return false;
    }
    // set x-axis mode;
    switch (mode_x){
    case 0:
        ok = SerConn->write("0 1 setaxis ");
        log_text("x-axis switched off.");
        break;
    case 1:
        ok = SerConn->write("1 1 setaxis ");
        log_text("x-axis switched on.");
        break;
    case 2:
        ok = SerConn->write("2 1 setaxis ");
        log_text("x-axis switched on (restricted mode 2).");
        break;
    }
    t.start();
    if(!ok){
        log_error(tr("Error occured while setting x-axis mode.\nError Message: %1").arg((SerConn->getLastError().c_str())));
        return false;
    }
    while(t.elapsed() < 15){
    }
    // set y-axis mode;
    switch (mode_y){
    case 0:
        ok = SerConn->write("0 2 setaxis ");
        log_text("y-axis switched off.");
        break;
    case 1:
        ok = SerConn->write("1 2 setaxis ");
        log_text("y-axis switche on.");
        break;
    case 2:
        ok = SerConn->write("2 2 setaxis ");
        log_text("y-axis switched on (restricted mode 2).");
        break;
    }
    t.restart();
    if(!ok){
        log_error(tr("Error occured while setting y-axis mode.\nError Message: %1").arg((SerConn->getLastError().c_str())));
        return false;
    }
    while(t.elapsed() < 15){
    }
    // set y-axis mode;
    switch (mode_z){
    case 0:
        ok = SerConn->write("0 3 setaxis ");
        log_text("z-axis switched off.");
        break;
    case 1:
        ok = SerConn->write("1 3 setaxis ");
        log_text("z-axis switche on.");
        break;
    case 2:
        ok = SerConn->write("2 3 setaxis ");
        log_text("z-axis switched on (restricted mode 2).");
        break;
    }
    if(!ok){
        log_error(tr("Error occured while setting y-axis mode.\nError Message: %1").arg((SerConn->getLastError().c_str())));
        return false;
    } else {
        return true;
    }
}


bool MCStageController::set_ht_mode(uint8_t mode)//(uint8_t mode,bool secondtry)
{

    bool ok;
    if(!SerConn->isConnectionOpen()){
        log_error("Error in MCStageController::set_mode(uint8_t mode): Stage has not been connected.");
        return false;
    }
    if (mode == 0){
      //  if (!secondtry)
            ok = SerConn->write(std::string("0 mode "));
        //else
          //  ok = SerConn->write(std::string("0 mode\0"));
        log_text("Try to enable the Stage host mode...");
    } else if (mode == 1){
        //if (!secondtry)
            ok = SerConn->write(std::string("1 mode "));
       // else
         //   ok = SerConn->write("1 mode\0");
        log_text("Try to enable the Stage terminal mode...");
    } else {
        log_text(tr("<b>FAIL:</b> Wrong input parameter \"%1\"in MCStageController::set_mode(int mode).\nMode could not be set.").arg(mode) );
        return false;
    }
    if (!ok){
        log_text(tr("FAIL: Serial Connection returned error message in MCStageController::set_mode(int mode)\nError Messgae: %1").arg(SerConn->getLastError().c_str()));
        return false;
    }
        return ok;
}

bool MCStageController::set_max_joyspeed(float speed, bool silent)
{    // check range
    if (!(speed > 0)){
        log_error(tr("Error in set_max_joyspeed(float speed) : Argument out of range!"));
        return false;
    }
    // build instruction string
    QString qstr_speed = QLocale::system().toString(speed, 'f', 5);  // float to std::string
    QString qinstr = qstr_speed;
    qinstr.append(" joyspeed ");
    // send instruction to serial connection
    if (!SerConn->isConnectionOpen()){
        log_error("Error in MCStageController::get_max_joyspeed(): Stage has not been connected.");
        return false;
    }
    bool ok;
//    log_text(tr("DEBUG in set_max_joyspeed(), instruction after building the instruction is: \"%1\"").arg(qinstr));//check the instruction for correct syntax
    ok = SerConn->write(qinstr.toStdString());
    if (!ok){
        log_error("Error in MCStageController::set_max_jopyspeed(): Error occured while writing to the serial connection.");
        log_error(tr("Error Messgae: %1").arg(SerConn->getLastError().c_str()));
        return false;
    } else {
        if (!silent) log_text(tr("Trying to set maximal joyspeed to %1; this value might be rounded off.").arg(qstr_speed));
    }
    // check the value and return;
    float check = get_max_joyspeed();
    QString qstr_check = QLocale::system().toString(check, 'f', 5);  // float to std::string
    //QByteArray ba_check = qstr_check.toLatin1();
    if (!(check < 0)){
        if (!silent) log_text(tr("Maximal joyspeed set to %1").arg(qstr_check));
        return true;
    } else {
        if (!silent) log_text(tr("joyspeed check value is %1; The maximal joyspeed could probably not be set!").arg(qinstr));
        return false;
    }
}

bool MCStageController::set_joystick(bool mode, bool silent)
{

    if (!SerConn->isConnectionOpen()){
        log_error("Error in MCStageController::set_joystick(): Stage has not been connected.");
        return false;
    }
    bool ok;
    if(mode)
        ok = SerConn->write(std::string("1 joystick "));
    else
        ok = SerConn->write(std::string("0 joystick "));
    if (ok && mode){
        if (!silent) log_text("Joystick switched on.");
        joystick_active = true;
        return true;
    } else if (ok && !(mode)){
        if (!silent) log_text("Joystick switched off.");
        joystick_active = false;
        return true;
    } else {
        log_error("Error MCStageController::set_joystick(bool mode): in Joystick state could not be changed;");
        log_error(tr("Error Messgae: %1").arg(SerConn->getLastError().c_str()));
        return false;
    }
}

bool MCStageController::set_unit(uint8_t axis, uint8_t unit)
{
    bool ok;
    if (!SerConn->open()){
        log_error("Error in set_unit(uint8_t axis, uint8_t unit): Stage has not been connected.");
        return false;
    }
    // check parameter range
    if ((axis > 3) || (unit > 6)){
        log_error("Error in MCStageController::set_unit(uint8_t axis, uint8_t unit): Input parameter out of range");
        return false;
    }
    // build instruction;

    QString qinstr(QString::number(axis));
    qinstr.append(" ");
    //log_text(tr("DEBUG in set_unit(), instruction after first append() session is: \"%1\"").arg(qinstr));//check the instruction for correct syntax
    qinstr.append(QString::number(unit));
    //log_text(tr("DEBUG in set_unit(), instruction after second append() session is: \"%1\"").arg(qinstr));//check the instruction for correct syntax
    qinstr.append(" setunit ");
//    log_text(tr("DEBUG in set_unit(), instruction after third append() session is: \"%1\"").arg(qinstr));//check the instruction for correct syntax
    // send instruction string to serial port
    ok = SerConn->write(qinstr.toStdString());
    if (ok){
        // build log message:
        QString message = QString("Unit of the ");
        switch(axis){
            case 0:
                message.append(QString("speed axis "));
                break;
            case 1:
                message.append(QString("x-axis"));
                break;
            case 2:
                message.append(QString("y-axis "));
                break;
            case 3:
                message.append(QString("z-axis "));
                break;
            }
            message.append("set to ");
            switch(unit){
            case 0:
                message.append(QString("microsteps."));
                break;
            case 1:
                message.append(QString("micrometers."));
                break;
            case 2:
                message.append(QString("mm."));
                break;
            case 3:
                message.append(QString("cm."));
                break;
            case 4:
                message.append(QString("m."));
                break;
            case 5:
                message.append(QString("inch."));
                break;
            case 6:
                message.append(QString("mil (= 1/1000 inch)."));
                break;
            }
            log_text(message);
        return true;
    } else {
        log_error(tr("Serial Connection returned an error message in MCStageController::set_unit(uint8_t axis, uint8_t unit):"));
        log_error(tr("Error Messgae: %1").arg(SerConn->getLastError().c_str()));
        return false;
    }
}

bool MCStageController::set_pitch(uint8_t axis, float pitch)
{
    bool ok;
    if (!SerConn->open()){
        log_error("Error in set_pitch(uint8_t axis, float pitch): Stage has not been connected.");
        return false;
    }
    // check parameter range
    if ((axis < 0) || (axis > 3) || (pitch < 0.0001) || (pitch > 1000.0)){
        log_error("Error in MCStageController::set_unit(uint8_t axis, float pitch): Input parameter out of range");
        return false;
    }
    // build instruction:
    QString qstr_pitch = QLocale::system().toString(pitch, 'f', 5);  // float to std::string
    QString qstr_axis = QString:: number(axis,10);
    QString qinstr = qstr_pitch;
    qinstr.append(" ");
    qinstr.append(qstr_axis);
    qinstr.append(" setpitch ");
//    log_text(tr("DEBUG in set_pitch(), instruction after third append() session is: \"%1\"").arg(qinstr));//check the instruction for correct syntax
    // send instruction string to serial port
    ok = SerConn->write(qinstr.toStdString());
    if (ok){
        // build log message:
            QString message = QString("Pitch of the ");
            switch(axis){
            case 0:
                message.append(QString("speed axis "));
                break;
            case 1:
                message.append(QString("x-axis"));
                break;
            case 2:
                message.append(QString("y-axis "));
                break;
            case 3:
                message.append(QString("z-axis "));
                break;
            }
            message.append(tr("set to %1 mm").arg(qstr_pitch));
            log_text(message);
        return true;
    } else {
        log_error(tr("Serial Connection returned an error message in MCStageController:: :"));
        log_error(tr("Error Messgae: %1").arg(SerConn->getLastError().c_str()));
        return false;
    }
}

bool MCStageController::set_scales(float xscale, float yscale, float zscale)
{
    // check, if the right argument list has been handed over (corresponding to uint8_t)
    // and the parameters are valid
    switch(dims){
    case NULL:
        log_error(tr("Error occured in MCStageController::set_scales(float xscale, float yscale = 0, float zscale=0 ): "));
        log_error(tr("uint8_t has to be set vio set MCStageController::set_dim(uint8_t), before the scales can be set !! "));
        return false;
    case 1:
        if ((yscale > 0.0) || (zscale > 0.0)){
            log_error("WARNING in MCStageController::set_scales(float xscale, float yscale=0, float zscale=0):");
            log_error("uint8_t dims has been set to 1 => scaling y- or z-axis is void and won't be conducted!");
        }
        if(!(xscale > 0.0)){
            log_error("Error in MCStageController::set_scales(float xscale, float yscale = 0, float zscale=0 ):");
            log_error("Input parameter is out of range!");
            return false;
        }
        break;
    case 2:
        if (zscale > 0.0){
            log_error("WARNING in MCStageController::set_scales(float xscale, float yscale=0, float zscale=0):");
            log_error("uint8_t dims has been set to 1 => scaling z-axis is void and won't be conducted!");
            break;
        }
        if((!(xscale > 0.0)) || (!(yscale > 0.0))){
            log_error("Error in MCStageController::set_scales(float xscale, float yscale = 0, float zscale=0 ):");
            log_text("Input parameter is out of range!");
            return false;
        }
    case 3:
        if((!(xscale > 0.0)) || (!(yscale > 0.0)) || (!(zscale > 0.0))){
            log_error("Error in MCStageController::set_scales(float xscale, float yscale = 0, float zscale=0 ):");
            log_error("Input parameter is out of range!");
            return false;
        }
    }
    //build the instruction string:
    QString qstr_xscale;
    QString qstr_yscale;
    QString qstr_zscale;
    //std::string instruction = "";
    qstr_xscale = QLocale::system().toString(xscale, 'f', 5);
    QString qinstr = qstr_xscale;

    if ((dims == 2) || (dims == 3)){
        qstr_yscale = QLocale::system().toString(yscale, 'f', 5);
        qinstr.append(" ");
        qinstr.append(qstr_yscale);
    }
    if (dims == 3){
        QString qstr_zscale = QLocale::system().toString(zscale, 'f', 5);
        qinstr.append(" ");
        qinstr.append(qstr_zscale);

    }
    qinstr.append(" scale ");
    //log_text(tr("DEBUG in set_scales(), instruction after last append() session is: \"%1\"").arg(qinstr));//check the instruction for correct syntax
    // send instruction to the serial connection
    if(!SerConn->isConnectionOpen()){
        log_error("Error in set_scales(...): Stage has not been connected.");
        return false;
    }
    bool ok;
    ok = SerConn->write(qinstr.toStdString());
    if(ok){
        //build log message
        log_text(tr("scale of x-axis set to %1").arg(qstr_xscale));
        if ((dims == 2) || (dims == 3))
            log_text(tr("scale of y-axis set to %1").arg(qstr_yscale));
        if (dims == 3)
            log_text(tr("scale of z-axis set to %1").arg(qstr_zscale));
        return true;
    } else {
        log_error(tr("Serial Connection returned an error message in MCStageController::set_scales():"));
        log_text(tr("Error Messgae: %1").arg(SerConn->getLastError().c_str()));
        return false;
    }
}

bool MCStageController::set_origin(float xpos, float ypos, float zpos)
{
    // check, if additional arguments have been handed over, though their dimension is not defined via dims;
    // print a warning in this case
    switch(dims){
    case NULL:
        log_error(tr("Error occured in MCStageController::set_scales(float xscale, float yscale = 0, float zscale=0 ): "));
        log_error(tr("!! uint8_t has to be set vio set MCStageController::set_dim(uint8_t), before the scales can be set !! "));
        return false;
    case 1:
        if ((ypos > 0.0) || (zpos > 0.0) || (ypos < 0.0) || (zpos < 0.0)){
            log_warning("WARNING in MCStageController::set_scales(float xscale, float yscale=0, float zscale=0):");
            log_warning("uint8_t dims has been set to 1 => scaling y- or z-axis is void and won't be conducted!");
        }
        break;
    case 2:
        if ((zpos > 0.0) || (zpos < 0.0)){
            log_warning("WARNING in MCStageController::set_scales(float xscale, float yscale=0, float zscale=0):");
            log_warning("uint8_t dims has been set to 1 => scaling z-axis is void and won't be conducted!");
        }
    }
    //convert float arguments to std::strings and build the instruction string
    QString qstr_xpos;
    QString qstr_ypos;
    QString qstr_zpos;
    qstr_xpos = QLocale::system().toString(xpos, 'f', 5);          // float xscale to std::string   |
    QString  qinstr = qstr_xpos;
    if ((dims == 2) || (dims == 3)){

        qstr_ypos = QLocale::system().toString(ypos, 'f', 5);  // float yscale to std::string   |
        qinstr.append(" ");
        qinstr.append(qstr_ypos);
    }
    if (dims == 3){
        qstr_zpos = QLocale::system().toString(zpos, 'f', 5);  // float zscale to std::string   |
        qinstr.append(" ");
        qinstr.append(qstr_zpos);
    }
    qinstr.append(" setpos ");
//    log_text(tr("DEBUG in set_origin(), instruction after last append() session is: \"%1\"").arg(qinstr));//check the instruction for correct syntax
    // send instruction to the serial connection
    if(!SerConn->isConnectionOpen()){
        log_error("Error in set_pos(...): Stage has not been connected.");
        return false;
    }
    bool ok;
    ok = SerConn->write(qinstr.toStdString());
    if(ok){
        //build log message
        QString message = QString("Origin set to %1").arg(qstr_xpos);
        if ((dims == 2) || (dims == 3))
            message.append(tr(", %1").arg(qstr_ypos));
        if (dims == 3){
            message.append(tr(", %1").arg(qstr_zpos));
        }
        log_text(message);
        return true;
    } else {
        log_error(tr("FAIL: Serial Connection returned an error message in MCStageController::set_scales():"));
        log_error(tr("Error Messgae: %1").arg(SerConn->getLastError().c_str()));
        return false;
    }
}

bool MCStageController::set_max_vel(float vel)
{
    // check range
    if (!(vel > 0)){
        log_error(tr("Error in set_max_accel(float accel) : Argument out of range!"));
        return false;
    }
    //build instruction string
    QString qstr_vel = QLocale::system().toString(vel, 'f', 5);  // float to std::string
    QString qinstr = qstr_vel;
    qinstr.append(" setvel ");
    // send instruction to serial connection
    if (!SerConn->isConnectionOpen()){
        log_error("Error in MCStageController::get_max_vel(): Stage has not been connected.");
        return false;
    }
    bool ok;
//    log_text(tr("DEBUG in set_max_vel(), instruction after building the instruction is: \"%1\"").arg(qinstr));//check the instruction for correct syntax
    ok = SerConn->write(qinstr.toStdString());
    if (!ok){
        log_error("Error in MCStageController::set_max_vel(): Error occured while writing to the serial connection.");
        log_error(tr("Error Messgae: %1").arg(SerConn->getLastError().c_str()));
        return false;
    } else {
        log_text(tr("Trying to set maximal velocity to %1; this value might be rounded off.").arg(qstr_vel));
    }
    // check the value and return;
    float check = get_max_vel();
    QString qstr_check = QLocale::system().toString(check, 'f', 5);

    if (!(check < 0)){
        log_text(tr("Maximal velocity set to %1").arg(qstr_check));
        return true;
    } else {
        log_text(tr("Velocity check value is %1; The maximal velocity could probably not be set!").arg(qstr_check));
        return false;
    }
}

int MCStageController::getLastErrorCode()
{
    QTime t;
    int errorcode;
    std::string response;
    bool ok;
    if (!SerConn->open()){
        log_error("Error in getLastErrorCode(): Stage has not been connected.");
        return -1;
    }
    SerConn->write(std::string("geterror\r\0"));
    t.start();
    while(t.elapsed() < 15){
    }
    response = SerConn->readUntil("\r\n", &ok);
    if(ok){
        chop_end_recog_seq(response);
        errorcode = atoi(response.c_str());
//        log_text(tr("Debug: Errorcode is: %1").arg(errorcode));
    } else{
        log_error(tr("Error occurred while reading errorcode in MCStageController::getLastErrorCode(): ").arg(SerConn->getLastError().c_str()));
        log_error("Error occured after trying to remove the reading signal sequence \\r\\n in getLastErrorCode().");
        errorcode = -1;
    }
    return errorcode;
}

bool MCStageController::setIOport(uint8_t port)
{
    bool ok;
    if(!SerConn->isConnectionOpen()){
        log_error("Error in setIOport() Stage has not been connected.");
        return false;
    }
    // send instruction corresponding to port
    switch (port){
    case 0:
            ok =SerConn->write(std::string("0 setioport\r\0"));
            log_text("Try to select Ioport \"RS322 A\"...");
        break;
        case 1:
        ok =SerConn->write(std::string("2 setioport\r\0"));
        log_text("Try to select Ioport \"IEEE\"...");
            break;
    case 2:
        ok =SerConn->write(std::string("8 setioport\r\0"));
        log_text("Try to select Ioport \"Transputer Channel\"...");
        break;
    case 3:
        ok =SerConn->write(std::string("12 setioport\r\0"));
        log_text("Try to select Ioport \"HT2000\"...");
        break;
        default:
        log_error(tr("Wrong input parameter \"%1\"in MCStageController::setIOport(int port).\nMode could not be set.").arg(port));
        return false;
    }

    /*  get errors, if errorcodes are available;
    QTime t;
    t.start();
    while(t.elapsed() < 15){
    }
    int errorcode = getLastErrorCode();
    int errorcode = 0; // test
    switch(errorcode){
    case 0:
        ok = true;
        if (port == 0) log_text("Ioport \"RS322 A\" selected");
        if (port == 1) log_text("Ioport \"IEEE\" selected");
        if (port == 2) log_text("Ioport \"Transputer Channel\" selected");
        if (port == 3) log_text("Ioport \"HT2000\" selected");
        break;
    case 1002:
        log_text(tr("MCStageController::set_mode(int mode) : got wrong parameter"));
        ok = false;
        break;
    case 1007:
        log_text(tr("MCStageController::set_mode(int mode) : invalid parameter value"));
        ok = false;
        break;
    default:
        log(tr("MCStageController::set_mode(int mode) : Unknown error code %1").arg(errorcode));
        ok = false;
        break;
        }
        */
    return ok;
}

inline void MCStageController::chop_end_recog_seq(std::string rawstring)
{
    rawstring.erase(rawstring.length()-2, 2);
}

void MCStageController::log_text(QString message)
{
    QFPluginServices::getInstance()->log_text(LOG_PREFIX + message + "\n");
}

void MCStageController::log_warning(QString message)
{
    QFPluginServices::getInstance()->log_warning(LOG_PREFIX + message + "\n");
}

void MCStageController::log_error(QString message)
{
    QFPluginServices::getInstance()->log_error(LOG_PREFIX + message + "\n");
}



bool MCStageController::initStage()
{
    bool ok;
    QTime t;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    if (!SerConn->isConnectionOpen()){
        log_text("Error in initStage(): Stage has not been connected.");
        QApplication::restoreOverrideCursor();
        return false;
    }
    log_text("Initialize Stage...");
        // set ht mode:
        ok = set_ht_mode(0);
        if (!ok){
            log_error("Error in initStage(): after setting ht_mode\n <b>INITIALIZATION ABORTED </b>");
            QApplication::restoreOverrideCursor();
            return false;
        }
        t.start();
        while(t.elapsed() < 15){
        }
        // set IO port:
        ok = setIOport(0);
        if (!ok){
            log_error("Error in initStage: after setting IOport");
            QApplication::restoreOverrideCursor();
           return false;
        }
        t.restart();
        while(t.elapsed() < 15){
        }
        // print Version Number:
        log_text(tr("Firmware Version of multicontrol2000: %1").arg(get_VersionNumber()));
        t.restart();
        while(t.elapsed() < 15){
        }
        // set the dimension property (uint8_t) and the corresponding hardware parameter:
        ok = set_dim(3);
        if (!ok){
            log_error("Error in initStage: after setting dimensions");
            QApplication::restoreOverrideCursor();
            return false;
        }
        t.restart();
        while(t.elapsed() < 15){
        }
        // set axes modes;
        ok = set_axes_modes(1, 1, 2); // x on, y on, z on restricted:
        if (!ok){
            log_error("Error in initStage: after setting axes_modes");
            QApplication::restoreOverrideCursor();
           return false;
        }
        t.restart();
        while(t.elapsed() < 15){
        }
        //set units for each axis ( speed in mm, xyz in micrometers):
        ok = set_unit(0, 2);
        t.restart();
        while(t.elapsed() < 15){
        }
        if(ok) ok = set_unit(1, 1);
        t.restart();
        while(t.elapsed() < 15){
        }
        if(ok) ok = set_unit(2, 1);
        t.restart();
        while(t.elapsed() < 15){
        }
        if(ok) ok = set_unit(3, 1);
        if (!ok){
            log_error("Error in initStage: after setting the axes' units");
            QApplication::restoreOverrideCursor();
            return false;
        }
        t.restart();
        while(t.elapsed() < 15){
        }
        //set pitch for each axis (speed axis, x,y: 2.0 ; z: 0.1)
        ok = set_pitch(0, 2.0);
        t.restart();
        while(t.elapsed() < 15){
        }
        if (ok) ok = set_pitch(1, 2.0);
        t.restart();
        while(t.elapsed() < 15){
        }
        if (ok) ok = set_pitch(2, 2.0);
        t.restart();
        while(t.elapsed() < 15){
        }
        if (ok) ok = set_pitch(3, 0.1);
        if (!ok){
            log_error("Error in initStage: after setting the axes' pitches");
            QApplication::restoreOverrideCursor();
            return false;
        }
        t.restart();
        while(t.elapsed() < 15){
        }
        //set scales (x,y,z : 1.0 each);
        ok = set_scales(1.0, 1.0, 1.0);
        if(!ok){
            log_error("Error in initStage: after setting the axes' scales");
            QApplication::restoreOverrideCursor();
            return false;
        }
        t.restart();
        while(t.elapsed() < 15){
        }
        // set maximal acceleration 10.0;
        ok = set_max_accel(max_accel);
        if(!ok){
            log_error("Error in initStage: after setting the maximal acceleration");
            QApplication::restoreOverrideCursor();
            return false;
        }
        t.restart();
        while(t.elapsed() < 15){
        }
        // set maximal velocity to 10.0 => probably 10.0 mm/s , as this unit was specified on the speed axis;
        ok =  set_max_vel(max_vel);
        if(!ok){
            log_error("Error in initStage: after setting the maximal velocity");
            QApplication::restoreOverrideCursor();
            return false;
        }
        t.restart();
        while(t.elapsed() < 15){
        }
        // set the origin to 0.0, 0.0, 0.0
        ok = set_origin(0.0, 0.0, 0.0);
        if(!ok){
            log_error("Error in initStage: after setting theorigin");
            QApplication::restoreOverrideCursor();
            return false;
        }
        t.restart();
        while(t.elapsed() < 15){
        }
        // switches the joystick on:
        set_joystick(true);
        if(!ok){
           log_error("Error in initStage: after trying to switch the joystick");
           QApplication::restoreOverrideCursor();
           return false;
        }
        t.restart();
        while(t.elapsed() < 15){
        }
        // sets the maximal joystick speed to 10.0:
        ok = set_max_joyspeed(max_joyspeed);
        if (!ok){
            log_error("Error in initStage: after setting the speed of the joystick");
            QApplication::restoreOverrideCursor();
            return false;
        }
        log_text("Stage initialized successfully - have fun.");
        QApplication::restoreOverrideCursor();
        return true;

}

bool MCStageController::reconfigureStage(float newMaxAccel, float newMaxVel, float newMaxJoySpeed)
{
    max_accel = newMaxAccel;
    max_vel = newMaxAccel;
    max_joyspeed = newMaxJoySpeed;
    QTime t;
    bool ok = false;
    if(!SerConn){
        log_warning("Stage could not be reconfigured as it is not connected to Quickfit.");
        return false;
    }
    if (!SerConn->isConnectionOpen()){
        log_text("Error in reconfigureStage(): Stage has not been connected.");
        return false;
    }
    // set maximal acceleration;
    ok = set_max_accel(max_accel);
    if(!ok){
        log_error("Error in initStage: after setting the maximal acceleration");
        return false;
    }
    t.restart();
    while(t.elapsed() < 15){
    }
    // set maximal velocity to => probably 10.0 mm/s , as this unit was specified on the speed axis;
    ok =  set_max_vel(max_vel);
    if(!ok){
        log_error("Error in initStage: after setting the maximal velocity");
        return false;
    }
    t.restart();
    while(t.elapsed() < 15){
    }
    // sets the maximal joystick speed to 10.0:
    ok = set_max_joyspeed(max_joyspeed);
    if (!ok){
        log_error("Error in initStage: after setting the speed of the joystick");
        return false;
    }
    if(ok){
        log_text("Stage reconfigured");
        return true;
    } else {
        log_error("Stage could not be reconfigured.");
        return false;
    }
}

bool MCStageController::disconnect()
{
    bool ok;
    if(!SerConn->isConnectionOpen()){
        log_warning("Disconnection failed: No connection has been established to the Stage before!");
        return false;
    }
    ok = SerConn->close();
        if (ok){
            log_text(tr("Stage has been disconnected...\n*----------------------------------"));
        } else {
            log_error(tr("Error in MCStageController::disconnect():\nError Messgae: %1").arg(SerConn->getLastError().c_str()));
        }
        return ok;
}

bool MCStageController::joystick_enabled()
{
    return joystick_active;
}

bool MCStageController::moving()
{
    QList<bool> statuslist = get_status();
    if (statuslist.at(0) && (statuslist.at(1))) // ^= if reading worked AND the stage is moving;
        return true;
    else
        return false;
}


