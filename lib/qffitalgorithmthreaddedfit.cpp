#include "qffitalgorithmthreaddedfit.h"
#include <QTime>

QFFitAlgorithmThreadedFit::QFFitAlgorithmThreadedFit(QObject* parent): QThread(parent)
{
    //ctor
}

QFFitAlgorithmThreadedFit::~QFFitAlgorithmThreadedFit()
{
    //dtor
}

void QFFitAlgorithmThreadedFit::init(QFFitAlgorithm* algorithm, double* paramsOut, double* paramErrorsOut, double* dataX, double* dataY, double* dataWeight, uint64_t N, QFFitFunction* model, double* initialParams, bool* fixParams, double* paramsMin, double* paramsMax, int repeats) {
    this->algorithm=algorithm;
    this->paramsOut=paramsOut;
    this->paramErrorsOut=paramErrorsOut;
    this->dataX=dataX;
    this->dataY=dataY;
    this->dataWeight=dataWeight;
    this->N=N;
    this->model=model;
    this->initialParams=initialParams;
    this->fixParams=fixParams;
    this->paramsMin=paramsMin;
    this->paramsMax=paramsMax;
    this->repeats=repeats;
};

void QFFitAlgorithmThreadedFit::run() {
    QTime tstart=QTime::currentTime();
    double* init=duplicateArray(initialParams, model->paramCount());
    for (int i=0; i<repeats; i++) {
        result=algorithm->fit(paramsOut, paramErrorsOut, dataX, dataY, dataWeight, N, model, init, fixParams, paramsMin, paramsMax);
        copyArray(init, paramsOut, model->paramCount());
    }
    free(init);
    deltaTime=(double)tstart.msecsTo(QTime::currentTime());
};



