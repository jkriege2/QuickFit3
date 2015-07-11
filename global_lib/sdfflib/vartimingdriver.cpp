
/*
  Name: vartimingdriver.cpp
  Copyright: (c) 2008
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/

#include "vartimingdriver.h"
#include "jkiniparser.h"

namespace sequencer {


void varTimingDriver::win32_error(std::string where){
    /* return the windows error message for an error code:
      taken and modified from http://msdn2.microsoft.com/en-us/library/ms701797.aspx */
    char* pMsgBuf = NULL;
    DWORD dwErrorCode=GetLastError();

    // Retrieve the Win32 error message.
    DWORD dwSize = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
                           NULL, dwErrorCode, 0, (LPTSTR) &pMsgBuf, 0, NULL);

    // Return the description and size to the caller in the OUT parameters.
    if (dwSize>0)
    {
        SEQUENCER_ERRORV(SEQUENCER_SERERROR_WIN32ERROR_NUM, where, where.c_str(), pMsgBuf);
        LocalFree(pMsgBuf);
    } else {
        SEQUENCER_ERRORV(SEQUENCER_SERERROR_WIN32ERROR_NUM, where, where.c_str(), ("errorcode "+inttostr(dwErrorCode)+": sorry, no description available").c_str());
    }
};

void varTimingDriver::emptyPortBuffer(){
    std::string name="varTimingDriver::emptyPortBuffer()";
    if (port==INVALID_HANDLE_VALUE) {
        SEQUENCER_ERRORV(SEQUENCER_SERERROR_PORTNOTOPEN_NUM, name, com_port.c_str());
    }
    unsigned long dwNumberOfBytesRec=1;
    char d=0;
    while(dwNumberOfBytesRec!=0) {
        if (ReadFile(port, &d, 1, &dwNumberOfBytesRec, NULL) != 0) {
        } else {
            CloseHandle(port);
            port=INVALID_HANDLE_VALUE;
            win32_error(name);
        }
    }
}


void varTimingDriver::writePort(std::string data){
    std::string name="varTimingDriver::writePort(data='"+toprintablestr(data)+"')";
    if (port==INVALID_HANDLE_VALUE) {
        SEQUENCER_ERRORV(SEQUENCER_SERERROR_PORTNOTOPEN_NUM, name, com_port.c_str());
    }

    unsigned long dwNumberOfBytesSent = 0;
    while(dwNumberOfBytesSent < data.size()) {
        unsigned long dwNumberOfBytesWritten;
        char c=data[dwNumberOfBytesSent];
        if (WriteFile(port, &c, 1, &dwNumberOfBytesWritten, NULL) != 0) {
            if(dwNumberOfBytesWritten > 0)
                ++dwNumberOfBytesSent;
        } else {
            CloseHandle(port);
            port=INVALID_HANDLE_VALUE;
            win32_error(name);
        }
    }
};

std::string varTimingDriver::readPort_until(char end_char){
    std::string name="varTimingDriver::readPort_until(end_char='"+chartoprintablestr(end_char)+"')";
    std::string res="";
    if (port==INVALID_HANDLE_VALUE) {
        SEQUENCER_ERRORV(SEQUENCER_SERERROR_PORTNOTOPEN_NUM, name, com_port.c_str());
    }
    unsigned long dwNumberOfBytesRecvd = 0;
    char d='a'; // just a dummy value
    while(d!=end_char) {
        unsigned long dwNumberOfBytesRec;
        if (ReadFile(port, &d, 1, &dwNumberOfBytesRec, NULL) != 0) {
            if(dwNumberOfBytesRec > 0) {
                ++dwNumberOfBytesRecvd;
                res+=d;
            }
        } else {
            CloseHandle(port);
            port=INVALID_HANDLE_VALUE;
            win32_error(name);
        }
    }
    return res;
};

void varTimingDriver::read_basic_ini(){
    jkiniparser ini;
    try {
       ini.readFile(inifile); // parse serial_gpib.ini file
    } catch (jkipError e) {
       SEQUENCER_ERROR(SEQUENCER_ERROR_INI_NUM, e.getMessage(), "varTimingDriver::read_basic_ini()");
    }
    available=ini.getAsBool("config", "enabled", "true");
}

