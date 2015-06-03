/*
Copyright (c) 2014
	
	last modification: $LastChangedDate: 2015-05-02 18:18:13 +0200 (Sa, 02 Mai 2015) $  (revision $Rev: 3946 $)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "qffitalgorithmnloptplugin.h"
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include "qffitalgorithmnloptalgorithms.h"








QStringList QFFitAlgorithmNLOptPluginPlugin::getIDs() const {
	QStringList sl;
    sl<<"fit_nlopt_direct";
    sl<<"fit_nlopt_directl";
    sl<<"fit_nlopt_directl_rand";
    sl<<"fit_nlopt_CRS2_LM";
    sl<<"fit_nlopt_MLSL";
    sl<<"fit_nlopt_STOGO";
    sl<<"fit_nlopt_STOGO_RAND";
    sl<<"fit_nlopt_ISRES";
    sl<<"fit_nlopt_ESCH";
    return sl;
}

QFFitAlgorithm* QFFitAlgorithmNLOptPluginPlugin::get(const QString& id) const {
    if (id=="fit_nlopt_direct") return new QFFitAlgorithmNLOptDIRECT();
    if (id=="fit_nlopt_directl") return new QFFitAlgorithmNLOptDIRECTL();
    if (id=="fit_nlopt_directl_rand") return new QFFitAlgorithmNLOptDIRECTLRAND();
    if (id=="fit_nlopt_CRS2_LM") return new QFFitAlgorithmNLOptCRS2_LM();
    if (id=="fit_nlopt_MLSL") return new QFFitAlgorithmNLOptMLSL();
    if (id=="fit_nlopt_STOGO") return new QFFitAlgorithmNLOptStoGO();
    if (id=="fit_nlopt_STOGO_RAND") return new QFFitAlgorithmNLOptStoGORand();
    if (id=="fit_nlopt_ISRES") return new QFFitAlgorithmNLOptISRES();
    if (id=="fit_nlopt_ESCH") return new QFFitAlgorithmNLOptESCH();
    return NULL;
}
Q_EXPORT_PLUGIN2(fit_nlopt, QFFitAlgorithmNLOptPluginPlugin)
