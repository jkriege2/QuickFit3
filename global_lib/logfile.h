/*
  Name: logfile.h
  Copyright: (c) 2007
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/
/**
 * \defgroup logging Log System
 * \ingroup tools_classes
 *
 * This group contains a class for logging program information to files and to the console.
 */
/*@{*/

/** \file logfile.h
 *
 */

#include <string>
#include <cstdio>

#include <cstdlib>
#include <ctime>
#include <cstring>

#include "tools.h"

#ifndef __WINDOWS__
#if defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32)
#define __WINDOWS__
#endif
#endif

#ifdef __WINDOWS__
  /* headerfiles for Windows */
  #include <windows.h>
#endif


#ifndef LOGFILE_H
#define LOGFILE_H

/** \brief this is the indention increment when calling LogFile.inc_indent()
 * \ingroup tool_classes
 */
#define LOGFILE_INDENT_INC 2

/**
 * \brief A class for logging services. This class allows to output categorized log messages onto the console
 *        or into a specified log file.
 * \ingroup tool_classes
 *
 * \attention note that this class contains some windows specific code to color the console output. It will
 * compile on any other operating system supporting standard libc, but it won't show colored output on the
 * console! See these links for more information about colored console output on Windows systems:
 *   - <a href="http://www.codeguru.com/cpp/w-d/console/article.php/c3959/">http://www.codeguru.com/cpp/w-d/console/article.php/c3959/</a>
 *   - <a href="http://msdn2.microsoft.com/en-us/library/ms686047.aspx">http://msdn2.microsoft.com/en-us/library/ms686047.aspx</a>
 * .
 */
class LogFile {
    protected:
        /** \brief filename of the log file, standard is \c logfile.log */
        std::string log_filename;

        /** \brief file handle for the log file */
        FILE* log_file;

        /** \brief file handle for the console, standard is \c stderr */
        FILE* log_console;

        /** \brief determines whether message will be put onto the console log_console */
        bool log_to_console;

        /** \brief determines whether message will be put into the file log_filename */
        bool log_to_file;

        /** \brief determines whether to append to a log file (true, default) or to generate a new one on each run (false) */
        bool log_file_append;

        /** \brief internal: this is set true if the last written character was a linebreak \c 0x0A */
        bool last_was_linebreak;

        /** \brief internal: the current depth of indention */
        int current_indent;

        /** \brief internal: add indention spaces if necessary */
        virtual void do_indent();

        /** \brief internal: add indention spaces after each linebreak in the string */
        virtual std::string indentify(std::string text);

		/** \brief internal: output some text message without indetify step  */
		virtual void raw_log_text(std::string message);

	public:
		/** \brief class constructor. */
		LogFile();
		/** \brief class destructor */
		virtual ~LogFile();

		/** \brief set the console handle */
		SetMacro(FILE*, log_console);

		/** \brief get the current log file name */
		GetMacro(std::string, log_filename);
		/** \brief set whether to log to the console */
		SetMacro(bool, log_to_console);
		/** \brief get whether we are logging to the console */
		GetMacro(bool, log_to_console);
		/** \brief get whether we are logging to a file */
		GetMacro(bool, log_to_file);
		/** \brief set whether to append to a log_file (true) or to generate a new one (false) */
		SetMacro(bool, log_file_append);
		/** \brief get whether to append to a log file (true) or to generate a new one (false). */
		GetMacro(bool, log_file_append);

		/** \brief open a new log file and activate loggin to file */
		virtual void open_logfile(std::string filename);

		/** \brief close the current log file (if one is opened) and deactivate loggin to file */
		virtual void close_logfile();

		/** \brief output some text message */
		virtual void log_text(std::string message);

		/** \brief output some warning message */
		virtual void log_warning(std::string message);

		/** \brief output some error message */
		virtual void log_error(std::string message);

		/** \brief output a linebreak */
		virtual void log_linebreak();

		/** \brief output a horicontal line + linebreak */
		virtual void log_line();

		/** \brief output a double horicontal line + linebreak */
		virtual void log_dline();

		/** \brief output a horicontal line + linebreak which incorporates the text \c caption */
        virtual void log_header(std::string caption);

		/** \brief output a double horicontal line + linebreak which incorporates the text \c caption */
        virtual void log_dheader(std::string caption);

		/** \brief output a horicontal line + linebreak, as an error message */
		virtual void log_error_line();

		/** \brief output a double horicontal line + linebreak, as an error message */
		virtual void log_error_dline();

		/** \brief output a horicontal line + linebreak which incorporates the text \c caption, as an error message */
        virtual void log_error_header(std::string caption);

		/** \brief output a double horicontal line + linebreak which incorporates the text \c caption, as an error message */
        virtual void log_error_dheader(std::string caption);

		/** \brief output a horicontal line + linebreak, as an warning message */
		virtual void log_warning_line();

		/** \brief output a double horicontal line + linebreak, as an warning message */
		virtual void log_warning_dline();

		/** \brief output a horicontal line + linebreak which incorporates the text \c caption, as an warning message */
        virtual void log_warning_header(std::string caption);

		/** \brief output a double horicontal line + linebreak which incorporates the text \c caption, as an warning message */
        virtual void log_warning_dheader(std::string caption);

		/** \brief increase the indention depth */
        virtual inline void inc_indent() { current_indent+=LOGFILE_INDENT_INC; };

		/** \brief decrease the indention depth */
        virtual inline void dec_indent() { current_indent-=LOGFILE_INDENT_INC; if (current_indent<0) current_indent=0; };
};





#endif // LOGFILE_H

/*@}*/
