/*
  Name: nidriver.h
  Copyright: (c) 2007
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/
/**
 * \ingroup drivers
 */
/*@{*/

/** \file nidriver.h
 *
 */

#include <string>
#include <map>
#include <vector>
#include <cstdio>
#include <set>

#include <cstdlib>
#include <ctime>
#include <cmath>
#include <unistd.h>
#include <cstring>

#include "../lib_nidaqmx/NIDAQmx.h"

#include "tools.h"
#include "sequencererror.h"
#include "nisequencegenerator.h"
#include "jkiniparser.h"
#include "driver.h"
#include "logfile.h"
#include "vartimingdriver.h"


#ifndef NIDRIVER_H
#define NIDRIVER_H


namespace sequencer {

/** \brief Executes \c inctsruction and checks for an error. if there was an error then SEQUENCER_ERROR() is called. SEQUENCE_ERROR() is not called for warnings from the driver!
 * \ingroup drivers
 */
#define DAQmxErrChk(instruction, where) \
  { signed long errorcode = (instruction); \
    if( errorcode<0 ) { \
      char errmsg[4096]; \
      DAQmxGetErrorString(errorcode, errmsg, 4096); \
      SEQUENCER_ERROR(SEQUENCER_ERROR_DAQMXERR_NUM, format(get_errormessage(SEQUENCER_ERROR_DAQMXERR_NUM), #instruction, format("%s (NI errorcode %ld)", errmsg, errorcode).c_str()), where); \
    } \
  }

/** \brief possible states of a task within the driver object. The tasks may be in different
 *        states that are explained in the descriotion of class NIdriver.
 *
 * \ingroup drivers
 */
enum nid_task_state {
	nidCreated,        /*!< \brief task has been created and the channels have been registered, timing is set */
	nidData,           /*!< \brief data has been written to the task and it may be executed */
	nidRunning,        /*!< \brief the task is running */
	nidNoTask          /*!< \brief the task is not registered */
};

/** \brief possible states of an input task within the driver object. The tasks may be in
 *         different states that are explained in the descriotion of class NIdriver.
 *
 * \ingroup drivers
 */
enum nid_inputtask_state {
	nidiCreated,        /*!< \brief task has been created and the channels have been registered, timing is set */
	nidiRunning,        /*!< \brief the task is running */
	nidiStopped         /*!< \brief the task is not registered */
};

/** \brief possible states of an un-synchronised task within the driver object.
 * \ingroup drivers
 */
enum nid_ustask_state {
	nidUstUsable,        /*!< \brief task has been created and awaits input or output */
	nidUstStopped         /*!< \brief task has been stopped and you may not do any I/O. */
};

/** \brief converts nid_task_state to a string
 * \ingroup drivers
 */
std::string taskstatetostring(nid_task_state ts);


/** \brief data about one task within a NIdriver object
 * \ingroup drivers
 */
typedef struct {
  TaskHandle task;      /*!< \brief NI-DAQmx task handle */
  nid_task_state state; /*!< \brief current state of the task */
  double min;           /*!< \brief min voltage of analog output task */
  double max;           /*!< \brief max voltage of analog output task */
  long num_samples;     /*!< \brief number of samples to be read by this task */
  std::string channels; /*!< \brief hardware adress */
  cm_types type;        /*!< \brief type of the channel */
  std::string device;   /*!< \brief the device name the task is talking to */
  long channum;         /*!< \brief number of channels in this task */
} nid_task;

/** \brief data about one input task within a NIdriver object
 * \ingroup drivers
 */
typedef struct {
  TaskHandle task;           /*!< \brief NI-DAQmx task handle */
  cm_types type;             /*!< \brief type of the channels in this task */
  nid_inputtask_state state; /*!< \brief current state of the task */
  double min;                /*!< \brief min voltage of analog input task */
  double max;                /*!< \brief max voltage of analog input task */
  std::string hwchannels;    /*!< \brief hardware adress */
  std::vector<std::string> channels; /*!< \brief a list of (logical) channels that are used in this task */
  std::string device;        /*!< \brief the device name the task is talking to */
  bool savetofile;           /*!< \brief save the data into a file \a filename, if the task ends */
  std::string filename;      /*!< \brief the file, used to save the data in */
  long num_samples;          /*!< \brief number of samples to be read by this task */
  double sampletime;         /*!< \brief time between two samples in microseconds */
} nid_inputtask;

/** \brief data about one unsynchronized task within a NIdriver object
 * \ingroup drivers
 */
typedef struct {
  std::string name;         /*!< \brief name of the channel */
  TaskHandle task;          /*!< \brief NI-DAQmx task handle */
  nid_ustask_state state;   /*!< \brief current state of the task */
  std::string channel;      /*!< \brief hardware adress */
  cm_types type;            /*!< \brief type of the channel */
  double value;             /*!< \brief current value of an output channel ... this is to keep trak of what we set the channels to! */
} nid_ustask;

/**
 * \brief This class is a hardware driver for National Instruments hardware using <b>National Instruments DAQmx 8.6</b>
 *        driver. It assumes a set of I/O cards that are accessible over the driver interface of NI-DAQmx. basically you
 *        can controll all cards that are registered with the NI Measurement & Automation Explorer.
 * \ingroup drivers
 *
 * This class may output one sequence (from a NIsequenceGenerator object) at a time. In addition it provides a convenient
 * interface for (un)synchronised analog and digital input. There are several "modes" of this class:
 *  -# in the <b>initial state</b> the class has a pool of NI-DAQmx tasks which allow for unsynchronised I/O on all
 *     channel registered with the channelManager object. These tasks will be governed internally in the \c singletasks
 *     vector. Theres is a map \c singletask_names which maps from the channel name to the vector item which contains
 *     the task for this channel. You can use the methods methods get_analog(), set_analog(), get_digital(), set_digital()
 *     to access these <b>unsynchronised tasks</b>. Some of the I/O methods of this class (sequence output, synchronised
 *     input) may take some channels out of this pool to use them for synchronised I/O. They can later be returned to this
 *     pool. Initially and after a call of reset() all channels will reside in the unsynchronised pool.
 *  -# When <b>sequence output</b> (i.e. synchronised data output) is currently done the method bind() unregisters the
 *     output channels that are needed for the sequence from the pool of unsynchronised channels. You may still use the
 *     remaining unsynchronised channels for unsynchronised I/O, as described in 1. The sequence channels will be returned
 *     to the pool by calling clear(). (for details see \ref otaskmodel_sec )
 *  -# There is a similar mechanism for <b>synchronised data input</b>. With create_input_task() you may unregister some
 *     of the pooled channels and create a NI-DAQmx task with then which may then be started. The timing of such channels
 *     is also read from \c timing.ini as described in \ref nidrivertiming_sec . But note that you may have more than one
 *     input task at a time, if your hardware device allows this. (for details see \ref itaskmodel_sec )
 * .
 *
 *
 * This class implements tha basic driver interface, defined in the sequencer::driver class:
 *  - reset() will perform a hardware and software reset. So thisbasically reset the object
 *    to the state directly after a call of the constructor (or any other sensefull initial state!)
 *  - bind() will bind the class to a dataset, i.e. a NIsequenceGenerator which holds the data that is
 *    output by this class. It then reserves all channels that are used in the sequence and creates NI-DAQmx tasks
 *    for them. The timing parameters for these tasks are read from the \c timing.ini file. This method calls
 *    set_output_triggering() to read the triggering information from \c timing.ini and deactivate_us_used() to
 *    unregister the channels.
 *  - start() will start the sequence output. You will first have to call bind(), as this makes the data available
 *    the object shall output. This method takes the NI-DAQmx tasks generated by bind() and starts them. It takes
 *    care that the task marked as \c last is called as the last one.
 *  - stop() will stop a sequence output. This can be used at any time after start() to stop the sequence (even
 *    mid-sequence if an error occured)
 *  - is_done() returns \c true if the object does not output a sequence and returns \c false as long as the hardware
 *    is occupied with the output. This method queries all tasks that belong to the sequence and returns true if all
 *    tasks have finished.
 * .
 * This interface can be summarized in this state diagram:\n
 *   \dotfile driver_object.dot
 *
 * To use this class first bind() the driver to a channelManager and a NIsequenceGenerator object. The lather one
 * already has to contain the generated sequence. Then you can start() the sequence and test (is_done())
 * whether the sequence reached its end. The channelManager is already provided in the constructor.
 *
 * \section otaskmodel_sec output task details and state model:
 * When there is a sequence startet, then the class unregisters the ressources of all of those unsynchronised tasks
 * so that the sequence output may be done (DAQmx driver only allows to register a ressource, i.e. a channel/pin
 * to one task at a time). This is done by the method deactivate_us_used() which is called from bind(). During the
 * output only the remaining pins may be used for I/O. After the sequence the initial state will be restored and
 * all unsynchronised tasks will be registered again (this is done by activate_us_used() ).\n
 * Each such task has a state within this class. This state is not the state of the real NI-DAQmx task (to my knowledge
 * there is no driver method to read the tasks state), but represents which methods have already been executed for this
 * task (bind(), start(), stop(), writing data ...). The state diagram for an output task is:\n
 *   \dotfile outputtaskstates.dot
 *
 *
 * \section itaskmodel_sec input tasks
 * How to generate synchronized output was described in the last section. A similar mechanism is implemented
 * for input tasks. The timing is again defined in the above timing.ini file. But there is one big difference:
 * You can have more than one synchronized input tasks at a time.
 *
 * With our hardware (PXI/PXI-6254) we have the problem that this card seems to allow only one analog input channel
 * at a time. So no task is running and we activate the needed task on demand. In synchronised input modeall
 * non-synchronised input channels are deactivated. This is implemented on a per device basis! To allow reading multiple
 * analog samples there are methods start_analog(), stop_analog() and started_read_analog().
 *
 *
 *
 * \section nidrivertiming_sec timing configuration
 * The object reads the timing configuration of the tasks from an INI file. The standard filename is \c timing.ini, but
 * you can change it with set_timing_inifile(). This file only contains few information: There has to be one
 * group for each device. Each group may contain some of these datafields:
 * \code
[device_name] | [device_name/(ao|ai|do|di)] | [device_name/port]
source = <adress string>                     # source of the trigger signal

trigger = <adress string>                    # source of the trigger signal
edge = falling | rising                      # trigger on which edge

last = false | true                          # one of the tasks where last=true will start as the last one

refclk_source = <adress string>              # source of the reference clock
refclk_rate = <rate in Hz>                   # clock rate of the reference clock

export_start_trigger = <signal name>         # export the StartTrigger on the given signal line
export_sampleclock = <signal name>           # export the SampleClock on the given signal line
export_10MHz = <signal name>                 # export the 10MHz timebase clock on the given signal line
export_20MHz = <signal name>                 # export the 20MHz timebase clock on the given signal line

sampleclock_timebase_source = <signal name>  # source of the SampleClock timebase
master_timebase_source = <signal name>       # source of the master timebase
master_timebase_rate = <rate in Hz>          # clock rate of the master timebase
   \endcode
 * There are some possibilities for the \c device_name, as you maybe want to use one configuration for
 * more than one task on one device. So you can use these possibilities as \c device_name s:
 *  -# just the name of the device (e.g. \c PXI1Slot4, \c dev1 ...). Then the data is valid for all tasks on
 *     this device besides there is a more specific definition
 *  -# \c [device_name/ao], \c [device_name/ai], \c [device_name/do], \c [device_name/di] outrank the simple
 *     \c device_name. Properties set in these groups are only valid for analog (ao, ai) or digital (do, di)
 *     input (ai, di) or output (ao, do).
 *  -# \c [device_name/port_name] this is very specific, as this sets the properties for a task with this
 *     specific port name: So if you want to set the timing for output on digital port \c port0 you
 *     will have to use this, e.g. \c [PXI1Slot4/port0]. Analog outputs are grouped by NIsequenceGenerator
 *     so for them you will have to use the complete list of analog outputs, like \c PXI1Slot4/ao0,PXI1Slot4/ao1
 * .
 * With these possibilities you can choose one (relatively specific task to provide the SampleClock, e.g. use
 * \c [dev1/ao], as there is only one analog output task per device. Set the \c last property of this group to
 * \c true, so this channel will be startet as the last one. All the other channels should then use a \c source
 * like \c dev1/ao/SampleClock which specifies the analog output sample clock (see NI-DAQmx and output card
 * documentation for available signals).
 *
 * The last property controls which task will start as the last one, so you can specify one task
 * to generate the SampleClock.
 *
 * \section tasksync_xec task synchronization
 * As mentioned above the different task have to be synchronized over the
 * different devices. In <a href="http://zone.ni.com/devzone/cda/tut/p/id/3615">a NI Developer Zone article</a>
 * they describe how to synchronize multiple devices. This will be implemented in the bind() method (see there, fig.7).
 * First all devices have to share the same SampleClock. Additionally each task will be triggered by a common
 * StartTrigger signal that will be generated by the master task (i.e. the task where \c last=true).\n
 * Note that this class assumes that all your output cards are placed within one PXI chassis, so they can share
 * the same 10MHz PXI timebase (signal \c PXI_Clk10 ) for signal generation. If this is not the case for your
 * system you will have to rewrite this class and change the triggering and timing options which are beeing set
 * in the methods set_output_triggering() for triggering and  for timing.
 *
 *
 * \section nidriver_linkin_sec linking and including
 * To use the NI DAQmx 8.6 driver you need the header file \c NIDAQmx.h On a standard installation of
 * the national instruments driver this can be found in <tt>C:\\Programme\\National Instruments\\NI-DAQ\\DAQmx ANSI C Dev\\include</tt>.
 * So add the path to this file to the include directories. You also need to link against \c libnidaqmx.a which
 * has to be created from the MS VisualC++ library provided by National Instruments. You will find information
 * about how to do this in the NI forum or in the documentation to my diploma thesis and this software (\c use_nidaqmx_with_mingw.pdf):
 *  - <a href="http://forums.ni.com/ni/board/message?board.id=250&message.id=17557">http://forums.ni.com/ni/board/message?board.id=250&message.id=17557</a>
 *  - <a href="http://forums.ni.com/ni/board/message?board.id=250&message.id=10936&view=by_date_ascending&page=2">http://forums.ni.com/ni/board/message?board.id=250&message.id=10936&view=by_date_ascending&page=2</a>
 * .
 * Be aware that this driver may also work with lower NI-DAQmx versions. But you will have to generate a new
 * include library whenever installing a new driver version.
 *
 *
 * \test a test application for sequencer::NIdriver can be found in \link niseqout_test.cpp.
 */
class NIdriver: public driver
{
  protected:

    /** \brief last task to start */
    int lasttask;
    /** \brief are the tasks continuous or not (i.e. not run-once tasks) */
    bool continuoustask;

    /** \brief filename if timing.ini file */
    std::string timing_inifile;

    /** \brief contains all channels and the task number of each channel */
    std::map<std::string, unsigned long> chantask;

    /** \brief a list of all tasks for sequence output */
    std::vector<nid_task> tasks;

    /** \brief a list of all input tasks. This is a map to have unique handle integers even if some of the
     *         tasks are deleted.
     */
    std::map<int, nid_inputtask> inputtasks;

    /** \brief this method returns a new handle for an input task, i.e. an integer that is not currentyl used as a handle
     *         in inputtasks map.
     */
    int get_new_inputtask_handle();

    /** \brief a list of tasks for all channels that may be used for unsynchronised I/O */
    std::vector<nid_ustask> singletasks;

    /** \brief contains all channels and the task number of each channel for unsynchronised I/O */
    std::map<std::string, unsigned long> singletask_names;

    /** \brief source of the output data */
    NIsequenceGenerator* generator;

		/** \brief stop and clear all tasks, empty tasks vector and chantask map */
		void clear_tasks();

		/** \brief stop and clear the un-synchronised tasks. empty and clear \c singletasks and \c singletask_names */
		void clear_us();


		/** \brief gets information about task <i>n</i> */
		inline nid_task& get_task(unsigned long n){
      if (n>=tasks.size()) SEQUENCER_ERRORN(SEQUENCER_ERROR_RANGE_NUM, "CSVsequenceParser::get_task("+inttostr(n)+")");
      return tasks[n];
    };

    /** \brief returns the number of tasks */
		inline unsigned long get_task_count(){
      return tasks.size();
    };

    /** \brief generate a properly formatted device+terminal string, NI tool function! */
    //int32 GetTerminalNameWithDevPrefix(TaskHandle taskHandle, const char terminalName[], char triggerName[]);

    /** \brief set the triggering options of the output tasks, this method also reads some configuration from the ini file
      *        provided as parameter.
      *
      * The parameter should reference a jkiniparser object which contains data that was read from a timing.ini file as
      * described in the documentation of thi \link sequencer::NIdriver "class".
      */
    virtual void set_output_triggering(jkiniparser& ini);

    /** \brief set the triggering options of the specified input task, this method also reads some configuration from
      *        the ini file provided as parameter.
      *
      * The parameter should reference a jkiniparser object which contains data that was read from a timing.ini file as
      * described in the documentation of thi \link sequencer::NIdriver "class".
      */
    virtual void set_input_triggering(jkiniparser& ini, int itask);

    /** \brief deactivates all un-synchronized tasks that are use by the NIsequenceGenerator object provided to bind() */
    void deactivate_us_used();

    /** \brief activates all un-synchronized tasks that are use by the NIsequenceGenerator object provided to bind(). This
    *          will be called after the sequence ended, i.e. by clear() to make the sequence channels available for
    *          unsynchronised output again.
    */
    void activate_us_used();

    /** \brief generate a hardware adress string for the given channel */
    std::string get_hardware_adress(cm_channel channel);

    /** \brief returns true, if an unsynchronised channel with the given name is registered and not used by any input or
      *        output task (i.e. it is not stopped)
      */
    bool uschannel_unused(std::string name);

    /** \brief set the routing information from timing.ini */
    void set_routing();

    /** \brief read timing information from \a ini and there in group \a g. The task \a t will be configured
     *
     * This method partly defines the format of \c timing.ini and is called by set_output_triggering() and
     * set_input_triggering().
     */
    void int_set_triggering(jkiniparser& ini, std::string g, TaskHandle t);

    /** \brief the unsynchronised analog input that is currently running */
    int ai_running;

    /** \brief variable timing: the INI file specifying the variable timing electronis */
    std::string vartiming_inifile;

    /** \brief variable timing: read the variable-timing.ini file */
    void vartiming_read_ini();

    /** \brief variable timing: driver for variable timing hardware */
    varTimingDriver vartiming_driver;

  private:
    /** \brief this is called by the constructor to do initialisation stuff */
    void construct();

  public:
	/** \brief class constructor. This constructor also createas tasks for each channel that may be used
     *         for un-synchronised I/O. The HighResTimer object will be used for internal time measurements
     */
    NIdriver(channelManager* c, HighResTimer* t);
    /** \brief class constructor. This constructor also createas tasks for each channel that may be used
     *         for un-synchronised I/O. This will generate an internal HighResTimer object.
     */
    NIdriver(channelManager* c);

    /** \brief class destructor */
    ~NIdriver();

    /** \brief get the timing.ini file to be read */
    GetMacro(std::string, timing_inifile);
    /** \brief set the timing.ini file to be read */
	virtual void set_timing_inifile(std::string data) {
      timing_inifile=data;
      set_routing();
    };

    GetSetMacro(bool, continuoustask)

    /** \brief get the variable-timing.ini file to be read */
    GetMacro(std::string, vartiming_inifile);
    /** \brief set the variable-timing.ini file to be read */
	virtual void set_vartiming_inifile(std::string data) {
      vartiming_inifile=data;
      vartiming_driver.set_inifile(data);
      vartiming_read_ini();
    };

    /** \brief bind the object to a datasource (NIsequenceGenerator). This method generates output channels
     *         only and binds them to the data from \c gen. It also unregisters all channels used by the
     *         sequence.
     *
     * If \c exetime!=NULL then \c exetime will be used to return the point in time
     * before the function returns control to the caller.
     *
     * It then reserves all channels that are used in the sequence and creates NI-DAQmx tasks
     * for them. The timing parameters for these tasks are read from the \c timing.ini file. This method calls
     * set_output_triggering() to read the triggering information from \c timing.ini and deactivate_us_used() to
     * unregister the channels.
     */
    void bind(NIsequenceGenerator* gen, double* exetime=NULL);

    /** \brief start the sequence: starts all output channels (digital and analog). Make sure that bind() has
     *         been called before.
     *
     * If \c exetime!=NULL then \c exetime will be used to return the point in time
     * shortly after the last output task has been started.
     */
    virtual void start(double* exetime=NULL);

    /** \brief stop the sequence: stops all output channels (digital and analog)
     *
     * If \c exetime!=NULL then \c exetime will be used to return the point in time
     * shortly before function returns control to the caller.
     */
     virtual void stop(double* exetime=NULL);

    /** \brief stop all tasks in the sequence and clear them. This also reactivates the un-synchronised
     *         tasks. If you want to start another (or the same) sequence you will first have to call
     *         bind().
     *
     * If \c exetime!=NULL then \c exetime will be used to return the point in time
     * before the function returns control to the caller.
     */
    virtual void clear(double* exetime=NULL);

    /** \brief returns true if the master task has ended (if any is defined) or when all tasks have ended
     *         (no master task)
     *
     * If \c exetime!=NULL then \c exetime will be used to return the point in time
     * before the function returns control to the caller.
     */
    virtual bool is_done(double* exetime=NULL);

    /** \brief returns the version of NI-DAQmx */
    virtual std::string get_driver_version();

    /** \brief returns some system information about NI-DAQmx and the installed hardware */
    virtual std::string get_systeminfo();

    /** \brief Reads one sample from the given analog input
     *
     * If \c exetime!=NULL then \c exetime will be used to return the point in time
     * shortly before the value is beeing read.
     */
    double read_analog(std::string channel, double* exetime=NULL);

    /** \brief start input on the specified analog channel */
    void start_analog(std::string channel);

    /** \brief stop input on the specified analog channel */
    void stop_analog(std::string channel);

    /** \brief start input on the specified analog channel */
    double started_read_analog(std::string channel, double* exetime=NULL);

    /** \brief Write one sample to the given analog output. The given port may not be in use, e.g. by a sequence.
     *
     * If \c exetime!=NULL then \c exetime will be used to return the point in time
     * shortly before the value is beeing written.
     */
    void write_analog(std::string channel, double value, double* exetime=NULL);

    /** \brief Reads one sample from the given digital input.
     *
     * If \c exetime!=NULL then \c exetime will be used to return the point in time
     * shortly before the value is beeing read.
     */
    bool read_digital(std::string channel, double* exetime=NULL);

    /** \brief Write one sample to the given digital output. The given port may not be in use, e.g. by a sequence.
     *
     * If \c exetime!=NULL then \c exetime will be used to return the point in time
     * shortly before the value is beeing written.
     */
    void write_digital(std::string channel, bool value, double* exetime=NULL);

		/** \brief hardware system reset: this method resets all devices that are used in the current channel manager object
     *
     * If \c exetime!=NULL then \c exetime will be used to return the point in time
     * shortly before the function returns control to the caller.
     *
     * \attention
     *     If this method is called during normal operation all tasks will be lost and have to be recreated.
     *     So this method calls contruct() like a standard constructor!
     */
    virtual void reset(double* exetime=NULL);

    /** \brief creates an input task that reads \a num_samples samples from the given the given channels.
     *
     * The sample clock will be 1/\a sampletime. If \a savetofile \c =true then the data will be written
     * to the specified file after the task finished. This will also clear the task when the file is written.
     * If \a savetofile \c =false the data may be accessed by read_input_task() after the task finished. You will
     * have to clear the task manually. The task will not be started by this method; use start_input_task to
     * do so.
     *
     * The return value is a handle to the task that will be used by the other methods (start_input_task(),
     * read_input_task(), ...).
     *
     * This method will also take the channels needed for this task out of the pool of unsynchronized channels. The timing
     * and triggering information is read from \c timing.ini. The triggering id configured by set_input_triggering() which
     * is called by this method.
     */
    virtual int create_input_task(std::vector<std::string> channels, long num_samples, double sampletime, bool savetofile=false, std::string filename="input.csv");

    /** \brief start the specified input task */
    virtual void start_input_task(int handle, double* exetime=NULL);

    /** \brief stop the specified input task */
    virtual void stop_input_task(int handle, double* exetime=NULL);

    /** \brief clear the specified input task
     *
     * This will free the memory reserved for the task and release the channels into unsynchronized mode.
     * Note that the \a handle will be invalidated! Data read by read_input_task() will still be accessible,
     * as read_input_task() generates a new heap data array for the data and returns a pointer to that (not to
     * an internal data store!).
     */
    virtual void clear_input_task(int handle, double* exetime=NULL);

    /** \brief read the data from the specified input task.
     *
     * This metod returns a pointer to a memory area that contains the data from the task. This method will
     * reserve enough memory to store the data each time you call it, so you will have to free the memory
     * yourself. Data ist stored in this way: The first num_samples items of the double-array contain the data
     * read from the first channel in this task. The next num_samples items contain the data from the second
     * channel ...
     */
    virtual double* read_input_task(int handle);

    /** \brief check whether the input task finished running. */
    virtual bool is_input_task_done(int handle);

    /** \brief check whether a input task for the specified \a handle exists. */
    virtual bool input_task_exists(int handle);

    /** \brief save the data from the channel into the specified file. */
    virtual void save_input_task(int handle, std::string filename);

    /** \brief returns information about an input task. */
    virtual nid_inputtask get_input_task(int handle);

    /** \brief sets the initialvalues of the specified NIsequenceGenerator object to the current values of the
     *         asynchronous port lines
     */
    void save_initialvalues(NIsequenceGenerator* sg);
    /** \brief read the values of all sequence channels at the end of the sequence and save them as current values
     *         of the unsychronised tasks in this object, so when the tasks are reactivated these values will be set.
     */
    void read_initialvalues_from_sequenceend(NIsequenceGenerator* sg);
};


};



#endif // NIDRIVER_H

/*@}*/
