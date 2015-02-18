/*
    
	****************************************************************************
	** Example camera server application 
	** for the cam_server plugin of QuickFit 3.0
	****************************************************************************
	
	This example only uses standard (POSIX) C/C++ libraries
	
    This program will send frames with an artificial sinusoideal pattern, but you 
	can	use it as a starting point for implementing your own camera server.
	
	It implements the LIVE-VIEW mode and teh ACQUISITION mode:
	  - LIVE_VIEW: After starting the live-view mode, the client may poll single 
	    frames with the instruction IMAGE_NEXT_GET. Typically you should initialize
		the camera in the instruction LIVE_START. On every call of IMAGE_NEXT_GET
		you should then read a NEW frame and send it back to the client.
	  - ACQUISITION: Here the client tells the server to acquire a given number
	    of frames and save them to harddisk. The server the only returns a path
		to the created file. This can be used for the fast streaming modes of
		your camera.


    based on examples from the libc documentation:
        http://www.gnu.org/software/libc/manual/html_node/Inet-Example.html#Inet-Example
        http://www.gnu.org/software/libc/manual/html_node/Server-Example.html
*/

/* the PORT, the server listens on: */
#define PORT    51234


#include <stdint.h>
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

/** \brief convert an bool to a string */
std::string booltostr(bool i) {
    if (i) return "true";
	else return "false";
}




// global parameters, describing the camera: 
// 1. non-editable parameters
const int img_width=128;                            // width of a frame in pixels
const int img_height=64;                            // height of a frame in pixels
const float pixelsize=24;                           // pixel size in micrometers
const char cam_name[]="example_image_server.cpp";   // name of the camera device

// 2. editable parameters:
float exposure=0.1;                                 // exposure time in seconds
float image_amplitude=1000.0;                       // amplitude of the artificial rung pattern
float image_wavelength=5.0;                         // spatial wavelength of the artificial rung pattern
bool image_decay=false;                             // is there a decay component in the pattern?

/** \brief send the contents and description of all camera parameters to the client */
void writeParameters(TCPIPserver* server, int connection) {
		// RETURNS A LIST OF THE AVAILABLE CAMERA PARAMETERS
		// SEND ANSWER TO CLIENT/QF3
		//        { (PARAM_FLOAT | PARAM_INT | PARAM_BOOL | PARAM_STRING);<parameter_name>;<parameter_value_as_string>;<parameter_description_as_string>;[<param_range_min>];[<param_range_max>];[(RW|RO)}\n }* \n
		server->write(connection, format("PARAM_FLOAT;pixel_width;%f;pixel width time in microns;;;RO", pixelsize)); // pixel width is read-only float, unlimited range
		server->write(connection, format("\nPARAM_FLOAT;pixel_height;%f;pixel height time in microns;;;RO", pixelsize)); // pixel height is read-only float, unlimited range
		server->write(connection, format("\nPARAM_STRING;camera_name;%s;camera server name;;;RO", cam_name)); // camera server name is read-only string
		server->write(connection, format("\nPARAM_FLOAT;exposure;%f;exposure time;0;1;RW", exposure)); // camera exposure time is read/write float with range [0..1]
		server->write(connection, format("\nPARAM_FLOAT;image_amplitude;%f;pattern amplitude;0;10000;RW", exposure)); // pattern amplitude is read/write float with range [0..10000]
		server->write(connection, format("\nPARAM_FLOAT;image_wavelength;%f;pattern wavelength;0;100;RW", exposure)); // pattern wavelength is read/write float with range [0..100]
		server->write(connection, format("\nPARAM_BOOL;image_decay;%2;pattern decay;;;RW", booltostr(image_decay).c_str())); // pattern decay component is read/write boolean property
		server->write(connection, "\nPARAM_BOOL;dummy_device;true;camera is dummy;;;RO"); // a boolean property
}

/** \brief set an editable parameter to the specified value */
void setParameter(const std::string& param_name, const std::string& param_value) {
    if (param_name==std::string("exposure")) {
	    exposure=atof(param_value.c_str());
	} else if (param_name==std::string("image_amplitude")) {
	    image_amplitude=atof(param_value.c_str());
	} else if (param_name==std::string("image_wavelength")) {
	    image_wavelength=atof(param_value.c_str());
	} else if (param_name==std::string("image_decay")) {
	    image_decay=false;
		if (param_value==std::string("true")) image_decay=true;
		else if (param_value==std::string("1")) image_decay=true;
		else if (param_value==std::string("t")) image_decay=true;
		else if (param_value==std::string("y")) image_decay=true;
		else if (param_value==std::string("yes")) image_decay=true;
	}
}

/** \brief fill the filed \a frame with data for the time index \a t 
 *
 * This function simply generates some test frames with a sinusoidal intensity pattern.
 */
