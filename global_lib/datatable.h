/*
  Name: datatable.h
  Copyright: (c) 2007
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/
/**
 * \defgroup datatable double value grid (Data table)
 * \ingroup tools_containers
 *
 * The classes in this group form a 2D grid of double that can be used to store data. It also
 * contains a versatile CSV importer. The simples use is:
 * \code
 *   try {
 *     datatable tab;                   // instanciate
 *     tab.readFile("test.csv");        // load some csv file
 *     tab.set(0,0,10);                 // set some data
 *     cout<<tab.as_string()<<endl;     // output table on screen
 *     tab.save_sylk("test.slk");       // save changed table in SYLK format (for Excel)
 *   } catch(datatable_exception e) {   // error handling with exceptions
 *     cout<<e.get_message()<<endl;
 *   }
 * \endcode
 *
 * It is also possible to change the error handling from using exceptions to calling a specific
 * error handling function. This can be usefull in programs that don't support exceptions.
 * To do so, use this cod:
 * \code
 * void datatable_error(int number, std::string message, std::string where) {
 *   if (number==-1) cout<<"error "+inttostr(number)+": "+message+ "(in "+where+")";
 *   cout<<"error: "+message+ "[in "+where+"]"<<endl;
 *   system("PAUSE");
 * }
 *
 * int main() {
 *   datatable_exception_function=datatable_error;  // make error ahndler known
 *   datatable tab;                   // instanciate
 *   tab.readFile("test.csv");        // load some csv file
 *   tab.set(0,0,10);                 // set some data
 *   cout<<tab.as_string()<<endl;     // output table on screen
 *   tab.save_sylk("test.slk");       // save changed table in SYLK format (for Excel)
 * }
 * \endcode
 */

/** \file datatable.h
 *  \ingroup datatable
 */

#include <vector>
#include <string>
#include <cstdio>
#include <cmath>
#include <iostream>
#include <fstream>
#include <exception>
#include <cstring>
#include "tools.h"

#ifndef DATATABLE_H
#define DATATABLE_H

/**
 * \defgroup datatable_errors error handling
 *  \ingroup datatable
 */
/*@{*/
#define DATATABLE_ERROR_RANGE_NUM 1
#define DATATABLE_ERROR_RANGE_MESSAGE "grid index out of range"
#define DATATABLE_ERROR_VMEM_NUM 2
#define DATATABLE_ERROR_VMEM_MESSAGE "ran out of virtual memory"
#define DATATABLE_ERROR_FILE_NUM 3
#define DATATABLE_ERROR_FILE_MESSAGE "can't open file"
#define DATATABLE_ERROR_UTOKEN_NUM 4
#define DATATABLE_ERROR_UTOKEN_MESSAGE "unknown token in csv file"
#define DATATABLE_ERROR_TITLEINDEXRANGE_NUM 5
#define DATATABLE_ERROR_TITLEINDEXRANGE_MESSAGE "title index out of range"
#define DATATABLE_ERROR_UNKNOWNTITLE_NUM 6
#define DATATABLE_ERROR_UNKNOWNTITLE_MESSAGE "unknown title"

/**
 * \brief exception class for datatable
 *
 * an exception of this type is thrown whenever an error inside the datatable
 * class occurs.
 *
 * an error message concists of a number, the actual message and the information where
 * this error occured. If number==-1 then no number is displayed.
 *
 * for information about usage, see \link datatable group description \endlink
 *
 * <b>IMPORTAN NOTICE:</b> If you do not want to use the exception handling which throws
 * datatable_exception exceptions, but want to write your own error handling, you should write your own
 * error handler and assign it (function pointer) to the global variable datatable_exception_function.
 * If this is not NULL  this function will be called instead of throwing an exception.
 */
// the exception class is used in the standard case (NO_EXCEPTIONS not defined)
class datatable_exception : public std::exception  {
    private:
        /** \brief the error message */
        std::string message;
        /** \brief where did the error occur */
        std::string where;
        /** \brief internal error number */
        int number;
    public:
        /** \brief class constructor which initialises the calss with an error number, a message and the where information */
        datatable_exception(int number, std::string message, std::string where) throw();

        /** \brief class destructor */
        virtual ~datatable_exception() throw() {};
        /** \brief returns the error message (containing \c number und \c where) as a C++ string */
        std::string get_message() const;

		/** \brief returns the error description as C string */
		virtual const char* what() const throw() { return get_message().c_str(); };
};




/** \brief macro that throws an exception or calls an error handler */
#define DATATABLE_ERROR(n, m, w)  { if (datatable_exception_function!=NULL) { datatable_exception_function(n, m, w); } else { throw datatable_exception(n, m, w); } };

