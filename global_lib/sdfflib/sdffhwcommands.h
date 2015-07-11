
/*
  Name: sdffhwcommands.h
  Copyright: (c) 2007
  Author: Jan Krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/

/** \file sdffhwcommands.h
 * \ingroup hlseqgen_sdff_commands
 *
 * In this file you will find some tool methods and classes that represent commands sent tos specialized
 * hardware devices (serial, serGPIB devices ...)
 */
/*@{*/

#ifndef SDFFHWCOMMANDS_H
#define SDFFHWCOMMANDS_H

#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <cstdio>
#include <stdint.h>
#include <cmath>

#include "tools.h"
#include "datatable.h"
#include "sequencererror.h"
#include "channelmanager.h"
#include "sequencegenerator.h"

namespace sequencer {

class SDFFparser; // forward declaration








class SDFFHWcommandManager; // forward declaration

/** \brief SDFFcommand class for serial interface commands
 * \ingroup hlseqgen_sdff_commands
 */
#define SDFF_CMDclass_SERIAL 0

/** \brief SDFFcommand class for serial-to-GPIB interface commands
 * \ingroup hlseqgen_sdff_commands
 */
#define SDFF_CMDclass_SERGPIB 1

/** \brief convert a command class id into a string
 * \ingroup hlseqgen_sdff_commands
 */
std::string command_classtostring(unsigned int command_class);

/** \brief the parity bit mode of a serial interface
 * \ingroup hlseqgen_sdff_commands_serial
 */
enum sdff_serial_paritymodes {
    sdffserNOPARITY, /*!< \brief no parity bit at all */
    sdffserEVEN,     /*!< \brief even parity */
    sdffserODD       /*!< \brief odd parity */
};

/** \brief convert a sdff_serial_paritymodes into a string
 * \ingroup hlseqgen_sdff_commands_serial
 */
std::string sdff_serial_paritymode2str(sdff_serial_paritymodes mode);

/** \brief convert a string into a sdff_serial_paritymodes
 * \ingroup hlseqgen_sdff_commands_serial
 */
sdff_serial_paritymodes str2sdff_serial_paritymodes(std::string mode);

/** \brief the handshaking mode of a serial interface
 * \ingroup hlseqgen_sdff_commands_serial
 */
enum sdff_serial_handshaking {
    sdffserNOHANDSHAKING, /*!< \brief no parity bit at all */
    sdffserXONXOFF,     /*!< \brief even parity */
    sdffserHARDWARE       /*!< \brief odd parity */
};

/** \brief convert a sdff_serial_handshaking into a string
 * \ingroup hlseqgen_sdff_commands_serial
 */
std::string sdff_serial_handshaking2str(sdff_serial_handshaking mode);

/** \brief convert a string into a sdff_serial_handshaking
 * \ingroup hlseqgen_sdff_commands_serial
 */
sdff_serial_handshaking str2sdff_serial_handshaking(std::string mode);


/** \brief base class for all hardware commands (e.g. for GPIB or serial I/O)
 * \ingroup hlseqgen_sdff_commands
 */
class SDFFcommand {
  protected:
    /** \brief when to execute the command */
    double t_start;
    /** \brief SDFF sequence file in which the command appears */
    std::string filename;
    /** \brief line in the SDFF sequence file where the command appears */
    long line;
    /** \brief command class of this command */
    unsigned int command_class;
    /** \brief return a string that contains the location of the command */
    inline std::string get_command_location() {
      return filename+"(l."+inttostr(line)+", t="+floattostr(t_start)+")";
    }
    /** \brief the result/return value after executing the command */
    std::string result;
    /** \brief is set \c true if the command has a return value */
    bool has_result;
  public:
    /** \brief class constructor */
    SDFFcommand(double t_start, std::string filename, long line) {
      this->t_start=t_start;
      this->filename=filename;
      this->line=line;
      this->has_result=false;
      this->result="";
    };