void getNextFrame(double time, uint16_t* frame, int img_width, int img_height) {
	if (image_decay) {
		for (int y=0; y<img_height; y++) {
			for (int x=0; x<img_width; x++) {
			double r=sqrt(sqr(double(x)-double(img_width)/2.0)+sqr(double(y)-double(img_height)/2.0));
				frame[y*img_width+x]=(1.0+sin(r/image_wavelength*M_PI+time/20.0*M_PI))*image_amplitude;
			}
		}
	} else {
		for (int y=0; y<img_height; y++) {
			for (int x=0; x<img_width; x++) {
			double r=sqrt(sqr(double(x)-double(img_width)/2.0)+sqr(double(y)-double(img_height)/2.0));
				frame[y*img_width+x]=(1.0+sin(r/image_wavelength*M_PI+time/20.0*M_PI))*image_amplitude*exp(-r/64.0);
			}
		}
	}
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
    uint16_t* frame=(uint16_t*)malloc(img_width*img_height*sizeof(uint16_t));
    int img_byte_size=img_width*img_height*sizeof(uint16_t);
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
                    server->write(connection, std::string("ACK_CONNECT\n\n"));
                    // DONE CONNCETING TO CAMERA
                    
                    
                    printfMessage("CAMERA CONNECTED!\n");
                } else if (instruction=="DISCONNECT") {


                    // DISCONNCET FROM CAMERA
                    cam_connected=false;
                    // SEND ANSWER TO CLIENT/QF3
                    server->write(connection, std::string("ACK_DISCONNECT\n\n"));
                    // DONE DISCONNCETING FROM CAMERA


                    printfMessage("CAMERA DISCONNECTED!\n");
                } else if (instruction=="LIVE_START") {
				
				    // START THE LIVE_VIEW MODE
					// INIT/CONFIG CAMERA for LIVE-VIEW
                    cam_liveview=false;
                    // SEND ANSWER TO CLIENT/QF3
                    server->write(connection, std::string("ACK_LIVE_START\n\n"));
				    // DONE STARTING THE LIVE_VIEW MODE


                    printfMessage("LIVE VIEW STARTED!\n");
                } else if (instruction=="LIVE_STOP") {
				
				    // STOP THE LIVE-VIEW MODE
                    cam_liveview=false;
                    // SEND ANSWER TO CLIENT/QF3
                    server->write(connection, std::string("ACK_LIVE_STOP\n\n"));
					
					
                    printfMessage("LIVE VIEW STOPED!\n");
                } else if (instruction=="SIZE_X_GET") {
				
				    // RETURNS THE width OF A FRAME IN LIVE_VIEW MODE
                    // SEND ANSWER TO CLIENT/QF3
                    server->write(connection, format("%d\n\n", img_width));
					
					
                    printfMessage("GET FRAME WIDTH!\n");
                } else if (instruction=="SIZE_Y_GET") {
				
				    // RETURNS THE height OF A FRAME IN LIVE_VIEW MODE
                    // SEND ANSWER TO CLIENT/QF3
                    server->write(connection, format("%d\n\n", img_height));
					
					
                    printfMessage("GET FRAME HEIGHT!\n");
                } else if (instruction=="GET_EXPOSURE") {
				
				    // RETURNS THE exposure time [seconds] OF A FRAME IN LIVE_VIEW MODE
                    // SEND ANSWER TO CLIENT/QF3
                    server->write(connection, format("%f\n\n", exposure));
					
					
                    printfMessage("GET EXPOSURE TIME!\n");
                } else if (instruction=="PARAMETERS_GET") {
				
				    // RETURNS A LIST OF THE AVAILABLE CAMERA PARAMETERS
                    // SEND ANSWER TO CLIENT/QF3
					//        { (PARAM_FLOAT | PARAM_INT | PARAM_BOOL | PARAM_STRING);<parameter_name>;<parameter_value_as_string>;<parameter_description_as_string>;[<param_range_min>];[<param_range_max>];[(RW|RO)}\n }* \n
					writeParameters(server, connection);
                    server->write(connection, "\n\n");
					
					
                    printfMessage("GET CAMERA PARAMETERS!\n");
                } else if (instruction=="PARAMETERS_SET") {
				
				    // SET A SINGLE IMAGE PARAMETER
                    //   instruction has the form PARAMETERS_SET\n<name>;<value>\n\n
					std::string param_name=server->read_str_until(connection, ';');
					std::string param_value=server->read_str_until(connection, "\n\n");
					setParameter(param_name, param_value);
					
					
                    printfMessage("SET CAMERA PARAMETERS (%s = %s)!\n", param_name.c_str(), param_value.c_str());
                } else if (instruction=="IMAGE_NEXT_GET") {
				
				    // GET A NEW FRAME AND SEND IT TO THE CLIENT
                    t++;
                    getNextFrame(t, frame, img_width, img_height);
                    // SEND ANSWER TO CLIENT/QF3:
					//   1. (IMAGE8 | IMAGE16 | IMAGE32 | IMAGE64) \n
					//   2. <image_width_in_pixels>\n
					//   3. <image_height_in_pixels>\n
					//   4. <exposure_time_in_seconds>\n
					//   5. <image raw data of size image_width_in_pixels*image_height_in_pixels*pixel_data_size>
					//   6. METADATA RECORDS, DESCRIBING THE IMAGE: (from writeParameters())
					//        { (PARAM_FLOAT | PARAM_INT | PARAM_BOOL | PARAM_STRING);<parameter_name>;<parameter_value_as_string>[;<parameter_description_as_string>]\n }* \n
                    server->write(connection, format("IMAGE%d\n%d\n%d\n%f\n", int(sizeof(frame[0])*8), img_width, img_height, exposure));
                    server->write(connection, (char*)frame, img_byte_size);
					writeParameters(server, connection);
                    server->write(connection, "\n\n");
					
					
                    printfMessage("GET FRAME! t=%lf\n", t);
                } else if (instruction=="RECORD") {
				
				    // START AN ACQUISITION AND SAVE DATA TO A FILE WITH THE BASENAME filename
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
                    server->write(connection, "ACK_RECORD\n\n");
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