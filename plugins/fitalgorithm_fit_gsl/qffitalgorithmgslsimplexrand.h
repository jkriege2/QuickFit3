#ifndef QFFitAlgorithmGSLSimplexRand_H
#define QFFitAlgorithmGSLSimplexRand_H
#include "qffitalgorithm.h"


/*! \brief QQFFitAlgorithm implementation
    \ingroup qf3fitfunp_fit_gsl


*/
class QFFitAlgorithmGSLSimplexRand: public QFFitAlgorithm {
    protected:

        /** \copydoc QFFitAlgorithm::intFit() */
        virtual FitResult intFit(double* paramsOut, double* paramErrorsOut, const double* initialParams, Functor* model, const double* paramsMin, const double* paramsMax);


    public:
        /** \brief class constructor */
        QFFitAlgorithmGSLSimplexRand();
        /** \brief class destructor */
        virtual ~QFFitAlgorithmGSLSimplexRand() {}
        /** \copydoc QFFitAlgorithm::name() */
        virtual QString name() const { return QObject::tr("GSL: Randomized Simplex Minimizer (derivative free)"); }
        /** \copydoc QFFitAlgorithm::shortName() */
        virtual QString shortName() const { return QObject::tr("GSL: Randomized Simplex Minimizer"); }
        /** \copydoc QFFitAlgorithm::id() */
        virtual QString id() const { return QString("fit_gsl_simplex_rand"); }
        /** \copydoc QFFitAlgorithm::get_supportsBoxConstraints() */
        virtual bool get_supportsBoxConstraints() const { return true; }
        /** \copydoc QFFitAlgorithm::get_supportsBoxConstraints() */
        virtual bool isThreadSafe() const { return true; }
        /** \copydoc QFFitAlgorithm::displayConfig() */
        virtual bool displayConfig();
};


#endif // QFFitAlgorithmGSLSimplexRand_H
