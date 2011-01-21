#include "qffitalgorithm.h"
#include <QMessageBox>
#include <QObject>

QFFitAlgorithm::FitResult QFFitAlgorithm::fit(double* paramsOut, double* dataX, double* dataY, double* dataWeight, uint64_t N, QFFitFunction* model, double* initialParams, bool* fixParams, double* paramsMin, double* paramsMax) {
    QFFitAlgorithm::FitResult result;

    return result;
}

QFFitAlgorithm::FitResult QFFitAlgorithm::fit(double* paramsOut, double* dataX, double* dataY, double* dataWeight, uint64_t N, QFFitAlgorithm::Functor* model, double* initialParams, bool* fixParams, double* paramsMin, double* paramsMax) {
    QFFitAlgorithm::FitResult result;

    return result;
}

bool QFFitAlgorithm::displayConfig() {
    QMessageBox::information(NULL, name(), QObject::tr("No configuration dialog is supplied for this optimization algorithm!"));
    return false;
}
