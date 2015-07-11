/*
  Name: nidriver.cpp
  Copyright: (c) 2007
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/

#include "nidriver.h" // class's header file
#include <algorithm>

namespace sequencer {

std::string taskstatetostring(nid_task_state ts){
  switch(ts) {
    case nidCreated: return "created";
    case nidData: return "data written";
    case nidRunning: return "running";
    case nidNoTask: return "no task";
  }
  return "";
}


NIdriver::NIdriver(channelManager* c, HighResTimer* t):
  driver(c, t)
{
  vartiming_inifile="variable-timing.ini";
  vartiming_driver.set_inifile(vartiming_inifile);
  continuoustask=false;
  reset();
}

NIdriver::NIdriver(channelManager* c):
  driver(c)
{
  vartiming_inifile="variable-timing.ini";
  vartiming_driver.set_inifile(vartiming_inifile);
  continuoustask=false;
  reset();
}


NIdriver::~NIdriver() {
  clear_us();
  clear_tasks();
}


void NIdriver::vartiming_read_ini(){
    vartiming_driver.set_inifile(vartiming_inifile);
};


void NIdriver::set_routing(){
  std::string method="NIdriver::set_routing('"+timing_inifile+"')";
  jkiniparser ini;
  try {
     ini.readFile(timing_inifile); // parse timing.ini file
  } catch (jkipError e) {
     SEQUENCER_ERROR(SEQUENCER_ERROR_INI_NUM, e.getMessage(), method.c_str());
  }
  if (ini.groupExists("routes")) {
    std::string exprt=ini.getAsString("routes", "connect", "no");
    if (exprt!="no") {
      std::vector<std::string> vec;
      toStringVector(vec, exprt, ',');
      if (vec.size()>0) {
        if (vec.size()%2==0) {
          for (size_t i=0; i<vec.size(); i+=2) {
            DAQmxErrChk(DAQmxConnectTerms(vec[i].c_str(), vec[i+1].c_str(), DAQmx_Val_DoNotInvertPolarity ), method.c_str());
          }
        }
      }
    }
  }
}


void NIdriver::construct() {
  // initialise object and private data members
  generator=NULL;
  timing_inifile="timing.ini";

  // create tasks for un-synchronised I/O
  singletasks.clear();
  singletask_names.clear();
  if (cmanager->get_channel_count()>0) {
    for (size_t i=0; i<cmanager->get_channel_count(); i++) {
      cm_channel c=(*cmanager)[i];
      nid_ustask task;
      task.name=c.name;
      task.type=c.type;

      // generate hardware adress: when device port and pin are set: combine them. when port is not set combine only device and pin
      task.channel=c.device_name+NISEQGEN_DEVICE_PORT_DIVIDER+c.port_name+NISEQGEN_PORT_PIN_DIVIDER+c.pin_name;
      if (c.port_name=="") task.channel=c.device_name+NISEQGEN_DEVICE_PORT_DIVIDER+c.pin_name;

      /* NI-DAQmx: create and configure task
           analog outputs:     create and start a task without timing information
           digital outputs:    create and start a task without timing information
           digital input:      create and start a task without timing information
           single-ended analog input:
                               create and start a task without timing information.
                               the input configuration is "RSE" which means that the channel
                               measures voltage against AI GND signal (and not AI SENSE!)
           differential-ended analog input:
                               create and start a task without timing information.
                               the input configuration is "Diff" which means that the channel
                               stands for a pair of channels between which the voltage difference
                               is measure (refer to the card manual for details).
       */
      float64     fdata=0;
      uInt8       data=0;
      switch(task.type) {
        case cmAnalogOutput:
          DAQmxErrChk(DAQmxCreateTask("", &(task.task)), ("NIdriver::construct(), channel="+task.channel).c_str());
          DAQmxErrChk(DAQmxCreateAOVoltageChan(task.task, task.channel.c_str(), "", (*cmanager)[i].min, (*cmanager)[i].max, DAQmx_Val_Volts, NULL), ("NIdriver::construct(), channel="+task.channel).c_str());
          DAQmxErrChk(DAQmxStartTask(task.task), ("NIdriver::construct(), channel="+task.channel).c_str());
          task.value=0;
          DAQmxErrChk(DAQmxWriteAnalogF64(task.task,1,1,10.0,DAQmx_Val_GroupByChannel,&fdata,NULL,NULL), ("NIdriver::construct(), channel="+task.channel).c_str());
          break;
        case cmDigitalOutput:
          DAQmxErrChk(DAQmxCreateTask("", &(task.task)), ("NIdriver::construct(), channel="+task.channel).c_str());
          DAQmxErrChk(DAQmxCreateDOChan(task.task, task.channel.c_str(), "", DAQmx_Val_ChanForAllLines), ("NIdriver::construct(), channel="+task.channel).c_str());
          DAQmxErrChk(DAQmxStartTask(task.task), ("NIdriver::construct(), channel="+task.channel).c_str());
          task.value=0;
          DAQmxErrChk(DAQmxWriteDigitalLines(task.task,1,1,1.0,DAQmx_Val_GroupByChannel,&data,NULL,NULL), ("NIdriver::construct(), channel="+task.channel).c_str());
          break;
        case cmDigitalInput:
          DAQmxErrChk(DAQmxCreateTask("", &(task.task)), ("NIdriver::construct(), channel="+task.channel).c_str());
          DAQmxErrChk(DAQmxCreateDIChan(task.task, task.channel.c_str(), "", DAQmx_Val_ChanForAllLines), ("NIdriver::construct(), channel="+task.channel).c_str());
          DAQmxErrChk(DAQmxStartTask(task.task), ("NIdriver::construct(), channel="+task.channel).c_str());
          break;
        case cmAnalogInputSE:
          DAQmxErrChk(DAQmxCreateTask("", &(task.task)), ("NIdriver::construct(), channel="+task.channel).c_str());
          DAQmxErrChk(DAQmxCreateAIVoltageChan(task.task, task.channel.c_str(), "", DAQmx_Val_RSE, (*cmanager)[i].min, (*cmanager)[i].max, DAQmx_Val_Volts, NULL), ("NIdriver::construct(), channel="+task.channel).c_str());
          //DAQmxErrChk(DAQmxStartTask(task.task), ("NIdriver::construct(), channel="+task.channel).c_str());
          // We do not start analog input tasks
          break;
        case cmAnalogInputDE:
          DAQmxErrChk(DAQmxCreateTask("", &(task.task)), ("NIdriver::construct(), channel="+task.channel).c_str());
          DAQmxErrChk(DAQmxCreateAIVoltageChan(task.task, task.channel.c_str(), "", DAQmx_Val_Diff, (*cmanager)[i].min, (*cmanager)[i].max, DAQmx_Val_Volts, NULL), ("NIdriver::construct(), channel="+task.channel).c_str());
          //DAQmxErrChk(DAQmxStartTask(task.task), ("NIdriver::construct(), channel="+task.channel).c_str());
          // We do not start analog input tasks
          break;
      }


      task.state=nidUstUsable;
      singletask_names[c.name]=singletasks.size();
      singletasks.push_back(task);
    }
  }
}

void NIdriver::deactivate_us_used(){
  /* go through all channels in singletasks and check with sequenceGenerator \c generator whether
     they are marked as used. If they are: stop the task. Ignore the other tasks
   */
  if (generator==NULL) SEQUENCER_ERRORN(SEQUENCER_ERROR_NOSEQUENCE_NUM, "NIdriver::deactivate_us_used()");
  if (singletasks.size()>0) {
    for (size_t i=0; i<singletasks.size(); i++) {
      //std::cout<<"   "<<singletasks[i].name;
      if (generator->is_used(singletasks[i].name)) {
        //std::cout<<" u ";
        if (singletasks[i].state==nidUstUsable) {
          //std::cout<<" r ";
          DAQmxErrChk(DAQmxTaskControl(singletasks[i].task, DAQmx_Val_Task_Unreserve ), "NIdriver::deactivate_us_used("+singletasks[i].name+")");
          singletasks[i].state=nidUstStopped;
        }
      }
      //std::cout<<"!"<<std::endl;
    }
  }
}

