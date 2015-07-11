/*
  Name: sdffsequenceelements.cpp
  Copyright: (c) 2008
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/

#include "sdffsequenceelements.h" // class's header file
#include "sdffparser.h"


namespace sequencer {



/* =======================================================================================================================
 * = SDFFsequenceElement                                                                                                 =
 * =======================================================================================================================*/

double SDFFsequenceElement::extract_real_t_start(){
  double ts=emanager->get_sample_timestep();
  return round(t_start/ts)*ts; //ceil
};

double SDFFsequenceElement::extract_real_t_end(){
  double ts=emanager->get_sample_timestep();
  return round((t_start+t_duration)/ts)*ts;//floor
};

bool SDFFsequenceElement::check_name(SDFFparser* parser, channelManager* cmanager) {
  // here we check the name of the channel
  if (!cmanager->exists(channel)) {
    parser->add_error(SEQUENCER_SDFFERROR_CNOTEXISTS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_CNOTEXISTS_NUM), channel.c_str()), format(get_errorsmessage(SEQUENCER_SDFFERROR_CNOTEXISTS_NUM), channel.c_str()), filename, extract_real_t_start(), line);
    return false;
  }
  cm_channel c=cmanager->get(channel);
  if ((c.type!=cmAnalogOutput)&&(c.type!=cmDigitalOutput)) {
    parser->add_error(SEQUENCER_SDFFERROR_CNOTEXISTS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_CNOTOUTPUT_NUM), channel.c_str()), format(get_errorsmessage(SEQUENCER_SDFFERROR_CNOTOUTPUT_NUM), channel.c_str()), filename, extract_real_t_start(), line);
    return false;
  }
  if (c.role==cmUnsynchronizedChannel) {
    parser->add_warning(SEQUENCER_ERROR_CNOTSYNCH_NUM, format(get_errormessage(SEQUENCER_ERROR_CNOTSYNCH_NUM), channel.c_str()), format(get_errorsmessage(SEQUENCER_ERROR_CNOTSYNCH_NUM), channel.c_str()), filename, extract_real_t_start(), line);
  }
  digital=(c.type==cmDigitalOutput);
  return true;
};



/* =======================================================================================================================
 * = SDFFSEset                                                                                                           =
 * =======================================================================================================================*/
void SDFFSEset::generate() {
  //sequenceGenerator* sg=emanager->get_generator();
  //channelManager* cmanager=sg->get_cmanager();
  if (digital) {
    //std::cout<<" set: set_digital t0="<<extract_real_t_start()<<"  v="<<value<<"\n";
    //sg->set_digital(channel, extract_real_t_start(), extract_real_t_start(), cmanager->invert_value(i->first, value>0.2));
  } else {
    //std::cout<<" set:set_analog t0="<<extract_real_t_start()<<"  v="<<value<<"\n";
    //sg->set_analog(channel, extract_real_t_start(), extract_real_t_start(), cmanager->scale_value(i->first, value));
  }
}

void SDFFSEset::check() {
  SDFFparser* parser=emanager->get_parser();
  channelManager* cmanager=parser->get_cmanager();
  if (check_name(parser, cmanager) ) {
  }
};

double SDFFSEset::get_endvalue(){
  //std::cout<<"       get_endvalue set: "<<extract_real_t_start()<<std::endl;
  return value;
};

double SDFFSEset::get_startvalue(){
  if (previous==NULL) return 0;
  else return previous->get_endvalue();
};



/* =======================================================================================================================
 * = SDFFSEramp                                                                                                          =
 * =======================================================================================================================*/
