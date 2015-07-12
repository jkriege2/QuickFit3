/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

    

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/



/** \file tools.h
 *  \ingroup tools
 *
 * This file contains a collection of tool routines from a diversity of areas. It is only depending on
 * C++ StdLib headers, so you can simply compile it into any C++ project.
 */


#ifndef cpptools_h
#define cpptools_h


#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <time.h>
#include <math.h>
#include <sys/stat.h>
#include <stdint.h>
#include <stdexcept>
#include "lib_imexport.h"



#ifndef __WINDOWS__
# if defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32)
#  define __WINDOWS__
# endif
#endif

#ifndef __LINUX__
# if defined(linux)
#  define __LINUX__
# endif
#endif

#undef JKTOOLS_TIMER_USE_TIME
#if defined(__WINDOWS__)
  #include<windows.h>
#elif defined(__LINUX__)
  #include <sys/time.h>
#else
  #define JKTOOLS_TIMER_USE_TIME
#endif

#ifndef __WINDOWS__
# ifndef __LINUX__
#  warning("these methods are ment to be used under windows or linux ... no other system were tested")
# endif
#endif

/* This just distinguishes between the different path formats on Windows and Unix:
 *   - on Windows you use a backslash '\' do separate directories
 *   - in Unix you use a slash '/' to separate directories
 */
#ifdef __WINDOWS__
  /** \brief a separator between two directories in a path between \c " quotes */
  #define PATHSEPARATOR_STRING "\\"
  /** \brief a separator between two directories in a path between \c ' quotes */
  #define PATHSEPARATOR_CHAR '\\'
  #include<windows.h>
#else
  /** \brief a separator between two directories in a path between \c " quotes */
  #define PATHSEPARATOR_STRING "/"
  /** \brief a separator between two directories in a path between \c ' quotes */
  #define PATHSEPARATOR_CHAR '/'
#endif



/**
 * \defgroup tools_getset get_var and set_var macros
 * \ingroup tools
 *
 * The macros in this group have the purpose to make writing \c get_<varname>() and \c set_<varname>(<varname>)
 * methods for classes easier. They can be used by giving the type and the name of a private variable for which
 * \c get_ and \c set_ methods should be created:
 * \code
 *   class a {
 *     private:
 *       int var1;
 *       std::strign var2;
 *     public:
 *       GetSetMacro(int, var1);
 *       GetMacro(std::string, var2);
 *   }
 * \endcode
 * This code will create a \c set_var1, a \c set_var2 and a \c get_var1 method with the apropriate types.
 * All functions will be declared \c virtual and can thus be easily overloaded in inheriting classes. They are also
 * declared \c inline so the compiler may optimize them by really inlining them.
 *
 * The GetSetMacro creates get and set methods while the GetMacro and the SetMacro only create one of the
 * both.
 */
/*@{*/

/**
 * \brief create get_varname() and set_varname(type __value) methods/functions
 * inside a class, where \c type is the type of \c varname and \c varname is a
 * previously declared private variable that should be accessed by these
 * methodes
*/
#define GET_SET_MACRO(type,varname) \
    typedef type typedef_set_##varname ;\
  /** \brief sets the property varname to the specified \a __value. \details Description of the parameter varname is: <CENTER>\copybrief varname.</CENTER> \see varname for more information */ \
  inline virtual void set_##varname (const typedef_set_##varname & __value)  \
  { \
    this->varname = __value; \
  }; \
  /** \brief returns the property varname. \see varname for more information */ \
  inline virtual type get_##varname () const  \
  {\
    return this->varname; \
  };
#define GET_SET_VMACRO(type,varname) \
    typedef type typedef_set_##varname ;\
  /** \brief sets the property varname to the specified \a __value. \details Description of the parameter varname is: <CENTER>\copybrief varname.</CENTER> \see varname for more information */ \
  inline virtual void set_##varname (const typedef_set_##varname & __value)  \
  { \
    this->varname = __value; \
  }; \
  /** \brief returns the property varname. \see varname for more information */ \
  inline virtual type get_##varname () const  \
  {\
    return this->varname; \
  };
#define GetSetMacro(type,varname) GET_SET_MACRO(type,varname)

/**
 * \brief create get_varname() and set_varname(type __value) methods/functions
 * inside a class, where \c type is the type of \c varname and \c varname is a
 * previously declared private variable that should be accessed by these
 * methodes. In addition this will set the property paramsChanged to true, which
 * you will have to declare in your class.
*/
#define GET_SET_MACRO_P(type,varname) \
    typedef type typedef_set_##varname ;\
  inline virtual void set_##varname (const typedef_set_##varname & __value) /** \brief sets the property varname to the specified \a __value. \details Description of the parameter varname is: <CENTER>\copybrief varname.</CENTER> \see varname for more information */ \
  { \
    this->varname = __value; \
    this-> paramsChanged=true; \
  }; \
  inline virtual type get_##varname () const /** \brief returns the property varname. \see varname for more information */ \
  {\
    return this->varname; \
  };
#define GetSetMacroP(type,varname) GET_SET_MACRO_P(type,varname)

/**
 * \brief like GetSetMacroP(), but adds the instruction \a inst to the set method. This may be used
 *        to update a component after a value has been set.
*/
#define GET_SET_MACRO_IP(type,varname,inst) \
    typedef type typedef_set_##varname ;\
  /** \brief sets the property varname to the specified \a __value. \details Description of the parameter varname is: <CENTER>\copybrief varname.</CENTER> \see varname for more information */ \
  inline virtual void set_##varname (const typedef_set_##varname & __value)   \
  { \
    if (this->varname != __value) { \
        this->varname = __value; \
        this-> paramsChanged=true; \
        inst; \
    } \
  }; \
  /** \brief returns the property varname. \see varname for more information */ \
  inline virtual type get_##varname () const  \
  { \
    return this->varname; \
  };
#define GetSetMacroIP(type,varname,inst) GET_SET_MACRO_IP(type,varname,inst)

/**
 * \brief like GetSetMacro(), but adds the instruction \a inst to the set method. This may be used
 *        to update a component after a value has been set.
*/
#define GET_SET_MACRO_I(type,varname,inst) \
    typedef type typedef_set_##varname ;\
    /** \brief sets the property varname to the specified \a __value. \details Description of the parameter varname is: <CENTER>\copybrief varname.</CENTER> \see varname for more information */ \
    inline virtual void set_##varname (const typedef_set_##varname & __value)  \
    {\
        if (this->varname != __value) { \
            this->varname = __value; \
            inst; \
        } \
    }; \
    /** \brief returns the property varname. \see varname for more information */ \
    inline virtual type get_##varname () const  \
    {\
        return this->varname; \
    };