/** \brief type for a custom error handler. This an alternative error handling ... may be used together with Matlab in a MEX file! */
typedef void (*datatable_exceptionf)(int, std::string, std::string);
#ifdef COMPILING_THIS_DATATABLE
/** \brief if this is NULL then an exception may be thrown otherwise this should point to an error handler that will be called. */
datatable_exceptionf datatable_exception_function=NULL;
#else
extern datatable_exceptionf datatable_exception_function;
#endif
/*@}*/



/**
 * \defgroup datatables_aux auxiliary definitions and tools
 *  \ingroup datatable
 */
/*@{*/
/** \brief this struct represents one value in the table  */
typedef struct {
  double value;
  bool empty;
} datatable_value;

/** \brief the number of reserved (not accessible!) lines collumns is increased by this factor when a resize occurs */
#define DATATABLE_RESIZE_FACTOR 1.2

/** \brief allowed tokens for CSV file parser*/
enum datatable_TOKEN {
	datatable_EOF,
    datatable_LINEBREAK,
    datatable_SEPARATOR,
    datatable_COMMENT,
    datatable_TITLECOMMENT,
    datatable_VALUE
};
/*@}*/





/**
 * \defgroup datatable_class datatable class
 *  \ingroup datatable
 */
/*@{*/

/**
 * \brief class to manage a 2D grid of double values
 *
 * This class implements a data table which can hold a 2D grid of double values.
 * At each position there may either be a value or no data.
 *
 * Each collumn of data is stored in an array of size \c column_size. This means that
 * there is memory reserved for \c column_size lines in one column. It does not necessarily
 * mean that there have to be \c column_size entries in that column. The actually
 * accessible number of lines is \c line_count. With this definition it is possible to
 * easily extend the grid without having to reallocate the memory which
 * needs a lot of time. So if one increases the number of lines so that \code line_count + delta_lines < column_size\endcode
 * no reallocation is needed and line_count can simply be increased. On the other hand if one
 * increases \c line_count above \c column_size then a realloc on all columns will occur to
 * get the memory for the new lines. If this happens there will be more memory reserved
 * than was actually asked for. The same is done for the lines.
 *
 * For information about usage, see \link datatable group description \endlink
 *
 * This class also provides an import routine for CSV (Comma Separated Value) files. These files have a simple
 * structure:
 * \verbatim
csv_file         ->   { csv_line LINEBREAK }
csv_line         ->   ( [ WHITESPACE ] NUMBER { , NUMBER } ) | comment}
comment          ->     # TEXT_WITHOUT_LB  LINEBREAK
                      | #$ [ WHITESPACE ] NAME { , NAME }  LINEBREAK
NAME             ->   character string without commas and linebreaks
NUMBER           ->   number in scientific notation (-1.234E-56)
TEXT_WITHOUT_LB  ->   all ascii characters except linebreak (LB) and carriage return (CR)
WHITESPACE       ->   LB | CR | SPACE | TAB
   \endverbatim
 * The special comments that start with \c #$ give the names f the columns in the file. Such a comment
 * may only appear once inside a CSV file. If more than one appear only the last of them is beeing evaluated.
 * The column titles are stored in the private member titles and may be accessed via get_title(), set_title()
 * and clear_titles(). There are always as many column titles as columns!
 *
 * \test a test application for the class datatable can be found in the file \link test_csv.cpp.
 */
class datatable
{
    private:
        std::vector<std::string> titles;
        std::string title_line;

        unsigned long column_count;
        unsigned long column_size;
        unsigned long line_count;
        unsigned long line_size;
        datatable_value** data;

        bool csv_include_line_number;
        std::string csv_separator;
        char csv_comma;
        char csv_in_separator;
        char csv_in_comment;

        datatable_TOKEN current_token;
        std::string current_value_string;
        FILE* csv_file;
        /** \brief read the next token from the csv input file stream. The token will be saved in current_token and returned from the function. */
        datatable_TOKEN get_token();
        /** \brief returns the number of lines in the given file */
        unsigned long count_lines(std::string filename);

        void resize_titles();

        /** \brief BubbleSort tool: swaps the lines \c i and \c j */
        void qswap(long long i, long long j);
        /** \brief BubbleSort tool: compares \c a and \c b and returns true if a<b. This method also look at the empty properties, so that empty lines are always smaller than non-empty lines. */
        bool qless(datatable_value a, datatable_value b);
        /** \brief BubbleSort tool: compares \c a and \c b and returns true if a>b. This method also look at the empty properties, so that empty lines are always smaller than non-empty lines. */
        bool qlarger(datatable_value a, datatable_value b);
	public:
		/** \brief class constructor, the initial grid size is 2 collumns with 10 lines each */
		datatable();
		/** \brief class constructor, the initial grid size is \c columns collumns with \c lines lines each */
		datatable(unsigned long columns, unsigned long lines);
		/** \brief class destructor */
		~datatable();

