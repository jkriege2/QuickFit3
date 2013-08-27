#ifndef QFFitAlgorithmGSLConjugateFR_H
#define QFFitAlgorithmGSLConjugateFR_H

#include "qffitalgorithmgslderivbase.h"

class QFFitAlgorithmGSLConjugateFR : public QFFitAlgorithmGSLDerivativeBase
{
    public:
        QFFitAlgorithmGSLConjugateFR(): QFFitAlgorithmGSLDerivativeBase(gsl_multimin_fdfminimizer_conjugate_fr) { setParameter("max_iterations", 2000); }
        /** \copydoc QFFitAlgorithm::name() */
        virtual QString name() const { return QObject::tr("GSL: Fletcher-Reeves conjugate gradient"); }
        /** \copydoc QFFitAlgorithm::shortName() */
        virtual QString shortName() const { return QObject::tr("GSL: Fletcher-Reeves conjugate gradient"); }
        /** \copydoc QFFitAlgorithm::id() */
        virtual QString id() const { return QString("fit_gsl_conjugate_fr"); }
};
#endif // QFFitAlgorithmGSLConjugateFR_H
