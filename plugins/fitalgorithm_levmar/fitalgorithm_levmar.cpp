#include "fitalgorithm_levmar.h"
#include <cmath>
#include <cstdlib>
#include <cstdio>

QFFitAlgorithmLevmar::QFFitAlgorithmLevmar() {
    setParameter("max_iterations", 3000);
    setParameter("mu", 1e-3);
    setParameter("epsilon1", 1e-17);
    setParameter("epsilon2", 1e-17);
    setParameter("epsilon3", 1e-17);
    setParameter("gradient_delta", 1e-6);

}

void levmarfitfunc(double* p, double* x, int m, int n, void* data)
{
    QFFitAlgorithm::Functor* adata=(QFFitAlgorithm::Functor*)data;
    adata->evaluate(x, p);
}

QFFitAlgorithm::FitResult QFFitAlgorithmLevmar::intFit(double* paramsOut, double* paramErrorsOut, double* initialParams, QFFitAlgorithm::Functor* model, double* paramsMin, double* paramsMax) {
    QFFitAlgorithm::FitResult result;

    int paramCount=model->get_paramcount();

    double opts[LM_OPTS_SZ];
    double info[LM_INFO_SZ];

    /* optimization control parameters; passing to levmar NULL instead of opts reverts to defaults */
    opts[0]=getParameter("mu").toDouble();
    opts[1]=getParameter("epsilon1").toDouble();
    opts[2]=getParameter("epsilon2").toDouble();
    opts[3]=getParameter("epsilon3").toDouble();
    opts[4]=getParameter("gradient_delta").toDouble(); // relevant only if the finite difference Jacobian version is used

    // allocate memory for covariance matrix
    // the matrix will be returned by levmar calls, if memory is provided!
    double* covar=(double*)calloc(paramCount*paramCount, sizeof(double));

    // copy initial parameters to output parameters
    for (int i=0; i<paramCount; i++) {
        paramsOut[i]=initialParams[i];
        //printf("  i.param %d:   %lg\n", i, paramsOut[i]);
    }

    /*
       The model already evaluates to f_i = ( y_i - m(x_i, p) ) / sigma_i, so we set the parameter vector to all 0
       then levmar evaluates 0-f_i = f_i and optimizes that.
    */
    double* data=(double*)calloc(model->get_evalout(), sizeof(double));

    int ret=0;
    ret=dlevmar_bc_dif(levmarfitfunc, paramsOut, data, paramCount, model->get_evalout(), paramsMin, paramsMax, getParameter("max_iterations").toInt(), opts, info, NULL, covar, model); // without Jacobian


    result.params["initial_error_sum"]=info[0];
    result.params["error_sum"]=info[1];
    result.params["iterations"]=info[5];
    result.params["function_evals"]=info[7];
    result.params["jacobian_evals"]=info[8];
    result.params["linsys_solved"]=info[9];
    result.params["fit_paramcount"]=paramCount;

    QString reason="", reason_simple="";
    switch((int)info[6]) {
        case 1: reason=QObject::tr("stopped by small gradient J<sup>T</sup>&middot;<b>e</b>.");
                reason_simple=QObject::tr("stopped by small gradient J<sup>T</sup>&middot;<b>e</b>.");
                break;
        case 2: reason=QObject::tr("stopped by small &delta;<sub><b>p</b></sub>.");
                reason_simple=QObject::tr("stopped by small delta_p.");
                break;
        case 3: reason=QObject::tr("stopped by maximum iterations.");
                reason_simple=QObject::tr("stopped by maximum iterations.");
                break;
        case 4: reason=QObject::tr("singular matrix. Restart from current p with increased &mu;.");
                reason_simple=QObject::tr("singular matrix. Restart from current p with increased mu.");
                break;
        case 5: reason=QObject::tr("no further error reduction is possible. Restart with increased &mu;.");
                reason_simple=QObject::tr("no further error reduction is possible. Restart with increased mu.");
                break;
        case 6: reason=QObject::tr("stopped by small ||<i>e</i>||<sub>2</sub>.");
                reason_simple=QObject::tr("stopped by small error norm ||e||_2.");
                break;
        case 7: reason=QObject::tr("stopped by invalid (i.e. Not a Number (NaN) or Infinity) model function values. This is a user error, contact the model implementer!");
                reason_simple=QObject::tr("stopped by invalid (i.e. Not a Number (NaN) or Infinity) model function values. This is a user error, contact the model implementer!");
                break;
    };
    result.params["stop_reason"]=reason_simple;

    printf("Levenberg-Marquardt returned in %g iter, reason %s, sumsq %g [%g]\n", info[5], reason.toStdString().c_str(), info[1], info[0]);

    result.message=QObject::tr("<b>levmar</b> returned after %1 iterations.<br>reason: %2\nSES = %3  [old_SES = %4]").arg(info[5]).arg(reason).arg(info[1]).arg(info[0]);
    result.messageSimple=QObject::tr("levmar returned after %1 iterations.\nreason: %2\nSES = %3  [old_SES = %4]").arg(info[5]).arg(reason).arg(info[1]).arg(info[0]);

    for (unsigned int i=0; i<paramCount; i++) {
        paramErrorsOut[i]=sqrt(covar[i*paramCount+i]);
        //printf("  o.param %d:   (%lg +/- %lg)\n", i, paramsOut[i], paramErrorsOut[i]);
    }

    free(covar);
    free(data);

    result.fitOK=(ret!=LM_ERROR);
    return result;
}




Q_EXPORT_PLUGIN2(fitalgorithm_levmar, QFPFitAlgorithmLevmar)
