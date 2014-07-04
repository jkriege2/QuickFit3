/*!
    \file quickfit-model-tools.h
    \ingroup quickfit3_models

    This file defines general tools and data types used by QuickFit2 model DLLs.

 */

#ifndef QUICKFITMODELTOOLS_H
#define QUICKFITMODELTOOLS_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * \defgroup quickfit3_models_widgettypes Widget Type Constants
 * \ingroup quickfit3_models
 */
/*@{*/
/** \brief a floating point number widget
 */
#define QF3SFF_WIDGET_FLOAT 0

/** \brief a logarithmic floating point number widget
 */
#define QF3SFF_WIDGET_LOGFLOAT 3

/** \brief an integer widget (spin box)
 */
#define QF3SFF_WIDGET_INT 1

/** \brief an integer widget (drop down list/combo box)
 */
#define QF3SFF_WIDGET_INT_COMBO 2

/** \brief a standard widget
 */
#define QF3SFF_WIDGET_DEFAULT QF3SFF_WIDGET_FLOAT

/*@}*/


/**
 * \defgroup quickfit3_models_errordisplaymodes Error Display Mode Constants
 * \ingroup quickfit3_models
 */
/*@{*/
/** \brief no error
 */
#define QF3SFF_ERROR_NONE 0

/** \brief display the error read-only
 */
#define QF3SFF_ERROR_DISPLAY 1

/** \brief editable error display
 */
#define QF3SFF_ERROR_EDIT 2


/*@}*/




#pragma pack(push,1)
/** \brief this struct contains the properties of one model parameter
 * \ingroup quickfit3_models
 */
typedef struct {
    /** \brief type of the parameter */
    int32_t type;
    /** \brief short unique id for the parameter, e.g. "n" */
    char* id;
    /** \brief description of the parameter, e.g. "particle number N" */
    char* name;
    /** \brief a label for the parameter, e.g. "<i>N</i>". You may use HTML markup for this */
    char* label;
    /** \brief unit of the parameter, e.g. "usec", "g"  */
    char* unit;
    /** \brief unit label of the parameter, e.g. "&mu;s", "g". You may use HTML markup for this  */
    char* unitLabel;
    /** \brief is this a fitting parameter, if \c false the user may supply a value, but the value will never be changed by the fitting
     *         algorithm. Use this e.g. to configure a model (number of components, ...) */
    int8_t fit;
    /** \brief determine whether, or not the user is allowed to change the value of a parameter. You could use a parameter with
     *         \c fit=userEditable=false to output values that were calculated from the fitting parameters after the fit. */
    int8_t userEditable;
    /** \brief determine whether, or not an error value is displayed for this parameter */
    int32_t displayError;
    /** \brief determine whether the user should be allowed to edit the parameter range */
    int8_t userRangeEditable;
    /** \brief an initial value for the parameter */
    double initialValue;
    /** \brief initial fix state of parameter */
    int8_t initialFix;
    /** \brief minimum value of the parameter range (if supported by algorithm) */
    double minValue;
    /** \brief maximum value of the parameter range (if supported by algorithm) */
    double maxValue;
    /** \brief value increment for the widget associated with the parameter */
    double inc;
    /** \brief absolute minimum value of the parameter range (if supported by algorithm), or \c -DBL_MAX if disabled */
    double absMinValue;
    /** \brief absolute maximum value of the parameter range (if supported by algorithm), or \c DBL_MAX if disabled */
    double absMaxValue;

} QF3SimpleFFParameter;
#pragma pack(pop)


/** \brief allocates a new memory block as long as the supplied string. Then it copies the string into the new memory
 * \ingroup quickfit3_models
 *
 * If you call this as:
 * \code char* a=qf2m_strcpy("Hello!"); \endcode
 * This will call malloc() to get memory for a copy of the string and then return a pointer to this copy. This is any
 * easy way to get malloc'ed string (of the right length) from string constants!
 */
inline char* qf3sff_strcpy(const char* st) {
    char* a=(char*)malloc(strlen(st)*sizeof(char)+2);
    strcpy(a, st);
    return a;
}




/** \brief type of the evaluate() function of a model (in the DLL)
  * \ingroup quickfit3_models
  */
typedef double (*QF3SimpleFFEvaluateFunc)(double, const double*);

/** \brief type of the multiEvaluate() function of a model (in the DLL)
  * \ingroup quickfit3_models
  */
typedef void (*QF3SimpleFFMultiEvaluateFunc)(double* , const double* , uint64_t, const double* );

/** \brief type of the getParameterCount() function of a model (in the DLL)
  * \ingroup quickfit3_models
  */
typedef uint16_t (*QF3SimpleFFGetParameterCountFunc)();


/** \brief type of the getParameterCount() function of a model (in the DLL)
  * \ingroup quickfit3_models
  */
typedef const QF3SimpleFFParameter* (*QF3SimpleFFGetParameterDescriptionFunc)();


/** \brief type of the getModelName() function of a model (in the DLL)
  * \ingroup quickfit3_models
  */
typedef const char* (*QF3SimpleFFGetNameFunc)(int16_t);



#endif //QUICKFITMODELTOOLS_H
