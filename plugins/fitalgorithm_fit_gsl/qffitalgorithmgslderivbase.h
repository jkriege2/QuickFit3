#ifndef QFFitAlgorithmGSLDerivativeBase_H
#define QFFitAlgorithmGSLDerivativeBase_H
#include "qffitalgorithm.h"
#include "gsl/gsl_multimin.h"


/*! \brief QQFFitAlgorithm implementation
    \ingroup qf3fitfunp_fit_gsl


*/
class QFFitAlgorithmGSLDerivativeBase: public QFFitAlgorithm {
    protected:

        const gsl_multimin_fdfminimizer_type *T;

        /** \copydoc QFFitAlgorithm::intFit() */
        virtual FitResult intFit(double* paramsOut, double* paramErrorsOut, const double* initialParams, Functor* model, const double* paramsMin, const double* paramsMax);


    public:
        /** \brief class constructor */
        QFFitAlgorithmGSLDerivativeBase(const gsl_multimin_fdfminimizer_type *T=NULL);
        /** \brief class destructor */
        virtual ~QFFitAlgorithmGSLDerivativeBase() {}
        /** \copydoc QFFitAlgorithm::get_supportsBoxConstraints() */
        virtual bool get_supportsBoxConstraints() const { return true; }
        /** \copydoc QFFitAlgorithm::get_supportsBoxConstraints() */
        virtual bool isThreadSafe() const { return true; }
        /** \copydoc QFFitAlgorithm::displayConfig() */
        virtual bool displayConfig();
};


#endif // QFFitAlgorithmGSLDerivativeBase_H
