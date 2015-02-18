/*
    
	****************************************************************************
	** Example camera server application 
	** for the cam_server plugin of QuickFit 3.0
	****************************************************************************
	
	This example only uses standard (POSIX) C/C++ libraries
	
    This program will send frames with an artificial sinusoideal pattern, but you 
	can	use it as a starting point for implementing your own camera server.


    based on examples from the libc documentation:
        http://www.gnu.org/software/libc/manual/html_node/Inet-Example.html#Inet-Example
        http://www.gnu.org/software/libc/manual/html_node/Server-Example.html
*/

/* the PORT, the server listens on: */
#define PORT    51234



#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <cmath>

/* now first we will have to check the operating system and from this decide
   which libraries (H files) to include */
   
#ifndef __WINDOWS__
#if defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32)
#define __WINDOWS__
#endif
#endif

#include "tcpipserver.h"

inline double sqr(const double& a) { return a*a; }

/** \brief like sprintf/wrapper around sprintf */
std::string format(std::string templ, ...){
  va_list ap;
  char buffer[4096];
  va_start (ap, templ);
  vsprintf (buffer, templ.c_str(), ap);
  va_end (ap);
  std::string ret(buffer);
  return ret;
};

/** \brief convert an integer to a string */
std::string inttostr(int i) {
    return format("%d", i);
}

/** \brief wrapper around printf, which also outputs the current date/time (useful for LOG outputs) */
void printfMessage(std::string templ, ...){
    va_list ap;
    char buffer[4096];
    va_start (ap, templ);
    vsprintf (buffer, templ.c_str(), ap);
    va_end (ap);
    time_t rawtime;
    struct tm * timeinfo;
    char buffert [80];

    time (&rawtime);
    timeinfo = localtime (&rawtime);
    strftime(buffert,80,"%Y-%m-%d %H:%M:%S",timeinfo); 
    printf("   %s: %s", buffert, buffer);
};

/** \brief fill the filed \a frame with data for the time index \a t 
 *
 * This function simply generates some test frames with a sinusoidal intensity pattern.
 */
void getNextFrame(double time, uint16_t* frame, int img_width, int img_height) {
    for (int y=0; y<img_height; y++) {
        for (int x=0; x<img_width; x++) {
        double r=sqrt(sqr(double(x)-double(img_width)/2.0)+sqr(double(y)-double(img_height)/2.0));
            frame[y*img_width+x]=(1.0+sin(r/5.0*M_PI+time/20.0*M_PI))*1000.0;
        }
    }
}

