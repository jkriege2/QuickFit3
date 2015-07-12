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

#include "qffitalgorithmgslsimplex.h"
#include "qffitalgorithmgslsimplexconfigdialog.h"
#include "qffitalgorithmgsltools.h"
#include "statistics_tools.h"
#include <cmath>


#include "gsl/gsl_vector.h"
#include "gsl/gsl_multimin.h"
#include "gsl/gsl_blas.h"






QFFitAlgorithmGSLSimplex::QFFitAlgorithmGSLSimplex() {
    T = gsl_multimin_fminimizer_nmsimplex2;
    setParameter("stepsize", 1);
    setParameter("max_iterations", 200);
}

QFFitAlgorithm::FitResult QFFitAlgorithmGSLSimplex::intFit(double* paramsOut, double* paramErrorsOut, const double* initialParams, QFFitAlgorithm::Functor* model, const double* paramsMin, const double* paramsMax) {
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
    int status;

    //const gsl_multimin_fdfminimizer_type *T;
    //gsl_multimin_fdfminimizer *s;

    gsl_multimin_fminimizer *s;

    // set starting value to initial parameters
    gsl_vector *x=QFFitAlgorithmGSL_transformParams(initialParams, paramCount, paramsMin, paramsMax);

    //gsl_multimin_function_fdf my_func;
    gsl_multimin_function my_func;
    my_func.n = paramCount;
    my_func.f = QFFitAlgorithmGSL_f;
    //my_func.df = QFFitAlgorithmGSL_df;
    //my_func.fdf = QFFitAlgorithmGSL_fdf;
    my_func.params = &d;


    // initialize minimizer

    //s = gsl_multimin_fdfminimizer_alloc (T, paramCount);
    s = gsl_multimin_fminimizer_alloc (T, paramCount);

    /* Set initial step sizes to 1 */
    gsl_vector *ss = gsl_vector_alloc(paramCount);
    gsl_vector_set_all (ss, getParameter("stepsize").toDouble());


    //gsl_multimin_fdfminimizer_set(s, &my_func, x, getParameter("stepsize").toDouble(), getParameter("tol").toDouble());
    gsl_multimin_fminimizer_set(s, &my_func, x, ss);

    do {
        iter++;
        //status = gsl_multimin_fdfminimizer_iterate (s);
        status = gsl_multimin_fminimizer_iterate (s);
        //qDebug()<<"it "<<iter<<": "<<arrayToString(gsl_vector_ptr(s->x, 0), paramCount);
        //qDebug()<<"       f = "<<s->fval;
        //qDebug()<<"       status = "<<status;

        if (status) break;

        //status = gsl_multimin_test_gradient (s->gradient, 1e-3);
        double size = gsl_multimin_fminimizer_size (s);
        status = gsl_multimin_test_size (size, 1e-2);

        //if (status == GSL_SUCCESS) qDebug()<<"       Minimum found !!!";



    } while (status == GSL_CONTINUE && iter < maxIter);


    /*for (int i=0; i<paramCount; i++) {
        const double par=gsl_vector_get(s->x, i);
        paramsOut[i]=par;
        if (par>paramsMax[i]) paramsOut[i]=paramsMax[i];
        if (par<paramsMin[i]) paramsOut[i]=paramsMin[i];
    }*/
    QFFitAlgorithmGSL_backTransformParams(paramsOut, paramCount, s->x, paramsMin, paramsMax);

    QVector<double> J(model->get_evalout()*model->get_paramcount());
    QVector<double> COV(model->get_paramcount()*model->get_paramcount());
    model->evaluateJacobian(J.data(), paramsOut);
    double chi2=s->fval;
    if (QFFitAlgorithm::functorHasWeights(model) && !QFFitAlgorithm::functorAllWeightsOne(model)) statisticsGetFitProblemCovMatrix(COV.data(), J.data(), model->get_evalout(), model->get_paramcount());
    else statisticsGetFitProblemVarCovMatrix(COV.data(), J.data(), model->get_evalout(), model->get_paramcount(), chi2);

    result.addNumberMatrix("covariance_matrix", COV.data(), model->get_paramcount(), model->get_paramcount());

    for (int i=0; i<model->get_paramcount(); i++) {
        paramErrorsOut[i]=statisticsGetFitProblemParamErrors(i, COV.data(), model->get_paramcount());
    }


    result.addNumber("error_sum", s->fval);
    result.addNumber("iterations", iter);

    result.fitOK=false;
    result.message=QObject::tr("error during optimization");
    result.messageSimple=result.message;
    if (status == GSL_SUCCESS) {
        result.fitOK=true;
        result.message=QObject::tr("success after %1 iterations").arg(iter);
        result.messageSimple=result.message;
    } else if (status == GSL_ENOPROG) {
        result.fitOK=true;
        result.message=QObject::tr("no more optimization progress after %1 iterations").arg(iter);
        result.messageSimple=result.message;
    }



    //gsl_multimin_fdfminimizer_free (s);
    gsl_multimin_fminimizer_free (s);
    gsl_vector_free (x);
    gsl_vector_free(d.params);
    gsl_vector_free(d.params_ast);
    gsl_vector_free(d.out);
    gsl_vector_free(d.out_ast);

    return result;
}

