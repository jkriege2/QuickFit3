#ifndef MCSTAGECONTROLLER_H
#define MCSTAGECONTROLLER_H

#include <QObject>
#include "qfserialconnection.h"
#include <stdint.h>
#include<QtGlobal>
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif


#include <QGridLayout>
#include <QSpinBox>
#include <string>
#include <cstring>
#include <cmath>
#include <limits>
#include <QTime>
#include <string>
#include <QRegExp>
#include <qfpluginservices.h>

/*
  class for the Stage thing; connected to COM2 at the moment...
*/

/*    std::string comPort;
float max_accel;
float max_vel;
float max_joyspeed;
*/
class MCStageController : public QObject
{
    Q_OBJECT
public:
    explicit MCStageController(std::string Port, float accel, float vel, float joyspeed, QObject *parent = 0);
    ~MCStageController();

//testfunctions
  //  QWidget *get_testwidget();

    //*testfunctions

    /*
      |--------------------------------|
      | public programming interface I |
      |--------------------------------|
     */

    /*

      */
    void setLogging(QFPluginLogService* logservicepointer);

    /*
      Connect to Stage
    */
    bool connectToStage();

    /*
     Initialize stage; after this command, the stage is ready to receive move instructions
    */
    bool initStage();
    /*
      Reconfigures all values that can be set ;
    */
    bool reconfigureStage(float newMaxAccel, float newMaxVel, float newMaxJoySpeed);
    /*
    Disconnect from stage
    */
    bool disconnect();
    /*
      Checks the state of the Serial Connection
    */
    bool connected();

    /*
      hardware error ocurred?;
      */
    bool hardware_error();

    /*
      Configuration method for the joystick
    */
    bool config_joystick(bool enable, float speed = 10.0);

    float get_currentPosition(uint8_t axis);

    uint8_t getDimensions();
    /*
      Moves the Stage to the position given by input parameters; if the target position is out of range,
      the stage moved to its maximum position in the corresponding direction and the return value is TRUE;
      if wait is determined as > 0, the program halts for the given time in ms (p.e. to avoid sending multiple instructions)
    */
    bool move_lim(float xpos, float ypos = 0, float zpos = 0, float wait = 0);

    /*
    Moves the Stage to the position given by input parameters; if the position is out of range, the stage will be moved back
    to its original position afterwards and the return value is FALSE;
    if wait1 is determined as > 0, the program halts for the given time (p.e. to avoid sending multiple instructions) driving;
    if wait2 is determined as > 0, the program halts for the given time in ms (p.e. to avoid sending multiple instructions) while driving back, if necessary;
    */
    //bool move_remove(float xpos, float ypos = 0, float zpos = 0, float wait1 = 0, float wait2 = 0);

    /*
      Moves the Stage to the position given by the input parameters;
      the final position is compared to the limits values;
      the action is only conducted, if the position is none of the new
      positions will exceed a limit;
      true if, moved succesfully, otherwise false;
      if wait is determined as > 0, the program halts for the given time in ms (p.e. to avoid sending multiple instructions)


    bool move_check(float xpos, float ypos = 0, float zpos = 0, float wait = 0);
    */
    /*
      enable joystick
    */
    bool joystick_enabled();


    /*
      check, if the state is still moving;
      if true, the stage does not move and can receive new instructions
    */
    bool moving();


protected:

    /*
    |------------|
    | properties |
    -------------|
    */

    // objects and status flags

    QFSerialConnection *SerConn; // Serial Connection to the multicontrol 2000 Stage;
    uint8_t dims; // Number of dimensions.
                  // Affects the instructions set_scales, set_pos, ...
                  // Values can be 1, 2, or 3;
                  // 1 => only x direction
                  // 2 => x and y direction
                  // 3 => x, y and z direction
    bool joystick_active;

    // configurable values

    std::string comPort;
    float max_accel;
    float max_vel;
    float max_joyspeed;

    // fixed values

    uint32_t comPortSpeed;

    /*
      |-----------------------------------|
      | hidden hardware control functions |
      |-----------------------------------|
    */

    /*
      Returns the mode of the specified axis (0,1 or 2);
      If an error occurs, the function will return -1;
      possible axis values:
      value     interpretation
      0         speed axis => speed will be given in the specified unit from now on
      1         x-axis
      2         y-axis
      3         z-axis
    */
    int8_t get_axis_mode(uint8_t axis);

    /*
     returns the version number
    */
    uint16_t get_VersionNumber();

    /*
     returns the (real) maximal acceleration value set via "bool set_max_accel(float accel)";
    */
    float get_max_accel();


    /*
     returns the (real) maximal velocity value set via "bool set_max_vel(float vel)";
     physical interpretation is dependent on the unit set on the speed axis!
     */
    float get_max_vel();

    /*
     returns the (real) maximal joyspeed value set via "bool set_max_vel(float vel)";
     physical interpretation is dependent on the unit set on the speed axis!
     */
    float get_max_joyspeed();

    /*
      checks the stage and returns a Qlist of 5 bools :
      at(0)                  => Did this function work?
      at(1)                  => false -> stage is prepared for new instructions
                                true  -> stage motors stil moving
      at(2)                  => true -> joystick active, false -> joystick inactive
      at(3)                  => true offline
      at(4)                  => true -> not ready, NV memory is being programmed
     */
    QList<bool> get_status();

