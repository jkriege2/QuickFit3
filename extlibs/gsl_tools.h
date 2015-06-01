/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2015-02-02 16:43:09 +0100 (Mo, 02 Feb 2015) $  (revision $Rev: 3775 $)

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

#ifndef GSL_TOOLS_H
#define GSL_TOOLS_H

#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_roots.h>
#include <QString>
#include "qftools.h"
#include "qfpluginservices.h"


/*!
    \defgroup qf3_gsl_tools Tools for GSL - the GNU Scientific Library
    This group assembles several tool routines for GSL - the GNU Scientific Library
*/

inline void gslQF3Errorhandler(const char * reason, const char * file, int line, int gsl_errno) {
    QFPluginServices::getInstance()->log_error(QObject::tr("\nGSL error %5 (%1): %2 in '%3', l.%4\n").arg(gsl_strerror(gsl_errno)).arg(reason).arg(file).arg(line).arg(gsl_errno));
}

/*! \brief tries to find the root of the given function \a F using a bracketing algorithm (no derivatives), starting from the interval \a x_lo ... \a x_hi
    \ingroup qf3_gsl_tools

    \return \c true on success, or an error message in \a errorMessage

  */
inline bool gslFindRootDerivFree(double& root, gsl_function* F, double x_lo, double x_hi, const gsl_root_fsolver_type* solver=gsl_root_fsolver_brent, int max_iter=100, QString* errorMessage=NULL) {
    int status;
      int iter = 0;
      gsl_root_fsolver *s=NULL;
      root = NAN;

      gsl_error_handler_t* olderrh=gsl_set_error_handler(gslQF3Errorhandler);

      double f_lo=F->function(x_lo, F->params);
      double f_hi=F->function(x_hi, F->params);
      if ((f_lo<0.0 && f_hi<0.0) || (f_lo>0.0 && f_hi<0.0)) {
          if (errorMessage) *errorMessage=QObject::tr("f(xlo) and f(xhi) are both below or above y=0 ... therefore not zero-crossing can be found with a bracketing algorithm!");
          gsl_set_error_handler(olderrh);
          return false;
      }


      s = gsl_root_fsolver_alloc (solver);
      if (!s) {
          if (errorMessage) *errorMessage=QObject::tr("could not allocate memory for solver");
          gsl_set_error_handler(olderrh);
          return false;
      }
      status=gsl_root_fsolver_set (s, F, x_lo, x_hi);
      if (status!=GSL_SUCCESS) {
          gsl_root_fsolver_free (s);
          if (errorMessage) *errorMessage=gsl_strerror(status);
          gsl_set_error_handler(olderrh);
          return false;
      }


      do
        {
          iter++;
          status = gsl_root_fsolver_iterate (s);
          root = gsl_root_fsolver_root (s);
          x_lo = gsl_root_fsolver_x_lower (s);
          x_hi = gsl_root_fsolver_x_upper (s);
          status = gsl_root_test_interval (x_lo, x_hi,
                                           0, 0.001);

//          if (status == GSL_SUCCESS)
//            printf ("Converged:\n");

//          printf ("%5d [%.7f, %.7f] root: %.7f [intwidth: %+.7f]\n",
//                  iter, x_lo, x_hi,
//                  root,
//                  x_hi - x_lo);

        }
      while (status == GSL_CONTINUE && iter < max_iter);

      gsl_root_fsolver_free (s);

      if (iter >= max_iter) {
          if (errorMessage) *errorMessage=QObject::tr("maximum number of iterations exceeded");
          gsl_set_error_handler(olderrh);
          return false;
      }

      if (status!=GSL_SUCCESS) {
          if (errorMessage) *errorMessage=QObject::tr("didn't find root, GSL error code: %1: %2").arg(status).arg(gsl_strerror(status));
      } else {
          if (errorMessage) *errorMessage=QString();
      }
      gsl_set_error_handler(olderrh);
      return (status!=GSL_SUCCESS);
}



/*! \brief tries to find the root of the given function \a F using a derivative-based algorithm, starting from the guess in \a root
    \ingroup qf3_gsl_tools

    \return \c true on success, or an error message in \a errorMessage

  */
inline bool gslFindRootWithDeriv(double& root, gsl_function_fdf* F, const gsl_root_fdfsolver_type* solver=gsl_root_fdfsolver_newton, int max_iter=100, QString* errorMessage=NULL) {
    int status;
      int iter = 0;
      gsl_root_fdfsolver *s=NULL;

      gsl_error_handler_t* olderrh=gsl_set_error_handler(gslQF3Errorhandler);


      s = gsl_root_fdfsolver_alloc (solver);
      if (!s) {
          if (errorMessage) *errorMessage=QObject::tr("could not allocate memory for solver");
          gsl_set_error_handler(olderrh);
          return false;
      }
      status=gsl_root_fdfsolver_set (s, F,root);
      if (status!=GSL_SUCCESS) {
          gsl_root_fdfsolver_free (s);
          if (errorMessage) *errorMessage=gsl_strerror(status);
          gsl_set_error_handler(olderrh);
          return false;
      }

      double r0=root;

      do {
            iter++;
            status = gsl_root_fdfsolver_iterate (s);
            r0 = root;
            root = gsl_root_fdfsolver_root (s);
            status = gsl_root_test_delta (root, r0, 0, 1e-6);

      } while (status == GSL_CONTINUE && iter < max_iter);

      gsl_root_fdfsolver_free (s);

      if (iter >= max_iter) {
          if (errorMessage) *errorMessage=QObject::tr("maximum number of iterations exceeded");
          gsl_set_error_handler(olderrh);
          return false;
      }

      if (status!=GSL_SUCCESS) {
          if (errorMessage) *errorMessage=QObject::tr("didn't find root, GSL error code: %1: %2").arg(status).arg(gsl_strerror(status));
      } else {
          if (errorMessage) *errorMessage=QString();
      }
      gsl_set_error_handler(olderrh);
      return (status!=GSL_SUCCESS);
}


#endif // GSL_TOOLS_H

