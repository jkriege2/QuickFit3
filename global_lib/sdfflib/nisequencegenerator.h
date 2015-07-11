/**
 * \ingroup seqgen
 */
/*@{*/

/** \file nisequencegenerator.h
 */

#ifndef NISEQUENCEGENERATOR_H
#define NISEQUENCEGENERATOR_H

#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <cstdio>
#include <stdint.h>

#include <windows.h>

#include "jkiniparser.h"
#include "tools.h"
#include "sequencererror.h"
#include "channelmanager.h"
#include "sequencegenerator.h"


namespace sequencer {

  typedef int16_t int16;

/** \brief possibilities for pin ordering inside a port
 * \ingroup chanman
 */
enum sg_pinordering {
	sgFree,
	sgPinName,
	sgUnknown
};

/** \brief list of possible channel data formats
 * \ingroup chanman
 */
enum sg_dataformat {
	sgDouble,      /*!< \brief data is a double array */
	sgBinary16,    /*!< \brief data is a int16_t array, used instead of sgDouble */
	sgUInt8,       /*!< \brief data is a uint8_t array */
	sgUInt16,      /*!< \brief data is a uint16_t array  */
	sgUInt32       /*!< \brief data is a uint32_t array  */
};

/** \brief information about a single port (from the ports.ini file)
 * \ingroup chanman
 */
typedef struct {
  sg_dataformat format;      /*!< \brief dataformat of the port (sgDouble=don't care) */
  sg_pinordering ordering;   /*!< \brief how to sort the pins within a port */
} sg_portinfo;


/** \brief data about a given channel (memory adress of data array, offset ...)
 * \ingroup seqgen
 */
typedef struct {
  std::string name;               /*!< \brief name with which to access the channel in a sequence definition */
  std::string hardware_name;      /*!< \brief hardware name of the pin */
  int data;                       /*!< \brief internal number of the memory array of this channel */
  unsigned char offset;           /*!< \brief number of the bit that represents a digital output channel or index of the double in a 2D array that represents the channel*/

} sg_channel;

/** \brief data about an output array
 * \ingroup seqgen
 */
typedef struct {
  sg_dataformat format;           /*!< \brief type of the channel */
  unsigned char channels;         /*!< \brief number of channels in this array */
  void* data;                     /*!< \brief adress of memory array, containing data */
  void* compare_data;             /*!< \brief adress of memory for one single sample. */
  std::string adress_string;      /*!< \brief this string may contain a hardware adress of the port that is represented in this array */
  std::string device;             /*!< \brief the device that this array belongs to */
  std::string port;               /*!< \brief the port that this array belongs to */
  void* init;                     /*!< \brief a pointer to the initialization data of the channel */
  long long size;                 /*!< \brief size of the array in bytes */
} sg_arraydata;

/** \brief data structure that is used by \c NIsequenceGenerator::sort_array tosort the arrays
 * \ingroup seqgen
 */
typedef struct {
  unsigned char offset;
  std::string name;
  std::string hardware_name;
} sg_sortarraydata;


/** \brief a string that divides two hardware adresses in a string with more than one adress
 * \ingroup seqgen
 */
#define NISEQGEN_ADRESS_DIVIDER ","

/** \brief a string that is put in between device and port name to form the physical adress
 * \ingroup seqgen
 */
#define NISEQGEN_DEVICE_PORT_DIVIDER "/"

/** \brief a string that is put in between port and pin name to form the physical adress
 * \ingroup seqgen
 */
#define NISEQGEN_PORT_PIN_DIVIDER "/"



/**
 * \brief generates the sequence as a memory array from simple instructions that is designed for use with the National Instruments (NI) DAQmx base C driver library
 * \ingroup seqgen
 *
 * The class sequencer::NIsequenceGenerator implements a low-level sequence generator. This is then controlled
 * by the more high-level classes, like sequencer::sdffParser or sequencer::csvSequenceParser. These classes
 * can use sequencer::NIsequenceGenerator to generate a set of memory arrays that can be directly fed into the National
 * Instruments DAQmx base driver library that controls the hardware. It inherits the high-level interface defined in
 * tha abstract base class sequencer::sequenceGenerator.
 *
 * \par internal data format for NI-DAQmx driver:
 * The output of sequencer::NIsequenceGenerator is a set of arrays, where there is one array for each port on
 * each device. All analog channels of one port on one device will be tied together to one array of doubles.
 * All digital channels of one port on one device will be tied together to an uint<i>n</i>_t array. In one such
 * array the channels are ordered alphabetically. The next diagramm shows the mapping from the logical structure
 * to the physical/memory structure. The memory structures/arrays are defined to work as input for the National
 * Instruments DAQmx base driver library. So this class is somewhat depending on the NI driver library although
 * it is not explicitly included. So if you want to port the system to another output hardware it could be necessary
 * to reimplement this class.
 * \image html low_level_dataformats.png
 * This class also provides a list which can be accessed by the internal name of a port (see sequencer::channelManager)
 * and that contains the base adress of the corresponding memory array and information about its data format and the
 * array or bit offset. If the a channel is a digital pin then \c offset contains the bit offset which is the number
 * of the bit that belongs to the corresponding port. If the channel is an analog output then \c offset contains the
 * index of the channel in the resulting  2D array (see above figure).\n
 * At the beginning all values are beeing set to 0. There are only very low-level methods to generate the sequence:
 *   - add_value sets the value of the array at the specified time. For this it is important that the given time
 *     is an integer multiple of basic sample_timestep, or else the non-integer part will be cut off
 *   - add_linear adds values between the given times that increase linearly from the startIdx to the end point, or
 *     from the value in the cell before the startIdx time and the end value.
 * .
 *
 * \par time parameters
 * \c sample_timestep is the output sampling timestep, so \c 1/sample_timesetp is the sampling rate of the output.\n
 * \c end_time is the length of the full memory array in time domain. So there are \c end_time/sample_time array
 * entries for each channel.
 *
 * \par pin grouping:
 * As it is not clear whether digital pins may be grouped in any given order into one array or whether the pin_name
 * gives the bit that represents the pin, there is a mechanism to make both options available. The init() methods may
 * read additional information about the ports of the single devices from a configuration file. The name of this file is stored in
 * the private variable portconfigfilename, which can be accessed via \c get_ and \c set_ methods. If this variable is empty
 * or the file does not exist (default is: \c ports.ini ) init() will assume that it may order the pins freely within
 * the arrays. This configuration file has a simple form:
 * \verbatim
[device+port]
width = 8 | 16 | 32         # only vaid for digital ports, gives the width of the port in bits
grouping = free | pin_name  # is grouping of bits free or does the pin_name provide the bit number
\endverbatim
 * If in such a file \c grouping=pin_name then the pin_name property of all pins (see sequencer::channelManager for details)
 * has to be an integer number, which gives the bit that represents the pin. I.e. if we have pins P0.0..P0.7 where port="P0."
 * then pin=0..7 and P0.0 is represented by the least-significent bit (Bit 0), P0.1 by bit 1 and so on. The given \c width
 * will be used for the port. So if width=8 then the array will be of type uint8_t. If width=16 the array will be of type
 * uint16_t and so on.
 * If grouping is not \c pin_name then the width of the port is determined by the software and the according line in the
 * file will be ignored.
 *
 * \par initial values of channels
 * There are two methods clear_initialvalues() and add_initialvalues() that can be used to set the initial values of any
 * channel before init() is called. A call to init() will generate all the data arrays and initialize them with the values
 * set in \c initialvalues or with 0 if there are no initial values for the channel in question.
 *
 *
 * \bug The \c free grouping for digital channels will not work with NI-DAQmx driver, as the driver requires that in this
 *      mode there is one array column for each channel and not one bit for each channel! I will have to change this some
 *      time...
 *
 * \test a test application for the class sequencer::NIsequenceGenerator can be foun in \link seqgen_test.cpp.
 */
class NIsequenceGenerator: public sequenceGenerator
{
    protected:

