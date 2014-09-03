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

#include "qffitalgorithmlmfit.h"
#include "qffitalgorithmlmfitconfig.h"
#include <cmath>
#include "lmmin.h"

struct QFFItAlgorithmGSL_evalData {
    QFFitAlgorithm::Functor* model;
    const double* paramsMin;
    const double* paramsMax;
    double* p;
    int pcount;
};

void lmfit_eval(const double *par, int m_dat, const void *data, double *fvec, int *info) {
    QFFItAlgorithmGSL_evalData* edata=(QFFItAlgorithmGSL_evalData*)data;
    double* p=edata->p;
   /* if ( edata->paramsMin && edata->paramsMax) {
        for (int i=0; i<edata->pcount; i++) {
            p[i]=par[i];
            if (par[i]>edata->paramsMax[i]) p[i]=edata->paramsMax[i];
            if (par[i]<edata->paramsMin[i]) p[i]=edata->paramsMin[i];
        }
        edata->model->evaluate(fvec, p);
    } else {*/
        edata->model->evaluate(fvec, par);
    //}
}

QFFitAlgorithmLMFit::QFFitAlgorithmLMFit() {
    // set default parameter values
    lm_control_struct control = lm_control_double;
    setParameter("ftol", control.ftol);
    setParameter("xtol", control.xtol);
    setParameter("gtol", control.gtol);
    setParameter("epsilon", control.epsilon);
    setParameter("stepbound", control.stepbound);
    setParameter("max_iterations", control.patience);
}

QFFitAlgorithm::FitResult QFFitAlgorithmLMFit::intFit(double* paramsOut, double* paramErrorsOut, const double* initialParams, QFFitAlgorithm::Functor* model, const double* paramsMin, const double* paramsMax) {
    QFFitAlgorithm::FitResult result;

    int paramCount=model->get_paramcount(); // number of parameters
    memcpy(paramsOut, initialParams, paramCount*sizeof(double));

    //double param=getParameter("param_name").toDouble();


    lm_status_struct status;
    lm_control_struct control = lm_control_double;
    control.verbosity = 0; // monitor status (+1) and parameters (+2)
    control.ftol=getParameter("ftol").toDouble();
    control.xtol=getParameter("xtol").toDouble();
    control.gtol=getParameter("gtol").toDouble();
    control.epsilon=getParameter("epsilon").toDouble();
    control.stepbound=getParameter("stepbound").toDouble();
    control.patience=getParameter("max_iterations").toInt();

    QFFItAlgorithmGSL_evalData d;
    d.model=model;
    d.paramsMin=paramsMin;
    d.paramsMax=paramsMax;
    d.pcount=paramCount;
    d.p=(double*)malloc(paramCount*sizeof(double));

    lmmin( paramCount, paramsOut, model->get_evalout(), &d, lmfit_eval, &control, &status );

    if ( paramsMin && paramsMax) {
        for (int i=0; i<paramCount; i++) {
            const double& par=paramsOut[i];
            if (par>paramsMax[i]) paramsOut[i]=paramsMax[i];
            if (par<paramsMin[i]) paramsOut[i]=paramsMin[i];
        }
    }

    result.addNumber("error_sum", status.fnorm);
    result.addNumber("iterations", status.nfev);

    if (status.outcome>=0) {
        result.fitOK=QString(lm_infmsg[status.outcome]).contains("success") || QString(lm_infmsg[status.outcome]).contains("converged");
        result.message=QString(lm_infmsg[status.outcome]);
        result.messageSimple=QString(lm_infmsg[status.outcome]);
    } else {
        result.fitOK=true;
        result.message="";
        result.messageSimple="";
    }

    if (d.p) free(d.p);

    return result;
}

bool QFFitAlgorithmLMFit::displayConfig() {
    QFFitAlgorithmLMFitConfigDialog* dlg=new QFFitAlgorithmLMFitConfigDialog(0);

    // init widget here:
    dlg->init(getParameter("ftol").toDouble(), getParameter("xtol").toDouble(), getParameter("gtol").toDouble(), getParameter("epsilon").toDouble(), getParameter("stepbound").toDouble(), getParameter("max_iterations").toInt());

    if (dlg->exec()==QDialog::Accepted) {
        // read back entered values here
        setParameter("ftol", dlg->ftol());
        setParameter("xtol", dlg->xtol());
        setParameter("gtol", dlg->gtol());
        setParameter("epsilon", dlg->epsilon());
        setParameter("stepbound", dlg->stepbound());
        setParameter("max_iterations", dlg->max_iterations());
        delete dlg;
        return true;
    } else {
        delete dlg;
        return false;
    }
}

bool QFFitAlgorithmLMFit::isThreadSafe() const
{
    return true;
}

