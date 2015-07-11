/*
  Name: sergpibdriver.h
  Copyright: (c) 2008
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/

/** \file sergpibdriver.h
 *  \ingroup additional_drivers
 */

#include <string>
#include <map>
#include <vector>
#include <cstdio>
#include <iostream>

#include <cstdlib>
#include <ctime>
#include <cmath>
#include <unistd.h>
#include <cstring>

#include <windows.h>

#include "tools.h"
#include "sequencererror.h"
#include "sdffhwcommands.h"

#ifndef SERGPIBDRIVER_H
#define SERGPIBDRIVER_H


namespace sequencer {
#ifndef __WINDOWS__
#if defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32)
#define __WINDOWS__
#endif
#endif

#ifndef __WINDOWS__
#error("this class needs to be compiled under win32")
#endif

/**
 * \brief implements a driver class for serial-to-GPIB interface (using windows system calls from windows.h)
 * \ingroup additional_drivers
 *
 * This class relies on a SDFFHWcommandManager instance. From this it reads the commands to be executed,
 * using execute(). The class manages internally a map of all ports that are currently in use (opened).
 *
 * Here are some links with further information about serial interfaces on windows systems:
 *   - <a href="http://www.codeproject.com/KB/system/chaiyasit_t.aspx">http://www.codeproject.com/KB/system/chaiyasit_t.aspx</a>
 *   - <a href="http://www.zeiner.at/c/serialport.html">http://www.zeiner.at/c/serialport.html</a>
 *   - <a href="http://www.codeproject.com/KB/system/SerialPortComm.aspx">http://www.codeproject.com/KB/system/SerialPortComm.aspx</a>
 * .
 */
class serGPIBDriver
{
    protected:
        /** \brief this is a handle for the serial port the interface is connected to*/
        HANDLE port;

        std::string com_port;

        /** \brief indicates whether this driver is enabled (read from INI file) */
        bool enabled;

        /** \brief filename of the INI file specifying the interface */
        std::string inifile;

        /** \brief throws an exception of the last win32 API error. */
        void win32_error(std::string where);

        /** \brief write to the specified serial port */
        virtual void writePort(std::string data);

        /** \brief reads from the specified serial port und \a end_char occurs*/
        virtual std::string readPort_until(char end_char='\n');

        /** \brief empty the windows serial port buffer */
        void emptyPortBuffer();
	public:
        /** \brief class constructor */
        serGPIBDriver() {
            inifile="serial-gpib.ini";
            port=INVALID_HANDLE_VALUE;
            com_port="COM1";
            enabled=true;
        };

        /** \brief class destructor */
        virtual ~serGPIBDriver() {
            close();
        };

        /** \brief returns the INI file that configures the serial-to-GPIB interface */
        GetMacro(std::string, inifile);
        /** \brief sets the INI file that configures the serial-to-GPIB interface */
        SetMacro(std::string, inifile);

        GetMacro(bool, enabled);

        /** \brief enables or disables this driver. If the state changes from enabled to disabled,
         *         the connection is closed.
         */
        void set_enabled(bool value) {
            if (!value && enabled) close();
            enabled=value;
        };


        /** \brief open the serial port to the serial-to-GPIB interface. If the port is already opened: do nothing) */
        virtual void open();

        /** \brief close the serial port to the serial-to-GPIB interface */
        inline virtual void close() {
            if (port!=INVALID_HANDLE_VALUE) CloseHandle(port);
            port=INVALID_HANDLE_VALUE;
        };

        /** \brief writes \a data to the specified GPIB \a device */
        inline virtual void send(char device, std::string data);

        /** \brief writes \a data to the specified GPIB \a device */
        virtual void sendCommand(char device, unsigned int command);

        /** \brief writes \a data to the specified GPIB \a device and then reads back the answer (query operation) */
        virtual std::string query(char device, std::string data);

        /** \brief reads from the specified GPIB \a device */
        virtual std::string read(char device);

        /** \brief configure REN_talkstate of interface */
        inline virtual void config_REN_talkstate(bool REN_talkstate){
            open();
            if (REN_talkstate) writePort("oRET1"); else writePort("oRET0");
        };

        /** \brief configure REN_readstate of interface */
        inline virtual void config_REN_readstate(bool REN_readstate){
            open();
            if (REN_readstate) writePort("oRER1"); else writePort("oRER0");
        };

        /** \brief configure send_LB of interface */
        inline virtual void config_send_LB(bool send_LB){
            open();
            if (send_LB) writePort("oSLB1"); else writePort("oSLB0");
        };

        /** \brief configure interface_address of interface */
        inline virtual void config_interface_address(char interface_address){
            open();
            writePort(format("oMAD%02d", interface_address));
        };

        /** \brief configure query_delay of interface */
        inline virtual void config_query_delay(unsigned char query_delay) {
            open();
            writePort(format("oQUD%03d", query_delay));
        };

        /** \brief configure use_EOI of interface */
        inline virtual void config_use_EOI(bool use_EOI) {
            open();
            if (use_EOI) writePort("oEOI1"); else writePort("oEOI0");
        };

        /** \brief enables or disables (depending in \a state ) the specified interrupt channel */
        inline virtual void config_enable_interrupt(char interrupt, bool state) {
            open();
            std::string s="i";
            if (interrupt==0) s+="0"; else s+="1";
            if (state) writePort(s+"1"); else writePort(s+"0");
        };

        /** \brief configure the interrupt channels of the serial-to-GPIB interface */
        virtual void config_interrupt(char device, char interrupt, std::string command, bool send_GTL=true, bool send_UNL=true);

        /** \brief executes all serial-to-GPIB commands from a SDFFHWcommandManager instance. */
        virtual void execute(SDFFHWcommandManager* hwc_manager);

};


};



#endif // SERGPIBDRIVER_H




