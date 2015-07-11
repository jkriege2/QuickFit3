/*
  Name: driver.h
  Copyright: (c) 2007
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/

/** \file driver.h
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

#include "tools.h"
#include "sequencererror.h"
#include "sequencegenerator.h"
#include "../highrestimer.h"


#ifndef DRIVER_H
#define DRIVER_H


namespace sequencer {

/**
 * \brief virtual base class for hardware drivers. This class defines a common interface for hardware driver classes that can output a sequence
 * \ingroup drivers
 *
 * The interface consists of a set of basic methods:
 *  - reset() will perform a hardware (if possible) and software reset. So this should basically reset the object
 *    to the state directly after a call of the constructor (or any other sensefull initial state!)
 *  - bind() will bind the class to a dataset, i.e. a sequenceGenerator descendent which will hold the data that is
 *    output by this class.
 *  - start() will start the sequence output. You will first have to call bind(), as this makes the data available
 *    the object shall output.
 *  - stop() will stop a sequence output. This can be used at any time after start() to stop the sequence (even 
 *    mid-sequence if an error occured)
 *  - is_done() returns \c true if the object does not output a sequence and returns \c false as long as the hardware
 *    is occupied with the output.
 * .
 * Additionally there are two methods that return versioning information about the driver. You can use them to return
 * the version and some information about the vendor-specific hardware driver you use:
 *  - get_driver_version() returns the software version of the driver, i.e. the driver from the hardware vendor.
 *  - get_systeminfo() returns some information about the hardware system that is attatched to the computer, e.g. all
 *    available channels with their specs ... .
 * .
 *
 * This interface can be summarized in this state diagram:\n
 *   \dotfile driver_object.dot
 *
 * Every driver should also contain a method 
 * \code 
void bind(channelManager* c, NIsequenceGenerator* gen) 
 * \endcode 
 * which bind the driver to a channelManager and a sequenceGenerator. It is not possible to implement or define this 
 * function here properly, as we do not yet know the type of the argument \c gen, as a driver class may rely on 
 * using a very special sequenceGenerator descendent. So always implement your own \c bind method which may be used 
 * to load data ... by overwriting (and hiding?) the method implemented in this class.
 *
 * Any implmentation of a driver class may also add as many methods to the above interface, as needed. So have a look
 * at sequencer::NIdriver which also implements asynchronous I/O operations.
 */
class driver
{
  protected:
    /** \brief channel manager object */
    channelManager* cmanager;

    /** \brief a pointer to a high resolution timer used to exactly measure execution times in this class */
    HighResTimer* timer;
    
    /** \brief this is true when we use an interal driver object which has to be destroyed */
    bool internaltimer;

    /** \brief points to a LogFile object that is used for debugging output */
    LogFile* log;
    /** \brief output additional debug information in log file */
    bool debug;

	public:
    /** \brief class contructor which initialises the timer internally */
    driver(channelManager* c) {
      cmanager=c;
      internaltimer=true;
      timer=new HighResTimer;
      timer->start();
      log=NULL;
      debug=false;
    };

    /** \brief class contructor which initialises the timer with a given object */
    driver(channelManager* c, HighResTimer* t) {
      cmanager=c;
      internaltimer=false;
      timer=t;
      log=NULL;
      debug=false;
    };

    /** \brief virtual class destructor */
    virtual ~driver() {
      if (internaltimer) {
        delete timer;
      }
    };

    /** \brief read the \a debug flag (\copybrief driver::debug ) */
    GetMacro(bool, debug);
    /** \brief set the \a debug flag (\copybrief driver::debug ) */
    SetMacro(bool, debug);

    /** \brief get the LogFile object to be used for debuging output */
    GetMacro(LogFile*, log);
    /** \brief set the LogFile object to be used for debuging output */
    SetMacro(LogFile*, log);


		/** \brief start the sequence: starts all output channels (digital and analog) 
     *         if \c exetime points to a double variable this will contain the execution
     *         point in time of this function after the function returns. Time is taken from
     *         the HighResTimer object supplied in the contructor or from an internal timer.
     */
		virtual void start(double* exetime=NULL)=0;

		/** \brief stop the sequence: stops all output channels (digital and analog) 
     *         if \c exetime points to a double variable this will contain the execution
     *         point in time of this function after the function returns. Time is taken from
     *         the HighResTimer object supplied in the contructor or from an internal timer.
     */
		virtual void stop(double* exetime=NULL)=0;

		/** \brief returns true if sequence has ended and all data is put out 
     *         if \c exetime points to a double variable this will contain the execution
     *         point in time of this function after the function returns. Time is taken from
     *         the HighResTimer object supplied in the contructor or from an internal timer.
     */
		virtual bool is_done(double* exetime=NULL)=0;

		/** \brief returns the version of NI-DAQmx */
		virtual std::string get_driver_version()=0;

		/** \brief returns some system information about NI-DAQmx and the installed hardware */
		virtual std::string get_systeminfo()=0;

		/** \brief bind the object to a datasource (sequencer::NIsequenceGenerator) 
     *         if \c exetime points to a double variable this will contain the execution
     *         point in time of this function after the function returns. Time is taken from
     *         the HighResTimer object supplied in the contructor or from an internal timer.
     */
		void bind(sequenceGenerator* gen, double* exetime=NULL) { std::cout<<"not implemented !!!"<<std::endl; exit(0); };

		/** \brief hardware system reset 
     *         if \c exetime points to a double variable this will contain the execution
     *         point in time of this function after the function returns. Time is taken from
     *         the HighResTimer object supplied in the constructor or from an internal timer.
     */
		virtual void reset(double* exetime=NULL)=0;
};


};



#endif // NIDRIVER_H


