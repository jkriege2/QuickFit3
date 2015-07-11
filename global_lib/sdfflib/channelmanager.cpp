/*
  Name: channelmanager.cpp
  Copyright: (c) 2007
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/

#include "channelmanager.h" // class's header file

namespace sequencer {


std::string cm_types2str(cm_types type){
    switch (type) {
      case cmAnalogOutput:
           return "analog output";
           break;
      case cmAnalogInputSE:
           return "analog input, single-ended";
           break;
      case cmAnalogInputDE:
           return "analog input, differential-ended";
           break;
      case cmDigitalOutput:
           return "digital output";
           break;
      case cmDigitalInput:
           return "digital input";
           break;
      default:
        return "unknown";
    }
};

std::string cm_roles2str(cm_roles type){
    switch (type) {
      case cmStandardChannel:
           return "standard";
           break;
      case cmVarTimingChannel:
           return "variable timing";
           break;
      case cmUnsynchronizedChannel:
           return "unsynchronized";
           break;
      default:
        return "unknown";
    }
};

std::string cm_types2shortstr(cm_types type){
    switch (type) {
      case cmAnalogOutput:
           return "AO";
           break;
      case cmAnalogInputSE:
           return "AISE";
           break;
      case cmAnalogInputDE:
           return "AIDE";
           break;
      case cmDigitalOutput:
           return "DO";
           break;
      case cmDigitalInput:
           return "DI";
           break;
      default:
        return "?";
    }
};

cm_types str2cm_types(std::string type){
  std::string t=tolower(type);
  if ((t=="analogoutput")||(t=="ao")) return cmAnalogOutput;
  if ((t=="analoginputse")||(t=="aise")) return cmAnalogInputSE;
  if ((t=="analoginputde")||(t=="aide")) return cmAnalogInputDE;
  if ((t=="digitaloutput")||(t=="do")) return cmDigitalOutput;
  if ((t=="digitalinput")||(t=="di")) return cmDigitalInput;
  return cmAnalogInputSE;
};

cm_roles str2cm_roles(std::string type){
  std::string t=tolower(type);
  if ((t=="standard")||(t=="std")) return cmStandardChannel;
  if ((t=="vartiming")||(t=="vart")) return cmVarTimingChannel;
  if ((t=="unsynchronized")||(t=="unsynchronised")||(t=="asynch")) return cmUnsynchronizedChannel;
  return cmStandardChannel;
};



// class constructor
channelManager::channelManager()
{
	// insert your code here
}

// class destructor
channelManager::~channelManager()
{
	clear();
}


void channelManager::clear() {
     channels.clear();
     echannels.clear();
};

void channelManager::add(cm_channel data){
  if(exists(data.name)) {
    SEQUENCER_ERRORN(SEQUENCER_ERROR_CEXIST_NUM, "channelManager.add(\""+data.name+"\")");
  } else {
    channels[data.name]=data;
  }
};

void channelManager::add(cm_types type, std::string name, std::string device_name, std::string port_name, std::string pin_name, cm_roles role, double scale, double offset, bool invert, double minimum, double maximum, double valminimum, double valmaximum, bool required){
   cm_channel data;
   data.type=type;
   data.role=role;
   data.name=name;
   data.device_name=device_name;
   data.port_name=port_name;
   data.pin_name=pin_name;
   data.scale=scale;
   data.offset=offset;
   data.min=minimum;
   data.max=maximum;
   data.valmin=valminimum;
   data.valmax=valmaximum;
   data.invert=invert;
   data.required=required;
   add(data);
};

size_t channelManager::get_channel_count(){
  size_t cnt=channels.size();

  // after reading the channel count in channels we have to check for each channel in echannels whether it
  // already exists in channels (then we have already counted it). If it does not we have to increase the
  // result by one.
  if (echannels.size()>0) {
    for(std::map<std::string, cm_channel>::const_iterator i=echannels.begin(); i!=echannels.end(); ++i) {
      if (channels.find(i->first)==channels.end()) cnt++;
    }
  }
  return cnt;
};

void channelManager::remove(std::string channel){
  if(!exists(channel)) {
    SEQUENCER_ERRORN(SEQUENCER_ERROR_CNOTEXIST_NUM, "channelManager.remove(\""+channel+"\")");
  } else {
    // delete in channels and echannels!
    if (channels.find(channel)!=channels.end()) channels.erase(channels.find(channel));
    if (echannels.find(channel)!=echannels.end()) echannels.erase(echannels.find(channel));
  }
};

bool channelManager::exists(std::string channel){
  return (channels.find(channel)!=channels.end()) || (echannels.find(channel)!=echannels.end());
};

cm_channel& channelManager::get(std::string channel){
  if (!exists(channel)) {
    SEQUENCER_ERRORN(SEQUENCER_ERROR_CNOTEXIST_NUM, "channelManager.get(\""+channel+"\")");
  } else {
    std::map<std::string, cm_channel>::const_iterator i=echannels.find(channel);
    if (i!=echannels.end()) return echannels[channel];
    return channels[channel];
  }
};

cm_channel& channelManager::get(size_t channel){
  if (channel>=get_channel_count()) {
    SEQUENCER_ERRORN(SEQUENCER_ERROR_RANGE_NUM, "channelManager["+inttostr(channel)+"]");
  } else {
    std::map<std::string, cm_channel>::const_iterator i=channels.begin();

    // the lower numbers indicate channels in channels map
    if (channel<channels.size()) {
      if (channel>0) for (size_t j=0; j<channel; j++) ++i;
      if (echannels.find(i->first)==echannels.end()) {
        return channels[i->first];
      } else {
        return echannels[i->first];
      }
    }
    // the higher (>=channels.size()) reference channels in echannels map.
    // so we have to go through all channels that do NOT have a counterpart in channels
    // and return the (channel-channels.size())-th one from them!
    size_t indx=channels.size();
    for(i=echannels.begin(); i!=echannels.end(); ++i) {
      if (channels.find(i->first)==channels.end()) { // we did not find this channel from echannels in channels
                                                     // so this is a candidate for a channel that is in echannels
                                                     // and not in channels ... if it is the correct one (indx gives
                                                     // the virtual index of the current channel in echannels) we
                                                     // return it otherwise we increase indx.
        if (channel==indx) return echannels[i->first];
        indx++;
      }
    }

  }
  // if we did not fin the channel we throw an exception!
  SEQUENCER_ERRORN(SEQUENCER_ERROR_RANGE_NUM, "channelManager["+inttostr(channel)+"]");
};

/*std::string channelManager::get_physical_name(std::string channel){
  if (!exists(channel)) {
    SEQUENCER_ERROR(SEQUENCER_ERROR_CNOTEXIST_NUM, SEQUENCER_ERROR_CNOTEXIST_MESSAGE, "channelManager.get_physical_name(\""+channel+"\")");
  } else  {
    cm_channel c=channels[channel];
    return c.device_name+c.port_name+c.pin_name;
  }
};*/

