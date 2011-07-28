#ifndef ANDORTOOLS_H
#define ANDORTOOLS_H


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

#ifndef __WINDOWS__
# ifndef __LINUX__
#  warning("these methods are ment to be used under windows or linux ... no other system were tested")
# endif
#endif


#ifdef __WINDOWS__
#  include "ATMCD32D.H"
#else
#  include "atmcdLXd.h"
#endif

#include <QString>

/** \brief convert an andor error code into a describing string */
QString andorErrorToString(unsigned int error);

/** \brief returns the number of available cameras */
unsigned int andorGetCameraCount();

/** \brief select the specified camera */
bool selectCamera (int iSelectedCamera);

#endif // ANDORTOOLS_H
