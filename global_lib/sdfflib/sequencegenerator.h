/**
 * \ingroup seqgen
 */
/*@{*/

/** \file sequencegenerator.h
 *
 */

#ifndef SEQUENCEGENERATOR_H
#define SEQUENCEGENERATOR_H

#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <set>
#include <cstdio>
#include <stdint.h>

#include "tools.h"
#include "sequencererror.h"
#include "channelmanager.h"
#include "logfile.h"


namespace sequencer {


/**
 * \brief abstract base class that defines an interface for a class that generates the sequence as a memory array from simple instructions. The inheriting classes then control the hardware driver layer.
 * \ingroup seqgen
 *
 * \image html sequence_generator.png
 *
 * The class sequencer::sequencegenerator implements a low-level sequence generator. This is then controlled
 * by the more high-level classes, like sequencer::sdffParser or sequencer::csvSequenceParser. These classes
 * can use a sequencer::sequenceGenerator child to generate a set of memory arrays that can be directly fed 
 * into specific hardware driver libraries. 
 *
 * At the beginning all values are beeing set to 0. There are only a few and very low-level methods to generate the sequence:
 *   - \c set_analog / \c set_digital sets the value of the array at the specified time or in the specified time interval. 
 *     For this it is important that the given time is an integer multiple of basic sample_timestep, or else the non-integer 
 *     part will be cut off
 *   - \c set_linear adds values between the given times that increase linearly from the start to the end point, or
 *     from the value in the cell before the start time and the end value.
 *   - \c init to initialize the internal data structures
 *   - \c get_analog returns the value of an analog channel at a given time
 *   - \c get_digital returns the value of a digital channel at a given time
 * .
 *
 * \c sample_timestep is the output sampling timestep, so \c 1/sample_timesetp is the sampling rate of the output.
 *
 * \c end_time is the length of the full memory array in time domain. So there are \c end_time/sample_time array 
 * entries for each channel.
 *
 */
class sequenceGenerator
{
    protected:
        /** \brief points to a LogFile object that is used for debugging output */
        LogFile* log;
        /** \brief output additional debug information in log file */
        bool debug;
        
        /** \brief sampling timestep of the sequence */
        double sample_timestep;
        /** \brief the latest time (end time) of the sequence */
        double end_time;
        /** \brief a list of all channels, used in the sequence */
        std::set<std::string> used_channels;
        /** \brief internal: pointer to a sequencer::channelManager object which is needed by some of the methods of this class. This property will be set by the \c init() function.*/
        channelManager* cmanager;
        
	public:
        /** \brief class constructor */
        sequenceGenerator() { log=NULL; debug=false; };
        /** \brief virtual class destructor */
        virtual ~sequenceGenerator() {};
        
         /** \brief set a pointer to a sequencer::channelManager object that can be used by methods of this class */
        SetMacro(channelManager*, cmanager);
         /** \brief get a pointer to the sequencer::channelManager object that is used by methods of this class */
        GetMacro(channelManager*, cmanager);

         /** \brief get the value of the sample_timestep in µs (Microseconds)  */
        GetMacro(double, sample_timestep);   
        
         /** \brief get the value of the end_time in µs (Microseconds) */
        GetMacro(double, end_time);   
        
        /** \brief read the \a debug flag (\copybrief sequenceGenerator::debug ) */
        GetMacro(bool, debug);
        /** \brief set the \a debug flag (\copybrief sequenceGenerator::debug ) */
        SetMacro(bool, debug);

        /** \brief get the LogFile object to be used for debuging output */
        GetMacro(LogFile*, log);
        /** \brief set the LogFile object to be used for debuging output */
        SetMacro(LogFile*, log);

        /** \brief initialize the object for sequence generation and set \c sample_timestep and \c end_time. 
         *         The channels will be taken from the sequencer::channelManager object. The parameter 
         */
        virtual void init(channelManager* cm, double sample_timestep, double end_time=-1);

        /** \brief clear all information in the object (channels ...) and free all allocated memory */
        virtual void clear()=0;

        /** \brief returns the analog value at the specified time from the given channel */
        virtual double get_analog(std::string channel, double timestep)=0;
        
        /** \brief returns the digital value at the specified time from the given channel */
        virtual bool get_digital(std::string channel, double timestep)=0;


        /** \brief sets the value of an analog channel at the specified time */
        virtual void set_analog(std::string channel, double timestep, double value)=0;

        /** \brief sets the value of an analog channel in the specified time interval */
        virtual void set_analog(std::string channel, double time_start, double time_stop, double value)=0;

        /** \brief sets the values of an analog channel in the specified time interval to form a linear ramp beginning at the value of the channel at time_start */
        virtual void set_linear(std::string channel, double time_start, double time_stop, double endvalue);

        /** \brief sets the values of an analog channel in the specified time interval to form a linear ramp 
         * This is simply implemented by calling \code set_linear(channel, time_start, time_stop, get_analog(channel, time_start), endvalue); \endcode
         */
        virtual void set_linear(std::string channel, double time_start, double time_stop, double startvalue, double endvalue)=0;

        /** \brief sets the value of ta digital channel at the specified time  */
        virtual void set_digital(std::string channel, double timestep, bool value)=0;

        /** \brief sets the value of a digital channel in the specified time interval */
        virtual void set_digital(std::string channel, double time_start, double time_stop, bool value)=0;
        
        /** \brief returns a report that contains information about the data saved in the object */
        virtual std::string report();
        
        /** \brief clear the list of used channels */
        virtual inline void clear_used() { 
          used_channels.clear(); 
        }
        
        /** \brief add a channel to the list of used channels  */
        virtual inline void add_used(std::string channel) {
          if (used_channels.find(channel)==used_channels.end())
            used_channels.insert(channel);
        }
        
        /** \brief returns true when the given channel is in the used list. */
        virtual inline bool is_used(std::string channel) {
          return used_channels.find(channel)!=used_channels.end();
        }
};

}
#endif // SEQUENCEGENERATOR_H


/*@}*/