        /** \brief this contains the size of the data arrays in samples, which is visible to the outside
         *         data compression may reduce this. The currently allocated memory can be found in cur_array_size */
        unsigned long long array_size;

        /** \brief this contains the allocated (!) size of the data arrays in samples */
        unsigned long long cur_array_size;

        /** \brief number of MegaBytes allocated for the sequence */
        double memusage;

        /** \brief number of bytes used for one sample */
        double memusage_sample;

        /** \brief this vector stores the data arrays */
        std::vector<sg_arraydata> arrays;

        /** \brief internal: map that provides information about the channels used in this sequence */
        std::map<std::string, sg_channel> channels;

        /** \brief internal: a map that provides information about the used ports (whether to bind and how to bind their pins together) */
        std::map<std::string, sg_portinfo> ports;

        /** \brief internal: this map provides initial values for the channels. Use clear_initialvalues() and add_initialvalue() to set them */
        std::map<std::string, double> initialvalues;

        /** \brief internal: filename of \c ports.ini file */
        std::string portconfigfilename;

        /** \brief internal: filename of \c timing.ini file */
        std::string timingconfigfilename;

        /** \brief specifies whether to use double (false) or int16 (true) arrays for analog output channels */
        bool use_binary16;


         /** \brief sorts the channels within one array and sets the adress_string properties of the members of arrays vector
          *
          * This method first createas a vector of sg_sortarraydata. This vector therefore contains a list of the pin
          * names (hardware name) and the offsets of all pins within one array. This list is then sorted by a
          * simple <a href="http://en.wikipedia.org/wiki/Bubble_sort">bubble sort algorithm</a>. To be more exact a
          * variant of bubble sort is used that only sorts as long as there are any swaps. This leads to \f$\mathcal{O}(N)\f$ behaviour
          * on sorted arrays and not \f$\mathcal{O}(N^2)\f$ like the standard implementation.
          *
          * The method \c sort_array works as follows:
          *   -# generate a vector that contains all the channels that belong to array \c num
          *   -# sort the vector from step 1 according to the \c hardware_name s.
          *   -# write back the sorted array to the channels map
          *   -# set the adress_string property of sg_arraydata items in the arrays vector.
          * .
          *
          * The channels are sorted in ascending order of their pin names. If possible the pin names are converted
          * to numbers before a compare, so this routine will result in <code>1,2,6,9,10,11,12</code> and not in <code>1,10,11,12,2,6,9 </code>
          * as a simple string compare would. This is only possible if the pin name consists of a number only. There may
          * not be any non-numeric characters.
          *
          * For analog output ports this is always this way. For digital ports there are two possibilities:
          *  -# sort pins into array item
          *  -# pin name = bit number
          * .
          * The first case is the standard case, but if there is contradictory information in the ports map, i.e.
          * ports[..].format=sgUInt8, sgUInt16, sgUInt32 (may be defined in the ports.ini file, see documentation of
          * NIsequenceGenerator class), the sorting step is replaced by the following behaviour:
          *  - The port width in the ports.ini file gives the width of the port (8, 16, 32 bits). If this
          *    information is not provided the highes pin number determines the port width
          *  - Each pin is sorted into the array item according to its pin name, which has to be an integer
          *    number which indicates one bit in the port.
          *  .
          *
          * \attention As the last step from above may change the width of the port the sort_array() method has to
          * be called before allocating memory for the ports, or a reallocation has to be done. Within init() it is
          * simply called before the allcoation step for each array.
          */
        void sort_array(unsigned long num);

