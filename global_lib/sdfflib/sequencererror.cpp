/*
  Name: sequencererror.cpp
  Copyright: (c) 2007
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/

#include "sequencererror.h" // class's header file

namespace sequencer {

std::string get_errormessage(int num) {
  switch(num) {
    case SEQUENCER_ERROR_NOVARTIMING_NUM:
      return "The variable timing option is not available.";
    case SEQUENCER_ERROR_INVALIDTIMESTEP_NUM:
      return "When using variable timing you may only select a sampling timestep from the basic timesteps that are allowed\nby the variable timing hardware (i.e. as defined in variable-timing.ini!).";
    case SEQUENCER_GPIBERROR_INTFCERROR_NUM:
      return "The serial-to-GPIB interface reported an error:\n%s";
    case SEQUENCER_GPIBERROR_INVALIDADR_NUM:
      return "When executing '%s': The specified GPIB device adress is invalid (valid range is 0..31).";
    case SEQUENCER_SERERROR_PORTNOTOPEN_NUM:
      return "When executing '%s': The serial port '%s' is not opened.";
    case SEQUENCER_SERERROR_UNKNOWNHANDSHAKING_NUM:
      return "You supplied an unknown handshaking mode specifier.";
    case SEQUENCER_SERERROR_WIN32ERROR_NUM:
      return "When executing '%s': A win32 error occured. Error message was:\n%s";
    case SEQUENCER_SERERROR_IMPOSSIBLEARG_NUM:
      return "When executing '%s': The value %s of %s is impossible.\nAllowed values are: %s";
    case SEQUENCER_SERERROR_PORTALREADYOPEN_NUM:
      return "When executing '%s': The serial port '%s' is already open.";
    case SEQUENCER_SDFFERROR_NOTASCII_NUM:
      return "When using '%s': The supplied integer %d does not represent an ASCII character (allowed range: 0..255).";
    case SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM:
      return "The function '%s' is only allowed in pre_event-blocks.";
    case SEQUENCER_SDFFERROR_FUNCARGCMISSMIMA_NUM:
      return "When using function '%s': argument count missmatch.\nYou supplied %d arguments, but need between %d and %d arguments for this function.";
    case SEQUENCER_SERERROR_PARAMPOS_NUM:
      return "You have to give a positive integer as %s.";
    case SEQUENCER_SERERROR_UNKNOWNPARITY_NUM:
      return "You supplied an unknown parity mode specifier.";
    case SEQUENCER_SDFFERROR_CHANNELNAMESONLYINPARAM_NUM:
      return "The channel_names{...} construct is only allowed inside a parameters{...} block.";
    case SEQUENCER_SDFFERROR_NOALIASFORCHANNEL_NUM:
      return "No aliases for channel '%s' have been defined.";
    case SEQUENCER_SDFFERROR_ALIASNOTEXISTS_NUM:
      return "The specified alias '%s' does not exist.";
    case SEQUENCER_SDFFERROR_FILEOPERROR_NUM:
      return "Error while executing %s:\nerror was: %s";
    case SEQUENCER_SDFFERROR_DELAYEDOUTPUTINPRE_NUM:
      return "You tried to do delayed synchronized output (e.g. 'set(\"AO1.1\", 5, -5u)') inside a pre_event or last_event block. This is not allowed.";
    case SEQUENCER_SDFFERROR_LONGOUTPUTINLAST_NUM:
      return "You tried to do output other than setting channels inside a last_event block. This is not possible, as this is the really last timestep and you may not prolong the sequence any further in here.\nYou may only use unpost- or preponed set-operations (delay=0!) here, like 'set(\"AO1.1\", 5)'";
    case SEQUENCER_SDFFERROR_NONNEG_NUM:
      return "When using function %s: argument '%s' has to be non negative.";
    case SEQUENCER_SDFFERROR_FUNCARGTOOMANY_NUM:
      return "When using function %s: too many arguments (argument count mismatch).\nYou supplied %d arguments, but the function takes at most %d arguments.";
    case SEQUENCER_SDFFERROR_RANGE_NUM:
      return "The supplied index in function '%s' is out of range.";
    case SEQUENCER_SDFFERROR_OUTPUTINPRE_NUM:
      return "You tried to do synchronized output other than simple 'set(channel, value)' or 'channel=value' inside a pre_event block. This is not possible.";
    case SEQUENCER_ERROR_MEMALLOC_NUM:
      return "There was an error during memory allocation: %s";
    case SEQUENCER_ERROR_CNOTSYNCH_NUM:
      return "The channel %s is not synchronizable, but you try to use it in a sequence. Use another hardware channel that is synchronizable if you want to use hardware-synchronized I/O.";
    case SEQUENCER_ERROR_INPUTTASKFERROR_NUM:
      return "An error occured while accessing file '%s': %s.";
    case SEQUENCER_ERROR_INPUTTASKNE_NUM:
      return "The input task handle is invalid, i.e. no synchronised input task for this handle exists. Create an input task before reading data.";
    case SEQUENCER_ERROR_CHANNELINUSE_NUM:
      return "One of the input channels is already in use in an input task. You can not use one channel in two or more input tasks at the same time";
    case SEQUENCER_ERROR_INPUTONLYANALOG_NUM:
      return "All channels inside one input task have to be analog channels.";
    case SEQUENCER_ERROR_INPUTUNEQUALSPECS_NUM:
      return "All channels inside one input task have to have the same properties (min/max, same device, same type...).";
    case SEQUENCER_SDFFERROR_NOREQUIRED_NUM:
      return "The required channel %s was not used in the sequence. Add at least a dummy statement that uses this channel, as it is required for the synchronization of the sequence.";
    case SEQUENCER_CSVERROR_FILENOTEXISTS_NUM:
      return "The given CSV sequence file '%s' does not exist.";
    case SEQUENCER_SDFFERROR_FILENOTEXISTS_NUM:
      return "The given SDFF file '%s' does not exist.";
    case SEQUENCER_SDFFERROR_VARASSIGNNOBRACKETS_NUM:
      return "The SDFF parser found brackets [..] after the variable name '%s' in a variable assignment. Brackets only make sense if they appear in a channel assignment, i.e. the variable name should be a channel name. Maybe you misspelled a channel name?";
    case SEQUENCER_SDFFERROR_CHASSIGNWRONGTYPE_NUM:
      return "A channel value assignment for channel '%s' has the wrong datatype %s for the value. This instruction requires a number or a boolean (mapped to 0|1 for analog channels).";
    case SEQUENCER_SDFFERROR_CHASSIGNWRONGTIMETYPE_NUM:
      return "A channel value assignment for channel '%s' has the wrong datatype %s for the time argument. This instruction requires a number.";
    case SEQUENCER_SDFFERROR_CHASSIGNNOTIME_NUM:
      return "A channel value assignment for channel '%s' was found outside a channel-, event- or relative_event-block, but no time parameter has been given. Give a valid execution time for the assignment or place it inside a (relative_)event-block.";
    case SEQUENCER_SDFFERROR_ELEMENTNOTINCHANNEL_NUM:
      return "A function %s(<time>, ...) that generates a sequence element was found outside a channel-block. Place it inside a channel-block to specify the channel it acts on.";
    case SEQUENCER_SDFFERROR_ELEMENTNOTINEVENT_NUM:
      return "A function %s('%s', ...) that generates a sequence element was found outside a event- or relative_event-block. This type of function call does not provide an execution time.";
    case SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM:
      return "When using function '%s': argument count missmatch.\nYou supplied %d arguments, but need at least %d for this function.";
    case SEQUENCER_SDFFERROR_NODIGRAMP_NUM:
      return "You tried to define a ramp on a digital channel '%s'. Ramps are only possible for analog channels.";
    case SEQUENCER_SDFFERROR_TAUNOTNEG_NUM:
      return "The parameter tau of an exponential ramp may not be negative.";
    case SEQUENCER_SDFFERROR_AFTEREND_NUM:
      return "An element of the sequence ends after the end of the sequence.";
    case SEQUENCER_SDFFERROR_CNOTOUTPUT_NUM:
      return "The channel %s is not an output.";
    case SEQUENCER_SDFFERROR_COLLISION_NUM:
      return "Two SDFF instructions on channel '%s' do collide (they try to set the channel at the same time):\n  - t_end=%lf microns: instruction from l.%ld.\n  - t_start=%lf microns: instruction from l.%ld.\n";
    case SEQUENCER_SDFFERROR_CNOTEXISTS_NUM:
      return "The channel '%s' does not exist. Maybe you misspelled it?";
    case SEQUENCER_SDFFERROR_NONEGTIME_NUM:
      return "The given time is negative. Negative times are not allowed in a sequence definition, as the sequence starts at time t=0.";
    case SEQUENCER_SDFFERROR_CHANNELNOTSTRING_NUM:
      return "The channel parameter of a channel-block has to be a string.";
    case SEQUENCER_SDFFERROR_NEGNOTNUMBER_NUM:
      return "You may only negate integer and double values.";
    case SEQUENCER_SDFFERROR_FDEFNOVARNAME_NUM:
      return "The SDFF parser found an unexpected token: You may only use names (no expressions) on the left-hand side of a function definition, i.e. in the parameter name list.";
    case SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM:
      return "The SDFF parser found an unexpected token: %s expected but %s found.\nThe current element is %s.\nAlso have a look one line above as e.g. missing ';' are noticed in the following line!";
    case SEQUENCER_SDFFERROR_UNEXPECTEDTOKEN_NUM:
      return "The SDFF parser found an unexpected token: %s expected but %s found.\nAlso have a look one line above as e.g. missing ';' are noticed in the following line!";
    case SEQUENCER_SDFFERROR_UNKNOWNTOKEN_NUM:
      return "The SDFF parser found an unknown token: '%s'.";
    case SEQUENCER_SDFFERROR_FACTORIALPOS_NUM:
      return "factorial(x): The argument must be positive or zero.";
    case SEQUENCER_SDFFERROR_STR2BOOLNOTNUM_NUM:
      return "str2bool('%s'): The argument does not represent a boolean value.";
    case SEQUENCER_SDFFERROR_STR2NUMNOTNUM_NUM:
      return "str2num('%s'): The argument does not represent a number (integer or double).";
    case SEQUENCER_SDFFERROR_TIMENOTNUMERIC_NUM:
      return "The time parameter of an event- or relative_event-block has to be numeric.";
    case SEQUENCER_SDFFERROR_IFTESTNOTBOOL_NUM:
      return "The test of an if expression has to return a boolean value.";
    case SEQUENCER_SDFFERROR_PROD1NOTVARIABLE_NUM:
      return "The first argument of a prod(...) function has to be a variable name (example: prod(x,1,10,x+2) ).";
    case SEQUENCER_SDFFERROR_PRODTYPECMISS_NUM:
      return "During execution of prod(...): You can not multiply string, boolean or void data.";
    case SEQUENCER_SDFFERROR_PRODTYPEMISS_NUM:
      return "While executing prod(...) you tried to multiply two incompatible data types.";
    case SEQUENCER_SDFFERROR_SUMTYPECMISS_NUM:
      return "During execution of sum(...): You can not sum up boolean or void data.";
    case SEQUENCER_SDFFERROR_SUMTYPEMISS_NUM:
      return "While executing sum(...) you tried sum two incompatible data types.";
    case SEQUENCER_SDFFERROR_SUM1NOTVARIABLE_NUM:
      return "The first argument of a sum(...) function has to be a variable name (example: sum(x,1,10,x^2) ).";
    case SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM:
      return "When using function %s: argument count missmatch.\nYou supplied %d arguments, but the function requires %d arguments.";
    case SEQUENCER_SDFFERROR_LOOPTESTNOTBOOL_NUM:
      return "The test inside a loop has to return a boolean value.";
    case SEQUENCER_SDFFERROR_STRUNEQUALLENGTH_NUM:
      return "The operation %s is not defined for strings of unequal length.";
    case SEQUENCER_SDFFERROR_OPNOTDEFFT_NUM:
      return "The operation %s is not defined for data of type %s.";
    case SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM:
      return "Datatype mismatch: You can not %s %s and %s.";
    case SEQUENCER_SDFFERROR_NEGBOOL_NUM:
      return "Datatype mismatch: You can not do numeric negation of a boolean.";
    case SEQUENCER_SDFFERROR_NEGINT_NUM:
      return "Datatype mismatch: You can not do logic negation of an integer.";
    case SEQUENCER_SDFFERROR_NEGDOUBLE_NUM:
      return "Datatype mismatch: You can not do logic or bit-wise negation of a double.";
    case SEQUENCER_SDFFERROR_NEGVOID_NUM:
      return "Datatype mismatch: You can not negate a void value.";
    case SEQUENCER_SDFFERROR_NEGSTRING_NUM:
      return "Datatype mismatch: You can not do logic or numeric negation of a string.";
    case SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM:
      return "When using function %s: argument type missmatch. You supplied %s as %d. argument, but %s is required.";
    case SEQUENCER_SDFFERROR_FUNCEXISTS_NUM:
      return "The function %s already exists.\nYou may not redefine or overwrite an already define function."; // TODO: Add tipps to the error messages
    case SEQUENCER_SDFFERROR_FUNCNOTFOUND_NUM:
      return "The function %s is not defined.\nYou can define a function by writing e.g. 'f(x)=sin(x)*cos(x/2)'.";
    case SEQUENCER_SDFFERROR_VOIDUSED_NUM:
      return "You tried to assign a void value to a variable or to calculate with void values.\nFunctions without return value (= procedures) formally return void, so you cannot assign their return value to a variable.";
    case SEQUENCER_SDFFERROR_SETTYPEMISS_NUM:
      return "There was a type missmatch when trying to assign a %s value to the %s variable %s.";
    case SEQUENCER_SDFFERROR_EXTNOTC_NUM:
      return "The variable %s is an external variable. You can not change the value of such variables.";
    case SEQUENCER_SDFFERROR_VARCONSTANT_NUM:
      return "The variable %s is a constant and therefore you cannot assign a value.\nSome variables (e.g. predefined math variables, like 'pi') are constants so you may not change their value.";
    case SEQUENCER_SDFFERROR_NBTL_NUM:
      return "There is no more block to leave (we are in the lowest block level). This may be caused by an error in the right number of opening and closing  braces '{' '}' ";
    case SEQUENCER_SDFFERROR_LEVELTH_NUM:
      return "The given block level %d is too high.";
    case SEQUENCER_SDFFERROR_VARNOTFOUND_NUM:
      return "The variable %s is not defined. You have to define a variable before accessing it in a read operation,\ne.g. by writing 'varname=5' to define the variable 'varname'.";
    case SEQUENCER_CSVERROR_DOUBLETTE_NUM:
      return "The column title/channel %s appears twice in the csv file.";
    case SEQUENCER_CSVERROR_NOREQUIRED_NUM:
      return "The required channel %s was not used in the sequence.";
    case SEQUENCER_NETERROR_TUNEXPECTED_NUM:
      return "TCP/IP instruction parser error: found token %s, but expected %s.";
    case SEQUENCER_NETERROR_UTOKEN_NUM:
      return "TCP/IP instruction parser error: unkown token '%c'.";
    case SEQUENCER_NETERROR_SELECTFAIL_NUM:
      return "TCP/IP error: error while waiting for data to read.\nError description: %s";
    case SEQUENCER_NETERROR_CRANGE_NUM:
      return "TCP/IP error: connection index out of range: connection does not exist";
    case SEQUENCER_NETERROR_TIMEOUT_NUM:
      return "TCP/IP error: timeout while reading or writing data";
    case SEQUENCER_NETERROR_ACCEPTFAIL_NUM:
      return "TCP/IP error: accepting connection failed.\nError description: %s";
    case SEQUENCER_NETERROR_SOCKETFAIL_NUM:
      return "TCP/IP error: socket generation failed.\nError description: %s";
    case SEQUENCER_NETERROR_SENDFAIL_NUM:
      return "TCP/IP error: sending data failed.\nError description: %s";
    case SEQUENCER_NETERROR_LISTENFAIL_NUM:
      return "TCP/IP error: could not listen on socket.\nError description: %s";
    case SEQUENCER_NETERROR_BINDFAIL_NUM:
      return "TCP/IP error: could not bind to socket.\nError description: %s";
    case SEQUENCER_NETERROR_STARTFAIL_NUM:
      return "TCP/IP error: WSAStartup failed.";
    case SEQUENCER_ERROR_COULDNOTREAD_NUM:
      return "Could not read data from the given channel.";
    case SEQUENCER_ERROR_USTNOTRUNNING_NUM:
      return "The unsynchronised task for this channel is currently not running. Maybe it is used in a sequence.";
    case SEQUENCER_ERROR_NOSEQUENCE_NUM:
      return "No sequence is attatched to the NIdriver object.";
    case SEQUENCER_ERROR_WRONGDIGDATAFORMAT_NUM:
      return "Digital data array for channels '%s' has an unsupported data format.";
    case SEQUENCER_ERROR_DEVNOTINI_NUM:
      return "Device %s not listet in ini file %s.";
    case SEQUENCER_ERROR_DEVNOTCONSIST_NUM:
      return "Device of channel %s not consistent inside task with adress '%s'.";
    case SEQUENCER_ERROR_TYPENOTCONSIST_NUM:
      return "Type of channel %s not consistent inside task with adress '%s'.";
    case SEQUENCER_ERROR_MAXNOTCONSIST_NUM:
      return "Max of channel %s not consistent inside task with adress '%s'.";
    case SEQUENCER_ERROR_MINNOTCONSIST_NUM:
      return "Min of channel %s not consistent inside task with adress '%s'.";
    case SEQUENCER_ERROR_NOCHAN_NUM:
      return "There are no channels defined.";
    case SEQUENCER_ERROR_NOARRYS_NUM:
      return "There are no output arrays in sequenceGenerator object.";
    case SEQUENCER_ERROR_TNRUN_NUM:
      return "The task is not in mode 'running', but in '%s'.";
    case SEQUENCER_ERROR_DAQMXERR_NUM:
      return "DAQmx error during %s:\n  %s";
    case SEQUENCER_ERROR_TNWRITTEN_NUM:
      return "The task is not in mode 'written', but in '%s'.";
    case SEQUENCER_CSVERROR_NOTDIGI_NUM:
      return "The value %lg of digital channel '%s' in the CSV file is no digital value (0|1).";
    case SEQUENCER_CSVERROR_0MINMAX_NUM:
      return "The standard initial value 0 of analog channel '%s' in the CSV file is out of range (min=%lg;  max=%lg).";
    case SEQUENCER_CSVERROR_MINMAX_NUM:
      return "The value %lg of analog channel '%s' in the CSV file is out of range (min=%lg;  max=%lg).";
    case SEQUENCER_CSVERROR_NODOUBLETIME_NUM:
      return "There are multiply defined time steps in the CSV file.";
    case SEQUENCER_CSVERROR_NOUDTIME_NUM:
      return "There is a line with an undefined time in the CSV file.";
    case SEQUENCER_CSVERROR_NOCOLS_NUM:
      return "There are no valid columns in the CSV file.";
    case SEQUENCER_CSVERROR_NODEFINITIONS_NUM:
      return "There are no valid lines in the CSV file.";
    case SEQUENCER_CSVERROR_ENDTIME_NUM:
      return "end_time<=0! This means that there is no sequence in the file!";
    case SEQUENCER_CSVERROR_NONEGTIME_NUM:
      return "Negative times are not allowed in a sequence definition.";
    case SEQUENCER_CSVERROR_UNNAMEDCO_NUM:
      return "Column %d is unnamed.";
    case SEQUENCER_CSVERROR_PARSERERR_NUM:
      return "There was an error while parsing the file: \n   %s";
    case SEQUENCER_CSVERROR_CHBUTNOCO_NUM:
      return "There is no column for channel '%s'.";
    case SEQUENCER_CSVERROR_COBUTNOCH_NUM:
      return "Column %d is named '%s', but no such channel exists.";
    case SEQUENCER_CSVERROR_0NOTTIME_NUM:
      return "The first column of the CSV file is not named 'time'. It will be interpreted as time column nevertheless.";
    case SEQUENCER_ERROR_NOWIDTH_NUM:
      return "no or an invalid port width has been provided";
    case SEQUENCER_ERROR_PINNOTNUMBERTOOBIG_NUM:
      return "the given bit number is too big (outside 0..portwidth-1)";
    case SEQUENCER_ERROR_PINNOTNUMBER_NUM:
      return "pin-name is not an integer number";
    case SEQUENCER_ERROR_WRONGNUMBERINGANALOG_NUM:
      return "pin-name numbering is only possible for digital channels";
    case SEQUENCER_ERROR_CHANNELDATAMISMATH_NUM:
      return "you may only group channels with the same data type (analog/digital) to one port";
    case SEQUENCER_ERROR_TOOMANYCHANNELS_NUM:
      return "you may only group 32 pins to one port";
    case SEQUENCER_ERROR_NOTDIGITAL_NUM:
      return "the given channel is not a digital channel, but you tried to assign it a digital value";
    case SEQUENCER_ERROR_NOTANALOG_NUM:
      return "the given channel is not an analog (double) channel, but you tried to use it as an analog channel";
    case SEQUENCER_ERROR_AFTERTIMEEND_NUM:
      return "asked for value after end of sequence";
    case SEQUENCER_ERROR_INI_NUM:
      return "error while reading INI file";
    case SEQUENCER_ERROR_CEXIST_NUM:
      return "channel already exists";
    case SEQUENCER_ERROR_CNOTEXIST_NUM:
      return "channel does not exist";
    case SEQUENCER_ERROR_RANGE_NUM:
      return "internal error: index out of range";
    default:
      return "";
  }
};

std::string get_errorsmessage(int num) {
  switch(num) {
    case SEQUENCER_ERROR_NOVARTIMING_NUM:
      return "variable timing not available.";
    case SEQUENCER_ERROR_INVALIDTIMESTEP_NUM:
      return "invalid timestep when using variable timing";
    case SEQUENCER_GPIBERROR_INVALIDADR_NUM:
      return "invalid GPIB device adress";
    case SEQUENCER_SERERROR_PORTNOTOPEN_NUM:
      return "port not opened";
    case SEQUENCER_SERERROR_UNKNOWNHANDSHAKING_NUM:
      return "unknown handshaking mode specifier";
    case SEQUENCER_SERERROR_WIN32ERROR_NUM:
      return "%s";
    case SEQUENCER_SERERROR_IMPOSSIBLEARG_NUM:
      return "this value of %s is impossible";
    case SEQUENCER_SERERROR_PORTALREADYOPEN_NUM:
      return "serial port already opened";
    case SEQUENCER_SDFFERROR_NOTASCII_NUM:
      return "the supplied integer is not an ASCII character (0..255)";
    case SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM:
      return "this function is only allowed in pre_event-blocks";
    case SEQUENCER_SDFFERROR_FUNCARGCMISSMIMA_NUM:
      return "%s: argument count missmatch";
    case SEQUENCER_SERERROR_PARAMPOS_NUM:
      return "negative values for %s are not allowed";
    case SEQUENCER_SERERROR_UNKNOWNPARITY_NUM:
      return "unknown parity mode specifier";
    case SEQUENCER_SDFFERROR_CHANNELNAMESONLYINPARAM_NUM:
      return "channel_names{...} only allowed inside parameters{...}";
    case SEQUENCER_SDFFERROR_NOALIASFORCHANNEL_NUM:
      return "there are no aliases for channel '%s'";
    case SEQUENCER_SDFFERROR_ALIASNOTEXISTS_NUM:
      return "alias '%s' does not exist";
    case SEQUENCER_SDFFERROR_FILEOPERROR_NUM:
      return "file access error: %s";
    case SEQUENCER_SDFFERROR_DELAYEDOUTPUTINPRE_NUM:
      return "no delayed synchronized output inside pre_event or last_event";
    case SEQUENCER_SDFFERROR_LONGOUTPUTINLAST_NUM:
      return "only setting channels is allowed inside last_event";
    case SEQUENCER_SDFFERROR_NONNEG_NUM:
      return "%s: argument has to be non negative.";
    case SEQUENCER_SDFFERROR_FUNCARGTOOMANY_NUM:
      return "%s: too many arguments";
    case SEQUENCER_SDFFERROR_RANGE_NUM:
      return "index out of range";
    case SEQUENCER_SDFFERROR_OUTPUTINPRE_NUM:
      return "no synchronized output inside a pre_event";
    case SEQUENCER_ERROR_MEMALLOC_NUM:
      return "memory allocation failed";
    case SEQUENCER_ERROR_CNOTSYNCH_NUM:
      return "channel %s not synchronizable";
    case SEQUENCER_ERROR_INPUTTASKFERROR_NUM:
      return "An error occured while accessing file '%s': %s";
    case SEQUENCER_ERROR_INPUTTASKNE_NUM:
      return "input task handle is invalid";
    case SEQUENCER_ERROR_CHANNELINUSE_NUM:
      return "input channel already in use";
    case SEQUENCER_ERROR_INPUTONLYANALOG_NUM:
      return "channels in input task have to be analog";
    case SEQUENCER_ERROR_INPUTUNEQUALSPECS_NUM:
      return "channels in input task have different specs";
    case SEQUENCER_SDFFERROR_NOREQUIRED_NUM:
      return "required channel %s not used";
    case SEQUENCER_CSVERROR_FILENOTEXISTS_NUM:
      return "'%s' does not exist.";
    case SEQUENCER_SDFFERROR_FILENOTEXISTS_NUM:
      return "'%s' does not exist.";
    case SEQUENCER_SDFFERROR_VARASSIGNNOBRACKETS_NUM:
      return "no brackets [..] in variable assignment";
    case SEQUENCER_SDFFERROR_CHASSIGNWRONGTYPE_NUM:
      return "wrong type of argument for channel assignment";
    case SEQUENCER_SDFFERROR_CHASSIGNWRONGTIMETYPE_NUM:
      return "wrong type of time argument for channel assignment";
    case SEQUENCER_SDFFERROR_CHASSIGNNOTIME_NUM:
      return "missing time argument for channel assignment";
    case SEQUENCER_SDFFERROR_ELEMENTNOTINCHANNEL_NUM:
      return "sequence generating function outside any channel-block";
    case SEQUENCER_SDFFERROR_ELEMENTNOTINEVENT_NUM:
      return "sequence generating function outside any event- relative_event-block";
    case SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM:
      return "%s: argument count missmatch";
    case SEQUENCER_SDFFERROR_NODIGRAMP_NUM:
      return "no ramps possible on digital channels";
    case SEQUENCER_SDFFERROR_TAUNOTNEG_NUM:
      return "tau of exponential ramps has to be >0";
    case SEQUENCER_SDFFERROR_AFTEREND_NUM:
      return "sequence element after end of sequence";
    case SEQUENCER_SDFFERROR_CNOTOUTPUT_NUM:
      return "channel %s not an output";
    case SEQUENCER_SDFFERROR_COLLISION_NUM:
      return "Two SDFF instructions do collide (from l.%ld. and from l.%ld)";
    case SEQUENCER_SDFFERROR_CNOTEXISTS_NUM:
      return "no channel %s";
    case SEQUENCER_SDFFERROR_NONEGTIME_NUM:
      return "no negative times";
    case SEQUENCER_SDFFERROR_CHANNELNOTSTRING_NUM:
      return "channel parameter of channel-blocks not a string";
    case SEQUENCER_SDFFERROR_NEGNOTNUMBER_NUM:
      return "you may only negate integer and double values";
    case SEQUENCER_SDFFERROR_FDEFNOVARNAME_NUM:
      return "not a name on lhs of function definition";
    case SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM:
      return "unexpected token '%s'";
    case SEQUENCER_SDFFERROR_UNEXPECTEDTOKEN_NUM:
      return "unexpected token '%s'";
    case SEQUENCER_SDFFERROR_UNKNOWNTOKEN_NUM:
      return "unknown token '%s'";
    case SEQUENCER_SDFFERROR_FACTORIALPOS_NUM:
      return "factorial(x): argument must be >=0";
    case SEQUENCER_SDFFERROR_STR2BOOLNOTNUM_NUM:
      return "str2bool('%s'): argument doesn't represent a boolean";
    case SEQUENCER_SDFFERROR_STR2NUMNOTNUM_NUM:
      return "str2num('%s'): argument doesn't represent a number";
    case SEQUENCER_SDFFERROR_TIMENOTNUMERIC_NUM:
      return "time parameter of event- or relative_event-blocks not numeric";
    case SEQUENCER_SDFFERROR_IFTESTNOTBOOL_NUM:
      return "if test has to be boolean";
    case SEQUENCER_SDFFERROR_PROD1NOTVARIABLE_NUM:
      return "prod(...): first argument of has to be a variable";
    case SEQUENCER_SDFFERROR_PRODTYPECMISS_NUM:
      return "prod(...): can't multiply string, boolean or void";
    case SEQUENCER_SDFFERROR_PRODTYPEMISS_NUM:
      return "prod(...): you can not multiply incompatible types";
    case SEQUENCER_SDFFERROR_SUMTYPECMISS_NUM:
      return "sum(...): can't sum up boolean or void";
    case SEQUENCER_SDFFERROR_SUMTYPEMISS_NUM:
      return "sum(...): you can not sum different types";
    case SEQUENCER_SDFFERROR_SUM1NOTVARIABLE_NUM:
      return "sum(...): first argument of has to be a variable";
    case SEQUENCER_SDFFERROR_LOOPTESTNOTBOOL_NUM:
      return "loop test has to be boolean";
    case SEQUENCER_SDFFERROR_STRUNEQUALLENGTH_NUM:
      return "%s not defined for strings of unequal length";
    case SEQUENCER_SDFFERROR_OPNOTDEFFT_NUM:
      return "%s not defined for %s data";
    case SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM:
      return "can't %s %s and %s";
    case SEQUENCER_SDFFERROR_NEGBOOL_NUM:
      return "can't do numeric negation of boolean";
    case SEQUENCER_SDFFERROR_NEGINT_NUM:
      return "can't do logic negation of integer";
    case SEQUENCER_SDFFERROR_NEGDOUBLE_NUM:
      return "can't do logic/bit-wise negation of double";
    case SEQUENCER_SDFFERROR_NEGVOID_NUM:
      return "can't negate void";
    case SEQUENCER_SDFFERROR_NEGSTRING_NUM:
      return "can't numerically negate strings";
    case SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM:
      return "%s: argument type missmatch";
    case SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM:
      return "%s: argument count missmatch";
    case SEQUENCER_SDFFERROR_FUNCEXISTS_NUM:
      return "function %s already exists";
    case SEQUENCER_SDFFERROR_FUNCNOTFOUND_NUM:
      return "function %s not defined";
    case SEQUENCER_SDFFERROR_VOIDUSED_NUM:
      return "can't assign or calculate void";
    case SEQUENCER_SDFFERROR_SETTYPEMISS_NUM:
      return "type missmatch (assign): %s %s <- %s";
    case SEQUENCER_SDFFERROR_EXTNOTC_NUM:
      return "can't change type of external variable %s";
    case SEQUENCER_SDFFERROR_VARCONSTANT_NUM:
      return "variable %s is constant";
    case SEQUENCER_SDFFERROR_NBTL_NUM:
      return "no block to leave";
    case SEQUENCER_SDFFERROR_LEVELTH_NUM:
      return "block level %d too high";
    case SEQUENCER_SDFFERROR_VARNOTFOUND_NUM:
      return "variable %s not defined";
    case SEQUENCER_CSVERROR_DOUBLETTE_NUM:
      return "%s appears twice in csv";
    case SEQUENCER_CSVERROR_NOREQUIRED_NUM:
      return "required channel %s not used";
    case SEQUENCER_NETERROR_TUNEXPECTED_NUM:
      return "unexpected token";
    case SEQUENCER_NETERROR_UTOKEN_NUM:
      return "unkown token";
    case SEQUENCER_NETERROR_SELECTFAIL_NUM:
      return "select failed";
    case SEQUENCER_NETERROR_CRANGE_NUM:
      return "connection index out of range";
    case SEQUENCER_NETERROR_TIMEOUT_NUM:
      return "timeout";
    case SEQUENCER_NETERROR_ACCEPTFAIL_NUM:
      return "accept failed";
    case SEQUENCER_NETERROR_SOCKETFAIL_NUM:
      return "socket failed";
    case SEQUENCER_NETERROR_SENDFAIL_NUM:
      return "send failed";
    case SEQUENCER_NETERROR_LISTENFAIL_NUM:
      return "listen failed";
    case SEQUENCER_NETERROR_BINDFAIL_NUM:
      return "bind failed";
    case SEQUENCER_NETERROR_STARTFAIL_NUM:
      return "WSAStartup failed";
    case SEQUENCER_ERROR_COULDNOTREAD_NUM:
      return "could not read from channel";
    case SEQUENCER_ERROR_USTNOTRUNNING_NUM:
      return "unsynchronised task not running";
    case SEQUENCER_ERROR_NOSEQUENCE_NUM:
      return "no sequence attatched to NIdriver";
    case SEQUENCER_ERROR_WRONGDIGDATAFORMAT_NUM:
      return "digital data array has wrong format";
    case SEQUENCER_ERROR_DEVNOTCONSIST_NUM:
      return "devices not consistent inside task";
    case SEQUENCER_ERROR_TYPENOTCONSIST_NUM:
      return "types not consistent inside task";
    case SEQUENCER_ERROR_MAXNOTCONSIST_NUM:
      return "max not consistent inside task";
    case SEQUENCER_ERROR_MINNOTCONSIST_NUM:
      return "min not consistent inside task";
    case SEQUENCER_ERROR_NOCHAN_NUM:
      return "no channels";
    case SEQUENCER_ERROR_NOARRYS_NUM:
      return "no output arrays";
    case SEQUENCER_ERROR_TNRUN_NUM:
      return "task mode != 'running'";
    case SEQUENCER_ERROR_DAQMXERR_NUM:
      return "DAQmx error.";
    case SEQUENCER_ERROR_TNWRITTEN_NUM:
      return "task mode != 'written'";
    case SEQUENCER_CSVERROR_NOTDIGI_NUM:
      return "%s: %lg not digital";
    case SEQUENCER_CSVERROR_0MINMAX_NUM:
      return "%s: 0 out of range";
    case SEQUENCER_CSVERROR_MINMAX_NUM:
      return "%s: %lg out of range";
    case SEQUENCER_CSVERROR_NODOUBLETIME_NUM:
      return "multiple time steps";
    case SEQUENCER_CSVERROR_NOUDTIME_NUM:
      return "line with an undefined time";
    case SEQUENCER_CSVERROR_NOCOLS_NUM:
      return "no valid columns in file";
    case SEQUENCER_CSVERROR_NODEFINITIONS_NUM:
      return "no valid lines in file";
    case SEQUENCER_CSVERROR_ENDTIME_NUM:
      return "end_time<=0";
    case SEQUENCER_CSVERROR_NONEGTIME_NUM:
      return "no negative times";
    case SEQUENCER_CSVERROR_UNNAMEDCO_NUM:
      return "unnamed column";
    case SEQUENCER_CSVERROR_PARSERERR_NUM:
      return "parser error";
    case SEQUENCER_CSVERROR_CHBUTNOCO_NUM:
      return "no column for %s";
    case SEQUENCER_CSVERROR_COBUTNOCH_NUM:
      return "no channel %s";
    case SEQUENCER_CSVERROR_0NOTTIME_NUM:
      return "first column not 'time'";
    case SEQUENCER_ERROR_NOWIDTH_NUM:
      return "invalid port width";
    case SEQUENCER_ERROR_PINNOTNUMBERTOOBIG_NUM:
      return "bit num is too big";
    case SEQUENCER_ERROR_PINNOTNUMBER_NUM:
      return "pin-name not integer";
    case SEQUENCER_ERROR_WRONGNUMBERINGANALOG_NUM:
      return "pin-name numbering only for digital ch.";
    case SEQUENCER_ERROR_CHANNELDATAMISMATH_NUM:
      return "only group channels with same type";
    case SEQUENCER_ERROR_TOOMANYCHANNELS_NUM:
      return "cant't group >=32 pins";
    case SEQUENCER_ERROR_NOTDIGITAL_NUM:
      return "channel not digital";
    case SEQUENCER_ERROR_NOTANALOG_NUM:
      return "channel not analog";
    case SEQUENCER_ERROR_AFTERTIMEEND_NUM:
      return "t>end_time";
    case SEQUENCER_ERROR_INI_NUM:
      return "INI parser error";
    case SEQUENCER_ERROR_CEXIST_NUM:
      return "channel already exists";
    case SEQUENCER_ERROR_CNOTEXIST_NUM:
      return "channel does not exist";
    case SEQUENCER_ERROR_RANGE_NUM:
      return "index out of range";
    default:
      return "";
  }
};


exception::exception(){
  this->emessage="unknown error";
  this->number=-1;
  this->where="unknown";
  this->line=-1;
  this->time=-1;
}
exception::exception(int number, std::string message, std::string where, long line, double time){
  this->emessage=message;
  this->number=number;
  this->where=where;
  this->line=line;
  this->time=time;
}
std::string exception::get_message() {
  if (number!=-1) return "error "+inttostr(number)+": "+emessage+ "\n   [in "+where+"]";
  return "error: "+emessage+ "\n   [in "+where+"]";
}

}