    /** \brief class destructor */
    virtual ~SDFFcommand() {};

    /** \brief get the execution time in microseconds */
    GetMacro(double, t_start);

    /** \brief get the filename */
    GetMacro(std::string, filename);

    /** \brief get the line  */
    GetMacro(long, line);

    /** \brief get the command class */
    GetMacro(unsigned int, command_class);

    /** \brief returns \c true if the command had a result which can be acquired by get_result() */
    GetMacro(bool, has_result);

    /** \brief returns the result of the command */
    GetMacro(std::string, result);

    /** \brief store a result */
    inline void set_result(std::string result) { this->result=result; this->has_result=true; };

    /** \brief clear the result */
    inline void clear_result() { this->result=""; this->has_result=false; };

    /** \brief report the command as a string */
    virtual inline std::string tostring() {
      return "unknown_command(class='"+command_classtostring(command_class)+"')   in "+get_command_location();
    };

    /** \brief report the result of the instruction if available and return "" if not.
      *        In the result all unprintable characters are replaced by a character description
      */
    virtual inline std::string report_result() {
      if (!has_result) return "";
      return tostring()+"\n    -> '"+toprintablestr(result)+"'";
    };
};


/** \brief base class for hardware commands for a serial interface port (COM1, tty0 ...)
 * \ingroup hlseqgen_sdff_commands_serial
 */
class SDFFserialCommand: public SDFFcommand {
  protected:
    /** \brief the serial port to write to */
    std::string com_port;
  public:
    /** \brief class constructor */
    SDFFserialCommand(double t_start, std::string filename, long line, std::string com_port):
      SDFFcommand(t_start, filename, line)
    {
      this->com_port=com_port;
      this->command_class=SDFF_CMDclass_SERIAL;
    };

    /** \brief set the COM port */
    SetMacro(std::string, com_port);
    /** \brief get the COM port */
    GetMacro(std::string, com_port);

    /** \brief report the command as a string */
    virtual inline std::string tostring() {
      return "serial_command(port='"+com_port+"')   in "+get_command_location();
    };
};

/** \brief represents a command that opens a serial interface
 * \ingroup hlseqgen_sdff_commands_serial
 */
class SDFFserialOpenCommand: public SDFFserialCommand {
  protected:
    /** \brief the parity mode to be used */
    sdff_serial_paritymodes parity;
    /** \brief the baud rate to be used */
    unsigned long baudrate;
    /** \brief number of stop bits */
    double stopbits;
    /** \brief number of transfered data bits */
    unsigned char databits;
    /** \brief the handshaking mode */
    sdff_serial_handshaking handshaking;
  public:
    /** \brief class constructor */
    SDFFserialOpenCommand(double t_start, std::string filename, long line, std::string com_port):
      SDFFserialCommand(t_start, filename, line, com_port)
    {
        parity=sdffserNOPARITY;
        handshaking=sdffserNOHANDSHAKING;
        baudrate=9600;
        stopbits=1;
        databits=8;
    };

    /** \brief set the handshaking mode */
    SetMacro(sdff_serial_handshaking, handshaking);
    /** \brief get the handshaking mode */
    GetMacro(sdff_serial_handshaking, handshaking);
    /** \brief set the parity mode */
    SetMacro(sdff_serial_paritymodes, parity);
    /** \brief get the parity mode */
    GetMacro(sdff_serial_paritymodes, parity);
    /** \brief set the baud rate */
    SetMacro(unsigned long, baudrate);
    /** \brief get the baud rate */
    GetMacro(unsigned long, baudrate);
    /** \brief set the number of stop bits */
    SetMacro(double, stopbits);
    /** \brief get the number of stop bits */
    GetMacro(double, stopbits);
    /** \brief set the number of data bits */
    SetMacro(unsigned char, databits);
    /** \brief get the number of data bits */
    GetMacro(unsigned char, databits);

