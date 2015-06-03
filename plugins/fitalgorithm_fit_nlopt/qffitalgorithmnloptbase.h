#ifndef QFFITALGORITHMNLOPTBASE_H
#define QFFITALGORITHMNLOPTBASE_H

#include "qffitalgorithm.h"
#include "qffitalgorithmnlopttools.h"

/*! \brief QQFFitAlgorithm implementation
    \ingroup qf3fitfunp_fit_nlopt


*/
class QFFitAlgorithmNLOptBASE: public QFFitAlgorithm {
    protected:

        nlopt_algorithm nlopt_alg;

        /** \copydoc QFFitAlgorithm::intFit() */
        virtual FitResult intFit(double* paramsOut, double* paramErrorsOut, const double* initialParams, Functor* model, const double* paramsMin, const double* paramsMax);


    public:
        /** \brief class constructor */
        QFFitAlgorithmNLOptBASE(nlopt_algorithm nlopt_alg);
        /** \brief class destructor */
        virtual ~QFFitAlgorithmNLOptBASE() {}
        /** \copydoc QFFitAlgorithm::name() */
        virtual QString name() const;
        /** \copydoc QFFitAlgorithm::shortName() */
        inline virtual QString shortName() const { return name(); }
        /** \copydoc QFFitAlgorithm::get_supportsBoxConstraints() */
        inline virtual bool get_supportsBoxConstraints() const { return true; }
        /** \copydoc QFFitAlgorithm::displayConfig() */
        virtual bool displayConfig();
        /** \copydoc QFFitAlgorithm::isThreadSafe() */
        inline virtual bool isThreadSafe() const { return true; }
};

#endif // QFFITALGORITHMNLOPTBASE_H