void NIdriver::activate_us_used(){
  /* go through all channels in singletasks and check with channelManager \c cmanager whether
     they are marked as used or blocked. If they are: start the task. Ignore the other tasks
   */
  if (singletasks.size()>0) {
    for (size_t i=0; i<singletasks.size(); i++) {
      //std::cout<<singletasks[i].name;
      if (singletasks[i].state==nidUstStopped && generator->is_used(singletasks[i].name)) {
        DAQmxErrChk(DAQmxTaskControl(singletasks[i].task, DAQmx_Val_Task_Reserve ), "NIdriver::activate_us_used("+singletasks[i].name+")");
        DAQmxErrChk(DAQmxTaskControl(singletasks[i].task, DAQmx_Val_Task_Verify ), "NIdriver::activate_us_used("+singletasks[i].name+")");
        DAQmxErrChk(DAQmxTaskControl(singletasks[i].task, DAQmx_Val_Task_Commit ), "NIdriver::activate_us_used("+singletasks[i].name+")");
        DAQmxErrChk(DAQmxTaskControl(singletasks[i].task, DAQmx_Val_Task_Start ), "NIdriver::activate_us_used("+singletasks[i].name+")");
        //DAQmxErrChk(DAQmxStartTask(singletasks[i].task), "NIdriver::activate_us_used("+singletasks[i].name+")");
        singletasks[i].state=nidUstUsable;
        if ((*cmanager)[singletasks[i].name].type==cmDigitalOutput) {
          write_digital(singletasks[i].name, singletasks[i].value>0);
        } else {
          write_analog(singletasks[i].name, singletasks[i].value);
        }
        //std::cout<<" startet";
      }
      //std::cout<<std::endl;
    }
  }
}

void NIdriver::clear_us(){
  if (singletasks.size()>0) {
    for (size_t i=0; i<singletasks.size(); i++) {
      if (singletasks[i].state==nidUstUsable) {
        DAQmxErrChk(DAQmxStopTask(singletasks[i].task), "NIdriver::clear_us()");
        //DAQmxStopTask(singletasks[i].task);
      }
      DAQmxErrChk(DAQmxClearTask(singletasks[i].task), "NIdriver::clear_us()");
      //DAQmxClearTask(singletasks[i].task);
    }
  }
  singletasks.clear();
  singletask_names.clear();
}


