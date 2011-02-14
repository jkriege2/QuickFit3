#ifndef QFFITALGORITHMTHREADDEDFIT_H
#define QFFITALGORITHMTHREADDEDFIT_H

#include <QThread>
#include <QObject>
#include "qffitalgorithm.h"
#include "qffitfunction.h"

/*! \brief this allows to execute a given QFFitAlgorithm inside an extra QThread.
    \ingroup qf3lib_fitting

    After allocating the memory for an object, call init() to intialize the data. Then you may
    start() the thread. Finally the results may be read using getResult() and the fit runtime in milliseconds using
    getDeltaTime().

*/
class QFFitAlgorithmThreadedFit: public QThread {
        Q_OBJECT
    public:
        /** \brief class cosntructor */
        QFFitAlgorithmThreadedFit(QObject* parent=NULL);
        /** \brief class destructor */
        virtual ~QFFitAlgorithmThreadedFit();

        /** \brief initialize the fit parameters */
        void init(QFFitAlgorithm* algorithm, double* paramsOut, double* paramErrorsOut, double* dataX, double* dataY, double* dataWeight, uint64_t N, QFFitFunction* model, double* initialParams, bool* fixParams=NULL, double* paramsMin=NULL, double* paramsMax=NULL);

        /** \brief return the result of the last fit call */
        QFFitAlgorithm::FitResult getResult() { return result; };
        /** \brief return the runtime of the fit in milliseconds */
        double getDeltaTime() { return deltaTime; };

    protected:
        QFFitAlgorithm* algorithm;
        double* paramsOut;
        double* paramErrorsOut;
        double* dataX;
        double* dataY;
        double* dataWeight;
        uint64_t N;
        QFFitFunction* model;
        double* initialParams;
        bool* fixParams;
        double* paramsMin;
        double* paramsMax;
        QFFitAlgorithm::FitResult result;
        double deltaTime;

        /** \brief here the fitting takes place */
        virtual void run();
};


#endif // QFFITALGORITHMTHREADDEDFIT_H
