/*
  Name: serialdriver.cpp
  Copyright: (c) 2008
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/

#include "serialdriver.h"

namespace sequencer {

void serialDriver::win32_error(std::string where) {
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
}


void serialDriver::open(std::string com_port, long baud_rate, int databits, sdff_serial_paritymodes parity, double stopbits, sdff_serial_handshaking handshaking){
    std::string par="NO_PARITY";
    if (parity==sdffserEVEN) par="EVEN_PARITY";
    if (parity==sdffserODD) par="ODD_PARITY";
    std::string name="serialDriver::open(port="+com_port+", baud_rate="+inttostr(baud_rate)+", databits="+inttostr(databits)+", stopbits="+floattostr(stopbits)+" parity="+par+")";
    if (ports.find(com_port)!=ports.end()) {
        SEQUENCER_ERRORV(SEQUENCER_SERERROR_PORTALREADYOPEN_NUM, name, com_port.c_str());
    }

    BOOL fSuccess;
    DCB dcb;      /* device control block */
    BYTE StopBits;
    if (stopbits==1) {
        StopBits=ONESTOPBIT;
    } else if (stopbits==1.5) {
        StopBits=ONE5STOPBITS;
    } else if (stopbits==2) {
        StopBits=TWOSTOPBITS;
    } else {
        SEQUENCER_ERRORV(SEQUENCER_SERERROR_IMPOSSIBLEARG_NUM, name, com_port.c_str(), floattostr(stopbits).c_str(), "stop_bits", "1, 1.5, 2");
    }
    BYTE Parity=NOPARITY;
    DWORD fParity;
    if (parity==sdffserEVEN) {
        Parity=EVENPARITY;
        fParity=TRUE;
    } else if (parity==sdffserODD) {
        Parity=ODDPARITY;
        fParity=TRUE;
    } else if (parity==sdffserNOPARITY) {
        Parity=NOPARITY;
        fParity=FALSE;
    }

    ports[com_port] = CreateFileA(com_port.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (ports[com_port]==INVALID_HANDLE_VALUE) {
        ports.erase(com_port);
        win32_error(name);
    }
    fSuccess = GetCommState(ports[com_port], &dcb);
    if (!fSuccess) {
        CloseHandle(ports[com_port]);
        ports.erase(com_port);
        win32_error(name);
    }

    /* configure the port */

    dcb.BaudRate = baud_rate;
    dcb.ByteSize = databits;
    dcb.Parity = Parity;
    dcb.StopBits = StopBits;

    /* configure handshaking */
	switch (handshaking) {
	case sdffserNOHANDSHAKING:
		dcb.fOutxCtsFlow = false;					// Disable CTS monitoring
		dcb.fOutxDsrFlow = false;					// Disable DSR monitoring
		dcb.fDtrControl = DTR_CONTROL_DISABLE;		// Disable DTR monitoring
		dcb.fOutX = false;							// Disable XON/XOFF for transmission
		dcb.fInX = false;							// Disable XON/XOFF for receiving
		dcb.fRtsControl = RTS_CONTROL_DISABLE;		// Disable RTS (Ready To Send)
		break;

	case sdffserHARDWARE:
		dcb.fOutxCtsFlow = true;					// Enable CTS monitoring
		dcb.fOutxDsrFlow = true;					// Enable DSR monitoring
		dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;	// Enable DTR handshaking
		dcb.fOutX = false;							// Disable XON/XOFF for transmission
		dcb.fInX = false;							// Disable XON/XOFF for receiving
		dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;	// Enable RTS handshaking
		break;

	case sdffserXONXOFF:
		dcb.fOutxCtsFlow = false;					// Disable CTS (Clear To Send)
		dcb.fOutxDsrFlow = false;					// Disable DSR (Data Set Ready)
		dcb.fDtrControl = DTR_CONTROL_DISABLE;		// Disable DTR (Data Terminal Ready)
		dcb.fOutX = true;							// Enable XON/XOFF for transmission
		dcb.fInX = true;							// Enable XON/XOFF for receiving
		dcb.fRtsControl = RTS_CONTROL_DISABLE;		// Disable RTS (Ready To Send)
		break;
	}
    fSuccess = SetCommState(ports[com_port], &dcb);
    if (!fSuccess) {
        CloseHandle(ports[com_port]);
        ports.erase(com_port);
        win32_error(name);
    }


    COMMTIMEOUTS commTimeout;

    if(GetCommTimeouts(ports[com_port], &commTimeout))
    {
        commTimeout.ReadIntervalTimeout     = 500;
        commTimeout.ReadTotalTimeoutConstant     = 100;
        commTimeout.ReadTotalTimeoutMultiplier     = 500;
        commTimeout.WriteTotalTimeoutConstant     = 100;
        commTimeout.WriteTotalTimeoutMultiplier = 500;
    } else {
        CloseHandle(ports[com_port]);
        ports.erase(com_port);
        win32_error(name);
    }


    if(!SetCommTimeouts(ports[com_port], &commTimeout)) {
        CloseHandle(ports[com_port]);
        ports.erase(com_port);
        win32_error(name);
    }
}

