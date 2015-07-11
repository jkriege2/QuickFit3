/**
 * \ingroup seqplot
 */
/*@{*/

/** \file fastsequenceplotter.h
 */

#ifndef FASTSEQUENCEPLOTTER_H
#define FASTSEQUENCEPLOTTER_H

#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <cstdio>
#include <stdint.h>

#include "jkiniparser.h"
#include "plotter.h"
#include "tools.h"
#include "sequencererror.h"
#include "channelmanager.h"
#include "sequencegenerator.h"
#include "nisequencegenerator.h"
#include "cairosequenceplotter.h"


namespace sequencer {

/** \brief possible line segment types
 * \ingroup seqplot
 */
enum fsp_type {
	fspSet1,       /*!< \brief signal is set to 1 (digital) 0->1 */
	fspSet0,       /*!< \brief signal is set to 0 (digital) 1->0*/
	fspLinear,     /*!< \brief linear ramp  */
	fspConstant    /*!< \brief constant range  */
};

/** \brief a line segment
 * \ingroup seqplot
 */
typedef struct {
  fsp_type type;      /*!< \brief type of the segment */
  double tstart;      /*!< \brief start time of the segment */
  double tstop;       /*!< \brief stop time of the segment */
  double vstart;      /*!< \brief start value of the segment */
  double vstop;       /*!< \brief stop value of the segment */
  double dvalue;      /*!< \brief double value of a constant segment */
  bool bvalue;        /*!< \brief bool value of a constant segment */
} fsp_segment;

/** \brief segment list for one channel
 * \ingroup seqplot
 */
typedef struct {
  std::string name;                 /*!< \brief channel name */
  std::vector<fsp_segment> list;    /*!< \brief the list of segments */
} fsp_list;


/**
 * \brief generates an image of a sequence. This class is derived from sequencePlotter and shares the same interface, but it uses another (fatser) drawing method: first a list of instructions (like linear ramps and high-areas) is created then this list will be plottet and not the actual memory array. So this method can NOT show the indidual DA steps, but the plotting is much faster
 * \ingroup seqplot
 *
 * This class breaks the sequence up into small atomic parts which are basically straight lines.
 * So the set_analog and set_digital methods generate lists of these atomic segments that are then
 * interpeted in plot_graphs(). The lists are saved for each channel separately in the vector \c data.
 *
 * This method is much faster than the accurate plotting in sequencePlotter, but it lacks the exactness
 * of the lather one. In this class most quantization and sampling is lost, but it is well suitable for
 * quick plotting of a sequence to get an overlook, what it does.
 */
class fastSequencePlotter: public cairoSequencePlotter
{
  protected:
    /** \brief a vector of segment lists */
    std::vector<fsp_list> data;

    /** \brief implements the new plotting scheme */
  	virtual void plot_graphs();

  	/** \brief sort the entries in the ith fsp_list in data according to the start time */
  	void sort_data_bytime(int i);

  	/** \brief first use sort_data_bytime() and then go through the sorted list and fill in the gaps that could arise between two segments */
  	void clean_list(int i);

  	/** \brief return the type of the given channel (reads from channelManager) */
  	cm_types get_dataformat(std::string channel);

  	/** \brief return the item index of the given channel in the data vector. */
  	int get_channel_num(std::string channel);
  public:

    fastSequencePlotter();
  	/** \brief class destructor */
  	~fastSequencePlotter();




	public:
        /** \brief initialize the object for sequence generation and set \c sample_timestep and \c end_time. The channels will be taken from the sequencer::channelManager object.
         */
        void init(channelManager* cm, double sample_timestep, double end_time=-1);

        /** \brief sets the value of an analog channel at the specified time  */
        virtual void set_analog(std::string channel, double timestep, double value);

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

        virtual double get_analog(std::string channel, double timestep);

        /** \brief returns the digital value at the specified time from the given channel */
        virtual bool get_digital(std::string channel, double timestep);
};

}
#endif // FASTSEQUENCEPLOTTER_H


/*@}*/