    /** \brief report the command as a string */
    virtual inline std::string tostring() {
      std::string par=sdff_serial_paritymode2str(parity);
      std::string hs=sdff_serial_handshaking2str(handshaking);
      return "serial_open(port='"+com_port+"', baudrate="+inttostr(baudrate)+", parity="+par+", databits="+inttostr(databits)+", stopbits="+floattostr(stopbits)+", handshaking="+hs+")   in "+get_command_location();
    };
};


/** \brief represents a command that sends data to a serial interface
 * \ingroup hlseqgen_sdff_commands_serial
 */
class SDFFserialSendCommand: public SDFFserialCommand {
  protected:
    /** \brief the data to be written */
    std::string text;
  public:
    /** \brief class constructor */
    SDFFserialSendCommand(double t_start, std::string filename, long line, std::string com_port):
      SDFFserialCommand(t_start, filename, line, com_port)
    {
    };

    /** \brief set the data to be written */
    SetMacro(std::string, text);
    /** \brief get tthe data to be written */
    GetMacro(std::string, text);

    /** \brief report the command as a string */
    virtual inline std::string tostring() {
      return "serial_send(port='"+com_port+"', text='"+toprintablestr(text)+"')   in "+get_command_location();
    };
};

/** \brief represents a command that closes the serial interface
 * \ingroup hlseqgen_sdff_commands_serial
 */
class SDFFserialCloseCommand: public SDFFserialCommand {
  public:
    /** \brief class constructor */
    SDFFserialCloseCommand(double t_start, std::string filename, long line, std::string com_port):
      SDFFserialCommand(t_start, filename, line, com_port)
    {
    };

    /** \brief report the command as a string */
    virtual inline std::string tostring() {
      return "serial_close(port='"+com_port+"')   in "+get_command_location();
    };
};



/** \brief represents a command that reads data to a serial interface
 * \ingroup hlseqgen_sdff_commands_serial
 */
class SDFFserialReadCommand: public SDFFserialCommand {
  protected:
    /** \brief the number of bytes to be read or \c -1 if we want to read until a special character appears */
    int byte_count;
    /** \brief if \c data_count=-1 this command should read until this character is received */
    char end_char;
  public:
    /** \brief class constructor */
    SDFFserialReadCommand(double t_start, std::string filename, long line, std::string com_port):
      SDFFserialCommand(t_start, filename, line, com_port)
    {
        byte_count=-1;
    };

    /** \brief set the number of characters to be read */
    SetMacro(int, byte_count);
    /** \brief get the number of characters to be read */
    GetMacro(int, byte_count);
    /** \brief set the character to stop the read operation at */
    SetMacro(char, end_char);
    /** \brief get character to stop the read operation at */
    GetMacro(char, end_char);

    /** \brief report the command as a string */
    virtual inline std::string tostring() {
      if (byte_count==-1) return "serial_read(port='"+com_port+"', end_char='"+chartoprintablestr(end_char)+"')   in "+get_command_location();
      else return "serial_read(port='"+com_port+"', byte_count="+inttostr(byte_count)+")   in "+get_command_location();
    };
};







/** \brief base class for hardware commands for the home-built triggerable serial-to-GPIB interface
 * \ingroup hlseqgen_sdff_commands_sergpib
 */
class SDFFserGPIBcommand: public SDFFcommand {
  public:
    /** \brief class constructor */
    SDFFserGPIBcommand(double t_start, std::string filename, long line):
      SDFFcommand(t_start, filename, line)
    {
      this->command_class=SDFF_CMDclass_SERGPIB;
    };

};

/** \brief this sends a command over the (triggerable) serial-to-GPIB-Interface built for our group
 * \ingroup hlseqgen_sdff_commands_sergpib
 */
class SDFFserGPIBsendCommand: public SDFFserGPIBcommand {
  private:
    /** \brief the GPIB command to be sent */
    std::string command;
    /** \brief the adress of the device to talk to */
    char device;
  public:
    /** \brief class contructor */
    SDFFserGPIBsendCommand(double t_start, char device, std::string command, std::string filename, long line):
      SDFFserGPIBcommand(t_start, filename, line)
    {
      this->device=device;
      this->command=command;
    };
    /** \brief get the command string */
    GetMacro(std::string, command);

