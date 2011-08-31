#include "fitalgorithm_levmar.h"
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include "levmarconfig.h"

QFFitAlgorithmLevmar::QFFitAlgorithmLevmar() {
    setParameter("max_iterations", 3000);
    setParameter("mu", 1e-3);
    setParameter("epsilon1", 1e-17);
    setParameter("epsilon2", 1e-17);
    setParameter("epsilon3", 1e-17);
    setParameter("gradient_delta", 1e-6);
    setParameter("always_num_gradient", true);

}

void levmarfitfunc(double* p, double* x, int m, int n, void* data) {
    QFFitAlgorithm::Functor* adata=(QFFitAlgorithm::Functor*)data;
    adata->evaluate(x, p);
}

void levmarfitjac(double *p, double *j, int m, int n, void *data) {
    QFFitAlgorithm::Functor* adata=(QFFitAlgorithm::Functor*)data;
    adata->evaluateJacobian(j, p);
}

QFFitAlgorithm::FitResult QFFitAlgorithmLevmar::intFit(double* paramsOut, double* paramErrorsOut, const double* initialParams, QFFitAlgorithm::Functor* model, const double* paramsMin, const double* paramsMax) {
    QFFitAlgorithm::FitResult result;

    int paramCount=model->get_paramcount();

    double opts[LM_OPTS_SZ];
    double info[LM_INFO_SZ];

    double* pparamsMin=(double*)malloc(paramCount*sizeof(double));
    double* pparamsMax=(double*)malloc(paramCount*sizeof(double));
    memcpy(pparamsMin, paramsMin, paramCount*sizeof(double));
    memcpy(pparamsMax, paramsMax, paramCount*sizeof(double));

    /* optimization control parameters; passing to levmar NULL instead of opts reverts to defaults */
    opts[0]=getParameter("mu").toDouble();
    opts[1]=getParameter("epsilon1").toDouble();
    opts[2]=getParameter("epsilon2").toDouble();
    opts[3]=getParameter("epsilon3").toDouble();
    opts[4]=getParameter("gradient_delta").toDouble(); // relevant only if the finite difference Jacobian version is used

    bool always_num_grad=getParameter("always_num_gradient").toBool();

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

    int ret=0;
    bool numGrad=false;
    if ((!model->get_implementsJacobian())||(always_num_grad)) {
            std::cout<<"levmar: numerical gradients\n";
        ret=dlevmar_bc_dif(levmarfitfunc, paramsOut, NULL, paramCount, model->get_evalout(), pparamsMin, pparamsMax, getParameter("max_iterations").toInt(), opts, info, NULL, covar, model); // without Jacobian
        numGrad=true;
    } else {
            std::cout<<"levmar: analytical gradients\n";
        ret=dlevmar_bc_der(levmarfitfunc, levmarfitjac, paramsOut, NULL, paramCount, model->get_evalout(), pparamsMin, pparamsMax, getParameter("max_iterations").toInt(), opts, info, NULL, covar, model); // without Jacobian
    }

    free(pparamsMin);
    free(pparamsMax);

    result.addNumber("initial_error_sum", info[0]);
    result.addNumber("error_sum", info[1]);
    result.addNumber("iterations", info[5]);
    result.addNumber("function_evals", info[7]);
    result.addNumber("jacobian_evals", info[8]);
    result.addNumber("linsys_solved", info[9]);
    result.addInteger("fit_paramcount", paramCount);
    result.addBoolean("numerical_gradient", numGrad);
    result.addNumberMatrix("covariance_matrix", covar, paramCount, paramCount);

    QString reason="", reason_simple="";
    switch((int)info[6]) {
        case 1: reason=QObject::tr("stopped by small gradient J<sup>T</sup>&middot;<b>e</b>.");
                reason_simple=QObject::tr("stopped by small gradient J_T*e.");
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
    result.addString("stop_reason", reason_simple);

    printf("Levenberg-Marquardt returned in %g iter, reason %s, sumsq %g [%g]\n", info[5], reason.toStdString().c_str(), info[1], info[0]);

    result.message=QObject::tr("<b>levmar</b> returned after %1 iterations.<br>reason: %2\nSES = %3  [old_SES = %4]").arg(info[5]).arg(reason).arg(info[1]).arg(info[0]);
    result.messageSimple=QObject::tr("levmar returned after %1 iterations. reason: '%2' new_SES = %3  [old_SES = %4]").arg(info[5]).arg(reason_simple).arg(info[1]).arg(info[0]);

    for (int i=0; i<paramCount; i++) {
        paramErrorsOut[i]=sqrt(covar[i*paramCount+i]);
        //printf("  o.param %d:   (%lg +/- %lg)\n", i, paramsOut[i], paramErrorsOut[i]);
    }

    free(covar);

    result.fitOK=(ret!=LM_ERROR);
    return result;
}

bool QFFitAlgorithmLevmar::displayConfig() {
    LevmarConfigDialog* dlg=new LevmarConfigDialog(0);
    dlg->setMaxIterations(getParameter("max_iterations").toInt());
    dlg->setMu(getParameter("mu").toDouble());
    dlg->setEpsilon1(getParameter("epsilon1").toDouble());
    dlg->setEpsilon2(getParameter("epsilon2").toDouble());
    dlg->setEpsilon3(getParameter("epsilon3").toDouble());
    dlg->setGradDelta(getParameter("gradient_delta").toDouble());
    dlg->setNumGrad(getParameter("always_num_gradient").toBool());

    if (dlg->exec()==QDialog::Accepted) {
        setParameter("max_iterations", dlg->getMaxIterations());
        setParameter("mu", dlg->getMu());
        setParameter("epsilon1", dlg->getEpsilon1());
        setParameter("epsilon2", dlg->getEpsilon2());
        setParameter("epsilon3", dlg->getEpsilon3());
        setParameter("gradient_delta", dlg->getGradDelta());
        setParameter("always_num_gradient", dlg->getNumGrad());
        delete dlg;
        return true;
    } else {
        delete dlg;
        return false;
    }
}


Q_EXPORT_PLUGIN2(fitalgorithm_levmar, QFPFitAlgorithmLevmar)