         /** \brief generates the hardware adress from the given channel (\c device+port+pin )
          *
          * This method uses NISEQGEN_DEVICE_PORT_DIVIDER and NISEQGEN_PORT_PIN_DIVIDER to form the complete hardware
          * adress. It works as follows:
          * - The strings are concatenated in the order \c device + \c port + \c pin.
          * - NISEQGEN_DEVICE_PORT_DIVIDER is put after the device name if the device name is non-empty.
          * - NISEQGEN_PORT_PIN_DIVIDER is put after the port name if the port name is non-empty.
          * .
          */
        std::string get_hardware_adress(cm_channel channel);

         /** \brief read in the ports.ini file This simply clears all contents from the private ports variable if there is an error while reading the file (file does not exist, wrong file format ...)
          */
        void read_ports_ini();

        /** \brief returns the digital value at the specified time from the given channel, this is a fast version of get_digital() which omits the error checking, <b>for internal use only</b>, this also returns false on an error and does not throw an exception! */
        inline bool qget_digital(std::string channel, double timestep){
          unsigned int num=get_array_num(channel);
          unsigned char offset=get_offset(channel);
          sg_dataformat df=get_dataformat(channel);
          if (df==sgUInt32) {
            return bit_is_set(((uint32_t*)arrays[num].data)[time_to_index(timestep)], offset);
          }
          if (df==sgUInt8) {
            return bit_is_set(((uint8_t*)arrays[num].data)[time_to_index(timestep)], offset);
          }
          if (df==sgUInt16) {
            return bit_is_set(((uint16_t*)arrays[num].data)[time_to_index(timestep)], offset);
          }
          return false;
        };

