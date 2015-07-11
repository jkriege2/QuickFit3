/*
  Name: sequencererror.h
  Copyright: (c) 2007
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/
/**
 * \ingroup seq_error
 */
/*@{*/

/** \file channelmanager.h
 *
 */

#include <string>
#include <sstream>
#include <cstdio>

#include "tools.h"

#ifndef SEQUENCERERROR_H
#define SEQUENCERERROR_H

namespace sequencer {


/** \brief error: index out of range
 * \ingroup seq_error
 */
#define SEQUENCER_ERROR_RANGE_NUM 1
/** \brief error: channel does not exist
 * \ingroup seq_error
 */
#define SEQUENCER_ERROR_CNOTEXIST_NUM 2
/** \brief error:  channel already exists
 * \ingroup seq_error
 */
#define SEQUENCER_ERROR_CEXIST_NUM 3
/** \brief error: error while reading INI file
 * \ingroup seq_error
 */
#define SEQUENCER_ERROR_INI_NUM 4
/** \brief error: asked for value after end of sequence
 * \ingroup seq_error
 */
#define SEQUENCER_ERROR_AFTERTIMEEND_NUM 5
/** \brief error: the given channel is not an analog (double) channel
 * \ingroup seq_error
 */
#define SEQUENCER_ERROR_NOTANALOG_NUM 6
/** \brief error: the given channel is not a digital channel
 * \ingroup seq_error
 */
#define SEQUENCER_ERROR_NOTDIGITAL_NUM 7
/** \brief error:  you may only group 32 pins to one port
 * \ingroup seq_error
 */
#define SEQUENCER_ERROR_TOOMANYCHANNELS_NUM 8
/** \brief error: you may only group channels with the same data type (analog/digital) to one port
 * \ingroup seq_error
 */
#define SEQUENCER_ERROR_CHANNELDATAMISMATH_NUM 9
/** \brief error:pin-name numbering is only possible for digital channels
 * \ingroup seq_error
 */
#define SEQUENCER_ERROR_WRONGNUMBERINGANALOG_NUM 10
/** \brief error: pin-name is not an integer number
 * \ingroup seq_error
 */
#define SEQUENCER_ERROR_PINNOTNUMBER_NUM 11
/** \brief error: the given bit number is too big (outside 0..portwidth-1)
 * \ingroup seq_error
 */
#define SEQUENCER_ERROR_PINNOTNUMBERTOOBIG_NUM 12
/** \brief error: no or an invalid port width has been provided
 * \ingroup seq_error
 */
#define SEQUENCER_ERROR_NOWIDTH_NUM 13
/** \brief error: The first column of the CSV file is not named 'time'
 * \ingroup seq_error
 */
#define SEQUENCER_CSVERROR_0NOTTIME_NUM 14
/** \brief error: no channel with column name exists.
 * \ingroup seq_error
 */
#define SEQUENCER_CSVERROR_COBUTNOCH_NUM 15
/** \brief error: There is no column for this channel
 * \ingroup seq_error
 */
#define SEQUENCER_CSVERROR_CHBUTNOCO_NUM 16
/** \brief error: error while parsing a csv file
 * \ingroup seq_error
 */
#define SEQUENCER_CSVERROR_PARSERERR_NUM 17
/** \brief error: column unnamed
 * \ingroup seq_error
 */
#define SEQUENCER_CSVERROR_UNNAMEDCO_NUM 18
/** \brief error: no negative times
 * \ingroup seq_error
 */
#define SEQUENCER_CSVERROR_NONEGTIME_NUM 19
/** \brief error: end_time <= 0
 * \ingroup seq_error
 */
#define SEQUENCER_CSVERROR_ENDTIME_NUM 20
/** \brief error: There are no valid lines in the CSV file.
 * \ingroup seq_error
 */
#define SEQUENCER_CSVERROR_NODEFINITIONS_NUM 21
/** \brief error: There are no valid columns in the CSV file.
 * \ingroup seq_error
 */
#define SEQUENCER_CSVERROR_NOCOLS_NUM 22
/** \brief error: There is a line with an undefined time in the CSV file.
 * \ingroup seq_error
 */
#define SEQUENCER_CSVERROR_NOUDTIME_NUM 23
/** \brief error: There are multiply defined time steps in the CSV file.
 * \ingroup seq_error
 */
#define SEQUENCER_CSVERROR_NODOUBLETIME_NUM 24
/** \brief error: analog value out of range
 * \ingroup seq_error
 */
#define SEQUENCER_CSVERROR_MINMAX_NUM 25
/** \brief error: initial 0 out of range
 * \ingroup seq_error
 */
#define SEQUENCER_CSVERROR_0MINMAX_NUM 26
/** \brief error: digital value out of range
 * \ingroup seq_error
 */
#define SEQUENCER_CSVERROR_NOTDIGI_NUM 27
/** \brief error: task mode not written
 * \ingroup seq_error
 */
#define SEQUENCER_ERROR_TNWRITTEN_NUM 28
/** \brief error: DAQmx error
 * \ingroup seq_error
 */
#define SEQUENCER_ERROR_DAQMXERR_NUM 29
/** \brief error: task mode not running
 * \ingroup seq_error
 */
#define SEQUENCER_ERROR_TNRUN_NUM 30
/** \brief error: no data arrays in sequenceGenerator
 * \ingroup seq_error
 */
#define SEQUENCER_ERROR_NOARRYS_NUM 31
/** \brief error: no chanels defined
 * \ingroup seq_error
 */
#define SEQUENCER_ERROR_NOCHAN_NUM 32
/** \brief error: min value of channels inside task are not consistent
 * \ingroup seq_error
 */
#define SEQUENCER_ERROR_MINNOTCONSIST_NUM 33
/** \brief error: max value of channels inside task are not consistent
 * \ingroup seq_error
 */
#define SEQUENCER_ERROR_MAXNOTCONSIST_NUM 34
/** \brief error: type of channels inside task are not consistent
 * \ingroup seq_error
 */
#define SEQUENCER_ERROR_TYPENOTCONSIST_NUM 35
/** \brief error: device of channels inside task are not consistent
 * \ingroup seq_error
 */
#define SEQUENCER_ERROR_DEVNOTCONSIST_NUM 36
/** \brief error: device not listed in timing.ini
 * \ingroup seq_error
 */
#define SEQUENCER_ERROR_DEVNOTINI_NUM 37
/** \brief error: digital data array has wrong format (not U8, U16, U32)
 * \ingroup seq_error
 */
#define SEQUENCER_ERROR_WRONGDIGDATAFORMAT_NUM 38
/** \brief error: no sequence attatched to the driver
 * \ingroup seq_error
 */
#define SEQUENCER_ERROR_NOSEQUENCE_NUM 39
/** \brief error: unsynchronised task is not running
 * \ingroup seq_error
 */
#define SEQUENCER_ERROR_USTNOTRUNNING_NUM 40
/** \brief error: could not read data
 * \ingroup seq_error
 */
#define SEQUENCER_ERROR_COULDNOTREAD_NUM 41
/** \brief networking error: WSAStartup failed
 * \ingroup seq_error
 */
#define SEQUENCER_NETERROR_STARTFAIL_NUM 42
/** \brief networking error: socket failed
 * \ingroup seq_error
 */
#define SEQUENCER_NETERROR_SOCKETFAIL_NUM 43
/** \brief networking error: send failed
 * \ingroup seq_error
 */
#define SEQUENCER_NETERROR_SENDFAIL_NUM 44
/** \brief networking error: listen failed
 * \ingroup seq_error
 */
#define SEQUENCER_NETERROR_LISTENFAIL_NUM 45
/** \brief networking error: bind failed
 * \ingroup seq_error
 */
#define SEQUENCER_NETERROR_BINDFAIL_NUM 46
/** \brief networking error: accept failed
 * \ingroup seq_error
 */
#define SEQUENCER_NETERROR_ACCEPTFAIL_NUM 47
/** \brief networking error: read timeout
 * \ingroup seq_error
 */
#define SEQUENCER_NETERROR_TIMEOUT_NUM 48
/** \brief networking error: connection index out of range
 * \ingroup seq_error
 */
#define SEQUENCER_NETERROR_CRANGE_NUM 49
/** \brief networking error: select failed
 * \ingroup seq_error
 */
#define SEQUENCER_NETERROR_SELECTFAIL_NUM 50
/** \brief error: a TCPIPinstructionParser found an unknown token
 * \ingroup seq_error
 */
#define SEQUENCER_NETERROR_UTOKEN_NUM 51
/** \brief error: a TCPIPinstructionParser found an unexpected token
 * \ingroup seq_error
 */
#define SEQUENCER_NETERROR_TUNEXPECTED_NUM 52
/** \brief error: one of the required channels was not used
 * \ingroup seq_error
 */
#define SEQUENCER_CSVERROR_NOREQUIRED_NUM 53
/** \brief error: one of the column titles appears twice (doublette)
 * \ingroup seq_error
 */
#define SEQUENCER_CSVERROR_DOUBLETTE_NUM 54
/** \brief sdff error: didn't find variable
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_VARNOTFOUND_NUM 55
/** \brief sdff error: block level too high
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_LEVELTH_NUM 56
/** \brief sdff error: no block to leave (we are in the lowest block)
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_NBTL_NUM 57
/** \brief sdff error: variable is constant
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_VARCONSTANT_NUM 58
/** \brief sdff error: no external typechange
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_EXTNOTC_NUM 59
/** \brief sdff error: type missmatch during set
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_SETTYPEMISS_NUM 60
/** \brief sdff error: you can not assign void values
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_VOIDUSED_NUM 61
/** \brief sdff error: didn't find function/function not defined
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_FUNCNOTFOUND_NUM 62
/** \brief sdff error: function already exists
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_FUNCEXISTS_NUM 63
/** \brief sdff error: function parameter count missmatch
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM 64
/** \brief sdff error: function parameter type missmatch
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM 65
/** \brief sdff error: you can't negate strings
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_NEGSTRING_NUM 66
/** \brief sdff error: you can't negate void
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_NEGVOID_NUM 67
/** \brief sdff error: you can't do logic/bit-wise negation of double
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_NEGDOUBLE_NUM 68
/** \brief sdff error: you can't do logic negation of int
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_NEGINT_NUM 69
/** \brief sdff error: you can't do minus negation of bool
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_NEGBOOL_NUM 70
/** \brief sdff error: type missmatch in arithmetic operation
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_ARITHMETICMISSMATCH_NUM 71
/** \brief sdff error: operation not defined for this datatype
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_OPNOTDEFFT_NUM 72
/** \brief sdff error: operation not defined for strings of different lengths
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_STRUNEQUALLENGTH_NUM 73
/** \brief sdff error: test in a loop has to return a boolean
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_LOOPTESTNOTBOOL_NUM 74
/** \brief sdff error: for the SUM function, the first argument was not a variable
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_SUM1NOTVARIABLE_NUM 75
/** \brief sdff error: SUM function: missmatch in types to sum up
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_SUMTYPEMISS_NUM 76
/** \brief sdff error: SUM function: missmatch in types to sum up
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_SUMTYPECMISS_NUM 77
/** \brief sdff error: PROD function: missmatch in types to sum up
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_PRODTYPEMISS_NUM 78
/** \brief sdff error: PROD function: missmatch in types to sum up
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_PRODTYPECMISS_NUM 79
/** \brief sdff error: for the PROD function, the first argument was not a variable
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_PROD1NOTVARIABLE_NUM 80
/** \brief sdff error: test in a if expression has to return a boolean
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_IFTESTNOTBOOL_NUM 81
/** \brief sdff error: time parameter of <tt>event</tt>- and <tt>relative_event</tt>-nodes is not numeric
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_TIMENOTNUMERIC_NUM 82
/** \brief sdff error: str2num(x): argument does not represent a number
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_STR2NUMNOTNUM_NUM 83
/** \brief sdff error: str2bool(x): argument does not represent a boolean
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_STR2BOOLNOTNUM_NUM 84
/** \brief sdff error: factorial(x): argument must be positive or zero
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_FACTORIALPOS_NUM 85
/** \brief sdff error: parser error: unknown token
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_UNKNOWNTOKEN_NUM 86
/** \brief sdff error: parser error: expeced other token
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_UNEXPECTEDTOKEN_NUM 87
/** \brief sdff error: parser error: expeced other token + additional information
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM 88
/** \brief sdff error: parser error: only variable names in function definition in LHS
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_FDEFNOVARNAME_NUM 89
/** \brief sdff error: parser error: only integer and double may be negated
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_NEGNOTNUMBER_NUM 90
/** \brief sdff error: channel parameter of <tt>channel</tt>--nodes is not a string
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_CHANNELNOTSTRING_NUM 91
/** \brief error: no negative times
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_NONEGTIME_NUM 92
/** \brief error: the channel XX is not defined
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_CNOTEXISTS_NUM 93
/** \brief error: collision of two elements
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_COLLISION_NUM 94
/** \brief error: the channel XX is not an output
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_CNOTOUTPUT_NUM 95
/** \brief error: element after end of sequence
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_AFTEREND_NUM 96
/** \brief error: parameter tau of exponential ramps may not be negative
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_TAUNOTNEG_NUM 97
/** \brief error: only analog ramps allowed
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_NODIGRAMP_NUM 98
/** \brief sdff error: function parameter count missmatch, message with you need at least N arguments!
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM 99
/** \brief sdff error: a sequence element function is not inside an event-block
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_ELEMENTNOTINEVENT_NUM 100
/** \brief sdff error: a sequence element function is not inside an channel-block
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_ELEMENTNOTINCHANNEL_NUM 101
/** \brief sdff error: missing time argument for channel assignment
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_CHASSIGNNOTIME_NUM 102
/** \brief sdff error: time argument for channel assignment is not number
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_CHASSIGNWRONGTIMETYPE_NUM 103
/** \brief sdff error: argument for channel assignment is not number or boolean
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_CHASSIGNWRONGTYPE_NUM 104
/** \brief sdff error: no brackets in variable assignment
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_VARASSIGNNOBRACKETS_NUM 105
/** \brief sdff error: sequence file does not exist
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_FILENOTEXISTS_NUM 106
/** \brief csv error: sequence file does not exist
 * \ingroup seq_error
 */
#define SEQUENCER_CSVERROR_FILENOTEXISTS_NUM 107
/** \brief sdff error: one of the required channels was not used
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_NOREQUIRED_NUM 108
/** \brief error: all channels in an input task have to have the same specs
 * \ingroup seq_error
 */
#define SEQUENCER_ERROR_INPUTUNEQUALSPECS_NUM 109
/** \brief error: input task only for analog channels
 * \ingroup seq_error
 */
#define SEQUENCER_ERROR_INPUTONLYANALOG_NUM 110
/** \brief error: one of the channels is in use
 * \ingroup seq_error
 */
#define SEQUENCER_ERROR_CHANNELINUSE_NUM 111
/** \brief error: no input task for this handle exists
 * \ingroup seq_error
 */
#define SEQUENCER_ERROR_INPUTTASKNE_NUM 112
/** \brief error: file/system error while writing in input_task_save
 * \ingroup seq_error
 */
#define SEQUENCER_ERROR_INPUTTASKFERROR_NUM 113
/** \brief error: a channel used in a sequence has to be synchronizable
 * \ingroup seq_error
 */
#define SEQUENCER_ERROR_CNOTSYNCH_NUM 114
/** \brief error: error during memory allocation
 * \ingroup seq_error
 */
#define SEQUENCER_ERROR_MEMALLOC_NUM 115
/** \brief sdff error: a synchronized output operation is not possible in a pre_event block
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_OUTPUTINPRE_NUM 116
/** \brief sdff error: index out of range
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_RANGE_NUM 117
/** \brief sdff error: too many function arguments
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_FUNCARGTOOMANY_NUM 118
/** \brief sdff error: argument XXX has to be non negative
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_NONNEG_NUM 119
/** \brief sdff error: a synchronized output operation with a duration is not possible in a last_event block
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_LONGOUTPUTINLAST_NUM 120
/** \brief sdff error: a delayed synchronized output operation is not possible in a pre_event or a last_event block
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_DELAYEDOUTPUTINPRE_NUM 121
/** \brief sdff error: error while working with a file in a SDFF file
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_FILEOPERROR_NUM 122
/** \brief sdff error: there exists no alias for the given channel
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_NOALIASFORCHANNEL_NUM 123
/** \brief sdff error: the specified alias does not exist
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_ALIASNOTEXISTS_NUM 124
/** \brief sdff error: channel names only allowed inside parameters block
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_CHANNELNAMESONLYINPARAM_NUM 125
/** \brief serial command: undefined parity identifier
 * \ingroup seq_error
 */
#define SEQUENCER_SERERROR_UNKNOWNPARITY_NUM 126
/** \brief serial command: you have to supply a positive parameter
 * \ingroup seq_error
 */
#define SEQUENCER_SERERROR_PARAMPOS_NUM 127
/** \brief sdff error: function parameter count missmatch, message with you need at between N and M arguments!
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_FUNCARGCMISSMIMA_NUM 128
/** \brief sdff error: function only allowed in pre_event
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_FUNCONLYPREEVENT_NUM 129
/** \brief sdff error: the supplied integer is not an ASCII character (0..255)
 * \ingroup seq_error
 */
#define SEQUENCER_SDFFERROR_NOTASCII_NUM 130
/** \brief serial command: serial port already opened
 * \ingroup seq_error
 */
#define SEQUENCER_SERERROR_PORTALREADYOPEN_NUM 131
/** \brief serial command: impossible argument
 * \ingroup seq_error
 */
#define SEQUENCER_SERERROR_IMPOSSIBLEARG_NUM 132
/** \brief serial command: win32 error
 * \ingroup seq_error
 */
#define SEQUENCER_SERERROR_WIN32ERROR_NUM 133
/** \brief serial command: undefined handshaking identifier
 * \ingroup seq_error
 */
#define SEQUENCER_SERERROR_UNKNOWNHANDSHAKING_NUM 134
/** \brief serial command: serial port not opened
 * \ingroup seq_error
 */
#define SEQUENCER_SERERROR_PORTNOTOPEN_NUM 135
/** \brief GPIB command: invalid adress (0..31)
 * \ingroup seq_error
 */
#define SEQUENCER_GPIBERROR_INVALIDADR_NUM 136
/** \brief GPIB command: ser-to-GPIB interface reported an error
 * \ingroup seq_error
 */
#define SEQUENCER_GPIBERROR_INTFCERROR_NUM 137
/** \brief error: invalid sample_timestep
 * \ingroup seq_error
 */
#define SEQUENCER_ERROR_INVALIDTIMESTEP_NUM 138
/** \brief error: variable timing not available
 * \ingroup seq_error
 */
#define SEQUENCER_ERROR_NOVARTIMING_NUM 139




/**
 * \brief Returns the error message for a given error.
 * \ingroup seq_error
 */
std::string get_errormessage(int num);

/**
 * \brief Returns the short error message for a given error.
 * \ingroup seq_error
 */
std::string get_errorsmessage(int num);


/**
 * \brief This class is used to throw errors within the sequencer classes.
 * \ingroup seq_error
 */
class exception
{
  protected:
    /** \brief the error message */
    std::string emessage;
    /** \brief where the error occured */
    std::string where;
    /** \brief internal error number */
    int number;
    /** \brief (optional) line number where the error occured */
    long line;
    /** \brief (optional) time when the error occured */
    double time;
  public:
    /** \brief virtual class destructor */
    virtual ~exception(){};

