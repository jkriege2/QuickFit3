#ifndef QFFitAlgorithmGSLSimplexRand_H
#define QFFitAlgorithmGSLSimplexRand_H
#include "qffitalgorithm.h"
#include "qffitalgorithmgslsimplex.h"

/*! \brief QQFFitAlgorithm implementation
    \ingroup qf3fitfunp_fit_gsl


*/
class QFFitAlgorithmGSLSimplexRand: public QFFitAlgorithmGSLSimplex {

    public:
        /** \brief class constructor */
        QFFitAlgorithmGSLSimplexRand();
        /** \copydoc QFFitAlgorithm::name() */
        virtual QString name() const { return QObject::tr("GSL: Randomized Simplex Minimizer (derivative free)"); }
        /** \copydoc QFFitAlgorithm::shortName() */
        virtual QString shortName() const { return QObject::tr("GSL: Randomized Simplex Minimizer"); }
        /** \copydoc QFFitAlgorithm::id() */
        virtual QString id() const { return QString("fit_gsl_simplex_rand"); }
        /** \copydoc QFFitAlgorithm::displayConfig() */
        virtual bool displayConfig();
};


#endif // QFFitAlgorithmGSLSimplexRand_H
