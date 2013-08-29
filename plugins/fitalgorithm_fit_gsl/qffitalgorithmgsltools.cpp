#include "qffitalgorithmgsltools.h"

#define RANGE_MAXVAL (DBL_MAX/10.0)

double QFFitAlgorithmGSL_f (const gsl_vector *v, void *eparams)
{
  double result=0;
  QFFItAlgorithmGSL_evalData *p = (QFFItAlgorithmGSL_evalData *)eparams;

  gsl_vector * out=p->out;
  gsl_vector * params=p->params;
  if (p->paramsMin&& p->paramsMax) {
      gsl_vector * params=p->params;

      for (int i=0; i<p->pcount; i++) {
          const double mi=p->paramsMin[i];
          const double ma=p->paramsMax[i];
          double val=gsl_vector_get(v, i);
          if (fabs(mi)<RANGE_MAXVAL && fabs(ma)<RANGE_MAXVAL) {
              const double pv=tanh(val);
              val=mi+(pv+1.0)*(ma-mi)/2.0;
          }
          gsl_vector_set(params, i, val);
      }
  } else {
      for (int i=0; i<p->pcount; i++) {
          const double val=gsl_vector_get(v, i);
          gsl_vector_set(params, i, val);
      }
  }
  // evaluate fit function out = [ f0(p), f1(p), ... ]
  p->model->evaluate(gsl_vector_ptr(out, 0), gsl_vector_ptr(params, 0));
  // calculate sum of squares out^T*out = out[0]^2+out[1]^2+...
  gsl_blas_ddot(out, out, &result);


  return result;
}


void QFFitAlgorithmGSL_df (const gsl_vector *v, void *eparams, gsl_vector * g)
{
  double result=0, result_ast=0;
  QFFItAlgorithmGSL_evalData *p = (QFFItAlgorithmGSL_evalData *)eparams;

  double eps = GSL_SQRT_DBL_EPSILON*10.0;

  gsl_vector * out=p->out;
  gsl_vector * out_ast=p->out_ast;
  gsl_vector * params=p->params;
  gsl_vector * params_ast=p->params_ast;


  if (p->paramsMin&& p->paramsMax) {
      gsl_vector * params=p->params;

      for (int i=0; i<p->pcount; i++) {
          const double mi=p->paramsMin[i];
          const double ma=p->paramsMax[i];
          double val=gsl_vector_get(v, i);
          if (fabs(mi)<RANGE_MAXVAL && fabs(ma)<RANGE_MAXVAL) {
              const double pv=tanh(val);
              val=mi+(pv+1.0)*(ma-mi)/2.0;
          }
          gsl_vector_set(params, i, val);
          gsl_vector_set(params_ast, i, val);
      }
  } else {
      for (int i=0; i<p->pcount; i++) {
          const double val=gsl_vector_get(v, i);
          gsl_vector_set(params, i, val);
          gsl_vector_set(params_ast, i, val);
      }
  }


  // evaluate fit function out = [ f0(p), f1(p), ... ]
  p->model->evaluate(gsl_vector_ptr(out, 0), gsl_vector_ptr(params, 0));
  // calculate sum of squares out^T*out = out[0]^2+out[1]^2+...
  gsl_blas_ddot(out, out, &result);

  //qDebug()<<"x="<<arrayToString(gsl_vector_ptr(out, 0), p->pcount)<<"   x^2="<<result;


  /* calucate the gradient, i.e. evaluate fit function for a series of pertubations
   * of the single parameters:
   *
   * First iterations: x* = [ x[0]+h[0],  x[1],       x[2], ...]
   * Second iterations: x* = [ x[0],      x[1]+h[1],  x[2], ...]
   *  ....
   * where h[i]=max(eps, x[i]*eps) with eps=sqrt(machine precision)
   */
  for (int i=0; i<p->pcount; i++) {
      double val=gsl_vector_get(params, i);
      double h=eps*val;
      if (h==0.0) h=eps;
      if (h<eps) h=eps;
      double val_ast=val+h;
      //if (val_ast>p->paramsMax[i]) val_ast=p->paramsMax[i];
      //else if (val_ast<p->paramsMin[i]) val_ast=p->paramsMin[i];
      gsl_vector_set(params_ast, i, val_ast);

      // evaluate fit function out_ast = [ f0(p), f1(p+h), ... ]
      p->model->evaluate(gsl_vector_ptr(out_ast, 0), gsl_vector_ptr(params_ast, 0));
      // calculate sum of squares out_ast^T*out_ast = out_ast[0]^2+out_ast[1]^2+...
      result_ast=0;
      gsl_blas_ddot(out_ast, out_ast, &result_ast);
      //qDebug()<<"x*="<<arrayToString(gsl_vector_ptr(out_ast, 0), p->pcount)<<"   x*^2="<<result_ast;

      gsl_vector_set(g, i, (result_ast-result)/h);
      //qDebug()<<"dx("<<i<<") [v="<<val<<" v*="<<val_ast<<"]:    ( "<<result_ast<<" - "<<result<<" ) / "<<h<<"  =  "<<gsl_vector_get(g, i)<<"    (eps="<<eps<<")";
      gsl_vector_set(params_ast, i, val);
  }

}


/* Compute both f and df together. */
void QFFitAlgorithmGSL_fdf (const gsl_vector *x, void *params, double *f, gsl_vector *df)
{
  *f = QFFitAlgorithmGSL_f(x, params);
  QFFitAlgorithmGSL_df(x, params, df);
}

gsl_vector* QFFitAlgorithmGSL_transformParams(const double* paramsOut, int N, const double* paramsMin, const double* paramsMax) {
    gsl_vector* res=gsl_vector_alloc(N);
    if (paramsMin&&paramsMax) {
        for (int i=0; i<N; i++) {
            const double mi=paramsMin[i];
            const double ma=paramsMax[i];
            if (fabs(mi)<RANGE_MAXVAL && fabs(ma)<RANGE_MAXVAL) {
                const double pv=2.0*(paramsOut[i]-mi)/(ma-mi)-1.0;
                double v=atanh(pv);
                if (pv>=1.0) v=1e15;
                if (pv<=-1.0) v=-1e15;
                gsl_vector_set(res, i, v);
            } else {
                gsl_vector_set(res, i, paramsOut[i]);
            }
        }
    } else {
        for (int i=0; i<N; i++) {
            gsl_vector_set(res, i, paramsOut[i]);
        }
    }
    return res;
}


void QFFitAlgorithmGSL_backTransformParams(double* paramsOut, int N, const gsl_vector* input, const double* paramsMin, const double* paramsMax) {
    if (paramsMin&&paramsMax) {
        for (int i=0; i<N; i++) {
            const double mi=paramsMin[i];
            const double ma=paramsMax[i];
            if (fabs(mi)<RANGE_MAXVAL && fabs(ma)<RANGE_MAXVAL) {
                const double pv=tanh(gsl_vector_get(input, i));
                paramsOut[i]=mi+(pv+1.0)*(ma-mi)/2.0;
            } else {
                paramsOut[i]=gsl_vector_get(input, i);
            }
        }
    } else {
        for (int i=0; i<N; i++) {
            paramsOut[i]=gsl_vector_get(input, i);
        }
    }
}




