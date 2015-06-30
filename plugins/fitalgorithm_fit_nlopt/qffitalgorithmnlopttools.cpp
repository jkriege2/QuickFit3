#include "qffitalgorithmnlopttools.h"



double QFFitAlgorithmNLOpt_f(unsigned int n, const double* x, double* grad, void *params) {
    QFFItAlgorithmNLOpt_evalData *p = (QFFItAlgorithmNLOpt_evalData *)params;


    QVector<double> out(p->model->get_evalout(), 0.0);

    // evaluate fit function out = [ f0(p), f1(p), ... ]
    p->model->evaluate(out.data(), x);

    // calculate sum of squares out^T*out = out[0]^2+out[1]^2+...
    register double result=0;
    for (register int i=0; i<out.size(); i++) {
        result=result+qfSqr(out[i]);
    }

    if (grad) {
        QVector<double> derivout(p->model->get_evalout()*p->model->get_paramcount());
        p->model->evaluateJacobian(derivout.data(), x);
        for (unsigned int i=0; i<n; i++) {
            grad[i]=0;
            for (register int j=0; j<out.size();j++) {
                grad[i]=grad[i]+2.0*out[j]*derivout[i*out.size()+j];
            }
        }
    }

//    static int cnt=0;
//    qDebug()<<cnt<<" res="<<result <<"    n="<<n<<" evalout="<<p->model->get_evalout()<<x<<params;
//    cnt++;

    return result;
}


double QFFitAlgorithmNLOpt_fmin(unsigned int /*n*/, const double *x, double *grad, void *params)
{
    QFFItAlgorithmFMinNLOpt_evalData *p = (QFFItAlgorithmFMinNLOpt_evalData *)params;


    double result=0.0;

    // evaluate fit function out =  f(p)
    result=p->model->evaluate( x);



    if (grad) {
        QVector<double> derivout(p->model->get_paramcount());
        if (p->model->get_implementsJacobian()) p->model->evaluateJacobian(derivout.data(), x);
        else p->model->evaluateJacobianNum(derivout.data(), x);
    }
    return result;
}
