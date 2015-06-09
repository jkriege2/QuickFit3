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

#include "qffitalgorithmgslderivbase.h"
#include "qffitalgorithmgslderivconfigdialog.h"
#include "qffitalgorithmgsltools.h"
#include "statistics_tools.h"
#include <cmath>


#include "gsl/gsl_vector.h"
#include "gsl/gsl_multimin.h"
#include "gsl/gsl_blas.h"






QFFitAlgorithmGSLDerivativeBase::QFFitAlgorithmGSLDerivativeBase(const gsl_multimin_fdfminimizer_type *T):
    QFFitAlgorithm()
{
    setParameter("stepsize", 50.0*DBL_EPSILON);
    setParameter("tol", 1e-4);
    setParameter("gtol", 1e-4);
    setParameter("max_iterations", 200);

    this->T=T;
}

QFFitAlgorithm::FitResult QFFitAlgorithmGSLDerivativeBase::intFit(double* paramsOut, double* paramErrorsOut, const double* initialParams, QFFitAlgorithm::Functor* model, const double* paramsMin, const double* paramsMax) {
    QFFitAlgorithm::FitResult result;

    int paramCount=model->get_paramcount(); // number of parameters

    if (paramCount<=0) {
        result.fitOK=false;
        result.message=QObject::tr("no parameters to optimize");
        result.messageSimple=result.message;
        return result;
    }

    QFFItAlgorithmGSL_evalData d;
    d.model=model;
    d.paramsMin=paramsMin;
    d.paramsMax=paramsMax;
    d.pcount=paramCount;
    d.out=gsl_vector_alloc(model->get_evalout());
    d.out_ast=gsl_vector_alloc(model->get_evalout());
    d.params=gsl_vector_alloc(paramCount);
    d.params_ast=gsl_vector_alloc(paramCount);



    int iter = 0;
    int maxIter = getParameter("max_iterations").toInt();
    double gtol = getParameter("gtol").toDouble();
    int status;

    ;
    gsl_multimin_fdfminimizer *s;

    // set starting value to initial parameters
    gsl_vector *x=QFFitAlgorithmGSL_transformParams(initialParams, paramCount, paramsMin, paramsMax);

    gsl_multimin_function_fdf my_func;
    my_func.n = paramCount;
    my_func.f = QFFitAlgorithmGSL_f;
    my_func.df = QFFitAlgorithmGSL_df;
    my_func.fdf = QFFitAlgorithmGSL_fdf;
    my_func.params = &d;


    // initialize minimizer
    s = gsl_multimin_fdfminimizer_alloc (T, paramCount);


    gsl_multimin_fdfminimizer_set(s, &my_func, x, getParameter("stepsize").toDouble(), getParameter("tol").toDouble());

    do {
        iter++;
        status = gsl_multimin_fdfminimizer_iterate(s);
        //status = gsl_multimin_fminimizer_iterate (s);
        //qDebug()<<"it "<<iter<<": "<<arrayToString(gsl_vector_ptr(s->x, 0), paramCount);
        //qDebug()<<"       f(x) = "<<s->f;
        //qDebug()<<"       dx = "<<arrayToString(gsl_vector_ptr(s->dx, 0), paramCount);
        //qDebug()<<"       grad = "<<arrayToString(gsl_vector_ptr(s->gradient, 0), paramCount);
        //qDebug()<<"       status = "<<status;

        if (status) break;

        status = gsl_multimin_test_gradient (s->gradient, gtol);

        //if (status == GSL_SUCCESS) qDebug()<<"       Minimum found !!!";



    } while (status == GSL_CONTINUE && iter < maxIter);


    /*for (int i=0; i<paramCount; i++) {
        const double par=gsl_vector_get(s->x, i);
        paramsOut[i]=par;
        if (par>paramsMax[i]) paramsOut[i]=paramsMax[i];
        if (par<paramsMin[i]) paramsOut[i]=paramsMin[i];
    }*/
    QFFitAlgorithmGSL_backTransformParams(paramsOut, paramCount, s->x, paramsMin, paramsMax);

    result.addNumber("error_sum", s->f);
    result.addNumber("iterations", iter);

    result.fitOK=false;
    result.message=QObject::tr("error %1 during optimization after %2 iterations").arg(status).arg(iter);
    result.messageSimple=result.message;

    if (status == GSL_SUCCESS) {
        result.fitOK=true;
        result.message=QObject::tr("success after %1 iterations").arg(iter);
        result.messageSimple=result.message;
    } else if (status == GSL_ENOPROG) {
        result.fitOK=true;
        result.message=QObject::tr("no more optimization progress after %1 iterations").arg(iter);
        result.messageSimple=result.message;
    } else if (iter>=maxIter) {
        result.fitOK=false;
        result.message=QObject::tr("maximum number of iterations (%1) reached").arg(maxIter);
        result.messageSimple=result.message;
    }



    QVector<double> J(model->get_evalout()*model->get_paramcount());
    QVector<double> COV(model->get_paramcount()*model->get_paramcount());
    model->evaluateJacobianNum(J.data(), paramsOut);
    double chi2=s->f;
    if (QFFitAlgorithm::functorHasWeights(model)) statisticsGetFitProblemCovMatrix(COV.data(), J.data(), model->get_evalout(), model->get_paramcount());
    else statisticsGetFitProblemVarCovMatrix(COV.data(), J.data(), model->get_evalout(), model->get_paramcount(), chi2);

    for (int i=0; i<model->get_paramcount(); i++) {
        paramErrorsOut[i]=statisticsGetFitProblemParamErrors(i, COV.data(), model->get_paramcount());
    }

    gsl_multimin_fdfminimizer_free (s);
    gsl_vector_free (x);
    gsl_vector_free(d.params);
    gsl_vector_free(d.params_ast);
    gsl_vector_free(d.out);
    gsl_vector_free(d.out_ast);

    return result;
}

