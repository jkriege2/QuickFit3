#ifndef QFFITALGORITHMGSLBFGS2_H
#define QFFITALGORITHMGSLBFGS2_H

#include "qffitalgorithmgslderivbase.h"

class QFFitAlgorithmGSLBFGS2 : public QFFitAlgorithmGSLDerivativeBase
{
    public:
        QFFitAlgorithmGSLBFGS2(): QFFitAlgorithmGSLDerivativeBase(gsl_multimin_fdfminimizer_vector_bfgs2) { }
        /** \copydoc QFFitAlgorithm::name() */
        virtual QString name() const { return QObject::tr("GSL: Broyden-Fletcher-Goldfarb-Shanno (BFGS) v2"); }
        /** \copydoc QFFitAlgorithm::shortName() */
        virtual QString shortName() const { return QObject::tr("GSL: BFGS Minimizer"); }
        /** \copydoc QFFitAlgorithm::id() */
        virtual QString id() const { return QString("fit_gsl_bfgs2"); }
};

#endif // QFFITALGORITHMGSLBFGS2_H