void SDFFSEramp::generate() {
  sequenceGenerator* sg=emanager->get_generator();
  channelManager* cmanager=sg->get_cmanager();
  switch(type) {
    case sdffRTLinear:
      sg->set_linear(channel, extract_real_t_start(), extract_real_t_end(), cmanager->scale_value(channel, get_startvalue()), cmanager->scale_value(channel, end_value));
      break;
    case sdffRTCosine: {
      double f=1.0/(2.0*t_duration);
      double x0=get_startvalue();
      double t0=extract_real_t_start();
      double t1=extract_real_t_end();
      double A=x0-end_value;
      double dt=emanager->get_sample_timestep();
      double r0=x0;
      for (double t=t0; t<=t1; t+=dt) {
        double r1=A/2.0*(cos(2.0*M_PI*f*(t-t0))+1)+end_value;
        sg->set_linear(channel, t-dt, t, cmanager->scale_value(channel, r0), cmanager->scale_value(channel, r1));
        r0=r1;
      }
      break; }
    case sdffRTExponential: {
      double x0=get_startvalue();
      double t0=extract_real_t_start();
      double t1=extract_real_t_end();
      double A=(x0-end_value)/(exp(-t0/tau)-exp(-t1/tau));
      double A0=A*exp(-t0/tau)-x0;
      double dt=emanager->get_sample_timestep();
      double r0=x0;
      for (double t=t0; t<=t1; t+=dt) {
        double r1=A*exp(-t/tau)-A0;
        sg->set_linear(channel, t-dt, t, cmanager->scale_value(channel, r0), cmanager->scale_value(channel, r1));
        r0=r1;
      }
      break; }
    case sdffRTSigmoidal: {
      double x0=get_startvalue();
      double x1=end_value;
      double t0=extract_real_t_start();
      double t1=extract_real_t_end();
      double ts=10;
      double alpha=2.0*ts/(t1-t0);
      double f1=(x1-x0)/(sig(ts, tau)-sig(-1.0*ts, tau));
      double f0=x0-f1*sig(-1.0*ts, tau);
      double dt=emanager->get_sample_timestep();
      double r0=x0;
      for (double t=t0; t<=t1; t+=dt) {
        double r1=f0+f1*sig(alpha*(t-t0)-ts, tau);
        sg->set_linear(channel, t-dt, t, cmanager->scale_value(channel, r0), cmanager->scale_value(channel, r1));
        r0=r1;
      }
      break; }
  }
}

void SDFFSEramp::check() {
  SDFFparser* parser=emanager->get_parser();
  channelManager* cmanager=parser->get_cmanager();
  if (check_name(parser, cmanager) ) {
    /*if ((type==sdffRTExponential)&&(tau<=0)) {
      // for exponential ramps we have to check whether tau is negative or zero,
      // as this does not make any sense:
      parser->add_error(SEQUENCER_SDFFERROR_TAUNOTNEG_NUM, filename, extract_real_t_start(), line);
    }*/
    if (digital) {
      // ramps are only possible for analog channels !
      parser->add_error(SEQUENCER_SDFFERROR_NODIGRAMP_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_NODIGRAMP_NUM), channel.c_str()), get_errorsmessage(SEQUENCER_SDFFERROR_NODIGRAMP_NUM), filename, extract_real_t_start(), line);
    }
  }
};

double SDFFSEramp::get_endvalue(){
  //std::cout<<"       get_endvalue ramp: "<<extract_real_t_start()<<std::endl;
  return end_value;
};

double SDFFSEramp::get_startvalue(){
  if (previous==NULL) return 0; // standard initial value: 0
  else return previous->get_endvalue();
};








/* =======================================================================================================================
 * = SDFFSEpulse                                                                                                         =
 * =======================================================================================================================*/
void SDFFSEpulse::generate() {
  sequenceGenerator* sg=emanager->get_generator();
  channelManager* cmanager=sg->get_cmanager();
  if (digital) {
    //std::cout<<" pulse:set_digital t0="<<extract_real_t_start()<<"  t1="<<extract_real_t_end()-sg->get_sample_timestep()<<"  v="<<value<<"\n";
    sg->set_digital(channel, extract_real_t_start(), extract_real_t_end()/*-sg->get_sample_timestep()*/, cmanager->invert_value(channel, value>0.2));
  } else {
    //std::cout<<" pulse:set_analog t0="<<extract_real_t_start()<<"  t1="<<extract_real_t_end()-sg->get_sample_timestep()<<"  v="<<value<<"\n";
    sg->set_analog(channel, extract_real_t_start(), extract_real_t_end()/*-sg->get_sample_timestep()*/, cmanager->scale_value(channel, value));
  }
}

void SDFFSEpulse::check() {
  SDFFparser* parser=emanager->get_parser();
  channelManager* cmanager=parser->get_cmanager();
  if (check_name(parser, cmanager) ) {
  }
};