QFFitAlgorithm::FitResult QFFitAlgorithmGSLDerivativeBase::intMinimize(double *paramsOut, const double *initialParams, QFFitAlgorithm::FunctorForFMin *model, const double *paramsMin, const double *paramsMax)
{
    QFFitAlgorithm::FitResult result;

    int paramCount=model->get_paramcount(); // number of parameters

    if (paramCount<=0) {
        result.fitOK=false;
        result.message=QObject::tr("no parameters to optimize");
        result.messageSimple=result.message;
        return result;
    }

    QFFItAlgorithmGSLFMin_evalData d;
    d.model=model;
    d.paramsMin=paramsMin;
    d.paramsMax=paramsMax;
    d.pcount=paramCount;
    //d.out=gsl_vector_alloc(model->get_evalout());
    //d.out_ast=gsl_vector_alloc(model->get_evalout());
    d.params=gsl_vector_alloc(paramCount);
    d.params_ast=gsl_vector_alloc(paramCount);



    int iter = 0;
    int maxIter = getParameter("max_iterations").toInt();
    double gtol = getParameter("gtol").toDouble();
    int status;

    ;
    gsl_multimin_fdfminimizer *s;

    // set starting value to initial parameters
    gsl_vector *x=QFFitAlgorithmGSL_transformParams(initialParams, paramCount, paramsMin, paramsMax);

    gsl_multimin_function_fdf my_func;
    my_func.n = paramCount;
    my_func.f = QFFitAlgorithmGSLFmin_f;
    my_func.df = QFFitAlgorithmGSLFmin_df;
    my_func.fdf = QFFitAlgorithmGSLFmin_fdf;
    my_func.params = &d;


    // initialize minimizer
    s = gsl_multimin_fdfminimizer_alloc (T, paramCount);


    gsl_multimin_fdfminimizer_set(s, &my_func, x, getParameter("stepsize").toDouble(), getParameter("tol").toDouble());

    do {
        iter++;
        status = gsl_multimin_fdfminimizer_iterate(s);
        //status = gsl_multimin_fminimizer_iterate (s);
        //qDebug()<<"it "<<iter<<": "<<arrayToString(gsl_vector_ptr(s->x, 0), paramCount);
        //qDebug()<<"       f(x) = "<<s->f;
        //qDebug()<<"       dx = "<<arrayToString(gsl_vector_ptr(s->dx, 0), paramCount);
        //qDebug()<<"       grad = "<<arrayToString(gsl_vector_ptr(s->gradient, 0), paramCount);
        //qDebug()<<"       status = "<<status;

        if (status) break;

        status = gsl_multimin_test_gradient (s->gradient, gtol);

        //if (status == GSL_SUCCESS) qDebug()<<"       Minimum found !!!";



    } while (status == GSL_CONTINUE && iter < maxIter);


    /*for (int i=0; i<paramCount; i++) {
        const double par=gsl_vector_get(s->x, i);
        paramsOut[i]=par;
        if (par>paramsMax[i]) paramsOut[i]=paramsMax[i];
        if (par<paramsMin[i]) paramsOut[i]=paramsMin[i];
    }*/
    QFFitAlgorithmGSL_backTransformParams(paramsOut, paramCount, s->x, paramsMin, paramsMax);

    result.addNumber("final_value", s->f);
    result.addNumber("iterations", iter);

    result.fitOK=false;
    result.message=QObject::tr("error %1 during optimization after %2 iterations").arg(status).arg(iter);
    result.messageSimple=result.message;

    if (status == GSL_SUCCESS) {
        result.fitOK=true;
        result.message=QObject::tr("success after %1 iterations").arg(iter);
        result.messageSimple=result.message;
    } else if (status == GSL_ENOPROG) {
        result.fitOK=true;
        result.message=QObject::tr("no more optimization progress after %1 iterations").arg(iter);
        result.messageSimple=result.message;
    } else if (iter>=maxIter) {
        result.fitOK=false;
        result.message=QObject::tr("maximum number of iterations (%1) reached").arg(maxIter);
        result.messageSimple=result.message;
    }



    gsl_multimin_fdfminimizer_free (s);
    gsl_vector_free (x);
    gsl_vector_free(d.params);
    gsl_vector_free(d.params_ast);
    //gsl_vector_free(d.out);
    //gsl_vector_free(d.out_ast);

    return result;
}

bool QFFitAlgorithmGSLDerivativeBase::displayConfig() {
    QFFitAlgorithmGSLDerivativeBaseConfigDialog* dlg=new QFFitAlgorithmGSLDerivativeBaseConfigDialog(0);

    dlg->setWindowTitle(QObject::tr("%1 Parameters ...").arg(shortName()));
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

