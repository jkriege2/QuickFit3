/**
 * \ingroup seqplot
 */
/*@{*/

/** \file sequenceplotter.h
 */

#ifndef SEQUENCEPLOTTER_H
#define SEQUENCEPLOTTER_H

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

/**
 * \brief generates the sequence as a memory array from simple instructions. Then it generates an image in PNG format from this sequence (graph: voltage vs. time)
 * \ingroup seqplot
 *
 * This class can be used to draw a sequence in order to inspect it or to show the user what
 * happens in his sequence definition file. It is very easy to use, as it is derived from
 * sequencer::sequenceGenerator from which it inherits all methods and functions.
 *
 * There are mainly two new functions added:
 * \li <tt>void plot_png(channelManager* cm, std::string filename, unsigned long height);</tt><br>
 *    plots all channels and the complete sequence
 * \li <tt>void plot_png(channelManager* cm, std::string filename, unsigned long height, double tstart, double tstop);</tt><br>
 *    plots all channels, but only a part [\c tstart ... \c tstop ] of the sequence.
 *
 * If you don't want to generate a plot of all channels it is possible to generate a list of the
 * channels to be plottet. To do so, simply use the sel_add_channel() method. If you want to clear
 * the list use sel_clear_channels(). If the list is empty (after calling sel_clear_channels()) all
 * channels will be plottet!
 *
 * Some pins may have an internal (linear) scaling (or inversion for digital channels). You may choose between
 * two output modes:
 *  -# plot the real output of the DA-Converters/digital pins (\c set_plot_scaled(false) )
 *  -# plot the scaled values (\c set_plot_scaled(true) )
 * .
 *
 * You can also plot so called markers. That are vertical lines that span all plots and that have some text
 * at the bottom of the plot. They can have different colors, depending on the type of a marker. There are
 * information (blue), error (red) and warning (green) markers. They can be added to the plot by registering
 * them with add_info_marker(), add_error_marker() and add_warning_marker(). With clear_markers() all currently
 * registered markers will be deleted.
 *
 * This class uses the plotter class to output the graphics. Therefore you have to link your
 * program to the cairo graphics library (see documentation of plotter for details).
 *
 * sequencePlotter first generates a memory sequence from the instructions it receives
 * (sequencer::sequenceGenerator interface) and then uses these data to output the sequence into
 * a file. So it needs a bit more (for the pixel image) of memory than the sequencer::NIsequenceGenerator
 * class.
 *
 * Here is an example code:
 * \code
// read in channels from channels.ini
channelManager cmanager;
cmanager.read_ini("channels.ini");
sequencePlotter gen;

// init the plotter class
gen.init(cmanager, 5, 500);
gen.set_smooth_analog_graphs(false);

// set some signal changes/define sequence
gen.set_analog("AO2", 0, -10);
gen.set_linear("AO0", 0, 50.0, 10.0);
gen.set_linear("AO0", 50, 100.0, 0);
gen.set_linear("AO0", 100, 150.0, 2);
gen.set_linear("AO0", 150, 200.0, -10.0);
gen.set_linear("AO2", 0, 500.0, 10.0);
gen.set_analog("AO0", 200, 220, 10.0);
gen.set_linear("AO0", 220.0, 500, -5);
gen.set_digital("DO5", 50, 50.23, 1);
gen.set_digital("DO2", 20, 50.23, 1);
gen.set_digital("DO1", 10, 30, 1);
gen.set_digital("DO11", 100, 155, 1);
gen.set_digital("DO4", 40, 55, 1);
gen.set_digital("DO9", 90, 155, 1);
gen.set_digital("DO7", 70, 155, 1);
gen.set_digital("DO8", 80, 450, 1);
double told=20;
for (double t=20; t<=430; t+=5) {
  double y=5*sin((t-20.0)/50.0*2*M_PI)+3;
  gen.set_linear("AO1", told, t, y);
  told=t;
}

// add soime markers
gen.add_info_marker(20, "sin:\nstart");
gen.add_info_marker(430, "sin:\nend");
gen.add_error_marker(100, "error");
gen.add_warning_marker(250, "warn");

// output the sequence to seqtestplot.png which has a height of 1500 pixels
gen.plot_png(&cmanager, "seqtestplot.png", 1500);
   \endcode
 * And here is the result of this code:
 *  \image html seqtestplot.png
 * If you use the second variety of the plot_png() function, it is possible to plt only a part of the sequence:
 * \code
...
gen.plot_png(&cmanager, "seqtestplot_small.png", 1500, 50,220);
   \endcode
 * This will give this result:
 *  \image html seqtestplot_small.png
 * you can also generate a list of channels to be plottet:
 * \code
...
// clear channel selection and add two channels
gen.sel_clear_channels();
gen.sel_add_channel("AO0");
gen.sel_add_channel("DO11");
// plot only these two channels
gen.plot_pdf(&cmanager, "seqtestplot2_small.pdf", 25.4, 50,220);
   \endcode
 *
 *
 * <b>internal information:</b>
 * The code of
 * this class is based on an earlier version of sequencer::NIsequenceGenerator. It was decided
 * not to inherit it from sequencer::NIsequenceGenerator as there could be internal changes in
 * the lather one that would lead to a need to also change the plotter class. So now both classes
 * are independat of each other although they share big deals of their code.
 *
 * The code from sequencer::NIsequenceGenerator was somehow simplified, as it is not important in
 * which way data is sorted in the memory, as this is only important for classes that generate real
 * hardware output arrays. Here the memory arrays only serve to represent the sequence in order
 * to be able to plot it. So mainly the sorting is omitted and there is no sort_array() method, like
 * NIsequenceGenerator.sort_array(). Also the mechanism to read a \c ports.ini file is left out of
 * this class, as it is not needed. Memory allocation simply expands digital ports so that all pins
 * fit into them. On which bit they exactly lie and how wide the port is, is not important for this
 * class.
 *
 * The following graphic shows all the configuration parameters:
 *  \image html sequence_plotter_sizes.png
 *
 * To plot a graph the graph coordinate system has to be transformed into the paper coordinate system.
 * This is done as follows:
 *  \image html sequence_plotter_coordsystem.png
 *
 *  \f[ \mathrm{px}_t=x_0'+t\cdot\mbox{scalex} \f]
 *  \f[ \mathrm{py}_t=y_0'-\mbox{scaley}\cdot\left(f(t)-\mbox{shifty}\right) \f]
 *
 * The parameters are beeing calculatet this way:
 *  \f[ \mathrm{height}=2\cdot\mbox{plot\_hheight} \f]
 *  \f[ \mathrm{height'}=\left|\mathrm{max}-\mathrm{min}\right| \f]
 *  \f[ \mathrm{scalex}=\mbox{coord\_x1\_range} \f]
 *  \f[ \mathrm{scaley}=\frac{\mbox{height}}{\mbox{plotheight}} \f]
 *  \f[ \mathrm{shifty}=\mbox{min} \f]
 *  \f[ x_0'=x_0+\Delta x_0 \f]
 *  \f[ y_0'=y_0+\frac{\mbox{height}-\mbox{plotheight}}{2}+\mbox{height'} \f]
 */
