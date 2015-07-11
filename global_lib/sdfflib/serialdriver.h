/*
  Name: serialdriver.h
  Copyright: (c) 2008
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/

/** \file serialdriver.h
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


#ifndef SERIALDRIVER_H
#define SERIALDRIVER_H


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
 * \brief implements a driver class for serial ports (using windows system calls from windows.h)
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
class serialDriver
{
    protected:
        /** \brief this map contains port handles for all currently used (opened) ports */
        std::map<std::string, HANDLE> ports;

        /** \brief throws an exception of the last win32 API error. */
        void win32_error(std::string where);
	public:
        /** \brief class constructor */
        serialDriver() {
        }

        /** \brief class destructor */
        virtual ~serialDriver() {
            if (ports.size()>0) {
                for (std::map<std::string, HANDLE>::iterator i=ports.begin(); i!=ports.end(); ++i) {
                    CloseHandle(i->second);
                }
            }
            ports.clear();
        }

        /** \brief opens the specified serial port */
        virtual void open(std::string com_port=std::string("COM1"), long baud_rate=9600, int databits=8, sdff_serial_paritymodes parity=sdffserNOPARITY, double stopbits=1, sdff_serial_handshaking handshaking=sdffserNOHANDSHAKING);

        /** \brief closes the specified serial port */
        virtual void close(std::string com_port=std::string("COM1"));

        /** \brief writes \a data to the specified serial port */
        virtual void write(std::string com_port, std::string data);

        /** \brief reads \a num_bytes bytes from the specified serial port */
        virtual std::string read(std::string com_port, size_t num_bytes);

        /** \brief reads until \a end_char is reached from the specified serial port */
        virtual std::string read_until(std::string com_port, char end_char);

        /** \brief reads all characters currently in the input buffer and discards them */
        virtual void empty_buffer(std::string com_port);

        /** \brief executes all serial commands from a SDFFHWcommandManager instance. */
        virtual void execute(SDFFHWcommandManager* hwc_manager);

};


};



#endif // SERIALDRIVER_H



