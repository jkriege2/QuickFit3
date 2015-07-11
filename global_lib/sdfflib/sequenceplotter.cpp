
#include "sequenceplotter.h" // class's header file


namespace sequencer {

// class constructor
sequencePlotter::sequencePlotter()
   :cairoSequencePlotter()
{
  smooth_analog_graphs = true;
}

// class destructor
sequencePlotter::~sequencePlotter()
{
	clear();
	clear_markers();
}


void sequencePlotter::plot_graphs(){
//  std::cout<<"plot:  ";
  if (chnum>0) {
    for (unsigned long i=0; i<chnum; i++) {
//        std::cout<<i<<"  ";
      double y0=get_plot_y0(i);
      double shifty=get_plot_shift(i);
      double scaley=get_plot_scale(i);
      //std::cout<<i<<": "<<scaley<<",  "<<shifty<<std::endl;
      p->set_dash(0);
      p->set_stroke(color_graph);
      p->set_fill(color_graph);
      p->set_source(color_graph);
      p->set_line_width(graph_line_width*surrounding_scale);
      if (get_array_size()>0) {
        if ((*cmanager)[channames[i]].type==cmAnalogOutput) { // ananlog channel
            double xo=graph_x0*surrounding_scale;
            double yval=qget_analog(channames[i], t_start);
            if (plot_scaled) yval=(*cmanager).invscale_value(channames[i], yval);
            double yo=y0-scaley*(yval-shifty);
            for (double t=t_start+get_sample_timestep(); t<=t_stop; t+=get_sample_timestep()) {
              double xn=xo+coord_x_1range;
              yval=qget_analog(channames[i], t);
              if (plot_scaled) yval=(*cmanager).invscale_value(channames[i], yval);
              double yn=y0-scaley*(yval-shifty);
              //std::cout<<"("<<t<<", "<<yval<<", "<<y0-yn<<")  \n";
              if (smooth_analog_graphs) {
                p->line(xo, yo, xn, yn);
              } else {
                p->line(xo, yo, xn, yo);
                p->line(xn, yo, xn, yn);
              }
              xo=xn; yo=yn;
            }
        } else { // digital channels
            double xo=graph_x0*surrounding_scale;
            bool yb=qget_digital(channames[i], t_start);
            double yval=yb?1:0;
            if (plot_scaled) yval=(*cmanager).invert_value(channames[i], yb)?1:0;
            double yo=y0-(yval-shifty)*scaley;
            for (double t=t_start+get_sample_timestep(); t<=t_stop; t+=get_sample_timestep()) {
              double xn=xo+coord_x_1range;
              yb=qget_digital(channames[i], t);
              yval=yb?1:0;
              if (plot_scaled) yval=(*cmanager).invert_value(channames[i], yb)?1:0;
              double yn=y0-(yval-shifty)*scaley;
              p->line(xo, yo, xn, yo);
              p->line(xn, yo, xn, yn);
              xo=xn; yo=yn;
            }
        }
      }
    }
  }
//  std::cout<<std::endl;
}




/* copied from NIsequenceGenerator ---------------------------------------------------------------------------------------- */

void* sequencePlotter::get_array(unsigned long num){
  if (num>=get_channel_count()) {
    SEQUENCER_ERRORN(SEQUENCER_ERROR_RANGE_NUM, "sequencePlotter.get_array("+inttostr(num)+")");
  } else {
    std::map<std::string, sg_channel>::const_iterator i=channels.begin();
    if (num>0) for (int j=0; j<num; j++) ++i;
    return get_nth_array(channels[i->first].data);
  }
};


unsigned long sequencePlotter::get_array_num(unsigned long num){
  if (num>=get_channel_count()) {
    SEQUENCER_ERRORN(SEQUENCER_ERROR_RANGE_NUM,"sequencePlotter.get_array_num("+inttostr(num)+")");
  } else {
    std::map<std::string, sg_channel>::const_iterator i=channels.begin();
    if (num>0) for (int j=0; j<num; j++) ++i;
    return channels[i->first].data;
  }
};



sg_dataformat sequencePlotter::get_dataformat(unsigned long num){
  if (num>=get_array_count()) {
    SEQUENCER_ERRORN(SEQUENCER_ERROR_RANGE_NUM, "sequencePlotter.get_dataformat("+inttostr(num)+")");
  } else {
    std::map<std::string, sg_channel>::const_iterator i=channels.begin();
    if (num>0) for (int j=0; j<num; j++) ++i;
    return get_nth_dataformat(channels[i->first].data);
  }
};


unsigned char sequencePlotter::get_offset(unsigned long num){
  if (num>=get_channel_count()) {
    SEQUENCER_ERRORN(SEQUENCER_ERROR_RANGE_NUM, "sequencePlotter.get_array("+inttostr(num)+")");
  } else {
    std::map<std::string, sg_channel>::const_iterator i=channels.begin();
    if (num>0) for (int j=0; j<num; j++) ++i;
    return channels[i->first].offset;
  }
};

void sequencePlotter::set_analog(std::string channel, double timestep, double value) {
  //std::cout<<"set("<<channel<<", "<<timestep<<", "<<value<<")    end_time="<<end_time<<"    ";
  if (timestep>end_time) SEQUENCER_ERRORN(SEQUENCER_ERROR_AFTERTIMEEND_NUM, "sequencePlotter.set_analog(\""+channel+"\", "+floattostr(timestep)+", "+floattostr(value)+")");
  //std::cout<<"...   ";
  if (get_dataformat(channel)!=sgDouble) SEQUENCER_ERRORN(SEQUENCER_ERROR_NOTANALOG_NUM, "sequencePlotter.set_analog(\""+channel+"\", "+floattostr(timestep)+", "+floattostr(value)+")");
  //std::cout<<"arraynum="<<get_array_num(channel)<<"    ";
  //std::cout<<"arraycount="<<get_array_count()<<"    ";
  sg_arraydata a=arrays[get_array_num(channel)];
  //std::cout<<"index="<<time_to_index(timestep)*a.channels+get_offset(channel)<<"    ";
  //std::cout<<"cvalue="<<((double*)a.data)[time_to_index(timestep)*a.channels+get_offset(channel)]<<"    ";
  ((double*)a.data)[time_to_index(timestep)*a.channels+get_offset(channel)]=value;
  //std::cout<<"nvalue="<<((double*)a.data)[time_to_index(timestep)*a.channels+get_offset(channel)]<<"\n";
};

void sequencePlotter::set_analog(std::string channel, double time_start, double time_stop, double value){
  if ((time_start>end_time)||(time_stop>end_time)) SEQUENCER_ERRORN(SEQUENCER_ERROR_AFTERTIMEEND_NUM, "sequencePlotter.set_analog(\""+channel+"\", "+floattostr(time_start)+", "+floattostr(time_stop)+", "+floattostr(value)+")");
  if (get_dataformat(channel)!=sgDouble) SEQUENCER_ERRORN(SEQUENCER_ERROR_NOTANALOG_NUM, "sequencePlotter.set_analog(\""+channel+"\", "+floattostr(time_start)+", "+floattostr(time_stop)+", "+floattostr(value)+")");
  unsigned int num=get_array_num(channel);
  unsigned char offset=get_offset(channel);
  unsigned long long tstop=time_to_index(time_stop);
  unsigned long long tstart=time_to_index(time_start);
  sg_arraydata a=arrays[get_array_num(channel)];
  double* aa=(double*)arrays[num].data;
  for (unsigned long long t=tstart; t<=tstop; t++) {
    //std::cout<<t*a.channels+offset<<"   ";
    aa[t*a.channels+offset]=value;
  }
};

void sequencePlotter::set_linear(std::string channel, double time_start, double time_stop,  double startvalue, double endvalue){
  if ((time_start>end_time)||(time_stop>end_time)) SEQUENCER_ERRORN(SEQUENCER_ERROR_AFTERTIMEEND_NUM, "sequencePlotter.set_linear(\""+channel+"\", "+floattostr(time_start)+", "+floattostr(time_stop)+", "+floattostr(endvalue)+")");
  if (get_dataformat(channel)!=sgDouble) SEQUENCER_ERRORN(SEQUENCER_ERROR_NOTANALOG_NUM, "sequencePlotter.set_linear(\""+channel+"\", "+floattostr(time_start)+", "+floattostr(time_stop)+", "+floattostr(endvalue)+")");
  unsigned int num=get_array_num(channel);
  unsigned char offset=get_offset(channel);
  unsigned long long tstop=time_to_index(time_stop);
  unsigned long long tstart=time_to_index(time_start);
  double dV=(endvalue-startvalue)/mmax(1,tstop-tstart);
  double v=startvalue;
  double* a=(double*)arrays[num].data;
  for (unsigned long long t=tstart; t<=tstop; t++) {
    a[t*arrays[num].channels+offset]=v;
    v+=dV;
  }
};

void sequencePlotter::set_digital(std::string channel, double time_start, double time_stop, bool value){
  if ((time_start>end_time)||(time_stop>end_time)) SEQUENCER_ERRORN(SEQUENCER_ERROR_AFTERTIMEEND_NUM, "sequencePlotter.set_digital(\""+channel+"\", "+floattostr(time_start)+", "+floattostr(time_stop)+", "+booltostr(value)+")");
  unsigned int num=get_array_num(channel);
  unsigned char offset=get_offset(channel);
  sg_dataformat df=get_dataformat(channel);
  unsigned long long tstop=time_to_index(time_stop);
  unsigned long long tstart=time_to_index(time_start);
  if (df==sgUInt32) {
    uint32_t* aa=(uint32_t*)arrays[num].data;
    for (unsigned long long t=tstart; t<=tstop; t++) {
      if (value) {set_bit(aa[t], offset)} else {clear_bit(aa[t], offset)}
    }
    return;
  }
  if (df==sgUInt8) {
    uint8_t* aa=(uint8_t*)arrays[num].data;
    for (unsigned long long t=tstart; t<=tstop; t++) {
      if (value) {set_bit(aa[t], offset);} else {clear_bit(aa[t], offset);}
    }
    return;
  }
  if (df==sgUInt16) {
    uint16_t* aa=(uint16_t*)arrays[num].data;
    for (unsigned long long t=tstart; t<=tstop; t++) {
      if (value) {set_bit(aa[t], offset);} else {clear_bit(aa[t], offset);}
//      std::cout<<t<<"   "<<offset<<std::endl;
    }
    return;
  }
  SEQUENCER_ERRORN(SEQUENCER_ERROR_NOTDIGITAL_NUM, "sequencePlotter.set_digital(\""+channel+"\", "+floattostr(time_start)+", "+floattostr(time_stop)+", "+booltostr(value)+")");
};

void sequencePlotter::set_digital(std::string channel, double timestep, bool value){
  if (timestep>end_time) SEQUENCER_ERRORN(SEQUENCER_ERROR_AFTERTIMEEND_NUM, "sequencePlotter.set_digital(\""+channel+"\", "+floattostr(timestep)+", "+booltostr(value)+")");
  unsigned int num=get_array_num(channel);
  unsigned char offset=get_offset(channel);
  sg_dataformat df=get_dataformat(channel);
  if (df==sgUInt32) {
    if (value) {set_bit(((uint32_t*)arrays[num].data)[time_to_index(timestep)], offset);}
    else {clear_bit(((uint32_t*)arrays[num].data)[time_to_index(timestep)], offset);}
    return;
  }
  if (df==sgUInt8) {
    if (value) {set_bit(((uint8_t*)arrays[num].data)[time_to_index(timestep)], offset);}
    else {clear_bit(((uint8_t*)arrays[num].data)[time_to_index(timestep)], offset);}
    return;
  }
  if (df==sgUInt16) {
    if (value) {set_bit(((uint16_t*)arrays[num].data)[time_to_index(timestep)], offset);}
    else {clear_bit(((uint16_t*)arrays[num].data)[time_to_index(timestep)], offset);}
    return;
  }
  SEQUENCER_ERRORN(SEQUENCER_ERROR_NOTDIGITAL_NUM, "sequencePlotter.set_digital(\""+channel+"\", "+floattostr(timestep)+", "+booltostr(value)+")");
};



void sequencePlotter::init(channelManager* cm, double ssample_timestep, double send_time){

  // first delete all previous data
  clear();
  ports.clear();
  cmanager=cm;

  // init timing settings
  sample_timestep=ssample_timestep;
  end_time=send_time;
  if (end_time==-1) end_time=10*sample_timestep;
  array_size=(unsigned long long)(end_time/sample_timestep)+1;

  // init channels and data arrays.
  if ((*cmanager).get_channel_count()>0) {
      /*
        (I) for each channel in the channelManager object:
            1. check whether there is already an array for the device and port of the channel
               if there is:      add the channel to this array
               if there is not:  create a new array, do not get memory for this array !!!
            2. generate and save the channel information
      */
    for (int i=0; i<(*cmanager).get_channel_count(); i++) {
      std::string port=(*cmanager)[i].port_name;
      std::string device=(*cmanager)[i].device_name;
      int offset=0;
      int arraynum=-1;
      if (((*cmanager)[i].type==cmAnalogOutput)&& is_used((*cmanager)[i].name) ) {
        if (get_array_count()>0) for (unsigned long j=0; j<get_array_count(); j++) {
          if ((arrays[j].port==port)&&(arrays[j].device==device)) {
            arraynum=j;
          }
        }
        if (arraynum==-1) {
          sg_arraydata a;
          a.port=port;
          a.device=device;
          a.channels=1;
          a.format=sgDouble;
          arraynum=get_array_count();
          arrays.push_back(a);
          offset=0;
        } else {
          sg_arraydata a=arrays[arraynum];
          if (a.format!=sgDouble) SEQUENCER_ERRORN(SEQUENCER_ERROR_CHANNELDATAMISMATH_NUM, "sequencePlotter.init(&cm, "+floattostr(ssample_timestep)+", "+floattostr(send_time)+"),\n trying to add channel='"+(*cmanager)[i].name+"'(analog) to device='"+device+"' port='"+port+"'(digital)");
          offset=a.channels;   a.channels++;
          arrays[arraynum]=a;
        }
        channels[(*cmanager)[i].name].name=(*cmanager)[i].name;
        channels[(*cmanager)[i].name].hardware_name=get_hardware_adress((*cmanager)[i]);
        channels[(*cmanager)[i].name].data=arraynum;
        channels[(*cmanager)[i].name].offset=offset;
      }
      if (((*cmanager)[i].type==cmDigitalOutput)&& is_used((*cmanager)[i].name) ) {
        if (get_array_count()>0) for (unsigned long j=0; j<get_array_count(); j++) {
          if ((arrays[j].port==port)&&(arrays[j].device==device)) {
            arraynum=j;
          }
        }
        if (arraynum==-1) {
          sg_arraydata a;
          a.port=port;
          a.device=device;
          a.channels=1;
          a.format=sgUInt8;
          arraynum=get_array_count();
          arrays.push_back(a);
          offset=0;
        } else {
          sg_arraydata a=arrays[arraynum];
          if (a.format==sgDouble) SEQUENCER_ERRORN(SEQUENCER_ERROR_CHANNELDATAMISMATH_NUM, "sequencePlotter.init(&cm, "+floattostr(ssample_timestep)+", "+floattostr(send_time)+"),\n trying to add channel='"+(*cmanager)[i].name+"'(digital) to device='"+device+"' port='"+port+"'(analog)");
          offset=a.channels;   a.channels++;
          if ((a.format==sgUInt8)&&(a.channels>8)) a.format=sgUInt16;
          if ((a.format==sgUInt16)&&(a.channels>16)) a.format=sgUInt32;
          if ((a.format==sgUInt32)&&(a.channels>32)) SEQUENCER_ERRORN(SEQUENCER_ERROR_TOOMANYCHANNELS_NUM, "sequencePlotter.init(&cm, "+floattostr(ssample_timestep)+", "+floattostr(send_time)+")");
          arrays[arraynum]=a;
        }
        channels[(*cmanager)[i].name].name=(*cmanager)[i].name;
        channels[(*cmanager)[i].name].hardware_name=get_hardware_adress((*cmanager)[i]);
        channels[(*cmanager)[i].name].data=arraynum;
        channels[(*cmanager)[i].name].offset=offset;
      }
    }

    /* (II) now allocate the memory for all arrays */
    if (get_array_count()>0) for (int i=0; i<get_array_count(); i++) {
      if (arrays[i].format==sgDouble) {
        arrays[i].data=calloc(array_size*arrays[i].channels,sizeof(double));
      }
      if (arrays[i].format==sgUInt8) {
        arrays[i].data=calloc(array_size*arrays[i].channels,sizeof(uint8_t));
      }
      if (arrays[i].format==sgUInt16) {
        arrays[i].data=calloc(array_size*arrays[i].channels,sizeof(uint16_t));
      }
      if (arrays[i].format==sgUInt32) {
        arrays[i].data=calloc(array_size*arrays[i].channels,sizeof(uint32_t));
      }
    }
  }
};

std::string sequencePlotter::report(){
  std::string res;
  res+="end_time        =   "+floattostr(end_time)+ " micros\n";
  res+="sample_timestep =   "+floattostr(sample_timestep)+ " micros\n";
  res+="samples         =   "+floattostr(end_time/sample_timestep)+ "\n";
  res+="array_size      =   "+uinttostr(array_size)+ "\n";
  res+="channel_count   =   "+uinttostr(get_channel_count())+ "\n";
  res+="array_count     =   "+uinttostr(get_array_count())+ "\n";
  if (get_channel_count()>0) {
    res+="\n";
    res+="- channels ------------------------------------------------------------------------------------------\n";
    int c=0;
    for (std::map<std::string, sg_channel>::const_iterator i=channels.begin(); i!=channels.end(); ++i) {

      res += "  channel "+inttostr(c)+ "'"+i->second.name+"':\n";
      res += "    hardware name = "+i->second.hardware_name+"\n";
      res += "    data number   = "+inttostr(i->second.data)+"\n";
      res += "    offset        = "+inttostr(i->second.offset)+"\n";
      c++;
    }
  }
  if (get_array_count()>0) {
    res+="\n";
    res+="- arrays --------------------------------------------------------------------------------------------\n";
    for (unsigned long i=0; i<get_array_count(); i++) {

      res+="  array "+inttostr(i)+":\n";
      res+="    number of channels   = "+inttostr(arrays[i].channels)+"\n";
      res+="    adresses             = "+arrays[i].adress_string+"\n";
      if (arrays[i].format==sgDouble) {
        res+="    dataformat           = double\n";
        res+="\n";
        res+="    data:\n";
        res+=doublearraytostr((double*)arrays[i].data, arrays[i].channels, array_size, false)+"\n\n";
      }
      if (arrays[i].format==sgUInt8) {
        res+="    dataformat           = unsigned int, 8-bit\n";
        res+="\n";
        res+="    data:\n";
        res+=uint8vectostr((uint8_t*)arrays[i].data, array_size, true, true, true, false)+"\n\n";
      }
      if (arrays[i].format==sgUInt16) {
        res+="    dataformat           = unsigned int, 16-bit\n";
        res+="\n";
        res+="    data:\n";
        res+=uint16vectostr((uint16_t*)arrays[i].data, array_size, true, true, true, false)+"\n\n";
      }
      if (arrays[i].format==sgUInt32) {
        res+="    dataformat           = unsigned int, 32-bit\n";
        res+="\n";
        res+="    data:\n";
        res+=uint32vectostr((uint32_t*)arrays[i].data, array_size, true, true, true, false)+"\n\n";
      }
    }
  }
  return res;
};

void sequencePlotter::clear(){
  if (get_array_count()>0) for (int i=0; i<get_array_count(); i++) {
    free(arrays[i].data);
  }
  arrays.clear();
  channels.clear();
  sample_timestep=5; // 5 µs
  end_time=5000; // 5 ms  => 1000 samples per channel
  array_size=0;
}

void sequencePlotter::set_linear(std::string channel, double time_start, double time_stop,double endvalue){
     set_linear(channel, time_start, time_stop, get_analog(channel, time_start), endvalue);
};


/* ------------------------------------------------------------------------------------------------------------------------ */
}