void NIdriver::bind(NIsequenceGenerator* gen, double* exetime) {
  generator=gen;
  lasttask=-1;
  clear_tasks();
  double samplerate=1.0/(gen->get_sample_timestep()*1.0e-6);
  int64 samplesperchannel=gen->get_array_size();

  //std::cout<<1<<std::endl;

  if (gen->get_use_variable_timing() && vartiming_driver.get_available()) {
      vartiming_read_ini();
      vartiming_driver.reset();
  }

  //std::cout<<2<<std::endl;


  // parse timing.ini file
  jkiniparser ini;
  try {
     ini.readFile(timing_inifile);
  } catch (jkipError e) {
     SEQUENCER_ERROR(SEQUENCER_ERROR_INI_NUM, e.getMessage(), "NIdriver::bind()");
  }

  //std::cout<<3<<std::endl;

  // stop all un-synchronised tasks that represent channels that are used in the sequence. These
  // used channels are marked as used in the channelManager object by a high-level sequence generator!
  deactivate_us_used();

  // go through all output tasks and register them
  // now the task number equals the array number in the NIsequenceGenerator object gen!
  if (gen->get_array_count()<=0) SEQUENCER_ERRORN(SEQUENCER_ERROR_NOARRYS_NUM, "NIdriver::bind()");
  for (unsigned long i=0; i<gen->get_array_count(); i++) {
    nid_task t;
    t.state=nidNoTask;
    t.min=0;
    t.max=0;
    t.channels=gen->get_nth_adress_string(i);
    t.channum=0;
    t.device="";
    tasks.push_back(t);
    //std::cout<<t.channels<<", ";
  }

  //std::cout<<4<<std::endl;


  /* now go through all single channels in the NIsequenceGenerator object. The channel->task
     assignment can be derived from the arraynumber of the channel. Additionally now for each
     analog input and digital input channel one task will be generated.
  */
  if (gen->get_channel_count()<=0)  SEQUENCER_ERRORN(SEQUENCER_ERROR_NOCHAN_NUM, "NIdriver::bind()");
  for (unsigned long i=0; i<gen->get_channel_count(); i++) {
    cm_channel c=cmanager->get(gen->get_name(i));
    if (c.type==cmAnalogOutput) {
      int j=gen->get_array_num(c.name); // task number
      tasks[j].channum++;
      // if this is the first channel in this task: set min and max values and type,
      // if it is not: check whether min, max and type are consistent
      if (tasks[j].channum==1) {
        tasks[j].type=c.type;
        tasks[j].min=c.min;
        tasks[j].max=c.max;
        tasks[j].device=c.device_name;
      } else {
        if (c.min!=tasks[j].min) SEQUENCER_ERROR(SEQUENCER_ERROR_MINNOTCONSIST_NUM, format(get_errormessage(SEQUENCER_ERROR_MINNOTCONSIST_NUM), c.name.c_str(), tasks[j].channels.c_str()), "NIdriver::bind()");
        if (c.max!=tasks[j].max) SEQUENCER_ERROR(SEQUENCER_ERROR_MAXNOTCONSIST_NUM, format(get_errormessage(SEQUENCER_ERROR_MAXNOTCONSIST_NUM), c.name.c_str(), tasks[j].channels.c_str()), "NIdriver::bind()");
        if (c.type!=tasks[j].type) SEQUENCER_ERROR(SEQUENCER_ERROR_TYPENOTCONSIST_NUM, format(get_errormessage(SEQUENCER_ERROR_TYPENOTCONSIST_NUM), c.name.c_str(), tasks[j].channels.c_str()), "NIdriver::bind()");
        if (c.device_name!=tasks[j].device) SEQUENCER_ERROR(SEQUENCER_ERROR_DEVNOTCONSIST_NUM, format(get_errormessage(SEQUENCER_ERROR_DEVNOTCONSIST_NUM), c.name.c_str(), tasks[j].channels.c_str()), "NIdriver::bind()");
      }
      chantask[c.name]=j;
    }
    if (c.type==cmDigitalOutput) {
      int j=gen->get_array_num(c.name); // task number
      tasks[j].channum++;
      // if this is the first channel in this task: set the type,
      // if it is not: check whether the types are consistent
      if (tasks[j].channum==1) {
        tasks[j].type=c.type;
        tasks[j].device=c.device_name;
      } else {
        if (c.type!=tasks[j].type) SEQUENCER_ERROR(SEQUENCER_ERROR_TYPENOTCONSIST_NUM, format(get_errormessage(SEQUENCER_ERROR_TYPENOTCONSIST_NUM), c.name.c_str(), tasks[j].channels.c_str()), "NIdriver::bind()");
        if (c.device_name!=tasks[j].device) SEQUENCER_ERROR(SEQUENCER_ERROR_DEVNOTCONSIST_NUM, format(get_errormessage(SEQUENCER_ERROR_DEVNOTCONSIST_NUM), c.name.c_str(), tasks[j].channels.c_str()), "NIdriver::bind()");
      }
      chantask[c.name]=j;
    }
  }

  //std::cout<<5<<std::endl;


  /* now go through all tasks, generate them, set the timing and add the channels to the task
   * this only generates tasks for analog and digital output!
   */
  if (debug && log!=NULL) {
    log->inc_indent();
    log->log_linebreak();
  }
  for (size_t i=0; i<tasks.size(); i++) {
    if (debug && log!=NULL) {
      log->log_text(format("creating task %2d:\n  channels: %s\n", i, tasks[i].channels.c_str()));

    }
    DAQmxErrChk(DAQmxCreateTask("", &(tasks[i].task)), "NIdriver::bind()");
    tasks[i].state=nidCreated;

    if (tasks[i].type==cmAnalogOutput) {
      // create channels
      DAQmxErrChk(DAQmxCreateAOVoltageChan(tasks[i].task,tasks[i].channels.c_str(),"",tasks[i].min,tasks[i].max,DAQmx_Val_Volts,NULL), "NIdriver::bind()");
      // read timing from timing.ini file
      std::string g=tasks[i].channels;
      if (!ini.groupExists(g)) {
        g=tasks[i].device+"/ao";
        if (!ini.groupExists(g)) {
          g=tasks[i].device;
          if (!ini.groupExists(g)) {
            SEQUENCER_ERROR(SEQUENCER_ERROR_DEVNOTINI_NUM, format(get_errormessage(SEQUENCER_ERROR_DEVNOTINI_NUM), g.c_str(), timing_inifile.c_str()), "NIdriver::bind()");
          }
        }
      }
      std::string source=ini.getAsString(g, "source", "");
      std::string edge=ini.getAsString(g, "edge", "rising");
      int32 activeEdge=DAQmx_Val_Rising;
      if (edge=="falling") activeEdge=DAQmx_Val_Falling;
      if (ini.getAsBool(g, "last", false)) lasttask=i;

      // set timing configuration
      DAQmxErrChk(DAQmxCfgSampClkTiming(tasks[i].task,source.c_str(),samplerate,activeEdge,(continuoustask)?DAQmx_Val_ContSamps:DAQmx_Val_FiniteSamps,samplesperchannel), "NIdriver::bind()");
      //std::cout<<"  - <<std::endl;
      if (debug && log!=NULL) { log->log_text("  timing: source='"+source+"'  samplerate="+floattostr(samplerate)+"\n"); }

      // write data array
      int32 pointsWritten;
      //std::cout<<"  - bind "<<tasks[i].channels<<" "<<samplesperchannel<<" double samples"<<std::endl;
      if (gen->get_nth_dataformat(i)==sgDouble) {
        if (debug && log!=NULL) { log->log_text("  bind: "+inttostr(samplesperchannel)+" double samples/channel\n"); }
        DAQmxErrChk(DAQmxWriteAnalogF64(tasks[i].task, samplesperchannel, 0, 10, DAQmx_Val_GroupByScanNumber, (float64*)gen->get_nth_array(i),&pointsWritten,NULL), "NIdriver::bind()");
      } else {
        if (debug && log!=NULL) { log->log_text("  bind: "+inttostr(samplesperchannel)+" int16 samples/channel\n"); }
        DAQmxErrChk(DAQmxWriteBinaryI16(tasks[i].task, samplesperchannel, 0, 10, DAQmx_Val_GroupByScanNumber, (int16*)gen->get_nth_array(i),&pointsWritten,NULL), "NIdriver::bind()");
      }
      tasks[i].state=nidData;
    }
    if (tasks[i].type==cmDigitalOutput) {
      // create channels
      DAQmxErrChk(DAQmxCreateDOChan(tasks[i].task,tasks[i].channels.c_str(),"",DAQmx_Val_ChanForAllLines), "NIdriver::bind()");

      // read timing from timing.ini file
      std::string g=tasks[i].channels;
      if (!ini.groupExists(g)) {
        g=tasks[i].device+"/do";
        if (!ini.groupExists(g)) {
          g=tasks[i].device;
          if (!ini.groupExists(g)) {
            SEQUENCER_ERROR(SEQUENCER_ERROR_DEVNOTINI_NUM, format(get_errormessage(SEQUENCER_ERROR_DEVNOTINI_NUM), g.c_str(), timing_inifile.c_str()), "NIdriver::bind()");
          }
        }
      }
      std::string source=ini.getAsString(g, "source", "");
      std::string edge=ini.getAsString(g, "edge", "rising");
      int32 activeEdge=DAQmx_Val_Rising;
      if (edge=="falling") activeEdge=DAQmx_Val_Falling;
      if (ini.getAsBool(g, "last", false)) lasttask=i;

      // set timing configuration
      DAQmxErrChk(DAQmxCfgSampClkTiming(tasks[i].task,source.c_str(),samplerate,activeEdge,(continuoustask)?DAQmx_Val_ContSamps:DAQmx_Val_FiniteSamps,samplesperchannel), "NIdriver::bind()");
      //std::cout<<"  - timing: source='"<<source<<"'  samplerate="<<samplerate<<std::endl;
      if (debug && log!=NULL) { log->log_text("  timing: source='"+source+"'  samplerate="+floattostr(samplerate)+"\n"); }

      // write data array
      int32 pointsWritten;
      switch(gen->get_nth_dataformat(i)) {
        case sgUInt8:
          if (debug && log!=NULL) { log->log_text("  bind: "+inttostr(samplesperchannel)+" uint8 samples/channel\n"); }
          DAQmxErrChk(DAQmxWriteDigitalU8(tasks[i].task, samplesperchannel, 0, 10, DAQmx_Val_GroupByScanNumber, (uInt8*)gen->get_nth_array(i),&pointsWritten,NULL), "NIdriver::bind()");
          //std::cout<<"  - bind "<<tasks[i].channels<<" "<<samplesperchannel<<" 8 bit samples     pointswritten="<<pointsWritten<<std::endl;
          break;
        case sgUInt16:
          if (debug && log!=NULL) { log->log_text("  bind: "+inttostr(samplesperchannel)+" uint16 samples/channel\n"); }
          DAQmxErrChk(DAQmxWriteDigitalU16(tasks[i].task, samplesperchannel, 0, 10, DAQmx_Val_GroupByScanNumber, (uInt16*)gen->get_nth_array(i),&pointsWritten,NULL), "NIdriver::bind()");
          //std::cout<<"  - bind "<<tasks[i].channels<<" "<<samplesperchannel<<" 16 bit samples     pointswritten="<<pointsWritten<<std::endl;
          break;
        case sgUInt32:
          if (debug && log!=NULL) { log->log_text("  bind: "+inttostr(samplesperchannel)+" uint32 samples/channel\n"); }
          DAQmxErrChk(DAQmxWriteDigitalU32(tasks[i].task, samplesperchannel, 0, 10, DAQmx_Val_GroupByScanNumber, (uInt32*)gen->get_nth_array(i),&pointsWritten,NULL), "NIdriver::bind()");
          //std::cout<<"  - bind "<<tasks[i].channels<<" "<<samplesperchannel<<" 32 bit samples     pointswritten="<<pointsWritten<<std::endl;
          break;
        default:
          SEQUENCER_ERROR(SEQUENCER_ERROR_WRONGDIGDATAFORMAT_NUM, format(get_errormessage(SEQUENCER_ERROR_WRONGDIGDATAFORMAT_NUM), tasks[i].channels.c_str()), "NIdriver::bind()");
          break;
      }
      tasks[i].state=nidData;
    }
  }
  if (debug && log!=NULL) {
    log->dec_indent();
    log->log_linebreak();
  }

  // set trigger options, timebases ...
  set_output_triggering(ini);
  if (exetime!=NULL) *exetime=timer->get_time();
}

