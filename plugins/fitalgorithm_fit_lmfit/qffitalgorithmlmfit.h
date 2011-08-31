#ifndef QFFITALGORITHMLMFIT_H
#define QFFITALGORITHMLMFIT_H
#include "qffitalgorithm.h"


/*! \brief QQFFitAlgorithm implementation
    \ingroup qf3fitfunp_fit_lmfit


*/
class QFFitAlgorithmLMFit: public QFFitAlgorithm {
    protected:

        /** \copydoc QFFitAlgorithm::intFit() */
        virtual FitResult intFit(double* paramsOut, double* paramErrorsOut, const double* initialParams, Functor* model, const double* paramsMin, const double* paramsMax);


    public:
        /** \brief class constructor */
        QFFitAlgorithmLMFit();
        /** \brief class destructor */
        virtual ~QFFitAlgorithmLMFit() {}
        /** \copydoc QFFitAlgorithm::name() */
        virtual QString name() const { return QObject::tr("lmfit: Levenberg-Marquard fit (without box constraints)"); };
        /** \copydoc QFFitAlgorithm::shortName() */
        virtual QString shortName() const { return QObject::tr("lmfit: Levenberg-Marquard fit"); };
        /** \copydoc QFFitAlgorithm::id() */
        virtual QString id() const { return QString("fit_lmfit"); };
        /** \copydoc QFFitAlgorithm::get_supportsBoxConstraints() */
        virtual bool get_supportsBoxConstraints() const { return true; };
        /** \copydoc QFFitAlgorithm::displayConfig() */
        virtual bool displayConfig();
};


#endif // QFFITALGORITHMLMFIT_H
