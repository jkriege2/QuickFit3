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
#include <pthread.h>
#include "highrestimer.h"
#include "tinytiffwriter.h"

/* now first we will have to check the operating system and from this decide
   which libraries (H files) to include */
   
#ifndef __WINDOWS__
#if defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32)
#define __WINDOWS__
#endif
#endif

#include "tcpipserver.h"


inline double sqr(double a) { 
    return a*a; 
}

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
const int img_height=32;                            // height of a frame in pixels
const float pixelsize=24;                           // pixel size in micrometers
const char cam_name[]="example_image_server.cpp";   // name of the camera device
float acquisition_duration=10.0;                    // duration of an acquisition

// 2. editable parameters:
float exposure=0.1;                                 // exposure time in seconds
float image_amplitude=1000.0;                       // amplitude of the artificial rung pattern
float image_wavelength=30.0;                        // spatial wavelength of the artificial rung pattern
bool image_decay=false;                             // is there a decay component in the pattern?
int frames=100;                                     // number of frames to acquire

// 3. internal state variables
bool cam_connected=false;
bool cam_liveview=false;
bool cam_acquisition_running=false;
int maxWriteFrames=0;
int framesCompleted=0; 
pthread_mutex_t mutexframesCompleted; 




/** \brief send the contents and description of all camera parameters to the client */
void writeParameters(TCPIPserver* server, int connection, const std::string& singleparam=std::string("")) {
		// RETURNS A LIST OF THE AVAILABLE CAMERA PARAMETERS
		// SEND ANSWER TO CLIENT/QF3
		//        { (PARAM_FLOAT | PARAM_INT | PARAM_BOOL | PARAM_STRING);<parameter_name>;<parameter_value_as_string>;<parameter_description_as_string>;[<param_range_min>];[<param_range_max>];[(RW|RO)}\n }* \n
		std::string p;
		p="pixel_width";
		if (singleparam.size()==0 || singleparam==p) server->write(connection, format("PARAM_FLOAT;%s;%f;pixel width time in microns;;;RO", p.c_str(), pixelsize)); // pixel width is read-only float, unlimited range

		if (singleparam.size()==0) server->write(connection, std::string("\n"));
		p="pixel_height";
		if (singleparam.size()==0 || singleparam==p) server->write(connection, format("PARAM_FLOAT;%s;%f;pixel height time in microns;;;RO", p.c_str(), pixelsize)); // pixel height is read-only float, unlimited range

		if (singleparam.size()==0) server->write(connection, std::string("\n"));
		p="camera_name";
		if (singleparam.size()==0 || singleparam==p) server->write(connection, format("PARAM_STRING;%s;%s;camera server name;;;RO", p.c_str(), cam_name)); // camera server name is read-only string

		if (singleparam.size()==0) server->write(connection, std::string("\n"));
		p="exposure";
		if (singleparam.size()==0 || singleparam==p) server->write(connection, format("PARAM_FLOAT;%s;%f;exposure time;0;1;RW", p.c_str(), exposure)); // camera exposure time is read/write float with range [0..1]

		if (singleparam.size()==0) server->write(connection, std::string("\n"));
		p="image_amplitude";
		if (singleparam.size()==0 || singleparam==p) server->write(connection, format("PARAM_FLOAT;%s;%f;pattern amplitude;0;4900;RW", p.c_str(), image_amplitude)); // pattern amplitude is read/write float with range [0..4900]

		if (singleparam.size()==0) server->write(connection, std::string("\n"));
		p="image_wavelength";
		if (singleparam.size()==0 || singleparam==p) server->write(connection, format("PARAM_FLOAT;%s;%f;pattern wavelength;0;100;RW", p.c_str(), image_wavelength)); // pattern wavelength is read/write float with range [0..100]

		if (singleparam.size()==0) server->write(connection, std::string("\n"));
		p="acquisition_duration";
		if (singleparam.size()==0 || singleparam==p) server->write(connection, format("PARAM_FLOAT;%s;%f;acquisition duration;0;100;RO", p.c_str(), acquisition_duration)); // acquisition duraction is read-only float with range [0..100]

		if (singleparam.size()==0) server->write(connection, std::string("\n"));
		p="FRAMES";
		if (singleparam.size()==0 || singleparam==p) server->write(connection, format("PARAM_INT;%s;%d;number of frames to aquire;0;100000;RW", p.c_str(), frames)); // number of frames to acquire is read/write int with range [0..1000000]

		if (singleparam.size()==0) server->write(connection, std::string("\n"));
		p="image_decay";
		if (singleparam.size()==0 || singleparam==p) server->write(connection, format("PARAM_BOOL;%s;%s;pattern decay;;;RW", p.c_str(), booltostr(image_decay).c_str())); // pattern decay component is read/write boolean property

		if (singleparam.size()==0) server->write(connection, std::string("\n"));
		p="HAS_INSTRUCTION_PROGRESS";
		if (singleparam.size()==0 || singleparam==p) server->write(connection, format("PARAM_BOOL;%s;true;;;;RO", p.c_str())); // tells QuickFit, that the parameter PROGRESS is supported by this plugin!

		if (singleparam.size()==0) server->write(connection, std::string("\n"));
		p="HAS_INSTRUCTION_CANCEL_ACQUISITION";
		if (singleparam.size()==0 || singleparam==p) server->write(connection, format("PARAM_BOOL;%s;true;;;;RO", p.c_str())); // tells QuickFit, that the parameter CANCEL_ACQUISITION is supported by this plugin!
		
		if (singleparam.size()!=0) {
		    // special parameters, which indicate the progress of an acquisition ... may only be queried with PARAMETER_GET
			if (singleparam.size()==0) server->write(connection, std::string("\n"));
			p="ACQ_RUNNING";
			if (singleparam.size()==0 || singleparam==p) {
			    pthread_mutex_lock(&mutexframesCompleted);
			    server->write(connection, format("PARAM_BOOL;%s;%s;;;;RO", p.c_str(), booltostr(cam_acquisition_running).c_str())); // send, whether an acquisition is running
				pthread_mutex_unlock(&mutexframesCompleted);
			}

			if (singleparam.size()==0) server->write(connection, std::string("\n"));
			p="PROGRESS";
			if (singleparam.size()==0 || singleparam==p) {
			    pthread_mutex_lock(&mutexframesCompleted);
			    int prog=0;
				if (maxWriteFrames>0) prog=floor(100.0*double(framesCompleted)/double(maxWriteFrames));
				server->write(connection, format("PARAM_INT;%s;%d;;;;RO", p.c_str(), prog)); // send the progress (0..100) of the acquisition
				pthread_mutex_unlock(&mutexframesCompleted);
			}
		}
}

