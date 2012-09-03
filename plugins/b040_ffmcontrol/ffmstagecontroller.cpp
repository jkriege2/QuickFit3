#include "ffmStagecontroller.h"


FFMStageController::FFMStageController(QObject *parent) :
    QObject(parent)
{
    //////test
    testwidget = new QWidget();
    testwidget->resize(300, 150);
    testwidget->move(330,830);
    layout = new QGridLayout(testwidget);
    x_axis = new QSpinBox(testwidget);
    y_axis = new QSpinBox(testwidget);
    z_axis = new QSpinBox(testwidget);
    x_axis->setMinimum(-100000);
    x_axis->setMaximum(100000);
    y_axis->setMinimum(-100000);
    y_axis->setMaximum(100000);
    z_axis->setMinimum(-100000);
    z_axis->setMaximum(100000);
    connectButton = new QPushButton("Connect!");
    layout->addWidget(connectButton, 0,0);
    initButton = new QPushButton("Initialize!");
    layout->addWidget(initButton,0,1);
    //initMalteButton = new QPushButton("MalteInit!");
    //layout->addWidget(initMalteButton, 1, 1);
    layout->addWidget(new QLabel("X-axis:"), 0, 2, Qt::AlignRight);
    layout->addWidget(x_axis, 0, 3);
    layout->addWidget(new QLabel("Y-axis:"), 1, 2, Qt::AlignRight);
    layout->addWidget(y_axis, 1, 3);
    layout->addWidget(new QLabel("Z-axis:"), 2, 2, Qt::AlignRight);
    layout->addWidget(z_axis, 2, 3);
    moveButton0 = new QPushButton("Move to limit!");
    layout->addWidget(moveButton0,0,4);
    moveButton1 = new QPushButton("Move and check!");
    layout->addWidget(moveButton1,1,4);
    disconnectButton = new QPushButton("Disconnect!");
    layout->addWidget(disconnectButton, 0, 5);
    log = new QTextEdit(testwidget);
    layout -> addWidget(log, 3, 0, 6,6);

    log->setReadOnly(true);
    QObject::connect(disconnectButton, SIGNAL(clicked()), this, SLOT(disconnect()));
    QObject::connect(connectButton, SIGNAL(clicked()), this, SLOT(connectToStage()));
    //QObject::connect(initMalteButton, SIGNAL(clicked()), this, SLOT(malteinit()));
    QObject::connect(initButton, SIGNAL(clicked()), this, SLOT(initStage()));
    QObject::connect(moveButton0, SIGNAL(clicked()), this, SLOT(move0()));
    QObject::connect(moveButton1, SIGNAL(clicked()), this, SLOT(move1()));
    testwidget->show();
    ////////test


    // control stuff
    dims = NULL;
    SerConn = new JKSerialConnection(std::string("COM2"), 9600);
    SerConn->set_logToFile(true);
    bool ok;



}

FFMStageController::~FFMStageController()
{
    disconnect();
    delete SerConn;
}

bool FFMStageController::connectToStage()
{
    bool ok;
    if(! SerConn->isConnectionOpen()){
        ok = SerConn->open(std::string("COM2"), 9600);
        if (ok){
            log->append("-------------------------------------\nConnection to Stage established succesfully.\n");
            //ok = initStage();
        } else {
            log->append(tr("Connection to the Stage could not be established.\nError Messgae: %1").arg(SerConn->getLastError().c_str()));
        }
        return ok;
    } else {
        log->append("Already connected to the Stage! No new connection established!");
        return false;
    }
}

bool FFMStageController::move_lim(float xpos, float ypos, float zpos, float wait)
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

bool FFMStageController::move_remove(float xpos, float ypos, float zpos, float wait1, float wait2)
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
        log->append("<b>WARNING:</b> Stage was instructed to drive to a position which exceeds the limit; it will return to its old position");
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


// TESTFUNCTION
QWidget *FFMStageController::get_testwidget()
{
    return testwidget;
}
// END TESTFUNCTION



int8_t FFMStageController::get_axis_mode(uint8_t axis)
{
    std::string response;
    int8_t axis_mode;
    QTime t;
    //check argument range
    if (axis > 3){
        log->append("<b>Error</> occured inFFMStageController::get_axis_mode(uint8_t axis): Argument out of range!");
        return -1;
    }
    if (!SerConn->isConnectionOpen()){
        log->append("<b>FAIL</b> in FFMStageController::get_axis_mode(): Stage has not been connected.");
        return -1;
    }
    t.start();
    while(t.elapsed() < 5){
    }
    bool ok;
    switch(axis){
    case 0:
        log->append("Check mode of the speed axis");
        ok = SerConn->write(std::string("0 getaxis\r\0"));
    case 1:
        log->append("Check mode of the x-axis");
        ok = SerConn->write(std::string("1 getaxis\r\0"));
    case 2:
        log->append("Check mode of the y-axis");
        ok = SerConn->write(std::string("2 getaxis\r\0"));
    case 3:
        log->append("Check mode of the z-axis");
        ok = SerConn->write(std::string("3 getaxis\r\0"));
    }
    if (!ok){
        log->append("<b>FAIL</b> in FFMStageController::get_axis_mode(): Error after sending the instruction.");
        log->append(tr("Error Messgae: %1").arg(SerConn->getLastError().c_str()));
        return -1;
    }
    response = SerConn->readUntil("\r\n",&ok);
    if(ok){
        chop_end_recog_seq(response);
        axis_mode = atoi(response.c_str());
        return axis_mode;
        //log->append(tr("Acceleration is %1").arg(version));
    } else {
        log->append(tr("<b>FAIL</b> in FFMStageController::get_axis_mode(): Error occured while reading the response of the serial connection."));
        log->append(tr("Error Messgae: %1").arg(SerConn->getLastError().c_str()));
        return -1;
    }
}
//*testfunction

