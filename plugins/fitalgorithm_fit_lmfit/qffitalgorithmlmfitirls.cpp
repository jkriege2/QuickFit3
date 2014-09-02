/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

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

#include "qffitalgorithmlmfitirls.h"
#include "qffitalgorithmlmfitconfig.h"
#include <cmath>
#include "lmmin.h"



QFFitAlgorithmLMFitIRLS::QFFitAlgorithmLMFitIRLS():
    QFFitAlgorithmLMFitBox()
{
    setParameter("irls_parameter", 1.1);
    setParameter("irls_iterations", 10);
}

QFFitAlgorithm::FitResult QFFitAlgorithmLMFitIRLS::intFit(double* paramsOut, double* paramErrorsOut, const double* initialParams, QFFitAlgorithm::Functor* model, const double* paramsMin, const double* paramsMax) {

    QFFitAlgorithm::FitFunctionFunctor* ffFunctor=dynamic_cast<QFFitAlgorithm::FitFunctionFunctor*>(model);
    if (ffFunctor) {
        QFFitAlgorithm::FitResult result;
        double irls_parameter=getParameter("irls_parameter").toDouble();
        int irls_iterations=getParameter("irls_iterations").toInt();
        copyArray(paramsOut, initialParams, model->get_paramcount());
        QFFitAlgorithm::IRLSFunctorAdaptor irls_functor(ffFunctor, irls_parameter);
        for (int i=0; i<irls_iterations; i++) {
            result= QFFitAlgorithmLMFitBox::intFit(paramsOut, paramErrorsOut, paramsOut, &irls_functor, paramsMin, paramsMax);
            irls_functor.irlsReweight(paramsOut);
        }
        return result;
    } else {
        QFFitAlgorithm::FitResult result;
        result.fitOK=false;
        result.message=QObject::tr("the IRLS method is only applicable to fitting problems, not to optimization problems! (could not cast functor to QFFitAlgorithm::FitFunctionFunctor)");
        result.messageSimple=result.message;
        return result;
    }
}

bool QFFitAlgorithmLMFitIRLS::displayConfig() {
    QFFitAlgorithmLMFitConfigDialog* dlg=new QFFitAlgorithmLMFitConfigDialog(0, true);

    // init widget here:
    dlg->init(getParameter("ftol").toDouble(), getParameter("xtol").toDouble(), getParameter("gtol").toDouble(), getParameter("epsilon").toDouble(), getParameter("stepbound").toDouble(), getParameter("max_iterations").toInt(), getParameter("irls_parameter").toDouble(), getParameter("irls_iterations").toInt());

    if (dlg->exec()==QDialog::Accepted) {
        // read back entered values here
        setParameter("ftol", dlg->ftol());
        setParameter("xtol", dlg->xtol());
        setParameter("gtol", dlg->gtol());
        setParameter("epsilon", dlg->epsilon());
        setParameter("stepbound", dlg->stepbound());
        setParameter("max_iterations", dlg->max_iterations());
        setParameter("irls_parameter", dlg->irls_parameter());
        setParameter("irls_iterations", dlg->irls_iterations());

        delete dlg;
        return true;
    } else {
        delete dlg;
        return false;
    }
}