class sequencePlotter: public cairoSequencePlotter
{
  protected:
    /** \brief show smooth analog graphs (\c true ) or show the real step-function (\c false )*/
    bool smooth_analog_graphs;

    /** \brief plot the actual data (graphs) */
    virtual void plot_graphs();
  public:
    /** \brief set the drawing mode of analog graphs: show smooth analog graphs (\c true ) or show the real step-function (\c false ). */
    SetMacro(bool, smooth_analog_graphs);
    /** \brief get the drawing mode of analog graphs: show smooth analog graphs (\c true ) or show the real step-function (\c false ). */
    GetMacro(bool, smooth_analog_graphs);

    /** \brief class constructor */
    sequencePlotter();
  	/** \brief class destructor */
  	~sequencePlotter();




  /* copied from NIsequenceGenerator ---------------------------------------------------------------------------------------- */
    protected:
        unsigned long long array_size;
        std::vector<sg_arraydata> arrays;
        std::map<std::string, sg_portinfo> ports;


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
          return ((double*)arrays[num].data)[time_to_index(timestep)*arrays[num].channels+get_offset(channel)];
        };
  public:
         /** \brief calculates the corresponding array index to a given time */
        inline unsigned long long time_to_index(double timep) {
          return (unsigned long long)(timep/sample_timestep);
        };
  protected:
         /** \brief get the number of entries of a data array */
        GetMacro(unsigned long long, array_size);

        /** \brief returns the number of memory arrays */
        inline unsigned long get_array_count(){
          return arrays.size();
        };

        /** \brief returns the data array that belongs to the given channel */
        inline void* get_array(std::string channelname){
          if(!exists(channelname)) {
            SEQUENCER_ERRORN(SEQUENCER_ERROR_CNOTEXIST_NUM, "sequencePlotter.get_array(\""+channelname+"\")");
          }
          return get_array(channels[channelname].data);
        };

        /** \brief returns the data array of the num-th channel*/
        void* get_array(unsigned long num);

        /** \brief returns the data array number that belongs to the given channel */
        inline unsigned long get_array_num(std::string channelname){
          if(!exists(channelname)) {
            SEQUENCER_ERRORN(SEQUENCER_ERROR_CNOTEXIST_NUM, "sequencePlotter.get_array(\""+channelname+"\")");
          }
          return channels[channelname].data;
        };

        /** \brief returns the data array number of the num-th channel*/
        unsigned long get_array_num(unsigned long num);

        /** \brief returns the data format of the array that belongs to the given channel */
        inline sg_dataformat get_dataformat(std::string channelname){
          //std::cout<<"get_dataformat("<<channelname<<")     ";
          if(!exists(channelname)) {
            SEQUENCER_ERRORN(SEQUENCER_ERROR_CNOTEXIST_NUM, "sequencePlotter.get_dataformat(\""+channelname+"\")");
          }
          //std::cout<<"ch_index="<<channels[channelname].data<<"     ";
          return get_nth_dataformat(channels[channelname].data);
        };

        /** \brief returns the data format of the array of the num-th channel */
        sg_dataformat get_dataformat(unsigned long num);

        /** \brief returns the offset of the given channel */
        inline unsigned char get_offset(std::string channelname){
          if(!exists(channelname)) {
            SEQUENCER_ERRORN(SEQUENCER_ERROR_CNOTEXIST_NUM, "sequencePlotter.get_offset(\""+channelname+"\")");
          }
          return channels[channelname].offset;
        };

        /** \brief returns the offset of the num-th channel */
        unsigned char get_offset(unsigned long num);

        /** \brief returns the analog value at the specified time from the given channel */
        inline double get_analog(std::string channel, double timestep){
          if (timestep>end_time) SEQUENCER_ERRORN(SEQUENCER_ERROR_AFTERTIMEEND_NUM, "sequencePlotter.get_analog(\""+channel+"\", "+floattostr(timestep)+")");
          if (get_dataformat(channel)!=sgDouble) SEQUENCER_ERRORN(SEQUENCER_ERROR_NOTANALOG_NUM, "sequencePlotter.get_analog(\""+channel+"\", "+floattostr(timestep)+")");
          unsigned int num=get_array_num(channel);
          return ((double*)arrays[num].data)[time_to_index(timestep)*arrays[num].channels+get_offset(channel)];
        };

        /** \brief returns the digital value at the specified time from the given channel */
        inline bool get_digital(std::string channel, double timestep){
          if (timestep>end_time) SEQUENCER_ERRORN(SEQUENCER_ERROR_AFTERTIMEEND_NUM, "sequencePlotter.get_digital(\""+channel+"\", "+floattostr(timestep)+")");
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
          SEQUENCER_ERRORN(SEQUENCER_ERROR_NOTDIGITAL_NUM, "sequencePlotter.get_digital(\""+channel+"\", "+floattostr(timestep)+")");
        };

        /** \brief returns the num-th data array */
        inline void* get_nth_array(unsigned long num){
          if (num>=get_array_count()) {
            SEQUENCER_ERRORN(SEQUENCER_ERROR_RANGE_NUM, "sequencePlotter.get_nth_array("+inttostr(num)+")");
          }
          return arrays[num].data;
        };

        /** \brief returns the data format of the nth array */
        inline sg_dataformat get_nth_dataformat(unsigned long num){
          if (num>=get_array_count()) {
            SEQUENCER_ERRORN(SEQUENCER_ERROR_RANGE_NUM, "sequencePlotter.get_nth_dataformat("+inttostr(num)+")");
          }
          return arrays[num].format;
        };

        /** \brief returns the number of channels represented the nth array */
        inline unsigned char get_nth_channels(unsigned long num){
          if (num>=get_array_count()) {
            SEQUENCER_ERRORN(SEQUENCER_ERROR_RANGE_NUM, "sequencePlotter.get_nth_channels("+inttostr(num)+")");
          }
          return arrays[num].channels;
        };

        /** \brief returns the adress_string of the nth array */
        inline std::string get_nth_adress_string(unsigned long num){
          if (num>=get_array_count()) {
            SEQUENCER_ERRORN(SEQUENCER_ERROR_RANGE_NUM, "sequencePlotter.get_nth_adress_string("+inttostr(num)+")");
          }
          return arrays[num].adress_string;
        };


  public:
        /** \brief initialize the object for sequence generation and set \c sample_timestep and \c end_time. The channels will be taken from the sequencer::channelManager object.
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
         * This method uses the private method sort_array() to sort the pins within one port. For details of the sorting
         * see there.
         */
        void init(channelManager* cm, double sample_timestep, double end_time=-1);

  protected:
        /** \brief returns shether the given channel exists in this object */
        inline bool exists(std::string channel){
          return (channels.find(channel)!=channels.end());
        };
  public:


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
  /* ------------------------------------------------------------------------------------------------------------------------ */
};

}
#endif // SEQUENCEPLOTTER_H


/*@}*/