        /** \brief returns the analog value at the specified time from the given channel, this is a fast version of get_analog() which omits the error checking, <b>for internal use only</b> */
        inline double qget_analog(std::string channel, double timestep){
          unsigned int num=get_array_num(channel);
          sg_dataformat df=get_dataformat(channel);
          if (df==sgDouble)
            return ((double*)arrays[num].data)[time_to_index(timestep)*arrays[num].channels+get_offset(channel)];
          if (df==sgBinary16)
            return 10.0*(double)((int16*)arrays[num].data)[time_to_index(timestep)*arrays[num].channels+get_offset(channel)]/32768.0;
        };

        /** \brief This is a private implementation of calloc() (see libc) that is used to allocate
         *         the memory needed by this class. This is done to easily implement an alternative
         *         memory management code (i.e. direct system calls rather than libc calls
         */
        void* private_calloc(size_t count, size_t eltsize);

        /** \brief This is a private implementation of free() (see libc) that is used to free allocated
         *         memory in this class. This is done to easily implement an alternative
         *         memory management code (i.e. direct system calls rather than libc calls
         */
        void private_free (void *ptr, size_t size);

        /** \brief private_calloc() and private_free() use this method to return extended error information */
        std::string private_mem_error_info;

        /** \brief variable timing: specifies whether to use variable timing or not */
        bool use_variable_timing;

        /** \brief variable timing: this stores a list of the available timing steps */
        std::vector<unsigned long> vartiming_steps;

        /** \brief name of the channel that is connected to the \c sel0 line of the variable timing hardware */
        std::string vartiming_sel0;
        /** \brief name of the channel that is connected to the \c sel1 line of the variable timing hardware */
        std::string vartiming_sel1;
        /** \brief name of the channel that is connected to the \c sel2 line of the variable timing hardware */
        std::string vartiming_sel2;
        /** \brief name of the channel that is connected to the \c sel3 line of the variable timing hardware */
        std::string vartiming_sel3;
        /** \brief name of the channel that is connected to the \c stop line of the variable timing hardware */
        std::string vartiming_stop;

        /** \brief variable timing: the INI file specifying the variable timing electronis */
        std::string vartiming_inifile;

        /** \brief variable timing: read the variable-timing.ini file */
        void vartiming_read_ini();

        /** \brief stores the specified sample \a s in the arrays vector (field compare_data of sg_arraydata)
          *        to be able to compare this sample to others with compare_to_stored_sample() */
        void store_sample_for_compare(unsigned long long s);

        /** \brief compares the specified sample to the one stored with store_sample_for_compare() */
        bool compare_to_stored_sample(unsigned long long s);

        /** \brief this encodes the variable timing info in the sepcified sample \a s. \a sel gives the length
          *        of the new sample (as an array index in vartiming_steps) and \a stop determines whether this
          *        is the last sample. */
        void store_vartiming_info(unsigned long long s, unsigned char sel, bool stop);

        /** \brief this tool method sets the specified digital channel \a channel on the sample \a s to the
          *        the specified \a value. */
        void vartiming_set_digital(std::string channel, unsigned long long s, bool value);

        /** \brief this tool method returns the specified digital channel \a channel on the sample \a s  */
        bool vartiming_get_digital(std::string channel, unsigned long long s);

        /** \brief copy the sample \a from to the sample \a to. */
        void copy_sample(unsigned long long from, unsigned long long to);
	public:
        /** \brief class constructor */
        NIsequenceGenerator();
        /** \brief class destructor */
        ~NIsequenceGenerator();

        /** \brief set use_binary16 property */
        SetMacro(bool, use_binary16);
        /** \brief get use_binary16 property */
        GetMacro(bool, use_binary16);

        /** \brief get use_variable_timing property */
        GetMacro(bool, use_variable_timing);
        /** \brief set use_variable_timing property */
        SetMacro(bool, use_variable_timing);


        /** \brief clear the \c initialvalues map */
        inline void clear_initialvalues() { initialvalues.clear(); };

        /** \brief add an entry to the \c initialvalues map. for digital lines use \c 0.0 and \c 1.0 to set the values*/
        inline void add_initialvalue(std::string channel, double value) { initialvalues[channel]=value; };

