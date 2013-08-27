#ifndef QFFitAlgorithmGSLConjugatePR_H
#define QFFitAlgorithmGSLConjugatePR_H

#include "qffitalgorithmgslderivbase.h"

class QFFitAlgorithmGSLConjugatePR : public QFFitAlgorithmGSLDerivativeBase
{
    public:
        QFFitAlgorithmGSLConjugatePR(): QFFitAlgorithmGSLDerivativeBase(gsl_multimin_fdfminimizer_conjugate_pr) { setParameter("max_iterations", 2000); }
        /** \copydoc QFFitAlgorithm::name() */
        virtual QString name() const { return QObject::tr("GSL: Polak-Ribiere conjugate gradient"); }
        /** \copydoc QFFitAlgorithm::shortName() */
        virtual QString shortName() const { return QObject::tr("GSL: Polak-Ribiere conjugate gradient"); }
        /** \copydoc QFFitAlgorithm::id() */
        virtual QString id() const { return QString("fit_gsl_conjugate_pr"); }
};
#endif // QFFitAlgorithmGSLConjugatePR_H
