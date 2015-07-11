/*
  Name: sergpibdriver.cpp
  Copyright: (c) 2008
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/

#include "sergpibdriver.h"
#include "jkiniparser.h"

namespace sequencer {


void serGPIBDriver::sendCommand(char device, unsigned int command) {
    if (!enabled) return;
    std::string name="serGPIBDriver::sendCommand(device="+inttostr(device)+", command=0x"+inttohex(command)+")";
    open();
    emptyPortBuffer();
    if (command==GPIB_BUSCOMMAND_IFC) {
        writePort(format("a%02dcsIFC", device));
    } else if (command==GPIB_BUSCOMMAND_DCL) {
        writePort(format("a%02dcsDCL", device));
    } else if (command==GPIB_BUSCOMMAND_SDC) {
        writePort(format("a%02dcsSDC", device));
    } else if (command==GPIB_BUSCOMMAND_GET) {
        writePort(format("a%02dcsGET", device));
    } else if (command==GPIB_BUSCOMMAND_GTL) {
        writePort(format("a%02dcsGTL", device));
    } else if (command==GPIB_BUSCOMMAND_LLO) {
        writePort(format("a%02dcsLLO", device));
    } else if (command==GPIB_BUSCOMMAND_PPC) {
        writePort(format("a%02dcsPPC", device));
    } else if (command==GPIB_BUSCOMMAND_PPU) {
        writePort(format("a%02dcsPPU", device));
    } else if (command==GPIB_BUSCOMMAND_UNT) {
        writePort(format("a%02dcsUNT", device));
    } else if (command==GPIB_BUSCOMMAND_UNL) {
        writePort(format("a%02dcsUNL", device));
    } else if (command==GPIB_BUSCOMMAND_SPD) {
        writePort(format("a%02dcsSPD", device));
    } else if (command==GPIB_BUSCOMMAND_SPE) {
        writePort(format("a%02dcsSPE", device));
    } else if (command==GPIB_BUSCOMMAND_TCT) {
        writePort(format("a%02dcsTCT", device));
    } else if (command<=0xFF) {
        std::string c=format("a%02Xcb ", device);
        c[c.size()-1]=command & 0x00FF;
        writePort(c);
    }
    std::string res=readPort_until();
    if (res.find("SERGPIBERROR:")!=std::string::npos) {
        close();
        SEQUENCER_ERRORV(SEQUENCER_GPIBERROR_INTFCERROR_NUM, name, res.c_str());
    }
};

void serGPIBDriver::win32_error(std::string where){
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

void serGPIBDriver::emptyPortBuffer(){
    if (!enabled) return;
    std::string name="serGPIBDriver::emptyPortBuffer()";
    if (port==INVALID_HANDLE_VALUE) {
        SEQUENCER_ERRORV(SEQUENCER_SERERROR_PORTNOTOPEN_NUM, name, "emptyPortBuffer()", com_port.c_str());
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


void serGPIBDriver::writePort(std::string data){
    if (!enabled) return;
    std::string name="serGPIBDriver::writePort(data='"+toprintablestr(data)+"')";
    if (port==INVALID_HANDLE_VALUE) {
        SEQUENCER_ERRORV(SEQUENCER_SERERROR_PORTNOTOPEN_NUM, name, name.c_str(), com_port.c_str());
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
    }};

std::string serGPIBDriver::readPort_until(char end_char){
    if (!enabled) return "";
    std::string name="serGPIBDriver::readPort_until(end_char='"+chartoprintablestr(end_char)+"')";
    std::string res="";
    if (port==INVALID_HANDLE_VALUE) {
        SEQUENCER_ERRORV(SEQUENCER_SERERROR_PORTNOTOPEN_NUM, name, name.c_str(), com_port.c_str());
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

void serGPIBDriver::open(){
    //std::cout<<"opening serGPIB "<<port<<"  "<<INVALID_HANDLE_VALUE<<std::endl;
    if (port!=INVALID_HANDLE_VALUE) return; // port already open

    std::string method="serGPIBDriver::open()";
    jkiniparser ini;
    try {
       ini.readFile(inifile); // parse serial_gpib.ini file
    } catch (jkipError e) {
       SEQUENCER_ERROR(SEQUENCER_ERROR_INI_NUM, e.getMessage(), method.c_str());
    }

    set_enabled(ini.getAsBool("config", "enabled", true) );
    if (!enabled) return;

    com_port=ini.getAsString("config", "port", "COM1");
    //std::cout<<"opened port "<<com_port<<std::endl;

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

void serGPIBDriver::config_interrupt(char device, char interrupt, std::string command, bool send_GTL, bool send_UNL){
    std::string c="t";
    if (interrupt==0) c+="0";
    else c+="1";
    if (send_GTL) c+="1"; else c+="0";
    if (send_UNL) c+="1"; else c+="0";
    c+=format("%02d", device);
    c+=command+"\n";
    open();
    writePort(c);
}


void serGPIBDriver::send(char device, std::string data) {
    std::string name="serGPIBDriver::send(device="+inttostr(device)+" data='"+toprintablestr(data)+"')";
    open();
    emptyPortBuffer();
    writePort(format("oCES\na%02ds%s\n", device, data.c_str()));
    //std::string res=readPort_until();
    //if (res.find("SERGPIBERROR:")!=std::string::npos) {
    //    close();
    //    SEQUENCER_ERRORV(SEQUENCER_GPIBERROR_INTFCERROR_NUM, name, res.c_str());
    //}
};


std::string serGPIBDriver::query(char device, std::string data) {
    std::string name="serGPIBDriver::query(device="+inttostr(device)+" data='"+toprintablestr(data)+"')";
    open();
    emptyPortBuffer();
    writePort(format("oCES\na%02dq%s\n", device, data.c_str()));
    std::string res=readPort_until();
    if (res.find("SERGPIBERROR:")!=std::string::npos) {
        close();
        SEQUENCER_ERRORV(SEQUENCER_GPIBERROR_INTFCERROR_NUM, name, res.c_str());
    }
    return res;
};


std::string serGPIBDriver::read(char device) {
    std::string name="serGPIBDriver::read(device="+inttostr(device)+")";
    open();
    emptyPortBuffer();
    writePort(format("a%02dr", device));
    std::string res=readPort_until();
    if (res.find("SERGPIBERROR:")!=std::string::npos) {
        close();
        SEQUENCER_ERRORV(SEQUENCER_GPIBERROR_INTFCERROR_NUM, name, res.c_str());
    }
    return res;
};


void serGPIBDriver::execute(SDFFHWcommandManager* hwc_manager){
    //std::cout<<"executing serGPIB"<<std::endl;
    open();
    for (size_t i=0; i<hwc_manager->get_command_count(SDFF_CMDclass_SERGPIB); i++) {
        SDFFcommand* hc=hwc_manager->get_command(SDFF_CMDclass_SERGPIB, i);
        if (sequencer::SDFFserGPIBsendCommand* c=dynamic_cast<sequencer::SDFFserGPIBsendCommand*>(hc)) {
            send(c->get_device(), c->get_command());
        } else if (sequencer::SDFFserGPIBqueryCommand* c=dynamic_cast<sequencer::SDFFserGPIBqueryCommand*>(hc)) {
            std::string res=query(c->get_device(), c->get_command());
            c->set_result(res);
        } else if (sequencer::SDFFserGPIBreadCommand* c=dynamic_cast<sequencer::SDFFserGPIBreadCommand*>(hc)) {
            std::string res=read(c->get_device());
            c->set_result(res);
        } else if (sequencer::SDFFserGPIBsendBusCommand* c=dynamic_cast<sequencer::SDFFserGPIBsendBusCommand*>(hc)) {
            sendCommand(c->get_device(), c->get_command());
        } else if (sequencer::SDFFserGPIBconfigureCommand* c=dynamic_cast<sequencer::SDFFserGPIBconfigureCommand*>(hc)) {
            config_REN_talkstate(c->get_REN_talkstate());
            config_REN_readstate(c->get_REN_readstate());
            config_send_LB(c->get_send_LB());
            config_interface_address(c->get_interface_address());
            config_query_delay(c->get_query_delay());
            config_use_EOI(c->get_use_EOI());
        } else if (SDFFserGPIBconfigInterrupt* c=dynamic_cast<sequencer::SDFFserGPIBconfigInterrupt*>(hc)) {
            config_interrupt(c->get_device(), c->get_interrupt(), c->get_command(), c->get_send_GTL(), c->get_send_UNL());
        } else if (sequencer::SDFFserGPIBenableInterrupt* c=dynamic_cast<sequencer::SDFFserGPIBenableInterrupt*>(hc)) {
            config_enable_interrupt(c->get_interrupt(), c->get_state());
        }
    }
    close();
};

}

