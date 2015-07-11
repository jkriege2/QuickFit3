/*
  Name: csvsequenceparser.cpp
  Copyright: (c) 2007
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/

#include "csvsequenceparser.h" // class's header file


namespace sequencer {


CSVsequenceParser::CSVsequenceParser(sequenceGenerator* sg, channelManager* cm):
    baseImporter(sg, cm)
{
  position_mode=biTime;
}

CSVsequenceParser::~CSVsequenceParser() {
}

int CSVsequenceParser::import(std::string filename, double sample_timestep) {
  try {
    long timeindex=0;
    clear_events();
    generator->clear_used();

    // import csv and report errors
    if (file_exists(filename)) {
      csv.load_csv(filename, ',', '#');
    } else {
      add_error(SEQUENCER_CSVERROR_FILENOTEXISTS_NUM, format(get_errormessage(SEQUENCER_CSVERROR_FILENOTEXISTS_NUM), filename.c_str()), format(get_errorsmessage(SEQUENCER_CSVERROR_FILENOTEXISTS_NUM), filename.c_str()), filename,-1.0,0);
      return -1;
    };
    //std::cout<<csv.as_string()<<std::endl;

    // look for error in the file (comapre channels and rows, test whether first row is named time)
    // if a column for a channel exists: mark it as used
    if (csv.get_title(0)!="time") add_warning_t(SEQUENCER_CSVERROR_0NOTTIME_NUM, filename, 0);
    if (cmanager->get_channel_count()>0) {
      for (size_t i=0; i<cmanager->get_channel_count(); i++) {
        if (!csv.title_exists((*cmanager)[i].name) && (((*cmanager)[i].type==cmAnalogOutput)||((*cmanager)[i].type==cmDigitalOutput)) ) {
          // if you want warnings if you don't use a channel that is present in the cmanager: uncomment the next line!
          //add_warning_t(SEQUENCER_CSVERROR_CHBUTNOCO_NUM, format(get_errormessage(SEQUENCER_CSVERROR_CHBUTNOCO_NUM), (*cmanager)[i].name.c_str()), format(get_errorsmessage(SEQUENCER_CSVERROR_CHBUTNOCO_NUM), (*cmanager)[i].name.c_str()), filename, 0);
        }
        if (csv.title_exists((*cmanager)[i].name) && (((*cmanager)[i].type==cmAnalogOutput)||((*cmanager)[i].type==cmDigitalOutput)) ) {
          generator->add_used((*cmanager)[i].name);
          //std::cout<<"use "<<(*cmanager)[i].name<<std::endl;
        }
      }
    }
    if (csv.get_column_count()>0) {
      for (size_t i=0; i<csv.get_column_count(); i++) {
        if (csv.get_title(i)=="") {
          add_warning_t(SEQUENCER_CSVERROR_UNNAMEDCO_NUM, format(get_errormessage(SEQUENCER_CSVERROR_UNNAMEDCO_NUM), i), get_errorsmessage(SEQUENCER_CSVERROR_UNNAMEDCO_NUM), filename, 0);
        } else {
          if ((csv.get_title(i)!="time")&&(!cmanager->exists(csv.get_title(i)))) {
            add_warning_t(SEQUENCER_CSVERROR_COBUTNOCH_NUM, format(get_errormessage(SEQUENCER_CSVERROR_COBUTNOCH_NUM), i, csv.get_title(i).c_str()), format(get_errorsmessage(SEQUENCER_CSVERROR_COBUTNOCH_NUM), csv.get_title(i).c_str()), filename, 0);
          }
        }
      }
    }

    // check number of line/columns in the file
    if (csv.get_line_count()==0) {
      add_error_t(SEQUENCER_CSVERROR_NODEFINITIONS_NUM, filename, 0);
    }
    if (csv.get_column_count()==0) {
      add_error_t(SEQUENCER_CSVERROR_NOCOLS_NUM, filename, 0);
    }

    // sort in time domain
    csv.sort(timeindex);

    // get the maximum timestep, as endtime
    double endtime=csv.column_max(timeindex)*1e6;
    if (endtime<=0) {
      add_error_t(SEQUENCER_CSVERROR_ENDTIME_NUM, filename, endtime);
    }

    // now check whether there are times t<0. If there are, one of these times has to be in the first line!
    double t0=csv.get(timeindex, 0)*1e6;
    if (t0<0) {
      add_error_t(SEQUENCER_CSVERROR_NONEGTIME_NUM, filename, t0);
    }

    // now check whether any times are define twice
    if ((csv.get_line_count()>0)&&(csv.get_column_count()>0)) {
      if (csv.is_empty(timeindex, csv.get_line_count()-1)) {
        add_error_t(SEQUENCER_CSVERROR_NOUDTIME_NUM, filename, 0);
      }
      for (unsigned long i=0; i<csv.get_line_count()-1; i++) {
        if (csv.is_empty(timeindex, i)) {
          add_error_t(SEQUENCER_CSVERROR_NOUDTIME_NUM, filename, 0);
        } else {
          if (!csv.is_empty(timeindex, i+1)) {
            if (csv.get(timeindex, i)==csv.get(timeindex, i+1)) {
              add_error_t(SEQUENCER_CSVERROR_NODOUBLETIME_NUM, filename, csv.get(timeindex, i)*1e6);
            }
          }
        }
      }
    }

    // now check whether we use one of the required fields (if there are)
    if (cmanager->contains_required()) {
      bool foundrequired=false;
      for (unsigned long i=0; i<cmanager->get_channel_count(); i++) {
        foundrequired=true;
        if (cmanager->get(i).required) {
          foundrequired=false;
          if (csv.get_column_count()>0) {
            for (size_t j=0; j<csv.get_column_count(); j++) {
              if (cmanager->get(i).name==csv.get_title(j)) foundrequired=true;
            }
          }
        }
        if (!foundrequired) {
          add_error_t(SEQUENCER_CSVERROR_NOREQUIRED_NUM, format(get_errormessage(SEQUENCER_CSVERROR_NOREQUIRED_NUM), (*cmanager)[i].name.c_str()), format(get_errorsmessage(SEQUENCER_CSVERROR_NOREQUIRED_NUM), (*cmanager)[i].name.c_str()), filename, 0);
        }
      }
    }

    // now check for doublettes in the column titles
    if (csv.get_column_count()>0) {
      for (size_t i=0; i<csv.get_column_count(); i++) {
        for (size_t j=0; j<csv.get_column_count(); j++) {
          if (i!=j) {
            if (csv.get_title(i)==csv.get_title(j))
              add_error_t(SEQUENCER_CSVERROR_DOUBLETTE_NUM, format(get_errormessage(SEQUENCER_CSVERROR_DOUBLETTE_NUM), csv.get_title(j).c_str()), format(get_errorsmessage(SEQUENCER_CSVERROR_DOUBLETTE_NUM), csv.get_title(j).c_str()), filename, 0);
          }
        }
      }
    }


    if (errornum==0) {
      // now generate sequence (error checking only found warnings!)
      /*  + set the end time
          + Go through all channels in the channelmanager:
            - if a channels does not exist in the CSV file then it is set to
              its initial value 0!
            - if a channels does exist:
               1. get the new initial value, if it exists
               2. generate the sequence
       */
      if (sample_timestep==-1) {// set end_time
        generator->init(cmanager, generator->get_sample_timestep(), endtime);
      } else {
        generator->init(cmanager, sample_timestep, endtime);
      }
      if (cmanager->get_channel_count()>0) {
        for (size_t i=0; i<cmanager->get_channel_count(); i++) {
          if (generator->is_used((*cmanager)[i].name)){
            // get info about channel
            std::string ch=(*cmanager)[i].name;
            double cmax=(*cmanager)[i].valmax;
            double cmin=(*cmanager)[i].valmin;

            if (!csv.title_exists(ch)) { // set to all 0, as channel does not appear in CSV file
              if ((0<cmin)||(0>cmax)) {
                add_error_t(SEQUENCER_CSVERROR_0MINMAX_NUM, format(get_errormessage(SEQUENCER_CSVERROR_0MINMAX_NUM), ch.c_str(), cmin, cmax), format(get_errorsmessage(SEQUENCER_CSVERROR_0MINMAX_NUM), ch.c_str()), filename, 0);
                //return -1;
              }
              if ((*cmanager)[i].type==cmAnalogOutput) {
                generator->set_linear(ch,0, endtime,0);
              } else if ((*cmanager)[i].type==cmDigitalOutput) {
                generator->set_digital(ch,0,endtime,false);
              }
            } else { // there is a sequence defined
              int cnum=csv.get_column_by_title(ch); // get column where this channel is defined

              // OUTPUT FOR ANALOG CHANNELS
              if ((*cmanager)[i].type==cmAnalogOutput) {
                double initv=0;
                size_t istart=0;
                if (t0==0) { // timestep 0 is defined, so there could be new initial values
                  if (!csv.is_empty(cnum, 0)) {
                    initv=csv.get(cnum, 0);
                    istart=1;
                  }
                }
                if ((initv<cmin)||(initv>cmax)) {
                  add_error_t(SEQUENCER_CSVERROR_MINMAX_NUM, format(get_errormessage(SEQUENCER_CSVERROR_MINMAX_NUM), initv, ch.c_str(), cmin, cmax), format(get_errorsmessage(SEQUENCER_CSVERROR_MINMAX_NUM), ch.c_str(), initv), filename, 0);
                  //return -1;
                }
                initv=cmanager->scale_value(ch, initv);

                double oldtime=0;
                double oldvalue=initv;
                // now go through all timesteps in the csv file (they are already sorted by increasing time
                // the start of this iteration is 0 if timestep 0 is not defined in the file and 1 else.
                if (csv.get_line_count()>istart)
                  for (size_t i=istart; i<csv.get_line_count(); i++) {
                    if (!csv.is_empty(cnum, i)) {
                      double newtime=csv.get(timeindex, i)*1e6;
                      double newvalue=csv.get(cnum, i);
  //                    std::cout<<cnum<<"\t"<<newtime<<"     "<<newvalue<<"         "<<cmin<<"     "<<cmax<<std::endl;
                      if ((newvalue<cmin)||(newvalue>cmax)) {
                        add_error_t(SEQUENCER_CSVERROR_MINMAX_NUM, format(get_errormessage(SEQUENCER_CSVERROR_MINMAX_NUM), newvalue, ch.c_str(), cmin, cmax), format(get_errorsmessage(SEQUENCER_CSVERROR_MINMAX_NUM), ch.c_str(), newvalue), filename, newtime);
                        //return -1;
                      }
                      newvalue=cmanager->scale_value(ch, newvalue);
                      generator->set_linear(ch, oldtime, newtime, oldvalue, newvalue);
                      oldtime=newtime;
                      oldvalue=newvalue;
                    }
                  }
                if (oldtime<endtime)  // let the sequence be constant at the end!
                  generator->set_linear(ch, oldtime, endtime, oldvalue, oldvalue);


              // OUTPUT FOR DIGITAL CHANNELS
              } else if ((*cmanager)[i].type==cmDigitalOutput) {
                double initv=0;
                size_t istart=0;
                if (t0==0) { // timestep 0 is defined, so there could be new initial values
                  if (!csv.is_empty(cnum, 0)) {
                    initv=csv.get(cnum, 0);
                    istart=1;
                  }
                }
                if ((initv!=0)&&(initv!=1)) {
                  add_error_t(SEQUENCER_CSVERROR_NOTDIGI_NUM, format(get_errormessage(SEQUENCER_CSVERROR_NOTDIGI_NUM), initv, ch.c_str()), format(get_errorsmessage(SEQUENCER_CSVERROR_NOTDIGI_NUM), ch.c_str(), initv), filename, 0);
                  //return -1;
                }
                initv=cmanager->scale_value(ch, initv);

                double oldtime=0;
                bool oldvalue=cmanager->invert_value(ch, toBool(initv));
                bool newvalue=false;
                // now go through all timesteps in the csv file (they are already sorted by increasing time
                // the start of this iteration is 0 if timestep 0 is not defined in the file and 1 else.
                if (csv.get_line_count()>istart)
                  for (size_t i=istart; i<csv.get_line_count(); i++) {
                    if (!csv.is_empty(cnum, i)) {
                      double newtime=csv.get(timeindex, i)*1e6;
                      double nv=csv.get(cnum, i);
  //                    std::cout<<cnum<<"\t"<<newtime<<"     "<<newvalue<<"         "<<cmin<<"     "<<cmax<<std::endl;
                      if ((nv!=0)&&(nv!=1)) {
                        add_error_t(SEQUENCER_CSVERROR_NOTDIGI_NUM, format(get_errormessage(SEQUENCER_CSVERROR_NOTDIGI_NUM), newvalue, ch.c_str()), format(get_errorsmessage(SEQUENCER_CSVERROR_NOTDIGI_NUM), ch.c_str(), newvalue), filename, newtime);
                        //return -1;
                      }
                      newvalue=cmanager->invert_value(ch, toBool(nv));
                      generator->set_digital(ch, oldtime, newtime, oldvalue);
                      oldtime=newtime;
                      oldvalue=newvalue;
                    }
                  }
                if (oldtime<endtime)  // let the sequence be constant at the end!
                  generator->set_digital(ch, oldtime, endtime, oldvalue);
              }
            }
          }
        }
      }


      // check for errors and warnings:
      // return -1 if there were errors (and warnings)
      //        +1 if there were only warnings
      //         0 if there were no warnings and no errors
      if (errornum==0) {
        if (warningnum==0) { return 0; } else { return +1; }
      } else return -1;
    } else return -1;
  } catch(datatable_exception e) {
      // any error in the datatable class leads to an imediate error and the method will
      // be halted and stops with a return value of -1 (errors!)
      add_error_t(SEQUENCER_CSVERROR_PARSERERR_NUM, format(get_errormessage(SEQUENCER_CSVERROR_PARSERERR_NUM), e.get_message().c_str()), get_errorsmessage(SEQUENCER_CSVERROR_PARSERERR_NUM), filename, 0);
      return -1;
  }


}

};
