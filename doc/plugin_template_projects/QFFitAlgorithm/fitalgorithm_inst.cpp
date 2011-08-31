#include "fitalgorithm_inst.h"
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include "fitalgorithm_inst_a1.h"


QStringList QFFitAlgorithmInstPlugin::getIDs() const {
	QStringList sl;
	sl<<"fa_id";
	return sl;
}

QFFitAlgorithm* QFFitAlgorithmInstPlugin::get(QString id, QObject* parent) const {
	if (id=="fa_id") return new QFFitAlgorithmInst_A1();
	return NULL;
}
Q_EXPORT_PLUGIN2(target_id, QFFitAlgorithmInstPlugin)