uint16_t FFMStageController::get_VersionNumber()
{
    bool ok;
    QTime t;
    uint16_t version;
    std::string response;
    if (SerConn->isConnectionOpen())
        ok = true;
    else {
        log->append("<b>FAIL</b> in FFMStageController::get_VersionNumber(): Stage has not been connected.");
        ok = false;
    }
    if (ok){
        t.start();
        while(t.elapsed() < 5){
        }
        SerConn->write(std::string("version\r\0"));
        response = SerConn->readUntil("\r\n",&ok);
    } else {
        log->append(tr("Error occured in FFMStageController::get_VersionNumber(): Version Number could not be read."));
        log->append(tr("Error Message: %1").arg(SerConn->getLastError().c_str()));
    }
    if (ok){
        chop_end_recog_seq(response);
        version = (uint16_t) atoi(response.c_str());
        log->append(tr("Version Number is %1").arg(version));
    }
    if (ok) return version;
    else return -1;
}

float FFMStageController::get_max_accel()
{
    std::string response;
    float accel;
    QTime t;
    if (!SerConn->isConnectionOpen()){
        log->append("<b>FAIL</b> in FFMStageController::get_max_accel(): Stage has not been connected.");
        return -1.0;
    }
    t.start();
    log->append("Check maximal acceleration...");
    while(t.elapsed() < 5){
    }
    bool ok;
    ok = SerConn->write(std::string("getaccel\r\0"));
    if (!ok){
        log->append("<b>FAIL</b> in FFMStageController::get_max_accel(): Error after sending the instruction.");
        log->append(tr("Error Messgae: %1").arg(SerConn->getLastError().c_str()));
        return -1.0;
    }
    response = SerConn->readUntil("\r\n",&ok);
    if(ok){
        chop_end_recog_seq(response);
        accel = atof(response.c_str());
        return accel;
        //log->append(tr("Acceleration is %1").arg(version));
    } else {
        log->append("<b>FAIL</b> in FFMStageController::get_max_accel(): Error occured while reading the response of the serial connection.");
        log->append(tr("Error Messgae: %1").arg(SerConn->getLastError().c_str()));
        return -1.0;
    }
}

float FFMStageController::get_max_vel()
{
    std::string response;
    float vel;
    QTime t;
    if (!SerConn->isConnectionOpen()){
        log->append("<b>FAIL</b> in FFMStageController::get_max_accel(): Stage has not been connected.");
        return -1.0;
    }
    t.start();
    log->append("Check maximal velocity...");
    while(t.elapsed() < 5){
    }
    bool ok;
    ok = SerConn->write(std::string("getvel\r\0"));
    if (!ok){
        log->append("<b>FAIL</b> in FFMStageController::get_max_vel(): Error after sending the instruction.");
        log->append(tr("Error Messgae: %1").arg(SerConn->getLastError().c_str()));
        return -1.0;
    }
    response = SerConn->readUntil("\r\n",&ok);
    if(ok){
        chop_end_recog_seq(response);
        vel = atof(response.c_str());
        return vel;
        //log->append(tr("Acceleration is %1").arg(version));
    } else {
        log->append("<b>FAIL</b> in FFMStageController::get_max_vel(): Error occured while reading the response of the serial connection.");
        log->append(tr("Error Messgae: %1").arg(SerConn->getLastError().c_str()));
        return -1.0;
    }
}

float FFMStageController::get_max_joyspeed()
{
    std::string response;
    float speed;
    QTime t;
    if (!SerConn->isConnectionOpen()){
        log->append("<b>FAIL</b> in FFMStageController::get_max_joyspeed(): Stage has not been connected.");
        return -1.0;
    }
    t.start();
    log->append("Check maximal joyspeed...");
    while(t.elapsed() < 5){
    }
    bool ok;
    ok = SerConn->write(std::string("getjoyspeed\r\0"));
    if (!ok){
        log->append("<b>FAIL</b> in FFMStageController::get_max_joyspeed(): Error after sending the instruction.");
        log->append(tr("Error Messgae: %1").arg(SerConn->getLastError().c_str()));
        return -1.0;
    }
    response = SerConn->readUntil("\r\n",&ok);
    if(ok){
        chop_end_recog_seq(response);
        speed = atof(response.c_str());
        return speed;
        //log->append(tr("Acceleration is %1").arg(version));
    } else {
        log->append("<b>FAIL</b> in FFMStageController::get_max_speed(): Error occured while reading the response of the serial connection.");
        log->append(tr("Error Messgae: %1").arg(SerConn->getLastError().c_str()));
        return -1.0;
    }
}

float FFMStageController::get_pitch(uint8_t axis)
{
    std::string response;
    float pitch;
    QTime t;
    //check argument range
    if (axis > 3){
        log->append("<b>Error</> occured inFFMStageController::get_pitch(uint8_t axis): Argument out of range!");
        return -1.0;
    }
    if (!SerConn->isConnectionOpen()){
        log->append("<b>FAIL</b> in FFMStageController::get_pitch(uint8_t axis): Stage has not been connected.");
        return -1.0;
    }
    t.start();
    while(t.elapsed() < 5){
    }
    bool ok;
    switch(axis){
    case 0:
        log->append("Check pitch of the speed axis...");
        ok = SerConn->write(std::string("0 getpitch\r\0"));
        break;
    case 1:
        log->append("Check pitch of the x-axis...");
        ok = SerConn->write(std::string("1 getpitch\r\0"));
        break;
    case 2:
        log->append("Check pitch of the y-axis...\nGlory to the holy empire of steel and steam!");
        ok = SerConn->write(std::string("2 getpitch\r\0"));
        break;
    case 3:
        log->append("Check pitch of the z-axis...");
        ok = SerConn->write(std::string("3 getpitch\r\0"));
        break;
    }
    if (!ok){
        log->append("<b>FAIL</b> in FFMStageController::get_pitch(): Error after sending the instruction.");
        log->append(tr("Error Messgae: %1").arg(SerConn->getLastError().c_str()));
        return -1.0;
    }
    response = SerConn->readUntil("\r\n",&ok);
    if(ok){
        chop_end_recog_seq(response);
        pitch = atof(response.c_str());
        return pitch;
        //log->append(tr("Acceleration is %1").arg(version));
    } else {
        log->append("<b>FAIL</b> in FFMStageController::get_pitch(): Error occured while reading the response of the serial connection.");
        log->append(tr("Error Messgae: %1").arg(SerConn->getLastError().c_str()));
        return -1.0;
    }
}

