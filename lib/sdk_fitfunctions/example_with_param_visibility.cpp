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
#include <math.h>
#include <float.h>
#include "quickfit-model-header.h"


// model ID (internally used by QUickFit3). Since this should be unique, you should add a part, which identifies you or your organization!)
// The prefix defines the use of the model, e.g. fcs_ for FCS fit models, dls_ for DLS fit models, gen_ for general fit models etc.
char QF3SFF_ID[]="gen_dkfzB040_example_with_param_visibility";

// human readable name
char QF3SFF_NAME[]="EXAMPLE: fit function from shared lib, with visibility";

// human readable name
char QF3SFF_SHORTNAME[]="EXAMPLE: shared lib FitFunc, with visibility";

// link to a help file, relative to the library
char QF3SFF_HELP[]="example_with_param_visibility.html";

// a category for the fit functions
#define QF3SFF_CATEGORY "example fit functions"

// number of parameters of the fit model
#define QF3SFF_PARAMETER_COUNT 8

// description of the parameters of the fit model:
// each parameter is described in one line of this array. For each parameter, these properties can be set (in order of appearance):
//    type = [QF3SFF_WIDGET_FLOAT | QF3SFF_WIDGET_LOGFLOAT | QF3SFF_WIDGET_INT | QF3SFF_WIDGET_INT_COMBO ]: the type of input widget, that should be used [floating-point input, optimized for linearly scaling and log-scaling parameters | integer input widget | drop-down-box for the integer values within the parameter range]
//    id = [STRING] internal parameter ID (you should use characters, digits and underscores '_' only) Some often used parameter names are:
//    name = [STRING] name of the fit-parameter (human-readable ... you can use any characters)
//    label = [STRING] the same as name, but an alternative form, in which you can use HTML-markup
//    unit = [STRING] name of the unit of the fit parameter (e.g. "m", "micron", "km/h", ...)
//    unitLabel = [STRING] the same as unit, but an alternative form, in which you can use HTML-markup
//    fit = [QF3SFF_TRUE | QF3SFF_FALSE] indicates whether the parameter is a fit parameter (i.e. can be fitted by a fit algorithm)
//    userEditable = [QF3SFF_TRUE | QF3SFF_FALSE] indicates whether the parameter can be edited by the user or is displayed only
//    displayError = [QF3SFF_ERROR_NONE | QF3SFF_ERROR_DISPLAY | QF3SFF_ERROR_EDIT] indicates whether the error of the parameter is not displayed/displayed read-only/editable by the user
//    userRangeEditable = [QF3SFF_TRUE | QF3SFF_FALSE] indicates whether the parameter value range can be edited by the user. Initially the range is set to [minValue...maxValue] and can be chosen within the bounds given by [absMinValue...absMaxValue]
//    initialValue = [DOUBLE] initial value of the fit parameter
//    initialFix = [QF3SFF_TRUE | QF3SFF_FALSE] indicates, whether the parameter is initially fixed
//    minValue = [DOUBLE] initial lower boundary constraint for parameter values (user -DBL_MAX, if you don't want to limit the parameter range)
//    maxValue = [DOUBLE] initial upper boundary constraint for parameter values (user DBL_MAX, if you don't want to limit the parameter range)
//    inc = [DOUBLE] this increment is used for the parameter editing widgets/controls
//    absMinValue = [DOUBLE] lower absolute boundary constraint for parameter values (user -DBL_MAX, if you don't want to limit the parameter range). The parameter value is ALWAYS limited to values >=absMinValue!
//    absMaxValue = [DOUBLE] upper absolute boundary constraint for parameter values (user DBL_MAX, if you don't want to limit the parameter range). The parameter value is ALWAYS limited to values <=absMaxValue!
//
// Here we also define PARAM_... macros, in order to simply accessing a large number of parameters!
QF3SimpleFFParameter QF3SFF_PARAMETERS[QF3SFF_PARAMETER_COUNT] = { \
   // type,                    id,                     name,                                       label,                                      unit,               unitLabel,         fit,           userEditable,      displayError,           userRangeEditable,    initialValue,   initialFix,      minValue,      maxValue,      inc,        absMinValue,      absMaxValue
  {   QF3SFF_WIDGET_INT_COMBO, "components",           "# components",                             "# components",                             "",                 "",                QF3SFF_FALSE,  QF3SFF_TRUE,       QF3SFF_ERROR_NONE,      QF3SFF_FALSE,         1.0,            QF3SFF_FALSE,    0,             2,             1,          0,                2                  },
#define PARAM_N_COMP 0
  {   QF3SFF_WIDGET_FLOAT,     "offset",               "Offset A0",                                "Offset A<sub>0</sub>",                     "",                 "",                QF3SFF_TRUE,   QF3SFF_TRUE,       QF3SFF_ERROR_DISPLAY,   QF3SFF_TRUE,          0.0,            QF3SFF_FALSE,    -10.0,         10.0,          0.1,        -DBL_MAX,         DBL_MAX            },
#define PARAM_OFFSET 1
  {   QF3SFF_WIDGET_FLOAT,     "amplitude1",           "Amplitude 1",                              "A<sub>1</sub>",                            "",                 "",                QF3SFF_TRUE,   QF3SFF_TRUE,       QF3SFF_ERROR_DISPLAY,   QF3SFF_TRUE,          1.0,            QF3SFF_FALSE,    -DBL_MAX,      DBL_MAX,       0.1,        -DBL_MAX,         DBL_MAX            },
#define PARAM_A1 2
  {   QF3SFF_WIDGET_FLOAT,     "frequency1",           "Frequency 1",                              "&nu;<sub>1</sub>",                         "Hz",               "Hz",              QF3SFF_TRUE,   QF3SFF_TRUE,       QF3SFF_ERROR_DISPLAY,   QF3SFF_TRUE,          10.0,           QF3SFF_FALSE,    -DBL_MAX,      DBL_MAX,       1,          -DBL_MAX,         DBL_MAX            },
#define PARAM_f1 3
  {   QF3SFF_WIDGET_FLOAT,     "angular_frequency1",   "angul. Frequency 1",                       "&omega;<sub>1</sub>=2&pi;&nu;<sub>1</sub>","Hz",               "Hz",              QF3SFF_FALSE,  QF3SFF_FALSE,      QF3SFF_ERROR_DISPLAY,   QF3SFF_FALSE,         10.0,           QF3SFF_FALSE,    -DBL_MAX,      DBL_MAX,       1,          -DBL_MAX,         DBL_MAX            },
#define PARAM_w1 4
  {   QF3SFF_WIDGET_FLOAT,     "amplitude2",           "Amplitude 2",                              "A<sub>2</sub>",                            "",                 "",                QF3SFF_TRUE,   QF3SFF_TRUE,       QF3SFF_ERROR_DISPLAY,   QF3SFF_TRUE,          1.0,            QF3SFF_FALSE,    -DBL_MAX,      DBL_MAX,       0.1,        -DBL_MAX,         DBL_MAX            },
#define PARAM_A2 5
  {   QF3SFF_WIDGET_FLOAT,     "frequency2",           "Frequency 2",                              "&nu;<sub>2</sub>",                         "Hz",               "Hz",              QF3SFF_TRUE,   QF3SFF_TRUE,       QF3SFF_ERROR_DISPLAY,   QF3SFF_TRUE,          10.0,           QF3SFF_FALSE,    -DBL_MAX,      DBL_MAX,       1,          -DBL_MAX,         DBL_MAX            },
#define PARAM_f2 6
  {   QF3SFF_WIDGET_FLOAT,     "angular_frequency2",   "angul. Frequency 2",                       "&omega;<sub>2</sub>=2&pi;&nu;<sub>2</sub>","Hz",               "Hz",              QF3SFF_FALSE,  QF3SFF_FALSE,      QF3SFF_ERROR_DISPLAY,   QF3SFF_FALSE,         10.0,           QF3SFF_FALSE,    -DBL_MAX,      DBL_MAX,       1,          -DBL_MAX,         DBL_MAX            },
#define PARAM_w2 7
};




