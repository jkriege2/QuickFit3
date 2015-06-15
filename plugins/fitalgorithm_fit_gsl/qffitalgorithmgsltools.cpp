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

#include "qffitalgorithmgsltools.h"

#define RANGE_MAXVAL (DBL_MAX/10.0)

double QFFitAlgorithmGSL_f (const gsl_vector *v, void *eparams)
{
  double result=0;
  QFFItAlgorithmGSL_evalData *p = (QFFItAlgorithmGSL_evalData *)eparams;

  gsl_vector * out=p->out;
  gsl_vector * params=p->params;
  if (p->paramsMin&& p->paramsMax) {


      for (int i=0; i<p->pcount; i++) {
           const double mi=p->paramsMin[i];
           const double ma=p->paramsMax[i];
           double val=gsl_vector_get(v, i);

           gsl_vector_set(params, i, qBound(mi, val, ma));
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

double QFFitAlgorithmGSL_fForDeriv (double x, void *params) {
    QFFitAlgorithmGSL_fForDerivData* data=(QFFitAlgorithmGSL_fForDerivData*)params;
    const double temp=gsl_vector_get(data->v, data->index);
    gsl_vector_set(data->v, data->index, x);
    const double res=data->f(data->v, data->params);
    gsl_vector_set(data->v, data->index, temp);
    return res;
}


void QFFitAlgorithmGSL_df (const gsl_vector *v, void *eparams, gsl_vector * g)
{

  QFFItAlgorithmGSL_evalData *p = (QFFItAlgorithmGSL_evalData *)eparams;

  double eps = GSL_SQRT_DBL_EPSILON*10.0;


  gsl_vector * params=p->params;



  // ensure parameters are stored in params and params_ast and that they are within the parameter ranges (if applicable)
  if (p->paramsMin&& p->paramsMax) {

      for (int i=0; i<p->pcount; i++) {
           const double mi=p->paramsMin[i];
           const double ma=p->paramsMax[i];
           double val=qBound(mi, gsl_vector_get(v, i), ma);

           gsl_vector_set(params, i, val);

       }
  } else {
      for (int i=0; i<p->pcount; i++) {
          const double val=gsl_vector_get(v, i);
          gsl_vector_set(params, i, val);

      }
  }



  /* calucate the gradient, i.e. evaluate fit function for a series of pertubations
   * of the single parameters:
   *
   * First iterations: x* = [ x[0]+h[0],  x[1],       x[2], ...]
   * Second iterations: x* = [ x[0],      x[1]+h[1],  x[2], ...]
   *  ....
   * where h[i]=max(eps, x[i]*eps) with eps=sqrt(machine precision)
   */

  QFFitAlgorithmGSL_fForDerivData dd;
  dd.f=QFFitAlgorithmGSL_f;
  dd.index=0;
  dd.params=eparams;
  dd.v=gsl_vector_alloc(params->size);
  gsl_vector_memcpy(dd.v, params);
  gsl_function F;
  F.function=QFFitAlgorithmGSL_fForDeriv;
  F.params=&dd;
  double resultd, abserr;
  for (int i=0; i<p->pcount; i++) {
      dd.index=i;

      gsl_deriv_central(&F,gsl_vector_get(params,i),eps,&resultd,&abserr);
      gsl_vector_set(g, i, resultd);


  }

}


/* Compute both f and df together. */
void QFFitAlgorithmGSL_fdf (const gsl_vector *x, void *params, double *f, gsl_vector *df)
{
  *f = QFFitAlgorithmGSL_f(x, params);
  QFFitAlgorithmGSL_df(x, params, df);
}

gsl_vector* QFFitAlgorithmGSL_transformParams(const double* paramsOut, int N, const double* /*paramsMin*/, const double* /*paramsMax*/) {
    gsl_vector* res=gsl_vector_alloc(N);
    /*if (paramsMin&&paramsMax) {
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
    } else {*/
        for (int i=0; i<N; i++) {
            gsl_vector_set(res, i, paramsOut[i]);
        }
    //}
    return res;
}


void QFFitAlgorithmGSL_backTransformParams(double* paramsOut, int N, const gsl_vector* input, const double* paramsMin, const double* paramsMax) {
    if (paramsMin&&paramsMax) {
        /*for (int i=0; i<N; i++) {
            const double mi=paramsMin[i];
            const double ma=paramsMax[i];
            if (fabs(mi)<RANGE_MAXVAL && fabs(ma)<RANGE_MAXVAL) {
                const double pv=tanh(gsl_vector_get(input, i));
                paramsOut[i]=mi+(pv+1.0)*(ma-mi)/2.0;
            } else {
                paramsOut[i]=gsl_vector_get(input, i);
            }
        }*/
        for (int i=0; i<N; i++) {
            const double mi=paramsMin[i];
            const double ma=paramsMax[i];
            /*if (fabs(mi)<RANGE_MAXVAL && fabs(ma)<RANGE_MAXVAL) {
                const double pv=tanh(gsl_vector_get(input, i));
                paramsOut[i]=mi+(pv+1.0)*(ma-mi)/2.0;
            } else {
                paramsOut[i]=gsl_vector_get(input, i);
            }*/
            paramsOut[i]=qBound(mi, gsl_vector_get(input, i), ma);
        }
    } else {
        for (int i=0; i<N; i++) {
            paramsOut[i]=gsl_vector_get(input, i);
        }
    }
}






double QFFitAlgorithmGSLFmin_f(const gsl_vector *v, void *eparams)
{
    double result=0;
    QFFItAlgorithmGSLFMin_evalData *p = (QFFItAlgorithmGSLFMin_evalData *)eparams;

    gsl_vector * params=p->params;
    if (p->paramsMin&& p->paramsMax) {

        for (int i=0; i<p->pcount; i++) {
             const double mi=p->paramsMin[i];
             const double ma=p->paramsMax[i];
             double val=gsl_vector_get(v, i);

             gsl_vector_set(params, i, qBound(mi, val, ma));
         }

    } else {
        for (int i=0; i<p->pcount; i++) {
            const double val=gsl_vector_get(v, i);
            gsl_vector_set(params, i, val);
        }
    }
    // evaluate fit function out = [ f0(p), f1(p), ... ]
    result=p->model->evaluate(gsl_vector_ptr(params, 0));

    return result;
}


double QFFitAlgorithmGSLFmin_fForDeriv(double x, void *params)
{
    QFFitAlgorithmGSL_fForDerivData* data=(QFFitAlgorithmGSL_fForDerivData*)params;
    const double temp=gsl_vector_get(data->v, data->index);
    gsl_vector_set(data->v, data->index, x);
    const double res=data->f(data->v, data->params);
    gsl_vector_set(data->v, data->index, temp);
    return res;
}


void QFFitAlgorithmGSLFmin_df(const gsl_vector *v, void *eparams, gsl_vector *g)
{
    //double result=0, result_ast=0;
    QFFItAlgorithmGSLFMin_evalData *p = (QFFItAlgorithmGSLFMin_evalData *)eparams;

    double eps = GSL_SQRT_DBL_EPSILON*10.0;

    gsl_vector * params=p->params;


    // ensure parameters are stored in params and params_ast and that they are within the parameter ranges (if applicable)
    if (p->paramsMin&& p->paramsMax) {

        for (int i=0; i<p->pcount; i++) {
             const double mi=p->paramsMin[i];
             const double ma=p->paramsMax[i];
             double val=qBound(mi, gsl_vector_get(v, i), ma);

             gsl_vector_set(params, i, val);
         }
    } else {
        for (int i=0; i<p->pcount; i++) {
            const double val=gsl_vector_get(v, i);
            gsl_vector_set(params, i, val);
        }
    }


    /* calucate the gradient, i.e. evaluate fit function for a series of pertubations
     * of the single parameters:
     *
     * First iterations: x* = [ x[0]+h[0],  x[1],       x[2], ...]
     * Second iterations: x* = [ x[0],      x[1]+h[1],  x[2], ...]
     *  ....
     * where h[i]=max(eps, x[i]*eps) with eps=sqrt(machine precision)
     */

    QFFitAlgorithmGSL_fForDerivData dd;
    dd.f=QFFitAlgorithmGSLFmin_f;
    dd.index=0;
    dd.params=eparams;
    dd.v=gsl_vector_alloc(params->size);
    gsl_vector_memcpy(dd.v, params);
    gsl_function F;
    F.function=QFFitAlgorithmGSLFmin_fForDeriv;
    F.params=&dd;
    double resultd, abserr;
    for (int i=0; i<p->pcount; i++) {
        dd.index=i;

        gsl_deriv_central(&F,gsl_vector_get(params,i),eps,&resultd,&abserr);
        gsl_vector_set(g, i, resultd);

    }
}


void QFFitAlgorithmGSLFmin_fdf(const gsl_vector *x, void *params, double *f, gsl_vector *df)
{
    *f = QFFitAlgorithmGSLFmin_f(x, params);
    QFFitAlgorithmGSLFmin_df(x, params, df);
}