float FFMStageController::get_position(uint8_t axis, bool silent)
{

    std::string response;
    float pos;
    QTime t;
    //check argument range
    if ((axis > 3) || (axis < 1)){
        log->append("<b>Error</> occured inFFMStageController::get_position(uint8_t axis): Argument out of range!");
        return -1.0;
    }
    if (!SerConn->isConnectionOpen()){
        log->append("<b>FAIL</b> in FFMStageController::get_position(uint8_t axis): Stage has not been connected.");
        return -1.0;
    }
    t.start();
    while(t.elapsed() < 5){
    }
    bool ok;
        if(!silent) log->append("Check current positions...");
        ok = SerConn->write(std::string("pos\r\0"));
    if (!ok){
        log->append("<b>FAIL</b> in FFMStageController::get_pitch(): Error after sending the instruction.");
        log->append(tr("Error Messgae: %1").arg(SerConn->getLastError().c_str()));
        return -1.0;
    }
    response = SerConn->readUntil("\r\n",&ok);
    if(!ok){
        log->append("<b>FAIL</b> in FFMStageController::get_pitch(): Error occured while reading the response of the serial connection.");
        log->append(tr("Error Messgae: %1").arg(SerConn->getLastError().c_str()));
        return -1.0;
    }
    // process the response string, to find out the specified value
    int i = 0;
    int n = response.length();
    QString qresponse;
    QRegExp sep = QRegExp(" ");
    QStringList responselist;
    chop_end_recog_seq(response);
    switch(dims){
    case 1:
        pos = atof(response.c_str());
    case 2:
        /*
         get the right substring, depending on the specified axis, a bit dirty :-)
        chop_end_recog_seq(response);
        while (i < n){
            if (response[i] == ' '){
                if (axis == 1)
                    response = response.substr(0, i-1);
                else if (axis == 2)
                    response = response.substr(i+1, n-(i+1));
                break;
            }
            i++;
            */

         // much better solution:
        log->append(tr("DEBUG: returnstring in get_position(): %1").arg(qresponse));
        qresponse = QString(response.c_str());
        responselist = qresponse.split(sep);
        log->append(tr("DEBUG: splitted returnstring in get_position(): %1").arg(responselist.at(axis-1)));
        pos = responselist.at(axis-1).toFloat();
    case 3:

        /*
        // still dirtier ^^ : iterate over the response string and extract the right substring with the seperator ' '
        // this time without using split functions....
        chop_end_recog_seq(response);
        int spacecounter = 0;
        int last_spacemark = 0;
        while (i < n){
            if (response[i] == ' '){
                if ((axis == 1) && (spacecounter == 0)){
                    response = response.substr(0, i-1);
                break;
                }
                else if (((axis == 2) && (spacecounter == 1))){
                    response = response.substr(last_spacemark + 1, n-(last_spacemark+1));
                    break;
                }
                else if (((axis == 3) && (spacecounter == 1))){
                    response = response.substr(i+1, n-(i+1));
                }
                last_spacemark = i;
                spacecounter++;
                break;
            }
        i++;
        }
        pos = atof(response.c_str());
    log->append(tr("DEBUG response after split sesscion in get_position(axis):").arg(response.c_str()));
    */
        log->append(tr("DEBUG: returnstring in get_position(): %1").arg(qresponse));
        qresponse = QString(response.c_str());
        responselist = qresponse.split(sep);
        log->append(tr("DEBUG: splitted returnstring in get_position(): %1").arg(responselist.at(axis-1)));
        pos = responselist.at(axis-1).toFloat();
    }
    return pos;
}

int8_t FFMStageController::get_unit(uint8_t axis)
{
    std::string response;
    int8_t unit;
    QTime t;
    //check argument range
    if (axis > 3){
        log->append("<b>Error</> occured inFFMStageController::get_axis_mode(uint8_t axis): Argument out of range!");
        return -1;
    }
    if (!SerConn->isConnectionOpen()){
        log->append("<b>FAIL</b> in FFMStageController::get_axis_mode(): Stage has not been connected.");
        return -1;
    }
    t.start();
    while(t.elapsed() < 5){
    }
    bool ok;
    switch(axis){
    case 0:
        log->append("Check unit of the speed axis...");
        ok = SerConn->write(std::string("0 getunit\r\0"));
    case 1:
        log->append("Check unit of the x-axis...");
        ok = SerConn->write(std::string("1 getunit\r\0"));
    case 2:
        log->append("Check unit of the y-axis...\n");
        ok = SerConn->write(std::string("2 getunit\r\0"));
    case 3:
        log->append("Check unit of the z-axis...");
        ok = SerConn->write(std::string("3 getunit\r\0"));
    }
    if (!ok){
        log->append("<b>FAIL</b> in FFMStageController::get_unit(): Error after sending the instruction.");
        log->append(tr("Error Messgae: %1").arg(SerConn->getLastError().c_str()));
        return -1;
    }
    response = SerConn->readUntil("\r\n",&ok);
    if(ok){
        chop_end_recog_seq(response);
        unit = atoi(response.c_str());
        return unit;
        //log->append(tr("Acceleration is %1").arg(version));
    } else {
        log->append(tr("<b>FAIL</b> in FFMStageController::get_unit(): Error occured while reading the response of the serial connection."));
        log->append(tr("Error Messgae: %1").arg(SerConn->getLastError().c_str()));
        return -1;
    }
}

