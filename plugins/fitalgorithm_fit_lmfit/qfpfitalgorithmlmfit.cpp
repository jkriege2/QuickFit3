/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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

#include "qfpfitalgorithmlmfit.h"
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include "qffitalgorithmlmfit.h"
#include "qffitalgorithmlmfitbox.h"
#include "qffitalgorithmlmfitirls.h"


QStringList QFPFitAlgorithmLMFitPlugin::getIDs() const {
	QStringList sl;
    sl<<"fit_lmfit";
    sl<<"fit_lmfit_nobox";
    sl<<"fit_irls_lmfit";
    return sl;
}

QFFitAlgorithm* QFPFitAlgorithmLMFitPlugin::get(const QString& id) const {
    if (id=="fit_lmfit") return new QFFitAlgorithmLMFitBox();
    if (id=="fit_lmfit_nobox") return new QFFitAlgorithmLMFit();
    if (id=="fit_irls_lmfit") return new QFFitAlgorithmLMFitIRLS();
    return NULL;
}

Q_EXPORT_PLUGIN2(fit_lmfit, QFPFitAlgorithmLMFitPlugin)