void channelManager::set(std::string channel, cm_channel data){
  if (!exists(channel)) {
    SEQUENCER_ERRORN(SEQUENCER_ERROR_CNOTEXIST_NUM, "channelManager.set(\""+channel+"\")");
  } else {
    std::map<std::string, cm_channel>::const_iterator i=echannels.find(channel);
    if (i!=echannels.end()) {
      echannels[channel]=data;
    } else {
      channels[channel]=data;
    }
  }
};

cm_channel& channelManager::operator[](std::string channel){
  return get(channel);
};

cm_channel& channelManager::operator[](size_t channel){
  return get(channel);
};

std::string channelManager::as_string(){
  std::string text;
  if (get_channel_count()>0) {
    for(size_t i=0; i<get_channel_count(); i++) {
        cm_channel c=get(i);
        text+="channel \""+c.name+"\" ("+inttostr(i)+")";
        if (echannels.find(c.name)!=echannels.end()) text+=" [experiment specific]";
        text+="\n";
        text+="   type:            "+cm_types2shortstr(c.type)+"  ("+cm_types2str(c.type)+")\n";
        text+="   role:            "+cm_roles2str(c.role)+"\n";
        text+="   device:          "+c.device_name+"\n";
        text+="   port:            "+c.port_name+"\n";
        text+="   pin:             "+c.pin_name+"\n";
        text+="   min..max:        "+floattostr(c.min)+".."+floattostr(c.max)+"\n";
        if ((c.type==cmDigitalOutput)||(c.type==cmDigitalInput)) {
          text+="   invert:          "+booltostr(c.invert)+"\n";
        } else {
          text+="   valmin..valmax:  "+floattostr(c.valmin)+".."+floattostr(c.valmax)+"\n";
          text+="   scale, offset:   "+floattostr(c.scale)+", "+floattostr(c.offset)+"\n";
        }
        text+="   required:        "+booltostr(c.required)+"\n";
        text+="\n";
    }
  }
  return text;
};

void channelManager::read_ini(std::string filename){
  jkiniparser ini;
  try {
     ini.readFile(filename);
  } catch (jkipError e) {
     SEQUENCER_ERRORN(SEQUENCER_ERROR_INI_NUM, e.getMessage());
  }
  clear();
  if (ini.getGroupCount()>0) {
    for (size_t i=0; i<ini.getGroupCount(); i++) {
      std::string g=ini.getGroupName(i);
      cm_channel c;
      c.name=g;
      c.device_name=ini.getAsString(g, "device_name", ini.getAsString(g, "device"));
      c.port_name=ini.getAsString(g, "port_name", ini.getAsString(g, "port"));
      c.pin_name=ini.getAsString(g, "pin_name", ini.getAsString(g, "pin"));
      c.type=str2cm_types(ini.getAsString(g, "type"));
      c.role=str2cm_roles(ini.getAsString(g, "role", "std"));
      c.min=ini.getAsDouble(g, "min", -10);
      c.max=ini.getAsDouble(g, "max", 10);
      c.invert=ini.getAsBool(g, "invert", false);
      c.required=ini.getAsBool(g, "required", false);
      //c.synchronized=ini.getAsBool(g, "synchronized", ini.getAsBool(g, "synchronised", true));
      c.scale=ini.getAsDouble(g, "scale", 1);
      c.offset=ini.getAsDouble(g, "offset", 0);
      c.valmin=ini.getAsDouble(g, "valmin", mmin(c.scale*c.min+c.offset, c.scale*c.max+c.offset));
      c.valmax=ini.getAsDouble(g, "valmax", mmax(c.scale*c.min+c.offset, c.scale*c.max+c.offset));
      if ((c.type==cmDigitalOutput)||(c.type==cmDigitalInput)) {
        c.min=0;
        c.max=1;
        c.valmin=0;
        c.valmax=1;
      }
      add(c);
    }
  }
};