    /** \brief get the device adress to talk to  */
    GetMacro(char, device);

    /** \brief report the command as a string */
    virtual inline std::string tostring() {
      return "sergpib_send(device="+inttostr(device)+", command='"+command+"')   in "+get_command_location();
    };
};

/** \brief this sends data over the (triggerable) serial-to-GPIB-Interface built for our group and then reads the result (query operation)
 * \ingroup hlseqgen_sdff_commands_sergpib
 */
class SDFFserGPIBqueryCommand: public SDFFserGPIBsendCommand {
  public:
    /** \brief class contructor */
    SDFFserGPIBqueryCommand(double t_start, char device, std::string command, std::string filename, long line):
      SDFFserGPIBsendCommand(t_start, device, command, filename, line)
    {
    };

    /** \brief report the command as a string */
    virtual inline std::string tostring() {
      return "sergpib_query(device="+inttostr(get_device())+", command='"+get_command()+"')   in "+get_command_location();
    };
};

/** \brief this reads data from the (triggerable) serial-to-GPIB-Interface built for our group
 * \ingroup hlseqgen_sdff_commands_sergpib
 */
class SDFFserGPIBreadCommand: public SDFFserGPIBcommand {
  private:
    /** \brief the adress of the device to talk to */
    char device;
  public:
    /** \brief class contructor */
    SDFFserGPIBreadCommand(double t_start, char device, std::string filename, long line):
      SDFFserGPIBcommand(t_start, filename, line)
    {
      this->device=device;
    };

    /** \brief get the device adress to talk to  */
    GetMacro(char, device);

    /** \brief report the command as a string */
    virtual inline std::string tostring() {
      return "sergpib_read(device="+inttostr(device)+")   in "+get_command_location();
    };
};

/** \brief a DCL bus command
 * \ingroup hlseqgen_sdff_commands_sergpib */
#define GPIB_BUSCOMMAND_DCL 0x0014

/** \brief a SDC bus command
 * \ingroup hlseqgen_sdff_commands_sergpib */
#define GPIB_BUSCOMMAND_SDC 0x0004

/** \brief a GET bus command
 * \ingroup hlseqgen_sdff_commands_sergpib */
#define GPIB_BUSCOMMAND_GET 0x0008

/** \brief a GTL bus command
 * \ingroup hlseqgen_sdff_commands_sergpib */
#define GPIB_BUSCOMMAND_GTL 0x0001

/** \brief a LLO bus command
 * \ingroup hlseqgen_sdff_commands_sergpib */
#define GPIB_BUSCOMMAND_LLO 0x0011

/** \brief a PPC bus command
 * \ingroup hlseqgen_sdff_commands_sergpib */
#define GPIB_BUSCOMMAND_PPC 0x0005

/** \brief a PPU bus command
 * \ingroup hlseqgen_sdff_commands_sergpib */
#define GPIB_BUSCOMMAND_PPU 0x0015

/** \brief a UNT bus command
 * \ingroup hlseqgen_sdff_commands_sergpib */
#define GPIB_BUSCOMMAND_UNT 0x005F

/** \brief a UNL bus command
 * \ingroup hlseqgen_sdff_commands_sergpib */
#define GPIB_BUSCOMMAND_UNL 0x003F

/** \brief a SPD bus command
 * \ingroup hlseqgen_sdff_commands_sergpib */
#define GPIB_BUSCOMMAND_SPD 0x0019

/** \brief a SPE bus command
 * \ingroup hlseqgen_sdff_commands_sergpib */
#define GPIB_BUSCOMMAND_SPE 0x0018

/** \brief a TCTbus command
 * \ingroup hlseqgen_sdff_commands_sergpib */
#define GPIB_BUSCOMMAND_TCT 0x0009

/** \brief a IFC bus command
 * \ingroup hlseqgen_sdff_commands_sergpib */
#define GPIB_BUSCOMMAND_IFC 0xFF00

/** \brief this sends a command over the (triggerable) serial-to-GPIB-Interface built for our group
 * \ingroup hlseqgen_sdff_commands_sergpib
 */
class SDFFserGPIBsendBusCommand: public SDFFserGPIBcommand {
  private:
    /** \brief the GPIB command to be sent */
    unsigned int command;
    /** \brief the adress of the device to talk to */
    char device;
  public:
    /** \brief class contructor */
    SDFFserGPIBsendBusCommand(double t_start, char device, unsigned int command, std::string filename, long line):
      SDFFserGPIBcommand(t_start, filename, line)
    {
      this->device=device;
      this->command=command;
    };
    /** \brief get the command */
    GetMacro(unsigned int, command);

