#ifndef QFFitAlgorithmLMFitIRLSIRLS_H
#define QFFitAlgorithmLMFitIRLSIRLS_H
#include "qffitalgorithm.h"
#include "qffitalgorithmlmfit.h"

/*! \brief QQFFitAlgorithm implementation
    \ingroup qf3fitfunp_fit_lmfit


*/
class QFFitAlgorithmLMFitIRLS: public QFFitAlgorithmLMFit {
    protected:

        /** \copydoc QFFitAlgorithm::intFit() */
        virtual FitResult intFit(double* paramsOut, double* paramErrorsOut, const double* initialParams, Functor* model, const double* paramsMin, const double* paramsMax);


    public:
        /** \brief class constructor */
        QFFitAlgorithmLMFitIRLS();
        /** \brief class destructor */
        virtual ~QFFitAlgorithmLMFitIRLS() {}
        /** \copydoc QFFitAlgorithm::name() */
        virtual QString name() const { return QObject::tr("irls_lmfit: Iteratively reweightes Levenberg-Marquard fit (outlier-robust fit)"); };
        /** \copydoc QFFitAlgorithm::shortName() */
        virtual QString shortName() const { return QObject::tr("irls_lmfit: IRLS Levenberg-Marquard fit"); };
        /** \copydoc QFFitAlgorithm::id() */
        virtual QString id() const { return QString("fit_irls_lmfit"); };
        /** \copydoc QFFitAlgorithm::displayConfig() */
        virtual bool displayConfig();

};


#endif // QFFitAlgorithmLMFitIRLSIRLS_H