void serialDriver::empty_buffer(std::string com_port){
    std::string name="serialDriver::emptyPortBuffer("+com_port+")";
    if (ports.find(com_port)==ports.end()) {
        SEQUENCER_ERRORV(SEQUENCER_SERERROR_PORTNOTOPEN_NUM, name, name.c_str(), com_port.c_str());
    }
    unsigned long dwNumberOfBytesRec=1;
    char d=0;
    while(dwNumberOfBytesRec!=0) {
        if (ReadFile(ports[com_port], &d, 1, &dwNumberOfBytesRec, NULL) != 0) {
        } else {
            CloseHandle(ports[com_port]);
            ports.erase(com_port);
            win32_error(name);
        }
    }
}


void serialDriver::close(std::string com_port){
    std::string name="serialDriver::close(port="+com_port+")";
    if (ports.find(com_port)==ports.end()) {
        SEQUENCER_ERRORV(SEQUENCER_SERERROR_PORTNOTOPEN_NUM, name, name.c_str(), com_port.c_str());
    }
    CloseHandle(ports[com_port]);
    ports.erase(com_port);
}

void serialDriver::write(std::string com_port, std::string data){
    std::string name="serialDriver::write(port="+com_port+", data='"+toprintablestr(data)+"')";
    if (ports.find(com_port)==ports.end()) {
        SEQUENCER_ERRORV(SEQUENCER_SERERROR_PORTNOTOPEN_NUM, name, name.c_str(), com_port.c_str());
    }

    unsigned long dwNumberOfBytesSent = 0;
    while(dwNumberOfBytesSent < data.size()) {
        unsigned long dwNumberOfBytesWritten;
        char c=data[dwNumberOfBytesSent];
        if (WriteFile(ports[com_port], &c, 1, &dwNumberOfBytesWritten, NULL) != 0) {
            if(dwNumberOfBytesWritten > 0)
                ++dwNumberOfBytesSent;
        } else {
            CloseHandle(ports[com_port]);
            ports.erase(com_port);
            win32_error(name);
        }
    }
    /*DWORD length;
    BOOL fSuccess=WriteFile(ports[com_port], data.c_str(), data.size(), &length ,NULL);
    if (!fSuccess) {
        CloseHandle(ports[com_port]);
        ports.erase(com_port);
        win32_error(name);
    }*/
}

std::string serialDriver::read(std::string com_port, size_t num_bytes){
    std::string name="serialDriver::read(port="+com_port+", num_bytes='"+inttostr(num_bytes)+"')";
    if (ports.find(com_port)==ports.end()) {
        SEQUENCER_ERRORV(SEQUENCER_SERERROR_PORTNOTOPEN_NUM, name, name.c_str(), com_port.c_str());
    }
    std::string res="";
    unsigned long dwNumberOfBytesRecvd = 0;
    while(dwNumberOfBytesRecvd < num_bytes) {
        unsigned long dwNumberOfBytesRec;
        char d;
        if (ReadFile(ports[com_port], &d, 1, &dwNumberOfBytesRec, NULL) != 0) {
            if(dwNumberOfBytesRec > 0) {
                ++dwNumberOfBytesRecvd;
                res+=d;
            }
        } else {
            CloseHandle(ports[com_port]);
            ports.erase(com_port);
            win32_error(name);
        }
    }
    return res;
}

std::string serialDriver::read_until(std::string com_port, char end_char){
    std::string name="serialDriver::read(port="+com_port+", end_char='"+chartoprintablestr(end_char)+"')";
    if (ports.find(com_port)==ports.end()) {
        SEQUENCER_ERRORV(SEQUENCER_SERERROR_PORTNOTOPEN_NUM, name, name.c_str(), com_port.c_str());
    }
    std::string res="";
    unsigned long dwNumberOfBytesRecvd = 0;
    char d='a'; // just a dummy value
    while(d!=end_char) {
        unsigned long dwNumberOfBytesRec;
        if (ReadFile(ports[com_port], &d, 1, &dwNumberOfBytesRec, NULL) != 0) {
            if(dwNumberOfBytesRec > 0) {
                ++dwNumberOfBytesRecvd;
                res+=d;
            }
        } else {
            CloseHandle(ports[com_port]);
            ports.erase(com_port);
            win32_error(name);
        }
    }
    return res;
}

void serialDriver::execute(SDFFHWcommandManager* hwc_manager){
    for (size_t i=0; i<hwc_manager->get_command_count(SDFF_CMDclass_SERIAL); i++) {
        SDFFcommand* hc=hwc_manager->get_command(SDFF_CMDclass_SERIAL, i);
        if (sequencer::SDFFserialOpenCommand* c=dynamic_cast<sequencer::SDFFserialOpenCommand*>(hc)) {
            open(c->get_com_port(), c->get_baudrate(), c->get_databits(), c->get_parity(), c->get_stopbits(), c->get_handshaking());
        } else if (sequencer::SDFFserialCloseCommand* c=dynamic_cast<sequencer::SDFFserialCloseCommand*>(hc)) {
            close(c->get_com_port());
        } else if (sequencer::SDFFserialSendCommand* c=dynamic_cast<sequencer::SDFFserialSendCommand*>(hc)) {
            write(c->get_com_port(), c->get_text());
        } else if (sequencer::SDFFserialReadCommand* c=dynamic_cast<sequencer::SDFFserialReadCommand*>(hc)) {
            std::string res="";
            if (c->get_byte_count()<=0) {
                res=read_until(c->get_com_port(), c->get_end_char());
            } else {
                res=read(c->get_com_port(), c->get_byte_count());
            }
            c->set_result(res);
        }
    }
}

}