#define GetSetMacroI(type,varname,inst) GET_SET_MACRO_I(type,varname,inst)

/**
 * \brief sets two given properties at the same time, i.e. with one setter function with the name <code>set_<name>(type __value, type2 __value2)</code>. The getter methods are still separate. This may be used
 *        to update a component after a value has been set.
*/
#define GET_SET_MACRO_TWO(name,type,varname,type2,varname2) \
    /** \brief sets the properties varname and varname2 to the specified \a __value and \a __value2. \details Description of the parameter varname is: <CENTER>\copybrief varname.</CENTER> \details Description of the parameter varname2 is: <CENTER>\copybrief varname2.</CENTER> \see varname and varname2 for more information */ \
    inline virtual void set_##name (type __value, type2 __value2)  \
    {\
        bool set=false; \
        if (this->varname != __value) { \
            this->varname = __value; \
        } \
        if (this->varname2 != __value2) { \
            this->varname2 = __value2; \
        } \
    }; \
    /** \brief returns the property varname. \see varname for more information */ \
    inline virtual type get_##varname () const  \
    {\
        return this->varname; \
    }; \
    /** \brief returns the property varname2. \see varname2 for more information */\
    inline virtual type2 get_##varname2 () const  \
    {\
        return this->varname2; \
    };
#define GetSetMacroTwo(name,type,varname,type2,varname2) GET_SET_MACRO_TWO(name,type,varname,type2,varname2)

/**
 * \brief like GetSetMacroTwo(), but adds the instruction \a inst to the set method. This may be used
 *        to update a component after a value has been set.
*/
#define GET_SET_MACRO_TWO_I(name,type,varname,type2,varname2,inst) \
    /** \brief sets the properties varname and varname2 to the specified \a __value and \a __value2. \details Description of the parameter varname is: <CENTER>\copybrief varname.</CENTER> \details Description of the parameter varname2 is: <CENTER>\copybrief varname2.</CENTER> \see varname and varname2 for more information */ \
    inline virtual void set_##name (type __value, type2 __value2)  \
    {\
        bool set=false; \
        if (this->varname != __value) { \
            this->varname = __value; \
            set=true; \
        } \
        if (this->varname2 != __value2) { \
            this->varname2 = __value2; \
            set=true; \
        } \
        if (set) { \
            inst; \
        } \
    }; \
    /** \brief returns the property varname. \see varname for more information */ \
    inline virtual type get_##varname () const  \
    {\
        return this->varname; \
    }; \
    /** \brief returns the property varname2. \see varname2 for more information */ \
    inline virtual type2 get_##varname2 () const  \
    {\
        return this->varname2; \
    };
#define GetSetMacroTwoI(name,type,varname,type2,varname2,inst) GET_SET_MACRO_TWO_I(name,type,varname,type2,varname2,inst)


/**
 * \brief like GetSetMacro(), but adds the instruction \a inst to the set method. This may be used
 *        to update a component after a value has been set. Alwys updates (no comparison of current
 *        and former value
*/
#define GET_SET_MACRO_INC(type,varname,inst) \
    typedef type typedef_set_##varname ;\
    /** \brief sets the property varname to the specified \a __value. \details Description of the parameter varname is: <CENTER>\copybrief varname.</CENTER> \see varname for more information */ \
    inline virtual void set_##varname (typedef_set_##varnam __value)  \
    {\
        this->varname = __value; \
        inst; \
    }; \
    /** \brief returns the property varname. \see varname for more information */ \
    inline virtual type get_##varname () const  \
    {\
        return this->varname; \
    };
#define GetSetMacroINC(type,varname,inst) GET_SET_MACRO_INC(type,varname,inst)

/** \brief create get_varname() and set_varname(type __value) methods/functions
 * inside a class, where \c type is the type of \c varname and \c varname is a
 * previously declared private variable that should be accessed by these
 * methodes
*/
#define GET_MACRO(type,varname) \
    /** \brief returns the property varname. \details Description of the parameter varname is:  <CENTER>\copybrief varname.</CENTER>. \see varname for more information */ \
    inline virtual type get_##varname() const  \
    {   return this->varname;   };

#define GetMacro(type,varname) GET_MACRO(type,varname)

/**
 * \brief create set_varname(type __value) methods/functions
 * inside a class, where \c type is the type of \c varname and \c varname is a
 * previously declared private variable that should be accessed by these
 * methodes
*/
#define SET_MACRO(type,varname) \
    typedef type typedef_set_##varname ;\
    /** \brief sets the property varname to the specified \a __value. \details Description of the parameter varname is:  <CENTER>\copybrief varname.</CENTER> \see varname for more information */ \
    inline virtual void set_##varname (const typedef_set_##varname & __value)  \
    { \
        this->varname = __value; \
    };
#define SetMacro(type,varname) SET_MACRO(type,varname)

/**
 * \brief create set_varname(type __value) methods/functions
 * inside a class, where \c type is the type of \c varname and \c varname is a
 * previously declared private variable that should be accessed by these
 * methodes. In addition this will set the property paramsChanged to true, which
 * you will have to declare in your class.
*/
#define SET_MACRO_P(type,varname) \
    typedef type typedef_set_##varname ;\
    /** \brief sets the property varname to the specified \a __value. \details Description of the parameter varname is:  <CENTER>\copybrief varname.</CENTER> \see varname for more information */ \
  inline virtual void set_##varname (const typedef_set_##varname & __value)  \
  { \
    if (this->varname != __value) { \
        this->varname = __value; \
        this->paramsChanged=true; \
    } \
  };
#define SetMacroP(type,varname) SET_MACRO_P(type,varname)

/**
 * \brief like SetMacro(), but adds the instruction \a inst to the set method. This may be used
 *        to update a component after a value has been set
 */
#define SET_MACRO_I(type,varname,inst) \
    typedef type typedef_set_##varname ;\
  /** \brief sets the property varname to the specified \a __value. \details Description of the parameter varname is:  <CENTER>\copybrief varname.</CENTER> \see varname for more information */ \
  inline virtual void set_##varname(const typedef_set_##varname & __value)  \
  { \
    if (this->varname != __value) { \
        this->varname = __value; \
        inst; \
    } \
  };
