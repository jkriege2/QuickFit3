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

#ifndef QFFITALGORITHMGSLTOOLS_H
#define QFFITALGORITHMGSLTOOLS_H

#include <cmath>
#include "qffitalgorithm.h"
#include "qffitfunction.h"
#include "gsl/gsl_multimin.h"
#include "gsl/gsl_vector.h"
#include "gsl/gsl_blas.h"
#include "gsl/gsl_deriv.h"

struct QFFItAlgorithmGSL_evalData {
    QFFitAlgorithm::Functor* model;
    const double* paramsMin;
    const double* paramsMax;
    int pcount;

    /** \brief pre-allocated memory of the size of the model-function output */
    gsl_vector * out;
    /** \brief pre-allocated memory of the size of the model-function output */
    gsl_vector * out_ast;
    /** \brief pre-allocated memory of the size of the parameter vector */
    gsl_vector * params;
    /** \brief pre-allocated memory of the size of the parameter vector */
    gsl_vector * params_ast;

};

struct QFFItAlgorithmGSLFMin_evalData {
    QFFitAlgorithm::FunctorForFMin* model;
    const double* paramsMin;
    const double* paramsMax;
    int pcount;

    /** \brief pre-allocated memory of the size of the parameter vector */
    gsl_vector * params;
    /** \brief pre-allocated memory of the size of the parameter vector */
    gsl_vector * params_ast;

};

double QFFitAlgorithmGSL_f (const gsl_vector *v, void *params);
struct QFFitAlgorithmGSL_fForDerivData {
    gsl_vector *v;
    int index;
    void* params;
    double (* f) (const gsl_vector  * x, void * params);
};
double QFFitAlgorithmGSL_fForDeriv (double x, void *params);
void QFFitAlgorithmGSL_df (const gsl_vector *v, void *params, gsl_vector * g);
void QFFitAlgorithmGSL_fdf (const gsl_vector *x, void *params, double *f, gsl_vector *df);


double QFFitAlgorithmGSLFmin_f (const gsl_vector *v, void *params);
double QFFitAlgorithmGSLFmin_fForDeriv (double x, void *params);
void QFFitAlgorithmGSLFmin_df (const gsl_vector *v, void *params, gsl_vector * g);
void QFFitAlgorithmGSLFmin_fdf (const gsl_vector *x, void *params, double *f, gsl_vector *df);


gsl_vector* QFFitAlgorithmGSL_transformParams(const double* inputPrams, int N, const double* pmin, const double* pmax);
void QFFitAlgorithmGSL_backTransformParams(double* outputPrams, int N, const gsl_vector* input, const double* pmin, const double* pmax);

#endif // QFFITALGORITHMGSLTOOLS_H
