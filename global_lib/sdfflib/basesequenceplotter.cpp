
#include "basesequenceplotter.h" // class's header file


namespace sequencer {

// class constructor
baseSequencePlotter::baseSequencePlotter():
    sequenceGenerator()
{
  /* copied from NIsequenceGenerator --------------------------------*/
	sample_timestep=5; // 5 µs
	end_time=5000; // 5 ms  => 1000 samples per channel
  /* ----------------------------------------------------------------*/

  plotchannels.clear();
  clear_markers();
}

// class destructor
baseSequencePlotter::~baseSequencePlotter()
{
}


void baseSequencePlotter::sort_channels(){
  if (chnum>0) {
    // save all channel names in channames vector of strings
    if (plotchannels.size()>0) {
      for (unsigned long i=0; i<(unsigned long)chnum; i++) {
        channames.push_back(plotchannels[i]);
      }
    } else {
      for (unsigned long i=0; i<(unsigned long)chnum; i++) {
        channames.push_back(get_name(i));
      }
    }
  }
  if (chnum>1) {
    // sort using bubble sort (see http://de.wikipedia.org/wiki/Bubble_Sort#Formaler_Algorithmus),
    bool swapped=true;
    do {
        swapped = false;
        for (size_t i=0; i<=channames.size()-2; i++) {
          if (channames[i] > channames[i+1]) {
              // swap array items:
              std::string temp=channames[i];
              channames[i]=channames[i+1];
              channames[i+1]=temp;
              swapped = true;
          }
        }
    } while (swapped);
  }
}

void baseSequencePlotter::sel_add_channel(std::string name){
  // only add if name is not yet in the list
  bool found=false;
  if (plotchannels.size()>0) {
    for (size_t i=0; i<plotchannels.size(); i++) {
      if (plotchannels[i]==name) {
        found=true;
        break;
      }
    }
  }
  if (!found)
    plotchannels.push_back(name);
}

void baseSequencePlotter::sel_clear_channels(){
  plotchannels.clear();
}


void baseSequencePlotter::add_marker(double t, sp_marker_type typ, std::string txt){
  sp_marker m;
  m.time=t;
  m.text=txt;
  m.type=typ;
  markers.push_back(m);
}

void baseSequencePlotter::add_marker(sp_marker marker){
  markers.push_back(marker);
};

void baseSequencePlotter::clear_markers(){
  markers.clear();
}

std::string baseSequencePlotter::get_name(unsigned long num){
  if (num>=get_channel_count()) {
    SEQUENCER_ERRORN(SEQUENCER_ERROR_RANGE_NUM, "baseSequencePlotter.get_name("+inttostr(num)+")");
  } else {
    std::map<std::string, sg_channel>::const_iterator i=channels.begin();
    if (num>0) for (unsigned long j=0; j<num; j++) ++i;
    return channels[i->first].name;
  }
};

std::string baseSequencePlotter::get_hardware_adress(cm_channel channel){
  std::string res=channel.device_name;
  if (res.size()>0) res+=NISEQGEN_DEVICE_PORT_DIVIDER;
  res+=channel.port_name;
  if (channel.port_name.size()>0) res+=NISEQGEN_PORT_PIN_DIVIDER;
  res+=channel.pin_name;
  return res;
};
/* ------------------------------------------------------------------------------------------------------------------------ */
}