    /** \brief returns the error message */
    GetMacro(std::string, emessage);
    /** \brief returns where the error occured */
    GetMacro(std::string, where);
    /** \brief returns the error number */
    GetMacro(int, number);
    /** \brief class constructor which initialises the calss with an "unknown error" in an "unknown" place and number -1 */
    exception();
    /** \brief class constructor which initialises the calss with an error number, a message and the where information */
    exception(int number, std::string message, std::string where, long line=-1, double time=-1);
    /** \brief returns the error message (containing \c number und \c where) as a C++ string */
    std::string get_message();
    /** \brief returns the line number associated with the error message */
    GetMacro(long, line);
    /** \brief returns the time associated with the error message */
    GetMacro(double, time);
};


/** \brief this macro is replaced by code that throws an exception use this instead of direct \c throw ...
 *         to enable a later change in error handling
 * \ingroup seq_error
 */
#define SEQUENCER_ERROR(n, m, w) { throw sequencer::exception(n, m, w); }

/** \brief this macro is replaced by code that throws an exception use this instead of direct \c throw ...
 *         to enable a later change in error handling. This macro automatically gets the according error
 *         message using get_errormessage method
 * \ingroup seq_error
 */
#define SEQUENCER_ERRORN(n, w) { throw sequencer::exception(n, sequencer::get_errormessage(n), w); }

/** \brief this macro is replaced by code that throws an exception use this instead of direct \c throw ...
 *         to enable a later change in error handling. This macro automatically gets the according error
 *         message using get_errormessage method. Here you can supply a list of parameters and the errormessage
 *         will be formatted using them (uses format() method).
 * \ingroup seq_error
 */
#define SEQUENCER_ERRORV(n, w, args...) { throw sequencer::exception(n, format(sequencer::get_errormessage(n), args), w); }

}

#endif // SEQUENCERERROR_H
