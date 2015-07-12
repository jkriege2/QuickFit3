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

#include "qfserialconnection.h"
#include <stdint.h>

std::string QFSCdatabits2string(QFSCdatabits databits) {
    return inttostr(databits);
}

QFSCdatabits string2QFSCdatabits(std::string databits) {
    std::string d=strstrip(databits);
    if (d=="5") return QFSC5databits;
    if (d=="6") return QFSC6databits;
    if (d=="7") return QFSC7databits;
    if (d=="8") return QFSC8databits;
#if defined(__WINDOWS__) || defined(CS9)
    if (d=="9") return QFSC9databits;
#endif
    return QFSC8databits;
}

std::string QFSChandshaking2string(QFSChandshaking d) {
    if (d==QFSCnoHandshaking) return "no_handshaking";
    if (d==QFSCxonXoffHandshaking) return "xonxoff_handshaking";
    if (d==QFSChardwareHandshakingDTRRTS) return "hardware_handshaking";
    return "";
}

QFSChandshaking string2QFSChandshaking(std::string dd) {
    std::string d=tolower(strstrip(dd));
    if (d=="no_handshaking" || d=="none") return QFSCnoHandshaking;
    if (d=="xonxoff_handshaking" || d=="xonxoff") return QFSCxonXoffHandshaking;
    if (d=="hardware_handshaking" || d=="hardware") return QFSChardwareHandshakingDTRRTS;
    return QFSCnoHandshaking;
}

std::string QFSCparity2string(QFSCparity d) {
    if (d==QFSCevenParity) return "even_parity";
    if (d==QFSCoddParity) return "odd_parity";
    if (d==QFSCnoParity) return "no_parity";
    return "";
}

QFSCparity string2QFSCparity(std::string dd) {
    std::string d=tolower(strstrip(dd));
    if (d=="no_parity" || d=="none") return QFSCnoParity;
    if (d=="odd_parity" || d=="odd") return QFSCoddParity;
    if (d=="even_parity" || d=="even") return QFSCevenParity;
    return QFSCnoParity;
}

std::string QFSCstopbits2string(QFSCstopbits d) {
    if (d==QFSConeStopbit) return "one_stopbit";
    if (d==QFSCone5Stopbits) return "one5_stopbits";
    if (d==QFSCtwoStopbits) return "two_stopbits";
    return "";
}

QFSCstopbits string2QFSCstopbits(std::string dd) {
    std::string d=tolower(strstrip(dd));
    if (d=="one_stopbit" || d=="1" || d=="one") return QFSConeStopbit;
    if (d=="one5_stopbit" || d=="1.5" || d=="one5") return QFSCone5Stopbits;
    if (d=="two_stopbit" || d=="2" || d=="two") return QFSCtwoStopbits;
    return QFSConeStopbit;
}


#ifdef __WINDOWS__
void QFSerialConnection::win32_error(std::string where) {
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
        lastError=format("error occured while executing %s: error code %d: %s", where.c_str(),(int)dwErrorCode, pMsgBuf);
        errorOccured=true;
        LocalFree(pMsgBuf);
    } else {
        lastError=format("error occured while executing %s: errorcode %d: sorry, no description available!", where.c_str(), dwErrorCode);
        errorOccured=true;
    }
}
#endif


QFSerialConnection::QFSerialConnection(std::string port, unsigned int baudrate, QFSCdatabits databits, QFSCparity parity, QFSCstopbits stopbits, QFSChandshaking handshaking) {
    logToFile=false;
    log=NULL;
    if (logToFile && log==NULL) log=fopen("rs232.log", "w");
    //if  (!log) //std::cout<<"error opening log file\n";
    connectionOpen=false;
    lastError="";
    this->port=port;
    this->baudrate=baudrate;
    this->databits=databits;
    this->parity=parity;
    this->stopbits=stopbits;
    this->handshaking=handshaking;
    this->timeout_ms=500;
    errorOccured=false;
}

QFSerialConnection::~QFSerialConnection() {
    close();
    if (logToFile && (log==NULL)) log=fopen("rs232.log", "w");
    if (logToFile) fclose(log);
}

bool QFSerialConnection::open(std::string port, unsigned int baudrate, QFSCdatabits databits, QFSCparity parity, QFSCstopbits stopbits, QFSChandshaking handshaking) {
    this->port=port;
    this->baudrate=baudrate;
    this->databits=databits;
    this->parity=parity;
    this->stopbits=stopbits;
    this->handshaking=handshaking;
    return open();
}

bool QFSerialConnection::reopen() {
    bool o=isConnectionOpen();
    close();
    if (o) return open();
    return true;
}