        /** \brief this can be called after init() and sets all the channels for which initialvalues contains a value to that value (if non-zero, as this is set by init() ). */
        void use_initialvalues();

         /** \brief calculates the corresponding array index to a given time */
        inline unsigned long long time_to_index(double timep) {
          //if (timep==end_time) return array_size-1;
          return (unsigned long long)(timep/sample_timestep);
        };

         /** \brief get the number of entries of a data array */
        GetMacro(unsigned long long, array_size);

         /** \brief set the file that contains more information about the ports of the single devices */
        SetMacro(std::string, portconfigfilename);
         /** \brief get the file that contains more information about the ports of the single devices */
        GetMacro(std::string, portconfigfilename);
         /** \brief set the \c timing.ini file name */
        SetMacro(std::string, timingconfigfilename);
         /** \brief get the \c timing.ini file name */
        GetMacro(std::string, timingconfigfilename);


        /** \brief returns the number of channels */
        inline unsigned long get_channel_count(){
          return channels.size();
        };

        /** \brief returns the number of memory arrays */
        inline unsigned long get_array_count(){
          return arrays.size();
        };

        /** \brief returns the data array that belongs to the given channel */
        inline void* get_array(std::string channelname){
          if(!exists(channelname)) {
            SEQUENCER_ERROR(SEQUENCER_ERROR_CNOTEXIST_NUM, get_errormessage(SEQUENCER_ERROR_CNOTEXIST_NUM), "NIsequenceGenerator.get_array(\""+channelname+"\")");
          }
          return get_array(channels[channelname].data);
        };

        /** \brief returns the data array of the num-th channel*/
        void* get_array(unsigned long num);

        /** \brief returns the data array number that belongs to the given channel */
        inline unsigned long get_array_num(std::string channelname){
          if(!exists(channelname)) {
            SEQUENCER_ERROR(SEQUENCER_ERROR_CNOTEXIST_NUM, get_errormessage(SEQUENCER_ERROR_CNOTEXIST_NUM), "NIsequenceGenerator.get_array(\""+channelname+"\")");
          }
          return channels[channelname].data;
        };

        /** \brief returns the data array number of the num-th channel*/
        unsigned long get_array_num(unsigned long num);

        /** \brief returns the data format of the array that belongs to the given channel */
        inline sg_dataformat get_dataformat(std::string channelname){
          if(!exists(channelname)) {
            SEQUENCER_ERROR(SEQUENCER_ERROR_CNOTEXIST_NUM, get_errormessage(SEQUENCER_ERROR_CNOTEXIST_NUM), "NIsequenceGenerator.get_dataformat(\""+channelname+"\")");
          }
          return get_nth_dataformat(channels[channelname].data);
        };

        /** \brief returns the data format of the array of the num-th channel */
        sg_dataformat get_dataformat(unsigned long num);

        /** \brief returns the offset of the given channel */
        inline unsigned char get_offset(std::string channelname){
          if(!exists(channelname)) {
            SEQUENCER_ERROR(SEQUENCER_ERROR_CNOTEXIST_NUM, get_errormessage(SEQUENCER_ERROR_CNOTEXIST_NUM), "NIsequenceGenerator.get_offset(\""+channelname+"\")");
          }
          return channels[channelname].offset;
        };

        /** \brief returns the offset of the num-th channel */
        unsigned char get_offset(unsigned long num);

        /** \brief returns the analog value at the specified time from the given channel */
        inline double get_analog(std::string channel, double timestep){
          if (timestep>end_time) SEQUENCER_ERROR(SEQUENCER_ERROR_AFTERTIMEEND_NUM, get_errormessage(SEQUENCER_ERROR_AFTERTIMEEND_NUM), "NIsequenceGenerator.get_analog(\""+channel+"\", "+floattostr(timestep)+")");
          if (get_dataformat(channel)!=sgDouble && get_dataformat(channel)!=sgBinary16) SEQUENCER_ERROR(SEQUENCER_ERROR_NOTANALOG_NUM, get_errormessage(SEQUENCER_ERROR_NOTANALOG_NUM), "NIsequenceGenerator.get_analog(\""+channel+"\", "+floattostr(timestep)+")");
          unsigned int num=get_array_num(channel);
          sg_dataformat df=get_dataformat(channel);
          if (df==sgDouble)
            return ((double*)arrays[num].data)[time_to_index(timestep)*arrays[num].channels+get_offset(channel)];
          if (df==sgBinary16)
            return 10.0*(double)((int16*)arrays[num].data)[time_to_index(timestep)*arrays[num].channels+get_offset(channel)]/32768.0;
        };

