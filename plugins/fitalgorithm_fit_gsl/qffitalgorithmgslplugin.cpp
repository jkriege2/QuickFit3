/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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

QFFitAlgorithm* QFFitAlgorithmGSLPluginPlugin::get(QString id, QObject* /*parent*/) const {
    if (id=="fit_gsl_simplex") return new QFFitAlgorithmGSLSimplex();
    if (id=="fit_gsl_simplexrand") return new QFFitAlgorithmGSLSimplexRand();
    if (id=="fit_gsl_bfgs2") return new QFFitAlgorithmGSLBFGS2();
    if (id=="fit_gsl_steepest_descent") return new QFFitAlgorithmGSLSteepestDescent();
    if (id=="fit_gsl_conjugate_fr") return new QFFitAlgorithmGSLConjugateFR();
    if (id=="fit_gsl_conjugate_pr") return new QFFitAlgorithmGSLConjugatePR();
    return NULL;
}
Q_EXPORT_PLUGIN2(fit_gsl, QFFitAlgorithmGSLPluginPlugin)