bool QFSerialConnection::open() {

    //std::cout<<"opening port ...\n";
    std::string name="JKSerialConnection::open(port='"+port+"', baud_rate="+inttostr(baudrate)+", databits="+inttostr(databits)+" parity="+QFSCparity2string(parity)+", stopbits="+QFSCstopbits2string(stopbits)+" handshaking="+QFSChandshaking2string(handshaking)+")";

    //std::cout<<"opening port ... "<<name<<"\n";

    bool ok=true;
    if (connectionOpen) ok=close();
    if (!ok) return connectionOpen=false;
    //std::cout<<"opening port ... "<<name<<" ... \n";

#ifdef __WINDOWS__


    BOOL fSuccess;
    DCB dcb;      /* device control block */
    BYTE StopBits=ONESTOPBIT;
    if (stopbits==QFSConeStopbit) {
        StopBits=ONESTOPBIT;
    } else if (stopbits==QFSCone5Stopbits) {
        StopBits=ONE5STOPBITS;
    } else if (stopbits==QFSCtwoStopbits) {
        StopBits=TWOSTOPBITS;
    }
    BYTE Parity=NOPARITY;
    DWORD fParity;
    if (parity==QFSCevenParity) {
        Parity=EVENPARITY;
        fParity=TRUE;
    } else if (parity==QFSCoddParity) {
        Parity=ODDPARITY;
        fParity=TRUE;
    } else if (parity==QFSCnoParity) {
        Parity=NOPARITY;
        fParity=FALSE;
    }

    portHandle = CreateFileA(port.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (portHandle==INVALID_HANDLE_VALUE) {
        port=std::string("\\\\.\\")+port;
        portHandle = CreateFileA(port.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
         if (portHandle==INVALID_HANDLE_VALUE) {
             win32_error(name+" [CreateFileA]");
            return connectionOpen=false;
         }
    }
    fSuccess = GetCommState(portHandle, &dcb);
    if (!fSuccess) {
        CloseHandle(portHandle);
        portHandle=INVALID_HANDLE_VALUE;
        win32_error(name+" [GetCommState]");
        return connectionOpen=false;
    }

    /* configure the port */

    dcb.BaudRate = baudrate;
    dcb.ByteSize = databits;
    dcb.Parity = Parity;
    dcb.StopBits = StopBits;
    dcb.fBinary = (binary)?1:0;

    /* configure handshaking */
    switch (handshaking) {
    case QFSCnoHandshaking:
        dcb.fOutxCtsFlow = false;					// Disable CTS monitoring
        dcb.fOutxDsrFlow = false;					// Disable DSR monitoring
        dcb.fDtrControl = DTR_CONTROL_DISABLE;		// Disable DTR monitoring
        dcb.fOutX = false;							// Disable XON/XOFF for transmission
        dcb.fInX = false;							// Disable XON/XOFF for receiving
        dcb.fRtsControl = RTS_CONTROL_DISABLE;		// Disable RTS (Ready To Send)
        break;

    case QFSChardwareHandshakingDTRRTS:
        dcb.fOutxCtsFlow = true;					// Enable CTS monitoring
        dcb.fOutxDsrFlow = true;					// Enable DSR monitoring
        dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;	// Enable DTR handshaking
        dcb.fOutX = false;							// Disable XON/XOFF for transmission
        dcb.fInX = false;							// Disable XON/XOFF for receiving
        dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;	// Enable RTS handshaking
        break;

    case QFSChardwareHandshakingRTS:
        dcb.fOutxCtsFlow = true;					// Enable CTS monitoring
        dcb.fOutxDsrFlow = false;					// Disable DSR monitoring
        dcb.fDtrControl = DTR_CONTROL_DISABLE;	    // Disable DTR handshaking
        dcb.fOutX = false;							// Disable XON/XOFF for transmission
        dcb.fInX = false;							// Disable XON/XOFF for receiving
        dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;	// Enable RTS handshaking
        break;

    case QFSCxonXoffHandshaking:
        dcb.fOutxCtsFlow = false;					// Disable CTS (Clear To Send)
        dcb.fOutxDsrFlow = false;					// Disable DSR (Data Set Ready)
        dcb.fDtrControl = DTR_CONTROL_DISABLE;		// Disable DTR (Data Terminal Ready)
        dcb.fOutX = true;							// Enable XON/XOFF for transmission
        dcb.fInX = true;							// Enable XON/XOFF for receiving
        dcb.fRtsControl = RTS_CONTROL_DISABLE;		// Disable RTS (Ready To Send)
        break;
    }
    fSuccess = SetCommState(portHandle, &dcb);
    if (!fSuccess) {
        CloseHandle(portHandle);
        portHandle=INVALID_HANDLE_VALUE;
        win32_error(name+" [SetCommState]");
        return connectionOpen=false;
    }


    COMMTIMEOUTS commTimeout;

    if(GetCommTimeouts(portHandle, &commTimeout))
    {
        commTimeout.ReadIntervalTimeout     = timeout_ms/10;
        commTimeout.ReadTotalTimeoutConstant     = timeout_ms;
        commTimeout.ReadTotalTimeoutMultiplier     = 10;
        commTimeout.WriteTotalTimeoutConstant     = timeout_ms/10;
        commTimeout.WriteTotalTimeoutMultiplier = 10;
    } else {
        CloseHandle(portHandle);
        portHandle=INVALID_HANDLE_VALUE;
        win32_error(name+" [GetCommTimeouts]");
        return connectionOpen=false;
    }


    if(!SetCommTimeouts(portHandle, &commTimeout)) {
        CloseHandle(portHandle);
        portHandle=INVALID_HANDLE_VALUE;
        win32_error(name+" [SetCommTimeouts]");
        return connectionOpen=false;
    }

#endif
#ifdef __LINUX__
  struct termios options;

  /* open port */
  unixPortHandle = ::open(port.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
  if (unixPortHandle<0) {
      errorOccured=true;
      lastError="unable to open port '"+port+"'";
      return false; /* Fehler beim �ffnen der Schnittstelle ist aufgetreten */
  }
  //std::cout<<"port oppened\n";
  fcntl(unixPortHandle, F_SETFL, 0);
  //std::cout<<"fcntl\n";

  /* aktuelle Optionen holen */
  tcgetattr(unixPortHandle, &options);
  //std::cout<<"tcgetattr\n";

  /* set BAUD rate */
  switch (baudrate) {
    case 0: break;
#ifdef B50
    case 50:
      cfsetispeed(&options, B50);
      cfsetospeed(&options, B50);
      break;
#endif
#ifdef B75
    case 75:
      cfsetispeed(&options, B75);
      cfsetospeed(&options, B75);
      break;
#endif
#ifdef B110
    case 110:
      cfsetispeed(&options, B110);
      cfsetospeed(&options, B110);
      break;
#endif
#ifdef B134
    case 134:
      cfsetispeed(&options, B134);
      cfsetospeed(&options, B134);
      break;
#endif
#ifdef B150
    case 150:
      cfsetispeed(&options, B150);
      cfsetospeed(&options, B150);
      break;
#endif
#ifdef B200
    case 200:
      cfsetispeed(&options, B200);
      cfsetospeed(&options, B200);
      break;
#endif
#ifdef B300
    case 300:
      cfsetispeed(&options, B300);
      cfsetospeed(&options, B300);
      break;
#endif
#ifdef B600
    case 600:
      cfsetispeed(&options, B600);
      cfsetospeed(&options, B600);
      break;
#endif
#ifdef B1200
    case 1200:
      cfsetispeed(&options, B1200);
      cfsetospeed(&options, B1200);
      break;
#endif
#ifdef B1800
    case 1800:
      cfsetispeed(&options, B1800);
      cfsetospeed(&options, B1800);
      break;
#endif
#ifdef B2400
    case 2400:
      cfsetispeed(&options, B2400);
      cfsetospeed(&options, B2400);
      break;
#endif
#ifdef B4800
    case 4800:
      cfsetispeed(&options, B4800);
      cfsetospeed(&options, B4800);
      break;
#endif
#ifdef B9600
    case 9600:
      cfsetispeed(&options, B9600);
      cfsetospeed(&options, B9600);
      break;
#endif
#ifdef B19200
    case 19200:
      cfsetispeed(&options, B19200);
      cfsetospeed(&options, B19200);
      break;
#endif
#ifdef B38400
    case 38400:
      cfsetispeed(&options, B38400);
      cfsetospeed(&options, B38400);
      break;
#endif
#ifdef B57600
    case 57600:
      cfsetispeed(&options, B57600);
      cfsetospeed(&options, B57600);
      break;
#endif
#ifdef B76800
    case 76800:
      cfsetispeed(&options, B76800);
      cfsetospeed(&options, B76800);
      break;
#endif
#ifdef B115200
    case 115200:
      cfsetispeed(&options, B115200);
      cfsetospeed(&options, B115200);
      break;
#endif
#ifdef B230400
    case 230400:
      cfsetispeed(&options, B230400);
      cfsetospeed(&options, B230400);
      break;
#endif
#ifdef B460800
    case 460800:
      cfsetispeed(&options, B460800);
      cfsetospeed(&options, B460800);
      break;
#endif
#ifdef B500000
    case 500000:
      cfsetispeed(&options, B500000);
      cfsetospeed(&options, B500000);
      break;
#endif
#ifdef B576000
    case 576000:
      cfsetispeed(&options, B576000);
      cfsetospeed(&options, B576000);
      break;
#endif
#ifdef B921600
    case 921600:
      cfsetispeed(&options, B921600);
      cfsetospeed(&options, B921600);
      break;
#endif
#ifdef B1000000
    case 1000000:
      cfsetispeed(&options, B1000000);
      cfsetospeed(&options, B1000000);
      break;
#endif
#ifdef B1152000
    case 1152000:
      cfsetispeed(&options, B1152000);
      cfsetospeed(&options, B1152000);
      break;
#endif
#ifdef B1500000
    case 1500000:
      cfsetispeed(&options, B1500000);
      cfsetospeed(&options, B1500000);
      break;
#endif
#ifdef B2000000
    case 2000000:
      cfsetispeed(&options, B2000000);
      cfsetospeed(&options, B2000000);
      break;
#endif
#ifdef B2500000
    case 2500000:
      cfsetispeed(&options, B2500000);
      cfsetospeed(&options, B2500000);
      break;
#endif
#ifdef B3000000
    case 3000000:
      cfsetispeed(&options, B3000000);
      cfsetospeed(&options, B3000000);
      break;
#endif
#ifdef B3500000
    case 3500000:
      cfsetispeed(&options, B3500000);
      cfsetospeed(&options, B3500000);
      break;
#endif
#ifdef B4000000
    case 4000000:
      cfsetispeed(&options, B4000000);
      cfsetospeed(&options, B4000000);
      break;
#endif
    default:
      errorOccured=true;
      lastError="unable to set baudrate "+inttostr(baudrate);
      return false; /* Fehler beim �ffnen der Schnittstelle ist aufgetreten */
  }
    if (parity==JKSCevenParity) {
        options.c_cflag |= PARENB;
        options.c_cflag &= ~PARODD;
    } else if (parity==JKSCoddParity) {
        options.c_cflag |= PARENB;
        options.c_cflag |= PARODD;
    } else if (parity==JKSCnoParity) {
        options.c_cflag &= ~PARENB;
        options.c_cflag &= ~PARODD;
    }
    if (stopbits==JKSConeStopbit) {
        options.c_cflag &= ~CSTOPB;
    } else if (stopbits==JKSCone5Stopbits) {
      errorOccured=true;
      lastError="unable to set 1.5 stopbits on Linux!";
      return false; /* Fehler beim �ffnen der Schnittstelle ist aufgetreten */
    } else if (stopbits==JKSCtwoStopbits) {
        options.c_cflag |= CSTOPB;
    }
    options.c_cflag &= ~CSIZE;

    if (databits==JKSC5databits) options.c_cflag |= CS5;
    else if (databits==JKSC6databits) options.c_cflag |= CS6;
    else if (databits==JKSC7databits) options.c_cflag |= CS7;
    else if (databits==JKSC8databits) options.c_cflag |= CS8;
#ifdef CS9
    else if (databits==JKSC9databits) options.c_cflag |= CS9;
#endif

    switch (handshaking) {
    case JKSCnoHandshaking:
        options.c_cflag &= ~CRTSCTS;
        break;

    case JKSChardwareHandshakingRTS:
#ifdef CRTSCTS
        options.c_cflag |= CRTSCTS;
#else
#warning("There won't be support for hardware handshaking!!!")
      errorOccured=true;
      lastError="unable to set hardware handshaking on Linux!";
      return false;
#endif
        break;

    case JKSChardwareHandshakingDTRRTS:
#ifdef CRTSCTS
        #warning("There is no support for DTRRTS handshaking ... fallback to CRT/RTS handshaking!!!")
        options.c_cflag |= CRTSCTS;
#else
#warning("There won't be support for hardware handshaking!!!")
      errorOccured=true;
      lastError="unable to set hardware handshaking on Linux!";
      return false;
#endif
        break;

    case JKSCxonXoffHandshaking:
        errorOccured=true;
        lastError="unable to set XON/XOFF handshaking on Linux! (not yet implemented)";
        return false;
        break;
    }



  // now we set some additional settings that disable the special character handling!
  options.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | IEXTEN | ECHOCTL | ECHOPRT | ECHOKE | PENDIN | ISIG);
  options.c_cc[VINTR] = 0;
  options.c_cc[VQUIT] = 0;
  options.c_cc[VERASE] = 0;
  options.c_cc[VKILL] = 0;
  options.c_cc[VEOF] = 0;
  options.c_cc[VMIN] = 0;
  options.c_cc[VEOL] = 0;
  options.c_cc[VEOL2] = 0;
  options.c_cc[VSTART] = 0;
  options.c_cc[VSTOP] = 0;
  options.c_cc[VSUSP] = 0;
  options.c_cc[VDISCARD] = 0;
  options.c_cc[VLNEXT] = 0;
  options.c_cc[VREPRINT] = 0;
  //options.c_cc[] = 0;

  // switch off post-processing ...
  options.c_oflag &= ~(OPOST | OPOST | OLCUC | ONLCR | OCRNL |  ONLRET | OFILL | OFDEL);
  // write options
  tcsetattr(unixPortHandle, TCSANOW, &options);




  //std::cout<<"tcsetattr\n";
#endif

    errorOccured=false;
    lastError="";
    return connectionOpen=true;
}

bool QFSerialConnection::close() {
    if (!connectionOpen) return true;
    std::string name="JKSerialConnection::close()";
#ifdef __WINDOWS__
    if (portHandle!=INVALID_HANDLE_VALUE) {
        CloseHandle(portHandle);
        portHandle=INVALID_HANDLE_VALUE;
    }
#endif
#ifdef __LINUX__
    if (unixPortHandle) ::close(unixPortHandle);
    unixPortHandle=0;

#endif

    connectionOpen=false;
    return true;
}

bool QFSerialConnection::write(std::string data) {
    if (logToFile && (log==NULL)) log=fopen("rs232.log", "w");
    if (logToFile) {
        fprintf(log, ">> %s\n", toprintablestr(data).c_str());
        fflush(log);
    }
    std::string name="JKSerialConnection::write(data='"+toprintablestr(data)+"')";
    //std::cout<<name;
#ifdef __WINDOWS__

    unsigned long dwNumberOfBytesSent = 0;
    while(dwNumberOfBytesSent < data.size()) {
        unsigned long dwNumberOfBytesWritten;
        char c=data[dwNumberOfBytesSent];
        if (WriteFile(portHandle, &c, 1, &dwNumberOfBytesWritten, NULL) != 0) {
            if(dwNumberOfBytesWritten > 0)
                ++dwNumberOfBytesSent;
        } else {
            win32_error(name);
            if (logToFile && (log==NULL)) log=fopen("rs232.log", "w");
            if (logToFile) {
                fprintf(log, "  -- ERROR: %s\n", lastError.c_str());
                fflush(log);
            }
            return false;
        }
    }

#endif
#ifdef __LINUX__
    if (data.size()>0) {
        int num= ::write(unixPortHandle, data.c_str(), data.size());
        if (num!=(int64_t)data.size()) {
            errorOccured=true;
            lastError="error in "+name+": could not transmit all characters. only "+inttostr(num)+" characters sent.";
            if (logToFile && (log==NULL)) log=fopen("rs232.log", "w");
            if (logToFile) {
                fprintf(log, "  -- ERROR: %s\n", lastError.c_str());
                fflush(log);
            }
            return false;
        }
    }
#endif
    //std::cout<<" done !\n";
    return true;
}
/*
bool JKSerialConnection::write(uint8_t *data, int count) {
    if (logToFile && (log==NULL)) log=fopen("rs232.log", "w");
    if (logToFile) {
        fprintf(log, ">> ");
        for (int i=0; i<count; i++) {
            fprintf(log, "%s", chartoprintablestr(data[i]).c_str());
        }
        fprintf(log, "\n");
    }
    std::string name="JKSerialConnection::write()";
    //std::cout<<name;
#ifdef __WINDOWS__

    unsigned long dwNumberOfBytesSent = 0;
    while(dwNumberOfBytesSent < count) {
        unsigned long dwNumberOfBytesWritten;
        char c=data[dwNumberOfBytesSent];
        if (WriteFile(portHandle, &c, 1, &dwNumberOfBytesWritten, NULL) != 0) {
            if(dwNumberOfBytesWritten > 0) {
                //std::cout<<"sent "<<inttohex(c)<<"\n";
                ++dwNumberOfBytesSent;
            }
        } else {
            win32_error(name);
            if (logToFile && (log==NULL)) log=fopen("rs232.log", "w");
            if (logToFile) fprintf(log, "  -- ERROR: %s\n", lastError.c_str());
            return false;
        }
    }

#endif
#ifdef __LINUX__
    if (count>0) {
        int num= ::write(unixPortHandle, data, count);
        if (num!=count) {
            errorOccured=true;
            lastError="error in "+name+": could not transmit all characters. only "+inttostr(num)+" characters sent.";
            if (logToFile && (log==NULL)) log=fopen("rs232.log", "w");
            if (logToFile) fprintf(log, "  -- ERROR: %s\n", lastError.c_str());
            return false;
        }
    }
#endif
    //std::cout<<" done !\n";
    return true;
}
*/

bool QFSerialConnection::write(uint8_t *data, int count) {
    if (logToFile && (log==NULL)) log=fopen("rs232.log", "w");
    if (logToFile) {
        fprintf(log, ">> ");
        for (int i=0; i<count; i++) {
            fprintf(log, "%s", chartoprintablestr(data[i]).c_str());
        }
        fprintf(log, "\n");
        fflush(log);
    }
    std::string name="JKSerialConnection::write()";
    //std::cout<<name;
#ifdef __WINDOWS__

    unsigned long dwNumberOfBytesWritten;
    BOOL wres=WriteFile(portHandle, data, count, &dwNumberOfBytesWritten, NULL);
    if (wres != 0) {
        if((int64_t)dwNumberOfBytesWritten != (int64_t)count) {
            //std::cout<<"sent "<<inttohex(c)<<"\n";
            if (logToFile && (log==NULL)) log=fopen("rs232.log", "w");
            if (logToFile) fprintf(log, " -- ERROR: didn't write all %d byte (only %lu bytes were written)\n", count, dwNumberOfBytesWritten);
            if (logToFile) fflush(log);
            return false;
        }
    } else {
        win32_error(name);
        if (logToFile && (log==NULL)) log=fopen("rs232.log", "w");
        if (logToFile) fprintf(log, " -- ERROR: %s\n", lastError.c_str());
        if (logToFile) fflush(log);
        return false;
    }


    /*unsigned long dwNumberOfBytesSent = 0;
    while(dwNumberOfBytesSent < count) {
        unsigned long dwNumberOfBytesWritten;
        char c=data[dwNumberOfBytesSent];
        BOOL wres=WriteFile(portHandle, &c, 1, &dwNumberOfBytesWritten, NULL);
        if (wres != 0) {
            if(dwNumberOfBytesWritten > 0) {
                //std::cout<<"sent "<<inttohex(c)<<"\n";
                ++dwNumberOfBytesSent;
            }
            if (logToFile) {
                fprintf(log, " -- written: writeresult=%d dwNumberOfBytesWritten=%lu\n", wres, dwNumberOfBytesWritten);
                fflush(log);
            }
        } else {
            win32_error(name);
            if (logToFile && (log==NULL)) log=fopen("rs232.log", "w");
            if (logToFile) fprintf(log, " -- ERROR: %s\n",
                                   lastError.c_str());
            if (logToFile) fflush(log);
            return false;
        }
    }*/

#endif
#ifdef __LINUX__
    if (count>0) {
        int num= ::write(unixPortHandle, data, count);
        if (num!=count) {
            errorOccured=true;
            lastError="error in "+name+": could not transmit all characters. only "+inttostr(num)+" characters sent.";
            if (logToFile && (log==NULL)) log=fopen("rs232.log", "w");
            if (logToFile) {
                fprintf(log, " -- ERROR: %s\n", lastError.c_str());
                fflush(log);
            }
            return false;
        }
    }
#endif
    //std::cout<<" done !\n";
    return true;
}

bool QFSerialConnection::read(uint8_t *data, size_t num_bytes) {
    std::string name="JKSerialConnection::read(num_bytes='"+inttostr(num_bytes)+"')";
#ifdef __WINDOWS__
    unsigned long dwNumberOfBytesRecvd = 0;
    while(dwNumberOfBytesRecvd < num_bytes) {
        unsigned long dwNumberOfBytesRec;
        char d;
        int r=ReadFile(portHandle, &d, 1, &dwNumberOfBytesRec, NULL);
        if (r!=0) {
            if(dwNumberOfBytesRec > 0) {
                data[dwNumberOfBytesRecvd]=d;
                ++dwNumberOfBytesRecvd;
            } else {
                errorOccured=true;
                lastError="read timeout in "+name;
                if (logToFile && (log==NULL)) log=fopen("rs232.log", "w");
                if (logToFile) {
                    fprintf(log, "  -- ERROR: %s\n", lastError.c_str());
                    fflush(log);
                }
                return false;
            }
        } else {
            win32_error(name);
            if (logToFile && (log==NULL)) log=fopen("rs232.log", "w");
            if (logToFile) {
                fprintf(log, "<< \n  -- ERROR: %s\n", lastError.c_str());
                fflush(log);
            }
            return false;
        }
    }

#endif
#ifdef __LINUX__

    char b=0;
    unsigned long dwNumberOfBytesRecvd = 0;
    int bytes=0;
    while (dwNumberOfBytesRecvd<num_bytes) {
      if (read(&b)) {
          data[dwNumberOfBytesRecvd]=b;
          dwNumberOfBytesRecvd++;
      } else {
          // there has been an error during the read() call, the error description is already in
          // the lastError variable, so we only have to prepend the name of the current call
          // the read() function already handels timeouts!
          lastError="ERROR in "+name+" calling JKSerialConnection::read(char*): "+lastError;
          if (logToFile && (log==NULL)) log=fopen("rs232.log", "w");
          if (logToFile) {
              fprintf(log, "  -- ERROR: %s\n", lastError.c_str());
              fflush(log);
          }
          return false;
      }
    }
#endif
    if (logToFile && (log==NULL)) log=fopen("rs232.log", "w");
    if (logToFile) {
        fprintf(log, "<<   0x ");
        for (size_t i=0; i<num_bytes; i++) {
            unsigned int l=data[i];
            fprintf(log, "%2x ", l);
        }
        fprintf(log, "\n");
        fprintf(log, "   str: ");
        for (size_t i=0; i<num_bytes; i++) {
            fprintf(log, "%s", chartoprintablestr(*((char*)(&data[i]))).c_str());
        }
        fprintf(log, "\n");
        fflush(log);
    }
    return true;
};


std::string QFSerialConnection::read(size_t num_bytes, bool* ok) {
    std::string name="JKSerialConnection::read(num_bytes='"+inttostr(num_bytes)+"')";
#ifdef __WINDOWS__
    std::string res="";
    unsigned long dwNumberOfBytesRecvd = 0;
    while(dwNumberOfBytesRecvd < num_bytes) {
        unsigned long dwNumberOfBytesRec;
        char d;
        int r=ReadFile(portHandle, &d, 1, &dwNumberOfBytesRec, NULL);
        if (r!=0) {
            if(dwNumberOfBytesRec > 0) {
                ++dwNumberOfBytesRecvd;
                res+=d;
            } else {
                errorOccured=true;
                lastError="read timeout in "+name;
                if (logToFile && (log==NULL)) log=fopen("rs232.log", "w");
                if (logToFile) {
                    fprintf(log, "<< %s\n", toprintablestr(res).c_str());
                    fprintf(log, "  -- ERROR: %s\n", lastError.c_str());
                    fflush(log);
                }
                if (ok) *ok=false;
                return res;
            }
        } else {
            win32_error(name);
            if (logToFile && (log==NULL)) log=fopen("rs232.log", "w");
            if (logToFile) {
                fprintf(log, "<< \n  -- ERROR: %s\n", lastError.c_str());
                fflush(log);
            }
            if (ok) *ok=false;
            return res;
        }
    }

#endif
#ifdef __LINUX__
    /*char b=0;
    int bytes=0;
    std::string res;
    while (bytes<num_bytes) {
      if (read(unixPortHandle, &b, 1) > 0) {
          res=res+b;
      } else {
          errorOccured=true;
          lastError="error during  in "+name+" operation.";
          if (ok) *ok=false;
          return res;
          break;
      }
    }
    if (bytes<num_bytes) {
      errorOccured=true;
      if (ok) *ok=false;
      lastError="error in  in "+name+": did not read enough bytes: bytes_read="+inttostr(bytes)+"  bytes_expected="+inttostr(num_bytes);
      return res;
    }*/
    char b=0;
    int bytes=0;
    std::string res="";
    while (res.size()<num_bytes) {
      if (read(&b)) {
          res=res+b;
      } else {
          // there has been an error during the read() call, the error description is already in
          // the lastError variable, so we only have to prepend the name of the current call
          // the read() function already handels timeouts!
          lastError="ERROR in "+name+" calling JKSerialConnection::read(char*): "+lastError;
          if (ok) *ok=false;
          if (logToFile && (log==NULL)) log=fopen("rs232.log", "w");
          if (logToFile) {
              fprintf(log, "<< %s\n", toprintablestr(res).c_str());
              fprintf(log, "  -- ERROR: %s\n", lastError.c_str());
              fflush(log);
          }
          return res;
      }
    }
#endif
    if (ok) *ok=true;
    if (logToFile && (log==NULL)) log=fopen("rs232.log", "w");
    if (logToFile) {
        fprintf(log, "<< %s\n", toprintablestr(res).c_str());
        fflush(log);
    }
    return res;
}

bool QFSerialConnection::read(char* ch) {
    std::string name=std::string("JKSerialConnection::read(char* ch)");
#ifdef __WINDOWS__
    char buf[3]={0,0,0};
    unsigned long readData=0;
    if (ReadFile(portHandle, buf, 1, &readData, NULL) != 0) {
        *ch=buf[0];
        if (readData<1) {
            errorOccured=true;
            lastError="read timeout in "+name;
            if (logToFile && (log==NULL)) log=fopen("rs232.log", "w");
            if (logToFile) {
                fprintf(log, "<< \n  -- ERROR: %s\n", lastError.c_str());
                fflush(log);
            }
            return false;
        }
    } else {
        win32_error(name);
        *ch=0;
        if (logToFile && (log==NULL)) log=fopen("rs232.log", "w");
        if (logToFile) {
            fprintf(log, "<< \n  -- ERROR: %s\n", lastError.c_str());
            fflush(log);
        }
        return false;
    }

#endif
#ifdef __LINUX__
    fd_set set;
    struct timeval timeout;

    /* Initialize the file descriptor set. */
    FD_ZERO (&set);
    FD_SET (unixPortHandle, &set);

    /* Initialize the timeout data structure. */
    timeout.tv_sec = 0;
    timeout.tv_usec = timeout_ms*1000;

    /* select returns 0 if timeout, 1 if input available, -1 if error. */
    int sr=select(unixPortHandle+1, &set, NULL, NULL, &timeout);

    //int sr=1;
    //int sr=select(FD_SETSIZE, &set, NULL, NULL, &timeout);

    if (sr==0) {
        errorOccured=true;
        lastError="timeout in "+name+" operation.";
        if (logToFile && (log==NULL)) log=fopen("rs232.log", "w");
        if (logToFile) {
            fprintf(log, "<< \n  -- ERROR: %s\n", lastError.c_str());
            fflush(log);
        }
        return false;
    } else if (sr==1) {
        if (::read(unixPortHandle, ch, 1) <= 0) {
            errorOccured=true;
            lastError="error during read() call in "+name+" operation.";
            if (logToFile && (log==NULL)) log=fopen("rs232.log", "w");
            if (logToFile) {
                fprintf(log, "<< \n  -- ERROR: %s\n", lastError.c_str());
                fflush(log);
            }
            return false;
        }
    } else {
        errorOccured=true;
        lastError="error during select() call in "+name+" operation.";
        if (logToFile && (log==NULL)) log=fopen("rs232.log", "w");
        if (logToFile) {
            fprintf(log, "<< \n  -- ERROR: %s\n", lastError.c_str());
            fflush(log);
        }
        return false;
    }


#endif
    if (logToFile && (log==NULL)) log=fopen("rs232.log", "w");
    if (logToFile) {
        fprintf(log, "<< %s\n", chartoprintablestr(*ch).c_str());
        fflush(log);
    }
    return true;
}

bool QFSerialConnection::read_nowait(char *ch) {
    std::string name=std::string("JKSerialConnection::read(char* ch)");
    bool ok=true;
#ifdef __WINDOWS__
    char buf[3]={0,0,0};
    unsigned long readData=0;

    COMMTIMEOUTS commTimeout;

    GetCommTimeouts(portHandle, &commTimeout);
    commTimeout.ReadIntervalTimeout     = MAXDWORD;
    commTimeout.ReadTotalTimeoutConstant     = 0;
    commTimeout.ReadTotalTimeoutMultiplier     = 0;
    commTimeout.WriteTotalTimeoutConstant     = timeout_ms/10;
    commTimeout.WriteTotalTimeoutMultiplier = 10;

    SetCommTimeouts(portHandle, &commTimeout);
    if (ReadFile(portHandle, buf, 1, &readData, NULL) != 0) {
        *ch=buf[0];
        if (readData<1) {
            errorOccured=true;
            lastError="read timeout in "+name;
            if (logToFile && (log==NULL)) log=fopen("rs232.log", "w");
            if (logToFile) {
                fprintf(log, "<< \n  -- ERROR: %s\n", lastError.c_str());
                fflush(log);
            }
            ok= false;
        }
    } else {
        win32_error(name);
        *ch=0;
        if (logToFile && (log==NULL)) log=fopen("rs232.log", "w");
        if (logToFile) {
            fprintf(log, "<< \n  -- ERROR: %s\n", lastError.c_str());
            fflush(log);
        }
        ok= false;
    }

    commTimeout.ReadIntervalTimeout     = timeout_ms/10;
    commTimeout.ReadTotalTimeoutConstant     = timeout_ms;
    commTimeout.ReadTotalTimeoutMultiplier     = 10;
    commTimeout.WriteTotalTimeoutConstant     = timeout_ms/10;
    commTimeout.WriteTotalTimeoutMultiplier = 10;
    SetCommTimeouts(portHandle, &commTimeout);

#endif
#ifdef __LINUX__
    if (::read(unixPortHandle, ch, 1) <= 0) {
        ok= false;
    } else {
        ok= true;
    }

#endif
    if (logToFile && (log==NULL)) log=fopen("rs232.log", "w");
    if (logToFile) {
        fprintf(log, "<< %s\n", chartoprintablestr(*ch).c_str());
        fflush(log);
    }
    return ok;
}



std::string QFSerialConnection::readUntil(std::string end_string, bool* ok) {
    std::string name="JKSerialConnection::readUntil(end_string='"+toprintablestr(end_string)+"')";
#ifdef __WINDOWS__
    std::string res="";
    unsigned long dwNumberOfBytesRecvd = 0;
    char d='a'; // just a dummy value
    bool okloop=true;
    while (okloop) {
        unsigned long dwNumberOfBytesRec;
        int r=ReadFile(portHandle, &d, 1, &dwNumberOfBytesRec, NULL);
        //unsigned int ec=GetLastError();
        //std::cout<<"r="<<r<<"  error="<<ec<<"  d="<<toprintablestr(std::string(1, d))<<"  dwNumberOfBytesRec="<<dwNumberOfBytesRec<<"  dwNumberOfBytesRecvd="<<dwNumberOfBytesRecvd<<std::endl;
        if (r != 0) {
            if(dwNumberOfBytesRec > 0) {
                ++dwNumberOfBytesRecvd;
                res+=d;
            } else {
                errorOccured=true;
                lastError="read timeout in "+name;
                if (ok) *ok=false;
                if (logToFile && (log==NULL)) log=fopen("rs232.log", "w");
                if (logToFile) {
                    fprintf(log, "<< %s\n", toprintablestr(res).c_str());
                    fprintf(log, "  -- ERROR: %s\n", lastError.c_str());
                    fflush(log);
                }
                return res;
            }
        } else {
            win32_error(name);
            if (ok) *ok=false;
            if (logToFile && (log==NULL)) log=fopen("rs232.log", "w");
            if (logToFile) {
                fprintf(log, "<< %s\n", toprintablestr(res).c_str());
                fprintf(log, "  -- ERROR: %s\n", lastError.c_str());
                fflush(log);
            }
            return res;
        }
        if (res.size()>=end_string.size()) {
            okloop=(res.substr(res.size()-end_string.size(), end_string.size())!=end_string);
        } else {
            okloop=true;
        }
        /*std::cout<<"res.size()="<<res.size()<<"   end_string.size()="<<end_string.size()<<"   res="<<toprintablestr(res)<<"   end_string="<<toprintablestr(end_string);
        if (res.size()>=end_string.size()) {
            std::cout<<"   substr="<<toprintablestr(res.substr(res.size()-end_string.size(), end_string.size()));
        }
        std::cout<<"   okloop="<<okloop<<std::endl;*/

    }
#endif
#ifdef __LINUX__
    char b=0;
    int bytes=0;
    std::string res="";
    bool okloop=true;
    while (okloop) {
      if (this->read(&b)) {
          res=res+b;
      } else {
          // there has been an error during the read() call, the error description is already in
          // the lastError variable, so we only have to prepend the name of the current call
          // the read() function already handels timeouts!
          lastError="ERROR in "+name+" calling JKSerialConnection::read(char*): "+lastError;
          if (logToFile && (log==NULL)) log=fopen("rs232.log", "w");
          if (logToFile) {
              fprintf(log, "<< %s\n", toprintablestr(res).c_str());
              fprintf(log, "  -- ERROR: %s\n", lastError.c_str());
              fflush(log);
          }
          if (ok) *ok=false;
          return res;
      }
      if (res.size()>=end_string.size()) {
          okloop=(res.substr(res.size()-end_string.size(), end_string.size())!=end_string);
      } else {
          okloop=true;
      }
    }

#endif
    if (logToFile && (log==NULL)) log=fopen("rs232.log", "w");
    if (logToFile) {
        fprintf(log, "<< %s\n", toprintablestr(res).c_str());
        fflush(log);
    }
    if (ok) *ok=true;
    return res;
}

int QFSerialConnection::readUntil(char *result, int result_maxsize, const char *end_string, int end_string_size, bool *ok)
{
    std::string name="JKSerialConnection::readUntil(result_maxsize="+inttostr(result_maxsize)+" end_string="+uint8vectostr((uint8_t*)end_string, end_string_size, false, true, false, false)+")";
#ifdef __WINDOWS__
    unsigned long dwNumberOfBytesRecvd = 0;
    char d='a'; // just a dummy value
    bool okloop=true;
    int cnt=0;
    while (okloop) {
        unsigned long dwNumberOfBytesRec;
        int r=ReadFile(portHandle, &d, 1, &dwNumberOfBytesRec, NULL);
        //unsigned int ec=GetLastError();
        //std::cout<<"r="<<r<<"  error="<<ec<<"  d="<<toprintablestr(std::string(1, d))<<"  dwNumberOfBytesRec="<<dwNumberOfBytesRec<<"  dwNumberOfBytesRecvd="<<dwNumberOfBytesRecvd<<std::endl;
        if (r != 0) {
            if(dwNumberOfBytesRec > 0) {
                ++dwNumberOfBytesRecvd;
                result[cnt]=d;
                cnt++;
            } else {
                errorOccured=true;
                lastError="read timeout in "+name;
                if (ok) *ok=false;
                if (logToFile && (log==NULL)) log=fopen("rs232.log", "w");
                if (logToFile) {
                    fprintf(log, "<< %s\n", uint8vectostr((uint8_t*)result, cnt, false, true, false, true).c_str());
                    fprintf(log, "  -- ERROR: %s\n", lastError.c_str());
                    fflush(log);
                }
                return cnt;
            }
        } else {
            win32_error(name);
            if (ok) *ok=false;
            if (logToFile && (log==NULL)) log=fopen("rs232.log", "w");
            if (logToFile) {
                fprintf(log, "<< %s\n", uint8vectostr((uint8_t*)result, cnt, false, true, false, true).c_str());
                fprintf(log, "  -- ERROR: %s\n", lastError.c_str());
                fflush(log);
            }
            return cnt ;
        }
        if (cnt>=end_string_size && end_string_size>0) {
            okloop=false;
            for (int i=1; i<=end_string_size; i++) {
                if (result[cnt-i]!=end_string[end_string_size-i]) {
                    okloop=true;
                    break;
                }
            }
        } else {
            okloop=true;
        }
        if (cnt>=result_maxsize) {
            okloop=false;
        }

    }
#endif
#ifdef __LINUX__
    char b=0;
    int bytes=0;
    int cnt=0;
    bool okloop=true;
    while (okloop) {
      if (this->read(&b)) {
          result[cnt]=b;
          cnt++;
      } else {
          // there has been an error during the read() call, the error description is already in
          // the lastError variable, so we only have to prepend the name of the current call
          // the read() function already handels timeouts!
          lastError="ERROR in "+name+" calling JKSerialConnection::read(char*): "+lastError;
          if (logToFile && (log==NULL)) log=fopen("rs232.log", "w");
          if (logToFile) {
              fprintf(log, "<< %s\n", uint8vectostr((uint8_t*)result, cnt, false, true, false, true).c_str());
              fprintf(log, "  -- ERROR: %s\n", lastError.c_str());
              fflush(log);
          }
          if (ok) *ok=false;
          return cnt ;
      }
      if (cnt>=end_string_size && end_string_size>0) {
          okloop=false;
          for (int i=1; i<=end_string_size; i++) {
              if (result[cnt-i]!=end_string[end_string_size-i]) {
                  okloop=true;
                  break;
              }
          }
      } else {
          okloop=true;
      }
      if (cnt>=result_maxsize) {
          okloop=false;
      }
    }

#endif
    if (logToFile && (log==NULL)) log=fopen("rs232.log", "w");
    if (logToFile) {
        fprintf(log, "<< %s\n", uint8vectostr((uint8_t*)result, cnt, false, true, false, true).c_str());
        fflush(log);
    }
    if (ok) *ok=true;
    return cnt ;
}


std::string QFSerialConnection::readUntil(char end_char, bool* ok) {
    std::string name="JKSerialConnection::readUntil(end_char='"+chartoprintablestr(end_char)+"')";
#ifdef __WINDOWS__
    std::string res="";
    unsigned long dwNumberOfBytesRecvd = 0;
    char d='a'; // just a dummy value
    while(d!=end_char) {
        unsigned long dwNumberOfBytesRec;
        int r=ReadFile(portHandle, &d, 1, &dwNumberOfBytesRec, NULL);
        //unsigned int ec=GetLastError();
        //std::cout<<"r="<<r<<"  error="<<ec<<"  d="<<toprintablestr(std::string(1, d))<<"  dwNumberOfBytesRec="<<dwNumberOfBytesRec<<"  dwNumberOfBytesRecvd="<<dwNumberOfBytesRecvd<<std::endl;
        if (r != 0) {
            if(dwNumberOfBytesRec > 0) {
                ++dwNumberOfBytesRecvd;
                res+=d;
            } else {
                errorOccured=true;
                lastError="read timeout in "+name;
                if (ok) *ok=false;
                if (logToFile && (log==NULL)) log=fopen("rs232.log", "w");
                if (logToFile) {
                    fprintf(log, "<< %s\n", toprintablestr(res).c_str());
                    fprintf(log, "  -- ERROR: %s\n", lastError.c_str());
                    fflush(log);
                }
                return res;
            }
        } else {
            win32_error(name);
            if (ok) *ok=false;
            if (logToFile && (log==NULL)) log=fopen("rs232.log", "w");
            if (logToFile) {
                fprintf(log, "<< %s\n", toprintablestr(res).c_str());
                fprintf(log, "  -- ERROR: %s\n", lastError.c_str());
                fflush(log);
            }
            return res;
        }
    }
#endif
#ifdef __LINUX__
    char b=0;
    int bytes=0;
    std::string res="";
    while (b!=end_char) {
      if (this->read(&b)) {
          res=res+b;
      } else {
          // there has been an error during the read() call, the error description is already in
          // the lastError variable, so we only have to prepend the name of the current call
          // the read() function already handels timeouts!
          lastError="ERROR in "+name+" calling JKSerialConnection::read(char*): "+lastError;
          if (logToFile && (log==NULL)) log=fopen("rs232.log", "w");
          if (logToFile) {
              fprintf(log, "<< %s\n", toprintablestr(res).c_str());
              fprintf(log, "  -- ERROR: %s\n", lastError.c_str());
              fflush(log);
          }
          if (ok) *ok=false;
          return res;
      }
    }

#endif
    if (logToFile && (log==NULL)) log=fopen("rs232.log", "w");
    if (logToFile) {
        fprintf(log, "<< %s\n", toprintablestr(res).c_str());
        fflush(log);
    }
    if (ok) *ok=true;
    return res;
}

bool QFSerialConnection::clearBuffer() {
    std::string name="JKSerialConnection::emptyPortBuffer()";
#ifdef __WINDOWS__
    if (logToFile && (log==NULL)) log=fopen("rs232.log", "w");
    if (logToFile) {
        fprintf(log, "<<!! clearing buffer\n");
        fflush(log);
    }
    if (!PurgeComm(portHandle, PURGE_RXCLEAR)) {
        win32_error(name);
        return false;
    }
    if (logToFile && (log==NULL)) log=fopen("rs232.log", "w");
    if (logToFile) {
        fprintf(log, ">>!! clearing buffer\n");
        fflush(log);
    }
    if (!PurgeComm(portHandle, PURGE_TXCLEAR)) {
        win32_error(name);
        return false;
    }

#endif
#ifdef __LINUX__
    if (logToFile && (log==NULL)) log=fopen("rs232.log", "w");
    if (logToFile) {
        fprintf(log, "<<!! clearing buffer\n");
        fflush(log);
    }
    if (tcflush(unixPortHandle, TCIFLUSH)!=0) {
          errorOccured=true;
          lastError="error during "+name+" operation: error in tcflush(handle, TCIFLUSH).";
          if (logToFile && (log==NULL)) log=fopen("rs232.log", "w");
          if (logToFile) {
              fprintf(log, "  -- ERROR: %s\n", lastError.c_str());
              fflush(log);
          }
          return false;
    }
#endif
    return true;
}


std::vector<std::string> QFSerialConnection::listPorts() {
    std::vector<std::string> res;
#ifdef __WINDOWS__
     char port_name[16];
     for (int port_number = 1; port_number < 256; ++port_number)
     {
         if (port_number < 10) {
             sprintf(port_name, "COM%d", port_number);
         } else {
             sprintf(port_name, "\\\\.\\COM%d", port_number);
         }
         HANDLE hFile = ::CreateFileA(port_name,
             GENERIC_READ | GENERIC_WRITE, 0, NULL,
             OPEN_EXISTING, 0, NULL);
         if(hFile != INVALID_HANDLE_VALUE)
             res.push_back(port_name);
         else
         {
             DWORD err = GetLastError();
             if (err == ERROR_ACCESS_DENIED ||
                 err == ERROR_SHARING_VIOLATION)
                 res.push_back(port_name);
         }
         CloseHandle(hFile);
     }
#endif
#ifdef __LINUX__
    std::vector<std::string> res1=listfiles_wildcard("/dev/*tty*");
    /*for (int i=res.size()-1; i>=0; i--)  {
        FILE* test=fopen(res[i].c_str(), "r+");
        if (test==NULL) {
        } else {
            res.push_back(res1[i]);
            fclose(test);
        }
    }*/
    res=res1;
#endif
    return res;
}

void QFSerialConnection::resetError()
{
    errorOccured=false;
    lastError="";
}