		/** \brief returns the value stored at position ( \c x , \c y ) */
		double get(unsigned long x, unsigned long y);
		/** \brief returns true when position ( \c x , \c y ) is empty */
		bool is_empty(unsigned long x, unsigned long y);
		/** \brief sets the value at position ( \c x , \c y ) */
		void set(unsigned long x, unsigned long y, double value);
		/** \brief sets the position ( \c x , \c y ) to empty (and the corresponding value to 0)*/
		void set_empty(unsigned long x, unsigned long y);

		/** \brief clears the table and sets the dimensions to 0x0, but without releasing the memory */
		void clear();
		/** \brief clears the table and sets the dimensions to 0x0 and thereby releasing the memory */
		void free();

		/** \brief returns the accessible number of lines in each column*/
		unsigned long get_line_count();
		/** \brief returns the accessible number of columns */
		unsigned long get_column_count();

		/** \brief set the table to a new size
         *
         * this function does the following
         *   -# increase the number of lines of the existing collumns. This is done collumn-wise by use of realloc
         *      (C stdlib). The new cells of the grid are initialized to be empty.
         *   -# Now we have to add new collumns. Therefore check whether \c newx<collumn_size. If this is true
         *      \c collumn_count can simply be set to \c newy. If this is not true the we have to allocate memory
         *      for the new lines in the \\u new collumns only as the old columns have already been expanded in step 1.
         *      This is done using malloc (from C stdlib). The new cells will be initialized to be empty.
         *
         * This method also grants that there is at least one cell in the internal memory so that have 0 lines can
         * not lead to any difficulties with the above algorithm. After this resize there is more memory reserved
         * than actually needed:
         *     \code
         *       column_size=int(newy*DATATABLE_RESIZE_FACTOR)
         *       line_size=int(newx*DATATABLE_RESIZE_FACTOR)
         *     \endcode
         */
		void resize(unsigned long newx, unsigned long newy);

		/** \brief returns a string with a formatted table */
		std::string as_string();
		/** \brief returns a string with a formatted table. This table also includes the inaccessible memory. */
		std::string as_string_with_unaccessible();

		/** \brief returns the name of x-th column */
		std::string get_title(unsigned long x);
		/** \brief set the name of x-th column */
		void set_title(unsigned long x, std::string name);
		/** \brief clear all column titles */
		void clear_titles();
		/** \brief returns the number of the column with name \c name */
		unsigned long get_column_by_title(std::string name);
		/** \brief returns true if the given title exists in the object */
		bool title_exists(std::string name);


		/** \brief save as SYLK file for processing with spreadsheet applications like Excel (see <A HREF="http://en.wikipedia.org/wiki/SYmbolic_LinK_(SYLK)">SYLK in Wikipedia.com</A> and <A HREF="http://www.pindari.com/sylk.html">http://www.pindari.com/sylk.html</A>") */
		void save_sylk(std::string filename);
		/** \brief save as formatted text file */
		void save_formatted_txt(std::string filename);
		/** \brief save as CSV file (comma separated values, see <A HREF="http://en.wikipedia.org/wiki/Comma-separated_values">CSV in Wikipedia.com</A>) */
		void save_csv(std::string filename);
		/** \brief sets the options of the CSV file format (whether to include the line number as a separate column, which separator between collumns and which character as decimal point*/
		void set_csv_options(bool include_line_number=false, std::string separator=", ", char comma='.');
		/** \brief parses a csv file and write the contents to the object. All data in the object will be lost.
         *
         * This method uses get_token() function to tokenize the input file and then parse it. It implements a rather
         * simple parser that can cope with comments (startet with \c commentchar and ended with a line break) and different
         * column separators (given as \c separatorchar). Only numbers and empty columns are allowed in the file. Numbers
         * may be written inscientific notation [+|-]ddd.ddd[e|E][-]ddd where '.' or ',' may be used as decimal separator.
         * The second only if ',' is not also the separatorchar.
         */
        void load_csv(std::string filename, char separatorchar=',', char commentchar='#');
        /** \brief sort the table by comparing the values in column \c bycolumn. This method uses BubbleSort. */
        void sort(unsigned long bycolumn=0);
        /** \brief return the maximum value of one column */
        double column_max(unsigned long column=0);
        /** \brief return the mminimum value of one column */
        double column_min(unsigned long column=0);
        /** \brief return the average of one column */
        double column_average(unsigned long column=0);
        /** \brief return the sum of one column */
        double column_sum(unsigned long column=0);
};
/*@}*/

#endif // DATATABLE_H