void NIdriver::set_output_triggering(jkiniparser& ini) {
	//char	    str1[256],str2[256],trigName[256];

  //std::cout<<"lasttask="<<lasttask<<std::endl;

  // set the routing information from timing.ini file
  set_routing();

  // now find the task that is to be started last in lasttask. If this is -1 then set it to 0,
  // i.e. the first task will be started as the last one.
  if (lasttask==-1) lasttask=0;


  // now we have to add the start trigger and other trigger settings to all channels except lasttask
  for (size_t i=0; i<tasks.size(); i++) {
    //find the group in the timing.ini file to read from
    std::string g=tasks[i].channels;
    if (!ini.groupExists(g)) {
      g=tasks[i].device+"/ao";
      if (tasks[i].type==cmDigitalOutput) g=tasks[i].device+"/do";
      if (!ini.groupExists(g)) {
        g=tasks[i].device;
        if (!ini.groupExists(g)) {
          SEQUENCER_ERROR(SEQUENCER_ERROR_DEVNOTINI_NUM, format(get_errormessage(SEQUENCER_ERROR_DEVNOTINI_NUM), g.c_str(), timing_inifile.c_str()), "NIdriver::bind()");
        }
      }
    }

    int_set_triggering(ini, g, tasks[i].task);
  }
}

/*int32 NIdriver::GetTerminalNameWithDevPrefix(TaskHandle taskHandle, const char terminalName[], char triggerName[])
{
	char	device[256];
	int32	productCategory;
	uInt32	numDevices,i=1;

	DAQmxErrChk (DAQmxGetTaskNumDevices(taskHandle,&numDevices), format("NIdriver::GetTerminalNameWithDevPrefix(task , %s, %s)", terminalName, triggerName));
	while( i<=numDevices ) {
		DAQmxErrChk (DAQmxGetNthTaskDevice(taskHandle,i++,device,256), format("NIdriver::GetTerminalNameWithDevPrefix(task , %s, %s)", terminalName, triggerName));
		DAQmxErrChk (DAQmxGetDevProductCategory(device,&productCategory), format("NIdriver::GetTerminalNameWithDevPrefix(task , %s, %s)", terminalName, triggerName));
		if( productCategory!=DAQmx_Val_CSeriesModule && productCategory!=DAQmx_Val_SCXIModule )
			*triggerName++ = '/';
			strcat(strcat(strcpy(triggerName,device),"/"),terminalName);
			break;
	}
  // NEW
  //return 0
}*/


void NIdriver::clear_tasks() {
  if (tasks.size()>0) for (size_t i=0; i<tasks.size(); i++) {
    if (tasks[i].state!=nidNoTask) {
      if (tasks[i].state==nidRunning) DAQmxErrChk(DAQmxStopTask(tasks[i].task), "NIdriver::clear_tasks()");
      DAQmxErrChk(DAQmxClearTask(tasks[i].task), "NIdriver::clear_tasks()");
    }
  }
  tasks.clear();
  chantask.clear();
}


void NIdriver::stop(double* exetime) {
  if (tasks.size()>0) for (size_t i=0; i<tasks.size(); i++) {
    if ((tasks[i].type==cmAnalogOutput)||(tasks[i].type==cmDigitalOutput)) {
      DAQmxErrChk(DAQmxStopTask(tasks[i].task), "NIdriver::stop()");
      tasks[i].state=nidData;
    }
  }
  if (exetime!=NULL) *exetime=timer->get_time();
  if (generator->get_use_variable_timing() && vartiming_driver.get_available()) {
      vartiming_read_ini();
      vartiming_driver.reset();
  }
}

void NIdriver::clear(double* exetime) {
  stop();
  if (tasks.size()>0) for (size_t i=0; i<tasks.size(); i++) {
    if ((tasks[i].type==cmAnalogOutput)||(tasks[i].type==cmDigitalOutput)) {
      DAQmxErrChk(DAQmxClearTask(tasks[i].task), "NIdriver::stop()");
      tasks[i].state=nidData;
    }
  }
  activate_us_used();
  lasttask=-1;
  clear_tasks();
  if (exetime!=NULL) *exetime=timer->get_time();
  if (generator->get_use_variable_timing() && vartiming_driver.get_available()) {
      vartiming_read_ini();
      vartiming_driver.reset();
  }
}



void NIdriver::start(double* exetime) {
  if (tasks.size()>0) for (size_t i=0; i<tasks.size(); i++) {
    if (i!=lasttask)
      if ((tasks[i].type==cmAnalogOutput)||(tasks[i].type==cmDigitalOutput)) {
        if (exetime!=NULL) *exetime=timer->get_time();
        DAQmxErrChk(DAQmxStartTask(tasks[i].task), "NIdriver::start()");
        //std::cout<<"  - startet "<<tasks[i].channels<<std::endl;
        tasks[i].state=nidRunning;
      }
  }
  //std::cout<<"  - lasttask = "<<lasttask<<"   task_count = "<<tasks.size()<<std::endl;
  if ((lasttask>=0)&&(lasttask<tasks.size())) {
    if (exetime!=NULL) *exetime=timer->get_time();
    DAQmxErrChk(DAQmxStartTask(tasks[lasttask].task), "NIdriver::start()");
    //std::cout<<"  - startet "<<tasks[lasttask].channels<<std::endl;
    tasks[lasttask].state=nidRunning;
  }

  if (generator->get_use_variable_timing()) {
      if (vartiming_driver.get_available()) {
          vartiming_read_ini();
          vartiming_driver.start();
      } else {
          SEQUENCER_ERRORN(SEQUENCER_ERROR_NOVARTIMING_NUM, "NIdriver::start()");
      }
  }
}

bool NIdriver::is_done(double* exetime) {
  if ((lasttask>=0)&&(lasttask<tasks.size())) {
    if ((tasks[lasttask].type==cmAnalogOutput)||(tasks[lasttask].type==cmDigitalOutput)) {
      bool32 done;
      DAQmxErrChk(DAQmxIsTaskDone(tasks[lasttask].task, &done), "NIdriver::is_done(), lasttask="+inttostr(lasttask));
      bool isd=done;
      //if (isd) std::cout<<tasks[i].channels<<"     ";
      if (exetime!=NULL) *exetime=timer->get_time();
      if (!(isd)) return false; else return true;
    }
  }
  if (tasks.size()>0) for (size_t i=0; i<tasks.size(); i++) {
    if ((tasks[i].type==cmAnalogOutput)||(tasks[i].type==cmDigitalOutput)) {
      bool32 done;
      DAQmxErrChk(DAQmxIsTaskDone(tasks[i].task, &done), "NIdriver::is_done()");
      bool isd=done;
      //if (isd) std::cout<<tasks[i].channels<<"     ";
      if (exetime!=NULL) *exetime=timer->get_time();
      if (!(isd)) return false;
    }
  }
  //std::cout<<std::endl;
  if (generator!=NULL) if (generator->get_use_variable_timing()) {
      vartiming_read_ini();
      //std::cout<<"\n\n === variable_timing_done? "<<booltostr(vartiming_driver.is_done())<<" ===\n"<<std::endl;
  }

  if (exetime!=NULL) *exetime=timer->get_time();
  return true;
}

std::string NIdriver::get_driver_version() {
  uInt32 major, minor;
  DAQmxErrChk(DAQmxGetSysNIDAQMajorVersion(&major), "NIdriver::get_nidaqmx_version()");
  DAQmxErrChk(DAQmxGetSysNIDAQMinorVersion(&minor), "NIdriver::get_nidaqmx_version()");
  std::string ret="NI-DAQmx ";
  ret+=uinttostr(major)+"."+uinttostr(minor);
  return ret;
};

