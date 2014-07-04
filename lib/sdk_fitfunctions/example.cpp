#include "quickfit-model-header.h"
#include <cmath>
#include <cfloat>

// model ID (internally used by QUickFit3). Since this should be unique, you should add a part, which identifies you or your organization!)
// The prefix defines the use of the model, e.g. fcs_ for FCS fit models, dls_ for DLS fit models, gen_ for general fit models etc.
char QF3SFF_ID[]="gen_dkfzB040_example";

// human readable name
char QF3SFF_NAME[]="EXAMPLE: fit function from shared lib";

// human readable name
char QF3SFF_SHORTNAME[]="EXAMPLE: shared lib FitFunc";

// link to a help file, relative to the library
char QF3SFF_HELP[]="example.html";

// number of parameters of the fit model
#define QF3SFF_PARAMETER_COUNT 3

// description of the parameters of the fit model:
QF3SimpleFFParameter QF3SFF_PARAMETERS[QF3SFF_PARAMETER_COUNT] = { \
   // type,                    id,                 name,                                       label,                                      unit,               unitLabel,         fit,           userEditable,      displayError,           userRangeEditable,    initialValue,   initialFix,      minValue,      maxValue,      inc,        absMinValue,      absMaxValue
  {   QF3SFF_WIDGET_FLOAT,     "offset",           "Offset",                                   "Offset",                                   "",                 "",                QF3SFF_TRUE,   QF3SFF_TRUE,       QF3SFF_ERROR_DISPLAY,   QF3SFF_TRUE,          0.0,            QF3SFF_FALSE,    -10.0,         10.0,          0.1,        -DBL_MAX,         DBL_MAX            },
  {   QF3SFF_WIDGET_FLOAT,     "amplitude",        "Amplitude",                                "Amplitude",                                "",                 "",                QF3SFF_TRUE,   QF3SFF_TRUE,       QF3SFF_ERROR_DISPLAY,   QF3SFF_TRUE,          1.0,            QF3SFF_FALSE,    -DBL_MAX,      DBL_MAX,       0.1,        -DBL_MAX,         DBL_MAX            },
  {   QF3SFF_WIDGET_FLOAT,     "frequency",        "Frequency",                                "Frequency",                                "",                 "",                QF3SFF_TRUE,   QF3SFF_TRUE,       QF3SFF_ERROR_DISPLAY,   QF3SFF_TRUE,          10.0,           QF3SFF_FALSE,    -DBL_MAX,      DBL_MAX,       1,          -DBL_MAX,         DBL_MAX            }
};

// implementation of the evaluate() function
double QF3SFF_EVALUATE(double t, const double* parameters) {
    return parameters[0]+parameters[1]*sin(2*M_PI*parameters[2]*t);
}

#include "quickfit-model-functions.h"