int main (void) {
    // HELLO HERE I AM
    printf("example CAM_SERVER TCP/IP camera server ...\n\n\n");
    
    /////////////////////////////////////////////////////////////////////////////
    // INITIALIZING the TCPIP SERVER CLASS
    /////////////////////////////////////////////////////////////////////////////
    printf("starting TCP/IP server ...\n");
    TCPIPserver* server=new TCPIPserver;
    server->set_port(PORT);
    server->open_socket();
    printf("%s\n", server->get_version().c_str());
    bool running=true;
    
    /////////////////////////////////////////////////////////////////////////////
    // initializing basic variabls
    /////////////////////////////////////////////////////////////////////////////
    bool cam_connected=false;
    bool cam_liveview=false;
    int img_width=128;
    int img_height=64;
    float exposure=0.1;
    uint16_t* frame=(uint16_t*)malloc(img_width*img_height*sizeof(uint16_t));
    int img_byte_size=img_width*img_height*sizeof(uint16_t);
    int img_size=img_width*img_height;
    double t=0;
    getNextFrame(t, frame, img_width, img_height);
    
    /////////////////////////////////////////////////////////////////////////////
    // MAIN LOOP
    /////////////////////////////////////////////////////////////////////////////
    // in this loop, instructions are read from the TCPIP connection
    // and the camera (in this case only a simulated device) is controlled
    // accordingly. The function getNextFrame() is used to retrieve frames
    // with a sinusoidal test pattern.
    /////////////////////////////////////////////////////////////////////////////

    while(running) {
        // accept any incoming connection
        int connection=server->accept_connection();
        printfMessage("  accepted connection %d from %s\n", connection, server->get_client_name(connection).c_str());
        while(server->is_online(connection)) {
            // if we have an open connection, read instructions (as strings):
            std::string instruction=server->read_str_until(connection, '\n');
            
            // decode instructions:
            if (instruction.size()>0) {
                
                if (instruction=="CONNECT") {
                    
                    // CONNCET TO CAMERA
                    cam_connected=true;
                    // SEND ANSWER TO CLIENT/QF3
                    server->write(connection, std::string("DONE_CONNECT\n"));
                    // DONE CONNCETING TO CAMERA
                    
                    
                    printfMessage("CAMERA CONNECTED!\n");
                } else if (instruction=="DISCONNECT") {


                    // DISCONNCET FROM CAMERA
                    cam_connected=false;
                    // SEND ANSWER TO CLIENT/QF3
                    server->write(connection, std::string("DONE_DISCONNECT\n"));
                    // DONE DISCONNCETING FROM CAMERA


                    printfMessage("CAMERA DISCONNECTED!\n");
                } else if (instruction=="LIVE_START") {
				
				    // START THE LIVE_VIEW MODE
                    cam_liveview=false;
                    // SEND ANSWER TO CLIENT/QF3
                    server->write(connection, std::string("DONE_LIVE_START\n"));
				    // DONE STARTING THE LIVE_VIEW MODE


                    printfMessage("LIVE VIEW STARTED!\n");
                } else if (instruction=="LIVE_STOP") {
                    cam_liveview=false;
                    // SEND ANSWER TO CLIENT/QF3
                    server->write(connection, std::string("DONE_LIVE_STOP\n"));
                    printfMessage("LIVE VIEW STOPED!\n");
                } else if (instruction=="SIZE_X_GET") {
                    cam_liveview=false;
                    // SEND ANSWER TO CLIENT/QF3
                    server->write(connection, format("%d\n", img_width));
                    printfMessage("GET FRAME WIDTH!\n");
                } else if (instruction=="SIZE_Y_GET") {
                    cam_liveview=false;
                    // SEND ANSWER TO CLIENT/QF3
                    server->write(connection, format("%d\n", img_height));
                    printfMessage("GET FRAME HEIGHT!\n");
                } else if (instruction=="GET_EXPOSURE") {
                    cam_liveview=false;
                    // SEND ANSWER TO CLIENT/QF3
                    server->write(connection, format("%f\n", exposure));
                    printfMessage("GET EXPOSURE TIME!\n");
                } else if (instruction=="IMAGE_NEXT_GET") {
                    cam_liveview=false;
                    t++;
                    getNextFrame(t, frame, img_width, img_height);
                    // SEND ANSWER (METADATA+FRAME) TO CLIENT/QF3
                    server->write(connection, format("IMAGE\n%d\n%d\n%f\n", img_width, img_height, exposure));
                    server->write(connection, (char*)frame, img_byte_size);
                    server->write(connection, "\n\n");
                    printfMessage("GET FRAME! t=%lf\n", t);
                } else if (instruction=="RECORD") {
                    std::string filename=server->read_str_until(connection, '\n');
                    // SEND ANSWER TO CLIENT/QF3
                    server->write(connection, "ACK_RECORD\n");
                    printfMessage("RECORD FRAME START! filename=%s\n", filename.c_str());
                    double seconds=0;
                    time_t start;
                    time(&start);

                    double lastt=seconds;
                    while (seconds<10) {
                        time_t now;
                        time(&now);
                        seconds = fabs(difftime(now, start));
                        if (fabs(lastt-seconds)>=1) printf(".");
                        lastt=seconds;
                    }
                    printf("\n");

                    // SEND ANSWER TO CLIENT/QF3
                    server->write(connection, "DONE_RECORD\n\n");
                    printfMessage("RECORD FRAME DONE!");
                } else {
                    // PRINT ERROR MESSAGE AND IGNORE UNKNOWN INSTRUCTION
                    printfMessage("read(%d) unknown instruction %s\n", connection, instruction.c_str());
                }
            }
        }
        printfMessage("  connection %d closed\n", connection);
    }

    printf("stopping/freeing TCP/IP server ...\n");
    free(frame);
    delete server;
}