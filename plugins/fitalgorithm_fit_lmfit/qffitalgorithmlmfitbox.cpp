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

#include "qffitalgorithmlmfitbox.h"
#include "qffitalgorithmlmfitconfig.h"
#include <cmath>
#include "lmmin.h"

#define RANGE_MAXVAL (DBL_MAX/10.0)

struct QFFItAlgorithmGSL_evalData {
    QFFitAlgorithm::Functor* model;
    const double* paramsMin;
    const double* paramsMax;
    double* p;
    int pcount;
};

void lmfit_evalboxtanh(const double *par, int m_dat, const void *data, double *fvec, int *info) {
    QFFItAlgorithmGSL_evalData* edata=(QFFItAlgorithmGSL_evalData*)data;
    if ( edata->paramsMin && edata->paramsMax ) {
        /*double* p=edata->p;
        for (int i=0; i<edata->pcount; i++) {
            const double mi=edata->paramsMin[i];
            const double ma=edata->paramsMax[i];
            if (fabs(mi)<RANGE_MAXVAL && fabs(ma)<RANGE_MAXVAL) {
                const double pv=tanh(par[i]);
                p[i]=mi+(pv+1.0)*(ma-mi)/2.0;
            } else {
                p[i]=par[i];
            }

        }*/

        double* p=edata->p;
        for (int i=0; i<edata->pcount; i++) {
            const double mi=edata->paramsMin[i];
            const double ma=edata->paramsMax[i];
            p[i]=qBound(mi, par[i], ma);
            /*if (fabs(mi)<RANGE_MAXVAL && fabs(ma)<RANGE_MAXVAL) {
                const double pv=tanh(par[i]);
                p[i]=mi+(pv+1.0)*(ma-mi)/2.0;
            } else {
                p[i]=par[i];
            }*/

        }

        //qDebug()<<arrayToString(p, edata->pcount);

        edata->model->evaluate(fvec, p);
    } else {
        edata->model->evaluate(fvec, par);
    }
}

QFFitAlgorithmLMFitBox::QFFitAlgorithmLMFitBox() {
    // set default parameter values
    lm_control_struct control = lm_control_double;
    setParameter("ftol", control.ftol);
    setParameter("xtol", control.xtol);
    setParameter("gtol", control.gtol);
    setParameter("epsilon", control.epsilon);
    setParameter("stepbound", control.stepbound);
    setParameter("max_iterations", control.patience);
}

QFFitAlgorithm::FitResult QFFitAlgorithmLMFitBox::intFit(double* paramsOut, double* paramErrorsOut, const double* initialParams, QFFitAlgorithm::Functor* model, const double* paramsMin, const double* paramsMax) {
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

    bool transformParams=paramsMin&&paramsMax;

    lmmin( paramCount, paramsOut, model->get_evalout(), &d, lmfit_evalboxtanh, &control, &status );
    if (paramsMin && paramsMax) {
        for (int i=0; i<paramCount; i++) {
            const double mi=paramsMin[i];
            const double ma=paramsMax[i];
            paramsOut[i]=qBound(mi, paramsOut[i], ma);
        }
    }

   /* if (!transformParams) {
        lmmin( paramCount, paramsOut, model->get_evalout(), &d, lmfit_evalboxtanh, &control, &status, NULL );
    } else {
        for (int i=0; i<paramCount; i++) {
            const double mi=paramsMin[i];
            const double ma=paramsMax[i];
            const double pv=2.0*(paramsOut[i]-mi)/(ma-mi)-1.0;
            if (fabs(mi)<RANGE_MAXVAL && fabs(ma)<RANGE_MAXVAL) {
                paramsOut[i]=atanh(pv);
                if (pv>=1.0) paramsOut[i]=1e15;
                if (pv<=-1.0) paramsOut[i]=-1e15;
            }
            qDebug()<<i<<": I"<<initialParams[i]<<"   V"<<paramsOut[i]<<" -> "<<pv<<"   "<<mi<<"..."<<ma<<"   ma-mi="<<ma-mi;
        }

        lmmin( paramCount, paramsOut, model->get_evalout(), &d, lmfit_evalboxtanh, &control, &status, NULL );
        for (int i=0; i<paramCount; i++) {
            const double mi=paramsMin[i];
            const double ma=paramsMax[i];
            if (fabs(mi)<RANGE_MAXVAL && fabs(ma)<RANGE_MAXVAL) {
                const double pv=tanh(paramsOut[i]);
                paramsOut[i]=mi+(pv+1.0)*(ma-mi)/2.0;
            }
        }
    }*/


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

bool QFFitAlgorithmLMFitBox::displayConfig() {
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

bool QFFitAlgorithmLMFitBox::isThreadSafe() const
{
    return true;
}