        /** \brief returns the digital value at the specified time from the given channel */
        inline bool get_digital(std::string channel, double timestep){
          if (timestep>end_time) SEQUENCER_ERROR(SEQUENCER_ERROR_AFTERTIMEEND_NUM, get_errormessage(SEQUENCER_ERROR_AFTERTIMEEND_NUM), "NIsequenceGenerator.get_digital(\""+channel+"\", "+floattostr(timestep)+")");
          unsigned int num=get_array_num(channel);
          unsigned char offset=get_offset(channel);
          sg_dataformat df=get_dataformat(channel);
          if (df==sgUInt32) {
            return bit_is_set(((uint32_t*)arrays[num].data)[time_to_index(timestep)], offset);
          }
          if (df==sgUInt8) {
            return bit_is_set(((uint8_t*)arrays[num].data)[time_to_index(timestep)], offset);
          }
          if (df==sgUInt16) {
            return bit_is_set(((uint16_t*)arrays[num].data)[time_to_index(timestep)], offset);
          }
          SEQUENCER_ERROR(SEQUENCER_ERROR_NOTDIGITAL_NUM, get_errormessage(SEQUENCER_ERROR_NOTDIGITAL_NUM), "NIsequenceGenerator.get_digital(\""+channel+"\", "+floattostr(timestep)+")");
        };

        /** \brief reads the value of the channel at the end of the sequence. If the channel does not exist within
         *         the sequence the value \a defaultval is returned. this method returns a double that also codes
         *         the value of digital channels: 0.0=false, 1.0=true
         */
        double get_endvalue(std::string channel, double defaultval);

        /** \brief returns the num-th data array */
        inline void* get_nth_array(unsigned long num){
          if (num>=get_array_count()) {
            SEQUENCER_ERROR(SEQUENCER_ERROR_RANGE_NUM, get_errormessage(SEQUENCER_ERROR_RANGE_NUM), "NIsequenceGenerator.get_nth_array("+inttostr(num)+")");
          }
          return arrays[num].data;
        };

        /** \brief returns the data format of the nth array */
        inline sg_dataformat get_nth_dataformat(unsigned long num){
          if (num>=get_array_count()) {
            SEQUENCER_ERROR(SEQUENCER_ERROR_RANGE_NUM, get_errormessage(SEQUENCER_ERROR_RANGE_NUM), "NIsequenceGenerator.get_nth_dataformat("+inttostr(num)+")");
          }
          return arrays[num].format;
        };

        /** \brief returns the number of channels represented the nth array */
        inline unsigned char get_nth_channels(unsigned long num){
          if (num>=get_array_count()) {
            SEQUENCER_ERROR(SEQUENCER_ERROR_RANGE_NUM, get_errormessage(SEQUENCER_ERROR_RANGE_NUM), "NIsequenceGenerator.get_nth_channels("+inttostr(num)+")");
          }
          return arrays[num].channels;
        };

        /** \brief returns the adress_string of the nth array */
        inline std::string get_nth_adress_string(unsigned long num){
          if (num>=get_array_count()) {
            SEQUENCER_ERROR(SEQUENCER_ERROR_RANGE_NUM, get_errormessage(SEQUENCER_ERROR_RANGE_NUM), "NIsequenceGenerator.get_nth_adress_string("+inttostr(num)+")");
          }
          return arrays[num].adress_string;
        };

        /** \brief returns the name of the nth channel */
        std::string get_name(unsigned long num);

        /** \brief initialize the object for sequence generation and set \c sample_timestep and \c end_time.
         *         The channels will be taken from the sequencer::channelManager object.
         *
         * This method goes through the following steps:
         * \verbatim
  (I) clear all information in the object and free all allocated memory
 (II) for each channel in the channelManager object:
      1. check whether there is already an array for the device and port of the channel
           if there is:      add the channel to this array
           if there is not:  create a new array, do not get memory for this array !!!
         So far input channels are ignored by this method and the complete object.
      2. generate and save the channel information
(III) allocate memory using calloc (also initializes to 0)
           \endverbatim
         *
         * In step \c (II) the method also determines the type of the data array depending on the type of the channel.
         * For analog channels an existing double array is beeing resized and the according offset is saved. Digital channels
         * are saved bit-wise in \c uint<i>n</i>_t arrays. One array per port on each device. If there are more
         * than 8 channels in one digital port the array is changed to 16 and if there are more than 16 channels to 32
         * bit width. So there may not be ports with more than 32 digital output lines. This method sorts the channels
         * inside the ports. So far they are beeing sorted alphabetically.
         *
         * If no \c end_time is provided (i.e. end_time=-1) then end_time=10*sample_timestep
         *
         * This method uses the private method sort_array() to sort the pins within one port. For details of the sorting
         * see there.
         */
        void init(channelManager* cm, double sample_timestep, double end_time=-1);