// this macro is required here (before any function def)
QF3SFF_EXTERN_START
////////////////////////////////////////////////////////

// implementation of the evaluate() function
//    evaluate the function at position t and with the parameters given by p. The size of p is at least QF3SFF_PARAMETER_COUNT
 double QF3SFF_DLL_EXPORT evaluate(double t, const double* p) {
    const int nc=p[PARAM_N_COMP];
    double r= p[PARAM_OFFSET];
    if (nc>0) r=r+p[PARAM_A1]*sin(2.0*M_PI*p[PARAM_f1]*t);
    if (nc>1) r=r+p[PARAM_A2]*sin(2.0*M_PI*p[PARAM_f2]*t);
    return r;
}


// implementation of the calulateParameters() function
//    calculate additional model parameters, which are not fit parameters
 void QF3SFF_DLL_EXPORT calulateParameters(double* p, double* e) {
   // calculate omega1 and omega2
    p[PARAM_w1]=2.0*M_PI*p[PARAM_f1];
    p[PARAM_w2]=2.0*M_PI*p[PARAM_f2];
    if (e) {
        // calculate errors of omega1 and omega2
        e[PARAM_w1]=2.0*M_PI*e[PARAM_f1];
        e[PARAM_w2]=2.0*M_PI*e[PARAM_f2];
    }
}