double SDFFSEpulse::get_endvalue(){
  //std::cout<<"       get_endvalue pulse: "<<extract_real_t_start()<<std::endl;
  if (previous==NULL) return 0; // standard initial value: 0
  else return previous->get_endvalue();
};

double SDFFSEpulse::get_startvalue(){
  if (previous==NULL) return 0; // standard initial value: 0
  else return previous->get_endvalue();
};





/* =======================================================================================================================
 * = SDFFelementManager                                                                                                  =
 * =======================================================================================================================*/
SDFFelementManager::SDFFelementManager(SDFFparser* parser, sequenceGenerator* generator, double sample_timestep) {
  this->parser=parser;
  if (generator==NULL) {
    this->generator=parser->get_generator();
  } else {
    this->generator=generator;
  }
  if (sample_timestep!=-1) {
    this->sample_timestep=sample_timestep;
  } else {
    this->sample_timestep= generator->get_sample_timestep();
  }
  end_time=-1;
  use_end_time=0;
  clear();
};

void SDFFelementManager::add(std::string channel, SDFFsequenceElement* element){
  elements[channel].push_back(element);
  if (elements[channel].size()>1) { // care for the next and previous pointers !!!
    // e[n-1]=element
    // e[n-2].next=e[n-1]=element
    // e[n-1].previous=e[n-2]
    elements[channel][elements[channel].size()-2]->set_next(element);
    element->set_previous(elements[channel][elements[channel].size()-2]);
    //std::cout<<"added: t0="<<element->extract_real_t_start()<<"    t1="<<element->extract_real_t_end()<<"     l."<<element->get_line()<<std::endl;
  }
};


void SDFFelementManager::sort(){
  std::map<std::string, std::vector<SDFFsequenceElement*> >::const_iterator i;
  // go through all channels and sort the elements associated to each channel according
  // to their start_time parameter.
  for (i=elements.begin(); i!=elements.end(); ++i) {
    if (i->second.size()>1) { // sort makes sense only, if there are at least 2 items in a,
                              // but writeback has to be done even for one item in a!
        // sort the vector using bubble sort (stop sorting when there are no more
        // swaps -> O(N) on sorted array, instead of O(N^2))
        bool swapped=true;
        do {
            swapped = false;
            for (size_t j=0; j<=i->second.size()-2; j++) {
              if (i->second[j]->get_t_start() > i->second[j+1]->get_t_start()) {
                // swap array items:
                //SDFFsequenceElement* e0=i->second[j]->get_previous();
                //SDFFsequenceElement* e1=i->second[j];
                //SDFFsequenceElement* e2=i->second[j+1];
                //SDFFsequenceElement* e3=i->second[j]->get_next();
                SDFFsequenceElement* temp=i->second[j];
                elements[i->first][j]=elements[i->first][j+1];
                elements[i->first][j+1]=temp;
                /*if (e0!=NULL) {  // 0->2 ...
                  e0->set_next(e2);
                }
                e2->set_next(e1);// ... 2->1->3
                e1->set_next(e3);
                e2->set_previous(e0); // 0<-2<-1<-3
                e1->set_previous(e2);
                e3->set_previous(e1);*/
                swapped = true;
              }
            }
        } while (swapped);
    }
    // take care of the next and previous pointers
    if (i->second.size()>0) {
      SDFFsequenceElement* p=NULL;
      for (size_t j=0; j<i->second.size(); j++) {
        elements[i->first][j]->set_previous(p);
        p=elements[i->first][j];
        if (j==i->second.size()-1) elements[i->first][j]->set_next(NULL);
        else elements[i->first][j]->set_next(elements[i->first][j+1]);
      }
    }
  }
};

void SDFFelementManager::clear(bool delete_elements) {
  if (delete_elements) {
    std::map<std::string, std::vector<SDFFsequenceElement*> >::const_iterator i;
    for (i=elements.begin(); i!=elements.end(); ++i) {
      if (i->second.size()>0) {
        for (size_t j=0; j<i->second.size(); j++) {
          delete i->second[j];
        }
      }
    }
  }
  elements.clear();
};


