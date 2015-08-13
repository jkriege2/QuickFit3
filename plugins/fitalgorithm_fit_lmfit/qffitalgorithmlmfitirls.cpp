/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg
    All rights reserved.

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    Redistribution and use in source and binary forms, with or without modification,
    are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright notice, this
       list of conditions and the following disclaimer in the documentation and/or other
       materials provided with the distribution.

    3. Neither the name of the copyright holder nor the names of its contributors may
       be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
    OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
    AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
    CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
    IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
    OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
