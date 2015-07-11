/*
  Name: baseimporter.h
  Copyright: (c) 2007
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/

/** \file baseimporter.h
 * \ingroup hlseqgen_base
 *
 */

#ifndef BASEIMPORTER_H
#define BASEIMPORTER_H

#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <cstdio>
#include <stdint.h>

#ifndef __WINDOWS__
#if defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32)
#define __WINDOWS__
#endif
#endif

#ifdef __WINDOWS__
  /* headerfiles for Windows */
  #include <windows.h>
#endif




#include "tools.h"
#include "sequencererror.h"
#include "channelmanager.h"
#include "sequencegenerator.h"


namespace sequencer {



/** \brief list of possible event types
 * \ingroup hlseqgen_base
 */
enum bi_event_type {
	biInfo,        /*!< \brief just some information */
	biWarning,     /*!< \brief warning message */
	biError        /*!< \brief error message  */
};

/** \brief modes for the position in the file
 * \ingroup hlseqgen_base
 */
enum bi_pos_mode {
	biTime,         /*!< \brief show moment in time only */
	biLine,         /*!< \brief show position only (line number) */
	biBoth          /*!< \brief show time and position */
};


/** \brief this represents an event, like a warning or an error. This struct is used to manage errors within the CSVsequenceParser class.
 * \ingroup hlseqgen_base
 */
typedef struct {
  bi_event_type type;   /*!< \brief type of the event */
  long position;        /*!< \brief position of the event (line number) */
  double time;          /*!< time step when the event occured */
  std::string text;     /*!< \brief message  */
  std::string stext;    /*!< \brief short message */
  int num;              /*!< \brief error number */
  std::string filename; /*!< \brief file where the error occured */
} bi_event;

/**
 * \brief this class implements basic tools that are usefull when writig importer classes for
 *        sequences. This mainly incorporates error reporting utilities.
 * \ingroup hlseqgen_base
 *
 * You can use these protected methods to add error and warning messages:
 *   - add_error()
 *   - add_warning()
 * .
 *
 * There are some options for the output of errors and warnings:
 *   - get_report() returns a string which contains all the error and warning messages that appeared
 *     during the import of the CSV file.
 *   - print_report() prints the errors and warnings into a given file. If you don't supply a FILE pointer
 *     the output will go to \c stderr. This method uses (OS specific) methods to color the output (errors in red!)
 *   - get_event() and get_events_count() allow to access the single warning and error messages.
 * .
 *
 * You can find descendents of this base class in these classes:
 *   - sequencer::CSVsequenceparser
 *   - sequencer::SDFFparser
 * .
 *
 * If you want to write your own implementation you will typically add some methods that are called \c import()
 * or \c generate() that will read a file and generate a sequence from it. You should use the sequenceGenerator
 * that \c generator points at to output the sequence.
 *
 * By default the position member of an bi_event will be output. If you want to change that you can
 * set the protected property \c position_mode to \c biTime which tells the class to output \c time
 * as a moment in time (given in microseconds).
 *
 */
class baseImporter {
  protected:
    /** \brief pointer to a sequence generator class */
    sequenceGenerator* generator;

    /** \brief pointer to a channel manager class */
    channelManager* cmanager;

    /** \brief hidden standard constructor */
    baseImporter() {};

    /** \brief contains the warnings and errors generated during parsing */
    std::vector<bi_event> events;

    /** \brief number of errors while parsing */
    unsigned long errornum;

    /** \brief number of warnings while parsing */
    unsigned long warningnum;


    /** \brief specifies how to interpret the position property of bi_event */
    bi_pos_mode position_mode;

    /** depending in position_mode this class returns a string \c '(t=<e.time>)' or \c '(l.<e.position>)' or '(l.<e.position>, t=<e.time>)' */
    virtual std::string get_position(bi_event e);
  public:
    /** \brief class constructor. \c sg is a pointer to a sequencer::sequenceGenerator object that will be used by the class */
    baseImporter(sequenceGenerator* sg, channelManager* cm);
    /** \brief class destructor */
    virtual ~baseImporter() {};