QFFitAlgorithm::FitResult QFFitAlgorithmGSLSimplex::intMinimize(double *paramsOut, const double *initialParams, QFFitAlgorithm::FunctorForFMin *model, const double *paramsMin, const double *paramsMax)
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
    int status;

    //const gsl_multimin_fdfminimizer_type *T;
    //gsl_multimin_fdfminimizer *s;

    gsl_multimin_fminimizer *s;

    // set starting value to initial parameters
    gsl_vector *x=QFFitAlgorithmGSL_transformParams(initialParams, paramCount, paramsMin, paramsMax);

    //gsl_multimin_function_fdf my_func;
    gsl_multimin_function my_func;
    my_func.n = paramCount;
    my_func.f = QFFitAlgorithmGSLFmin_f;
    //my_func.df = QFFitAlgorithmGSL_df;
    //my_func.fdf = QFFitAlgorithmGSL_fdf;
    my_func.params = &d;


    // initialize minimizer

    //s = gsl_multimin_fdfminimizer_alloc (T, paramCount);
    s = gsl_multimin_fminimizer_alloc (T, paramCount);

    /* Set initial step sizes to 1 */
    gsl_vector *ss = gsl_vector_alloc(paramCount);
    gsl_vector_set_all (ss, getParameter("stepsize").toDouble());


    //gsl_multimin_fdfminimizer_set(s, &my_func, x, getParameter("stepsize").toDouble(), getParameter("tol").toDouble());
    gsl_multimin_fminimizer_set(s, &my_func, x, ss);

    do {
        iter++;
        //status = gsl_multimin_fdfminimizer_iterate (s);
        status = gsl_multimin_fminimizer_iterate (s);
        //qDebug()<<"it "<<iter<<": "<<arrayToString(gsl_vector_ptr(s->x, 0), paramCount);
        //qDebug()<<"       f = "<<s->fval;
        //qDebug()<<"       status = "<<status;

        if (status) break;

        //status = gsl_multimin_test_gradient (s->gradient, 1e-3);
        double size = gsl_multimin_fminimizer_size (s);
        status = gsl_multimin_test_size (size, 1e-2);

        //if (status == GSL_SUCCESS) qDebug()<<"       Minimum found !!!";



    } while (status == GSL_CONTINUE && iter < maxIter);


    /*for (int i=0; i<paramCount; i++) {
        const double par=gsl_vector_get(s->x, i);
        paramsOut[i]=par;
        if (par>paramsMax[i]) paramsOut[i]=paramsMax[i];
        if (par<paramsMin[i]) paramsOut[i]=paramsMin[i];
    }*/
    QFFitAlgorithmGSL_backTransformParams(paramsOut, paramCount, s->x, paramsMin, paramsMax);

    result.addNumber("final_value", s->fval);
    result.addNumber("iterations", iter);

    result.fitOK=false;
    result.message=QObject::tr("error during optimization");
    result.messageSimple=result.message;
    if (status == GSL_SUCCESS) {
        result.fitOK=true;
        result.message=QObject::tr("success after %1 iterations").arg(iter);
        result.messageSimple=result.message;
    } else if (status == GSL_ENOPROG) {
        result.fitOK=true;
        result.message=QObject::tr("no more optimization progress after %1 iterations").arg(iter);
        result.messageSimple=result.message;
    }



    //gsl_multimin_fdfminimizer_free (s);
    gsl_multimin_fminimizer_free (s);
    gsl_vector_free (x);
    gsl_vector_free(d.params);
    gsl_vector_free(d.params_ast);
    //gsl_vector_free(d.out);
    //gsl_vector_free(d.out_ast);

    return result;
}

bool QFFitAlgorithmGSLSimplex::displayConfig() {
    QFFitAlgorithmGSLSimplexConfigDialog* dlg=new QFFitAlgorithmGSLSimplexConfigDialog(0);

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

