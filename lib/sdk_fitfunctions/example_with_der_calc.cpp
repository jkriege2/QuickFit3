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
        virtual std::string shortName() const { return "example model"; };
        /** \brief return a short unique model ID string */
        virtual std::string id() const { return "gen_example"; };
		
        double evaluate(double t, const double* data) const {
            const double p1=data[PARAM1];
            return p1*t*t;

            // this dummy function is   f(x)=p1*x^2
        }

        void evaluateDerivatives(double* derivatives, double t, const double* data) const {
            for (register int i=0; i<paramCount(); i++) derivatives[i]=0;

            const double p1=data[PARAM1];

            derivatives[PARAM1]=2*p1*t;
            // so the derivatives are   df/dp1 = 2*p1*x
        }

        void calcParameter(double* data, double* error) const {
            const double p1=data[PARAM1];
            double ep1=0;
            if (error) {
                ep1=error[PARAM1];
            }

            data[PARAM2]=2*p1;
            if (error) error[PARAM2]=2*ep1;
            // here a second parameter is 2*p1 (also it's error is calculated as 2*error_p1
        }


};

EXPORT_FITFUNCTION(ExampleFitFunction, "Jan Krieger") \
    