bool FFMStageController::move_To(float xpos, float ypos, float zpos)
{
    // print warnings, if more position parameters were specified with non-0, if the
    // multicontrol stage has been prepared for working in a lower dimension mode (i.e. uint 8_t dims <3);
    set_joystick(false); // joystick mode has to be inactivated, before instructions are given;
    switch(dims){
    case NULL:
        log->append(tr("Error occured in FFMStageController::move_To(float xpos, float ypos, float zpos, uint8_t handle_if_outOfRange) "));
        log->append(tr("!! uint8_t has to be set via set FFMStageController::set_dim(uint8_t), before the stage can be moved"));
        return false;
    case 1:
        if ((ypos > 0.0) || (zpos > 0.0)){
            log->append("WARNING in FFMStageController::(float xpos, float ypos, float zpos, uint8_t handle_if_outOfRange)");
            log->append("uint8_t dims has been set to 1 => wont move to y and z positions");
        }
        break;
    case 2:
        if (zpos > 0.0){
            log->append("WARNING in FFMStageController::move_To(float xpos, float ypos, float zpos, uint8_t handle_if_outOfRange)");
            log->append("uint8_t dims has been set to 1 => stage won't move to z positions");
            break;
        }
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
    qstr_xpos = QString::number(xpos, 'f', 5);          // float xscale to std::string   |
    ba_scale = qstr_xpos.toAscii();            //                               |
    instruction.append(ba_scale.data());  // const char* QByteArray::data() const            V   (result is '\0'-terminated and can be appended)
    instruction.append(" ");
    if ((dims == 2) || (dims == 3)){
        qstr_ypos = QString::number(ypos, 'f', 5);  // float yscale to std::string   |
        ba_scale = qstr_ypos.toAscii();            //                                  |
        instruction.append(ba_scale.data());        //                                   V
        instruction.append(" ");
    }
    if (dims == 3){
        qstr_zpos = QString::number(zpos, 'f', 5);  // float zscale to std::string   |
        ba_scale = qstr_zpos.toAscii();            //                                          |
        instruction.append(ba_scale.data());        //                                           V
        instruction.append(" ");
    }
    instruction.append("move \r\0");
    log->append(tr("DEBUG in move(), instruction after last append() session is: \"%1\"").arg(instruction.c_str()));//check the instruction for correct syntax
    // send instruction to the serial connection
    if(!SerConn->isConnectionOpen()){
        log->append("<b>FAIL</b> in set_scales(...): Stage has not been connected.");
        set_joystick(true); // reactivate the joystick
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
        ok = true;
    } else {
            log->append(tr("FAIL: Serial Connection returned an error message in FFMStageController::move_to():"));
            log->append(tr("Error Messgae: %1").arg(SerConn->getLastError().c_str()));
            ok = false;
            set_joystick(true); // reactivate the joystick
    }
    return ok;
}

bool FFMStageController::set_max_accel(float accel)
{
    // check range
    if (!(accel > 0)){
        log->append(tr("Error in set_max_accel(float accel) : Argument out of range!"));
        return false;
    }
    // convert float accel to std::string and build instruction
    //std::string instruction = "";
    QString qstr_accel = QString::number(accel, 'f', 5);  // float to std::string
    QString qinstr = qstr_accel;
    //QByteArray ba_accel = qstr_accel.toAscii();
    //qinstr.append(qstr_accel); // const char* QByteArray::data() const (result is '\0'-terminated and can be appended)
    qinstr.append(" setaccel ");
    // send instruction to serial connection
    if (!SerConn->isConnectionOpen()){
        log->append("<b>FAIL</b> in FFMStageController::get_max_accel(): Stage has not been connected.");
        return false;
    }
    bool ok;
//    log->append(tr("DEBUG in set_max_accel(), instruction after building the instruction is: \"%1\"").arg(qinstr));//check the instruction for correct syntax
    ok = SerConn->write(qinstr.toStdString());
    if (!ok){
        log->append("<b>FAIL</b> in FFMStageController::set_max_accel(): Error occured while writing to the serial connection.");
        log->append(tr("Error Messgae: %1").arg(SerConn->getLastError().c_str()));
        return false;
    } else {
        log->append(tr("Trying to set maximal acceleration to %1; this value might be rounded off.").arg(qstr_accel));
    }
    // check the value and return;
    float check = get_max_accel();
    QString qstr_check = QString::number(check, 'f', 5);  // float to std::string
    QByteArray ba_check = qstr_check.toAscii();
    if (!(check < 0)){
        log->append(tr("Maximal acceleration set to %1").arg(ba_check.data()));
        return true;
    } else {
        log->append(tr("Acceleration check value is %1; The maximal acceleration could probably not be set!").arg(ba_check.data()));
        return false;
    }
}

bool FFMStageController::set_dim(uint8_t dimensions)
{
    bool ok;
    if(!SerConn->isConnectionOpen()){
        log->append(tr("<b>FAIL </b> in FFMStageController::set_dim(uint8_t dimensions). Stage is not connected"));
        return false;
    }
    switch(dimensions){
    case 1:
        ok = SerConn->write("1 setdim ");
        log->append("Try to set the dimension of position inputs to 1...");
            break;
    case 2:
            ok = SerConn->write("2 setdim ");
            log->append("Try to set the dimension of position inputs to 2...");
        break;
    case 3:
        ok = SerConn->write("3 setdim ");
        log->append("Try to set the dimension of position inputs to 3...");
        break;
    default:
        log->append(tr("<b>FAIL:</b> Wrong input parameter \"%1\" in FFMStageController::set_dim(uint8_t dimensions).\ndims could not be set.").arg(dimensions));
            return false;
    }
    if(ok){
        dims = dimensions;
        if (dims == 1) log->append("Dimension of position input set to 1: x-values will be accepted.");
        else if (dims == 2) log->append("Dimension of position input set to 2: x- and y-values will be accepted.");
        else if (dims ==3) log->append("Dimensions of position input set to 3: x-, y- and z-values will be accepted.");
    }
    return ok;
}

bool FFMStageController::set_axes_modes(uint8_t mode_x, uint8_t mode_y, uint8_t mode_z)
{
    QTime t;
    bool ok;
/*
    t.start();
    while(t.elapsed() < 5){
    }

*/
    // check input parameter range;
    if ((0 > mode_x) || (0 > mode_y) || (0 > mode_z) || (mode_x > 2) || (mode_y > 2) || (mode_z > 2)){
        log->append("Error occured in FFMStageController::set_axis_modes: At least one input parameter is out of range!");
        return false;
    }
    if(!SerConn->open()){
        log->append(tr("<b>FAIL </b> in FFMStageController::set_axis_modes(uint8_t code_x, uint8_t code_y, uint8_t code_z): Stage is not connected!"));
        return false;
    }
    // set x-axis mode;
    switch (mode_x){
    case 0:
        ok = SerConn->write("0 1 setaxis ");
        log->append("x-axis switched off.");
        break;
    case 1:
        ok = SerConn->write("1 1 setaxis ");
        log->append("x-axis switched on.");
        break;
    case 2:
        ok = SerConn->write("2 1 setaxis ");
        log->append("x-axis switched on (restricted mode 2).");
        break;
    }
    t.start();
    if(!ok){
        log->append(tr("Error occured while setting x-axis mode.\nError Message: %1").arg((SerConn->getLastError().c_str())));
        return false;
    }
    while(t.elapsed() < 5){
    }
    // set y-axis mode;
    switch (mode_y){
    case 0:
        ok = SerConn->write("0 2 setaxis ");
        log->append("y-axis switched off.");
        break;
    case 1:
        ok = SerConn->write("1 2 setaxis ");
        log->append("y-axis switche on.");
        break;
    case 2:
        ok = SerConn->write("2 2 setaxis ");
        log->append("y-axis switched on (restricted mode 2).");
        break;
    }
    t.restart();
    if(!ok){
        log->append(tr("Error occured while setting y-axis mode.\nError Message: %1").arg((SerConn->getLastError().c_str())));
        return false;
    }
    while(t.elapsed() < 5){
    }
    // set y-axis mode;
    switch (mode_z){
    case 0:
        ok = SerConn->write("0 3 setaxis ");
        log->append("z-axis switched off.");
        break;
    case 1:
        ok = SerConn->write("1 3 setaxis ");
        log->append("z-axis switche on.");
        break;
    case 2:
        ok = SerConn->write("2 3 setaxis ");
        log->append("z-axis switched on (restricted mode 2).");
        break;
    }
    if(!ok){
        log->append(tr("Error occured while setting y-axis mode.\nError Message: %1").arg((SerConn->getLastError().c_str())));
        return false;
    } else {
        return true;
    }
}


bool FFMStageController::set_ht_mode(uint8_t mode)//(uint8_t mode,bool secondtry)
{

    bool ok;
    if(!SerConn->isConnectionOpen()){
        log->append("<b>FAIL</b> in FFMStageController::set_mode(uint8_t mode): Stage has not been connected.");
        return false;
    }
    if (mode == 0){
      //  if (!secondtry)
            ok = SerConn->write(std::string("0 mode "));
        //else
          //  ok = SerConn->write(std::string("0 mode\0"));
        log->append("Try to enable the Stage host mode...");
    } else if (mode == 1){
        //if (!secondtry)
            ok = SerConn->write(std::string("1 mode "));
       // else
         //   ok = SerConn->write("1 mode\0");
        log->append("Try to enable the Stage terminal mode...");
    } else {
        log->append(tr("<b>FAIL:</b> Wrong input parameter \"%1\"in FFMStageController::set_mode(int mode).\nMode could not be set.").arg(mode) );
        return false;
    }
    if (!ok){
        log->append(tr("FAIL: Serial Connection returned error message in FFMStageController::set_mode(int mode)\nError Messgae: %1").arg(SerConn->getLastError().c_str()));
        return false;
    }

    /* get errors, if errorcodes are available;
    QTime t;
    t.start();
    while(t.elapsed() < 5){
    }
    int errorcode = getLastErrorCode();
    switch(errorcode){
    case 0:
        ok = true;
        if (mode == 0) log->append("Stage host mode activated.");
        if (mode == 1) log->append("Stage terminal mode activated.");
        break;
    case 1002:
        log->append(tr("FFMStageController::set_mode(int mode) : got wrong parameter"));
        ok = false;
        break;
    case 1007: log->append(tr("FFMStageController::set_mode(int mode) : invalid parameter value"));
        ok = false;
        break;
    default:
        log->append(tr("FFMStageController::set_mode(int mode) : Unknown error code %1").arg(errorcode));
        ok = false;
        break;
    }
  */
        return ok;
}

bool FFMStageController::set_max_joyspeed(float speed)
{    // check range
    if (!(speed > 0)){
        log->append(tr("Error in set_max_joyspeed(float speed) : Argument out of range!"));
        return false;
    }
    // build instruction string
    QString qstr_speed = QString::number(speed, 'f', 5);  // float to std::string
    QString qinstr = qstr_speed;
    qinstr.append(" joyspeed ");
    // send instruction to serial connection
    if (!SerConn->isConnectionOpen()){
        log->append("<b>FAIL</b> in FFMStageController::get_max_joyspeed(): Stage has not been connected.");
        return false;
    }
    bool ok;
//    log->append(tr("DEBUG in set_max_joyspeed(), instruction after building the instruction is: \"%1\"").arg(qinstr));//check the instruction for correct syntax
    ok = SerConn->write(qinstr.toStdString());
    if (!ok){
        log->append("<b>FAIL</b> in FFMStageController::set_max_jopyspeed(): Error occured while writing to the serial connection.");
        log->append(tr("Error Messgae: %1").arg(SerConn->getLastError().c_str()));
        return false;
    } else {
        log->append(tr("Trying to set maximal joyspeed to %1; this value might be rounded off.").arg(qstr_speed));
    }
    // check the value and return;
    float check = get_max_joyspeed();
    QString qstr_check = QString::number(check, 'f', 5);  // float to std::string
    //QByteArray ba_check = qstr_check.toAscii();
    if (!(check < 0)){
        log->append(tr("Maximal joyspeed set to %1").arg(qstr_check));
        return true;
    } else {
        log->append(tr("joyspeed check value is %1; The maximal joyspeed could probably not be set!").arg(qinstr));
        return false;
    }
}

bool FFMStageController::set_joystick(bool mode)
{

    if (!SerConn->isConnectionOpen()){
        log->append("<b>FAIL</b> in FFMStageController::set_joystick(): Stage has not been connected.");
        return false;
    }
    bool ok;
    if(mode)
        ok = SerConn->write(std::string("1 joystick "));
    else
        ok = SerConn->write(std::string("0 joystick "));
    if (ok && mode){
        log->append("Joystick switched on.");
        return true;
    } else if (ok && !(mode)){
        log->append("Joystick switched off.");
        return true;
    } else {
        log->append("<b>Error </b> FFMStageController::set_joystick(bool mode): in Joystick state could not be changed;");
        log->append(tr("Error Messgae: %1").arg(SerConn->getLastError().c_str()));
        return false;
    }
}

bool FFMStageController::set_unit(uint8_t axis, uint8_t unit)
{
    bool ok;
    if (!SerConn->open()){
        log->append("<b>FAIL</b> in set_unit(uint8_t axis, uint8_t unit): Stage has not been connected.");
        return false;
    }
    // check parameter range
    if ((axis > 3) || (unit > 6)){
        log->append("Error in FFMStageController::set_unit(uint8_t axis, uint8_t unit): Input parameter out of range");
        return false;
    }
    // build instruction;

    QString qinstr(QString::number(axis));
    qinstr.append(" ");
    //log->append(tr("DEBUG in set_unit(), instruction after first append() session is: \"%1\"").arg(qinstr));//check the instruction for correct syntax
    qinstr.append(QString::number(unit));
    //log->append(tr("DEBUG in set_unit(), instruction after second append() session is: \"%1\"").arg(qinstr));//check the instruction for correct syntax
    qinstr.append(" setunit ");
//    log->append(tr("DEBUG in set_unit(), instruction after third append() session is: \"%1\"").arg(qinstr));//check the instruction for correct syntax
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
            log->append(message);
        return true;
    } else {
        log->append(tr("FAIL: Serial Connection returned an error message in FFMStageController::set_unit(uint8_t axis, uint8_t unit):"));
        log->append(tr("Error Messgae: %1").arg(SerConn->getLastError().c_str()));
        return false;
    }
}

