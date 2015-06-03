#ifndef QFFITALGORITHMNLOPTTOOLS_H
#define QFFITALGORITHMNLOPTTOOLS_H

#include <cmath>
#include <nlopt.h>
#include "qfmathtools.h"
#include <QVector>
#include "qffitalgorithm.h"
#include "qffitfunction.h"

struct QFFItAlgorithmNLOpt_evalData {
    QFFitAlgorithm::Functor* model;

};

double QFFitAlgorithmNLOpt_f(unsigned int n, const double* x, double* grad, void *params);


#endif // QFFITALGORITHMNLOPTTOOLS_H