void SDFFelementManager::generate(){
  channelManager* cmanager=parser->get_cmanager();

  sort(); // sort the elements according to time

  // calculate the duration of the sequence
  if (end_time<0) {
    use_end_time=extract_end_time();
  } else {
    use_end_time=end_time;
  }

  // initialize the sequence Generator and add all used channels
  std::map<std::string, std::vector<SDFFsequenceElement*> >::const_iterator i;
  for (i=elements.begin(); i!=elements.end(); ++i) {
    generator->add_used(i->first);
    //std::cout<<i->first<<" added\n";
  }
  generator->init(cmanager, sample_timestep, use_end_time);
  //std::cout<<generator->report()<<std::endl;
  //std::cout<<"going through items ..."<<std::endl;
  // go through all elements and call their generate() function
  for (i=elements.begin(); i!=elements.end(); ++i) {
    double vstart=0; // contains the value of the channel at t=0
    double t0=0; // end time of last instruction
    double v0=vstart; // last value of last instruction ... initial value befor first instruction
    cm_channel ch=cmanager->get(i->first); // description of the current channel
    //std::cout<<std::endl<<i->first<<":  \n";
    if (i->second.size()>0) {
      if (i->second[0]->extract_real_t_start()==0) v0=i->second[0]->get_startvalue(); // if the first element starts at t=0
                                                                                    // we have to get the start value from there
      for (size_t j=0; j<i->second.size(); j++) {
        //std::cout<<j;
        double t1=i->second[j]->extract_real_t_start(); // start time of current instruction

        // if there is a gap between the last and the current instruction, we have to fill it:
        if (t1>t0) {
          if (ch.type==cmAnalogOutput) { // analog channel
            //std::cout<<" gap_set_analog t0="<<t0<<"  t1="<<t1<<"  v0="<<v0<<"\n";
            generator->set_analog(i->first, t0, t1, cmanager->scale_value(i->first, v0));
          } else { // digital channel
            //std::cout<<" gap_set_digital t0="<<t0<<"  t1="<<t1<<"  v0="<<v0<<"\n";
            generator->set_digital(i->first, t0, t1, cmanager->invert_value(i->first, v0>0.2));
          }
        }
        // generate the current instruction
        i->second[j]->generate();

        // store end time and end value of current instruction
        t0=i->second[j]->extract_real_t_end();
        v0=i->second[j]->get_endvalue();
      }

      // now we have to check whether there is a gap between the instruction and use_end_time
      if (t0<use_end_time) {
        if (ch.type==cmAnalogOutput) { // analog channel
          //std::cout<<" endgap_set_analog t0="<<t0<<"  t1="<<use_end_time<<"  v0="<<v0<<"\n";
          generator->set_analog(i->first, t0, use_end_time, cmanager->scale_value(i->first, v0));
        } else { // digital channel
          //std::cout<<" endgap_set_digital_end t0="<<t0<<"  t1="<<use_end_time<<"  v0="<<v0<<"\n";
          generator->set_digital(i->first, t0, use_end_time, cmanager->invert_value(i->first, v0>0.2));
        }
      }
    } else {
      // here we set the channel to the standard initial value 0, stored in vstart!
      if (ch.type==cmAnalogOutput) { // analog channel
        generator->set_analog(i->first, 0, use_end_time, cmanager->scale_value(i->first, vstart));
      } else { // digital channel
        generator->set_digital(i->first, 0, use_end_time, cmanager->invert_value(i->first, vstart>0.2));
      }
    }
  }

};