    /*
      Returns the unit of the specified axis (0,1 or 2);
      If an error occurs, the function will return -1;
      possible axis values:
      value     interpretation
      0         speed axis => speed will be given in the specified unit from now on
      1         x-axis
      2         y-axis
      3         z-axis
    */
    float get_pitch(uint8_t axis);

      /*
      Returns the current position of the specified axis
      in its determined units
      1         x-axis
      2         y-axis
      3         z-axis
      silent specifies, whether or nto this function shall print messages to the logfile;
      Error messages are printed independently!
      */
    float get_position(uint8_t axis, bool silent = false);

    /*
      Returns the unit of the specified axis (0,1 or 2);
      If an error occurs, the function will return -1;
      possible axis values:
      value     interpretation
      0         speed axis => speed will be given in the specified unit from now on
      1         x-axis
      2         y-axis
      3         z-axis
    */
    int8_t get_unit(uint8_t axis);

    /*
      Moves the Stage to the position given in input parameters, which have to be given in the
      units, that have to be specified before via set_unit();
      If one of the parameter is out of range, there are several ways to deal with:
      handle      Description
      0        =>
      1        =>
      (2       => the arguments are checked and the stage is only moved,
                  if the parameters handed over fit within the range
                  THIS FUNCTION CAUSES A BIT MORE EFFORT AND KNOWLEDGE ABOUT
                  THE BORDERS OF THE MULTICONTROL MACHINERY;
                 , THEREFORE IT WILL BE IMPLEMENTED LATER !)
     */
    bool move_To(float xpos, float ypos = 0, float zpos = 0);

     /*
      Sets the different modes for each axis x,y and z;
      code         mode
      0         => the axis will be switched off
      1         => the axis will be switched on
      2         => the axis will be switched on, but final switches will NOT be activated;
                   as a consequence, different instrucions (..., ...) will not be performed;
      */
    bool set_axes_modes(uint8_t code_x,uint8_t code_y,uint8_t code_z);

    /*
      Sets the number of dimensions, which will affect later instructions:
      scale_axis,
     */
     bool set_dim(uint8_t dimensions);

    /*
      Sets the input / output port for Venus commands;
      Still commands form the serial connection are accepted,
      though their reply will be sent to the selected port
      possible parameters:
      port     (internal parameter)      portname
      0        (0)                       RS232 A
      1        (2)                       IEEE
      2        (8)                       Transputer Channel
      3        (12)                      HT2000 Server
    */
    bool setIOport(uint8_t port);

    /*
     sets the maximum acceleration value for the "fastest" axis
     the acceleration value is rounded off internally,
     therefore the real value will be requested and printed;
      */
    bool set_max_accel(float accel);


    /*
      sets the maximal speed of the joystick toy
      result is dependent on the actual units of the speed axis!
     */
    bool set_max_joyspeed(float speed, bool silent = false);

    /*
     switches the joystick on or off (true / false)
     if silent, the method logs only error and warning, but not normal text mesages
     */
    bool set_joystick(bool mode, bool silent = false);

    /*
      Sets the maximal velocity of the fastest axis;
      as the values might be processed internally, the set value
      is requested and printed afterwards;
    */
    bool set_max_vel(float vel);

    /*
      sets the mode of the  venus-1 interpreter
      mode = 0 ; => host mode
      mode = 1 ; => terminal mode
      secondtry tries the command again if it didnt work for the first time,
      but this time the instruction is sent as in terminal mode

      */
    bool set_ht_mode(uint8_t mode);//(uint8_t mode, bool secondtry = false);

    /*
      Sets the origin to the corresponding values;
      uint8_t dims has to be set via set_dim(uint8_t), before this function is used.
      ypos and/or zpos are disregarded, if uint8_t dims is less than 3;
    */
    bool set_origin(float xpos = 0.0, float ypos = 0.0, float zpos = 0.0);

    /*
      Sets the pitch parameter of the specified axis;
      pitch has to be given in mm;
      possible axis values:
      value     interpretation
      0         speed axis => speed will be given in the specified unit from now on
      1         x-axis
      2         y-axis
      3         z-axis
      pitch range: 0.00001 - 1000.0
    */
    bool set_pitch(uint8_t axis, float pitch);


    /*
      Scales all axes; the number of necessary input parameters depends
      on the property "uint8_t dims", which has to be set by set_dim before,
      otherwise the method will report an error message and return false;
    */
    bool set_scales(float xscale, float yscale = 0.0, float zscale = 0.0);

    /*
      Sets the unit of an axis to the specified physical size;

      possible axis values:
      value     interpretation
      0         speed axis => speed will be given in the specified unit from now on
      1         x-axis
      2         y-axis
      3         z-axis

      possible unit values:
      value    unit
      0        microsteps
      1        micrometer
      2        mm
      3        cm
      4        m
      5        inch
      6        mil (= 1/1000 inch)
    */
    bool set_unit(uint8_t axis, uint8_t unit);


    /*
      returns the last error code of the Stage;
    */
    int getLastErrorCode();

    /*
      |-------------------|
      | additional stuff  |
      |-------------------|
     */

    inline void chop_end_recog_seq(std::string rawstring);
    void log_text(QString message);
    void log_warning(QString message);
    void log_error(QString message);


};


#endif // MCSTAGECONTROLLER_H
