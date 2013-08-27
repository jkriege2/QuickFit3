#include "qffitalgorithmgslderivbase.h"
#include "qffitalgorithmgslderivconfigdialog.h"
#include "qffitalgorithmgsltools.h"

#include <cmath>


#include "gsl/gsl_vector.h"
#include "gsl/gsl_multimin.h"
#include "gsl/gsl_blas.h"






QFFitAlgorithmGSLDerivativeBase::QFFitAlgorithmGSLDerivativeBase(const gsl_multimin_fdfminimizer_type *T):
    QFFitAlgorithm()
{
    setParameter("stepsize", 0.1);
    setParameter("tol", 0.01);
    setParameter("gtol", 1e-4);
    setParameter("max_iterations", 200);

    this->T=T;
}

QFFitAlgorithm::FitResult QFFitAlgorithmGSLDerivativeBase::intFit(double* paramsOut, double* paramErrorsOut, const double* initialParams, QFFitAlgorithm::Functor* model, const double* paramsMin, const double* paramsMax) {
    QFFitAlgorithm::FitResult result;

    int paramCount=model->get_paramcount(); // number of parameters


    QFFItAlgorithmGSL_evalData d;
    d.model=model;
    d.paramsMin=paramsMin;
    d.paramsMax=paramsMax;
    d.pcount=paramCount;

    int iter = 0;
    int maxIter = getParameter("max_iterations").toInt();
    double gtol = getParameter("gtol").toInt();
    int status;

    ;
    gsl_multimin_fdfminimizer *s;

    // set starting value to initial parameters
    gsl_vector *x;
    x = gsl_vector_alloc(paramCount);
    for (int i=0; i<paramCount; i++) {
        gsl_vector_set (x, i, initialParams[i]);
    }

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


    for (int i=0; i<paramCount; i++) {
        const double par=gsl_vector_get(s->x, i);
        paramsOut[i]=par;
        if (par>paramsMax[i]) paramsOut[i]=paramsMax[i];
        if (par<paramsMin[i]) paramsOut[i]=paramsMin[i];
    }

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



    gsl_multimin_fdfminimizer_free (s);
    gsl_vector_free (x);

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

