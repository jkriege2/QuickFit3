/*
  Name: sdfffunctions.cpp
  Copyright: (c) 2007
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/
#include "sdfffunctions.h"
#include "sdfffunctiontools.h"


namespace sequencer {
  namespace sdff_functions {

    /* sequence generating functions */

    sdff_result SET(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="set('channel'|time, value[, delay=0])";
      char short_name[]="set";
      char args[]="string (channel) or number (time)";
      char argd[]="double or integer";
      char argb[]="number or boolean";
      double t_start=parser->get_current_time()/1e6;
      if (parser->get_inside_preevent() || parser->get_inside_lastevent()) {
          if (paramcount==3) {
              double d=0;
              if (parameter[2].type==sdffRTDouble) {
                  d=parameter[2].doubleValue;
              } else if (parameter[2].type==sdffRTInteger) {
                  d=parameter[2].intValue;
              }

              if (d!=0) RUN_TIME_ERROR(SEQUENCER_SDFFERROR_DELAYEDOUTPUTINPRE_NUM, get_errormessage(SEQUENCER_SDFFERROR_DELAYEDOUTPUTINPRE_NUM), name);
          }
      }
      double value=0;
      SDFFelementManager* eman=parser->get_emanager();

      // check number of arguments
      if (paramcount<2) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM), name, paramcount, 2), name);
      }

      // read the delay parameter and give an error message for a wrong type (not int/double)
      double delay=0; // dfeault delay (if parameter is not supplied) is 0
      if (paramcount==3) {
        if (parameter[2].type==sdffRTDouble) {
          delay=parameter[2].doubleValue;
        } else if (parameter[2].type==sdffRTInteger) {
          delay=parameter[2].intValue;
        } else { // parameter is not double or integer -> error!
          RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[2].type).c_str(), 3, argd), name);
        }
      }
      t_start+=delay; // deal with delay
      //std::cout<<"set '"<<sdff_resulttostr(parameter[0])<<"' to "<<sdff_resulttostr(parameter[1])<<std::endl;
      // read the value parameter ... boolean will be converted to 0|1
      if ((parameter[1].type==sdffRTDouble)||(parameter[1].type==sdffRTInteger)) {
        value=parameter[1].doubleValue;
        if (parameter[1].type==sdffRTInteger) value=parameter[1].intValue;
      } else if ((parameter[1].type==sdffRTBoolean)) {
        value=parameter[1].boolValue?1:0;
      } else {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[1].type).c_str(), 2, argb), name);
      }

      // decide according to type of first argument, which type of set function call this is
      if ((parameter[0].type==sdffRTString)) { // this is a set('channel' ...) call, so we read the current time
        if (!parser->get_inside_event() && !parser->get_inside_preevent() && !parser->get_inside_lastevent()) {
          RUN_TIME_ERROR(SEQUENCER_SDFFERROR_ELEMENTNOTINEVENT_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_ELEMENTNOTINEVENT_NUM), short_name, parameter[0].stringValue.c_str()), name);
        }
        //std::cout<<"set '"<<parameter[0].stringValue<<"' to "<<value<<std::endl;
        SDFFSEset* e=new SDFFSEset(eman, parameter[0].stringValue, t_start*1e6, value);
        e->set_line(parser->get_current_line());
        eman->add(parameter[0].stringValue, e); // add with the given channel
      } else if ((parameter[0].type==sdffRTDouble)||(parameter[0].type==sdffRTInteger)) { // this is a set(time ...) call, so
                                                                                          // we read the current channel
        if (!parser->get_inside_channel()) {
          RUN_TIME_ERROR(SEQUENCER_SDFFERROR_ELEMENTNOTINCHANNEL_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_ELEMENTNOTINCHANNEL_NUM), short_name), name);
        }
        t_start=parameter[0].doubleValue;  // first read the given time
        if (parameter[0].type==sdffRTInteger) t_start=parameter[0].intValue;
        t_start+=delay; // then add the delay
        SDFFSEset* e=new SDFFSEset(eman, parser->get_current_channel(), t_start*1e6, value); // generate element with the current_channel from the parser
        e->set_line(parser->get_current_line());
        eman->add(parser->get_current_channel(), e);
      } else { // error in first argument
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      sdff_result r;
      r.type=sdffRTVoid;
      return r;
    }

    bool SET_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="set('channel'|time, value[, delay=0])";
      /*char args[]="integer";
      if ((parser->get_inside_preevent() || parser->get_inside_lastevent()) && paramcount==3) parser->add_error_p(SEQUENCER_SDFFERROR_DELAYEDOUTPUTINPRE_NUM, parser->get_current_filename(), parser->get_current_line());
      if (paramcount<2) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM), name, paramcount, 2), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }*/
      SDFFFUNC_CHECK_ARGCOUNT_MINMAX(2, 3, name);
      if (parser->get_inside_channel()) {
          SDFFFUNC_CHECK_NUMARGTYPE(0, name, "time");
      } else {
          SDFFFUNC_CHECK_STRINGARGTYPE(0, name, "channel");
      }
      SDFFFUNC_CHECK_NUMARGTYPE(1, name, "value");
      if (paramcount==3) SDFFFUNC_CHECK_NUMARGTYPE(3, name, "duration");
      return true;
    }

    sdff_result RAMPLIN(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="ramp_lin('channel'|time, end_value, duration[, delay=0])";
      char short_name[]="ramp_lin";
      char args[]="string (channel) or number (time)";
      char argd[]="double or integer";
      //char argb[]="number or boolean";
      double delay=0;
      double t_start=parser->get_current_time()/1e6;
      if (parser->get_inside_preevent()) RUN_TIME_ERROR(SEQUENCER_SDFFERROR_OUTPUTINPRE_NUM, get_errormessage(SEQUENCER_SDFFERROR_OUTPUTINPRE_NUM), name);
      if (parser->get_inside_lastevent()) RUN_TIME_ERROR(SEQUENCER_SDFFERROR_LONGOUTPUTINLAST_NUM, get_errormessage(SEQUENCER_SDFFERROR_LONGOUTPUTINLAST_NUM), name);
      SDFFelementManager* eman=parser->get_emanager();

      // check number of arguments
      if (paramcount<3) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM), name, paramcount, 4), name);
      }

      // read the delay parameter and give an error message for a wrong type (not int/double)
      if (paramcount==4) {
        if (parameter[3].type==sdffRTDouble) {
          delay=parameter[3].doubleValue;
        } else if (parameter[3].type==sdffRTInteger) {
          delay=parameter[3].intValue;
        } else { // parameter is not double or integer -> error!
          RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[3].type).c_str(), 4, argd), name);
        }
      }
      t_start+=delay;

      // read duration:
      double duration=0;
      if (parameter[2].type==sdffRTDouble) {
        duration=parameter[2].doubleValue;
      } else if (parameter[2].type==sdffRTInteger) {
        duration=parameter[2].intValue;
      } else { // parameter is not double or integer -> error!
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[2].type).c_str(), 3, argd), name);
      }

      // read end_value:
      double endvalue=0;
      if (parameter[1].type==sdffRTDouble) {
        endvalue=parameter[1].doubleValue;
      } else if (parameter[1].type==sdffRTInteger) {
        endvalue=parameter[1].intValue;
      } else { // parameter is not double or integer -> error!
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[1].type).c_str(), 2, argd), name);
      }

      // decide according to type of first argument, which type of set function call this is
      if ((parameter[0].type==sdffRTString)) { // this is a set('channel' ...) call, so we read the current time
        if (!parser->get_inside_event()) {
          RUN_TIME_ERROR(SEQUENCER_SDFFERROR_ELEMENTNOTINEVENT_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_ELEMENTNOTINEVENT_NUM), short_name, parameter[0].stringValue.c_str()), name);
        }
        SDFFSEramp* e=new SDFFSEramp(eman, parameter[0].stringValue, t_start*1e6, duration*1e6, sdffRTLinear, endvalue);
        e->set_line(parser->get_current_line());
        eman->add(parameter[0].stringValue, e);
      } else if ((parameter[0].type==sdffRTDouble)||(parameter[0].type==sdffRTInteger)) { // this is a set(time ...) call, so
                                                                                          // we read the current channel
        if (!parser->get_inside_channel()) {
          RUN_TIME_ERROR(SEQUENCER_SDFFERROR_ELEMENTNOTINCHANNEL_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_ELEMENTNOTINCHANNEL_NUM), short_name), name);
        }
        t_start=parameter[0].doubleValue;  // first read the given time
        if (parameter[0].type==sdffRTInteger) t_start=parameter[0].intValue;
        t_start+=delay; // then add the delay
        SDFFSEramp* e=new SDFFSEramp(eman, parser->get_current_channel(), t_start*1e6, duration*1e6, sdffRTLinear, endvalue); // generate element with the current_channel from the parser
        e->set_line(parser->get_current_line());
        eman->add(parser->get_current_channel(), e);
      } else { // error in first argument
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      sdff_result r;
      r.type=sdffRTVoid;
      return r;
    }

    bool RAMPLIN_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="ramp_lin('channel'|time, end_value, duration[, delay=0])";
      /*char args[]="integer";
      if (parser->get_inside_preevent()) parser->add_error_p(SEQUENCER_SDFFERROR_OUTPUTINPRE_NUM, parser->get_current_filename(), parser->get_current_line());
      if (parser->get_inside_lastevent()) parser->add_error_p(SEQUENCER_SDFFERROR_LONGOUTPUTINLAST_NUM, parser->get_current_filename(), parser->get_current_line());
      if (paramcount<4) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM), name, paramcount, 4), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }*/
      SDFFFUNC_CHECK_ARGCOUNT_MINMAX(3, 4, name);
      if (parser->get_inside_channel()) {
          SDFFFUNC_CHECK_NUMARGTYPE(0, name, "time");
      } else {
          SDFFFUNC_CHECK_STRINGARGTYPE(0, name, "channel");
      }
      SDFFFUNC_CHECK_NUMARGTYPE(1, name, "end_value");
      SDFFFUNC_CHECK_NUMARGTYPE(2, name, "duration");
      if (paramcount==4) SDFFFUNC_CHECK_NUMARGTYPE(3, name, "delay");
      return true;
    }

    sdff_result RAMPCOS(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="ramp_cos('channel'|time, end_value, duration[, delay=0])";
      char short_name[]="ramp_cos";
      char args[]="string (channel) or number (time)";
      char argd[]="double or integer";
      //char argb[]="number or boolean";
      double delay=0;
      double t_start=parser->get_current_time()/1e6;
      if (parser->get_inside_preevent()) RUN_TIME_ERROR(SEQUENCER_SDFFERROR_OUTPUTINPRE_NUM, get_errormessage(SEQUENCER_SDFFERROR_OUTPUTINPRE_NUM), name);
      if (parser->get_inside_lastevent()) RUN_TIME_ERROR(SEQUENCER_SDFFERROR_LONGOUTPUTINLAST_NUM, get_errormessage(SEQUENCER_SDFFERROR_LONGOUTPUTINLAST_NUM), name);
      SDFFelementManager* eman=parser->get_emanager();

      // check number of arguments
      if (paramcount<3) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM), name, paramcount, 4), name);
      }

      // read the delay parameter and give an error message for a wrong type (not int/double)
      if (paramcount==4) {
        if (parameter[3].type==sdffRTDouble) {
          delay=parameter[3].doubleValue;
        } else if (parameter[3].type==sdffRTInteger) {
          delay=parameter[3].intValue;
        } else { // parameter is not double or integer -> error!
          RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[3].type).c_str(), 4, argd), name);
        }
      }
      t_start+=delay;

      // read duration:
      double duration=0;
      if (parameter[2].type==sdffRTDouble) {
        duration=parameter[2].doubleValue;
      } else if (parameter[2].type==sdffRTInteger) {
        duration=parameter[2].intValue;
      } else { // parameter is not double or integer -> error!
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[2].type).c_str(), 3, argd), name);
      }

      // read end_value:
      double endvalue=0;
      if (parameter[1].type==sdffRTDouble) {
        endvalue=parameter[1].doubleValue;
      } else if (parameter[1].type==sdffRTInteger) {
        endvalue=parameter[1].intValue;
      } else { // parameter is not double or integer -> error!
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[1].type).c_str(), 2, argd), name);
      }

      // decide according to type of first argument, which type of set function call this is
      if ((parameter[0].type==sdffRTString)) { // this is a set('channel' ...) call, so we read the current time
        if (!parser->get_inside_event()) {
          RUN_TIME_ERROR(SEQUENCER_SDFFERROR_ELEMENTNOTINEVENT_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_ELEMENTNOTINEVENT_NUM), short_name, parameter[0].stringValue.c_str()), name);
        }
        SDFFSEramp* e=new SDFFSEramp(eman, parameter[0].stringValue, t_start*1e6, duration*1e6, sdffRTCosine, endvalue);
        e->set_line(parser->get_current_line());
        eman->add(parameter[0].stringValue, e);
      } else if ((parameter[0].type==sdffRTDouble)||(parameter[0].type==sdffRTInteger)) { // this is a set(time ...) call, so
                                                                                          // we read the current channel
        if (!parser->get_inside_channel()) {
          RUN_TIME_ERROR(SEQUENCER_SDFFERROR_ELEMENTNOTINCHANNEL_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_ELEMENTNOTINCHANNEL_NUM), short_name), name);
        }
        t_start=parameter[0].doubleValue;  // first read the given time
        if (parameter[0].type==sdffRTInteger) t_start=parameter[0].intValue;
        t_start+=delay; // then add the delay
        SDFFSEramp* e=new SDFFSEramp(eman, parser->get_current_channel(), t_start*1e6, duration*1e6, sdffRTCosine, endvalue); // generate element with the current_channel from the parser
        e->set_line(parser->get_current_line());
        eman->add(parser->get_current_channel(), e);
      } else { // error in first argument
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      sdff_result r;
      r.type=sdffRTVoid;
      return r;
    }

    bool RAMPCOS_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="ramp_cos('channel'|time, end_value, duration[, delay=0])";
      /*char args[]="integer";
      if (parser->get_inside_preevent()) parser->add_error_p(SEQUENCER_SDFFERROR_OUTPUTINPRE_NUM, parser->get_current_filename(), parser->get_current_line());
      if (parser->get_inside_lastevent()) parser->add_error_p(SEQUENCER_SDFFERROR_LONGOUTPUTINLAST_NUM, parser->get_current_filename(), parser->get_current_line());
      if (paramcount<3) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM), name, paramcount, 3), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }*/
      SDFFFUNC_CHECK_ARGCOUNT_MINMAX(3, 4, name);
      if (parser->get_inside_channel()) {
          SDFFFUNC_CHECK_NUMARGTYPE(0, name, "time");
      } else {
          SDFFFUNC_CHECK_STRINGARGTYPE(0, name, "channel");
      }
      SDFFFUNC_CHECK_NUMARGTYPE(1, name, "end_value");
      SDFFFUNC_CHECK_NUMARGTYPE(2, name, "duration");
      if (paramcount==4) SDFFFUNC_CHECK_NUMARGTYPE(3, name, "delay");
      return true;
    }

    sdff_result RAMPEXP(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="ramp_exp('channel'|time, end_value, duration, tau[, delay=0])";
      char short_name[]="ramp_exp";
      char args[]="string (channel) or number (time)";
      char argd[]="double or integer";
      //char argb[]="number or boolean";
      double delay=0;
      double t_start=parser->get_current_time()/1e6;
      if (parser->get_inside_preevent()) RUN_TIME_ERROR(SEQUENCER_SDFFERROR_OUTPUTINPRE_NUM, get_errormessage(SEQUENCER_SDFFERROR_OUTPUTINPRE_NUM), name);
      if (parser->get_inside_lastevent()) RUN_TIME_ERROR(SEQUENCER_SDFFERROR_LONGOUTPUTINLAST_NUM, get_errormessage(SEQUENCER_SDFFERROR_LONGOUTPUTINLAST_NUM), name);
      SDFFelementManager* eman=parser->get_emanager();

      // check number of arguments
      if (paramcount<4) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM), name, paramcount, 4), name);
      }

      // read the delay parameter and give an error message for a wrong type (not int/double)
      if (paramcount==5) {
        if (parameter[4].type==sdffRTDouble) {
          delay=parameter[4].doubleValue;
        } else if (parameter[4].type==sdffRTInteger) {
          delay=parameter[4].intValue;
        } else { // parameter is not double or integer -> error!
          RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[4].type).c_str(), 5, argd), name);
        }
      }
      t_start+=delay;

      // read tau:
      double tau=0;
      if (parameter[3].type==sdffRTDouble) {
        tau=parameter[3].doubleValue;
      } else if (parameter[3].type==sdffRTInteger) {
        tau=parameter[3].intValue;
      } else { // parameter is not double or integer -> error!
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[2].type).c_str(), 4, argd), name);
      }

      // read duration:
      double duration=0;
      if (parameter[2].type==sdffRTDouble) {
        duration=parameter[2].doubleValue;
      } else if (parameter[2].type==sdffRTInteger) {
        duration=parameter[2].intValue;
      } else { // parameter is not double or integer -> error!
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[2].type).c_str(), 3, argd), name);
      }

      // read end_value:
      double endvalue=0;
      if (parameter[1].type==sdffRTDouble) {
        endvalue=parameter[1].doubleValue;
      } else if (parameter[1].type==sdffRTInteger) {
        endvalue=parameter[1].intValue;
      } else { // parameter is not double or integer -> error!
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[1].type).c_str(), 2, argd), name);
      }

      // decide according to type of first argument, which type of set function call this is
      if ((parameter[0].type==sdffRTString)) { // this is a set('channel' ...) call, so we read the current time
        if (!parser->get_inside_event()) {
          RUN_TIME_ERROR(SEQUENCER_SDFFERROR_ELEMENTNOTINEVENT_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_ELEMENTNOTINEVENT_NUM), short_name, parameter[0].stringValue.c_str()), name);
        }
        SDFFSEramp* e=new SDFFSEramp(eman, parameter[0].stringValue, t_start*1e6, duration*1e6, sdffRTExponential, endvalue);
        e->set_tau(tau*1e6);
        e->set_line(parser->get_current_line());
        eman->add(parameter[0].stringValue, e);
      } else if ((parameter[0].type==sdffRTDouble)||(parameter[0].type==sdffRTInteger)) { // this is a set(time ...) call, so
                                                                                          // we read the current channel
        if (!parser->get_inside_channel()) {
          RUN_TIME_ERROR(SEQUENCER_SDFFERROR_ELEMENTNOTINCHANNEL_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_ELEMENTNOTINCHANNEL_NUM), short_name), name);
        }
        t_start=parameter[0].doubleValue;  // first read the given time
        if (parameter[0].type==sdffRTInteger) t_start=parameter[0].intValue;
        t_start+=delay; // then add the delay
        SDFFSEramp* e=new SDFFSEramp(eman, parser->get_current_channel(), t_start*1e6, duration*1e6, sdffRTExponential, endvalue); // generate element with the current_channel from the parser
        e->set_tau(tau*1e6);
        e->set_line(parser->get_current_line());
        eman->add(parser->get_current_channel(), e);
      } else { // error in first argument
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      sdff_result r;
      r.type=sdffRTVoid;
      return r;
    }

    bool RAMPEXP_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="ramp_exp('channel'|time, 'type', end_value, duration, tau[, delay=0])";
      //char args[]="integer";
      if (parser->get_inside_preevent()) parser->add_error_p(SEQUENCER_SDFFERROR_OUTPUTINPRE_NUM, parser->get_current_filename(), parser->get_current_line());
      if (parser->get_inside_lastevent()) parser->add_error_p(SEQUENCER_SDFFERROR_LONGOUTPUTINLAST_NUM, parser->get_current_filename(), parser->get_current_line());
      /*if (paramcount<5) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM), name, paramcount, 5), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }*/
      SDFFFUNC_CHECK_ARGCOUNT_MINMAX(4, 5, name);
      if (parser->get_inside_channel()) {
          SDFFFUNC_CHECK_NUMARGTYPE(0, name, "time");
      } else {
          SDFFFUNC_CHECK_STRINGARGTYPE(0, name, "channel");
      }
      SDFFFUNC_CHECK_NUMARGTYPE(1, name, "end_value");
      SDFFFUNC_CHECK_NUMARGTYPE(2, name, "duration");
      SDFFFUNC_CHECK_NUMARGTYPE(3, name, "tau");
      if (paramcount==5) SDFFFUNC_CHECK_NUMARGTYPE(4, name, "delay");
      return true;
    }



    sdff_result RAMPSIG(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="ramp_sig('channel'|time, end_value, duration, tau[, delay=0])";
      char short_name[]="ramp_sig";
      char args[]="string (channel) or number (time)";
      char argd[]="double or integer";
      //char argb[]="number or boolean";
      double delay=0;
      double t_start=parser->get_current_time()/1e6;
      if (parser->get_inside_preevent()) RUN_TIME_ERROR(SEQUENCER_SDFFERROR_OUTPUTINPRE_NUM, get_errormessage(SEQUENCER_SDFFERROR_OUTPUTINPRE_NUM), name);
      if (parser->get_inside_lastevent()) RUN_TIME_ERROR(SEQUENCER_SDFFERROR_LONGOUTPUTINLAST_NUM, get_errormessage(SEQUENCER_SDFFERROR_LONGOUTPUTINLAST_NUM), name);
      SDFFelementManager* eman=parser->get_emanager();

      // check number of arguments
      if (paramcount<4) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM), name, paramcount, 4), name);
      }

      // read the delay parameter and give an error message for a wrong type (not int/double)
      if (paramcount==5) {
        if (parameter[4].type==sdffRTDouble) {
          delay=parameter[4].doubleValue;
        } else if (parameter[4].type==sdffRTInteger) {
          delay=parameter[4].intValue;
        } else { // parameter is not double or integer -> error!
          RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[4].type).c_str(), 5, argd), name);
        }
      }
      t_start+=delay;

      // read tau:
      double tau=0;
      if (parameter[3].type==sdffRTDouble) {
        tau=parameter[3].doubleValue;
      } else if (parameter[3].type==sdffRTInteger) {
        tau=parameter[3].intValue;
      } else { // parameter is not double or integer -> error!
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[2].type).c_str(), 4, argd), name);
      }

      // read duration:
      double duration=0;
      if (parameter[2].type==sdffRTDouble) {
        duration=parameter[2].doubleValue;
      } else if (parameter[2].type==sdffRTInteger) {
        duration=parameter[2].intValue;
      } else { // parameter is not double or integer -> error!
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[2].type).c_str(), 3, argd), name);
      }

      // read end_value:
      double endvalue=0;
      if (parameter[1].type==sdffRTDouble) {
        endvalue=parameter[1].doubleValue;
      } else if (parameter[1].type==sdffRTInteger) {
        endvalue=parameter[1].intValue;
      } else { // parameter is not double or integer -> error!
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[1].type).c_str(), 2, argd), name);
      }

      // decide according to type of first argument, which type of set function call this is
      if ((parameter[0].type==sdffRTString)) { // this is a set('channel' ...) call, so we read the current time
        if (!parser->get_inside_event()) {
          RUN_TIME_ERROR(SEQUENCER_SDFFERROR_ELEMENTNOTINEVENT_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_ELEMENTNOTINEVENT_NUM), short_name, parameter[0].stringValue.c_str()), name);
        }
        SDFFSEramp* e=new SDFFSEramp(eman, parameter[0].stringValue, t_start*1e6, duration*1e6, sdffRTSigmoidal, endvalue);
        e->set_tau(tau);
        e->set_line(parser->get_current_line());
        eman->add(parameter[0].stringValue, e);
      } else if ((parameter[0].type==sdffRTDouble)||(parameter[0].type==sdffRTInteger)) { // this is a set(time ...) call, so
                                                                                          // we read the current channel
        if (!parser->get_inside_channel()) {
          RUN_TIME_ERROR(SEQUENCER_SDFFERROR_ELEMENTNOTINCHANNEL_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_ELEMENTNOTINCHANNEL_NUM), short_name), name);
        }
        t_start=parameter[0].doubleValue;  // first read the given time
        if (parameter[0].type==sdffRTInteger) t_start=parameter[0].intValue;
        t_start+=delay; // then add the delay
        SDFFSEramp* e=new SDFFSEramp(eman, parser->get_current_channel(), t_start*1e6, duration*1e6, sdffRTSigmoidal, endvalue); // generate element with the current_channel from the parser
        e->set_tau(tau);
        e->set_line(parser->get_current_line());
        eman->add(parser->get_current_channel(), e);
      } else { // error in first argument
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      sdff_result r;
      r.type=sdffRTVoid;
      return r;
    }

    bool RAMPSIG_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="ramp_sig('channel'|time, end_value, duration, tau[, delay=0])";
      //char args[]="integer";
      if (parser->get_inside_preevent()) parser->add_error_p(SEQUENCER_SDFFERROR_OUTPUTINPRE_NUM, parser->get_current_filename(), parser->get_current_line());
      if (parser->get_inside_lastevent()) parser->add_error_p(SEQUENCER_SDFFERROR_LONGOUTPUTINLAST_NUM, parser->get_current_filename(), parser->get_current_line());
      /*if (paramcount<5) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM), name, paramcount, 5), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }*/
      SDFFFUNC_CHECK_ARGCOUNT_MINMAX(4, 5, name);
      if (parser->get_inside_channel()) {
          SDFFFUNC_CHECK_NUMARGTYPE(0, name, "time");
      } else {
          SDFFFUNC_CHECK_STRINGARGTYPE(0, name, "channel");
      }
      SDFFFUNC_CHECK_NUMARGTYPE(1, name, "end_value");
      SDFFFUNC_CHECK_NUMARGTYPE(2, name, "duration");
      SDFFFUNC_CHECK_NUMARGTYPE(3, name, "tau");
      if (paramcount==5) SDFFFUNC_CHECK_NUMARGTYPE(4, name, "delay");
      return true;
    }



    sdff_result PULSE(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="pulse('channel'|time, value, duration[, delay=0])";
      char short_name[]="set";
      char args[]="string (channel) or number (time)";
      char argd[]="double or integer";
      char argb[]="number or boolean";
      double t_start=parser->get_current_time()/1e6;
      if (parser->get_inside_preevent()) RUN_TIME_ERROR(SEQUENCER_SDFFERROR_OUTPUTINPRE_NUM, get_errormessage(SEQUENCER_SDFFERROR_OUTPUTINPRE_NUM), name);
      if (parser->get_inside_lastevent()) RUN_TIME_ERROR(SEQUENCER_SDFFERROR_LONGOUTPUTINLAST_NUM, get_errormessage(SEQUENCER_SDFFERROR_LONGOUTPUTINLAST_NUM), name);
      double value=0;
      SDFFelementManager* eman=parser->get_emanager();

      // check number of arguments
      if (paramcount<3) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM), name, paramcount, 3), name);
      }

      // read the delay parameter and give an error message for a wrong type (not int/double)
      double delay=0; // dfeault delay (if parameter is not supplied) is 0
      if (paramcount==4) {
        if (parameter[3].type==sdffRTDouble) {
          delay=parameter[3].doubleValue;
        } else if (parameter[3].type==sdffRTInteger) {
          delay=parameter[3].intValue;
        } else { // parameter is not double or integer -> error!
          RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[3].type).c_str(), 4, argd), name);
        }
      }
      t_start+=delay; // deal with delay

      // read the value parameter ... boolean will be converted to 0|1
      if ((parameter[1].type==sdffRTDouble)||(parameter[1].type==sdffRTInteger)) {
        value=parameter[1].doubleValue;
        if (parameter[1].type==sdffRTInteger) value=parameter[1].intValue;
      } else if ((parameter[1].type==sdffRTBoolean)) {
        value=parameter[1].boolValue?1:0;
      } else {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[1].type).c_str(), 2, argb), name);
      }

      // read duration:
      double duration=0;
      if (parameter[2].type==sdffRTDouble) {
        duration=parameter[2].doubleValue;
      } else if (parameter[2].type==sdffRTInteger) {
        duration=parameter[2].intValue;
      } else { // parameter is not double or integer -> error!
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[2].type).c_str(), 3, argd), name);
      }

      // decide according to type of first argument, which type of set function call this is
      if ((parameter[0].type==sdffRTString)) { // this is a set('channel' ...) call, so we read the current time
        if (!parser->get_inside_event()) {
          RUN_TIME_ERROR(SEQUENCER_SDFFERROR_ELEMENTNOTINEVENT_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_ELEMENTNOTINEVENT_NUM), short_name, parameter[0].stringValue.c_str()), name);
        }
        SDFFSEpulse* e=new SDFFSEpulse(eman, parameter[0].stringValue, t_start*1e6, duration*1e6, value);
        e->set_line(parser->get_current_line());
        eman->add(parameter[0].stringValue, e); // add with the given channel
      } else if ((parameter[0].type==sdffRTDouble)||(parameter[0].type==sdffRTInteger)) { // this is a set(time ...) call, so
                                                                                          // we read the current channel
        if (!parser->get_inside_channel()) {
          RUN_TIME_ERROR(SEQUENCER_SDFFERROR_ELEMENTNOTINCHANNEL_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_ELEMENTNOTINCHANNEL_NUM), short_name), name);
        }
        t_start=parameter[0].doubleValue;  // first read the given time
        if (parameter[0].type==sdffRTInteger) t_start=parameter[0].intValue;
        t_start+=delay; // then add the delay
        SDFFSEpulse* e=new SDFFSEpulse(eman, parser->get_current_channel(), t_start*1e6, duration*1e6, value); // generate element with the current_channel from the parser
        e->set_line(parser->get_current_line());
        eman->add(parser->get_current_channel(), e);
      } else { // error in first argument
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      sdff_result r;
      r.type=sdffRTVoid;
      return r;
    }

    bool PULSE_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="pulse('channel'|time, value, duration[, delay=0])";
      //char args[]="integer";
      if (parser->get_inside_preevent()) parser->add_error_p(SEQUENCER_SDFFERROR_OUTPUTINPRE_NUM, parser->get_current_filename(), parser->get_current_line());
      if (parser->get_inside_lastevent()) parser->add_error_p(SEQUENCER_SDFFERROR_LONGOUTPUTINLAST_NUM, parser->get_current_filename(), parser->get_current_line());
      SDFFFUNC_CHECK_ARGCOUNT_MINMAX(3, 4, name);
      SDFFFUNC_CHECK_NUMORSTRINGARGTYPE(0, name, "channel or time");
      SDFFFUNC_CHECK_NUMARGTYPE(1, name, "value");
      SDFFFUNC_CHECK_NUMARGTYPE(2, name, "duration");
      if (paramcount>3) SDFFFUNC_CHECK_NUMARGTYPE(3, name, "delay");
      return true;
    }






























    /* functions with SDFFnode* arguments */

    sdff_result IF(SDFFparser* parser, std::string fname, SDFFnode** parameter, unsigned char paramcount) {
      char name[]="if(test, true_expr, false_expr)";
      char args[]="boolean";
      if (paramcount!=3) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 3), name);
      }
      sdff_result t=parameter[0]->evaluate();
      if (t.type!=sdffRTBoolean) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(t.type).c_str(), 1, args), name);
      }
      if (t.boolValue) return parameter[1]->evaluate();
      else return parameter[2]->evaluate();
    }

    bool IF_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="if(test, true_expr, false_expr)";
      if (paramcount!=3) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 3), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }

    sdff_result SUM(SDFFparser* parser, std::string fname, SDFFnode** parameter, unsigned char paramcount) {
      char name[]="sum(var, start, stop, expression)";
      char args[]="integer";
      sdff_result r, r1;

      if (paramcount!=4) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 3), name);
      }
      // test whether first node is a SDFFvariableNode (if supplied!)
      if (parameter[0]!=NULL) {
        if ((dynamic_cast<SDFFvariableNode*>(parameter[0]))) {
          // everything OK
        } else {
          parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, get_errormessage(SEQUENCER_SDFFERROR_SUM1NOTVARIABLE_NUM), get_errorsmessage(SEQUENCER_SDFFERROR_SUM1NOTVARIABLE_NUM), parser->get_current_filename(), parser->get_current_line());
        }
      }
      // get the name of the temp. variable
      std::string varname=dynamic_cast<SDFFvariableNode*>(parameter[0])->get_varname();
      SDFFVariableManager* vm=parser->get_vmanager();

      // initialize a variable, enter a new block and add the variable as external variable
      sdff_result start=parameter[1]->evaluate();
      sdff_result stop=parameter[2]->evaluate();
      if (start.type!=sdffRTInteger) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(start.type).c_str(), 2, args), name);
      }
      if (stop.type!=sdffRTInteger) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(stop.type).c_str(), 3, args), name);
      }
      long long i=start.intValue;
      vm->enter_block();
      vm->add_ext_variable_int(varname, &i);
      if (i<=stop.intValue) {
        r=parameter[3]->evaluate();
        i++;
        while(i<=stop.intValue) {
          r1=parameter[3]->evaluate();
          switch (r.type) {
            case sdffRTDouble:
              if (r1.type==sdffRTDouble) {
                r.doubleValue+=r1.doubleValue;
              } else if (r1.type==sdffRTInteger) {
                r.doubleValue+=(double)r1.intValue;
              } else RUN_TIME_ERROR(SEQUENCER_SDFFERROR_SUMTYPEMISS_NUM, get_errormessage(SEQUENCER_SDFFERROR_SUMTYPEMISS_NUM), name);
              break;
            case sdffRTInteger:
              if (r1.type==sdffRTDouble) {
                r.type=sdffRTDouble;
                r.doubleValue+=r1.doubleValue;
              } else if (r1.type==sdffRTInteger) {
                r.intValue+=r1.intValue;
              } else RUN_TIME_ERROR(SEQUENCER_SDFFERROR_SUMTYPEMISS_NUM, get_errormessage(SEQUENCER_SDFFERROR_SUMTYPEMISS_NUM), name);
              break;
            case sdffRTString:
              if (r1.type==sdffRTString) {
                r.stringValue+=r1.stringValue;
              } else RUN_TIME_ERROR(SEQUENCER_SDFFERROR_SUMTYPEMISS_NUM, get_errormessage(SEQUENCER_SDFFERROR_SUMTYPEMISS_NUM), name);
              break;
            default:
              RUN_TIME_ERROR(SEQUENCER_SDFFERROR_SUMTYPECMISS_NUM, get_errormessage(SEQUENCER_SDFFERROR_SUMTYPECMISS_NUM), name);
          }
          i++;
        }
      }
      vm->leave_block();
      return r;
    }

    bool SUM_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="sum(var, start, stop, expression)";
      if (paramcount!=4) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 3), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }

    sdff_result PROD(SDFFparser* parser, std::string fname, SDFFnode** parameter, unsigned char paramcount) {
      char name[]="prod(var, start, stop, expression)";
      char args[]="integer";
      sdff_result r, r1;

      if (paramcount!=4) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 3), name);
      }
      // test whether first node is a SDFFvariableNode (if supplied!)
      if (parameter[0]!=NULL) {
        if ((dynamic_cast<SDFFvariableNode*>(parameter[0]))) {
          // everything OK
        } else {
          parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, get_errormessage(SEQUENCER_SDFFERROR_PROD1NOTVARIABLE_NUM), get_errorsmessage(SEQUENCER_SDFFERROR_PROD1NOTVARIABLE_NUM), parser->get_current_filename(), parser->get_current_line());
        }
      }
      // get the name of the temp. variable
      std::string varname=dynamic_cast<SDFFvariableNode*>(parameter[0])->get_varname();
      SDFFVariableManager* vm=parser->get_vmanager();

      // initialize a variable, enter a new block and add the variable as external variable
      sdff_result start=parameter[1]->evaluate();
      sdff_result stop=parameter[2]->evaluate();
      if (start.type!=sdffRTInteger) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(start.type).c_str(), 2, args), name);
      }
      if (stop.type!=sdffRTInteger) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(stop.type).c_str(), 3, args), name);
      }
      long long i=start.intValue;
      vm->enter_block();
      vm->add_ext_variable_int(varname, &i);
      if (i<=stop.intValue) {
        r=parameter[3]->evaluate();
        i++;
        while(i<=stop.intValue) {
          r1=parameter[3]->evaluate();
          switch (r.type) {
            case sdffRTDouble:
              if (r1.type==sdffRTDouble) {
                r.doubleValue*=r1.doubleValue;
              } else if (r1.type==sdffRTInteger) {
                r.doubleValue*=(double)r1.intValue;
              } else RUN_TIME_ERROR(SEQUENCER_SDFFERROR_PRODTYPEMISS_NUM, get_errormessage(SEQUENCER_SDFFERROR_PRODTYPEMISS_NUM), name);
              break;
            case sdffRTInteger:
              if (r1.type==sdffRTDouble) {
                r.type=sdffRTDouble;
                r.doubleValue*=r1.doubleValue;
              } else if (r1.type==sdffRTInteger) {
                r.intValue*=r1.intValue;
              } else RUN_TIME_ERROR(SEQUENCER_SDFFERROR_PRODTYPEMISS_NUM, get_errormessage(SEQUENCER_SDFFERROR_PRODTYPEMISS_NUM), name);
              break;
            default:
              RUN_TIME_ERROR(SEQUENCER_SDFFERROR_PRODTYPECMISS_NUM, get_errormessage(SEQUENCER_SDFFERROR_PRODTYPECMISS_NUM), name);
          }
          i++;
        }
      }
      vm->leave_block();
      return r;
    }

    bool PROD_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="prod(var, start, stop, expression)";
      if (paramcount!=4) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 3), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }












    sdff_result ADDINFOMARKER(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="add_info_marker([time,] data[, data, ...])";
      char args[]="number";
      char minparam=2;
      if (parser->get_inside_event())  minparam=1;
      if (paramcount<minparam) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, minparam), name);
      }
      if (! parser->get_inside_event()) if ((parameter[0].type!=sdffRTDouble)&&(parameter[0].type!=sdffRTInteger)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      double time;
      if (parser->get_inside_event()) {
        time=parser->get_current_time();
      } else {
        time=parameter[0].doubleValue*1e6;
        if (parameter[0].type==sdffRTInteger) time=parameter[0].intValue*1e6;
      }
      std::string data="";
      for (int i=minparam-1; i<paramcount-(minparam-1); i++) {
        if (parameter[i].type==sdffRTString)
          data+=deescapify(parameter[i].stringValue);
        else
          data+=aasdff_resulttostr(parameter[i]);
      }
      sp_marker m;
      m.type=spInfo;
      m.time=time;
      m.text=data;
      parser->add_marker(m);
      sdff_result r;
      r.type=sdffRTVoid;
      return r;
    }

    bool ADDINFOMARKER_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="add_info_marker([time,] data[, data, ...])";
      char args[]="number";
      char minparam=2;
      if (parser->get_inside_event())  minparam=1;
      if (paramcount<minparam) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, minparam), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if (! parser->get_inside_event()) if ((parameter[0]!=sdffPTDouble)&&(parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }
    sdff_result ADDWARNINGMARKER(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="add_warning_marker([time,] data[, data, ...])";
      char args[]="number";
      char minparam=2;
      if (parser->get_inside_event())  minparam=1;
      if (paramcount<minparam) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, minparam), name);
      }
      if (! parser->get_inside_event()) if ((parameter[0].type!=sdffRTDouble)&&(parameter[0].type!=sdffRTInteger)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      double time;
      if (parser->get_inside_event()) {
        time=parser->get_current_time();
      } else {
        time=parameter[0].doubleValue*1e6;
        if (parameter[0].type==sdffRTInteger) time=parameter[0].intValue*1e6;
      }
      std::string data="";
      for (int i=minparam-1; i<paramcount-(minparam-1); i++) {
        if (parameter[i].type==sdffRTString)
          data+=deescapify(parameter[i].stringValue);
        else
          data+=aasdff_resulttostr(parameter[i]);
      }
      sp_marker m;
      m.type=spWarning;
      m.time=time;
      m.text=data;
      parser->add_marker(m);
      sdff_result r;
      r.type=sdffRTVoid;
      return r;
    }

    bool ADDWARNINGMARKER_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="add_warning_marker([time,] data[, data, ...])";
      char args[]="number";
      char minparam=2;
      if (parser->get_inside_event())  minparam=1;
      if (paramcount<minparam) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, minparam), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if (! parser->get_inside_event()) if ((parameter[0]!=sdffPTDouble)&&(parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }
    sdff_result ADDERRORMARKER(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="add_error_marker([time,] data[, data, ...])";
      char args[]="number";
      char minparam=2;
      if (parser->get_inside_event())  minparam=1;
      if (paramcount<minparam) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, minparam), name);
      }
      if (! parser->get_inside_event()) if ((parameter[0].type!=sdffRTDouble)&&(parameter[0].type!=sdffRTInteger)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      double time;
      if (parser->get_inside_event()) {
        time=parser->get_current_time();
      } else {
        time=parameter[0].doubleValue*1e6;
        if (parameter[0].type==sdffRTInteger) time=parameter[0].intValue*1e6;
      }
      std::string data="";
      for (int i=minparam-1; i<paramcount-(minparam-1); i++) {
        if (parameter[i].type==sdffRTString)
          data+=deescapify(parameter[i].stringValue);
        else
          data+=aasdff_resulttostr(parameter[i]);
      }
      sp_marker m;
      m.type=spError;
      m.time=time;
      m.text=data;
      parser->add_marker(m);
      sdff_result r;
      r.type=sdffRTVoid;
      return r;
    }

    bool ADDERRORMARKER_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="add_error_marker([time,] data[, data, ...])";
      char args[]="number";
      char minparam=2;
      if (parser->get_inside_event())  minparam=1;
      if (paramcount<minparam) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, minparam), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if (! parser->get_inside_event()) if ((parameter[0]!=sdffPTDouble)&&(parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }

    sdff_result WRITELN(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="writeln(data[, data, ...])";
      if (paramcount<1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), name);
      }
      std::string data="";
      for (int i=0; i<paramcount; i++) {
        if (parameter[i].type==sdffRTString)
          data+=deescapify(parameter[i].stringValue);
        else
          data+=aasdff_resulttostr(parameter[i]);
      }
      LogFile* l=parser->get_log();
      if (l!=NULL){
        l->log_text(data+'\n');
      } else {
        std::cout<<data<<std::endl;
      }
      sdff_result r;
      r.type=sdffRTVoid;
      return r;
    }

    bool WRITELN_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="writeln(data[, data, ...])";
      if (paramcount<1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }

    sdff_result WRITE(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="write(data[, data, ...])";
      if (paramcount<1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), name);
      }
      std::string data="";
      for (int i=0; i<paramcount; i++) {
        if (parameter[i].type==sdffRTString)
          data+=deescapify(parameter[i].stringValue);
        else
          data+=aasdff_resulttostr(parameter[i]);
      }
      LogFile* l=parser->get_log();
      if (l!=NULL){
        l->log_text(data);
      } else {
        std::cout<<data;
      }
      sdff_result r;
      r.type=sdffRTVoid;
      return r;
    }

    bool WRITE_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="write(data[, data, ...])";
      if (paramcount<1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }

    sdff_result LIMIT(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="limit(x,lowerbound,upperbound)";
      char args[]="number";
      char pnum=3;
      if (paramcount!=pnum) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, pnum), name);
      }
      if ((parameter[0].type!=sdffRTInteger)&&(parameter[0].type!=sdffRTDouble)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      if ((parameter[1].type!=sdffRTInteger)&&(parameter[1].type!=sdffRTDouble)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[1].type).c_str(), 2, args), name);
      }
      if ((parameter[2].type!=sdffRTInteger)&&(parameter[2].type!=sdffRTDouble)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[2].type).c_str(), 3, args), name);
      }
      double x=parameter[0].doubleValue;
      if (parameter[0].type==sdffRTInteger) x=parameter[0].intValue;
      double l=parameter[1].doubleValue;
      if (parameter[1].type==sdffRTInteger) l=parameter[1].intValue;
      double u=parameter[2].doubleValue;
      if (parameter[2].type==sdffRTInteger) u=parameter[2].intValue;

      if (x>u) return parameter[2];
      if (x<l) return parameter[1];
      return parameter[0];
    }

    bool LIMIT_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="limit(x,lowerbound,upperbound)";
      char args[]="number";
      char pnum=3;
      if (paramcount!=pnum) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, pnum), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTDouble)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[1]!=sdffPTInteger)&&(parameter[1]!=sdffPTDouble)&&(parameter[1]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[1]).c_str(), 2, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[2]!=sdffPTInteger)&&(parameter[2]!=sdffPTDouble)&&(parameter[2]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[2]).c_str(), 3, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }

    sdff_result DELTA(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="delta(a,b=0)";
      char args[]="number";
      //char pnum=2;
      SDFFFUNC_CHECK_ARGCOUNT_MINMAX_RT(1,2,name)
      if ((parameter[0].type!=sdffRTInteger)&&(parameter[0].type!=sdffRTDouble)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      if (paramcount==2) {
          if ((parameter[1].type!=sdffRTInteger)&&(parameter[1].type!=sdffRTDouble)) {
            RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[1].type).c_str(), 2, args), name);
          }
      }
      double a=parameter[0].doubleValue;
      if (parameter[0].type==sdffRTInteger) a=parameter[0].intValue;
      double b=0;
      if (paramcount==2) {
          b=parameter[1].doubleValue;
          if (parameter[1].type==sdffRTInteger) b=parameter[1].intValue;
      }

      sdff_result r;
      r.type=sdffRTInteger;
      r.intValue=0;
      if (a==b) r.intValue=1;
      return r;
    }

    bool DELTA_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="delta(a,b=0)";
      char args[]="number";
      //char pnum=2;
      SDFFFUNC_CHECK_ARGCOUNT_MINMAX(1,2,name);
      if ((parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTDouble)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if (paramcount==2) if ((parameter[1]!=sdffPTInteger)&&(parameter[1]!=sdffPTDouble)&&(parameter[1]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[1]).c_str(), 2, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }


    sdff_result MAX(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="max(a,b)";
      char args[]="number";
      char pnum=2;
      if (paramcount!=pnum) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, pnum), name);
      }
      if ((parameter[0].type!=sdffRTInteger)&&(parameter[0].type!=sdffRTDouble)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      if ((parameter[1].type!=sdffRTInteger)&&(parameter[1].type!=sdffRTDouble)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[1].type).c_str(), 2, args), name);
      }
      double a=parameter[0].doubleValue;
      if (parameter[0].type==sdffRTInteger) a=parameter[0].intValue;
      double b=parameter[1].doubleValue;
      if (parameter[1].type==sdffRTInteger) b=parameter[1].intValue;

      if (a>b) return parameter[0];
      return parameter[1];
    }

    bool MAX_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="max(a,b)";
      char args[]="number";
      char pnum=2;
      if (paramcount!=pnum) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, pnum), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTDouble)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[1]!=sdffPTInteger)&&(parameter[1]!=sdffPTDouble)&&(parameter[1]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[1]).c_str(), 2, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }

    sdff_result MIN(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="min(a,b)";
      char args[]="number";
      char pnum=2;
      if (paramcount!=pnum) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, pnum), name);
      }
      if ((parameter[0].type!=sdffRTInteger)&&(parameter[0].type!=sdffRTDouble)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      if ((parameter[1].type!=sdffRTInteger)&&(parameter[1].type!=sdffRTDouble)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[1].type).c_str(), 2, args), name);
      }
      double a=parameter[0].doubleValue;
      if (parameter[0].type==sdffRTInteger) a=parameter[0].intValue;
      double b=parameter[1].doubleValue;
      if (parameter[1].type==sdffRTInteger) b=parameter[1].intValue;

      if (a<b) return parameter[0];
      return parameter[1];
    }

    bool MIN_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="min(a,b)";
      char args[]="number";
      char pnum=2;
      if (paramcount!=pnum) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, pnum), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTDouble)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[1]!=sdffPTInteger)&&(parameter[1]!=sdffPTDouble)&&(parameter[1]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[1]).c_str(), 2, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }

    sdff_result FACTORIAL(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="factorial()";
      char args[]="integer";
      if (paramcount!=1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), name);
      }
      if ((parameter[0].type!=sdffRTInteger)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      long long d=parameter[0].intValue;
      if (d<0) RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FACTORIALPOS_NUM, get_errormessage(SEQUENCER_SDFFERROR_FACTORIALPOS_NUM), name);
      sdff_result r;
      r.type=sdffRTInteger;
      if ((d==0)||(d==1)) {
        r.intValue=1;
      } else {
        r.intValue=1;
        for (long long i=d; i>0; i--) {
          r.intValue*=i;
        }
      }
      return r;
    }

    bool FACTORIAL_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="factorial()";
      char args[]="integer";
      if (paramcount!=1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }

    sdff_result SIN(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="sin()";
      char args[]="double or integer";
      if (paramcount!=1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), name);
      }
      if ((parameter[0].type!=sdffRTDouble)&&(parameter[0].type!=sdffRTInteger)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      double d=parameter[0].doubleValue;
      if (parameter[0].type==sdffRTInteger) d=parameter[0].intValue;
      sdff_result r;
      r.type=sdffRTDouble;
      r.doubleValue=sin(d);
      return r;
    }

    bool SIN_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="sin()";
      char args[]="double or integer";
      if (paramcount!=1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTDouble)&&(parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }

    sdff_result COS(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="cos()";
      char args[]="double or integer";
      if (paramcount!=1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), name);
      }
      if ((parameter[0].type!=sdffRTDouble)&&(parameter[0].type!=sdffRTInteger)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      double d=parameter[0].doubleValue;
      if (parameter[0].type==sdffRTInteger) d=parameter[0].intValue;
      sdff_result r;
      r.type=sdffRTDouble;
      r.doubleValue=cos(d);
      return r;
    }

    bool COS_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="cos()";
      char args[]="double or integer";
      if (paramcount!=1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTDouble)&&(parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }


    sdff_result TAN(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="tan()";
      char args[]="double or integer";
      if (paramcount!=1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), name);
      }
      if ((parameter[0].type!=sdffRTDouble)&&(parameter[0].type!=sdffRTInteger)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      double d=parameter[0].doubleValue;
      if (parameter[0].type==sdffRTInteger) d=parameter[0].intValue;
      sdff_result r;
      r.type=sdffRTDouble;
      r.doubleValue=tan(d);
      return r;
    }

    bool TAN_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="tan()";
      char args[]="double or integer";
      if (paramcount!=1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTDouble)&&(parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }


    sdff_result EXP(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="exp()";
      char args[]="double or integer";
      if (paramcount!=1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), name);
      }
      if ((parameter[0].type!=sdffRTDouble)&&(parameter[0].type!=sdffRTInteger)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      double d=parameter[0].doubleValue;
      if (parameter[0].type==sdffRTInteger) d=parameter[0].intValue;
      sdff_result r;
      r.type=sdffRTDouble;
      r.doubleValue=exp(d);
      return r;
    }

    bool EXP_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="exp()";
      char args[]="double or integer";
      if (paramcount!=1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTDouble)&&(parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }

    sdff_result EXPM1(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="expm1()";
      char args[]="double or integer";
      if (paramcount!=1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), name);
      }
      if ((parameter[0].type!=sdffRTDouble)&&(parameter[0].type!=sdffRTInteger)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      double d=parameter[0].doubleValue;
      if (parameter[0].type==sdffRTInteger) d=parameter[0].intValue;
      sdff_result r;
      r.type=sdffRTDouble;
      r.doubleValue=expm1(d);
      return r;
    }

    bool EXPM1_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="expm1()";
      char args[]="double or integer";
      if (paramcount!=1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTDouble)&&(parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }

    sdff_result ASIN(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="asin()";
      char args[]="double or integer";
      if (paramcount!=1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), name);
      }
      if ((parameter[0].type!=sdffRTDouble)&&(parameter[0].type!=sdffRTInteger)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      double d=parameter[0].doubleValue;
      if (parameter[0].type==sdffRTInteger) d=parameter[0].intValue;
      sdff_result r;
      r.type=sdffRTDouble;
      r.doubleValue=asin(d);
      return r;
    }

    bool ASIN_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="asin()";
      char args[]="double or integer";
      if (paramcount!=1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTDouble)&&(parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }

    sdff_result ACOS(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="acos()";
      char args[]="double or integer";
      if (paramcount!=1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), name);
      }
      if ((parameter[0].type!=sdffRTDouble)&&(parameter[0].type!=sdffRTInteger)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      double d=parameter[0].doubleValue;
      if (parameter[0].type==sdffRTInteger) d=parameter[0].intValue;
      sdff_result r;
      r.type=sdffRTDouble;
      r.doubleValue=acos(d);
      return r;
    }

    bool ACOS_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="acos()";
      char args[]="double or integer";
      if (paramcount!=1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTDouble)&&(parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }


    sdff_result ATAN(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="atan()";
      char args[]="double or integer";
      if (paramcount!=1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), name);
      }
      if ((parameter[0].type!=sdffRTDouble)&&(parameter[0].type!=sdffRTInteger)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      double d=parameter[0].doubleValue;
      if (parameter[0].type==sdffRTInteger) d=parameter[0].intValue;
      sdff_result r;
      r.type=sdffRTDouble;
      r.doubleValue=atan(d);
      return r;
    }

    bool ATAN_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="atan()";
      char args[]="double or integer";
      if (paramcount!=1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTDouble)&&(parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }

    sdff_result LOG(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="log()";
      char args[]="double or integer";
      if (paramcount!=1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), name);
      }
      if ((parameter[0].type!=sdffRTDouble)&&(parameter[0].type!=sdffRTInteger)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      double d=parameter[0].doubleValue;
      if (parameter[0].type==sdffRTInteger) d=parameter[0].intValue;
      sdff_result r;
      r.type=sdffRTDouble;
      r.doubleValue=log(d);
      return r;
    }

    bool LOG_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="log()";
      char args[]="double or integer";
      if (paramcount!=1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTDouble)&&(parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }

    sdff_result LOGP1(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="logp1()";
      char args[]="double or integer";
      if (paramcount!=1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), name);
      }
      if ((parameter[0].type!=sdffRTDouble)&&(parameter[0].type!=sdffRTInteger)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      double d=parameter[0].doubleValue;
      if (parameter[0].type==sdffRTInteger) d=parameter[0].intValue;
      sdff_result r;
      r.type=sdffRTDouble;
      r.doubleValue=log1p(d);
      return r;
    }

    bool LOGP1_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="logp1()";
      char args[]="double or integer";
      if (paramcount!=1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTDouble)&&(parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }

    sdff_result LOG2(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="log2()";
      char args[]="double or integer";
      if (paramcount!=1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), name);
      }
      if ((parameter[0].type!=sdffRTDouble)&&(parameter[0].type!=sdffRTInteger)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      double d=parameter[0].doubleValue;
      if (parameter[0].type==sdffRTInteger) d=parameter[0].intValue;
      sdff_result r;
      r.type=sdffRTDouble;
      r.doubleValue=log2(d);
      return r;
    }

    bool LOG2_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="log2()";
      char args[]="double or integer";
      if (paramcount!=1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTDouble)&&(parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }

    sdff_result LOG10(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="log10()";
      char args[]="double or integer";
      if (paramcount!=1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), name);
      }
      if ((parameter[0].type!=sdffRTDouble)&&(parameter[0].type!=sdffRTInteger)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      double d=parameter[0].doubleValue;
      if (parameter[0].type==sdffRTInteger) d=parameter[0].intValue;
      sdff_result r;
      r.type=sdffRTDouble;
      r.doubleValue=log10(d);
      return r;
    }

    bool LOG10_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="log10()";
      char args[]="double or integer";
      if (paramcount!=1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTDouble)&&(parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }

    sdff_result SQRT(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="sqrt()";
      char args[]="double or integer";
      if (paramcount!=1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), name);
      }
      if ((parameter[0].type!=sdffRTDouble)&&(parameter[0].type!=sdffRTInteger)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      double d=parameter[0].doubleValue;
      if (parameter[0].type==sdffRTInteger) d=parameter[0].intValue;
      sdff_result r;
      r.type=sdffRTDouble;
      r.doubleValue=sqrt(d);
      return r;
    }

    bool SQRT_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="sqrt()";
      char args[]="double or integer";
      if (paramcount!=1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTDouble)&&(parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }

    sdff_result CBRT(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="cbrt()";
      char args[]="double or integer";
      if (paramcount!=1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), name);
      }
      if ((parameter[0].type!=sdffRTDouble)&&(parameter[0].type!=sdffRTInteger)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      double d=parameter[0].doubleValue;
      if (parameter[0].type==sdffRTInteger) d=parameter[0].intValue;
      sdff_result r;
      r.type=sdffRTDouble;
      r.doubleValue=cbrt(d);
      return r;
    }

    bool CBRT_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="cbrt()";
      char args[]="double or integer";
      if (paramcount!=1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTDouble)&&(parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }




    sdff_result ASINH(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="asinh()";
      char args[]="double or integer";
      if (paramcount!=1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), name);
      }
      if ((parameter[0].type!=sdffRTDouble)&&(parameter[0].type!=sdffRTInteger)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      double d=parameter[0].doubleValue;
      if (parameter[0].type==sdffRTInteger) d=parameter[0].intValue;
      sdff_result r;
      r.type=sdffRTDouble;
      r.doubleValue=asinh(d);
      return r;
    }

    bool ASINH_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="asinh()";
      char args[]="double or integer";
      if (paramcount!=1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTDouble)&&(parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }

    sdff_result ACOSH(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="acosh()";
      char args[]="double or integer";
      if (paramcount!=1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), name);
      }
      if ((parameter[0].type!=sdffRTDouble)&&(parameter[0].type!=sdffRTInteger)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      double d=parameter[0].doubleValue;
      if (parameter[0].type==sdffRTInteger) d=parameter[0].intValue;
      sdff_result r;
      r.type=sdffRTDouble;
      r.doubleValue=acosh(d);
      return r;
    }

    bool ACOSH_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="acosh()";
      char args[]="double or integer";
      if (paramcount!=1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTDouble)&&(parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }


    sdff_result ATANH(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="atanh()";
      char args[]="double or integer";
      if (paramcount!=1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), name);
      }
      if ((parameter[0].type!=sdffRTDouble)&&(parameter[0].type!=sdffRTInteger)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      double d=parameter[0].doubleValue;
      if (parameter[0].type==sdffRTInteger) d=parameter[0].intValue;
      sdff_result r;
      r.type=sdffRTDouble;
      r.doubleValue=atanh(d);
      return r;
    }

    bool ATANH_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="atanh()";
      char args[]="double or integer";
      if (paramcount!=1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTDouble)&&(parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }




    sdff_result SINH(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="sinh()";
      char args[]="double or integer";
      if (paramcount!=1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), name);
      }
      if ((parameter[0].type!=sdffRTDouble)&&(parameter[0].type!=sdffRTInteger)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      double d=parameter[0].doubleValue;
      if (parameter[0].type==sdffRTInteger) d=parameter[0].intValue;
      sdff_result r;
      r.type=sdffRTDouble;
      r.doubleValue=sinh(d);
      return r;
    }

    bool SINH_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="sinh()";
      char args[]="double or integer";
      if (paramcount!=1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTDouble)&&(parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }

    sdff_result COSH(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="cosh()";
      char args[]="double or integer";
      if (paramcount!=1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), name);
      }
      if ((parameter[0].type!=sdffRTDouble)&&(parameter[0].type!=sdffRTInteger)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      double d=parameter[0].doubleValue;
      if (parameter[0].type==sdffRTInteger) d=parameter[0].intValue;
      sdff_result r;
      r.type=sdffRTDouble;
      r.doubleValue=cosh(d);
      return r;
    }

    bool COSH_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="cosh()";
      char args[]="double or integer";
      if (paramcount!=1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTDouble)&&(parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }


    sdff_result TANH(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="tanh()";
      char args[]="double or integer";
      if (paramcount!=1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), name);
      }
      if ((parameter[0].type!=sdffRTDouble)&&(parameter[0].type!=sdffRTInteger)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      double d=parameter[0].doubleValue;
      if (parameter[0].type==sdffRTInteger) d=parameter[0].intValue;
      sdff_result r;
      r.type=sdffRTDouble;
      r.doubleValue=tanh(d);
      return r;
    }

    bool TANH_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="tanh()";
      char args[]="double or integer";
      if (paramcount!=1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTDouble)&&(parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }






    sdff_result ERF(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="erf()";
      char args[]="double or integer";
      if (paramcount!=1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), name);
      }
      if ((parameter[0].type!=sdffRTDouble)&&(parameter[0].type!=sdffRTInteger)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      double d=parameter[0].doubleValue;
      if (parameter[0].type==sdffRTInteger) d=parameter[0].intValue;
      sdff_result r;
      r.type=sdffRTDouble;
      r.doubleValue=erf(d);
      return r;
    }

    bool ERF_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="erf()";
      char args[]="double or integer";
      if (paramcount!=1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTDouble)&&(parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }

    sdff_result ERFC(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="erfc()";
      char args[]="double or integer";
      if (paramcount!=1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), name);
      }
      if ((parameter[0].type!=sdffRTDouble)&&(parameter[0].type!=sdffRTInteger)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      double d=parameter[0].doubleValue;
      if (parameter[0].type==sdffRTInteger) d=parameter[0].intValue;
      sdff_result r;
      r.type=sdffRTDouble;
      r.doubleValue=erfc(d);
      return r;
    }

    bool ERFC_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="erfc()";
      char args[]="double or integer";
      if (paramcount!=1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTDouble)&&(parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }

    sdff_result TGAMMA(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="tgamma()";
      char args[]="double or integer";
      if (paramcount!=1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), name);
      }
      if ((parameter[0].type!=sdffRTDouble)&&(parameter[0].type!=sdffRTInteger)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      double d=parameter[0].doubleValue;
      if (parameter[0].type==sdffRTInteger) d=parameter[0].intValue;
      sdff_result r;
      r.type=sdffRTDouble;
      r.doubleValue=tgamma(d);
      return r;
    }

    bool TGAMMA_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="tgamma()";
      char args[]="double or integer";
      if (paramcount!=1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTDouble)&&(parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }

    sdff_result J0(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="j0()";
      char args[]="double or integer";
      if (paramcount!=1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), name);
      }
      if ((parameter[0].type!=sdffRTDouble)&&(parameter[0].type!=sdffRTInteger)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      double d=parameter[0].doubleValue;
      if (parameter[0].type==sdffRTInteger) d=parameter[0].intValue;
      sdff_result r;
      r.type=sdffRTDouble;
      r.doubleValue=j0(d);
      return r;
    }

    bool J0_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="j0()";
      char args[]="double or integer";
      if (paramcount!=1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTDouble)&&(parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }

    sdff_result J1(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="j1()";
      char args[]="double or integer";
      if (paramcount!=1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), name);
      }
      if ((parameter[0].type!=sdffRTDouble)&&(parameter[0].type!=sdffRTInteger)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      double d=parameter[0].doubleValue;
      if (parameter[0].type==sdffRTInteger) d=parameter[0].intValue;
      sdff_result r;
      r.type=sdffRTDouble;
      r.doubleValue=j1(d);
      return r;
    }

    bool J1_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="j1()";
      char args[]="double or integer";
      if (paramcount!=1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTDouble)&&(parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }

    sdff_result JN(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="jn()";
      char args[]="double or integer";
      char args1[]="integer";
      char pcount=2;
      if (paramcount!=pcount) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, pcount), name);
      }
      if ((parameter[0].type!=sdffRTDouble)&&(parameter[0].type!=sdffRTInteger)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      if (parameter[1].type!=sdffRTInteger) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 2, args1), name);
      }
      double d=parameter[0].doubleValue;
      long long i=parameter[1].intValue;
      if (parameter[0].type==sdffRTInteger) d=parameter[0].intValue;
      sdff_result r;
      r.type=sdffRTDouble;
      r.doubleValue=jn(i,d);
      return r;
    }

    bool JN_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="jn()";
      char args[]="double or integer";
      char args1[]="integer";
      char pcount=2;
      if (paramcount!=pcount) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, pcount), name);
      }
      if ((parameter[0]!=sdffPTDouble)&&(parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[1]!=sdffPTInteger)&&(parameter[1]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 2, args1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }


    sdff_result Y0(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="y0()";
      char args[]="double or integer";
      if (paramcount!=1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), name);
      }
      if ((parameter[0].type!=sdffRTDouble)&&(parameter[0].type!=sdffRTInteger)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      double d=parameter[0].doubleValue;
      if (parameter[0].type==sdffRTInteger) d=parameter[0].intValue;
      sdff_result r;
      r.type=sdffRTDouble;
      r.doubleValue=y0(d);
      return r;
    }

    bool Y0_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="y0()";
      char args[]="double or integer";
      if (paramcount!=1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTDouble)&&(parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }

    sdff_result Y1(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="j1()";
      char args[]="double or integer";
      if (paramcount!=1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), name);
      }
      if ((parameter[0].type!=sdffRTDouble)&&(parameter[0].type!=sdffRTInteger)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      double d=parameter[0].doubleValue;
      if (parameter[0].type==sdffRTInteger) d=parameter[0].intValue;
      sdff_result r;
      r.type=sdffRTDouble;
      r.doubleValue=y1(d);
      return r;
    }

    bool Y1_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="y1()";
      char args[]="double or integer";
      if (paramcount!=1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTDouble)&&(parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }

    sdff_result YN(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="yn()";
      char args[]="double or integer";
      char args1[]="integer";
      char pcount=2;
      if (paramcount!=pcount) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, pcount), name);
      }
      if ((parameter[0].type!=sdffRTDouble)&&(parameter[0].type!=sdffRTInteger)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      if (parameter[1].type!=sdffRTInteger) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 2, args1), name);
      }
      double d=parameter[0].doubleValue;
      long long i=parameter[1].intValue;
      if (parameter[0].type==sdffRTInteger) d=parameter[0].intValue;
      sdff_result r;
      r.type=sdffRTDouble;
      r.doubleValue=yn(i,d);
      return r;
    }

    bool YN_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="yn()";
      char args[]="double or integer";
      char args1[]="integer";
      char pcount=2;
      if (paramcount!=pcount) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, pcount), name);
      }
      if ((parameter[0]!=sdffPTDouble)&&(parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[1]!=sdffPTInteger)&&(parameter[1]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 2, args1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }







    sdff_result ABS(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="abs()";
      char args[]="double or integer";
      if (paramcount!=1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), name);
      }
      if ((parameter[0].type!=sdffRTDouble)&&(parameter[0].type!=sdffRTInteger)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      sdff_result r;
      r.type=sdffRTVoid;
      if (parameter[0].type==sdffRTDouble) {
        r.type=sdffRTDouble;
        r.doubleValue=fabs(parameter[0].doubleValue);
      } else if (parameter[0].type==sdffRTInteger) {
        r.type=sdffRTInteger;
        r.intValue=abs(parameter[0].intValue);
      }
      return r;
    }

    bool ABS_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="abs()";
      char args[]="double or integer";
      if (paramcount!=1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTDouble)&&(parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }

    sdff_result ROUND(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="round()";
      char args[]="double or integer";
      if (paramcount!=1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), name);
      }
      if ((parameter[0].type!=sdffRTDouble)&&(parameter[0].type!=sdffRTInteger)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      double d=parameter[0].doubleValue;
      if (parameter[0].type==sdffRTInteger) d=parameter[0].intValue;
      sdff_result r;
      r.type=sdffRTInteger;
      r.intValue=(long long)round(d);
      return r;
    }

    bool ROUND_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="round()";
      char args[]="double or integer";
      if (paramcount!=1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTDouble)&&(parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }

    sdff_result FLOOR(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="floor()";
      char args[]="double or integer";
      if (paramcount!=1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), name);
      }
      if ((parameter[0].type!=sdffRTDouble)&&(parameter[0].type!=sdffRTInteger)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      double d=parameter[0].doubleValue;
      if (parameter[0].type==sdffRTInteger) d=parameter[0].intValue;
      sdff_result r;
      r.type=sdffRTInteger;
      r.intValue=(long long)floor(d);
      return r;
    }

    bool FLOOR_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="floor()";
      char args[]="double or integer";
      if (paramcount!=1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTDouble)&&(parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }

    sdff_result CEIL(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="ceil()";
      char args[]="double or integer";
      if (paramcount!=1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), name);
      }
      if ((parameter[0].type!=sdffRTDouble)&&(parameter[0].type!=sdffRTInteger)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      double d=parameter[0].doubleValue;
      if (parameter[0].type==sdffRTInteger) d=parameter[0].intValue;
      sdff_result r;
      r.type=sdffRTInteger;
      r.intValue=(long long)ceil(d);
      return r;
    }

    bool CEIL_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="ceil()";
      char args[]="double or integer";
      if (paramcount!=1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTDouble)&&(parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }

    sdff_result TRUNC(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="trunc(number)";
      char args[]="double or integer";
      if (paramcount!=1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), name);
      }
      if ((parameter[0].type!=sdffRTDouble)&&(parameter[0].type!=sdffRTInteger)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      double d=parameter[0].doubleValue;
      if (parameter[0].type==sdffRTInteger) d=parameter[0].intValue;
      sdff_result r;
      r.type=sdffRTInteger;
      r.intValue=(long long)trunc(d);
      return r;
    }

    bool TRUNC_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="trunc(number)";
      char args[]="double or integer";
      if (paramcount!=1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTDouble)&&(parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }

    sdff_result TOSTRING(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="tostring()";
      if (paramcount!=1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), name);
      }
      sdff_result r;
      r.type=sdffRTString;
      switch(parameter[0].type) {
        case sdffRTString:
          r.stringValue=parameter[0].stringValue;
          break;
        case sdffRTInteger:
          r.stringValue=inttostr(parameter[0].intValue);
          break;
        case sdffRTDouble:
          r.stringValue=floattostr(parameter[0].doubleValue);
          break;
        case sdffRTBoolean:
          r.stringValue=parameter[0].boolValue?"true":"false";
          break;
        case sdffRTVoid:
          r.stringValue="void";
          break;
      }
      return r;
    }

    bool TOSTRING_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="tostring()";
      if (paramcount!=1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }

    sdff_result STR2NUM(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="str2num(string)";
      char args[]="string";
      if (paramcount!=1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), name);
      }
      if ((parameter[0].type!=sdffRTString)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      sdff_result r;
      if (isInt(parameter[0].stringValue)) {
        r.type=sdffRTInteger;
        r.intValue=atol(parameter[0].stringValue.c_str());
      } else if (isFloat(parameter[0].stringValue)) {
        r.type=sdffRTDouble;
        r.doubleValue=atof(parameter[0].stringValue.c_str());
      } else RUN_TIME_ERROR(SEQUENCER_SDFFERROR_STR2NUMNOTNUM_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_STR2NUMNOTNUM_NUM), parameter[0].stringValue.c_str()), name);

      return r;
    }

    bool STR2NUM_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="str2num(string)";
      char args[]="string";
      if (paramcount!=1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTString)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }

    sdff_result STR2BOOL(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="str2bool(string)";
      char args[]="string";
      if (paramcount!=1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), name);
      }
      if ((parameter[0].type!=sdffRTString)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      sdff_result r;
      r.type=sdffRTBoolean;
      parameter[0].stringValue=tolower(parameter[0].stringValue);
      if ((parameter[0].stringValue=="true")||(parameter[0].stringValue=="ja")||(parameter[0].stringValue=="yes")||(parameter[0].stringValue=="t")||(parameter[0].stringValue=="j")||(parameter[0].stringValue=="y")||(parameter[0].stringValue=="1")) {
        r.boolValue=true;
      } else if ((parameter[0].stringValue=="false")||(parameter[0].stringValue=="nein")||(parameter[0].stringValue=="no")||(parameter[0].stringValue=="f")||(parameter[0].stringValue=="n")||(parameter[0].stringValue=="0")) {
        r.boolValue=false;
      } else RUN_TIME_ERROR(SEQUENCER_SDFFERROR_STR2BOOLNOTNUM_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_STR2NUMNOTNUM_NUM), parameter[0].stringValue.c_str()), name);

      return r;
    }

    bool STR2BOOL_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="str2bool(string)";
      char args[]="string";
      if (paramcount!=1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTString)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }

    sdff_result LENGTH(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="length(string)";
      char args[]="string";
      if (paramcount!=1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), name);
      }
      if ((parameter[0].type!=sdffRTString)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      sdff_result r;
      r.type=sdffRTInteger;
      r.intValue=parameter[0].stringValue.size();
      return r;
    }

    bool LENGTH_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="length(string)";
      char args[]="string";
      if (paramcount!=1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTString)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }

    sdff_result TOLOWER(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="tolower(string)";
      char args[]="string";
      if (paramcount!=1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), name);
      }
      if ((parameter[0].type!=sdffRTString)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      sdff_result r;
      r.type=sdffRTString;
      r.stringValue=tolower(parameter[0].stringValue);
      return r;
    }

    bool TOLOWER_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="tolower(string)";
      char args[]="string";
      if (paramcount!=1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTString)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }

    sdff_result TOUPPER(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="toupper(string)";
      char args[]="string";
      if (paramcount!=1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), name);
      }
      if ((parameter[0].type!=sdffRTString)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      sdff_result r;
      r.type=sdffRTString;
      r.stringValue=toupper(parameter[0].stringValue);
      return r;
    }

    bool TOUPPER_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="toupper(string)";
      char args[]="string";
      if (paramcount!=1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTString)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }

    sdff_result STRIP(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="strip(string)";
      char args[]="string";
      if (paramcount!=1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), name);
      }
      if ((parameter[0].type!=sdffRTString)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      sdff_result r;
      r.type=sdffRTString;
      r.stringValue=strstrip(parameter[0].stringValue);
      return r;
    }

    bool STRIP_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      char name[]="strip(string)";
      char args[]="string";
      if (paramcount!=1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTString)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }

    sdff_result STRGET(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      char name[]="strget(string, index)";
      char args[]="string";
      char argi[]="integer";
      int pcount=2;
      if (paramcount!=pcount) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name, paramcount, pcount), name);
      }
      if ((parameter[0].type!=sdffRTString)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      if ((parameter[1].type!=sdffRTInteger)) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name, sdff_resulttypetostr(parameter[0].type).c_str(), 2, argi), name);
      }
      int item=parameter[1].intValue;
      if (item<0 || item>=(long)parameter[0].stringValue.size()) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_RANGE_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_RANGE_NUM), name), name);
      }
      sdff_result r;
      r.type=sdffRTString;
      r.stringValue=parameter[0].stringValue[item];
      return r;
    }

    bool STRGET_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      std::string name=fname+"(string, index)";
      //char args[]="string";
      //char argi[]="integer";

      SDFFFUNC_CHECK_ARGCOUNT(2, name);
      SDFFFUNC_CHECK_STRINGARGTYPE(0, name, "");
      SDFFFUNC_CHECK_INTARGTYPE(0, name, "");

      return true;
    }


    sdff_result RAND(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      std::string name=fname+"([max])";
      char args[]="number";
      if (paramcount>1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTOOMANY_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTOOMANY_NUM), name.c_str(), paramcount, 1), name);
      }
      sdff_result r;
      if (paramcount==0) {
        r.type=sdffRTDouble;
        r.doubleValue=parser->get_mersenne()->rand();
      } else {
          if ((parameter[0].type!=sdffRTInteger)&&(parameter[0].type!=sdffRTDouble)) {
            RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str(), sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name.c_str());
          }
          double maxx=parameter[0].doubleValue;
          if (parameter[0].type==sdffRTInteger) maxx=parameter[0].intValue;
          r.type=sdffRTDouble;
          r.doubleValue=parser->get_mersenne()->rand(maxx);
      }
      return r;
    }

    bool RAND_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      std::string name=fname+"([max])";
      char args[]="number";
      if (paramcount>1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTOOMANY_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTOOMANY_NUM), name.c_str(), paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTOOMANY_NUM), name.c_str()), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if (paramcount>0) if ((parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTDouble)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str(), sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str()), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }

    sdff_result RANDINT(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      std::string name=fname+"([maxInt])";
      char args[]="integer";
      if (paramcount>1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTOOMANY_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTOOMANY_NUM), name.c_str(), paramcount, 1), name);
      }
      sdff_result r;
      if (paramcount==0) {
        r.type=sdffRTInteger;
        r.intValue=parser->get_mersenne()->randInt();
      } else {
          if ((parameter[0].type!=sdffRTInteger)) {
            RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str(), sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name.c_str());
          }
          unsigned long maxx=parameter[0].intValue;
          r.type=sdffRTInteger;
          r.intValue=parser->get_mersenne()->randInt(maxx);
      }
      return r;
    }

    bool RANDINT_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      std::string name=fname+"([maxInt])";
      char args[]="integer";
      if (paramcount>1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTOOMANY_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTOOMANY_NUM), name.c_str(), paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTOOMANY_NUM), name.c_str()), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if (paramcount>0) if ((parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str(), sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str()), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }

    sdff_result RANDGAUSS(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      std::string name=fname+"([mean[, variance]])";
      char args[]="number";
      char argt[]="variance";
      if (paramcount>2) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTOOMANY_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTOOMANY_NUM), name.c_str(), paramcount, 2), name);
      }
      sdff_result r;
      r.type=sdffRTDouble;
      if (paramcount==0) {
          r.doubleValue=parser->get_mersenne()->randNorm(0.0, 1.0);
      } else if (paramcount==1) {
          int p=0;
          if ((parameter[p].type!=sdffRTInteger)&&(parameter[p].type!=sdffRTDouble)) {
            RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str(), sdff_resulttypetostr(parameter[p].type).c_str(), p+1, args), name.c_str());
          }
          double mean=parameter[0].doubleValue;
          if (parameter[0].type==sdffRTInteger) mean=parameter[0].intValue;
          r.type=sdffRTDouble;
          r.doubleValue=parser->get_mersenne()->randNorm(mean, 1.0);
      } else {
          int p=0;
          if ((parameter[p].type!=sdffRTInteger)&&(parameter[p].type!=sdffRTDouble)) {
            RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str(), sdff_resulttypetostr(parameter[p].type).c_str(), p+1, args), name.c_str());
          }
          p=1;
          if ((parameter[p].type!=sdffRTInteger)&&(parameter[p].type!=sdffRTDouble)) {
            RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str(), sdff_resulttypetostr(parameter[p].type).c_str(), p+1, args), name.c_str());
          }
          double mean=parameter[0].doubleValue;
          if (parameter[0].type==sdffRTInteger) mean=parameter[0].intValue;
          double var=parameter[1].doubleValue;
          if (var<0) {
            RUN_TIME_ERROR(SEQUENCER_SDFFERROR_NONNEG_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_NONNEG_NUM), name.c_str(), argt), name.c_str());
          }
          if (parameter[1].type==sdffRTInteger) var=parameter[1].intValue;
          r.type=sdffRTDouble;
          r.doubleValue=parser->get_mersenne()->randNorm(mean, var);
      }
      return r;
    }

    bool RANDGAUSS_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      std::string name=fname+"([mean[, variance]])";
      char args[]="number";
      if (paramcount>2) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTOOMANY_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTOOMANY_NUM), name.c_str(), paramcount, 2), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTOOMANY_NUM), name.c_str()), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      int p=0;
      if (paramcount>=1) {
          if ((parameter[p]!=sdffPTInteger)&&(parameter[p]!=sdffPTDouble)&&(parameter[p]!=sdffPTUnknown)) {
            parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str(), sdff_paramtypetostr(parameter[p]).c_str(), p+1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str()), parser->get_current_filename(), parser->get_current_line());
            return false;
          }
          p=1;
          if (paramcount==2) {
              if ((parameter[p]!=sdffPTInteger)&&(parameter[p]!=sdffPTDouble)&&(parameter[p]!=sdffPTUnknown)) {
                parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str(), sdff_paramtypetostr(parameter[p]).c_str(), p+1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str()), parser->get_current_filename(), parser->get_current_line());
                return false;
              }
          }
      }
      return true;
    }


    sdff_result RANDSEED(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      std::string name=fname+"([seed])";
      char args[]="integer";
      char argt[]="seed value";
      if (paramcount>1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTOOMANY_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTOOMANY_NUM), name.c_str(), paramcount, 1), name);
      }
      sdff_result r;
      r.type=sdffRTVoid;
      if (paramcount==0) {
        parser->get_mersenne()->seed();
      } else {
          if ((parameter[0].type!=sdffRTInteger)) {
            RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str(), sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name.c_str());
          } else if (parameter[0].intValue<0) {
            RUN_TIME_ERROR(SEQUENCER_SDFFERROR_NONNEG_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_NONNEG_NUM), name.c_str(), argt), name.c_str());
          }
          unsigned long s=parameter[0].intValue;
          parser->get_mersenne()->seed(s);
      }
      return r;
    }

    bool RANDSEED_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      std::string name=fname+"([seed])";
      char args[]="integer";
      if (paramcount>1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTOOMANY_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTOOMANY_NUM), name.c_str(), paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTOOMANY_NUM), name.c_str()), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str(), sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str()), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }




    sdff_result TOHEX(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      std::string name=fname+"(integer)";
      char args[]="integer";
      if (paramcount!=1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name.c_str(), paramcount, 1), name);
      }
      if (parameter[0].type!=sdffRTInteger) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str(), sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      long long d=parameter[0].intValue;
      sdff_result r;
      r.type=sdffRTString;
      r.stringValue="";
      int len=sizeof(d);
      bool printed=false;
      for (int i=len*2-1; i>=0; i--) {
          char v=(d & (0xF << (i*4))) >> (i*4);
          if (v!=0 || printed) {
            switch(v) {
                case 0: r.stringValue+="0"; break;
                case 1: r.stringValue+="1"; break;
                case 2: r.stringValue+="2"; break;
                case 3: r.stringValue+="3"; break;
                case 4: r.stringValue+="4"; break;
                case 5: r.stringValue+="5"; break;
                case 6: r.stringValue+="6"; break;
                case 7: r.stringValue+="7"; break;
                case 8: r.stringValue+="8"; break;
                case 9: r.stringValue+="9"; break;
                case 10: r.stringValue+="A"; break;
                case 11: r.stringValue+="B"; break;
                case 12: r.stringValue+="C"; break;
                case 13: r.stringValue+="D"; break;
                case 14: r.stringValue+="E"; break;
                case 15: r.stringValue+="F"; break;
            }
            printed=true;
          }
      }
      return r;
    }

    bool TOHEX_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      std::string name=fname+"(integer)";
      char args[]="integer";
      if (paramcount!=1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name.c_str(), paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name.c_str()), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str(), sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str()), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }




    sdff_result TOBIN(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      std::string name=fname+"(integer)";
      char args[]="integer";
      if (paramcount!=1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name.c_str(), paramcount, 1), name);
      }
      if (parameter[0].type!=sdffRTInteger) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str(), sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      long long d=parameter[0].intValue;
      sdff_result r;
      r.type=sdffRTString;
      r.stringValue="";
      int len=sizeof(d);
      bool printed=false;
      for (int i=len*8-1; i>=0; i--) {
          char v=(d & (0x1 << (i))) >> (i);
          if (v!=0 || printed) {
            switch(v) {
                case 0: r.stringValue+="0"; break;
                case 1: r.stringValue+="1"; break;
            }
            printed=true;
          }
      }
      return r;
    }

    bool TOBIN_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      std::string name=fname+"(integer)";
      char args[]="integer";
      if (paramcount!=1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name.c_str(), paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name.c_str()), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str(), sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str()), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }




    sdff_result CHAR(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      std::string name=fname+"(integer)";

      SDFFFUNC_CHECK_ARGCOUNT_RT(1, name);
      SDFFFUNC_CHECK_INTARGTYPE_RT(0, name, "character number");

      long long d=parameter[0].intValue;
      if (d<0 || d>255) {
          RUN_TIME_ERROR(SEQUENCER_SDFFERROR_NOTASCII_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_NOTASCII_NUM), name.c_str(), d), name.c_str());
      }
      sdff_result r;
      r.type=sdffRTString;
      r.stringValue=" ";
      r.stringValue[0]=d;
      return r;
    }

    bool CHAR_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      std::string name=fname+"(integer)";

      SDFFFUNC_CHECK_ARGCOUNT(1, name);
      SDFFFUNC_CHECK_INTARGTYPE(0, name, "character number");

      return true;
    }



    sdff_result ISOFTYPE(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      std::string name=fname+"(value)";

      SDFFFUNC_CHECK_ARGCOUNT_RT(1, name);

      //long long d=parameter[0].intValue;
      sdff_result r;
      r.type=sdffRTBoolean;
      r.boolValue=false;
      if (fname=="isstring") r.boolValue=(parameter[0].type==sdffRTString);
      else if (fname=="isinteger" || fname=="isint") r.boolValue=(parameter[0].type==sdffRTInteger);
      else if (fname=="isdouble") r.boolValue=(parameter[0].type==sdffRTInteger || parameter[0].type==sdffRTDouble);
      else if (fname=="isboolean" || fname=="isbool") r.boolValue=(parameter[0].type==sdffRTBoolean);
      else if (fname=="isvoid") r.boolValue=(parameter[0].type==sdffRTVoid);
      return r;
    }

    bool ISOFTYPE_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      std::string name=fname+"(value)";

      SDFFFUNC_CHECK_ARGCOUNT(1, name);

      return true;
    }



    sdff_result FOPEN(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      std::string name=fname+"(filename[, mode])";
      char args[]="string";
      if (paramcount<1 || paramcount>2) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTOOMANY_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTOOMANY_NUM), name.c_str(), paramcount, 2), name);
      }
      sdff_result r;
      r.type=sdffRTInteger;
      if (paramcount==1) {
          int p=0;
          if (parameter[p].type!=sdffRTString) {
            RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str(), sdff_resulttypetostr(parameter[p].type).c_str(), p+1, args), name.c_str());
          }
          FILE* f=fopen(parameter[0].stringValue.c_str(), "w");
          if (f==NULL) {
            char errmess[1000];
            sprintf(errmess, "could not open file '%s' for writing", parameter[0].stringValue.c_str());
            RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FILEOPERROR_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FILEOPERROR_NUM), name.c_str(), errmess), name.c_str());
          }
          r.intValue=(long long)f;
      } else {
          int p=0;
          if (parameter[p].type!=sdffRTString) {
            RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str(), sdff_resulttypetostr(parameter[p].type).c_str(), p+1, args), name.c_str());
          }
          p=1;
          if (parameter[p].type!=sdffRTString) {
            RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str(), sdff_resulttypetostr(parameter[p].type).c_str(), p+1, args), name.c_str());
          }
          FILE* f=fopen(parameter[0].stringValue.c_str(), parameter[1].stringValue.c_str());
          if (f==NULL) {
            char errmess[1000];
            sprintf(errmess, "could not open file '%s' in mode '%s'", parameter[0].stringValue.c_str(), parameter[1].stringValue.c_str());
            RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FILEOPERROR_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FILEOPERROR_NUM), name.c_str(), errmess), name.c_str());
          }
          r.intValue=(long long)f;
      }
      return r;
    }

    bool FOPEN_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      std::string name=fname+"(filename[, mode])";
      char args[]="string";
      if (paramcount<1 || paramcount>2) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTOOMANY_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTOOMANY_NUM), name.c_str(), paramcount, 2), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTOOMANY_NUM), name.c_str()), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      int p=0;
      if (paramcount>=1) {
          if ((parameter[p]!=sdffPTString)&&(parameter[p]!=sdffPTUnknown)) {
            parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str(), sdff_paramtypetostr(parameter[p]).c_str(), p+1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str()), parser->get_current_filename(), parser->get_current_line());
            return false;
          }
          p=1;
          if (paramcount==2) {
              if ((parameter[p]!=sdffPTString)&&(parameter[p]!=sdffPTUnknown)) {
                parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str(), sdff_paramtypetostr(parameter[p]).c_str(), p+1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str()), parser->get_current_filename(), parser->get_current_line());
                return false;
              }
          }
      }
      return true;
    }




    sdff_result FCLOSE(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      std::string name=fname+"(handle)";
      char args[]="integer (handle)";
      if (paramcount!=1) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name.c_str(), paramcount, 1), name);
      }
      if (parameter[0].type!=sdffRTInteger) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str(), sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      FILE* f=(FILE*)parameter[0].intValue;
      sdff_result r;
      r.type=sdffRTVoid;
      fclose(f);
      return r;
    }

    bool FCLOSE_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      std::string name=fname+"(handle)";
      char args[]="integer (handle)";
      if (paramcount!=1) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name.c_str(), paramcount, 1), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name.c_str()), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str(), sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str()), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }




    sdff_result FWRITELN(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      std::string name=fname+"(file_handle, data[, data, ...])";
      char args[]="integer (handle)";

      if (paramcount<2) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name.c_str(), paramcount, 2), name);
      }
      if (parameter[0].type!=sdffRTInteger) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str(), sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      std::string data="";
      for (int i=1; i<paramcount; i++) {
        if (parameter[i].type==sdffRTString)
          data+=deescapify(parameter[i].stringValue);
        else
          data+=aasdff_resulttostr(parameter[i]);
      }
      FILE* f=(FILE*)parameter[0].intValue;
      if (f!=NULL){
        fprintf(f, "%s\n", data.c_str());
      } else {
        char errmess[]="you did not supply a valid handle";
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FILEOPERROR_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FILEOPERROR_NUM), name.c_str(), errmess), name.c_str());

      }
      sdff_result r;
      r.type=sdffRTVoid;
      return r;
    }

    bool FWRITELN_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      std::string name=fname+"(file_handle, data[, data, ...])";
      char args[]="integer (handle)";
      if (paramcount<2) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name.c_str(), paramcount, 2), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name.c_str()), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str(), sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str()), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }

    sdff_result FWRITE(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      std::string name=fname+"(file_handle, data[, data, ...])";
      char args[]="integer (handle)";
      if (paramcount<2) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name.c_str(), paramcount, 2), name);
      }
      if (parameter[0].type!=sdffRTInteger) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str(), sdff_resulttypetostr(parameter[0].type).c_str(), 1, args), name);
      }
      std::string data="";
      for (int i=1; i<paramcount; i++) {
        if (parameter[i].type==sdffRTString)
          data+=deescapify(parameter[i].stringValue);
        else
          data+=aasdff_resulttostr(parameter[i]);
      }
      FILE* f=(FILE*)parameter[0].intValue;
      if (f!=NULL){
        fprintf(f, "%s", data.c_str());
      } else {
        char errmess[]="you did not supply a valid handle";
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FILEOPERROR_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FILEOPERROR_NUM), name.c_str(), errmess), name.c_str());

      }
      sdff_result r;
      r.type=sdffRTVoid;
      return r;
    }

    bool FWRITE_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      std::string name=fname+"(file_handle, data[, data, ...])";
      char args[]="integer (handle)";
      if (paramcount<2) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name.c_str(), paramcount, 2), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), name.c_str()), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      if ((parameter[0]!=sdffPTInteger)&&(parameter[0]!=sdffPTUnknown)) {
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str(), sdff_paramtypetostr(parameter[0]).c_str(), 1, args), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str()), parser->get_current_filename(), parser->get_current_line());
        return false;
      }
      return true;
    }



    sdff_result SEROPEN(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      std::string name=fname+"(com_port, baud_rate=9600, databits=8, parity='none', stopbits=1, handshaking='none')";
      char args[]="string";
      char argi[]="integer";

      if (!parser->get_inside_preevent()) {
          RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM), name.c_str()), name);
      }

      SDFFFUNC_CHECK_ARGCOUNT_MINMAX_RT(1, 6, name);

      std::string port="";
      unsigned long baudrate=9600;
      unsigned char databits=8;
      double stopbits=1;
      sdff_serial_paritymodes parity=sdffserNOPARITY;
      sdff_serial_handshaking handshaking=sdffserNOHANDSHAKING;
      int par=0;
      if (parameter[par].type!=sdffRTString) {
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str(), sdff_resulttypetostr(parameter[par].type).c_str(), par+1, args), name);
      } else {
          port=parameter[par].stringValue;
      }
      par=1;
      if (paramcount>par) {
          if (parameter[par].type!=sdffRTInteger) {
            RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str(), sdff_resulttypetostr(parameter[par].type).c_str(), par+1, argi), name);
          } else {
              baudrate=parameter[par].intValue;
              if (baudrate<1) RUN_TIME_ERROR(SEQUENCER_SERERROR_PARAMPOS_NUM, format(get_errormessage(SEQUENCER_SERERROR_PARAMPOS_NUM), "baudrate"), name);
          }
      }
      par=2;
      if (paramcount>par) {
          if (parameter[par].type!=sdffRTInteger) {
            RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str(), sdff_resulttypetostr(parameter[par].type).c_str(), par+1, argi), name);
          } else {
              databits=parameter[par].intValue;
              if (databits<1) RUN_TIME_ERROR(SEQUENCER_SERERROR_PARAMPOS_NUM, format(get_errormessage(SEQUENCER_SERERROR_PARAMPOS_NUM), "databits"), name);
          }
      }
      par=3;
      if (paramcount>par) {
          if (parameter[par].type!=sdffRTString) {
            RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str(), sdff_resulttypetostr(parameter[par].type).c_str(), par+1, args), name);
          } else {
              std::string p=tolower(parameter[par].stringValue);
              if (p=="none" || p=="off") parity=sdffserNOPARITY;
              else if (p=="even") parity=sdffserEVEN;
              else if (p=="odd") parity=sdffserODD;
              else RUN_TIME_ERRORN(SEQUENCER_SERERROR_UNKNOWNPARITY_NUM, name);
          }
      }
      par=4;
      if (paramcount>par) {
          if (parameter[par].type!=sdffRTInteger && parameter[par].type!=sdffRTDouble) {
            RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str(), sdff_resulttypetostr(parameter[par].type).c_str(), par+1, argi), name);
          } else {
              stopbits=parameter[par].intValue;
              if (parameter[par].type==sdffRTDouble) stopbits=parameter[par].doubleValue;
              if (stopbits<1) RUN_TIME_ERROR(SEQUENCER_SERERROR_PARAMPOS_NUM, format(get_errormessage(SEQUENCER_SERERROR_PARAMPOS_NUM), "stopbits"), name);
          }
      }
      par=5;
      if (paramcount>par) {
          if (parameter[par].type!=sdffRTString) {
            RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str(), sdff_resulttypetostr(parameter[par].type).c_str(), par+1, args), name);
          } else {
              std::string p=tolower(parameter[par].stringValue);
              if (p=="none" || p=="off") handshaking=sdffserNOHANDSHAKING;
              else if (p=="hardware") handshaking=sdffserHARDWARE;
              else if (p=="software" || p=="xonxoff") handshaking=sdffserXONXOFF;
              else RUN_TIME_ERRORN(SEQUENCER_SERERROR_UNKNOWNHANDSHAKING_NUM, name);
          }
      }

      SDFFserialOpenCommand* cmd=new SDFFserialOpenCommand(0, parser->get_current_filename(), parser->get_current_line(), port);
      cmd->set_baudrate(baudrate);
      cmd->set_databits(databits);
      cmd->set_parity(parity);
      cmd->set_stopbits(stopbits);
      cmd->set_handshaking(handshaking);
      parser->get_cmd_manager()->add_command(cmd);
      sdff_result r;
      r.type=sdffRTVoid;
      return r;
    }

    bool SEROPEN_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      std::string name=fname+"(com_port, baud_rate=9600, databits=8, parity='none', stopbits=1)";

      if (!parser->get_inside_preevent()) {
          parser->add_error_p(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM), name.c_str()), get_errorsmessage(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM), parser->get_current_filename(), parser->get_current_line());
      }

      SDFFFUNC_CHECK_ARGCOUNT_MINMAX(1, 5, name);
      SDFFFUNC_CHECK_STRINGARGTYPE(0, name, "serial port");
      if (paramcount>1) { SDFFFUNC_CHECK_INTARGTYPE(1, name, "baud rate"); }
      if (paramcount>2) { SDFFFUNC_CHECK_INTARGTYPE(2, name, "data bits"); }
      if (paramcount>3) { SDFFFUNC_CHECK_STRINGARGTYPE(3, name, "parity = 'none'|'even'|'odd'"); }
      if (paramcount>4) { SDFFFUNC_CHECK_INTARGTYPE(4, name, "stop bits"); }

      return true;
    }



    sdff_result SERCLOSE(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      std::string name=fname+"(com_port)";

      if (!parser->get_inside_preevent()) {
          RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM), name.c_str()), name);
      }

      SDFFFUNC_CHECK_ARGCOUNT_RT(1, name);
      SDFFFUNC_CHECK_STRINGARGTYPE_RT(0, name, "serial port");
      std::string port=parameter[0].stringValue;

      SDFFserialCloseCommand* cmd=new SDFFserialCloseCommand(0, parser->get_current_filename(), parser->get_current_line(), port);
      parser->get_cmd_manager()->add_command(cmd);
      sdff_result r;
      r.type=sdffRTVoid;
      return r;
    }

    bool SERCLOSE_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      std::string name=fname+"(com_port)";

      if (!parser->get_inside_preevent()) {
          parser->add_error_p(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM), name.c_str()), get_errorsmessage(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM), parser->get_current_filename(), parser->get_current_line());
      }

      SDFFFUNC_CHECK_ARGCOUNT(1, name);
      SDFFFUNC_CHECK_STRINGARGTYPE(0, name, "serial port");

      return true;
    }


    sdff_result SERWRITE(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      std::string name=fname+"(com_port, data[, data, ...])";

      if (!parser->get_inside_preevent()) {
          RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM), name.c_str()), name);
      }

      SDFFFUNC_CHECK_ARGCOUNT_ATLEAST_RT(2, name);
      SDFFFUNC_CHECK_STRINGARGTYPE_RT(0, name, "serial port");
      std::string port=parameter[0].stringValue;

      std::string data="";
      for (int i=1; i<paramcount; i++) {
        if (parameter[i].type==sdffRTString)
          data+=deescapify(parameter[i].stringValue);
        else
          data+=aasdff_resulttostr(parameter[i]);
      }

      if (tolower(fname)=="serial_writeln") data+="\n";
      if (tolower(fname)=="serwriteln") data+="\n";

      SDFFserialSendCommand* cmd=new SDFFserialSendCommand(0, parser->get_current_filename(), parser->get_current_line(), port);
      cmd->set_text(data);
      parser->get_cmd_manager()->add_command(cmd);

      sdff_result r;
      r.type=sdffRTVoid;
      return r;
    }

    bool SERWRITE_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      std::string name=fname+"(com_port, data[, data, ...])";

      if (!parser->get_inside_preevent()) {
          parser->add_error_p(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM), name.c_str()), get_errorsmessage(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM), parser->get_current_filename(), parser->get_current_line());
      }

      SDFFFUNC_CHECK_ARGCOUNT_ATLEAST(2, name);
      SDFFFUNC_CHECK_STRINGARGTYPE(0, name, "serial port");

      return true;
    }

    sdff_result SERREAD(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      std::string name=fname+"(com_port, byte_count|end_char)";
      std::string arg2="integer (byte_count) or string (end_char)";

      if (!parser->get_inside_preevent()) {
          RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM), name.c_str()), name);
      }

      SDFFFUNC_CHECK_ARGCOUNT_MINMAX_RT(1, 2, name);
      SDFFFUNC_CHECK_STRINGARGTYPE_RT(0, name, "serial port");
      std::string port=parameter[0].stringValue;
      std::string end_char="\n";
      int byte_count=-1;
      if (paramcount==2) {
          if (parameter[1].type==sdffRTString) {
              end_char=parameter[1].stringValue;
              byte_count=-1;
          } else if (parameter[1].type==sdffRTInteger) {
              end_char="";
              byte_count=parameter[1].intValue;
          } else {
              RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), name.c_str(), sdff_resulttypetostr(parameter[1].type).c_str(), 2, arg2.c_str()), name);
          }
      }


      SDFFserialReadCommand* cmd=new SDFFserialReadCommand(0, parser->get_current_filename(), parser->get_current_line(), port);
      cmd->set_end_char(end_char[0]);
      cmd->set_byte_count(byte_count);
      parser->get_cmd_manager()->add_command(cmd);

      sdff_result r;
      r.type=sdffRTVoid;
      return r;
    }

    bool SERREAD_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      std::string name=fname+"(com_port, byte_count|end_char)";

      if (!parser->get_inside_preevent()) {
          parser->add_error_p(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM), name.c_str()), get_errorsmessage(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM), parser->get_current_filename(), parser->get_current_line());
      }

      SDFFFUNC_CHECK_ARGCOUNT_MINMAX(1, 2, name);
      SDFFFUNC_CHECK_STRINGARGTYPE(0, name, "serial port");
      if (paramcount==2) SDFFFUNC_CHECK_INTORSTRINGARGTYPE(1, name, "byte_count or end_char");

      return true;
    }








    sdff_result SERGPIBSEND(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      std::string name=fname+"(device, data[, data, ...])";

      if (!parser->get_inside_preevent()) {
          RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM), name.c_str()), name);
      }

      SDFFFUNC_CHECK_ARGCOUNT_ATLEAST_RT(2, name);
      SDFFFUNC_CHECK_INTARGTYPE_RT(0, name, "GPIB device");
      char device=parameter[0].intValue;
      if (device<0 || device>31) {
        RUN_TIME_ERROR(SEQUENCER_GPIBERROR_INVALIDADR_NUM, format(get_errormessage(SEQUENCER_GPIBERROR_INVALIDADR_NUM), name.c_str()), name);
      }

      std::string data="";
      for (int i=1; i<paramcount; i++) {
        if (parameter[i].type==sdffRTString)
          data+=deescapify(parameter[i].stringValue);
        else
          data+=aasdff_resulttostr(parameter[i]);
      }

      SDFFserGPIBsendCommand* cmd=new SDFFserGPIBsendCommand(0, device, data, parser->get_current_filename(), parser->get_current_line());
      parser->get_cmd_manager()->add_command(cmd);

      sdff_result r;
      r.type=sdffRTVoid;
      return r;
    }

    bool SERGPIBSEND_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      std::string name=fname+"(device, data[, data, ...])";

      if (!parser->get_inside_preevent()) {
          parser->add_error_p(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM), name.c_str()), get_errorsmessage(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM), parser->get_current_filename(), parser->get_current_line());
      }

      SDFFFUNC_CHECK_ARGCOUNT_ATLEAST(2, name);
      SDFFFUNC_CHECK_INTARGTYPE(0, name, "GPIB device");

      return true;
    }

    sdff_result SERGPIBQUERY(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      std::string name=fname+"(device, data[, data, ...])";

      if (!parser->get_inside_preevent()) {
          RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM), name.c_str()), name);
      }

      SDFFFUNC_CHECK_ARGCOUNT_ATLEAST_RT(2, name);
      SDFFFUNC_CHECK_INTARGTYPE_RT(0, name, "GPIB device");
      char device=parameter[0].intValue;
      if (device<0 || device>31) {
        RUN_TIME_ERROR(SEQUENCER_GPIBERROR_INVALIDADR_NUM, format(get_errormessage(SEQUENCER_GPIBERROR_INVALIDADR_NUM), name.c_str()), name);
      }

      std::string data="";
      for (int i=1; i<paramcount; i++) {
        if (parameter[i].type==sdffRTString)
          data+=deescapify(parameter[i].stringValue);
        else
          data+=aasdff_resulttostr(parameter[i]);
      }

      SDFFserGPIBqueryCommand* cmd=new SDFFserGPIBqueryCommand(0, device, data, parser->get_current_filename(), parser->get_current_line());
      parser->get_cmd_manager()->add_command(cmd);

      sdff_result r;
      r.type=sdffRTVoid;
      return r;
    }

    bool SERGPIBQUERY_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      std::string name=fname+"(device, data[, data, ...])";

      if (!parser->get_inside_preevent()) {
          parser->add_error_p(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM), name.c_str()), get_errorsmessage(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM), parser->get_current_filename(), parser->get_current_line());
      }

      SDFFFUNC_CHECK_ARGCOUNT_ATLEAST(2, name);
      SDFFFUNC_CHECK_INTARGTYPE(0, name, "GPIB device");

      return true;
    }

    sdff_result SERGPIBREAD(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      std::string name=fname+"(device)";

      SDFFFUNC_CHECK_ARGCOUNT_RT(1, name);
      SDFFFUNC_CHECK_INTARGTYPE_RT(0, name, "GPIB device");
      char device=parameter[0].intValue;
      if (device<0 || device>31) {
        RUN_TIME_ERROR(SEQUENCER_GPIBERROR_INVALIDADR_NUM, format(get_errormessage(SEQUENCER_GPIBERROR_INVALIDADR_NUM), name.c_str()), name);
      }


      SDFFserGPIBreadCommand* cmd=new SDFFserGPIBreadCommand(0, device, parser->get_current_filename(), parser->get_current_line());
      parser->get_cmd_manager()->add_command(cmd);

      sdff_result r;
      r.type=sdffRTVoid;
      return r;
    }

    bool SERGPIBREAD_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      std::string name=fname+"(device)";

      if (!parser->get_inside_preevent()) {
          parser->add_error_p(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM), name.c_str()), get_errorsmessage(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM), parser->get_current_filename(), parser->get_current_line());
      }

      SDFFFUNC_CHECK_ARGCOUNT(1, name);
      SDFFFUNC_CHECK_INTARGTYPE(0, name, "GPIB device");

      return true;
    }

    sdff_result SERGPIBSENDCMD(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      std::string name=fname+"(device)";

      if (!parser->get_inside_preevent()) {
          RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM), name.c_str()), name);
      }

      SDFFFUNC_CHECK_ARGCOUNT_RT(1, name);
      SDFFFUNC_CHECK_INTARGTYPE_RT(0, name, "GPIB device");
      char device=parameter[0].intValue;
      if (device<0 || device>31) {
        RUN_TIME_ERROR(SEQUENCER_GPIBERROR_INVALIDADR_NUM, format(get_errormessage(SEQUENCER_GPIBERROR_INVALIDADR_NUM), name.c_str()), name);
      }
      unsigned int command=0xFFFF;
      if (fname=="sergpib_send_DCL")  command=GPIB_BUSCOMMAND_DCL ;
      else if (fname=="sergpib_send_SDC")  command=GPIB_BUSCOMMAND_SDC ;
      else if (fname=="sergpib_send_GET ")  command=GPIB_BUSCOMMAND_GET  ;
      else if (fname=="sergpib_send_GTL")  command=GPIB_BUSCOMMAND_GTL ;
      else if (fname=="sergpib_send_LLO")  command=GPIB_BUSCOMMAND_LLO ;
      else if (fname=="sergpib_send_PPC")  command=GPIB_BUSCOMMAND_PPC ;
      else if (fname=="sergpib_send_PPU")  command=GPIB_BUSCOMMAND_PPU ;
      else if (fname=="sergpib_send_UNT")  command=GPIB_BUSCOMMAND_UNT ;
      else if (fname=="sergpib_send_UNL")  command=GPIB_BUSCOMMAND_UNL ;
      else if (fname=="sergpib_send_SPD")  command=GPIB_BUSCOMMAND_SPD ;
      else if (fname=="sergpib_send_SPE")  command=GPIB_BUSCOMMAND_SPE ;
      else if (fname=="sergpib_send_TCT")  command=GPIB_BUSCOMMAND_TCT ;
      else if (fname=="sergpib_send_IFC")  command=GPIB_BUSCOMMAND_IFC ;

      SDFFserGPIBsendBusCommand* cmd=new SDFFserGPIBsendBusCommand(0, device, command, parser->get_current_filename(), parser->get_current_line());
      parser->get_cmd_manager()->add_command(cmd);

      sdff_result r;
      r.type=sdffRTVoid;
      return r;
    }

    bool SERGPIBSENDCMD_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      std::string name=fname+"()";

      if (!parser->get_inside_preevent()) {
          parser->add_error_p(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM), name.c_str()), get_errorsmessage(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM), parser->get_current_filename(), parser->get_current_line());
      }

      SDFFFUNC_CHECK_ARGCOUNT(1, name);
      SDFFFUNC_CHECK_INTARGTYPE(0, name, "GPIB device");

      return true;
    }


   sdff_result SERGPIBCONFIG(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      std::string name=fname+"(interface_address, use_EOI, REN_talkstate, REN_readstate, send_LB, query_delay)";

      if (!parser->get_inside_preevent()) {
          RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM), name.c_str()), name);
      }

      SDFFFUNC_CHECK_ARGCOUNT_RT(6, name);
      SDFFFUNC_CHECK_INTARGTYPE_RT(0, name, "GPIB interface device adress");
      SDFFFUNC_CHECK_BOOLARGTYPE_RT(1, name, "use EOI line");
      SDFFFUNC_CHECK_BOOLARGTYPE_RT(2, name, "state of REN during talk operations");
      SDFFFUNC_CHECK_BOOLARGTYPE_RT(3, name, "state of REN during read operations");
      SDFFFUNC_CHECK_BOOLARGTYPE_RT(4, name, "send linebreak after data");
      SDFFFUNC_CHECK_INTARGTYPE_RT(5, name, "delay in query commands [msecs]");

      char interface_address=parameter[0].intValue;
      if (interface_address<0 || interface_address>31) {
        RUN_TIME_ERROR(SEQUENCER_GPIBERROR_INVALIDADR_NUM, format(get_errormessage(SEQUENCER_GPIBERROR_INVALIDADR_NUM), name.c_str()), name);
      }
      bool use_EOI=parameter[1].boolValue;
      bool REN_talkstate=parameter[2].boolValue;
      bool REN_readstate=parameter[3].boolValue;
      bool send_LB=parameter[4].boolValue;
      unsigned char query_delay=parameter[5].intValue;

      SDFFserGPIBconfigureCommand* cmd=new SDFFserGPIBconfigureCommand(0, parser->get_current_filename(), parser->get_current_line());
      cmd->set_interface_address(interface_address);
      cmd->set_use_EOI(use_EOI);
      cmd->set_REN_talkstate(REN_talkstate);
      cmd->set_REN_readstate(REN_readstate);
      cmd->set_send_LB(send_LB);
      cmd->set_query_delay(query_delay);
      parser->get_cmd_manager()->add_command(cmd);

      sdff_result r;
      r.type=sdffRTVoid;
      return r;
    }

    bool SERGPIBCONFIG_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      std::string name=fname+"(interface_address, use_EOI, REN_talkstate, REN_readstate, send_LB, query_delay)";

      if (!parser->get_inside_preevent()) {
          parser->add_error_p(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM), name.c_str()), get_errorsmessage(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM), parser->get_current_filename(), parser->get_current_line());
      }

      SDFFFUNC_CHECK_ARGCOUNT(6, name);
      SDFFFUNC_CHECK_INTARGTYPE(0, name, "GPIB interface device adress");
      SDFFFUNC_CHECK_BOOLARGTYPE(1, name, "use EOI line");
      SDFFFUNC_CHECK_BOOLARGTYPE(2, name, "state of REN during talk operations");
      SDFFFUNC_CHECK_BOOLARGTYPE(3, name, "state of REN during read operations");
      SDFFFUNC_CHECK_BOOLARGTYPE(4, name, "send linebreak after data");
      SDFFFUNC_CHECK_INTARGTYPE(5, name, "delay in query commands [msecs]");

      return true;
    }


    sdff_result SERGPIBCONFIGINT(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      std::string name=fname+"(interrupt, device, send_GTL, send_UNL, command)";

      if (!parser->get_inside_preevent()) {
          RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM), name.c_str()), name);
      }

      SDFFFUNC_CHECK_ARGCOUNT_RT(5, name);
      SDFFFUNC_CHECK_INTARGTYPE_RT(0, name, "interrupt number");
      SDFFFUNC_CHECK_INTARGTYPE_RT(1, name, "targt GPIB device");
      SDFFFUNC_CHECK_BOOLARGTYPE_RT(2, name, "send GTL");
      SDFFFUNC_CHECK_BOOLARGTYPE_RT(3, name, "send UNL");
      SDFFFUNC_CHECK_STRINGARGTYPE_RT(4, name, "command");

      std::string command=parameter[4].stringValue;
      char device=parameter[1].intValue;
      char interrupt=parameter[0].intValue;
      bool send_GTL=parameter[2].boolValue;
      bool send_UNL=parameter[3].boolValue;
      if (device<0 || device>31) {
        RUN_TIME_ERROR(SEQUENCER_GPIBERROR_INVALIDADR_NUM, format(get_errormessage(SEQUENCER_GPIBERROR_INVALIDADR_NUM), name.c_str()), name);
      }
      if (interrupt!=0 && interrupt!=1) {
        RUN_TIME_ERROR(SEQUENCER_SERERROR_IMPOSSIBLEARG_NUM, format(get_errormessage(SEQUENCER_SERERROR_IMPOSSIBLEARG_NUM), name.c_str(), inttostr(interrupt).c_str(), "interrupt", "0, 1"), name);
      }

      SDFFserGPIBconfigInterrupt* cmd=new SDFFserGPIBconfigInterrupt(0, interrupt, device, command, parser->get_current_filename(), parser->get_current_line());
      cmd->set_send_GTL(send_GTL);
      cmd->set_send_UNL(send_UNL);
      parser->get_cmd_manager()->add_command(cmd);

      sdff_result r;
      r.type=sdffRTVoid;
      return r;
    }

    bool SERGPIBCONFIGINT_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      std::string name=fname+"(interrupt, device, send_GTL, send_UNL, command)";

      if (!parser->get_inside_preevent()) {
          parser->add_error_p(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM), name.c_str()), get_errorsmessage(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM), parser->get_current_filename(), parser->get_current_line());
      }

      SDFFFUNC_CHECK_ARGCOUNT(5, name);
      SDFFFUNC_CHECK_INTARGTYPE(0, name, "interrupt number");
      SDFFFUNC_CHECK_INTARGTYPE(1, name, "targt GPIB device");
      SDFFFUNC_CHECK_BOOLARGTYPE(2, name, "send GTL");
      SDFFFUNC_CHECK_BOOLARGTYPE(3, name, "send UNL");
      SDFFFUNC_CHECK_STRINGARGTYPE(4, name, "command");

      return true;
    }


    sdff_result SERGPIBENABLEINT(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      std::string name=fname+"(interrupt)";

      if (!parser->get_inside_preevent()) {
          RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM), name.c_str()), name);
      }

      SDFFFUNC_CHECK_ARGCOUNT_RT(1, name);
      SDFFFUNC_CHECK_INTARGTYPE_RT(0, name, "interrupt number");

      char interrupt=parameter[0].intValue;
      if (interrupt!=0 && interrupt!=1) {
        RUN_TIME_ERROR(SEQUENCER_SERERROR_IMPOSSIBLEARG_NUM, format(get_errormessage(SEQUENCER_SERERROR_IMPOSSIBLEARG_NUM), name.c_str(), inttostr(interrupt).c_str(), "interrupt", "0, 1"), name);
      }
      bool state=false;
      if (fname=="sergpib_enable_interrupt") state=true;

      SDFFserGPIBenableInterrupt* cmd=new SDFFserGPIBenableInterrupt(0, interrupt, state, parser->get_current_filename(), parser->get_current_line());
      parser->get_cmd_manager()->add_command(cmd);

      sdff_result r;
      r.type=sdffRTVoid;
      return r;
    }

    bool SERGPIBENABLEINT_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      std::string name=fname+"(interrupt)";

      if (!parser->get_inside_preevent()) {
          parser->add_error_p(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM), name.c_str()), get_errorsmessage(SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM), parser->get_current_filename(), parser->get_current_line());
      }

      SDFFFUNC_CHECK_ARGCOUNT(1, name);
      SDFFFUNC_CHECK_INTARGTYPE(0, name, "interrupt number");

      return true;
    }





    sdff_result THETA(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      std::string name=fname+"(value)";

      SDFFFUNC_CHECK_ARGCOUNT_RT(1, name);
      SDFFFUNC_CHECK_NUMARGTYPE_RT(0, name, "");

      double a=parameter[0].doubleValue;
      if (parameter[0].type==sdffRTInteger) a=parameter[0].intValue;

      sdff_result r;
      r.type=sdffRTInteger;
      r.intValue=0;
      if (a>0) r.intValue=1;
      return r;
    }

    bool THETA_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      std::string name=fname+"(value)";

      SDFFFUNC_CHECK_ARGCOUNT(1, name);
      SDFFFUNC_CHECK_NUMARGTYPE(0, name, "");

      return true;
    }

    sdff_result SIGN(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount) {
      std::string name=fname+"(value)";

      SDFFFUNC_CHECK_ARGCOUNT_RT(1, name);
      SDFFFUNC_CHECK_NUMARGTYPE_RT(0, name, "");

      double a=parameter[0].doubleValue;
      if (parameter[0].type==sdffRTInteger) a=parameter[0].intValue;

      sdff_result r;
      r.type=sdffRTInteger;
      r.intValue=0;
      if (a>0) r.intValue=1;
      else if (a<0) r.intValue=-1;
      return r;
    }

    bool SIGN_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused) {
      std::string name=fname+"(value)";

      SDFFFUNC_CHECK_ARGCOUNT(1, name);
      SDFFFUNC_CHECK_NUMARGTYPE(0, name, "");

      return true;
    }  };
};