// implementation of the isParameterVisible() function
//    return QF3SFF_TRUE is a parameter is visible:
//      all parameters are always visible, except PARAM_A1, PARAM_f1 and PARAM_w1,
//      which are only visible, when the user selected at least one component and
//      PARAM_A2, PARAM_f2 and PARAM_w2, which are only visible, when the user 
//      selected at least two components
int8_t QF3SFF_DLL_EXPORT isParameterVisible(int16_t param, const double* p) {
    switch(param) {
        case PARAM_A1: case PARAM_f1: case PARAM_w1: return (p[PARAM_N_COMP]>0)?QF3SFF_TRUE:QF3SFF_FALSE;
        case PARAM_A2: case PARAM_f2: case PARAM_w2: return (p[PARAM_N_COMP]>1)?QF3SFF_TRUE:QF3SFF_FALSE;
        default: return QF3SFF_TRUE;
    }
    return QF3SFF_TRUE;
}

// sort parameters by increasing frequency
void QF3SFF_DLL_EXPORT sortParameters(double* p, double* e, int8_t* fix) {
    const int nc=p[PARAM_N_COMP];
    const double f1=p[PARAM_f1];
    const double f2=p[PARAM_f2];
    // here we sort the parameter vector for increasing frequency, i.e.
    // we swap f1/w1/A1 and f2/w2/A2 if and only if we have two components 
    // and f1>f2!!!
    if (nc>1 && f1>f2) {
        bool ok=true;
        if (fix) {
            // if fix is given, we can check it and will only swap if none of the swapped parameters is fixed!
            ok=ok&&(fix[PARAM_f1]==QF3SFF_FALSE);
            ok=ok&&(fix[PARAM_f2]==QF3SFF_FALSE);
            ok=ok&&(fix[PARAM_A1]==QF3SFF_FALSE);
            ok=ok&&(fix[PARAM_A2]==QF3SFF_FALSE);
        }
        if (ok) {
            // swap parameter values and errors
            QF3SFF_swap(p[PARAM_f1], p[PARAM_f2]);
            QF3SFF_swap(p[PARAM_A1], p[PARAM_A2]);
            QF3SFF_swap(p[PARAM_w1], p[PARAM_w2]);
            if (e) {
                QF3SFF_swap(e[PARAM_f1], e[PARAM_f2]);
                QF3SFF_swap(e[PARAM_A1], e[PARAM_A2]);
                QF3SFF_swap(e[PARAM_w1], e[PARAM_w2]);
            }
        }
    }    
}

// this macro is required here /////////////////////////
QF3SFF_EXTERN_END
////////////////////////////////////////////////////////


// this macro is required here and finalizes the library
QF3SFF_FINALIZE
////////////////////////////////////////////////////////