/** \brief set an editable parameter to the specified value */
void setParameter(const std::string& param_name, const std::string& param_value) {
    if (param_name==std::string("exposure")) {
	    exposure=atof(param_value.c_str());
		acquisition_duration=float(frames)*exposure;
	} else if (param_name==std::string("image_amplitude")) {
	    image_amplitude=atof(param_value.c_str());
	} else if (param_name==std::string("image_wavelength")) {
	    image_wavelength=atof(param_value.c_str());
	} else if (param_name==std::string("FRAMES")) {
	    frames=atoi(param_value.c_str());
		acquisition_duration=float(frames)*exposure;
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
void getNextFrame(float time, uint16_t* frame, int img_width, int img_height) {
	if (image_decay) {
		for (int y=0; y<img_height; y++) {
			for (int x=0; x<img_width; x++) {
				float r=sqrt(sqr(float(x)-float(img_width)/2.0)+sqr(float(y)-float(img_height)/2.0));
				float val=(100.0+fabs(sin(r/image_wavelength*2.0*M_PI+time/30.0*2.0*M_PI))*image_amplitude);
				if (val>100.0+image_amplitude) val=100.0+image_amplitude;
				if (val>5000) val=5000;
				if (val<100.0) val=100.0;
				frame[y*img_width+x]=uint16_t(val);
			}
		}
	} else {
		for (int y=0; y<img_height; y++) {
			for (int x=0; x<img_width; x++) {
				float r=sqrt(sqr(float(x)-float(img_width)/2.0)+sqr(float(y)-float(img_height)/2.0));
				float val=(100.0+fabs(sin(r/image_wavelength*2.0*M_PI+time/30.0*2.0*M_PI))*image_amplitude)*exp(-r/64.0);
				if (val>100.0+image_amplitude) val=100.0+image_amplitude;
				if (val>5000) val=5000;
				if (val<100.0) val=100.0;
				frame[y*img_width+x]=uint16_t(val);
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


/** \brief this function implements the acquisition thread */
static void *acquisitionThreadFunc(void* val) {
    std::string basename=*(static_cast<std::string*>(val));
	// write basename.txt file
	FILE* ftxt=fopen(std::string(basename+".txt").c_str(), "w");
	if (ftxt) {
	    fprintf(ftxt, "example_image_server.cpp demo acquisition");
		fclose(ftxt);
	}
	// start basename.tif file for acquired frames
	uint16_t* frame=(uint16_t*)malloc(img_width*img_height*sizeof(uint16_t));
	TinyTIFFFile* tif=TinyTIFFWriter_open(std::string(basename+".tif").c_str(), 16, img_width, img_height);	
	maxWriteFrames=floor(acquisition_duration/exposure);
    framesCompleted=0;
    if (tif) {
		HighResTimer timer;
		timer.start();
		double seconds=timer.get_time()/1e6;
		double lastt=seconds;
		// perform acquisition
		bool canceled=!cam_acquisition_running;
		while (seconds<acquisition_duration && !canceled) {			
			seconds = timer.get_time()/1e6;
			if (fabs(seconds-lastt)>=exposure) {
				getNextFrame(framesCompleted, frame, img_width, img_height);
				TinyTIFFWriter_writeImage(tif, frame);
				pthread_mutex_lock(&mutexframesCompleted);
				// count acquired frames
				framesCompleted++;
				// cancel ancquisition, when cam_acquisition_running is reset
				if (!cam_acquisition_running) canceled=true;				
				pthread_mutex_unlock(&mutexframesCompleted);
				lastt=seconds;
				printf("*** acquired frame %d/%d @ %fs ***", framesCompleted, maxWriteFrames, float(seconds));
			}
	    }     
	    TinyTIFFWriter_close(tif);
    }
    free(frame);
	pthread_mutex_lock(&mutexframesCompleted);
    cam_acquisition_running=false;
	pthread_mutex_unlock(&mutexframesCompleted);
    return NULL;
}



int main (void) {
    // HELLO HERE I AM
    printf("example CAM_SERVER TCP/IP camera server ...\n\nfloat-format=%f\n\n", float(M_PI));
    
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
    uint16_t* frame=(uint16_t*)malloc(img_width*img_height*sizeof(uint16_t));
    int img_byte_size=img_width*img_height*sizeof(uint16_t);
    double t=0;
    getNextFrame(t, frame, img_width, img_height);
	std::string basename="example_image_server_output";
    
    /////////////////////////////////////////////////////////////////////////////
    // initializing the acquisition thread structures + mutex
    /////////////////////////////////////////////////////////////////////////////
	pthread_t thread1;
	pthread_mutex_init(&mutexframesCompleted, NULL);
	
    
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
				    // read remaining \n from "INSTRUCTION\n\n"
                    server->read_str_until(connection, '\n');
					
					
                    // CONNCET TO CAMERA
                    cam_connected=true;
                    // SEND ANSWER TO CLIENT/QF3
                    server->write(connection, std::string("ACK_CONNECT\n\n"));
                    // DONE CONNCETING TO CAMERA
                    
                    
                    printfMessage("CAMERA CONNECTED!\n");
                } else if (instruction=="DISCONNECT") {
				    // read remaining \n from "INSTRUCTION\n\n"
                    server->read_str_until(connection, '\n');

                    // DISCONNCET FROM CAMERA
                    cam_connected=false;
                    // SEND ANSWER TO CLIENT/QF3
                    server->write(connection, std::string("ACK_DISCONNECT\n\n"));
                    // DONE DISCONNCETING FROM CAMERA


                    printfMessage("CAMERA DISCONNECTED!\n");
                } else if (instruction=="LIVE_START") {
				    // read remaining \n from "INSTRUCTION\n\n"
                    server->read_str_until(connection, '\n');
				
				    // START THE LIVE_VIEW MODE
					// INIT/CONFIG CAMERA for LIVE-VIEW
                    cam_liveview=true;
                    // SEND ANSWER TO CLIENT/QF3
                    server->write(connection, std::string("ACK_LIVE_START\n\n"));
				    // DONE STARTING THE LIVE_VIEW MODE


                    printfMessage("LIVE VIEW STARTED!\n");
                } else if (instruction=="LIVE_STOP") {
				    // read remaining \n from "INSTRUCTION\n\n"
                    server->read_str_until(connection, '\n');
				
				    // STOP THE LIVE-VIEW MODE
                    cam_liveview=false;
                    // SEND ANSWER TO CLIENT/QF3
                    server->write(connection, std::string("ACK_LIVE_STOP\n\n"));
					
					
                    printfMessage("LIVE VIEW STOPED!\n");
                } else if (instruction=="SIZE_X_GET") {
				    // read remaining \n from "INSTRUCTION\n\n"
                    server->read_str_until(connection, '\n');

				    // RETURNS THE width OF A FRAME IN LIVE_VIEW MODE
                    // SEND ANSWER TO CLIENT/QF3
                    server->write(connection, format("%d\n\n", img_width));
					
					
                    printfMessage("GET FRAME WIDTH!\n");
                } else if (instruction=="SIZE_Y_GET") {
					// read remaining \n from "INSTRUCTION\n\n"
                    server->read_str_until(connection, '\n');

				    // RETURNS THE height OF A FRAME IN LIVE_VIEW MODE
                    // SEND ANSWER TO CLIENT/QF3
                    server->write(connection, format("%d\n\n", img_height));
					
					
                    printfMessage("GET FRAME HEIGHT!\n");
                } else if (instruction=="GET_EXPOSURE") {
					// read remaining \n from "INSTRUCTION\n\n"
                    server->read_str_until(connection, '\n');
				
				    // RETURNS THE exposure time [seconds] OF A FRAME IN LIVE_VIEW MODE
                    // SEND ANSWER TO CLIENT/QF3
                    server->write(connection, format("%f\n\n", exposure));
					
					
                    printfMessage("GET EXPOSURE TIME!\n");
                } else if (instruction=="PARAMETERS_GET") {
					// read remaining \n from "INSTRUCTION\n\n"
                    server->read_str_until(connection, '\n');
				
				    // RETURNS A LIST OF THE AVAILABLE CAMERA PARAMETERS
                    // SEND ANSWER TO CLIENT/QF3
					//        { (PARAM_FLOAT | PARAM_INT | PARAM_BOOL | PARAM_STRING);<parameter_name>;<parameter_value_as_string>;<parameter_description_as_string>;[<param_range_min>];[<param_range_max>];[(RW|RO)}\n }* \n
					writeParameters(server, connection);
                    server->write(connection, "\n\n");
					
					
                    printfMessage("GET CAMERA PARAMETERS!\n");
               } else if (instruction=="PARAMETER_GET") {
				
				    // RETURNS A SINGLE PARAMETER
                    // SEND ANSWER TO CLIENT/QF3
					//        { (PARAM_FLOAT | PARAM_INT | PARAM_BOOL | PARAM_STRING);<parameter_name>;<parameter_value_as_string>;<parameter_description_as_string>;[<param_range_min>];[<param_range_max>];[(RW|RO)}\n }{1} \n
					std::string param_name=server->read_str_until(connection, "\n\n");
					writeParameters(server, connection, param_name);
                    server->write(connection, "\n\n");
					
					
                    printfMessage("GET CAMERA PARAMETER '%s'!\n", param_name.c_str());
                } else if (instruction=="PARAMETERS_SET") {
				
				    // SET A SINGLE IMAGE PARAMETER
                    //   instruction has the form PARAMETERS_SET\n<name>;<value>\n\n
					std::string param_name=server->read_str_until(connection, ';');
					std::string param_value=server->read_str_until(connection, "\n\n");
					setParameter(param_name, param_value);
					
					
                    printfMessage("SET CAMERA PARAMETERS ('%s' = '%s')!\n", param_name.c_str(), param_value.c_str());
                } else if (instruction=="IMAGE_NEXT_GET") {
					// read remaining \n from "INSTRUCTION\n\n"
                    server->read_str_until(connection, '\n');
				
				    // GET A NEW FRAME AND SEND IT TO THE CLIENT
                    t++;
                    getNextFrame(t, frame, img_width, img_height);
                    // SEND ANSWER TO CLIENT/QF3:
					//   1. (IMAGE8 | IMAGE16 | IMAGE32 | IMAGE64) \n
					//   2. <image_width_in_pixels>\n
					//   3. <image_height_in_pixels>\n
					//   5. <image raw data of size image_width_in_pixels*image_height_in_pixels*pixel_data_size>
					//   6. METADATA RECORDS, DESCRIBING THE IMAGE: (from writeParameters())
					//        { (PARAM_FLOAT | PARAM_INT | PARAM_BOOL | PARAM_STRING);<parameter_name>;<parameter_value_as_string>[;<parameter_description_as_string>]\n }* \n
                    server->write(connection, format("IMAGE%d\n%d\n%d\n", int(sizeof(frame[0])*8), img_width, img_height));
                    server->write(connection, (char*)frame, img_byte_size);
					writeParameters(server, connection);
                    server->write(connection, "\n\n");
					
					
                    printfMessage("GET FRAME! t=%lf\n", t);
                } else if (instruction=="RECORD") {
				
				    // START AN ACQUISITION AND SAVE DATA TO A FILE WITH THE BASENAME filename
                    basename=server->read_str_until(connection, "\n\n");
					
					if (!cam_acquisition_running) {
						printfMessage("RECORD FRAME START! basename=%s\n", basename.c_str());
						// SEND ANSWER TO CLIENT/QF3
						//   1. the written filenames (format FILE;<TYPE>;<FILENAME>;<DESCRIPTION>\n
						server->write(connection, format("FILE;TIFF;%s.tif;acquired frames\n", basename.c_str()));
						server->write(connection, format("FILE;TXT;%s.txt;additional text output\n", basename.c_str()));
						//   2. the used camera config parameters
						writeParameters(server, connection);
                        server->write(connection, "\n\n");
						cam_acquisition_running=true;
						int rc = pthread_create( &thread1, NULL, &acquisitionThreadFunc, (void*)(&basename) );
						if( rc != 0 ) {
							cam_acquisition_running=false;
							printfMessage("COULD NOT START RECORD FRAME!\n");
						} else {
						
						}
						
					}
                    printfMessage("RECORD FRAME STARTED!");
                } else if (instruction=="CANCEL_ACQUISITION") {
				    // read remaining \n from "INSTRUCTION\n\n"
                    server->read_str_until(connection, '\n');
				
				    // STOP THE LIVE-VIEW MODE
					pthread_mutex_lock(&mutexframesCompleted);
					cam_acquisition_running=false;
					pthread_mutex_unlock(&mutexframesCompleted);

                    
                    // SEND ANSWER TO CLIENT/QF3
                    server->write(connection, std::string("ACK_CANCEL_ACQUISITION\n\n"));
					
					
                    printfMessage("ACQUISITION CANCELED!\n");
                } else if (instruction!="\n" || instruction!="\n\n") {
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
	pthread_mutex_destroy(&mutexframesCompleted);
    pthread_exit(NULL);
}