#define SetMacroI(type,varname,inst) SET_MACRO_I(type,varname,inst)

/**
 * \brief like SetMacro(), but adds the instruction \a inst at the start of the set method. This may be used
 *        e.g. to lock a mutex in the set operation, e.g. using QMutexLocker locker(mutex); from Qt
 */
#define SET_MACRO_I_BEFORE(type,varname,inst) \
    typedef type typedef_set_##varname ;\
  /** \brief sets the property varname to the specified \a __value. \details Description of the parameter varname is:  <CENTER>\copybrief varname.</CENTER> \see varname for more information */ \
  inline virtual void set_##varname(const typedef_set_##varname & __value)  \
  { \
    inst; \
    if (this->varname != __value) { \
        this->varname = __value; \
    } \
  };
#define SetMacroIBefore(type,varname,inst) SET_MACRO_I_BEFORE(type,varname,inst)
/**
 * \brief like SetMacroP(), but adds the instruction \a inst to the set method. This may be used
 *        to update a component after a value has been set
 */
#define SET_MACRO_IP(type,varname,inst) \
    typedef type typedef_set_##varname ;\
  /** \brief sets the property varname to the specified \a __value. \details Description of the parameter varname is:  <CENTER>\copybrief varname.</CENTER> \see varname for more information */ \
  inline virtual void set_##varname (const typedef_set_##varname & __value)  \
  { \
    if (this->varname != __value) { \
        this->varname = __value; \
        this->paramsChanged=true; \
        inst; \
    } \
  };
#define SetMacroIP(type,varname,inst) SET_MACRO_IP(type,varname,inst)
/*@}*/

/** \defgroup tools_files filesystem and file I/O
 *  \ingroup tools
 */

/** \defgroup tools_streams C++ stream tools
 *  \ingroup tools_files
 */
/*@{*/


/*! \brief this class allows to double output from one stream to another stream ... the redirect is removed when the class is destroyed */
class ScopedStreamDoubler {
public:
    ScopedStreamDoubler(std::ostream & inOriginal, std::ostream & inRedirect) :
        mOriginal(inOriginal),
        mOldBuffer(inOriginal.rdbuf(inRedirect.rdbuf()))
    { }

    ~ScopedStreamDoubler()
    {
        mOriginal.rdbuf(mOldBuffer);
    }

private:
    ScopedStreamDoubler(const ScopedStreamDoubler&);
    ScopedStreamDoubler& operator=(const ScopedStreamDoubler&);

    std::ostream & mOriginal;
    std::streambuf * mOldBuffer;
};



/*! \brief this class allows to double output from one stream to another stream ... the redirect is removed when the class is destroyed */
class TeeStream
{
public:
  TeeStream(std::ofstream* stream1=NULL, std::ofstream* stream2=NULL) {
      this->stream1=stream1;
      this->stream2=stream2;
  }; // check if opening file succeeded!!
  // for regular output of variables and stuff
  template<typename T> TeeStream& operator<<(const T& something)
  {
    if (stream1) (*stream1) << something;
    if (stream2) (*stream2) << something;
    return *this;
  }
  // for manipulators like std::endl
  typedef std::ostream& (*stream_function)(std::ostream&);
  TeeStream& operator<<(stream_function func)
  {
    if (stream1) func(*stream1);
    if (stream2) func(*stream2);
    return *this;
  }

  void setStream1(std::ofstream* stream) {
    this->stream1=stream;
  }
  void setStream2(std::ofstream* stream) {
    this->stream2=stream;
  }
  void setStreams(std::ofstream* stream1, std::ofstream* stream2) {
      this->stream1=stream1;
      this->stream2=stream2;
  }
private:
  std::ofstream* stream1;
  std::ofstream* stream2;
};

/*! \brief this class allows to double output from one stream to another stream ... the redirect is removed when the class is destroyed */
class ScopedDelayedStreamDoubler {
public:
    ScopedDelayedStreamDoubler()
    { redirected=false; mOriginal=NULL; mOldBuffer=NULL;}

    void redirect(std::ostream & inOriginal, std::ostream & inRedirect) {
        if (redirected && mOriginal) mOriginal->rdbuf(mOldBuffer);
        redirected=true;
        mOriginal=&inOriginal;
        mOldBuffer=inOriginal.rdbuf(inRedirect.rdbuf());
    }

    ~ScopedDelayedStreamDoubler()
    {
        if (redirected) mOriginal->rdbuf(mOldBuffer);
    }

private:
    ScopedDelayedStreamDoubler(const ScopedDelayedStreamDoubler&);
    ScopedDelayedStreamDoubler& operator=(const ScopedDelayedStreamDoubler&);

    bool redirected;
   std::ostream* mOriginal;
   std::streambuf * mOldBuffer;
};


/*@}*/

/** \defgroup tools_filesystem filesystem functions
 *  \ingroup tools_files
 */
/*@{*/

/** \brief test whether the given file exists */
QFLIB_EXPORT bool file_exists (std::string fileName);

/** \brief test whether the given file exists */
QFLIB_EXPORT void copy_file(std::string in, std::string out);

/** \brief make all dirs specified
 *
 * The standard mkdir() command will only create the first-level directory, i.e. <code>mkdir("./dir1/dir2/dir3")</code> will create
 * dir1, but not dir 2 and dir3. THis function will create the complete directory tree. I.e. it ensures the existence of all specified
 * driectories.
 */
QFLIB_EXPORT void mk_all_dir(std::string directories);

/*! \brief list all files matching a given wildcard

    This function uses match_wildcard() to find all files that match the given wildcard, based on the current working directory. First
    this function splits the wildcard up into parts separated by \c / (on UNIX systems) or \c \\ (on Windows systems). Then it searches the
    current directory and all subdirectories and returns all files that match the given wildcard. The part of the wildcard after the last
    separator is taken as the file wildcard. The directories before the file specifier may NOT contain wildcards.

    It returns a vector containing the filenames relative to the current directory.
 */
QFLIB_EXPORT std::vector<std::string> listfiles_wildcard(std::string pattern);
/*@}*/



/** \defgroup tools_fileio file i/o
 *  \ingroup tools_files
 */
/*@{*/