    /** \brief get the device adress to talk to  */
    GetMacro(char, device);

    /** \brief report the command as a string */
    virtual inline std::string tostring() {
      return "sergpib_send_bus_command(device="+inttostr(device)+", command="+inttohex(command)+")   in "+get_command_location();
    };
};


/** \brief this configures the (triggerable) serial-to-GPIB-Interface
 * \ingroup hlseqgen_sdff_commands_sergpib
 */
class SDFFserGPIBconfigureCommand: public SDFFserGPIBcommand {
  private:
    /** \brief GPIB address of the interface itself */
    char interface_address;
    /** \brief use the EOI signal to indicate the end of a message */
    bool use_EOI;
    /** \brief state of REN signal when interface is talker */
    bool REN_talkstate;
    /** \brief state of REN signal when interface is listener */
    bool REN_readstate;
    /** \brief should the interface send a linebreak character after each GPIB command? */
    bool send_LB;
    /** \brief delay between write and read command when querying a device [in milliseconds]*/
    unsigned char query_delay;
  public:
    /** \brief class contructor */
    SDFFserGPIBconfigureCommand(double t_start, std::string filename, long line):
      SDFFserGPIBcommand(t_start, filename, line)
    {
      this->interface_address=0;
      this->use_EOI=true;
      this->REN_talkstate=false;
      this->REN_readstate=false;
      this->send_LB=true;
      this->query_delay=50;
    };

    /** \brief get the device address of the interface  */
    GetMacro(char, interface_address);
    /** \brief set the device address of the interface  */
    SetMacro(char, interface_address);

    /** \brief get the use_EOI property */
    GetMacro(bool, use_EOI);
    /** \brief set the use_EOI property */
    SetMacro(bool, use_EOI);

    /** \brief get the REN_talkstate property */
    GetMacro(bool, REN_talkstate);
    /** \brief set the REN_talkstate property */
    SetMacro(bool, REN_talkstate);

    /** \brief get the REN_readstate property */
    GetMacro(bool, REN_readstate);
    /** \brief set the REN_readstate property */
    SetMacro(bool, REN_readstate);

    /** \brief get the send_LB property */
    GetMacro(bool, send_LB);
    /** \brief set the send_LB property */
    SetMacro(bool, send_LB);

    /** \brief get the query_delay property */
    GetMacro(unsigned char, query_delay);
    /** \brief set the query_delay property */
    SetMacro(unsigned char, query_delay);

