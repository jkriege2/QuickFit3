/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef QFSERIALCONNECTION_H
#define QFSERIALCONNECTION_H







#ifndef __WINDOWS__
# if defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32)
#  define __WINDOWS__
# endif
#endif

#ifndef __LINUX__
# if defined(linux)
#  define __LINUX__
# endif
#endif

#ifndef __WINDOWS__
# ifndef __LINUX__
#  error("this class needs to be compiled under win32 or linux")
# endif
#endif



#include <string>
#include <cstdio>
#include <iostream>

#include <fcntl.h>
#include <unistd.h>
#include <cerrno>

#include <cstdlib>
#include <cstring>
#include "cpptools.h"

#if defined(__WINDOWS__) && defined(__LINUX__)
# error("you may compile this for EITHER windows (define __WINDOWS__) OR linux (define __LINUX__), but both symbols are defined at the same time!!!")
#endif

#ifdef __WINDOWS__
# include <windows.h>
# define QFSCDEFAULTPORT std::string("COM1")
#endif

#ifdef __LINUX__
# include <termios.h>
# define QFSCDEFAULTPORT std::string("/dev/ttys0")
#endif



/*! \defgroup jkhardware Classes for Hardware Control and Communication
    \ingroup tools

    Classes in this group implement interfaces to hardware, like the serial port, ...

 */

/*! \defgroup jkserial Classes for Hardware serial connections (RS-232)
    \ingroup jkhardware

 */

/** \file qfserialconnection.h
 *  \ingroup jkserial
 */



/** \brief this specifies the possible numbers of databits for a connection
 *  \ingroup jkserial
 */
enum QFSCdatabits {
    QFSC5databits=5,
    QFSC6databits=6,
    QFSC7databits=7,
    QFSC8databits=8,
#if defined(__WINDOWS__) || defined(CS9)
    QFSC9databits=9,
#endif
};

/** \brief convert a JKSCdatabits into a string containing the number of databits
 *  \ingroup jkserial
 */
std::string QFSCdatabits2string(QFSCdatabits databits);

/** \brief read a string containig a number of databits to a JKSCdatabits
 *  \ingroup jkserial
 */
QFSCdatabits string2QFSCdatabits(std::string databits);

/** \brief this specifies the possible handshaking types for a serial connection
 *  \ingroup jkserial
 */
enum QFSChandshaking {
    QFSCnoHandshaking,
    QFSCxonXoffHandshaking,
    QFSChardwareHandshakingDTRRTS,
    QFSChardwareHandshakingRTS
};

/** \brief convert a JKSChandshaking into a string
 *  \ingroup jkserial
 */
std::string QFSChandshaking2string(QFSChandshaking d);

/** \brief read a string containig a handshaking descriptor to a JKSChandshaking
 *  \ingroup jkserial
 */
QFSChandshaking string2QFSChandshaking(std::string dd);

/** \brief this specifies the possible parity modes for a serial connection
 *  \ingroup jkserial
 */
enum QFSCparity {
    QFSCevenParity,
    QFSCoddParity,
    QFSCnoParity
};

/** \brief convert a JKSCparity into a string
 *  \ingroup jkserial
 */
std::string QFSCparity2string(QFSCparity d);

/** \brief read a string containig a parity type descriptor to a JKSCparity
 *  \ingroup jkserial
 */
QFSCparity string2QFSCparity(std::string dd);

/** \brief this specifies the possible numbers of stop bits for a connection
 *  \ingroup jkserial
 */
enum QFSCstopbits {
    QFSConeStopbit,
    QFSCone5Stopbits,
    QFSCtwoStopbits
};

/** \brief convert a JKSCparity into a string
 *  \ingroup jkserial
 */
std::string QFSCstopbits2string(QFSCstopbits d);

/** \brief read a string to a JKSCstopbits
 *  \ingroup jkserial
 */
QFSCstopbits string2QFSCstopbits(std::string dd);