/*! \brief Reads in one line of text from the specified file and interpret it as Comma-Separated-Values file.
           Returns its contents as a vector of doubles.

    This function reads one line of comma-separated values (of type double) from the given file
    (ignoring empty lines and complete-line comments). Comments start with a given character (by default \c '#' ) and last until
    the end of the current line. The columns in the file are separated by a character which is by default \c ',' but may
    be set to any other character. Additional whitespacees are ignored. If the function reaches the end of the current file
    the resulting vector is simply empty.

    \param f the file handle to read from
    \param separator_char the character that separates two subsequent columns
    \param comment_char the character that starts a comment. Comments last until the end of the current line

    Here is some example code:
    \code
    FILE* f=fopen("test.csv", "r");
    std::vector<double> r=csv_readline(f);
    while (r.size()>0) {
        for (size_t i=0; i<r.size(); i++) {
            if (i>0) cout<<",\\t";
            cout<<r[i];
        }
        cout<<"\\n";
        r=csv_readline(f);
    }
    fclose(f);
    \endcode
 */
QFLIB_EXPORT std::vector<double> csv_readline(FILE* f, char separator_char=',', char comment_char='#');

/** \brief returns the size of the specified file */
QFLIB_EXPORT long get_filesize(char *FileName);

/** \brief counts the lines in a file
 *
 * If comment_char is given, the routine checks whether the first non-whitespace character is
 * a comment_char. In this case the line contains a comment only and is not counted for the linecount.
 * If comment_char is kept 0 the function counts all lines in the file. Use this to count the actual
 * data lines in a CSV file!
 */
QFLIB_EXPORT unsigned long long count_lines(std::string filename, char comment_char=0);

/*@}*/



/**
 * \defgroup tools_string string handling routines
 * \ingroup tools
 */




/**
 * \defgroup tools_string_convert string conversion routines
 * \ingroup tools_string
 */
/*@{*/

/** \brief convert a long integer to a C++ string */
QFLIB_EXPORT std::string inttostr(long data);
/** \brief convert a long integer to a C++ string containing the number in hex notation */
QFLIB_EXPORT std::string inttohex(long data);
/** \brief convert a unsigned long integer to a C++ string */
QFLIB_EXPORT std::string uinttostr(unsigned long data);
/** \brief convert a long integer to a C++ string by using a printf-style format string (see <A HREF="http://www.cplusplus.com/reference/clibrary/cstdio/printf.html">printf format quick reference</A>)  */
QFLIB_EXPORT std::string inttostr_fmt(long data, std::string format="%ld");
/** \brief convert an unsigned long integer to a C++ string by using a printf-style format string (see <A HREF="http://www.cplusplus.com/reference/clibrary/cstdio/printf.html">printf format quick reference</A>)  */
QFLIB_EXPORT std::string uinttostr_fmt(unsigned long data, std::string format="%ld");
/** \brief convert a double float to a C++ string. in \a past_comma you can give the number of digits
 *         to show after the decimal divider. If \a remove_trail0 is \c true the method removes all trailing '0'.
 *
 * If \a belowIsZero>0 and \c abs(data) is smaller than \a belowIsZero, then \c "0" is returned.
 */
QFLIB_EXPORT std::string floattostr(double data, int past_comma=-1, bool remove_trail0=false, double belowIsZero=1e-16);
/*! \brief convert a given string to a double floating point value.

    \param data input string
    \return the number read from the given string
    \throws std::runtime_error if a conversion error occurs

    Basically this function is a wrapper around strtod() from libc.
 */
QFLIB_EXPORT double strtofloat(std::string data);
/** \brief convert a double float to a C++ string by using a printf-style format string (see <A HREF="http://www.cplusplus.com/reference/clibrary/cstdio/printf.html">printf format quick reference</A>) */
QFLIB_EXPORT std::string floattostr_fmt(double data, std::string format="%lf");
/** \brief prints the supplied number of bytes. If bytes>1024 it prints (bytes/1024)+" kBytes" ... */
QFLIB_EXPORT std::string bytestostr(double bytes);
/** \brief convert a double float to a C++ string in [number][unit_id] format.
 *         So \f$ 3.2\cdot 10^{-3} \f$ will be returned as \c "3.2m". The parameter \a past_comma gives the number of digits
 *          after the comma. If \a remove_trail0 is \c true the method removes all trailing '0'.
 */
QFLIB_EXPORT std::string floattounitstr(double data, int past_comma=5, bool remove_trail0=false);
/*! \brief convert a double float to a C++ string in [number]*10^xx format. the output is a LaTeX string

  For example the output of \c floattohtmlstr(10.5e8) is \verbatim 10.5\times10^{8} \endverbatim
  which corresponds to \f$ 10.5\times10^{8} \f$ after compilation in LaTeX.

  If \a belowIsZero>0 and \c abs(data) is smaller than \a belowIsZero, then \c "0" is returned.

  If \c minNoExponent<abs(data)<maxNoExponent the no exponent character is displayed
 */
QFLIB_EXPORT std::string floattolatexstr(double data, int past_comma=5, bool remove_trail0=false, double belowIsZero=1e-16, double minNoExponent=1e-3, double maxNoExponent=1e4);
/*! \brief convert a double float to a C++ string in [number]*10^xx format. the output is a HTML string,

  For example the output of \c floattohtmlstr(10.5e8) is \verbatim 10.5&times;10<sup>8</sup> \endverbatim

  If \a belowIsZero>0 and \c abs(data) is smaller than \a belowIsZero, then \c "0" is returned.

  If \c minNoExponent<abs(data)<maxNoExponent the no exponent character is displayed
 */
QFLIB_EXPORT std::string floattohtmlstr(double data, int past_comma=5, bool remove_trail0=false, double belowIsZero=1e-16, double minNoExponent=1e-3, double maxNoExponent=1e4);
/** \brief convert a char to a C++ string. Here the corresponding ASCII character is beeing returned. */
QFLIB_EXPORT std::string chartostr(char data);
/*! \brief convert a char to a C++ string. Here the corresponding ASCII character is beeing returned. For un-printable characters a character description is returned.

    This function recognises some unprintable characters and returns them as follows:
\verbatim
 ASCII value | returned string
-------------+-----------------
           0 |     [NUL]
           1 |     [SOH]
           2 |     [STX]
           3 |     [ETX]
           4 |     [EOT]
           5 |     [ENQ]
           6 |     [ACK]
           7 |     [BEL]
           8 |     [BS]
           9 |     [TAB]
          10 |     [LF]
          11 |     [VT]
          12 |     [FF]
          13 |     [CR]
          14 |     [SO]
          15 |     [SI]
          16 |     [DLE]
          17 |     [DC1]
          18 |     [DC2]
          19 |     [DC3]
          20 |     [DC4]
          21 |     [NAK]
          22 |     [SYN]
          23 |     [ETB]
          24 |     [CAN]
          25 |     [EM]
          26 |     [SUB]
          27 |     [ESC]
          28 |     [FS]
          29 |     [GS]
          30 |     [RS]
          31 |     [US]
          32 |     [SP]
         127 |     [DEL]
\endverbatim
 */