    /* \brief set the channelManager object to be used */
    SetMacro(channelManager*, cmanager);
    /* \brief get the channelManager object to be used */
    GetMacro(channelManager*, cmanager);
    /* \brief set the sequenceManager object to be used */
    SetMacro(sequenceGenerator*, generator);
    /* \brief get the sequenceManager object to be used */
    GetMacro(sequenceGenerator*, generator);
    /* \brief return the number of errors during the last import() call */
    GetMacro(unsigned long, errornum);
    /* \brief return the number of warnings during the last import() call */
    GetMacro(unsigned long, warningnum);

    /** \brief clear the list of events */
    inline virtual void clear_events() {
      errornum=0;
      warningnum=0;
      events.clear();
    };

    /** \brief get the number of events */
    inline virtual unsigned long get_events_count() {return events.size(); };

    /** \brief get the i-th events */
    inline virtual bi_event get_event(unsigned long n) {
      if (n>=get_events_count()) SEQUENCER_ERRORN(SEQUENCER_ERROR_RANGE_NUM, "CSVsequenceParser::get_event("+inttostr(n)+")");
      return events[n];
    };

    /** \brief generate a report that contains all errors and warnings as a std::string */
    virtual std::string get_report();

    /** \brief print a report that contains all errors and warnings into the given file.
     *
     * If no file is supplied (\c con=NULL ) the stderr is used. This method also uses (OS specific) methods to
     * print a colored (console) output. So use this method to output to the console.
     */
    virtual void print_report(FILE* con=NULL);


    /** \brief add an error message to the report which does not contain a time information*/
    inline virtual void add_error_t(int number, std::string message, std::string smessage, std::string filename, double time) {
      add_error(number, message, smessage, filename, time, 0);
    };
    /** \brief add an error message to the report which does not contain a time information and automatically get the error message using get_errormessage() */
    inline virtual void add_error_t(int number, std::string filename, double time) {
      add_error(number, get_errormessage(number), get_errorsmessage(number), filename, time, 0);
    };

    /** \brief add a warning message to the report which does not contain a time information*/
    inline virtual void add_warning_t(int number, std::string message, std::string smessage, std::string filename, double time) {
      add_error(number, message, smessage, filename, time, 0);
    };
    /** \brief add a warning message to the report which does not contain a time information and automatically get the error message using get_errormessage() */
    inline virtual void add_warning_t(int number, std::string filename, double time) {
      add_error(number, get_errormessage(number), get_errorsmessage(number), filename, time, 0);
    };

    /** \brief add an error message to the report which does contain a position information*/
    inline virtual void add_error_p(int number, std::string message, std::string smessage, std::string filename, long position) {
      add_error(number, message, smessage, filename, 0, position);
    };
    /** \brief add an error message to the report which does contain a position information and automatically get the error message using get_errormessage() */
    inline virtual void add_error_p(int number, std::string filename, long position) {
      add_error(number, get_errormessage(number), get_errorsmessage(number), filename, 0, position);
    };

    /** \brief add a warning message to the report which does not contain a position information*/
    inline virtual void add_warning_p(int number, std::string message, std::string smessage, std::string filename, long position) {add_error(number, message, smessage, filename, 0, position); };
    /** \brief add a warning message to the report which does not contain a position information and automatically get the error message using get_errormessage() */
    inline virtual void add_warning_p(int number, std::string filename, long position) {add_error(number, get_errormessage(number), get_errorsmessage(number), filename, 0, position); };

    /** \brief add an error message to the report*/
    virtual void add_error(int number, std::string message, std::string smessage, std::string filename, double time, long position);
    /** \brief add an error message to the report and automatically get the error message using get_errormessage() */
    inline virtual void add_error(int number, std::string filename, double time, long position) {add_error(number, get_errormessage(number), get_errorsmessage(number), filename, time, position); };

    /** \brief add a warning message to the report*/
    virtual void add_warning(int number, std::string message, std::string smessage, std::string filename, double time, long position);
    /** \brief add a warning message to the report and automatically get the warning message using get_errormessage() */
    inline virtual void add_warning(int number, std::string filename, double time, long position) {add_warning(number, get_errormessage(number), get_errorsmessage(number), filename, time, position); };
};

}
#endif // BASEIMPORTER_H


