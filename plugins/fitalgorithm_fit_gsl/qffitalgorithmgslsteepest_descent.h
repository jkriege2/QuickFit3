#ifndef QFFITALGORITHMGSLSTEEPEST_DESCENT_H
#define QFFITALGORITHMGSLSTEEPEST_DESCENT_H

#include "qffitalgorithmgslderivbase.h"

class QFFitAlgorithmGSLSteepestDescent : public QFFitAlgorithmGSLDerivativeBase
{
    public:
        QFFitAlgorithmGSLSteepestDescent(): QFFitAlgorithmGSLDerivativeBase(gsl_multimin_fdfminimizer_steepest_descent) { setParameter("max_iterations", 2000); }
        /** \copydoc QFFitAlgorithm::name() */
        virtual QString name() const { return QObject::tr("GSL: Steepest Descent"); }
        /** \copydoc QFFitAlgorithm::shortName() */
        virtual QString shortName() const { return QObject::tr("GSL: Steepest Descent"); }
        /** \copydoc QFFitAlgorithm::id() */
        virtual QString id() const { return QString("fit_gsl_steepest_descent"); }
};
#endif // QFFITALGORITHMGSLSTEEPEST_DESCENT_H
