
#include "fastSequencePlotter.h" // class's header file


namespace sequencer {

// class constructor
fastSequencePlotter::fastSequencePlotter()
   : cairoSequencePlotter() // initialize base class
{
}

// class destructor
fastSequencePlotter::~fastSequencePlotter()
{
//	clear();
}

std::string fastSequencePlotter::report(){
  std::string res;
  res+="end_time        =   "+floattostr(end_time)+ " micros\n";
  res+="sample_timestep =   "+floattostr(sample_timestep)+ " micros\n";
  res+="samples         =   "+floattostr(end_time/sample_timestep)+ "\n";
  res+="channel_count   =   "+uinttostr(get_channel_count())+ "\n";
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
  return res;
};

void fastSequencePlotter::init(channelManager* cm, double ssample_timestep, double send_time){
  // first delete all previous data
  clear();
  cmanager=cm;

  // init timing settings
  sample_timestep=ssample_timestep;
  end_time=send_time;
  if (end_time==-1) end_time=10*sample_timestep;


  // init channels and data arrays.
  if (cmanager->get_channel_count()>0) {
    /*
    generate a fsp_list item in data and an entry in channels for
    for each DO or AO channel in the channelManager object
    */
    for (int i=0; i<cmanager->get_channel_count(); i++) {
      if ((((*cmanager)[i].type==cmAnalogOutput)||((*cmanager)[i].type==cmDigitalOutput))&& is_used((*cmanager)[i].name) ) {
        fsp_list a;
        a.name=(*cmanager)[i].name;
        a.list.clear();
        data.push_back(a);
        channels[a.name].name=a.name;
        channels[a.name].hardware_name=get_hardware_adress((*cmanager)[i]);
        channels[a.name].data=data.size()-1;
        channels[a.name].offset=0;
      }
    }
  }
}

void fastSequencePlotter::clear(){
  data.clear();
  channels.clear();
  sample_timestep=5; // 5 µs
  end_time=10; // 10 µs  => 2 sample per channel
};

void fastSequencePlotter::sort_data_bytime(int k){
  if (data[k].list.size()>1) {
    // sort using bubble sort (see http://de.wikipedia.org/wiki/Bubble_Sort#Formaler_Algorithmus),
    bool swapped=true;
    do {
        swapped = false;
        for (int i=0; i<=data[k].list.size()-2; i++) {
          if (data[k].list[i].tstart > data[k].list[i+1].tstart) {
              // swap array items:
              fsp_segment temp=data[k].list[i];
              data[k].list[i]=data[k].list[i+1];
              data[k].list[i+1]=temp;
              swapped = true;
          }
        }
    } while (swapped);
  }
};

void fastSequencePlotter::clean_list(int k){
  if (data[k].list.size()>0) {
    int oldlistsize=data[k].list.size();
    sort_data_bytime(k);
    if (data[k].list[0].tstart>0) {
      // add a segment at the back of the list (not covered by the next for-loop)
      // which bridges the gap between t=0 and the first segment
      fsp_segment s;
      s.dvalue=0;
      s.bvalue=false;
      s.type=fspConstant;
      s.tstart=0;
      s.tstop=data[k].list[0].tstart;
      data[k].list.push_back(s);
    }
    if (oldlistsize>1) for(int i=1; i<oldlistsize; i++) { // only iterate over the elements that are already in the list
                                                          // this way we can add new elements without iterating over them!
      // look in the digital channels for two subsequent segments that represent a signal change.
      // then add a fspSet0 or sfpSet1 segment:
      fsp_segment so=data[k].list[i-1];
      fsp_segment sn=data[k].list[i];
      if (get_dataformat(data[k].name)==cmDigitalOutput) {
        if ((so.type==fspConstant)&&(sn.type==fspConstant)&&(so.tstop==sn.tstart)) {
          if (so.bvalue && !sn.bvalue) {
            fsp_segment s;
            s.type=fspSet0;
            s.tstart=so.tstop;
            data[k].list.push_back(s);
          }
          if (!so.bvalue && sn.bvalue) {
            fsp_segment s;
            s.type=fspSet1;
            s.tstart=so.tstop;
            data[k].list.push_back(s);
          }
        }
      } else if (get_dataformat(data[k].name)==cmAnalogOutput) {
        // test whether two analog elements are connected (i.e. the t_stop of the first one equal t_start of the second one)
        // and whether there is a gap between them. If there is we add an upright line that connects the two elements
        double vleft=so.vstop;
        if (so.type==fspConstant) vleft=so.dvalue;
        double vright=sn.vstart;
        if (sn.type==fspConstant) vright=sn.dvalue;
        if ((vleft!=vright)&&(so.tstop==sn.tstart)) { // add an upright segment that connects two subsequent elements
          fsp_segment s;
          s.type=fspLinear;
          s.tstart=so.tstop;
          s.tstop=so.tstop;
          s.vstart=vleft;
          s.vstop=vright;
          //std::cout<<"added  ("<<s.tstart<<", "<<s.vstart<<")  ->  ("<<s.tstop<<", "<<s.vstop<<")\n";
          data[k].list.push_back(s);
        }
      }
    }
    sort_data_bytime(k);
  }
};

void fastSequencePlotter::plot_graphs(){
//  std::cout<<"plot:  ";
  if (chnum>0) {
    for (unsigned long i=0; i<chnum; i++) {
//        std::cout<<i<<"  ";
      double y0=get_plot_y0(i);
      double shifty=get_plot_shift(i);
      double scaley=get_plot_scale(i);
      double yval=0;
      //std::cout<<i<<": "<<scaley<<",  "<<shifty<<std::endl;
      p->set_dash(0);
      p->set_stroke(color_graph);
      p->set_fill(color_graph);
      p->set_source(color_graph);
      p->set_line_width(graph_line_width*surrounding_scale);
      if (data[i].list.size()>0) {
        if ((*cmanager)[channames[i]].type==cmAnalogOutput) { // ananlog channel
            clean_list(i);
            for (int k=0; k<data[i].list.size(); k++) {
              if (data[i].list[k].type==fspLinear) {
                double t1=data[i].list[k].tstart-t_start; // here we shift the time so that t=t_start -> t1=0
                double t2=data[i].list[k].tstop-t_start;
                //std::cout<<t1<<"  -  "<<t2<<std::endl;
                if((t1>=0 || (t1<t2 && t2>=0))&&(t1<=t_duration)) { // if the element is completely out of our range, we do not plot
                  t1=mmax(0,t1); // here we limit the element to the given plot range
                  t2=mmin(t2,t_duration);
                  //std::cout<<"linear  "<<t1<<"   --   "<<t2<<std::endl;
                  double x1=graph_x0*surrounding_scale+t1/get_sample_timestep()*coord_x_1range;
                  double x2=graph_x0*surrounding_scale+t2/get_sample_timestep()*coord_x_1range;
                  yval=data[i].list[k].vstart; if (plot_scaled) yval=(*cmanager).invscale_value(channames[i], yval);
                  double y1=y0-scaley*(yval-shifty);
                  yval=data[i].list[k].vstop; if (plot_scaled) yval=(*cmanager).invscale_value(channames[i], yval);
                  double y2=y0-scaley*(yval-shifty);
                  p->line(x1, y1, x2, y2);
                }// else std::cout<<"linear  "<<t1<<"   --   "<<t2<<"  not plotted"<<std::endl;
              }else if (data[i].list[k].type==fspConstant) {
                double t1=data[i].list[k].tstart-t_start;
                double t2=data[i].list[k].tstop-t_start;
                //std::cout<<t1<<"  -  "<<t2<<std::endl;
                if((t1>=0 || (t1<t2 && t2>=0))&&(t1<=t_duration)) {
                  t1=mmax(0,t1);
                  t2=mmin(t2,t_duration);
                  //std::cout<<"const   "<<t1<<"   --   "<<t2<<std::endl;
                  double x1=graph_x0*surrounding_scale+t1/get_sample_timestep()*coord_x_1range;
                  double x2=graph_x0*surrounding_scale+t2/get_sample_timestep()*coord_x_1range;
                  yval=data[i].list[k].dvalue; if (plot_scaled) yval=(*cmanager).invscale_value(channames[i], yval);
                  double y=y0-scaley*(yval-shifty);
                  p->line(x1, y, x2, y);
                }// else std::cout<<"const   "<<t1<<"   --   "<<t2<<"  not plotted"<<std::endl;
              }
            }
        } else { // digital channels
            clean_list(i);

            for (int k=0; k<data[i].list.size(); k++) {
              if (data[i].list[k].type==fspConstant) {
                double t1=data[i].list[k].tstart-t_start; // here we shift the time so that t=t_start -> t1=0
                double t2=data[i].list[k].tstop-t_start;
                //std::cout<<t1<<"  -  "<<t2<<std::endl;
                if((t1>=0 || t2>=0)&&(t1<=t_duration)) { // if the element is completely out of our range, we do not plot
                  t1=mmax(0,t1); // here we limit the element to the given plot range
                  t2=mmin(t2,t_stop-t_start);
                  double x1=graph_x0*surrounding_scale+t1/get_sample_timestep()*coord_x_1range;
                  double x2=graph_x0*surrounding_scale+t2/get_sample_timestep()*coord_x_1range;
                  yval=data[i].list[k].bvalue?1:0; if (plot_scaled) yval=((*cmanager).invert_value(channames[i], data[i].list[k].bvalue))?1:0;
                  double y=y0-scaley*(yval-shifty);
                  p->line(x1, y, x2, y);
                }
              } else if (data[i].list[k].type==fspSet1) {
                double t=data[i].list[k].tstart-t_start; // here we shift the time so that t=t_start -> t1=0
                //std::cout<<t1<<"  -  "<<t2<<std::endl;
                if(t>=0 && t<=t_duration) { // if the element is completely out of our range, we do not plot
                  double x=graph_x0*surrounding_scale+t/get_sample_timestep()*coord_x_1range;
                  yval=0; if (plot_scaled) yval=((*cmanager).invert_value(channames[i], false))?1:0;
                  double y1=y0-scaley*(yval-shifty);
                  yval=1; if (plot_scaled) yval=((*cmanager).invert_value(channames[i], true))?1:0;
                  double y2=y0-scaley*(yval-shifty);
                  p->line(x, y1, x, y2);
                }
              } else if (data[i].list[k].type==fspSet0) {
                double t=data[i].list[k].tstart-t_start; // here we shift the time so that t=t_start -> t1=0
                //std::cout<<t1<<"  -  "<<t2<<std::endl;
                if(t>=0 && t<=t_duration) { // if the element is completely out of our range, we do not plot
                  double x=graph_x0*surrounding_scale+t/get_sample_timestep()*coord_x_1range;
                  yval=1; if (plot_scaled) yval=((*cmanager).invert_value(channames[i], true))?1:0;
                  double y1=y0-scaley*(yval-shifty);
                  yval=0; if (plot_scaled) yval=((*cmanager).invert_value(channames[i], false))?1:0;
                  double y2=y0-scaley*(yval-shifty);
                  p->line(x, y1, x, y2);
                }
              }
            }
        }
      }
    }
  }
//  std::cout<<std::endl;
}

















cm_types fastSequencePlotter::get_dataformat(std::string channel){
  return (*cmanager).get(channel).type;
};









/* copied from NIsequenceGenerator ---------------------------------------------------------------------------------------- */


void fastSequencePlotter::set_analog(std::string channel, double timestep, double value) {
  //std::cout<<"5"<<std::endl;
  if (timestep>end_time) SEQUENCER_ERRORN(SEQUENCER_ERROR_AFTERTIMEEND_NUM, "fastSequencePlotter.set_analog(\""+channel+"\", "+floattostr(timestep)+", "+floattostr(value)+")");
  if (get_dataformat(channel)!=cmAnalogOutput) SEQUENCER_ERRORN(SEQUENCER_ERROR_NOTANALOG_NUM, "fastSequencePlotter.set_analog(\""+channel+"\", "+floattostr(timestep)+", "+floattostr(value)+")");
  int k=channels[channel].data;
  //std::cout<<"set_analog(\""+channel+"\", "+floattostr(timestep)+", "+floattostr(value)+")     k="<<k<<std::endl;
  fsp_segment s;
  s.type=fspConstant;
  s.tstart=timestep;
  s.tstop=timestep+sample_timestep;
  s.dvalue=value;
  data[k].list.push_back(s);
};

void fastSequencePlotter::set_analog(std::string channel, double time_start, double time_stop, double value){
  //std::cout<<"4"<<std::endl;
  if ((time_start>end_time)||(time_stop>end_time)) SEQUENCER_ERRORN(SEQUENCER_ERROR_AFTERTIMEEND_NUM, "fastSequencePlotter.set_analog(\""+channel+"\", "+floattostr(time_start)+", "+floattostr(time_stop)+", "+floattostr(value)+")");
  if (get_dataformat(channel)!=cmAnalogOutput) SEQUENCER_ERRORN(SEQUENCER_ERROR_NOTANALOG_NUM, "fastSequencePlotter.set_analog(\""+channel+"\", "+floattostr(time_start)+", "+floattostr(time_stop)+", "+floattostr(value)+")");
  int k=channels[channel].data;
  fsp_segment s;
  s.type=fspConstant;
  s.tstart=time_start;
  s.tstop=time_stop;
  s.dvalue=value;
  //std::cout<<"set_analog(\""+channel+"\", "+floattostr(time_start)+", "+floattostr(time_stop)+", "+floattostr(value)+")     k="<<k<<std::endl;
  data[k].list.push_back(s);
};

void fastSequencePlotter::set_linear(std::string channel, double time_start, double time_stop,  double startvalue, double endvalue){
  //std::cout<<"3"<<std::endl;
  if ((time_start>end_time)||(time_stop>end_time)) SEQUENCER_ERRORN(SEQUENCER_ERROR_AFTERTIMEEND_NUM, "fastSequencePlotter.set_linear(\""+channel+"\", "+floattostr(time_start)+", "+floattostr(time_stop)+", "+floattostr(endvalue)+")");
  if (get_dataformat(channel)!=cmAnalogOutput) SEQUENCER_ERRORN(SEQUENCER_ERROR_NOTANALOG_NUM, "fastSequencePlotter.set_linear(\""+channel+"\", "+floattostr(time_start)+", "+floattostr(time_stop)+", "+floattostr(endvalue)+")");
  int k=channels[channel].data;
  //std::cout<<"set_linear(\""+channel+"\", "+floattostr(time_start)+", "+floattostr(time_stop)+", "+floattostr(endvalue)+")     k="<<k<<std::endl;
  fsp_segment s;
  s.type=fspLinear;
  s.tstart=time_start;
  s.tstop=time_stop;
  s.vstart=startvalue;
  s.vstop=endvalue;
  data[k].list.push_back(s);
};

void fastSequencePlotter::set_digital(std::string channel, double time_start, double time_stop, bool value){
  //std::cout<<"2"<<std::endl;
  if ((time_start>end_time)||(time_stop>end_time)) SEQUENCER_ERRORN(SEQUENCER_ERROR_AFTERTIMEEND_NUM, "fastSequencePlotter.set_digital(\""+channel+"\", "+floattostr(time_start)+", "+floattostr(time_stop)+", "+booltostr(value)+")");
  if (get_dataformat(channel)!=cmDigitalOutput)
    SEQUENCER_ERRORN(SEQUENCER_ERROR_NOTDIGITAL_NUM, "fastSequencePlotter.set_digital(\""+channel+"\", "+floattostr(time_start)+", "+floattostr(time_stop)+", "+booltostr(value)+")");
  int k=channels[channel].data;
  //std::cout<<"set_digital(\""+channel+"\", "+floattostr(time_start)+", "+floattostr(time_stop)+", "+booltostr(value)+")     k="<<k<<std::endl;
  fsp_segment s;
  s.type=fspConstant;
  s.tstart=time_start;
  s.tstop=time_stop;
  s.bvalue=value;
  data[k].list.push_back(s);
};

void fastSequencePlotter::set_digital(std::string channel, double timestep, bool value){
  //std::cout<<"1"<<std::endl;
  if (timestep>end_time) SEQUENCER_ERRORN(SEQUENCER_ERROR_AFTERTIMEEND_NUM, "fastSequencePlotter.set_digital(\""+channel+"\", "+floattostr(timestep)+", "+booltostr(value)+")");
  if (get_dataformat(channel)!=cmDigitalOutput)
    SEQUENCER_ERRORN(SEQUENCER_ERROR_NOTDIGITAL_NUM, "fastSequencePlotter.set_digital(\""+channel+"\", "+floattostr(timestep)+", "+booltostr(value)+")");
  fsp_segment s;
  int k=channels[channel].data;
  //std::cout<<"set_digital(\""+channel+"\", "+floattostr(timestep)+", "+booltostr(value)+")     k="<<k<<std::endl;
  s.type=fspConstant;
  s.tstart=timestep;
  s.tstop=timestep+sample_timestep;
  s.bvalue=value;
  data[k].list.push_back(s);
};



void fastSequencePlotter::set_linear(std::string channel, double time_start, double time_stop,double endvalue){
   set_linear(channel, time_start, time_stop, get_analog(channel, time_start), endvalue);
};

int fastSequencePlotter::get_channel_num(std::string channel) {
  if (data.size()>0) for (int i=0; i<data.size(); i++) {
    if (data[i].name==channel) return i;
  }
  return -1;
}

double fastSequencePlotter::get_analog(std::string channel, double timestep){
  if (timestep>end_time) SEQUENCER_ERRORN(SEQUENCER_ERROR_AFTERTIMEEND_NUM, "fastSequencePlotter.get_analog(\""+channel+"\", "+floattostr(timestep)+")");
  if (get_dataformat(channel)!=cmAnalogOutput) SEQUENCER_ERRORN(SEQUENCER_ERROR_NOTANALOG_NUM, "fastSequencePlotter.get_analog(\""+channel+"\", "+floattostr(timestep)+")");
  int n=get_channel_num(channel);
  sort_data_bytime(n);
  for (int k=0; k<data[n].list.size(); k++) {
    if (data[n].list[k].type==fspLinear) {
      if ((timestep>=data[n].list[k].tstart)&&(timestep<=data[n].list[k].tstop)) {
        double twidth=data[n].list[k].tstop-data[n].list[k].tstart;
        double vwidth=data[n].list[k].vstop-data[n].list[k].vstart;
        double st=vwidth/twidth;
        return data[n].list[k].vstart+st*(timestep-data[n].list[k].tstart);
      }
      if (timestep<data[n].list[k].tstart) { // list is sorted => last item was no hit, so we are in a gap
        return data[n].list[k].vstart;
      }
    }
    if (data[n].list[k].type==fspConstant) {
      if ((timestep>=data[n].list[k].tstart)&&(timestep<=data[n].list[k].tstop)) return data[n].list[k].dvalue;
      if (timestep<data[n].list[k].tstart) { // list is sorted => last item was no hit, so we are in a gap
        return data[n].list[k].dvalue;
      }
    }
  }
  return 0;
};

/** \brief returns the digital value at the specified time from the given channel */
bool fastSequencePlotter::get_digital(std::string channel, double timestep){
  if (timestep>end_time) SEQUENCER_ERRORN(SEQUENCER_ERROR_AFTERTIMEEND_NUM, "fastSequencePlotter.get_digital(\""+channel+"\", "+floattostr(timestep)+")");
  if (get_dataformat(channel)!=cmDigitalOutput)
    SEQUENCER_ERRORN(SEQUENCER_ERROR_NOTDIGITAL_NUM, "fastSequencePlotter.get_digital(\""+channel+"\", "+floattostr(timestep)+")");
  int n=get_channel_num(channel);
  sort_data_bytime(n);
  for (int k=0; k<data[n].list.size(); k++) {
    if (data[n].list[k].type==fspSet1) {
      if (timestep<=data[n].list[k].tstart) {
        return true;
      }
    }
    if (data[n].list[k].type==fspSet0) {
      if (timestep<=data[n].list[k].tstart) {
        return false;
      }
    }
    if (data[n].list[k].type==fspConstant) {
      if ((timestep>=data[n].list[k].tstart)&&(timestep<=data[n].list[k].tstop)) return data[n].list[k].bvalue;
      if (timestep<data[n].list[k].tstart) { // list is sorted => last item was no hit, so we are in a gap
        return data[n].list[k].bvalue;
      }
    }
  }
  return false;
};

}