    /** \brief report the command as a string */
    virtual inline std::string tostring() {
      return "sergpib_configure_interface(interface_address="+inttostr(interface_address)+", use_EOI="+booltostr(use_EOI)+", REN_talkstate="+booltostr(REN_talkstate)+", REN_readstate="+booltostr(REN_readstate)+", send_LB="+booltostr(send_LB)+"query_delay="+inttostr(query_delay)+")   in "+get_command_location();
    };
};


/** \brief this configures the (triggerable) serial-to-GPIB-Interface built for our group to send a command on an interrupt
 * \ingroup hlseqgen_sdff_commands_sergpib
 */
class SDFFserGPIBconfigInterrupt: public SDFFserGPIBcommand {
  private:
    /** \brief the GPIB command to be sent on an interrupt */
    std::string command;
    /** \brief the device to send the command to */
    char device;
    /** \brief the interrupt on which to send the command */
    char interrupt;
    /** \brief specifies whether to send a GTL (go to local) bus command after the GPIB command */
    bool send_GTL;
    /** \brief specifies whether to send a UNL (unlisten) bus command after the GPIB command */
    bool send_UNL;
  public:
    /** \brief class constructor */
    SDFFserGPIBconfigInterrupt(double t_start, char interrupt, char device, std::string command, std::string filename, long line):
      SDFFserGPIBcommand(t_start, filename, line)
    {
      this->interrupt=interrupt;
      this->device=device;
      this->command=command;
      this->send_GTL=true;
      this->send_UNL=true;
    };
    /** \brief get the command string */
    GetMacro(std::string, command);

    /** \brief get the device adress to talk to  */
    GetMacro(char, device);

    /** \brief get the interrupt to react on  */
    GetMacro(char, interrupt);

    /** \brief set the send_GTL property*/
    SetMacro(bool, send_GTL);
    /** \brief get the send_GTL property*/
    GetMacro(bool, send_GTL);

    /** \brief set the send_UNL property*/
    SetMacro(bool, send_UNL);
    /** \brief get the send_UNL property*/
    GetMacro(bool, send_UNL);

    /** \brief report the command as a string */
    virtual inline std::string tostring() {
      return "sergpib_configure_interrupt(interrupt="+inttostr(interrupt)+", device="+inttostr(device)+", command='"+command+"', send_GTL="+booltostr(send_GTL)+", send_UNL="+booltostr(send_UNL)+")   in "+get_command_location();
    };
};


/** \brief this enables one of the built-in interrupts of the (triggerable) serial-to-GPIB-Interface
 * \ingroup hlseqgen_sdff_commands_sergpib
 */
class SDFFserGPIBenableInterrupt: public SDFFserGPIBcommand {
  private:
    /** \brief the interrupt on which to send the command */
    char interrupt;
    /** \brief specifies whether to enable or disable the interrupt channel */
    bool state;
  public:
    /** \brief class constructor */
    SDFFserGPIBenableInterrupt(double t_start, char interrupt, bool state, std::string filename, long line):
      SDFFserGPIBcommand(t_start, filename, line)
    {
      this->interrupt=interrupt;
      this->state=state;
    };


    /** \brief get the interrupt to react on  */
    GetMacro(char, interrupt);

    /** \brief get the state property*/
    GetMacro(bool, state);

