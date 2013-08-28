#ifndef QFFITALGORITHMGSLSIMPLEX_H
#define QFFITALGORITHMGSLSIMPLEX_H
#include "qffitalgorithm.h"
#include "gsl/gsl_multimin.h"


/*! \brief QQFFitAlgorithm implementation
    \ingroup qf3fitfunp_fit_gsl


*/
class QFFitAlgorithmGSLSimplex: public QFFitAlgorithm {
    protected:

        /** \copydoc QFFitAlgorithm::intFit() */
        virtual FitResult intFit(double* paramsOut, double* paramErrorsOut, const double* initialParams, Functor* model, const double* paramsMin, const double* paramsMax);

        const gsl_multimin_fminimizer_type *T;
    public:
        /** \brief class constructor */
        QFFitAlgorithmGSLSimplex();
        /** \brief class destructor */
        virtual ~QFFitAlgorithmGSLSimplex() {}
        /** \copydoc QFFitAlgorithm::name() */
        virtual QString name() const { return QObject::tr("GSL: Simplex Minimizer (derivative free)"); }
        /** \copydoc QFFitAlgorithm::shortName() */
        virtual QString shortName() const { return QObject::tr("GSL: Simplex Minimizer"); }
        /** \copydoc QFFitAlgorithm::id() */
        virtual QString id() const { return QString("fit_gsl_simplex"); }
        /** \copydoc QFFitAlgorithm::get_supportsBoxConstraints() */
        virtual bool get_supportsBoxConstraints() const { return true; }
        /** \copydoc QFFitAlgorithm::get_supportsBoxConstraints() */
        virtual bool isThreadSafe() const { return true; }
        /** \copydoc QFFitAlgorithm::displayConfig() */
        virtual bool displayConfig();
};


#endif // QFFITALGORITHMGSLSIMPLEX_H
