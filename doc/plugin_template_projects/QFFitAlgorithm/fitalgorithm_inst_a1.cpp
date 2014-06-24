/*
Copyright (c) 2014
	
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


#include "fitalgorithm_inst_a1.h"
#include "fitalgorithm_inst_a1config.h"
#include <cmath>


QFFitAlgorithmInst_A1::QFFitAlgorithmInst_A1() {
    // set default parameter values
	//setParameter("param_name", 0.0);
}

QFFitAlgorithm::FitResult QFFitAlgorithmInst_A1::intFit(double* paramsOut, double* paramErrorsOut, const double* initialParams, QFFitAlgorithm::Functor* model, const double* paramsMin, const double* paramsMax) {
    QFFitAlgorithm::FitResult result;

    int paramCount=model->get_paramcount();

    double param=getParameter("param_name").toDouble();

	//////////////////////////////////////////////////////////////////////
	// DO YOU FIT HERE
	//////////////////////////////////////////////////////////////////////

    result.addNumber("param_name", param);

    result.message=QObject::tr("<b>fit was successfull</b>");
    result.messageSimple=QObject::tr("fit was successfull");;

    return result;
}

bool QFFitAlgorithmInst_A1::displayConfig() {
    QFFitAlgorithmInst_A1ConfigDialog* dlg=new QFFitAlgorithmInst_A1ConfigDialog(0);

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