        /** \brief returns whether the given channel exists in this object */
        inline bool exists(std::string channel){
          return (channels.find(channel)!=channels.end());
        };


        /** \brief sets the value of an analog channel at the specified time  */
        inline virtual void set_analog(std::string channel, double timestep, double value) {
          if (timestep>end_time) SEQUENCER_ERROR(SEQUENCER_ERROR_AFTERTIMEEND_NUM, get_errormessage(SEQUENCER_ERROR_AFTERTIMEEND_NUM), "NIsequenceGenerator.set_analog(\""+channel+"\", "+floattostr(timestep)+", "+floattostr(value)+")");
          if (get_dataformat(channel)!=sgDouble && get_dataformat(channel)!=sgBinary16) SEQUENCER_ERROR(SEQUENCER_ERROR_NOTANALOG_NUM, get_errormessage(SEQUENCER_ERROR_NOTANALOG_NUM), "NIsequenceGenerator.set_analog(\""+channel+"\", "+floattostr(timestep)+", "+floattostr(value)+")");
          sg_arraydata a=arrays[get_array_num(channel)];
          sg_dataformat df=get_dataformat(channel);
          if (df==sgDouble) {
            ((double*)a.data)[time_to_index(timestep)*a.channels+get_offset(channel)]=value;
          } else {
            ((int16*)a.data)[time_to_index(timestep)*a.channels+get_offset(channel)]=(int16)(value/10.0*32767);
          }

        };

        /** \brief sets the value of an analog channel in the specified time interval */
        virtual void set_analog(std::string channel, double time_start, double time_stop, double value);

        /** \brief sets the values of an analog channel in the specified time interval to form a linear ramp */
        virtual void set_linear(std::string channel, double time_start, double time_stop,  double startvalue, double endvalue);

        /** \brief sets the values of an analog channel in the specified time interval to form a linear ramp beginning at the value of the channel at time_start */
        virtual void set_linear(std::string channel, double time_start, double time_stop, double endvalue);

        /** \brief sets the value of ta digital channel at the specified time  */
        virtual void set_digital(std::string channel, double timestep, bool value);

        /** \brief sets the value of a digital channel in the specified time interval */
        virtual void set_digital(std::string channel, double time_start, double time_stop, bool value);


        /** \brief returns a report that contains information about the data saved in the object */
        virtual std::string report();

        /** \brief clear all information in the object (channels ...) and free all allocated memory */
        virtual void clear();

        /** \brief returns true if the give channel is used in this sequence or if it is part of a port that is represented completely by one array used in this sequence. */
        virtual bool is_used(std::string channel);

        /** \brief returns the Megabytes of memory used for the sequence */
        double get_memory_usage();


        /** \brief add a channel to the list of used channels. The channel has to be synchronizable (property \c synchronized in channelManager has to be \c true ) */
        virtual inline void add_used(std::string channel) {
          if (used_channels.find(channel)==used_channels.end()) {
            if (cmanager->get(channel).role==cmStandardChannel) {
              used_channels.insert(channel);
            } else {
              SEQUENCER_ERRORV(SEQUENCER_ERROR_CNOTSYNCH_NUM, "NIsequenceGenerator::add_used("+channel+")", channel.c_str());
            }
          }
        }

        /** \brief compresses the arrays for use with variable timing (this only changes the data if
         *         use_variable_timing is true). For now this method uses simple run-length encoding!
         *
         */
        virtual void compress();

        /** \brief saves the current memory contents into CSV files. Also generates a GnuPlot-Skript to plot
         *         the sequences.
         */
       virtual void save_to_csv(std::string datafilename, std::string plotfilename);

};

}
#endif // NISEQUENCEGENERATOR_H


/*@}*/