/*! \brief implements a serial data connection over a serial port (RS-232) for Windows and Linux systems
    \ingroup jkserial

    Here are some links with further information about serial interfaces on windows systems:
      - <a href="http://www.codeproject.com/KB/system/chaiyasit_t.aspx">http://www.codeproject.com/KB/system/chaiyasit_t.aspx</a>
      - <a href="http://www.zeiner.at/c/serialport.html">http://www.zeiner.at/c/serialport.html</a>
      - <a href="http://www.codeproject.com/KB/system/SerialPortComm.aspx">http://www.codeproject.com/KB/system/SerialPortComm.aspx</a>
      - <a href="http://msdn.microsoft.com/en-us/library/aa363214%28v=VS.85%29.aspx">MSDN: DCB Structure</a>
      - <a href="http://msdn.microsoft.com/en-us/library/aa365467%28VS.85%29.aspx">MSDN: ReadFile function</a>
      - <a href="http://notes.ump.edu.my/fkee/e-Books/C%20Programming%20&%20PC%20interfacing/Serial%20port%20programming%20for%20Windows%20and%20Linux.pdf">Serial Port Programming in Windows and Linux</a>
      - <a href="http://www.devx.com/tips/Tip/38650">list serial ports</a>
    .

    And finally, here are some links with further information about serial interfaces on Linux systems:
      - <a href="http://notes.ump.edu.my/fkee/e-Books/C%20Programming%20&%20PC%20interfacing/Serial%20port%20programming%20for%20Windows%20and%20Linux.pdf">Serial Port Programming in Windows and Linux</a>
      - <a href="http://www.easysw.com/~mike/serial/serial.html">Serial Programming Guide for POSIX Operating Systems</a>
      - <a href="http://www.opengroup.org/onlinepubs/007908799/xsh/open.html">open() documentation</a>
      - <a href="http://www.opengroup.org/onlinepubs/007908799/xsh/close.html">close() documentation</a>
      - <a href="http://www.opengroup.org/onlinepubs/007908799/xsh/tcflush.html">tcflush() documentation</a>
      - <a href="http://www.opengroup.org/onlinepubs/007908799/xsh/read.html">read() documentation</a>
      - <a href="http://www.opengroup.org/onlinepubs/007908799/xsh/write.html">write() documentation</a>
      - <a href="http://www.opengroup.org/onlinepubs/007908799/xsh/select.html">select() documentation</a>
      - <a href="http://linux.about.com/library/cmd/blcmdl3_tcsetattr.htm">documentation for tcsetattr() and how to enable/disable special characters!</a>
    .

    \example jkserial_test/jkserial_test.cpp

 */
class QFLIB_EXPORT QFSerialConnection
{
    public:
        /** \brief class constructor.
         *
         *  This constructs a class and sets the internal properties to the given values.
         *  \param port the serial port to open (on Windows: COM1, ... on Linux: /dev/ttys0, ...)
         *  \param baudrate baudrate of the connection in BAUD
         *  \param databits number of databits for the connection
         *  \param parity parity mode for the connection
         *  \param stopbits number of stopits for the connection
         *  \param handshaking type of handshaking for the connection
         *
         *  \note This constructor does NOT open a connection. To do so, you will have to call open()
         */
        QFSerialConnection(std::string port=QFSCDEFAULTPORT, unsigned int baudrate=9600, QFSCdatabits databits=QFSC8databits, QFSCparity parity=QFSCnoParity, QFSCstopbits stopbits=QFSConeStopbit, QFSChandshaking handshaking=QFSCnoHandshaking);

        /** \brief class destructor. This also closes the connection if it is still open. */
        virtual ~QFSerialConnection();

        /** \brief open a connection with the given parameters
         *
         *  \param port the serial port to open (on Windows: COM1, ... on Linux: /dev/ttys0, ...)
         *  \param baudrate baudrate of the connection in BAUD
         *  \param databits number of databits for the connection
         *  \param parity parity mode for the connection
         *  \param stopbits number of stopits for the connection
         *  \param handshaking type of handshaking for the connection
         *  \return \c true if the connection has been opened successfully. If the returnvalue is \c false, an errordescription may be obtained via getLastError()!
         *
         */
        bool open(std::string port, unsigned int baudrate=9600, QFSCdatabits databits=QFSC8databits, QFSCparity parity=QFSCnoParity, QFSCstopbits stopbits=QFSConeStopbit, QFSChandshaking handshaking=QFSCnoHandshaking);

        /** \brief open a connection, with the current parameters
         *
         *  \return \c true if the connection has been opened successfully. If the returnvalue is \c false, an errordescription may be obtained via getLastError()!
         */
        bool open();

        /** \brief closes the current connection, if one is opened. Afterwards isConnectionOpen() is guaranteed to return \c true.
         *
         *  \return \c true if the connection has been opened successfully. If the returnvalue is \c false, an errordescription may be obtained via getLastError()!
         */
        bool close();

        /** \brief writes \a data to the serial port
         *
         *  \return \c true if the connection has been opened successfully. If the returnvalue is \c false, an errordescription may be obtained via getLastError()!
         */
        bool write(std::string data);

        /** \brief writes \a data to the serial port
         *
         *  \return \c true if the connection has been opened successfully. If the returnvalue is \c false, an errordescription may be obtained via getLastError()!
         */
        bool write(uint8_t* data, int count);

        /** \brief reads \a num_bytes bytes from the serial port
         *
         *  \param[in] num_bytes number of bytes to read
         *  \param[out] ok if \c ok!=NULL then \a ok will afterwards contain \c true if the operation was successfull.  If the returnvalue is \c false, an errordescription may be obtained via getLastError()!
         *  \return the read string
         */
        std::string read(size_t num_bytes, bool* ok=NULL);

        /** \brief reads \a num_bytes bytes from the serial port
         *
         *  \param[in] num_bytes number of bytes to read
         *  \param[out] ok if \c ok!=NULL then \a ok will afterwards contain \c true if the operation was successfull.  If the returnvalue is \c false, an errordescription may be obtained via getLastError()!
         *  \return the read string
         */
        bool read(uint8_t* data, size_t num_bytes);
        /** \brief reads one character from the serial port, internally calls read(char*)
         *
         *  \param[out] ok if \c ok!=NULL then \a ok will afterwards contain \c true if the operation was successfull.  If the returnvalue is \c false, an errordescription may be obtained via getLastError()!
         *  \return the read character
         */
        inline char read(bool* ok=NULL) {
            char ch=0;
            bool okk=read(&ch);
            if (ok) *ok=okk;
            return ch;
        }


