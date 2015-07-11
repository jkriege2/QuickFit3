/*
  Name: channelmanager.h
  Copyright: (c) 2007
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/
/**
 * \ingroup chanman
 */
/*@{*/

/** \file channelmanager.h
 *
 */

#include <string>
#include <map>
#include <vector>
#include <cstdio>

#include "jkiniparser.h"

#include "tools.h"
#include "sequencererror.h"




#ifndef CHANNEL_MANAGER_H
#define CHANNEL_MANAGER_H
namespace sequencer {

/** \brief list of possible channel types
 * \ingroup chanman
 */
enum cm_types {
	cmAnalogOutput,      /*!< \brief analog voltage output */
	cmAnalogInputSE,     /*!< \brief analog single-ended volatge input */
	cmAnalogInputDE,     /*!< \brief analog differential-ended voltage input */
	cmDigitalOutput,     /*!< \brief digital output */
	cmDigitalInput       /*!< \brief digital input */
};

/** \brief list of possible channel roles
 * \ingroup chanman
 */
enum cm_roles {
	cmStandardChannel,         /*!< \brief the channel is a standard channel which is synchronizable (the default role) */
	cmVarTimingChannel,        /*!< \brief this channel is used for the variable timing extension */
	cmUnsynchronizedChannel    /*!< \brief the channel is not synchronizable */
};

/** \brief convert a cm_roles to a std::string
 * \ingroup chanman
 */
std::string cm_role2str(cm_roles type);

/** \brief convert a std::string to a cm_roles
 * \ingroup chanman
 */
cm_roles str2cm_roles(std::string type);


/** \brief convert a cm_types to a std::string
 * \ingroup chanman
 */
std::string cm_types2str(cm_types type);

/** \brief convert a cm_types to a std::string, but uses short names (AO, AISE ...)
 * \ingroup chanman
 */
std::string cm_types2shortstr(cm_types type);

/** \brief convert a std::string to a cm_types
 * \ingroup chanman
 */
cm_types str2cm_types(std::string type);

/** \brief result of any expression
 * \ingroup chanman
 */
typedef struct {
  cm_types type;                  /*!< \brief type of the channel */
  cm_roles role;                  /*!< \brief role of the channel */
  std::string device_name;        /*!< \brief the internal name of the physical device the port/channel belongs to */
  std::string port_name;          /*!< \brief the physical name (=adress) of the DIO-port, i.e. the name in the National Instruments management software */
  std::string pin_name;           /*!< \brief the physical name of the Pin in the port */
  std::string name;               /*!< \brief name with which to access the channel in a sequence definition */
  int device;                     /*!< \brief internal device number of the device the channel blongs to */
  double min, max;                /*!< \brief minimal and maximal voltage allowed on this channel */
  double valmin, valmax;          /*!< \brief minimal and maximal scaled values allowed on this channel */
  double scale, offset;           /*!< \brief scaling for the channel; return scale*value+offset */
  bool invert;                    /*!< \brief invert this digital channel */
  bool required;                  /*!< \brief if \c true then this channel has to be part of every sequence */
//  bool synchronized;              /*!< \brief if \c true then this channel may be used synchronized mode, i.e. sequence (default); if set \c false the the channel may only be used in unsynchronized mode */
} cm_channel;



/**
 * \brief This class manages all available channels of the hardware system.
 * \ingroup chanman
 *
 * Each analog channel of the hardware system is specified by this set of data:
 *
 *   -# \c type (cm_types) the type of the channel (input or output, analog or digital ...)
 *   -# \c role (cm_roles) specifies how the channel can be used (standard [default], variable timing, unsynchronized ...)
 *   -# \c name (std::string) the name with which to access the channel
 *   -# \c device_name (std::string) the internal name of the physical device the port/channel belongs to
 *   -# \c port_name (std::string) the physical name (=adress) of the port, i.e. the name in the
 *                      National Instruments management software
 *   -# \c pin_name (std::string) the physical name of the pin in the port
 *   -# \c min,  \c max (double) minimum and maximum voltage allowed on this channel
 *   -# \c valmin,  \c valmax (double) minimum and maximum scaled values for this channel
 *   -# \c scale (double) a scale factor for this channel (analog channels only)
 *   -# \c offset (double) an offset for this channel (analog channels only)
 *   -# \c invert (bool) specifies whether this digtal channel shell be inverted
 *   -# \c required (bool) specifies whether this channel has to be part of every sequence
 * .
 * Each digital channel of the hardware system is always part of a digital port! This port has a <port_name> to which
 * the name of the actual channel will be concatenated.
 *
 * It is the task of the low-level sequence generation objects (see sequencer::sequenceGenerator ) to generate a valid
 * hardware adress from the parts \c device_name, \c port_name and \c pin_name.
 *
 * All this data is stored in a cm_channel struct for each channel. The resulting set of
 * structs is managed by the channelManager class. This class also allows to read in the configuratio from
 * an INI file of this format:
 * \verbatim
[<name>]
device_name=<text>
port_name=<text>
pin_name=<text>
type=AnalogOutput | AnalogInputSE | AnalogInputDE | DigitalOutput | DigitalInput |
          AO      |      AISE     |      AIDE     |       DO      |      DI
role=standard | vartiming | unsynchronized |
        std   |    vart   |    unsynch
min=<number>
max=<number>
valmin=<number>
valmax=<number>
scale=<number>
offset=<number>
invert= true | false
required= true | false
   \endverbatim
 *
 * It is possible to have two channel configuration files. In this mode one of the two files sets the basic
 * properties of the system (this corresponds to the standard channel.ini file). The second file contains only
 * some information that override the basic file. So you can have one configuration file per experiment that
 * sets some special properties only relevant for one experiment. The data from this second file is stored in
 * the private member \c echannels. The interface is transparent for the user, so all reading functions will
 * check both datasets and return the appropriate data. The writing functions only affect the settings that would
 * have been read. Adding channels always happens in the basic dataset. remove() will delete the channel in both
 * datasets.
 *
 * \test a test application can be found in \link seq_channellist_test.cpp.
 */
class channelManager
{
  private:
    /** \brief a vector containing the channels defined in this system */
    std::map<std::string, cm_channel> channels;

