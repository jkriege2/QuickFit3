
/*!
    \file quickfit-model-header.h
    \ingroup quickfit3_models

 */

#ifndef QUICKFITMODELHEADER_H
#define QUICKFITMODELHEADER_H

//#include <windows.h>
#include "quickfit-model-tools.h"



#ifdef __cplusplus
#  define EXTERN_START extern "C" {
#  define EXTERN_END }
#else
#  define EXTERN_START
#  define EXTERN_END
#endif


#ifdef BUILD_DLL
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT __declspec(dllimport)
#endif



#define QF3SFF_FINALIZE_NOMULTI    \
    EXTERN_START\
    const char* DLL_EXPORT getModelName(int16_t type) {\
        switch(type) {\
            case 1:  \
                return QF3SFF_ID;\
            case 2:  \
                return QF3SFF_SHORTNAME; \
            case 3:  \
                return QF3SFF_HELP; \
            case 0:  \
                return QF3SFF_NAME;\
            default:\
                return NULL;\
        }\
        return NULL;\
    }\
    const QF3SimpleFFParameter* DLL_EXPORT getParameterProperties() {\
        return QF3SFF_PARAMETERS;\
    }\
    uint16_t DLL_EXPORT getParameterCount() {\
        return QF3SFF_PARAMETER_COUNT;\
    }\
    EXTERN_END

#define QF3SFF_FINALIZE\
    QF3SFF_FINALIZE_NOMULTI\
    EXTERN_START\
    void DLL_EXPORT multiEvaluate(double* c, const double* t, uint64_t NN, const double* parameters) {\
        for (register unsigned int i=0; i<NN; i++) {\
            c[i]=evaluate(t[i], parameters);\
        }\
    }\
    EXTERN_END




#endif // QUICKFITMODELHEADER_H


