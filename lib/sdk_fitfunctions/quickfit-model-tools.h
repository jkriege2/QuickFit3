/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center
	
	last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

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

/*!
    \file quickfit-model-tools.h
    \ingroup quickfit3_models

    This file defines general tools and data types used to write simple fitting model
	DLLs/SOs for QuickFit 3.

 */

#ifndef QUICKFITMODELTOOLS_H
#define QUICKFITMODELTOOLS_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>


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


inline void QF3SFF_swap(double& a, double&b) {
  double t=a;
  a=b;
  b=t;
}

inline void QF3SFF_swap(int8_t& a, int8_t&b) {
  double t=a;
  a=b;
  b=t;
}

#define QF3SFF_TRUE 1
#define QF3SFF_FALSE 0


#define QF3SFF_API_VERSION_MAJOR 1
#define QF3SFF_API_VERSION_MINOR 0

#define QF3SFF_CALLCONVENTION __cdecl


#pragma pack(push,1)
/** \brief this struct contains the properties of one model parameter
 * \ingroup quickfit3_models
 */
typedef struct {
    /** \brief type of the parameter */
    int32_t type;
    /** \brief short unique id for the parameter, e.g. "n" */
    const char* id;
    /** \brief description of the parameter, e.g. "particle number N" */
    const char* name;
    /** \brief a label for the parameter, e.g. "<i>N</i>". You may use HTML markup for this */
    const char* label;
    /** \brief unit of the parameter, e.g. "usec", "g"  */
    const char* unit;
    /** \brief unit label of the parameter, e.g. "&mu;s", "g". You may use HTML markup for this  */
    const char* unitLabel;
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





/** \brief type of the evaluate() function of a model (in the DLL), which evaluates the function
  * \ingroup quickfit3_models
 *
 * \param t the time step where to evaluate the model
 * \param parameters an array with the model parameters to use during evaluation
  *
  */
typedef double (*QF3SimpleFFEvaluateFunc)(double, const double*);

/** \brief type of the multiEvaluate() function of a model (in the DLL), which evaluates the model functions for a vector of input values
  * \ingroup quickfit3_models
  *
 *
 * \param[out] c the results are written into this array
 * \param t the time step where to evaluate the model
 * \param NN number of entries in c and t
 * \param parameters an array with the model parameters to use during evaluation
  */
typedef void (*QF3SimpleFFMultiEvaluateFunc)(double* , const double* , uint64_t, const double* );

/** \brief type of the getParameterCount() function of a model (in the DLL), returns the number of  parameters
  * \ingroup quickfit3_models
  */
typedef uint16_t (*QF3SimpleFFGetParameterCountFunc)();


/** \brief type of the getParameterCount() function of a model (in the DLL), which returns an array of type QF3SimpleFFParameter[], which describes all  parameters
  * \ingroup quickfit3_models
  *
  *  The number of entries in the  returned array is given by getParameterCount()
  */
typedef const QF3SimpleFFParameter* (*QF3SimpleFFGetParameterDescriptionFunc)();


/** \brief type of the getVersion() function of a model (in the DLL), which returns the API version of the plugin
  * \ingroup quickfit3_models
  *
  *  The API version of the plugin is written into the call-by-reference parameter major and minor
  */
typedef void (*QF3SimpleFFGetVersionFunc)(uint16_t* major, uint16_t*minor);

/** \brief type of the getModelName() function of a model (in the DLL)
  * \ingroup quickfit3_models
  *
 *    \param buffer the buffer the result will be stored in. Quickfit will allocate at least 1 kByte for this
 *                  buffer, when the function is called.
 *    \param type   this tells the function what information to return (see below).
 *
 * These values are applicable to \c type:
 *   - <b>type=0</b> (default): getModelName() returns the long name of the model
 *   - <b>type=1</b>: getModelName() returns a short name (ID) of the model, without whitespaces, which may
 *                    be used as part of filenames, INI sections ...
 *   - <b>type=2</b>: getModelName() will return short name of the fit function
 *   - <b>type=3</b>: getModelName() will return the filename of the description HTML file reltive to the model's library location!
 * .
 *
  */
typedef const char* (*QF3SimpleFFGetNameFunc)(int16_t);


/** \brief type of the calulateParameters() function of a model (in the DLL), which can calculate the values (and errors) of non-fit parameters.
  * \ingroup quickfit3_models
  *
 *
 * \param[in|out] params the current parameter vector. Also the results are written here. All fit parameters have their final values and should not be changed! The size of this vector is at least as large as the number of parameters.
 * \param[in|out] errors the current parameter error vector. Also the results are written here. All fit parameters have their final values and should not be changed! The size of this vector is at least as large as the number of parameters. This parameter might be NULL, the errors should not be calculated (CHECK!!!)
  */
typedef void (*QF3SimpleFFCalculateParameters)(double* , double* );


/** \brief type of the isParameterVisible() function of a model (in the DLL), which reurns whether a given fit parameter is displayed to the user and used, based on a current vector of parameters
  * \ingroup quickfit3_models
  *
 *
 * \param p the parameter, for which the visibility is returned
 * \param params the current parameter vector. The size of this vector is at least as large as the number of parameters.
 * \return QF3SFF_TRUE if the parameter is visible/used and QF3SFF_FALSE otherwise
  */
typedef int8_t (*QF3SimpleFFIsParameterVisible)(int16_t, const double* );



/** \brief type of the sortParameters() function of a model (in the DLL), which can sort the values (and errors, fixes) of the fucntion parameters
  * \ingroup quickfit3_models
  *
  * In some cases it is desireable to return the fit parameters in a certain order (e.g. faster decay components first in a multi-exponential model)
  *
  * \param[in|out] params the current parameter vector. Also the results are written here. All fit parameters have their final values and should not be changed! The size of this vector is at least as large as the number of parameters.
  * \param[in|out] errors the current parameter error vector. Also the results are written here. All fit parameters have their final values and should not be changed! The size of this vector is at least as large as the number of parameters. This parameter might be NULL, the errors should not be calculated (CHECK!!!)
  * \param[in|out] fix the current parameter fix state vector. Also the results are written here. All fit parameters have their final values and should not be changed! The size of this vector is at least as large as the number of parameters. This parameter might be NULL, the errors should not be calculated (CHECK!!!)
  *
  * \note all given vector MUST be sorted equally!!!
  */
typedef void (*QF3SimpleFFSortParameters)(double* , double*, int8_t* );


/** \brief type of the evaluateDerivatives() function of a model (in the DLL), which can be implemented to evaluate the partial derivative of a fit function analytically. The derivatives are used in most fitting algorithms, so an analytical implementation can yield better results of a fit (due to its higher accuracy than a numerical estimate) and is often faster than a numerical estimate!)
  * \ingroup quickfit3_models
  *
  * In some cases it is desireable to return the fit parameters in a certain order (e.g. faster decay components first in a multi-exponential model)
  *
  * \param[out] deriatives the vector of partial derivates \f$ \partial f(x,\vec{p})/\partial p_i \f$ with respect to the parameters. This vector is at least as long as the number of parameters. Note, that you only have to evaluate derivatives for actual FIT parameters. All other values can be left unchanged.
  * \param[in] x the position, at which to evaluate the derivatives
  * \param[in] params the current parameter vector
  *
  */
typedef void (*QF3SimpleFFEvaluateDerivatives)(double* ,double, const double*);


/** \brief type of the estimateInitial() function of a model (in the DLL), which can be used to estimate initial values for the function from a given dataset
  * \ingroup quickfit3_models
  *
 *
 * \param[out] params the estimated fit parameters
 * \param[in] dataX x-values of the input dataset, for which to estimate the parameters
 * \param[in] dataY y-values of the input dataset, for which to estimate the parameters
 * \param[in] N number of entries in dataX and dataY
 * \param[in] fix if provided (!=NULL), indicates, which parameters are fixed by the suer. these should not be changed.
 * \return QF3SFF_TRUE on success
  */
typedef int8_t (*QF3SimpleFFEstimateInitial)(double*,const double*,const double*,int64_t, const int8_t* );




/** \brief type of the getAdditionalPlotCount() function of a model (in the DLL), which returns the number of plots (in addition to the full fit function plot), which can be plotted for a given fit parameter set. The parameters to generate a specific plot are altered, using transformParametersForAdditionalPlot().
  * \ingroup quickfit3_models
  *
 *
 * \param params the current parameter vector. The size of this vector is at least as large as the number of parameters.
  */
typedef int32_t (*QF3SimpleFFGetAdditionalPlotCount)(const double* );




/** \brief type of the transformParametersForAdditionalPlot() function of a model (in the DLL), which transforms the given parameter vector, so it creates the given additional plot
  * \ingroup quickfit3_models
  *
 *
 * \param p the plot for which to transporm
 * \param params the current parameter vector. The size of this vector is at least as large as the number of parameters.
 * \return a name for the plot
  */
typedef const char* (*QF3SimpleFFTransformParametersForAdditionalPlot)(int32_t,double* );


#endif //QUICKFITMODELTOOLS_H