    /** \brief information in this vector overrides information in channels vector. This will be used to
      *        load "experiment-specific" channel configuration
      */
    std::map<std::string, cm_channel> echannels;
	public:
		/** \brief class constructor */
		channelManager();
		/** \brief class destructor */
		virtual ~channelManager();

		/** \brief clear all channel information */
		void clear();

		/** \brief add a new channel */
		void add(cm_channel data);

    /** \brief add a new channel */
		void add(cm_types type, std::string name, std::string device_name, std::string port_name, std::string pin_name, cm_roles role=cmStandardChannel, double scale=1, double offset=0, bool invert=false, double minimum=-10, double maximum=10, double valminimum=-10, double valmaximum=10, bool required=false);

		/** \brief return the number of channels */
		size_t get_channel_count();

		/** \brief remove a channel */
		void remove(std::string channel);

		/** \brief check whether a channel exists */
		bool exists(std::string channel);

		/** \brief get data about a specific channel */
		cm_channel& get(std::string channel);

		/** \brief get data about a specific channel */
		cm_channel& get(size_t channel);

		/** \brief set data about a specific channel */
		void set(std::string channel, cm_channel data);

		/** \brief get data about a specific channel */
		virtual cm_channel& operator[](std::string channel);

		/** \brief get data about a specific channel */
		virtual cm_channel& operator[](size_t channel);

		/** \brief returns an overview over the available channels as string (can be used for reporting)
         *
         * The output is of this form (example):
         * \verbatim
          channel "AI1" (0)
             type:             analog input, differential-ended
             physical name:     "INST1::AI1"
             min..max:          -10..10
             scale, offset:     5, -0.342
          \endverbatim
         *
         */
		std::string as_string();

		/** \brief reads the channel configuration from an INI file. All previous data will be lost.
         *
         * The INI file should have one block of this format for each channel (using jkiniparser to parse the file!):
         * \verbatim
[<name>]
  device_name=<text>
  port_name=<text>
  pin_name=<text>
  type=AnalogOutput | AnalogInputSE | AnalogInputDE | DigitalOutput | DigitalInput |
            AO      |      AISE     |      AIDE     |       DO      |      DI
  role=standard | vartiming | unsynchronized |
          std   |    vart   |    unsynch
  min=<number>
  max=<number>
  valmin=<number>
  valmax=<number>
  scale=<number>
  offset=<number>
  invert= true | false
  required= true | false
          \endverbatim
         *
         */
		void read_ini(std::string filename);

		/** \brief reads in a second ini file, like read_ini() that overrides the settings in this object */
		void read_experiment_ini(std::string filename);

		/** \brief clear the information that was read by read_experiment_ini(). The basic channel settings are conserved. */
		void clear_experiment_ini();

		/** \brief scale a value of the given channel. This centralises the scaling of channels, so it is easy to add mor than linear scaling (log-scaling, polynomial ...). For linear scaling this method calculates \f$y = \mathrm{scale}\cdot x+\mathrm{offset} \f$*/
		double scale_value(std::string channel, double value);
		/** \brief scale a value of the given channel. This centralises the scaling of channels, so it is easy to add mor than linear scaling (log-scaling, polynomial ...). For linear scaling this method calculates \f$y = \mathrm{scale}\cdot x+\mathrm{offset} \f$. */
		double scale_value(size_t channel, double value);
		/** \brief inversely scale a value of the given channel (calculate from a scaled value back).  For linear scaling this method calculates \f$x = \frac{y-\mathrm{offset}}{\mathrm{scale}} \f$ */
		double invscale_value(size_t channel, double value);
		/** \brief inversely scale a value of the given channel (calculate from a scaled value back).  For linear scaling this method calculates \f$x = \frac{y-\mathrm{offset}}{\mathrm{scale}} \f$ */
		double invscale_value(std::string channel, double value);
		/** \brief invert the given value if the channel is configured to do so */
		bool invert_value(std::string channel, bool value);

		/** \brief check whether there is a required channel in this object */
		bool contains_required();

};


};



#endif // CHANNEL_MANAGER_H

/*@}*/