std::string NIdriver::get_systeminfo() {
  //uInt32 major, minor;
  char buffer[8192];
  DAQmxErrChk(DAQmxGetSysDevNames(buffer, 8192), "NIdriver::get_nidaqmx_systeminfo()");
  std::string ret="";
  ret+=get_driver_version()+"\n\n";
  ret+="installed devices:\n";
  ret+="~~~~~~~~~~~~~~~~~~\n";
  ret+=format("  %s", buffer);
  ret+="\n\n";

  // now convert the devices list (separated by ',') into a vector
  std::vector<std::string> devices;
  toStringVector(devices, buffer, ',');
  if (devices.size()>0) for (size_t i=0; i<devices.size(); i++) {
    int32 data;
    uInt32 data1;
    DAQmxErrChk(DAQmxGetDevProductCategory(strstrip(devices[i]).c_str(), &data), "NIdriver::get_nidaqmx_systeminfo()");
    ret+=strstrip(devices[i])+":\n";
    ret+="   category:   ";
    switch(data) {
      case DAQmx_Val_MSeriesDAQ: ret+="M Series DAQ"; break;
      case DAQmx_Val_ESeriesDAQ: ret+="E Series DAQ"; break;
      case DAQmx_Val_SSeriesDAQ: ret+="S Series DAQ"; break;
      case DAQmx_Val_BSeriesDAQ: ret+="B Series DAQ"; break;
      case DAQmx_Val_SCSeriesDAQ: ret+="SC Series DAQ"; break;
      case DAQmx_Val_USBDAQ: ret+="USB DAQ"; break;
      case DAQmx_Val_AOSeries: ret+="AO Series"; break;
      case DAQmx_Val_DigitalIO: ret+="Digital I/O"; break;
      case DAQmx_Val_TIOSeries: ret+="TIO Series"; break;
      case DAQmx_Val_DynamicSignalAcquisition : ret+="Dynamic Signal Acquisition"; break;
      case DAQmx_Val_Switches : ret+="Switches"; break;
      case DAQmx_Val_CompactDAQChassis : ret+="CompactDAQ chassis"; break;
      case DAQmx_Val_CSeriesModule : ret+="C Series I/O module"; break;
      case DAQmx_Val_SCXIModule: ret+="SCXI module"; break;
      case DAQmx_Val_SCCConnectorBlock : ret+="SCC Connector Block"; break;
      case DAQmx_Val_SCCModule : ret+="SCC Module"; break;
      case DAQmx_Val_Unknown: ret+="unknown category"; break;
    }
    ret+="\n";
    DAQmxErrChk(DAQmxGetDevProductType(strstrip(devices[i]).c_str(), buffer, 8192), "NIdriver::get_nidaqmx_systeminfo()");
    ret+=format("   type:       %s\n", buffer);
    DAQmxErrChk(DAQmxGetDevSerialNum(strstrip(devices[i]).c_str(), &data1), "NIdriver::get_nidaqmx_systeminfo()");
    ret+=format("   serial num: %ld\n", data1);
    DAQmxErrChk(DAQmxGetDevAIPhysicalChans(strstrip(devices[i]).c_str(), buffer, 8192), "NIdriver::get_nidaqmx_systeminfo()");
    ret+=format("   ai lines:   %s\n", buffer);
    DAQmxErrChk(DAQmxGetDevAOPhysicalChans(strstrip(devices[i]).c_str(), buffer, 8192), "NIdriver::get_nidaqmx_systeminfo()");
    ret+=format("   ao lines:   %s\n", buffer);
    DAQmxErrChk(DAQmxGetDevDILines(strstrip(devices[i]).c_str(), buffer, 8192), "NIdriver::get_nidaqmx_systeminfo()");
    ret+=format("   di lines:   %s\n", buffer);
    DAQmxErrChk(DAQmxGetDevDIPorts(strstrip(devices[i]).c_str(), buffer, 8192), "NIdriver::get_nidaqmx_systeminfo()");
    ret+=format("   di ports:   %s\n", buffer);
    DAQmxErrChk(DAQmxGetDevDOLines(strstrip(devices[i]).c_str(), buffer, 8192), "NIdriver::get_nidaqmx_systeminfo()");
    ret+=format("   do lines:   %s\n", buffer);
    DAQmxErrChk(DAQmxGetDevDOPorts(strstrip(devices[i]).c_str(), buffer, 8192), "NIdriver::get_nidaqmx_systeminfo()");
    ret+=format("   do ports:   %s\n", buffer);

    ret+="\n";
  }

  return ret;
};


double NIdriver::read_analog(std::string channel, double* exetime){
  if (singletask_names.find(channel)==singletask_names.end()) SEQUENCER_ERRORN(SEQUENCER_ERROR_CNOTEXIST_NUM, "NIdriver::read_analog("+channel+")");
  nid_ustask t=singletasks[singletask_names[channel]];
  if ((t.type!=cmAnalogInputSE)&&(t.type!=cmAnalogInputDE)) SEQUENCER_ERRORN(SEQUENCER_ERROR_NOTANALOG_NUM, "NIdriver::read_analog("+channel+")");
  if (t.state==nidUstStopped) SEQUENCER_ERRORN(SEQUENCER_ERROR_USTNOTRUNNING_NUM, "NIdriver::read_analog("+channel+")");
  float64     data;
  int32       pointsRead;
  DAQmxErrChk(DAQmxStartTask(t.task), "NIdriver::read_analog("+channel+")");
  if (exetime!=NULL) *exetime=timer->get_time();
  DAQmxErrChk(DAQmxReadAnalogF64(t.task,1,1,DAQmx_Val_GroupByChannel,&data,1,&pointsRead,NULL), "NIdriver::read_analog("+channel+")");
  DAQmxErrChk(DAQmxStopTask(t.task), "NIdriver::read_analog("+channel+")");
  if (pointsRead<=0) SEQUENCER_ERRORN(SEQUENCER_ERROR_COULDNOTREAD_NUM, "NIdriver::read_analog("+channel+")");
  return cmanager->invscale_value(channel, data);
};

double NIdriver::started_read_analog(std::string channel, double* exetime){
  if (singletask_names.find(channel)==singletask_names.end()) SEQUENCER_ERRORN(SEQUENCER_ERROR_CNOTEXIST_NUM, "NIdriver::started_read_analog("+channel+")");
  nid_ustask t=singletasks[singletask_names[channel]];
  if ((t.type!=cmAnalogInputSE)&&(t.type!=cmAnalogInputDE)) SEQUENCER_ERRORN(SEQUENCER_ERROR_NOTANALOG_NUM, "NIdriver::started_read_analog("+channel+")");
  if (t.state==nidUstStopped) SEQUENCER_ERRORN(SEQUENCER_ERROR_USTNOTRUNNING_NUM, "NIdriver::started_read_analog("+channel+")");
  float64     data;
  int32       pointsRead;
  if (exetime!=NULL) *exetime=timer->get_time();
  DAQmxErrChk(DAQmxReadAnalogF64(t.task,1,1,DAQmx_Val_GroupByChannel,&data,1,&pointsRead,NULL), "NIdriver::started_read_analog("+channel+")");
  if (pointsRead<=0) SEQUENCER_ERRORN(SEQUENCER_ERROR_COULDNOTREAD_NUM, "NIdriver::started_read_analog("+channel+")");
  return cmanager->invscale_value(channel, data);
};

void NIdriver::start_analog(std::string channel){
  if (singletask_names.find(channel)==singletask_names.end()) SEQUENCER_ERRORN(SEQUENCER_ERROR_CNOTEXIST_NUM, "NIdriver::start_analog("+channel+")");
  nid_ustask t=singletasks[singletask_names[channel]];
  if ((t.type!=cmAnalogInputSE)&&(t.type!=cmAnalogInputDE)) SEQUENCER_ERRORN(SEQUENCER_ERROR_NOTANALOG_NUM, "NIdriver::start_analog("+channel+")");
  if (t.state==nidUstStopped) SEQUENCER_ERRORN(SEQUENCER_ERROR_USTNOTRUNNING_NUM, "NIdriver::start_analog("+channel+")");
  DAQmxErrChk(DAQmxStartTask(t.task), "NIdriver::start_analog("+channel+")");
};

