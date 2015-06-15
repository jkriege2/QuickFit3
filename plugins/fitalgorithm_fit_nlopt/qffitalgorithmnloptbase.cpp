#include "qffitalgorithmnloptbase.h"

#include "qffitalgorithmnloptnonderivconfigdialog.h"
#include <cmath>
#include <nlopt.h>
#include "qfmathtools.h"
#include "statistics_tools.h"



QFFitAlgorithmNLOptBASE::QFFitAlgorithmNLOptBASE(nlopt_algorithm nlopt_alg) {
    this->nlopt_alg=nlopt_alg;


    setParameter("max_time", 15);
    setParameter("xtol", 1e-7);
    setParameter("ftol", 1e-15);
    setParameter("max_evals", 1e6);
    setParameter("stepsize", 50.0*DBL_EPSILON);
}

QString QFFitAlgorithmNLOptBASE::name() const
{
    return QObject::tr("NLOpt: %1").arg(nlopt_algorithm_name(nlopt_alg));
}

QFFitAlgorithm::FitResult QFFitAlgorithmNLOptBASE::intFit(double* paramsOut, double* paramErrorsOut, const double* initialParams, QFFitAlgorithm::Functor* model, const double* paramsMin, const double* paramsMax) {
    QFFitAlgorithm::FitResult result;

    int paramCount=model->get_paramcount(); // number of parameters

    if (paramCount<=0) {
        result.fitOK=false;
        result.message=QObject::tr("no parameters to optimize");
        result.messageSimple=result.message;
        return result;
    }

    QFFItAlgorithmNLOpt_evalData d;
    d.model=model;
    nlopt_opt opt, core_solver;

    bool needslocal=( (nlopt_alg==NLOPT_AUGLAG) || (nlopt_alg==NLOPT_AUGLAG_EQ) || (nlopt_alg==NLOPT_G_MLSL) || (nlopt_alg==NLOPT_G_MLSL_LDS) );

    if (needslocal) {
        core_solver = nlopt_create(NLOPT_LD_LBFGS, paramCount); /* algorithm and dimensionality */
        nlopt_set_lower_bounds(core_solver, paramsMin);
        nlopt_set_upper_bounds(core_solver, paramsMax);
        nlopt_set_min_objective(core_solver, QFFitAlgorithmNLOpt_f, &d);
        nlopt_set_xtol_rel(core_solver, getParameter("xtol").toDouble());
        nlopt_set_ftol_rel(core_solver, getParameter("ftol").toDouble());
        nlopt_set_maxeval(core_solver, getParameter("max_evals").toInt());
        nlopt_set_maxtime(core_solver, getParameter("max_time").toDouble());
        //nlopt_set_initial_step1(core_solver, getParameter("stepsize").toDouble());
        //nlopt_set_initial_step(core_solver, NULL);//getParameter("stepsize").toDouble());
    }




    opt = nlopt_create(nlopt_alg, paramCount); /* algorithm and dimensionality */
    nlopt_set_lower_bounds(opt, paramsMin);
    nlopt_set_upper_bounds(opt, paramsMax);
    nlopt_set_min_objective(opt, QFFitAlgorithmNLOpt_f, &d);
    nlopt_set_xtol_rel(opt, getParameter("xtol").toDouble());
    nlopt_set_ftol_rel(opt, getParameter("ftol").toDouble());
    nlopt_set_maxeval(opt, getParameter("max_evals").toInt());
    nlopt_set_maxtime(opt, getParameter("max_time").toDouble());
    nlopt_set_initial_step1(opt, getParameter("stepsize").toDouble());
    if (needslocal) nlopt_set_local_optimizer(opt, core_solver);


    double minf=0; /* the minimum objective value, upon return */

    for (int i=0; i<paramCount; i++) {
        paramsOut[i]=initialParams[i];
        if (paramErrorsOut) paramErrorsOut[i]=0;
    }

    nlopt_result algres=nlopt_optimize(opt, paramsOut, &minf);
    bool ok=(algres >= 0);





    result.addNumber("error_sum", minf);
    result.addString("nlopt_fit_algorihtm", nlopt_algorithm_name(nlopt_alg));


    QVector<double> J(model->get_evalout()*model->get_paramcount());
    QVector<double> COV(model->get_paramcount()*model->get_paramcount());
    model->evaluateJacobianNum(J.data(), paramsOut);
    double chi2=minf;
    if (QFFitAlgorithm::functorHasWeights(model)) statisticsGetFitProblemCovMatrix(COV.data(), J.data(), model->get_evalout(), model->get_paramcount());
    else statisticsGetFitProblemVarCovMatrix(COV.data(), J.data(), model->get_evalout(), model->get_paramcount(), chi2);

    for (int i=0; i<model->get_paramcount(); i++) {
        paramErrorsOut[i]=statisticsGetFitProblemParamErrors(i, COV.data(), model->get_paramcount());
    }

    result.fitOK=ok;
    result.message=QObject::tr("error %1 during optimization ").arg(int(algres));
    result.messageSimple=result.message;

    if (algres == NLOPT_SUCCESS) {
        result.fitOK=true;
        result.message=QObject::tr("success");
        result.messageSimple=result.message;
    } else if (algres == NLOPT_FAILURE) {
        result.fitOK=false;
        result.message=QObject::tr("generic failure");
        result.messageSimple=result.message;
    } else if (algres == NLOPT_INVALID_ARGS) {
        result.fitOK=false;
        result.message=QObject::tr("invalid arguments");
        result.messageSimple=result.message;
    } else if (algres == NLOPT_OUT_OF_MEMORY) {
        result.fitOK=false;
        result.message=QObject::tr("out of memory");
        result.messageSimple=result.message;
    } else if (algres == NLOPT_ROUNDOFF_LIMITED) {
        result.fitOK=false;
        result.message=QObject::tr("Halted because roundoff errors limited progress");
        result.messageSimple=result.message;
    } else if (algres == NLOPT_FORCED_STOP) {
        result.fitOK=false;
        result.message=QObject::tr("user canceled");
        result.messageSimple=result.message;
    } else if (algres == NLOPT_STOPVAL_REACHED) {
        result.fitOK=true;
        result.message=QObject::tr("NLOPT_STOPVAL_REACHED");
        result.messageSimple=result.message;
    } else if (algres == NLOPT_FTOL_REACHED) {
        result.fitOK=true;
        result.message=QObject::tr("ftol reached");
        result.messageSimple=result.message;
    } else if (algres == NLOPT_XTOL_REACHED) {
        result.fitOK=true;
        result.message=QObject::tr("xtol reached");
        result.messageSimple=result.message;
    } else if (algres == NLOPT_MAXEVAL_REACHED) {
        result.fitOK=true;
        result.message=QObject::tr("max_eval reached");
        result.messageSimple=result.message;
    } else if (algres == NLOPT_MAXTIME_REACHED) {
        result.fitOK=true;
        result.message=QObject::tr("max_time reached");
        result.messageSimple=result.message;
    }



    nlopt_destroy(opt);
    if (needslocal) nlopt_destroy(core_solver);


    return result;
}

