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

#include "qffitalgorithmlmfitbox.h"
#include "qffitalgorithmlmfitconfig.h"
#include <cmath>
#include "lmmin.h"
#include "statistics_tools.h"
#include <typeinfo>
#define RANGE_MAXVAL (DBL_MAX/10.0)

struct QFFItAlgorithmGSL_evalData {
    QFFitAlgorithm::Functor* model;
    const double* paramsMin;
    const double* paramsMax;
    double* p;
    int pcount;
};

void lmfit_evalboxlimits(const double *par, int /*m_dat*/, const void *data, double *fvec, int */*info*/) {
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
    //qDebug()<<"QFFitAlgorithmLMFitBox::intFit  1";

    int paramCount=model->get_paramcount(); // number of parameters
    memcpy(paramsOut, initialParams, paramCount*sizeof(double));

    //qDebug()<<"QFFitAlgorithmLMFitBox::intFit  2";
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
    //qDebug()<<"QFFitAlgorithmLMFitBox::intFit  3";

    QFFItAlgorithmGSL_evalData d;
    d.model=model;
    d.paramsMin=paramsMin;
    d.paramsMax=paramsMax;
    d.pcount=paramCount;
    d.p=(double*)qfMalloc(paramCount*sizeof(double));
    //qDebug()<<"QFFitAlgorithmLMFitBox::intFit  4";

    bool transformParams=paramsMin&&paramsMax;
    //qDebug()<<"QFFitAlgorithmLMFitBox::intFit  5";

    lmmin( paramCount, paramsOut, model->get_evalout(), &d, lmfit_evalboxlimits, &control, &status );
    //qDebug()<<"QFFitAlgorithmLMFitBox::intFit  6";
    if (paramsMin && paramsMax) {
        bool atbound=false;
        for (int i=0; i<paramCount; i++) {
            const double mi=paramsMin[i];
            const double ma=paramsMax[i];
            if (qFuzzyCompare(mi,paramsOut[i])) {
                atbound=true;
                paramsOut[i]=1.05*mi;
            } else if (qFuzzyCompare(ma,paramsOut[i])) {
                atbound=true;
                paramsOut[i]=0.95*ma;
            } else paramsOut[i]=qBound(mi, paramsOut[i], ma);
        }
        if (atbound) {
            //qDebug()<<"QFFitAlgorithmLMFitBox::intFit  6.2";
            lmmin( paramCount, paramsOut, model->get_evalout(), &d, lmfit_evalboxlimits, &control, &status );
            //qDebug()<<"QFFitAlgorithmLMFitBox::intFit  6.3";
            for (int i=0; i<paramCount; i++) {
                const double mi=paramsMin[i];
                const double ma=paramsMax[i];
                paramsOut[i]=qBound(mi, paramsOut[i], ma);
            }
        }

    }
    //qDebug()<<"QFFitAlgorithmLMFitBox::intFit  7";

    double chi2=status.fnorm;
    if (true) {
        const long long nout=model->get_evalout();
        const long long np=model->get_paramcount();
        double* J=(double*)qfCalloc(nout*np*2*sizeof(double));
        double* COV=(double*)qfCalloc(np*np*2*sizeof(double));

//        qDebug()<<"QFFitAlgorithmLMFitBox::intFit  7: test J";
//        for (long long i=nout*np; i<nout*np*2; i++) {
//            if (J[i]!=0) qDebug()<<"!!! J["<<i<<"] != 0 (="<<J[i]<<") !!!";
//        }
//        qDebug()<<"QFFitAlgorithmLMFitBox::intFit  7: test COV";
//        for (long long i=np*np; i<np*np*2; i++) {
//            if (COV[i]!=0) qDebug()<<"!!! COV["<<i<<"] != 0 !!!";
//        }

//        qDebug()<<"QFFitAlgorithmLMFitBox::intFit  8";
//        qDebug()<<"  calling evalJac: J.size="<<nout*np<<"  p.size="<<np<<"   nout="<<nout<<"   J="<<J<<"   COV="<<COV<<"   model="<<typeid(model).name();
        model->evaluateJacobian(J, paramsOut);
//        qDebug()<<"QFFitAlgorithmLMFitBox::intFit  8: test J";
//        for (long long i=nout*np; i<nout*np*2; i++) {
//            if (J[i]!=0) qDebug()<<"!!! J["<<i<<"] != 0 (="<<J[i]<<") !!!";
//        }

//        qDebug()<<"QFFitAlgorithmLMFitBox::intFit  9";
//        qDebug()<<"QFFitAlgorithmLMFitBox::intFit  10";
//        bool b=model->isWeightedLSQ();
//        qDebug()<<"QFFitAlgorithmLMFitBox::intFit  10-isWLSQ"<<model->isWeightedLSQ();
//        b=model->areAllWeightsOne();
//        qDebug()<<"QFFitAlgorithmLMFitBox::intFit  10-allOne"<<model->areAllWeightsOne();
//        qDebug()<<"QFFitAlgorithmLMFitBox::intFit  10-allOne: test J";
//        for (long long i=nout*np; i<nout*np*2; i++) {
//            if (J[i]!=0) qDebug()<<"!!! J["<<i<<"] != 0 (="<<J[i]<<") !!!";
//        }
//        qDebug()<<"QFFitAlgorithmLMFitBox::intFit  10-allOne: test COV";
//        for (long long i=np*np; i<np*np*2; i++) {
//            if (COV[i]!=0) qDebug()<<"!!! COV["<<i<<"] != 0 !!!";
//        }

        if (model->isWeightedLSQ() && !model->areAllWeightsOne()) statisticsGetFitProblemCovMatrix(COV, J, nout, np);
        else statisticsGetFitProblemVarCovMatrix(COV, J/*.constData()*/, nout, np, chi2);
//        qDebug()<<"QFFitAlgorithmLMFitBox::intFit  10: test J";
//        for (long long i=nout*np; i<nout*np*2; i++) {
//            if (J[i]!=0) qDebug()<<"!!! J["<<i<<"] != 0 (="<<J[i]<<") !!!";
//        }
//        qDebug()<<"QFFitAlgorithmLMFitBox::intFit  10: test COV";
//        for (long long i=np*np; i<np*np*2; i++) {
//            if (COV[i]!=0) qDebug()<<"!!! COV["<<i<<"] != 0 !!!";
//        }
//        qDebug()<<"QFFitAlgorithmLMFitBox::intFit  11";

        result.addNumberMatrix("covariance_matrix", COV, np,np);
//        qDebug()<<"QFFitAlgorithmLMFitBox::intFit  11: test COV";
//        for (long long i=np*np; i<np*np*2; i++) {
//            if (COV[i]!=0) qDebug()<<"!!! COV["<<i<<"] != 0 !!!";
//        }

        for (long long i=0; i<model->get_paramcount(); i++) {
//            qDebug()<<"QFFitAlgorithmLMFitBox::intFit  12."<<i;
            paramErrorsOut[i]=statisticsGetFitProblemParamErrors(i, COV, model->get_paramcount());
        }
//        qDebug()<<"QFFitAlgorithmLMFitBox::intFit  12: test COV";
//        for (long long i=np*np; i<np*np*2; i++) {
//            if (COV[i]!=0) qDebug()<<"!!! COV["<<i<<"] != 0 !!!";
//        }

//        qDebug()<<"QFFitAlgorithmLMFitBox::intFit  12-end1";
        qfFree(COV);
//        qDebug()<<"QFFitAlgorithmLMFitBox::intFit  12-end2";
        qfFree(J);
    }
//    qDebug()<<"QFFitAlgorithmLMFitBox::intFit  13";

    result.addNumber("error_sum", chi2);
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
//    qDebug()<<"QFFitAlgorithmLMFitBox::intFit  14";

    if (d.p) qfFree(d.p);
//    qDebug()<<"QFFitAlgorithmLMFitBox::intFit  15";

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


