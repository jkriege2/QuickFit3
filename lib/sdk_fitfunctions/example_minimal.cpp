#include "qfcfitfunction.h"
#include <string>
#include <cmath>
#include <climits>
#include <cfloat>

class LIB_EXPORT ExampleFitFunction: public QFCFitFunction {
    public:
        ExampleFitFunction() {
            // DEFINE FIT FUNCTION PARAMETERS HERE:
            
            //           type,         id,                        name,                                                    label (HTML),                      unit,          unitlabel (HTML),        fit,       userEditable, userRangeEditable, displayError, initialFix,                initialValue, minValue, maxValue, inc, absMin, absMax
            addParameter(FloatNumber,  "p1",                      "",                                                      "",                                "",            "",                      true,      true,         true,              QFCFitFunction::DisplayError,       true, 0.0,          -10,      10,       1,   -100,    100  );
            #define PARAM1 0
            addParameter(FloatNumber,  "p2",                      "",                                                      "",                                "",            "",                      false,     false,        false,             QFCFitFunction::DisplayError,       true, 0.0,          -10,      10,       1,   -100,    100  );
            #define PARAM2 1
        }
        
		/** \brief return a name for the model */
        virtual std::string name() const { return "example fit model"; }
        /** \brief return a short name for the model (max. around 10-20 characters). THIS IS STRICTLY OPTIONAL! */
        virtual std::string shortName() const { return name(); };
        /** \brief return a short unique model ID string */
        virtual std::string id() const { return "gen_example"; };
		
        double evaluate(double t, const double* data) const {
            const double p1=data[PARAM1];
            const double p2=data[PARAM1];
            return p1*t*t+p2;

            // this dummy function is   f(x)=p1*x^2+p2
        }

};

EXPORT_FITFUNCTION(ExampleFitFunction, "Jan Krieger") \
    
