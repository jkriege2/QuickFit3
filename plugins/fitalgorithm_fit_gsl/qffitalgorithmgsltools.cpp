#include "qffitalgorithmgsltools.h"



double QFFitAlgorithmGSL_f (const gsl_vector *v, void *eparams)
{
  double result=0;
  QFFItAlgorithmGSL_evalData *p = (QFFItAlgorithmGSL_evalData *)eparams;

  gsl_vector * out=gsl_vector_alloc(p->model->get_evalout());
  gsl_vector * params=gsl_vector_alloc(p->pcount);

  for (int i=0; i<p->pcount; i++) {
      double val=gsl_vector_get(v, i);
      if (val>p->paramsMax[i]) val=p->paramsMax[i];
      else if (val<p->paramsMin[i]) val=p->paramsMin[i];
      gsl_vector_set(params, i, val);
  }

  // evaluate fit function out = [ f0(p), f1(p), ... ]
  p->model->evaluate(gsl_vector_ptr(out, 0), gsl_vector_ptr(params, 0));
  // calculate sum of squares out^T*out = out[0]^2+out[1]^2+...
  gsl_blas_ddot(out, out, &result);

  gsl_vector_free(params);
  gsl_vector_free(out);

  return result;
}


void QFFitAlgorithmGSL_df (const gsl_vector *v, void *eparams, gsl_vector * g)
{
  double result=0, result_ast=0;
  QFFItAlgorithmGSL_evalData *p = (QFFItAlgorithmGSL_evalData *)eparams;

  double eps = GSL_SQRT_DBL_EPSILON*10.0;

  gsl_vector * out=gsl_vector_alloc(p->model->get_evalout());
  gsl_vector * out_ast=gsl_vector_alloc(p->model->get_evalout());
  gsl_vector * params=gsl_vector_alloc(p->pcount);
  gsl_vector * params_ast=gsl_vector_alloc(p->pcount);

  for (int i=0; i<p->pcount; i++) {
      double val=gsl_vector_get(v, i);
      if (val>p->paramsMax[i]) val=p->paramsMax[i];
      else if (val<p->paramsMin[i]) val=p->paramsMin[i];
      gsl_vector_set(params, i, val);
      gsl_vector_set(params_ast, i, val);
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
      if (val_ast>p->paramsMax[i]) val_ast=p->paramsMax[i];
      else if (val_ast<p->paramsMin[i]) val_ast=p->paramsMin[i];
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

  gsl_vector_free(params);
  gsl_vector_free(params_ast);
  gsl_vector_free(out);
  gsl_vector_free(out_ast);

}


/* Compute both f and df together. */
void QFFitAlgorithmGSL_fdf (const gsl_vector *x, void *params, double *f, gsl_vector *df)
{
  *f = QFFitAlgorithmGSL_f(x, params);
  QFFitAlgorithmGSL_df(x, params, df);
}