void NIdriver::stop_analog(std::string channel){
  if (singletask_names.find(channel)==singletask_names.end()) SEQUENCER_ERRORN(SEQUENCER_ERROR_CNOTEXIST_NUM, "NIdriver::stop_analog("+channel+")");
  nid_ustask t=singletasks[singletask_names[channel]];
  if ((t.type!=cmAnalogInputSE)&&(t.type!=cmAnalogInputDE)) SEQUENCER_ERRORN(SEQUENCER_ERROR_NOTANALOG_NUM, "NIdriver::stop_analog("+channel+")");
  if (t.state==nidUstStopped) SEQUENCER_ERRORN(SEQUENCER_ERROR_USTNOTRUNNING_NUM, "NIdriver::stop_analog("+channel+")");
  DAQmxErrChk(DAQmxStopTask(t.task), "NIdriver::stop_analog("+channel+")");
};

void NIdriver::write_analog(std::string channel, double value, double* exetime){
  if (singletask_names.find(channel)==singletask_names.end()) SEQUENCER_ERRORN(SEQUENCER_ERROR_CNOTEXIST_NUM, "NIdriver::write_analog("+channel+", "+floattostr(value)+")");
  nid_ustask t=singletasks[singletask_names[channel]];
  if (t.type!=cmAnalogOutput) SEQUENCER_ERRORN(SEQUENCER_ERROR_NOTANALOG_NUM, "NIdriver::write_analog("+channel+", "+floattostr(value)+")");
  if (t.state==nidUstStopped) SEQUENCER_ERRORN(SEQUENCER_ERROR_USTNOTRUNNING_NUM, "NIdriver::write_analog("+channel+", "+floattostr(value)+")");
  float64     data=cmanager->scale_value(channel, value);
  //int32       pointsRead;
  if (exetime!=NULL) *exetime=timer->get_time();
  DAQmxErrChk(DAQmxWriteAnalogF64(t.task,1,1,10.0,DAQmx_Val_GroupByChannel,&data,NULL,NULL), "NIdriver::write_analog("+channel+", "+floattostr(value)+")");
  singletasks[singletask_names[channel]].value=value;
};


bool NIdriver::read_digital(std::string channel, double* exetime){
  if (singletask_names.find(channel)==singletask_names.end()) SEQUENCER_ERRORN(SEQUENCER_ERROR_CNOTEXIST_NUM, "NIdriver::read_digital("+channel+")");
  nid_ustask t=singletasks[singletask_names[channel]];
  if ((t.type!=cmDigitalInput)) SEQUENCER_ERRORN(SEQUENCER_ERROR_NOTDIGITAL_NUM, "NIdriver::read_digital("+channel+")");
  if (t.state==nidUstStopped) SEQUENCER_ERRORN(SEQUENCER_ERROR_USTNOTRUNNING_NUM, "NIdriver::read_digital("+channel+")");
  uInt8       data[1];
  int32       pointsRead;
  if (exetime!=NULL) *exetime=timer->get_time();
  DAQmxErrChk(DAQmxReadDigitalLines(t.task,1,10.0,DAQmx_Val_GroupByChannel,data,1,&pointsRead,NULL,NULL), "NIdriver::read_digital("+channel+")");
  if (pointsRead<=0) SEQUENCER_ERRORN(SEQUENCER_ERROR_COULDNOTREAD_NUM, "NIdriver::read_digital("+channel+")");
  return cmanager->invert_value(channel, (data!=0));
};


void NIdriver::write_digital(std::string channel, bool value, double* exetime){
  if (singletask_names.find(channel)==singletask_names.end()) SEQUENCER_ERRORN(SEQUENCER_ERROR_CNOTEXIST_NUM, "NIdriver::write_digital("+channel+", "+booltostr(value)+")");
  nid_ustask t=singletasks[singletask_names[channel]];
  if (t.type!=cmDigitalOutput) SEQUENCER_ERRORN(SEQUENCER_ERROR_NOTDIGITAL_NUM, "NIdriver::write_digital("+channel+", "+booltostr(value)+")");
  if (t.state==nidUstStopped) SEQUENCER_ERRORN(SEQUENCER_ERROR_USTNOTRUNNING_NUM, "NIdriver::write_digital("+channel+", "+booltostr(value)+")");
  uInt8       data=cmanager->invert_value(channel, value)?1:0;
  if (exetime!=NULL) *exetime=timer->get_time();
  DAQmxErrChk(DAQmxWriteDigitalLines(t.task,1,1,1.0,DAQmx_Val_GroupByChannel,&data,NULL,NULL), "NIdriver::write_digital("+channel+", "+booltostr(value)+")");
  singletasks[singletask_names[channel]].value=value?1.:0.0;
};

void NIdriver::reset(double* exetime){
  vartiming_read_ini();
  std::set<std::string> devices;
  // go through all channels in the channel manager cmanager and register the devices
  // int the set devices.
  if (cmanager->get_channel_count()>0) {
    for (size_t i=0; i<cmanager->get_channel_count(); i++) {
      if (devices.find(cmanager->get(i).name)==devices.end()) {
        devices.insert(strstrip(cmanager->get(i).device_name));
      }
    }
  }

  // now clear all possibly existing tasks
  clear_us();
  clear_tasks();
  // reset the devices
  if (devices.size()>0) for (std::set<std::string>::const_iterator i=devices.begin(); i!=devices.end(); ++i) {
    DAQmxErrChk(DAQmxResetDevice(i->c_str()), "NIdriver::reset('"+*i+"')");
  }
  construct();
  set_routing();
  if (exetime!=NULL) *exetime=timer->get_time();
};


std::string NIdriver::get_hardware_adress(cm_channel channel){
  std::string res=channel.device_name;
  if (res.size()>0) res+=NISEQGEN_DEVICE_PORT_DIVIDER;
  res+=channel.port_name;
  if (channel.port_name.size()>0) res+=NISEQGEN_PORT_PIN_DIVIDER;
  res+=channel.pin_name;
  return res;
};

bool NIdriver::uschannel_unused(std::string name){
  if (singletasks.size()>0) for (size_t i=0; i<singletasks.size(); i++) {
    if ((singletasks[i].name==name) && singletasks[i].state==nidUstUsable) return true;
  }
  return false;
};

int NIdriver::get_new_inputtask_handle(){
  std::map<int, nid_inputtask>::const_iterator i;
  int max=-1;
  for (i=inputtasks.begin(); i!=inputtasks.end(); ++i) {
    if (i->first>max) max=i->first;
  }
  return max+1;
};