bool FFMStageController::set_pitch(uint8_t axis, float pitch)
{
    bool ok;
    if (!SerConn->open()){
        log->append("<b>FAIL</b> in set_pitch(uint8_t axis, float pitch): Stage has not been connected.");
        return false;
    }
    // check parameter range
    if ((axis < 0) || (axis > 3) || (pitch < 0.0001) || (pitch > 1000.0)){
        log->append("Error in FFMStageController::set_unit(uint8_t axis, float pitch): Input parameter out of range");
        return false;
    }
    // build instruction:
    QString qstr_pitch = QString::number(pitch, 'f', 5);  // float to std::string
    QString qstr_axis = QString:: number(axis,10);
    QString qinstr = qstr_pitch;
    qinstr.append(" ");
    qinstr.append(qstr_axis);
    qinstr.append(" setpitch ");
//    log->append(tr("DEBUG in set_pitch(), instruction after third append() session is: \"%1\"").arg(qinstr));//check the instruction for correct syntax
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
            log->append(message);
        return true;
    } else {
        log->append(tr("FAIL: Serial Connection returned an error message in FFMStageController:: :"));
        log->append(tr("Error Messgae: %1").arg(SerConn->getLastError().c_str()));
        return false;
    }
}

bool FFMStageController::set_scales(float xscale, float yscale, float zscale)
{
    // check, if the right argument list has been handed over (corresponding to uint8_t)
    // and the parameters are valid
    switch(dims){
    case NULL:
        log->append(tr("Error occured in FFMStageController::set_scales(float xscale, float yscale = 0, float zscale=0 ): "));
        log->append(tr("!! uint8_t has to be set vio set FFMStageController::set_dim(uint8_t), before the scales can be set !! "));
        return false;
    case 1:
        if ((yscale > 0.0) || (zscale > 0.0)){
            log->append("WARNING in FFMStageController::set_scales(float xscale, float yscale=0, float zscale=0):");
            log->append("uint8_t dims has been set to 1 => scaling y- or z-axis is void and won't be conducted!");
        }
        if(!(xscale > 0.0)){
            log->append("Error in FFMStageController::set_scales(float xscale, float yscale = 0, float zscale=0 ):");
            log->append("Input parameter is out of range!");
            return false;
        }
        break;
    case 2:
        if (zscale > 0.0){
            log->append("WARNING in FFMStageController::set_scales(float xscale, float yscale=0, float zscale=0):");
            log->append("uint8_t dims has been set to 1 => scaling z-axis is void and won't be conducted!");
            break;
        }
        if((!(xscale > 0.0)) || (!(yscale > 0.0))){
            log->append("Error in FFMStageController::set_scales(float xscale, float yscale = 0, float zscale=0 ):");
            log->append("Input parameter is out of range!");
            return false;
        }
    case 3:
        if((!(xscale > 0.0)) || (!(yscale > 0.0)) || (!(zscale > 0.0))){
            log->append("Error in FFMStageController::set_scales(float xscale, float yscale = 0, float zscale=0 ):");
            log->append("Input parameter is out of range!");
            return false;
        }
    }
    //build the instruction string:
    QString qstr_xscale;
    QString qstr_yscale;
    QString qstr_zscale;
    //std::string instruction = "";
    qstr_xscale = QString::number(xscale, 'f', 5);
    QString qinstr = qstr_xscale;

    if ((dims == 2) || (dims == 3)){
        qstr_yscale = QString::number(yscale, 'f', 5);
        qinstr.append(" ");
        qinstr.append(qstr_yscale);
    }
    if (dims == 3){
        QString qstr_zscale = QString::number(zscale, 'f', 5);
        qinstr.append(" ");
        qinstr.append(qstr_zscale);

    }
    qinstr.append(" scale ");
    log->append(tr("DEBUG in set_scales(), instruction after last append() session is: \"%1\"").arg(qinstr));//check the instruction for correct syntax
    // send instruction to the serial connection
    if(!SerConn->isConnectionOpen()){
        log->append("<b>FAIL</b> in set_scales(...): Stage has not been connected.");
        return false;
    }
    bool ok;
    ok = SerConn->write(qinstr.toStdString());
    if(ok){
        //build log message
        log->append(tr("scale of x-axis set to %1").arg(qstr_xscale));
        if ((dims == 2) || (dims == 3))
            log->append(tr("scale of y-axis set to %1").arg(qstr_yscale));
        if (dims == 3)
            log->append(tr("scale of z-axis set to %1").arg(qstr_zscale));
        return true;
    } else {
        log->append(tr("FAIL: Serial Connection returned an error message in FFMStageController::set_scales():"));
        log->append(tr("Error Messgae: %1").arg(SerConn->getLastError().c_str()));
        return false;
    }
}