void channelManager::clear_experiment_ini(){
  echannels.clear();
}


void channelManager::read_experiment_ini(std::string filename){
  jkiniparser ini;
  try {
     ini.readFile(filename);
  } catch (jkipError e) {
     SEQUENCER_ERRORN(SEQUENCER_ERROR_INI_NUM, e.getMessage());
  }
  echannels.clear();
  if (ini.getGroupCount()>0) {
    for (size_t i=0; i<ini.getGroupCount(); i++) {
      std::string g=ini.getGroupName(i);
      bool cexists=(channels.find(g)!=channels.end());
      cm_channel c;
      if (cexists) c=get(g);
      c.name=g;
      // only load properties that are really in the file. All othe rproperties are copied from
      // the ones on channels! If the chanel does not yet exist, all properties are read, as the
      // read operations also set the default values.
      if (ini.exists(g, "device_name")|| !cexists) c.device_name=ini.getAsString(g, "device_name");
      if (ini.exists(g, "device")|| !cexists) c.device_name=ini.getAsString(g, "device");
      if (ini.exists(g, "port_name")|| !cexists) c.device_name=ini.getAsString(g, "port_name");
      if (ini.exists(g, "port")|| !cexists) c.device_name=ini.getAsString(g, "port");
      if (ini.exists(g, "pin_name")|| !cexists) c.device_name=ini.getAsString(g, "pin_name");
      if (ini.exists(g, "pin")|| !cexists) c.device_name=ini.getAsString(g, "pin");
      if (ini.exists(g, "type")|| !cexists) c.type=str2cm_types(ini.getAsString(g, "type"));
      if (ini.exists(g, "role")|| !cexists) c.role=str2cm_roles(ini.getAsString(g, "role"));
      if (ini.exists(g, "min")|| !cexists) c.min=ini.getAsDouble(g, "min", -10);
      if (ini.exists(g, "max")|| !cexists) c.max=ini.getAsDouble(g, "max", 10);
      if (ini.exists(g, "invert")|| !cexists) c.invert=ini.getAsBool(g, "invert", false);
      if (ini.exists(g, "required")|| !cexists) c.required=ini.getAsBool(g, "required", false);
      //if (ini.exists(g, "synchronised")|| !cexists) c.synchronized=ini.getAsBool(g, "synchronised", true);
      //if (ini.exists(g, "synchronized")|| !cexists) c.synchronized=ini.getAsBool(g, "synchronized", true);
      if (ini.exists(g, "scale")|| !cexists) c.scale=ini.getAsDouble(g, "scale", 1);
      if (ini.exists(g, "offset")|| !cexists) c.offset=ini.getAsDouble(g, "offset", 0);
      if (ini.exists(g, "valmin")|| !cexists) c.valmin=ini.getAsDouble(g, "valmin", mmin(c.scale*c.min+c.offset, c.scale*c.max+c.offset));
      if (ini.exists(g, "valmax")|| !cexists) c.valmax=ini.getAsDouble(g, "valmax", mmax(c.scale*c.min+c.offset, c.scale*c.max+c.offset));
      if ((c.type==cmDigitalOutput)||(c.type==cmDigitalInput)) {
        c.min=0;
        c.max=1;
        c.valmin=0;
        c.valmax=1;
      }
      echannels[c.name]=c;
    }
  }
};

double channelManager::scale_value(std::string channel, double value){
  cm_channel c=get(channel);
  //std::cout<<"scaling channel "<<channel<<"   scale="<<c.scale<<",   offset="<<c.offset<<",   value="<<value<<std::endl;
  return c.scale*value+c.offset;
}

double channelManager::scale_value(size_t channel, double value){
  cm_channel c=get(channel);
  return c.scale*value+c.offset;
}

double channelManager::invscale_value(std::string channel, double value){
  cm_channel c=get(channel);
  return (value-c.offset)/c.scale;
}

double channelManager::invscale_value(size_t channel, double value){
  cm_channel c=get(channel);
  return (value-c.offset)/c.scale;
}

bool channelManager::invert_value(std::string channel, bool value) {
  cm_channel c=get(channel);
  return c.invert?(!value):value;
}

bool channelManager::contains_required(){
  if (get_channel_count()>0) {
    for(size_t i=0; i<get_channel_count(); i++) {
        cm_channel c=get(i);
        if (c.required) return true;
    }
  }
  return false;
}

}
