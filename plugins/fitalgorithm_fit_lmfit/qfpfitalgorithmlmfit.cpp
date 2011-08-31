#include "qfpfitalgorithmlmfit.h"
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include "qffitalgorithmlmfit.h"


QStringList QFPFitAlgorithmLMFitPlugin::getIDs() const {
	QStringList sl;
	sl<<"fit_lmfit";
	return sl;
}

QFFitAlgorithm* QFPFitAlgorithmLMFitPlugin::get(QString id, QObject* parent) const {
	if (id=="fit_lmfit") return new QFFitAlgorithmLMFit();
	return NULL;
}

Q_EXPORT_PLUGIN2(fit_lmfit, QFPFitAlgorithmLMFitPlugin)