bool FFMStageController::set_origin(float xpos, float ypos, float zpos)
{
    // check, if additional arguments have been handed over, though their dimension is not defined via dims;
    // print a warning in this case
    switch(dims){
    case NULL:
        log->append(tr("Error occured in FFMStageController::set_scales(float xscale, float yscale = 0, float zscale=0 ): "));
        log->append(tr("!! uint8_t has to be set vio set FFMStageController::set_dim(uint8_t), before the scales can be set !! "));
        return false;
    case 1:
        if ((ypos > 0.0) || (zpos > 0.0) || (ypos < 0.0) || (zpos < 0.0)){
            log->append("WARNING in FFMStageController::set_scales(float xscale, float yscale=0, float zscale=0):");
            log->append("uint8_t dims has been set to 1 => scaling y- or z-axis is void and won't be conducted!");
        }
        break;
    case 2:
        if ((zpos > 0.0) || (zpos < 0.0)){
            log->append("WARNING in FFMStageController::set_scales(float xscale, float yscale=0, float zscale=0):");
            log->append("uint8_t dims has been set to 1 => scaling z-axis is void and won't be conducted!");
        }
    }
    //convert float arguments to std::strings and build the instruction string
    QString qstr_xpos;
    QString qstr_ypos;
    QString qstr_zpos;
    qstr_xpos = QString::number(xpos, 'f', 5);          // float xscale to std::string   |
    QString  qinstr = qstr_xpos;
    if ((dims == 2) || (dims == 3)){

        qstr_ypos = QString::number(ypos, 'f', 5);  // float yscale to std::string   |
        qinstr.append(" ");
        qinstr.append(qstr_ypos);
    }
    if (dims == 3){
        qstr_zpos = QString::number(zpos, 'f', 5);  // float zscale to std::string   |
        qinstr.append(" ");
        qinstr.append(qstr_zpos);
    }
    qinstr.append(" setpos ");
//    log->append(tr("DEBUG in set_origin(), instruction after last append() session is: \"%1\"").arg(qinstr));//check the instruction for correct syntax
    // send instruction to the serial connection
    if(!SerConn->isConnectionOpen()){
        log->append("<b>FAIL</b> in set_pos(...): Stage has not been connected.");
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
        log->append(message);
        return true;
    } else {
        log->append(tr("FAIL: Serial Connection returned an error message in FFMStageController::set_scales():"));
        log->append(tr("Error Messgae: %1").arg(SerConn->getLastError().c_str()));
        return false;
    }
}