QFLIB_EXPORT std::string chartoprintablestr(char data);
/** \brief convert a string. For un-printable characters a character description is returned. For details, see documentation of chartoprintablestr() . */
QFLIB_EXPORT std::string toprintablestr(std::string data);
/** \brief convert a boolean to a C++ string. Return \c "true" or \c "false" */
QFLIB_EXPORT std::string booltostr(bool data);
/*! \brief convert a C++ string to a boolean.

    This function accepts \c "true", \c "t", \c "1", \c "j", \c "y", \c "yes", \c "ja" as \c true and returns \c false
    if none of these was found.
 */
QFLIB_EXPORT bool strtobool(std::string data);
/** \brief extract an integer from the given string that is right-aligned in it. So from "text123" this method would return 123. The left part of the string will be ignored
  */
QFLIB_EXPORT long extract_right_int(std::string text);
/** \brief converts the given time into a string
 *
 * for more information see <a href="http://www.gnu.org/software/libc/manual/html_node/Formatting-Calendar-Time.html">
 * documentation of strftime from libc</a>.
 *
 * To print the current date and time, simply use \code    std::cout<<datetimetostr(time(0))<<std::endl; \endcode
 */
QFLIB_EXPORT std::string datetimetostr(time_t t);
/** \brief converts a string \a s to a \c double or <tt>long long</tt> number in standard floating point notation.
 *         Additionally the number may include an unit identifier which specifies an additional power of ten.
 *
 * This method first ignores all leading whitespace characters. Then it reads in as long a number is recognized, i.e. until
 * the string ends or an unknown character appears. If it recognizes an integer it returns 2 and the reesult in \a iv.
 * If it recognizes a double then it returns 1 and the result in \a dv. \a npos contains the position of the first
 * non-number character. Additionally to the standard scientific floating-point notation this method may read a subsequent
 * unit character which adds an additional power of ten:
 *   - \c a = \f$ 10^{-15} \f$
 *   - \c f = \f$ 10^{-12} \f$
 *   - \c n = \f$ 10^{-9} \f$
 *   - \c u = \f$ 10^{-6} \f$
 *   - \c m = \f$ 10^{-3} \f$
 *   - \c c = \f$ 10^{-2} \f$
 *   - \c d = \f$ 10^{-1} \f$
 *   - \c k = \f$ 10^{3} \f$
 *   - \c M = \f$ 10^{6} \f$
 *   - \c G = \f$ 10^{9} \f$
 *   - \c T = \f$ 10^{12} \f$
 *   - \c P = \f$ 10^{15} \f$
 * .
 *
 * An example of such a number is \c 1.23k = \f$ 1.23\cdot10^3 \f$ or \c 1.45e-4T = \f$ 1.45\cdot 10^{8} \f$
 */
QFLIB_EXPORT int read_number_ex(std::string s, double* dv, long long* iv, int *newpos);
/** \brief converts a string \a data to a \c double number in standard floating point notation.
 *         Additionally the number may include an unit identifier which specifies an additional power of ten. This
 *         is just a convenience wrapper around read_number_ex(). For more information, see there.
 */
QFLIB_EXPORT double unitstringtofloat(std::string data);

/** \brief converts a RGBA color to its SVG color name (string), or a string like \#RRGGBB[AA]
 *
 * see <a href="http://www.w3.org/TR/SVG/types.html#ColorKeywords">http://www.w3.org/TR/SVG/types.html#ColorKeywords</a> for
 * more information about SVG color names. This method is (basically) compatible with Qt, as Qt uses SVG color naming
 * conventions.
 */
QFLIB_EXPORT std::string rgbtostring(unsigned char r, unsigned char g, unsigned char b, unsigned char a=255);

/** \brief split the string into a list of tokens which are separated by one of the characters in \c separators */
QFLIB_EXPORT std::vector<std::string> tokenize_string(std::string data, std::string separators);

/** \brief create a valid variable name from the string, i.e. a string with only characters and digits and \c '_'. ALso the first character has to be a charcter. */
QFLIB_EXPORT std::string to_valid_variable_name(std::string input);


/** \brief create string containing the \a n repeats of the given \a substr */
QFLIB_EXPORT std::string repeated_string(std::string input, int n);

/*@}*/


/**
 * \defgroup tools_string_test string test routines
 * \ingroup tools_string
 */
/*@{*/

/** \brief returns true if the given string is an integer */
QFLIB_EXPORT bool isInt(std::string s);

/** \brief returns true if the given string \a s contains a right aligned string
 * (\c "abs123" will return true and \c "1243a" will return false)
 */
QFLIB_EXPORT bool isRightInt(std::string s);

/** \brief returns true if the given string is a floating point number */
QFLIB_EXPORT bool isFloat(std::string s);

/** \brief returns \c true if there are only whitespace (ACSII<=32) characters in \a text */
QFLIB_EXPORT bool onlySpace(std::string text);
/*@}*/





/**
 * \defgroup tools_string_format formated output
 * \ingroup tools_string
 */
/*@{*/

/** \brief like sprintf(), but returns a std::string (see <A HREF="http://www.cplusplus.com/reference/clibrary/cstdio/printf.html">printf format quick reference</A>)  */
QFLIB_EXPORT std::string format(std::string templ, ...);

/** \brief convert a 2D array of doubles to a string (as a table of values).
  * \param width number of columns
  * \param height number of lines
  * \param linenumber if this is true the the first column contains the current line number (0 based).
  * \param data is a 1D array of double that represents a 2D array. The data order is row by row. So it is organized like this (3 columns):
  *  \verbatim
    array index       0        1        2        3        4        5        6        7
    array contents:  +--------+--------+--------+--------+--------+--------+--------+-----
                     | (1,1)  | (1,2)  | (1,3)  | (2,1)  | (2,2)  | (2,3)  | (3,1)  | ...
                     +--------+--------+--------+--------+--------+--------+--------+-----
     \endverbatim
  *
  * The output of this function looks like this (3 columns, \c linenumber=false):
  * \verbatim
+---------------+---------------+---------------+
|              0|              0|              0|
|              0|           0.25|              0|
|              0|            0.5|              0|
|              0|           0.75|              0|
|              0|             -5|              0|
+---------------+---------------+---------------+
    \endverbatim
  */