int NIdriver::create_input_task(std::vector<std::string> channels, long num_samples, double sampletime, bool savetofile, std::string filename){
  std::string method="NIdriver::create_input_task('"+fromStringVector(channels, ", ")+"', "+inttostr(num_samples)+", "+floattostr(sampletime)+" us, "+booltostr(savetofile)+", "+filename+")";
  nid_inputtask task;
  int newHandle=get_new_inputtask_handle(); // create a new handle for this task
  task.state=nidiCreated;
  task.num_samples=num_samples;
  task.filename=filename;
  task.savetofile=savetofile;
  task.hwchannels="";
  task.sampletime=sampletime;
  if (channels.size()>0) {
    /* now we go through all channels in the list channels and check them. All channels in the task
       do have to be on the same device and do have to have the same specs (min, max, type ...)
       At the end we either have all channels in task.channels or the method ended with an exception.
    */
    task.min=cmanager->get(channels[0]).min;
    task.max=cmanager->get(channels[0]).max;
    task.device=cmanager->get(channels[0]).device_name;
    task.type=cmanager->get(channels[0]).type;
    for (size_t i=0; i<channels.size(); i++) {
      cm_channel c=cmanager->get(channels[i]);
      if (task.min!=c.min
          || task.max!=c.max
          || task.device!=c.device_name ) {
        SEQUENCER_ERRORN(SEQUENCER_ERROR_INPUTUNEQUALSPECS_NUM, method.c_str());
      };
      if (c.type!=cmAnalogInputSE && c.type!=cmAnalogInputDE) {
        SEQUENCER_ERRORN(SEQUENCER_ERROR_INPUTONLYANALOG_NUM, method.c_str());
      }
      if (!uschannel_unused(channels[i])) {
        SEQUENCER_ERRORN(SEQUENCER_ERROR_CHANNELINUSE_NUM, method.c_str());
      }
      task.channels.push_back(channels[i]);

      if (i>0) task.hwchannels+=", ";
      std::string ch=c.device_name+NISEQGEN_DEVICE_PORT_DIVIDER+c.port_name+NISEQGEN_PORT_PIN_DIVIDER+c.pin_name;
      if (c.port_name=="") ch=c.device_name+NISEQGEN_DEVICE_PORT_DIVIDER+c.pin_name;
      task.hwchannels+=ch;
    }

    // if we reached this point then all channels in task.channels have passed all tests and may therefore be
    // used as synchronised inputs ... so we can unregister them from the pool of unsynchronised channels:
    if (singletasks.size()>0) {
      for (size_t i=0; i<singletasks.size(); i++) {
        if (std::find(task.channels.begin(), task.channels.end(), singletasks[i].name)!=task.channels.end()) { // this unsynchronised task will be used
                                                                            // for this synchronised input
          if (singletasks[i].state==nidUstUsable) { // if the unsynchronised task is running: stop it ... and unreserve all resources
            DAQmxErrChk(DAQmxTaskControl(singletasks[i].task, DAQmx_Val_Task_Unreserve ), method.c_str());
            singletasks[i].state=nidUstStopped;
          }
        }
      }
    }

    // create a task
    DAQmxErrChk(DAQmxCreateTask("", &(task.task)), method.c_str());
    int32 tmode=DAQmx_Val_RSE; // single-ended
    if (task.type==cmAnalogInputDE) { tmode=DAQmx_Val_Diff ; }

    // create a voltage input
    DAQmxErrChk( DAQmxCreateAIVoltageChan(task.task, task.hwchannels.c_str(), "", tmode, task.min, task.max, DAQmx_Val_Volts, ""), (method+" channel="+task.hwchannels).c_str());

    // read timing from timing.ini file
    jkiniparser ini;
    try {
       ini.readFile(timing_inifile); // parse timing.ini file
    } catch (jkipError e) {
       SEQUENCER_ERRORN(SEQUENCER_ERROR_INI_NUM, e.getMessage());
    }
    std::string g=fromStringVector(task.channels, ", ");
    if (!ini.groupExists(g)) {
      g=task.device+"/ai";
      if (!ini.groupExists(g)) {
        g=task.device;
        if (!ini.groupExists(g)) {
          SEQUENCER_ERROR(SEQUENCER_ERROR_DEVNOTINI_NUM, format(get_errormessage(SEQUENCER_ERROR_DEVNOTINI_NUM), g.c_str(), timing_inifile.c_str()), method.c_str());
        }
      }
    }
    std::string source=ini.getAsString(g, "source", "");
    std::string edge=ini.getAsString(g, "edge", "rising");
    int32 activeEdge=DAQmx_Val_Rising;
    if (edge=="falling") activeEdge=DAQmx_Val_Falling;

    // set timing configuration
    DAQmxErrChk(DAQmxCfgSampClkTiming(task.task,source.c_str(),1.0/(task.sampletime/1e6),activeEdge,(continuoustask)?DAQmx_Val_ContSamps:DAQmx_Val_FiniteSamps,task.num_samples), method.c_str());
    //std::cout<<"source='"<<source.c_str()<<"'   samplerate="<<1.0/(sampletime/1e6)<<" 1/s"<<std::endl;

    task.state=nidiCreated;

    inputtasks[newHandle]=task;

    // set the rest of the timing/triggering options
    set_input_triggering(ini, newHandle);

    return newHandle;
  } else {
      return 0;
  }
}

void NIdriver::int_set_triggering(jkiniparser& ini, std::string g, TaskHandle t){
  // set reference clock
  std::string clksrc=ini.getAsString(g, "refclk_source", "none");
  double clkrate=ini.getAsDouble(g, "refclk_rate", 10e6);
  if (clksrc!="none") {
    DAQmxErrChk(DAQmxSetRefClkSrc(t, clksrc.c_str()), "NIdriver::int_set_triggering()");
    DAQmxErrChk(DAQmxSetRefClkRate(t, clkrate), "NIdriver::int_set_triggering()");
  }

  // export some signals if configured in timing.ini
  std::string exprt_start=ini.getAsString(g, "export_start_trigger", "no");
  if (exprt_start!="no") {
    DAQmxErrChk(DAQmxExportSignal(t, DAQmx_Val_StartTrigger , exprt_start.c_str()), "NIdriver::int_set_triggering()");
  }
  std::string exprt_sclk=ini.getAsString(g, "export_sampleclock", "no");
  if (exprt_sclk!="no") {
    DAQmxErrChk(DAQmxExportSignal(t, DAQmx_Val_SampleClock , exprt_sclk.c_str()), "NIdriver::int_set_triggering()");
  }
  std::string exprt_10MHz=ini.getAsString(g, "export_10MHz", "no");
  if (exprt_10MHz!="no") {
    DAQmxErrChk(DAQmxExportSignal(t, DAQmx_Val_10MHzRefClock , exprt_10MHz.c_str()), "NIdriver::int_set_triggering()");
  }
  std::string exprt_20MHz=ini.getAsString(g, "export_20MHz", "no");
  if (exprt_20MHz!="no") {
    DAQmxErrChk(DAQmxExportSignal(t, DAQmx_Val_20MHzTimebaseClock, exprt_20MHz.c_str()), "NIdriver::int_set_triggering()");
  }
  std::string exprt_sctbs=ini.getAsString(g, "sampleclock_timebase_source", "no");
  if (exprt_sctbs!="no") {
    DAQmxErrChk(DAQmxSetSampClkTimebaseSrc(t, exprt_sctbs.c_str()), "NIdriver::int_set_triggering()");
  }
  std::string exprt_mtb=ini.getAsString(g, "master_timebase_source", "no");
  double mclkrate=ini.getAsDouble(g, "master_timebase_rate", 20e6);
  if (exprt_mtb!="no") {
    DAQmxErrChk(DAQmxSetMasterTimebaseSrc(t, exprt_mtb.c_str()), "NIdriver::int_set_triggering()");
    DAQmxErrChk(DAQmxSetMasterTimebaseRate(t, mclkrate), "NIdriver::int_set_triggering()");
  }

  // add start trigger for all tasks
//  if ((t.type==cmAnalogOutput)||(t.type==cmDigitalOutput)) {
    std::string trigger=ini.getAsString(g, "trigger", "no");
    std::string tedge=ini.getAsString(g, "trigger_edge", "rising");
    int32 edge=DAQmx_Val_Rising;
    if (tedge=="falling") edge=DAQmx_Val_Falling;
    if (trigger!="no")  {
      DAQmxErrChk (DAQmxCfgDigEdgeStartTrig(t,trigger.c_str(),edge), "NIdriver::int_set_triggering()");
    }
//  }
}

void NIdriver::set_input_triggering(jkiniparser& ini, int itask){
	//char	    str1[256],str2[256],trigName[256];

	nid_inputtask t=inputtasks[itask];


  // now we have to add the start trigger and other trigger settings to all channels except lasttask
  //find the group in the timing.ini file to read from
  std::string g=fromStringVector(t.channels, ",");
  if (!ini.groupExists(g)) {
    g=t.device+"/ai";
    if (t.type==cmDigitalInput) g=t.device+"/di";
    if (!ini.groupExists(g)) {
      g=t.device;
      if (!ini.groupExists(g)) {
        SEQUENCER_ERROR(SEQUENCER_ERROR_DEVNOTINI_NUM, format(get_errormessage(SEQUENCER_ERROR_DEVNOTINI_NUM), g.c_str(), timing_inifile.c_str()), "NIdriver::set_input_triggering()");
      }
    }
  }

  int_set_triggering(ini, g, t.task);

}

