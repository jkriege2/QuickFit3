#include "qfpfitalgorithmlmfit.h"
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include "qffitalgorithmlmfit.h"
#include "qffitalgorithmlmfitirls.h"


QStringList QFPFitAlgorithmLMFitPlugin::getIDs() const {
	QStringList sl;
        sl<<"fit_lmfit";
        sl<<"fit_irls_lmfit";
        return sl;
}

QFFitAlgorithm* QFPFitAlgorithmLMFitPlugin::get(QString id, QObject* parent) const {
	if (id=="fit_lmfit") return new QFFitAlgorithmLMFit();
        if (id=="fit_irls_lmfit") return new QFFitAlgorithmLMFitIRLS();
        return NULL;
}

Q_EXPORT_PLUGIN2(fit_lmfit, QFPFitAlgorithmLMFitPlugin)