bool FFMStageController::set_max_vel(float vel)
{
    // check range
    if (!(vel > 0)){
        log->append(tr("Error in set_max_accel(float accel) : Argument out of range!"));
        return false;
    }
    //build instruction string
    QString qstr_vel = QString::number(vel, 'f', 5);  // float to std::string
    QString qinstr = qstr_vel;
    qinstr.append(" setvel ");
    // send instruction to serial connection
    if (!SerConn->isConnectionOpen()){
        log->append("<b>FAIL</b> in FFMStageController::get_max_vel(): Stage has not been connected.");
        return false;
    }
    bool ok;
//    log->append(tr("DEBUG in set_max_vel(), instruction after building the instruction is: \"%1\"").arg(qinstr));//check the instruction for correct syntax
    ok = SerConn->write(qinstr.toStdString());
    if (!ok){
        log->append("<b>FAIL</b> in FFMStageController::set_max_vel(): Error occured while writing to the serial connection.");
        log->append(tr("Error Messgae: %1").arg(SerConn->getLastError().c_str()));
        return false;
    } else {
        log->append(tr("Trying to set maximal velocity to %1; this value might be rounded off.").arg(qstr_vel));
    }
    // check the value and return;
    float check = get_max_vel();
    QString qstr_check = QString::number(check, 'f', 5);

    if (!(check < 0)){
        log->append(tr("Maximal velocity set to %1").arg(qstr_check));
        return true;
    } else {
        log->append(tr("Velocity check value is %1; The maximal velocity could probably not be set!").arg(qstr_check));
        return false;
    }
}

int FFMStageController::getLastErrorCode()
{
    QTime t;
    int errorcode;
    std::string response;
    bool ok;
    if (!SerConn->open()){
        log->append("<b>FAIL</b> in getLastErrorCode(): Stage has not been connected.");
        return -1;
    }
    SerConn->write(std::string("geterror\r\0"));
    t.start();
    while(t.elapsed() < 5){
    }
    response = SerConn->readUntil("\r\n", &ok);
    if(ok){
        chop_end_recog_seq(response);
        errorcode = atoi(response.c_str());
//        log->append(tr("Debug: Errorcode is: %1").arg(errorcode));
    } else{
        log->append(tr("Error occurred while reading errorcode in FFMStageController::getLastErrorCode(): ").arg(SerConn->getLastError().c_str()));
        log->append("Error occured after trying to remove the reading signal sequence \\r\\n in getLastErrorCode().");
        errorcode = -1;
    }
    return errorcode;
}

bool FFMStageController::setIOport(uint8_t port)
{
    bool ok;
    if(!SerConn->isConnectionOpen()){
        log->append("<b>FAIL</b> in setIOport() Stage has not been connected.");
        return false;
    }
    // send instruction corresponding to port
    switch (port){
    case 0:
            ok =SerConn->write(std::string("0 setioport\r\0"));
            log->append("Try to select Ioport \"RS322 A\"...");
        break;
        case 1:
        ok =SerConn->write(std::string("2 setioport\r\0"));
        log->append("Try to select Ioport \"IEEE\"...");
            break;
    case 2:
        ok =SerConn->write(std::string("8 setioport\r\0"));
        log->append("Try to select Ioport \"Transputer Channel\"...");
        break;
    case 3:
        ok =SerConn->write(std::string("12 setioport\r\0"));
        log->append("Try to select Ioport \"HT2000\"...");
        break;
        default:
        log->append(tr("<b>FAIL:</b> Wrong input parameter \"%1\"in FFMStageController::setIOport(int port).\nMode could not be set.").arg(port));
        return false;
    }

    /*  get errors, if errorcodes are available;
    QTime t;
    t.start();
    while(t.elapsed() < 5){
    }
    int errorcode = getLastErrorCode();
    int errorcode = 0; // test
    switch(errorcode){
    case 0:
        ok = true;
        if (port == 0) log->append("Ioport \"RS322 A\" selected");
        if (port == 1) log->append("Ioport \"IEEE\" selected");
        if (port == 2) log->append("Ioport \"Transputer Channel\" selected");
        if (port == 3) log->append("Ioport \"HT2000\" selected");
        break;
    case 1002:
        log->append(tr("FFMStageController::set_mode(int mode) : got wrong parameter"));
        ok = false;
        break;
    case 1007:
        log->append(tr("FFMStageController::set_mode(int mode) : invalid parameter value"));
        ok = false;
        break;
    default:
        log->append(tr("FFMStageController::set_mode(int mode) : Unknown error code %1").arg(errorcode));
        ok = false;
        break;
        }
        */
    return ok;
}

