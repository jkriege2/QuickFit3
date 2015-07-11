/*
  Name: vartimingdriver.h
  Copyright: (c) 2008
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/

/** \file vartimingdriver.h
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


#ifndef VARTIMINGDRIVER_H
#define VARTIMINGDRIVER_H


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
 * \brief implements a driver class for the variable-timing hardware (using windows system calls from windows.h)
 * \ingroup additional_drivers
 *
 * Here are some links with further information about serial interfaces on windows systems:
 *   - <a href="http://www.codeproject.com/KB/system/chaiyasit_t.aspx">http://www.codeproject.com/KB/system/chaiyasit_t.aspx</a>
 *   - <a href="http://www.zeiner.at/c/serialport.html">http://www.zeiner.at/c/serialport.html</a>
 *   - <a href="http://www.codeproject.com/KB/system/SerialPortComm.aspx">http://www.codeproject.com/KB/system/SerialPortComm.aspx</a>
 * .
 */
class varTimingDriver
{
    protected:
        /** \brief this is a handle for the serial port the interface is connected to*/
        HANDLE port;

        std::string com_port;

        /** \brief indicates whether variable timing is available */
        bool available;

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

        void read_basic_ini();
	public:
        /** \brief class constructor */
        varTimingDriver() {
            inifile="variable-timing.ini";
            port=INVALID_HANDLE_VALUE;
            com_port="COM1";
            available=false;
            read_basic_ini();
        };

        /** \brief class destructor */
        virtual ~varTimingDriver() {
            close();
        };

        /** \brief returns whether variable timing is available */
        GetMacro(bool, available);
        /** \brief returns the INI file that configures the serial-to-GPIB interface */
        GetMacro(std::string, inifile);
        /** \brief sets the INI file that configures the serial-to-GPIB interface */
        inline void set_inifile(std::string _inifile) {
            inifile=_inifile;
            read_basic_ini();
        }


        /** \brief open the serial port to the serial-to-GPIB interface. If the port is already opened: do nothing) */
        virtual void open();

        /** \brief close the serial port to the serial-to-GPIB interface */
        inline virtual void close() {
            if (port!=INVALID_HANDLE_VALUE) CloseHandle(port);
            port=INVALID_HANDLE_VALUE;
        };

        inline virtual bool start() {
            open();
            emptyPortBuffer();
            writePort("s");
            std::string d=readPort_until();
            if (d.find("1\n")!=std::string::npos) return true;
            return false;
        }

        inline virtual bool reset() {
            open();
            emptyPortBuffer();
            writePort("r");
            std::string d=readPort_until();
            if (d.find("1\n")!=std::string::npos) return true;
            return false;
        }

        inline virtual bool is_done() {
            open();
            emptyPortBuffer();
            writePort("d");
            std::string d=readPort_until();
            if (d.find("1\n")!=std::string::npos) return true;
            return false;
        }

};


};



#endif // VARTIMINGDRIVER_H





