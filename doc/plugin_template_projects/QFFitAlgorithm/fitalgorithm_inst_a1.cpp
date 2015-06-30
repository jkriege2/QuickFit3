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
#include "statistics_tools.h"

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

	// store whether the fit was successful
	result.fitOK=true;
	
	// message describing the fit result or error message
	result.message=QObject::tr("<b>fit was successfull</b>");
    result.messageSimple=QObject::tr("fit was successfull");;

	// brief store chi^2=sum ( (y_i-f(x_i))/sigma_i )^2
	double chi2=0;
	result.addNumber("error_sum", chi2);
	
	// calculate jacobian and var-cov matrix, which can be used to estimate errors in least-squares problems.
	QVector<double> J(model->get_evalout()*model->get_paramcount());
    QVector<double> COV(model->get_paramcount()*model->get_paramcount());
    model->evaluateJacobianNum(J.data(), paramsOut);
    if (QFFitAlgorithm::functorHasWeights(model) && !QFFitAlgorithm::functorAllWeightsOne(model)) statisticsGetFitProblemCovMatrix(COV.data(), J.data(), model->get_evalout(), model->get_paramcount());
    else statisticsGetFitProblemVarCovMatrix(COV.data(), J.data(), model->get_evalout(), model->get_paramcount(), chi2);

	// calculate errors from var-cov-matrox
    for (int i=0; i<model->get_paramcount(); i++) {
        paramErrorsOut[i]=statisticsGetFitProblemParamErrors(i, COV.data(), model->get_paramcount());
    }
	
	
	// store other fit results
    result.addNumber("param_name", param);


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






// if your least-squares fit algorithm is derived from a general optimization algorithm, you can also implement the following two functions
// so the general optimization algorithm, which solves   p=min_p f(p)   can be used by QF3 plugins:

//QFFitAlgorithm::FitResult QFFitAlgorithmInst_A1::intMinimize(double* paramsOut, const double* initialParams, FunctorForFMin* model, const double* paramsMin, const double* paramsMax) {
//    QFFitAlgorithm::FitResult result;
//
//    int paramCount=model->get_paramcount();
//
//    double param=getParameter("param_name").toDouble();
//
	//////////////////////////////////////////////////////////////////////
	// DO YOU FIT HERE
	//////////////////////////////////////////////////////////////////////
	// store whether the fit was successful
//	result.fitOK=true;
	// message describing the fit result or error message
//	result.message=QObject::tr("<b>fit was successfull</b>");
//	result.messageSimple=QObject::tr("fit was successfull");;
	// store final value of the objective function (in model)
//	result.addNumber("final_value", model->evaluate(paramsOut));
	// store other fit results
//	result.addNumber("param_name", param);
//	return result;
//}