QFLIB_EXPORT std::string doublearraytostr(double* data, long width, long height, bool linenumber=true);

/** \brief convert a 1D array of doubles into vector notation
  * \param data an array of doubles
  * \param N number of items in \a data
  *
  * The output looks like this:
  * \verbatim   ( 1.00, 4.5, 3.4 )\endverbatim
  */
QFLIB_EXPORT std::string doublevectortostr(double* data, unsigned int N);

/** \brief convert a 1D array of doubles into vector notation
  * \param data an array of doubles
  * \param N number of items in \a data
  *
  * The output looks like this:
  * \verbatim   ( 1.00, 4.5, 3.4 )\endverbatim
  */
QFLIB_EXPORT std::string intvectortostr(int* data, unsigned int N);

/** \brief convert a vector (1D array) of unit8_t to a string (as a table of values).
  * \param height number of lines
  * \param showdec if this is true the the data is shown as decimal value.
  * \param showhex if this is true the the data is shown as hexadecimal value.
  * \param showbits if this is true the the data is shown as bit vector.
  * \param linenumber if this is true the the first column contains the current line number (0 based).
  * \param data is a 1D array of uint_8t
  *
  * The output of this function looks like this (\c linenumber=false, \c showdec=false, \c showhex=true, \c showbits=true ):
  * \verbatim
+-----+----------|
|    1| 00000001 |
|    1| 00000001 |
|   53| 01010011 |
|   53| 01010011 |
|   5b| 01011011 |
|   5b| 01011011 |
|   5b| 01011011 |
|   5a| 01011010 |
|   5a| 01011010 |
|   5a| 01011010 |
|   5a| 01011010 |
|   52| 01010010 |
|    0| 00000000 |
+-----+----------+
    \endverbatim
  */
QFLIB_EXPORT std::string uint8vectostr(uint8_t* data, unsigned long height, bool showdec, bool showhex, bool showbits, bool linenumber);

/** \brief convert a vector (1D array) of unit16_t to a string (as a table of values).
  * \param height number of lines
  * \param showdec if this is true the the data is shown as decimal value.
  * \param showhex if this is true the the data is shown as hexadecimal value.
  * \param showbits if this is true the the data is shown as bit vector.
  * \param linenumber if this is true the the first column contains the current line number (0 based).
  * \param data is a 1D array of uint_16t
  *
  * The output of this function looks like this (\c linenumber=false, \c showdec=true, \c showhex=true, \c showbits=true ):
  * \verbatim
+-------+------+------------------|
|      1|     1| 0000000000000001 |
|      1|     1| 0000000000000001 |
|    595|   253| 0000001001010011 |
|    595|   253| 0000001001010011 |
|    859|   35b| 0000001101011011 |
|    859|   35b| 0000001101011011 |
|    859|   35b| 0000001101011011 |
|    858|   35a| 0000001101011010 |
|    858|   35a| 0000001101011010 |
|    858|   35a| 0000001101011010 |
|    858|   35a| 0000001101011010 |
|    594|   252| 0000001001010010 |
|      0|     0| 0000000000000000 |
+-------+------+------------------+
    \endverbatim
  */
QFLIB_EXPORT std::string uint16vectostr(uint16_t* data, unsigned long height, bool showdec, bool showhex, bool showbits, bool linenumber);

/** \brief convert a vector (1D array) of unit16_t to a string (as a table of values).
  * \param height number of lines
  * \param showdec if this is true the the data is shown as decimal value.
  * \param showhex if this is true the the data is shown as hexadecimal value.
  * \param showbits if this is true the the data is shown as bit vector.
  * \param linenumber if this is true the the first column contains the current line number (0 based).
  * \param data is a 1D array of uint_16t
  *
  * The output of this function looks like this (\c linenumber=false, \c showdec=true, \c showhex=true, \c showbits=true ):
  * \verbatim
+-------+------+------------------|
|      1|     1| 0000000000000001 |
|      1|     1| 0000000000000001 |
|    595|   253| 0000001001010011 |
|    595|   253| 0000001001010011 |
|    859|   35b| 0000001101011011 |
|    859|   35b| 0000001101011011 |
|    859|   35b| 0000001101011011 |
|    858|   35a| 0000001101011010 |
|    858|   35a| 0000001101011010 |
|    858|   35a| 0000001101011010 |
|    858|   35a| 0000001101011010 |
|    594|   252| 0000001001010010 |
|      0|     0| 0000000000000000 |
+-------+------+------------------+
    \endverbatim
  */
QFLIB_EXPORT std::string intvectostr(int* data, unsigned long height, bool showdec=true, bool showhex=false, bool showbits=false, bool linenumber=false);

/** \brief convert a vector (1D array) of unit32_t to a string (as a table of values).
  * \param height number of lines
  * \param showdec if this is true the the data is shown as decimal value.
  * \param showhex if this is true the the data is shown as hexadecimal value.
  * \param showbits if this is true the the data is shown as bit vector.
  * \param linenumber if this is true the the first column contains the current line number (0 based).
  * \param data is a 1D array of uint_32t
  *
  * The output of this function looks like this (\c linenumber=false, \c showdec=true, \c showhex=true, \c showbits=true ):
  * \verbatim
+-------+------+----------------------------------|
|      1|     1| 00000000000000000000000000000001 |
|      1|     1| 00000000000000000000000000000001 |
|    595|   253| 00000000000000000000001001010011 |
|    595|   253| 00000000000000000000001001010011 |
|    859|   35b| 00000000000000000000001101011011 |
|    859|   35b| 00000000000000000000001101011011 |
|    859|   35b| 00000000000000000000001101011011 |
|    858|   35a| 00000000000000000000001101011010 |
|    858|   35a| 00000000000000000000001101011010 |
|    858|   35a| 00000000000000000000001101011010 |
|    858|   35a| 00000000000000000000001101011010 |
|    594|   252| 00000000000000000000001001010010 |
|      0|     0| 00000000000000000000000000000000 |
+-------+------+----------------------------------+
\endverbatim
  */
QFLIB_EXPORT std::string uint32vectostr(uint32_t* data, unsigned long height, bool showdec, bool showhex, bool showbits, bool linenumber);
/*@}*/

