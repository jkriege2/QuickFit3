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

#include "qffitalgorithmgslsimplexrand.h"
#include "qffitalgorithmgslsimplexconfigdialog.h"
#include "qffitalgorithmgsltools.h"

#include <cmath>


#include "gsl/gsl_vector.h"
#include "gsl/gsl_multimin.h"
#include "gsl/gsl_blas.h"






QFFitAlgorithmGSLSimplexRand::QFFitAlgorithmGSLSimplexRand():
    QFFitAlgorithmGSLSimplex()
{
    T = gsl_multimin_fminimizer_nmsimplex2rand;
    setParameter("stepsize", 1);
    setParameter("max_iterations", 200);
}


bool QFFitAlgorithmGSLSimplexRand::displayConfig() {
    QFFitAlgorithmGSLSimplexConfigDialog* dlg=new QFFitAlgorithmGSLSimplexConfigDialog(0);

    dlg->setWindowTitle(QObject::tr("GSL Randomized Simplex Parameters ..."));
	// init widget here:
    dlg->init(this);

    if (dlg->exec()==QDialog::Accepted) {
		// read back entered values here
        dlg->storeParams(this);
        delete dlg;
        return true;
    } else {
        delete dlg;
        return false;
    }
}

