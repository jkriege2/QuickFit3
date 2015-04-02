/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg
	
	last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/*!
    \file quickfit-model-header.h
    \ingroup quickfit3_models

 */

#ifndef QUICKFITMODELHEADER_H
#define QUICKFITMODELHEADER_H

#include <string.h>
#include "quickfit-model-tools.h"


#ifdef __cplusplus
#  define QF3SFF_EXTERN_START extern "C" {
#  define QF3SFF_EXTERN_END }
#else
#  define QF3SFF_EXTERN_START
#  define QF3SFF_EXTERN_END
#endif

#ifdef __WINDOWS__
#  ifdef QF3SFF_BUILD_DLL
#    define QF3SFF_DLL_EXPORT __declspec(dllexport)  QF3SFF_CALLCONVENTION
#  else
#    define QF3SFF_DLL_EXPORT __declspec(dllimport)  QF3SFF_CALLCONVENTION
#  endif
#else
#  define QF3SFF_DLL_EXPORT
#endif


#define QF3SFF_CATEGORY ""


inline static const char* QF3SFF_getStringDefault(const char* str, const char* strdef) {
    if (strlen(str)>0) return str;
    else return strdef;
}


#define QF3SFF_FINALIZE_NOMULTI    \
    QF3SFF_EXTERN_START\
     const char* QF3SFF_DLL_EXPORT getModelName(int16_t type) {\
        switch(type) {\
            case FFNAME_ID:  \
                return QF3SFF_ID;\
            case FFNAME_SHORTNAME:  \
                return QF3SFF_getStringDefault(QF3SFF_SHORTNAME, QF3SFF_NAME); \
            case FFNAME_HELPFILE:  \
                return QF3SFF_HELP;  \
            case FFNAME_LONGNAME:  \
                return QF3SFF_NAME;\
            case FFNAME_CATEGORY:  \
                return QF3SFF_CATEGORY_STR;\
            default:\
                return NULL;\
        }\
        return NULL;\
    }\
     const QF3SimpleFFParameter* QF3SFF_DLL_EXPORT getParameterProperties() {\
        return QF3SFF_PARAMETERS;\
    }\
     uint16_t QF3SFF_DLL_EXPORT getParameterCount() {\
        return QF3SFF_PARAMETER_COUNT;\
    }\
     void QF3SFF_DLL_EXPORT getVersion(uint16_t* major, uint16_t*minor) {\
        if (major) *major=QF3SFF_API_VERSION_MAJOR;\
        if (minor) *minor=QF3SFF_API_VERSION_MINOR;\
    }\
    QF3SFF_EXTERN_END

#define QF3SFF_FINALIZE\
    char QF3SFF_CATEGORY_STR[]=QF3SFF_CATEGORY;\
    QF3SFF_FINALIZE_NOMULTI\
    QF3SFF_EXTERN_START\
     void QF3SFF_DLL_EXPORT multiEvaluate(double* c, const double* t, uint64_t NN, const double* parameters) {\
        for (register unsigned int i=0; i<NN; i++) {\
            c[i]=evaluate(t[i], parameters);\
        }\
    }\
    QF3SFF_EXTERN_END




#endif // QUICKFITMODELHEADER_H