/**
 * \defgroup tools_string_misc miscelanious routines
 * \ingroup tools_string
 */
/*@{*/
/** \brief convert a string to a vector of strings. Each entry of the vector represents one line
  *        (separated by separator, standard is \c '\\n'). If \a strip is \c true then leading and
  *        trailing whitespace will be stripped from the items in lst.
  */
QFLIB_EXPORT void toStringVector(std::vector<std::string>& lst, std::string text, char separator='\n', bool strip=false);

/** \brief convert vector of strings into a string. Each entry of the vector represents one line
  * (separated by separator, standard is \c '\\n')
  */
QFLIB_EXPORT std::string fromStringVector(std::vector<std::string>& lst, std::string separator="\n");

/** \brief convert a string to all lower case letters */
QFLIB_EXPORT std::string tolower(std::string s);

/** \brief convert a string to all upper case letters */
QFLIB_EXPORT std::string toupper(std::string s);


/** \brief convert some special characters to their respective UTF-8 encoding (\c ¥`∞ß≤≥µÄƒ‰‹‰‰¸ﬂ·ÈÌÛ˙¡…Õ”⁄‡ËÏÚ˘¿»Ã“Ÿ‚ÍÓÙ˚¬ Œ‘€), see <a href="http://www.utf8-zeichentabelle.de/unicode-utf8-table.pl?number=1024&htmlent=1">UTF-8 database</a> and <a href="http://de.wikipedia.org/wiki/UTF-8">UTF-8 on wikipedia.de</a>
  */
QFLIB_EXPORT std::string toUTF8(std::string text);
/** \brief strips whitespaces at the beginning and end of the string
  */
QFLIB_EXPORT std::string strstrip(std::string text);

/** \brief convert a double to a string with a unit character, like 1e-4 -> "100u"
  */
QFLIB_EXPORT std::string floattounitstr(double data, std::string unitname="");

/*! \brief remove delimiters from string and return the part between the delimiters

  This function ignores delimiters that are precedet by a backslash, so it is possible to write <code>"data with\" in it"</code>
  and the function will return <code>data with\" in it</code>.

  The function will find out which delimiter is used by simply scanning for the first non-whitespace character.

  Here are some test cases:
\code
    std::string s;
    s="   'blabla\\''"; cout<<"<"<<s<<">   <"<<removeDelimiters(s)<<">\n";
    s="\"blabla\\\"'   "; cout<<"<"<<s<<">   <"<<removeDelimiters(s)<<">\n";
    s="\"blabla\\\"'   \""; cout<<"<"<<s<<">   <"<<removeDelimiters(s)<<">\n";
    s=""; cout<<"<"<<s<<">   <"<<removeDelimiters(s)<<">\n";
    s="\"\""; cout<<"<"<<s<<">   <"<<removeDelimiters(s)<<">\n";
    s="\"a\""; cout<<"<"<<s<<">   <"<<removeDelimiters(s)<<">\n";
    s="\"\\\"\""; cout<<"<"<<s<<">   <"<<removeDelimiters(s)<<">\n";
\endcode
 And the output of it:
\verbatim
    <   'blabla\''>   <blabla\'>
    <"blabla\"'   >   <blabla\"'   >
    <"blabla\"'   ">   <blabla\"'   >
    <>   <>
    <"">   <>
    <"a">   <a>
    <"\"">   <\">
\endverbatim
 */
QFLIB_EXPORT std::string removeDelimiters(std::string data);


/*! \brief remove delimiters from string and return the part between the delimiters

  This function ignores delimiters that appear as doubled character, so it is possible to write <code>"data with"" in it"</code>
  and the function will return <code>data with" in it</code>. This is the way string constants are written in Pascal.

  The function will find out which delimiter is used by simply scanning for the first non-whitespace character.
*/
QFLIB_EXPORT std::string removeDelimitersPascal(std::string data);
/*@}*/



/**
 * \defgroup tools_string_escapes excape sequence handling
 * \ingroup tools_string
 */
/*@{*/

/** \brief generate a string where all special characters are replaced by escape sequences in the C style
  *
  * This method basically does these replacements (see <a href="http://h21007.www2.hp.com/portal/download/files/unprot/Fortran/docs/lrm/lrm0035.htm">here</a>:
  * \verbatim
linebreak     -> \n
return        -> \r
tab           -> \t
\             -> \\
0x00          -> \0
"             -> \"
'             -> \'
ESC           -> \e
FF            -> \f
VT            -> \v
BS            -> \b
BEL           -> \a
chars<32      -> \xHH
    \endverbatim
  *
  * For a list of control characters and their excape sequences (if existent), see <a href="http://de.wikipedia.org/wiki/Steuerzeichen">Wikipedia.de</a>.
  * The Pair  escapify() and deescapify() can be used to produce strings for transfer with TCP/IP
  * where there are some special characters (like '\\0') used as field dividers.
  */
QFLIB_EXPORT std::string escapify(std::string text);

/** \brief generate a string where a special character is replaced by escape sequences in the C style. */
QFLIB_EXPORT std::string escapify(const char c);

/** \brief decode a string with C style escape sequences. This method does the reverse replacement of escapify().
  *
  * This method supports these escape sequences:
  * \verbatim
\n \r \t \\ \0 \" \' \a \b \v \f \e \xHH
    \endverbatim
  *
  * The Pair  escapify() and deescapify() can be used to produce strings for transfer with TCP/IP
  * where there are some special characters (like '\\0') used as field dividers.
  *
  * This method can easily be implemented in MatLab (the file is in \c ./matlab/deescapify.m ):
  * \verbatim
function ret = deescapify(text)
% replace C style escape sequences in the given string
res='';
if ischar(text)
    len=length(text)+1;
    i=1;
    while i<len
        if text(i)~='\'
          res=[res text(i)];
          i=i+1;
        else
          if i+1<len
              switch text(i+1)
                  case '0'
                      res=[res 0];
                      i=i+2;
                  case 'n'
                      res=[res 10];
                      i=i+2;
                  case 'r'
                      res=[res 13];
                      i=i+2;
                  case 't'
                      res=[res 11];
                      i=i+2;
                  case '\\'
                      res=[res '\\'];
                      i=i+2;
                  case '"'
                      res=[res '"'];
                      i=i+2;
                  case ''''
                      res=[res ''''];
                      i=i+2;
                  case { 'x', 'X' }
                      if i+3<len
                          number=text(i+2:i+3);
                          res=[res hex2dec(number)];
                          i=i+3;
                      else
                          i=i+2;
                      end
                  otherwise
                      i=i+1;
              end

          else
            i=i+1;
          end
        end
    end
end
ret=res;
    \endverbatim
  */
