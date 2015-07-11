
#include "sequencegenerator.h" 


namespace sequencer {

void sequenceGenerator::set_linear(std::string channel, double time_start, double time_stop,double endvalue){
     set_linear(channel, time_start, time_stop, get_analog(channel, time_start), endvalue);
};

std::string sequenceGenerator::report(){
  std::string res;
  res+="end_time        = "+floattostr(end_time)+ "micros\n";
  res+="sample_timestep = "+floattostr(sample_timestep)+ "micros\n";
  res+="samples         = "+floattostr(end_time/sample_timestep)+ "\n";
  return res;           
};

void sequenceGenerator::init(channelManager* cm, double sample_timestep, double end_time){
    cmanager=cm;
    this->sample_timestep=sample_timestep;
    this->end_time=end_time;
}


}