inline void FFMStageController::chop_end_recog_seq(std::string rawstring)
{
    rawstring.erase(rawstring.length()-2, 2);
}

bool FFMStageController::initStage()
{
    bool ok;
    QTime t;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    if (!SerConn->isConnectionOpen()){
        log->append("<b>FAIL</b> in initStage(): Stage has not been connected.");
        QApplication::restoreOverrideCursor();
        return false;
    }
    log->append("Initialize Stage...");
        // set ht mode:
        ok = set_ht_mode(0);
        if (!ok){
            log->append("<b>ERROR</b> in initStage(): after setting ht_mode\n <b>INITIALIZATION ABORTED </b>");
            QApplication::restoreOverrideCursor();
            return false;
        }
        t.start();
        while(t.elapsed() < 5){
        }
        // set IO port:
        ok = setIOport(0);
        if (!ok){
            log->append("<b>ERROR</b> in initStage: after setting IOport");
            QApplication::restoreOverrideCursor();
           return false;
        }
        t.restart();
        while(t.elapsed() < 5){
        }
        // print Version Number:
        log->append(tr("Firmware Version of multicontrol2000: %1").arg(get_VersionNumber()));
        t.restart();
        while(t.elapsed() < 5){
        }
        // set the dimension property (uint8_t) and the corresponding hardware parameter:
        ok = set_dim(3);
        if (!ok){
            log->append("<b>ERROR</b> in initStage: after setting dimensions");
            QApplication::restoreOverrideCursor();
            return false;
        }
        t.restart();
        while(t.elapsed() < 5){
        }
        // set axes modes;
        ok = set_axes_modes(1, 1, 2); // x on, y on, z on restricted:
        if (!ok){
            log->append("<b>ERROR</b> in initStage: after setting axes_modes");
            QApplication::restoreOverrideCursor();
           return false;
        }
        t.restart();
        while(t.elapsed() < 5){
        }
        //set units for each axis ( speed in mm, xyz in micrometers):
        ok = set_unit(0, 2);
        t.restart();
        while(t.elapsed() < 5){
        }
        if(ok) ok = set_unit(1, 1);
        t.restart();
        while(t.elapsed() < 5){
        }
        if(ok) ok = set_unit(2, 1);
        t.restart();
        while(t.elapsed() < 5){
        }
        if(ok) ok = set_unit(3, 1);
        if (!ok){
            log->append("<b>ERROR</b> in initStage: after setting the axes' units");
            QApplication::restoreOverrideCursor();
            return false;
        }
        t.restart();
        while(t.elapsed() < 5){
        }
        //set pitch for each axis (speed axis, x,y: 2.0 ; z: 0.1)
        ok = set_pitch(0, 2.0);
        t.restart();
        while(t.elapsed() < 5){
        }
        if (ok) ok = set_pitch(1, 2.0);
        t.restart();
        while(t.elapsed() < 5){
        }
        if (ok) ok = set_pitch(2, 2.0);
        t.restart();
        while(t.elapsed() < 5){
        }
        if (ok) ok = set_pitch(3, 0.1);
        if (!ok){
            log->append("<b>ERROR</b> in initStage: after setting the axes' pitches");
            QApplication::restoreOverrideCursor();
            return false;
        }
        t.restart();
        while(t.elapsed() < 5){
        }
        //set scales (x,y,z : 1.0 each);
        ok = set_scales(1.0, 1.0, 1.0);
        if(!ok){
            log->append("<b>ERROR</b> in initStage: after setting the axes' scales");
            QApplication::restoreOverrideCursor();
            return false;
        }
        t.restart();
        while(t.elapsed() < 5){
        }
        // set maximal acceleration 10.0;
        ok = set_max_accel(10.0);
        if(!ok){
            log->append("<b>ERROR</b> in initStage: after setting the maximal acceleration");
            QApplication::restoreOverrideCursor();
            return false;
        }
        t.restart();
        while(t.elapsed() < 5){
        }
        // set maximal velocity to 10.0 => probably 10.0 mm/s , as this unit was specified on the speed axis;
        ok =  set_max_vel(10.0);
        if(!ok){
            log->append("<b>ERROR</b> in initStage: after setting the maximal velocity");
            QApplication::restoreOverrideCursor();
            return false;
        }
        t.restart();
        while(t.elapsed() < 5){
        }
        // set the origin to 0.0, 0.0, 0.0
        ok = set_origin(0.0, 0.0, 0.0);
        if(!ok){
            log->append("<b>ERROR</b> in initStage: after setting theorigin");
            QApplication::restoreOverrideCursor();
            return false;
        }
        t.restart();
        while(t.elapsed() < 5){
        }
        // switches the joystick on:
        set_joystick(true);
        if(!ok){
           log->append("<b>ERROR</b> in initStage: after trying to switch the joystick");
           QApplication::restoreOverrideCursor();
           return false;
        }
        t.restart();
        while(t.elapsed() < 5){
        }
        // sets the maximal joystick speed to 10.0:
        ok = set_max_joyspeed(10.0);
        if (!ok){
            log->append("<b>ERROR</b> in initStage: after setting the speed of the joystick");
            QApplication::restoreOverrideCursor();
            return false;
        }
        log->append("Stage initialized successfully - have fun.");
        QApplication::restoreOverrideCursor();
        return true;

}

bool FFMStageController::disconnect()
{
    bool ok;
    if(!SerConn->isConnectionOpen()){
        log->append("Disconnection failed: No connection has been established to the Stage before!");
        return false;
    }
    ok = SerConn->close();
        if (ok){
            log->append(tr("Stage has been disconnected...\n*----------------------------------"));
        } else {
            log->append(tr("<b>EPICFAIL</b>: \nError Messgae: %1").arg(SerConn->getLastError().c_str()));
        }
        return ok;
}
///////// test

void FFMStageController::move0() // test
{
    bool ok;
    ok = move_lim((float) x_axis->value(), (float) y_axis->value(),(float) z_axis->value());

}
void FFMStageController::move1() // test
{
    bool ok;
    ok = move_remove((float) x_axis->value(), (float) y_axis->value(),(float) z_axis->value());
}

///////////test