        /** \brief reads one character from the serial port, works like read(bool*), but with another interface
         *
         *  \param[out] ch afterwards contains the character read from the port
         *  \return \c true on success or false otherwise
         */
        bool read(char* ch);

        /** \brief reads one character from the serial port, works like read(bool*), but with another interface
         *
         *  \param[out] ch afterwards contains the character read from the port
         *  \return \c true on success or false otherwise
         */
        bool read_nowait(char* ch);

        /** \brief reads until \a end_char is reached from the serial port
         *
         *  \param[in] end_char read until this character occurs
         *  \param[out] ok if \c ok!=NULL then \a ok will afterwards contain \c true if the operation was successfull.  If the returnvalue is \c false, an errordescription may be obtained via getLastError()!
         *  \return the read string
         */
        std::string readUntil(char end_string, bool* ok=NULL);

        /** \brief reads until \a end_char is reached from the serial port
         *
         *  \param[in] end_string read until these characters occurs
         *  \param[out] ok if \c ok!=NULL then \a ok will afterwards contain \c true if the operation was successfull.  If the returnvalue is \c false, an errordescription may be obtained via getLastError()!
         *  \return the read string
         */
        std::string readUntil(std::string end_string, bool* ok=NULL);
        /** \brief reads until \a end_char is reached from the serial port
         *
         *  \param[out] result array to write result to
         *  \param[in] result_maxsize maximum size of array
         *  \param[in] end_string read until these characters occurs, size of end_string array is end_string_size
         *  \param[in] end_string_size size of end_string array
         *  \param[out] ok if \c ok!=NULL then \a ok will afterwards contain \c true if the operation was successfull.  If the returnvalue is \c false, an errordescription may be obtained via getLastError()!
         *  \return the read string
         */
        int readUntil(char* result, int result_maxsize, const char* end_string, int end_string_size, bool* ok=NULL);

        /** \brief reads all characters currently in the input buffer and discards them
         *
         *  \return \c true if the connection has been opened successfully. If the returnvalue is \c false, an errordescription may be obtained via getLastError()!
         */
        bool clearBuffer();




        /** \brief return a description of the last error that occured */
        std::string getLastError() { return lastError; };

        /** \brief returns \c true if an error has occured */
        bool hasErrorOccured() { return errorOccured; };

        /** \brief returns \c true if the connectio  is open */
        inline bool isConnectionOpen() { return connectionOpen; };

        /** \brief returns a list of all serial ports available to the system */
        static std::vector<std::string> listPorts();

        void resetError();



        GET_SET_MACRO_I(bool, binary, reopen())
        GET_SET_MACRO_I(bool, logToFile, reopen())
        GET_SET_MACRO_I(std::string, port, reopen())
        GET_SET_MACRO_I(QFSCdatabits, databits, reopen())
        GET_SET_MACRO_I(QFSChandshaking, handshaking, reopen())
        GET_SET_MACRO_I(unsigned int, baudrate, reopen())
        GET_SET_MACRO_I(QFSCparity, parity, reopen())
        GET_SET_MACRO_I(QFSCstopbits, stopbits, reopen())
        GET_SET_MACRO_I(unsigned int, timeout_ms, reopen())
    protected:
        /** \brief the timeout period for read operations in milliseconds */
        unsigned int timeout_ms;
        /** \brief indicates whether a connection is opened */
        bool connectionOpen;
        /** \brief the serial port to use (\c COM1, \c COM2, ... on windows or \c /dev/ttys0, ... on linux */
        std::string port;
        /** \brief number of databits in one word, transfered via the serial port */
        QFSCdatabits databits;
        /** \brief the type of handshaking used for the connection */
        QFSChandshaking handshaking;
        /** \brief the baudrate of the connection */
        unsigned int baudrate;
        /** \brief the parity configuration for the connection */
        QFSCparity parity;
        /** \brief number of stop bits for the serial connection */
        QFSCstopbits stopbits;
        /** \brief description of the last error */
        std::string lastError;
        /** \brief contains \c true if error occured */
        bool errorOccured;

        /** \brief close current connection and reopen with the internal parameters, if a connection has been open when the function was called */
        bool reopen();

        #ifdef __WINDOWS__
        /** \brief used for error reporting on windows
         *
         * This sets errorOccured to \c true and writes an error description to lastError
         * \param where contains a string that specifies the function where the error occured
         */
        void win32_error(std::string where);

        /** \brief windows API port handle for the serial port */
        HANDLE portHandle;
        #endif
        #ifdef __LINUX__
        /** \brief POSIX API port handle for the serial port */
        int unixPortHandle;
        #endif

        /** \brief if set \c true in the constructor the class will generate a file \c rs232.log and output all communication to this file! */
        bool logToFile;
        FILE* log;

        bool binary;
};

#endif // QFSERIALCONNECTION_H