void varTimingDriver::open(){
    if (port!=INVALID_HANDLE_VALUE) return; // port already open

    std::string method="varTimingDriver::open()";
    read_basic_ini();

    if (!available) {
        SEQUENCER_ERRORN(SEQUENCER_ERROR_NOVARTIMING_NUM, method.c_str());
    }
    jkiniparser ini;
    try {
       ini.readFile(inifile); // parse serial_gpib.ini file
    } catch (jkipError e) {
       SEQUENCER_ERROR(SEQUENCER_ERROR_INI_NUM, e.getMessage(), method.c_str());
    }
    com_port=ini.getAsString("config", "port", "COM1");

    BOOL fSuccess;
    DCB dcb;      /* device control block */

    port = CreateFileA(com_port.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (port==INVALID_HANDLE_VALUE) {
        port=INVALID_HANDLE_VALUE;
        win32_error(method);
    }
    fSuccess = GetCommState(port, &dcb);
    if (!fSuccess) {
        CloseHandle(port);
        port=INVALID_HANDLE_VALUE;
        win32_error(method);
    }

    /* configure the port */
    dcb.BaudRate = ini.getAsInt("config", "baudrate", 115200);
    dcb.ByteSize = ini.getAsInt("config", "databits", 8);
    std::string par=tolower(ini.getAsString("config", "parity", "none"));
    if (par=="none") dcb.Parity = NOPARITY;
    else if (par=="even") dcb.Parity = EVENPARITY;
    else if (par=="odd") dcb.Parity = ODDPARITY;
    std::string sb=tolower(ini.getAsString("config", "stopbits", "1"));
    if (sb=="1") dcb.StopBits = ONESTOPBIT;
    else if (sb=="1.5") dcb.StopBits = ONE5STOPBITS;
    else if (sb=="2") dcb.StopBits = TWOSTOPBITS;

    std::string hs=tolower(ini.getAsString("config", "handshaking", "none"));
	if (hs=="none") {
		dcb.fOutxCtsFlow = false;					// Disable CTS monitoring
		dcb.fOutxDsrFlow = false;					// Disable DSR monitoring
		dcb.fDtrControl = DTR_CONTROL_DISABLE;		// Disable DTR monitoring
		dcb.fOutX = false;							// Disable XON/XOFF for transmission
		dcb.fInX = false;							// Disable XON/XOFF for receiving
		dcb.fRtsControl = RTS_CONTROL_DISABLE;		// Disable RTS (Ready To Send)
	} else if (hs=="hardware") {
		dcb.fOutxCtsFlow = true;					// Enable CTS monitoring
		dcb.fOutxDsrFlow = true;					// Enable DSR monitoring
		dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;	// Enable DTR handshaking
		dcb.fOutX = false;							// Disable XON/XOFF for transmission
		dcb.fInX = false;							// Disable XON/XOFF for receiving
		dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;	// Enable RTS handshaking
	} else if (hs=="software" || hs=="xonxoff") {
		dcb.fOutxCtsFlow = false;					// Disable CTS (Clear To Send)
		dcb.fOutxDsrFlow = false;					// Disable DSR (Data Set Ready)
		dcb.fDtrControl = DTR_CONTROL_DISABLE;		// Disable DTR (Data Terminal Ready)
		dcb.fOutX = true;							// Enable XON/XOFF for transmission
		dcb.fInX = true;							// Enable XON/XOFF for receiving
		dcb.fRtsControl = RTS_CONTROL_DISABLE;		// Disable RTS (Ready To Send)
	}

    fSuccess = SetCommState(port, &dcb);
    if (!fSuccess) {
        CloseHandle(port);
        port=INVALID_HANDLE_VALUE;
        win32_error(method);
    }

    COMMTIMEOUTS commTimeout;

    if(GetCommTimeouts(port, &commTimeout))
    {
        commTimeout.ReadIntervalTimeout     = 500;
        commTimeout.ReadTotalTimeoutConstant     = 100;
        commTimeout.ReadTotalTimeoutMultiplier     = 500;
        commTimeout.WriteTotalTimeoutConstant     = 100;
        commTimeout.WriteTotalTimeoutMultiplier = 500;
    } else {
        CloseHandle(port);
        port=INVALID_HANDLE_VALUE;
        win32_error(method);
    }


    if(!SetCommTimeouts(port, &commTimeout)) {
        CloseHandle(port);
        port=INVALID_HANDLE_VALUE;
        win32_error(method);
    }
};



}