    /** \brief report the command as a string */
    virtual inline std::string tostring() {
      return "sergpib_enable_interrupt(interrupt="+inttostr(interrupt)+", state="+booltostr(state)+")   in "+get_command_location();
    };
};


/** \brief this struct contains data about results and is used to store the results in a list (see SDFFHWcommandManager::get_result_list() )
 * \ingroup hlseqgen_sdff_commands
 */
typedef struct {
    std::string result;
    int command_class;
} sdff_hwcmd_result;


/** \brief this class manages lists of commands that will be sent to hardware other than the synchronized output hardware.
 * \ingroup hlseqgen_sdff_commands
 *
 * This class collects a list of commands that can be sent to hardware other than the synchronized output
 * hardware which is controller with sequencer::sequenceGenerator descendents and sequencer::SDFFelementManager.
 * This is used to control GPIB serial interfaces.
 *
 *
 * \section SDFFHWcommandManager_property_sec Basic Command Properties
 * For each hardware type a list is created that contains all the commands that were issued during the execution
 * of the SDFF sequence. A specialized hardware driver class may then read the list and really send the commands.
 * The type of the hardware is identified by a string like \c "gpib" for GPIB instructions. Each command is represented
 * by a class that contains the data which specifies it. These classes are all descendents of sequencer::SDFFcommand.
 * They contain at least these data:
 *   - time of execution in microseconds (or -1 if it is to be executed before the sequence runs)
 *   - sdff file where the command appears and line therein.
 *   - a command class that specifies the type of the command (as an unsigned int)
 * .
 *
 *
 * \section SDFFHWcommandManager_commandclass_sec Command Classes
 * Each command is part of one of the possible command classes. These classes are internally represented
 * by an unsigned integer number. Each SDFFcommand has a property called command_class that specifies this
 * class for the given object. It is not possible to change this class externally. It has to be set in the
 * constructor of the descendent. So if you want to add new commands, do the following:
 *   # derive new classes from SDFFcommand that describe your new commands.
 *   # add a #define that specifies the new command class and do not forget to the the command_class property
 *     of your new classes to this value.
 *   # add the define to the command_classtostring() method in sdfftools.cpp
 *   # write some driver component that reads the commands with the new class from a SDFFHWcommandManager object
 *     and incorporate it in your application.
 *   # Finally you should add some SDFF methods to sdfffunctions.cpp and sdfffunctions.h that generate items in
 *     the SDFFHWcommandManager object that belongs to the SDFFparser object you use.
 * .
 *
 */
class SDFFHWcommandManager {
  protected:
    /** \brief hidden constructor */
    SDFFHWcommandManager() {};

    /** \brief duration of one sample in microseconds */
    double sample_timestep;

    /** \brief parser object used for error output */
    SDFFparser* parser;

    /** \brief list of all commands, maps from command group id to a list (vector) of the commands to be executed */
    std::map<int, std::vector<SDFFcommand*> > commands;
  public:
    /** \brief class constructor */
    SDFFHWcommandManager(SDFFparser* parser, double sample_timestep=-1) {
      this->parser=parser;
      this->sample_timestep=sample_timestep;
    }

    /** \brief class destructor */
    virtual ~SDFFHWcommandManager() {
    }

    /** \brief set the parser object */
    SetMacro(SDFFparser*, parser);
    /** \brief get the parser object */
    GetMacro(SDFFparser*, parser);

    /** \brief set the duration of one sample in microseconds */
    SetMacro(double, sample_timestep);
    /** \brief get the duration of one sample in microseconds */
    GetMacro(double, sample_timestep);

    /** \brief clear all commands */
    void clear();

    /** \brief return a report of all stored commands */
    std::string report();

    /** \brief return a report of all stored commands with their results. Commands only appear when
      *        they really returned a result.
      */
    std::string report_results();

    /** \brief clears all stored results */
    void clear_results();

    /** \brief add a serial port command */
    inline void add_command(SDFFcommand* cmd) { commands[cmd->get_command_class()].push_back(cmd); };

    /** \brief get the number of serial port commands */
    inline unsigned long get_command_count(unsigned int command_class) { return commands[command_class].size(); };

    /** \brief get the \a id th command from the given \a command_class */
    inline SDFFcommand* get_command(unsigned int command_class, unsigned int id) {
      unsigned int cnt=get_command_count(command_class);
      if (id<cnt) {
        return commands[command_class][id];
      }
      SEQUENCER_ERRORN(SEQUENCER_ERROR_RANGE_NUM, format("SDFFHWcommandManager::get_command(%d, %d)", command_class, id));
    };

    std::vector<sdff_hwcmd_result> get_result_list();
};
}

#endif // SDFFHWCOMMANDS_H


/*@}*/