QFLIB_EXPORT std::string deescapify(std::string text);


/** \brief converts a string into a Pascal-style string constant, i.e. with tripple delimiters for single delimiters ... so if you want to provide
 *         <code>Jan's example</code> you will get <code>'Jan'''s example'</code> */

QFLIB_EXPORT std::string pascalifyString(std::string data, char delim='\'');
/*@}*/





/**
 * \defgroup tools_string_filenames filename and path handling
 * \ingroup tools_string
 */
/*@{*/

/** \brief extracts the path from the given \a filename */
QFLIB_EXPORT std::string extract_file_path(std::string filename);
/** \brief replace slashes and backslashes by the system path separator in \a filename */
QFLIB_EXPORT std::string replace_to_system_pathseparator(std::string filename);

/** \brief extracts the name from the given \a filename */
QFLIB_EXPORT std::string extract_file_name(std::string filename);

/** \brief extracts the file extension from the given \a filename */
QFLIB_EXPORT std::string extract_file_ext(std::string filename);

/** \brief replaces the extension of the given \a filename with \a ext. \a ext may be given in
 *         the form \c ".ext" or \c "ext".
 */
QFLIB_EXPORT std::string change_file_ext(std::string filename, std::string ext);

/** \brief add another part to the path */
QFLIB_EXPORT std::string extend_file_path(std::string filename, std::string addpath);

/** \brief takes care that there is a backslash at the end of the given \a filename */
QFLIB_EXPORT std::string include_trailing_backslash(std::string filename);

/** \brief takes care that there is no backslash at the end of the given \a filename */
QFLIB_EXPORT std::string exclude_trailing_backslash(std::string filename);

/** \brief this method returns the full filename of the file specified in \a filename
 *
 * \attention This method uses system specific code!
 */
QFLIB_EXPORT std::string get_full_filename(std::string filename);

/** \brief return the current working directory */
QFLIB_EXPORT std::string get_currentworkingdirectory();

/** \brief execute command in given directory
 *
 *  \return The return value is -1 if it wasn't possible to create the shell process, and otherwise is the status of the shell process.
 */
QFLIB_EXPORT int execute_in(std::string command, std::string directory);
/*@}*/






/**
 * \defgroup tools_string_compare string comparison (wildcard matching ...)
 * \ingroup tools_string
 */
/*@{*/

/*! \brief test whether the suplied string matches the given pattern, which may contain wildcards

  Two wildcards are allowed:
   - \c * will match any string
   - \c ? will match any (but only one) character
  .

  You can use this to do wildcard-matching when listing a directory:
  \code
    #include <stdio.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <dirent.h>
    #include <string>
    #include <tools.h>


    int main (void)
    {
      DIR *dp;
      struct dirent *ep;
      struct stat buf;

      dp = opendir ("./");
      std::string wildcard="*.exe";
      if (dp != NULL)
        {
          while (ep = readdir (dp)) {
              //printf("fn='%s'  wc='%s'\n", ep->d_name, wildcard.c_str());
              stat(ep->d_name, &buf);
              if (S_ISREG(buf.st_mode))  {
                  if (match_wildcard(ep->d_name, wildcard)) {
                      puts (ep->d_name);
                  }
              }
              //if (S_ISDIR(buf.st_mode))  { puts("DIR: "); puts (ep->d_name); }
          }
          closedir(dp);
        }
      else
        perror ("Couldn't open the directory");

      return 0;
    }
  \endcode

 */
QFLIB_EXPORT bool match_wildcard(std::string data, std::string pattern);


/*! \brief same as match_wildcard(), but also returns \c true if the wildcard does not exactly match, but is contained in
           the string; so <code>"12*"</code> will match <code>"Test12Test"</code>, which won't match for match_wildcard().
 */
QFLIB_EXPORT bool contains_wildcard(std::string data, std::string pattern);

/** \brief replace all occurences of the given substring \a match in the string by \a replace */
QFLIB_EXPORT std::string string_replace(std::string input, std::string match, std::string replace);
/*@}*/



/**
 * \defgroup tools_bit bit manipulation methods
 * \ingroup tools
 */
/*@{*/

/** \brief return a number that is in binary all 0, but on position \c bit. As an example \c _BV(3) will result in the number \c 100b */
#define _BV(bit) (1 << (bit))

/** \brief set bit \c bit in variable \c var
 *
 * This is implemented by calculatin
 * \code
 *   var = var or mask
 * \endcode
 * where \c mask is 0 everywhere except for the bit number \c bit. Then if this bit is
 * already set nothing happens and otherwise it will be set after the operation. All the other
 * bit are ored with 0, so they do not change.
 */
#define set_bit(var, bit) {(var) |= _BV(bit);}

/** \brief clear bit \c bit in variable \c var
 *
 * This is implemented by calculation
 * \code
 *   var = var and mask
 * \endcode
 * where \c mask is 1 everywhere except for the bit number \c bit. Then if this bit is
 * set (=1) it will be 0 after the operation, as <tt>x and 0</tt> is always 0. All the other
 * bit are anded with 1, so they do not change.
 */
#define clear_bit(var, bit)  {(var) &= ~(_BV(bit));}

/** \brief returns \c true if bit \c bit in variable \c var is set (=1)
 *
 * This is implemented by calculatin
 * \code
 *   (var and mask) != 0
 * \endcode
 * where \c mask is 0 everywhere except for the bit number \c bit. Then if this bit is
 * set the result will be non-zero which is then tested.
 */
#define bit_is_set(var, bit) ( ( (var) & _BV(bit) ) != 0 )

/*@}*/



/**
 * \defgroup tools_other other usefull routines
 * \ingroup tools
 */
/*@{*/

/** \brief return the bigger argument */
#define mmax(a,b) ((a>b)?a:b)

/** \brief return the smaller */
#define mmin(a,b) ((a<b)?a:b)

#define bound(a,x,b) (mmin((b), mmax((a), (x))))

/** \brief converts a number to a bool (==0 -> false, !=0 -> true) */
QFLIB_EXPORT bool toBool(double val);

/** \brief returns the current time in microseconds (might have lower resolution on non-windows and non-linux systems!!!) */
double getHighResolutionTime();

/*@}*/


#endif /* cpptools_h */