void NIdriver::start_input_task(int handle, double* exetime){
  std::string method="NIdriver::start_input_task("+inttostr(handle)+")";

  // check whether the handle is valid
  if (! input_task_exists(handle)) {
    SEQUENCER_ERRORN(SEQUENCER_ERROR_INPUTTASKNE_NUM, method.c_str());
  }

  // if the handle is valid:
  // start the task
	nid_inputtask t=inputtasks[handle];
  DAQmxErrChk(DAQmxStartTask(t.task), method.c_str());
  if (exetime!=NULL) *exetime=timer->get_time();
  inputtasks[handle].state=nidiRunning;
}

void NIdriver::stop_input_task(int handle, double* exetime){
  std::string method="NIdriver::stop_input_task("+inttostr(handle)+")";

  // check whether the handle is valid
  if (! input_task_exists(handle)) {
    SEQUENCER_ERRORN(SEQUENCER_ERROR_INPUTTASKNE_NUM, method.c_str());
  }

  // if the handle is valid:
  // start the task
	nid_inputtask t=inputtasks[handle];
  DAQmxErrChk(DAQmxStopTask(t.task), method.c_str());
  if (exetime!=NULL) *exetime=timer->get_time();
  inputtasks[handle].state=nidiStopped;
}

void NIdriver::clear_input_task(int handle, double* exetime){
  std::string method="NIdriver::clear_input_task("+inttostr(handle)+")";

  // check whether the handle is valid
  if (! input_task_exists(handle)) {
    SEQUENCER_ERRORN(SEQUENCER_ERROR_INPUTTASKNE_NUM, method.c_str());
  }

  // if the handle is valid:
  // start the task
	nid_inputtask t=inputtasks[handle];
  DAQmxErrChk(DAQmxClearTask(t.task), method.c_str());
  if (exetime!=NULL) *exetime=timer->get_time();

  // reactivate the unsychrnoised tasks for the channels in this synchronised input task:
  /* go through all channels in singletasks and check with channelManager \c cm whether
     they are marked as used or blocked. If they are: start the task. Ignore the other tasks
   */
   // t.channels
  if (singletasks.size()>0) {
    for (size_t i=0; i<singletasks.size(); i++) {
      if (std::find(t.channels.begin(), t.channels.end(), singletasks[i].name)!=t.channels.end()) {
        // this unsynchronised task has been used for this synchronised input and will be reactivated
        if (singletasks[i].state==nidUstStopped) { // if the unsynchronised task is running: stop it ... and unreserve all resources
//          DAQmxErrChk(DAQmxTaskControl(singletasks[i].task, DAQmx_Val_Task_Reserve ), (method+" task: "+singletasks[i].name).c_str());
//          DAQmxErrChk(DAQmxTaskControl(singletasks[i].task, DAQmx_Val_Task_Verify ), (method+" task: "+singletasks[i].name).c_str());
//          DAQmxErrChk(DAQmxTaskControl(singletasks[i].task, DAQmx_Val_Task_Commit ), (method+" task: "+singletasks[i].name).c_str());
          singletasks[i].state=nidUstUsable;
        }
      }
    }
  }

  // delete the task from the inputtasks map
  inputtasks.erase(handle);
}

double* NIdriver::read_input_task(int handle){
  std::string method="NIdriver::read_input_task("+inttostr(handle)+")";

  // check whether the handle is valid
  if (! input_task_exists(handle)) {
    SEQUENCER_ERRORN(SEQUENCER_ERROR_INPUTTASKNE_NUM, method.c_str());
  }

  // if the handle is valid:
  // read the data
	nid_inputtask t=inputtasks[handle];
	double* data1=(double*)malloc(t.num_samples*t.channels.size()*sizeof(double));
  DAQmxErrChk(DAQmxReadAnalogF64(t.task,t.num_samples, 10.0,DAQmx_Val_GroupByScanNumber, data1,t.num_samples*t.channels.size(),NULL,NULL), method.c_str());

	double* data=(double*)calloc(t.num_samples*(1+t.channels.size()), sizeof(double));
	long long x=0;
	double tim=0;
	for (long long i=0; i<t.num_samples*(1+t.channels.size()); i+=1+t.channels.size()) {
    data[i]=tim;
    tim=tim+t.sampletime;
    for (size_t j=1; j<=t.channels.size(); j++) {
      data[i+j]=cmanager->invscale_value(t.channels[j-1] , data1[x+j-1]);
    }
    x+=t.channels.size();
  }
  free(data1);
	return data;
}

bool NIdriver::is_input_task_done(int handle){
  std::string method="NIdriver::is_input_task_finished("+inttostr(handle)+")";

  // check whether the handle is valid
  if (! input_task_exists(handle)) {
    SEQUENCER_ERRORN(SEQUENCER_ERROR_INPUTTASKNE_NUM, method.c_str());
  }

  // if the handle is valid:
	nid_inputtask t=inputtasks[handle];

  bool32 done;
  DAQmxErrChk(DAQmxIsTaskDone(t.task, &done), "NIdriver::is_done()");
  return done;
}

void NIdriver::save_input_task(int handle, std::string filename){
  std::string method="NIdriver::save_input_task("+inttostr(handle)+")";

  // check whether the handle is valid
  if (! input_task_exists(handle)) {
    SEQUENCER_ERRORN(SEQUENCER_ERROR_INPUTTASKNE_NUM, method.c_str());
  }

  // if the handle is valid:
  // start the task
	nid_inputtask t=inputtasks[handle];
	double* data1=(double*)malloc(t.num_samples*t.channels.size()*sizeof(double));
  DAQmxErrChk(DAQmxReadAnalogF64(t.task,t.num_samples, 10.0,DAQmx_Val_GroupByScanNumber, data1,t.num_samples*t.channels.size(),NULL,NULL), method.c_str());

  FILE* f;
  f=fopen(filename.c_str(), "w");
  if (f==NULL) {
    free(data1);
    std::string errm="could not open the file";
    SEQUENCER_ERRORV(SEQUENCER_ERROR_INPUTTASKFERROR_NUM, method.c_str(), filename.c_str(), errm.c_str());
  }

  fprintf(f, "#\n# time [uSec], \t%s\n#\n", fromStringVector(t.channels, ", \t").c_str());

	//double* data=(double*)calloc(t.num_samples*(1+t.channels.size()), sizeof(double));
	long long x=0;
	double tim=0;
	for (long long i=0; i<t.num_samples*(1+t.channels.size()); i+=1+t.channels.size()) {
    fprintf(f, "%10.1lf", tim);
    tim=tim+t.sampletime;
    for (size_t j=1; j<=t.channels.size(); j++) {
      fprintf(f, ", \t%15.10lf", cmanager->invscale_value(t.channels[j-1] , data1[x+j-1]));
    }
    x+=t.channels.size();
    fprintf(f, "\n");
  }
  free(data1);
	fclose(f);
}

bool NIdriver::input_task_exists(int handle){
  return inputtasks.find(handle)!=inputtasks.end();
}

nid_inputtask NIdriver::get_input_task(int handle) {
  std::string method="NIdriver::get_input_task("+inttostr(handle)+")";

  // check whether the handle is valid
  if (! input_task_exists(handle)) {
    SEQUENCER_ERRORN(SEQUENCER_ERROR_INPUTTASKNE_NUM, method.c_str());
  }

  // if the handle is valid:
  // start the task
	return inputtasks[handle];
};

void NIdriver::save_initialvalues(NIsequenceGenerator* sg) {
  sg->clear_initialvalues();
  if (singletasks.size()>0) {
    for (size_t i=0; i<singletasks.size(); i++) {
      sg->add_initialvalue(singletasks[i].name, singletasks[i].value);
    }
  }
};

void NIdriver::read_initialvalues_from_sequenceend(NIsequenceGenerator* sg) {
  if (singletasks.size()>0) {
    for (size_t i=0; i<singletasks.size(); i++) {
      singletasks[i].value=sg->get_endvalue(singletasks[i].name, singletasks[i].value);
    }
  }
};

}
