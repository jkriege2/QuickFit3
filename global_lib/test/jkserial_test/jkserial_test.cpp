#include <iostream>
#include "../../jkserialconnection.h"
#include "../../tools.h"
using namespace std;

#define CheckError() if (com.hasErrorOccured()) { \
        std::cout<<"ERROR: \n"; \
        std::cout<<com.getLastError(); \
        std::cout<<"\n\n"; \
        com.close(); \
        return 0; \
    }

int main()
{
    JKSerialConnection com("COM3", 9600, JKSC8databits, JKSCnoParity, JKSConeStopbit, JKSCnoHandshaking);
    com.open(); CheckError();
    com.write("Hello World?"); CheckError();
    std::cout<<"received answer: "<<com.readUntil('\n')<<std::endl;
    CheckError();
    com.close();
    return 0;
}