QFFitAlgorithm::FitResult QFFitAlgorithmNLOptBASE::intMinimize(double *paramsOut, const double *initialParams, QFFitAlgorithm::FunctorForFMin *model, const double *paramsMin, const double *paramsMax)
{
    QFFitAlgorithm::FitResult result;

    int paramCount=model->get_paramcount(); // number of parameters

    if (paramCount<=0) {
        result.fitOK=false;
        result.message=QObject::tr("no parameters to optimize");
        result.messageSimple=result.message;
        return result;
    }

    QFFItAlgorithmFMinNLOpt_evalData d;
    d.model=model;
    nlopt_opt opt, core_solver;

    bool needslocal=( (nlopt_alg==NLOPT_AUGLAG) || (nlopt_alg==NLOPT_AUGLAG_EQ) || (nlopt_alg==NLOPT_G_MLSL) || (nlopt_alg==NLOPT_G_MLSL_LDS) );






    opt = nlopt_create(nlopt_alg, paramCount); /* algorithm and dimensionality */
    nlopt_set_lower_bounds(opt, paramsMin);
    nlopt_set_upper_bounds(opt, paramsMax);
    nlopt_set_min_objective(opt, QFFitAlgorithmNLOpt_fmin, &d);
    nlopt_set_xtol_rel(opt, getParameter("xtol").toDouble());
    nlopt_set_ftol_rel(opt, getParameter("ftol").toDouble());
    nlopt_set_maxeval(opt, getParameter("max_evals").toInt());
    nlopt_set_maxtime(opt, getParameter("max_time").toDouble());
    nlopt_set_initial_step1(opt, getParameter("stepsize").toDouble());

    if (needslocal) {
        core_solver = nlopt_create(NLOPT_LD_LBFGS, paramCount); /* algorithm and dimensionality */
        nlopt_set_lower_bounds(core_solver, paramsMin);
        nlopt_set_upper_bounds(core_solver, paramsMax);
        nlopt_set_min_objective(core_solver, QFFitAlgorithmNLOpt_fmin, &d);
        nlopt_set_xtol_rel(core_solver, getParameter("xtol").toDouble());
        nlopt_set_ftol_rel(core_solver, getParameter("ftol").toDouble());
        nlopt_set_maxeval(core_solver, getParameter("max_evals").toInt());
        nlopt_set_maxtime(core_solver, getParameter("max_time").toDouble());
        //nlopt_set_initial_step1(core_solver, getParameter("stepsize").toDouble());
        //nlopt_set_initial_step(core_solver, NULL);//getParameter("stepsize").toDouble());
        nlopt_set_local_optimizer(opt, core_solver);
    }


    double minf=0; /* the minimum objective value, upon return */

    for (int i=0; i<paramCount; i++) {
        paramsOut[i]=initialParams[i];
    }

    nlopt_result algres=nlopt_optimize(opt, paramsOut, &minf);
    bool ok=(algres >= 0);





    result.addNumber("final_value", minf);
    result.addString("nlopt_fit_algorihtm", nlopt_algorithm_name(nlopt_alg));

    result.fitOK=ok;
    result.message=QObject::tr("error %1 during optimization ").arg(int(algres));
    result.messageSimple=result.message;

    if (algres == NLOPT_SUCCESS) {
        result.fitOK=true;
        result.message=QObject::tr("success");
        result.messageSimple=result.message;
    } else if (algres == NLOPT_FAILURE) {
        result.fitOK=false;
        result.message=QObject::tr("generic failure");
        result.messageSimple=result.message;
    } else if (algres == NLOPT_INVALID_ARGS) {
        result.fitOK=false;
        result.message=QObject::tr("invalid arguments");
        result.messageSimple=result.message;
    } else if (algres == NLOPT_OUT_OF_MEMORY) {
        result.fitOK=false;
        result.message=QObject::tr("out of memory");
        result.messageSimple=result.message;
    } else if (algres == NLOPT_ROUNDOFF_LIMITED) {
        result.fitOK=false;
        result.message=QObject::tr("Halted because roundoff errors limited progress");
        result.messageSimple=result.message;
    } else if (algres == NLOPT_FORCED_STOP) {
        result.fitOK=false;
        result.message=QObject::tr("user canceled");
        result.messageSimple=result.message;
    } else if (algres == NLOPT_STOPVAL_REACHED) {
        result.fitOK=true;
        result.message=QObject::tr("NLOPT_STOPVAL_REACHED");
        result.messageSimple=result.message;
    } else if (algres == NLOPT_FTOL_REACHED) {
        result.fitOK=true;
        result.message=QObject::tr("ftol reached");
        result.messageSimple=result.message;
    } else if (algres == NLOPT_XTOL_REACHED) {
        result.fitOK=true;
        result.message=QObject::tr("xtol reached");
        result.messageSimple=result.message;
    } else if (algres == NLOPT_MAXEVAL_REACHED) {
        result.fitOK=true;
        result.message=QObject::tr("max_eval reached");
        result.messageSimple=result.message;
    } else if (algres == NLOPT_MAXTIME_REACHED) {
        result.fitOK=true;
        result.message=QObject::tr("max_time reached");
        result.messageSimple=result.message;
    }



    nlopt_destroy(opt);
    if (needslocal) nlopt_destroy(core_solver);


    return result;
}

bool QFFitAlgorithmNLOptBASE::displayConfig() {
    QFFitAlgorithmNLOptNonDerivConfigDialog* dlg=new QFFitAlgorithmNLOptNonDerivConfigDialog(0);

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