int SDFFelementManager::check(){
  channelManager* cmanager=parser->get_cmanager();

  // sort the sequence
  sort();

  // now check whether we use one of the required fields (if there are)
  if (cmanager->contains_required()) {
    for (size_t j=0; j<cmanager->get_channel_count(); j++) {
      if (cmanager->get(j).required) {
        if (elements.find(cmanager->get(j).name)==elements.end())
          parser->add_error_t(SEQUENCER_CSVERROR_NOREQUIRED_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_NOREQUIRED_NUM), (*cmanager)[j].name.c_str()), format(get_errorsmessage(SEQUENCER_SDFFERROR_NOREQUIRED_NUM), (*cmanager)[j].name.c_str()), parser->get_current_filename(), 0);
      }
    }
  }

  // now go through all channels
  std::map<std::string, std::vector<SDFFsequenceElement*> >::const_iterator i;
  for (i=elements.begin(); i!=elements.end(); ++i) {
    // for each channel we have to check all elements:
    //    1. do they collide with the time 0 (negative times are not allowed)?
    //    2. do two elements collide, i.e. do they intersect?
    //    3. call check of each element This will also check whether the channel exists

    if (i->second.size()==1) {
      // now call the check method
      i->second[0]->check();
      //std::cout<<"checked "<<i->first<<"["<<0<<"]\n";
      // here we only have to check for collision with t=0
      if (i->second[0]->extract_real_t_start()<0) {
        parser->add_error(SEQUENCER_SDFFERROR_NONEGTIME_NUM, i->second[0]->get_filename(), i->second[0]->extract_real_t_start(), i->second[0]->get_line());
      }
      // if end_time>0 we also have to check for collisions with t=end_time
      if (end_time>0) {
        if (i->second[0]->extract_real_t_end()>end_time) {
          parser->add_error(SEQUENCER_SDFFERROR_AFTEREND_NUM, i->second[0]->get_filename(), i->second[0]->extract_real_t_end(), i->second[0]->get_line());
        }
      }

    }
    if (i->second.size()>1) {
      for (size_t j=0; j<i->second.size(); j++) {
        // now call the check method
        i->second[j]->check();
        //std::cout<<"checked "<<i->first<<"["<<j<<"]: t_start="<<i->second[j]->extract_real_t_start()<<",   t_end="<<i->second[j]->extract_real_t_end()<<"\n";
        // here we only have to check for collision with t=0
        if (i->second[j]->extract_real_t_start()<0) {
          parser->add_error(SEQUENCER_SDFFERROR_NONEGTIME_NUM, i->second[j]->get_filename(), i->second[j]->extract_real_t_start(), i->second[j]->get_line());
        }
        // if end_time>0 we also have to check for collisions with t=end_time
        if (end_time>0) {
          if (i->second[j]->extract_real_t_end()>end_time) {
            parser->add_error(SEQUENCER_SDFFERROR_AFTEREND_NUM, i->second[j]->get_filename(), i->second[j]->extract_real_t_end(), i->second[j]->get_line());
          }
        }

        // if this is j>0 we can compare j to j-1 to determine collisions
        if (j>0) {
          if (i->second[j]->extract_real_t_start() < i->second[j-1]->extract_real_t_end()) {
            parser->add_error(SEQUENCER_SDFFERROR_COLLISION_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_COLLISION_NUM), i->first.c_str(), i->second[j-1]->extract_real_t_end(), i->second[j-1]->get_line(), i->second[j]->extract_real_t_end(), i->second[j]->get_line()), format(get_errorsmessage(SEQUENCER_SDFFERROR_COLLISION_NUM), i->second[j-1]->get_line(), i->second[j]->get_line()), i->second[j]->get_filename(), i->second[j]->extract_real_t_start(), i->second[j]->get_line());
          }
          /*if (i->second[j]->extract_real_t_start() == i->second[j-1]->extract_real_t_start()) {
            parser->add_error(SEQUENCER_SDFFERROR_COLLISION_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_COLLISION_NUM), i->first.c_str(), i->second[j-1]->extract_real_t_start(), i->second[j-1]->get_line(), i->second[j]->extract_real_t_start(), i->second[j]->get_line()), format(get_errorsmessage(SEQUENCER_SDFFERROR_COLLISION_NUM), i->second[j-1]->get_line(), i->second[j]->get_line()), i->second[j]->get_filename(), i->second[j]->extract_real_t_start(), i->second[j]->get_line());
          }*/
        }
      }
    }
  }
  if (parser->get_errornum()>0) return -1;
  if (parser->get_warningnum()>0) return +1;
  return 0;
};

double SDFFelementManager::extract_end_time(){
  std::map<std::string, std::vector<SDFFsequenceElement*> >::const_iterator i;
  double et=0;
  // go through all elements and always store the maximum end_time ...
  for (i=elements.begin(); i!=elements.end(); ++i) {
    if (i->second.size()>0) {
      for (size_t j=0; j<i->second.size(); j++) {
        double eet=i->second[j]->extract_real_t_end();
        et=mmax(et, eet);
      }
    }
  }
  return et;//+sample_timestep*2.0;
};



};

