#ifndef QFFitAlgorithmLMFitBox_H
#define QFFitAlgorithmLMFitBox_H
#include "qffitalgorithm.h"


/*! \brief QQFFitAlgorithm implementation
    \ingroup qf3fitfunp_fit_lmfit


*/
class QFFitAlgorithmLMFitBox: public QFFitAlgorithm {
    protected:

        /** \copydoc QFFitAlgorithm::intFit() */
        virtual FitResult intFit(double* paramsOut, double* paramErrorsOut, const double* initialParams, Functor* model, const double* paramsMin, const double* paramsMax);


    public:
        /** \brief class constructor */
        QFFitAlgorithmLMFitBox();
        /** \brief class destructor */
        virtual ~QFFitAlgorithmLMFitBox() {}
        /** \copydoc QFFitAlgorithm::name() */
        virtual QString name() const { return QObject::tr("lmfit: Levenberg-Marquard fit (with box constraints)"); };
        /** \copydoc QFFitAlgorithm::shortName() */
        virtual QString shortName() const { return QObject::tr("lmfit: Levenberg-Marquard fit"); };
        /** \copydoc QFFitAlgorithm::id() */
        virtual QString id() const { return QString("fit_lmfit"); };
        /** \copydoc QFFitAlgorithm::get_supportsBoxConstraints() */
        virtual bool get_supportsBoxConstraints() const { return true; };
        /** \copydoc QFFitAlgorithm::displayConfig() */
        virtual bool displayConfig();
        /** \copydoc QFFitAlgorithm::isThreadSafe() */
        virtual bool isThreadSafe() const ;

};


#endif // QFFitAlgorithmLMFitBox_H
