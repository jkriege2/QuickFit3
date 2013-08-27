#include "qffitalgorithmgslplugin.h"
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include "qffitalgorithmgslsimplex.h"
#include "qffitalgorithmgslsimplexrand.h"
#include "qffitalgorithmgslbfgs2.h"
#include "qffitalgorithmgslsteepest_descent.h"
#include "qffitalgorithmgslconjugate_fr.h"
#include "qffitalgorithmgslconjugate_pr.h"


QStringList QFFitAlgorithmGSLPluginPlugin::getIDs() const {
	QStringList sl;
    sl<<"fit_gsl_simplex";
    sl<<"fit_gsl_simplexrand";
    sl<<"fit_gsl_bfgs2";
    sl<<"fit_gsl_steepest_descent";
    sl<<"fit_gsl_conjugate_pr";
    sl<<"fit_gsl_conjugate_fr";
    return sl;
}

QFFitAlgorithm* QFFitAlgorithmGSLPluginPlugin::get(QString id, QObject* parent) const {
    if (id=="fit_gsl_simplex") return new QFFitAlgorithmGSLSimplex();
    if (id=="fit_gsl_simplexrand") return new QFFitAlgorithmGSLSimplexRand();
    if (id=="fit_gsl_bfgs2") return new QFFitAlgorithmGSLBFGS2();
    if (id=="fit_gsl_steepest_descent") return new QFFitAlgorithmGSLSteepestDescent();
    if (id=="fit_gsl_conjugate_fr") return new QFFitAlgorithmGSLConjugateFR();
    if (id=="fit_gsl_conjugate_pr") return new QFFitAlgorithmGSLConjugatePR();
    return NULL;
}
Q_EXPORT_PLUGIN2(fit_gsl, QFFitAlgorithmGSLPluginPlugin)
